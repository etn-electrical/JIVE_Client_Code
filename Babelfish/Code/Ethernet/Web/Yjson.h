/**
 *****************************************************************************************
 * @file Yjson.h
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef YJSON_H
#define YJSON_H

#include <stdint.h>
#include <stddef.h>
#include "Etn_Types.h"
#include "Json_Packetizer.h"

#define PUT_SINGLE_YJSON_STACK_SIZE 32

static const uint8_t YJSON_PUT_IS_HEX = 0x01;
static const uint8_t YJSON_PUT_IS_NEGATIVE = 0x02;
static const uint8_t YJSON_PUT_HAS_LEADING_ZERO = 0x04;
static const uint8_t YJSON_PUT_IS_BASE64 = 0x08;
static const uint8_t YJSON_PUT_BASE64_PAD = 0x10;
static const uint8_t YJSON_PUT_END = 0x20;

typedef struct yjson_t_s
{

	/* DDB HACK: Make binary compatible with dynamic_buffer so server_request_idx appears at 3rd byte. */

	uint16_t nextstate;		/* Used for '@' state remembering and for the "string" consuming state */

	/* Added by DDB. Used for identifying block containing server request.*/
	uint8_t server_request_idx;

	uint8_t ignore;

	/* PRIVATE */
	int32_t state;
	uint16_t stacksize;
} yjson_t;

struct yjson_put_json_state
{
	yjson_t json;
	uint8_t stack[PUT_SINGLE_YJSON_STACK_SIZE];
	uint64_t tmp_number;
	union
	{
		char_t buff[4U];
		uint32_t all;
	} base64;
	uint8_t flags;
	uint8_t put_state;
	uint8_t val_index;
	uint8_t separator;
	uint8_t base64_bits;
	uint8_t num_of_units;
	uint8_t max_num_list;
	uint8_t curr_num_list;
};

void yjson_init( yjson_t*, size_t );

#endif