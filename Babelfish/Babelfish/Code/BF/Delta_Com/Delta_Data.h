/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DELTA_DATA_H
#define DELTA_DATA_H

#include "Delta_Com_Defines.h"
#include "Delta_Com.h"
#include "DCI_Workers.h"
#include "Change_Tracker_Plus.h"
#include "Bit_List.h"

/**
 *
 */
class Delta_Data
{
	public:
		struct data_def_t
		{
			uint16_t pid_dcid_list_len;
			Delta_Com::pid_to_dcid_map_t const* pid_to_dcid_map;
			uint16_t const* dcid_to_pid_map;

			uint8_t const* owner_patron_not_list;

			uint8_t const* subscription_bit_list;
			uint8_t const* client_sot_subscription_bit_list;
			uint8_t const* general_subscription_bit_list;
			uint8_t const* general_client_sot_subscription_bit_list;

			uint8_t const* value_clear;
		};

		Delta_Data( data_def_t const* data_def, bool owner_access = true,
					bool correct_out_range = true );
		~Delta_Data( void );

		void Reset_Full( void );

		bool Publish_Request( uint8_t* rx_data, uint16_t param_count, uint8_t tracking_type );

		/**
		 * @brief Fills out a buffer with Parameters which are desired to be subscribed to.
		 * @details The subscription list here is the general subscription list.  Not a special
		 * SOT list.  The client is the only one that gets to define the SOT.  The SOT is defined
		 * in the appropriate data def block.  We will exhaust the list meaning the values will
		 * be queued out and every time you call it you will get more until it is empty.  After
		 * that if you want the list to start over you have to call the reset subscription queues.
		 * @param buffer:  Destination buffer for the parameters.
		 * @param length:  A pointer to the max length allowed and when returned will indicate
		 * how much data length was actually used.
		 * @return A count of the parameters left to go.
		 */
		uint16_t Subscription_Req_List( uint8_t* tx_data, uint16_t* tx_length_used,
										uint16_t tx_length_available, uint8_t tracking_type );

		bool Subscription_Complete( void );

		delta_com_error_t Get_Delta_Data( uint8_t* tx_data, uint16_t* tx_length_used,
										  uint16_t tx_length_available );

		delta_com_error_t Set_Delta_Data( uint8_t* rx_data, uint16_t rx_data_len );

		delta_com_error_t Get_Data( Delta_Com::param_id_t pid, delta_com_attrib_t attribute,
									uint8_t* data, DCI_LENGTH_TD* amount_gotten,
									DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
									DCI_LENGTH_TD amount = ACCESS_LENGTH_GET_ALL );

		delta_com_error_t Set_Data( Delta_Com::param_id_t pid, delta_com_attrib_t attribute,
									uint8_t* data, DCI_LENGTH_TD* amount_set, DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
									DCI_LENGTH_TD amount = ACCESS_LENGTH_GET_ALL );

		/**
		 * @brief Shall indicate whether the initial synchronization has been completed.
		 *
		 * @details On initial connection all parameters must be synchronized across the two
		 * elements.  Because of this the code needs to know when this initial synch is complete.
		 * @return True: A full exchange of our data has happened.
		 * False: A full exchange has not been completed or is not in progress.
		 */
		bool Sync_Complete( void );

		/**
		 * @brief In some instances we need to reduce the level of access of a connected DCOM user.
		 * This function shall set the state of the access
		 *
		 * @details In some cases we need Delta Com to interface with a PC Tool and in some cases
		 * we need to be an integrated cache.  A PC Tool should be protected from doing unintended
		 * setting.
		 * @param protected_access:  If set to true then we will convert the interface to a more
		 * limited access set based on patron like behaviors.  If you set a false then we will use
		 * the owner level accesses.
		 */
		void Set_Access_Level( bool owner_access );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Delta_Data( const Delta_Data& rhs );
		Delta_Data & operator =( const Delta_Data& object );

		static const uint8_t DCOM_TO_CHANGE_TRACKER_ATTRIB[END_OF_ATTRIBUTE_LIST];

		// To help calculate that the total parameters do not exceed the length of the provided
		// buffer.
		static const uint16_t MIN_CHANGE_PARAM_ENTRY_LENGTH = PARAMETER_LENGTH + MIN_DATA_LENGTH;


		// Prioritized list of attributes to check for a publish of changes.
		static const uint8_t GET_DELTA_DATA_ATTRIB_LIST_LEN;
		static const Delta_Com::attribute_t GET_DELTA_DATA_ATTRIB_LIST[];
		// If we get to this attribute then we can consider the device synced up.
		static const Delta_Com::attribute_t SYNC_COMPLETE_ATTRIBUTE = CURRENT_VALUE;

		static const Change_Tracker::attrib_mask_t CHANGE_TRACKER_ATTRIB_TRACK_MASK =
			Change_Tracker::RAM_ATTRIB_MASK | Change_Tracker::NV_ATTRIB_MASK |
			Change_Tracker::DEFAULT_ATTRIB_MASK | Change_Tracker::MIN_ATTRIB_MASK |
			Change_Tracker::MAX_ATTRIB_MASK;

		delta_com_error_t Get_DCI_Data( DCI_ID_TD dcid, delta_com_attrib_t attribute,
										uint8_t* data, DCI_LENGTH_TD* amount_gotten,
										DCI_LENGTH_TD offset = ACCESS_OFFSET_NONE,
										DCI_LENGTH_TD amount = ACCESS_LENGTH_GET_ALL );

		uint16_t Get_PID_DCID_Map_Index_From_PID( Delta_Com::param_id_t parameter_id ) const;

		DCI_ID_TD Convert_PID_To_DCID( Delta_Com::param_id_t parameter_id ) const;

		Delta_Com::param_id_t Convert_DCID_to_PID( DCI_ID_TD dcid ) const;

		bool Publish_PID_Request( Delta_Com::param_id_t param_id, uint8_t tracking_type );

		delta_com_error_t Convert_DCI_Status( DCI_ERROR_TD dci_access_result ) const;

		void Reset_Subscription_Req_List_Que( void );

		void Reset_Data( void ) const;

		void Reset_Publish_Req_List( void );

		data_def_t const* m_data_def;
		Change_Tracker_Plus* m_val_tracker;
		DCI_Workers* m_workers;

		BF_Lib::Bit_List* m_sub_req_list_owner;
		BF_Lib::Bit_List* m_client_sot_sub_req_list_owner;
		BF_Lib::Bit_List* m_sub_req_list_patron;
		BF_Lib::Bit_List* m_client_sot_sub_req_list_patron;

		BF_Lib::Bit_List* m_sub_req_list;
		BF_Lib::Bit_List* m_client_sot_sub_req_list;

		BF_Lib::Bit_List::bit_list_bit_td m_sub_req_ctr;
		BF_Lib::Bit_List::bit_list_bit_td m_client_sot_sub_req_ctr;

		BF_Lib::Bit_List* m_startup_sync;
		bool m_owner_access;

		// uint16_t Get_PID_DCID_Map_Index_From_DCID( DCI_ID_TD dcid );  //DEPRECATE:  No longer used.
};

#endif
