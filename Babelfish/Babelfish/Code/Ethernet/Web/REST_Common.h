/**
 *****************************************************************************************
 * @file
 *
 * @brief
 * @details : This file contains declaration for common REST functions
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef REST_COMMON_H
#define REST_COMMON_H

#include "httpd_xml.h"
#include "server.h"

/**
 * @brief This function validates the CORS(Cross-Origin Resource Sharing) origin field (buffer and
 * length)
 * @param[in] parser : Pointer to the parser.
 * @param[in] rs : Pointer to server request state
 * @param[in] set_user_callback : user callback
 * @return None.
 */
void Validate_Options_Cors_Headers( hams_parser* parser, http_server_req_state* rs,
									hams_data_cb set_user_callback );

/**
 * @brief Access the data from database for given dcid
 * @param[in] dci_data_id : DCI ID
 * @param[out] data : Pointer to the destination buffer
 * @param[out] length : Byte length
 * @param[in] offset : Byte offset
 * @return None.
 */
DCI_ERROR_TD get_dcid_ram_data( DCI_ID_TD dci_data_id, DCI_DATA_PASS_TD* data,
								DCI_LENGTH_TD* length, DCI_LENGTH_TD offset );

/**
 * @brief Update the database for given dcid
 * @param[in] dci_data_id : DCI ID
 * @param[in] data : Pointer to the source buffer
 * @param[in] length : Byte length
 * @param[in] offset : Byte offset
 * @return None.
 */
DCI_ERROR_TD set_dcid_ram_nv_data( DCI_ID_TD dci_data_id, DCI_DATA_PASS_TD* data,
								   DCI_LENGTH_TD* length, DCI_LENGTH_TD offset );

#endif	// REST_COMMON_H