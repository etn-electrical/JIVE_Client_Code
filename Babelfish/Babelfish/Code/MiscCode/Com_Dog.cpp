/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Com_Dog.h"
#include "Timers_Lib.h"
#include "Ram.h"
#include "Com_Dog_Data.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

namespace BF_Misc
{

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
Com_Dog* Com_Dog::m_dog_list = reinterpret_cast<Com_Dog*>( nullptr );
BF_Lib::Linked_List* Com_Dog::m_cb_list =
	reinterpret_cast<BF_Lib::Linked_List*>( nullptr );
DCI_SOURCE_ID_TD Com_Dog::m_com_dog_source_id = DCI_UNKNOWN_SOURCE_ID;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Com_Dog::Com_Dog( DCI_Owner* comm_loss_timeout_owner, DCI_SOURCE_ID_TD source_id ) :
	m_timeout_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_source_id( 0U ),
	m_timer_value( 0U ),
	m_timeout_value( 0U ),
	m_state( COM_DOG_DISABLED_STATE ),
	m_next_dog( reinterpret_cast<Com_Dog*>( nullptr ) )
{
	Com_Dog* walker;

	m_timeout_owner = comm_loss_timeout_owner;

	m_timeout_owner->Check_Out( m_timeout_value );

	m_source_id = source_id;

	m_next_dog = reinterpret_cast<Com_Dog*>( nullptr );

	// If we have not entered this constructor yet then we need to create the time task.
	if ( m_cb_list == nullptr )
	{
		Initialize_Dog_List();
		m_com_dog_source_id = DCI_SOURCE_IDS_Get();
	}

	walker = m_dog_list;
	if ( m_dog_list == nullptr )
	{
		m_dog_list = this;
	}
	else
	{
		while ( walker->m_next_dog != nullptr )
		{
			walker = walker->m_next_dog;
		}
		walker->m_next_dog = this;
	}

	m_state = COM_DOG_DISABLED_STATE;

	m_timer_value = 0U;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Com_Dog::Com_Dog( void ) :
	m_timeout_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_source_id( DCI_UNKNOWN_SOURCE_ID ),
	m_timer_value( 0U ),
	m_timeout_value( 0U ),
	m_state( COM_DOG_DISABLED_STATE ),
	m_next_dog( reinterpret_cast<Com_Dog*>( nullptr ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Feed_Dog( void )
{
	m_timer_value = 0U;
	m_timeout_owner->Check_Out( m_timeout_value );
	m_state = COM_DOG_ENABLED_STATE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Initialize_Dog_List( void )
{
	BF_Lib::Timers* tick_timer;

	m_cb_list = new BF_Lib::Linked_List();

	// Setup the timer tick.  This is static stuff.
	// Must come before the list addition going on below.
	tick_timer = new BF_Lib::Timers( &Timeout_Checker,
									 reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( nullptr ),
									 BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
									 "Com Dog Check" );

	tick_timer->Start( COM_DOG_TICK_INTERVAL, true );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Goto_Idle( void )
{
	Trigger_Comm_State( COM_DOG_COMM_IDLE, m_source_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Timeout_Checker( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	Com_Dog* walker;

	walker = m_dog_list;

	while ( walker != nullptr )
	{
		if ( ( walker->m_state != COM_DOG_DISABLED_STATE ) &&
			 ( walker->m_timeout_value > COM_DOG_DISABLE_TIMEOUT ) )
		{
			if ( walker->m_timer_value >= COM_DOG_MAX_VAL_CHECK )
			{
				walker->m_timer_value = COM_DOG_MAX_VAL;
			}
			else
			{
				walker->m_timer_value += COM_DOG_TICK_INTERVAL;
			}
			if ( walker->m_timer_value >= walker->m_timeout_value )
			{
				walker->m_state = COM_DOG_DISABLED_STATE;
				Handle_Comm_State_Change( COM_DOG_COMM_LOSS,
										  walker->m_source_id );
			}
		}
		walker = walker->m_next_dog;
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 0-1-11 There shall be no unused parameters (named or unnamed) in
	 * non-virtual functions.
	 * @n Justification: This is an exception as documented in MISRA C++ 2008.
	 * An unnamed parameter in the definition of a function that is used as a callback does not
	 * violate this rule.
	 */
	/*lint -e{715} # MISRA Deviation */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Attach_State_Change_Callback( COM_DOG_TIMEOUT_CB callback,
											COM_DOG_TIMEOUT_CB_PARAM param )
{
	COM_DOG_CB_STRUCT* cb_struct;

	cb_struct = reinterpret_cast<COM_DOG_CB_STRUCT*>( Ram::Allocate(
														  sizeof( COM_DOG_CB_STRUCT ), true,
														  0U ) );

	cb_struct->cb = callback;
	cb_struct->cb_param = param;

	if ( m_cb_list == nullptr )
	{
		Initialize_Dog_List();
	}
	// TODO: Possible misuse of pointer - Mark to look into use of Payload
	m_cb_list->Add( reinterpret_cast<BF_Lib::Linked_List::LL_PAYLOAD>( cb_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Trigger_Comm_State( COM_DOG_TRIG_TD trigger,
								  DCI_SOURCE_ID_TD source_responsible )
{
	Com_Dog* walker;

	if ( source_responsible != DCI_UNKNOWN_SOURCE_ID )
	{
		if ( m_dog_list == nullptr )
		{
			Initialize_Dog_List();
		}

		walker = m_dog_list;
		while ( ( walker != nullptr ) && ( walker->m_source_id != source_responsible ) )
		{
			walker = walker->m_next_dog;
		}
		if ( ( walker != nullptr ) && ( walker->m_source_id == source_responsible ) )
		{
			walker->m_state = COM_DOG_DISABLED_STATE;
		}

	}

	Handle_Comm_State_Change( trigger, source_responsible );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Com_Dog::Handle_Comm_State_Change( COM_DOG_TRIG_TD trigger,
										DCI_SOURCE_ID_TD source_responsible )
{
	COM_DOG_CB_STRUCT* cb_walker;

	if ( m_cb_list != nullptr )
	{
		m_cb_list->Restart();
		// TODO: Possible misuse of pointer - Mark to look into use of Payload
		cb_walker = reinterpret_cast<COM_DOG_CB_STRUCT*>( m_cb_list->Get_And_Inc() );
		while ( cb_walker != nullptr )
		{
			( *cb_walker->cb )( cb_walker->cb_param, source_responsible, trigger );
			// TODO: Possible misuse of pointer - Mark to look into use of Payload
			cb_walker = reinterpret_cast<COM_DOG_CB_STRUCT*>( m_cb_list->Get_And_Inc() );
		}
	}
}

}
