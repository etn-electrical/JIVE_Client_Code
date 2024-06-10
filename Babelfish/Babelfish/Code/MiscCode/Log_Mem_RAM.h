/**
 *****************************************************************************************
 *	@file Log_Mem_RAM.h
 *
 *	@brief A file which contains a part of the Log functionality.  Module responsible
 *	for the queue memory.  This is a purely virtual class.  Associated modules
 *	will need to be actually created to meet the requirements defined below.
 *
 *	@n @b Definitions:
 *  @li Entity = A value contained in the DCI database.
 *	@li Entry = A single blob in the log.  Can contain one or multiple entity values.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOG_MEM_RAM_H
	#define LOG_MEM_RAM_H

#include "Log_Mem.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief This class is responsible for handling the actual queue functionality which is
 *  appropriate for the type of memory it is applied to.
 *  @details The basic function of a log is to take an entry and put it into a list in a first
 *  in order.  Each memory type (RAM, Flash, EEPROM, FRAM) requires a unique method of tracking
 *  the incremental entries.  This virtual class tries to capture the most common functions of a log
 *  queue.
 **************************************************************************************************
 */

class Log_Mem_RAM : public Log_Mem
{
	public:

		/**
		 * @brief callback for log mem event
		 */
		typedef void (* log_mem_cb_t)( log_id_t m_log_index );

		/**
		 *  @brief Constructor for the data logging on RAM.
		 *  @brief log_index: Log Id
		 *  @param log_data_buffer: Pointer to log buffer.
		 *  @param log_data_buf_size: Size of log buffer allocated by application
		 *	@param clear_log_event_cb: Callback for capturing clear log memory event.
		 */
		Log_Mem_RAM( log_id_t log_index, uint8_t* log_data_buffer, log_mem_size_t log_data_buf_size,
					 log_mem_cb_t clear_log_event_cb = nullptr );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~Log_Mem_RAM( void );

		/**
		 *  @brief Takes the entry blob passed in and writes it to the queue.
		 *  @details This function should be able to exit quickly.  A target time of
		 *  less than 250us.  This would allow the full operation to be done on a
		 *  RAM queue or an FRAM Queue.  This would typically not allow someone to operate
		 *  on a flash based queue storage.  This is of course up to the adopters to decide
		 *  ultimately what fast is.
		 *  @param[in] entry_data: Pointer to data to be entered in a queue.
		 */
		error_t Enque( uint8_t* entry_data );

		/**
		 *  @brief Clears the log memory.
		 *	@param entry_ctr:Clears entries from entry_ctr to tail. Will move tail.
		 *	if entry_ctr is 0, clear all log memmory.
		 *  @param deleted_qty: Actual deleted entries.
		 *  @details
		 */
		error_t Clear( entry_index_t entry_ctr = CLEAR_ALL, entry_index_t* deleted_qty = nullptr );

		/**
		 *  @brief The number of entries actually queued.  If this is a circular queue and is
		 *  full, the num entries should equal the total entries allowed.
		 */
		entry_index_t Num_Entries_Queued( void );

		/**
		 *  @brief Indicates what index the head is at.
		 */
		entry_index_t Head_Index( void );

		/**
		 *  @brief Indicates what index the tail is at.
		 */
		entry_index_t Tail_Index( void );

		/**
		 * @brief Get the log entry
		 * @param entry_index: The entry index into the queue.  This number will continue to
		 * rotate and is really an internal concept used for maintaining the queue. Typically
		 * this should be a free running counter which eventually will overflow.  It should be used
		 * as almost a record lookup id.  It is an index for a whole entry NOT an entity.
		 * The data will be loaded into the entry data one entry at a time starting with the
		 * entry_index number and counting up.  It is not advised to guess the next entry index.
		 * The data will be loaded tail to head.  Entries closest to the tail will be loaded
		 * first and moved closer to the head.
		 * @param entry_count: The number of entries to retrieve.
		 * @param entry_data: A pointer to the data destination.
		 * @param next_entry: Returned back to caller to indicate where to start the retrieval
		 * on next request so as to not lose an entry.
		 * @param entries_to_head: Returned back to caller to indicate how many entries are
		 * still left in the queue for retrieval.
		 * @param busy_delay_time: Time for client to wait for flash ( eg. erase ) operation.
		 * @param dest_buff_data_size: Protocol with send Max Size available for data in this
		 * variable.
		 * @return The success or failure cause.  This may include an overrun.
		 */
		error_t Get_Entry( entry_index_t entry_index, entry_index_t* entry_count,
						   uint8_t* entry_data, entry_index_t* next_entry,
						   entry_index_t* entries_to_head,
						   entry_size_t dest_buff_data_size = 0U );

		/**
		 *  @brief	Gets Entry_size from Log_DCI and allocate array.
		 *  Also confiugures all the clase variables like, head index,
		 *  entry_block_size, etc.
		 *	@param entry_size: entry_size.
		 *      @param log_spec_ver_match_flag: this flag indicates if log spec version match or not.
		 *	@return Pointer of the entry_cluster.
		 */
		uint8_t* Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag = true );

		/**
		 *  @brief Read the entry size.
		 */
		entry_size_t Get_Entry_Size( void );

		/**
		 *  @brief Read the maximum entries possible.
		 */
		entry_index_t Get_Max_Entry_Count( void );


		/**
		 *  @brief Read the log index of the current log.
		 */
		log_id_t Log_Index( void )
		{
			return ( m_log_index );
		}

	private:
		Log_Mem_RAM( const Log_Mem_RAM& object );		///< Private copy constructor
		Log_Mem_RAM & operator =( const Log_Mem_RAM& object );		///< Private copy assignment

		// operator
		bool_t m_overflow_flag;
		bool_t m_index_reset_flag;
		uint8_t* m_log_data_buffer;
		entry_index_t m_max_entry_count;
		log_mem_size_t m_log_data_buf_size_aligned;
		log_mem_size_t m_log_data_buf_size;
		log_mem_size_t m_cur_mem_loc;
		entry_index_t m_entry_size;
		entry_index_t m_max_entry;
		entry_index_t m_log_head_index;
		entry_index_t m_log_tail_index;
		entry_index_t m_num_queued_entries;
		entry_index_t m_max_head_tail_index;
		uint8_t* m_entry_cluster;
		bool_t m_clear_enable;
		log_id_t m_log_index;
		log_mem_cb_t m_clear_log_event_cb;

};

}
#endif	/* end namespace BF_Misc for this module */

