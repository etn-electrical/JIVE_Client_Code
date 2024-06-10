/**
 **********************************************************************************************
 *	@file            Log_Mem_List.cpp
 *
 *	@brief           Handle the list of log memory (RAM/NV) instances created for each log type
 *	@details		 The file shall include the function definitions for handling list
 *                   of log memory (RAM/NV) instances required for general logging
 *                   on RAM and NV Memory.
 *	@copyright       2020 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "Log_Mem_List.h"
#include "Babelfish_Assert.h"

namespace BF_Misc
{

BF_Misc::Log_Mem** BF_Misc::Log_Mem_List::m_log_mem_list = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem* Log_Mem_List::Get_Instance( log_id_t log_index )
{
	Log_Mem* log_mem = nullptr;

	if ( log_index < Total_Logs() )
	{
		log_mem = m_log_mem_list[log_index];
	}
	return ( log_mem );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem** Log_Mem_List::Get_List( void )
{
	return ( m_log_mem_list );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_List::Log_Mem_List( log_id_t log_index, Log_Mem* this_ptr )
{
	log_id_t total_logs = Total_Logs();

	if ( log_index < total_logs )
	{
		if ( m_log_mem_list == nullptr )
		{
			m_log_mem_list = reinterpret_cast<Log_Mem**>( Ram::Allocate( sizeof( Log_Mem* ) * total_logs ) );
			BF_ASSERT( m_log_mem_list );
			for ( log_id_t i = 0U; i < total_logs; i++ )
			{
				m_log_mem_list[i] = nullptr;
			}
			m_log_mem_list[log_index] = this_ptr;
		}
		else
		{
			if ( ( m_log_mem_list[log_index] == nullptr ) )
			{
				m_log_mem_list[log_index] = this_ptr;
			}
			else
			{
				/* Something wrong here, check if condition */
				BF_ASSERT( false );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Log_Mem_List::Update_List( log_id_t log_index, Log_Mem* log_mem )
{
	bool status = false;

	BF_ASSERT( m_log_mem_list );
	if ( log_index < Total_Logs() )
	{
		m_log_mem_list[log_index] = log_mem;
		status = true;
	}
	return ( status );
}

}	/* End namespace BF_Misc for this module */