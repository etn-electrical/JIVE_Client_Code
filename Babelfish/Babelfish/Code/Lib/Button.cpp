/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Button.h"
#include "CR_Tasker.h"

namespace BF_Lib
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Button::Button( Input* input, DCI_Owner* button_owner,
				DCI_Owner* changes_owner, uint32_t debounce_time ) :
	m_input( input ),
	m_button_owner( button_owner ),
	m_changes_owner( changes_owner ),
	m_debounce_time( debounce_time ),
	m_change_time( 0U ),
	m_state( true ),
	m_debounced_state( true )
{
	m_input = input;
	m_button_owner = button_owner;
	m_debounce_time = debounce_time;
	m_changes_owner = changes_owner;

	new CR_Tasker( &Update_Button_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "Button monitor task" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Button::Get_State( void ) const
{
	return ( m_debounced_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Button::Update_Button( void )
{
	bool current_state = m_input->Get();

	if ( current_state != m_state )
	{
		m_state = current_state;
		m_change_time = Timers::Get_Time();
	}

	if ( m_state != m_debounced_state )
	{
		if ( Timers::Expired( m_change_time, m_debounce_time ) )
		{
			uint8_t changes_count;

			m_debounced_state = m_state;
			m_button_owner->Check_In( m_debounced_state );
			if ( m_changes_owner != nullptr )
			{
				m_changes_owner->Check_Out( &changes_count );
				changes_count++;
				m_changes_owner->Check_In( changes_count );
			}
		}
	}
}

}	/* end namespace BF_Lib for this module */
