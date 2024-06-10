/**
 *****************************************************************************************
 * @file REST_Log.cpp
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Defines.h"
#include "httpd_xml.h"
#include "REST.h"
#include "REST_Log.h"
#include "server.h"
#include "Log_Config.h"
#include "ASCII_Conversion.h"
#include "DCI_Constants.h"
#include "StdLib_MV.h"
#include "REST_Common.h"
#include "Babelfish_Assert.h"
#include "User_Management.h"

#define LOG_OPTIONS   "GET, HEAD, OPTIONS"
#define LOG_TAIL_OPTIONS   "GET, HEAD, DELETE, OPTIONS"

using namespace BF_Misc;

/*
   Services like rs/param requires more number of data block while sending response. We need to limit the number of
      blocks used by large data service so that remaining blocks can be used by other services if any of new serice is
      received concurrently. So out of total 48 (HTTP_POOL_BLOCK_COUNT) blocks only 5 are used in each xmit function.
      This is similar like rs/param.
 */
static const uint16_t MAX_BLOCKS_FOR_LOG_DATA = 2U;

extern uint16_t Print_Rest_Format( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								   uint8_t* data, uint32_t tot_length, uint32_t offset );

extern uint8_t dest_data[];

static uint32_t Log_Id_Details_Cb( hams_parser* parser, const uint8_t* data,
								   uint32_t length );

static uint32_t Log_Parse_Tail_Cb( hams_parser* parser, const uint8_t* data,
								   uint32_t length );

static uint32_t Log_Data_From_Tail_Cb( hams_parser* parser, const uint8_t* data,
									   uint32_t length );

static uint32_t Log_Tail_Delete_Cb( hams_parser* parser, const uint8_t* data,
									uint32_t length );

static void Log_Entry_Details( hams_parser* parser, uint8_t log_id,
							   uint32_t start_index, uint32_t entry_count, bool data_after_tail );

static BF_Misc::Log_Mem** s_rest_log_mem_list = nullptr;
static BF_Misc::Log_DCI** s_rest_log_dci_list = nullptr;
static const DCI_ID_TD* s_rest_log_type_dcid_array = nullptr;

#ifndef REST_INTERFACE_TOTAL_LOGS
static const log_id_t s_rest_total_logs = TOTAL_LOG_ID;
#else
static const log_id_t s_rest_total_logs = REST_INTERFACE_TOTAL_LOGS;
#endif

/**
 * @brief Initialize REST log interface
 * @details This initialization allows REST to be able to access log memory (RAM/NV) instances, Log DCI instances and
 * DCID array of supported log types.
 * @param[in] log_mem_list: Pointer to the list of log memory(RAM/NV) handles created for each log type
 * @param[in] log_dci_list: Pointer to the list of log DCI handles created for each log type
 * @param[in] log_type_dcid_array: List of Log Type DCIDs
 * @param[in] log_type_dcid_array_length: Total number of Log Type DCIDs
 * @return None.
 */
void Init_REST_Log_Interface( BF_Misc::Log_Mem** log_mem_list, BF_Misc::Log_DCI** log_dci_list,
							  DCI_ID_TD const* log_type_dcid_array, log_id_t log_type_dcid_array_length )
{
	s_rest_log_mem_list = log_mem_list;
	s_rest_log_dci_list = log_dci_list;
	s_rest_log_type_dcid_array = log_type_dcid_array;

	BF_ASSERT( log_type_dcid_array_length == s_rest_total_logs );
}

/**
 * @brief Callback function to get log details.
 * @details This function sends the details of all the logs when a rs/log is requested.
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
uint32_t Log_Info_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
		 ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, 405U );
		return_val = 0U;
	}
	else
	{
		if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					Validate_Options_Cors_Headers( parser, rs, nullptr );
				}
				else if ( rs->auth_level >= LOG_READ_ACCESS_ROLE_LEVEL )
				{
					if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, 413U );
						return_val = 0U;
					}
					else
					{
						uint16_t tot_length = 0U;
						if ( DCI_ERR_NO_ERROR ==
							 get_dcid_ram_data( DCI_PRODUCT_NAME_DCID,
												&dest_data, &tot_length, 0U ) )
						{
							parser->response_code = 200U;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );
							Http_Server_Print_Dynamic( parser, XML_RS_LOG );
							const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;

							for ( uint8_t i = 0U; i < s_rest_total_logs; i++ )
							{
								pDCI = httpd_rest_dci::Find_Index_Rest_From_Dcid( s_rest_log_type_dcid_array[i] );
								if ( pDCI != nullptr )
								{
									Http_Server_Print_Dynamic( parser, XML_LOG_DETAILS_SEG1, i, pDCI->lenName,
															   pDCI->pName, pDCI->param_id, i );
								}
								else
								{
									BF_ASSERT( false );	///< DCID should have REST support
								}
							}
							Http_Server_Print_Dynamic( parser, XML_LOG_DETAILS_SEG5 );
							return_val = 1U;
						}
						else
						{
							hams_respond_error( parser, 500U );
							return_val = 0U;
						}
					}
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else
			{
				return_val = 1U;
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, LOG_OPTIONS,
										  STRLEN( LOG_OPTIONS ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else
				{
					if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
		else
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb );
		}
	}
	return ( return_val );
}

/**
 * @brief Callback function to get log details.
 * @details This function sends the details of all the logs when a rs/log/"anything" is requested.
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
uint32_t Parse_Log_Resource_Cb( hams_parser* parser, const uint8_t* data,
								uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
		{
			uint8_t log_id = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint8( ( data ), 1U,
													 length - 1U, &log_id ) )
			{
				if ( log_id < s_rest_total_logs )
				{
					if ( s_rest_log_mem_list[log_id] != nullptr )
					{
						rs->p.log_s.log_id = log_id;
					}
					else
					{
						hams_respond_error( parser, 404U );
					}
				}
				else
				{
					hams_respond_error( parser, 404U );
				}
			}
			else
			{
				hams_respond_error( parser, 400U );
			}

			if ( data[length] == 0x2FU )// '/'
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Log_Parse_Tail_Cb );
			}
			else
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Log_Id_Details_Cb );
			}
		}
		else
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb );
		}
	}
	else
	{
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb );
		}
	}
	return ( 0U );
}

/**
 * @brief Callback function to get log details.
 * @details This function sends the details of all the logs when a rs/log/{x} is requested, where x is log id
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
static uint32_t Log_Id_Details_Cb( hams_parser* parser, const uint8_t* data,
								   uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	/* Drop query variables. */
	if ( !hams_has_uri_part( data, length ) )
	{
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, 405U );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{

				if ( parser->content_length )
				{
					hams_respond_error( parser, 413U );
				}
				else
				{
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );
					}
					else
					{
						if ( ( rs->auth_level >= LOG_READ_ACCESS_ROLE_LEVEL ) )
						{
							DCI_ID_TD rest_dcid_count = 0U;
							uint8_t log_id = 0U;
							uint32_t max_entry_count = 0U;
							uint32_t current_entry_count = 0U;
							const DCI_ID_TD* log_dcid_array = nullptr;
							const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;

							log_id = rs->p.log_s.log_id;
							BF_Misc::Log_Mem* rest_log_mem_handle = s_rest_log_mem_list[log_id];
							BF_Misc::Log_DCI* rest_log_dci_handle = s_rest_log_dci_list[log_id];
							rest_dcid_count = rest_log_dci_handle->Get_DCID_Count();
							log_dcid_array = rest_log_dci_handle->Get_DCID_Array();

							parser->response_code = 200U;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );
							Http_Server_Print_Dynamic( parser, XML_LOG_X_DETAIL_PREFIX, log_id,
													   log_id );

							max_entry_count = rest_log_mem_handle->Get_Max_Entry_Count();
							Http_Server_Print_Dynamic( parser, XML_LOG_X_MAX_ENTRIES,
													   max_entry_count );

							current_entry_count = rest_log_mem_handle->Num_Entries_Queued();
							Http_Server_Print_Dynamic( parser, XML_LOG_X_CURRENT_ENTRIES,
													   current_entry_count );

							for ( uint16_t i = 0; i < rest_dcid_count; i++ )
							{
								pDCI = httpd_rest_dci::Find_Index_Rest_From_Dcid( log_dcid_array[i] );
								if ( pDCI != nullptr )
								{
									Http_Server_Print_Dynamic( parser,
															   "\t" XML_PARAM_ROOT_PREFIX1 "\bu",
															   pDCI->param_id );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
									Http_Server_Print_Dynamic( parser,
															   XML_PARAM_ALL_LINE_SEG2 );
									Http_Server_Print_Dynamic( parser, "\bs", pDCI->lenName, pDCI->pName );
#endif
									Http_Server_Print_Dynamic( parser,
															   XML_STRUCT_P_SEGMENT3 "\bu" "\"/>\n",
															   pDCI->param_id );
								}
								else
								{
									BF_ASSERT( false );	///< DCID should have REST support
								}
							}
							Http_Server_Print_Dynamic( parser, XML_LOG_X_DETAIL_SUFFIX );
						}
						else
						{
							hams_respond_error( parser, 401U );
						}
					}
				}
			}
			else
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					hams_response_header( parser, HTTP_HDR_ALLOW, LOG_OPTIONS,
										  STRLEN( LOG_OPTIONS ) );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
				else
				{
					if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
	}
	return ( 0U );
}

/**
 * @brief Callback function to get log details.
 * @details This function sends the details of all the logs when a rs/log/x/tail  is requested.
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
static uint32_t Log_Parse_Tail_Cb( hams_parser* parser, const uint8_t* data,
								   uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	rs->p.log_s.entry_id = 0U;
	rs->p.log_s.num_of_entries = 0U;
	rs->p.log_s.data_after_tail = false;
	rs->p.log_s.tail = false;

	if ( 0U != hams_has_uri_part( data, length ) )
	{
		if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
		{
			if ( ( length == 5U ) &&
				 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/tail", length ) ==
				   0 ) )
			{
				rs->p.log_s.tail = true;
				if ( HTTP_METHOD_DELETE != parser->method )
				{
					parser->user_data_cb = static_cast<hams_data_cb>( &Log_Data_From_Tail_Cb );
				}
				else
				{
					parser->user_data_cb = static_cast<hams_data_cb>( &Log_Tail_Delete_Cb );
				}
			}
			else
			{
				uint32_t log_entry_id = 0U;
				if ( ERR_PARSING_OK ==
					 INT_ASCII_Conversion::str_to_uint32( ( data ), 1U,
														  length - 1U, &log_entry_id ) )
				{
					rs->p.log_s.entry_id = log_entry_id;
					if ( HTTP_METHOD_DELETE != parser->method )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Log_Data_From_Tail_Cb );
					}
					else
					{
						hams_respond_error( parser, 405U );
					}
				}
				else
				{
					hams_respond_error( parser, 400U );
				}
			}
		}
		else
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Id_Details_Cb );
		}
	}
	else
	{
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb );
		}
	}
	return ( 0U );
}

/**
 * @brief Callback function to get log details.
 * @details This function is called when log data is required to be accessed or when we get log/x/tail/z or log/x/y/z
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
static uint32_t Log_Data_From_Tail_Cb( hams_parser* parser, const uint8_t* data,
									   uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );
	Log_Mem::entry_index_t max_entry_possible = 0U;

	/* Drop query variables. */
	if ( !hams_has_uri_part( data, length ) )
	{
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, 405U );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{
				if ( parser->content_length )
				{
					hams_respond_error( parser, 413U );
				}
				else
				{
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );

						max_entry_possible = s_rest_log_mem_list[rs->p.log_s.log_id]->Head_Index();
						if ( rs->p.log_s.entry_id > max_entry_possible )
						{
							hams_respond_error( parser, 404U );
						}
					}
					else
					{
						if ( ( rs->auth_level >= LOG_READ_ACCESS_ROLE_LEVEL ) )
						{
							Log_Entry_Details( parser, rs->p.log_s.log_id, rs->p.log_s.entry_id,
											   rs->p.log_s.num_of_entries, rs->p.log_s.data_after_tail );
						}
						else
						{
							hams_respond_error( parser, 401U );
						}
					}
				}
			}
			else
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					if ( ( true == rs->p.log_s.tail ) && ( 0U == rs->p.log_s.num_of_entries ) &&
						 ( 0U == rs->p.log_s.data_after_tail ) )
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, LOG_TAIL_OPTIONS,
											  STRLEN( LOG_TAIL_OPTIONS ) );
					}
					else
					{
						hams_response_header( parser, HTTP_HDR_ALLOW, LOG_OPTIONS,
											  STRLEN( LOG_OPTIONS ) );
					}
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
					rs->p.log_s.num_of_entries = 0U;
					rs->p.log_s.tail = false;
				}
				else
				{
					if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
	}
	else
	{
		if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
		{
			uint32_t log_entry_count = 0U;
			rs->p.log_s.data_after_tail = true;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint32( ( data ), 1U,
													  length - 1U, &log_entry_count ) )
			{
				rs->p.log_s.num_of_entries = log_entry_count;
				parser->user_data_cb = static_cast<hams_data_cb>( &Log_Data_From_Tail_Cb );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
		else
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Data_From_Tail_Cb );
		}
	}
	return ( 0U );
}

/**
 * @brief Callback function to get log details.
 * @details This function calls log mem and retrieves log data as required.
 * @param[in] parser: hams parser.
 * @param[in] log_id: log id of the log to be retreived.
 * @param[in] start_index: start index of log to be retrieved.
 * @param[in] entry_count: number of entries to retrieve.
 * @return None.
 */
static void Log_Entry_Details( hams_parser* parser, uint8_t log_id, uint32_t start_index,
							   uint32_t entry_count, bool data_after_tail )
{
	BF_Misc::Log_Mem::error_t return_status = BF_Misc::Log_Mem::NO_ERROR;
	BF_Misc::Log_Mem::entry_index_t next_index = 0U;
	BF_Misc::Log_Mem::entry_index_t remaining_entries = 0U;
	const DCI_ID_TD* log_dcid_array = nullptr;
	DCI_ID_TD rest_dcid_count = 0U;
	uint16_t total_char_count = 0U;
	uint16_t num_of_char = 0U;
	uint8_t datatype_size = 0U;
	uint32_t valid_start_entry = 0U;
	uint16_t log_buffer_size = 0U;
	BF_Misc::Log_Mem::entry_size_t blocks_required = MAX_BLOCKS_FOR_LOG_DATA;
	BF_Misc::Log_Mem::entry_size_t entry_size = 0U;
	BF_Misc::Log_Mem::entry_index_t i = 0U;

	entry_size = s_rest_log_mem_list[log_id]->Get_Entry_Size();
	if ( entry_count == 0U )
	{
		entry_count = ( MAX_BLOCKS_FOR_LOG_DATA * HTTP_POOL_BLOCK_SIZE ) / entry_size;
	}
	else
	{
		blocks_required = ( ( entry_count * entry_size ) + ( HTTP_POOL_BLOCK_SIZE - 1 ) ) /
			HTTP_POOL_BLOCK_SIZE;
		if ( blocks_required > MAX_BLOCKS_FOR_LOG_DATA )
		{
			blocks_required = MAX_BLOCKS_FOR_LOG_DATA;
		}
	}

	/* Allocate blocks for getting log data */
	uint8_t* log_data =
		reinterpret_cast<uint8_t*>( Http_Server_Alloc_Scratch( parser, blocks_required ) );

	if ( log_data != nullptr )
	{
		http_server_req_state* rs = Http_Server_Get_State( parser );
		log_buffer_size = ( static_cast<uint16_t>( HTTP_POOL_BLOCK_SIZE ) ) * ( blocks_required );

		return_status = s_rest_log_mem_list[log_id]->Get_Entry( start_index, &entry_count,
																log_data,
																&next_index, &remaining_entries,
																log_buffer_size );

		if ( HTTP_METHOD_OPTIONS == parser->method )
		{
			if ( ( remaining_entries == 0 ) && ( next_index == 0 ) )
			{
				parser->response_code = 404U;
			}
			else
			{
				parser->response_code = 200U;
			}
		}
		else
		{
			if ( BF_Misc::Log_Mem::NO_ERROR == return_status )
			{
				bool_t log_tail = rs->p.log_s.tail;
				BF_Misc::Log_Mem::entry_index_t num_of_entries = rs->p.log_s.num_of_entries;
				BF_Misc::Log_Mem::entry_index_t entry_id = rs->p.log_s.entry_id;

				parser->response_code = 200U;
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				Http_Server_Begin_Dynamic( parser );

				Http_Server_Print_Dynamic( parser, XML_LOG_X_GET_DETAIL_PREFIX, log_id, log_id );
				if ( log_tail == true )
				{
					Http_Server_Print_Dynamic( parser, XML_LOG_X_GET_DETAIL_TAIL );
				}
				else
				{
					Http_Server_Print_Dynamic( parser, "/\bu", entry_id );
				}
				if ( ( num_of_entries >= valid_start_entry ) && ( rs->p.log_s.data_after_tail != false ) )
				{
					Http_Server_Print_Dynamic( parser, "/\bu", num_of_entries );
					rs->p.log_s.data_after_tail = false;
				}

				Http_Server_Print_Dynamic( parser, XML_LOG_X_GET_DETAIL_END );

				rest_dcid_count = s_rest_log_dci_list[log_id]->Get_DCID_Count();
				log_dcid_array = s_rest_log_dci_list[log_id]->Get_DCID_Array();

				if ( 0U != entry_count )
				{
					BF_Misc::Log_Mem::entry_index_t end_index = 0U;
					if ( next_index == BF_Misc::Log_Mem::START_INDEX )	// Index rollover condition
					{
						if ( entry_id == 0U )
						{
							entry_id = s_rest_log_mem_list[log_id]->Tail_Index();
						}
						end_index = entry_id + entry_count;
					}
					else
					{
						entry_id = next_index - entry_count;
						end_index = next_index;
					}

					for ( i = entry_id; ( i < end_index ) &&
						  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
					{
						DCI_ID_TD j = 0U;
						Http_Server_Print_Dynamic( parser, XML_LOG_X_ENTRY_PREFIX, i );
						while ( j < rest_dcid_count )
						{
							DCI_DATATYPE_TD datatype;
							uint16_t tot_length = 0U;
							const DCI_DATA_BLOCK_TD* dci_block = DCI::Get_Data_Block(
								log_dcid_array[j] );
							datatype = dci_block->datatype;
							tot_length = *dci_block->length_ptr;
							datatype_size = DCI::Get_Datatype_Size( datatype );
							if ( ( tot_length > datatype_size ) && ( DCI_DTYPE_STRING8 != datatype ) )
							{
								const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI =
									httpd_rest_dci::Find_Index_Rest_From_Dcid( log_dcid_array[j] );
								BF_ASSERT( pDCI != nullptr );
								Http_Server_Print_Dynamic( parser, "\t\t<e>" );
								Print_Rest_Format( parser, pDCI, log_data, tot_length,
												   total_char_count );
								total_char_count = total_char_count + tot_length;
								Http_Server_Print_Dynamic( parser, "</e>\n" );
							}
							else if ( DCI_DTYPE_STRING8 == datatype )
							{
								uint16_t data_length = 0U;
								data_length = BF_Lib::Get_String_Length(
									reinterpret_cast<uint8_t*>(
										&log_data[total_char_count] ), tot_length );
								Http_Server_Print_Dynamic( parser, XML_LOG_X_ENTRY, data_length,
														   &log_data[total_char_count] );
								total_char_count += tot_length;
							}
							else
							{
								if ( ( datatype == DCI_DTYPE_BYTE ) || ( datatype == DCI_DTYPE_WORD ) ||
									 ( datatype == DCI_DTYPE_DWORD ) )
								{
									num_of_char = Convert_To_String( log_data, dest_data, datatype,
																	 total_char_count, FALSE );
								}
								else
								{
									num_of_char = Convert_To_String( log_data, dest_data, datatype,
																	 total_char_count, TRUE );
								}
								Http_Server_Print_Dynamic( parser, XML_LOG_X_ENTRY, num_of_char,
														   dest_data );
								total_char_count += datatype_size;
							}
							j++;
						}
						Http_Server_Print_Dynamic( parser, XML_LOG_X_ENTRY_SUFFIX );
					}
					Http_Server_Print_Dynamic( parser, XML_LOG_X_NEXT_EID, i );
					Http_Server_Print_Dynamic( parser, XML_LOG_X_REM_ENTRIES,
											   ( remaining_entries + next_index - i ) );
				}
				else
				{
					Http_Server_Print_Dynamic( parser, XML_LOG_X_NEXT_EID, next_index );
					Http_Server_Print_Dynamic( parser, XML_LOG_X_REM_ENTRIES, remaining_entries );
				}
				Http_Server_Print_Dynamic( parser, XML_LOG_X_GET_DETAIL_SUFFIX );
			}
			else
			{
				if ( BF_Misc::Log_Mem::INVALID_INDEX == return_status )
				{
					hams_respond_error( parser, 404U );
				}
				else
				{
					hams_respond_error( parser, 503U );
				}
			}
			Http_Server_Free( log_data );
		}
	}
	else
	{
		hams_respond_error( parser, E503_SCRATCH_SPACE );
	}
}

/**
 * @brief Callback function to get log details.
 * @details This is called for delete operation on rs/log/x/tail
 * @param[in] parser: hams parser
 * @param[in] data: data sent by the client.
 * @param[in] length: length of the data to be parsed.
 * @return None.
 */
static uint32_t Log_Tail_Delete_Cb( hams_parser* parser, const uint8_t* data,
									uint32_t length )
{
	http_server_req_state* rs = Http_Server_Get_State( parser );

	if ( !hams_has_uri_part( data, length ) )
	{
		if ( length != 1U )
		{
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				if ( rs->auth_level >= LOG_DELETE_ACCESS_ROLE_LEVEL )
				{
					http_server_req_state* rs = Http_Server_Get_State( parser );
					if ( true == rs->p.log_s.tail )
					{
						BF_Misc::Log_Mem::error_t return_status = BF_Misc::Log_Mem::NO_ERROR;
						BF_Misc::Log_Mem::entry_index_t deleted_entries = 0U;
						uint8_t log_id = rs->p.log_s.log_id;
						User_Audit_Log_Params_Update( parser );
						return_status = s_rest_log_mem_list[log_id]->Clear(
							BF_Misc::Log_Mem::CLEAR_ALL, &deleted_entries );
						if ( BF_Misc::Log_Mem::NO_ERROR == return_status )
						{
							parser->response_code = 200U;
						}
						else if ( BF_Misc::Log_Mem::BUSY_TIME == return_status )
						{
							parser->response_code = 202U;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );

							Http_Server_Print_Dynamic( parser, XML_LOG_X_DELETE_SUFFIX XML_LOG_X_DELETE_WAIT
													   XML_LOG_X_DELETE_PREFIX, log_id,
													   deleted_entries );
						}
						else
						{
							hams_respond_error( parser, 503U );
						}
					}
					else
					{
						hams_respond_error( parser, 405U );
					}
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else
			{
				if ( parser->response_code == 202U )
				{
					if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<void*>( nullptr ), 0U );
					}
					else
					{
						hams_respond_error( parser, 400U );
					}
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, "application/xml",
											  15U );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<void*>( nullptr ), 0U );
					}
					else
					{
						hams_respond_error( parser, 400U );
					}
				}
			}
		}
		else
		{
			hams_respond_error( parser, 400U );
		}
	}
	else
	{
		if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
		{
			uint32_t log_entry_count = 0U;
			if ( ERR_PARSING_OK ==
				 INT_ASCII_Conversion::str_to_uint32( ( data ), 1U,
													  length - 1U, &log_entry_count ) )
			{
				hams_respond_error( parser, 405U );	// todo when we have delete support for limited entries
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( 0U );
}