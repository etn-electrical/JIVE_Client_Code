/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "Cert_Signed_CA_Matrix.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Signed_CA_Matrix::Cert_Signed_CA_Matrix( Csr_Lib* csr_handle, Key_Lib* ca_key_handle,
											  Cert_Lib* ca_cert_handle ) :
	m_ca_key_handle( ca_key_handle ),
	m_ca_cert_handle( ca_cert_handle ),
	m_csr_handle( csr_handle )
{
	BF_ASSERT( m_csr_handle != nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Signed_CA_Matrix::Sign( uint8_t** key_mem, uint16_t* key_mem_len,
													  uint8_t** cert_mem, uint32_t* cert_mem_len )
{
	PKI_Common::pki_status_t ret_status;
	psPool_t* pool = nullptr;
	int32_t ret;
	psPubKey_t* pub_key = nullptr;
	psCertConfig_t* cert_config = nullptr;
	psX509Cert_t* ca_cert = nullptr;
	psPubKey_t* ca_priv_key = nullptr;

	uint8_t* ca_key_mem = nullptr;
	uint16_t ca_key_mem_len = 0U;

	uint8_t* ssl_cert_mem = nullptr;
	int32_t ssl_cert_mem_len = 0U;

	x509DNattributes_t* dn = nullptr;
	x509v3extensions_t* reqExt = nullptr;
	Cert_Conf_Lib* cert_conf_handle = nullptr;
	psPubKey_t* priv_key = nullptr;

	Key_Lib* end_entity_key_handle = nullptr;

	ret_status =
		m_csr_handle->Get( reinterpret_cast<void**>( &dn ), reinterpret_cast<void**>( &pub_key ),
						   reinterpret_cast<void**>( &reqExt ) );

	if ( ret_status == PKI_Common::SUCCESS )
	{
		ret_status = m_csr_handle->Get_Cert_Conf_Handle( &cert_conf_handle );

		if ( ret_status == PKI_Common::SUCCESS )
		{
			ret_status = cert_conf_handle->Get_Cert_Config(
				reinterpret_cast<void**>( &cert_config ) );

			if ( ret_status == PKI_Common::SUCCESS )
			{
				m_ca_cert_handle->Get_Cert( reinterpret_cast<void**>( &ca_cert ) );

				ret_status = m_ca_key_handle->Get_Key( reinterpret_cast<void**>( &ca_priv_key ), &ca_key_mem,
													   &ca_key_mem_len );

				if ( ret_status == PKI_Common::SUCCESS )
				{
					ret = psX509SetCAIssuedCertExtensions( pool, cert_config, reqExt, ca_cert );

					if ( ret == PS_SUCCESS )
					{
						ret = psX509WriteCAIssuedCertMem( pool, cert_config, pub_key, dn->dnenc,
														  dn->dnencLen, ca_cert, ca_priv_key,
														  reinterpret_cast<unsigned char**>( &ssl_cert_mem ),
														  &ssl_cert_mem_len );
						if ( ret != PS_SUCCESS )
						{
							ret_status = PKI_Common::FAILURE;
							PKI_DEBUG_PRINT( DBG_MSG_ERROR, "CA issued Signing failed" );
						}
						else
						{
							ret_status = m_csr_handle->Get_Key_Handle( &end_entity_key_handle );

							if ( ret_status == PKI_Common::SUCCESS )
							{
								ret_status =
									end_entity_key_handle->Get_Key(
										reinterpret_cast<void**>( &priv_key ),
										key_mem, key_mem_len );

								if ( ret_status != PKI_Common::FAILURE )
								{
									*cert_mem = ssl_cert_mem;
									*cert_mem_len = ssl_cert_mem_len;
									ret_status = PKI_Common::SUCCESS;
									PKI_DEBUG_PRINT( DBG_MSG_INFO, "Signing of certificate successful" );
								}
							}
						}
					}
				}
			}
		}
	}

	if ( dn != nullptr )
	{
		psX509FreeDNStruct( dn, pool );
		psFree( dn, nullptr );
	}

	if ( pub_key != nullptr )
	{
		psDeletePubKey( &pub_key );
	}

	if ( reqExt != nullptr )
	{
		x509FreeExtensions( reqExt );
		psFree( reqExt, nullptr );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Signed_CA_Matrix::~Cert_Signed_CA_Matrix( void )
{
	if ( m_ca_cert_handle != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		m_ca_cert_handle->~Cert_Lib();
		delete ( m_ca_cert_handle );
		m_ca_cert_handle = nullptr;
	}

	if ( m_csr_handle != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		m_csr_handle->~Csr_Lib();
		delete ( m_csr_handle );
		m_csr_handle = nullptr;
	}
}
