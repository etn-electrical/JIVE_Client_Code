/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_RS_STRUCT_H__
#define __HTTPD_RS_STRUCT_H__

#include <stdint.h>

enum
{
	RS_STRUCT_ITEM_PARAM,
	RS_STRUCT_ITEM_STRUCT,
};

typedef struct RS_STRUCT_ITEM
{
	uint8_t type;
	uint16_t item_id;
} RS_STRUCT_ITEM_TD;

typedef struct RS_STRUCT
{
	uint16_t sid;
	const char* const pName;
	const char* const pDescription;
	const uint16_t lenName;
	const uint16_t lenDescription;
	uint16_t item_count;
	const RS_STRUCT_ITEM_TD* const pItems;
} RS_STRUCT_TD;

typedef struct RS_LIST_OF_STRUCT
{
	const char* const pName;
	uint16_t pNameLen;
	uint16_t numStructs;
	const RS_STRUCT_TD* const pStructs;
} RS_LIST_OF_STRUCT_TD;

extern const RS_LIST_OF_STRUCT_TD struct_lists[];

#endif	/* __HTTPD_RS_STRUCT_H__ */



