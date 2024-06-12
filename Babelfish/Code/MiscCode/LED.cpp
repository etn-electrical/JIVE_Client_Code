/**
 **********************************************************************************************
 *  @file            LED.cpp C++ Implementation File for LED controlling functionalities
 *
 *  @brief           The file shall include the definitions of all the functions required for
 *                   LED controllong and which are declared in corresponding header file
 *  @details
 *  @copyright       2014 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "LED.h"

namespace BF_Misc
{

LED::led_test_cback_t* const LED::CALLBACK_LIST_END =
	reinterpret_cast<led_test_cback_t*>( nullptr );
LED* const LED::LED_LIST_END =
	reinterpret_cast<LED*>( nullptr );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
LED* LED::m_led_list = LED::LED_LIST_END;
BF_Lib::Timers* LED::m_test_timer = reinterpret_cast<BF_Lib::Timers*>( nullptr );
LED::led_test_cback_t* LED::m_test_callbacks = CALLBACK_LIST_END;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LED::LED( BF_Lib::Output* output_ctrl, bool led_test_include ) :
	m_ctrl_bits( 0U ),
	m_output_ctrl( output_ctrl ),
	m_next( m_led_list ),
	m_flash_timer( new BF_Lib::Timers( &Flash_Task_Static, this,
									   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
									   "LED Flash" ) ),
	m_on_time( 0U ),
	m_off_time( 0U )
{
	m_led_list = this;

	Include_In_LED_Test( led_test_include );

	if ( m_test_timer == reinterpret_cast<BF_Lib::Timers*>( nullptr ) )
	{
		m_test_timer = new BF_Lib::Timers( &Test_End,
										   reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( nullptr ),
										   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
										   "LED Test" );

	}

	if ( !m_test_timer->Is_Dead() )
	{
		m_output_ctrl->On();
	}
	else
	{
		Off();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LED::~LED( void )
{
	LED* walker;

	// Must be tested and verified.
	BF_Lib::Exception::Destructor_Not_Supported();

	if ( m_led_list == this )
	{
		m_led_list = LED_LIST_END;
	}
	else
	{
		walker = m_led_list;
		while ( walker->m_next != this )
		{
			walker = walker->m_next;
		}
		walker->m_next = walker->m_next->m_next;
	}
	m_next = LED_LIST_END;

	delete m_flash_timer;
	m_output_ctrl = reinterpret_cast<BF_Lib::Output*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::On( void )
{
	m_output_ctrl->On();
	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_ON_BIT );
	BF_Lib::Bit::Set( m_ctrl_bits, STORED_STATE_ON_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Off( void )
{
	bool test_timer_dead;
	bool test_led_include;

	test_timer_dead = m_test_timer->Is_Dead();
	test_led_include = BF_Lib::Bit::Test( m_ctrl_bits, LED_TEST_INCLUDE_BIT );

	if ( ( test_timer_dead == true ) || ( test_led_include == false ) )
	{
		m_output_ctrl->Off();
	}

	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_ON_BIT );
	BF_Lib::Bit::Clr( m_ctrl_bits, STORED_STATE_ON_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Set_To( bool state )
{
	if ( state == true )
	{
		On();
	}
	else
	{
		Off();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Toggle( void )
{
	if ( Is_On() )
	{
		Off();
	}
	else
	{
		On();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Include_In_LED_Test( bool include )
{
	if ( include == true )
	{
		BF_Lib::Bit::Set( m_ctrl_bits, LED_TEST_INCLUDE_BIT );
	}
	else
	{
		BF_Lib::Bit::Clr( m_ctrl_bits, LED_TEST_INCLUDE_BIT );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Test_All( BF_Lib::Timers::TIMERS_TIME_TD time_test )
{
	LED* led_walker;

	if ( ( m_test_timer->Is_Dead() ) && ( m_led_list != LED_LIST_END ) )
	{
		led_walker = m_led_list;
		while ( led_walker != LED_LIST_END )
		{
			if ( BF_Lib::Bit::Test( led_walker->m_ctrl_bits, LED_TEST_INCLUDE_BIT ) )
			{
				led_walker->m_output_ctrl->On();
			}
			led_walker = led_walker->m_next;
		}

		m_test_timer->Start( time_test, false );
	}

	Call_External_Test_Handlers( time_test );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Attach_Test_Handler( led_test_cback_t* callback_struct )
{
	callback_struct->next = m_test_callbacks;
	m_test_callbacks = callback_struct;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Call_External_Test_Handlers( BF_Lib::Timers::TIMERS_TIME_TD time_test )
{
	led_test_cback_t* walker;

	walker = m_test_callbacks;
	while ( walker != CALLBACK_LIST_END )
	{
		( *walker->func )( walker->param, time_test );
		walker = walker->next;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Test_End( void* param )
{
	LED* led_walker;
	bool led_stored_state_on;
	bool led_test_include;

	m_test_timer->Stop();
	led_walker = m_led_list;
	while ( led_walker != LED_LIST_END )
	{

		led_stored_state_on = BF_Lib::Bit::Test( led_walker->m_ctrl_bits, STORED_STATE_ON_BIT );
		led_test_include = BF_Lib::Bit::Test( led_walker->m_ctrl_bits, LED_TEST_INCLUDE_BIT );
		if ( ( led_stored_state_on == false ) && ( led_test_include == true ) )
		{
			led_walker->m_output_ctrl->Off();
		}
		led_walker = led_walker->m_next;

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
void LED::Flash( BF_Lib::Timers::TIMERS_TIME_TD on_time,
				 BF_Lib::Timers::TIMERS_TIME_TD off_time,
				 bool initial_state )
{
	bool flash_timer_dead;
	bool test_flash_on;

	m_on_time = on_time;
	m_off_time = off_time;
	flash_timer_dead = m_flash_timer->Is_Dead();
	test_flash_on = BF_Lib::Bit::Test( m_ctrl_bits, FLASH_ON_BIT );
	if ( ( flash_timer_dead == true ) || ( test_flash_on == false ) )
	{
		if ( initial_state == true )
		{
			m_flash_timer->Start( m_off_time );
			Off();
		}
		else
		{
			m_flash_timer->Start( m_on_time );
			On();
		}
	}
	BF_Lib::Bit::Set( m_ctrl_bits, FLASH_ON_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Flash( BF_Lib::Timers::TIMERS_TIME_TD interval, bool initial_state )
{
	Flash( interval, interval, initial_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Flash( void )
{
	Flash( m_on_time, m_off_time, BF_Lib::Bit::Test( m_ctrl_bits, STORED_STATE_ON_BIT ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LED::Flash_Task( void )
{
	if ( BF_Lib::Bit::Test( m_ctrl_bits, FLASH_ON_BIT ) )
	{
		if ( m_output_ctrl->State() )
		{
			if ( m_off_time != SINGLE_FLASH_OFF_TIME )
			{
				m_flash_timer->Start( m_off_time );
			}
			else
			{
				m_flash_timer->Stop();
			}
			if ( m_test_timer->Is_Dead() )
			{
				m_output_ctrl->Off();
			}
		}
		else
		{
			m_flash_timer->Start( m_on_time );
			if ( m_test_timer->Is_Dead() )
			{
				m_output_ctrl->On();
			}
		}
	}
	else
	{
		m_flash_timer->Stop();
	}
}

}
