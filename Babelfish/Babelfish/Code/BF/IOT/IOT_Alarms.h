/**
 *****************************************************************************************
 * @file
 * @brief This file contains class that handles alarms over IOT
 * @details IoT_Alarms contains all the necessary methods required to publish alarms over
 * IOT and calls Alarm APIs
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IOT_ALARMS_H
#define IOT_ALARMS_H


#include "iot_device.h"
#include "Includes.h"	///< Need to include when Timers_Lib.h is used
#include "Timers_Lib.h"
#include "IOT_Device_Command.h"
#include "Event_Msg_Struct.h"
#include "Event_Interface_Struct.h"
#include "IOT_Config.h"
#include "Event_Config.h"
#include "IOT_Event_Interface.h"
#include "Babelfish_Assert.h"
#include "Alarm_DCI.h"
#include "IOT_Alarms_Cadence.h"

using namespace BF_Event;

#ifdef IOT_ALARM_SUPPORT

/**
 ****************************************************************************************
 * @brief Class to handle alarms over IOT
 *
 * @details  This class provides all the methods required for handling alarms over IOT
 *
 * @ingroup alarms_over_iot
 ****************************************************************************************
 */
class IOT_Alarms
{
	public:
		/**
		 * @brief Constructor.
		 * @details This constructor initializes the member variables and starts alarm cadence timer.
		 * @param iot_event_interface: Holds event interface instance
		 * @param pub_cadence_owners: Array of cadence rate owners
		 */
		IOT_Alarms( IOT_Event_Interface* iot_event_interface, DCI_Owner** pub_cadence_owners );

		/** Default destructor for this class. */
		~IOT_Alarms( void );

		/**
		 * @brief Function to get alarm update count
		 * @details This returns the count of new entries of alarm updates
		 * which are yet(or ready) to be published
		 * @param[in] None
		 * @param[out] None
		 * @return Count of alarm update
		 */
		uint32_t Get_Alarm_Update_Count( void );

		/**
		 * @brief Function to fill in alarms message
		 * @details This fills in alarms message structure with alarm config and alarm status update
		 * information
		 * @param[out] alarm_item : Alarm message structure
		 * @param[out] threshold_value : Threshold value as per config
		 * @param[in] log_data : Saved alarm log structure
		 * @return unix epoch timestamp of alarm update
		 */
		time_t Fill_Alarms_Message( IOT_DATA_ALARM_ITEM& alarm_item, STRING_HANDLE& threshold_value,
									event_log_t log_data );

		/**
		 * @brief Function that returns IOT_Alarms instance m_iot_alarm_handle
		 * @param[in] None
		 * @param[out] None
		 * @return IOT_Alarms instance m_iot_alarm_handle
		 */
		static IOT_Alarms* Get_IOT_Alarms_Handle_Static( void );

		/**
		 * @brief Function that that clears alarm history
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return None
		 */
		void Clear_Alarm_History( IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that acknowledges the active alarms
		 * @details This functions parses a generic AckAlarms C2D command as well as swagger API C2D command
		 * /api/v1/alarms/acknowledge, acknowledge active alarms
		 * mentioned in the command payload and responds with success/failure
		 * @param[in] iot_map_handle: Map handle for map of key:value parameters in the request.
		 * @param[in] command: IOT C2D command structure
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return None
		 */

		void Ack_Alarms( MAP_HANDLE iot_map_handle, const IOT_DEVICE_COMMAND* command,
						 IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that that resets/closes alarms This will be useful for dangling
		 * Alarms and when we have CHANGE_OF_VALUE type Alarm/Event Rules via Channel Monitoring]
		 * @details This functions parses a generic CloseAlarms C2D command, closes/resets all the alarms
		 * @param[in] iot_map_handle: Map handle for map of key:value parameters in the request.
		 * @param[in] command: IOT C2D command structure
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return None
		 */
		void Close_Alarms( MAP_HANDLE iot_map_handle, const IOT_DEVICE_COMMAND* command,
						   IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that that resets/closes alarms as per given alarm_ids
		 * @details This function closes/resets all the alarms given in the alarm_id_arr
		 * and fills in command_response message
		 * @param[in] alarm_id_arr: Array containing alarm_ids to be closed
		 * @param[in] arr_size: Size of alarm_id_arr
		 * @param[in] event_req: event request information
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return True on success, else false
		 */
		bool Close_Given_Alarms( uint32_t* alarm_id_arr, size_t arr_size, user_info_req_t event_req,
								 IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that fills in command_response as per error codes
		 * @details This function fills in command_response as per error codes defined by enum event_op_status_t
		 * @param[in] status: Error codes reported for alarm
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return None
		 */
		void Alarm_Error_Response( event_op_status_t status,
								   IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that closes all the alarms
		 * @details This function closes all the alarms and fills in command_response
		 * @param[in] event_req: event request information
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return True on success, else false
		 */
		bool Close_All_Alarms( user_info_req_t event_req, IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Function that reads an alarm update log
		 * @details This function will read an alarm update structure from logging memory space
		 * @param[in] log_events_req : Structure containing required information to read event updates
		 * @param[out] log_events_resp : Structure containing alarm update
		 * @return None
		 */
		void Get_Event_Data( log_events_req_t log_events_req, log_events_resp_t log_events_resp );

		/**
		 * @brief Function that initializes head and tail indices on power-up
		 * @param[in] None
		 * @param[out] None
		 * @return None
		 */
		void Init_Head_Tail_Indices( void );

		/**
		 * @brief Function that initializes cadence rate as per severity levels
		 * @param[in] None
		 * @param[out] None
		 * @return None
		 */
		void Init_Cadence( void );

		/*
		 * @brief Function to get maximum length of rule name. This is required
		 * for estimating the alarm payload length
		 * @param[in] None
		 * @param[out] None
		 * @return Maximum length of rule name
		 */
		uint8_t Get_Max_Rulename_Len( void );

		/*
		 * @brief Function to get maximum value of trigger type. This is required
		 * for estimating the alarm payload length
		 * @param[in] None
		 * @param[out] None
		 * @return Maximum value of trigger type
		 */
		uint16_t Get_Max_Trigger_Type( void );

		/*
		 * @brief Function to get maximum length of printed alarm priority. This is
		 * required for estimating the alarm payload length
		 * @param[in] None
		 * @param[out] None
		 * @return Maximum value of alarm priority
		 */
		uint8_t Get_Max_Priority( void );

		static const unsigned long ALARM_SCHEMA_VERSION = 1;
		static const char* ALARM_SOURCE;
		static const uint8_t ACK_LEVEL = 1;	///< TODO - This should come from Alarms interface
		static const uint8_t MAX_NOTE_LENGTH = NOTE_MAX_LENGTH;	///< From alarm config
		static const uint8_t MAX_USER_LENGTH = USER_NAME_MAX_LENGTH;///< From alarm config

		///< Head and tail indices to keep track of alarms published. They're initialized to 1(not 0)
		uint32_t m_last_pub_head_index;

		///< Flag that takes care of head and tail index initialization after power-up
		bool m_head_tail_init_done;

		static const char* ALARM_MESSAGE_TEMPLATE;

		DCI_Owner* m_last_pub_idx_owner;

	private:

		/**
		 * @brief Function to fill trigger information in alarms message
		 * @details This fills in trigger structure with trigger config information
		 * @param[out] trigger_item : Trigger message structure
		 * @param[out] threshold_value : Threshold value as per configuration
		 * @param[in] rule_config : Rule config structure
		 * @return None
		 */
		void Fill_Trigger_Info( IOT_DATA_ALARM_TRIGGER_ITEM*& trigger_item, STRING_HANDLE& threshold_value,
								rule_config_t rule_config );

		/**
		 * @brief Function that extracts alarm id from alarm id string
		 * @details The alarm id string is in the UUID format ,for e.g "00000000-0000-0000-0000-000000000001"
		 * This function removes the prefix and then converts the string to integer
		 * @param[in] alarm_id_str: Alarm id string , for e.g "00000000-0000-0000-0000-000000000001"
		 * @param[out] alarm_id: extracted alarm id in integer format
		 * @return True if extraction is successful, else false
		 */
		bool Extract_Alarm_Id( const char* alarm_id_str, uint32_t& alarm_id );

		/**
		 * @brief Function that validates alarm sr.no provided in the command payload
		 * @details This function validates that each alarm id has numeric sequence number(Sr.no/key)
		 * @param[in] param_key: Sr.no(in string format) provided in payload
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return True sequence number(Sr.no/key) is numeric, else false
		 */
		bool Validate_Alarm_Sr_No( const char* param_key, const char* param_value,
								   IOT_DEVICE_COMMAND_RESPONSE* command_response );


		/**
		 * @brief Holds "this" instance of IOT_Alarms class */
		static IOT_Alarms* m_iot_alarm_handle;

		/**
		 * @brief Holds "this" instance of Alarm_Interface class */
		IOT_Event_Interface* m_iot_event_interface;

		static const char ALARM_ID_PREFIX[];

		DCI_Owner** m_pub_cadence_owners;

		/** Holds the array (Azure VECTOR) of IOT_Alarms_Cadence instances */
		VECTOR_HANDLE m_cadence_handlers;

		static const uint16_t LOG_ERASE_TIME_MS = 5000U;
};

#endif	// IOT_ALARM_SUPPORT
#endif	// IOT_ALARMS_H
