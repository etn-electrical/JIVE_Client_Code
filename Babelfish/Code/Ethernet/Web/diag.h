/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTP_DIAGNOSTIC_H__
#define __HTTP_DIAGNOSTIC_H__

#include "hams.h"

uint32_t s_diag_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

extern uint32_t S_Xmit_Dynamic_Data( hams_parser* parser, const uint8_t* data,
									 uint32_t length );

#endif
