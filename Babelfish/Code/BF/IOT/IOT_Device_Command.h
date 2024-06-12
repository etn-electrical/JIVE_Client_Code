/**
 *****************************************************************************************
 *  @file
 *	@brief This file defines the class that handles C2D Device Commands.
 *
 *	@details IOT_Device_Command contains all the necessary methods for responding to
 *              C2D DeviceCommands, including the DLM UpdateFirmware APIs.
 *              It also manages some related D2C messages.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IOT_DEVICE_COMMAND_H
#define IOT_DEVICE_COMMAND_H
// #include "DCI_Patron.h"
// #include "IoT_DCI.h"
// #include "IOT_DCI_Data.h"
// #include "Change_Tracker.h"
// #include "Bit_List.h"
#include "iot_device.h"
// #include "azure_c_shared_utility/vector.h"
#include "FUS.h"

class IOT_Net;
class IOT_Pub_Sub;
class IOT_DCI;

// Define some HTTP Status codes that we might use
// 2xx Success
#define HTTP_STATUS_OK  200
#define HTTP_STATUS_CREATED 201
// 4xx Client Failure
#define HTTP_STATUS_BAD_REQUEST 400
#define HTTP_STATUS_NOT_FOUND 404
#define HTTP_STATUS_PAYLOAD_TOO_LARGE 413
#define HTTP_STATUS_BUSY    420
#define HTTP_STATUS_UNPROCESSABLE 422
#define HTTP_STATUS_LOCKED 423
#define HTTP_STATUS_FORBIDDEN 403

// 5xx Server Failure
#define HTTP_STATUS_INTERNAL_SERVER_ERROR 500
#define HTTP_STATUS_NOT_IMPLEMENTED 501
#define HTTP_STATUS_SERVICE_UNAVAILABLE 503
#define HTTP_STATUS_GATEWAY_TIMEOUT 504
#define HTTP_STATUS_INSUFFICIENT_STORAGE 507

/* Multiplier to convert to milliseconds */
#define MULTIPLIER_1000MS   1000


/************* Typedefs ***********************/

/**
 * Typedef for the Product-level Callback function to handle product-defined Cloud-to-Device Device Commands.
 * This is used if the product wants to add its own C2D Device Commands, and the callback that uses this form
 * will be invoked by IOT_Device_Command::Execute_C2D_Command() if none of the built-in (Babelfish) commands
 * can handle the request.
 * @ingroup iot_c2d_commands
 *
 * @param command_method_name  The name of the C2D Device Command (method) request
 * @param iot_map_handle: Map handle for map that contains key:value pairs of the command parameters.
 * @param[out] command_response: The structure for the response to this command;
 *              the command handler must set the status and message in the response.
 *              If the given command name is not implemented by the product, the status
 *              should be set to HTTP_STATUS_BAD_REQUEST, and the message to "Not Implemented".
 *              Note: The allocated fields for the IOT_DEVICE_COMMAND_RESPONSE structure will be freed
 *                  by the SDK after serializing the response for Azure.
 *                  The implementor's callback must allocate any const char* members used.
 */
typedef void (* C2D_Poduct_Command_Callback)( const char* command_method_name, MAP_HANDLE iot_map_handle,
											  IOT_DEVICE_COMMAND_RESPONSE* command_response );

/**
 * Typedef for the handler for one UpdateFirmware parameter (here, the "key" to the parameter).
 * This is used to validate or otherwise process the parameter value.
 * If the value is recognized as invalid, an appropriate status and message will be placed in
 * the command_response structure.
 * @ingroup iot_c2d_commands
 *
 * @param param_key  The name ("key") for this parameter
 * @param param_value: The value for this parameter.
 * @param[out] command_response: The structure for the response to this command,
 *              which is only altered if the processing indicates an invalid request.
 *              In that failure case, the command handler must set the status and message in the response.
 *              Note: The allocated fields for the IOT_DEVICE_COMMAND_RESPONSE structure will be freed
 *                  by the SDK after serializing the response for Azure.
 *                  The implementor's callback must allocate any const char* members used.
 * @return True on success (valid value for this parameter), else False if invalid and the
 *              response should be returned now.
 */
typedef bool (* Image_Param_Handler)( const char* param_key, const char* param_value,
									  IOT_DEVICE_COMMAND_RESPONSE* command_response );

/** Defines a structure that maps a parameter name to its handler function. */
typedef struct update_firmware_handler_s
{
	const char* param_name;				/**< Name of the parameter */
	Image_Param_Handler handler;		/**< Function pointer for the verification handler for this parameter */
} update_firmware_handler_t;

/** Defines a structure to be used for an UpdateFirmware job, holding the pertinent metadata. */

typedef struct fus_job_control_s
{
	char job_uuid[IOT_MAX_DEVICE_UUID_SIZE];	/**< UUID for the job in progress */
	char job_who[IOT_MAX_DEVICE_UUID_SIZE];		/**< UUID for "who" requested this job */
	BF_FUS::FUS* fus_handle;					/**< Handle we need to the FUS interface. */
	uint8_t processor_index;					/**< Index for the Processor for this FUS job. */
	uint8_t image_index;						/**< Index for the Image for this FUS job. */
	uint32_t image_start_address;				/**< Start address of image which is to be upgraded */
	bool is_reboot_required;					/**< Flag indicating that App firmware upgrade is in progress */
	// Fields for updating DeviceCommandStatus:
	const char* device_uuid;					/**< Pointer to our main device's UUID. */
	const char* task_text;						/**< The name of the job task step (eg, "UpdateFirmware") */
	IOT_COMMAND_STATUS step_status;				/**< The status of the step, from the iot_command_status enums. */
	const char* step_description;				/**< A free-form text description of the step. */
} fus_job_control_t;



/**
 * @brief The 'C' callback for C2D Device Commands, registered with the IoT SDK.
 * @details This callback will create an IOT_Device_Command instance to handle the command.
 * This method conforms to the IoT SDK's C2DDeviceCommandCallback function type.
 * @ingroup iot_c2d_commands
 *
 * @param device_handle: Handle of the IOT connection.
 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Net.
 * @param command: The Request message for this command, from the cloud.
 * @param[out] command_response: Our Response to the command from the cloud.status
 */
extern "C" void C2D_Device_Command_Callback( IOT_DEVICE_HANDLE device_handle, void* owner_context,
											 const IOT_DEVICE_COMMAND* command,
											 IOT_DEVICE_COMMAND_RESPONSE* command_response );


/** Class to handle C2D Device Commands.
 * This class provides a method that determines which function should handle the C2D request,
 * if it is one of the defined Babelfish command handlers, and then returns the response
 * from that handler.
 * Normally this class is instantiated by each command request instance; some members are static
 * so they can persist across commands.
 *
 * @ingroup iot_c2d_commands
 */
class IOT_Device_Command
{
	public:

		/** Constructor.
		 * @param iot_net: Pointer to our IOT_Net instance.
		 */
		IOT_Device_Command( IOT_Net* iot_net );

		/** Destructor */
		~IOT_Device_Command() {}

		/* Different states that can be scheduled after particular delay. For eg, When validate
		 * operation in FUS is called, it returns "in progress" status and provides us with wait
		 * time. We need to call validate operation again after wait time is over. In this case,
		 * we schedule  */
		enum iot_device_command_scheduled_states_t
		{
			SCHEDULED_NONE = 0,
			/** Schedule an erase of the Scratch area, so we can write received data into it. */
			SCHEDULED_ERASE_FUS,
			/** Schedule a reset operation of this class, ie, cancel any Firmware Update job */
			SCHEDULED_RESET_FUS,
			/** Schedule image validation operation */
			SCHEDULED_VALIDATE_FUS,
			/** Schedule to get the status of commit operation */
			SCHEDULED_STATUS_FUS,
			/** Schedule system reset. This will be required while upgrading App image */
			SCHEDULED_SYSTEM_RESET_FUS
		};

		/**
		 * @brief Overall handler for a C2D Device Command, which processes and then responds to the command.
		 * @details This is the callback function to handle incoming commands from the cloud.
		 * @param device_handle: Handle of the IOT connection.
		 * @param command: The Request message for this command, from PXWhite.
		 * @param[out] command_response: Pointer to the structure for our Response to the command.
		 */
		void Execute_C2D_Command( IOT_DEVICE_HANDLE device_handle, const IOT_DEVICE_COMMAND* command,
								  IOT_DEVICE_COMMAND_RESPONSE* command_response );


		/** Registers the Product-level Callback function to handle product-defined Cloud-to-Device Device Commands.
		 * The function registered here will be invoked by IOT_Device_Command::Execute_C2D_Command()
		 * if none of the built-in (Babelfish) commands can handle the request.
		 *
		 * @param callback A pointer to the callback function of type C2D_Poduct_Command_Callback.
		 *      Though not normally necessary, this can be called with callback = nullptr to unregister the function.
		 */
		static void Register_C2D_Command_Callback( C2D_Poduct_Command_Callback callback );

		/** Reset the operation of this class, ie, cancel any Firmware Update job.
		 * For now, this means to clear the m_job_control structure, if it is valid.
		 * This helps prevent us from getting stuck with a valid m_job_control after a job
		 * has actually timed out, and would prevent us from accepting a new job. */
		void Reset();

		/** Assemble and send the indicated D2C command to PXWhite.
		 * The job control structure contains the information necessary to fill out the indicated command.
		 * @param device_handle: Handle of the IOT connection.
		 * @param command_type: The type for this command, one of the IOT_COMMAND_TYPE enums.
		 * @return True if successfully sent, else False if failed to serialize or send.
		 */
		bool Send_D2C_Command( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND_TYPE command_type );

		/** Gets the value of constant private member m_COMMAND_TIMEOUT_SECS of this class
		 * @return Constant value of m_COMMAND_TIMEOUT_SECS
		 */
		time_t Get_Command_Timeout_Seconds( void );


		/** Sets the arguments for, and requests that a DeviceCommandStatus update be sent.
		 * Saves the arguments in our Job Control structure, and schedules the update with the IOT_Net code.
		 * The IOT_Net code will call back, and the update will actually be sent at that time.
		 *
		 * @param task_text     The name of the task step; must be "UpdateFirmware" for a final step,
		 *                      can be anything before that (eg, "Step 1 of 3")
		 * @param step_status   The status of the step, from the iot_command_status enums.
		 * @param step_description   A free-form text description of this step.
		 */
		void Update_Device_Command_Status( const char* task_text, IOT_COMMAND_STATUS step_status,
										   const char* step_description );

		/** This function handles different states of scheduled FUS over IOT operations
		 * and also different states of IoT D2C commands
		 */
		void State_Machine_Handler( void );


		/** Pointer to our IOT_Net instance; lets us access the rest of the IoT classes, as needed. */
		IOT_Net* m_iot_net;

		/** Pointer to the IOT_Pub_Sub instance, which helps with sending messages. */
		IOT_Pub_Sub* m_pub_sub;

		/** Pointer to our IoT to DCI interface, that helps us look up channel data given IoT Tags. */
		IOT_DCI* m_iot_dci;

		/** Holds an instance of an UpdateFirmware job's control data.
		 * Initialized at the start of a job, deleted when done. */
		static fus_job_control_t* m_job_control;

	private:

		/****************** Command Handlers *****************************/

		/**
		 * @brief Sets the Params data for Channel values for the SetChannelValues command.
		 * @details This function Sets the values from Cloud to the DCIDs.
		 * @param iot_map_handle: Map handle for map that contains key:value pairs of tag:values
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 */
		void Set_Params( MAP_HANDLE iot_map_handle, IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Gets the Params data for Channel values for the GetChannelValues command.
		 * @details This function Sets the values from Cloud to the DCIDs.
		 * @param iot_map_handle: Map handle  for map that contains tag list
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 */
		void Get_Params( MAP_HANDLE iot_map_handle, IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/****************** Send D2C Command functions *****************************/

		/** Assemble and send the TransferImage D2C command to PXWhite from the job control data.
		 * The job control structure contains the job UUID needed for this command.
		 * @param device_handle: Handle of the IOT connection.
		 * @param command Pointer to the structure that will hold the command information.
		 * @return True if successfully sent, else False if failed to serialize or send.
		 */
		bool Send_Transfer_Image_Command( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND* command );

		/** Assemble and send the DeviceCommandStatus message (D2C command) to PXWhite from the job control data.
		 * The job control structure contains the job status information needed for this command.
		 * @param device_handle: Handle of the IOT connection.
		 * @param command Pointer to the structure that will hold the command information.
		 * @return True if successfully sent, else False if failed to serialize or send.
		 */
		bool Send_Device_Command_Status( IOT_DEVICE_HANDLE device_handle, IOT_COMMAND* command );


		/****************** Command handling utility functions *****************************/

		/**
		 * @brief Function to unpack the command received from cloud and store the params in map.
		 * @details This is a function that extracts params from cloud-to-device command and stores them in map.
		 * @param handle: Handle of map.
		 * @param command: Request command from the cloud.
		 * @param command_params_org The original string containing the command parameters
		 * @return True for success and false for fail
		 */
		bool Unpack_Cmd_Params_And_Store_In_Map( MAP_HANDLE handle, const IOT_DEVICE_COMMAND* command,
												 char* command_params_org );

		/**
		 * @brief Function to store tag list in map.
		 * @details This is a function that extracts GetChannelvalues cloud-to-device command which contains list of
		 * params and stores them in map.
		 * Here tag is stored as key and dummy values "0" is stored as value in map
		 * @param handle: Handle of map.
		 * @param extracted_str: String containing extracted list of tags.
		 * @return True for success and false for fail
		 */
		bool Store_Tag_List_In_Map( MAP_HANDLE handle, char* extracted_str );

		/**
		 * @brief Function to store tag-value pair in map.
		 * @details This is a function that extracts SetChannelvalues cloud-to-device command which contains list of
		 * params and stores them in map.
		 * Here tag is stored as key and value to be set is stored as value in map
		 * @param handle: Handle of map.
		 * @param extracted_str: String containing extracted tag-value pairs.
		 * @return True for success and false for fail
		 */
		bool Store_Tag_Value_Pair_In_Map( MAP_HANDLE handle, char* extracted_str );

		/**
		 * @brief Function to extract string between two delimiters.
		 * @details This is a function that extracts string between two delimiters,for e.g if the src string is {1234},
		 * dest string will be 1234.
		 * @param[out] dest: Stores extracted string .
		 * @param src: Stores source string containing delimiters that is to be extracted. [ Note: Src string will be
		 * altered by this function.
		 * So avoid passing same Src string if this function is called multiple times ]
		 * @param delimiter_start: Start delimiter that is to be removed.
		 * @param delimiter_end: End delimiter that is to be removed.
		 * @return True for success and false for fail
		 */
		bool Extract_String_Between_Delimiter( char*& dest, char* src, char delimiter_start, char delimiter_end );

		/**
		 * @brief Check if all of the channel tags are valid
		 * @details This function checks whether all of the channel tags requested by cloud are present in device
		 * database i.e DCID or not
		 * @param handle: Map handle  for map that contains tags
		 * @param[out] count: Count of key:value pairs in map
		 * @return True if all the tags are valid and false even if a single tag is found invalid.
		 */
		bool Check_Tag_Validity( MAP_HANDLE handle, size_t* count );

		/** Count of C2D Device Command messages that we received. */
		static uint32_t m_c2d_message_count;

		/** Pointer to the optional Product-level C2D Device Command callback */
		static C2D_Poduct_Command_Callback m_c2d_product_command_callback;


		/** Define the chunk_size that we will request for ImageTransfer messages.
		 * This is the size of the binary (Base64-decoded) bytes per message.
		 * This should not exceed MAX_CHUNK_SIZE (from FUS_Config.h).
		 * This size must also not be greater than 5500 (Azure DirectMethod buffer size limitation).
		 * This will affect the size of the C2D ImageTransfer messages, and thus affects the time it
		 * takes to transfer the full image (larger is faster). */
#if ( defined ( uC_STM32F767_USAGE ) || defined ( ESP32_SETUP ) )
		static const size_t m_TRANSFER_IMAGE_CHUNK_SIZE = 1536;
#else
		static const size_t m_TRANSFER_IMAGE_CHUNK_SIZE = 512;
#endif

		/** Define the delay to request that PXWhite observe before it starts sending us
		 * the ImageTransfer messages.
		 * Unfortunately, it seems that PX White currently uses this delay bewteen
		 * every ImageTransfer message it sends us, not just the first one,
		 * so setting this to the minimum of 0.
		 */
		static const size_t m_TRANSFER_DELAY_SECS = 0;

		/** Timeout delay for Firmware Update jobs, in seconds.
		 * If this delay is exceeded, the IOT_Net class will call the Reset() function. */
		/* This delay is also used while creating FUS session for IOT */
		const time_t m_COMMAND_TIMEOUT_SECS = 90;

};


/***************** Inline functions. ***************************************/

// Registers the Product-level Callback function to handle product-defined Cloud-to-Device Device Commands.
// nullptr is a legal argument here
inline void IOT_Device_Command::Register_C2D_Command_Callback( C2D_Poduct_Command_Callback callback )
{
	m_c2d_product_command_callback = callback;
}

#endif	// IOT_DEVICE_COMMAND_H
