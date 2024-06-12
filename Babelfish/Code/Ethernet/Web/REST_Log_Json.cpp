/**
 *****************************************************************************************
 * @file REST_Log_Json.cpp
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Json_Packetizer.h"
#include "iot_json_parser.h"
#include "Includes.h"
#include "DCI_Defines.h"
#include "httpd_xml.h"
#include "REST.h"
#include "REST_Json.h"
#include "REST_Log_Json.h"
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

extern uint16_t Print_Rest_Format_Json( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
										uint8_t* data, uint32_t tot_length, uint32_t offset );

extern uint8_t j_dest_data[];

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
void Init_REST_Log_Interface_Json( BF_Misc::Log_Mem** log_mem_list, BF_Misc::Log_DCI** log_dci_list,
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
uint32_t Log_Info_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
		 ( HTTP_METHOD_OPTIONS != parser->method ) )
	{
		hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
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
						hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
						return_val = 0U;
					}
					else
					{
						uint16_t tot_length = 0U;
						if ( DCI_ERR_NO_ERROR ==
							 get_dcid_ram_data( DCI_PRODUCT_NAME_DCID,
												&j_dest_data, &tot_length, 0U ) )
						{
							parser->response_code = RESPONSE_OK;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );
							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0U;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
								char_t formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0U, BUFFER_LEN );
								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "LogList", NULL, false );
								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, HREF, "/rs/log", true );
								Add_String_Member( json_buffer, "Log", nullptr, false );
								Open_Json_Array( json_buffer );

								const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI = nullptr;

								for ( uint8_t i = 0U; i < s_rest_total_logs; i++ )
								{
									pDCI = httpd_rest_dci::Find_Index_Rest_From_Dcid( s_rest_log_type_dcid_array[i] );
									if ( pDCI != nullptr )
									{
										Open_Json_Object( json_buffer );
										Add_Integer_Member( json_buffer, "lid", i, true );
										Add_String_Member( json_buffer, "name", pDCI->pName, true );
										Add_Integer_Member( json_buffer, "pid", pDCI->param_id, true );
										snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d", i );
										Add_String_Member( json_buffer, HREF, formatted_buff, false );
										if ( i < ( s_rest_total_logs - 1U ) )
										{
											Close_Json_Object( json_buffer, true );
										}
										else
										{
											Close_Json_Object( json_buffer, false );
										}
										Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
										json_buffer->used_buff_len = 0U;
										memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
									}
									else
									{
										BF_ASSERT( false );										///< DCID should have
																								///< REST support
									}
								}
								Close_Json_Array( json_buffer, false );
								Multiple_Close_Object( json_buffer, 2U );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
							return_val = 1U;
						}
						else
						{
							hams_respond_error( parser, ERROR_INTERNAL_SERVER );
							return_val = 0U;
						}
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
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
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<const void*>( NULL ), 0U );
					}
				}
			}
		}
		else
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb_Json );
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
uint32_t Parse_Log_Resource_Cb_Json( hams_parser* parser, const uint8_t* data,
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
						hams_respond_error( parser, STATUS_NOT_FOUND );
					}
				}
				else
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
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
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb_Json );
		}
	}
	else
	{
		if ( data[length] != 0x2FU )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb_Json );
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
			hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{

				if ( parser->content_length )
				{
					hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
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

							parser->response_code = RESPONSE_OK;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );
							if ( json_buffer != nullptr )
							{
								memset( json_buffer, 0U, JSON_MAXBUFF_LEN );
								char_t formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0U, BUFFER_LEN );
								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "LogId", NULL, false );
								Open_Json_Object( json_buffer );
								Add_Integer_Member( json_buffer, "lid", log_id, true );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d", log_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, true );

								max_entry_count = rest_log_mem_handle->Get_Max_Entry_Count();
								Add_Integer_Member( json_buffer, "MaxEntries", max_entry_count, true );

								current_entry_count = rest_log_mem_handle->Num_Entries_Queued();
								Add_Integer_Member( json_buffer, "CurrentEntries", current_entry_count, true );

								Add_String_Member( json_buffer, "Param", nullptr, false );
								Open_Json_Array( json_buffer );

								for ( uint16_t i = 0; i < rest_dcid_count; i++ )
								{
									pDCI = httpd_rest_dci::Find_Index_Rest_From_Dcid( log_dcid_array[i] );
									if ( pDCI != nullptr )
									{
										Open_Json_Object( json_buffer );
										Add_Integer_Member( json_buffer, "pid", pDCI->param_id, true );
#ifndef REMOVE_REST_PARAM_NAME_TEXT
										Add_String_Member( json_buffer, "name", pDCI->pName, true );
#endif
										snprintf( formatted_buff, BUFFER_LEN, "/rs/param/%d", pDCI->param_id );
										Add_String_Member( json_buffer, HREF, formatted_buff, false );
										if ( i < ( rest_dcid_count - 1U ) )
										{
											Close_Json_Object( json_buffer, true );
										}
										else
										{
											Close_Json_Object( json_buffer, false );
										}
										Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
										json_buffer->used_buff_len = 0U;
										memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
									}
									else
									{
										BF_ASSERT( false );										///< DCID should have
																								///< REST support
									}
								}
								Close_Json_Array( json_buffer, false );
								Multiple_Close_Object( json_buffer, 2 );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
						else
						{
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
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
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
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
						hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_BAD_REQUEST );
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
			parser->user_data_cb = static_cast<hams_data_cb>( &Log_Info_Cb_Json );
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
			hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
		}
		else
		{
			if ( hams_check_headers_ended( data, length ) )
			{
				if ( parser->content_length )
				{
					hams_respond_error( parser, ERROR_REQUEST_ENTITY_TOO_LARGE );
				}
				else
				{
					if ( HTTP_METHOD_OPTIONS == parser->method )
					{
						Validate_Options_Cors_Headers( parser, rs, nullptr );

						max_entry_possible = s_rest_log_mem_list[rs->p.log_s.log_id]->Head_Index();
						if ( rs->p.log_s.entry_id > max_entry_possible )
						{
							hams_respond_error( parser, STATUS_NOT_FOUND );
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
							hams_respond_error( parser, ERROR_UNAUTHORIZED );
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
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
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
				hams_respond_error( parser, ERROR_BAD_REQUEST );
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
				parser->response_code = STATUS_NOT_FOUND;
			}
			else
			{
				parser->response_code = RESPONSE_OK;
			}
		}
		else
		{
			if ( BF_Misc::Log_Mem::NO_ERROR == return_status )
			{
				bool_t log_tail = rs->p.log_s.tail;
				BF_Misc::Log_Mem::entry_index_t num_of_entries = rs->p.log_s.num_of_entries;
				BF_Misc::Log_Mem::entry_index_t entry_id = rs->p.log_s.entry_id;

				parser->response_code = RESPONSE_OK;
				rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
				Http_Server_Begin_Dynamic( parser );

				json_response_t* json_buffer =
					reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );
				if ( json_buffer != nullptr )
				{
					json_buffer->used_buff_len = 0U;
					memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
					char_t formatted_buff[BUFFER_LEN];
					memset( formatted_buff, 0U, BUFFER_LEN );
					Open_Json_Object( json_buffer );
					Add_String_Member( json_buffer, "LogTail", nullptr, false );
					Open_Json_Object( json_buffer );
					Add_Integer_Member( json_buffer, "lid", log_id, true );

					if ( log_tail == true )
					{
						if ( ( num_of_entries >= valid_start_entry ) && ( rs->p.log_s.data_after_tail != false ) )
						{
							snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d/tail/%d", log_id, num_of_entries );
							Add_String_Member( json_buffer, HREF, formatted_buff, true );
							rs->p.log_s.data_after_tail = false;
						}
						else
						{
							snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d/tail", log_id );
							Add_String_Member( json_buffer, HREF, formatted_buff, true );
						}
					}
					else
					{
						if ( ( num_of_entries >= valid_start_entry ) && ( rs->p.log_s.data_after_tail != false ) )
						{
							snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d/%d/%d", log_id, entry_id,
									  num_of_entries );
							Add_String_Member( json_buffer, HREF, formatted_buff, true );
							rs->p.log_s.data_after_tail = false;
						}
						else
						{
							snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d/%d", log_id, entry_id );
							Add_String_Member( json_buffer, HREF, formatted_buff, true );
						}
					}

					Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
					json_buffer->used_buff_len = 0U;
					memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

					rest_dcid_count = s_rest_log_dci_list[log_id]->Get_DCID_Count();
					log_dcid_array = s_rest_log_dci_list[log_id]->Get_DCID_Array();

					if ( 0U != entry_count )
					{
						BF_Misc::Log_Mem::entry_index_t end_index = 0U;
						if ( next_index == BF_Misc::Log_Mem::START_INDEX )						// Index rollover
																								// condition
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

						Add_String_Member( json_buffer, "Entry", NULL, false );
						Open_Json_Array( json_buffer );

						for ( i = entry_id; ( i < end_index ) &&
							  ( !( Check_Block_Overflow_And_Datablock_Limit( parser ) ) ); i++ )
						{
							DCI_ID_TD j = 0U;
							Open_Json_Object( json_buffer );
							Add_Integer_Member( json_buffer, "eid", i, true );
							Add_String_Member( json_buffer, "e", NULL, false );
							Open_Json_Array( json_buffer );
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

							json_buffer->used_buff_len = 0U;
							memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );

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

									Http_Server_Print_Dynamic( parser, "\"" );
									Print_Rest_Format_Json( parser, pDCI, log_data, tot_length,
															total_char_count );
									total_char_count = total_char_count + tot_length;
									Http_Server_Print_Dynamic( parser, "\"" );
								}
								else if ( DCI_DTYPE_STRING8 == datatype )
								{
									uint16_t data_length = 0U;
									data_length = BF_Lib::Get_String_Length(
										reinterpret_cast<uint8_t*>(
											&log_data[total_char_count] ), tot_length );
									Http_Server_Print_Dynamic( parser, "\"\bs\"", data_length,
															   &log_data[total_char_count] );
									total_char_count += tot_length;
								}
								else
								{
									if ( ( datatype == DCI_DTYPE_BYTE ) || ( datatype == DCI_DTYPE_WORD ) ||
										 ( datatype == DCI_DTYPE_DWORD ) )
									{
										num_of_char = Convert_To_String( log_data, j_dest_data, datatype,
																		 total_char_count, FALSE );
									}
									else
									{
										num_of_char = Convert_To_String( log_data, j_dest_data, datatype,
																		 total_char_count, TRUE );
									}
									Http_Server_Print_Dynamic( parser, "\"\bs\"", num_of_char,
															   j_dest_data );
									total_char_count += datatype_size;
								}
								j++;
								if ( j < ( rest_dcid_count - 1U ) )
								{
									Http_Server_Print_Dynamic( parser, JSON_ADD_COMMA );
								}
							}
							Close_Json_Array( json_buffer, false );
							if ( i < ( end_index - 1 ) )
							{
								Close_Json_Object( json_buffer, true );
							}
							else
							{
								Close_Json_Object( json_buffer, false );
							}
							Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

							json_buffer->used_buff_len = 0U;
							memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
						}
						Close_Json_Array( json_buffer, true );
						Add_Integer_Member( json_buffer, "NextEID", i, true );
						Add_Integer_Member( json_buffer, "RemEntries", ( remaining_entries + next_index - i ), false );
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
					}
					else
					{
						Add_Integer_Member( json_buffer, "NextEID", next_index, true );
						Add_Integer_Member( json_buffer, "RemEntries", remaining_entries, false );
						Http_Server_Print_Dynamic( parser, json_buffer->json_buff );

						json_buffer->used_buff_len = 0U;
						memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
					}
					Multiple_Close_Object( json_buffer, 2U );
					Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
					Ram::De_Allocate( json_buffer );
				}
			}
			else
			{
				if ( BF_Misc::Log_Mem::INVALID_INDEX == return_status )
				{
					hams_respond_error( parser, STATUS_NOT_FOUND );
				}
				else
				{
					hams_respond_error( parser, ERROR_SERVICE_UNAVAILABLE );
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
						// To do - will be updated when log gets updated
						// User_Audit_Log_Params_Update_Json( parser );
						User_Audit_Log_Params_Update( parser );
						return_status = s_rest_log_mem_list[log_id]->Clear(
							BF_Misc::Log_Mem::CLEAR_ALL, &deleted_entries );
						if ( BF_Misc::Log_Mem::NO_ERROR == return_status )
						{
							parser->response_code = RESPONSE_OK;
						}
						else if ( BF_Misc::Log_Mem::BUSY_TIME == return_status )
						{
							parser->response_code = RESPONSE_ACCEPTED;
							rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
							Http_Server_Begin_Dynamic( parser );

							json_response_t* json_buffer =
								reinterpret_cast<json_response_t*>( Ram::Allocate( sizeof( json_response_t ) ) );
							if ( json_buffer != nullptr )
							{
								json_buffer->used_buff_len = 0U;
								memset( json_buffer->json_buff, 0U, JSON_MAXBUFF_LEN );
								char_t formatted_buff[BUFFER_LEN];
								memset( formatted_buff, 0U, BUFFER_LEN );
								Open_Json_Object( json_buffer );
								Add_String_Member( json_buffer, "DeleteLog", nullptr, false );
								Open_Json_Object( json_buffer );
								snprintf( formatted_buff, BUFFER_LEN, "/rs/log/%d/tail", log_id );
								Add_String_Member( json_buffer, HREF, formatted_buff, true );
								Add_Integer_Member( json_buffer, "OpTime", deleted_entries, false );
								Multiple_Close_Object( json_buffer, 2U );
								Http_Server_Print_Dynamic( parser, json_buffer->json_buff );
								Ram::De_Allocate( json_buffer );
							}
						}
						else
						{
							hams_respond_error( parser, ERROR_SERVICE_UNAVAILABLE );
						}
					}
					else
					{
						hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );
					}
				}
				else
				{
					hams_respond_error( parser, ERROR_UNAUTHORIZED );
				}
			}
			else
			{
				if ( parser->response_code == RESPONSE_ACCEPTED )
				{
					if ( 0U != Http_Server_End_Dynamic( parser ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<void*>( nullptr ), 0U );
					}
					else
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
				else
				{
					if ( 0U != hams_can_respond( data, length ) )
					{
						hams_response_header( parser, HTTP_HDR_CONTENT_TYPE, HTTP_HDR_DATA_TYPE,
											  HTTP_HDR_DATA_LEN );
						hams_response_header( parser, COUNT_HTTP_HDRS,
											  reinterpret_cast<void*>( nullptr ), 0U );
					}
					else
					{
						hams_respond_error( parser, ERROR_BAD_REQUEST );
					}
				}
			}
		}
		else
		{
			hams_respond_error( parser, ERROR_BAD_REQUEST );
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
				hams_respond_error( parser, ERROR_METHOD_NOT_ALLOWED );	// todo when we have delete support for limited
																		// entries
			}
			else
			{
				hams_respond_error( parser, ERROR_BAD_REQUEST );
			}
		}
	}
	return ( 0U );
}
