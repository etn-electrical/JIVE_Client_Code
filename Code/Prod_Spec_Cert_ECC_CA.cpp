/**
 **********************************************************************************************
 * @file            Prod_Spec_Cert_ECC_CA.cpp
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize and generate the CA signed device certificate here.
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
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
#include "Includes.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_Cert_ECC_CA.h"

#if defined ( WEB_SERVER_SUPPORT ) || ( PX_GREEN_DTLS_SUPPORT )
#include "Cert_Conf_ECC_Matrix.h"
#include "Key_ECC_Matrix.h"
#include "Cert_Signed_SS_Matrix.h"
#include "Cert_Signed_CA_Matrix.h"
#include "PKI_Common.h"
#include "Prod_Spec_MEM.h"
#include "DCI_Enums.h"
#include "Eth_if.h"
#include "Format_Handler.h"
#include "CRC16.h"
#include "Cert_Matrix.h"
#include "Csr_Matrix.h"
#include "Mem_Check.h"
#include "Ssl_Context.h"
#include "uC_Rand_Num.h"
#include "StdLib_MV.h"
#include "Services.h"
#include "Certs.h"
#include "Prod_Spec_Debug.h"
#include "BIN_ASCII_Conversion.h"
#include "Babelfish_Assert.h"

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT

/*
 *****************************************************************************************
 *		Constant
 *****************************************************************************************
 */

/* Device Certificate Configuration */
const PKI_Common::hash_alg_t ALG_SHA_256 = PKI_Common::SHA256;
const uint16_t KEY_USAGE = PKI_Common::KEY_USG_DIGITAL_SIGNATURE;
const uint8_t EXT_KEY_USAGE = PKI_Common::EXT_KEY_USG_TLS_SERVER_AUTH
	| PKI_Common::EXT_KEY_USG_TLS_CLIENT_AUTH;

const PKI_Common::pki_conf_t PKI_TLS_CONF =
{
	ALG_SHA_256,
	false,
	0U,
	KEY_USAGE,
	EXT_KEY_USAGE
};

/* CA Certificate Configuration */
const uint16_t CA_KEY_USAGE = PKI_Common::KEY_USG_KEY_CERT_SIGN
	| PKI_Common::KEY_USG_DIGITAL_SIGNATURE | PKI_Common::KEY_USG_CRL_SIGN;

const uint8_t CA_EXT_KEY_USAGE = 0;

const PKI_Common::pki_conf_t PKI_CA_CONF =
{
	ALG_SHA_256,
	true,
	0U,
	CA_KEY_USAGE,
	CA_EXT_KEY_USAGE
};

/* Constant to add random number in common name to make it unique on each generation, must be in multiple of 2 */
static const uint8_t COMMON_NAME_RANDOM_NUM_LEN = 8U;
#define CN_STR "CN"
static const uint8_t CN_STR_LEN = sizeof ( CN_STR );
static const uint8_t TWO_CHARS_PER_BYTE = 2U;
static const uint8_t CA_COMMON_NAME_LEN = ( sizeof ( CA_COMMON_NAME )
											+ ( COMMON_NAME_RANDOM_NUM_LEN +
												( COMMON_NAME_RANDOM_NUM_LEN / TWO_CHARS_PER_BYTE ) )
											+ CN_STR_LEN + 1U );

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
static DCI_Owner* cert_status_owner = nullptr;
static DCI_Owner* cert_control_owner = nullptr;

static bool cert_init_done = false;
Ssl_Context* ssl_context = nullptr;

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

bool Update_DCI_Status_Value( uint8_t check_in_value );

extern void Attach_TLS_Server_SSL_Context( Ssl_Context* ssl_context );

extern void Attach_Lwm2m_Dtls_SSL_Context( Ssl_Context* ssl_context );

uint32_t Get_Certificate_Generation_Epoch_Time( void );

static bool Get_DCI_Device_Cert_Control_Value( uint8_t* check_out_value );

static DCI_CB_RET_TD Process_Cert_Control_DCID_Cb( DCI_CBACK_PARAM_TD cback_param,
												   DCI_ACCESS_ST_TD* access_struct );


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_CERT_ECC_CA_Init( void )
{
	if ( cert_init_done == false )
	{
		ssl_context = new Ssl_Context();
		cert_init_done = true;

		/* Provide Ssl_Context handle to the tls_server. Provide Ssl_Context handle to
		 * all the modules which needs ssl_context. ssl_context can be different
		 * as well as same for multiple modules
		 */
		Attach_TLS_Server_SSL_Context( ssl_context );
#ifdef PX_GREEN_DTLS_SUPPORT
		Attach_Lwm2m_Dtls_SSL_Context( ssl_context );
#endif
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Certificate Generation Init in Progress" );
		cert_status_owner = new DCI_Owner( DCI_DEVICE_CERT_VALID_DCID );
		cert_control_owner = new DCI_Owner( DCI_DEVICE_CERT_CONTROL_DCID );

		cert_control_owner->Attach_Callback( &Process_Cert_Control_DCID_Cb,
											 NULL, DCI_ACCESS_POST_SET_RAM_CMD_MSK );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Prod_Spec_Certificate_Handler( Cert_Store* cert_store_if )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( cert_init_done == true )
	{
		bool cert_gen_error = true;
		uint8_t ip_address_str[IP_ADDR_STR_LEN] =
		{ 0U };
		uint8_t serial_number[SERIAL_NUM_LEN + 1] =
		{ 0U };
		const uint8_t DEFAULT_ZERO_REPLACEMENT = 0x7CU;
		uint8_t serial_num_buf[SERIAL_NUM_STR_MAX_LEN] =
		{ 0U };
		uC_ETHERNET_IP_CONFIG_ST active_ip_config;

		// serial number should not be less than half of common name number defined.
		BF_ASSERT( ( ( COMMON_NAME_RANDOM_NUM_LEN / 2 ) <= SERIAL_NUM_LEN ) );

		/*
		 *****************************************************************************************
		 * ************************ Step 1 : Check which fields are present in the certificate.
		 *****************************************************************************************
		 */
		bool ca_key_available = false;
		bool ca_cert_available = false;
		bool dev_key_available = false;

		if ( cert_store_if->Is_CA_Key_Available() == true )
		{
			ca_key_available = true;
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "CA Key Available" );
			if ( cert_store_if->Is_CA_Cert_Available() == true )
			{
				ca_cert_available = true;
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "CA Cert Available" );
				if ( cert_store_if->Is_Dev_Key_Available() == true )
				{
					dev_key_available = true;
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Device Key Available" );
				}
				else
				{
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Device Key Not Available" );
				}
			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "CA Cert Not Available" );
			}
		}
		else
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "CA Key Not Available" );
		}

		if ( ( ca_key_available == false ) || ( dev_key_available == false ) )
		{
			uint8_t ctrl_val = 0;
			bool status = Get_DCI_Device_Cert_Control_Value( &ctrl_val );

			if ( ( status == true ) && ( ctrl_val == DCI_DEVICE_CERT_CONTROL_ENUM::CERT_GENERATE ) )
			{
				cert_gen_error = false;
			}
		}
		else
		{
			cert_gen_error = false;
		}

		/*
		 *****************************************************************************************
		 * ************************ Step 2 : Create Certificate Configuration data.
		 *****************************************************************************************
		 */

		if ( cert_gen_error == false )
		{
			/* Certificate fields configuration */
			Cert_Conf_Lib::cert_config_data_t* cert_config_data = new Cert_Conf_Lib::cert_config_data_t;

			if ( cert_config_data != nullptr )
			{
				cert_config_data->generate_time_epoch = CERT_GEN_EPOCH_TIME;
				cert_config_data->valid_days = CA_CERT_VALID_DAYS;
				cert_config_data->country = const_cast<char_t*>( COUNTRY );
				cert_config_data->state_or_province = const_cast<char_t*>( STATE_OR_PROVINCE );
				cert_config_data->locality = const_cast<char_t*>( LOCALITY );
				cert_config_data->org_unit = const_cast<char_t*>( ORG_UNIT );
				cert_config_data->organization = const_cast<char_t*>( ORGANIZATION );
				cert_config_data->email_addr = const_cast<char_t*>( EMAIL_ADDRESS );

				Eth_if::Get_IP_Config( &active_ip_config );

				/* covert IP address u32 format into string format - a.b.c.d */
				FORMAT_HANDLER_IPV4_BIG_ENDIAN( &( active_ip_config.ip_address ),
												sizeof ( active_ip_config.ip_address ),
												ip_address_str, sizeof ( ip_address_str ),
												RAW_TO_STRING, nullptr );

				/* Generate serial number ( random ), part of this is also feed into common name */
				uC_Rand_Num::Get( serial_number, SERIAL_NUM_LEN );

				for ( uint8_t i = 0U; i < SERIAL_NUM_LEN; i++ )
				{
					if ( ( serial_number[0] & 0x80U ) == 0x80 )
					{
						/* certificate serial number must not be a negative number */
						serial_number[0] &= ~( 0x80 );
					}
					/* Random number generator may generate a value 0
					 * This loop is just to avoid those instances i.e. non-null, hack */
					if ( serial_number[i] == 0U )
					{
						serial_number[i] = DEFAULT_ZERO_REPLACEMENT;
					}
				}
				cert_config_data->serial_num = reinterpret_cast<char_t*>( serial_number );

				char_t common_name[CA_COMMON_NAME_LEN] =
				{ 0U };

				BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( common_name ),
									 reinterpret_cast<uint8_t const*>( CA_COMMON_NAME ),
									 sizeof ( CA_COMMON_NAME ) );

				uint8_t index = 0;
				uint8_t loop_count = 0;
				for ( uint8_t i = 0; i < ( COMMON_NAME_RANDOM_NUM_LEN / TWO_CHARS_PER_BYTE ); i++ )
				{
					index = ( sizeof ( CA_COMMON_NAME ) - 1 ) + ( i * TWO_CHARS_PER_BYTE )
						+ loop_count;

					BIN_ASCII_Conversion::byte_to_str_no_prefix( serial_number[i],
																 reinterpret_cast<uint8_t*>( &common_name[index] ) );

					/* Add hyphen( separator ) after COMMON NAME RANDOM NUMBER */
					common_name[index + TWO_CHARS_PER_BYTE] = '-';
					loop_count++;
				}

				BF_Lib::Copy_String(
					reinterpret_cast<uint8_t*>( &common_name[index + TWO_CHARS_PER_BYTE + 1] ),
					reinterpret_cast<uint8_t const*>( CN_STR ), CN_STR_LEN );

				cert_config_data->common_name = ( common_name );

				/*
				 *****************************************************************************************
				 * ************************ Step 3 : Generate CA certificate
				 *****************************************************************************************
				 */

				Key_Lib* ca_ecc_key = nullptr;
				Cert_Conf_Lib* ca_ecc_cert_conf = nullptr;
				Cert_Signed* ca_cert_sign_handle = nullptr;

				uint8_t* cert_store_ca_key_ptr = nullptr;
				uint16_t cert_store_ca_key_len = nullptr;
				uint8_t* ca_key_mem = nullptr;
				uint16_t ca_key_mem_len = 0U;
				uint8_t* ca_cert_mem = nullptr;
				uint32_t ca_cert_mem_len = 0U;
				PKI_Common::pki_status_t error_val;

				if ( ca_key_available == true )
				{
					error_val = cert_store_if->Get_CA_Key( &cert_store_ca_key_ptr, &cert_store_ca_key_len );

					if ( error_val == PKI_Common::SUCCESS )
					{
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Cert Store - CA Key Read Successful" );
						ca_ecc_key = new Key_ECC_Matrix( cert_store_ca_key_ptr, cert_store_ca_key_len );

						if ( ca_cert_available == true )
						{
							ca_key_mem = cert_store_ca_key_ptr;
							ca_key_mem_len = cert_store_ca_key_len;

							if ( cert_store_if->Get_CA_Cert( &ca_cert_mem, &ca_cert_mem_len ) !=
								 PKI_Common::SUCCESS )
							{
								cert_gen_error = true;
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA Cert Read Failure" );
							}
							else
							{
								FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( &( serial_number ), ( SERIAL_NUM_LEN ),
																	 serial_num_buf, SERIAL_NUM_STR_MAX_LEN,
																	 RAW_TO_STRING, nullptr );
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Cert Store - CA Cert Read Successful" );
							}
						}
						else
						{
							ca_ecc_cert_conf = new Cert_Conf_ECC_Matrix( cert_config_data, &PKI_CA_CONF );

							ca_cert_sign_handle = new Cert_Signed_SS_Matrix( ca_ecc_key, ca_ecc_cert_conf );

							error_val = ca_cert_sign_handle->Sign( &ca_key_mem, &ca_key_mem_len,
																   &ca_cert_mem,
																   &ca_cert_mem_len );
							if ( error_val != PKI_Common::SUCCESS )
							{
								cert_gen_error = true;
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "CA Certificate Generation Failed" );
							}
							else
							{
								FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( &( serial_number ), ( SERIAL_NUM_LEN ),
																	 serial_num_buf, SERIAL_NUM_STR_MAX_LEN,
																	 RAW_TO_STRING, nullptr );
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "CA Certificate Generation Successful" );
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Common Name: %s and Serial Number: %s",
													   cert_config_data->common_name, serial_num_buf );
							}
						}
					}
					else
					{
						cert_gen_error = true;
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA Key Read Failure" );
					}
				}
				else
				{
					ca_ecc_key = new Key_ECC_Matrix( PKI_Common::KEY_ECC_256,
													 PKI_Common::CURVE_SECP256R1 );

					ca_ecc_cert_conf = new Cert_Conf_ECC_Matrix( cert_config_data, &PKI_CA_CONF );

					ca_cert_sign_handle = new Cert_Signed_SS_Matrix( ca_ecc_key, ca_ecc_cert_conf );

					error_val = ca_cert_sign_handle->Sign( &ca_key_mem, &ca_key_mem_len, &ca_cert_mem,
														   &ca_cert_mem_len );
					if ( error_val != PKI_Common::SUCCESS )
					{
						cert_gen_error = true;
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "CA Key and Certificate Generation Failed" );
					}
					else
					{
						FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( &( serial_number ), ( SERIAL_NUM_LEN ),
															 serial_num_buf, SERIAL_NUM_STR_MAX_LEN,
															 RAW_TO_STRING, nullptr );
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "CA Key and Certificate Generation Successful" );
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Common Name: %s and Serial Number: %s",
											   cert_config_data->common_name, serial_num_buf );
					}
				}

				/*
				 *****************************************************************************************
				 * ************************ Step 4 : Generate module specific certificate.
				 *****************************************************************************************
				 */

				PKI_Common::pki_status_t cert_parse_status = PKI_Common::FAILURE;
				Key_Lib* dev_ecc_key = nullptr;
				Cert_Conf_Lib* dev_ecc_cert_conf = nullptr;
				Cert_Signed* dev_cert_sign_handle = nullptr;
				Cert_Lib* ca_cert = nullptr;
				Csr_Lib* dev_csr = nullptr;

				uint8_t* dev_key_mem = nullptr;
				uint16_t dev_key_mem_len = 0U;
				uint8_t* dev_cert_mem = nullptr;
				uint32_t dev_cert_mem_len = 0U;
				uint8_t* cert_store_dev_key_ptr = nullptr;
				uint16_t cert_store_dev_key_len = nullptr;

				/* if CA cert is available or generated successfully then, generate device certificate */
				if ( cert_gen_error == false )
				{
					/* Every certificate should be unique and
					 * adopter/product team should find a way to make it so */
					uC_Rand_Num::Get( serial_number, SERIAL_NUM_LEN );

					for ( uint8_t i = 0U; i < SERIAL_NUM_LEN; i++ )
					{
						if ( ( serial_number[0] & 0x80U ) == 0x80 )
						{
							serial_number[0] &= ~( 0x80 );
						}
						/* Random number generator may generate a value 0
						 * This loop is just to avoid those instances i.e. non-null, hack */
						if ( serial_number[i] == 0U )
						{
							serial_number[i] = DEFAULT_ZERO_REPLACEMENT;
						}
					}

					serial_number[SERIAL_NUM_LEN] = 0U;
					cert_config_data->common_name = reinterpret_cast<char_t*>( ip_address_str );
					cert_config_data->serial_num = reinterpret_cast<char_t*>( serial_number );
					cert_config_data->san_ip_address = reinterpret_cast<char_t*>( ip_address_str );
					cert_config_data->valid_days = SSL_CERT_VALID_DAYS;
					cert_config_data->san_dns_name = reinterpret_cast<char_t*>( ip_address_str );

					if ( dev_key_available == false )
					{
						dev_ecc_key = new Key_ECC_Matrix( PKI_Common::KEY_ECC_256,
														  PKI_Common::CURVE_SECP256R1 );
					}
					else
					{
						error_val = cert_store_if->Get_Dev_Key( &cert_store_dev_key_ptr,
																&cert_store_dev_key_len );

						if ( error_val != PKI_Common::SUCCESS )
						{
							cert_gen_error = true;
							PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - Device Key Read Failure" );
						}
						else
						{
							PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Cert Store - Device Key Read Successful" );
							dev_ecc_key = new Key_ECC_Matrix( cert_store_dev_key_ptr,
															  cert_store_dev_key_len );
						}
					}

					if ( cert_gen_error == false )
					{
						dev_ecc_cert_conf = new Cert_Conf_ECC_Matrix( cert_config_data, &PKI_TLS_CONF );

						dev_csr = new Csr_Matrix( dev_ecc_key, dev_ecc_cert_conf );

						ca_cert = new Cert_Matrix( ca_cert_mem, ca_cert_mem_len, &cert_parse_status );
						if ( cert_parse_status == PKI_Common::SUCCESS )
						{
							PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "CA Certificate Parsing Successful" );
							dev_cert_sign_handle = new Cert_Signed_CA_Matrix( dev_csr, ca_ecc_key,
																			  ca_cert );

							error_val = dev_cert_sign_handle->Sign( &dev_key_mem, &dev_key_mem_len,
																	&dev_cert_mem,
																	&dev_cert_mem_len );
							if ( error_val != PKI_Common::SUCCESS )
							{
								cert_gen_error = true;
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Device Certificate Signing Failure" );
							}
							else
							{
								FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( &( serial_number ), ( SERIAL_NUM_LEN ),
																	 serial_num_buf, SERIAL_NUM_STR_MAX_LEN,
																	 RAW_TO_STRING, nullptr );
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Device Certificate Signing Successful" );
								PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Common Name: %s and Serial Number: %s",
													   cert_config_data->common_name, serial_num_buf );
							}
						}
						else
						{
							PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "CA Certificate Parsing Failure" );
						}
					}
				}
				/*
				 *****************************************************************************************
				 * ************************ Step 5 : Store generated certificates ( CA and device cert )
				 *                                 in cert storage area
				 *****************************************************************************************
				 */
				if ( cert_gen_error == false )
				{
					if ( ( ca_cert_available == false ) || ( dev_key_available == false ) )
					{
						cert_gen_error = true;
						/* update keys and certificate in local buffer */
						if ( cert_store_if->Update_CA_Key_Buf( ca_key_mem,
															   ca_key_mem_len ) == PKI_Common::SUCCESS )
						{
							if ( cert_store_if->Update_CA_Cert_Buf( ca_cert_mem,
																	ca_cert_mem_len ) ==
								 PKI_Common::SUCCESS )
							{
								if ( cert_store_if->Update_Dev_Key_Buf( dev_key_mem,
																		dev_key_mem_len ) ==
									 PKI_Common::SUCCESS )
								{
									cert_gen_error = false;
									/* Write updated local buffer back to cert store */
									if ( cert_store_if->Cert_Store_Write() == PKI_Common::SUCCESS )
									{
										PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Certificates stored in Cert Store" );
									}
									else
									{
										PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR,
															   "Failure in Storing Certificates in Cert Store" );
									}
								}
							}
						}
					}
				}

				/*
				 *****************************************************************************************
				 * ************************ Step 6 : Initialize SSL
				 *****************************************************************************************
				 */
				if ( ca_ecc_key != nullptr )
				{
					ca_ecc_key->~Key_Lib();
					delete ca_ecc_key;
					ca_ecc_key = nullptr;
				}
				/* Clear the structures and variables from heap memory */
				if ( cert_config_data != nullptr )
				{
					delete cert_config_data;
					cert_config_data = nullptr;
				}

				/* Call the destructor */
				if ( ca_cert_sign_handle != nullptr )
				{
					ca_cert_sign_handle->~Cert_Signed();
					delete ca_cert_sign_handle;
					ca_cert_sign_handle = nullptr;
				}

				bool load_key_state = false;

				if ( cert_gen_error == false )
				{
					load_key_state = ssl_context->Load_Keys( KEY_ECC_LOAD, dev_cert_mem, dev_cert_mem_len,
															 dev_key_mem, dev_key_mem_len,
															 dev_cert_mem,
															 dev_cert_mem_len );
				}

				if ( dev_cert_mem != nullptr )
				{
					psFree( dev_cert_mem, nullptr );
					dev_cert_mem = nullptr;
				}

				if ( ca_cert_mem != nullptr )
				{
					if ( ca_cert_available == false )
					{
						psFree( ca_cert_mem, nullptr );
					}
					else
					{
						cert_store_if->Free_CA_Cert();
					}
					ca_cert_mem = nullptr;
				}

				if ( dev_cert_sign_handle != nullptr )
				{
					dev_cert_sign_handle->~Cert_Signed();
					delete dev_cert_sign_handle;
					dev_cert_sign_handle = nullptr;
				}

				if ( cert_store_ca_key_ptr != nullptr )
				{
					cert_store_if->Free_CA_Key();
					cert_store_ca_key_ptr = nullptr;
				}

				if ( cert_store_dev_key_ptr != nullptr )
				{
					cert_store_if->Free_Dev_Key();
					cert_store_dev_key_ptr = nullptr;
				}

				if ( load_key_state == true )
				{

					Attach_Cert_Store_Interface( cert_store_if );
					Update_DCI_Status_Value( DCI_DEVICE_CERT_VALID_ENUM::CERT_VALID );
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "SSL_Context Loading Successful" );

					/* Signal other threads that certificate generation is successful */
					ssl_context->Signal();
					ret = PKI_Common::SUCCESS;
				}
				else
				{
					Update_DCI_Status_Value( DCI_DEVICE_CERT_VALID_ENUM::CERT_INVALID );
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "SSL_Context Loading Failed" );
					ret = PKI_Common::FAILURE;
				}
			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "cert_config_data Memory Allocation failed" );
			}
		}
	}
	if ( ret == PKI_Common::SUCCESS )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_INFO, "Certificate Generation Completed" );
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Update_DCI_Status_Value( uint8_t check_in_value )
{
	bool return_val = false;

	if ( cert_status_owner != nullptr )
	{
		cert_status_owner->Check_In_Init( &check_in_value );
		cert_status_owner->Check_In( check_in_value );
		return_val = true;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * Gets the value for DCI_DEVICE_CERT_CONTROL_DCID
 * Note: this function may be optimized into inline code where it is invoked.
 *****************************************************************************************
 */
static bool Get_DCI_Device_Cert_Control_Value( uint8_t* check_out_value )
{
	bool return_val = false;
	uint8_t val = 0;

	if ( cert_control_owner != nullptr )
	{
		cert_control_owner->Check_Out( val );
		*check_out_value = val;
		return_val = true;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * Callback for handling commands to update the Device Certificate
 *****************************************************************************************
 */
static DCI_CB_RET_TD Process_Cert_Control_DCID_Cb( DCI_CBACK_PARAM_TD cback_param,
												   DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	uint8_t cert_control_val = 0;

	/* check if command is valid */
	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{
		cert_control_owner->Check_Out( cert_control_val );

		if ( cert_control_val == DCI_DEVICE_CERT_CONTROL_ENUM::CERT_GENERATE )
		{
			/*Delay added for the 200 OK response*/
			uC_Delay( SOFT_RESET_RESPONSE_TIME_uS );
			BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET, nullptr );
		}
	}
	return ( return_status );
}

#endif	// DISABLE_PKI_CERT_GEN_SUPPORT

#endif	// ( WEB_SERVER_SUPPORT ) || (  PX_GREEN_DTLS_SUPPORT )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Get_Certificate_Generation_Epoch_Time( void )
{
	return ( CERT_GEN_EPOCH_TIME );
}
