/**
 *****************************************************************************************
 *	@file        REST header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for webservices using REST protocol.
 *
 *	@details     The LTK REST services are designed primarily for use from the adopting  product web front end for
 * browser based data access. However, they provide an open and standard method for data access from any supporting REST
 * client
 *

 *      @copyright   2022 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed here in.  This technical
 * information should not be reproduced or used without direct written permission from Eaton Corporation.
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */

#ifndef __REST_JSON_H__
#define __REST_JSON_H__

#include "Json_Packetizer.h"

/**
 * @brief Callback for /rs request
 * @details This function is called on /rs request request, For "GET /rs" request it returns all available REST paths in
 * response
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
uint32_t S_Rest_Root_Hdrs_End_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/** External user defined functions required for REST services to function.
 * @brief       This function is used to write the JSON object name and REST parameter id.
 * @param[in]   parser : Pointer to parser structure
 * @param       pDCI : Pointer to DCID lookup structure consisting of REST parameter
 * @param       param : Parameter
 * @return bool Returns either true or false
 */
extern bool Get_Json_Param_Single_Item( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI, uint32_t param,
										uint32_t verbose );

/**
 * @brief Callback for /rs/param request
 * @details This function is called on /rs/param request request, For "PUT /rs/param" request it set the present value
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
uint32_t S_Rest_Param_Value_Put_Parse_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/** External user defined functions required for REST services to function.
 * @brief     This function is used to write the JSON object name and REST parameter id.
 * @param[in] parser   Pointer to parser structure pDCI
 * @param[in] Pointer to DCID lookup structure consisting of REST parameter ids.
 * @param[in] param    Parameter
 * @return bool  Returns either true or false
 */
extern bool Get_Json_Param_Single_Item( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI, uint32_t param,
										uint32_t verbose );

int32_t Set_Param_Numeric_Json( ReusedRecvBuf* pTarget,
								const struct yjson_put_json_state* ypx,
								uint32_t datatype );

#endif // __REST_JSON_H__
