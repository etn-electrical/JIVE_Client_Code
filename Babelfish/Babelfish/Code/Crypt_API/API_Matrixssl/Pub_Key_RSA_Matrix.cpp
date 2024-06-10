/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Pub_Key_RSA_Matrix.h"

#include "StdLib_MV.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Pub_Key_RSA_Matrix::Pub_Key_RSA_Matrix( uint8_t const* key_mem, uint16_t key_mem_len,
										PKI_Common::pki_status_t* ret_status, char_t* password ) :
	m_key( nullptr ),
	m_key_mem( key_mem ),
	m_key_mem_len( key_mem_len )
{
	psPool_t* pool = nullptr;
	int32_t rc;

	BF_ASSERT( ( m_key_mem != nullptr ) && ( m_key_mem_len != 0U ) );

	m_key = new( psRsaKey_t );

	BF_ASSERT( m_key != nullptr );
	rc = psRsaParsePubKeyMem( pool, const_cast<uint8_t*>( key_mem ), m_key_mem_len, nullptr,
							  m_key );
	if ( rc == PS_SUCCESS )
	{
		( *ret_status ) = PKI_Common::SUCCESS;
		PKI_DEBUG_PRINT( DBG_MSG_INFO, "RSA: Pub Key Parsing successful" );
	}
	else
	{
		( *ret_status ) = PKI_Common::FAILURE;
		PKI_DEBUG_PRINT( DBG_MSG_ERROR, "RSA: Pub Key Parsing Failure" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Pub_Key_RSA_Matrix::Get_Key( void** key, uint8_t** key_mem,
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
PKI_Common::pki_status_t Pub_Key_RSA_Matrix::Verify_Hash( uint8_t* sig, uint16_t sig_len,
														  uint8_t* calculated_hash,
														  uint32_t hash_size )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	int32_t rc;

	uint8_t* decrypted_hash = new uint8_t[hash_size];

	if ( ( decrypted_hash != nullptr ) && ( sig != nullptr ) &&
		 ( ( sig_len ) <= SIGNATURE_MAX_LENGTH ) )
	{
		psPool_t* pool = nullptr;
		uint8_t* signature_ram = new uint8_t[sig_len];
		if ( signature_ram != nullptr )
		{
			BF_Lib::Copy_String( signature_ram, sig, sig_len );
			/**
			 * getting hash from signature( decrypting the signature using the public key )
			 */
			rc =
				pubRsaDecryptSignedElement( pool, m_key,
											reinterpret_cast<unsigned char*>( signature_ram ),
											sig_len, reinterpret_cast<uint8_t*>( decrypted_hash ),
											hash_size, nullptr );
			if ( rc == PS_SUCCESS )
			{
				if ( BF_Lib::String_Compare( calculated_hash, decrypted_hash, hash_size ) == true )
				{
					ret_status = PKI_Common::SUCCESS;
					PKI_DEBUG_PRINT( DBG_MSG_INFO, "Hash Verification Successful" );
				}
			}
			delete[] signature_ram;
		}
	}
	if ( decrypted_hash != nullptr )
	{
		delete[] decrypted_hash;
	}
	if ( ret_status != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Hash Verification Failed" );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Pub_Key_RSA_Matrix::~Pub_Key_RSA_Matrix( void )
{
	/* Clear working memory allocated by matrixssl */
	if ( m_key != nullptr )
	{
		pstm_clear( &( m_key->N ) );
		pstm_clear( &( m_key->e ) );
		m_key->size = 0;
		m_key->optimized = 0;
		m_key->pool = nullptr;
		delete( m_key );
		m_key = nullptr;
	}
}
