/**
 **********************************************************************************************
 * @file            Eth_Port.cpp C++ Implementation File for Eth_Port software module.
 *
 * @brief           The file contains all APIs required for accessing port specific settings.
 *                  This class is responsible for individually operate and control
 *                  each Ethernet port of a product.
 * @details         See header file for module overview.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Eth_Port.h"

// Initialize static member variable
PHY_Driver* Eth_Port::m_eth_phy_driver = reinterpret_cast<PHY_Driver*>( nullptr );
// constant array used for reading/updating port speed
const Eth_Port::Eth_Link_Speed_t Eth_Port::LINK_SPEED_ARRAY[PHY_Driver::SPEED_OPTIONS] =
{ ETH_LINK_SPEED_UNDETERMINED, ETH_LINK_SPEED_10M, ETH_LINK_SPEED_100M, ETH_LINK_SPEED_1G };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Eth_Port::Eth_Port( uint8_t port, eth_dcid_t const* eth_dcids ) :
	m_port_up_on_curr_poll( false ),
	m_port_up_on_last_poll( false ),
	m_active_autoneg_own( new DCI_Owner( eth_dcids->eth_active_link_autoneg_state ) ),
	m_active_full_dup_own( new DCI_Owner( eth_dcids->eth_active_link_duplex ) ),
	m_active_speed_own( new DCI_Owner( eth_dcids->eth_active_link_speed ) ),
	m_link_status( PHY_Driver::LINK_DOWN ),
	m_link_ack( PHY_Driver::LINK_PARNTER_ACK_DISABLE ),
	m_port( port )
{
	bool_t p_ena;
	bool_t p_autoneg_ena;
	bool_t p_full_dup;
	uint16_t p_speed;

	for ( uint8_t idx = 0U; idx < 4U; idx++ )
	{
		m_diag_counter[idx] = 0U;
	}
	// Create DCI Owners for port configuration DCIDs
	DCI_Owner* port_ena_own = new DCI_Owner( eth_dcids->eth_port_enable_status );
	DCI_Owner* auto_neg_own = new DCI_Owner( eth_dcids->eth_port_auto_neg_ena );
	DCI_Owner* full_dup_own = new DCI_Owner( eth_dcids->eth_port_full_duplex_ena );
	DCI_Owner* link_speed_own = new DCI_Owner( eth_dcids->eth_port_link_speed_select );

	// Clear port configuration word before use
	m_port_config.port_config_word = PHY_Driver::CLEAR_CONFIG_WORD;

	// Update Read port enable/disable value from DCID and update to port configuration word
	port_ena_own->Check_Out( p_ena );
	m_port_config.fields.port_enable =
		( TRUE == p_ena ) ? PHY_Driver::PORT_ENABLE : PHY_Driver::PORT_DISABLE;

	// Update Read port auto negotiation  value from DCID and update to port configuration word
	auto_neg_own->Check_Out( p_autoneg_ena );
	m_port_config.fields.auto_neg =
		( TRUE == p_autoneg_ena ) ? PHY_Driver::AUTO_NEG_ENABLE :
		PHY_Driver::AUTO_NEG_DISABLE;

	// Update Read port duplex value from DCID and update to port configuration word
	full_dup_own->Check_Out( p_full_dup );
	m_port_config.fields.duplex =
		( TRUE == p_full_dup ) ? PHY_Driver::FULL_DUPLEX : PHY_Driver::HALF_DUPLEX;

	// Update Read port speed value from DCID and update to port configuration word
	link_speed_own->Check_Out( p_speed );
	for ( uint8_t i = 0U; i < PHY_Driver::SPEED_OPTIONS; i++ )
	{
		if ( LINK_SPEED_ARRAY[i] == p_speed )
		{
			m_port_config.fields.speed = i;
			break;
		}
		else
		{
			// comment to avoid misra warning
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Eth_Port::Set_Port_Config()
{
	bool_t result = false;
	PHY_Driver::port_config_t config_word;

	// power down port if port is set inactive in DCID
	if ( false == m_port_config.fields.port_enable )
	{
		config_word.fields.port_enable = PHY_Driver::PORT_DISABLE;
		config_word.fields.auto_neg = PHY_Driver::AUTO_NEG_DISABLE;
		config_word.fields.speed = PHY_Driver::SPEED_10MBPS;
		config_word.fields.duplex = PHY_Driver::HALF_DUPLEX;

		// all default values, 10Mbps, half-duplex,PHY_AN disabled (if not passed default value will be set)
		if ( true == m_eth_phy_driver->Write_Port_Config( m_port, &config_word ) )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else
	{
		// now the port must be enabled, check if auto negotiation is enabled
		if ( true == m_port_config.fields.auto_neg )
		{
			config_word.fields.port_enable = PHY_Driver::PORT_ENABLE;
			config_word.fields.auto_neg = PHY_Driver::AUTO_NEG_ENABLE;

			// enable Auto Negotiation
			if ( true == m_eth_phy_driver->Write_Port_Config( m_port, &config_word ) )
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else
		{
			// if auto negotiation is disabled, Set port's values received from NV mem(DCID)
			if ( true == m_eth_phy_driver->Write_Port_Config( m_port, &m_port_config ) )
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_Port::Update_Port_Status()
{
	bool_t temp_duplex = false;
	Eth_Link_Speed_t temp_speed = ETH_LINK_SPEED_UNDETERMINED;
	Eth_AN_State_t temp_aneg_state = ETH_AUTO_NEG_STATE_PORT_DISABLED;
	PHY_Driver::port_config_t read_status_word;

	// Clear port configuration word before use
	read_status_word.port_config_word = PHY_Driver::CLEAR_CONFIG_WORD;

	// Read current status of port registers
	bool_t read_status = m_eth_phy_driver->Read_Port_Config( m_port, &read_status_word );

	if ( read_status == true )
	{
		if ( read_status_word.fields.port_enable == PHY_Driver::PORT_ENABLE )
		{
			if ( read_status_word.fields.link_good == PHY_Driver::LINK_UP )
			{
				m_port_up_on_curr_poll = true;

				if ( m_link_status == PHY_Driver::LINK_DOWN )
				{
					m_link_status = PHY_Driver::LINK_UP;
					m_diag_counter[PHY_Driver::LINK_UP_OFFSET]++;
				}

				// check if auto negotiation is enabled
				if ( read_status_word.fields.auto_neg == PHY_Driver::AUTO_NEG_ENABLE )
				{
					if ( read_status_word.fields.auto_neg_done == PHY_Driver::AUTO_NEG_DONE )
					{
						temp_aneg_state = ETH_AUTO_NEG_STATE_SUCCESS;
					}
					else
					{
						temp_aneg_state = ETH_AUTO_NEG_STATE_IN_PROGRESS;
					}
				}
				else
				{
					temp_aneg_state = ETH_AUTO_NEG_STATE_NEGOTIATION_DISABLED;
				}
				// Write port's current speed and duplex values
				temp_speed = LINK_SPEED_ARRAY[read_status_word.fields.speed];
				temp_duplex = static_cast<bool_t>( read_status_word.fields.duplex );
			}
			else
			{
				m_port_up_on_curr_poll = false;
				temp_aneg_state = ETH_AUTO_NEG_STATE_LINK_INACTIVE;
				if ( m_link_status == PHY_Driver::LINK_UP )
				{
					m_link_status = PHY_Driver::LINK_DOWN;
					m_diag_counter[PHY_Driver::LINK_DOWN_OFFSET]++;
				}
			}
			Update_Port_Error_Counts( read_status_word );
		}
		else
		{
			m_port_up_on_curr_poll = false;
		}
	}
	else
	{
		// Don't do anything as the read failed
	}

	// Update port DCIDs with latest port status
	m_active_speed_own->Check_In( temp_speed );
	m_active_full_dup_own->Check_In( temp_duplex );
	m_active_autoneg_own->Check_In( temp_aneg_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_Port::Select_PHY( PHY_Driver* phy_driver )
{
	// Assign received PHY Driver handle to Eth_Port member pointer variable.
	// This handle will be used for further communication with PHY chip
	m_eth_phy_driver = phy_driver;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint32_t* Eth_Port::Read_Port_Diag_Info()
{
	return ( m_diag_counter );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Eth_Port::Update_Port_Error_Counts( const PHY_Driver::port_config_t& read_status_word )
{


	if ( read_status_word.fields.jabber == PHY_Driver::JABBER_DETECT_ON )
	{
		m_diag_counter[PHY_Driver::JABBER_DETECT_OFFSET]++;
	}

	if ( read_status_word.fields.remote_fault == PHY_Driver::REMOTE_FAULT_ON )
	{
		m_diag_counter[PHY_Driver::REMOTE_FAULT_OFFSET]++;
	}
	if ( read_status_word.fields.link_partner_ack == PHY_Driver::LINK_PARNTER_ACK_ENABLE )
	{
		if ( ( m_link_ack == PHY_Driver::LINK_PARNTER_ACK_DISABLE ) )
		{
			m_diag_counter[PHY_Driver::LINK_PARNTER_ACK_OFFSET]++;
			m_link_ack = PHY_Driver::LINK_PARNTER_ACK_ENABLE;
		}
	}
	else
	{
		if ( ( m_link_ack == PHY_Driver::LINK_PARNTER_ACK_ENABLE ) )
		{
			m_link_ack = PHY_Driver::LINK_PARNTER_ACK_DISABLE;
		}
	}

	if ( read_status_word.fields.parallel_detect_flt == PHY_Driver::PARALLEL_DETECT_FAULT_ON )
	{
		m_diag_counter[PHY_Driver::PARALLEL_DETECT_FAULT_OFFSET]++;
	}

	if ( read_status_word.fields.page_rx == PHY_Driver::PAGE_RX )
	{
		m_diag_counter[PHY_Driver::PAGE_RX_OFFSET]++;
	}
	m_diag_counter[PHY_Driver::RX_ERROR_OFFSET] = static_cast<uint32_t>( read_status_word.fields.rx_error_counter );
	m_diag_counter[PHY_Driver::TX_ERROR_OFFSET] = static_cast<uint32_t>( read_status_word.fields.tx_error_counter );
}
