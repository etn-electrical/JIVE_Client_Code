/**
 *****************************************************************************************
 * @file
 *
 * @brief Implementation of Voltage_Monitor_Plus class.
 *
 * @copyright 2015 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Voltage_Monitor_Plus.h"
#include "uC_Reset.h"
#include "Babelfish_Assert.h"

namespace BF_Misc
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Monitor_Plus::Voltage_Monitor_Plus(
	Voltage_Divider* voltage_divider,
	uint16_t shutdown_threshold,
	uint16_t startup_threshold,
	callback_t callback_function,
	uint16_t hysteresis,
	BF_Lib::Timers::TIMERS_TIME_TD sample_interval,
	BF_Lib::Timers::TIMERS_TIME_TD startup_interval ) :
	m_voltage_divider( voltage_divider ),
	m_shutdown_threshold( shutdown_threshold ),
	m_startup_threshold( startup_threshold ),
	m_callback_function( callback_function ),
	m_hysteresis( hysteresis ),
	m_sample_interval( sample_interval ),
	m_startup_interval( startup_interval ),
	m_state( STARTUP ),
	m_voltage( 0U ),
	m_startup_counter( 0U ),
	m_timer( reinterpret_cast<BF_Lib::Timers*>( nullptr ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Monitor_Plus::~Voltage_Monitor_Plus()
{
	m_voltage_divider = reinterpret_cast<Voltage_Divider*>( nullptr );
	m_callback_function = reinterpret_cast<Voltage_Monitor_Plus::callback_t>( nullptr );
	delete m_timer;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Wait_For_Power_To_Stablize( void )
{
	BF_Lib::Timers_uSec::TIME_TD start_time;
	BF_Lib::Timers_uSec::TIME_TD sample_interval = m_sample_interval * 1000U;

	Init_Startup_Counter();
	while ( Is_Startup_Counter_Expired() == false )
	{
		start_time = BF_Lib::Timers_uSec::Get_Time();
		while ( BF_Lib::Timers_uSec::Expired( start_time, sample_interval ) == false )
		{}

		uC_Watchdog::Force_Feed_Dog();

		m_voltage = m_voltage_divider->Get_Voltage();
		if ( m_voltage < ( m_startup_threshold - m_hysteresis ) )
		{
			Init_Startup_Counter();
		}
		else
		{
			Bump_Startup_Counter();
		}
	}
	m_state = RUN;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Commence_Monitoring( void )
{
	m_timer = new BF_Lib::Timers(
		&Timer_Callback_Static,
		reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
		BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
		"Voltage Monitor Plus" );

	m_timer->Start( m_sample_interval, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Voltage_Monitor_Plus::Get_Voltage( void ) const
{
	return ( m_voltage );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Monitor_Plus::state_t Voltage_Monitor_Plus::Get_State( void ) const
{
	return ( m_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Init_Startup_Counter( void )
{
	m_startup_counter = static_cast<uint16_t>( m_startup_interval / m_sample_interval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Voltage_Monitor_Plus::Is_Startup_Counter_Expired( void ) const
{
	return ( m_startup_counter == 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Bump_Startup_Counter( void )
{
	if ( m_startup_counter != 0U )
	{
		m_startup_counter -= 1U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Timer_Callback( void )
{
	bool notification_required = false;

	m_voltage = m_voltage_divider->Refresh();

	if ( m_voltage < m_shutdown_threshold )
	{
		uC_Reset::Soft_Reset();
	}

	switch ( m_state )
	{
		case RUN:
			if ( m_voltage < ( m_startup_threshold - m_hysteresis ) )
			{
				m_state = LOW_POWER;
				notification_required = true;
			}
			break;

		case LOW_POWER:
			if ( m_voltage > m_startup_threshold )
			{
				m_state = RUN;
				notification_required = true;
			}
			break;

		case STARTUP:		// NOTE: fall-through is intended
		default:
			BF_ASSERT( false );
			break;
	}

	if ( ( m_callback_function != reinterpret_cast<callback_t>( nullptr ) ) &&
		 ( notification_required == true ) )
	{
		( *m_callback_function )( m_state, m_voltage );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor_Plus::Timer_Callback_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
{
	( reinterpret_cast<Voltage_Monitor_Plus*>( handle ) )->Timer_Callback();
}

}	// end namespace

