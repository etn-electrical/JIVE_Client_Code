/**
 **********************************************************************************************
 *	@file            Log_Mem_FRAM.cpp
 *
 *	@brief           General logging on FRAM
 *
 *	@details	 The file shall include the definitions of all the functions required for general logging on internal or
 * external non volatile memory.
 *	@copyright       2022 Eaton Corporation. All Rights Reserved.
 *      @remark          Eaton Corporation claims proprietary rights to the material disclosed hereon. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "Log_Mem_FRAM.h"
#include "OS_Tasker.h"
#include "StdLib_MV.h"
#include "Ram.h"
#include "uC_Watchdog.h"
#include "Mem_Check.h"
#include "Log_Mem_List.h"
#include "Babelfish_Assert.h"
#include "NV_Ctrl_Debug.h"

namespace BF_Misc
{

#ifdef DEBUG
#define DEBUG_GENERAL_LOGGING   1
#endif

NV_Ctrl::nv_status_t Log_Mem_FRAM::m_block_erase_finish_status = NV_Ctrl::SUCCESS;
NV_Ctrl::nv_status_t Log_Mem_FRAM::m_all_block_erase_finish_status = NV_Ctrl::SUCCESS;

static Mem_Integrity_Audit_Function_Decl_1 m_mem_integrity_log_audit_capture_cb =
	reinterpret_cast<Mem_Integrity_Audit_Function_Decl_1>( nullptr );
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::Log_Mem_FRAM( log_id_t log_index, NV_Ctrl* nv_ctrl_log_mem,
							const memory_block_t* memory_block,
							bool use_protection,
							log_mem_cb_t clear_log_event_cb,
							uint8_t backup_num_events ) :
	m_size_of_index( sizeof( entry_index_t ) ),
	m_head_index( 0U ),
	m_tail_index( 0U ),
	m_entry_size( 0U ),
	m_entry_block_size( 0U ),
	m_max_entries( 0U ),
	m_max_counter_value( 0U ),
	m_entry_cluster( nullptr ),
	m_memory_block( memory_block ),
	m_nv_ctrl_log_mem( nv_ctrl_log_mem ),
	m_clear_log_event_cb( clear_log_event_cb ),
	m_log_index( log_index ),
	m_use_protection( use_protection ),
	m_mem_integrity_status( NO_ERROR ),
	m_crc_check_fail( false )
{
	// coverity[leaked_storage]
	new CR_Tasker( NV_Mem_Operation_Handler_Static, ( CR_TASKER_PARAM )this );

	BF_Misc::Log_Mem_List* log_mem_list_handle = nullptr;

	log_mem_list_handle = new BF_Misc::Log_Mem_List( m_log_index, this );

	// coverity[leaked_storage]
	BF_ASSERT( log_mem_list_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::error_t Log_Mem_FRAM::Enque( uint8_t* entry_data )
{
	error_t return_status = NO_ERROR;

	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;
	log_mem_size_t address = 0U;
	entry_index_t absolute_index = 0U;

	/* head index is used to point the FRAM location to write a new entry absolute_index will be used to get the
	   absolute index to calculate current address memory */

	absolute_index = m_head_index;

	while ( absolute_index > m_max_entries )
	{
		absolute_index -= m_max_entries;
	}

	/* Find out the absolute_index to check that the index lies in which block */

	if ( absolute_index > m_max_entry_per_block_array[0U] )
	{
		absolute_index = absolute_index - m_max_entry_per_block_array[0U];
	}

	address = ( m_memory_block->block_start_addr + INDEX_OFFSET ) + ( ( absolute_index - 1U ) * m_entry_block_size );

	if ( ( nv_ret_status == NV_Ctrl::SUCCESS ) &&
		 ( Log_Mem_FRAM::m_block_erase_finish_status == NV_Ctrl::SUCCESS ) )
	{
		if ( m_mem_integrity_status != MEMORY_FAIL )
		{
			return_status = Push_To_NV_Mem( entry_data, address );
		}
		else
		{
			return_status = MEMORY_FAIL;
		}
	}
	else
	{
		return_status = BUSY;
	}

	if ( return_status != NO_ERROR )
	{
		/* Save the entry in the reserve cluster */
		if ( m_backup_num_events > BACKUP_NUM_EVENTS )
		{
			if ( m_backup_entry_saved < m_backup_num_events )
			{
				BF_Lib::Copy_String( m_entry_data_saved + ( m_entry_size * m_backup_entry_saved ), entry_data,
									 m_entry_size );
				m_backup_entry_saved++;
				m_delayed_enque_flag = true;
			}
			else
			{
				return_status = MEMORY_REJECT;
			}
		}
		else
		{
			BF_Lib::Copy_String( m_entry_data_saved, entry_data, m_entry_size );
			m_delayed_enque_flag = true;
			m_delayed_enque_address = address;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::error_t Log_Mem_FRAM::Push_To_NV_Mem( uint8_t* entry_data_final, log_mem_size_t address )
{
	error_t return_status = NO_ERROR;

	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	if ( m_head_index > m_max_counter_value )
	{
		m_head_index = START_INDEX;
	}

	/* Create a cluster buffer with entry */
	BF_Lib::Copy_String( ( m_entry_cluster ), entry_data_final, m_entry_size );

	/* if protection is enabled then add checksum byte into each entry */
	if ( m_use_protection == true )
	{
		BF_Lib::SPLIT_UINT16 checksum;

		Calculate_Checksum( m_entry_cluster,
							&( checksum.u16 ) );
		BF_Lib::Copy_String( ( m_entry_cluster + m_entry_size ),
							 checksum.u8, SIZE_OF_CHECKSUM );
	}
	/* Invalidate cache memeory buffer before write operation */
#if defined( DATA_CACHE_ENABLE )
	SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( m_entry_cluster ), m_entry_block_size );
#endif  // DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
	SCB_InvalidateICache();
#endif  // INSTRUCTION_CACHE_ENABLE

	return_status = Write_NV_Mem_Data( m_entry_cluster, address, m_entry_block_size, NV::NV_Mem::NO_PROTECTION );

	if ( nv_ret_status != NV_Ctrl::SUCCESS )
	{
		return_status = BUSY;
	}
	else
	{
		m_head_index++;
		if ( m_num_queued_entries != m_max_entries )
		{
			m_num_queued_entries++;
		}

		/* if the rollover scenario, calculate tail index  */
		if ( m_head_index > m_max_entries )
		{
			m_tail_index = m_head_index - m_max_entries;

			/* Save the updated tail index at start of every logging types */
			Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_tail_index ),
							   m_memory_block->block_start_addr + TAIL_INDEX_OFFSET, m_size_of_index,
							   NV::NV_Mem::USE_PROTECTION );
		}
	}
	/* Save Head Index at start of every logging types */
	Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_head_index ), m_memory_block->block_start_addr,
					   m_size_of_index, NV::NV_Mem::USE_PROTECTION );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_Mem_FRAM::Calculate_Checksum( uint8_t* entry_data, uint16_t* checksum )
{
	entry_size_t entry_size = m_entry_size;
	uint16_t cal_checksum = 0U;

	while ( entry_size > 0U )
	{
		cal_checksum += *entry_data;
		entry_data++;
		entry_size--;
	}
	*checksum = cal_checksum;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

Log_Mem_FRAM::error_t Log_Mem_FRAM::Clear( entry_index_t entry_ctr, entry_index_t* deleted_qty )
{
	error_t return_status = BUSY;

	if ( entry_ctr == CLEAR_ALL )
	{
		if ( Log_Mem_FRAM::m_block_erase_finish_status != NV_Ctrl::SUCCESS )
		{
			return_status = BUSY;
		}
		else
		{
			if ( deleted_qty != nullptr )
			{
				*deleted_qty = 0U;	///< Return zero if cleared all
			}
			m_num_queued_entries = 0U;
			m_clear_all_enable = true;
			return_status = NO_ERROR;
		}
	}
	else
	{
		/* TODO: Selective log memory clear */
		return_status = INVALID_INDEX;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Num_Entries_Queued( void )
{
	return ( m_num_queued_entries );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Head_Index( void )
{
	return ( m_head_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Tail_Index( void )
{
	return ( m_tail_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::error_t Log_Mem_FRAM::Get_Entry( entry_index_t entry_index, entry_index_t* entry_count,
											   uint8_t* entry_data, entry_index_t* next_entry,
											   entry_index_t* entries_to_head, entry_size_t dest_buff_data_size )
{
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;
	error_t return_status = NO_ERROR;
	log_mem_size_t address = 0U;
	entry_index_t index_counter = 0U;
	entry_index_t absolute_index = 0U;
	entry_index_t max_entry_count = 0U;
	uint8_t* entry_data_ptr = entry_data;
	bool loop_break = false;
	bool crc_check = false;
	entry_index_t entries_to_read = *entry_count;


	*entry_count = 0U;

	if ( m_mem_integrity_status == MEMORY_FAIL )
	{
		return_status = MEMORY_FAIL;
	}
	else
	{

#ifdef DEBUG_GENERAL_LOGGING
		if ( ( dest_buff_data_size != 0U ) && ( m_entry_size > dest_buff_data_size ) )
		{
			/* Max entry cannot be more than max protocol buffer */
			BF_ASSERT( false );
		}
#endif

		if ( dest_buff_data_size != 0U )
		{
			max_entry_count = static_cast<entry_index_t>
				( dest_buff_data_size / static_cast<entry_index_t>( m_entry_size ) );
		}

		if ( entries_to_read > max_entry_count )
		{
			/* Limit the entry_count as per limitation of client protocol buffer. eg. Logging over Modbus protocol can
			   have only 238 bytes of data. */
			entries_to_read = max_entry_count;
		}

		/* Read Tail Index */
		m_tail_index = Find_Tail_Index();

		if ( m_tail_index != 0U )
		{

			/* If NV_Mem memory is completely empty. */
			if ( ( m_head_index == START_INDEX ) && ( m_tail_index == MAX_VALUE ) )
			{
				m_tail_index = m_head_index;
			}

			if ( entry_index == 0U )
			{
				entry_index = m_tail_index;
				index_counter = m_tail_index;
			}
			else
			{
				index_counter = entry_index;
			}

			if ( ( ( entry_index < m_tail_index ) && ( entry_index > m_head_index ) ) ||
				 ( entry_index > m_max_counter_value ) )
			{
				return_status = INVALID_INDEX;
			}
			else
			{
				while ( ( entries_to_read > 0U ) && ( loop_break == false ) )
				{
					absolute_index = ( entry_index - 1U ) % m_max_entries;

					/* Calculate absolute_index to find out that the entry lies in which block */

					if ( absolute_index >= m_max_entry_per_block_array[0U] )
					{
						absolute_index = absolute_index - m_max_entry_per_block_array[0U];
					}

					if ( entry_index == index_counter )
					{
						address = ( m_memory_block->block_start_addr + INDEX_OFFSET ) +
							( ( absolute_index ) * m_entry_block_size );

						( *entry_count )++;

						nv_ret_status =
							Read_NV_Mem_Data( entry_data_ptr, address, m_entry_size, NV::NV_Mem::NO_PROTECTION );

						if ( nv_ret_status == NV_Ctrl::SUCCESS )
						{
							if ( m_use_protection == true )
							{
								crc_check = Check_Data( entry_data_ptr, address );
								if ( crc_check == false )
								{
									if ( *entry_data_ptr != 0U )
									{
										nv_ret_status = Erase_NV_Mem_Synchronous();
									}
									m_crc_check_fail = true;
									loop_break = true;
								}
							}

							entry_data_ptr = ( uint8_t* )( entry_data_ptr + m_entry_size );
							entry_index++;
							entries_to_read--;

							if ( index_counter < ( m_head_index - 1U ) )
							{
								index_counter++;
							}
						}
						else
						{
							loop_break = true;
						}
					}
					else
					{
						loop_break = true;
					}
				}

				if ( nv_ret_status == NV_Ctrl::SUCCESS )
				{
					/*  Entry_index shows what was the last read location so next entry will start from that point */
					if ( entry_index > m_max_counter_value )
					{
						*next_entry = START_INDEX;
						entry_index = START_INDEX;
					}
					else
					{
						*next_entry = entry_index;
					}
					/* Calculate Remaining Entries after reading the requested entries */

					/* There can be different different case that from which entry index how many entries has been read
					   according to that remaining entries calculated */
					if ( ( m_head_index <= m_max_entries ) &&
						 ( m_tail_index < m_max_entries ) )
					{
						if ( m_crc_check_fail == false )
						{
							*entries_to_head = m_head_index - entry_index;
						}
						else
						{
							*entries_to_head = 0U;
							*next_entry = 0U;
							*entry_count = 0U;
							entry_index = 0U;
							m_crc_check_fail = false;
						}
					}
					/* if head counter has rolled over */
					else if ( m_head_index < m_max_entries )
					{
						if ( entry_index < m_head_index )
						{
							*entries_to_head = m_head_index - entry_index;
						}
						else if ( ( entry_index > m_head_index ) &&
								  ( entry_index < m_max_counter_value ) )
						{
							*entries_to_head = m_max_counter_value - *next_entry + m_head_index;
						}
						else
						{
							*entries_to_head = m_head_index - entry_index;
						}
					}
				}
				else
				{
					return_status = BUSY;
				}
			}
		}
		else
		{
			return_status = BUSY;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Log_Mem_FRAM::Check_Data( uint8_t* entry_data, uint32_t address )
{
	bool good_data = true;

	BF_Lib::SPLIT_UINT16 read_data;
	BF_Lib::SPLIT_UINT16 checksum;
	NV_Ctrl::nv_status_t nv_ret_status;

	read_data.u16 = 0U;
	Calculate_Checksum( entry_data, &( checksum.u16 ) );

	address += m_entry_size;
	nv_ret_status = Read_NV_Mem_Data( read_data.u8, address, SIZE_OF_CHECKSUM, NV::NV_Mem::NO_PROTECTION );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		if ( ( read_data.u16 != checksum.u16 ) || ( ( read_data.u16 == 0U ) && ( checksum.u16 == 0U ) ) )
		{
			good_data = false;
		}
	}
	else
	{
		good_data = false;
	}
	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* Log_Mem_FRAM::Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag )
{
	entry_index_t total_entries = 0U;
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	m_entry_size = entry_size;

	/* If Protection is enable then each entry will contain entry data + checksum so these two elements will contribute
	   to each entry size */
	if ( m_use_protection == true )
	{
		m_entry_block_size = m_entry_size + SIZE_OF_CHECKSUM;
	}
	else
	{
		m_entry_block_size = m_entry_size;
	}
	m_max_entry_per_block_array[0U] = ( m_memory_block->block_size - INDEX_OFFSET ) / m_entry_block_size;


	total_entries = ( m_memory_block->block_size - INDEX_OFFSET ) / m_entry_block_size;

	m_max_entries = total_entries;

	if ( m_max_entries > 0U )
	{
		m_max_counter_value = ( MAX_VALUE - ( MAX_VALUE % m_max_entries ) ) - m_max_entries;
	}

	nv_ret_status = Check_Integrity( log_spec_ver_match_flag );	///< check if the memory used for logging is valid or
																///< not

	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		m_head_index = Find_Head_Index();			///< Read head index when device is powered up

		if ( ( m_head_index == MAX_VALUE ) || ( m_head_index == 0U ) )			///< If Invalid Index No Received
		{
			m_head_index = START_INDEX;
		}

		m_entry_cluster = reinterpret_cast<uint8_t*>( Ram::Allocate( m_entry_block_size ) );

		if ( m_max_entries > m_backup_num_events )
		{
			m_backup_num_events = m_max_entries;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Total backup entries are more than minimum entries possible in a page. Truncating the value equal to m_max_entries i.e. %d",
						   m_max_entries )
		}

		m_entry_data_saved = ( reinterpret_cast<uint8_t*>( Ram::Allocate( m_entry_size * m_backup_num_events ) ) );

		/* Read tail index */
		m_tail_index = Find_Tail_Index();		///< Read head index when device is powered up

		/* If NV_Mem memory is completely empty. */
		if ( ( m_head_index == START_INDEX ) || ( m_tail_index == 0U ) )
		{
			m_tail_index = m_head_index;
			/* Save the tail index at start of every logging types */
			Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_tail_index ),
							   m_memory_block->block_start_addr + TAIL_INDEX_OFFSET, m_size_of_index,
							   NV::NV_Mem::USE_PROTECTION );
		}
		else if ( m_head_index >= m_max_counter_value )
		{
			/* Index Rollover Case */
			m_tail_index = ( m_max_counter_value + 1U ) - m_head_index;
		}
		else
		{
			/* If tail index MAX_VALUE, Assign tail index = 1U */
			if ( m_tail_index == MAX_VALUE )
			{
				m_tail_index = m_head_index;
			}
		}

		if ( m_head_index >= m_tail_index )
		{
			m_num_queued_entries = m_head_index - m_tail_index;
		}
		else
		{
			m_num_queued_entries = m_max_counter_value - m_tail_index + m_head_index;		// maximum entries
		}
	}
	else
	{
		m_mem_integrity_status = MEMORY_FAIL;
	}

	return ( m_entry_cluster + m_size_of_index );		///< Exclude location of Index counter
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Find_Head_Index( void )
{
	entry_index_t head_index = 0U;
	entry_index_t return_index = 0U;
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	/* Read Head Index from start address of logging */
	nv_ret_status = Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &head_index ) ),
									  m_memory_block->block_start_addr, m_size_of_index, NV::NV_Mem::USE_PROTECTION );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		return_index = head_index;
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Find_Tail_Index( void )
{
	entry_index_t tail_index = 0U;
	entry_index_t return_index = 0U;
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	/* Read Head Index from start address of logging*/
	nv_ret_status = Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &tail_index ) ),
									  m_memory_block->block_start_addr + TAIL_INDEX_OFFSET, m_size_of_index,
									  NV::NV_Mem::USE_PROTECTION );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		return_index = tail_index;
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_size_t Log_Mem_FRAM::Get_Entry_Size( void )
{
	return ( m_entry_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_FRAM::entry_index_t Log_Mem_FRAM::Get_Max_Entry_Count( void )
{

	return ( m_max_entries );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_Mem_FRAM::NV_Mem_Operation_Handler( void )
{
	if ( ( m_clear_all_enable == true ) &&
		 ( Log_Mem_FRAM::m_block_erase_finish_status == NV_Ctrl::SUCCESS ) )
	{
		Log_Mem_FRAM::m_block_erase_finish_status = Erase_NV_Mem_Block();
		m_clear_all_enable = false;

		// Erase_NV_Mem_Block() will erase full block for specific logging, need to save m_head_index for new entries
		m_head_index = START_INDEX;
		Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_head_index ), m_memory_block->block_start_addr,
						   m_size_of_index, NV::NV_Mem::USE_PROTECTION );

		if ( m_clear_log_event_cb != nullptr )
		{
			/* Execute the callback for capturing the log memory clear events */
			( *m_clear_log_event_cb )( m_log_index );
		}

		// Erase_NV_Mem_Block() will erase full block for specific logging, need to save m_tail_index for new entries
		// reading
		m_tail_index = m_head_index;
		Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_tail_index ),
						   m_memory_block->block_start_addr + TAIL_INDEX_OFFSET, m_size_of_index,
						   NV::NV_Mem::USE_PROTECTION );
	}

	if ( ( m_delayed_enque_flag == true ) && ( Log_Mem_FRAM::m_block_erase_finish_status == NV_Ctrl::SUCCESS ) )
	{
		if ( m_backup_num_events > BACKUP_NUM_EVENTS )
		{
			if ( m_backup_entry_saved > 0U )
			{
				Enque( m_entry_data_saved + ( m_entry_size * m_backup_next_index ) );
				m_backup_entry_saved--;
				m_backup_next_index++;
			}
			else
			{
				m_delayed_enque_flag = false;
				m_backup_next_index = 0U;

			}
		}
		else
		{
			Enque( m_entry_data_saved );
			m_delayed_enque_flag = false;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_FRAM::Erase_NV_Mem_Block( void )
{
	Log_Mem_FRAM::m_block_erase_finish_status = m_nv_ctrl_log_mem->Erase(
		m_memory_block->block_start_addr,
		m_memory_block->block_end_addr - m_memory_block->block_start_addr,
		0x00U, false );

	return ( Log_Mem_FRAM::m_block_erase_finish_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_FRAM::Write_NV_Mem_Data( uint8_t* data, uint32_t address,
													  uint32_t length, bool protection )
{
	uint8_t retry_count = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	do
	{
		nv_ret_status = m_nv_ctrl_log_mem->Write( data, address, length, protection );
		retry_count++;
	} while ( ( nv_ret_status != NV_Ctrl::SUCCESS ) &&
			  ( retry_count < NV_MEM_OPERATION_RETRY ) );
	return ( nv_ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_FRAM::Read_NV_Mem_Data( uint8_t* data, uint32_t address,
													 uint32_t length, bool protection )
{
	uint8_t retry_count = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	do
	{
		nv_ret_status = m_nv_ctrl_log_mem->Read( data, address, length, protection );
		retry_count++;
	} while ( ( nv_ret_status != NV_Ctrl::SUCCESS ) &&
			  ( retry_count < NV_MEM_OPERATION_RETRY ) );
	return ( nv_ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_FRAM::Check_Integrity( bool log_spec_ver_match_flag )
{
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	/* Check log spec version match status */
	if ( log_spec_ver_match_flag == false )
	{
		/* if log spec version found mismatch (change in log entry), no use of data proceed for erase */
		nv_ret_status = Erase_NV_Mem_Synchronous();
	}

	return ( nv_ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_FRAM::Erase_NV_Mem_Synchronous( void )
{
	Log_Mem_FRAM::m_block_erase_finish_status = Erase_NV_Mem_Block();

	if ( Log_Mem_FRAM::m_block_erase_finish_status == NV_Ctrl::SUCCESS )
	{
		// Erase_NV_Mem_Block() will erase full block for specific logging, need to save m_head_index for new entries
		m_head_index = START_INDEX;
		Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_head_index ), m_memory_block->block_start_addr,
						   m_size_of_index, NV::NV_Mem::USE_PROTECTION );

		if ( m_mem_integrity_log_audit_capture_cb != nullptr )
		{
			/* Execute the callback for capturing the log memory clear events for Data/Memory Integrity failure*/
			( *m_mem_integrity_log_audit_capture_cb )( m_log_index );
		}


		// Erase_NV_Mem_Block() will erase full block for specific logging, need to save m_tail_index for new entries
		// reading
		m_tail_index = m_head_index;
		Write_NV_Mem_Data( reinterpret_cast<uint8_t*>( &m_tail_index ),
						   m_memory_block->block_start_addr + TAIL_INDEX_OFFSET, m_size_of_index,
						   NV::NV_Mem::USE_PROTECTION );
	}

	return ( Log_Mem_FRAM::m_block_erase_finish_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_Mem_FRAM::Attach_Log_Mem_Integrity_Failure_Callback(
	Mem_Integrity_Audit_Function_Decl_1 mem_integrity_audit_log_cb )
{
	m_mem_integrity_log_audit_capture_cb = mem_integrity_audit_log_cb;
}

}/* end namespace BF_Misc for this module */
