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
#include "Prod_Spec_REST.h"
#include "Includes.h"
#include "StdLib_MV.h"
#include "server.h"
#include "Language_Config.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_REST_Init( void )
{
	/* Attach a callback to handle REST APIs which web server does not support */
	Attach_Prod_Spec_Rest_Callback( Prod_Spec_Rest_Handler );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Prod_Spec_Rest_Handler( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( IS_EXTRA_SLASH( data ) == false )
	{
		/* Loop to find the Prod spec Rest Path match */
		bool choice_found = false;
		uint32_t choice = PROD_SPEC_COUNT_REST;
		for ( uint32_t i = 0U; ( ( i < PROD_SPEC_COUNT_REST ) && ( choice_found == false ) ); ++i )
		{
			uint32_t len = BF_Lib::Get_String_Length(
				const_cast<uint8_t*>( prod_spec_rest_paths[i] ), REST_PATH_MAX_LEN );
			if ( ( length == len ) &&
				 ( BF_Lib::Compare_Data( data, prod_spec_rest_paths[i], length ) == true ) )
			{
				choice = i;
				choice_found = true;
			}
		}

		switch ( choice )
		{
			case PROD_SPEC_REST_ROOT:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( ( HTTP_METHOD_GET != parser->method ) &&
						 ( HTTP_METHOD_HEAD != parser->method ) )
					{
						hams_respond_error( parser, 405U );
					}
					else
					{
						parser->response_code = 200U;
						parser->user_data_cb = static_cast<hams_data_cb>
							( &s_rest_root_hdrs_end );
					}
				}
				break;

#ifdef REST_I18N_LANGUAGE
			case PROD_SPEC_REST_LANG:
				if ( Firmware_Upgrade_Inprogress() == true )
				{
					hams_respond_error( parser, E422_FIRMWARE_UPGRADE_SESSION_IN_PROGRESS );
				}
				else
				{
					if ( '/' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Rest_Lang_Cb );
					}
					else if ( ' ' == data[length] )
					{
						parser->user_data_cb = static_cast<hams_data_cb>( &Rest_Lang_All_Cb );
					}
					else
					{
						hams_respond_error( parser, 400U );
					}
				}
				break;

#endif	// REST_I18N_LANGUAGE
			default:
				hams_respond_error( parser, 400U );
				break;
		}
	}
	return ( return_val );
}
