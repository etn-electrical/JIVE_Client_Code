/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "httpd_common.h"
#include "httpd_rs_struct.h"

static const char list_struct_name_0[] = "Eaton Device A";

static const char struct_0_name[] = "Struct0";
static const char struct_1_name[] = "Struct1";
static const char struct_2_name[] = "Struct2";
static const char struct_0_disp_name[] = "Basic Structure";
static const char struct_1_disp_name[] = "Complex Structure";
static const char struct_2_disp_name[] = "More Complex Structure";

static const RS_STRUCT_ITEM_TD struct_0_list[] =
{
	{ RS_STRUCT_ITEM_PARAM, 0 },
	{ RS_STRUCT_ITEM_PARAM, 1 },
	{ RS_STRUCT_ITEM_PARAM, 2 },
};

static const RS_STRUCT_ITEM_TD struct_1_list[] =
{
	{ RS_STRUCT_ITEM_PARAM, 3 },
	{ RS_STRUCT_ITEM_PARAM, 4 },
	{ RS_STRUCT_ITEM_PARAM, 5 },
	{ RS_STRUCT_ITEM_STRUCT, 0 },
};

static const RS_STRUCT_ITEM_TD struct_2_list[] =
{
	{ RS_STRUCT_ITEM_PARAM, 6 },
	{ RS_STRUCT_ITEM_PARAM, 7 },
	{ RS_STRUCT_ITEM_STRUCT, 0 },
	{ RS_STRUCT_ITEM_STRUCT, 1 },
};

static const RS_STRUCT_TD struct_set_0[] =
{

	{
		0,
		struct_0_name,
		struct_0_disp_name,
		STRLEN( struct_0_name ),
		STRLEN( struct_0_disp_name ),
		sizeof ( struct_0_list ) / sizeof( RS_STRUCT_ITEM_TD ),
		struct_0_list
	},
	{
		1,
		struct_1_name,
		struct_1_disp_name,
		STRLEN( struct_1_name ),
		STRLEN( struct_1_disp_name ),
		sizeof ( struct_1_list ) / sizeof( RS_STRUCT_ITEM_TD ),
		struct_1_list
	},
	{
		2,
		struct_2_name,
		struct_2_disp_name,
		STRLEN( struct_2_name ),
		STRLEN( struct_2_disp_name ),
		sizeof ( struct_2_list ) / sizeof( RS_STRUCT_ITEM_TD ),
		struct_2_list
	},
};

const RS_LIST_OF_STRUCT_TD struct_lists[] =
{
	{
		list_struct_name_0,
		STRLEN( list_struct_name_0 ),
		sizeof ( struct_set_0 ) / sizeof( RS_STRUCT_TD ),
		struct_set_0,
	},
};
