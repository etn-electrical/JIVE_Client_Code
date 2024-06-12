/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "httpd_rest_dci.h"
#include "httpd_xml.h"
#include "httpd_common.h"
#include "REST_DCI_Data.h"
#include "REST.h"
#include "StdLib_MV.h"

#include "server.h"
#include "yxml.h"
#include "Certs.h"
#include "PKI_Common.h"
#include "REST_Common.h"

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define Param_RO   "GET, HEAD, OPTIONS"

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
Cert_Store* m_cert_store_handle = nullptr;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
uint32_t Get_Cert_Store_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Ca_Cert_Options_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );

static uint32_t Get_Ca_Cert_Cb( hams_parser* parser, const uint8_t* data, uint32_t length );


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Attach_Cert_Store_Interface( Cert_Store* handle )
{
	m_cert_store_handle = handle;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Get_Cert_Store_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	if ( ( false == ENDS_WITH_SLASH( data ) ) && ( false == IS_EXTRA_SLASH( data ) ) )
	{

#if 0	// TODO: Support cert store GET method
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, 405U );
			return_val = 0U;
		}
		else
#endif
		{
			/* Look for 'values'*/
			if ( ( length == 7U ) &&
				 ( strncmp( reinterpret_cast<const char_t*>( &data[0] ), "/cacert", length ) == 0 ) )
			{
				parser->user_data_cb = static_cast<hams_data_cb>( &Get_Ca_Cert_Cb );
			}
			else
			{
				hams_respond_error( parser, 400U );
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Get_Ca_Cert_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{
	uint8_t return_val = 0U;

	/*
	 * In case data is nullptr there is no need to check for slash.
	 * If we go for check then system crashes due to nullptr
	 */
	if ( ( nullptr == data ) || ( ( false == ENDS_WITH_SLASH( data ) ) &&
								  ( false == IS_EXTRA_SLASH( data ) ) ) )
	{
		if ( ( HTTP_METHOD_GET != parser->method ) && ( HTTP_METHOD_HEAD != parser->method ) &&
			 ( HTTP_METHOD_OPTIONS != parser->method ) )
		{
			hams_respond_error( parser, 405U );
			return_val = 0U;
		}
		else
		{
			http_server_req_state* rs = Http_Server_Get_State( parser );
			if ( 0U != hams_check_headers_ended( data, length ) )
			{
				if ( HTTP_METHOD_OPTIONS == parser->method )
				{
					Validate_Options_Cors_Headers( parser, rs, Ca_Cert_Options_Cb );
				}
				else if ( rs->max_auth_exempt_level != 0U )
				{
					if ( 0U != parser->content_length )
					{
						hams_respond_error( parser, 413U );
						return_val = 0U;
					}
					else
					{
						if ( m_cert_store_handle != nullptr )
						{
							uint8_t* ca_cert;
							uint32_t ca_cert_len;

							PKI_Common::pki_status_t status =
								m_cert_store_handle->Get_CA_Cert( &ca_cert, &ca_cert_len );

							if ( status == PKI_Common::SUCCESS )
							{
								if ( ca_cert != nullptr )
								{
									parser->response_code = 200U;
									rs->flags |= REQ_FLAG_NO_DYN_PARSER_SPACE;
									Http_Server_Begin_Dynamic( parser );

									Http_Server_Print_Dynamic( parser, "\bs", ca_cert_len,
															   ca_cert );
									m_cert_store_handle->Free_CA_Cert();
									return_val = 1U;
								}
								else
								{
									hams_respond_error( parser, 404U );
								}
							}
							else
							{
								hams_respond_error( parser, 404U );
							}
						}
						else
						{
							hams_respond_error( parser, 404U );
						}
					}
				}
				else
				{
					hams_respond_error( parser, 401U );
				}
			}
			else
			{
				return_val = 1U;

				if ( 0U != Http_Server_End_Dynamic( parser ) )
				{
					hams_response_header( parser, HTTP_HDR_CONTENT_TYPE,
										  "application/x-x509-ca-cert", 26U );
					hams_response_header( parser, COUNT_HTTP_HDRS,
										  reinterpret_cast<const void*>( NULL ), 0U );
				}
			}
		}
	}
	return ( return_val );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static uint32_t Ca_Cert_Options_Cb( hams_parser* parser, const uint8_t* data, uint32_t length )
{

	hams_response_header( parser, HTTP_HDR_ALLOW, Param_RO, STRLEN( Param_RO ) );
	hams_response_header( parser, COUNT_HTTP_HDRS,
						  reinterpret_cast<const void*>( NULL ), 0U );

	return ( 0U );
}

#endif	// DISABLE_PKI_CERT_GEN_SUPPORT