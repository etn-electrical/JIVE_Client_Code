/**
 **********************************************************************************************
 *	@file            Log_Mem_RAM.cpp
 *
 *	@brief           General logging on a RAM buffer
 *	@details		 The file shall include the definitions of all the functions required for
 *                   general logging on internal RAM.
 *	@copyright       2018 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "Log_Mem_RAM.h"
#include "Ram.h"
#include "Log_Mem_List.h"
#include "Babelfish_Assert.h"

namespace BF_Misc
{

#ifdef DEBUG
	#define DEBUG_GENERAL_LOGGING   1
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::Log_Mem_RAM( log_id_t log_index, uint8_t* log_data_buffer, log_mem_size_t log_data_buf_size,
						  log_mem_cb_t clear_log_event_cb ) :
	m_log_data_buffer( log_data_buffer ),
	m_log_data_buf_size( log_data_buf_size ),
	m_log_head_index( START_INDEX ),
	m_log_tail_index( START_INDEX ),
	m_cur_mem_loc( 0U ),
	m_entry_size( 0U ),
	m_max_entry( 0U ),
	m_num_queued_entries( 0U ),
	m_max_head_tail_index( 0U ),
	m_index_reset_flag( 0U ),
	m_clear_log_event_cb( clear_log_event_cb ),
	m_log_index( log_index ),
	m_log_data_buf_size_aligned( 0 ),
	m_overflow_flag( false ),
	m_clear_enable( false ),
	m_max_entry_count( 0 ),
	m_entry_cluster( nullptr )
{
	BF_Misc::Log_Mem_List* log_mem_list_handle = nullptr;

	log_mem_list_handle = new BF_Misc::Log_Mem_List( m_log_index, this );
	BF_ASSERT( log_mem_list_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::~Log_Mem_RAM( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::error_t Log_Mem_RAM::Enque( uint8_t* entry_data )
{
	error_t ret_val = NO_ERROR;

	/*
	   Store the head_index which will be used to access the entry.
	   First 4 bytes of entry_data array are asigned for head index.
	 */
	if ( m_log_head_index == m_max_head_tail_index )
	{
		m_log_head_index = START_INDEX;
	}
	else
	{
		m_log_head_index++;
	}

	if ( m_overflow_flag == TRUE )
	{
		/* In case of overflow tail index will always ++ */
		if ( m_log_tail_index == m_max_head_tail_index )
		{
			m_log_tail_index = START_INDEX;
		}
		else
		{
			m_log_tail_index++;
		}
	}

	for ( uint_fast32_t i = 0U; i < m_entry_size; i++ )
	{
		m_log_data_buffer[m_cur_mem_loc++] = entry_data[i];
	}
	/* As we have aligned the 'm_log_data_buffer' size to 'm_entry_size' there won't
	   be a situation where memory overflow appear in above for loop.*/
	if ( m_cur_mem_loc == m_log_data_buf_size_aligned )
	{
		m_cur_mem_loc = 0U;
		m_overflow_flag = TRUE;	///< Flag won't reset until user executes Clear command
	}

	if ( m_num_queued_entries < m_max_entry )
	{
		m_num_queued_entries++;
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::error_t Log_Mem_RAM::Clear( entry_index_t entry_ctr, entry_index_t* deleted_qty )
{
	error_t ret_val = NO_ERROR;

	if ( entry_ctr == CLEAR_ALL )
	{
		/* Full log memory clear */
		for ( uint_fast32_t i = 0; i < m_log_data_buf_size_aligned; i++ )
		{
			m_log_data_buffer[i] = 0U;
		}
		m_log_head_index = START_INDEX;
		m_log_tail_index = START_INDEX;
		m_overflow_flag = FALSE;
		m_cur_mem_loc = 0U;
		m_num_queued_entries = 0U;
		if ( deleted_qty != nullptr )
		{
			*deleted_qty = 0U;	///< Return zero if cleared all
		}
		ret_val = NO_ERROR;
		if ( m_clear_log_event_cb != nullptr )
		{
			/* Execute the callback for capturing the log memory clear events */
			( *m_clear_log_event_cb )( m_log_index );
		}
	}
	else
	{
		/* TODO: Selective log memory clear */
		ret_val = INVALID_INDEX;
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::entry_index_t Log_Mem_RAM::Num_Entries_Queued( void )
{
	return ( m_num_queued_entries );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::entry_index_t Log_Mem_RAM::Head_Index( void )
{
	return ( m_log_head_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::entry_index_t Log_Mem_RAM::Tail_Index( void )
{
	return ( m_log_tail_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::error_t Log_Mem_RAM::Get_Entry( entry_index_t entry_index,
											 entry_index_t* entry_count,
											 uint8_t* entry_data, entry_index_t* next_entry,
											 entry_index_t* entries_to_head,
											 entry_size_t dest_buff_data_size )
{
	error_t ret_val = NO_ERROR;
	entry_index_t absolute_index;
	bool_t function_end = FALSE;

#ifdef DEBUG_GENERAL_LOGGING
	if ( ( dest_buff_data_size != 0U ) && ( m_entry_size > dest_buff_data_size ) )
	{
		/* Max entry cannot be more than max protocol buffer */
		BF_ASSERT( false );
	}
#endif

	if ( dest_buff_data_size != 0U )
	{
		m_max_entry_count = static_cast<entry_index_t>
			( dest_buff_data_size / static_cast<entry_index_t>( m_entry_size ) );
	}

	if ( entry_index == 0U )
	{
		entry_index = m_log_tail_index;
	}

	if ( m_log_tail_index > m_log_head_index )
	{
		/* This is a scenario where head index has reached it's max value and overrolled to
		   value 1 and increases further. So tail index will be a very very high value
		   and head is starting again with 1,2,3 etc. */
		if ( ( entry_index >= m_log_tail_index ) && ( entry_index <= m_max_head_tail_index ) )
		{
			m_index_reset_flag = true;
		}
		/* Check if entry index is in valid range */
		if ( ( ( entry_index < m_log_tail_index ) && ( entry_index > m_log_head_index ) ) ||
			 ( entry_index > m_max_head_tail_index ) )
		{
			/* Client has lost the tail so entry_index is NOT in valid log window range */
			ret_val = INVALID_INDEX;
		}
	}
	else
	{
		/* Check if entry index is in valid range */
		if ( ( entry_index < m_log_tail_index ) || ( entry_index > m_log_head_index ) )
		{
			/* Client has lost the tail so entry_index is NOT in valid log window range */
			ret_val = INVALID_INDEX;
		}
	}

	if ( ret_val == NO_ERROR )
	{
		if ( *entry_count > m_max_entry_count )
		{
			/* Limit the entry_count as per limitation of called protocol buffer.  byte total
			   modbus frame which includes only 238 byte data */
			*entry_count = m_max_entry_count;
		}

		if ( entry_index == m_log_head_index )
		{	/* Entry index is equal to head. */
			*entry_count = 0U;
			*entries_to_head = 0U;
			*next_entry = entry_index;
			function_end = TRUE;
		}

		if ( function_end == FALSE )
		{
			/* entry_index in valid log window range */
			if ( m_index_reset_flag == false )
			{
				if ( ( entry_index + *entry_count ) < m_log_head_index )
				{
					/* Entry index and requested data in in valid range. */
					*entries_to_head = m_log_head_index - entry_index - *entry_count;
					*next_entry = entry_index + *entry_count;
				}
				else
				{
					/* Client requested more than available data after entry_index. */
					*entries_to_head = 0;
					*entry_count = m_log_head_index - entry_index;
					*next_entry = m_log_head_index;
				}
			}
			else
			{
				if ( ( entry_index + *entry_count ) <= ( m_max_head_tail_index + 1U ) )
				{
					/* Entry index and requested data in in valid range. */
					if ( ( entry_index + *entry_count ) == ( m_max_head_tail_index + 1U ) )
					{
						/* Return all the entries till end of the queue and set next to start(1U) */
						*next_entry = START_INDEX;
					}
					else
					{
						*next_entry = entry_index + *entry_count;
					}
				}
				else
				{
					/* Client requested more than available data after top index. */
					*entry_count = m_max_head_tail_index - entry_index + 1U;
					*next_entry = START_INDEX;
				}
				*entries_to_head = m_max_head_tail_index + m_log_head_index - entry_index - *entry_count;

				m_index_reset_flag = FALSE;
			}
		}

		absolute_index = ( entry_index - 1U ) % m_max_entry;

		if ( absolute_index < m_max_entry )
		{
			uint_fast32_t j = 0U;
			entry_index_t abs_index_base;
			entry_index_t max_index_end;
			entry_index_t max_data_bytes;

			abs_index_base = absolute_index * m_entry_size;
			max_index_end = m_max_entry * m_entry_size;
			max_data_bytes = m_entry_size * ( *entry_count );

			for ( uint_fast32_t i = 0U; i < max_data_bytes; i++ )
			{
				entry_data[i] = m_log_data_buffer[abs_index_base + j];
				j++;
				if ( ( abs_index_base + j ) == ( max_index_end ) )
				{
					abs_index_base = 0U;
					j = 0U;
				}
			}
		}
		else
		{
		#ifdef DEBUG_GENERAL_LOGGING
			BF_ASSERT( false );
			/* Absolute index pointing to out of range buffer locations */
		#endif
			ret_val = INVALID_INDEX;
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* Log_Mem_RAM::Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag )
{
	m_entry_size = entry_size;
	/* passed in value is unused */
	BF_Lib::Unused<bool>::Okay( log_spec_ver_match_flag );

	/* Log data on memory till last 'entry_size' aligned location */
	m_log_data_buf_size_aligned = m_log_data_buf_size - ( m_log_data_buf_size % entry_size );
	m_max_entry = static_cast<uint16_t>( m_log_data_buf_size_aligned / entry_size );
	m_max_head_tail_index = MAX_VALUE - ( MAX_VALUE % m_max_entry );

	m_entry_cluster = reinterpret_cast<uint8_t*>( Ram::Allocate( m_entry_size ) );
	return ( m_entry_cluster );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Log_Mem_RAM::Get_Entry_Size( void )
{
	return ( m_entry_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_RAM::entry_index_t Log_Mem_RAM::Get_Max_Entry_Count( void )
{
	return ( m_max_entry );
}

}/* End namespace BF_Misc for this module */

