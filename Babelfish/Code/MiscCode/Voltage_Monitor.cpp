/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Voltage_Monitor.h"
#include "Voltage_Divider.h"

namespace BF_Misc
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Monitor::Voltage_Monitor( Voltage_Divider* pVoltDivider, uint16_t nLowThreshVolt,
								  uint16_t nRunningVolt, VoltMonCallBack_t pCallback,
								  uint16_t nHysteresisVolt,
								  BF_Lib::Timers::TIMERS_TIME_TD checkInterval ) :
	m_pVoltDivider( reinterpret_cast<Voltage_Divider*>( nullptr ) ),
	m_currentState( Voltage_Monitor::UNKNOWN ),
	m_pCallback( reinterpret_cast<VoltMonCallBack_t>( nullptr ) ),
	m_nLowThreshVolt( nLowThreshVolt ),
	m_nRunningVolt( nRunningVolt ),
	m_nCurVoltage( 0U ),
	m_nHysteresisVolt( nHysteresisVolt )
{
	BF_Lib::Timers* timer;

	m_pCallback = pCallback;
	m_pVoltDivider = pVoltDivider;

	timer = new BF_Lib::Timers( &MonitorVoltageStatic,
								reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
								BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "Voltage Monitor" );

	timer->Start( checkInterval, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Monitor::VoltageState_t Voltage_Monitor::GetVoltState()
{
	MonitorVoltage();

	return ( m_currentState );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Voltage_Monitor::GetCurrentVoltage()
{
	MonitorVoltage();

	return ( m_nCurVoltage );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Voltage_Monitor::MonitorVoltage( void )
{
	bool bNotificationRequired = false;

	m_nCurVoltage = m_pVoltDivider->Refresh();

	switch ( m_currentState )
	{
		case RUNNING_OR_ABOVE_RUNNING:
			if ( m_nCurVoltage < m_nRunningVolt )
			{
				m_currentState = BELOW_RUNNING_ABOVE_LOW_THRESH;
				bNotificationRequired = true;
			}
			break;

		case BELOW_RUNNING_ABOVE_LOW_THRESH:
			if ( m_nCurVoltage < m_nLowThreshVolt )
			{
				m_currentState = BELOW_LOW_THRESH;
				bNotificationRequired = true;
			}
			else if ( m_nCurVoltage > ( m_nRunningVolt + m_nHysteresisVolt ) )
			{
				m_currentState = RUNNING_OR_ABOVE_RUNNING;
				bNotificationRequired = true;
			}
			else
			{
				// Remain in this state.
			}
			break;

		case BELOW_LOW_THRESH:
			if ( m_nCurVoltage > ( m_nLowThreshVolt + m_nHysteresisVolt ) )
			{
				m_currentState = BELOW_RUNNING_ABOVE_LOW_THRESH;
				bNotificationRequired = true;
			}
			break;

		case UNKNOWN:
			if ( m_nCurVoltage < m_nLowThreshVolt )
			{
				m_currentState = BELOW_LOW_THRESH;
				bNotificationRequired = true;
			}
			else if ( ( m_nCurVoltage >= m_nLowThreshVolt ) &&
					  ( m_nCurVoltage < m_nRunningVolt ) )
			{
				m_currentState = BELOW_RUNNING_ABOVE_LOW_THRESH;
				bNotificationRequired = true;
			}
			else
			{
				m_currentState = RUNNING_OR_ABOVE_RUNNING;
				bNotificationRequired = true;
			}
			break;
	}

	if ( ( nullptr != m_pCallback ) && ( true == bNotificationRequired ) )
	{
		( *m_pCallback )( m_currentState, m_nCurVoltage );
	}
}

}// BF_Misc
