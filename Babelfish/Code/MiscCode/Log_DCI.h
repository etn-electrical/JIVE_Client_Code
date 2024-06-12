/**
 *****************************************************************************************
 *	@file Log_DCI.h
 *
 *	@brief A module capable of logging DCI Parameters into a blob.  This blob will then
 *  be written into a queue.  This queue can be accessible through a DCI or other internal
 *  means.
 *
 *	@details This module will help log a DCI or several DCIs using various
 *	triggers. The data will be retrieved directly from the DCI and be logged in a single
 *	blob entry. Data logging should be triggered through time, change, or manual.
 *
 *	@n @b Definitions:
 *  @li Entity = A value contained in the DCI database.
 *	@li Entry = A single blob in the log.  Can contain one or multiple entity values.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOG_DCI_H
#define LOG_DCI_H

#include "Includes.h"
#include "DCI.h"
#include "Change_Tracker.h"
#include "Log_Mem.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief The Log_DCI class is multi-instance and is capable of logging single or multiple DCIDs.
 *  @details Logging of DCIs will be done through timed, change, manual triggers.  The DCIDs will
 *  be retrieved from the DCI and put into a blob.  The blob will then be logged in a circular
 *  queue.  The handling of the queue storage will be handled by a Log_Mem interface.  This allows
 *  the Log_DCI module to focus on the sampling of the values.
 *  @n @b Usage:
 **************************************************************************************************
 */
class Log_DCI
{
	public:
		/**
		 * @brief Defines the types of changes required for the change triggered behavior.
		 */
		typedef enum
		{
			ONLY_MANUAL_TRIGGER,		///< no logging done on any value change;
			LOG_ON_ANY_CHANGE,			///< An entry will be logged if any change happens.
			LOG_ON_ALL_CHANGE,			///< An entry will be logged if all values actually change.
			LOG_ON_ALL_WRITE			///< An entry will be logged if all values are written (or
										/// touched)
		} log_change_trigger_t;

		/**
		 *  @brief Constructs an instance of a logger.
		 *  @details
		 *  @param[in] dcid_array: A list of DCIDs which will be logged in a single entry.  For
		 *  example if we send in DCI_NAME(8 bytes), DCI_DATE(4 bytes), DCI_COUNTER(u16)
		 *  as the array of DCIDs with a dcid count of 3.  Then when the blob is retrieved the data
		 *  will be packed and arranged just as they were identified DCI_NAME at byte address 0-7,
		 *  DCI_DATE at byte address 8-11, and DCI_COUNTER at byte address 12 and 13 in little
		 *  endian arrangment.  In other words the DCID data will be logged in order and in a
		 *  packed fashion within the blob.
		 *  @param[in] dcid_count: How many DCIDs are in the DCID array.
		 *  @param[in] log_mem_handle:  The Log_Mem handle is the control to which the blob
		 *  will be handed to for queueing.  This interface will handle the actual Queue.
		 *  @param[in] log_spec_version_owner: The log spec DCIDs owner.This help detect the change in log entry format
		 *  and triggers the erasing of historical log when the new log format is not compatible with earlier version.
		 *  @param[in] log_spec_version: The log spec version which indicates different logging constant log spec
		 *  version.
		 *  @param[in] log_change_trigger
		 *  @n @b Usage: indicates the type of change to trigger change based logging
		 *  @param[in] dcid_watch_list
		 *  @n @b Usage: array indicating dcids to be monitored for change
		 *  @param[in] dcid_watch_list_count
		 *  @n @b Usage: number of dcids in the dcid_watch_list
		 */
		Log_DCI( const DCI_ID_TD* dcid_array, uint16_t dcid_count,
				 Log_Mem* log_mem_handle, DCI_Owner* log_spec_version_owner = nullptr,
				 const log_version_t log_spec_version = 0U,
				 log_change_trigger_t log_change_trigger = ONLY_MANUAL_TRIGGER,
				 const DCI_ID_TD* dcid_watch_list = nullptr, uint16_t dcid_watch_list_count = 0U );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~Log_DCI( void );

		/**
		 *  @brief Will start the interval timer for sampling.
		 *  @details The interval time is in milliseconds.  It sets the time to sample a
		 *  single entry. If you execute a stop interval immediately after this
		 *  call the timer will have captured sample and the timer will stop.  If
		 *  your interval is greater than 60000 milliseconds (1minute) your resolution
		 *  will round to seconds.  See the following examples.
		 *  @n For Example:
		 *  @li 60000 to 60499 milliseconds will trigger at 60 seconds.
		 *  @li 60500 to 61499 milliseconds will trigger at 61 seconds.
		 */
		void Start_Interval( uint32_t interval_time_msec );

		/**
		 *  @brief Stops the interval timer.
		 *  @details The timer will be reset to 0 and will
		 *  start all new on the next start interval call.
		 */
		void Stop_Interval( void );

		/**
		 *  @brief Trigger a log entry on change.
		 *  @details  A change is defined as an actual modification of the parameter from
		 *  from one value to another (ie: 0->1). A write is any write access to the
		 *  value (0->0).
		 */
		void Start_Change( void );

		/**
		 *  @brief Will stop the change based triggering.
		 *  @details This will also clear any changes that might be tracked.
		 */
		void Stop_Change( void );

		/**
		 *  @brief Performs a manual trigger of logging the DCI entities.
		 *  @details This will not reset the timer.
		 */
		Log_Mem::error_t Trigger( void );

		/**
		 *  @brief Get DCID count in the logging array
		 */
		inline uint16_t Get_DCID_Count( void )
		{
			return ( m_dcid_count );
		}

		/**
		 *  @brief Get DCID logging array
		 */
		inline const DCI_ID_TD* Get_DCID_Array( void )
		{
			return ( m_dcid_array );
		}

		/**
		 *  @brief Get list of All Log DCIs handles ( instances ) created
		 */
		static Log_DCI** Get_List( void )
		{
			return ( m_log_dci_list );
		}

		/**
		 *  @brief Updates Log_DCI handle ( This function should be avoided unless there is real need to overwrite the
		 * auto updated list
		 */
		static bool Update_List( log_id_t log_index, Log_DCI* log_dci );

	private:
		Log_DCI( const Log_DCI& );			///< Private copy constructor
		Log_DCI & operator =( const Log_DCI & );			///< Private copy assignment operator

		/**
		 * @brief The milliseconds to seconds count will kick in when our log interval exceeds this
		 * time.
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD MINUTE_TO_MILLISECOND_COUNT = 60000U;
		BF_Lib::Timers* m_timer;

		/**
		 * @brief When the interval exceeds 60seconds then the m_second_tracker will be
		 * used to keep track of the interval.  See Timed task below.
		 */
		uint32_t m_second_ticker;

		/**
		 * @brief A storage of the desired sample interval.
		 * @details This is in milliseconds. Keeping in mind time above 60seconds will switch to seconds.
		 * See start time interval function above for details of what happens
		 * with the different millisecond values.
		 */
		uint32_t m_desired_interval;

		/**
		 * @brief We have given the adopter the ability to log on change of one or all of the parameters.
		 * The bit list will not be used if we are tracking on any parameter
		 * change. If we are configured for "all changed" then every parameter
		 * change that happens will set a bit in the bit list corresponding to its place
		 * in the array. When the bit list is full then all values have changed and the
		 * queue can be triggered. The bit list should be cleared immediately after entering
		 * the queue.
		 */
		BF_Lib::Bit_List* m_bit_list;

		/**
		 * @brief A change tracker is used to monitor the parameters which are to be captured.
		 * @details A current value tracker is all that is needed. The init and other attributes
		 * are not needed or monitored in the queue.
		 */
		Change_Tracker* m_change_tracker;

		/**
		 * @brief Specifies and stores the type of trigger being used for this instance.
		 */
		log_change_trigger_t m_log_change_trigger;

		/**
		 * @brief Specifies the log memory handle.
		 */
		Log_Mem* m_log_mem_handle;

		/**
		 * @brief Used to handle the interval time capture method.  If the adopter
		 * sets a timeout of 100ms per sample then the timer interval will be 100ms.
		 * If the adopter sets an interval for a time greater than 60s then every entry
		 * into the timer will decrement the m_second_ticker until it reaches 0 at which
		 * point the log will be triggered.  Upon triggering the m_second_ticker will then
		 * be reset to the number of seconds listed in the m_desired_interval.
		 * @param param The this pointer so we can reference back to this instance.
		 */
		static void Timed_Task_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{
			reinterpret_cast<Log_DCI*>( param )->Timed_Task();
		}

		void Timed_Task( void );

		/**
		 * @brief Handles the change tracker callbacks.
		 */
		static void Change_Track_Static( Change_Tracker::cback_param_t param,
										 DCI_ID_TD dci_id,
										 Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<Log_DCI*>( param )->Change_Track( dci_id, attribute_mask );
		}

		void Change_Track( DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask );

		/**
		 *  @brief function used to check log spec version for all logging type
		 *      @param log_spec_version_owner: log spec version owner for specific logging.
		 *      @param log_spec_version: log spec version of different log.
		 *      @param log_index: log index for different log type.
		 */
		bool Check_Log_Spec_Version( DCI_Owner* log_spec_version_owner, log_version_t log_spec_version,
									 log_id_t log_index );

		/**
		 *  @brief function used to update log spec version for all logging type in RAM and NV
		 *      @param log_spec_version_owner: log spec version owner for specific logging type.
		 *      @param log_spec_version: new updated log spec version.
		 */
		void Update_Log_Spec_Version( DCI_Owner* log_spec_version_owner, log_version_t log_spec_version );

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_patron;
		DCI_ERROR_TD Log_Get_DCID( DCI_ID_TD dcid, uint8_t* data );

		const DCI_ID_TD* m_dcid_array;
		uint16_t m_dcid_count;
		uint8_t* m_dci_value_array;
		const DCI_ID_TD* m_dcid_watch_list;
		uint16_t m_dcid_watch_list_count;
		const BF_Lib::Timers::TIMERS_TIME_TD SECOND_TO_MILLISECOND_COUNT = 1000U;

		/**
		 * @brief Pointer to list of Log DCIs handles created for all log types.
		 */
		static Log_DCI** m_log_dci_list;
};

}
#endif