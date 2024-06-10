/**
 ******************************************************************************
 * @file    app_ble.c
 * @author  MCD Application Team
 * @brief   BLE Application
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; You may not use this
 * file except in compliance with the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/

#include "CR_Tasker.h"
#include "CR_Queue.h"
#include "Timers_Lib.h"
#include "hci_tl.h"
#include "ble_gap_aci.h"
#include "ble_gatt_aci.h"
#include "ble_l2cap_aci.h"
#include "ble_hci_le.h"
#include "ble_hal_aci.h"
#include "Prod_Spec_APP.h"
#include "BLE_DCI_Interface.h"
#include "BLE_Stack_Interface.h"
#include "stm32_wpan_entry.h"
#include "stm32wbxx_hal.h"
#include "app_conf.h"
#include "ble.h"
#include "tl.h"
#include "shci.h"
#include "otp.h"
#include "stm32_wpan_app_Ble.h"

/* Private typedef -----------------------------------------------------------*/


static uint8_t const* m_ad_data = nullptr;
static uint8_t m_ad_data_size = 0U;
static uint8_t m_device_name_length = 0U;
static const char* m_device_name = nullptr;
static SHCI_C2_Ble_Init_Cmd_Packet_t* m_ble_init_cmd_packet = nullptr;


/**
 * security parameters structure
 */
typedef struct _tSecurityParams
{
	/**
	 * IO capability of the device
	 */
	uint8_t ioCapability;

	/**
	 * Authentication requirement of the device Man In the Middle protection required?
	 */
	uint8_t mitm_mode;

	/**
	 * bonding mode of the device
	 */
	uint8_t bonding_mode;

	/**
	 * Flag to tell whether OOB data has to be used during the pairing process
	 */
	uint8_t OOB_Data_Present;

	/**
	 * OOB data to be used in the pairing process if OOB_Data_Present is set to TRUE
	 */
	uint8_t OOB_Data[16];

	/**
	 * this variable indicates whether to use a fixed pin during the pairing process or a passkey has to be requested to
	 * the application during the pairing process 0 implies use fixed pin and 1 implies request for passkey
	 */
	uint8_t Use_Fixed_Pin;

	/**
	 * minimum encryption key size requirement
	 */
	uint8_t encryptionKeySizeMin;

	/**
	 * maximum encryption key size requirement
	 */
	uint8_t encryptionKeySizeMax;

	/**
	 * fixed pin to be used in the pairing process if Use_Fixed_Pin is set to 1
	 */
	uint32_t Fixed_Pin;

	/**
	 * this flag indicates whether the host has to initiate the security, wait for pairing or does not have any security
	 * requirements.\n 0x00 : no security required 0x01 : host should initiate security by sending the slave security
	 * request command 0x02 : host need not send the clave security request but it has to wait for paiirng to complete
	 * before doing any other processing
	 */
	uint8_t initiateSecurity;
} tSecurityParams;

/**
 * global context contains the variables common to all services
 */
typedef struct _tBLEProfileGlobalContext
{

	/**
	 * security requirements of the host
	 */
	tSecurityParams bleSecurityParam;

	/**
	 * gap service handle
	 */
	uint16_t gapServiceHandle;

	/**
	 * device name characteristic handle
	 */
	uint16_t devNameCharHandle;

	/**
	 * appearance characteristic handle
	 */
	uint16_t appearanceCharHandle;

	/**
	 * connection handle of the current active connection When not in connection, the handle is set to 0xFFFF
	 */
	uint16_t connectionHandle;

	/**
	 * length of the UUID list to be used while advertising
	 */
	uint8_t advtServUUIDlen;

	/**
	 * the UUID list to be used while advertising
	 */
	uint8_t advtServUUID[100];

} BleGlobalContext_t;

typedef struct
{
	BleGlobalContext_t BleApplicationContext_legacy;
	APP_BLE_ConnStatus_t Device_Connection_Status;
	/**
	 * ID of the Advertising Timeout
	 */
	uint8_t Advertising_mgr_timer_Id;
} BleApplicationContext_t;

#define BD_ADDR_SIZE_LOCAL    6


typedef enum
{
	HCI_CMD_BUSY,
	HCI_CMD_AVAILABLE,
} HCI_CmdBusyStatus_t;

/* Private variables ---------------------------------------------------------*/
PLACE_IN_SECTION( "MB_MEM1" ) ALIGN( 4 ) static TL_CmdPacket_t BleCmdBuffer;

static const uint8_t M_bd_addr[BD_ADDR_SIZE_LOCAL] = {
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0x0000000000FF ) ),
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0x00000000FF00 ) >> 8 ),
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0x000000FF0000 ) >> 16 ),
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0x0000FF000000 ) >> 24 ),
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0x00FF00000000 ) >> 32 ),
	( uint8_t )( ( CFG_ADV_BD_ADDRESS & 0xFF0000000000 ) >> 40 )
};

static uint8_t bd_addr_udn[BD_ADDR_SIZE_LOCAL];

// Identity root key used to derive LTK and CSRK
static const uint8_t BLE_CFG_IR_VALUE[16] = CFG_BLE_IRK;

// Encryption root key used to derive LTK and CSRK
static const uint8_t BLE_CFG_ER_VALUE[16] = CFG_BLE_ERK;

PLACE_IN_SECTION( "BLE_APP_CONTEXT" ) static BleApplicationContext_t BleApplicationContext;


/* Global variables ----------------------------------------------------------*/
static CR_Queue* hci_user_evt_proc_flag;
static HCI_CmdBusyStatus_t hci_cmd_busy_status = HCI_CMD_AVAILABLE;

static BLE_Stack_Interface* m_ble_stack_interface_handler = nullptr;
static BLE_DCI_Interface* m_ble_dci_interface_handle = nullptr;

/* Private function prototypes -----------------------------------------------*/
static void BLE_UserEvtRx( void* pPayload );

static void BLE_StatusNot( HCI_TL_CmdStatus_t status );

static void Ble_Tl_Init( void );

static void Ble_Hci_Gap_Gatt_Init( void );

static const uint8_t* BleGetBdAddress( void );

static void BleErrorHandler( const char* msg );

static SVCCTL_EvtAckStatus_t Svc_Event_Handler( void* Event );

static void hci_user_evt_proc_task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

/*************************************************************
*   Initialization functions
*************************************************************/


void APP_BLE_Init( void )
{
	tBleStatus sensapp_status;

	/**
	 * Initialize Ble Transport Layer
	 */
	Ble_Tl_Init();

	/**
	 * Register the hci transport layer to handle BLE User Asynchronous Events
	 */
	hci_user_evt_proc_flag = new CR_Queue( 10 );
	new CR_Tasker( hci_user_evt_proc_task, NULL, CR_TASKER_PRIORITY_4, "hci_user_evt_proc_task" );
	/**
	 * Starts the BLE Stack on CPU2
	 */
	SHCI_C2_BLE_Init( m_ble_init_cmd_packet );

	/**
	 * Initialization of HCI & GATT & GAP layer
	 */
	Ble_Hci_Gap_Gatt_Init();

	/**
	 * Initialization of the BLE Services
	 */
	SVCCTL_Init();

	/**
	 * Initialization of the BLE App Context
	 */

	BleApplicationContext.Device_Connection_Status = APP_BLE_IDLE;
	BleApplicationContext.BleApplicationContext_legacy.connectionHandle = 0xFFFF;

	/**
	 * From here, all initialization are BLE application specific
	 */
	/**
	 * Register the service event handler to the BLE controller
	 */
	SVCCTL_RegisterSvcHandler( Svc_Event_Handler );

	/**
	 * Add GATT services
	 */

	sensapp_status = m_ble_dci_interface_handle->Init();

	if ( sensapp_status != BLE_STATUS_SUCCESS )
	{
		BleErrorHandler( "Error while adding services" );
	}

	// Start to advertise
	Adv_Request();

}

/*************************************************************
*   BLE status and event notifications
*************************************************************/

/**
 * @brief            Main BLE event handler
 * @param[in] pckt   Buffer holding the Event
 */
SVCCTL_UserEvtFlowStatus_t SVCCTL_App_Notification( void* pckt )
{
	hci_event_pckt* event_pckt;
	evt_le_meta_event* meta_evt;
	evt_blue_aci* blue_evt;
	static uint16_t connection_handle;
	static uint8_t conn_update_status;

	event_pckt = ( hci_event_pckt* ) ( ( hci_uart_pckt* ) pckt )->data;

	switch ( event_pckt->evt )
	{

		case EVT_DISCONN_COMPLETE:
		{
			hci_disconnection_complete_event_rp0* disconnection_complete_event;
			disconnection_complete_event = ( hci_disconnection_complete_event_rp0* ) event_pckt->data;

			if ( disconnection_complete_event->Connection_Handle ==
				 BleApplicationContext.BleApplicationContext_legacy.connectionHandle )
			{
				BleApplicationContext.BleApplicationContext_legacy.connectionHandle = 0;
				BleApplicationContext.Device_Connection_Status = APP_BLE_IDLE;
			}

			// Restart advertising
			Adv_Request();
		}
		break;

		case EVT_LE_META_EVENT:
		{
			meta_evt = ( evt_le_meta_event* ) event_pckt->data;
			switch ( meta_evt->subevent )
			{

				case EVT_LE_CONN_UPDATE_COMPLETE:
				{
					if ( conn_update_status == 1 )
					{
						tBleStatus res = aci_l2cap_connection_parameter_update_req( connection_handle, 0x06, 0x0A, 0x00,
																					200 );
						if ( res != BLE_STATUS_SUCCESS )
						{
							BleErrorHandler( "Parameters update request failed" );
						}
						else
						{
							conn_update_status++;
						}
					}
				}
				break;

				case EVT_LE_CONN_COMPLETE:
				{
					hci_le_connection_complete_event_rp0* connection_complete_event;
					connection_complete_event = ( hci_le_connection_complete_event_rp0* ) meta_evt->data;

					if ( BleApplicationContext.Device_Connection_Status == APP_BLE_LP_CONNECTING )
					{
						// Connection as client
						BleApplicationContext.Device_Connection_Status = APP_BLE_CONNECTED_CLIENT;
					}
					else
					{
						// Connection as server
						BleApplicationContext.Device_Connection_Status = APP_BLE_CONNECTED_SERVER;
					}
					BleApplicationContext.BleApplicationContext_legacy.connectionHandle =
						connection_complete_event->Connection_Handle;

					conn_update_status = 0;
					connection_handle = connection_complete_event->Connection_Handle;
					tBleStatus res = aci_gatt_exchange_config( connection_complete_event->Connection_Handle );
					if ( res != BLE_STATUS_SUCCESS )
					{
						BleErrorHandler( "MTU update request failed" );
					}
					else
					{
						conn_update_status++;
					}
				}
				break;

				case HCI_LE_ADVERTISING_REPORT_SUBEVT_CODE:
					break;

				default:
					break;
			}
		}
		break;

		case EVT_VENDOR:
			blue_evt = ( evt_blue_aci* ) event_pckt->data;
			switch ( blue_evt->ecode )
			{
				case EVT_BLUE_GAP_PROCEDURE_COMPLETE:
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return ( SVCCTL_UserEvtFlowEnable );
}

/**
 * @brief   Obtain the connection status
 * @return  Connection status
 */
APP_BLE_ConnStatus_t APP_BLE_Get_Server_Connection_Status( void )
{
	return ( BleApplicationContext.Device_Connection_Status );
}

/**
 * @brief            GATT service event handler
 * @param[in] Event  Buffer holding the Event
 * @return           Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Svc_Event_Handler( void* Event )
{
	SVCCTL_EvtAckStatus_t return_value;
	hci_event_pckt* event_pckt;
	evt_blue_aci* blue_evt;
	aci_gatt_attribute_modified_event_rp0* attribute_modified;
	aci_gatt_read_permit_req_event_rp0* read_permit_req = nullptr;
	aci_gatt_write_permit_req_event_rp0* write_permit_req = nullptr;

	return_value = SVCCTL_EvtNotAck;
	event_pckt = ( hci_event_pckt* )( ( ( hci_uart_pckt* )Event )->data );

	switch ( event_pckt->evt )
	{
		case EVT_VENDOR:
		{
			blue_evt = ( evt_blue_aci* )event_pckt->data;
			switch ( blue_evt->ecode )
			{
				case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
				{
					uint8_t write_err_code = 0U;

					attribute_modified = ( aci_gatt_attribute_modified_event_rp0* )blue_evt->data;
					m_ble_stack_interface_handler->Write_Event( ( attribute_modified->Attr_Handle - 1U ),
																&( attribute_modified->Attr_Data[0] ),
																&write_err_code );
					break;
				}

				case EVT_BLUE_GATT_WRITE_PERMIT_REQ:
				{
					uint8_t write_err_code = 0U;
					bool process_ret = false;

					ErrorStatus write_status = SUCCESS;
					return_value = SVCCTL_EvtAckFlowEnable;
					write_permit_req = ( aci_gatt_write_permit_req_event_rp0* )blue_evt->data;
					process_ret =
						m_ble_stack_interface_handler->Write_Event( ( write_permit_req->Attribute_Handle - 1U ),
																	&( write_permit_req->Data[0] ),
																	&write_err_code );
					if ( process_ret == BLE_EVENT_ERROR )
					{
						write_status = ERROR;
						BleErrorHandler( "Error in Write_Event\n\r" );
					}
					aci_gatt_write_resp( write_permit_req->Connection_Handle,
										 write_permit_req->Attribute_Handle,
										 write_status,
										 write_err_code,
										 write_permit_req->Data_Length,
										 ( uint8_t* )&( write_permit_req->Data[0] ) );

					break;
				}

				case EVT_BLUE_GATT_READ_PERMIT_REQ:
				{
					uint8_t read_err_code = 0U;
					bool process_ret = false;

					read_permit_req = ( aci_gatt_read_permit_req_event_rp0* )blue_evt->data;
					process_ret = m_ble_stack_interface_handler->Read_Event( read_permit_req->Attribute_Handle - 1U,
																			 read_permit_req->Offset,
																			 &read_err_code );
					if ( process_ret == BLE_EVENT_SUCCESS )
					{
						( void )aci_gatt_allow_read( read_permit_req->Connection_Handle );
					}
					else
					{
						// error detected in offset calculation or read.
						BleErrorHandler( "Error in Read_Event\n\r" );
						read_permit_req->Offset = 0;
						( void )aci_gatt_deny_read( read_permit_req->Connection_Handle, read_err_code );
					}
					break;
				}

				case EVT_BLUE_GATT_NOTIFICATION:
				{
					break;
				}

				default:
					break;
			}
		}
		break;	/* HCI_EVT_VENDOR_SPECIFIC */

		default:
			break;
	}
	return ( return_value );
}

/**
 * @brief    Initialize the low-level transport layer for BLE
 */
static void Ble_Tl_Init( void )
{
	HCI_TL_HciInitConf_t Hci_Tl_Init_Conf;

	Hci_Tl_Init_Conf.p_cmdbuffer = ( uint8_t* )&BleCmdBuffer;
	Hci_Tl_Init_Conf.StatusNotCallBack = BLE_StatusNot;
	hci_init( BLE_UserEvtRx, ( void* ) &Hci_Tl_Init_Conf );
	return;
}

/**
 * @brief    Initialize the HCI, GAP, and GATT layers.
 */
static void Ble_Hci_Gap_Gatt_Init( void )
{

	uint8_t role;
	uint8_t index;
	uint16_t gap_service_handle, gap_dev_name_char_handle, gap_appearance_char_handle;
	const uint8_t* bd_addr;
	uint32_t srd_bd_addr[2];
	uint16_t appearance[1] = { BLE_CFG_GAP_APPEARANCE };

	/**
	 * Initialize HCI layer
	 */
	/*HCI Reset to synchronise BLE Stack*/
	hci_reset();

	/**
	 * Write the BD Address
	 */
	bd_addr = BleGetBdAddress();
	aci_hal_write_config_data( CONFIG_DATA_PUBADDR_OFFSET,
							   CONFIG_DATA_PUBADDR_LEN,
							   ( uint8_t* ) bd_addr );

	// Write Identity root key used to derive LTK and CSRK
	aci_hal_write_config_data( CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN,
							   ( uint8_t* ) BLE_CFG_IR_VALUE );

	// Write Encryption root key used to derive LTK and CSRK
	aci_hal_write_config_data( CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN,
							   ( uint8_t* ) BLE_CFG_ER_VALUE );

	// Write random bd_address
	/*
	   random_bd_address = R_bd_address;
	   aci_hal_write_config_data(CONFIG_DATA_RANDOM_ADDRESS_WR, CONFIG_DATA_RANDOM_ADDRESS_LEN, (uint8_t*)
	      random_bd_address);
	 */

	/**
	 * Static random Address The two upper bits shall be set to 1 The lowest 32bits is read from the UDN to
	 * differentiate between devices The RNG may be used to provide a random number on each power on
	 */
	srd_bd_addr[1] = 0x0000ED6E;
	srd_bd_addr[0] = LL_FLASH_GetUDN();
	aci_hal_write_config_data( CONFIG_DATA_RANDOM_ADDRESS_OFFSET, CONFIG_DATA_RANDOM_ADDRESS_LEN,
							   ( uint8_t* )srd_bd_addr );
	/**
	 * Write Identity root key used to derive LTK and CSRK
	 */
	aci_hal_write_config_data( CONFIG_DATA_IR_OFFSET, CONFIG_DATA_IR_LEN, ( uint8_t* )BLE_CFG_IR_VALUE );

	/**
	 * Write Encryption root key used to derive LTK and CSRK
	 */
	aci_hal_write_config_data( CONFIG_DATA_ER_OFFSET, CONFIG_DATA_ER_LEN, ( uint8_t* )BLE_CFG_ER_VALUE );

	/**
	 * Set TX Power to 0dBm.
	 */
	aci_hal_set_tx_power_level( 1, CFG_TX_POWER );

	/**
	 * Initialize GATT interface
	 */
	aci_gatt_init();

	/**
	 * Initialize GAP interface
	 */
	role = 0;

#if ( BLE_CFG_PERIPHERAL == 1 )
	role |= GAP_PERIPHERAL_ROLE;
#endif

#if ( BLE_CFG_CENTRAL == 1 )
	role |= GAP_CENTRAL_ROLE;
#endif

	if ( role > 0 )
	{
		aci_gap_init( role, PRIVACY_DISABLED, m_device_name_length,
					  &gap_service_handle, &gap_dev_name_char_handle, &gap_appearance_char_handle );

		if ( aci_gatt_update_char_value( gap_service_handle, gap_dev_name_char_handle, 0, strlen( m_device_name ),
										 ( uint8_t* ) m_device_name ) )
		{
			BleErrorHandler( "Device Name aci_gatt_update_char_value failed.\n\r" );
		}
	}

	if ( aci_gatt_update_char_value( gap_service_handle, gap_appearance_char_handle,
									 0, 2, ( uint8_t* )&appearance ) )
	{
		BleErrorHandler( "Appearance aci_gatt_update_char_value failed.\n\r" );
	}

	// Initialize Default PHY
	hci_le_set_default_phy( ALL_PHYS_PREFERENCE, TX_2M_PREFERRED, RX_2M_PREFERRED );

	// Initialize IO capability
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability = CFG_IO_CAPABILITY;
	aci_gap_set_io_capability( BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability );

	// Initialize authentication
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode = CFG_MITM_PROTECTION;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data_Present = 0;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin = 8;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax = 16;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin = CFG_USED_FIXED_PIN;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin = CFG_FIXED_PIN;
	BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode = 1;
	for ( index = 0; index < 16; index++ )
	{
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data[index] = ( uint8_t ) index;
	}

	aci_gap_set_authentication_requirement(
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode,
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode,
		CFG_SC_SUPPORT,
		0,
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin,
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax,
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin,
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin,
		0
		);

	/**
	 * Initialize whitelist
	 */
	if ( BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode )
	{
		aci_gap_configure_whitelist();
	}
}

/**
 * @brief             Start the BLE adveritising. Stop the advertising by calling aci_gap_set_non_discoverable();
 */
void Adv_Request( void )
{
	tBleStatus ret = BLE_STATUS_INVALID_PARAMS;

	// Start advertising
	BleApplicationContext.Device_Connection_Status = APP_BLE_FAST_ADV;
	/* Start Fast or Low Power Advertising */
	ret = aci_gap_set_discoverable(
		ADV_IND,
		CFG_CONN_ADV_INTERVAL_MIN,
		CFG_CONN_ADV_INTERVAL_MAX,
		PUBLIC_ADDR,
		NO_WHITE_LIST_USE,	// use/not use white list
		0,
		0,
		0,
		0,
		0,
		0 );
	// Update Advertising data
	ret = aci_gap_update_adv_data( m_ad_data_size, ( uint8_t* ) m_ad_data );

	if ( ret != BLE_STATUS_SUCCESS )
	{
		BleErrorHandler( "Start advertising failed" );
	}

	return;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Adv_Stop( void )
{
	aci_gap_set_non_discoverable();

	// Set the TX Power back to default.
	aci_hal_set_tx_power_level( 1, CFG_TX_POWER );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Set_TX_Power( uint8_t tx_power_level )
{
	aci_hal_set_tx_power_level( 0, tx_power_level );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Discovery_Request( void )
{
	tBleStatus status = 0xFF;
	static int Start_Discovery_Failure_Count = 0;
	static int Start_Discovery_Success_Count = 0;

	if ( ( BleApplicationContext.Device_Connection_Status == APP_BLE_IDLE ) ||
		 ( BleApplicationContext.Device_Connection_Status == APP_BLE_FAST_ADV ) )
	{
		status = aci_gap_start_general_discovery_proc( LE_SCAN_INTERVAL, LE_SCAN_WINDOW, PUBLIC_ADDR, 0x00 );

		if ( status != BLE_STATUS_SUCCESS )
		{
			Start_Discovery_Failure_Count++;
		}
		else
		{
			Start_Discovery_Success_Count++;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stop_Discovery( void )
{
	tBleStatus status;
	static int Stop_Discovery_Failure_Count = 0;
	static int Stop_Discovery_Success_Count = 0;

	status = aci_gap_terminate_gap_proc( GAP_GENERAL_DISCOVERY_PROC );

	if ( status != BLE_STATUS_SUCCESS )
	{
		Stop_Discovery_Failure_Count++;
	}
	else
	{
		Stop_Discovery_Success_Count++;
	}
}

/*************************************************************
*   Utility functions
*************************************************************/

/**
 * @brief   Get the Bluetooth address
 * @return  Bluetooth device address
 */
static const uint8_t* BleGetBdAddress( void )
{
	uint8_t* otp_addr;
	const uint8_t* bd_addr;
	uint32_t udn;
	uint32_t company_id;
	uint32_t device_id;

	udn = LL_FLASH_GetUDN();

	if ( udn != 0xFFFFFFFF )
	{
		company_id = LL_FLASH_GetSTCompanyID();
		device_id = LL_FLASH_GetDeviceID();

		bd_addr_udn[0] = ( uint8_t )( udn & 0x000000FF );
		bd_addr_udn[1] = ( uint8_t )( ( udn & 0x0000FF00 ) >> 8 );
		bd_addr_udn[2] = ( uint8_t )( ( udn & 0x00FF0000 ) >> 16 );
		bd_addr_udn[3] = ( uint8_t )device_id;
		bd_addr_udn[4] = ( uint8_t )( company_id & 0x000000FF );
		bd_addr_udn[5] = ( uint8_t )( ( company_id & 0x0000FF00 ) >> 8 );

		bd_addr = ( const uint8_t* )bd_addr_udn;
	}
	else
	{
		otp_addr = OTP_Read( 0 );
		if ( otp_addr )
		{
			bd_addr = ( ( OTP_ID0_t* )otp_addr )->bd_address;
		}
		else
		{
			bd_addr = M_bd_addr;
		}
	}

	return ( bd_addr );
}

/**
 * @brief             Handles errors happening in this file
 * @param[in] msg     Error message
 */
void BleErrorHandler( const char* msg )
{
	// TODO: report as preferred
	return;
}

/*************************************************************
*   HCI event callbacks
*************************************************************/

/**
 * @brief    Receive notifications of HCI events from the stack. It needs to trigger hci_user_evt_proc to run via the
 * task manager.
 * @remark   This function runs only after APP_BLE_Init, which runs only after receiving an SHCI event, which is
 * processed (in shci_notify_asynch_evt_callback) only when the OS is running.
 * @remark   The number of concurrent HCI events is low, the queue can be set to a relatively small size (e.g., 10
 * elements) without worrying to make the queue full.
 */
void hci_notify_asynch_evt( void* pdata )
{
	// coverity[var_deref_model]
	hci_user_evt_proc_flag->Send( NULL, 0 );
	return;
}

/**
 * @brief    Processes HCI events received from the BLE stack by calling hci_user_evt_proc().
 */
static void hci_user_evt_proc_task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static CR_QUEUE_STATUS status;
	static uint32_t queue_receive;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Receive( cr_task, hci_user_evt_proc_flag, ( CR_RX_QUEUE_DATA )&queue_receive, 1000, status );
		if ( CR_QUEUE_PASS == status )
		{
			while ( hci_cmd_busy_status == HCI_CMD_BUSY )
			{
				CR_Tasker_Yield( cr_task );
			}
			hci_user_evt_proc();
		}
	}
	CR_Tasker_End();
}

/**
 * See declaration in hci_tl.h. This functions is weakly implemented in hci_tl.c but it has been reimplemented for
 * better performance.
 */
void hci_cmd_resp_release( uint32_t flag )
{
	HAL_HSEM_Release( HCI_CMD_RESP_HSEM, 0 );
}

/**
 * See declaration in hci_tl.h. This function must block if the response has not been released. This functions is weakly
 * implemented in hci_tl.c but it has been reimplemented for better performance.
 */
void hci_cmd_resp_wait( uint32_t timeout )
{
	HAL_HSEM_FastTake( HCI_CMD_RESP_HSEM );
	while ( HAL_HSEM_IsSemTaken( HCI_CMD_RESP_HSEM ) )
	{
		continue;
	}
}

/**
 * @brief               User event handler
 * @param[in] pPayload  Event payload
 */
static void BLE_UserEvtRx( void* pPayload )
{
	SVCCTL_UserEvtFlowStatus_t svctl_return_status;
	tHCI_UserEvtRxParam* pParam;

	pParam = ( tHCI_UserEvtRxParam* )pPayload;

	svctl_return_status = SVCCTL_UserEvtRx( ( void* )&( pParam->pckt->evtserial ) );
	if ( svctl_return_status != SVCCTL_UserEvtFlowDisable )
	{
		pParam->status = HCI_TL_UserEventFlow_Enable;
	}
	else
	{
		pParam->status = HCI_TL_UserEventFlow_Disable;
	}
}

/*
 * See declaration for function definition.
 */
static void BLE_StatusNot( HCI_TL_CmdStatus_t status )
{
	switch ( status )
	{
		case HCI_TL_CmdBusy:
			/*
			 *	Signal to all tasks that may send an aci/hci commands to block. This is to prevent sending a new command
			 * while one is already pending.
			 */

			hci_cmd_busy_status = HCI_CMD_BUSY;
			break;

		case HCI_TL_CmdAvailable:
			// Signal to all tasks that may send an aci/hci commands that they can continue.
			hci_cmd_busy_status = HCI_CMD_AVAILABLE;
			break;

		default:
			break;
	}
}

/*
 * See declaration for function definition.
 */
void SVCCTL_ResumeUserEventFlow( void )
{
	hci_resume_flow();
}

/**
 * @brief			Get_Max_BLE_Server_Buffer_Size
 * @param[out]		Max Communication buffer
 */
uint16_t Get_Max_BLE_Server_Buffer_Size( void )
{
	return ( MAX_BUFFER );	// TODO:Should be a negotiated value and should not be more than log data dcid length
}

/**
 * @brief			Application updates the handles and other parameters for the dci and stack interface files.
 * @param[in]		ble_dci_interface_handle
 * @param[in]		ble_stack_interface_handler
 * @param[in]		ad_data
 * @param[in]		ad_data_size
 * @param[in]		device_name
 * @param[in]		device_name_length
 * @param[in]		ble_init_cmd_packet
 */
void Init_Interface_Parameters( BLE_DCI_Interface* ble_dci_interface_handle,
								BLE_Stack_Interface* ble_stack_interface_handler, uint8_t const* ad_data,
								uint8_t ad_data_size, const char* device_name, uint8_t device_name_length,
								SHCI_C2_Ble_Init_Cmd_Packet_t* ble_init_cmd_packet )
{
	m_ble_dci_interface_handle = ble_dci_interface_handle;
	m_ble_stack_interface_handler = ble_stack_interface_handler;
	m_ad_data = ad_data;
	m_device_name_length = device_name_length;
	m_ad_data_size = ad_data_size;
	m_device_name = device_name;
	m_ble_init_cmd_packet = ble_init_cmd_packet;
}

/**
 * @brief			Returns BLE Stack interface handle
 * @param[out]		Handle
 */
BLE_Stack_Interface* Get_BLE_Stack_Interface_Handle( void )
{
	return ( m_ble_stack_interface_handler );
}
