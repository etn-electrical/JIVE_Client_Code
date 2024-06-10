/**
 **********************************************************************************************
 * @file            Prod_Spec_REST.cpp
 *
 * @brief           REST API handler
 * @details         This file shall include the REST handler and the common information required for
 * REST APIs
 *
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "REST_Common.h"
#include "Includes.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Validate_Options_Cors_Headers( hams_parser* parser, http_server_req_state* rs,
									hams_data_cb set_user_callback )
{
	if ( ( 0U == parser->cors_auth_common->origin_length ) ||
		 ( NULL == parser->cors_auth_common->origin_buffer ) )
	{
		hams_respond_error( parser, 400U );
	}
	else
	{
		parser->response_code = 200U;
		rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
		if ( set_user_callback != nullptr )
		{
			parser->user_data_cb = static_cast<hams_data_cb>( set_user_callback );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD get_dcid_ram_data( DCI_ID_TD dci_data_id, DCI_DATA_PASS_TD* data,
								DCI_LENGTH_TD* length, DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = REST_DCI_SOURCE_ID;
	access_struct.data_access.data = data;
	access_struct.data_access.length = *length;
	access_struct.data_access.offset = offset;
	access_struct.data_id = dci_data_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	return_status = httpd_dci_access->Data_Access( &access_struct );

	if ( *length == 0U )
	{
		*length = access_struct.data_access.length;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD set_dcid_ram_nv_data( DCI_ID_TD dci_data_id, DCI_DATA_PASS_TD* data,
								   DCI_LENGTH_TD* length, DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	const DCI_DATA_BLOCK_TD* data_block = DCI::Get_Data_Block( dci_data_id );

	if ( data_block->datatype == DCI_DTYPE_STRING8 )
	{
		if ( *length != 0U )
		{
			*( ( ( uint8_t* )data + ( *length ) ) ) = 0;
			( *length )++;
		}
	}

	access_struct.source_id = REST_DCI_SOURCE_ID;
	access_struct.data_access.data = data;
	access_struct.data_access.length = *length;
	access_struct.data_access.offset = offset;
	access_struct.data_id = dci_data_id;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;

	return_status = httpd_dci_access->Data_Access( &access_struct );

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		if ( *length == 0U )
		{
			*length = access_struct.data_access.length;
		}

		if ( Test_Bit( data_block->patron_attrib_info,
					   DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
		{
			access_struct.command = DCI_ACCESS_SET_INIT_CMD;

			return_status = httpd_dci_access->Data_Access(
				&access_struct );
		}
	}
	return ( return_status );
}