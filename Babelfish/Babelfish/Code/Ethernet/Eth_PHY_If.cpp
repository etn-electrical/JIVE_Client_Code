/**
 **********************************************************************************************
 * @file            ETH_PHY_If.cpp C++ Implementation File for interfacing PHY Driver software
 * module.
 *
 * @brief           The file contains ETH_PHY_If, an interface class required
 *                  for communication with PHY/SWITCH Driver.
 * @details         See header file for module overview.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Eth_PHY_If.h"
#include <string.h>
#include "lwip/netif.h"
#include "Ethernet_Debug.h"

// The following extern is grabbed from the stack.
extern struct netif lwip_netif;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Eth_PHY_If::Eth_PHY_If( PHY_Driver* phy_ctrl,
						uint8_t total_ports, Eth_Port::eth_dcid_t const* eth_dcids ) :
	m_initial_polling( true ),
	m_phy_handle( phy_ctrl ),
	m_port( reinterpret_cast<Eth_Port**>( nullptr ) ),
	m_total_ports( total_ports )
{
	// allocate memory for pointers to all Eth_Port instances
	m_port = new Eth_Port*[total_ports];

	uint8_t index = 0U;

	// Eth_Port class instance construction for each port
	while ( index < total_ports )
	{
		m_port[index] = new Eth_Port( index, &eth_dcids[index] );

		index++;
	}
	// Assign PHY Driver handle(received from Ethernet class) to PHY handler in Eth_Port
	// Eth_Port will handle all port specific operations using this handle
	Eth_Port::Select_PHY( phy_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_PHY_If::Reset_PHY()
{
	// PHY specific Initialization
	m_phy_handle->Initialize_PHY();

	// Set PHY's port configuration received from DCIDs
	for ( uint8_t port_index = 0U; port_index < m_total_ports; port_index++ )
	{
		m_port[port_index]->Set_Port_Config();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_PHY_If::Update_PHY_Status( void )
{
	// Read Port status from PHY Registers and update to associated DCIDs
	for ( uint8_t port_index = 0U; port_index < m_total_ports; port_index++ )
	{
		m_port[port_index]->Update_Port_Status();
	}

	// Check if this is first execution
	if ( m_initial_polling )
	{
		m_initial_polling = false;
	}
	else
	{
		// Set correct Netif Link depending upon status of port's current and last polling
		Set_Netif_Link();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Eth_PHY_If::Initial_Link_Check( void )
{
	bool_t current_polling = false;

	// Read the Port configuration and update into DCIDs
	Update_PHY_Status();

	// return true if any port is up currently
	for ( uint8_t port_index = 0U; port_index < m_total_ports; port_index++ )
	{
		current_polling =
			( current_polling || m_port[port_index]->m_port_up_on_curr_poll );
	}
	return ( current_polling );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_PHY_If::Set_Netif_Link() const
{

	bool_t last_poll = false;
	bool_t current_poll = false;
	bool_t all_curr_poll_up = true;
	bool_t any_last_poll_down = false;

	for ( uint8_t i = 0U; i < m_total_ports; i++ )
	{
		// True if any port was up in last polling
		last_poll = last_poll || m_port[i]->m_port_up_on_last_poll;
		// True if any port is up in current polling
		current_poll = current_poll || m_port[i]->m_port_up_on_curr_poll;
		// True if all active ports are up in current polling
		all_curr_poll_up = all_curr_poll_up && m_port[i]->m_port_up_on_curr_poll;
		// True if any port was down in last polling
		any_last_poll_down =
			( any_last_poll_down == m_port[i]->m_port_up_on_last_poll ) ? false : true;

		m_port[i]->m_port_up_on_last_poll = m_port[i]->m_port_up_on_curr_poll;
	}

	if ( ( last_poll == false ) && ( current_poll == true ) )
	{
		// links from both down to at least one up
		netif_set_link_up( &lwip_netif );
		ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "netif_set_link_up\r\n" );
	}
	else if ( ( last_poll == true ) && ( current_poll == false ) )
	{
		// links from at least one up to both down
		netif_set_link_down( &lwip_netif );
		ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "netif_set_link_down\r\n" );
	}
	else if ( ( all_curr_poll_up == true ) && ( any_last_poll_down == true ) &&
			  ( m_total_ports > 1U ) )
	{
		// links from only one up to both up
#ifndef ESP32_SETUP
		netif_link_semi_up( &lwip_netif );
#endif
	}
	else
	{
		// comment to avoid misra warning
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t const* Eth_PHY_If::Read_Diag_Information( uint8_t port_index ) const
{
	return ( m_port[port_index]->Read_Port_Diag_Info() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* const* Eth_PHY_If::Read_Phy_Driver_Error_List() const
{
	return ( m_phy_handle->Get_Phy_Driver_Error_List() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Eth_PHY_If::Read_Port_Count() const
{
	return ( m_total_ports );
}
