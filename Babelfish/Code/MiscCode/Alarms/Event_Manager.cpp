/**
 *****************************************************************************************
 * @file
 * @details event Manager file help to get the execute event operation.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Event_Manager.h"
#include "OS_Task_Config.h"

namespace BF_Event
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Event_Manager::Event_Manager( rule_id_t rule_count,  event_interface_cb_t new_event_cb )
	: m_rule_count( rule_count ),
	m_new_event_cb( new_event_cb )
{
	m_set_q = new Bit_List( m_rule_count );
	m_clr_q = new Bit_List( m_rule_count );
	m_enable_q = new Bit_List( m_rule_count );
	m_disable_q = new Bit_List( m_rule_count );
	m_reset_q = new Bit_List( m_rule_count );
	m_ack_q = new Bit_List( m_rule_count );
	m_islatched_q = new Bit_List( m_rule_count );
	m_active = new Bit_List( m_rule_count );
	m_latched = new Bit_List( m_rule_count );
	m_unacked = new Bit_List( m_rule_count );
	m_enabled = new Bit_List( m_rule_count );
	m_event_tasker = nullptr;

	m_event_tasker = OS_Tasker::Create_Task( &Event_Handler_Static, EVENT_HANDLER_STACKSIZE,
											 EVENT_HANDLER_THREAD_PRIO,
											 reinterpret_cast<uint8_t const*>( EVENT_HANDLER_THREAD_NAME ),
											 reinterpret_cast<OS_TASK_PARAM>( this ) );
	OS_Tasker::Suspend( m_event_tasker );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Event_Manager::~Event_Manager( void )
{
	delete m_enabled;
	delete m_islatched_q;
	delete m_set_q;
	delete m_clr_q;
	delete m_enable_q;
	delete m_disable_q;
	delete m_reset_q;
	delete m_ack_q;
	delete m_active;
	delete m_latched;
	delete m_unacked;
	delete m_event_tasker;

}

/* *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************/

void Event_Manager::Set_Active( rule_id_t rule_index )
{
	m_set_q->Set( rule_index );	// Set rule/event bit in the bitlist
	Wakeup_Run();
}

/*****************************************************************************************
* See header file for function definition.
*****************************************************************************************/

void Event_Manager::Set_Inactive( rule_id_t rule_index )
{
	m_clr_q->Set( rule_index );	// clear rule/event bit in the bitlist
	Wakeup_Run();
}

/*****************************************************************************************
* See header file for function definition.
*****************************************************************************************/

void Event_Manager::Enable( rule_id_t rule_index, bool is_latched )
{
	m_enable_q->Set( rule_index );	// Enable Rule/event bit in the bitlist
	if ( is_latched == true )		// Check the rule/event configured as latched or not
	{
		m_islatched_q->Set( rule_index );	// set latched bit in the bitlist w. r t rule index
	}
	Wakeup_Run();
}

/*****************************************************************************************
 * See header file for function definition.
 ******************************************************************************************/

void Event_Manager::Disable( rule_id_t rule_index )
{
	m_disable_q->Set( rule_index );	// Disable Rule bit in the bitlist
	Wakeup_Run();
}

/******************************************************************************************
 * See header file for function definition.
 *****************************************************************************************/

status_bitmask_t Event_Manager::Status( rule_id_t rule_index )
{
	status_bitmask_t status = 0U;

	if ( !m_active->Test( rule_index ) )	// Check event is active/not
	{
		status |= ACTIVE_BIT;
	}
	if ( !m_latched->Test( rule_index ) )	// Check event latched/ not
	{
		status |= LATCHED_BIT;
	}
	if ( !m_unacked->Test( rule_index ) )	// Check event acked/ not
	{
		status |= UNACKED_BIT;
	}
	if ( !m_enabled->Test( rule_index ) )	// Check event enable/ not
	{
		status |= ENABLED_BIT;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

event_op_status_t Event_Manager::Reset( rule_id_t rule_index )
{
	event_op_status_t op_status = NO_EVENT_TO_RESET;

	/* Check the reset event request for all events or not */
	if ( rule_index == MAX_RULES )
	{
		if ( m_enabled->Active_Bits() > 0U )
		{
			/* Check event existing or not */
			m_reset_q->Set();
			op_status = EVENT_NO_ERROR;
			Wakeup_Run();

		}
		else
		{
			op_status = EVENT_RULE_DISABLE;
		}
	}
	else
	{
		/* condition for single event reset request */
		if ( true == m_enabled->Test( rule_index ) )
		{
			/* If rule / event configured as latched */
			if ( true == m_islatched_q->Test( rule_index ) )
			{
				/* Before closing the event it will check the event condition is cleared or not */
				if ( !m_active->Test( rule_index ) )
				{
					m_reset_q->Set( rule_index );			// reset Rule/event bit in the bitlist
					op_status = EVENT_NO_ERROR;
					Wakeup_Run();
				}
				else
				{
					op_status = EVENT_NOT_CLEARED;
				}
			}
			else
			{
				if ( !m_active->Test( rule_index ) )
				{
					/* Unlatched configured rule are directly reset the event */
					m_reset_q->Set( rule_index );		// reset Rule/event bit in the bitlist
					op_status = EVENT_NO_ERROR;
					Wakeup_Run();
				}
			}
		}
		else
		{
			op_status = EVENT_RULE_DISABLE;
		}

	}
	return ( op_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
event_op_status_t Event_Manager::Ack( rule_id_t rule_index )
{
	event_op_status_t op_status = EVENT_ALREADY_ACK;

	/* check the ack level is similar for the rule or not */
	if ( true == m_unacked->Test( rule_index ) )
	{
		m_ack_q->Set( rule_index );				// Acked Rule/event bit in the bitlist
		op_status = EVENT_NO_ERROR;
		Wakeup_Run();
	}
	return ( op_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Event_Manager::Event_Handler_Task( void )
{
	// coverity[no_escape]
	while ( 1U )
	{
		new_event_t new_event = { 0U };
		uint8_t rule_index = 0U;
		status_bitmask_t event_status_change = 0U;
		bool raise_event = false;
		while ( rule_index < m_rule_count )
		{
			if ( true == m_enable_q->Test( rule_index ) )
			{
				m_enabled->Set( rule_index );	// Enable Rule/event bit in the bitlist
			}

			if ( true == m_disable_q->Test( rule_index ) )
			{
				/* clear the all bitlist with respect to the rule index */
				m_active->Dump( rule_index );
				m_latched->Dump( rule_index );
				m_unacked->Dump( rule_index );
				m_enabled->Dump( rule_index );
				m_enable_q->Dump( rule_index );
				m_disable_q->Dump( rule_index );
				m_islatched_q->Dump( rule_index );
				event_status_change = 0U;
			}
			else if ( true == m_enabled->Test( rule_index ) )
			{
				event_status_change |= ENABLED_BIT;
				if ( true == m_active->Test( rule_index ) )
				{
					event_status_change |= ACTIVE_BIT;
				}
				if ( true == m_latched->Test( rule_index ) )
				{
					event_status_change |= LATCHED_BIT;
				}
				if ( true == m_unacked->Test( rule_index ) )
				{
					event_status_change |= UNACKED_BIT;
				}

				if ( true == m_set_q->Test( rule_index ) )
				{
					if ( true == m_islatched_q->Test( rule_index ) )
					{
						/* Rule configured as Latched then it is maintaining the m_latched bit list */
						if ( !m_latched->Test( rule_index ) )
						{
							m_latched->Set( rule_index );
							event_status_change |= LATCHED_BIT;
							m_active->Set( rule_index );
							event_status_change |= ACTIVE_BIT;
							m_unacked->Set( rule_index );
							event_status_change |= UNACKED_BIT;
							raise_event = true;
						}
					}
					else
					{
						/* Rule configured as un-Latched then it is maintaining the m_active bit list */
						if ( !m_active->Test( rule_index ) )
						{
							event_status_change &= CLEAR_LATCHED_BIT;
							m_active->Set( rule_index );
							event_status_change |= ACTIVE_BIT;
							m_unacked->Set( rule_index );
							event_status_change |= UNACKED_BIT;
							raise_event = true;
						}
					}
					m_set_q->Dump( rule_index );
				}

				if ( true == m_clr_q->Test( rule_index ) )
				{
					/* Check the event is Active ot not */
					if ( m_active->Test( rule_index ) )
					{
						m_active->Dump( rule_index );
						event_status_change &= CLEAR_ACTIVE_BIT;
						m_clr_q->Dump( rule_index );
						if ( !m_islatched_q->Test( rule_index ) )
						{
							m_unacked->Dump( rule_index );
							event_status_change &= CLEAR_UNACKED_BIT;
							event_status_change &= CLEAR_LATCHED_BIT;
						}
						raise_event = true;
					}
				}
				/* reset event logic for all events  */
				if ( m_reset_q->Active_Bits() == m_rule_count )
				{
					if ( m_islatched_q->Test( rule_index ) )
					{
						if ( ( m_latched->Test( rule_index ) ) && ( !m_active->Test( rule_index ) ) )
						{
							m_latched->Dump( rule_index );
							event_status_change &= CLEAR_LATCHED_BIT;
							event_status_change &= CLEAR_UNACKED_BIT;
							event_status_change &= CLEAR_ACTIVE_BIT;
							raise_event = true;
						}
					}
					else
					{
						if ( !m_active->Test( rule_index ) )
						{
							event_status_change &= CLEAR_LATCHED_BIT;
							m_active->Dump( rule_index );
							event_status_change &= CLEAR_ACTIVE_BIT;
							m_unacked->Dump( rule_index );
							event_status_change &= CLEAR_UNACKED_BIT;
							raise_event = true;
						}
					}
				}
				else
				{
					/* reset logic for single event */
					if ( true == m_reset_q->Test( rule_index ) )
					{
						if ( m_islatched_q->Test( rule_index ) )
						{
							if ( ( m_latched->Test( rule_index ) ) && ( !m_active->Test( rule_index ) ) )
							{
								m_latched->Dump( rule_index );
								event_status_change &= CLEAR_LATCHED_BIT;
								event_status_change &= CLEAR_UNACKED_BIT;
								event_status_change &= CLEAR_ACTIVE_BIT;
								raise_event = true;
							}
						}
						else
						{
							if ( !m_active->Test( rule_index ) )
							{
								event_status_change &= CLEAR_LATCHED_BIT;
								m_active->Dump( rule_index );
								event_status_change &= CLEAR_ACTIVE_BIT;
								m_unacked->Dump( rule_index );
								event_status_change &= CLEAR_UNACKED_BIT;
								raise_event = true;
							}
						}
						m_reset_q->Dump( rule_index );
					}
				}
				/* Ack the event logic  */
				if ( true == m_ack_q->Test( rule_index ) )
				{
					event_status_change &= CLEAR_UNACKED_BIT;
					m_ack_q->Dump( rule_index );
					m_unacked->Dump( rule_index );
					raise_event = true;
				}
				if ( raise_event == true )
				{
					new_event.rule_id = rule_index;
					new_event.event_status = event_status_change;
					if ( nullptr != m_new_event_cb )
					{
						m_new_event_cb( new_event );
					}
					raise_event = false;
				}
			}
			else
			{}
			rule_index++;
		}
		if ( m_reset_q->Active_Bits() == m_rule_count )
		{
			m_reset_q->Dump();
		}
		OS_Tasker::Suspend( m_event_tasker );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Event_Manager::Wakeup_Run( void )
{
	OS_Tasker::Resume( m_event_tasker );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
active_event_t Event_Manager::Get_Active_Event_Count( void )
{
	active_event_t active_event_cnt = 0U;
	uint8_t rule_index = 0U;

	while ( rule_index < m_rule_count )
	{
		if ( true == m_enabled->Test( rule_index ) )
		{
			if ( true == m_islatched_q->Test( rule_index ) )
			{
				if ( true == m_latched->Test( rule_index ) )
				{
					active_event_cnt++;
				}
			}
			else
			{
				if ( true == m_active->Test( rule_index ) )
				{
					active_event_cnt++;
				}
			}
		}
		rule_index++;
	}
	return ( active_event_cnt );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Event_Manager::Clear_Events( void )
{
	m_set_q->Dump();
	m_clr_q->Dump();
	m_enable_q->Dump();
	m_disable_q->Dump();
	m_reset_q->Dump();
	m_ack_q->Dump();
	m_active->Dump();
	m_latched->Dump();
	m_unacked->Dump();
}

}
