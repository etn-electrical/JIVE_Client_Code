/**
 *****************************************************************************************
 *	@file DSI_Defines.h
 *
 *	@brief Defination of DSI module
 *
 *	@details
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DSI_DEFINES_H
#define DSI_DEFINES_H

#include "Includes.h"
// DSI = Data Stream Interface

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef uint16_t dsi_id_t;
typedef uint16_t dsi_item_size_t;
typedef uint16_t dsi_item_count_t;
typedef uint8_t* dsi_data_t;

/// Provides a success or failure response for a pub.
typedef uint8_t dsi_pub_resp_t;
static const dsi_pub_resp_t NO_DSI_PUB_ERROR = 0U;
static const dsi_pub_resp_t DSI_PUB_FAILURE = 1U;

/// Provides a success or failure response for a subscriber.
typedef uint8_t dsi_sub_resp_t;
static const dsi_sub_resp_t NO_DSI_SUB_ERROR = 0U;
static const dsi_sub_resp_t DSI_SUB_FAILURE = 1U;

/// Define a dsid which is indicating a non-DSID.
static const dsi_id_t DSI_UNDEFINED_ID = static_cast<dsi_id_t>( ~0 );

/*
 *********************************************************
 *		Access Struct
 *********************************************************
 */

#endif	// DSI_DEFINES_H
