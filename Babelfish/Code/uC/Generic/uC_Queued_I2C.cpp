/**
 **************************************************************************************************
 *@file           uC_Queued_I2C.cpp implementation File for I2C Queue implementation.
 *
 *@brief          Its a processor independent file for I2C queued class implementation
 *@details
 *@copyright      2014 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 **************************************************************************************************
 */
#include "Includes.h"
#include "uC_Queued_I2C.h"

/*
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
uC_Queued_I2C::uC_Queued_I2C( void )
{
	m_exchange_queue = reinterpret_cast<exchange_struct_t*>( nullptr );
	m_active_segment = reinterpret_cast<exchange_segment_t*>( nullptr );
}

/*
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
void uC_Queued_I2C::Initialize_Exchange_Struct( exchange_struct_t* exchange_struct )
{
	exchange_struct->next_exchange_struct = reinterpret_cast<struct exchange_struct_t*>( nullptr );
	exchange_struct->post_exchange_cback = reinterpret_cast<cback_func_t>( nullptr );
	exchange_struct->exchange_segment = reinterpret_cast<struct exchange_segment_t*>( nullptr );
	exchange_struct->status = EXCHANGE_COMPLETE;

}

/*
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
uC_Queued_I2C::exchange_segment_t* uC_Queued_I2C::Generate_Segment_Chain(
	exchange_segment_t* segment1,
	exchange_segment_t* segment2 )
{

	segment1->next_exchange_segment = segment2;
	segment2->next_exchange_segment = reinterpret_cast<struct exchange_segment_t*>( nullptr );
	return ( segment1 );
}

/*
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
void uC_Queued_I2C::Process_Exchange( exchange_struct_t* exchange_struct )
{
	exchange_struct_t* next;

	if ( nullptr != exchange_struct->exchange_segment )
	{
		exchange_struct->next_exchange_struct =
			reinterpret_cast<struct exchange_struct_t*>( nullptr );
		exchange_struct->status = EXCHANGE_PENDING;

		Push_TGINT();
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
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
void uC_Queued_I2C::Exchange_Complete( bool_t exchange_failure )
{
	exchange_struct_t* exchange_struct;
	bool_t exchange_complete;

	m_active_segment = m_active_segment->next_exchange_segment;

	if ( ( nullptr != m_active_segment ) &&
		 ( false == exchange_failure ) )
	{
		if ( ( 0U < m_active_segment->tx_length ) || ( 0U < m_active_segment->rx_length ) )
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
 **************************************************************************************************
 * See header file for function definition.
 **************************************************************************************************
 */
void uC_Queued_I2C::Check_Queue( void )
{
	if ( nullptr != m_exchange_queue )
	{
		m_active_segment = m_exchange_queue->exchange_segment;
		if ( ( 0U < m_active_segment->tx_length ) || ( 0U < m_active_segment->rx_length ) )
		{
			Start_Exchange( true );
		}
		else
		{
			Exchange_Complete( true );
		}
	}
}
