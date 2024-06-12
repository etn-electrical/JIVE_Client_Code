/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "Key_RSA_Matrix.h"
#include "StdLib_MV.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_RSA_Matrix::Key_RSA_Matrix( PKI_Common::key_size_rsa_t key_size ) :
	m_key( nullptr ),
	m_key_size( key_size ),
	m_key_mem( nullptr ),
	m_key_mem_len( 0U ),
	m_construct_failed( false ),
	m_key_mem_rec( false )
{
	if ( m_key_size % 1024U != 0U )
	{
		m_construct_failed = true;
		/* Key should be multiple of 1024 */
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_RSA_Matrix::Key_RSA_Matrix( uint8_t* key_mem, uint16_t key_mem_len, char_t* password ) :
	m_key( nullptr ),
	m_key_size( 0U ),
	m_key_mem( key_mem ),
	m_key_mem_len( key_mem_len ),
	m_construct_failed( false ),
	m_key_mem_rec( true )
{
	psPool_t* pool = nullptr;

	m_key = new( psPubKey_t );
	int32_t rc;

	psRsaInitKey( NULL, &m_key->key.rsa );

	m_key->keysize = 0U;
	rc = psRsaParsePkcs1PrivKey( pool, key_mem, key_mem_len, &m_key->key.rsa );
	if ( rc >= PS_SUCCESS )
	{
		m_key->type = PS_RSA;
		m_key->keysize = psRsaSize( &m_key->key.rsa );
		m_key->pool = pool;
	}
	else
	{
		if ( &m_key->key.rsa != nullptr )
		{
			psRsaClearKey( &m_key->key.rsa );
		}
		delete( m_key );
		m_key = nullptr;
		m_construct_failed = true;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Key_RSA_Matrix::Generate( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	psPool_t* pool = nullptr;
	psRsaKey_t* ptr_key = nullptr;

	int32_t ret;

	m_key = new ( psPubKey_t );

	ret = psRsaMakeKey( pool, m_key_size, E_VALUE, &ptr_key );

	if ( ret == PS_SUCCESS )
	{
		m_key->pool = pool;
		m_key->type = PS_RSA;
		m_key->keysize = ptr_key->size;
		BF_Lib::Copy_Const_String( reinterpret_cast<uint8_t*>( &( m_key->key.rsa ) ),
								   reinterpret_cast<uint8_t*>( ptr_key ), sizeof( psRsaKey_t ) );

		ret_status = PKI_Common::SUCCESS;
	}

	if ( ptr_key != nullptr )
	{
		psFree( ptr_key, nullptr );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Key_RSA_Matrix::Get_Key( void** key_rsa, uint8_t** key_mem,
												  uint16_t* key_mem_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::SUCCESS;
	int32_t ret = PS_SUCCESS;

	if ( m_construct_failed == false )
	{
		if ( m_key == nullptr )
		{
			ret_status = Generate();
		}

		if ( ret_status == PKI_Common::SUCCESS )
		{
			*key_rsa = m_key;

			if ( m_key_mem == nullptr )
			{
				ret =
					psRsaWritePrivKeyMem( nullptr, &( m_key->key.rsa ), &m_key_mem,
										  &m_key_mem_len );
			}

			if ( ret == PS_SUCCESS )
			{
				*key_mem = m_key_mem;
				*key_mem_len = m_key_mem_len;
			}
			else
			{
				ret_status = PKI_Common::FAILURE;
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "RSA: Private key Parsing Failure" );
			}
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "RSA Key Generation Failure" );
		}
	}
	else
	{
		ret_status = PKI_Common::FAILURE;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_RSA_Matrix::~Key_RSA_Matrix( void )
{
	if ( m_key != nullptr )
	{
		if ( &m_key->key.rsa != nullptr )
		{
			psRsaClearKey( &m_key->key.rsa );
		}
		delete( m_key );
		m_key = nullptr;
	}
	if ( ( m_key_mem_rec == false ) && ( m_key_mem != nullptr ) )
	{
		psFree( m_key_mem, nullptr );
		m_key_mem = nullptr;
	}
}
