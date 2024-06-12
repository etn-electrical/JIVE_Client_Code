/**
 *****************************************************************************************
 *	@file        REST_Log.h
 *
 *	@brief       LOG header file for the webservices. This file shall wrap the functions required
 *               for webservices using REST protocol to asscess LOG information.
 *
 *	@details     The LTK REST services are designed primarily for use from the adopting  product
 *	             web front end for browser based data access. However, they provide an open and
 *	             standard method for data access from any supporting REST client
 *
 *  @copyright   2018 Eaton Corporation. All Rights Reserved.
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed
 *               here in.  This technical information should not be reproduced or used
 *               without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */

#ifndef REST_LOG_H
	#define REST_LOG_H

#include <stdint.h>
#include "httpd_common.h"
#include "hams.h"
#include "Log_Mem.h"
#include "Log_DCI.h"

/**
 *  @brief	Callback to get log type info when rs/log uri is requested
 */
uint32_t Log_Info_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 *  @brief	Callback to get rest of the log uri paths data lik rs/log/0, rs/log/0/tail..
 */
uint32_t Parse_Log_Resource_Cb( hams_parser* parser, const uint8_t* data,
								uint32_t length );

/**
 *  @brief	Function to initialize REST log interface
 */
void Init_REST_Log_Interface( BF_Misc::Log_Mem** log_mem_list, BF_Misc::Log_DCI** log_dci_list,
							  DCI_ID_TD const* log_type_dcid_array, log_id_t log_type_dcid_array_length );

#endif	// REST_LOG_H
