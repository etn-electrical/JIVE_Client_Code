/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Pub_Key_ECC_Matrix.h"

#include "StdLib_MV.h"
#include "PKI_Config.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Pub_Key_ECC_Matrix::Pub_Key_ECC_Matrix( uint8_t const* key_mem, uint16_t key_mem_len,
										PKI_Common::pki_status_t* ret_status, char_t* password ) :
	m_key( nullptr ),
	m_key_mem( key_mem ),
	m_key_mem_len( key_mem_len )
{
	BF_ASSERT( ( m_key_mem != nullptr ) && ( m_key_mem_len != 0U ) );
	psPool_t* pool = nullptr;
	int32_t rc;

	m_key = new( psPubKey_t );
	m_key->type = 0U;
	BF_ASSERT( m_key != nullptr );
#if 0	// Enable if ECC public Key format is PEM
	// To be fixed :264 Bytes of memory leak observed in case of PEM
	uint8_t* derOut;
	int32_t derOutLen;
	psPool_t* pool_new = nullptr;
	if ( psPemDecode( pool_new,
					  m_key_mem,
					  m_key_mem_len,
					  NULL,
					  &derOut,
					  reinterpret_cast<size_t*>( &derOutLen ) ) < 0 )
	{
		( *ret_status ) = PKI_Common::FAILURE;
	}
	rc = psParseUnknownPubKeyMem( pool, derOut, derOutLen, nullptr, m_key );
#endif
	rc = psParseUnknownPubKeyMem( pool, m_key_mem, m_key_mem_len, nullptr, m_key );

	if ( rc == PS_SUCCESS )
	{
		( *ret_status ) = PKI_Common::SUCCESS;
		PKI_DEBUG_PRINT( DBG_MSG_INFO, "pub key parsing successful" );
	}
	else
	{
		( *ret_status ) = PKI_Common::FAILURE;
		PKI_DEBUG_PRINT( DBG_MSG_INFO, "pub key parsing failed" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Pub_Key_ECC_Matrix::Get_Key( void** key, uint8_t** key_mem,
													  uint16_t* key_mem_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_key != nullptr )
	{
		*key = m_key;
		*key_mem = const_cast<uint8_t*>( m_key_mem );
		*key_mem_len = m_key_mem_len;

		ret_status = PKI_Common::SUCCESS;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Pub_Key_ECC_Matrix::Verify_Hash( uint8_t* sig, uint16_t sig_len,
														  uint8_t* calculated_hash,
														  uint32_t hash_size )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	int32_t rc = 0U;
	int32 ecc_ret = 0U;

	if ( ( sig != nullptr ) && ( sig_len <= SIGNATURE_MAX_LENGTH ) )
	{
		psPool_t* pool = nullptr;
		uint8_t* signature_ram = new uint8_t[sig_len];
		if ( signature_ram != nullptr )
		{
			BF_Lib::Copy_String( signature_ram, sig, sig_len );
			/**
			 * verify the signature
			 */
			rc = psEccDsaVerify( pool, &m_key->key.ecc, calculated_hash, hash_size, signature_ram,
								 sig_len, &ecc_ret, nullptr );
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
Pub_Key_ECC_Matrix::~Pub_Key_ECC_Matrix( void )
{
	if ( m_key != nullptr )
	{
		m_key->pool = nullptr;
		m_key->keysize = 0;
		m_key->type = 0;
		psEccClearKey( &m_key->key.ecc );
		delete( m_key );
		m_key = nullptr;
	}
}
