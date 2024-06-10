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
#ifndef CHANGE_TRACKER_PLUS_H
#define CHANGE_TRACKER_PLUS_H

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
#include "Change_Tracker.h"

/**
 ****************************************************************************************
 * @brief This is a Change_Tracker_Plus class
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
class Change_Tracker_Plus : public Change_Tracker
{
	public:

		/**
		 *  @brief constructor
		 *  @details This interface allows user to create the Change Tracker for DCI Patrons
		 *
		 *  @param[in] change_cb: Function pointer to Callback function, it will be called once one of the
		 *  DCI id is changed
		 * 	@n @b Usage: Make sure callback function is defined
		 *
		 *  @param[in] change_cb_param: parameters for callback function
		 * 	@n @b Usage:  By default its initialized with nullptr
		 *
		 *  @param[in] value_change_only: Decides to trigger the callback tracker on change of DCI parameter value
		 *  or trigger every time data is set and there is no value change
		 * 	@n @b Usage: true triggers callback tracker if there is change in current value from previous value
		 * 	@n @b Usage: false triggers callback tracker on every write/set of value though current value is not changed
		 * from previous value
		 *
		 *	@param[in] attribute_track_bitfield: To reduce unneeded calls on certain attributes,
		 *	it shall be possible to select a certain subset of attributes (or all) which can be
		 *	tracked.
		 * 	@n @b Usage: If it is intended to only track the RAM values for a series of parameters
		 * 	then the main usage would be a simple RAM tracking attribute mask.
		 * 	If RAM and Initial values are to be tracked then both RAM and NV attribute masks
		 * 	must be sent.
		 *
		 *  @return none
		 */
		Change_Tracker_Plus( Change_Tracker::cback_func_t change_cb,
							 Change_Tracker::cback_param_t change_cb_param = nullptr,
							 bool value_change_only = true,
							 Change_Tracker::attrib_mask_t attribute_track_bitfield =
							 Change_Tracker::COMPATIBLE_TRACKING_MASK );

		/**
		 *  @brief Constructor for Polled change tracker
		 *  @details Change tracker without Callback functionality however it is polling based
		 * 	This change tracker has no callback attached to it and will therefore not trigger when changes occur.
		 * 	However, the peek and get functions will allow for changes to be polled for and action can be
		 * 	taken according to the results of those functions.
		 *
		 *  @param[in] value_change_only
		 * 	@todo describe usage
		 * 	@n @b Usage:
		 *
		 *	@param[in] attribute_track_bitfield: To reduce unneeded calls on certain attributes,
		 *	it shall be possible to select a certain subset of attributes (or all) which can be
		 *	tracked.
		 * 	@n @b Usage: If it is intended to only track the RAM values for a series of parameters
		 * 	then the main usage would be a simple RAM tracking attribute mask.
		 * 	If RAM and Initial values are to be tracked then both RAM and NV attribute masks
		 * 	must be sent.
		 *
		 *  @return none
		 */
		Change_Tracker_Plus( bool value_change_only = true,
							 attrib_mask_t attribute_track_bitfield =
							 COMPATIBLE_TRACKING_MASK );

		/**
		 *  @brief destrucor
		 *  @details It will be invoked when object of Change_Tracker goes out of scope
		 *  @return none
		 */
		~Change_Tracker_Plus( void );

		/**
		 *  @brief Clears the change indication for this specific tracker.
		 *  @details Flush a value if it was changed by you but you happen to have your own change
		 *  tracker. This really should not be ever used and should be deprecated at the first opportunity.
		 *  @param[in] data_id DCI ID to Clear Change.
		 * 	@n @b Usage: valid DCI ID
		 * 	@todo describe usage
		 * 	@param[in] attribute:  The attribute bitfield.
		 *  @n @b Usage:
		 *  @return None
		 */
		void Clear_Change( DCI_ID_TD data_id, attrib_mask_t attribute_bitfield ) const;

		/**
		 *  @brief Provides the ability to flag this parameter as changed for this specific
		 *  change tracker.
		 *  @details In some cases it is necessary to have more discrete control over a change
		 *  tracker.  In cases where a module needs to update the fact that a value has changed
		 *  or in a case where it needs to be serviced later after it was already cleared.
		 *  @param[in] data_id: DCI ID to set as changed.
		 * 	@n @b Usage: valid DCI ID
		 * 	@todo describe usage
		 * 	@param[in] attribute:  The attribute bitfield.
		 *  @n @b Usage:
		 *  @return None
		 */
		void Set_Change( DCI_ID_TD data_id,
						 attrib_mask_t attribute_bitfield ) const;

		/**
		 * @details This function will search for the next DCID with a changed init value.
		 * If no DCI parameter values are changed, the function will return false
		 * @param data_id: Where the data id which has changed will be stored.
		 * @n@b Usage:
		 * @param attribute: The attribute chain you would like to query.
		 * @n@b Usage:  For example if you want to know what is the next changed
		 * RAM value and not necessarily the next value regardless of attribute.
		 * @return: true if there was a DCID that had changed. false otherwise
		 */
		bool Peek_Next( DCI_ID_TD* data_id, attrib_t attribute ) const;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Change_Tracker_Plus( const Change_Tracker_Plus& rhs );
		Change_Tracker_Plus & operator =( const Change_Tracker_Plus& object );

};

#endif
