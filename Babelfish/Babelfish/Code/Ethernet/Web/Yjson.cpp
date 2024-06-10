/**
 *****************************************************************************************
 * @file Yjson.cpp
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Yjson.h"
#include <string.h>

static const uint32_t YJSON_init = 30;

void yjson_init( yjson_t* x, size_t stacksize )
{
	memset( x, 0, sizeof ( *x ) );
	x->stacksize = ( uint16_t )( stacksize );
	x->state = YJSON_init;
}