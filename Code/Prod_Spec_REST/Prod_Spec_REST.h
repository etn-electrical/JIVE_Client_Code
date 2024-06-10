/**
 *****************************************************************************************
 * @file	: Prod_Spec_REST.h
 *
 * @brief
 * @details : This file contains declaration and Macros related to REST APIs
 *
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_REST_H
#define PROD_SPEC_REST_H

#include "REST.h"

/**
 * @brief REST initialization
 * @details Attach REST API handler as a callback for HTTP request
 * @return None.
 */
void PROD_SPEC_REST_Init( void );

/**
 * @brief REST API hadler for prod spec
 * @details This is used to add new REST API and handle the same on HTTP request
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
uint32_t Prod_Spec_Rest_Handler( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif	// PROD_SPEC_REST_H
