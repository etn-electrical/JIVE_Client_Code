/**
 *****************************************************************************************
 *	@file uC_Queued_SPI.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Queued_SPI.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Queued_SPI::uC_Queued_SPI( uint8_t select_count ) :
	m_exchange_queue( reinterpret_cast<exchange_struct_t*>( nullptr ) ),
	m_active_segment( reinterpret_cast<exchange_segment_t*>( nullptr ) ),
	m_active_select( 0U ),
	m_select_count( 0U )
{
	m_select_count = select_count;
	m_exchange_queue = reinterpret_cast<exchange_struct_t*>( nullptr );
	m_active_segment = reinterpret_cast<exchange_segment_t*>( nullptr );
	m_active_select = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Queued_SPI::Initialize_Exchange_Struct( exchange_struct_t* exchange_struct )
{
	exchange_struct->next_exchange_struct = reinterpret_cast<struct exchange_struct_t*>( nullptr );
	exchange_struct->post_exchange_cback = reinterpret_cast<cback_func_t>( nullptr );
	exchange_struct->exchange_segment = reinterpret_cast<struct exchange_segment_t*>( nullptr );
	exchange_struct->status = EXCHANGE_COMPLETE;
	exchange_struct->select_id = 0xFFU;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Queued_SPI::exchange_segment_t* uC_Queued_SPI::Generate_Segment_Chain(
	exchange_segment_t* segment1,
	exchange_segment_t* segment2,
	uint8_t* rx_buffer,
	uint8_t* tx_buffer,
	uint32_t rx_length,
	uint32_t tx_length )
{
	segment1->rx_data = rx_buffer;
	segment1->tx_data = tx_buffer;

	if ( rx_length == tx_length )
	{
		segment1->length = static_cast<uint16_t>( rx_length );
		segment1->next_exchange_segment = reinterpret_cast<struct exchange_segment_t*>( nullptr );
	}
	else
	{
		segment1->next_exchange_segment = segment2;
		segment2->next_exchange_segment = reinterpret_cast<struct exchange_segment_t*>( nullptr );

		if ( rx_length < tx_length )
		{
			segment1->length = static_cast<uint16_t>( rx_length );
			segment2->rx_data = reinterpret_cast<uint8_t*>( nullptr );
			segment2->tx_data = &tx_buffer[rx_length];
			segment2->length = static_cast<uint16_t>( tx_length - rx_length );
		}
		else
		{
			segment1->length = static_cast<uint16_t>( tx_length );
			segment2->rx_data = &rx_buffer[tx_length];
			segment2->tx_data = reinterpret_cast<uint8_t*>( nullptr );
			segment2->length = static_cast<uint16_t>( rx_length - tx_length );
		}
	}

	return ( segment1 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Queued_SPI::Process_Exchange( exchange_struct_t* exchange_struct )
{
	exchange_struct_t* next;

	if ( ( exchange_struct->select_id < m_select_count ) &&
		 ( nullptr != exchange_struct->exchange_segment ) )
	{
		exchange_struct->next_exchange_struct =
			reinterpret_cast<struct exchange_struct_t*>( nullptr );
		exchange_struct->status = EXCHANGE_PENDING;

		Push_TGINT();
		/* Adding the element to queue */
		if ( nullptr != m_exchange_queue )
		{
			next = m_exchange_queue;
			while ( nullptr != next->next_exchange_struct )
			{
				next = next->next_exchange_struct;
			}
			next->next_exchange_struct = exchange_struct;
			Pop_TGINT();
		}
		else
		{
			m_exchange_queue = exchange_struct;
			Pop_TGINT();
			Check_Queue();
		}
	}
	else
	{
		exchange_struct->status = EXCHANGE_FAILED;

		if ( nullptr != exchange_struct->post_exchange_cback )
		{
			exchange_struct->post_exchange_cback( exchange_struct->call_back_arg );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Queued_SPI::Exchange_Complete( bool exchange_failure )
{
	exchange_struct_t* exchange_struct;
	bool exchange_complete;

	m_active_segment = m_active_segment->next_exchange_segment;

	if ( ( nullptr != m_active_segment ) &&
		 ( false == exchange_failure ) )
	{
		if ( 0U < m_active_segment->length )
		{
			Start_Exchange( false );
			exchange_complete = false;
		}
		else
		{
			exchange_failure = true;
			exchange_complete = true;
		}
	}
	else
	{
		exchange_complete = true;
	}

	if ( false != exchange_complete )
	{
		exchange_struct = m_exchange_queue;

		/* Advance to next exchange in queue and start exchange */
		Push_TGINT();
		m_exchange_queue = m_exchange_queue->next_exchange_struct;
		Pop_TGINT();
		Check_Queue();

		/* Complete post exchange processing of recently completed exchange */
		exchange_struct->status = ( false == exchange_failure ) ?
			EXCHANGE_COMPLETE : EXCHANGE_FAILED;

		if ( nullptr != exchange_struct->post_exchange_cback )
		{
			exchange_struct->post_exchange_cback( exchange_struct->call_back_arg );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Queued_SPI::Check_Queue( void )
{
	if ( nullptr != m_exchange_queue )
	{
		m_active_segment = m_exchange_queue->exchange_segment;
		m_active_select = m_exchange_queue->select_id;
		if ( 0U < m_active_segment->length )
		{
			Start_Exchange( true );
		}
		else
		{
			Exchange_Complete( true );
		}
	}
}
