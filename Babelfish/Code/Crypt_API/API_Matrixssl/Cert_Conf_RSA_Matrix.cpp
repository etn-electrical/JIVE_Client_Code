/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Cert_Conf_RSA_Matrix.h"

#include "Includes.h"
#include "PKI_Common.h"
#include "StdLib_MV.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Conf_RSA_Matrix::Cert_Conf_RSA_Matrix( Cert_Conf_Lib::cert_config_data_t* cert_conf,
											const PKI_Common::pki_conf_t* pki_conf ) :
	m_cert_conf( cert_conf ),
	m_pki_conf( pki_conf ),
	m_pscert_conf( nullptr )
{
	if ( ( m_cert_conf == nullptr ) || ( m_cert_conf->common_name == nullptr ) ||
		 ( m_cert_conf->serial_num == nullptr ) )
	{
		/* Certificate configuration with mandatory fields must be filled, else stop. */
		BF_ASSERT( false );
	}
	if ( m_pki_conf == nullptr )
	{
		/* PKI configuration must be filled, else stop. */
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Cert_Conf_RSA_Matrix::Write_DN_Attribute( psPool_t* pool, psCertConfig_t* conf,
												  char_t* device_cn,
												  uint8_t device_cn_len )
{
	int32_t rc;

	rc = psX509SetDNAttribute( pool,
							   conf,
							   "country",
							   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( const_cast<
																						  char_t*>(
																						  "country" ) ),
														  PKI_Common::MAX_STRING_LENGTH ),
							   m_cert_conf->country,
							   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( m_cert_conf->
																					  country ),
														  PKI_Common::MAX_STRING_LENGTH ),
							   ASN_UTF8STRING );
	if ( rc == PS_SUCCESS )
	{
		rc = psX509SetDNAttribute( pool,
								   conf,
								   "stateOrProvince",
								   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( const_cast
																						  <char_t*>(
																							  "stateOrProvince" ) ),
															  PKI_Common::MAX_STRING_LENGTH ),
								   m_cert_conf->state_or_province,
								   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																  m_cert_conf->state_or_province ),
															  PKI_Common::MAX_STRING_LENGTH ),
								   ASN_UTF8STRING );
		if ( rc == PS_SUCCESS )
		{

			rc = psX509SetDNAttribute( pool,
									   conf,
									   "locality",
									   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																	  const_cast<char_t*>(
																		  "locality" ) ),
																  PKI_Common::MAX_STRING_LENGTH ),
									   m_cert_conf->locality,
									   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																	  m_cert_conf->locality ),
																  PKI_Common::MAX_STRING_LENGTH ),
									   ASN_UTF8STRING );
			if ( rc == PS_SUCCESS )
			{
				rc = psX509SetDNAttribute( pool,
										   conf,
										   "organizationalUnit",
										   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																		  const_cast<char_t*>(
																			  "organizationalUnit" ) ),
																	  PKI_Common::MAX_STRING_LENGTH ),
										   m_cert_conf->org_unit,
										   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																		  m_cert_conf->org_unit ),
																	  PKI_Common::MAX_STRING_LENGTH ),
										   ASN_UTF8STRING );
				if ( rc == PS_SUCCESS )
				{
					rc = psX509SetDNAttribute( pool,
											   conf,
											   "organization",
											   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																			  const_cast<char_t*>(
																				  "organization" ) ),
																		  PKI_Common::
																		  MAX_STRING_LENGTH ),
											   m_cert_conf->organization,
											   BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>(
																			  m_cert_conf->
																			  organization ),
																		  PKI_Common::
																		  MAX_STRING_LENGTH ),
											   ASN_UTF8STRING );
					if ( rc == PS_SUCCESS )
					{

						rc = psX509SetDNAttribute( pool,
												   conf,
												   "commonName",
												   BF_Lib::Get_String_Length( reinterpret_cast<
																				  uint8_t*>(
																				  const_cast<char_t*>(
																					  "commonName" ) ),
																			  PKI_Common::
																			  MAX_STRING_LENGTH ),
												   device_cn,
												   device_cn_len,
												   ASN_UTF8STRING );
						if ( rc == PS_SUCCESS )
						{
							rc = psX509SetDNAttribute( pool,
													   conf,
													   "email",
													   BF_Lib::Get_String_Length( reinterpret_cast<
																					  uint8_t*>(
																					  const_cast<
																						  char_t*>(
																						  "email" ) ),
																				  PKI_Common::
																				  MAX_STRING_LENGTH ),
													   m_cert_conf->email_addr,
													   BF_Lib::Get_String_Length( reinterpret_cast<
																					  uint8_t*>(
																					  m_cert_conf->
																					  email_addr ),
																				  PKI_Common::
																				  MAX_STRING_LENGTH ),
													   ASN_UTF8STRING );
						}
					}
				}
			}
		}
	}
	return ( rc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Conf_RSA_Matrix::Generate( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	psPool_t* pool = nullptr;

	int32_t ret = PS_FAILURE;

	m_pscert_conf = new ( psCertConfig_t );

	if ( m_cert_conf != nullptr )
	{
		BF_Lib::Copy_Val_To_String( reinterpret_cast<uint8_t*>( m_pscert_conf ), 0U,
									sizeof( psCertConfig_t ) );

		ret = psX509SetSerialNum( pool, m_pscert_conf, ( uint8_t* )m_cert_conf->serial_num,
								  BF_Lib::Get_String_Length( reinterpret_cast<uint8_t*>( const_cast<
																							 char_t*>(
																							 m_cert_conf
																							 ->
																							 serial_num ) ),
															 PKI_Common::MAX_STRING_LENGTH ) );
		if ( ret == PS_SUCCESS )
		{
			ret = psX509SetBasicConstraintsCA( pool, m_pscert_conf, m_pki_conf->bsc_constraint_ca );

			if ( ret == PS_SUCCESS )
			{
				ret = psX509SetBasicConstraintsPathLen( pool, m_pscert_conf,
														m_pki_conf->bsc_path_len );
				if ( ret == PS_SUCCESS )
				{
					ret = psX509SetValidDays( pool, m_pscert_conf, m_cert_conf->valid_days );

					if ( ret == PS_SUCCESS )
					{
						ret = psX509SetCertHashAlg( pool, m_pscert_conf, m_pki_conf->cert_alg );

						if ( ret == PS_SUCCESS )
						{
							if ( m_pki_conf->key_usage & PKI_Common::KEY_USG_CRL_SIGN )
							{
								ret = psX509AddKeyUsageBit( pool, m_pscert_conf, CRL_SIGN );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_KEY_CERT_SIGN ) )
							{
								ret = psX509AddKeyUsageBit( pool, m_pscert_conf, KEY_CERT_SIGN );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_KEY_AGREEMENT ) )
							{
								ret = psX509AddKeyUsageBit( pool, m_pscert_conf, KEY_AGREEMENT );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_DATA_ENCIPHERMENT ) )
							{
								ret =
									psX509AddKeyUsageBit( pool, m_pscert_conf, DATA_ENCIPHERMENT );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_KEY_ENCIPHERMENT ) )
							{
								ret = psX509AddKeyUsageBit( pool, m_pscert_conf, KEY_ENCIPHERMENT );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_NON_REPUDIATION ) )
							{
								ret = psX509AddKeyUsageBit( pool, m_pscert_conf, NON_REPUDIATION );
							}
							if ( ( ret == PS_SUCCESS ) &&
								 ( m_pki_conf->key_usage & PKI_Common::KEY_USG_DIGITAL_SIGNATURE ) )
							{
								ret =
									psX509AddKeyUsageBit( pool, m_pscert_conf, DIGITAL_SIGNATURE );
							}
							if ( ret == PS_SUCCESS )
							{
								if ( m_pki_conf->ext_key_usage &
									 PKI_Common::EXT_KEY_USG_TLS_SERVER_AUTH )
								{
									ret = psX509AddExtendedKeyUsage( pool, m_pscert_conf,
																	 SERVER_AUTH );
								}
								if ( ( ret == PS_SUCCESS ) &&
									 ( m_pki_conf->ext_key_usage &
									   PKI_Common::EXT_KEY_USG_TLS_CLIENT_AUTH ) )
								{
									ret = psX509AddExtendedKeyUsage( pool, m_pscert_conf,
																	 CLIENT_AUTH );
								}
								if ( ( ret == PS_SUCCESS ) &&
									 ( m_pki_conf->ext_key_usage &
									   PKI_Common::EXT_KEY_USG_CODE_SIGNING ) )
								{
									ret = psX509AddExtendedKeyUsage( pool, m_pscert_conf,
																	 CODE_SIGNING );
								}
								if ( ret == PS_SUCCESS )
								{
									if ( m_cert_conf->common_name != nullptr )
									{
										ret = Write_DN_Attribute( pool,
																  m_pscert_conf,
																  m_cert_conf->common_name,
																  BF_Lib::Get_String_Length(
																	  reinterpret_cast<uint8_t*>(
																		  m_cert_conf->common_name ),
																	  PKI_Common
																	  ::MAX_STRING_LENGTH ) );

										if ( ret == PS_SUCCESS )
										{
											if ( ( m_cert_conf->san_dns_name != nullptr ) ||
												 ( m_cert_conf->san_ip_address != nullptr ) )
											{
												subjectAltNameEntry_t sanEntry;

												BF_Lib::Copy_Val_To_String( reinterpret_cast<uint8_t
																							 *>( &
																								 sanEntry ), 0U,
																			sizeof(
																				subjectAltNameEntry_t ) );

												sanEntry.dNSName = m_cert_conf->san_dns_name;
												sanEntry.dNSNameLen = BF_Lib::Get_String_Length(
													reinterpret_cast<uint8_t*>( m_cert_conf->
																				san_dns_name ),
													PKI_Common::MAX_STRING_LENGTH ),
												sanEntry.iPAddress = m_cert_conf->san_ip_address;
												sanEntry.iPAddressLen = BF_Lib::Get_String_Length(
													reinterpret_cast<uint8_t*>( m_cert_conf->
																				san_ip_address ),
													PKI_Common::MAX_STRING_LENGTH ),

												ret = psX509AddSubjectAltName( pool, m_pscert_conf,
																			   &sanEntry );
											}
										}
										else
										{
											ret = PS_FAILURE;
											PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Write_DN_Attribute Failed" );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ( ret == PS_SUCCESS )
	{
		ret_status = PKI_Common::SUCCESS;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Conf_RSA_Matrix::Validate( void )
{
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Conf_RSA_Matrix::Get_Cert_Config( void** cert_conf )
{
	PKI_Common::pki_status_t ret_status;

	if ( m_pscert_conf == nullptr )
	{
		ret_status = Generate();

		if ( ret_status == PKI_Common::SUCCESS )
		{
			*cert_conf = m_pscert_conf;
		}
	}
	else
	{
		*cert_conf = m_pscert_conf;
		ret_status = PKI_Common::SUCCESS;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Conf_RSA_Matrix::~Cert_Conf_RSA_Matrix( void )
{
	if ( m_pscert_conf != nullptr )
	{
		psX509FreeCertConfig( m_pscert_conf );
		delete( m_pscert_conf );
		m_pscert_conf = nullptr;
	}
}
