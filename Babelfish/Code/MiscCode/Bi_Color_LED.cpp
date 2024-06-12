/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Bi_Color_LED.h"
#include "Babelfish_Assert.h"
namespace BF_Misc
{

Bi_Color_LED* const Bi_Color_LED::LED_LIST_END = reinterpret_cast<Bi_Color_LED*>( nullptr );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
Bi_Color_LED* Bi_Color_LED::m_led_list = Bi_Color_LED::LED_LIST_END;
BF_Lib::Timers* Bi_Color_LED::m_test_timer = reinterpret_cast<BF_Lib::Timers*>( nullptr );
BF_Misc::LED::led_test_cback_t Bi_Color_LED::m_led_test_cback =
{
	reinterpret_cast<BF_Misc::LED::cback_func_t>( nullptr ),	///< The function to call when an
	// LED test is to be executed.
	reinterpret_cast<BF_Misc::LED::cback_param_t>( nullptr ),		///< The parameter to pass back
	// (typically a this).
	reinterpret_cast<BF_Misc::LED::led_test_cback_t*>( nullptr )
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bi_Color_LED::Bi_Color_LED( BF_Misc::LED* led_one, BF_Misc::LED* led_two ) :
	m_color_one_ctrl( led_one ),
	m_color_two_ctrl( led_two ),
	m_next( m_led_list ),
	m_ctrl_bits( 0U )
{
	m_led_list = this;

	m_color_one_ctrl->Include_In_LED_Test( false );
	m_color_two_ctrl->Include_In_LED_Test( false );

	if ( m_test_timer == reinterpret_cast<BF_Lib::Timers*>( nullptr ) )
	{
		m_test_timer = new BF_Lib::Timers( &Test_Handler,
										   reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( nullptr ),
										   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
										   "Bi-Color LED Test" );

		m_led_test_cback.func = &Test_All_LED_Cback;
		m_led_test_cback.param = reinterpret_cast<BF_Misc::LED::cback_param_t>( nullptr );

		LED::Attach_Test_Handler( &m_led_test_cback );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bi_Color_LED::~Bi_Color_LED()
{
	Bi_Color_LED* walker;

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
	delete m_color_one_ctrl;
	delete m_color_two_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::On( bi_color_state_t color )
{
	bool led_test_running;

	if ( m_test_timer->Is_Dead() )
	{
		led_test_running = false;
	}
	else
	{
		led_test_running = true;
	}

	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_1_ON_BIT );
	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_2_ON_BIT );

	switch ( color )
	{
		case COLOR_ONE:
			if ( led_test_running == false )
			{
				m_color_one_ctrl->On();
				m_color_two_ctrl->Off();
			}
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
			BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
			break;

		case COLOR_TWO:
			if ( led_test_running == false )
			{
				m_color_one_ctrl->Off();
				m_color_two_ctrl->On();
			}
			BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
			break;

		case COMBINED_COLOR:
			if ( led_test_running == false )
			{
				m_color_one_ctrl->On();
				m_color_two_ctrl->On();
			}
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::Off( void )
{
	if ( m_test_timer->Is_Dead() )
	{
		m_color_one_ctrl->Off();
		m_color_two_ctrl->Off();
	}

	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_1_ON_BIT );
	BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_2_ON_BIT );
	BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
	BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::Flash( bi_color_state_t color, BF_Lib::Timers::TIMERS_TIME_TD on_time,
						  BF_Lib::Timers::TIMERS_TIME_TD off_time, bool initial_state )
{
	bool led_test_running;
	bool led_1_state;
	bool led_2_state;

	if ( m_test_timer->Is_Dead() )
	{
		led_test_running = false;
	}
	else
	{
		led_test_running = true;
	}

	switch ( color )
	{
		case COLOR_ONE:
			led_1_state = m_color_one_ctrl->Is_On();
			m_color_one_ctrl->Flash( on_time, off_time, initial_state );
			if ( led_test_running == false )
			{
				m_color_two_ctrl->Off();
			}
			else
			{
				// If we are currently under LED test then we need to set the flash up then
				// go and turn it back to whatever state it currently is supposed to be.
				m_color_one_ctrl->Set_To( led_1_state );
			}
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
			BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, FLASH_1_ON_BIT );
			BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_2_ON_BIT );
			break;

		case COLOR_TWO:
			led_2_state = m_color_two_ctrl->Is_On();
			m_color_two_ctrl->Flash( on_time, off_time, initial_state );
			if ( led_test_running == false )
			{
				m_color_one_ctrl->Off();
			}
			else
			{
				// If we are currently under LED test then we need to set the flash up then
				// go and turn it back to whatever state it currently is supposed to be.
				m_color_two_ctrl->Set_To( led_2_state );
			}
			BF_Lib::Bit::Clr( m_ctrl_bits, STORED_LED1_STATE_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, STORED_LED2_STATE_ON_BIT );
			BF_Lib::Bit::Clr( m_ctrl_bits, FLASH_1_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, FLASH_2_ON_BIT );
			break;

		case COMBINED_COLOR:
			led_1_state = m_color_one_ctrl->Is_On();
			led_2_state = m_color_two_ctrl->Is_On();
			// The LEDs are set to off initially, to ensure that they sync up with regards to
			// timing.
			m_color_one_ctrl->Off();
			m_color_two_ctrl->Off();
			m_color_one_ctrl->Flash( on_time, off_time, initial_state );
			m_color_two_ctrl->Flash( on_time, off_time, initial_state );
			if ( led_test_running == true )
			{
				// If we are currently under LED test then we need to set the flash up then
				// go and turn it back to whatever state it currently is supposed to be.
				m_color_one_ctrl->Set_To( led_1_state );
				m_color_two_ctrl->Set_To( led_2_state );
			}
			BF_Lib::Bit::Set( m_ctrl_bits, FLASH_1_ON_BIT );
			BF_Lib::Bit::Set( m_ctrl_bits, FLASH_2_ON_BIT );
			break;

		default:
			BF_ASSERT( false );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::Flash( bi_color_state_t color, BF_Lib::Timers::TIMERS_TIME_TD interval,
						  bool initial_state )
{
	Flash( color, interval, interval, initial_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::Test_All( BF_Lib::Timers::TIMERS_TIME_TD time_test )
{
	Bi_Color_LED* led_walker;

	if ( ( m_test_timer->Is_Dead() ) && ( m_led_list != LED_LIST_END ) )
	{
		led_walker = m_led_list;
		while ( led_walker != LED_LIST_END )
		{
			led_walker->m_color_one_ctrl->On();
			led_walker->m_color_two_ctrl->Off();
			led_walker = led_walker->m_next;
		}

		m_test_timer->Start( time_test / 2U, true );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bi_Color_LED::Test_Handler( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	Bi_Color_LED* led_walker;
	bool end_timer = true;

	BF_Lib::Unused<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>::Okay( param );

	if ( m_led_list != LED_LIST_END )
	{
		led_walker = m_led_list;
		while ( led_walker != LED_LIST_END )
		{
			// If LED 1 is on then we need to turn on LED 2 before we end.
			if ( led_walker->m_color_one_ctrl->Is_On() == true )
			{
				led_walker->m_color_one_ctrl->Off();
				led_walker->m_color_two_ctrl->On();
				end_timer = false;
			}
			else
			{
				bool flash_1_on = BF_Lib::Bit::Test( led_walker->m_ctrl_bits, FLASH_1_ON_BIT );
				bool flash_2_on = BF_Lib::Bit::Test( led_walker->m_ctrl_bits, FLASH_2_ON_BIT );
				if ( ( flash_1_on == true ) && ( flash_2_on == true ) )
				{
					led_walker->m_color_one_ctrl->Flash();
					led_walker->m_color_two_ctrl->Flash();
				}
				else if ( flash_1_on == true )
				{
					led_walker->m_color_one_ctrl->Flash();
					led_walker->m_color_two_ctrl->Off();
				}
				else if ( flash_2_on == true )
				{
					led_walker->m_color_one_ctrl->Off();
					led_walker->m_color_two_ctrl->Flash();
				}
				else
				{
					led_walker->m_color_one_ctrl->Set_To(
						BF_Lib::Bit::Test( led_walker->m_ctrl_bits,
										   STORED_LED1_STATE_ON_BIT ) );
					led_walker->m_color_two_ctrl->Set_To(
						BF_Lib::Bit::Test( led_walker->m_ctrl_bits,
										   STORED_LED2_STATE_ON_BIT ) );
				}
				end_timer = true;
			}
			led_walker = led_walker->m_next;
		}
	}
	if ( end_timer == true )
	{
		m_test_timer->Stop();
	}
}

}

