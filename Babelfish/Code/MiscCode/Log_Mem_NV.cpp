/**
 **********************************************************************************************
 *	@file            Log_Mem_NV.cpp
 *
 *	@brief           General logging on any type of non volatile memory
 *                   ( Internal Flash, Serial flash, SD Card.)
 *	@details		 The file shall include the definitions of all the functions required for
 *                   general logging on internal or external non volatile memory.
 *	@copyright       2018 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "Log_Mem_NV.h"
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

NV_Ctrl::nv_status_t Log_Mem_NV::m_sector_erase_finish_status = NV_Ctrl::SUCCESS;
NV_Ctrl::nv_status_t Log_Mem_NV::m_all_sector_erase_finish_status = NV_Ctrl::SUCCESS;

static Mem_Integrity_Audit_Function_Decl_1 m_mem_integrity_log_audit_capture_cb =
	reinterpret_cast<Mem_Integrity_Audit_Function_Decl_1>( nullptr );
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::Log_Mem_NV( log_id_t log_index, NV_Ctrl* nv_ctrl_log_mem, const memory_block_t* memory_block,
						sector_num_t total_sectors, bool use_protection,
						log_mem_cb_t clear_log_event_cb, uint8_t backup_num_events ) :
	m_size_of_index( sizeof( entry_index_t ) ),
	m_start_sector( 0U ),
	m_last_sector( 0U ),
	m_tail_sector( 0U ),
	m_current_sector( 0U ),
	m_erased_sector_num( 0U ),
	m_num_queued_entries( 0U ),
	m_head_index( 0U ),
	m_tail_index( 0U ),
	m_total_log_nv_mem( 0U ),
	m_sliding_window_const( 0U ),
	m_entry_size( 0U ),
	m_entry_block_size( 0U ),
	m_max_entries( 0U ),
	m_max_counter_value( 0U ),
	m_entry_cluster( nullptr ),
	m_entry_data_saved( nullptr ),
	m_use_protection( use_protection ),
	m_memory_block( memory_block ),
	m_nv_ctrl_log_mem( nv_ctrl_log_mem ),
	m_total_sectors( total_sectors ),
	m_max_entry_per_sector_array( nullptr ),
	m_clear_all_enable( false ),
	m_delayed_enque_flag( false ),
	m_delayed_enque_address( 0U ),
	m_all_sector_erase_time( 0U ),
	m_clear_log_event_cb( clear_log_event_cb ),
	m_log_index( log_index ),
	m_current_sector_erase_flag( false ),
	m_mem_integrity_status( NO_ERROR ),
	m_backup_num_events( backup_num_events ),
	m_backup_entry_saved( 0U ),
	m_backup_next_index( 0U )
{
	m_max_entry_per_sector_array =
		reinterpret_cast<entry_index_t*>( Ram::Allocate( m_total_sectors *
														 sizeof( entry_index_t ) ) );
	BF_ASSERT( m_max_entry_per_sector_array != nullptr );
	for ( sector_num_t i = 0U; i < m_total_sectors; i++ )
	{
		m_max_entry_per_sector_array[i] = 0U;
		m_total_log_nv_mem += m_memory_block[i].sector_size;
	}
	/* starting sector number */
	m_start_sector = Get_Sector( m_memory_block[0U].sector_start_addr );
	/* find last sector number */
	m_last_sector = Get_Sector( m_memory_block[m_total_sectors - 1U].sector_start_addr );
	/* TODO
	 * After every bootup this will erase external flash for logging
	 * Need to work for future prospective
	 * */
	/*for ( sector_num_t i = m_start_sector; i < m_last_sector; i++ )
	   {
	    Erase_NV_Mem_Sector( i );
	   }*/
	m_current_sector = m_start_sector;
	m_tail_sector = m_start_sector;

#if defined( LOGGING_ON_EXT_FLASH ) || ( !defined( uC_STM32F407_USAGE ) && !defined( uC_STM32F411_USAGE ) && \
	!defined( uC_STM32F427_USAGE ) && !defined( uC_STM32F437_USAGE ) && \
	!defined( uC_STM32F767_USAGE ) && !defined( uC_STM32H743_USAGE ) )
	m_nv_ctrl_log_mem->Attach_Callback( Update_Erase_Finish_Status_Static, this );
#endif
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
Log_Mem_NV::error_t Log_Mem_NV::Enque( uint8_t* entry_data )
{
	error_t return_status = NO_ERROR;

	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;
	log_mem_size_t address = 0U;
	entry_index_t absolute_index = 0U;
	sector_num_t sector_no = 0U;
	bool for_loop_break = false;

	/* head index is used to point the flash location to write a new entry
	   absolute_index will be used to get the absolute index to calculate current address
	   memory */

	absolute_index = m_head_index;

	sector_no = m_start_sector;

	while ( absolute_index > m_max_entries )
	{
		absolute_index -= m_max_entries;
	}

	/* Find out the absolute_index to check that the index lies in which sector */
	for ( sector_num_t i = m_start_sector;
		  ( i <= m_last_sector ) && ( false == for_loop_break );
		  i++ )
	{
		if ( absolute_index > m_max_entry_per_sector_array[i] )
		{
			absolute_index = absolute_index - m_max_entry_per_sector_array[i];
			sector_no++;
		}
		else
		{
			for_loop_break = true;
		}
	}
	m_current_sector = sector_no;
	address = m_memory_block[sector_no].sector_start_addr +
		( ( absolute_index - 1U ) * m_entry_block_size );

	if ( ( absolute_index == START_INDEX ) &&
		 ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
		 ( Log_Mem_NV::m_all_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
		 ( m_delayed_enque_flag == false ) )
	{
		nv_ret_status = Erase_NV_Mem_Sector( sector_no );
		// m_current_sector_erase_flag = true;
		// nv_ret_status = NV_Ctrl::BUSY_ERROR;
	}

	if ( ( nv_ret_status == NV_Ctrl::SUCCESS ) &&
		 ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
		 ( Log_Mem_NV::m_all_sector_erase_finish_status == NV_Ctrl::SUCCESS ) )
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
Log_Mem_NV::error_t Log_Mem_NV::Push_To_NV_Mem( uint8_t* entry_data_final, log_mem_size_t address )
{
	error_t return_status = NO_ERROR;

	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	if ( m_head_index > m_max_counter_value )
	{
		m_head_index = START_INDEX;
	}
	/* Create a cluster buffer with entry,index data and cs if protection enabled */
	BF_Lib::Copy_String( m_entry_cluster, reinterpret_cast<uint8_t*>( &m_head_index ),
						 m_size_of_index );

	BF_Lib::Copy_String( ( m_entry_cluster + m_size_of_index ), entry_data_final, m_entry_size );

	/* if protection is enabled then add checksum byte into each entry */
	if ( m_use_protection == true )
	{
		BF_Lib::SPLIT_UINT16 checksum;

		Calculate_Checksum( ( const_cast<uint8_t*>( entry_data_final ) ),
							&( checksum.u16 ) );
		BF_Lib::Copy_String( ( m_entry_cluster + m_size_of_index + m_entry_size ),
							 checksum.u8, SIZE_OF_CHECKSUM );
	}
	// Invalidate cache memeory buffer before write operation
#if defined( DATA_CACHE_ENABLE )
	SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( m_entry_cluster ), m_entry_block_size );
#endif	// DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
	SCB_InvalidateICache();
#endif	// INSTRUCTION_CACHE_ENABLE

	nv_ret_status = Write_NV_Mem_Data( m_entry_cluster, address, m_entry_block_size );
	if ( nv_ret_status != NV_Ctrl::SUCCESS )
	{
		return_status = BUSY;
	}
	else
	{
		m_head_index++;
		if ( m_num_queued_entries != m_sliding_window_const )
		{
			m_num_queued_entries++;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

Log_Mem_NV::error_t Log_Mem_NV::Clear( entry_index_t entry_ctr, entry_index_t* deleted_qty )
{
	error_t return_status = BUSY;

	if ( entry_ctr == CLEAR_ALL )
	{
		if ( ( Log_Mem_NV::m_sector_erase_finish_status != NV_Ctrl::SUCCESS ) ||
			 ( Log_Mem_NV::m_all_sector_erase_finish_status != NV_Ctrl::SUCCESS ) )
		{
			// m_clear_all_enable = true;
			return_status = BUSY;
		}
		else
		{
			m_current_sector = m_start_sector;
			m_tail_sector = m_start_sector;
			m_erased_sector_num = m_start_sector;
			if ( deleted_qty != nullptr )
			{
				*deleted_qty = 0U;	///< Return zero if cleared all
			}
			m_num_queued_entries = 0U;
			Log_Mem_NV::m_all_sector_erase_finish_status = NV_Ctrl::BUSY_ERROR;
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
Log_Mem_NV::error_t Log_Mem_NV::Get_Entry( entry_index_t entry_index,
										   entry_index_t* entry_count,
										   uint8_t* entry_data, entry_index_t* next_entry,
										   entry_index_t* entries_to_head,
										   entry_size_t dest_buff_data_size )
{
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;
	error_t return_status = NO_ERROR;
	log_mem_size_t address = 0U;
	entry_index_t index_counter = 0U;
	sector_num_t sector_no = 0U;
	entry_index_t absolute_index = 0U;
	entry_index_t max_entry_count = 0U;
	uint8_t* entry_data_ptr = entry_data;
	sector_num_t tail_sector = 0U;
	bool loop_break = false;
	bool for_loop_break = false;
	entry_index_t entries_to_read = *entry_count;

	// uint32_t busy_delay_time = 0U;
	*entry_count = 0U;

	if ( m_mem_integrity_status == MEMORY_FAIL )
	{
		return_status = MEMORY_FAIL;
	}
	else if ( ( Log_Mem_NV::m_sector_erase_finish_status != NV_Ctrl::SUCCESS ) ||
			  ( Log_Mem_NV::m_all_sector_erase_finish_status != NV_Ctrl::SUCCESS ) )
	{
		if ( Log_Mem_NV::m_all_sector_erase_finish_status != NV_Ctrl::SUCCESS )
		{
			// busy_delay_time = m_all_sector_erase_time;
		}
		else if ( Log_Mem_NV::m_sector_erase_finish_status != NV_Ctrl::SUCCESS )
		{
			// busy_delay_time = m_memory_block[m_current_sector].sector_erase_time;
		}
		else
		{}
		return_status = BUSY;
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
			/* Limit the entry_count as per limitation of called protocol buffer.  byte total
			   modbus frame which includes only 238 byte data */
			entries_to_read = max_entry_count;
		}
		/* Read Tail Sector */
		tail_sector = Find_Tail_Sector();
		sector_no = tail_sector;
		address = m_memory_block[sector_no].sector_start_addr;
		nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &m_tail_index ), address, m_size_of_index );

		if ( nv_ret_status == NV_Ctrl::SUCCESS )
		{
			/* If NV_Mem memory is completely empty. */
			if ( ( m_head_index == START_INDEX ) && ( m_tail_index == MAX_VALUE ) )
			{
				m_tail_index = m_head_index;
			}

			/* Get the largest sector to calculate the sliding window size. */
			if ( entries_to_read > m_sliding_window_const )
			{
				entries_to_read = m_sliding_window_const;
			}

			if ( ( m_head_index > m_sliding_window_const ) &&
				 ( m_tail_index < m_sliding_window_const ) ||
				 ( m_head_index > m_sliding_window_const ) &&
				 ( m_tail_index > m_sliding_window_const ) )
			{
				m_tail_index = m_head_index - m_sliding_window_const;
			}
			else if ( ( m_head_index < m_sliding_window_const ) &&
					  ( m_tail_index > m_sliding_window_const ) )
			{
				/* Index Rollover Case */
				m_tail_index = m_max_counter_value - m_sliding_window_const + m_head_index;
			}

			if ( entry_index == 0U )
			{
				entry_index = m_tail_index;
			}

			if ( ( entry_index > m_max_counter_value ) ||
				 ( ( m_head_index >= m_tail_index ) &&
				   ( ( entry_index > m_head_index ) || ( entry_index < m_tail_index ) ) ) ||
				 ( ( m_head_index < m_tail_index ) && ( m_tail_index > entry_index ) &&
				   ( m_head_index < entry_index ) ) )
			{
				return_status = INVALID_INDEX;
			}
			else
			{
				while ( ( entries_to_read > 0U ) && ( loop_break == false ) )
				{
					sector_no = m_start_sector;
					absolute_index = ( entry_index - 1U ) % m_max_entries;

					/* Calculate absolute_index to find out that the entry lies in which sector */
					for ( sector_num_t i = m_start_sector;
						  ( i < m_last_sector ) && ( false == for_loop_break ); i++ )
					{
						if ( absolute_index >= m_max_entry_per_sector_array[i] )
						{
							absolute_index = absolute_index - m_max_entry_per_sector_array[i];
							sector_no++;
						}
						else
						{
							for_loop_break = true;
						}
					}
					for_loop_break = false;

					address = m_memory_block[sector_no].sector_start_addr +
						( absolute_index * m_entry_block_size );

					nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &index_counter ), address,
													  m_size_of_index );
					if ( ( entry_index == index_counter ) && ( nv_ret_status == NV_Ctrl::SUCCESS ) )
					{
						address = m_memory_block[sector_no].sector_start_addr +
							( absolute_index * m_entry_block_size ) + m_size_of_index;
						( *entry_count )++;

						nv_ret_status = Read_NV_Mem_Data( entry_data_ptr, address, m_entry_size );

						if ( nv_ret_status == NV_Ctrl::SUCCESS )
						{
							if ( m_use_protection == true )
							{
								Check_Data( entry_data_ptr, address );
							}

							entry_data_ptr = ( uint8_t* )( entry_data_ptr + m_entry_size );
							entry_index++;
							entries_to_read--;
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
					/*  Entry_index shows what was the last read location so next entry will start from
					    that point */
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

					/* There can be different different case that from which entry index how many
					   entries has been read according to that remaining entries calculated */
					if ( ( m_head_index <= m_sliding_window_const ) &&
						 ( m_tail_index < m_sliding_window_const ) )
					{
						*entries_to_head = m_head_index - entry_index;
					}
					else if ( ( m_head_index > m_sliding_window_const ) )
					{
						*entries_to_head = m_head_index - entry_index;
					}
					/* if head counter has rolled over */
					else if ( ( m_head_index < m_sliding_window_const ) &&
							  ( m_tail_index > m_sliding_window_const ) )
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
					// busy_delay_time = SECTOR_ERASE_TIME_LARGEST_MS;
					return_status = BUSY;
				}
			}
		}
		else
		{
			// busy_delay_time = SECTOR_ERASE_TIME_LARGEST_MS;
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
bool Log_Mem_NV::Check_Data( uint8_t* entry_data, uint32_t address )
{
	bool good_data = true;

	BF_Lib::SPLIT_UINT16 read_data;
	BF_Lib::SPLIT_UINT16 checksum;
	NV_Ctrl::nv_status_t nv_ret_status;

	read_data.u16 = 0U;
	Calculate_Checksum( entry_data, &( checksum.u16 ) );

	address += m_entry_size + m_size_of_index;
	nv_ret_status = Read_NV_Mem_Data( read_data.u8, address, SIZE_OF_CHECKSUM );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		if ( read_data.u16 != checksum.u16 )
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
void Log_Mem_NV::Calculate_Checksum( uint8_t* entry_data, uint16_t* checksum )
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
Log_Mem_NV::entry_index_t Log_Mem_NV::Num_Entries_Queued( void )
{
	return ( m_num_queued_entries );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Head_Index( void )
{
	return ( m_head_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Tail_Index( void )
{
	return ( m_tail_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::sector_num_t Log_Mem_NV::Get_Sector( log_mem_size_t addr )
{
	sector_num_t sector_counter = 0U;

	while ( ( sector_counter < m_total_sectors ) &&
			( ( ( addr >= m_memory_block[sector_counter].sector_start_addr ) &&
				( addr <= m_memory_block[sector_counter].sector_end_addr ) ) == false ) )
	{
		sector_counter++;
	}
	return ( sector_counter );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::sector_num_t Log_Mem_NV::Find_Tail_Sector( void )
{
	entry_index_t read_counter = 0U;
	sector_num_t tail_sector = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	if ( m_current_sector < m_last_sector )
	{
		tail_sector = m_current_sector + 1U;
	}
	else
	{
		tail_sector = m_start_sector;
	}

	nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &read_counter ),
									  m_memory_block[tail_sector].sector_start_addr,
									  m_size_of_index );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		if ( read_counter == MAX_VALUE )
		{
			tail_sector = m_start_sector;
		}
	}
	return ( tail_sector );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Find_Head_Index( void )
{
	entry_index_t counter_read = 0U;
	log_mem_size_t address = 0U;
	entry_index_t max_counter = 0U;
	entry_index_t min_counter = 0U;
	entry_index_t absolute_index = 0U;
	entry_index_t trace_counter = 0U;
	entry_index_t return_index = 0U;
	sector_num_t sector_no = 0U;
	bool loop_break = false;
	bool func_end = false;

	NV_Ctrl::nv_status_t nv_ret_status;

	address = m_memory_block[m_start_sector].sector_start_addr;

	while ( ( address < m_memory_block[sector_no].sector_end_addr ) &&
			( sector_no <= m_last_sector ) && ( loop_break == false ) )
	{
		nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &counter_read ), address, m_size_of_index );
		if ( nv_ret_status == NV_Ctrl::SUCCESS )
		{
			if ( counter_read == MAX_VALUE )
			{
				loop_break = true;
			}
			else
			{
				trace_counter++;
				max_counter = counter_read;
				absolute_index = max_counter;

				sector_no = m_start_sector;
				absolute_index = ( absolute_index ) % m_max_entries;

				/* To find out the absolute_index index lies in which sector */
				for ( sector_num_t i = m_start_sector; ( i < m_last_sector ); i++ )
				{
					if ( absolute_index >= m_max_entry_per_sector_array[i] )
					{
						absolute_index = absolute_index - m_max_entry_per_sector_array[i];
						sector_no++;
					}
				}
				address = m_memory_block[sector_no].sector_start_addr +
					( absolute_index * m_entry_block_size );

				if ( trace_counter >= m_max_entries )
				{
					/*If NV_Mem memory if completely full */
					max_counter = Find_Max_Counter();
					if ( ( max_counter == m_max_counter_value ) &&
						 ( m_start_sector != m_last_sector ) )	///< if counter reached to maxvalue
					{
						/* Find min val to get head and tail */
						min_counter = Find_Min_Counter();
						/* Counter rolled over so end of min counter sector will be head */
						if ( ( max_counter - min_counter ) > m_max_entries )
						{
							return_index = Find_Head_Counter();
							loop_break = true;
							func_end = true;
						}
					}
					else
					{
						max_counter++;
						return_index = max_counter;	///< else the max counter will be head
						loop_break = true;
						func_end = true;
					}
				}
			}
		}
		else
		{
			return_index = MAX_VALUE;	///< Invalid Index No
			loop_break = true;
			func_end = true;
		}
	}
	if ( func_end == false )
	{
		max_counter++;
		return_index = max_counter;
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Find_Max_Counter( void )
{
	entry_index_t counter_read = 0U;
	entry_index_t max_counter = 0U;
	log_mem_size_t address;

	NV_Ctrl::nv_status_t nv_ret_status;
	entry_index_t return_index = 0U;
	entry_index_t absolute_index = 0U;
	sector_num_t sector_no = 0U;
	bool loop_break = false;

	address = m_memory_block[m_start_sector].sector_start_addr;

	while ( ( address < m_memory_block[sector_no].sector_end_addr ) &&
			( m_current_sector <= m_last_sector ) && ( loop_break == false ) )
	{
		nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &counter_read ), address, m_size_of_index );
		if ( nv_ret_status == NV_Ctrl::SUCCESS )
		{
			if ( counter_read == m_max_counter_value )
			{
				return_index = m_max_counter_value;
				loop_break = true;
			}
			else if ( ( max_counter < counter_read ) && ( counter_read == MAX_VALUE ) )
			{
				max_counter++;
				return_index = max_counter;	///< else the max counter will be head
				loop_break = true;
			}
			else if ( max_counter >= counter_read )
			{
				return_index = max_counter;	///< else the max counter will be head
				loop_break = true;
			}
			else if ( max_counter < counter_read )
			{
				max_counter = counter_read;
			}

			if ( loop_break == false )
			{
				absolute_index = max_counter;
				sector_no = m_start_sector;
				absolute_index = ( absolute_index ) % m_max_entries;

				/* To find out the absolute_index index lies in which sector */
				for ( sector_num_t i = m_start_sector; i <= m_last_sector; i++ )
				{
					if ( absolute_index >= m_max_entry_per_sector_array[i] )
					{
						absolute_index = absolute_index - m_max_entry_per_sector_array[i];
						sector_no++;
					}
				}
				address = m_memory_block[sector_no].sector_start_addr +
					( absolute_index * m_entry_block_size );
			}
		}
		else
		{
			return_index = MAX_VALUE;	///< Invalid Index No
			loop_break = true;
		}
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Find_Head_Counter( void )
{
	entry_index_t counter_read = 0U;
	log_mem_size_t address = 0U;
	entry_index_t return_index = 0U;
	sector_num_t sector_no = 0U;
	bool loop_break = false;

	NV_Ctrl::nv_status_t nv_ret_status;
	uint32_t max_entry_in_current_sector = 0U;

	for ( sector_no = m_last_sector;
		  ( sector_no > m_start_sector ) && ( loop_break == false );
		  sector_no-- )
	{
		max_entry_in_current_sector = m_memory_block[sector_no - 1U].sector_size /
			m_entry_block_size;
		address = m_memory_block[sector_no - 1U].sector_start_addr +
			( max_entry_in_current_sector - 1U ) * m_entry_block_size;

		nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &counter_read ), address, m_size_of_index );
		if ( nv_ret_status == NV_Ctrl::SUCCESS )
		{
			if ( ( m_max_counter_value - counter_read ) > m_max_entries )
			{
				m_current_sector = sector_no - 1U;
				return_index = counter_read;
				loop_break = true;
			}
		}
		else
		{
			return_index = MAX_VALUE;	///< Invalid Index No
			loop_break = true;
		}
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Find_Min_Counter( void )
{
	entry_index_t min_index = 0U;
	log_mem_size_t address = 0U;
	entry_index_t return_index = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	m_current_sector = m_start_sector;
	address = m_memory_block[m_start_sector].sector_start_addr;

	nv_ret_status = Read_NV_Mem_Data( ( uint8_t* )( &min_index ), address, m_size_of_index );
	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		return_index = min_index;
	}
	else
	{
		return_index = MAX_VALUE;	///< Invalid Index No
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_NV::Erase_NV_Mem_Sector( sector_num_t sector_no )
{
#if defined( LOGGING_ON_EXT_FLASH ) || ( !defined( uC_STM32F407_USAGE ) && !defined( uC_STM32F411_USAGE ) && \
	!defined( uC_STM32F427_USAGE ) && !defined( uC_STM32F437_USAGE ) && \
	!defined( uC_STM32F767_USAGE ) && !defined( uC_STM32H743_USAGE ) )
	Log_Mem_NV::m_sector_erase_finish_status = m_nv_ctrl_log_mem->Erase(
		m_memory_block[sector_no].sector_start_addr,
		m_memory_block[sector_no].sector_end_addr - m_memory_block[sector_no].sector_start_addr + 1U,
		0xFFU, false );
#else
	Log_Mem_NV::m_sector_erase_finish_status = m_nv_ctrl_log_mem->Erase(
		m_memory_block[sector_no].sector_start_addr,
		m_memory_block[sector_no].sector_end_addr - m_memory_block[sector_no].sector_start_addr + 1U,
		Update_Erase_Finish_Status_Static, this );

#endif
	return ( Log_Mem_NV::m_sector_erase_finish_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* Log_Mem_NV::Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag )
{
	entry_index_t total_entries = 0U;
	sector_num_t largest_sector = 0U;

	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;
	log_mem_size_t address = 0U;
	sector_num_t sector_no = 0U;
	sector_num_t tail_sector = 0U;

	m_entry_size = entry_size;

	/* If Protection is enable then each entry will contain counter+entry data+checsum
	   so these three elements will contribute to each entry size */
	if ( m_use_protection == true )
	{
		m_entry_block_size = m_entry_size + m_size_of_index + SIZE_OF_CHECKSUM;
	}
	else
	{
		m_entry_block_size = m_entry_size + m_size_of_index;
	}

	/* Calculate maximum possible entries in the allocated memory sector */
	for ( sector_num_t counter = m_start_sector; counter <= m_last_sector; counter++ )
	{
		total_entries += ( m_memory_block[counter].sector_size / m_entry_block_size );
	}

	m_max_entries = total_entries;
	if ( m_max_entries > 0U )
	{
		m_max_counter_value = ( MAX_VALUE - ( MAX_VALUE % m_max_entries ) ) -
			m_max_entries;
	}
	/* Calculate Max Entries in Each Sector
	   Constant value taken as default assignment to find out minimum possible per page*/
	uint32_t min_sector_entries = MAX_SECTOR_ENTRY;

	for ( sector_num_t i = m_start_sector; i <= m_last_sector; i++ )
	{
		m_max_entry_per_sector_array[i] =
			( m_memory_block[i].sector_size / m_entry_block_size );

		if ( min_sector_entries < m_max_entry_per_sector_array[i] )
		{
			min_sector_entries = m_max_entry_per_sector_array[i];
		}

		m_all_sector_erase_time += m_memory_block[i].sector_erase_time;
	}
	/* Find the largest sector to calculate the sliding window size. */
	for ( sector_num_t i = m_start_sector; i < m_last_sector; i++ )
	{
		if ( m_memory_block[i].sector_size <= m_memory_block[i + 1U].sector_size )
		{
			largest_sector = i + 1U;
		}
	}

	m_sliding_window_const = m_max_entries - ( m_memory_block[largest_sector].sector_size
											   / m_entry_block_size );

	nv_ret_status = Check_Integrity( log_spec_ver_match_flag );	///< check if the memory used for logging is valid or
																///< not

	if ( nv_ret_status == NV_Ctrl::SUCCESS )
	{
		m_head_index = Find_Head_Index();	///< Read head index when device is powered up

		Find_Current_Sector();

		if ( m_head_index == MAX_VALUE )///< If Invalid Index No Received
		{
			m_head_index = START_INDEX;
		}

		m_entry_cluster = reinterpret_cast<uint8_t*>( Ram::Allocate( m_entry_block_size ) );
		if ( min_sector_entries > m_backup_num_events )
		{
			m_backup_num_events = min_sector_entries;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Total backup entries are more than minimum entries possible in a page. Truncating the value equal to min_sector_entries i.e. %d",
						   min_sector_entries )
		}
		m_entry_data_saved = ( reinterpret_cast<uint8_t*>( Ram::Allocate( m_entry_size * m_backup_num_events ) ) );

		/* Read Tail Sector */
		tail_sector = Find_Tail_Sector();
		sector_no = tail_sector;
		address = m_memory_block[sector_no].sector_start_addr;
		nv_ret_status = Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &m_tail_index ) ), address, m_size_of_index );

		if ( nv_ret_status == NV_Ctrl::SUCCESS )
		{
			/* If NV_Mem memory is completely empty. */
			if ( ( m_head_index == START_INDEX ) && ( ( m_tail_index == MAX_VALUE ) || ( m_tail_index == 0U ) ) )
			{
				m_tail_index = m_head_index;
			}

			if ( ( m_head_index > m_sliding_window_const ) &&
				 ( m_tail_index < m_sliding_window_const ) ||
				 ( m_head_index > m_sliding_window_const ) &&
				 ( m_tail_index > m_sliding_window_const ) )
			{
				m_tail_index = m_head_index - m_sliding_window_const;
			}
			else if ( ( m_head_index < m_sliding_window_const ) &&
					  ( m_tail_index > m_sliding_window_const ) )
			{
				/* Index Rollover Case */
				m_tail_index = m_max_counter_value - m_sliding_window_const + m_head_index;
			}
			else
			{
				// for misra
			}

			if ( m_head_index >= m_tail_index )
			{
				m_num_queued_entries = m_head_index - m_tail_index;
			}
			else
			{
				m_num_queued_entries = m_max_counter_value - m_tail_index + m_head_index;	// maximum entries
			}
		}
		else
		{
			m_num_queued_entries = 0U;
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
void Log_Mem_NV::Find_Current_Sector()
{
	entry_index_t absolute_index = 0U;
	sector_num_t sector_no = 0U;
	bool for_loop_break = false;

	absolute_index = m_head_index;

	sector_no = m_start_sector;

	while ( absolute_index > m_max_entries )
	{
		absolute_index -= m_max_entries;
	}
	/* Find out the absolute_index to check that the index lies in which sector */
	for ( sector_num_t i = m_start_sector;
		  ( i <= m_last_sector ) && ( false == for_loop_break );
		  i++ )
	{
		if ( absolute_index > m_max_entry_per_sector_array[i] )
		{
			absolute_index = absolute_index - m_max_entry_per_sector_array[i];
			sector_no++;
		}
		else
		{
			for_loop_break = true;
		}
	}
	m_current_sector = sector_no;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_size_t Log_Mem_NV::Get_Entry_Size( void )
{
	return ( m_entry_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_NV::Write_NV_Mem_Data( uint8_t* data, uint32_t address,
													uint32_t length )
{
	uint8_t retry_count = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	do
	{
		nv_ret_status = m_nv_ctrl_log_mem->Write( data, address, length, false );
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
NV_Ctrl::nv_status_t Log_Mem_NV::Read_NV_Mem_Data( uint8_t* data, uint32_t address,
												   uint32_t length )
{
	uint8_t retry_count = 0U;

	NV_Ctrl::nv_status_t nv_ret_status;

	do
	{
		nv_ret_status = m_nv_ctrl_log_mem->Read( data, address, length, false );
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
void Log_Mem_NV::NV_Mem_Operation_Handler( void )
{
	if ( ( m_clear_all_enable == true ) &&
		 ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
		 ( m_erased_sector_num < m_total_sectors ) )
	{
		Log_Mem_NV::m_sector_erase_finish_status = Erase_NV_Mem_Sector( m_erased_sector_num );
		m_erased_sector_num++;
	}

	if ( ( m_clear_all_enable == true ) && ( m_erased_sector_num >= m_total_sectors ) &&
		 ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) )		/*All sectors erased*/
	{
		m_clear_all_enable = false;
		Log_Mem_NV::m_all_sector_erase_finish_status = NV_Ctrl::SUCCESS;
		m_head_index = Find_Head_Index();
		if ( m_clear_log_event_cb != nullptr )
		{
			/* Execute the callback for capturing the log memory clear events */
			( *m_clear_log_event_cb )( m_log_index );
		}
	}

	if ( ( m_delayed_enque_flag == true ) && ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
		 ( Log_Mem_NV::m_all_sector_erase_finish_status == NV_Ctrl::SUCCESS ) )
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
	if ( m_current_sector_erase_flag == true )
	{
		Erase_NV_Mem_Sector( m_current_sector );
		m_current_sector_erase_flag = false;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Log_Mem_NV::entry_index_t Log_Mem_NV::Get_Max_Entry_Count( void )
{
	return ( m_sliding_window_const );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_NV::Check_Integrity( bool log_spec_ver_match_flag )
{
	entry_index_t index = 0U;
	entry_index_t next_index = 0U;
	log_mem_size_t index_address = 0U;
	entry_index_t index_data = 0U;
	entry_index_t max_entries_lastsector = 0U;
	log_mem_size_t start_add_lastsector = 0U;
	entry_index_t last_index_lastsector = 0U;
	entry_index_t filled_data = 0U;
	entry_index_t blank_length = 0U;
	entry_index_t blank_check_length = 0U;
	sector_num_t erased_sector_num = 0U;
	bool loop_break = false;
	bool mem_clear_log_event = false;
	uint8_t CHUNK[READ_CHUNK_LENGTH] = {0};
	NV_Ctrl::nv_status_t nv_ret_status = NV_Ctrl::SUCCESS;

	/* Check log spec version status before log mem integrity check */
	if ( log_spec_ver_match_flag == true )
	{
		/* Implementation to check if the memory used for logging is valid or not */
		for ( sector_num_t i = m_start_sector; i < m_total_sectors; i++ )
		{
			index_address = m_memory_block[i].sector_start_addr;
			/* Read First Logging Index */
			nv_ret_status =
				Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &index ) ), index_address, m_size_of_index );
			if ( nv_ret_status == NV_Ctrl::SUCCESS )
			{
				index_address += m_entry_block_size;

				/* Read Next Logging Index */
				nv_ret_status =
					Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &next_index ) ), index_address, m_size_of_index );

				if ( nv_ret_status == NV_Ctrl::SUCCESS )
				{
					/* Sector Rollover */
					if ( ( next_index == DEFAULT_VALUE ) && ( index > 1U ) && ( index <= m_max_counter_value ) &&
						 ( i == 0U ) )
					{
						/* Find last sector last index */
						max_entries_lastsector = m_max_entry_per_sector_array[m_total_sectors - 1U];
						start_add_lastsector = m_memory_block[m_total_sectors - 1U].sector_start_addr;
						if ( nv_ret_status ==
							 Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &last_index_lastsector ) ),
											   start_add_lastsector +
											   ( m_entry_block_size * ( max_entries_lastsector - 1U ) ),
											   m_size_of_index ) )
						{
							/* Difference must be 1U */
							if ( ( index - last_index_lastsector ) != 1U )
							{
								Log_Mem_NV::m_sector_erase_finish_status = Erase_NV_Mem_Sector_Synchronous( i );
								if ( Log_Mem_NV::m_sector_erase_finish_status != NV_Ctrl::SUCCESS )
								{
									nv_ret_status = NV_Ctrl::BUSY_ERROR;
								}
								else
								{
									/* Capture clear event flag */
									mem_clear_log_event = true;
								}
							}
						}
						else
						{
							nv_ret_status = NV_Ctrl::DATA_ERROR;
						}
					}

					/* Check index value with FF and maximum counter*/
					if ( ( index != DEFAULT_VALUE ) && ( index <= m_max_counter_value ) &&
						 ( nv_ret_status == NV_Ctrl::SUCCESS ) )
					{
						/* Check index and next index value with FF and index value with next index & Index Rollover*/
						while ( ( ( index != DEFAULT_VALUE ) && ( next_index != DEFAULT_VALUE ) &&
								  ( index + 1U == next_index ) &&
								  ( loop_break == false ) && ( nv_ret_status == NV_Ctrl::SUCCESS ) ) ||
								( ( index - next_index ) == ( m_max_counter_value - 1U ) ) )
						{
							index_address += m_entry_block_size;
							if ( ( index_address >= m_memory_block[i].sector_start_addr ) &&
								 ( index_address < m_memory_block[i].sector_end_addr ) )
							{
								index_data = 0U;
								if ( nv_ret_status ==
									 Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( &index_data ) ), index_address,
													   m_size_of_index ) )
								{
									index = next_index;
									next_index = 0U;
									next_index = index_data;
								}
								else
								{
									nv_ret_status = NV_Ctrl::DATA_ERROR;
								}
							}
							else
							{
								loop_break = true;
							}
						}
					}
					else
					{
						index_address = m_memory_block[i].sector_start_addr;
					}


					/* Read chunk and Do a blank check for rest of the memory of this sector. if found non ff
					   then memory is corrupt. proceed for erase */
					if ( index != DEFAULT_VALUE )
					{
						/* Find filled data of last */
						filled_data = index_address - m_memory_block[i].sector_start_addr;
						/* Find number of blank location  */
						blank_check_length = m_memory_block[i].sector_size - filled_data;
						while ( ( loop_break == false ) && ( nv_ret_status == NV_Ctrl::SUCCESS ) &&
								( blank_check_length <= m_memory_block[i].sector_size ) &&
								( blank_check_length != 0U ) )
						{
							if ( ( index_address >= m_memory_block[i].sector_start_addr ) &&
								 ( index_address <= ( m_memory_block[i].sector_end_addr ) ) )
							{
								if ( blank_check_length > READ_CHUNK_LENGTH )
								{
									blank_check_length = READ_CHUNK_LENGTH;
								}

								if ( nv_ret_status ==
									 Read_NV_Mem_Data( ( reinterpret_cast<uint8_t*>( CHUNK ) ), index_address,
													   blank_check_length ) )
								{
									for ( uint16_t j = 0; j < blank_check_length; j++ )
									{
										if ( CHUNK[j] != 0xFFU )
										{
											Log_Mem_NV::m_sector_erase_finish_status =
												Erase_NV_Mem_Sector_Synchronous( i );
											if ( Log_Mem_NV::m_sector_erase_finish_status != NV_Ctrl::SUCCESS )
											{
												nv_ret_status = NV_Ctrl::BUSY_ERROR;
											}
											else
											{
												/* Capture clear event flag */
												mem_clear_log_event = true;
											}
											loop_break = true;
											break;
										}
									}

									index_address += blank_check_length;
									blank_length += blank_check_length;

									blank_check_length = m_memory_block[i].sector_size - ( filled_data + blank_length );
									if ( blank_check_length <= 0U )
									{
										blank_check_length = ( filled_data + blank_length + 1U ) -
											m_memory_block[i].sector_size;
									}

									if ( index_address > m_memory_block[i].sector_end_addr )
									{
										index_address = m_memory_block[i].sector_end_addr - blank_check_length;
									}
								}
								else
								{
									nv_ret_status = NV_Ctrl::DATA_ERROR;
								}
							}
							else
							{
								loop_break = true;
							}
						}
					}
					else
					{
						index_address = m_memory_block[i].sector_start_addr;
					}
				}
				else
				{
					nv_ret_status = NV_Ctrl::DATA_ERROR;
				}
			}
			else
			{
				nv_ret_status = NV_Ctrl::DATA_ERROR;
			}
		}
		/* Capture event in user audit log after erasing the specific logging */
		if ( mem_clear_log_event == true )
		{
			/* if sector discontinuity are present, no use of data
			   because it will not show in webUI in some cases */
			while ( ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
					( erased_sector_num < m_total_sectors ) )
			{
				Log_Mem_NV::m_sector_erase_finish_status =
					Erase_NV_Mem_Sector_Synchronous( erased_sector_num );
				erased_sector_num++;
			}
			if ( m_mem_integrity_log_audit_capture_cb != nullptr )
			{
				/* Execute the callback for capturing the log memory clear events */
				( *m_mem_integrity_log_audit_capture_cb )( m_log_index );
			}
		}
	}
	else
	{
		/* if log spec version found mismatch (change in log entry), no use of data proceed for erase */
		while ( ( Log_Mem_NV::m_sector_erase_finish_status == NV_Ctrl::SUCCESS ) &&
				( erased_sector_num < m_total_sectors ) )
		{
			Log_Mem_NV::m_sector_erase_finish_status =
				Erase_NV_Mem_Sector_Synchronous( erased_sector_num );
			erased_sector_num++;
		}
		if ( m_mem_integrity_log_audit_capture_cb != nullptr )
		{
			/* Execute the callback for capturing the log memory clear events */
			( *m_mem_integrity_log_audit_capture_cb )( m_log_index );
		}
	}
	return ( nv_ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Log_Mem_NV::Erase_NV_Mem_Sector_Synchronous( sector_num_t sector_no )
{
	Log_Mem_NV::m_sector_erase_finish_status = m_nv_ctrl_log_mem->Erase(
		m_memory_block[sector_no].sector_start_addr,
		m_memory_block[sector_no].sector_end_addr - m_memory_block[sector_no].sector_start_addr + 1U,
		0xFFU, false );

	return ( Log_Mem_NV::m_sector_erase_finish_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Log_Mem_NV::Attach_Log_Mem_Integrity_Failure_Callback(
	Mem_Integrity_Audit_Function_Decl_1 mem_integrity_audit_log_cb )
{
	m_mem_integrity_log_audit_capture_cb = mem_integrity_audit_log_cb;
}

}/* end namespace BF_Misc for this module */
