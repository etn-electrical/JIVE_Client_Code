/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTP_DIAGNOSTIC_JSON_H__
#define __HTTP_DIAGNOSTIC_JSON_H__

#include "hams.h"

/**
 * @brief:  diag call back
 * @param[in] parser : Pointer to the parser.
 * @param[out] void
 */
uint32_t s_diag_cb( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief: Diag Write callback
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 */
static uint32_t S_Diag_Write_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief: product specific Ethernet/EMAC initialization and address conflict detection machine
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 */
extern uint32_t S_Xmit_Dynamic_Data( hams_parser* parser, const uint8_t* data,
									 uint32_t length );

/**
 * @brief: LWIP Diagnostic callback
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 */
uint32_t S_Lwip_Diag_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif
