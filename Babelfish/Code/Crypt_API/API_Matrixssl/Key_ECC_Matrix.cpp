/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "Key_ECC_Matrix.h"
#include "StdLib_MV.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_ECC_Matrix::Key_ECC_Matrix( PKI_Common::key_size_ecc_t key_size,
								PKI_Common::key_curve_type_t curve_type ) :
	m_key( nullptr ),
	m_key_size( key_size ),
	m_key_mem( nullptr ),
	m_key_mem_len( 0U ),
	m_key_mem_rec( false )
{
	switch ( curve_type )
	{
		case PKI_Common::CURVE_SECP192R1:
			m_curve_type = 19U;
			break;

		case PKI_Common::CURVE_SECP224R1:
			m_curve_type = 21U;
			break;

		case PKI_Common::CURVE_SECP256R1:
			m_curve_type = 23U;
			break;

		case PKI_Common::CURVE_SECP384R1:
			m_curve_type = 24U;
			break;

		case PKI_Common::CURVE_SECP521R1:
			m_curve_type = 25U;
			break;

		case PKI_Common::CURVE_BRAIN256R1:
			m_curve_type = 26U;
			break;

		case PKI_Common::CURVE_BRAIN384R1:
			m_curve_type = 27U;
			break;

		case PKI_Common::CURVE_BRAIN512R1:
			m_curve_type = 28U;
			break;

		case PKI_Common::CURVE_X25519:
			m_curve_type = 29U;
			break;

		case PKI_Common::CURVE_X44830U:
			m_curve_type = 30U;
			break;

		case PKI_Common::CURVE_BRAIN224R1:
			m_curve_type = 255U;
			break;

		default:
			/* Unsupported curve type. */
			BF_ASSERT( false );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_ECC_Matrix::Key_ECC_Matrix( uint8_t* key_mem, uint16_t key_mem_len, char_t* password ) :
	m_key( nullptr ),
	m_key_size( 0U ),
	m_key_mem( key_mem ),
	m_key_mem_len( key_mem_len ),
	m_curve_type( 0U ),
	m_key_mem_rec( true )
{
	int32_t rc;
	psPool_t* pool = nullptr;

	m_key = new( psPubKey_t );
	psEccInitKey( nullptr, &( m_key->key.ecc ), nullptr );

	m_key->keysize = 0U;

	rc = psEccParsePrivKey( pool, m_key_mem, m_key_mem_len, &( m_key->key.ecc ), NULL );
	if ( rc >= PS_SUCCESS )
	{
		m_key->type = PS_ECC;
		m_key->keysize = psEccSize( &( m_key->key.ecc ) );
		m_key->pool = pool;
	}
	else
	{
		if ( &( m_key->key.ecc ) != nullptr )
		{
			psEccClearKey( &( m_key->key.ecc ) );
		}
		delete( m_key );
		m_key = nullptr;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Key_ECC_Matrix::Generate( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	psPool_t* pool = nullptr;
	const psEccCurve_t* params = nullptr;

	int32_t ret;

	m_key = new ( psPubKey_t );
	psInitPubKey( pool, m_key, PS_ECC );

	if ( m_key != nullptr )
	{
		ret = getEccParamById( m_curve_type, &params );

		if ( ret == PS_SUCCESS )
		{
			ret = psEccGenKey( pool, &( m_key->key.ecc ), params, nullptr );

			if ( ret == PS_SUCCESS )
			{
				m_key->pool = pool;
				m_key->type = PS_ECC;
				m_key->keysize = m_key_size;

				ret_status = PKI_Common::SUCCESS;
				PKI_DEBUG_PRINT( DBG_MSG_INFO, "ECC Key generation successful" );
			}
		}
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Key_ECC_Matrix::Get_Key( void** key_ecc, uint8_t** key_mem,
												  uint16_t* key_mem_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	int32_t ret;

	if ( m_key == nullptr )
	{
		ret_status = Generate();

		if ( ret_status == PKI_Common::SUCCESS )
		{
			*key_ecc = m_key;

			ret = psEccWritePrivKeyMem( nullptr, &( m_key->key.ecc ), &m_key_mem, &m_key_mem_len );

			if ( ret == PS_SUCCESS )
			{
				*key_mem = m_key_mem;
				*key_mem_len = m_key_mem_len;
				ret_status = PKI_Common::SUCCESS;
			}
			else
			{
				ret_status = PKI_Common::FAILURE;
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Private key parsing failure" );
			}
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "ECC Key Generation Failure" );
		}
	}
	else
	{
		*key_ecc = m_key;
		if ( m_key_mem != nullptr )
		{
			*key_mem = m_key_mem;
			*key_mem_len = m_key_mem_len;
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
PKI_Common::pki_status_t Key_ECC_Matrix::Set_Key( uint8_t* key_mem, uint16_t key_mem_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( m_key_mem == nullptr )
	{
		m_key_mem = reinterpret_cast<uint8_t*>( psMalloc( nullptr, key_mem_len ) );

		if ( m_key_mem != nullptr )
		{
			BF_Lib::Copy_Const_String( reinterpret_cast<uint8_t*>( m_key_mem ),
									   reinterpret_cast<uint8_t*>( key_mem ), key_mem_len );

			m_key_mem_len = key_mem_len;
			ret_status = PKI_Common::SUCCESS;
		}
	}
	else
	{
		if ( m_key_mem_len < key_mem_len )
		{
			void* buf = psRealloc( m_key_mem, key_mem_len, nullptr );

			if ( buf != nullptr )
			{
				BF_Lib::Copy_Const_String( reinterpret_cast<uint8_t*>( buf ),
										   reinterpret_cast<uint8_t*>( key_mem ), key_mem_len );

				m_key_mem = reinterpret_cast<uint8_t*>( buf );
				m_key_mem_len = key_mem_len;
				ret_status = PKI_Common::SUCCESS;
			}
		}
	}
	if ( ret_status != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Set key failed" );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Key_ECC_Matrix::~Key_ECC_Matrix( void )
{
	if ( m_key != nullptr )
	{
		if ( &( m_key->key.ecc ) != nullptr )
		{
			psEccClearKey( &( m_key->key.ecc ) );
		}
		delete( m_key );
		m_key = nullptr;
	}
	if ( ( m_key_mem_rec == false ) && ( m_key_mem != nullptr ) )
	{
		psFree( m_key_mem, nullptr );
		m_key_mem = nullptr;
		// Set to true to allow execution only once in case of optimization
		m_key_mem_rec = true;
	}
}
