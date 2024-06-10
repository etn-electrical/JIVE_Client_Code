/**
 *****************************************************************************************
 * @file
 * @brief This file defines all the functions required to handle alarms over IOT
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "IoT_Config.h"
#include "IOT_Debug.h"
#include "Babelfish_Assert.h"
#include "IOT_Alarms.h"

#ifdef IOT_ALARM_SUPPORT

bool IOT_Alarms_Cadence::m_alarms_ready = false;
uint16_t IOT_Alarms_Cadence::m_total_severity_levels = 0U;
uint16_t IOT_Alarms_Cadence::m_severity_lvl_marker = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Alarms_Cadence::IOT_Alarms_Cadence( DCI_Owner* pub_cadence_owner, uint16_t sev_level_idx,
										VECTOR_HANDLE cadence_owners_vect ) :
	m_cadence_ms( 0U ),
	m_cadence_timer( nullptr ),
	m_pub_cadence_owner( pub_cadence_owner ),
	m_severity_level_index( sev_level_idx ),
	m_cadence_owners_vect( cadence_owners_vect )
{
	m_alarms_ready = false;
	if ( false == m_pub_cadence_owner->Check_Out( m_cadence_ms ) )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get cadence rate" );
	}
	m_pub_cadence_owner->Attach_Callback( Reinitialize_Cadence_Rate_Static,
										  reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
										  DCI_ACCESS_POST_SET_RAM_CMD_MSK );
	if ( m_cadence_ms == 0U )///< Immediate publishing with 100ms resolution
	{
		m_cadence_ms = static_cast<uint32_t>( IOT_MESSAGE_SLEEP_MS );
	}

	// Create a new timer to handle the cadence timing for alarms
	m_cadence_timer = new BF_Lib::Timers( Cadence_Timer_Static,
										  reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
										  CR_TASKER_PRIORITY_0 );
	if ( m_cadence_timer != nullptr )
	{
		m_cadence_timer->Start( m_cadence_ms, true );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_Alarms_Cadence::~IOT_Alarms_Cadence( void )
{
	if ( m_cadence_timer != nullptr )
	{
		delete m_cadence_timer;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms_Cadence::Cadence_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	( reinterpret_cast<IOT_Alarms_Cadence*>( param ) )->Cadence_Timer_Task();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms_Cadence::Cadence_Timer_Task( void )
{
	/**
	 * If head and tail indices are initialized after power-up then go for checking alarm updates
	 */
	IOT_Alarms* iot_alarms_instance = IOT_Alarms::Get_IOT_Alarms_Handle_Static();

	BF_ASSERT( iot_alarms_instance );
	if ( iot_alarms_instance->m_head_tail_init_done )
	{
		/**
		 * Check severity level of logged alarm updates and then mark them ready accordingly
		 * m_severity_lvl_marker helps us to know the severity level of logged updates
		 */
		if ( m_severity_lvl_marker & ( 1 << m_severity_level_index ) )
		{
			if ( iot_alarms_instance->Get_Alarm_Update_Count() > 0U )
			{
				///< If there are alarm updates to be published then mark them ready
				m_alarms_ready = true;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Alarms_Cadence::Alarms_Ready( void )
{
	return ( m_alarms_ready );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms_Cadence::Clear_Alarms_Ready( void )
{
	m_alarms_ready = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms_Cadence::Mark_Severity_Level( uint16_t severity_level_idx )
{
	m_severity_lvl_marker |= ( 1 << severity_level_idx );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Alarms_Cadence::Clear_Severity_Level_Marker( void )
{
	m_severity_lvl_marker = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_Alarms_Cadence::Reinitialize_Cadence_Rate_Static( DCI_CBACK_PARAM_TD handle,
																	DCI_ACCESS_ST_TD* access_struct )
{
	IOT_Alarms_Cadence* object_ptr = static_cast<IOT_Alarms_Cadence*>( handle );

	return ( object_ptr->Reinitialize_Cadence( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD IOT_Alarms_Cadence::Reinitialize_Cadence( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	bool success = false;

	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		DCI_Owner* the_owner =
			reinterpret_cast<DCI_Owner*>( VECTOR_element( m_cadence_owners_vect, m_severity_level_index ) );
		success = the_owner->Check_Out( m_cadence_ms );
		if ( success )
		{
			if ( m_cadence_ms == 0U )///< Immediate publishing with 100ms resolution
			{
				m_cadence_ms = static_cast<uint32_t>( IOT_MESSAGE_SLEEP_MS );
			}
			///< Restart the timer with new cadence rate
			m_cadence_timer->Stop();
			m_cadence_timer->Start( m_cadence_ms, true );
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Setting new cadence rate %u ms for severity level %d", m_cadence_ms,
							 m_severity_level_index );
		}
	}
	return ( return_status );
}

#endif	// IOT_ALARM_SUPPORT
