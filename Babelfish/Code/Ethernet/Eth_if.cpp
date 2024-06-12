/**
 **********************************************************************************************
 *	@file            Ethernet.cpp C++ Implementation File for Ethernet common functions.
 *
 *	@brief           The file shall include the definitions of all the functions required for Ethernet initilization.
 *	@details
 *	@copyright 		 2014 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed hereon.  This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "StdLib_MV.h"
#include "Eth_if.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Assert.h"
#ifdef ESP32_SETUP
#include "OS_Task_Config.h"
#endif

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */

DCI_Owner* active_ip_address_owner;
DCI_Owner* active_netmask_owner;
DCI_Owner* active_default_gateway_owner;
DCI_Owner* static_ip_address_owner;
DCI_Owner* static_netmask_owner;
DCI_Owner* static_default_gateway_owner;
CALLBACK_FUNC* Eth_if::m_post_setup_func;
// These are the roads into the active ip address owners which need to be
// created before the stack gets up and running.

Eth_if* Eth_if::m_self = reinterpret_cast<Eth_if*>( NULL );
Eth_PHY_If* Eth_if::m_eth_phy = reinterpret_cast<Eth_PHY_If*>( NULL );
BOOL Eth_if::m_ip_address_locked = false;
uC_ETHERNET_IP_CONFIG_ST Eth_if::m_ip_config;
extern bool_t ip_adrs_cntrl_from_ethernet_lock_active_ip;
void( *Eth_if::m_update_hostname_cb )( uint32_t ip ) = nullptr;

#define ETH_STARTUP_DELAY_MS                    500U				/* Ethernet task (Ethernet_Start_Thread_Static)
																	   Start-up delay */
#define ETH_PHY_POLLING_PERIOD_MS               257U				/* Ethernet task (Ethernet_Start_Thread_Static)
																	   polling period to check PHY chip status in loop
																	 */
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Eth_if::Eth_if( PHY_Driver* phy_ctrl, uint8_t total_ports,
				Eth_Port::eth_dcid_t const* eth_dcids,
				uC_ETHERNET_IP_CONFIG_ST* ip_config,
				CALLBACK_FUNC* post_setup_func )
{
	m_eth_phy = new Eth_PHY_If( phy_ctrl, total_ports, eth_dcids );
	OS_Task* os_task = nullptr;

	os_task = OS_Tasker::Create_Task( &Ethernet_Start_Thread_Static,
									  static_cast<uint16_t>( uC_EMAC_START_THREAD_OS_STACK_SIZE ),
									  static_cast<uint8_t>( uC_EMAC_START_THREAD_OS_PRIORITY ),
									  reinterpret_cast<uint8_t const*>( "Ethernet Start" ),
									  reinterpret_cast<OS_TASK_PARAM>( this ) );
	BF_ASSERT( os_task );
	m_ip_config = *ip_config;

	m_self = this;
	m_post_setup_func = post_setup_func;
	m_ip_address_locked = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Eth_if::UpdateActiveIPConfig( void )
{
	m_self->Update_IP_Config();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

BOOL Eth_if::InitialLinkCheck( void )
{
	return ( m_eth_phy->Initial_Link_Check() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_if::Reset_PHY( void )
{
	return ( m_eth_phy->Reset_PHY() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Eth_if::Ethernet_Start_Thread( void ) const
{
	IP_CONFIG stack_ipconfig;

	OS_Tasker::Delay(
		static_cast<uint32_t>( ETH_STARTUP_DELAY_MS ) / static_cast<uint32_t>( OS_TICK_RATE_MS ) );

	if ( m_ip_config.ip_select == uC_ETHERNET_IP_SELECT_DHCP )
	{
		stack_ipconfig.IPMode = static_cast<uint32_t>( IPADDR_USE_DHCP );
	}
	else
	{
		stack_ipconfig.IPMode = static_cast<uint32_t>( IPADDR_USE_STATIC );
		stack_ipconfig.IPAddr = m_ip_config.ip_address;
		stack_ipconfig.NetMask = m_ip_config.subnet_mask;
		stack_ipconfig.GWAddr = m_ip_config.default_gateway;
	}

	LWIPServiceTaskInit( reinterpret_cast<void*>( &stack_ipconfig ) );	/// Must be called from an OS task.

	Execute_Callback_Func( m_post_setup_func );

	// This is it.  We are done.  We are out.
	// OS_Tasker::Delete_Task();
	// coverity[no_escape]
	for (;;)
	{
		m_eth_phy->Update_PHY_Status();
		OS_Tasker::Delay( ETH_PHY_POLLING_PERIOD_MS / OS_TICK_RATE_MS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_if::Wait_Till_Ip_Configured( void )
{
	struct in_addr active_ip_address;
	uC_ETHERNET_IP_CONFIG_ST temp_active_ip_address;

	ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "Waiting till IP is allocated" );
	uC_Watchdog::Force_Feed_Dog();

	do
	{
		temp_active_ip_address.ip_address = 0;
		temp_active_ip_address.subnet_mask = 0;
		temp_active_ip_address.default_gateway = 0;
		Get_IP_Config( &temp_active_ip_address );
		OS_Tasker::Delay( 500 );
		// Need to pet the watchdog here, since we might spend some significant time in this loop
		uC_Watchdog::Force_Feed_Dog();
	} while ( temp_active_ip_address.ip_address == 0 );

	active_ip_address.s_addr = temp_active_ip_address.ip_address;
	uint8_t first = active_ip_address.s_addr & 0xFF;
	uint8_t second = ( ( active_ip_address.s_addr & 0xFF00 ) >> 8 );
	uint8_t third = ( ( active_ip_address.s_addr & 0xFF0000 ) >> 16 );
	uint8_t fourth = ( ( active_ip_address.s_addr & 0xFF000000 ) >> 24 );

	active_ip_address.s_addr = fourth | ( third << 8 ) | ( second << 16 ) | ( first << 24 );

	char src_ip_addr[16] = "000.000.000.000";
	char* ipv4_addr = ( char* )inet_ntoa( active_ip_address );

	if ( ipv4_addr != nullptr )
	{
		strncpy( src_ip_addr, ipv4_addr, sizeof( src_ip_addr ) );
	}

	src_ip_addr[sizeof( src_ip_addr ) - 1] = '\0';

	ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "IP allocated - %s", src_ip_addr );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Eth_if::Is_Ip_Configured( void )
{
	struct in_addr active_ip_address;
	bool_t ret_val = false;

	active_ip_address.s_addr = 0;
	active_ip_address_owner->Check_Out( reinterpret_cast<uint8_t*>( &active_ip_address.s_addr ) );

	if ( active_ip_address.s_addr != 0 )
	{
		ret_val = true;

		uint8_t first = active_ip_address.s_addr & 0xFF;
		uint8_t second = ( ( active_ip_address.s_addr & 0xFF00 ) >> 8 );
		uint8_t third = ( ( active_ip_address.s_addr & 0xFF0000 ) >> 16 );
		uint8_t fourth = ( ( active_ip_address.s_addr & 0xFF000000 ) >> 24 );

		active_ip_address.s_addr = fourth | ( third << 8 ) | ( second << 16 ) | ( first << 24 );

		char src_ip_addr[16] = "000.000.000.000";

		char* ipv4_addr = ( char* )inet_ntoa( active_ip_address );
		if ( ipv4_addr != nullptr )
		{
			strncpy( src_ip_addr, ipv4_addr, sizeof( src_ip_addr ) );
		}
		src_ip_addr[sizeof( src_ip_addr ) - 1] = '\0';

		ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "IP allocated  - %s", src_ip_addr );
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_if::Update_IP_Config( void ) const
{
	uC_ETHERNET_IP_CONFIG_ST temp_active_ip_address;
	uC_ETHERNET_IP_CONFIG_ST static_ip_address;

	temp_active_ip_address.ip_address = 0;
	temp_active_ip_address.subnet_mask = 0;
	temp_active_ip_address.default_gateway = 0;

	Get_IP_Config( &temp_active_ip_address );

	active_ip_address_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.ip_address ) );

	active_netmask_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.subnet_mask ) );

	active_default_gateway_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.default_gateway ) );


	/* if((ip_adrs_select_control_status ==IP_ADDRESS_SELECT_FROM_ETHERNET_STATUS)
	   && ( ip_adrs_cntrl_from_ethernet_lock_active_ip)) */

	// Dont write static IP if DHCP assigned IP is 0
	if ( ( ip_adrs_cntrl_from_ethernet_lock_active_ip ) && ( 0U != temp_active_ip_address.ip_address ) )
	{
		static_ip_address = temp_active_ip_address;

		static_ip_address_owner->Check_In(
			reinterpret_cast<uint8_t*>( &static_ip_address.ip_address ) );

		static_netmask_owner->Check_In(
			reinterpret_cast<uint8_t*>( &static_ip_address.subnet_mask ) );

		static_default_gateway_owner->Check_In(
			reinterpret_cast<uint8_t*>( &static_ip_address.default_gateway ) );

		static_ip_address_owner->Check_In_Init(
			reinterpret_cast<uint8_t*>( &static_ip_address.ip_address ) );

		static_netmask_owner->Check_In_Init(
			reinterpret_cast<uint8_t*>( &static_ip_address.subnet_mask ) );

		static_default_gateway_owner->Check_In_Init(
			reinterpret_cast<uint8_t*>( &static_ip_address.default_gateway ) );
	}
	if ( m_update_hostname_cb != nullptr )
	{
		m_update_hostname_cb( temp_active_ip_address.ip_address );
	}

	if ( temp_active_ip_address.ip_address != 0 )
	{
		char src_ip_addr[16] = "000.000.000.000";
		uint8_t first = temp_active_ip_address.ip_address & 0xFF;
		uint8_t second = ( ( temp_active_ip_address.ip_address & 0xFF00 ) >> 8 );
		uint8_t third = ( ( temp_active_ip_address.ip_address & 0xFF0000 ) >> 16 );
		uint8_t fourth = ( ( temp_active_ip_address.ip_address & 0xFF000000 ) >> 24 );

		in_addr temp_ip;
		temp_ip.s_addr = fourth | ( third << 8 ) | ( second << 16 ) | ( first << 24 );

		uint8_t* buffer_ptr = reinterpret_cast<uint8_t*>( inet_ntoa( temp_ip ) );
		BF_ASSERT( buffer_ptr != nullptr );

		strncpy( src_ip_addr, ( char* )buffer_ptr, sizeof( src_ip_addr ) );

		src_ip_addr[sizeof( src_ip_addr ) - 1] = '\0';

		ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "IP allocated - %s", src_ip_addr );
	}
	else
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "IP address is not allocated...please wait" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL Eth_if::Get_IP_Config( uC_ETHERNET_IP_CONFIG_ST* active_ip_config )
{
	IP_CONFIG ip_config;

	LWIPServiceGetIPDef( &ip_config );

	BF_Lib::Copy_String( reinterpret_cast<UINT8*>( &active_ip_config->ip_address ),
						 reinterpret_cast<UINT8*>( &ip_config.IPAddr ),
						 sizeof ( active_ip_config->ip_address ) );
	BF_Lib::Copy_String( reinterpret_cast<UINT8*>( &active_ip_config->subnet_mask ),
						 reinterpret_cast<UINT8*>( &ip_config.NetMask ),
						 sizeof ( active_ip_config->ip_address ) );
	BF_Lib::Copy_String( reinterpret_cast<UINT8*>( &active_ip_config->default_gateway ),
						 reinterpret_cast<UINT8*>( &ip_config.GWAddr ),
						 sizeof ( active_ip_config->ip_address ) );
	active_ip_config->ip_select = m_ip_config.ip_select;

	return ( m_ip_address_locked );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t const* Eth_if::Read_Diag_Information( uint8_t port_index ) const
{
	return ( m_eth_phy->Read_Diag_Information( port_index ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* const* Eth_if::Read_Phy_Driver_Error_List() const
{
	return ( m_eth_phy->Read_Phy_Driver_Error_List() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Eth_if::Read_Port_Count() const
{
	return ( m_eth_phy->Read_Port_Count() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_if::Update_Hostname_Handle( void ( * fptr_update_hostname )( uint32_t ) )
{
	m_self->m_update_hostname_cb = fptr_update_hostname;
}
