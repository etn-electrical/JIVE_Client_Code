/**
 *****************************************************************************************
 *	@file Log_Mem.h
 *
 *	@brief A file which contains a part of the Log functionality. Module responsible
 *	for the queue memory. This is a purely virtual class. Associated modules
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
#ifndef LOG_MEM_H
	#define LOG_MEM_H

#include "Log_Events.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief This class is responsible for handling the actual queue functionality which is
 *  appropriate for the type of memory it is applied to.
 *  @details The basic function of a log is to take an entry and put it into a list in a first
 *  in order.  Each memory type (RAM, Flash, EEPROM, FRAM) requires a unique method of tracking the
 *  incremental entries.  This virtual class tries to capture the most common functions of a log
 *  queue.
 **************************************************************************************************
 */
class Log_Mem
{
	public:
		/**
		 * @brief An entry_index value type keeps track of whole blocks not single entities.
		 * This is not byte oriented but indexed by entry block.  Index into entries - not entities.
		 */
		typedef uint32_t entry_index_t;

		/**
		 * @brief An entry_size value which is logged at single event.
		 */
		typedef uint16_t entry_size_t;

		/**
		 * @brief An log_mem_size_t memory size and/or mem pointer .
		 * This is not byte oriented but indexed by entry block.  Index into entries - not entities.
		 */
		typedef uint32_t log_mem_size_t;

		/**
		 * @brief This is a way for the log_mem to return an error.  It is not an enum so that
		 * the specific log mems can create their own errors without causing type issues.
		 * Standard errors are defined below.
		 */
		typedef uint8_t error_t;

		/**
		 * @brief One of the common defined errors which can be returned by the log_mem
		 * implementations.
		 * Error: Indicates success or no error occurred.
		 */
		static const error_t NO_ERROR = 0U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Indicates the log command value is incorrect.
		 */
		static const error_t INVALID_CMD = 1U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Error: A get request index is out of bounds of the current index space.
		 * This is likely due to trying to keep up with the log by utilizing the return points
		 * and then calling get often enough to keep up with the number of entries being put in.
		 */
		static const error_t INVALID_INDEX = 2U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Indicates the log mem is busy.  It should be accessed again.  The operation
		 * was not successful.
		 */
		static const error_t BUSY = 3U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Indicates the log mem is busy and time after It should be accessed again.
		 */
		static const error_t BUSY_TIME = 4U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Indicates the log integrity check fail.
		 */
		static const error_t MEMORY_FAIL = 5U;

		/**
		 * @brief A common defined error which can be returned by a log_mem implementation.
		 * Indicates the RAM log Buffer has been full, not able to accept any logs if Enqueue
		 * status BUSY.
		 */
		static const error_t MEMORY_REJECT = 6U;

		/**
		 * @brief For a Clear function if entry_ctr is 0, whole log memory will be cleared.
		 */
		static const entry_index_t CLEAR_ALL = 0U;

		/**
		 * @brief 32 bit max value.
		 */
		static const entry_index_t MAX_VALUE = 0xFFFFFFFFU;

		/**
		 * @brief 32 bit default value .
		 */
		static const entry_index_t DEFAULT_VALUE = 0xFFFFFFFFU;

		/**
		 * @brief 32 bit read external flash sector in chunk and read chunk lenght is 256.
		 */
		static const entry_size_t READ_CHUNK_LENGTH = 256U;

		/**
		 * @brief Index starts at 1.
		 */
		static const entry_index_t START_INDEX = 1U;

		/**
		 * @brief default backup event count to save the event in the buffer.
		 */
		static const uint8_t BACKUP_NUM_EVENTS = 1U;

		/**
		 *  @brief Constructor for the purely virtual class.
		 *  @details Example constructor for a real instance of a log mem interface.
		 *  @n @b Log_Mem_FRAM( NV_Ctrl* nv_mem_ctrl, uint32_t start_address, uint16_t
		 *  entry_size, entry_index_t num_entries );
		 */
		Log_Mem( void ){}

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		virtual ~Log_Mem( void );

		/**
		 *  @brief Takes the entry blob passed in and writes it to the queue.
		 *  @details This function should be able to exit quickly.  A target time of
		 *  less than 250us.  This would allow the full operation to be done on a
		 *  RAM queue or an FRAM Queue.  This would typically not allow someone to operate
		 *  on a flash based queue storage.  This is of course up to the adopters to decide
		 *  ultimately what fast is.
		 */
		virtual error_t Enque( uint8_t* entry_data ) = 0;

		/**
		 *  @brief Clears the log memory.
		 *	@param entry_ctr:Clears entries from entry_ctr to tail. Will move tail.
		 *	if entry_ctr is 0, clear all log memmory.
		 *  @param deleted_qty: Actual deleted entries.
		 *  @details
		 */
		virtual error_t Clear( entry_index_t entry_ctr = CLEAR_ALL,
							   entry_index_t* deleted_qty = nullptr ) = 0;

		/**
		 *  @brief The number of entries actually queued.  If this is a circular queue and is
		 *  full, the num entries should equal the total entries allowed.
		 */
		virtual entry_index_t Num_Entries_Queued( void ) = 0;

		/**
		 *  @brief Indicates what index the head is at.
		 */
		virtual entry_index_t Head_Index( void ) = 0;

		/**
		 *  @brief Indicates what index the tail is at.
		 */
		virtual entry_index_t Tail_Index( void ) = 0;

		/**
		 * @brief
		 * @param entry_index: The entry index into the queue.  This number will continue to
		 * rotate and is really an internal concept used for maintaining the queue.  Typically this
		 * should be a free running counter which eventually will overflow.  It should be used as
		 * almost a record lookup id.  It is an index for a whole entry NOT an entity.
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
		 * @param dest_buff_data_size: Protocol with send Max Size available for data in this
		 * variable.
		 * @return The success or failure cause.  This may include an overrun.
		 */
		virtual error_t Get_Entry( entry_index_t entry_index, entry_index_t* entry_count,
								   uint8_t* entry_data, entry_index_t* next_entry,
								   entry_index_t* entries_to_head,
								   entry_size_t dest_buff_data_size = 0U ) = 0;

		/**
		 *  @brief Gets Entry_size from Log_DCI and allocate array.
		 *  Also confiugures all the clase variables like, head index,
		 *  entry_block_size, etc.
		 *	@param entry_size: entry_size.
		 *      @param log_spec_ver_match_flag: this flag indicates if log spec version match or not.
		 *	@return Pointer of the entry_cluster.
		 */
		virtual uint8_t* Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag = true ) = 0;

		/**
		 *  @brief Read the entry size.
		 */
		virtual entry_size_t Get_Entry_Size( void ) = 0;

		/**
		 *  @brief Read the maximum entries possible.
		 */
		virtual entry_index_t Get_Max_Entry_Count( void ) = 0;

		/**
		 *  @brief Read the current log index.
		 */
		virtual log_id_t Log_Index( void ) = 0;

	private:
		Log_Mem( const Log_Mem& object );		///< Private copy constructor
		Log_Mem & operator =( const Log_Mem& object );		///< Private copy assignment operator

};

}
#endif	/* end namespace BF_Misc for this module */

