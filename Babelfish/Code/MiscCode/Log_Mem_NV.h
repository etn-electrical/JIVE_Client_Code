/**
 *****************************************************************************************
 *	@file Log_Mem_NV.h
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
#ifndef LOG_MEM_NV_H
	#define LOG_MEM_NV_H

#include "Log_Mem.h"
#include "CR_Tasker.h"
#include "NV_Ctrl.h"

namespace BF_Misc
{

/**
 * @brief callback for mem integrity check event
 */
typedef void (* Mem_Integrity_Audit_Function_Decl_1 ) ( log_event_t event_code );

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

class Log_Mem_NV : public Log_Mem
{
	public:

		static const uint8_t MAX_SECTOR_ENTRY = 32U;
		/**
		 * @brief callback for log mem event
		 */
		typedef void (* log_mem_cb_t)( log_id_t log_index );

		/**
		 * @brief Sector number of the flash block.
		 */
		typedef uint8_t sector_num_t;

		/**
		 * @brief Address and size information of each sector
		 */
		typedef struct
		{
			sector_addr_t sector_start_addr;
			sector_addr_t sector_end_addr;
			sector_addr_t sector_size;
			uint32_t sector_erase_time;
		} memory_block_t;

		/**
		 *  @brief Constructor for the Log data logging in to the Flash.
		 *  @param log_index : Log Id
		 *  @param nv_ctrl_log_mem : Log NV_Ctrl pointer
		 *  @param memory_block: Log's sector start, end address and size information for
		 *  logging
		 *	@param total_sectors: Total sectors of the memory block.
		 *	@param use_protection: Enable check_sum.
		 *	@param clear_event_cb: Callback for capturing clear log memory event.
		 *	@param backup_num_events : Number of event saved in temp buffer logs if status busy.
		 */

		Log_Mem_NV( log_id_t log_index, NV_Ctrl* nv_ctrl_log_mem, const memory_block_t* memory_block,
					sector_num_t total_sectors, bool_t use_protection = false,
					log_mem_cb_t clear_log_event_cb = nullptr, uint8_t backup_num_events = BACKUP_NUM_EVENTS );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~Log_Mem_NV( void ){}

		/**
		 *  @brief Takes the entry blob passed in and writes it to the queue.
		 *  @details This function should be able to exit quickly.  A target time of
		 *  less than 250us.  This would allow the full operation to be done on a
		 *  RAM queue or an FRAM Queue.  This would typically not allow someone to operate
		 *  on a flash based queue storage.  This is of course up to the adopters to decide
		 *  ultimately what fast is.
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
		 * @return The success or failure cause. This may include an overrun.
		 */
		error_t Get_Entry( entry_index_t entry_index, entry_index_t* entry_count,
						   uint8_t* entry_data, entry_index_t* next_entry,
						   entry_index_t* entries_to_head,
						   entry_size_t dest_buff_data_size = 0U );

		/**
		 *  @brief Gets Entry_size from Log_DCI and allocate array.
		 *  Also confiugures all the clase variables like, head index,
		 *  entry_block_size, etc.
		 *	@param entry_size: entry_size.
		 *      @param log_spec_ver_match_flag: this flag indicates if log spec version match or not.
		 *	@return Pointer of the entry_cluster.
		 */
		uint8_t* Log_Mem_Config( entry_size_t entry_size, bool log_spec_ver_match_flag = true );

		/**
		 *  @brief ead the current sector no to perform read or write operation.
		 */
		void Find_Current_Sector();

		/**
		 *  @brief Read the entry size.
		 */
		entry_size_t Get_Entry_Size( void );


		/**
		 *  @brief Get the handle of the NV memory instance.
		 *  erase sector operation is completed.
		 *	@param nv_ctrl_log_mem: NV_Ctrl instance handle where erase, write, read
		 *  operations are performed.
		 */
		void Update_NV_Ctrl( NV_Ctrl* nv_ctrl_log_mem );

		/**
		 *  @brief Read the maximum entries possible.
		 */
		entry_index_t Get_Max_Entry_Count( void );

		/**
		 *  @brief Read the current log index in the list
		 */
		log_id_t Log_Index( void )
		{
			return ( m_log_index );
		}

		/**
		 * @brief Attach Log Mem Integrity Failure Callback.
		 * @details Callback to get index id of the log type for erased log while log memory interity check
		 * @return this.
		 */
		static void Attach_Log_Mem_Integrity_Failure_Callback(
			Mem_Integrity_Audit_Function_Decl_1 mem_integrity_audit_log_cb );

	private:
		Log_Mem_NV( const Log_Mem_NV& object );		///< Private copy constructor
		Log_Mem_NV & operator =( const Log_Mem_NV& object );		///< Private copy assignment

		entry_index_t Find_Head_Index( void );

		entry_index_t Find_Max_Counter( void );

		entry_index_t Find_Min_Counter( void );

		entry_index_t Find_Head_Counter( void );

		sector_num_t Find_Tail_Sector( void );

		sector_num_t Get_Sector( uint32_t addr );

		NV_Ctrl::nv_status_t Erase_NV_Mem_Sector( sector_num_t sector_no );

		void Calculate_Checksum( uint8_t* entry_data, uint16_t* checksum );

		bool_t Check_Data( uint8_t* entry_data, uint32_t address );

		NV_Ctrl::nv_status_t Write_NV_Mem_Data( uint8_t* data, uint32_t address,
												uint32_t length );

		NV_Ctrl::nv_status_t Read_NV_Mem_Data( uint8_t* data, uint32_t address,
											   uint32_t length );

		error_t Push_To_NV_Mem( uint8_t* entry_data_final, log_mem_size_t address );


		/**
		 * @brief Handles the erase command in cr task.
		 */
		void NV_Mem_Operation_Handler();

		/*
		 * @ brief Handle the NV operations in CR Tasker .
		 */
		static void NV_Mem_Operation_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( reinterpret_cast<Log_Mem_NV*>( handle ) )->NV_Mem_Operation_Handler();
		}

		/**
		 *  @brief This function will be called by the uC_Flash/or any other NV callback when
		 *  erase sector operation is completed.
		 *	@param result: uC_Flash callback will return success or error status.
		 */
		void Update_Erase_Finish_Status( NV_Ctrl::nv_status_t result );

		/*
		 *  @brief This static function will be called by the uC_Flash/or any other NV callback
		 *	@param func_param: uC_Flash callback function .
		 */
		static void Update_Erase_Finish_Status_Static( NV_Ctrl::cback_param_t func_param,
													   NV_Ctrl::nv_status_t result )
		{
			m_sector_erase_finish_status = result;
		}

		/**
		 *  @brief check if the memory used for logging is valid or not.
		 *  check mismatch of LOGGING first index and next index and perform the blank check with FF.
		 *  check rollover for sector and index and handle it properly.
		 *  if any mismatch found, proceed for erase of current sector.
		 *  generate clear log event in user audit log.
		 *      @param log_spec_ver_match_flag: this flag indicates if log spec version match or not
		 */
		NV_Ctrl::nv_status_t Check_Integrity( bool log_spec_ver_match_flag );

		/**
		 *  @brief function used to erase sector for external and internal flash
		 *      @param sector_no: sector number which will be erased
		 */
		NV_Ctrl::nv_status_t Erase_NV_Mem_Sector_Synchronous( sector_num_t sector_no );

		static const uint8_t SIZE_OF_CHECKSUM = 2U;
		static const uint8_t NV_MEM_OPERATION_RETRY = 3U;

		uint8_t m_size_of_index;
		sector_num_t m_start_sector;
		sector_num_t m_last_sector;

		sector_num_t m_tail_sector;
		sector_num_t m_current_sector;

		sector_num_t m_erased_sector_num;
		entry_index_t m_num_queued_entries;

		entry_index_t m_head_index;
		entry_index_t m_tail_index;

		uint32_t m_total_log_nv_mem;

		entry_index_t m_sliding_window_const;
		entry_size_t m_entry_size;
		entry_size_t m_entry_block_size;

		entry_index_t m_max_entries;
		entry_index_t m_max_counter_value;
		uint8_t* m_entry_cluster;
		uint8_t* m_entry_data_saved;
		bool_t m_use_protection;
		const memory_block_t* m_memory_block;
		NV_Ctrl* m_nv_ctrl_log_mem;
		entry_index_t* m_max_entry_per_sector_array;
		sector_num_t m_total_sectors;
		static NV_Ctrl::nv_status_t m_sector_erase_finish_status;
		static NV_Ctrl::nv_status_t m_all_sector_erase_finish_status;

		bool_t m_clear_all_enable;
		bool_t m_delayed_enque_flag;
		log_mem_size_t m_delayed_enque_address;
		uint32_t m_all_sector_erase_time;
		log_mem_cb_t m_clear_log_event_cb;
		log_id_t m_log_index;
		bool_t m_current_sector_erase_flag;
		error_t m_mem_integrity_status;
		uint8_t m_backup_num_events;
		uint8_t m_backup_entry_saved;
		uint8_t m_backup_next_index;
};

}
#endif	/* End namespace BF_Misc for this module */

