/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_DCI_Config.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */// Slave implementation.
Modbus_DCI_Config::Modbus_DCI_Config( Modbus_Net* modbus_net_ctrl,
									  DCI_ID_TD address_id, DCI_ID_TD parity_id,
									  DCI_ID_TD tx_mode_id, DCI_ID_TD baud_rate_id,
									  bool use_long_holdoff ) :
	m_address_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_parity_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_stop_bit_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_tx_mode_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_baud_rate_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_modbus_net( modbus_net_ctrl ),
	m_use_long_holdoff( use_long_holdoff )
{
	uint8_t new_address;

	m_modbus_net = modbus_net_ctrl;
	m_use_long_holdoff = use_long_holdoff;
	m_address_owner = new DCI_Owner( address_id );

	m_address_owner->Check_Out( new_address );
	m_modbus_net->Change_Address( new_address );
	m_parity_owner = new DCI_Owner( parity_id );

	m_tx_mode_owner = new DCI_Owner( tx_mode_id );

	m_baud_rate_owner = new DCI_Owner( baud_rate_id );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI_Config::Enable_Modbus( void )
{
	uint8_t tx_mode;
	uint8_t dci_tx_mode;
	uint8_t parity;
	uint8_t stop_bits;
	uint32_t baud_rate;

	m_parity_owner->Check_Out( parity );
	m_stop_bit_owner->Check_Out( stop_bits );
	m_tx_mode_owner->Check_Out( dci_tx_mode );
	m_baud_rate_owner->Check_Out( baud_rate );
	tx_mode = dci_tx_mode;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers,
	 * not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	m_modbus_net->Enable_Port( static_cast<tx_mode_t>( tx_mode ),
							   static_cast<parity_t>( parity ), baud_rate,
							   m_use_long_holdoff,
							   static_cast<stop_bits_t>( stop_bits ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI_Config::Disable_Modbus( void )
{
	m_modbus_net->Disable_Port();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_DCI_Config::Is_Modbus_Enabled( void )
{
	return ( m_modbus_net->Port_Enabled() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI_Config::Update_TX_Mode( uint8_t tx_mode ) const
{
	m_tx_mode_owner->Check_In( tx_mode );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI_Config::Update_Baud_Rate( uint32_t baud_rate ) const
{
	m_baud_rate_owner->Check_In( baud_rate );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI_Config::Update_Parity( uint8_t parity ) const
{
	m_parity_owner->Check_In( parity );
}

}	/* End namespace BF_Mbus for this module*/
