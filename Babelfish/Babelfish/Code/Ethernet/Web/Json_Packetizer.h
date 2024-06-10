/**
 *****************************************************************************************
 * @file Json_Packetizer.h
 * @details This file help to provide the functionality in JSON format for RESTful web service.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __JSON_PACKETIZER_H__
#define __JSON_PACKETIZER_H__

#include "Json_Config.h"
#include "Etn_Types.h"
#include <string.h>
#include <stdio.h>

#define JSON_MAXBUFF_LEN 1024
#define BUFFER_LEN 128
#define JSON_VALUE_LEN 256

typedef struct json_response
{
	uint16_t used_buff_len;
	char_t json_buff[JSON_MAXBUFF_LEN];
} json_response_t;

/**
 * @brief : Function to add key and its Value in string format as Json member.
 * @param[in] json_key  : JSON key
 * @param[in] json_val : JSON value
 * @param[in] include_trailing_comma : if true add comma
 * @param[out] buffer : Pointer to the destination buffer
 */
void Add_String_Member( json_response_t* buffer, const char_t* json_key, const char_t* json_val,
						bool include_trailing_comma );

/**
 * @brief : Function to add key and its Value in string format as Json member.
 * @param[in] json_key  : JSON key
 * @param[in] json_val : JSON value
 * @param[in] include_trailing_comma : if true add comma
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Add_Integer_Member( json_response_t* buffer, const char_t* json_key, const uint32_t json_val,
						 bool include_trailing_comma );

/**
 * @brief : Function to open Json Object
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Open_Json_Object( json_response_t* buffer );

/**
 * @brief : Function to close Json Object and if true add comma.
 * @param[in] include_trailing_comma : if true add comma
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Close_Json_Object( json_response_t* buffer, bool include_trailing_comma );

/**
 * @brief : Function to open Json Array
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Open_Json_Array( json_response_t* buffer );

/**
 * @brief : Function to close Json Array
 * @param[in] include_trailing_comma : if true add comma
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Close_Json_Array( json_response_t* buffer, bool include_trailing_comma );

/**
 * @brief : Function to add comma in JSON
 * @param[out] pBuffer : Pointer to the destination buffer
 */
void Add_Comma( json_response_t* buffer );

/**
 * @brief : Function to close multiple json object.
 * @param[in] count : number of object to be closed
 * @param[out] buffer : Pointer to the destination buffer
 */
void Multiple_Close_Object( json_response_t* buffer, uint16_t count );

#define HREF "href"

#define JSON_KEY_PID "pid"
#define JSON_KEY_AID "aid"
#define JSON_KEY_TEXT "text"

#ifdef JSON_ACTIVE
#define HTTP_CONTENT_TYPE HTTP_CONTENT_TYPE_JSON
#define HTTP_HDR_DATA_TYPE "application/json"
#define HTTP_HDR_DATA_LEN 16U
#else
#define HTTP_CONTENT_TYPE HTTP_CONTENT_TYPE_X_WWW_FORM_URLENCODED
#define HTTP_HDR_DATA_TYPE "application/xml"
#define HTTP_HDR_DATA_LEN 15U
#endif

/* HTTP error response code */
static const uint16_t ERROR_BAD_REQUEST = 400U;
static const uint16_t ERROR_UNAUTHORIZED = 401U;
static const uint16_t ERROR_FORBIDDEN = 403U;
static const uint16_t STATUS_NOT_FOUND = 404U;
static const uint16_t ERROR_METHOD_NOT_ALLOWED = 405U;
static const uint16_t ERROR_LENGTH_REQUIRED = 411U;
static const uint16_t ERROR_REQUEST_ENTITY_TOO_LARGE = 413U;
static const uint16_t ERROR_UNSUPPORTED_MEDIA_TYPE = 415U;
static const uint16_t ERROR_INTERNAL_SERVER = 500U;
static const uint16_t ERROR_NOT_IMPLEMENTED = 501U;
static const uint16_t RESPONSE_OK = 200U;
static const uint16_t RESPONSE_ACCEPTED = 202U;
static const uint16_t ERROR_SERVICE_UNAVAILABLE = 503U;

static const uint16_t E400_JSON_SYNTAX_ERROR = 0x4044;

static const uint16_t E503_JSON_PARSER = 0x5001;

#define JSON_ADD_COMMA  ","
#define OPEN_JSON_OBJECT "{"
#define CLOSE_JSON_OBJECT "}"
#define CLOSE_JSON_ARRAY "]"

#define JSON_ENUM_PREFIX \
	"{\n" \
	"\"Enum\": {\n" \
	"\"pid\": \"\",\n" \
	"\"href\": \"/rs/param//enum\",\n" \
	"\"Total\": \"\",\n" \
	"\"Val\": [\n"

#define JSON_ENUM_SUFFIX \
	"]\n" \
	"}\n" \
	"}\n"

#define JSON_ENUM_FIX_STRING \
	"{\n" \
	"\"href\": \"/rs/param//enum/\"\n" \
	"\"text\": \"\""  \
	"},\n"

#define JSON_ASSY_PARAMS_CONST_DATA \
	"{" \
	"\"Assy\": {" \
	"\"aid\": \"\"," \
	"\"name\": \"\"," \
	"\"list_ro\": \"\"," \
	"\"open_slots\": \"\"," \
	"\"r\": \"\"," \
	"\"w\": \"\"," \
	"\"Param\": ["

#define JSON_RS_PARAM_SUFFIX \
	"]" \
	"}" \
	"}"

#define JSON_RS_PARAM_FIX_STRING \
	"{" \
	"\"pid\":\"\"," \
	"\"href\":\"/rs/param/\"" \
	"},"

#define JSON_RS_PARAM_PREFIX \
	"{" \
	"\"ParamList\":{" \
	"\"href\":\"/rs/param\"" \
	"\"Param\":["

#define JSON_RS_PARAM_NAME \
	"\"name\":\"\","

#endif // __JSON_PACKETIZER_H__