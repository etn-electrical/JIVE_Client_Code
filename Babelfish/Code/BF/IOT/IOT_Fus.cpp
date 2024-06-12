/**
 *****************************************************************************************
 * @file
 * @brief This file defines all the functions required to connect IoT to FUS
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "IOT_Debug.h"
#include "Prod_Spec_FUS.h"
#include "IOT_FUS.h"
#include "IOT_Net.h"
#include "IOT_Pub_Sub.h"

#include "Breaker.h"
#include "EventManager.h"

/* Set the size of the Map of UpdateFirmware parameters to their handler functions. */
static const uint16_t HANDLER_MAP_SIZE = 7;
#ifndef ESP32_SETUP
static IOT_FUS_Audit_Function_Decl_1 iot_fus_log_audit_capture_cb =
	reinterpret_cast<IOT_FUS_Audit_Function_Decl_1>( nullptr );
static IOT_FUS_Audit_Function_Decl_2 iot_fus_log_param_cb =
	reinterpret_cast<IOT_FUS_Audit_Function_Decl_2>( nullptr );
#endif

//Ahmed
extern DRAM_ATTR Device_Info_str DeviceInfo;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Query_Processor_Images( IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	FUS* iot_fus_handle = FUS::Get_FUS_Handle_Static();
	prod_info_t prod_info = { };

	command_response->status = HTTP_STATUS_OK;
	command_response->msg = const_cast<char*>( "OK" );

	/* FUS related initialization */
	BF_ASSERT( iot_fus_handle );

	/* 1. Get product information from FUS and store it in arg1, arg2, arg3, arg4  */
	if ( BF_FUS::SUCCESS == iot_fus_handle->FUS::Product_Info( &prod_info ) )
	{
		// Response arg_x are freed when done, so we must malloc storage for this string
		IOT_Fus::Fill_Arg_With_Num_Str( command_response->arg_1, prod_info.guid );	// "cpu_id" of processor
		IOT_Fus::Fill_Arg_With_Num_Str_Version( command_response->arg_2, prod_info.boot_version );
		IOT_Fus::Fill_Arg_With_Num_Str_Version( command_response->arg_3, prod_info.boot_spec_version );
		IOT_Fus::Fill_Arg_With_Num_Str( command_response->arg_4, prod_info.serial_num );

		/* 2. Get processor information from FUS and store it in linked list */
		if ( IOT_Fus::Processor_Info_As_JSON_String( iot_fus_handle, command_response ) )
		{
			/* 3. Get image information and store it in linked list */
			if ( IOT_Fus::Images_Info_As_JSON_String( iot_fus_handle, command_response ) )
			{
				command_response->list_1_cleanup = true;
				command_response->list_2_cleanup = true;
			}
			else// error condition
			{
				command_response->status = HTTP_STATUS_BAD_REQUEST;
				command_response->msg = const_cast<char*>( "Failed to get images information" );
			}
		}
		else// error condition
		{
			command_response->status = HTTP_STATUS_BAD_REQUEST;
			command_response->msg = const_cast<char*>( "Failed to get component information" );
		}
	}
	else
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Failed to get product information" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Convert_Image_Info_To_Map( uint8_t x, uint8_t y, FUS* iot_fus_handle,
										 MAP_HANDLE image_map_handle, char*& map_value )
{
	image_info_req_t image_info;
	image_info_resp_t image_resp = { 0 };

	image_info.proc_id = x;
	image_info.image_id = y;

	bool return_status = true;

	if ( BF_FUS::SUCCESS == iot_fus_handle->FUS::Image( &image_info, &image_resp ) )
	{
		sprintf( map_value, "%d|%d", x, y );
		Map_AddOrUpdate( image_map_handle, "path", map_value );

		Map_AddOrUpdate( image_map_handle, "image_type", ( const char* )image_resp.name );

		IOT_Fus::Convert_Version_To_String( map_value, image_resp.version );
		Map_AddOrUpdate( image_map_handle, "image_version", ( const char* )map_value );

		sprintf( map_value, "%u", image_resp.compatibility_num );
		Map_AddOrUpdate( image_map_handle, "compat_number", ( const char* )map_value );

		INT_ASCII_Conversion::uint32_to_str( image_info.proc_id, reinterpret_cast<uint8_t*>( map_value ) );
		Map_AddOrUpdate( image_map_handle, "cpu_index", map_value );

		INT_ASCII_Conversion::uint32_to_str( image_resp.guid, reinterpret_cast<uint8_t*>( map_value ) );
		Map_AddOrUpdate( image_map_handle, "img_id", map_value );
	}
	else
	{
		return_status = false;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Convert_Processor_Info_To_Map( uint8_t x, FUS* iot_fus_handle,
											 MAP_HANDLE processor_map_handle, char*& map_value )
{
	comp_info_req_t comp_req;	// TOTAL_COMPONENTS = 1 in FUS_Config.h
	comp_info_resp_t comp_resp = { 0 };

	comp_req.comp_id = x;

	bool return_status = true;

	if ( BF_FUS::SUCCESS == iot_fus_handle->FUS::Component( &comp_req, &comp_resp ) )
	{
		/* Fill key:value pairs in map */
		INT_ASCII_Conversion::uint32_to_str( comp_req.comp_id, reinterpret_cast<uint8_t*>( map_value ) );
		Map_AddOrUpdate( processor_map_handle, "index", map_value );

		Map_AddOrUpdate( processor_map_handle, "name", ( const char* )( comp_resp.name ) );

		INT_ASCII_Conversion::uint32_to_str( comp_resp.serial_num, reinterpret_cast<uint8_t*>( map_value ) );
		Map_AddOrUpdate( processor_map_handle, "serial_num", map_value );

		IOT_Fus::Convert_Version_To_String( map_value, comp_resp.firmware_version );
		Map_AddOrUpdate( processor_map_handle, "cpu_version", map_value );

		IOT_Fus::Convert_Version_To_String( map_value, comp_resp.hardware_version );
		Map_AddOrUpdate( processor_map_handle, "hardware_version", map_value );

		INT_ASCII_Conversion::uint32_to_str( comp_resp.guid, reinterpret_cast<uint8_t*>( map_value ) );
		Map_AddOrUpdate( processor_map_handle, "cpu_id", map_value );
	}
	else
	{
		return_status = false;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Store_Map_As_JSON( char*& info_json_string, MAP_HANDLE map_handle,
									 uint32_t str_len )
{
	STRING_HANDLE json_string = Map_ToJSON( map_handle );	/* Uses string_new. Must call string_delete at the end */

	str_len = str_len + STRING_length( json_string );
	info_json_string =
		reinterpret_cast<char*>( Ram::Reallocate_Basic( info_json_string, ( str_len + 2 ) * sizeof( char ) ) );		/*
																													   Added
																													   2
																													   for
																													   storing
																													      ','
																													      and
																													   '\0'
																													 */
	if ( info_json_string != nullptr )
	{
		const char* json_c_str = STRING_c_str( json_string );
		strcat( info_json_string, json_c_str );
		strcat( info_json_string, "," );
		str_len = str_len + strlen( info_json_string );
	}
	STRING_delete( json_string );
	return ( str_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Images_Info_As_JSON_String( FUS* iot_fus_handle,
											  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	uint32_t str_len = 0;
	char* image_info_json_string = nullptr;	// need to malloc before inserting into linked list
	char* map_value = nullptr;

	/* Create map and convert to json string */
	/*
	   "images": [
	           {
	            "path":         "<string>",         // Processor/Image indices, eg "0/1"
	            "image_type":   "<string>",       // Image type, the key as used in UpdateFirmware
	            "image_version": "<string>",      // The current image version
	            "compat_number": "<string>",	  // The Compatibility Number for this Image
	            "cpu_index":       "<string>",         // The "GUID" (identifier) for this image's processor i.e 'x' in
	               Path x/y
	            "img_id":       "<string>"          // The "GUID" (identifier) for this image
	           }
	        ]
	 */

	image_info_json_string = reinterpret_cast<char*>( Ram::Allocate_Basic( 2 * sizeof( char ) ) );
	if ( image_info_json_string == nullptr )
	{
		return ( str_len );		// Just leave; this is an unlikely failure
	}
	strcpy( image_info_json_string, "[" );
	str_len = strlen( image_info_json_string );
	MAP_HANDLE image_map_handle = Map_Create( nullptr );

	map_value = reinterpret_cast<char*>( iot_malloc( sizeof( UINT32_MAX_STRING ) + 1 + 2 ) );	// Added 1 to store
																								// '\0', added 2 for
	// storing '.' for version like
	// 111.222.333
	BF_ASSERT( map_value != nullptr );
	uint32_t total_components = iot_fus_handle->Get_Component_Count();
	bool status = true;

	// path x/y
	for ( uint32_t x = 0; x < total_components; x++ )
	{
		uint8_t total_images = 0;
		comp_info_req_t comp_req;
		comp_info_resp_t comp_resp = { 0 };

		comp_req.comp_id = x;	/* Processor index */
		iot_fus_handle->FUS::Component( &comp_req, &comp_resp );
		total_images = comp_resp.image_count;
		for ( uint8_t y = 0; y < total_images; y++ )
		{
			if ( IOT_Fus::Convert_Image_Info_To_Map( x, y, iot_fus_handle, image_map_handle, map_value ) )
			{
				str_len = IOT_Fus::Store_Map_As_JSON( image_info_json_string, image_map_handle, str_len );
			}
			else
			{
				status = false;
				break;	// break on failure
			}
		}
	}
	/* If something failed while getting processor info then set str_len to return 0
	 * and also free the memory occupied by image_info_json_string */
	if ( status == false )
	{
		str_len = 0;
		iot_free( image_info_json_string );
	}
	else
	{
		memset( image_info_json_string + strlen( image_info_json_string ) - 1, '\0', 1 );	// remove trailing comma
		strcat( image_info_json_string, "]" );

		/* Add "images" as key and images info as value in singly linked list */
		char* images_key;
		images_key = reinterpret_cast<char*>( Ram::Allocate_Basic( ( strlen( "images" ) + 1 ) * sizeof( char ) ) );
		if ( images_key != nullptr )
		{
			strcpy( images_key, "images" );
			singlylinkedlist_add( command_response->list_1, images_key );
			singlylinkedlist_add( command_response->list_2, image_info_json_string );
		}
		else
		{
			iot_free( image_info_json_string );
		}
	}

	iot_free( map_value );
	Map_Destroy( image_map_handle );
	return ( str_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Processor_Info_As_JSON_String( FUS* iot_fus_handle,
												 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	uint32_t str_len = 0;
	char* processor_info_json_string = nullptr;	// need to malloc before inserting into linked list
	char* map_value = nullptr;

	/* Create map and convert to json string */
	/*
	   {
	   "index":        "0",
	   "name":         "RTK_MAIN_PROCESSOR",
	   "serial_num":   "0",
	   "cpu_version":  "1.1.1",
	   "hardware_version":  "1.2.3",
	   "cpu_id":       "1"
	   }
	 */
	processor_info_json_string = reinterpret_cast<char*>( Ram::Allocate_Basic( 2 * sizeof( char ) ) );
	if ( processor_info_json_string == nullptr )
	{
		return ( str_len );		// Just leave; this is an unlikely failure
	}
	strcpy( processor_info_json_string, "[" );
	str_len = strlen( processor_info_json_string );
	MAP_HANDLE processor_map_handle = Map_Create( nullptr );

	map_value = reinterpret_cast<char*>( iot_malloc( sizeof( UINT32_MAX_STRING ) + 1 + 2 ) );	// Added 1 to store
	bool status = true;

	if ( ( map_value != nullptr ) && ( processor_map_handle != nullptr ) )
	{
		// '\0', added 2 for
		// storing '.' for version like
		// 111.222.333
		uint32_t total_components = iot_fus_handle->Get_Component_Count();
		for ( uint32_t x = 0; x < total_components; x++ )
		{
			if ( IOT_Fus::Convert_Processor_Info_To_Map( x, iot_fus_handle, processor_map_handle, map_value ) )
			{
				str_len = IOT_Fus::Store_Map_As_JSON( processor_info_json_string, processor_map_handle, str_len );
			}
			else
			{
				status = false;
				break;	// break on failure
			}
		}
	}
	else
	{
		status = false;
	}

	/* If something failed while getting processor info then set str_len to return 0
	 * and also free the memory occupied by processor_info_json_string */
	if ( status == false )
	{
		str_len = 0;
		iot_free( processor_info_json_string );
	}
	else
	{
		memset( processor_info_json_string + strlen( processor_info_json_string ) - 1, '\0', 1 );	// remove trailing
																									// comma
		strcat( processor_info_json_string, "]" );
		/* Add "processors" as key and processor info as value in singly linked list */
		char* processor_key;
		processor_key =
			reinterpret_cast<char*>( Ram::Allocate_Basic( ( strlen( "processors" ) + 1 ) * sizeof( char ) ) );
		if ( processor_key != nullptr )
		{
			strcpy( processor_key, "processors" );
			singlylinkedlist_add( command_response->list_1, processor_key );
			singlylinkedlist_add( command_response->list_2, processor_info_json_string );
		}
		else
		{
			iot_free( processor_info_json_string );
		}
	}

	/* Delete json string */
	iot_free( map_value );
	Map_Destroy( processor_map_handle );
	return ( str_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Convert_Version_To_String( char*& dest_str_version, uint32_t version_num )
{
	uint8_t ver_len = 0;
	uint8_t* dest_str_version_uint8 = nullptr;

	dest_str_version_uint8 = reinterpret_cast<uint8_t*>( const_cast<char*>( dest_str_version ) );
	ver_len = INT_ASCII_Conversion::uint32_to_str( ( version_num & 0x00FF ), dest_str_version_uint8 );	/* major */
	*( dest_str_version_uint8 + ver_len ) = '.';
	dest_str_version_uint8 = dest_str_version_uint8 + ver_len + 1;
	ver_len = INT_ASCII_Conversion::uint32_to_str( ( ( ( version_num >> 8 ) & 0xFF ) ), dest_str_version_uint8 );	/*
																													   minor
																													 */
	*( dest_str_version_uint8 + ver_len ) = '.';
	dest_str_version_uint8 = dest_str_version_uint8 + ver_len + 1;
	ver_len = INT_ASCII_Conversion::uint32_to_str( ( ( ( version_num >> 16 ) & 0xFFFFU ) ), dest_str_version_uint8 );	/*
																														   rev
																														 */
	*( dest_str_version_uint8 + ver_len ) = '\0';	// Terminate the string
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Fill_Arg_With_Num_Str( const char*& arg_n, uint32_t num )
{
	uint8_t arg_size = 0;
	uint8_t* arg_n_uint8_ptr = nullptr;

	arg_n = reinterpret_cast<char*>( iot_malloc( sizeof( UINT32_MAX_STRING ) + 1 ) );// Added 1 to store '\0'
	if ( arg_n != nullptr )
	{
		arg_n_uint8_ptr = reinterpret_cast<uint8_t*>( const_cast<char*>( arg_n ) );
		arg_size = INT_ASCII_Conversion::uint32_to_str( num, arg_n_uint8_ptr );
		*( arg_n_uint8_ptr + arg_size ) = '\0';	// Terminate the string
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Fill_Arg_With_Num_Str_Version( const char*& arg_n, uint32_t num_having_ver )
{
	uint8_t arg_size = 0;
	uint8_t* arg_n_uint8_ptr = nullptr;

	arg_n = reinterpret_cast<char*>( iot_malloc( sizeof( UINT32_MAX_STRING ) + 1 + 2 ) );// Added 1 to store '\0', added
																						// 2 for storing
	if ( arg_n != nullptr )
	{
		// '.' for version like 111.222.333
		arg_n_uint8_ptr = reinterpret_cast<uint8_t*>( const_cast<char*>( arg_n ) );
		arg_size = INT_ASCII_Conversion::uint32_to_str( ( num_having_ver >> 24 ), arg_n_uint8_ptr );/* major */
		*( arg_n_uint8_ptr + arg_size ) = '.';
		arg_n_uint8_ptr = arg_n_uint8_ptr + arg_size + 1;
		arg_size = INT_ASCII_Conversion::uint32_to_str( ( ( num_having_ver >> 16 ) & 0xFF ), arg_n_uint8_ptr );	/* minor
																												 */
		*( arg_n_uint8_ptr + arg_size ) = '.';
		arg_n_uint8_ptr = arg_n_uint8_ptr + arg_size + 1;
		arg_size = INT_ASCII_Conversion::uint32_to_str( ( num_having_ver & 0xFFFF ), arg_n_uint8_ptr );	/* rev */
		*( arg_n_uint8_ptr + arg_size ) = '\0';	// Terminate the string
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Update_Firmware( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle,
							   IOT_DEVICE_COMMAND_RESPONSE* command_response, void* owner_context )
{
	/* Map of UpdateFirmware parameters to their handler functions. */
	update_firmware_handler_t update_handler_map[HANDLER_MAP_SIZE];
	const char* const* keys = nullptr;
	const char* const* values = nullptr;
	size_t mcount = 0;
	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	IOT_DEBUG_PRINT( DBG_MSG_INFO, "Processing UpdateFirmware request for %u parameters:", mcount );

	// There must not already be another job in progress
	if ( IOT_Device_Command::m_job_control )
	{
		command_response->status = HTTP_STATUS_LOCKED;
		command_response->msg = const_cast<char*>( "UpdateFirmware already in progress" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Attempted %s UpdateFirmware when %s already in progress",
						 command->commandID, IOT_Device_Command::m_job_control->job_uuid );
		return;
	}
	IOT_Device_Command::m_job_control = ( fus_job_control_t* ) Ram::Allocate_Basic( sizeof( fus_job_control_t ) );
	if ( IOT_Device_Command::m_job_control == nullptr )
	{
		command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
		command_response->msg = const_cast<char*>( "Insufficient memory" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Insufficent memory; allocate failed" );
		return;
	}
	strncpy_s( IOT_Device_Command::m_job_control->job_uuid, sizeof( IOT_Device_Command::m_job_control->job_uuid ),
			   command->commandID,
			   sizeof( IOT_Device_Command::m_job_control->job_uuid ) - 1 );
	strncpy_s( IOT_Device_Command::m_job_control->job_who, sizeof( IOT_Device_Command::m_job_control->job_who ),
			   command->commandWho,
			   sizeof( IOT_Device_Command::m_job_control->job_who ) - 1 );

	IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );

	IOT_Device_Command::m_job_control->device_uuid = iot_dev_cmd->m_pub_sub->Get_Publisher_Device_Uuid();
	IOT_Device_Command::m_job_control->fus_handle = FUS::Get_FUS_Handle_Static();
	BF_ASSERT( IOT_Device_Command::m_job_control->fus_handle );

	// We want the handler map parameters processed in the order given here
	update_handler_map[0] = { "name",           Job_Name_Handler };
	update_handler_map[1] = { "image_type",     Image_Type_Handler };
	update_handler_map[2] = { "device_model",   Model_Handler };
	update_handler_map[3] = { "image_description",  Image_Description_Handler };
	update_handler_map[4] = { "image_version",  Image_Version_Handler };
	update_handler_map[5] = { "image_format",   Image_Format_Handler };
	update_handler_map[6] = { "image_size",     Image_Size_Handler };

	bool success = false;

	for ( auto& entry : update_handler_map )
	{
		success = false;
		command_response->status = 0;		// an invalid value
		if ( result == MAP_OK )
		{
			for ( uint16_t i = 0; i < mcount; i++ )
			{
				char* param_name = const_cast<char*>( keys[i] );
				char* value_str = const_cast<char*>( values[i] );
				if ( strcmp( param_name, entry.param_name ) == 0 )
				{
					IOT_DEBUG_PRINT( DBG_MSG_INFO, "Handling Param %s = %s", param_name, value_str );
					success = entry.handler( param_name, value_str, command_response );
					break;
				}
				// IOT_DEBUG_PRINT( DBG_MSG_INFO, "Param %s = %s", param_name, value_str );
			}
		}

		// Check for a missing parameter (should we bother?)
		if ( !success )
		{
			if ( command_response->status == 0 )
			{
				command_response->status = HTTP_STATUS_BAD_REQUEST;
				command_response->msg = const_cast<char*>( "Missing required parameter" );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Missing required parameter %s", entry.param_name );
			}
			break;
		}
	}

	const time_t command_timeout_sec = iot_dev_cmd->Get_Command_Timeout_Seconds();

	/* Register state machine callbacks here since we use it even in error conditions. So it needs to be registered
	   before using */
	iot_dev_cmd->m_iot_net->Register_State_Machine_Callback( &IOT_Device_Command::State_Machine_Handler );

    //Ahmed 
    DeviceInfo.OTA_State = OTA_START;

	if ( success )
	{
		/* Create FUS session */
		success = IOT_Fus::Create_Session( command_timeout_sec, command_response );	/* Using same timeout as that of
																					   device command reset */
		if ( success )
		{

			// Now ask for the TransferImage command to be sent
			iot_dev_cmd->m_iot_net->Request_Timer_Task_State( IOT_Net::SEND_TRANSFER_IMAGE );
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_STARTED );
#endif
		}

	}
	else
	{
#ifndef ESP32_SETUP
		Capture_Audit_Log( LOG_EVENT_IOT_FUS_FAILED_DUE_TO_BAD_PARAM );
#endif
	}
	if ( !success )
	{
		/* No need to deallocate m_job_control here because it will be automatically done when
		 * Firmware update is aborted/cancelled when SCHEDULED_RESET_FUS state is reached */
		iot_dev_cmd->m_iot_net->Schedule_Device_Command( command_timeout_sec * MULTIPLIER_1000MS,
														 IOT_Device_Command::SCHEDULED_RESET_FUS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Job_Name_Handler( const char*	/*param_key*/, const char* param_value,
								IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	const char* processor_pos = nullptr;

	BF_ASSERT( IOT_Device_Command::m_job_control );
	const char* path_pos = strstr( param_value, "Path:" );
	bool success = ( path_pos != nullptr );

	if ( success )
	{
		const char* slash_pos = strchr( path_pos, '/' );
		success = ( slash_pos != nullptr );
		if ( success )
		{
			processor_pos = slash_pos - 1;
		}
	}
	if ( !success )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "No Processor/Image Path in job name" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "No Processor/Image Path in job name %s", param_value );
	}
	else
	{
		// Convert what should be single-digit numeric text (eg, "0/1") to numeric values
		int processor_index = processor_pos[0] - '0';
		int image_index = processor_pos[2] - '0';

		FUS* iot_fus_handle = FUS::Get_FUS_Handle_Static();

		///< Need to capture processor index and image index early since audit log will be using it
		IOT_Device_Command::m_job_control->processor_index = processor_index;
		IOT_Device_Command::m_job_control->image_index = image_index;

		/* If image is App image then set the flag */
		if ( Prod_Spec_FUS_Is_App_Image( processor_index, image_index ) )
		{
			IOT_Device_Command::m_job_control->is_reboot_required = true;
		}
		else
		{
			IOT_Device_Command::m_job_control->is_reboot_required = false;
		}

		uint32_t total_components = iot_fus_handle->Get_Component_Count();
		success = ( processor_index >= 0 ) && ( processor_index < total_components );
		if ( !success )
		{
			command_response->status = HTTP_STATUS_BAD_REQUEST;
			command_response->msg = const_cast<char*>( "Invalid Processor index in job name" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Processor index in %s", param_value );
		}
		else
		{
			uint8_t total_images = 0;
			comp_info_req_t comp_req;
			comp_info_resp_t comp_resp = { 0 };

			comp_req.comp_id = processor_index;
			iot_fus_handle->FUS::Component( &comp_req, &comp_resp );
			total_images = comp_resp.image_count;
			// This test is a little dicey - not sure how image counts go on second, third, etc processor
			success = ( image_index >= 0 ) && ( image_index < total_images );
			if ( !success )
			{
				command_response->status = HTTP_STATUS_BAD_REQUEST;
				command_response->msg = const_cast<char*>( "Invalid Image index in job name" );
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Image index in %s", param_value );
			}
			else
			{
				IOT_Device_Command::m_job_control->processor_index = ( uint8_t ) processor_index;
				IOT_Device_Command::m_job_control->image_index = ( uint8_t ) image_index;
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Found Processor/Image = %d/%d", processor_index, image_index );
			}
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Image_Type_Handler( const char* param_key, const char* param_value,
								  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	BF_ASSERT( IOT_Device_Command::m_job_control );
	bool success = false;
	// Just like Firmware_Details_Cb() in REST_FUS_Interface.cpp
	image_info_req_t image_info;

	image_info.proc_id = IOT_Device_Command::m_job_control->processor_index;
	image_info.image_id = IOT_Device_Command::m_job_control->image_index;
	image_info_resp_t image_resp = { 0 };

	if ( BF_FUS::SUCCESS == IOT_Device_Command::m_job_control->fus_handle->Image( &image_info, &image_resp ) )
	{
		if ( image_resp.name != nullptr )
		{
			if ( strcmp( param_value, reinterpret_cast<char*>( image_resp.name ) ) == 0 )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Matched Image Type = %s", param_value );
				success = true;
			}
			else if ( IOT_Device_Command::m_job_control->image_index != 0 )
			{
				/* @ToDo: the CodePack image names given do not match the FUS/REST image names.
				*	"PXGREEN WEB 2.0 FIRMWARE" vs "RTK_WEB_IMAGE"
				*	"PXGREEN LANGUAGE IMAGE" vs "RTK LANGUAGE IMAGE"
				* So we have to just accept anything here, but we will register a complaint! */
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Did not match Image Type '%s' vs '%s', but proceeding...",
								 param_value, reinterpret_cast<char*>( image_resp.name ) );
				success = true;
			}
		}
		else
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get image name" );
		}
	}
	if ( !success )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Invalid Image Type" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Image Type: %s", param_value );
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Image_Version_Handler( const char* param_key, const char* param_value,
									 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	BF_ASSERT( IOT_Device_Command::m_job_control );
	bool success = false;
	// Just like Firmware_Details_Cb() in REST_FUS_Interface.cpp
	image_info_req_t image_info;

	image_info.proc_id = IOT_Device_Command::m_job_control->processor_index;
	image_info.image_id = IOT_Device_Command::m_job_control->image_index;
	image_info_resp_t image_resp = { 0 };

	if ( BF_FUS::SUCCESS == IOT_Device_Command::m_job_control->fus_handle->Image( &image_info, &image_resp ) )
	{
		// We accept any version, until we have some reason not to
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Upgrading Image Version from: %u to: %s", image_resp.version, param_value );
		success = true;
	}
	if ( !success )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Not allowing new version" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Not allowing Upgrade to Image Version: %s", param_value );
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Model_Handler( const char* param_key, const char* param_value,
							 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	BF_ASSERT( IOT_Device_Command::m_job_control );
	bool success = false;
	prod_info_t prod_info = { };
	uint8_t prod_name[PROD_NAME_MAX_LENGTH] = "";

	prod_info.name = prod_name;
	if ( BF_FUS::SUCCESS == IOT_Device_Command::m_job_control->fus_handle->Product_Info( &prod_info ) )
	{
		char guid_text[8];
		snprintf( guid_text, 8, "%u", prod_info.guid );
		if ( strcmp( param_value, guid_text ) == 0 )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Matched Product 'Guid' = %s", param_value );
			success = true;
		}
	}
	if ( !success )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Invalid Product Identifier (guid)" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Product Identifier (guid): %s", param_value );
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Image_Format_Handler( const char* param_key, const char* param_value,
									IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	BF_ASSERT( IOT_Device_Command::m_job_control );
	bool success = false;

	if ( strcmp( param_value, "binary" ) == 0 )
	{
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Correct Image Format = %s", param_value );
		success = true;
	}
	else
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Invalid Image Format" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Image Format: %s, not binary", param_value );
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Image_Size_Handler( const char* param_key, const char* param_value,
								  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	BF_ASSERT( IOT_Device_Command::m_job_control );
	bool success = false;
	// Since size is determined internally by PXWhite, it's unlikely that it will be a non-numeric param_value
	unsigned long received_image_size = strtoul( param_value, nullptr, 10 );

	uint32_t image_area = Get_Image_Size( IOT_Device_Command::m_job_control->processor_index,
										  IOT_Device_Command::m_job_control->image_index );

	IOT_Device_Command::m_job_control->image_start_address = Get_Image_Start_Address(
		IOT_Device_Command::m_job_control->processor_index, IOT_Device_Command::m_job_control->image_index );
	if ( ( received_image_size == 0 ) || ( image_area == 0 ) )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Could not validate Image Size" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Could not validate Image Size: %s vs limit: %u", param_value, image_area );
	}
	else if ( received_image_size > image_area )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Invalid Image Size - Too Large" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid Image Size - Too Large: %s > %u", param_value, image_area );
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_INFO, "Valid Image Size = %s", param_value );
		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 */
bool IOT_Fus::Image_Description_Handler( const char* /*param_key*/, const char* param_value,
										 IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	const char* compat_num_pos = nullptr;

	const char* tag_pos = strstr( param_value, "Compat#:" );
	bool success = ( tag_pos != nullptr );

	if ( success )
	{
		compat_num_pos = tag_pos + sizeof( "Compat#:" );
		if ( *compat_num_pos == ' ' )
		{
			compat_num_pos++;		// skip ahead if pointing to a space char
		}
	}
	if ( !success )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "No Compatibility Number in Description" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "No Compatibility Number in Description: %s", param_value );
	}
	else
	{
		// Convert what should be numeric text (eg, "24") to a numeric value
		unsigned long compat_number = strtoul( compat_num_pos, nullptr, 10 );
		if ( Prod_Spec_FUS_Is_Compatible( compat_number ) == true )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Compatibility Number %lu is valid for firmware upgrade",
							 compat_number );
		}
		else
		{
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_REJECTED_BY_PRODUCT );
#endif
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid compatibility number %lu", compat_number );
			success = false;
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Do_FUS_Update( const char** final_message,  void* owner_context, bool* is_command_scheduled )
{
	bool success = false;

	if ( IOT_Device_Command::m_job_control )
	{
		/* Check the status after commit */
		success = IOT_Fus::Status( final_message, owner_context, is_command_scheduled );

		if ( success )	/* Status succeeds in first attempt(No waiting required) */
		{
			/* Delete FUS session */
			success = IOT_Fus::Exit_Firmware_Session( final_message );
			if ( success )
			{
				*final_message = "Done with Flash Update";
#ifndef ESP32_SETUP
				Capture_Audit_Log( LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_COMPLETED );
#endif
			}
		}
	}
	if ( *final_message )
	{
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Final message %s", *final_message );
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Write_Image_To_Scratch( unsigned char* decoded_data, size_t decoded_len, size_t offset,
									  IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	bool success_status = true;

	image_write_req_t write_image_in_mem;
	image_write_resp_t write_optime = { 0 };
	op_status_t return_status = BF_FUS::SUCCESS;

	write_image_in_mem.proc_index = IOT_Device_Command::m_job_control->processor_index;
	write_image_in_mem.image_index = IOT_Device_Command::m_job_control->image_index;
	write_image_in_mem.session_id = IOT_Fus::Get_Firmware_Session_Id();
	write_image_in_mem.data = decoded_data;
	write_image_in_mem.data_len = decoded_len;
	write_image_in_mem.address = IOT_Device_Command::m_job_control->image_start_address + offset;	/* Start address
																									 +
																									   offset */

	return_status = IOT_Device_Command::m_job_control->fus_handle->Write( &write_image_in_mem, &write_optime );
	if ( BF_FUS::SUCCESS == return_status )
	{
		// Normal response, set the delay for next transfer to how long the write operation tells us it needs to
		// complete.
		// In practice, looks like 19s on the first write, very quick for subsequent writes.
		command_response->status = HTTP_STATUS_OK;
		command_response->msg = const_cast<char*>( "OK" );
		IOT_Fus::Fill_Arg_With_Num_Str( command_response->arg_1, ( write_optime.op_time / MULTIPLIER_1000MS ) );// delay
																												// in
																												// seconds

	}
	else
	{
		success_status = false;
		if ( BF_FUS::REQUEST_INVALID == return_status )
		{
			command_response->status = HTTP_STATUS_NOT_FOUND;
			command_response->msg = const_cast<char*>( "Write failed due to invalid processor or image id" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Write failed due to invalid processor or image id" );
		}
		else if ( BF_FUS::BUSY == return_status )
		{
			/* Needs retransmission of chunk from PX White. Would sure be handy if it waited before resending... */
			command_response->status = HTTP_STATUS_BAD_REQUEST;
			command_response->msg = const_cast<char*>( "Device is busy to write chunks" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Device is busy to write chunks" );
			/* Impose a tough workaround to the failure to wait properly until the erase is done;
			 * Wait for 10s here before returning the response, to give this time, since
			 * PX White will resend this chunk almost immediately after getting the response.
			 * @ToDo Remove this wait once the erase is working correctly. */
			OS_Tasker::Delay( 10 * MULTIPLIER_1000MS );
		}
		else if ( BF_FUS::REJECT == return_status )
		{
			command_response->status = HTTP_STATUS_UNPROCESSABLE;
			command_response->msg = const_cast<char*>( "Write failed due to FUS session ID mismatch" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Write failed due to FUS session ID mismatch" );
		}
		else
		{
			command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
			command_response->msg = const_cast<char*>( "Write failed" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Write failed" );
		}
	}
	return ( success_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Validate_Image( const char** final_message, void* owner_context, bool* is_command_scheduled )
{
	image_validate_req_t validate_req;
	image_validate_resp_t validate_resp = { 0 };
	op_status_t return_status = BF_FUS::SUCCESS;
	bool success_status = true;

	validate_req.comp_index = IOT_Device_Command::m_job_control->processor_index;
	validate_req.image_index = IOT_Device_Command::m_job_control->image_index;

	return_status = IOT_Device_Command::m_job_control->fus_handle->Validate( &validate_req, &validate_resp );
	if ( BF_FUS::SUCCESS == return_status )
	{
		if ( validate_resp.status == BF_FUS::FUS_VALID_IMAGE )
		{
			IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Image validation successful" );
		}
		else if ( validate_resp.status == BF_FUS::FUS_VALIDATION_IN_PROGRESS )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image validation is in progress. op_time = %d milliseconds",
							 validate_resp.op_time );
			/* Schedule next call to validate after op_time delay */
			IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );
			iot_dev_cmd->m_iot_net->Schedule_Device_Command( validate_resp.op_time,
															 IOT_Device_Command::SCHEDULED_VALIDATE_FUS );
			success_status = false;	/* False because validate is just scheduled(not completed yet) */
			*is_command_scheduled = true;
		}
		else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_CHECKSUM_FAILED )
		{
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_CHECKSUM_FAILURE );
#endif
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image checksum validation failed" );
			*final_message = const_cast<char*>( "Image checksum validation failed" );
			success_status = false;
		}
		else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_SIGNATURE_FAILED )
		{
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_SIGNATURE_VERIFICATION_FAILURE );
#endif
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image signature verification failed" );
			*final_message = const_cast<char*>( "Image signature verification failed" );
			success_status = false;
		}
		else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED )
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Product code validation failed" );
			*final_message = const_cast<char*>( "Product code validation failed" );
			success_status = false;
		}
		else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED )
		{
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_VERSION_ROLLBACK_FAILURE );
#endif
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image version roll back failed" );
			*final_message = const_cast<char*>( "Image version roll back failed" );
			success_status = false;
		}
	}
	else if ( BF_FUS::REQUEST_INVALID == return_status )
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image validation request invalid - Processor/image not available" );
		*final_message = const_cast<char*>( "Image validation request invalid - Processor/image not available" );
		success_status = false;
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Image validation failed" );
		*final_message = const_cast<char*>( "Image validation failed" );
		success_status = false;
	}
	return ( success_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Create_Session( uint32_t session_timeout, IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	session_info_resp_t session_resp = { 0 };
	session_info_req_t session_req = { };
	op_status_t return_status = BF_FUS::SUCCESS;

	session_req.timeout_sec = session_timeout;
	bool success = false;

	if ( session_req.timeout_sec > MAX_SESSION_TIMEOUT_SEC )
	{
		/* Not sure which HTTP error code to be used here. Used 500 indicating cancellation */
		command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
		command_response->msg = const_cast<char*>( "Session timeout too large" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Session timeout %d is too large. Max allowed timeout is %d seconds",
						 session_req.timeout_sec, MAX_SESSION_TIMEOUT_SEC );
	}
	else if ( session_req.timeout_sec == 0 )
	{
		command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
		command_response->msg = const_cast<char*>( "Session timeout is less than minimum value" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Session timeout is less than minimum value" );
	}
	else
	{
		session_req.interface = fus_if_t::IOT_FUS;
		return_status = IOT_Device_Command::m_job_control->fus_handle->Goto_FUM( &session_req, &session_resp );
		if ( BF_FUS::SUCCESS == return_status )
		{
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Session created. Session id = %d, wait time = %d seconds",
							 session_req.session_id, session_resp.wait_time );

			command_response->status = HTTP_STATUS_OK;	// HTTP_STATUS_CREATED;
			command_response->msg = const_cast<char*>( "FUS session created" );
			IOT_DEBUG_PRINT( DBG_MSG_INFO, "Accepting UpdateFirmware request for job: %s",
							 IOT_Device_Command::m_job_control->job_uuid );
			success = true;
		}
		else if ( BF_FUS::REJECT == return_status )	/* Status when firmware upgrade DCID is disabled */
		{
			command_response->status = HTTP_STATUS_FORBIDDEN;	/* Access denied */
			command_response->msg = const_cast<char*>( "FUS access denied" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "FUS access denied" );
		}
		else if ( BF_FUS::REQUEST_INVALID == return_status )
		{
			command_response->status = HTTP_STATUS_UNPROCESSABLE;
			command_response->msg = const_cast<char*>( "Create session - request invalid" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Create session - request invalid" );
		}
		else
		{
			command_response->status = HTTP_STATUS_INTERNAL_SERVER_ERROR;
			command_response->msg = const_cast<char*>( "Create session - Unknown error" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Create session - Unknown error" );
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Commit( const char** final_message )
{
	image_commit_resp_t commit_resp;
	image_commit_req_t commit_req;

	commit_req.comp_id = IOT_Device_Command::m_job_control->processor_index;
	commit_req.image_id = IOT_Device_Command::m_job_control->image_index;
	commit_req.session_id = IOT_Fus::Get_Firmware_Session_Id();
	op_status_t return_status = BF_FUS::SUCCESS;
	bool success = false;

	return_status = IOT_Device_Command::m_job_control->fus_handle->Commit( &commit_req, &commit_resp );
	if ( BF_FUS::SUCCESS == return_status )
	{
		success = true;
		IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Commit initiated successfully" );
	}
	else if ( BF_FUS::REJECT == return_status )
	{
		*final_message = "Session ID mismatch while initiating commit";
	}
	else if ( BF_FUS::REQUEST_INVALID == return_status )
	{
		*final_message = "Commit initiation request invalid - Processor/image not available";
	}
	else
	{
		*final_message = "Failed to copy image to flash";
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Status( const char** final_message, void* owner_context, bool* is_command_scheduled )
{
	image_status_req_t status_req;
	image_status_resp_t status_resp = { 0 };
	op_status_t return_status = BF_FUS::SUCCESS;

	status_req.proc_index = IOT_Device_Command::m_job_control->processor_index;
	status_req.image_index = IOT_Device_Command::m_job_control->image_index;
	bool success = false;

	if ( IOT_Fus::Firmware_Upgrade_Inprogress() == true )
	{
		return_status = IOT_Device_Command::m_job_control->fus_handle->Status( &status_req, &status_resp );
		if ( BF_FUS::SUCCESS == return_status )
		{

			if ( status_resp.status == BF_FUS::FUS_COMMIT_DONE )
			{
				IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Copy image to flash done" );
				success = true;
			}
			else if ( status_resp.status == BF_FUS::FUS_PENDING_CALL_BACK )
			{
				IOT_DEBUG_PRINT( DBG_MSG_DEBUG,
								 "Status - Waiting for pending callback. Remaining op_time = %d milliseconds",
								 status_resp.remaining_op_time );
				/* Schedule next call to status after remaining_op_time delay */
				IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );
				iot_dev_cmd->m_iot_net->Schedule_Device_Command( status_resp.remaining_op_time,
																 IOT_Device_Command::SCHEDULED_STATUS_FUS );
				/* Will return false because status is just schedule(not completed yet) */
				*is_command_scheduled = true;
			}
			else
			{
				*final_message = "Status - Failed to copy image to flash";
			}
		}
		else if ( BF_FUS::REQUEST_INVALID == return_status )
		{
			*final_message = "Status request invalid - Processor/image not available";
		}
		else
		{
			*final_message = "Failed to get status after copying image to flash";
		}
	}
	else
	{
		*final_message = "Session doesn't exist";
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Exit_Firmware_Session( const char** final_message )
{
	session_info_resp_t session_resp;
	session_info_req_t session_req;
	bool success = false;

	session_req.session_id = IOT_Fus::Get_Firmware_Session_Id();
	/* Delete the session */
	op_status_t return_status = IOT_Device_Command::m_job_control->fus_handle->Exit_FUM( &session_req,
																						 &session_resp );

	if ( BF_FUS::SUCCESS == return_status )
	{
		// We'll claim success
		success = true;
	}
	else if ( BF_FUS::REQUEST_INVALID == return_status )
	{
		*final_message = "Session id not matched";
	}
	else
	{
		*final_message = "Failed to delete the session";
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Firmware_Upgrade_Inprogress( void )
{
	return ( IOT_Device_Command::m_job_control->fus_handle->Is_Session_Alive() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Get_Firmware_Session_Id( void )
{
	return ( IOT_Device_Command::m_job_control->fus_handle->Get_Session_Id() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Reboot_Required( void )
{
	if ( IOT_Device_Command::m_job_control == nullptr )
	{
		/*  m_job_control is allocated and deallocated only during IOT FUS process
		 *  m_job_control=nullptr indicates that FUS process is being done by some
		 *	other interface so we simply return false here
		 */
		return ( false );
	}
	else
	{
		return ( IOT_Device_Command::m_job_control->is_reboot_required );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Fw_Reboot( void )
{
	IOT_DEBUG_PRINT( DBG_MSG_INFO, "Rebooting now ..." );
	OS_Tasker::Delay( 1 * MULTIPLIER_1000MS );
	BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Handle_Image_Transfer( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle,
									 IOT_DEVICE_COMMAND_RESPONSE* command_response, void* owner_context )
{
	const char* const* keys = nullptr;
	const char* const* values = nullptr;
	size_t mcount = 0;

	MAP_RESULT result = Map_GetInternals( iot_map_handle, &keys, &values, &mcount );

	BF_ASSERT( result == MAP_OK );

	IOT_DEBUG_PRINT( DBG_MSG_INFO, "Processing ImageTransfer request for %u parameters:", mcount );
	IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );

	// Check that this job is in progress
	if ( IOT_Device_Command::m_job_control == nullptr )
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "No Firmware Update in progress" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Attempted %s ImageTransfer when no job is in progress",
						 command->commandID );
		return;
	}
	if ( strcmp( IOT_Device_Command::m_job_control->job_uuid, command->commandID ) != 0 )
	{
		command_response->status = HTTP_STATUS_LOCKED;
		command_response->msg = const_cast<char*>( "Wrong job UUID for ImageTransfer" );
		IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Attempted Job %s ImageTransfer when %s already in progress",
						 command->commandID, IOT_Device_Command::m_job_control->job_uuid );
		return;
	}

	bool success = true;
	size_t offset = 0;
	size_t chunk_size = 0;
	bool last_chunk = false;
	const char* base64_encoding = "BASE64";
	char* data_encoded = nullptr;

	for ( uint16_t i = 0; i < mcount; i++ )
	{
		char* param_name = const_cast<char*>( keys[i] );
		char* value_str = const_cast<char*>( values[i] );
		// IOT_DEBUG_PRINT( DBG_MSG_INFO, "Param %s = %s", param_name, value_str );
		if ( strcmp( param_name, "data" ) == 0 )
		{
			data_encoded = value_str;
		}
		else if ( strcmp( param_name, "encode" ) == 0 )
		{
			if ( strcasecmp( value_str, base64_encoding ) != 0 )
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid decoding type: %s", value_str );
				success = false;
			}
		}
		else if ( strcmp( param_name, "address" ) == 0 )
		{
			offset = strtoul( value_str, nullptr, 10 );
		}
		else if ( strcmp( param_name, "size" ) == 0 )
		{
			chunk_size = strtoul( value_str, nullptr, 10 );
			if ( chunk_size == 0 )
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid chunk_size: %s", value_str );
				success = false;
			}
		}
		else if ( strcmp( param_name, "more" ) == 0 )
		{
			if ( strcasecmp( value_str, "true" ) == 0 )
			{
				last_chunk = false;
			}
			else if ( strcasecmp( value_str, "false" ) == 0 )
			{
				last_chunk = true;
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Received the last chunk." );
			}
			else
			{
				IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid 'more' flag: %s", value_str );
				success = false;
			}
		}
		else
		{
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Unhandled ImageTransfer parameter %s = %s",
							 param_name, value_str );
			success = false;
		}
	}

	if ( success )
	{
		// Check for a missing critical parameter
		if ( ( data_encoded == nullptr ) || ( chunk_size == 0 ) )
		{
			command_response->status = HTTP_STATUS_BAD_REQUEST;
			command_response->msg = const_cast<char*>( "Missing a required parameter" );
			IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Missing a required parameter: data or size" );
			success = false;
		}
		else
		{
			// Do the base64 decode
			size_t decoded_len = 0;
			size_t encoded_len = strlen( data_encoded );
			// First, a quick check: the PXGreen binary files often end in CR-LF-EOF
			// So look for those and remove them, if found. No valid characters less than '/' (0x2F)
			if ( last_chunk )
			{
				for ( size_t i = 0; i < 3; i++ )
				{
					if ( data_encoded[encoded_len - 1] < '/' )
					{
						data_encoded[encoded_len - 1] = 0;
						encoded_len--;
					}
				}
			}
			unsigned char* decoded_data = iot_base64_decode( data_encoded, encoded_len, &decoded_len );
			if ( decoded_data != nullptr )
			{
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Received and decoded %u bytes for offset %u.", decoded_len, offset );

				// Now hand over this buffer to FUS
				success = IOT_Fus::Write_Image_To_Scratch( decoded_data, decoded_len, offset, command_response );

				// And free the decoded data buffer
				iot_free( decoded_data );

			}
			else
			{
				command_response->status = HTTP_STATUS_BAD_REQUEST;
				command_response->msg = const_cast<char*>( "Decoding of data buffer failed" );
				printf( "Data: %s", data_encoded );		// Use printf, so no length limit, to see what the failing data
														// was
				IOT_DEBUG_PRINT( DBG_MSG_INFO, "Encoded Data length: %u", chunk_size );
				success = false;
			}
		}
		if ( success )
		{
			if ( last_chunk )
			{
				// We made it (so far)!
				// Send a DeviceCommandStatus update saying that we're beginning the FUS process
				iot_dev_cmd->Update_Device_Command_Status( "Step 1", COMMAND_PROCESSING, "Starting Flash Update" );

				/* Validate the image for good header, image data, CRC, Code signing and product info(Like product code,
				   roll back and version)  */
				const char* final_message = "";
				IOT_COMMAND_STATUS step_status = COMMAND_FAILED;
				bool is_command_scheduled = false;
				/* Here, final_message will be filled only when there's error */
				success = IOT_Fus::Validate_Image( &final_message, reinterpret_cast<void*>( iot_dev_cmd ),
												   &is_command_scheduled );
				if ( success )	/* Validation successful in first attempt(Waiting not required) */
				{
					/* Now, here is where we do the FUS to transfer from Scratch to the real area.
					 * If updating the App, before rebooting, be sure to send the final DeviceCommandStatus update.
					 */
					/* Commit i.e copy image from scratch to flash */
					success = IOT_Fus::Commit( &final_message );/* Scheduling not required */
					if ( success )
					{
						is_command_scheduled = false;
						/* 2 steps - status and exit FUS session */
						success = IOT_Fus::Do_FUS_Update( &final_message, reinterpret_cast<void*>( iot_dev_cmd ),
														  &is_command_scheduled );
					}

				}
				/* Send status response to "UpdateFirmware" command when either of the following conditions are true:
				 * 1. Send "success" response if all steps(validate, commit, status, exit) of FUS update succeeded in
				 *    first attempt(Waiting not required)
				 * 2. Send "fail" response if any of the steps fail.
				 *    IOT_Fus::Validate_Image() and IOT_Fus::Do_FUS_Update() return success=false even when
				 *    command is scheduled. We don't consider this as an "error". In this case, we will send
				 *    response from IOT_Net::Timer_Task(), not here
				 *      */
				if ( success )
				{
					step_status = COMMAND_SUCCEEDED;
					// Now send the final DeviceCommandStatus update
					// The task_text MUST be "UpdateFirmware" for PXWhite to recognize that we are done.
					iot_dev_cmd->Update_Device_Command_Status( "UpdateFirmware", step_status, final_message );
				}
				else if ( is_command_scheduled == false )
				{
					/* Error condition - Command is not scheduled and success=false  */
					// step_status is COMMAND_FAILED;
					// Now send the final DeviceCommandStatus update
					// The task_text MUST be "UpdateFirmware" for PXWhite to recognize that we are done.
					iot_dev_cmd->Update_Device_Command_Status( "UpdateFirmware", step_status, final_message );

					/* Also delete ongoing FUS session and job after 1 second */
					iot_dev_cmd->m_iot_net->Schedule_Device_Command( MULTIPLIER_1000MS,
																	 IOT_Device_Command::SCHEDULED_RESET_FUS );
				}
				else
				{
					// misra
					/* Nothing to do here. Scheduled command will execute in state machine implemented in
					    IOT_Net::Timer_Task()  */
				}
			}
			else
			{
				// Re-schedule (postpone) the Update Job Reset in case we get lost in an update, and timeout
				const time_t command_timeout_sec = iot_dev_cmd->Get_Command_Timeout_Seconds();
				iot_dev_cmd->m_iot_net->Schedule_Device_Command( command_timeout_sec * MULTIPLIER_1000MS,
																 IOT_Device_Command::SCHEDULED_RESET_FUS );
			}
		}
	}
	else
	{
		command_response->status = HTTP_STATUS_BAD_REQUEST;
		command_response->msg = const_cast<char*>( "Missing required parameter" );
		// But don't abort here; maybe it was just a bad message?
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IOT_Fus::Command_Handler( const IOT_DEVICE_COMMAND* command, MAP_HANDLE iot_map_handle, void* owner_context,
							   IOT_DEVICE_COMMAND_RESPONSE* command_response )
{
	bool return_status = true;

    //Ahmed start OTA

	// Check if the method received is the UpdateFirmware request
	if ( !strcmp( command->commandMethod, IMAGE_TRANSFER_COMMAND ) )
	{
		IOT_Fus::Handle_Image_Transfer( command, iot_map_handle, command_response, owner_context );
	}
	// Check if the method received is the UpdateFirmware request
	else if ( !strcmp( command->commandMethod, UPDATE_FIRMWARE_COMMAND ) )
	{
		IOT_Fus::Update_Firmware( command, iot_map_handle, command_response, owner_context );
	}
	else
	{
		return_status = false;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Scheduled_State_Machine_Handler( void* owner_context )
{
	const char* final_message = "";
	IOT_COMMAND_STATUS step_status = COMMAND_FAILED;
	bool success = false;
	bool is_command_scheduled = false;
	IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );

	if ( iot_dev_cmd == nullptr )
	{
		return;		// Can't do anything here
	}

	switch ( iot_dev_cmd->m_iot_net->m_scheduled_state )
	{
		case IOT_Device_Command::SCHEDULED_RESET_FUS:
#ifndef ESP32_SETUP
			Capture_Audit_Log( LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_ABORTED );
#endif
			iot_dev_cmd->Reset();
			break;

		case IOT_Device_Command::SCHEDULED_VALIDATE_FUS:
			/* Here, final_message will be filled only when there's error */
			success = IOT_Fus::Validate_Image( &final_message,  reinterpret_cast<void*>( iot_dev_cmd ),
											   &is_command_scheduled );
			if ( success )	/* Validation successful in first attempt(Waiting not required) */
			{
				/* Commit i.e copy image from scratch to flash */
				success = IOT_Fus::Commit( &final_message );/* Scheduling not required */
				if ( success )
				{
					/* Execute status operation now i.e after 1 second */
					iot_dev_cmd->m_iot_net->Schedule_Device_Command( MULTIPLIER_1000MS,
																	 IOT_Device_Command::SCHEDULED_STATUS_FUS );
					/* Go to next state immediately */
				}

			}
			else if ( is_command_scheduled == false )
			{
				/* Error condition - Command is neither scheduled nor success */
				step_status = COMMAND_FAILED;
				iot_dev_cmd->Update_Device_Command_Status( "UpdateFirmware", step_status, final_message );

				/* Also delete ongoing FUS session and job after 1 second */
				iot_dev_cmd->m_iot_net->Schedule_Device_Command( MULTIPLIER_1000MS,
																 IOT_Device_Command::SCHEDULED_RESET_FUS );
			}
			else
			{
				// misra
			}
			break;

		case IOT_Device_Command::SCHEDULED_STATUS_FUS:
			success = IOT_Fus::Do_FUS_Update( &final_message,  reinterpret_cast<void*>( iot_dev_cmd ),
											  &is_command_scheduled );
			/* 2 steps - status and exit FUS session */

			/* success could be false when status operation is scheduled(not completed) */
			if ( success )	/* If FUS update succeeded in first attempt(Waiting not required) */
			{
				step_status = COMMAND_SUCCEEDED;
				// Now send the final DeviceCommandStatus update
				// The task_text MUST be "UpdateFirmware" for PXWhite to recognize that we are done.
				iot_dev_cmd->Update_Device_Command_Status( "UpdateFirmware", step_status, final_message );
			}
			else if ( is_command_scheduled == false )
			{
				/* Error condition - Command is neither scheduled nor success */
				step_status = COMMAND_FAILED;
				iot_dev_cmd->Update_Device_Command_Status( "UpdateFirmware", step_status, final_message );

				/* Also delete ongoing FUS session and job after 1 second */
				iot_dev_cmd->m_iot_net->Schedule_Device_Command( MULTIPLIER_1000MS,
																 IOT_Device_Command::SCHEDULED_RESET_FUS );
			}
			else
			{
				// misra
			}
			break;

		case IOT_Device_Command::SCHEDULED_SYSTEM_RESET_FUS:
			IOT_Fus::Fw_Reboot();	/* Hardware reset */
			break;

		default:
			// Some other state, eg SCHEDULED_NONE; do nothing here
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::D2C_State_Machine_Handler( void* owner_context )
{
	IOT_Device_Command* iot_dev_cmd = reinterpret_cast<IOT_Device_Command*>( owner_context );

	switch ( iot_dev_cmd->m_iot_net->m_timer_task_state )
	{
		case IOT_Net::SEND_TRANSFER_IMAGE:
		{
			iot_dev_cmd->Send_D2C_Command( iot_dev_cmd->m_iot_net->m_device_handle, TRANSFER_IMAGE );
			// Now revert to the previous state
			iot_dev_cmd->m_iot_net->m_timer_task_state = iot_dev_cmd->m_iot_net->m_previous_task_state;
		}
		break;

		case IOT_Net::SEND_DEVICE_COMMAND_STATUS:
		{
			iot_dev_cmd->Send_D2C_Command( iot_dev_cmd->m_iot_net->m_device_handle, DEVICE_COMMAND_STATUS );
			// Now revert to the previous state
			iot_dev_cmd->m_iot_net->m_timer_task_state = iot_dev_cmd->m_iot_net->m_previous_task_state;
		}
		break;

		default:
			// Some other state, eg NOT_CONNECTED; do nothing here
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Get_Image_Size( uint8_t processor_index, uint8_t image_index )
{
	uint32_t image_size = 0;
	NV_Ctrl* image_nv_ctrl = nullptr;

	Prod_Spec_FUS_Get_Image_NV_Handle( processor_index, image_index, &image_nv_ctrl );
	if ( image_nv_ctrl != nullptr )
	{
		NV_Ctrl::mem_range_info_t mem_range = {0U};
		image_nv_ctrl->Get_Memory_Info( &mem_range );
		image_size = mem_range.end_address - mem_range.start_address + 1;
		/* The App image fills up the entire space and has 3-4KB of overhead, so we have to make
		 * an allowance for that case of 4KB. Will have to do something in FUS too, to not overrun our Scratch area. */
		if ( image_index == 0 )
		{
			image_size += CODE_SIGN_INFO_MAX_SIZE;
			IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "Adding 4KB to allowed Image Size for App" );
		}
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "Failed to get image NV handle" );
	}
	return ( image_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t IOT_Fus::Get_Image_Start_Address( uint8_t processor_index, uint8_t image_index )
{
	uint32_t image_start_address = 0;
	NV_Ctrl* image_nv_ctrl = nullptr;

	Prod_Spec_FUS_Get_Image_NV_Handle( processor_index, image_index, &image_nv_ctrl );
	if ( image_nv_ctrl != nullptr )
	{
		NV_Ctrl::mem_range_info_t mem_range = {0U};
		image_nv_ctrl->Get_Memory_Info( &mem_range );
		image_start_address = mem_range.start_address;
	}
	else
	{
		IOT_DEBUG_PRINT( DBG_MSG_CRITICAL, "Failed to get image NV handle" );
	}
	return ( image_start_address );
}

#ifndef ESP32_SETUP
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Register_Audit_Log_Callback( IOT_FUS_Audit_Function_Decl_1 fw_audit_log_cb,
										   IOT_FUS_Audit_Function_Decl_2 fw_audit_param_cb )
{
	iot_fus_log_audit_capture_cb = fw_audit_log_cb;
	iot_fus_log_param_cb = fw_audit_param_cb;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IOT_Fus::Capture_Audit_Log( log_event_t event_code )
{
	///< Capture log parameters
	if ( iot_fus_log_param_cb != nullptr )
	{
		switch ( event_code )
		{
			case LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_STARTED:
				if ( IOT_Device_Command::m_job_control )
				{
					uint8_t* fw_param =
						reinterpret_cast<uint8_t*>( iot_malloc( User_Account::USER_NAME_LIMIT + 3U +
																IOT_MAX_DEVICE_UUID_SIZE ) );
					BF_ASSERT( fw_param != nullptr );
					/** Fill all the params
					 ***************************************************************************************************
					 * fw_param[0] = "Not applicable"                         --fw_param[0] to fw_param[20] is user name
					 * fw_param[User_Account::USER_NAME_LIMIT + 1] = proc_id  --fw_param[21] is processor id
					 * fw_param[User_Account::USER_NAME_LIMIT + 2] = img_id   --fw_param[22] is image id
					 * fw_param[User_Account::USER_NAME_LIMIT + 3] = job_uuid --fw_param[23] to fw_param[59] is job id
					 ***************************************************************************************************
					 */
					strncpy( reinterpret_cast<char*>( fw_param ), "Not Applicable", 15 );	///< Fixed string length
					fw_param[User_Account::USER_NAME_LIMIT + 1] = IOT_Device_Command::m_job_control->processor_index;
					fw_param[User_Account::USER_NAME_LIMIT + 2] = IOT_Device_Command::m_job_control->image_index;
					memcpy( &fw_param[User_Account::USER_NAME_LIMIT + 3],
							reinterpret_cast<uint8_t*>( IOT_Device_Command::m_job_control->job_uuid ),
							IOT_MAX_DEVICE_UUID_SIZE );

					///< Pass params to callback
					( *iot_fus_log_param_cb )( LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_STARTED, fw_param );
					iot_free( fw_param );
				}
				break;

			case LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_COMPLETED:
				if ( IOT_Device_Command::m_job_control )
				{
					uint8_t fw_param[4] = { 0 };
					image_info_req_t image_info;
					image_info_resp_t image_resp = { 0 };
					uint32_t rev_num = 0U;

					image_info.proc_id = IOT_Device_Command::m_job_control->processor_index;
					image_info.image_id = IOT_Device_Command::m_job_control->image_index;

					if ( BF_FUS::SUCCESS ==
						 IOT_Device_Command::m_job_control->fus_handle->Image( &image_info, &image_resp ) )
					{
						rev_num = image_resp.version;
					}
					else
					{
						IOT_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get firmware version" );
					}
					///< Pass firmware revision to log param callback.
					fw_param[2] = ( ( rev_num ) & 0x000000ff );
					fw_param[3] = ( ( rev_num ) & 0x0000ff00 ) >> 8;
					fw_param[1] = ( ( rev_num ) & 0x00ff0000 ) >> 16;
					fw_param[0] = ( ( rev_num ) & 0xff000000 ) >> 24;
					( *iot_fus_log_param_cb )( LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_COMPLETED, fw_param );
				}
				break;

			default:
				break;
		}
	}

	///< Capture log using event code
	if ( iot_fus_log_audit_capture_cb != nullptr )
	{
		( *iot_fus_log_audit_capture_cb )( event_code );
	}
}

#endif
