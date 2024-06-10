/**
 *****************************************************************************************
 * @file
 * @brief This file contains class that handles FUS over IOT
 * @details IoT_FuS contains all the necessary methods required to connect IoT to FUS
 * and calls FUS APIs
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IOT_FUS_H
#define IOT_FUS_H

#include "FUS.h"
#include "IOT_Device_Command.h"
#include "Babelfish_Assert.h"
#include "Fw_Store.h"
#ifndef ESP32_SETUP
#include "Log_Events.h"
#endif
#include "User_Account.h"

using namespace BF_FUS;

/************* Typedefs ***********************/
#ifndef ESP32_SETUP
typedef void (* IOT_FUS_Audit_Function_Decl_1) ( log_event_t event_code );
typedef void (* IOT_FUS_Audit_Function_Decl_2) ( log_event_t event_code, uint8_t* log_param );
#endif
/**
 ****************************************************************************************
 * @brief Class to handle FUS over IOT
 *
 * @details  This class provides all static methods required for FUS over IOT operation
 *
 * @ingroup fus_over_iot
 ****************************************************************************************
 */
class IOT_Fus
{
	public:

		/**
		 * @brief Gets processor and images information from FUS and creates a response to send to cloud
		 *  @details This function sends processor and images information to cloud
		 * @param[out] command_response: Destination pointer at which JSON formatted response string will be stored
		 */
		static void Query_Processor_Images( IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Handles IOT FUS related command parsing
		 * @details This function parses IOT FUS related commands and calls appropriate APIs for further processing
		 * @param command: The command structure, from which we get the Job UUID and "Who" requested this
		 * @param iot_map_handle: Map handle for map of key:value parameters in the request.
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Device_Command.
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 * @return True if IOT FUS related commands are received and parsed successfully
		 */
		static bool Command_Handler( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle, void* owner_context,
									 IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * This function deletes the existing FUS session
		 *
		 * @param[out] final_message: This message will be filled only when there's an error/failure
		 * @return True on success, false on failure/error
		 */
		static bool Exit_Firmware_Session( const char** final_message );

		/**
		 * This function checks whether FUS session is alive or not
		 *
		 * @return True on success, false on failure/error
		 */
		static bool Firmware_Upgrade_Inprogress( void );

		/**
		 * This function returns whether IoT FUS in progress is for App firmware or not.
		 * If yes then reboot will be required.
		 *
		 * @return True if App firmware upgrade is in progress and reboot required
		 */
		static bool Reboot_Required( void );

		/** This function handles different states of scheduled FUS over IOT operations
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Device_Command.
		 */
		static void Scheduled_State_Machine_Handler( void* owner_context );

		/** This function handles different states of IoT D2C commands
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Device_Command.
		 */
		static void D2C_State_Machine_Handler( void* owner_context );

		/**  Static Call back function for the NV Ctrls erase operation status
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 *	@param[in] status: NV Ctrl status
		 * @return : None
		 */
		static void Erase_Status_Cb_Static( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t status );

		/**  Static function that registers audit logging related call backs with IOT FUS
		 * @param[in] fw_audit_log_cb:      Pointer to a function that captures log on product side
		 *	@param[in] fw_audit_param_cb:   Pointer to a function that sends parameters to log on product side
		 * @return : None
		 */
#ifndef ESP32_SETUP
		static void Register_Audit_Log_Callback( IOT_FUS_Audit_Function_Decl_1 fw_audit_log_cb,
												 IOT_FUS_Audit_Function_Decl_2 fw_audit_param_cb );

#endif

	private:

		static bool is_erase_scratch_successful;

		/**
		 * @brief Converts images information to map
		 * @details This function gets image information from FUS and stores it as key:value pair using
		 * maps(azure-c-shared
		 * utility)
		 * @param x: processor index
		 * @param y: image index
		 * @param iot_fus_handle: FUS static handle
		 * @param map_value: pointer that will act as map_value(It must be previously allocated using malloc)
		 * @param[out] image_map_handle: map in which image information will be stored
		 */
		static bool Convert_Image_Info_To_Map( uint8_t x, uint8_t y, FUS* iot_fus_handle,
											   MAP_HANDLE image_map_handle,
											   char*& map_value );

		/**
		 * @brief Converts processors information to map
		 * @details This function gets processor information from FUS and stores it as key:value pair using
		 * maps(azure-c-shared
		 * utility)
		 * @param x: processor index
		 * @param iot_fus_handle: FUS static handle
		 * @param map_value: pointer that will act as map_value(It must be previously allocated using malloc)
		 * @param[out] processor_map_handle: map in which processor information will be stored
		 */
		static bool Convert_Processor_Info_To_Map( uint8_t x, FUS* iot_fus_handle,
												   MAP_HANDLE processor_map_handle, char*& map_value );

		/**
		 * @brief Converts map into JSON format
		 * @param[out] info_json_string: String containing JSON formatted map
		 * @param str_len: initial length of image_info_json_string
		 * @param map_handle: map containing image or processor information
		 * @return length of JSON formatted string
		 */
		static uint32_t Store_Map_As_JSON( char*& info_json_string, MAP_HANDLE map_handle,
										   uint32_t str_len );

		/**
		 * @brief Gets images information from FUS and stores it in the form of JSON string
		 * @param[out] command_response: image information in the form of JSON string
		 * @param iot_fus_handle: static FUS handle
		 * @return length of JSON formatted string
		 */
		static uint32_t Images_Info_As_JSON_String( FUS* iot_fus_handle,
													IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * @brief Gets processor information from FUS and stores it in the form of JSON string
		 * @param[out] command_response: processor information in the form of JSON string
		 * @param iot_fus_handle: static FUS handle
		 * @return length of JSON formatted string
		 */
		static uint32_t Processor_Info_As_JSON_String( FUS* iot_fus_handle,
													   IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/** This function converts version number from integer to string as major.minor.rev.
		 * Suppose version number is 111 it will be converted as "1.1.1"
		 * @param[out] dest_str_version: Destination pointer at which version would be stored/copied as string
		 * @param version_num: Integer containing version number
		 */
		static void Convert_Version_To_String( char*& dest_str_version, uint32_t version_num );

		/** This function copies an integer(eg guid) and stores it as string
		 * Note: arg_n will be allocated using malloc
		 * @param[out] arg_n: Destination pointer at which number would be stored/copied as string
		 * @param num: number(eg guid)
		 */
		static void Fill_Arg_With_Num_Str( const char*& arg_n, uint32_t num );

		/** This function copies an integer version(eg 111) and stores it as major.minor.rev string(eg 1.1.1)
		 * Note: arg_n will be allocated using malloc
		 * @param[out] arg_n: Destination pointer at which number would be stored/copied as major.minor.rev string
		 * @param num_having_ver: number(eg firmware version)
		 */
		static void Fill_Arg_With_Num_Str_Version( const char*& arg_n, uint32_t num_having_ver );

		/**
		 * @brief Handles the UpdateFirmware command, evaluating its parameters before accepting.
		 * @details This function checks the supplied parameters and determines whether or not it will
		 *          accept the requested UpdateFirmware, and indicates so in the response.
		 *          If it does accept the update, the next step for us to take is to send the
		 *          D2C TransferImage command.
		 * @param command: The command structure, from which we get the Job UUID and "Who" requested this
		 * @param iot_map_handle: Map handle for map of key:value parameters in the request.
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Device_Command.
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 */
		static void Update_Firmware( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle,
									 IOT_DEVICE_COMMAND_RESPONSE* command_response, void* owner_context );

		/**
		 * Handler for the Job Name, which importantly must contain the Process/Image path.
		 * The Path should be some part of the param_value here, in the form "... Path: X/Y ..."
		 * where X is the processor index (normally 0) and Y is the image index (eg, 0, 1, or 2).
		 * The Processor and Image indices will be saved in the job control structure.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "name", but meaning the UpdateFirmware job's name.
		 * @param param_value: The name for this job, which should contain "Path: X/Y"
		 * @param[out] command_response: If the Processor/Image are invalid (or not supplied),
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Job_Name_Handler( const char* param_key, const char* param_value,
									  IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Image Type, which must match the name we have for the image_index.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "image_type".
		 * @param param_value: The name for this image_type, which should agree with the FUS configuration.
		 * @param[out] command_response: If the image type is invalid,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Image_Type_Handler( const char* param_key, const char* param_value,
										IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Image Version, which allows the product to decide if it can be upgraded
		 * (or downgraded) to this version.
		 * At present, we have no basis for refusing any version, so will always accept.
		 * Later product code can build in rules for what is acceptable, maybe replacing this handler with
		 * a product-specific version.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "image_size".
		 * @param param_value: The base64-decoded image_size, in bytes, which should agree with the FUS configuration.
		 * @param[out] command_response: If the image size is invalid,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Image_Version_Handler( const char* param_key, const char* param_value,
										   IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Model, which must match the text we have for the product's Model.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "device_model".
		 * @param param_value: The text for this device_model, which should agree with the FUS configuration.
		 * @param[out] command_response: If the device_model is invalid,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Model_Handler( const char* param_key, const char* param_value,
								   IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Image Format, which must be "binary".
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "image_format".
		 * @param param_value: The text for this image_format, which should be "binary".
		 * @param[out] command_response: If the image format is invalid,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Image_Format_Handler( const char* param_key, const char* param_value,
										  IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Image Size, which must fit within the Flash allocation for this image type.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "image_size".
		 * @param param_value: The base64-decoded image_size, in bytes, which should agree with the FUS configuration.
		 * @param[out] command_response: If the image size is invalid,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Image_Size_Handler( const char* param_key, const char* param_value,
										IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * Handler for the Image Description, containing the image's Compatibility Number,
		 * which must be equal or greater than the current Compatibility Number, unless rollback is allowed.
		 * This function conforms to type Image_Param_Handler.
		 *
		 * @param param_key  The name for this parameter, "image_description".
		 * @param param_value: The description, which should contain "... Compat#: X", where X is the Compatibility
		 * Number.
		 * @param[out] command_response: If the image compatability is missing or incompatible,
		 *              will set command response status to HTTP_STATUS_BAD_REQUEST and provide a message.
		 * @return True on success (valid value for this parameter), else False if invalid and the
		 *              response should be returned now.
		 */
		static bool Image_Description_Handler( const char* param_key, const char* param_value,
											   IOT_DEVICE_COMMAND_RESPONSE* command_response );


		/** Using the Job Control information, now execute the FUS flash update, and return the result.
		 * @param[out] final_message Pointer to text to use for the final step_description for
		 *              the DeviceCommandStatus update, depending upon success or failure.
		 * @param owner_context The context for this command (is a pointer to the IOT_Device_Command instance)
		 * @param[out] is_command_scheduled Will return as true if status is just scheduled (not completed yet)
		 * @return True on successful update, else False on failure.
		 */
		static bool Do_FUS_Update( const char** final_message, void* owner_context, bool* is_command_scheduled );

		/** Erase the Scratch area in preparation for writing received FUS image data to that Scratch area.
		 * When done successfully, will request the TransferImage command to be sent next.
		 *
		 * @param iot_dev_cmd Pointer to the IOT_Device_Command instance that is needed for some services.
		 * @return True on success erasing, else False.
		 */
		static bool Erase_Scratch( IOT_Device_Command* iot_dev_cmd );

		/**
		 * This function writes received decoded binary image to scratch area(untrusted memory type) chunk by chunk
		 *
		 * @param decoded_data:  Decoded image chunk
		 * @param decoded_len: Length of decoded image chunk
		 * @param offset: offset will get added to start address to form destination address where image chunk is to be
		 * written
		 * @param[out] command_response: Response containing HTTP status code, msg and delay required for write
		 * operation
		 * @return True on success, false on failure/error
		 */
		static bool Write_Image_To_Scratch( unsigned char* decoded_data, size_t decoded_len, size_t offset,
											IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * This function validates the image for good header, image data, CRC, Code signing and product info(Like
		 * product code, roll back and version)
		 *
		 * @param[out] final_message: This message will be filled only when there's an error/failure
		 * @param owner_context The context for this command (is a pointer to the IOT_Device_Command instance)
		 * @param[out] is_command_scheduled Will return as true if status is just scheduled (not completed yet)
		 * @return True on success, false on failure/error
		 */
		static bool Validate_Image( const char** final_message, void* owner_context, bool* is_command_scheduled );

		/**
		 * This function creates FUS session
		 *
		 * @param session_timeout:  Timeout for session validity
		 * @param[out] command_response: Response containing HTTP status code and msg
		 * @return True on success, false on failure/error
		 */
		static bool Create_Session( uint32_t session_timeout, IOT_DEVICE_COMMAND_RESPONSE* command_response );

		/**
		 * This function is used for FUS commit operation i.e copy image from scratch to flash
		 *
		 * @param[out] final_message: This message will be filled only when there's an error/failure
		 * @return True on success, false on failure/error
		 */
		static bool Commit( const char** final_message );

		/**
		 * This function is used for FUS status operation i.e get the current status of the memory controls
		 *
		 * @param[out] final_message: This message will be filled only when there's an error/failure
		 * @param owner_context The context for this command (is a pointer to the IOT_Device_Command instance)
		 * @param[out] is_command_scheduled Will return as true if status is just scheduled (not completed yet)
		 * @return True on success, false on failure/error
		 */
		static bool Status( const char** final_message, void* owner_context, bool* is_command_scheduled );


		/**
		 * This function returns session ID of the ongoing FUS session
		 *
		 * @return session ID
		 */
		static uint32_t Get_Firmware_Session_Id( void );

		/**
		 * This function performs device reboot
		 */
		static void Fw_Reboot( void );

		/**
		 * @brief Handles the ImageTransfer command, containing one Base64-encoded chunk of the Image.
		 * @details This function validates the Job UUID and other parameters, then decodes the
		 *          bytes and gives them to the FUS code to store in the Scratch area.
		 *          If the "more" flag is clear, then this is the last chunk, and FUS should begin
		 *          processing the Image.
		 * @param command: The command structure, from which we get the Job UUID and "Who" requested this
		 * @param iot_map_handle: Map handle for map of key:value parameters in the request.
		 * @param owner_context: Opaque pointer to our "context", which is "this" for IOT_Device_Command.
		 * @param[out] command_response: The structure for the response to this command;
		 *              we must set the status and message in the response.
		 */
		static void Handle_Image_Transfer( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle,
										   IOT_DEVICE_COMMAND_RESPONSE* command_response, void* owner_context );

		/**
		 * This function calculates the total size required by particular image to fit in
		 *
		 * @param processor_index:  Processor index
		 * @param image_index:      Image index
		 * @return Size of image
		 */
		static uint32_t Get_Image_Size( uint8_t processor_index, uint8_t image_index );

		/**
		 * This function returns the start address of image area
		 *
		 * @param processor_index:  Processor index
		 * @param image_index:      Image index
		 * @return Start address of image area
		 */
		static uint32_t Get_Image_Start_Address( uint8_t processor_index, uint8_t image_index );

		/**
		 * This function captures an event and its related parameters for audit logging and passes it
		 * on to product side
		 *
		 * @param event_code:   IOT FUS related event code as defined in Log_Events.h
		 * @return              None
		 */
#ifndef ESP32_SETUP
		static void Capture_Audit_Log( log_event_t event_code );

#endif
};


#endif	// IOT_FUS_H
