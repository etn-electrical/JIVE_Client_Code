/**
 *****************************************************************************************
 *  @file IOT_Group_Handler.h
 *	@brief This is the header that declares the IOT_Group_Handler class.
 *
 *	@details IOT_Group_Handler manages the group cadence and updating for one Cadence Group.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IOT_GROUP_HANDLER_H
#define IOT_GROUP_HANDLER_H
#include "DCI_Patron.h"
#include "IoT_DCI.h"
#include "IOT_DCI_Data.h"
#include "Change_Tracker.h"
#include "Bit_List.h"
#include "iot_device.h"
#include "azure_c_shared_utility/vector.h"

class Change_Tracker_Plus;

extern "C" time_t get_time( time_t* currentTime );

/**
 ****************************************************************************************
 * @brief This class manages one Cadence Group's operation.
 * @details It provides the change tracker implementation and supports the publishing
 *          of SeeMe (realtime) data.
 * @ingroup iot_publish_cadence
 *
 * @li It provides public methods to
 * @li Attach change tracker.
 ****************************************************************************************
 */
class IOT_Group_Handler
{
	public:
		/**
		 * @brief Constructor.
		 * @details This function initialze the member variables and attaches the change tracker(s).
		 * @param iot_dci_handler: Handler for the IOT channel to DCID database.
		 * @param group_index: The index for this group in the target's group_list and cadence_owners.
		 * @param cadence_owners Handle to the array (Azure vector) of DCI owners for the group cadence channels.
		 * 				These are in the array in the order of the groups in iot_database.
		 */
		IOT_Group_Handler( const IOT_DCI* iot_dci_handler, uint16_t group_index, VECTOR_HANDLE cadence_owners );

		/** Default XTOR for this class. */
		~IOT_Group_Handler( void );

		/**
		 * @brief Getter for the current cadence, in milliseconds, for this group.
		 * @return The cadence in milliseconds.
		 */
		uint32_t Get_Group_Cadence_ms( void );

		/**
		 * @brief Tells if this group has any StoreMe/trended on-interval channel members.
		 * @details This is a getter for the flag that indicates whether any of this group's channels were found
		 *          to have the StoreMe on-interval type set for them.
		 * @return true if any of this group's channels have the StoreMe/trended on-interval type, false if none do.
		 */
		bool Has_Trends_On_Interval( void );

		/**
		 * @brief Tells if this group has any SeeMe/realtime update channel members.
		 * @details This is a getter that indicates whether any of this group's channels were found
		 *          to have the SeeMe type set for them. Does not indicate whether any are available right now.
		 * @return true if any of this group's channels have the SeeMe/realtime type, false if none do.
		 */
		bool Has_Realtimes( void );

		/**
		 * @brief Tells if this group has any StoreMe/Trends update channel members.
		 * @details This is a getter that indicates whether any of this group's channels were found
		 *          to have the StoreMe type set for them. Does not indicate whether any are available right now.
		 * @return true if any of this group's channels have the StoreMe/Trends type, false if none do.
		 */
		bool Has_Trends( void );

		/**
		 * @brief Tells if this group has change tracker attached.
		 * @details This is a getter that indicates whether this group has change tracker attached to
		 * determine whether the channels should publish when their value is changed e.g SeeMe and
		 * StoreMeOnChange types.
		 * @return true if this group has change tracker attached.
		 */
		bool Has_Change_Tracker_Attached( void );

		/**
		 * @brief Checks if any of the SeeMe/realtime channel values subscribed to for this group are changed.
		 * @details This function checks if any of the channel values subscribed to for this group are changed,
		 * 			using the Peek_Next function.
		 * @return true if any of this group's SeeMe/realtime channel values changed,
		 *          false if none changed or always returns false if none are subscribed for realtime changes.
		 */
		bool Is_Data_Changed( void );

		/**
		 * @brief Simple getter for the Group Marker for this group.
		 * @return The enum from IOT_GROUP_MARKERS that marks which group this is.
		 */
		uint16_t Get_Group_Marker( void );

		/**
		 * @brief Simple getter for the number of channels in this group.
		 * @return The value of m_channel_count, the number of channels in this group.
		 */
		uint16_t Get_Channel_Count( void );

		/**
		 * @brief Peek into the Change Tracker and learn what the next changed channel's DCI ID would be.
		 * @details This is only a "peek" into the Change Tracker that does not alter its contents,
		 * 		except to set the internal "pointer" at that ID, so a subsequent Get_Next will
		 * 		start there and certainly get this ID.
		 * @return DCI ID of the next tracked and changed channel,
		 *          or 0 if none are tracked or none (no more) are changed.
		 */
		DCI_ID_TD Peek_Next_Channel( void );

		/**
		 * @brief Gets the next changed channel's DCI ID from the Change Tracker.
		 * @details This fetch from the Change Tracker does alter its contents, clearing the returned
		 *          entry from the Channel Tracker's queue.
		 * @return DCI ID of the next tracked and changed channel,
		 *          or 0 if none are tracked or none (no more) are changed.
		 */
		DCI_ID_TD Get_Next_Channel( void );

		/**
		 * @brief Tells if this group allows publishing all channels
		 * @details This is a getter that indicates whether this group is allowed to publish all
		 * channels on establishing IOT connection
		 * @return true if this group allows publishing all channels on connect
		 */
		bool Allow_Publishing_All_channels_On_Connect( void );

	private:
		/**
		 * @brief Read the DCI value for this group's cadence, and convert to milliseconds.
		 * @details Gets the correct DCI owner for this group, reads the value in milliseconds (int32),
		 * 			and stores it in m_cadence_ms;
		 * @param group_index: The index for this group in the target's group_list and cadence_owners.
		 * @param cadence_owners Handle to the array (Azure vector) of DCI owners for the group cadence channels.
		 * 				These are in the array in the order of the groups in iot_database.
		 * @return True on success, else False on failure to get the cadence.
		 */
		bool Initialize_Cadence( uint16_t group_index, VECTOR_HANDLE cadence_owners );

		/**
		 * @brief Attach change tracker.
		 * @details This function attaches a change tracker (plus) for all the IOT DCIDs that are to be
		 *          published as SeeMe/realtimes or StoreMe(aka StoreMe on change) in this group.
		 * @param iot_dci_handler Handler for IOT channel to DCID database.
		 */
		void Attach_Change_Tracker( const IOT_DCI* iot_dci_handler );

		/**
		 * @brief Static function to redirect the call to run our Cadence Check timer task.
		 * @param param: An opaque argument that resolves to a pointer to "this".
		 */
		static void Cadence_Check_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Steady timer task that checks to see if this group needs anything to be published.
		 * @details This timer task is invoked periodically on the cadence for this group.
		 * 	It will check, if this group has SeeMe/realtime subscribers, to see if any of them have
		 *  changed and thus need to be published.
		 *  If so, it will set the bit for this group in the IOT_Net's m_groups_ready collection.
		 *  We keep this task intentionally small and quick, since it runs under the task for all
		 *  FreeRTOS timers.
		 */
		void Cadence_Check_Timer_Task( void );

		/**
		 * @brief Callback function updates the cadence rate of IOT cadence group at run time.
		 * @details After updating the cadence rate, the publishing should happen as per new cadence rate
		 * [Note: There's no need to re-initialize/re-connect IOT]
		 * @param cback_param: Parameters for Callback function.
		 * @param access_struct: The access struct is the order slip.
		 * @return DCI callback return type.
		 */
		static DCI_CB_RET_TD Reinitialize_Cadence_static( DCI_CBACK_PARAM_TD handle,
														  DCI_ACCESS_ST_TD* access_struct );

		DCI_CB_RET_TD Reinitialize_Cadence( DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief Validates and sets the cadence rate of a group
		 * @details Validates the cadence rate for values 0(Immediate publishing), -1(Stop publishing)
		 * and other values and sets m_cadence_ms accordingly. m_cadence_ms is then used for setting the
		 * time base of Cadence_Check_Timer_Task. It also sets m_is_group_publishing_enabled flag to
		 * enable/disable publishing
		 * @param cadence_rate_msec: Cadence rate in milliseconds
		 * @return None
		 */
		void Validate_Set_Cadence_Rate( int32_t cadence_rate_msec );


		/** Enum from IOT_GROUP_MARKERS that marks which group this is. */
		uint16_t m_group_marker;

		/** A count of the number of channels in this group. */
		uint16_t m_channel_count;

		/** Holds the current value for this group's cadence, read from the DCI. */
		uint32_t m_cadence_ms;

		/** Flag that indicates whether the publishing is enabled or disabled for a particular group. */
		bool m_is_group_publishing_enabled;

		/** Initialized to true if this group has any StoreMe/trended on-interval type members. */
		bool m_has_trends_on_interval;

		/** Initialized to true if this group has any StoreMe/trended (on-change) type members. */
		bool m_has_trends;

		/** Initialized to true if this group has any SeeMe/realtimes type members. */
		bool m_has_realtimes;

		/** Holds the change tracker instance for this group, if it has SeeMe/realtime or
		 * StoreMe(aka StoreMe on change) channels in it.
		 * Else will remain null. */
		Change_Tracker_Plus* m_group_tracker;

		/** Holds the timer instance that will manage the cadence update rate for this group. */
		BF_Lib::Timers* m_cadence_check_timer;

		/**  Flag that allows/restricts us from publishing all channels on establishing IOT connection.
		 * 	By default we allow publishing on connect.   */
		bool m_publish_all_on_connect;

		VECTOR_HANDLE m_cadence_rate_owner;

		uint_fast16_t m_group_index;

		static const int32_t m_CADENCE_RATE_0_SEC = 0;

		static const int32_t m_CADENCE_RATE_10_SEC = 10000;

		static const int32_t m_CADENCE_STOP_PUBLISHING = -1;

};

#endif	// IOT_GROUP_HANDLER_H
