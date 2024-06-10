/**
 *****************************************************************************************
 * @file
 * @details REST_WebUI_interface which is used for the loading of Webpages.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Rest_WebUI_Interface.h"
#include "server.h"
#include "Babelfish_Assert.h"
#include "NV_Ctrl.h"
#include "StdLib_MV.h"
#include "Web_Debug.h"
#include "OS_Binary_Semaphore.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
   WEBUI_ON_EXT_FLASH macro need to be define in the common_define_config.xcl file
   Example: -D WEBUI_ON_EXT_FLASH
 */
#ifdef WEBUI_ON_EXT_FLASH

struct webui_metadata_s
{
	uint8_t fw_www_file_data[WEB_PAGE_READ_BUFF_SIZE];

	const uint8_t* fw_www_hdr_ptr;
};

webui_metadata_s webui_mdata;

// Protect HTTP and HTTPS from simultaneous read from NV and transmit the data
OS_Binary_Semaphore* webui_semaphore = nullptr;
#endif

static NV_Ctrl* web_nv_ctrl = nullptr;


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const fw_www_header* Get_Fw_WWW_Header( NV_Ctrl* web_ui_nv_ctrl )
{
	NV_Ctrl::mem_range_info_t mem_range_info = {0};

	web_nv_ctrl = web_ui_nv_ctrl;

	BF_ASSERT( web_nv_ctrl != nullptr );

	// coverity[var_deref_model]
	web_nv_ctrl->Get_Memory_Info( &mem_range_info );

#ifdef WEBUI_ON_EXT_FLASH

	NV_Ctrl::nv_status_t status = NV_Ctrl::BUSY_ERROR;
	uint8_t temp_buffer[sizeof( fw_www_header )] = {0};

	uint32_t offset = ( sizeof( crc_info ) + sizeof( fw_header ) + sizeof( prodinfo_for_boot_s ) );

	status =
		web_nv_ctrl->Read( ( reinterpret_cast<uint8_t*>( &temp_buffer ) ),
						   ( mem_range_info.start_address + offset ),
						   sizeof( fw_www_header ), false );

	if ( status != NV_Ctrl::SUCCESS )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image Header Info From NV Memory" );
		return ( nullptr );
	}

	uint32_t tot_entries =
		Fw_WWW_Total_Entries( reinterpret_cast<const fw_www_header*>( &temp_buffer ) );

	uint32_t buffer_size = ( sizeof( fw_www_header ) ) + ( ( sizeof( fw_data_entry ) ) * tot_entries );

	// free webui_mdata.fw_www_hdr_ptr if initalized earlier
	if ( webui_mdata.fw_www_hdr_ptr != nullptr )
	{
		Ram::De_Allocate_Basic( const_cast<uint8_t*>( webui_mdata.fw_www_hdr_ptr ) );
		webui_mdata.fw_www_hdr_ptr = nullptr;
	}

	webui_mdata.fw_www_hdr_ptr =
		( reinterpret_cast<const uint8_t*>( Ram::Allocate_Basic( buffer_size, true, 0x00 ) ) );
	if ( webui_mdata.fw_www_hdr_ptr == nullptr )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Allocate Memory to Store Web Image Header Info" );
		return ( nullptr );
	}

	// coverity[var_deref_model]
	status =
		web_nv_ctrl->Read( ( const_cast<uint8_t*>( webui_mdata.fw_www_hdr_ptr ) ),
						   ( mem_range_info.start_address + offset ),
						   buffer_size, false );
	if ( status != NV_Ctrl::SUCCESS )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image Header Info From NV Memory" );
		return ( nullptr );
	}

	return ( reinterpret_cast<const fw_www_header*>( webui_mdata.fw_www_hdr_ptr ) );

#else
	const real_fw_header* hdr =
		( reinterpret_cast<const real_fw_header*>( mem_range_info.start_address ) );
	/* Make sure header type is WWW
	   Initialize web page state.*/
	return ( reinterpret_cast<const fw_www_header*>( hdr->data ) );

#endif

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Fw_WWW_Total_Entries( const fw_www_header* www )
{
	return ( static_cast<uint32_t>( static_cast<uint32_t>( www->mime_type_count ) +
									( 2U * static_cast<uint32_t>( www->file_count ) ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* Fw_WWW_Data_Begin( const fw_www_header* www )
{
	const uint8_t* data_begin = nullptr;

	uint16_t server_flag = Get_Server_Flags();

	if ( ( server_flag & HTTP_SERVER_FLAG_WEB_PAGES ) ==
		 HTTP_SERVER_FLAG_WEB_PAGES )
	{
		const uint8_t* x = reinterpret_cast<const uint8_t*>( www );
		x -= sizeof( real_fw_header );
		x += 4U;// size of real_fw_header.data[4]
		const real_fw_header* fw = ( reinterpret_cast<const real_fw_header*>( x ) );
		data_begin = fw->data;
		data_begin = data_begin + sizeof( fw_www_header ) +
			( ( sizeof( fw_data_entry ) ) * ( Fw_WWW_Total_Entries( www ) ) );
	}
	else if ( ( server_flag & HTTP_SERVER_FLAG_MAINTENANCE_PAGES ) ==
			  HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
	{
		data_begin = ( ( const uint8_t* )www + ( sizeof( fw_www_header ) )
					   + ( ( sizeof( fw_data_entry ) ) * ( Fw_WWW_Total_Entries( www ) ) ) );
	}
	else
	{
		/* Atleast one page ( Maintenance or WebUI ) should be available */
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "HTTP Server Page Not Available" );
		BF_ASSERT( false );
	}
	return ( data_begin );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Fw_WWW_Get_Filesize( const fw_www_header* www, uint32_t idx )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );

	WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "File Size = %ld",
						   wwwp->entries[www->mime_type_count + www->file_count + idx].length );
	return ( wwwp->entries[www->mime_type_count + www->file_count + idx].length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const fw_www_file* Fw_WWW_Get_File( const fw_www_header* www, uint32_t idx, const uint8_t* www_file_ptr )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );
	const uint8_t* data_ptr = nullptr;

	// get length of the real_fw_www_file
	uint32_t length = sizeof( real_fw_www_file );

#ifdef WEBUI_ON_EXT_FLASH
	uint16_t server_flag = Get_Server_Flags();
	uint_fast8_t read_count = 0;
	if ( webui_semaphore == nullptr )
	{
		webui_semaphore = new OS_Binary_Semaphore( true );
	}

	if ( server_flag == HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
	{
		data_ptr = ( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www ) +
													   wwwp->entries[www->mime_type_count + www->file_count +
																	 idx].offset ) );
		BF_Lib::Copy_String( ( const_cast<uint8_t*>( www_file_ptr ) ), data_ptr, length );
	}
	else
	{
		NV_Ctrl::nv_status_t status = NV_Ctrl::BUSY_ERROR;
		NV_Ctrl::mem_range_info_t mem_range_info = {0};
		// find the start address of wwwp structure starting from address www
		uint32_t offset =
			( reinterpret_cast<uint32_t>( Fw_WWW_Data_Begin( www ) ) ) - ( reinterpret_cast<uint32_t>( www ) ) +
			( sizeof( real_fw_header ) - 4 ) + ( wwwp->entries[www->mime_type_count + www->file_count + idx].offset );

		web_nv_ctrl->Get_Memory_Info( &mem_range_info );

		do
		{
			status = web_nv_ctrl->Read( ( const_cast<uint8_t*>( www_file_ptr ) ),
										( mem_range_info.start_address + offset ), length, false );

			if ( status == NV_Ctrl::BUSY_ERROR )
			{
				read_count++;
				OS_Tasker::Delay( 1 );
			}
		}while( ( status == NV_Ctrl::BUSY_ERROR ) && ( read_count < MAX_NUMBER_OF_READS ) );

		if ( status != NV_Ctrl::SUCCESS )
		{
			WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image File Info From NV Memory" );
			return ( nullptr );
		}
	}
#else
	data_ptr =
		( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www ) +
											wwwp->entries[www->mime_type_count + www->file_count + idx].offset ) );
	BF_Lib::Copy_String( ( const_cast<uint8_t*>( www_file_ptr ) ), data_ptr, length );
#endif

	return ( reinterpret_cast<const fw_www_file*>( www_file_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef WEBUI_ON_EXT_FLASH
const uint8_t* Fw_WWW_Get_File_Data_Chunk( hams_parser* parser, uint32_t address, uint32_t length )
{
	uint_fast8_t read_count = 0;
	NV_Ctrl::nv_status_t status = NV_Ctrl::BUSY_ERROR;

	NV_Ctrl::mem_range_info_t mem_range_info = {0};

	web_nv_ctrl->Get_Memory_Info( &mem_range_info );

	do
	{
		status = web_nv_ctrl->Read( ( reinterpret_cast<uint8_t*>( &webui_mdata.fw_www_file_data ) ), address,
									length, false );
		if ( status == NV_Ctrl::BUSY_ERROR )
		{
			read_count++;
			// WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Read Count = %ld", read_count );
			OS_Tasker::Delay( 1 );
		}
	}while( ( status == NV_Ctrl::BUSY_ERROR ) && ( read_count < MAX_NUMBER_OF_READS ) );

	if ( status != NV_Ctrl::SUCCESS )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image Data Chunk From NV Memory" );
		return ( nullptr );
	}

	return ( reinterpret_cast<const uint8_t*>( &webui_mdata.fw_www_file_data ) );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* Fw_WWW_Get_File_Data( hams_parser* parser, const fw_www_header* www, uint32_t idx )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );

#ifdef WEBUI_ON_EXT_FLASH
	uint16_t server_flag = Get_Server_Flags();
	NV_Ctrl::mem_range_info_t mem_range_info = {0};
	uint32_t file_offset = 0;

	if ( server_flag == HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
	{
		const struct real_fw_www_file* r =
			( reinterpret_cast<const struct real_fw_www_file*>( Fw_WWW_Data_Begin( www ) +
																wwwp->entries[www->mime_type_count + www->file_count +
																			  idx].offset ) );
		return ( r->data );
	}
	else
	{
		web_nv_ctrl->Get_Memory_Info( &mem_range_info );
		// find the start address of wwwp structure starting from address www
		file_offset =
			( reinterpret_cast<uint32_t>( Fw_WWW_Data_Begin( www ) ) - reinterpret_cast<uint32_t>( www ) +
			  ( sizeof( real_fw_header ) - 4 ) + sizeof( fw_www_file_s ) +
			  wwwp->entries[www->mime_type_count + www->file_count + idx].offset );

		return ( reinterpret_cast<const uint8_t*>( mem_range_info.start_address + file_offset ) );
	}
#else
	const struct real_fw_www_file* r =
		( reinterpret_cast<const struct real_fw_www_file*>( Fw_WWW_Data_Begin( www ) +
															wwwp->entries[www->mime_type_count + www->file_count +
																		  idx].offset ) );
	return ( r->data );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Fw_WWW_Get_Filename( const fw_www_header* www, uint32_t idx,  const uint8_t* filename_ptr )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );
	const uint8_t* data_ptr = nullptr;

	uint32_t length = wwwp->entries[www->mime_type_count + idx].length;

	if ( length > FILENAME_BUFFER_SIZE )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Filename Length Is Greater Than Buffer Size" );
		BF_ASSERT( false );
	}

#ifdef WEBUI_ON_EXT_FLASH
	uint16_t server_flag = Get_Server_Flags();
	uint_fast8_t read_count = 0;

	if ( server_flag == HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
	{
		data_ptr = ( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www )
													   + wwwp->entries[www->mime_type_count + idx].offset ) );
		BF_Lib::Copy_String( ( const_cast<uint8_t*>( filename_ptr ) ), data_ptr, length );
	}
	else
	{
		NV_Ctrl::nv_status_t status = NV_Ctrl::BUSY_ERROR;
		NV_Ctrl::mem_range_info_t mem_range_info = {0};

		uint32_t offset =
			( reinterpret_cast<uint32_t>( Fw_WWW_Data_Begin( www ) ) - reinterpret_cast<uint32_t>( www ) +
			  ( sizeof( real_fw_header ) - 4 ) + wwwp->entries[www->mime_type_count + idx].offset );

		web_nv_ctrl->Get_Memory_Info( &mem_range_info );

		do
		{
			status = web_nv_ctrl->Read( ( const_cast<uint8_t*>( filename_ptr ) ),
										( mem_range_info.start_address + offset ), length,
										false );
			if ( status == NV_Ctrl::BUSY_ERROR )
			{
				read_count++;
				OS_Tasker::Delay( 1 );
			}
		}while( ( status == NV_Ctrl::BUSY_ERROR ) && ( read_count < MAX_NUMBER_OF_READS ) );

		if ( status != NV_Ctrl::SUCCESS )
		{
			WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image Filenames From NV Memory" );
			// copy null string to clear the buffer
			BF_Lib::Copy_String( ( const_cast<uint8_t*>( filename_ptr ) ), 0x00, length );
		}
	}
#else
	data_ptr =
		( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www ) +
											wwwp->entries[www->mime_type_count + idx].offset ) );

	BF_Lib::Copy_String( ( const_cast<uint8_t*>( filename_ptr ) ), data_ptr, length );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Fw_WWW_Get_Mimetype_Length( const fw_www_header* www, uint32_t idx )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );

	WEBSERVER_DEBUG_PRINT( DBG_MSG_DEBUG, "MimeType Length = %ld", wwwp->entries[idx].length );
	return ( wwwp->entries[idx].length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Fw_WWW_Get_Mimetype( const fw_www_header* www, uint32_t idx, const uint8_t* mime_ptr )
{
	const struct real_fw_www_header* wwwp = ( reinterpret_cast<const struct real_fw_www_header*>( www ) );
	const uint8_t* data_ptr = nullptr;
	uint32_t length = wwwp->entries[idx].length;

	if ( length > MIMETYPE_BUFFER_SIZE )
	{
		WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Mime Type Length Is Greater Than Buffer Size" );
		BF_ASSERT( false );
	}

#ifdef WEBUI_ON_EXT_FLASH
	uint16_t server_flag = Get_Server_Flags();
	uint_fast8_t read_count = 0;

	if ( server_flag == HTTP_SERVER_FLAG_MAINTENANCE_PAGES )
	{
		data_ptr = ( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www ) + wwwp->entries[idx].offset ) );

		BF_Lib::Copy_String( ( const_cast<uint8_t*>( mime_ptr ) ), data_ptr, length );

	}
	else
	{
		NV_Ctrl::nv_status_t status = NV_Ctrl::BUSY_ERROR;
		NV_Ctrl::mem_range_info_t mem_range_info = {0};

		uint32_t offset =
			( reinterpret_cast<uint32_t>( Fw_WWW_Data_Begin( www ) ) - reinterpret_cast<uint32_t>( www ) +
			  ( sizeof( real_fw_header ) - 4 ) + wwwp->entries[idx].offset );

		web_nv_ctrl->Get_Memory_Info( &mem_range_info );

		do
		{
			status =
				web_nv_ctrl->Read( ( const_cast<uint8_t*>( mime_ptr ) ), ( mem_range_info.start_address + offset ),
								   length, false );

			if ( status == NV_Ctrl::BUSY_ERROR )
			{
				read_count++;
				OS_Tasker::Delay( 1 );
			}
		}while( ( status == NV_Ctrl::BUSY_ERROR ) && ( read_count < MAX_NUMBER_OF_READS ) );

		if ( status != NV_Ctrl::SUCCESS )
		{
			WEBSERVER_DEBUG_PRINT( DBG_MSG_ERROR, "Fail To Read Web Image File Mime Type From NV Memory" );
			// copy null string to clear the buffer
			BF_Lib::Copy_String( ( const_cast<uint8_t*>( mime_ptr ) ), 0x00, length );
		}

	}
#else
	data_ptr = ( reinterpret_cast<const uint8_t*>( Fw_WWW_Data_Begin( www ) + wwwp->entries[idx].offset ) );

	BF_Lib::Copy_String( ( const_cast<uint8_t*>( mime_ptr ) ), data_ptr, length );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Fw_WWW_Init_Search_Ctx( const fw_www_header* www, fw_www_search_ctx* ctx )
{
	ctx->low = 0U;
	ctx->high = www->file_count - 1U;

	ctx->str_idx = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Fw_WWW_Search( const fw_www_header* www, const uint8_t* path, uint32_t pathlen, fw_www_search_ctx* ctx )
{
	uint32_t return_val = 0U;
	bool loop_break = false;
	bool function_end = false;
	const uint8_t filename_buff[FILENAME_BUFFER_SIZE] = {0};
	const uint8_t* low_str = ( reinterpret_cast<const uint8_t*>( &filename_buff ) );
	const uint8_t* high_str = ( reinterpret_cast<const uint8_t*>( &filename_buff ) );

	if ( ( BAD_FILE == ctx->low ) || ( BAD_FILE == ctx->high ) )
	{
		return_val = 0U;
	}
	else
	{
		const uint8_t* x = path;
		const uint8_t* end = path + pathlen;
		while ( ( x != end ) && ( loop_break == false ) )
		{

			Fw_WWW_Get_Filename( www, ctx->low, low_str );
			while ( ( ctx->low <= ctx->high ) && ( *x > ( low_str[( ctx->str_idx + x ) - path] ) ) )
			{
				++ctx->low;
				Fw_WWW_Get_Filename( www, ctx->low, low_str );
			}

			if ( *x != ( low_str[( ctx->str_idx + x ) - path] ) )
			{
				ctx->low = BAD_FILE;
				ctx->high = BAD_FILE;
				return_val = 0U;
				loop_break = true;
				function_end = true;
			}
			else
			{
				Fw_WWW_Get_Filename( www, ctx->high, high_str );
				while ( ( ctx->low <= ctx->high ) &&
						( *x < ( high_str[( ctx->str_idx + x ) - path] ) ) )
				{
					--ctx->high;
					Fw_WWW_Get_Filename( www, ctx->high, high_str );
				}
				/* Go onto next character. */
				++x;
			}

		}
		if ( function_end == false )
		{
			ctx->str_idx += pathlen;
			return_val = static_cast<uint32_t>( ( ctx->high - ctx->low ) + 1U );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Fw_WWW_Match_Count( const fw_www_search_ctx* ctx )
{
	uint32_t return_val;

	if ( ( BAD_FILE == ctx->low ) || ( BAD_FILE == ctx->high ) )
	{
		return_val = 0U;
	}
	else
	{
		return_val = static_cast<uint32_t>( ( ctx->high - ctx->low ) + 1U );
	}

	return ( return_val );
}
