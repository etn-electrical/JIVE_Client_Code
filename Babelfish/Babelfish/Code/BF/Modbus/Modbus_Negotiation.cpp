/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Modbus_Negotiation.h"
#include "Modbus_Defines.h"
#include "Data_Align.hpp"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Negotiation::Modbus_Negotiation( Modbus_Net* modbus_net ) :
	m_modbus_net( modbus_net ),
	m_change_tasker( reinterpret_cast<CR_Tasker*>( nullptr ) ),
	m_baud( 0U ),
	m_use_long_holdoff( 0U ),
	m_parity( MODBUS_PARITY_NONE ),
	m_stop_bits( MODBUS_1_STOP_BIT ),
	m_tx_mode( MODBUS_RTU_TX_MODE )
{
	m_modbus_net = modbus_net;

	m_modbus_net->Attach_User_Function( MODBUS_NEGOTIATION_CODE, this, &Negotiate_Static );

	m_change_tasker = new CR_Tasker( &Baud_Process_Static, this,
									 CR_TASKER_IDLE_PRIORITY, "Modbus Negotiation" );

	m_change_tasker->Suspend();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Negotiation::~Modbus_Negotiation()
{
	delete m_modbus_net;
	delete m_change_tasker;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Negotiation::Parity( parity_t parity )
{
	uint8_t return_val;

	switch ( parity )
	{
		case MODBUS_PARITY_ODD:
			return_val = ODD_PARITY;
			break;

		case MODBUS_PARITY_NONE:
			return_val = NO_PARITY;
			break;

		case MODBUS_PARITY_EVEN:
		default:
			return_val = EVEN_PARITY;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
parity_t Modbus_Negotiation::Parity( uint8_t parity )
{
	parity_t return_val;

	switch ( parity )
	{
		case ODD_PARITY:
			return_val = MODBUS_PARITY_ODD;
			break;

		case NO_PARITY:
			return_val = MODBUS_PARITY_NONE;
			break;

		case EVEN_PARITY:
		default:
			return_val = MODBUS_PARITY_EVEN;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Negotiation::TX_Mode( tx_mode_t tx_mode )
{
	uint8_t return_val;

	switch ( tx_mode )
	{
		case MODBUS_ASCII_TX_MODE:
			return_val = ASCII;
			break;

		case MODBUS_RTU_TX_MODE:
		default:
			return_val = RTU;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
tx_mode_t Modbus_Negotiation::TX_Mode( uint8_t tx_mode )
{
	tx_mode_t return_val;

	switch ( tx_mode )
	{
		case ASCII:
			return_val = MODBUS_ASCII_TX_MODE;
			break;

		case RTU:
		default:
			return_val = MODBUS_RTU_TX_MODE;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Negotiation::Stop_Bits( stop_bits_t stop_bits )
{
	uint8_t return_val;

	switch ( stop_bits )
	{
		case MODBUS_2_STOP_BIT:
			return_val = STOP_BITS_2;
			break;

		case MODBUS_1_STOP_BIT:
		default:
			return_val = STOP_BITS_1;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
stop_bits_t Modbus_Negotiation::Stop_Bits( uint8_t stop_bits )
{
	stop_bits_t return_val;

	switch ( stop_bits )
	{
		case STOP_BITS_2:
			return_val = MODBUS_2_STOP_BIT;
			break;

		case STOP_BITS_1:
		default:
			return_val = MODBUS_1_STOP_BIT;
			break;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Negotiation::Long_Holdoff( bool long_holdoff )
{
	uint8_t return_val = USE_LONG_HOLDOFF_TRUE;

	if ( long_holdoff == false )
	{
		return_val = USE_LONG_HOLDOFF_FALSE;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Negotiation::Long_Holdoff( uint8_t long_holdoff )
{
	return ( long_holdoff == USE_LONG_HOLDOFF_TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Negotiation::Negotiate( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;

	m_tx_mode = TX_Mode( rx_struct->data[TX_MODE + CONFIG_DATA_OFFSET] );
	m_parity = Parity( rx_struct->data[PARITY + CONFIG_DATA_OFFSET] );
	m_stop_bits = Stop_Bits( rx_struct->data[STOP_BIT + CONFIG_DATA_OFFSET] );

	if ( rx_struct->data[USE_LONG_HOLDOFF + CONFIG_DATA_OFFSET] == USE_LONG_HOLDOFF_TRUE )
	{
		m_use_long_holdoff = true;
	}
	else
	{
		m_use_long_holdoff = false;
	}

	m_baud = Data_Align<uint32_t>
		::From_Array( &rx_struct->data[BAUD_RATE + CONFIG_DATA_OFFSET] );

	/// TODO:  More verification should be done in the future on the settings above.
	tx_struct->data[CONFIG_DATA_OFFSET] = CONFIG_SUCCESS;
	tx_struct->data_length = NEGOTIATION_FRAME_RESP_LENGTH;

	m_change_tasker->Resume();

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Negotiation::Baud_Process( void )
{
	if ( m_modbus_net->Port_Idle() )
	{
		m_modbus_net->Disable_Port();
		m_modbus_net->Enable_Port( m_tx_mode, m_parity, m_baud,
								   m_use_long_holdoff, m_stop_bits );
		m_change_tasker->Suspend();
	}
}
