/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Faults.h"
#include "NV_Mem.h"
#include "Ram.h"
#include "Bit.hpp"
#include "Faults_Debug.h"

namespace BF_Misc
{

static const uint_fast8_t FAULTS_RESET_FAULT_REQ_BIT = 0U;
static const uint_fast8_t FAULTS_RESET_FLUSH_REQ_BIT = 1U;
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define FAULTS_LATCHED_BITS_NV_LEN      ( FAULTS_TOTAL_BIT_WORDS * sizeof( FAULTS_BIT_TD ) )

typedef enum POST_EVENT_STATE_MACHINE
{
	POST_EVENT_IDLE,
	INITIATE_STATE_MACHINE,
	UPDATE_FAULT_NV_DATA,
	CALL_STATE_CHANGE_MONITORS,
	MAX_POST_EVENTS
} POST_EVENT_STATE_MACHINE;

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
DCI_Owner* Faults::m_sorted_que_owner;
DCI_Owner* Faults::m_event_que_owner;
DCI_Owner* Faults::m_status_bits;

FAULTS_BIT_TD Faults::m_latched[FAULTS_TOTAL_BIT_WORDS];
FAULTS_BIT_TD Faults::m_active[FAULTS_TOTAL_BIT_WORDS];
BF_Lib::Bit_List* Faults::m_set_que = reinterpret_cast<BF_Lib::Bit_List*>( nullptr );
BF_Lib::Bit_List* Faults::m_clr_que = reinterpret_cast<BF_Lib::Bit_List*>( nullptr );

FAULTS_ID_TD Faults::m_sorted_que[FAULTS_SORTED_QUEUE_LEN];
FAULTS_ID_TD Faults::m_event_order_que[FAULTS_EVENT_ORDER_QUEUE_LEN];
FAULTS_IND_TD Faults::m_nv_sorted_que[FAULTS_SORTED_QUEUE_LEN];
FAULTS_IND_TD Faults::m_nv_event_order_que[FAULTS_EVENT_ORDER_QUEUE_LEN];

uint8_t Faults::m_reset_request;

uint8_t Faults::m_latched_fault_ctr;
uint8_t Faults::m_fault_ctr;
uint8_t Faults::m_warn_ctr;

uint8_t Faults::m_initialization_fault = static_cast<uint8_t>( FAULTS_LIST_TOTAL_INDS );
BF_Lib::Linked_List* Faults::m_cback_list = reinterpret_cast<BF_Lib::Linked_List*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Faults::Faults( DCI_ID_TD sorted_que_dcid, DCI_ID_TD event_order_dcid,
				DCI_ID_TD status_bits_dcid )
{
	uint_fast8_t i;

	Faults::m_set_que = new BF_Lib::Bit_List( FAULTS_LIST_TOTAL_INDS );

	Faults::m_clr_que = new BF_Lib::Bit_List( FAULTS_LIST_TOTAL_INDS );

	for ( i = 0U; i < FAULTS_TOTAL_BIT_WORDS; i++ )
	{
		m_latched[i] = 0U;
		m_active[i] = 0U;
	}

	m_latched_fault_ctr = 0U;
	m_fault_ctr = 0U;
	m_warn_ctr = 0U;

	m_sorted_que_owner = new DCI_Owner( sorted_que_dcid );

	m_event_que_owner = new DCI_Owner( event_order_dcid );

	m_status_bits = new DCI_Owner( status_bits_dcid );

	Get_Faults_From_NV();

	// coverity[leaked_storage]
	new BF::System_Reset( &Reset_Handler,
						  reinterpret_cast<BF::System_Reset::cback_param_t>( this ) );

	// coverity[leaked_storage]
	new CR_Tasker( &System_Handler, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "Fault Handler" );

	if ( m_initialization_fault != FAULTS_LIST_TOTAL_INDS )
	// Then we had an initialization fault.
	{
		Set( FAULT_INITIALIZATION_ERROR_FLT );
		Set( m_initialization_fault );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Set( FAULTS_IND_TD fault_id )
{
	if ( m_set_que == NULL )
	{
		m_initialization_fault = fault_id;
	}
	else
	{
		m_set_que->Set( fault_id );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Clr( FAULTS_IND_TD fault_id )
{
	if ( m_clr_que != NULL )
	{
		m_clr_que->Set( fault_id );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Faults::Test( FAULTS_IND_TD fault_id )
{
	return ( FAULTS_Test_Bit( m_latched, fault_id ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Reset( void )
{
	BF_Lib::Bit::Set( m_reset_request, FAULTS_RESET_FAULT_REQ_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Flush_Lists( void )
{
	BF_Lib::Bit::Set( m_reset_request, FAULTS_RESET_FLUSH_REQ_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Faults::Faulted( void )
{
	return ( m_latched_fault_ctr != 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Faults::Warned( void )
{
	return ( m_warn_ctr != 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::System_Handler( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	static POST_EVENT_STATE_MACHINE post_event_state = POST_EVENT_IDLE;

	BF_Lib::Bit_List::bit_list_bit_td working_bit;
	uint8_t reset_req_shadow;

	Push_TGINT()
	;
	reset_req_shadow = m_reset_request;
	m_reset_request = 0U;
	Pop_TGINT();

	if ( reset_req_shadow != 0U )
	{
		if ( BF_Lib::Bit::Test( reset_req_shadow, FAULTS_RESET_FAULT_REQ_BIT ) )
		{
			m_latched_fault_ctr = m_fault_ctr;
			for ( uint_fast8_t i = 0U; i < FAULTS_TOTAL_BIT_WORDS; i++ )
			{
				if ( m_latched[i] != m_active[i] )
				{
					m_latched[i] = m_active[i];
					post_event_state = INITIATE_STATE_MACHINE;
					// only update NV if something has actually changed.
				}
			}
		}
		if ( BF_Lib::Bit::Test( reset_req_shadow, FAULTS_RESET_FLUSH_REQ_BIT ) )
		{
			Process_Flush_Lists();
			post_event_state = INITIATE_STATE_MACHINE;
		}
	}

	while ( !m_set_que->Is_Empty() )
	{
		working_bit = m_set_que->Get_Next();
		if ( !FAULTS_Test_Bit( m_active, ( static_cast<FAULTS_IND_TD>( working_bit ) ) ) )
		{
			Add_New_Fault( static_cast<FAULTS_IND_TD>( working_bit ) );
			post_event_state = INITIATE_STATE_MACHINE;
		}
	}
	while ( !m_clr_que->Is_Empty() )
	{
		working_bit = m_clr_que->Get_Next();
		if ( FAULTS_Test_Bit( m_active, ( static_cast<FAULTS_IND_TD>( working_bit ) ) ) )
		{
			Remove_Fault( static_cast<uint8_t>( working_bit ) );
			post_event_state = INITIATE_STATE_MACHINE;
		}
	}

	switch ( post_event_state )
	{
		case INITIATE_STATE_MACHINE:
			post_event_state = UPDATE_FAULT_NV_DATA;
			break;

		case UPDATE_FAULT_NV_DATA:
			Store_Lists();
			post_event_state = CALL_STATE_CHANGE_MONITORS;
			break;

		case CALL_STATE_CHANGE_MONITORS:
			FAULTS_STATE_CHANGE_CBACK_ST* walker;
			if ( m_cback_list != NULL )
			{
				m_cback_list->Restart();
				walker = reinterpret_cast<FAULTS_STATE_CHANGE_CBACK_ST*>( m_cback_list
																		  ->Get_And_Inc() );
				while ( walker != NULL )
				{
					// This line is commneted and following line is added to fix conformance
					// error - Shekhar
					// reinterpret_cast< STATE_CHANGE_CBACK_PARAM >( walker->param );
					// Call back function is called now.
					walker->cback( reinterpret_cast<STATE_CHANGE_CBACK_PARAM>( walker->param ) );
					walker = reinterpret_cast<FAULTS_STATE_CHANGE_CBACK_ST*>( m_cback_list->Get_And_Inc() );
				}
			}
			post_event_state = POST_EVENT_IDLE;
			break;

		case POST_EVENT_IDLE:
			break;

		default:
			post_event_state = POST_EVENT_IDLE;
			break;
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 0-1-11 There shall be no unused parameters (named or unnamed) in
	 * non-virtual functions.
	 * @n Justification: This is an exception as documented in MISRA C++ 2008.
	 * Symbols "cr_task and "param" are marked as possible later use.
	 */
	/*lint -e{715} # MISRA Deviation */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Attach_State_Change_Cback( STATE_CHANGE_CBACK* cback, STATE_CHANGE_CBACK_PARAM param )
{
	FAULTS_STATE_CHANGE_CBACK_ST* new_block;

	if ( m_cback_list == NULL )
	{
		m_cback_list = new BF_Lib::Linked_List();

	}

	new_block = reinterpret_cast<FAULTS_STATE_CHANGE_CBACK_ST*>( Ram::Allocate( sizeof (
																					FAULTS_STATE_CHANGE_CBACK_ST ) ) );

	new_block->cback = cback;
	new_block->param = param;

	// coverity[leaked_storage]
	m_cback_list->Add( reinterpret_cast<BF_Lib::Linked_List::LL_PAYLOAD>( new_block ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Faults::FAULTS_Test_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault )
{

	return ( ( bit_array[FAULTS_Word_Select( fault )] &
			   ( static_cast<uint8_t>( FAULTS_Bit_Mask( fault ) ) ) ) != 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::FAULTS_Clr_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault )
{
	bit_array[FAULTS_Word_Select( fault )] &=
		~( static_cast<uint8_t>( FAULTS_Bit_Mask( fault ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::FAULTS_Set_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault )
{
	bit_array[FAULTS_Word_Select( fault )] |= FAULTS_Bit_Mask( fault );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Add_New_Fault( FAULTS_IND_TD fault )
{
	Faults::FAULTS_Set_Bit( m_latched, fault );
	Faults::FAULTS_Set_Bit( m_active, fault );

	Add_New_ID_To_Sorted_Que( fault );
	Add_New_ID_To_Event_Order_Que( fault );

	if ( faults_list[fault].self_clear == false )
	{
		m_fault_ctr++;
		m_latched_fault_ctr++;
	}
	else
	{
		m_warn_ctr++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Remove_Fault( FAULTS_IND_TD fault )
{
	FAULTS_Clr_Bit( m_active, fault );
	if ( faults_list[fault].self_clear )
	{
		FAULTS_Clr_Bit( m_latched, fault );		// This is a warning and auto clears.
		m_warn_ctr--;
	}
	else
	{
		m_fault_ctr--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Add_New_ID_To_Sorted_Que( FAULTS_IND_TD new_fault )
{
	uint_fast8_t i;

	// Add to fault queue.  This will find the duplicate and move it to the top.
	i = 0U;
	while ( ( i < ( FAULTS_SORTED_QUEUE_LEN - 1U ) ) &&
			( m_nv_sorted_que[i] != new_fault ) )
	{
		i++;
	}

	for (; i > 0U; i-- )
	{
		m_nv_sorted_que[i] = m_nv_sorted_que[i - 1U];
	}

	m_nv_sorted_que[0] = new_fault;

	for ( i = 0U; i < FAULTS_SORTED_QUEUE_LEN; i++ )
	{
		m_sorted_que[i] = faults_list[m_nv_sorted_que[i]].fault_id;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Add_New_ID_To_Event_Order_Que( FAULTS_IND_TD new_fault )
{
	uint_fast8_t i;

	// Add to fault list.
	for ( i = ( FAULTS_EVENT_ORDER_QUEUE_LEN - 1U ); i > 0U; i-- )
	{
		m_nv_event_order_que[i] = m_nv_event_order_que[i - 1U];
		m_event_order_que[i] = m_event_order_que[i - 1U];
	}

	m_nv_event_order_que[0] = new_fault;
	m_event_order_que[0] = faults_list[new_fault].fault_id;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Store_Lists( void )
{
	NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( m_nv_sorted_que ), FAULTS_SORTED_QUE_NVADD,
					   FAULTS_SORTED_QUEUE_LEN );
	NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( m_nv_event_order_que ),
					   FAULTS_EVENT_ORDERED_QUE_NVADD,
					   FAULTS_EVENT_ORDER_QUEUE_LEN );
	NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( m_latched ), FAULTS_LATCHED_BITS_NVADD,
					   FAULTS_LATCHED_BITS_NV_LEN );

	Update_Status_Bits();

	m_sorted_que_owner->Check_In( m_sorted_que );
	m_event_que_owner->Check_In( m_event_order_que );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Get_Faults_From_NV( void )
{
	bool status = false;

	status = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( m_nv_sorted_que ), FAULTS_SORTED_QUE_NVADD,
							   FAULTS_SORTED_QUEUE_LEN );
	if ( false == status )
	{
		FAULTS_DEBUG_PRINT( DBG_MSG_ERROR, "Error in reading from nv sorted queue at address: %X",
							FAULTS_SORTED_QUE_NVADD, "of length: %u", FAULTS_SORTED_QUEUE_LEN );
	}

	status = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( m_nv_event_order_que ),
							   FAULTS_EVENT_ORDERED_QUE_NVADD, FAULTS_EVENT_ORDER_QUEUE_LEN );
	if ( false == status )
	{
		FAULTS_DEBUG_PRINT( DBG_MSG_ERROR, "Error in reading from nv event order queue at address: %X",
							FAULTS_EVENT_ORDERED_QUE_NVADD, "of length: %u", FAULTS_EVENT_ORDER_QUEUE_LEN );
	}

	status = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( m_latched ), FAULTS_LATCHED_BITS_NVADD,
							   FAULTS_LATCHED_BITS_NV_LEN );
	if ( false == status )
	{
		FAULTS_DEBUG_PRINT( DBG_MSG_ERROR, "Error in reading from latched buffer at address: %X",
							FAULTS_LATCHED_BITS_NVADD, "of length: %u", FAULTS_LATCHED_BITS_NV_LEN );
	}

	m_latched_fault_ctr = 0U;
	for ( uint_fast8_t i = 0U; i < FAULTS_LIST_TOTAL_INDS; i++ )
	{
		if ( FAULTS_Test_Bit( m_latched, ( static_cast<FAULTS_IND_TD>( i ) ) ) )
		{
			if ( faults_list[i].self_clear == false )
			{
				m_latched_fault_ctr++;
			}
			else
			{
				FAULTS_Clr_Bit( m_latched, static_cast<FAULTS_IND_TD>( i ) );
			}
		}
	}

	for ( uint_fast8_t i = 0U; i < FAULTS_SORTED_QUEUE_LEN; i++ )
	{
		m_sorted_que[i] = faults_list[m_nv_sorted_que[i]].fault_id;
	}
	for ( uint_fast8_t i = 0U; i < FAULTS_EVENT_ORDER_QUEUE_LEN; i++ )
	{
		m_event_order_que[i] = faults_list[m_nv_event_order_que[i]].fault_id;
	}

	Update_Status_Bits();

	m_sorted_que_owner->Check_In( m_sorted_que );
	m_event_que_owner->Check_In( m_event_order_que );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Process_Flush_Lists( void )
{
	uint_fast8_t i;

	for ( i = 0U; i < FAULTS_SORTED_QUEUE_LEN; i++ )
	{
		m_sorted_que[i] = 0U;
		m_nv_sorted_que[i] = 0U;
	}

	for ( i = 0U; i < FAULTS_EVENT_ORDER_QUEUE_LEN; i++ )
	{
		m_event_order_que[i] = 0U;
		m_nv_event_order_que[i] = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Faults::Update_Status_Bits( void )
{
	uint8_t status_bits = 0U;

	if ( Faulted() )
	{
		BF_Lib::Bit::Set( status_bits, FAULTS_STATUS_FAULTED_BIT );
	}
	if ( Warned() )
	{
		BF_Lib::Bit::Set( status_bits, FAULTS_STATUS_WARNED_BIT );
	}

	m_status_bits->Check_In( status_bits );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Faults::Reset_Handler( BF::System_Reset::cback_param_t param,
							BF::System_Reset::reset_select_t reset_req )
{
	uint_fast8_t i;
	bool reset_done = true;

	for ( i = 0U; i < FAULTS_SORTED_QUEUE_LEN; i++ )
	{
		if ( m_nv_sorted_que[i] != 0U )
		{
			reset_done = false;
		}
	}

	if ( reset_done == true )
	{
		for ( i = 0U; i < FAULTS_EVENT_ORDER_QUEUE_LEN; i++ )
		{
			if ( m_nv_event_order_que[i] != 0U )
			{
				reset_done = false;
			}
		}
	}

	if ( reset_done == false )
	{
		Flush_Lists();
	}

	return ( reset_done );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 0-1-11 There shall be no unused parameters (named or unnamed) in
	 * non-virtual functions.
	 * @n Justification: This is an exception as documented in MISRA C++ 2008.
	 * Symbol "param" is marked as possible later use.
	 */
	/*lint -e{715} # MISRA Deviation */
}

}
