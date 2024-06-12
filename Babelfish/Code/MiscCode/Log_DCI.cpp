/**
 **********************************************************************************************
 *	@file            Log_DCI.cpp
 *
 *	@brief           General logging interface with DCI.
 *	@details		 The file shall include the definitions of all the functions required for
 *                   general logging interface functions with Log_Mem_xxx and
 *					 different trigger methods.
 *	@copyright       2018 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Log_DCI.h"
#include "DCI_Defines.h"
#include "Babelfish_Assert.h"
#include "Log_Config.h"

namespace BF_Misc
{

Log_DCI** Log_DCI::m_log_dci_list = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_DCI::Log_DCI( const DCI_ID_TD* dcid_array, uint16_t dcid_count,
				  Log_Mem* log_mem_handle, DCI_Owner* log_spec_version_owner, const log_version_t log_spec_version,
				  log_change_trigger_t log_change_trigger,
				  const DCI_ID_TD* dcid_watch_list, uint16_t dcid_watch_list_count ) :
	m_log_mem_handle( log_mem_handle ),
	m_timer( nullptr ),
	m_second_ticker( 0U ),
	m_desired_interval( 0U ),
	m_bit_list( nullptr ),
	m_change_tracker( nullptr ),
	m_log_change_trigger( log_change_trigger ),
	m_dcid_array( dcid_array ),
	m_dcid_count( dcid_count ),
	m_source_id( DCI_SOURCE_IDS_Get() ),
	m_patron( new DCI_Patron( false ) ),
	m_dci_value_array( nullptr ),
	m_dcid_watch_list( nullptr ),
	m_dcid_watch_list_count( 0U )
{
	uint_fast16_t i;
	uint16_t entry_size = 0U;
	DCI_LENGTH_TD dci_length = 0U;
	bool value_change_only = true;	///< if this is true we will only track change of value from 0->1
	log_id_t total_logs = 0U;
	bool log_spec_ver_match_flag = false;

	for ( i = 0; i < m_dcid_count; i++ )
	{
		m_patron->Get_Length( m_dcid_array[i], &dci_length );
		entry_size += dci_length;
	}

	log_id_t log_index = m_log_mem_handle->Log_Index();

	/* Check log spec version and get status */
	log_spec_ver_match_flag = Check_Log_Spec_Version( log_spec_version_owner, log_spec_version, log_index );

	m_dci_value_array = m_log_mem_handle->Log_Mem_Config( entry_size, log_spec_ver_match_flag );

	/*Update the log spec version for change in log entry format */
	if ( log_spec_ver_match_flag != true )
	{
		Update_Log_Spec_Version( log_spec_version_owner, log_spec_version );
	}

	m_timer = new BF_Lib::Timers( &Timed_Task_Static, this,
								  BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY, "General Logging" );

	/* Change based logging Initialization */
	if ( m_log_change_trigger != ONLY_MANUAL_TRIGGER )
	{
		if ( dcid_watch_list_count != 0U )
		{
			m_dcid_watch_list = dcid_watch_list;
			m_dcid_watch_list_count = dcid_watch_list_count;
		}
		else
		{
			m_dcid_watch_list = m_dcid_array;
			m_dcid_watch_list_count = m_dcid_count;
		}

		if ( m_log_change_trigger == LOG_ON_ALL_WRITE )
		{
			value_change_only = false;	///< to track value change from 0->0 also ie to track any write operation
		}
		m_change_tracker = new Change_Tracker( &Change_Track_Static,
											   reinterpret_cast<Change_Tracker::cback_param_t>( this ),
											   value_change_only, Change_Tracker::RAM_ATTRIB_MASK );

		if ( m_log_change_trigger != LOG_ON_ANY_CHANGE )
		{
			m_bit_list = new BF_Lib::Bit_List( m_dcid_watch_list_count );
		}
	}


	if ( m_log_dci_list == nullptr )
	{
#ifndef LOG_DCI_TOTAL_LOGS
		m_log_dci_list = ( Log_DCI** )Ram::Allocate( sizeof( Log_DCI* ) * TOTAL_LOG_ID );
		total_logs = TOTAL_LOG_ID;
#else
		m_log_dci_list = ( Log_DCI** )Ram::Allocate( sizeof( Log_DCI* ) * LOG_DCI_TOTAL_LOGS );
		total_logs = LOG_DCI_TOTAL_LOGS;
#endif
		BF_ASSERT( m_log_dci_list );
		for ( log_id_t i = 0U; i < total_logs; i++ )
		{
			m_log_dci_list[i] = nullptr;
		}

		m_log_dci_list[log_index] = this;
	}
	else
	{
		if ( m_log_dci_list[log_index] != nullptr )
		{
			/* how it was found already filled, same log ID repeated by mistake? */
			BF_ASSERT( false );
		}
		else
		{
			m_log_dci_list[log_index] = this;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Log_DCI::Update_List( log_id_t log_index, Log_DCI* log_dci )
{
	bool status = false;

#ifndef LOG_DCI_TOTAL_LOGS
	log_id_t total_logs = TOTAL_LOG_ID;
#else
	log_id_t total_logs = LOG_DCI_TOTAL_LOGS;
#endif

	BF_ASSERT( m_log_dci_list );
	if ( log_index < total_logs )
	{
		m_log_dci_list[log_index] = log_dci;
		status = true;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Start_Interval( uint32_t interval_time_msec )
{
	Stop_Interval();
	Trigger();	///< Capture first log

	if ( interval_time_msec > MINUTE_TO_MILLISECOND_COUNT )
	{
		/* Interval in greater than 1 Minute. Resolution changed to second */
		m_desired_interval = interval_time_msec;
		m_timer->Start( SECOND_TO_MILLISECOND_COUNT, true );	///< Set one second auto reload timer
	}
	else
	{
		m_desired_interval = interval_time_msec;
		m_timer->Start( interval_time_msec, true );			///< Set arg millisec auto reload timer
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Timed_Task( void )
{
	if ( m_desired_interval > MINUTE_TO_MILLISECOND_COUNT )	///< Interval in greater than 1 Minute
	{
		m_second_ticker++;

		/* Check for interval complete event */
		if ( m_second_ticker >= ( m_desired_interval / SECOND_TO_MILLISECOND_COUNT ) )
		{
			m_second_ticker = 0U;
			Trigger();
		}
	}
	else
	{
		Trigger();	///< Interval in less than or equal to 1 Minute
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Stop_Interval( void )
{
	m_timer->Stop();
	m_second_ticker = 0U;
	m_desired_interval = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Start_Change( void )
{
	for ( uint_fast16_t i = 0U; i < m_dcid_watch_list_count; i++ )
	{
		m_change_tracker->Track_ID( m_dcid_watch_list[i] );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Stop_Change( void )
{
	m_change_tracker->UnTrack_All_IDs();
	if ( m_bit_list != nullptr )
	{
		m_bit_list->Dump();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem::error_t Log_DCI::Trigger( void )
{
	uint16_t dcid;
	uint16_t data_index;
	DCI_LENGTH_TD dci_length = 0U;

	Log_Mem::error_t return_status;

	for ( dcid = 0U, data_index = 0U; dcid < m_dcid_count; dcid++, data_index += dci_length )
	{
		Log_Get_DCID( m_dcid_array[dcid], &m_dci_value_array[data_index] );
		m_patron->Get_Length( m_dcid_array[dcid], &dci_length );
	}
	return_status = m_log_mem_handle->Enque( m_dci_value_array );
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Log_DCI::Log_Get_DCID( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ERROR_TD return_status;		// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Change_Track( DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
{
	uint_fast16_t dci_index = 0U;
	uint_fast16_t i;

	if ( m_log_change_trigger != LOG_ON_ANY_CHANGE )
	{
		for ( i = 0U; i < m_dcid_watch_list_count; i++ )
		{
			if ( m_dcid_watch_list[i] == dci_id )
			{
				dci_index = i;
			}
		}
		/* Set the bit that matches the dcid location in bit location. */
		m_bit_list->Set( dci_index );
		if ( m_bit_list->Is_Full() == true )
		{
			Trigger();
			m_bit_list->Dump();
		}
	}
	else
	{
		Trigger();
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Log_DCI::Check_Log_Spec_Version( DCI_Owner* log_spec_version_owner, log_version_t log_spec_version,
									  log_id_t log_index )
{
	log_version_t nv_log_spec_version = 0U;
	bool log_spec_ver_match_flag = false;

	if ( log_spec_version_owner != nullptr )
	{
		/* Read log spec version from NV */
		if ( log_spec_version_owner->Check_Out_Init( &nv_log_spec_version ) )
		{
			if ( nv_log_spec_version != log_spec_version )
			{
				/* Log spec version not matched, Go for erase all sector of particular logging */
				log_spec_ver_match_flag = false;
			}
			else
			{
				/* Log spec version matched, Go for log mem integrity check */
				log_spec_ver_match_flag = true;
			}
		}
	}
	else
	{
		log_spec_ver_match_flag = true;
	}
	return ( log_spec_ver_match_flag );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_DCI::Update_Log_Spec_Version( DCI_Owner* log_spec_version_owner, log_version_t log_spec_version )
{
	if ( log_spec_version_owner != nullptr )
	{
		/* Update log spec version for specific logging in RAM and NV */
		log_spec_version_owner->Check_In( &log_spec_version );
		log_spec_version_owner->Check_In_Init( &log_spec_version );
	}
}

}/* end namespace BF_Misc for this module */
