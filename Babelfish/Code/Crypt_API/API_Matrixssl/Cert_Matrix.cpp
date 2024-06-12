/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Cert_Matrix.h"

#include "Includes.h"
#include "PKI_Common.h"
#include "StdLib_MV.h"
#include "PKI_Config.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Matrix::Cert_Matrix( uint8_t* cert_mem, uint16_t cert_mem_len,
						  PKI_Common::pki_status_t* ret_status ) :
	m_cert_mem( cert_mem ),
	m_cert_mem_len( cert_mem_len ),
	m_cert( nullptr )
{
	psPool_t* pool = nullptr;
	int32_t ret;
	psX509Cert_t* x_cert;

	BF_ASSERT( m_cert_mem != nullptr );
	ret = psX509ParseCert( pool, m_cert_mem, m_cert_mem_len, &x_cert,
						   CERT_STORE_UNPARSED_BUFFER | CERT_STORE_DN_BUFFER );

	if ( ret > 0 )
	{
		m_cert = x_cert;
		*ret_status = PKI_Common::SUCCESS;
	}
	else
	{
		psX509FreeCert( x_cert );
		*ret_status = PKI_Common::FAILURE;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Get_Pub_Key( uint8_t** pub_key, uint16_t* pub_key_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_cert != nullptr )
	{
		/* get public key from unparsed bin of the certificate */

		if ( ( m_cert->publicKeyDerOffsetIntoUnparsedBin != 0 ) &&
			 ( m_cert->publicKeyDerLen != 0 ) && ( m_cert->binLen != 0U ) )
		{
			*pub_key =
				reinterpret_cast<uint8_t*>( m_cert->unparsedBin +
											m_cert->publicKeyDerOffsetIntoUnparsedBin );
			*pub_key_len = m_cert->publicKeyDerLen;
			ret_status = PKI_Common::SUCCESS;
		}
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Get_Signature( uint8_t** signature, uint16_t* signature_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_cert != nullptr )
	{
		*signature = m_cert->signature;
		*signature_len = m_cert->signatureLen;
		ret_status = PKI_Common::SUCCESS;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Get_Cert_Hash( uint8_t** key_hash, uint16_t* key_hash_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_cert != nullptr )
	{
		*key_hash = m_cert->sigHash;
		*key_hash_len = m_cert->sigHashLen;
		ret_status = PKI_Common::SUCCESS;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Verify_Hash( uint8_t* sig, uint16_t sig_len,
												   uint8_t* hash,
												   uint32_t hash_size )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	int32_t rc;
	int32 ecc_ret;

	if ( ( sig != nullptr ) && ( sig_len <= SIGNATURE_MAX_LENGTH ) )
	{
		psPool_t* pool = nullptr;
		uint8_t* signature_ram = new uint8_t[sig_len];
		if ( signature_ram != nullptr )
		{
			BF_Lib::Copy_String( signature_ram, sig, ( sig_len ) );

			rc =
				psEccDsaVerify( pool,
								reinterpret_cast<const psEccKey_t*>( &m_cert->publicKey.key.ecc ),
								hash, hash_size, signature_ram, sig_len, &ecc_ret, nullptr );
			if ( ( rc < PS_SUCCESS ) || ( ecc_ret != 1U ) )
			{
				ret_status = PKI_Common::FAILURE;
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Verification of signature failed" );
			}
			else
			{
				ret_status = PKI_Common::SUCCESS;
				PKI_DEBUG_PRINT( DBG_MSG_INFO, "Verification of signature successful" );
			}
			delete[] signature_ram;
		}
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Get_Cert( void** cert )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_cert != nullptr )
	{
		*cert = reinterpret_cast<void*>( m_cert );
		ret_status = PKI_Common::SUCCESS;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Matrix::Authenticate( psX509Cert_t* issuer_cert )
{
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Matrix::~Cert_Matrix( void )
{
	if ( m_cert != nullptr )
	{
		psX509FreeCert( m_cert );
		m_cert = nullptr;
	}
}
