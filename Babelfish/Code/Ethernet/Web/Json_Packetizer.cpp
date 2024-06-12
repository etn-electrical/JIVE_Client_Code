/**
 *****************************************************************************************
 * @file Json_Packetizer.cpp
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Json_Packetizer.h"
#include "Babelfish_Assert.h"

#define FIELD_LENGTH 100
#define MAX_INT_LEN 10

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Add_String_Member( json_response_t* buffer, const char_t* json_key, const char_t* json_val,
						bool include_trailing_comma )
{
	if ( ( buffer != nullptr ) || ( json_key != nullptr ) )
	{
		uint16_t val_size = 0U;
		uint16_t key_size = strlen( json_key );

		if ( json_val != nullptr )
		{
			val_size = strlen( json_val );
		}

		buffer->used_buff_len += key_size + val_size + 6U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "\"", 1 );
		strncat( buffer->json_buff, json_key, key_size );
		strncat( buffer->json_buff, "\":", 2 );

		if ( json_val != nullptr )
		{
			strncat( buffer->json_buff, "\"", 1 );
			strncat( buffer->json_buff, json_val, val_size );
			strncat( buffer->json_buff, "\"", 1 );
		}

		if ( include_trailing_comma )
		{
			strncat( buffer->json_buff, ",", 1 );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Add_Integer_Member( json_response_t* buffer, const char_t* json_key, const uint32_t json_val,
						 bool include_trailing_comma )
{
	if ( ( buffer != nullptr ) || ( json_key != nullptr ) )
	{
		uint16_t val_size = 0U;
		uint16_t key_size = strlen( json_key );

		char_t format_buff[MAX_INT_LEN];

		snprintf( format_buff, MAX_INT_LEN, "%d", json_val );

		val_size = strlen( format_buff );

		buffer->used_buff_len += key_size + val_size + 6U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "\"", 1U );
		strncat( buffer->json_buff, json_key, key_size );
		strncat( buffer->json_buff, "\":\"", 3U );
		strncat( buffer->json_buff, format_buff, val_size );
		strncat( buffer->json_buff, "\"", 1U );

		if ( include_trailing_comma )
		{
			strncat( buffer->json_buff, ",", 1U );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Open_Json_Object( json_response_t* buffer )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += 1U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "{", 1 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Close_Json_Object( json_response_t* buffer, bool include_trailing_comma )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += 2U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "}", 1 );

		if ( include_trailing_comma )
		{
			strncat( buffer->json_buff, ",", 1 );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Open_Json_Array( json_response_t* buffer )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += 1U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "[", 1 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Close_Json_Array( json_response_t* buffer, bool include_trailing_comma )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += 2U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, "]", 1 );

		if ( include_trailing_comma )
		{
			strncat( buffer->json_buff, ",", 1 );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Add_Comma( json_response_t* buffer )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += 1U;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		strncat( buffer->json_buff, ",", 1 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Multiple_Close_Object( json_response_t* buffer, uint16_t count )
{
	if ( buffer != nullptr )
	{
		buffer->used_buff_len += count;

		BF_ASSERT( buffer->used_buff_len <= JSON_MAXBUFF_LEN );

		for (uint16_t i = 0; i < count; i++)
		{
			strncat( buffer->json_buff, "}", 1 );
		}
	}
}