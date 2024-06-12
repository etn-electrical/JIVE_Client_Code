/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the methods to open the IOT connection.
 *
 *	@details IOT_Net contains all the necessary methods and callbacks to open IOT connection
 *	and to generate various callbacks
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IOT_NET_H
#define IOT_NET_H

#include "iot_device.h"
#include "ssl_connection.h"
#include "IOT_Device_Command.h"
#include "OS_Task_Mon.h"
#include "DCI_Patron.h"
#include "IOT_DCI_Data.h"
#include "IOT_DCI_Config.h"
#include "IOT_Pub_Sub.h"
#include "IOT_Config.h"
#include "INT_ASCII_Conversion.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/map.h"
#include "IOT_Fus.h"
#ifndef ESP32_SETUP
#include "uC_CStack.h"
#endif

/************* Typedefs ***********************/

/**
 * Typedef for Callback function to handle different states
 * of IoT D2C commands
 */
typedef void (IOT_Device_Command::* State_Machine_Callback)( void );

/**
 * Typedef for Callback function used by the product code to send Custom Messages.
 *
 * @param iot_task_state The present state of the IOT_Net Timer Task. One of iot_net_states_t enums.
 * @param iot_device_handle The handle to the IoT connection, needed to send a message.
 * @return The Message ID if the callback sent a custom message, else 0 if not.
 */
typedef unsigned long (* Custom_IoT_Message_Callback)( uint16_t iot_task_state, IOT_DEVICE_HANDLE iot_device_handle );


/**
 ****************************************************************************************
 * @brief This is an IoT_Net class
 * @details It provides method and callbacks to initialize IOT connection.
 * @ingroup iot_init_tasks
 *
 * @li It provides public methods to
 * @li 1. Initialize and open an IOT connection.
 * @li 2. Callback to register timer.
 * @li 3. Callback to register incoming commands from the cloud.
 ****************************************************************************************
 */
class IOT_Net
{
	public:

		/** Define a set of states that this class, in the Timer Task, will progress through.
		 * Dividing the Timer Task into these states will space out the initial burst of messages
		 * so the device resources are not overwhelmed.
		 */
		enum iot_net_states_t
		{
			/** Not Connected to Azure. The default, initial state. */
			NOT_CONNECTED = 0,
			/**	TLSIO uses 3 retries for opening xio layer. We should wait for 3 tries to get over before declaring
			   FORCE_TO_CLOSE	*/
			AZURE_TLSIO_RETRYING,
			/** Call for iot_close() when set to this state. Set by Azure callback. */
			FORCE_TO_CLOSE,
			/** Transient state after calling iot_open() but before we believe that the connection is open. */
			IOT_OPENED,
			/** First publishing state, set once after the connection is established. */
			SEND_DEVICE_TREE,
			/** Second publishing state, following sending of the Device Tree.
			 *  Used to publish SeeMe/realtimes for all channels, regardless of whether they were updated.
			 *  Provides an initial value for all channels to PX White when the connection is established. */
			SEND_ALL_CHANNELS,
#ifdef IOT_ALARM_SUPPORT
			/** The running state which runs at higher priority than any other data publishing state,
			 *  where the Timer Task will see if there are any alarm updates available. It will stay in this state
			 *  till all alarms are published successfully.
			 **/
			SEND_ALARMS,
#endif	// IOT_ALARM_SUPPORT
			/** The normal, running state, where the Timer Task will see if any of the groups' Change Trackers
			 *  signal that some channel(s) havve been updated and need to be published as SeeMe/realtimes.
			 *  Also any group that publishes StoreMe/trends will be checked in this state, on their cadence interval.
			 **/
			SEND_UPDATED_CHANNELS,
			/** After an UpdateFirmware command has been accepted, this is the state that causes us to send
			 *  the TransferImage D2C command to Azure.
			 *  After sending that, we will resume the previous state. */
			SEND_TRANSFER_IMAGE,
			/** At step points in the FUS over IoT process, this is the state that causes us to send
			 *  the DeviceCommandStatus message (D2C command) to Azure. This will be requested by the
			 *  IOT_Device_Command class members at least at the beginning and end of the FUS work.
			 *  After sending this, we will resume the previous state. */
			SEND_DEVICE_COMMAND_STATUS,
			// More may be added
		};

		/** Holds the current state for publishing in the Timer Task. */
		iot_net_states_t m_timer_task_state;

		/** Holds the previous state of the Timer Task, for when we want to revert. */
		iot_net_states_t m_previous_task_state;

		/** The Low-Level Azure Device Handle, needed to send messages. */
		IOT_DEVICE_HANDLE m_device_handle;

		/** Holds the state of scheduled operation required for FUS over IOT */
		IOT_Device_Command::iot_device_command_scheduled_states_t m_scheduled_state;

		/** Function pointer for state machine callback handler */
		State_Machine_Callback m_state_machine_callback;

		/** Function pointer for custom message callback handler */
		static Custom_IoT_Message_Callback m_custom_message_callback;

		/** Optional time(milliseconds) mark at which we will "reset" the IOT_Device_Command class.
		 * (Eg, to cancel a hung Firmware Update session.) */
#ifdef ESP32_SETUP
		uint64_t m_reset_device_command_time_ms;
#else
		time_t m_reset_device_command_time_ms;
#endif

		/**
		 * @brief Constructor.
		 * @details Initialize and open an IOT connection between device and the cloud.
		 * @param iot_database: IOT_DCI database, static structure listing all IoT channels and cadence groups in the
		 * DCI.
		 * @param iot_config_struct The interface to the DCI data that configures IoT operation.
		 * @param cadence_owners Handle to the array (Azure vector) of DCI owners for the group cadence channels.
		 *              These are in the array in the order of the groups in iot_database.
		 */
		IOT_Net( const iot_target_info_st_t* iot_database,
				 const IOT_DCI_Config::iot_config_param_t* iot_config_struct,
				 VECTOR_HANDLE cadence_owners );

		/** Simple XTOR. Releases config variables. */
		~IOT_Net();


		/**
		 * @brief Function used by the Group Handler's Timer Task to mark its group as "ready".
		 * @details Adds (ORs) the enum into our m_groups_ready collection.
		 * @param group_marker: Enum from IOT_GROUP_MARKERS that marks which group is now ready.
		 */
		static void Mark_Group_Ready( uint16_t group_marker );

		/**
		 * @brief Function used by the Group Handler's Timer Task to clear the group if it was ready
		 * @details Clears the enum from our m_groups_ready collection.
		 * @param group_marker: Enum from IOT_GROUP_MARKERS
		 */
		static void Clear_Group_Ready( uint16_t group_marker );

		/** Getter for our IOT_Pub_Sub class instance, to help with sending messages.
		 * @return Pointer to our m_pub_sub pointer to the IOT_Pub_Sub class.
		 */
		IOT_Pub_Sub* Get_Pub_Sub_Instance();

		/** Utility that checks a request to change state, and either permits or rejects the request.
		 * If the change is accepted, the previous state is retained, in case it is needed later.
		 * @param new_timer_task_state The enum for the new state being requested.
		 * @return True if the change is accepted, else False if cannot be done at this time.
		 */
		bool Request_Timer_Task_State( iot_net_states_t new_timer_task_state );

		/** Schedules a new device command FUS state after the given delay.
		 * The Timer_Task will call this after 1s or sooner after this delay expires.
		 * This actual delay i.e 1s or sooner is set by messageThread.
		 * @param delay_millisecs The delay to use, in milliseconds.
		 *                   If set to 0, any pending Reset will be canceled.
		 * @param new_scheduled_state The state to be executed when the timer expires.
		 */
		void Schedule_Device_Command( time_t delay_millisecs,
									  IOT_Device_Command::iot_device_command_scheduled_states_t new_scheduled_state );

		/** @brief Registers a callback for scheduled state machine handler
		 * and callback for d2c command state machine handler
		 * @param scheduled_callback scheduled state machine handler function
		 */
		void Register_State_Machine_Callback( State_Machine_Callback scheduled_callback );

		/** @brief Unregisters a callback for scheduled state machine handler
		 * and callback for d2c command state machine handler
		 * @param scheduled_callback scheduled state machine handler function
		 */
		void Unregister_State_Machine_Callback( State_Machine_Callback scheduled_callback );

		/** @brief Registers a callback for sending custom messages.
		 * @param custom_message_callback scheduled state machine handler function
		 */
		static void Register_Custom_IoT_Message_Callback( Custom_IoT_Message_Callback custom_message_callback );

		/** @brief Unregisters the callback for custom messages
		 */
		static void Unregister_Custom_IoT_Message_Callback();

	private:

		/** Function that initializes the parts of the IoT code that are only initialized once,
		 * and only if IoT is Enabled. */
		void Do_Onetime_Initialization( void );

		/**
		 * @brief Static function to redirect the call to run our Connection Check timer task.
		 * @param param: An opaque argument that resolves to a pointer to "this".
		 */
		static void Connection_Check_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Steady timer task that checks to see if we need to connect/reconnect.
		 * @details This timer task is invoked periodically at a rate of CONNECT_TIMER_MS (eg, 10s).
		 *  First it does some preliminary checks:
		 *    -# Have we lost the network connection?
		 *    -# Has the IoT configuration changed?
		 *    -# Are we stalled, waiting to complete the connection (eg, hung up in TLS handshaking)?
		 *  Then process the two states of interest here
		 *    - If we want to force the connection to close, and it appears to be open,
		 *          call for iot_close(), then set the state for next to reconnect.
		 *    - If we are not currently connected, it will attempt to reconnect us.
		 *  We keep this task intentionally small and quick, since it runs under the task for all
		 *  FreeRTOS timers.
		 */
		void Connection_Check_Timer_Task( void );


		/** Function which attempts to open the connectiong to Azure, and register the necessary callbacks.
		 * The SDK will create its own connection structure, which will hold the pointers to the callback
		 * functions, among other data.
		 * If the attempt to connect appears to succeed,
		 * the handle to the Low-Level Azure connection will be stored in m_device_handle,
		 * and IOT_Pub_Sub (and its IOT_Group_Handlers) will be initialized.
		 * This function must be called again to reconnect.
		 * @return True if the attempt connect appeared to succeed, False if it is a known failure.
		 *  Be aware that success here is conditional; until we have a message acknowledgement
		 *  from Azure, we don't know for sure that the connection has succeeded.
		 */
		bool Open_Connection_And_Register( void );

		/** Function which checks to see if we are connected to Azure.
		 * First checks that the network is Up, then that our status shows connected.
		 * @return True if we appear to be connected, False if not.
		 */
		bool Is_Connected( void );

		/** Builds and sends a DeviceTree message.
		 * @return Results of the iot_send(): True if data was successfully sent, else False.
		 */
		bool Send_Device_Tree();


		/**
		 * @brief Timer callback, called from the IoT SDK's messageThread every second (or sooner).
		 * @details This is just a shim to call the C++ Timer_Task.
		 *          This callback conforms to the IoT SDK type TimerCallback.
		 * @param device_handle: Handle of the Low-Level Azure IOT connection.
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Net.
		 */
		static void Timer_static( IOT_DEVICE_HANDLE device_handle, void* owner_context );

		/**
		 * @brief Timer task, called from the messageThread every second (or sooner).
		 * @details This callback is given the chance to run every second, or sooner if
		 *          any of the group cadence timers expire and that group has data to be published.
		 *          Each time, we check all the groups to see if they have set a marker in m_groups_ready,
		 *          and if so, determine which type(s) of messages they want published, and publish
		 *          their data.
		 * @param device_handle: Handle of the Low-Level Azure IOT connection.
		 */
		void Timer_Task( IOT_DEVICE_HANDLE device_handle );

		/**
		 * @brief Connection status callback.
		 * @details This is just a shim to call the C++ Connection_Status_Update.
		 *          This callback conforms to the IoT SDK type ConnectionStatusCallback.
		 * @param device_handle: Handle of the IOT connection.
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Net.
		 * @param statusCode: Status of the IOT connection.
		 * @param reasonCode: Reason of the status of the connection.
		 */
		static void Connection_Status_Update_static( IOT_DEVICE_HANDLE device_handle, void* owner_context,
													 IOTHUB_CLIENT_CONNECTION_STATUS statusCode,
													 IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode );


		/**
		 * @brief Connection status callback.
		 * @details This is the function that handles changes in the connection status.
		 * @param[in] device_handle: Handle of the IOT connection.
		 * @param[in] statusCode: Status of the IOT connection.
		 * @param[in] reasonCode: Reason of the status of the connection.
		 */
		void Connection_Status_Update( IOT_DEVICE_HANDLE device_handle,
									   IOTHUB_CLIENT_CONNECTION_STATUS statusCode,
									   IOTHUB_CLIENT_CONNECTION_STATUS_REASON reasonCode );


		/** Holds a reference to the DCI data for the channels and groups. */
		const iot_target_info_st_t* m_iot_database;

		/** Holds a reference to the class we create to Subscribe to DCI channel data
		 * and publish messages using that data. */
		IOT_Pub_Sub* m_pub_sub;

		/** Handle to the DCI Patron for IoT data. */
		DCI_Patron* m_iot_patron;
		/** The source ID sequentially assigned for IoT updates to the DCI. */
		DCI_SOURCE_ID_TD m_iot_source_id;

		/** The interface to the DCI data that configures IoT operation. */
		IOT_DCI_Config* m_iot_dci_config;

		/** Handle (pointer) to the array (Azure VECTOR) of DCI owners of the group cadence channels. */
		VECTOR_HANDLE m_cadence_owners;

		/** Flag that indicates that we have done the initialization steps only done if IoT is Enabled,
		 * and only done once, even if IoT becomes Disabled and then re-Enabled. */
		bool m_is_initialized_once;

		/** The structure for the main (publisher) device's DeviceTree entry. */
		IOT_DATA_DEVICE_ITEM m_device;

		/** The Babelfish SSL (TLS) context class for the IoT connection.
		 * Needed only for its private key and to hold the trusted Azure Certificate Authority cert. */
		Ssl_Context* m_bf_ssl_context;

		/** Holds the last connection status "reasonCode" returned from Azure. */
		IOTHUB_CLIENT_CONNECTION_STATUS_REASON m_lastReasonCode;

		/** A simple bit-masked list of each cadence group that is ready to be processed.
		 * The Timer task for each cadence group will set its bit when it is ready. */
		static uint16_t m_groups_ready;

		/** Holds the index of the group that needs to be published now.
		 * Once that group's data has all been published, will advance to the next. */
		uint16_t m_current_group_index;

		/** Holds the timer instance that will manage the (re)connect attempts. */
		BF_Lib::Timers* m_connection_check_timer;

		/** Counts up times that we are still in state IOT_OPENED.
		 * Also used to count times we check and fail to have an available message buffer. */
		uint16_t m_iot_open_stall_count;

		/** Sets the delay we will use between connection attempts, to 10s.
		 * In use (eg, TIMERS_TIME_TD) apparently 1 Tick = 1 ms. */
		const OS_TICK_TD CONNECT_TIMER_MS = 10000;

		/** The limit on m_iot_open_stall_count; if we hit this, we must force a reconnect.
		 * Set to be 9 * 10s = 90s; seems long enough that if we don't advance beyond
		 * IOT_OPENED, something has gone wrong. */
		const uint16_t MAX_IOT_OPEN_STALLS = 9;

		/** The other limit on m_iot_open_stall_count, after the connection is established.
		 * Set the limit to 3 successive times (ie, over 30s); if we can't get a message buffer
		 * in that time, something has gone wrong and we should reconnect. */
		const uint16_t MAX_NO_IOT_MSG_BUFFERS = 3;
};


/***************** Inline functions. ***************************************/

inline IOT_Pub_Sub* IOT_Net::Get_Pub_Sub_Instance()
{
	return ( m_pub_sub );
}

#endif
