/**
 *****************************************************************************************
 * @file Assy_Json.h

 * @brief This file shall wrap the functions required for webservices using REST protocol.
 *
 * @details The LTK REST services are designed primarily for use from the adopting  product web front end for browser
 * based data access. However, they provide an open and standard method for data access from any supporting REST client
 *
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef __ASSY_JSON_H__
#define __ASSY_JSON_H__

#include "Json_Packetizer.h"

/**
 * @brief Callback for /rs/assy request
 * @details This function is called on /rs/assy request, For "GET /rs/assy" request it should return all assembly list.
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
uint32_t Assy_Get_All_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

uint32_t Assy_Index_Callback_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif