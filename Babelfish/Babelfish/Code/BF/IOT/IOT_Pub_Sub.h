/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the cadence and change tracker functionality.
 *
 *	@details IOT_Pub_Sub contains all the necessary methods for data transmission b/w
 *  cloud and the device.
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IOT_PUB_SUB_H
#define IOT_PUB_SUB_H
#include "DCI_Patron.h"
#include "IoT_DCI.h"
#include "IOT_DCI_Data.h"
#include "Change_Tracker.h"
#include "Bit_List.h"
#include "iot_device.h"
#include "azure_c_shared_utility/vector.h"

class IOT_Group_Handler;

extern "C" time_t get_time( time_t* currentTime );

/**
 ****************************************************************************************
 * @brief This is an IOT Publish Subscribe class
 * @details It provides change tracker implementaion and cadence inplementation.
 * @ingroup iot_publish_cadence
 *
 * @li It provides public methods to
 * @li 1. IOT publish subscribe constructor.
 * @li 2. Attach change tracker.
 * @li 3. Create channel data.
 ****************************************************************************************
 */
class IOT_Pub_Sub
{
	public:
		/**
		 * @brief Constructor.
		 * @details This function initialze the member variables and attaches the change tracker(s).
		 * @param[in] iot_database: IOT channel to DCID database.
		 * @param patron: The DCI patron instance created for accessing the channel data for IoT.
		 * @param source_id: The given DCI Source ID for this class to use.
		 * @param cadence_owners Handle to the array (Azure vector) of DCI owners for the group cadence channels.
		 * 				These are in the array in the order of the groups in iot_database.
		 * @param device_uuid Pointer to the string with the publisher device's UUID, in 8-4-4-4-12 format.
		 */
		IOT_Pub_Sub( const iot_target_info_st_t* iot_database, DCI_Patron* patron, DCI_SOURCE_ID_TD source_id,
					 VECTOR_HANDLE cadence_owners, const char* device_uuid );
		/**
		 *  @brief Destructor
		 *  @details This will get called when object of IOT_Pub_Sub goes out of scope
		 */
		~IOT_Pub_Sub( void );

		/** (Re)Initialize class members before starting to publish.
		 * Used after connecting or reconnecting. */
		void Initialize( void );

		/** Publish all the channel data for the indicated group.
		 * This function is normally called right after connecting, to publish the SeeMe/realtime data for all
		 * channels, so that PX White has a known good, fresh copy of all the channel data to start with.
		 * (After this, updates will refresh the cloud's copy of the channel data.)
		 * This will also send the initial StoreMe/trend messages for any group that supports trends.
		 * @param group_index: The index of the group being addressed for publishing.
		 * @return True if done sending all the data for the indicated group, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_All_Channels( uint16_t group_index );

		/**
		 * @brief Publish the StoreMe/trend and/or SeeMe/realtime data for the indicated group.
		 * @details Determine which data is desired, and while we have message pool buffes available,
		 * 		put together the messages as appropriate and send them.
		 * 		If the messages cannot be sent at this time (no buffers available), then indicate that in the return.
		 * @param group_index: The index of the group being addressed for publishing.
		 * @param send_all: If true, send all channels in the group, regardless of whether they were changed.
		 * 					This is used right after the connection is established.
		 * @return True if done sending all the data for this group, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_Channel_Data( uint16_t group_index, bool send_all );

		/**
		 * @brief Publish the SeeMe/realtime data for all, or all changed, channels of the given group.
		 * @details For either all channels of this group, or only those that have been marked as changed,
		 * 		and while we have message pool buffers available,
		 * 		put together the realtimes update message(s) and send them.
		 * 		If the messages cannot be sent at this time (no buffers available), then indicate that in the return.
		 * @param this_group: The pointer to the IOT_Group_Handler instance for this message.
		 * @param send_all: If true, send all channels in the group, regardless of whether they were changed.
		 * 					This is used right after the connection is established.
		 * @return True if done sending all the data for this group, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_SeeMe_Realtimes_Message( IOT_Group_Handler* this_group, bool send_all );

		/**
		 * @brief Publish the StoreMe/trend on-interval type data for all channels of the given group.
		 * @details Determine which data is desired, and while we have message pool buffers available,
		 * 		put together the trends message(s) and send them.
		 * 		If the messages cannot be sent at this time (no buffers available), then indicate that in the return.
		 * @param this_group: The pointer to the IOT_Group_Handler instance for this message.
		 * @return True if done sending all the data for this group, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_StoreMeOnInterval_Trends_Message( IOT_Group_Handler* this_group );

		/**
		 * @brief Publish the StoreMe/trend data for all, or all changed channels of the given group.
		 * @details For either all channels of this group, or only those that have been marked as changed,
		 * 		and while we have message pool buffers available,
		 * 		put together the trends update message(s) and send them.
		 * 		If the messages cannot be sent at this time (no buffers available), then indicate that in the return.
		 * @param this_group: The pointer to the IOT_Group_Handler instance for this message.
		 * @param send_all: If true, send all channels in the group, regardless of whether they were changed.
		 * 					This is used right after the connection is established.
		 * @return True if done sending all the data for this group, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_StoreMe_Trends_Message( IOT_Group_Handler* this_group, bool send_all );

		/**
		 * @brief Publish alarms message to IOT hub
		 * @details This function captures all the alarm updates related details and publishes the
		 * alarm message once its cadence interval is reached
		 * If the messages cannot be sent at this time (no buffers available), then indicate that in the return.
		 * @param[in] None
		 * @param[out] None
		 * @return True if done sending all the alarms, else False if this should be called
		 * 			again in the near future to attempt to publish more.
		 */
		bool Publish_Alarms_Message( void );

		/** Simple setter for the Device Handle.
		 * @param device_handle The Low-Level Azure handle needed to send messages.
		 */
		void Set_Device_Handle( IOT_DEVICE_HANDLE device_handle );

		/** Getter for our IoT to DCI interface instance.
		 * @return Pointer to our m_iot_dci member.
		 */
		IOT_DCI* Get_Iot_Dci_Interface();

		/** Getter for a pointer to the publisher device's UUID string.
		 * @return Pointer to the publisher device's UUID string.
		 */
		const char* Get_Publisher_Device_Uuid();

		/**
		 * @brief Tells if this group allows publishing all channels
		 * @details This is a getter that indicates whether this group is allowed to publish all
		 * channels on establishing IOT connection
		 * @param group_index: The index of the group being addressed for publishing.
		 * @return true if this group allows publishing all channels on connect
		 */
		bool Is_Publish_All_Channels_On_Connect( uint16_t group_index );

	private:

		/** @brief  Get the list of all the channel offsets for this group, but cap the list at a size that will fit.
		 * @details Will scan the list of all IOT channels, pulling the ones that belong to the given group,
		 * 			but checking that the estimated length of this channel should fit when serialized,
		 * 			and stopping if we estimate that we hit the endpoint of the message buffer.
		 * @param this_group: The pointer to the IOT_Group_Handler instance for this message.
		 * @param[out] group_indices On successful return, this array will contain the indices of channels
		 * 			to be processed for the message, up to this_group->Get_Channel_Count().
		 * @param[out] idx_count The number of indices in the returned array.
		 * @param[out] estimated_size The estimated size that the message buffer will be after the channels are
		 * serialized.
		 * @return True if we estimate that we are able to fit all the channels in the message, so the array has all
		 * 			the pertinent offsets, or False if we had to leave some.
		 */
		bool Get_All_Fitting_Channels( IOT_Group_Handler* this_group, uint16_t* group_indices, uint16_t& idx_count,
									   uint16_t& estimated_size );

		/** @brief  Get the list of all the channel which had updates for this group, but cap the list at a size that
		   will fit.
		 * @details Will get the channels that have been updated from this group's Change Tracker,
		 * 			but checking that the estimated length of this channel should fit when serialized,
		 * 			and stopping if we estimate that we hit the endpoint of the message buffer.
		 * @param this_group: The pointer to the IOT_Group_Handler instance for this message.
		 * @param[out] group_indices On successful return, this array will contain the indices of channels
		 * 			to be processed for the message, up to this_group->Get_Channel_Count().
		 * @param[out] idx_count The number of indices in the returned array.
		 * @param[out] estimated_size The estimated size that the message buffer will be after the channels are
		 * serialized.
		 * @return True if we estimate that we are able to fit all the updated channels in the message, so the array has
		 * all
		 * 			the pertinent offsets, or False if we had to leave some.
		 */
		bool Get_Updated_Fitting_Channels( IOT_Group_Handler* this_group, uint16_t* group_indices,
										   uint16_t& idx_count, uint16_t& estimated_size );

#ifdef IOT_ALARM_SUPPORT
		/** @brief  Get all the alarm updates from the queue, but cap the list at a size that
		   will fit.
		 * @details Will get all the alarm updates,
		 * 			but checking that the estimated length of this channel should fit when serialized,
		 * 			and stopping if we estimate that we hit the endpoint of the message buffer.
		 * @param[out] group_indices On successful return, this array will contain the indices of channels
		 * 			to be processed for the message, up to this_group->Get_Channel_Count().
		 * @param[out] alarm_fit_count:  The number of alarms fitting into message buffer
		 * @param[out] estimated_message_length The estimated size that the message buffer will be after
		 * alarm updates are serialized.
		 * @return True if we estimate that we are able to fit all the alarm updates in the message, or
		 * False if we had to leave some.
		 */
		bool Get_Fitting_Alarm_Updates( uint16_t& alarm_fit_count,
										uint16_t& estimated_message_length );

		/** @brief  Estimate the length of its value when serialized.
		 * @details Will make a conservative estimate, since we don't want to overrun our message buffer.
		 * @param[in] None
		 * @return The estimated maximum length of alarms message when serialized.
		 */
		uint16_t Estimate_Serialized_Alarms_Size( void );

#endif	// IOT_ALARM_SUPPORT

		/** @brief  Given the DCI ID, get its type and estimate the length of its value when serialized.
		 * @details Will make a conservative estimate, since we don't want to overrun our message buffer.
		 *   @ToDo: See if we can leave out the timestamp, or t_ms portion at least.
		 * @param dcid The DCI ID of the channel to be estimated.
		 * @return The estimated maximum length of the channel's value when serialized.
		 */
		uint16_t Estimate_Serialized_Size( DCI_ID_TD dcid );

		/** @brief Initialize the given data structure for one message, of the given message type.
		 * @details Utility function to null out the structure and then fill the correct fields,
		 * 		including the Device UUID.
		 * @param message_data_struct: Reference to the structure to be initialized.
		 * @param message_type The type of this message (eg, CHANNEL_REALTIMES) from the IOT_DATA_TYPE enums.
		 * @return True if we were able to initialize the message and allocate its list.
		 */
		bool Initialize_Message_Data_Struct( IOT_DATA& message_data_struct, IOT_DATA_TYPE message_type );

		/** Holds our reference to the IoT Channel Database */
		const iot_target_info_st_t* m_iot_dci_database;

		/** Handle to the Low-Leve Azure connection, needed to send messages. */
		IOT_DEVICE_HANDLE m_device_handle;

		/** Holds a pointer to the publisher device's UUID string. */
		const char* m_device_uuid;

		/** The DCI interface for IOT Channels, created by this class. */
		IOT_DCI* m_iot_dci;

		/** Holds the array (Azure VECTOR) of IOT_Group_Handler instances for our groups. */
		VECTOR_HANDLE m_group_handlers;

		/** For Get_All_Fitting_Channels(), provides the starting point for searching through the channel data.
		 * Important when Get_All_Fitting_Channels() cannot fit all into the message buffer and it must be called again.
		 **/
		uint16_t m_starting_search_index;

		/** Flag to tell when we have already sent (or didn't need to send) trend data.
		 * Important when can't publish all data at once, but must cycle back and send more later. */
		bool m_trends_done;

		/** The array of DCI Owners for the Group Cadence channels. */
		// std::vector<DCI_Owner*> m_cadence_owners;

		/** Estimated length of the serialized header for Realtimes, Trend and alarms messages.
		 * Using "{"realtimes":[...]}", assuming don't have to allow for the parameters. */
		static const uint8_t MESSAGE_HEADER_LENGTH = 16;

		/** Estimated length of the serialized timestamp per channel,
		 * using `"t":1581016758,` */
		static const uint8_t SERIALIZED_TIMESTAMP_LENGTH = 15;

		/** Estimated length of the serialized milliseconds times per channel,
		 * using `"t_ms":999,`  Not currently used, since "t_ms":0 is dropped when serializing. */
		static const uint8_t SERIALIZED_MS_TIME_LENGTH = 11;

		/** Estimated length of the serialized PX White "Tag ID" per channel, plus opening brace.
		 * using "{"c":"12978",".  */
		static const uint8_t SERIALIZED_TAG_LENGTH = 14;

		/** Estimated length of the serialized text surrounding the channel's value, plus closing brace.
		 * Using `"v":"7"},`  Not including the actual value portion. */
		static const uint8_t SERIALIZED_VALUE_ITEM_LENGTH = 8;

};


/***************** Inline functions. ***************************************/

inline void IOT_Pub_Sub::Set_Device_Handle( IOT_DEVICE_HANDLE device_handle )
{
	m_device_handle = device_handle;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline IOT_DCI* IOT_Pub_Sub::Get_Iot_Dci_Interface()
{
	return ( m_iot_dci );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline const char* IOT_Pub_Sub::Get_Publisher_Device_Uuid()
{
	return ( m_device_uuid );
}

#endif
