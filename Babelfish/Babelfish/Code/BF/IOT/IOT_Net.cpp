/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram.h"
#include "IOT_Net.h"
#include "IOT_Device_Command.h"
#include "BIN_ASCII_Conversion.h"
#include "netif.h"
#include "platform_ltk_azure.h"
#include "IOT_Debug.h"
#include <string.h>

#ifdef IOT_ALARM_SUPPORT
#include "IOT_Alarms.h"
#endif

#ifdef ESP32_SETUP
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_wifi_netif.h"
#endif

//Ahmed
#include "Breaker.h"
#include "DeviceGPIO.h"

extern DRAM_ATTR Device_Info_str DeviceInfo;

// For logging RAM usage. Use the one defined for SSL debug if available,
#ifdef LTK_TLS_DEBUG
extern struct debug_max_data max_data;
#elif defined IOT_ENABLE_INFO_LOGGING
// else instantiate our own local one
struct debug_iot_max_data
{
	// Leaving out a lot of debug_max_data fields here, keeping just our three
	uint8_t max_cstack_percent;
	uint8_t max_heap_percent;
	uint8_t max_task_stack_percent;
};

struct debug_iot_max_data max_data;
struct debug_iot_max_data previous_data = { 0, 0, 0 };
#endif

/*
 *****************************************************************************************
 *		Static Variables, accessible to our static callbacks
 *****************************************************************************************
 */
uint16_t IOT_Net::m_groups_ready = 0U;
Custom_IoT_Message_Callback IOT_Net::m_custom_message_callback = nullptr;


/* CTOR */
IOT_Net::IOT_Net( const iot_target_info_st_t* iot_database,
				  const IOT_DCI_Config::iot_config_param_t* iot_config_struct, VECTOR_HANDLE cadence_owners ) :
	m_iot_database( iot_database ),
	m_device_handle( nullptr ),
	m_pub_sub( nullptr ),
	m_iot_patron( nullptr ),
	m_iot_source_id( 0U ),
	m_cadence_owners( cadence_owners ),
	m_is_initialized_once( false ),
	m_bf_ssl_context( nullptr ),
	m_lastReasonCode( IOTHUB_CLIENT_CONNECTION_NO_NETWORK ),
	m_current_group_index( 0 ),
	m_timer_task_state( NOT_CONNECTED ),
	m_previous_task_state( NOT_CONNECTED ),
	m_scheduled_state( IOT_Device_Command::SCHEDULED_NONE ),
	m_connection_check_timer( nullptr ),
	m_iot_open_stall_count( 0 ),
	m_reset_device_command_time_ms( 0 ),
	m_state_machine_callback( nullptr )
{
	memset( &m_device, 0, sizeof( IOT_DATA_DEVICE_ITEM ) );

	// Create a new timer to watch the Connection status and possibly reconnect
	m_connection_check_timer = new BF_Lib::Timers( Connection_Check_Timer_Static,
												   reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
												   CR_TASKER_PRIORITY_0 );
	if ( m_connection_check_timer != nullptr )
	{
		m_connection_check_timer->Start( CONNECT_TIMER_MS, true );
	}

	//
	m_iot_dci_config = new IOT_DCI_Config( iot_config_struct );
	if ( m_iot_dci_config->Is_IOT_Enable() == true )
	{
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_INITIALIZING );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "IoT initializing" );

		Do_Onetime_Initialization();
	}
	else
	{
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_DISABLED );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "IOT is Disabled" );
	}
#ifdef IOT_ALARM_SUPPORT
	IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();
	BF_ASSERT( iot_alarms_instance );
	iot_alarms_instance->Init_Head_Tail_Indices();
	iot_alarms_instance->Init_Cadence();
#endif
}

// Simple XTOR. Releases config variables.
IOT_Net::~IOT_Net()
{
	delete m_pub_sub;
	delete m_iot_patron;
	delete m_iot_dci_config;
	if ( m_bf_ssl_context != nullptr )
	{
		delete m_bf_ssl_context;
	}
}

// Function that initializes the parts of the IoT code that are only initialized once,
void IOT_Net::Do_Onetime_Initialization( void )
{
	if ( !m_is_initialized_once )
	{
		m_iot_patron = new DCI_Patron( false );
		m_iot_source_id = DCI_SOURCE_IDS_Get();

		m_device.profile = m_iot_dci_config->Get_Device_Profile();
		m_device.deviceUUID = m_iot_dci_config->Get_Device_Uuid();

		m_bf_ssl_context = new Ssl_Context();
		if ( m_bf_ssl_context != nullptr )
		{
			// For the Azure connection, only the CA matters
			const char* azure_ca_cert = get_azure_trusted_certificate();
			if ( m_bf_ssl_context->Load_Keys( KEY_ECC_LOAD, nullptr, 0, nullptr, 0,
											  ( const unsigned char* ) azure_ca_cert, strlen( azure_ca_cert ) ) )
			{
				Attach_IOT_SSL_Context( m_bf_ssl_context );
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "SSL_Context Key Loading of CA Successful" );

				/* Signal any potential other threads that certificate generation is successful */
				m_bf_ssl_context->Signal();

				m_pub_sub = new IOT_Pub_Sub( m_iot_database, m_iot_patron, m_iot_source_id,
											 m_cadence_owners, m_device.deviceUUID );
				m_is_initialized_once = true;

			}
			else
			{
				// @ToDo: We need a status enum for TLS Initialization Failure
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_TLS_INIT_FAILED );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "SSL_Context Key Loading of CA Failed" );
			}
		}
	}
}

// Function which opens the IoT connection and registers the necessary callbacks.
bool IOT_Net::Open_Connection_And_Register( void )
{
	bool is_success = false;
	int status = 0;

	// Test the system clock first
	if ( Is_System_Clock_Set() )
	{
		IOT_CONNECTION_OPTIONS* connection_options = m_iot_dci_config->Get_IOT_Connection_Options();
		// Give a pointer to "this" as the owner_context for this connection; useful for the C function callbacks.
		m_device_handle = iot_open( connection_options, &status, reinterpret_cast<void*>( this ) );

		if ( m_device_handle != nullptr )
		{
			// Make sure we have our callback registered first!
			iot_registerConnectionStatusCallback( m_device_handle, &Connection_Status_Update_static );

			iot_registerC2DDeviceCommandCallback( m_device_handle, C2D_Device_Command_Callback );
			iot_registerTimerCallback( m_device_handle, 0, Timer_static );

			m_pub_sub->Set_Device_Handle( m_device_handle );
			m_pub_sub->Initialize();

			is_success = true;
            if (DeviceInfo.DeviceInNetworkStatus != DEVICE_IS_IN_A_NETWORK)
            {
                //This code will be running only when the device is done provising
                DeviceInfo.DeviceInNetworkStatus = DEVICE_IS_IN_A_NETWORK;
                DeviceInfo.ProvisionState = Provisioned;
                WriteByteToFlash(EEOFFSET_PROVISION_LABEL,DeviceInfo.DeviceInNetworkStatus );
                if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
                {
                    //if the device is in factory mode, that means we didn't use BLE, 
                    //so we don't set DeviceInfo.JustProvisioned flag true to avoid turning off BLE
                    // which create sometimes issues if the BLE was not able in the first place.
                    DeviceInfo.JustProvisioned = true;
                }
            }
            
            //Ahmed
            //If we are here that means either we just provisioned the device or we powered up a provisiond device
            DeviceInfo.IoTConnected = true;

		}
		else
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to open IoT connection" );
			m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_FAILED_TO_OPEN_CONN );
		}
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "System clock not set correctly; can't proceed with IoT" );
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_CLOCK_INCORRECT );
        //Ahmed
        BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
        SetRGB_Color(WIFI_IOT_CLOCK_NOT_SET_COLOR);
	}
	return ( is_success );
}

// Builds and sends a DeviceTree message.
bool IOT_Net::Send_Device_Tree()
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_iot_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	uint16_t prod_name_len = 0U;
	uint16_t serial_num_len = 0U;

#ifdef ESP32_SETUP
	char mac_str[19] = {0};
	uint8_t mac[6] = {0};
#else
	uint16_t mac_addr_len = 0U;
#endif

	m_iot_patron->Get_Length( DCI_PRODUCT_NAME_DCID, &prod_name_len );
	uint8_t* product_name = new uint8_t[prod_name_len];

	access_struct.data_id = DCI_PRODUCT_NAME_DCID;
	access_struct.data_access.data = product_name;
	access_struct.data_access.length = 0;
	access_struct.data_access.offset = 0;
	if ( m_iot_patron->Data_Access( &access_struct ) == DCI_ERR_NO_ERROR )
	{
        
		m_device.name = reinterpret_cast<char*>( product_name );
         //Ahmed
        //m_device.name = DeviceInfo.DeviceName;
	}

	m_iot_patron->Get_Length( DCI_PRODUCT_SERIAL_NUM_DCID, &serial_num_len );
	uint32_t* serial_num = new uint32_t[serial_num_len / sizeof( uint32_t )];
	uint8_t* serial_num_str =
		new uint8_t[( ( serial_num_len / sizeof( uint32_t ) ) * 10 ) + 1];

	access_struct.data_id = DCI_PRODUCT_SERIAL_NUM_DCID;
	access_struct.data_access.data = serial_num;
	access_struct.data_access.length = 0;
	if ( m_iot_patron->Data_Access( &access_struct ) == DCI_ERR_NO_ERROR )
	{
		INT_ASCII_Conversion::uint32_to_str( *serial_num, serial_num_str );
		m_device.serial = reinterpret_cast<char*>( serial_num_str );
	}

#ifndef ESP32_SETUP
	m_iot_patron->Get_Length( DCI_ETHERNET_MAC_ADDRESS_DCID, &mac_addr_len );
	uint8_t* mac_addr = new uint8_t[mac_addr_len];
	uint8_t* mac_addr_str = new uint8_t[( mac_addr_len * 2 ) + ( mac_addr_len - 1 ) + 1];

	access_struct.data_id = DCI_ETHERNET_MAC_ADDRESS_DCID;
	access_struct.data_access.data = mac_addr;
	access_struct.data_access.length = 0;
	if ( m_iot_patron->Data_Access( &access_struct ) == DCI_ERR_NO_ERROR )
	{
		uint8_t length = 0;
		uint8_t val;

		for ( uint_fast8_t i = 1; i <= mac_addr_len; i++ )
		{
			val = mac_addr[mac_addr_len - i];
			BIN_ASCII_Conversion::byte_to_str_no_prefix( val, mac_addr_str + length );
			length += 2;
			if ( i < mac_addr_len )
			{
				mac_addr_str[length++] = ':';
			}
		}
		m_device.mac = reinterpret_cast<char*>( mac_addr_str );
	}
#endif

#if ( defined( ESP32_SETUP ) && defined( WIFI_STATION_MODE ) )
	esp_wifi_get_mac( static_cast<wifi_interface_t>( ESP_IF_WIFI_STA ), mac );
	sprintf( mac_str, MACSTR, MAC2STR( mac ) );
	m_device.mac = mac_str;
#elif ( defined( ESP32_SETUP ) && defined( ETHERNET_SETUP ) )
	esp_read_mac( mac, ESP_MAC_ETH );
	sprintf( mac_str, MACSTR, MAC2STR( mac ) );
	m_device.mac = mac_str;
#endif

	m_device.assetTag = "Asset Name";
	m_device.subDevices = NULL;

	IOT_DATA data;

	memset( &data, 0, sizeof( IOT_DATA ) );
	data.dataType = DEVICE_TREE;
	data.deviceUUID = m_device.deviceUUID;
	data.devices = singlylinkedlist_create();
	singlylinkedlist_add( data.devices, &m_device );
	int message_size;

	bool result = iot_send( m_device_handle, &data, &message_size );

	if ( result )
	{
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Sent DeviceTree message, len = %d", message_size );
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to send DeviceTree message" );
	}


	// @ToDo: maybe this should wait until the XTOR to be done?
	delete[] product_name;
	delete[] serial_num;
	delete[] serial_num_str;
#ifndef ESP32_SETUP
	delete[] mac_addr;
	delete[] mac_addr_str;
#endif

	singlylinkedlist_destroy( data.devices );

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Net::Mark_Group_Ready( uint16_t group_marker )
{
	///< Function used by the Group Handler's Timer Task to mark its group as "ready".
	///< Just OR the bit in (in case it's already set)
	IOT_Net::m_groups_ready |= group_marker;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Net::Clear_Group_Ready( uint16_t group_marker )
{
	///< Clear the ready bit if it was marked ready
	if ( IOT_Net::m_groups_ready && group_marker )
	{
		IOT_Net::m_groups_ready &= ~group_marker;
	}
}

/*
 *****************************************************************************************
 * The Connection Check task is run every 10s
 *****************************************************************************************
 */
void IOT_Net::Connection_Check_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	( reinterpret_cast<IOT_Net*>( param ) )->Connection_Check_Timer_Task();
}

// Steady timer task that checks to see if we need to (re)connect
void IOT_Net::Connection_Check_Timer_Task( void )
{
//Ahmed
#ifdef _DEBUG_
        static int flag = 0;
        if (flag)
        {
        	//gpio_set_level(TEST_PIN_2, HI);
        }
        else
        {
        	//gpio_set_level(TEST_PIN_2, LOW);
        }
        flag^=1;
#endif        
	/* Check if Azure side retry is in progress */
	if ( get_tlsio_retry_status() == true )
	{
		m_timer_task_state = AZURE_TLSIO_RETRYING;
	}

	if ( m_timer_task_state == AZURE_TLSIO_RETRYING )
	{
		/** If the state was AZURE_TLSIO_RETRYING then check if retries are over. If yes the we can declare
		   FORCE_TO_CLOSE */
		if ( get_tlsio_retry_status() == false )
		{
			m_timer_task_state = FORCE_TO_CLOSE;
		}
	}
	if ( m_timer_task_state != AZURE_TLSIO_RETRYING )
	{
		// See if IOT just been changed to Enable
		if ( !m_is_initialized_once )
		{
			if ( m_iot_dci_config->Is_IOT_Enable() == true )
			{
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_INITIALIZING );
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "IoT initializing" );

				Do_Onetime_Initialization();
			}
			else
			{
				// Show the (optional) LWIP and heap stats:
				stats_display();
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Checking: 0 heap usage = %u%% and available = %u",
								 Ram::Used( 0 ), Ram::Available( 0 ) );
			}
		}
		// If just changed to disabled, force IOT to close
		if ( !m_iot_dci_config->Is_IOT_Enable() )
		{
			m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_DISABLED );
			if ( ( m_timer_task_state != NOT_CONNECTED ) && ( m_timer_task_state != FORCE_TO_CLOSE ) )
			{
				m_iot_dci_config->Set_Connection_Status( false );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "IOT has been Disabled" );
				m_timer_task_state = FORCE_TO_CLOSE;
			}
			else if ( m_timer_task_state == NOT_CONNECTED )
			{
				// If we know we are disabled, just exit; we don't want to do any of the other processing here
				return;
			}
		}

#ifdef WIFI_STATION_MODE
		tcpip_adapter_ip_info_t ip;
		memset( &ip, 0, sizeof( tcpip_adapter_ip_info_t ) );
		// Checking network link up status
		bool status = tcpip_adapter_is_netif_up( ( tcpip_adapter_if_t )ESP_IF_WIFI_STA );
		if ( status != true )
		{
			m_iot_dci_config->Set_Connection_Status( false );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Network down" );
			m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_NETWORK_LINK_DOWN );
			m_timer_task_state = FORCE_TO_CLOSE;
            //Ahmed
            BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
            if (DeviceInfo.ProvisionState != ProvisionStarted)
            {
            	SetRGB_Color(WIFI_IOT_DISCONNECT_COLOR);
            }
            DeviceInfo.WiFiDisconnected = true;
            if ( (DeviceInfo.DeviceInFactoryMode == DEVICE_IS_IN_FACTORY_MODE) && (DeviceInfo.TheDeviceIsLocked == true))
            {
                // if we are here, that means we were connected to Wifi
                // that means we are in factory mode and we reset the wifi
                // so will turn the color to non provising color
                if (DeviceInfo.ConnectionStringSaved)
                {
                    SetRGB_Color(UNPROVISIONED_COLOR);
                }
                else
                {
                    SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
                }
            }
		}
#else
		// First check that the network interface is Up
		if ( ( netif_list->flags & NETIF_FLAG_LINK_UP ) != NETIF_FLAG_LINK_UP )
		{
			m_iot_dci_config->Set_Connection_Status( false );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Network down" );
			m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_NETWORK_LINK_DOWN );
			m_timer_task_state = FORCE_TO_CLOSE;
		}
#endif

		// Then see if our configuration changed
		if ( m_iot_dci_config->Is_Config_Change_Flag_Set() )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "IoT configuration changed; do reconnect." );
			m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_RECONNECTING );
			m_timer_task_state = FORCE_TO_CLOSE;
			m_iot_dci_config->Reset_Config_Change_Flag();
		}
		else if ( m_timer_task_state == IOT_OPENED )
		{
			if ( ++m_iot_open_stall_count >= MAX_IOT_OPEN_STALLS )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Failed to complete the IoT Connection; do reconnect." );
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_FAILED_TO_OPEN_CONN );
				m_timer_task_state = FORCE_TO_CLOSE;
			}
		}

		if ( m_timer_task_state == FORCE_TO_CLOSE )
		{
			if ( m_device_handle != nullptr )
			{
                //Ahmed
                BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
                if (DeviceInfo.ProvisionState != ProvisionStarted)
				{
					SetRGB_Color(WIFI_IOT_DISCONNECT_COLOR);
				}
                DeviceInfo.WiFiDisconnected = true; 
                if ( (DeviceInfo.DeviceInFactoryMode == DEVICE_IS_IN_FACTORY_MODE) && (DeviceInfo.TheDeviceIsLocked == true))
                {
                    // if we are here, that means we were connected to Wifi
                    // that means we are in factory mode and we reset the wifi
                    // so will turn the color to non provising color
                    if (DeviceInfo.ConnectionStringSaved)
                    {
                        SetRGB_Color(UNPROVISIONED_COLOR);
                    }
                    else
                    {
                        SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
                    }
                }                
				// Close the SDK's connection, and null the device handle
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Closing IoT Connection." );
				iot_close( m_device_handle );
				m_device_handle = nullptr;
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "After Closing: 0 heap usage = %u%% and available = %u",
								 Ram::Used( 0 ), Ram::Available( 0 ) );
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "After Closing: 1 heap usage = %u%% and available = %u",
								 Ram::Used( 1 ), Ram::Available( 1 ) );
#ifndef ESP32_SETUP
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "After Closing: IoT Stack usage = %u%% and CStack = %u%%",
								 OS_TASKER_current_task->Stack_Usage_Percent(), uC_CSTACK_Used() );
#endif
				// Show the (optional) LWIP stats here, too:
				stats_display();
			}
			m_timer_task_state = NOT_CONNECTED;
			// Don't go right into the NOT_CONNECTED processing this time, to keep this timer task short
		}
		// Check the status; if not connected, then try
		else if ( m_timer_task_state == NOT_CONNECTED )
		{
			// IOT_DEBUG_PRINT( DBG_MSG_INFO, "Before Connection: 0 heap usage = %u%% and available = %zu",
			// Ram::Used(0),
			// Ram::Available(0) );
			if ( Open_Connection_And_Register() )
			{
				/* Don't set the state to Publish the device tree information here;
				 * that comes with change in status to connected. */
				// m_timer_task_state = SEND_DEVICE_TREE;
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Connection appears to be successfully opened." );
				m_current_group_index = 0;
				m_timer_task_state = IOT_OPENED;
				m_iot_open_stall_count = 0;
				// IOT_DEBUG_PRINT( DBG_MSG_INFO, "After Connection: 0 heap usage = %u%% and available = %zu",
				// Ram::Used(0),
				// Ram::Available(0) );
			}
		}
		else if ( ( m_timer_task_state == SEND_ALL_CHANNELS ) || ( m_timer_task_state == SEND_UPDATED_CHANNELS ) )
		{
			/* In these normal running states, check that we haven't gotten stuck unable to publish because none of our
			   message buffers are available. Re-purposing the stall counter here. Note that it can be cleared by a
			   single success in getting a message buffer in the Timer_Task(). */
			if ( getFreeMessageCount( m_device_handle ) == 0 )
			{
				if ( ++m_iot_open_stall_count >= MAX_NO_IOT_MSG_BUFFERS )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Too long without available message buffers; do reconnect." );
					m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_TOO_MANY_LOST_MESSAGES );
					m_timer_task_state = FORCE_TO_CLOSE;
				}
			}
			else
			{
				m_iot_open_stall_count = 0;
			}
			// Sometimes the Matrix TLS interface gets messed up; quick check for that:
			if ( !is_tlsio_open() )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "TLS IO layer not Open now; do reconnect." );
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_COMMUNICATION_ERROR );
				m_timer_task_state = FORCE_TO_CLOSE;
			}
		}

		// Else, wait until next time

	}
}

/*
 ***************************************************************************************************
 * The Timer Task is run every second (or sooner) by the messageThread, when connected or connecting
 ***************************************************************************************************
 */
void IOT_Net::Timer_static( IOT_DEVICE_HANDLE device_handle, void* owner_context )
{
	if ( owner_context != nullptr )
	{
		reinterpret_cast<IOT_Net*>( owner_context )->Timer_Task( device_handle );
	}
}

/* The Timer Task, as a class member. */
void IOT_Net::Timer_Task( IOT_DEVICE_HANDLE device_handle )
{

	if ( Is_Connected() == true )
	{
		/* Check if the callbacks for state machine are registered. This is currently used by FUS over IoT */
		if ( m_state_machine_callback != nullptr )
		{
			IOT_Device_Command* iotdc = new IOT_Device_Command( this );
			( iotdc->*m_state_machine_callback )();
			delete iotdc;
		}
		// See if the product has a custom message to send
		if ( m_custom_message_callback != nullptr )
		{
			unsigned long custom_message_id = ( *m_custom_message_callback )( m_timer_task_state, m_device_handle );
			if ( custom_message_id > 0 )
			{
				// Sent a message; do anything else here?
				IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Sent Custom Message, ID = %lu", custom_message_id );
			}
		}

		switch ( m_timer_task_state )
		{
			case SEND_DEVICE_TREE:
				Send_Device_Tree();
				m_timer_task_state = SEND_ALL_CHANNELS;
				m_groups_ready = 0;				// Clear any pending group requests
				m_iot_open_stall_count = 0;		// Clear this counter too
				break;

			case SEND_ALL_CHANNELS:
				while ( getFreeMessageCount( m_device_handle ) > 0 )
				{
					///< Publish all channels on establishing IOT connection if this group is configured to do so
					if ( m_pub_sub->Is_Publish_All_Channels_On_Connect( m_current_group_index ) )
					{
						if ( m_pub_sub->Publish_All_Channels( m_current_group_index ) )
						{
							// Clear this group's ready bit, in case it was set
							const iot_cadence_group_struct_t* the_group =
								&m_iot_database->group_list[m_current_group_index];
							m_groups_ready &= ~the_group->group_marker;
							m_current_group_index++;
						}
					}
					else
					{
						m_current_group_index++;
					}

					if ( m_current_group_index >= m_iot_database->total_groups )
					{
						m_current_group_index = 0;
						// If done sending all updates for all groups, advance to the next state.
						// (Else, loop or come back to here next time.)
#ifdef IOT_ALARM_SUPPORT
						m_timer_task_state = SEND_ALARMS;
#else
						m_timer_task_state = SEND_UPDATED_CHANNELS;
#endif  // IOT_ALARM_SUPPORT
						break;
					}
				}
				if ( getFreeMessageCount( m_device_handle ) == 0 )
				{
					IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "No more message buffers available for sending." );
				}
				break;

#ifdef IOT_ALARM_SUPPORT
			case SEND_ALARMS:
			{
				/** Check for alarm updates and publish them if ready We will advance to next state either when there
				   are no alarm updates available or when all alarm updates are published successfully
				 */
				if ( IOT_Alarms_Cadence::Alarms_Ready() )
				{
					while ( getFreeMessageCount( m_device_handle ) > 0 )
					{
						m_iot_open_stall_count = 0;			// Clear this, since there was an available message buffer
						bool return_status = false;
						return_status = m_pub_sub->Publish_Alarms_Message();
						if ( return_status )
						{
							IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "All Alarms published successfully" );
							IOT_Alarms_Cadence::Clear_Severity_Level_Marker();
							IOT_Alarms_Cadence::Clear_Alarms_Ready();
							m_timer_task_state = SEND_UPDATED_CHANNELS;
							break;
						}
					}
					if ( getFreeMessageCount( m_device_handle ) == 0 )
					{
						/** Even though message buffers are not available, stay in this state SEND_ALARMS. We will
						   advance to next state only when all alarms are published successfully
						 */
						IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "No more message buffers available for updating." );
					}
				}
				else
				{
					m_timer_task_state = SEND_UPDATED_CHANNELS;
				}
			}
			break;
#endif  // IOT_ALARM_SUPPORT

			case SEND_UPDATED_CHANNELS:
				if ( m_groups_ready > 0 )
				{
					bool send_all = false;		// Only sending SeeMe updates from here
					while ( getFreeMessageCount( m_device_handle ) > 0 )
					{
						m_iot_open_stall_count = 0;			// Clear this, since there was an available message buffer
						const iot_cadence_group_struct_t* the_group =
							&m_iot_database->group_list[m_current_group_index];
						if ( m_groups_ready & the_group->group_marker )
						{
							// This group is ready. Call Pub_Sub to do it.
							if ( m_pub_sub->Publish_Channel_Data( m_current_group_index, send_all ) )
							{
								// Clear this group's ready bit with XOR
								m_groups_ready ^= the_group->group_marker;
								m_current_group_index++;
								if ( m_current_group_index >= m_iot_database->total_groups )
								{
									m_current_group_index = 0;
									// If done sending all updates for all groups, leave this timer task
#ifdef IOT_ALARM_SUPPORT
									m_timer_task_state = SEND_ALARMS;
#endif  // IOT_ALARM_SUPPORT
									break;
								}
							}
						}
						else
						{
							m_current_group_index++;
							if ( m_current_group_index >= m_iot_database->total_groups )
							{
								m_current_group_index = 0;
								// If done sending all updates for all groups, leave this timer task
								// (Else, loop or come back to here next time.)
								break;
							}
						}
					}
					if ( getFreeMessageCount( m_device_handle ) == 0 )
					{
						IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "No more message buffers available for updating." );
					}
				}
				else
				{
#ifdef IOT_ALARM_SUPPORT
					m_timer_task_state = SEND_ALARMS;
#endif  // IOT_ALARM_SUPPORT
				}
				break;

			default:
				// Some other state, eg NOT_CONNECTED; do nothing here
				break;
		}
#ifndef ESP32_SETUP
		#ifdef IOT_ENABLE_INFO_LOGGING
		// Besides Print output, max_data is a good candidate for LiveWatch
		// Don't show this output every 5s unless it changes.
		max_data.max_cstack_percent = uC_CSTACK_Used();
		max_data.max_heap_percent = Ram::Used();
		// max_data.max_task_stack_percent = OS_Task_Mon::Fragile_Task_Stack_Usage();
		max_data.max_task_stack_percent = OS_TASKER_current_task->Stack_Usage_Percent();
		if ( ( previous_data.max_cstack_percent != max_data.max_cstack_percent ) ||
			 ( previous_data.max_heap_percent != max_data.max_heap_percent ) ||
			 ( previous_data.max_task_stack_percent != max_data.max_task_stack_percent ) )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Memory Usage percentages: Task stack = %d, CSTACK = %d, Heap = %d\n",
							 max_data.max_task_stack_percent,
							 max_data.max_cstack_percent, max_data.max_heap_percent );
			previous_data.max_cstack_percent = max_data.max_cstack_percent;
			previous_data.max_heap_percent = max_data.max_heap_percent;
			previous_data.max_task_stack_percent = max_data.max_task_stack_percent;
		}
		#endif
#endif
	}
}

// Function which checks to see if we are connected to Azure.
bool IOT_Net::Is_Connected( void )
{
	bool is_connected = m_iot_dci_config->Get_Connection_Status();

#ifdef WIFI_STATION_MODE
	tcpip_adapter_ip_info_t ip;
	memset( &ip, 0, sizeof( tcpip_adapter_ip_info_t ) );
	// Checking network link up status
	bool status = tcpip_adapter_is_netif_up( ( tcpip_adapter_if_t )ESP_IF_WIFI_STA );
	if ( is_connected && ( status != true ) )
	{
		m_iot_dci_config->Set_Connection_Status( false );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Network down now" );
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_NETWORK_LINK_DOWN );
		m_timer_task_state = FORCE_TO_CLOSE;
		is_connected = false;
	}
#else
	// Check if ethernet link is not up now, then set IoT_Connection_Status to false.
	if ( is_connected && ( ( netif_list->flags & NETIF_FLAG_LINK_UP ) != NETIF_FLAG_LINK_UP ) )
	{
		m_iot_dci_config->Set_Connection_Status( false );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Network down now" );
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_NETWORK_LINK_DOWN );
		m_timer_task_state = FORCE_TO_CLOSE;
		is_connected = false;
        //Ahmed
        BlinkLEDs(BLINK_250_mS,BLINK_FOREVER,LED_BLINK);
        if (DeviceInfo.ProvisionState != ProvisionStarted)
		{
			SetRGB_Color(WIFI_IOT_DISCONNECT_COLOR);
		}
        DeviceInfo.WiFiDisconnected = true;
        if ( (DeviceInfo.DeviceInFactoryMode == DEVICE_IS_IN_FACTORY_MODE) && (DeviceInfo.TheDeviceIsLocked == true))
        {
            // if we are here, that means we were connected to Wifi
            // that means we are in factory mode and we reset the wifi
            // so will turn the color to non provising color
            if (DeviceInfo.ConnectionStringSaved)
            {
                SetRGB_Color(UNPROVISIONED_COLOR);
            }
            else
            {
                SetRGB_Color(NO_CONNECTION_STRING_PROGRAMMED_COLOR);
            }
        }   
	}
#endif

	return ( is_connected );
}

// Connection status callback.
void IOT_Net::Connection_Status_Update_static( IOT_DEVICE_HANDLE device_handle, void* owner_context,
											   IOTHUB_CLIENT_CONNECTION_STATUS statusCode,
											   IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode )
{
	if ( owner_context != nullptr )
	{
		reinterpret_cast<IOT_Net*>( owner_context )->Connection_Status_Update( device_handle, statusCode, reasonCode );
	}
}

// Connection status callback.
void IOT_Net::Connection_Status_Update( IOT_DEVICE_HANDLE device_handle,
										IOTHUB_CLIENT_CONNECTION_STATUS statusCode,
										IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode )
{
	m_lastReasonCode = reasonCode;
	if ( statusCode == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED )
	{
		m_iot_dci_config->Set_Connection_Status( true );
		m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_CLOUD_CONNECTED );
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "IoT connected" );
        StopBlinkingLEDs();
		DeviceInfo.WiFiDisconnected = false;
		m_timer_task_state = SEND_DEVICE_TREE;
	}
	else
	{
		/* Call for the connection to be closed. However, we can't call iot_close() here, because it will kill the
		   messageThread, and this happens to be running *in* the messageThread, by way of Azure. But we can set the
		   flag so the messageThread knows. */
		closeMessageThread( device_handle );
		m_timer_task_state = FORCE_TO_CLOSE;
		m_iot_dci_config->Set_Connection_Status( false );
		const char_t* err_ptr = NULL;

		switch ( reasonCode )
		{
			case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
				err_ptr = "EXPIRED_SAS_TOKEN";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_EXPIRED_SAS_TOKEN );
				break;

			case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
				// Adding some additional text here to clarify a common problem:
				err_ptr = "DEVICE_DISABLED or system time incorrect";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_DEVICE_DISABLED );
				break;

			case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
				err_ptr = "BAD_CREDENTIAL";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_INVALID_CONN_STRING );
				break;

			case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
				err_ptr = "RETRY_EXPIRED";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_RETRY_EXPIRED );
				break;

			case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
				err_ptr = "NO_NETWORK";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_NO_NETWORK );
				break;

			case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
				err_ptr = "COMMUNICATION_ERROR";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_COMMUNICATION_ERROR );
				break;

			default:
				err_ptr = "ERROR UNKNOWN";
				m_iot_dci_config->Set_Conn_Status_Reason( DCI_IOT_CONN_STAT_REASON_ENUM::IOT_UNKNOWN_ERROR );
				break;
		}
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "IoT Connection Failure: %s", err_ptr );
		BF_Lib::Unused<const char_t*>::Okay( err_ptr );		// If debugging isn't enabled
	}
}

// Utility that checks a request to change state, and either permits or rejects the request.
bool IOT_Net::Request_Timer_Task_State( iot_net_states_t new_timer_task_state )
{
	// At this time, don't have any reason to refuse a state change request
	// We don't want to be storing SEND_TRANSFER_IMAGE or SEND_DEVICE_COMMAND_STATUS as the "previous" state,
	// or we might end up in a loop that we never get out of.
	if ( m_timer_task_state < SEND_TRANSFER_IMAGE )
	{
		m_previous_task_state = m_timer_task_state;
	}
	m_timer_task_state = new_timer_task_state;
	return ( true );
}

// Schedule new device command FUS state after delay_millisecs
void IOT_Net::Schedule_Device_Command( time_t delay_millisecs,
									   IOT_Device_Command::iot_device_command_scheduled_states_t new_scheduled_state )
{
	m_scheduled_state = new_scheduled_state;
	if ( delay_millisecs > 0 )
	{
		time_t tnow = get_time( nullptr );
#ifdef ESP32_SETUP
		m_reset_device_command_time_ms =
			static_cast<uint64_t>( static_cast<uint64_t>( tnow ) * MULTIPLIER_1000MS + delay_millisecs );
#else
		m_reset_device_command_time_ms = tnow * MULTIPLIER_1000MS + delay_millisecs;
#endif
	}
	else
	{
		m_reset_device_command_time_ms = 0;
	}
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
void IOT_Net::Register_State_Machine_Callback( State_Machine_Callback state_machine_callback )
{
	m_state_machine_callback = state_machine_callback;
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
void IOT_Net::Unregister_State_Machine_Callback( State_Machine_Callback state_machine_callback )
{
	m_state_machine_callback = nullptr;
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
void IOT_Net::Register_Custom_IoT_Message_Callback( Custom_IoT_Message_Callback custom_message_callback )
{
	m_custom_message_callback = custom_message_callback;
}

/******************************************************************************************
* See header file for function definition.
******************************************************************************************/
void IOT_Net::Unregister_Custom_IoT_Message_Callback()
{
	m_custom_message_callback = nullptr;
}
