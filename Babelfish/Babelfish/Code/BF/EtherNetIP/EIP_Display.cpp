/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "EIP_Display.h"
#include "Ram.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define EIP_DISPLAY_TICK_TIME           10U
#define EIP_DISPLAY_TEST_TICK_TIME      250U		// in milliseconds.
enum
{
	EIP_DISP_OFF,
	EIP_DISP_GREEN,
	EIP_DISP_FLASH_GREEN,
	EIP_DISP_RED,
	EIP_DISP_FLASH_RED,
	EIP_DISP_SLOW_FLASH_GREEN_RED,
	EIP_DISP_FAST_FLASH_GREEN_RED,
	EIP_DISPLAY_MAX_STATES
};


// MS = Module Status
// NS = Network Status
// CS = Combined Status
enum
{
	EIP_DISP_OPERATIONAL_MS = EIP_DISP_GREEN,					// GREEN
	EIP_DISP_STANDBY_MS = EIP_DISP_FLASH_GREEN,				// GREEN FLASHING
	EIP_DISP_UNRECOVERABLE_FLT_MS = EIP_DISP_RED,				// RED
	EIP_DISP_MINOR_FAULT_MS = EIP_DISP_FLASH_RED,				// FLASHING_RED
	EIP_DISP_SELF_TEST_MS = EIP_DISP_SLOW_FLASH_GREEN_RED,	// RED-GREEN FLASHING
	EIP_DISP_MAX_STATES_MS = EIP_DISPLAY_MAX_STATES
};


enum
{
	EIP_DISP_OFFLINE_NS = EIP_DISP_OFF,							// OFF
	EIP_DISP_ONLINE_CONNECTED_NS = EIP_DISP_GREEN,				// GREEN
	EIP_DISP_ONLINE_NS = EIP_DISP_FLASH_GREEN,					// GREEN FLASHING
	EIP_DISP_CRITICAL_LINK_FLT_NS = EIP_DISP_RED,					// RED
	EIP_DISP_CONNECTION_TIMEOUT_NS = EIP_DISP_FLASH_RED,			// RED FLASHING
	EIP_DISP_COMM_FAULTED_IDENTIFY_NS = EIP_DISP_FAST_FLASH_GREEN_RED,	// RED-GREEN FLASHING
	EIP_DISP_MAX_STATES_NS = EIP_DISPLAY_MAX_STATES
};


enum
{
	EIP_DISP_OFFLINE_CS = EIP_DISP_OFF,								// OFF
	EIP_DISP_OP_ONLINE_CONNECTED_CS = EIP_DISP_GREEN,					// GREEN
	EIP_DISP_OP_ONLINE_OR_NEEDS_COMMIS_CS = EIP_DISP_FLASH_GREEN,		// GREEN FLASHING
	EIP_DISP_CRITICAL_FLT_OR_LINK_FAIL_CS = EIP_DISP_RED,				// RED
	EIP_DISP_MINOR_FAULT_OR_TIMEOUT_CS = EIP_DISP_FLASH_RED,			// RED FLASHING
	EIP_DISP_COMM_FAULTED_IDENTIFY_CS = EIP_DISP_FAST_FLASH_GREEN_RED,	// RED-GREEN FLASHING
	EIP_DISP_MAX_STATES_CS = EIP_DISPLAY_MAX_STATES
};

#define EIP_DISPLAY_SLOW_FLASH_TIME     ( 500U / EIP_DISPLAY_TICK_TIME )
#define EIP_DISPLAY_FAST_FLASH_TIME     ( 250U / EIP_DISPLAY_TICK_TIME )
#define EIP_DISPLAY_FULL_TIME_ON_TIME       ( 1000U / EIP_DISPLAY_TICK_TIME )
#define EIP_DISPLAY_FULL_TIME_OFF_TIME      EIP_DISPLAY_FULL_TIME_ON_TIME

enum
{
	EIP_DISP_PWR_UP_TEST_MS_GREEN,
	EIP_DISP_PWR_UP_TEST_MS_RED,
	EIP_DISP_PWR_UP_TEST_NS_GREEN,
	EIP_DISP_PWR_UP_TEST_NS_RED,
	EIP_DISP_PWR_UP_TEST_NS_OFF,
	EIP_DISP_PWR_UP_TEST_CS_GREEN,
	EIP_DISP_PWR_UP_TEST_CS_RED,
	EIP_DISP_PWR_UP_TEST_CS_OFF,
	EIP_DISP_PWR_UP_TEST_END_STATE
};

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
EIP_Display::EIP_Display( BF_Misc::LED* grn_mod_led_ctrl, BF_Misc::LED* red_mod_led_ctrl,
						  BF_Misc::LED* grn_net_led_ctrl, BF_Misc::LED* red_net_led_ctrl ) :
	m_grn_mod_led_ctrl( reinterpret_cast<BF_Misc::LED*>( nullptr ) ),
	m_red_mod_led_ctrl( reinterpret_cast<BF_Misc::LED*>( nullptr ) ),
	m_grn_net_led_ctrl( reinterpret_cast<BF_Misc::LED*>( nullptr ) ),
	m_red_net_led_ctrl( reinterpret_cast<BF_Misc::LED*>( nullptr ) ),
	m_ms_state( 0U ),
	m_prev_ms_state( 0U ),
	m_ns_state( 0U ),
	m_prev_ns_state( 0U ),
	m_cs_state( 0U ),
	m_prev_cs_state( 0U ),
	m_ms_tick( 0U ),
	m_ns_tick( 0U ),
	m_ms_identify_ctr( 0U ),
	m_ns_identify_ctr( 0U ),
	m_led_test_state( 0U ),
	m_timer( reinterpret_cast<BF_Lib::Timers*>( nullptr ) )
{
	m_grn_mod_led_ctrl = grn_mod_led_ctrl;
	m_red_mod_led_ctrl = red_mod_led_ctrl;
	if ( grn_net_led_ctrl != NULL )
	{
		m_grn_net_led_ctrl = grn_net_led_ctrl;
		m_red_net_led_ctrl = red_net_led_ctrl;
	}
	else
	{
		m_grn_net_led_ctrl = grn_mod_led_ctrl;		// This is done to insure that they are not run
													// from NULL.
		m_red_net_led_ctrl = red_mod_led_ctrl;		// This is done to insure that they are not run
													// from NULL.
	}

	m_ms_state = static_cast<uint8_t>( EIP_DISP_OPERATIONAL_MS );
	m_prev_ms_state = m_ms_state;
	m_ns_state = static_cast<uint8_t>( EIP_DISP_ONLINE_CONNECTED_NS );
	m_prev_ns_state = m_ns_state;
	m_cs_state = static_cast<uint8_t>( EIP_DISP_OP_ONLINE_CONNECTED_CS );
	m_prev_cs_state = m_cs_state;

	m_timer = new BF_Lib::Timers( &LED_Test_Handler_Static, this,
								  BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "EIP Display" );

	Reset_Display();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::LED_Test( void )
{
	if ( m_led_test_state == static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE ) )
	{
		if ( m_grn_mod_led_ctrl != m_grn_net_led_ctrl )
		{
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_MS_GREEN );
		}
		else
		{
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_CS_GREEN );
		}

		m_timer->Attach_Callback( &LED_Test_Handler_Static, this );

		m_timer->Start( EIP_DISPLAY_TEST_TICK_TIME, TRUE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::Reset_Display( void )
{
	m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE );

	if ( m_grn_mod_led_ctrl == m_grn_net_led_ctrl )
	{
		m_timer->Attach_Callback( &Combined_Flash_Task_Static, this );
	}
	else
	{
		m_timer->Attach_Callback( &Flash_Task_Static, this );
	}

	m_timer->Start( EIP_DISPLAY_TICK_TIME, TRUE );
	m_ms_tick = 0U;
	m_ns_tick = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::Flash_Task( void )
{
	if ( m_ms_tick == 0U )
	{
		m_ms_tick = Update_Display( m_grn_mod_led_ctrl, m_red_mod_led_ctrl, m_ms_state,
									m_prev_ms_state );
	}
	m_ms_tick--;

	if ( m_ns_tick == 0U )
	{
		m_ns_tick = Update_Display( m_grn_net_led_ctrl, m_red_net_led_ctrl, m_ns_state,
									m_prev_ns_state );
	}
	m_ns_tick--;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::Combined_Flash_Task( void )
{
	if ( m_ms_tick == 0U )
	{
		m_ms_tick = Update_Display( m_grn_mod_led_ctrl, m_red_mod_led_ctrl,
									m_cs_state, m_prev_cs_state );
	}
	m_ms_tick--;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// Module Status
void EIP_Display::MS_Operational( void )
{
	if ( m_ms_state != static_cast<uint8_t>( EIP_DISP_OPERATIONAL_MS ) )
	{
		m_ms_state = static_cast<uint8_t>( EIP_DISP_OPERATIONAL_MS );
		m_prev_ms_state = m_ms_state;
		// m_cs_state = EIP_DISP_OP_ONLINE_CONNECTED_CS;
		m_ms_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::MS_Standby( void )
{
	if ( m_ms_state != static_cast<uint8_t>( EIP_DISP_STANDBY_MS ) )
	{
		m_ms_state = static_cast<uint8_t>( EIP_DISP_STANDBY_MS );
		m_prev_ms_state = m_ms_state;
		// m_cs_state = EIP_DISP_OP_ONLINE_CONNECTED_CS;
		m_ms_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::MS_Minor_Fault( void )
{
	if ( m_ms_state != static_cast<uint8_t>( EIP_DISP_MINOR_FAULT_MS ) )
	{
		m_ms_state = static_cast<uint8_t>( EIP_DISP_MINOR_FAULT_MS );
		m_prev_ms_state = m_ms_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_MINOR_FAULT_OR_TIMEOUT_CS );
		m_prev_cs_state = m_cs_state;
		m_ms_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::MS_Unrecoverable_Fault( void )
{
	if ( m_ms_state != static_cast<uint8_t>( EIP_DISP_UNRECOVERABLE_FLT_MS ) )
	{
		m_ms_state = static_cast<uint8_t>( EIP_DISP_UNRECOVERABLE_FLT_MS );
		m_prev_ms_state = m_ms_state;
		// m_cs_state = EIP_DISP_MINOR_FAULT_OR_TIMEOUT_CS;
		m_ms_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::MS_Self_Test( void )
{
	if ( m_ms_state != static_cast<uint8_t>( EIP_DISP_SELF_TEST_MS ) )
	{
		m_ms_state = static_cast<uint8_t>( EIP_DISP_SELF_TEST_MS );
		m_prev_ms_state = m_ms_state;
		// m_cs_state = EIP_DISP_MINOR_FAULT_OR_TIMEOUT_CS;
		m_ms_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// Network Status
void EIP_Display::NS_Offline( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_OFFLINE_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_OFFLINE_NS );
		m_prev_ns_state = static_cast<uint8_t>( EIP_DISP_OFFLINE_NS );
		m_cs_state = static_cast<uint8_t>( EIP_DISP_OFFLINE_CS );
		m_prev_cs_state = static_cast<uint8_t>( EIP_DISP_OFFLINE_CS );
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// Network Status
void EIP_Display::NS_Online_Disconnected( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_ONLINE_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_ONLINE_NS );
		m_prev_ns_state = m_ns_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_OP_ONLINE_OR_NEEDS_COMMIS_CS );
		m_prev_cs_state = static_cast<uint8_t>( EIP_DISP_OP_ONLINE_OR_NEEDS_COMMIS_CS );
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::NS_Online_Connected( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_ONLINE_CONNECTED_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_ONLINE_CONNECTED_NS );
		m_prev_ns_state = m_ns_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_OP_ONLINE_CONNECTED_CS );
		m_prev_cs_state = m_cs_state;
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::NS_Connection_Timeout( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_CONNECTION_TIMEOUT_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_CONNECTION_TIMEOUT_NS );
		m_prev_ns_state = m_ns_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_MINOR_FAULT_OR_TIMEOUT_CS );
		m_prev_cs_state = m_cs_state;
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   void     EIP_Display::NS_Critical_Link_Timeout( void )
   {
    if ( m_ns_state != EIP_DISP_CONNECTION_TIMEOUT_NS )
    {
        m_ns_state = EIP_DISP_CONNECTION_TIMEOUT_NS;
        m_prev_ns_state = m_ns_state;
        m_cs_state = EIP_DISP_CRITICAL_FLT_OR_LINK_FAIL_CS;
        m_prev_cs_state = m_cs_state;
        m_ns_tick = 0;
    }
   }
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::NS_Critical_Link_Failure( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_CRITICAL_LINK_FLT_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_CRITICAL_LINK_FLT_NS );
		m_prev_ns_state = m_ns_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_CRITICAL_FLT_OR_LINK_FAIL_CS );
		m_prev_cs_state = m_cs_state;
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::NS_Comm_Faulted_With_Identify( void )
{
	if ( m_ns_state != static_cast<uint8_t>( EIP_DISP_COMM_FAULTED_IDENTIFY_NS ) )
	{
		m_ns_state = static_cast<uint8_t>( EIP_DISP_COMM_FAULTED_IDENTIFY_NS );
		m_prev_ns_state = m_ns_state;
		m_cs_state = static_cast<uint8_t>( EIP_DISP_COMM_FAULTED_IDENTIFY_CS );
		m_prev_cs_state = m_cs_state;
		m_ns_tick = 0U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Display::LED_Test_Handler( void )
{
	switch ( m_led_test_state )
	{
		case EIP_DISP_PWR_UP_TEST_MS_GREEN:
			m_grn_mod_led_ctrl->On();
			m_red_mod_led_ctrl->Off();
			m_grn_net_led_ctrl->Off();
			m_red_net_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_MS_RED );
			break;

		case EIP_DISP_PWR_UP_TEST_MS_RED:
			m_grn_mod_led_ctrl->Off();
			m_red_mod_led_ctrl->On();
			// m_grn_net_led_ctrl->Off();
			// m_red_net_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_NS_GREEN );
			break;

		case EIP_DISP_PWR_UP_TEST_NS_GREEN:
			m_grn_mod_led_ctrl->On();
			m_red_mod_led_ctrl->Off();
			m_grn_net_led_ctrl->On();
			// m_red_net_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_NS_RED );
			break;

		case EIP_DISP_PWR_UP_TEST_NS_RED:
			// m_grn_mod_led_ctrl->On();
			// m_red_mod_led_ctrl->Off();
			m_grn_net_led_ctrl->Off();
			m_red_net_led_ctrl->On();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_NS_OFF );
			break;

		case EIP_DISP_PWR_UP_TEST_NS_OFF:
			// m_grn_mod_led_ctrl->On();
			// m_red_mod_led_ctrl->Off();
			// m_grn_net_led_ctrl->Off();
			m_red_net_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE );
			break;

		/// This is the beginning of the combo LED test states.
		case EIP_DISP_PWR_UP_TEST_CS_GREEN:
			m_grn_mod_led_ctrl->On();
			m_red_mod_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_CS_RED );
			break;

		case EIP_DISP_PWR_UP_TEST_CS_RED:
			m_grn_mod_led_ctrl->Off();
			m_red_mod_led_ctrl->On();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_CS_OFF );
			break;

		case EIP_DISP_PWR_UP_TEST_CS_OFF:
			m_grn_mod_led_ctrl->Off();
			m_red_mod_led_ctrl->Off();
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE );
			break;

		default:
			m_led_test_state = static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE );
			break;
	}

	if ( m_led_test_state == static_cast<uint8_t>( EIP_DISP_PWR_UP_TEST_END_STATE ) )
	{
		Reset_Display();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t EIP_Display::Update_Display( BF_Misc::LED* green, BF_Misc::LED* red, uint8_t& state,
									  uint8_t prev_state ) const
{
	uint16_t tick_time = 0U;

	switch ( state )
	{
		case EIP_DISP_OFF:
			green->Off();
			red->Off();
			tick_time = static_cast<uint16_t>( EIP_DISPLAY_FULL_TIME_OFF_TIME );
			break;

		case EIP_DISP_GREEN:
			green->On();
			red->Off();
			tick_time = static_cast<uint16_t>( EIP_DISPLAY_FULL_TIME_ON_TIME );
			break;

		case EIP_DISP_FLASH_GREEN:
			red->Off();
			green->Toggle();	// Toggle_Led( m_grn_mod_led_ctrl );
			tick_time = static_cast<uint16_t>( EIP_DISPLAY_SLOW_FLASH_TIME );
			break;

		case EIP_DISP_RED:
			green->Off();
			red->On();
			tick_time = static_cast<uint16_t>( EIP_DISPLAY_FULL_TIME_ON_TIME );
			break;

		case EIP_DISP_FLASH_RED:
			green->Off();
			red->Toggle();		// Toggle_Led( m_red_mod_led_ctrl );
			tick_time = static_cast<uint16_t>( EIP_DISPLAY_SLOW_FLASH_TIME );
			break;

		case EIP_DISP_SLOW_FLASH_GREEN_RED:
		case EIP_DISP_FAST_FLASH_GREEN_RED:
			if ( state == static_cast<uint8_t>( EIP_DISP_SLOW_FLASH_GREEN_RED ) )
			{
				tick_time = static_cast<uint16_t>( EIP_DISPLAY_SLOW_FLASH_TIME );
			}
			else
			{
				tick_time = static_cast<uint16_t>( EIP_DISPLAY_FAST_FLASH_TIME );
			}
			if ( green->Is_On() )
			{
				green->Off();
				red->On();
			}
			else
			{
				green->On();
				red->Off();
			}
			break;

		default:
			green->On();
			red->On();
			break;
	}

	return ( tick_time );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'prev_state' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}
