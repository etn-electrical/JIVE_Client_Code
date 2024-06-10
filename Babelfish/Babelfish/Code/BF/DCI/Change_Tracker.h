/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Interface used to create the Change_Tracker for DCI
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CHANGE_TRACKER_H
#define CHANGE_TRACKER_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI.h"
#include "Bit_List.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"

/**
 ****************************************************************************************
 * @brief This is a Change_Tracker class
 * @details It provides an interface used to create the Change_Tracker for DCI
 * @n @b Usage:
 * @n It provides public methods to
 * @n @b 1. To Add, remove DCI ids to be tracked
 * @n @b 2. To know if DCId is being tracked or not
 * @n @b 3. To remove all the DCI ID from the tracker list
 * @n @b 4. To search the next DCI ID with a changed init, RAM or other than init and RAM value.
 *
 ****************************************************************************************
 */
class Change_Tracker
{
	public:
		/// The following is used as a bitfield definition and thus why it lacks a type declaration.

		typedef uint8_t attrib_t;
		static const uint8_t RAM_ATTRIB = 0U;
		static const uint8_t NV_ATTRIB = 1U;
		static const uint8_t DEFAULT_ATTRIB = 2U;
		static const uint8_t MIN_ATTRIB = 3U;
		static const uint8_t MAX_ATTRIB = 4U;
		static const uint8_t LENGTH_ATTRIB = 5U;
		static const uint8_t ENUM_ATTRIB = 6U;
		static const uint8_t TOTAL_ATTRIBS = 7U;

		typedef uint16_t attrib_mask_t;
		static const attrib_mask_t RAM_ATTRIB_MASK = static_cast<uint8_t>( 1U << RAM_ATTRIB );
		static const attrib_mask_t NV_ATTRIB_MASK = static_cast<uint8_t>( 1U << NV_ATTRIB );
		static const attrib_mask_t DEFAULT_ATTRIB_MASK = static_cast<uint8_t>
			( 1U << DEFAULT_ATTRIB );
		static const attrib_mask_t MIN_ATTRIB_MASK = static_cast<uint8_t>( 1U << MIN_ATTRIB );
		static const attrib_mask_t MAX_ATTRIB_MASK = static_cast<uint8_t>( 1U << MAX_ATTRIB );
		static const attrib_mask_t LENGTH_ATTRIB_MASK = static_cast<uint8_t>
			( 1U << LENGTH_ATTRIB );
		static const attrib_mask_t ENUM_ATTRIB_MASK = static_cast<uint8_t>( 1U << ENUM_ATTRIB );
		static const attrib_mask_t TRACK_ALL_ATTRIBUTES_MASK = RAM_ATTRIB_MASK | NV_ATTRIB_MASK |
			DEFAULT_ATTRIB_MASK | MIN_ATTRIB_MASK | MAX_ATTRIB_MASK | LENGTH_ATTRIB_MASK |
			ENUM_ATTRIB_MASK;

		static const attrib_mask_t COMPATIBLE_TRACKING_MASK = RAM_ATTRIB_MASK | NV_ATTRIB_MASK;

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param, DCI_ID_TD dci_id,
									   attrib_mask_t attribute_bitfield );

		/**
		 *  @brief constructor
		 *  @details This interface allows user to create the Change Tracker for DCI Patrons
		 *  @note If the RTOS is enabled, any callback called from the change tracker is handled in the task context of
		 *  the CR_Tasker. What this means is that a callback that pends the CR_Tasker will also be pended and
		 *  the priority can potentially fall into the fallback idle task. If the pending operation takes too
		 *  much time, then the CR_Tasker can be starved.
		 *
		 *  @param[in] change_cb: Function pointer to Callback function, it will be called once one
		 * of the
		 *  DCI id is changed
		 *  @n @b Usage: Make sure callback function is defined
		 *
		 *  @param[in] change_cb_param: parameters for callback function
		 *  @n @b Usage:  By default its initialized with nullptr
		 *
		 *  @param[in] value_change_only: Decides to trigger the callback tracker on change of DCI
		 * parameter value
		 *  or trigger every time data is set and there is no value change
		 *  @n @b Usage: true triggers callback tracker if there is change in current value from
		 * previous value
		 *  @n @b Usage: false triggers callback tracker on every write/set of value though current
		 * value is not changed from previous value
		 *
		 *	@param[in] attribute_track_bitfield: To reduce unneeded calls on certain attributes,
		 *	it shall be possible to select a certain subset of attributes (or all) which can be
		 *	tracked. One or more of the attrib_mask_t _MASK types.
		 *  @n @b Usage: If it is intended to only track the RAM values for a series of parameters
		 *  then the main usage would be a simple RAM tracking attribute mask.
		 *  If RAM and Initial values are to be tracked then both RAM and NV attribute masks
		 *  must be sent.
		 *
		 *  @return none
		 */
		Change_Tracker( cback_func_t change_cb, cback_param_t change_cb_param =
						reinterpret_cast<cback_param_t>( nullptr ),
						bool value_change_only = true,
						attrib_mask_t attribute_track_bitfield = COMPATIBLE_TRACKING_MASK );

		/**
		 *  @brief Constructor for Polled change tracker
		 *  @details Change tracker without Callback functionality however it is polling based
		 *  This change tracker has no callback attached to it and will therefore not trigger when
		 * changes occur.
		 *  However, the peek and get functions will allow for changes to be polled for and action
		 * can be
		 *  taken according to the results of those functions.
		 *
		 *  @param[in] value_change_only
		 *  @todo describe usage
		 *  @n @b Usage:
		 *
		 *	@param[in] attribute_track_bitfield: To reduce unneeded calls on certain attributes,
		 *	it shall be possible to select a certain subset of attributes (or all) which can be
		 *	tracked.
		 *  @n @b Usage: If it is intended to only track the RAM values for a series of parameters
		 *  then the main usage would be a simple RAM tracking attribute mask.
		 *  If RAM and Initial values are to be tracked then both RAM and NV attribute masks
		 *  must be sent.
		 *
		 *  @return none
		 */
		Change_Tracker( bool value_change_only = true,
						attrib_mask_t attribute_track_bitfield = COMPATIBLE_TRACKING_MASK );

		/**
		 *  @brief destrucor
		 *  @details It will be invoked when object of Change_Tracker goes out of scope
		 *  @return none
		 */
		~Change_Tracker( void );

		/**
		 *  @brief Add DCI ID to be Tracked
		 *  @details This functions is used to add the DCI ID to be added to Chnage tracker list so
		 * that it can be tracked
		 *  if its value is changed or its being written
		 *  @param[in] data_id
		 *  @n @b Usage: Valid DCI ID to be tracked
		 *  @param[in] set_id_as_changed
		 *  @n @b Usage: When set to true, this DCID will be flagged as changed.  This provides
		 *	the ability to indicate that you want to track a parameter as well as get an
		 *	initial callback that it has changed.  If you set it to false, the tracker user
		 *	will have to wait until the value changes after this initial track command.
		 *  @return none
		 */
		void Track_ID( DCI_ID_TD data_id, bool set_id_as_changed = false );

		/**
		 *  @brief UnSelect the data ID to inhibit tracking
		 *  @details This functions is used to unselect the DCI IDs being Tracked
		 *  @param[in] data_id
		 *  @n @b Usage: Valid DCI ID to be unselected
		 *  @param[in] set_id_as_changed
		 *  @n @b Usage:
		 *  @todo is this parameter required, what is the use ?
		 *  @n @b Usage:
		 *  @return none
		 */
		void UnTrack_ID( DCI_ID_TD data_id, bool clear_pending_changes = false );

		/// UnSelect the all data IDs to inhibit tracking.
		/**
		 *  @brief UnSelect all the data IDs to inhibit tracking
		 *  @details This functions is used to unselect all the DCI IDs being tracked
		 *  @param[in] clear_pending_changes
		 *  @todo write usage
		 *  @n @b Usage:
		 *  @return none
		 */
		void UnTrack_All_IDs( bool clear_pending_changes = false );

		/**
		 *  @brief Check if DCI ID is being tracked
		 *  @details This functions is used to check if given DCI ID is being tracked or not
		 *  @param[in] data_id DCI ID to be checked for being tracked or not
		 *  @n @b Usage: valid DCI ID
		 *  @return true if ID is being tracked else False
		 */
		bool ID_Tracked( DCI_ID_TD data_id );

		/**
		 * @brief Get the Next available DCI ID in the Change Tracker, if any is available.
		 * @details Search the queue of the indicated attribute and get the next available DCI ID, if any.
		 * @param[out] data_id: The DCID which has changed, or DCI_ID_UNDEFINED if none is found.
		 * @param[in] attribute: The attribute type to check for, from the attrib_t _ATTRIB types.
		 * 				Each attribute has its own change queue.
		 * @return: true if there was a DCID that had changed. false if none had changed.
		 */
		bool Get_Next( DCI_ID_TD* data_id, attrib_t attribute );

		/**
		 * @details These friends calls allows to use the private static
		 * function calls contained below inside of the DCI Owner and DCI Patron.
		 */
		friend class DCI_Owner;
		friend class DCI_Patron;
		friend class DCI_Worker;
		friend class DCI_Workers;

	protected:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Change_Tracker( const Change_Tracker& rhs );
		Change_Tracker & operator =( const Change_Tracker& object );

		// Contains the datatype for the lookup word array.
		typedef uint8_t bit_word_t;

		/// The following constants break apart at 64bit.
		static const bit_word_t WORD_INDEX_SHIFT = ( sizeof( bit_word_t ) / 2 ) + 3;
		static const bit_word_t BIT_INDEX_MASK = ( sizeof( bit_word_t ) * 8 ) - 1;
		static const DCI_ID_TD TOTAL_BIT_WORDS = ( DCI_TOTAL_DCIDS >> WORD_INDEX_SHIFT ) + 1;

		// handles the initialization of the attribute bit lists and tracker bitfield.
		void Initialize_Trackers( attrib_mask_t attribute_track_bitfield );

		/**
		 * Called by the module responsible for doing the changes.
		 * This is a private function made public only for the DCI behavior.
		 * This should not be accessed by an instance.
		 */
		static void ID_Change( DCI_ID_TD data_id, bool value_changed, attrib_t attribute,
							   Change_Tracker const* req_tracker );

		static void Maintenance_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * The maintenance task is woken using a CR Queue.  We need to keep track in the
		 * cases where the task might be woken up twice in one function.
		 */
		static bool m_task_woken;

		static Change_Tracker* m_tracker_list;
		static CR_Queue* m_cr_queue;

		/**
		 * @brief Used internally to navigate the list regardless of attribute.  Attribute
		 * will be returned.
		 * @details This function is used to extract what the next attribute and DCID which has
		 * changed.
		 * @param[out] data_id: The DCID which has changed.
		 * @param[out] attribute_bitfield: The attribute which has changed.  We will return the
		 * attributes which have changed for that specific DCID.
		 * @return: true if there was a DCID that had changed. false otherwise
		 */
		bool Get_Next_Full_DCID( DCI_ID_TD* data_id, attrib_mask_t* attribute_bitfield ) const;

		static void Add_Tracker( Change_Tracker* new_tracker );

		static void Remove_Tracker( Change_Tracker* old_tracker );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_Track_Bit( DCI_ID_TD dcid )
		{ m_track_array[dcid >> WORD_INDEX_SHIFT] |= 1 << ( dcid & BIT_INDEX_MASK ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Clr_Track_Bit( DCI_ID_TD dcid )
		{ m_track_array[dcid >> WORD_INDEX_SHIFT] &= ~( 1 << ( dcid & BIT_INDEX_MASK ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Test_Track_Bit( DCI_ID_TD dcid )
		{
			return ( ( m_track_array[dcid >> WORD_INDEX_SHIFT] &
					   ( 1 << ( dcid & BIT_INDEX_MASK ) ) ) != 0 );
		}

		bit_word_t m_track_array[TOTAL_BIT_WORDS];
		Change_Tracker* m_next;

		cback_func_t m_change_cb;
		cback_param_t m_change_cb_param;
		bool m_value_change_only;

		/**@details m_attrib_scan_start is here to help control the amount of scanning that must
		 * go on for a full attribute scan.  If someone only uses the init and ram tracking then
		 * we will avoid scanning the entire list of attributes.  In the case where
		 * the full list is tracked we have to navigate through the whole list of attribute ids.
		 */
		attrib_t m_attrib_scan_start;
		BF_Lib::Bit_List* m_que[TOTAL_ATTRIBS];

		static Change_Tracker* const NULL_TRACKER;
		static BF_Lib::Bit_List* const NULL_BITLIST;
};

#endif
