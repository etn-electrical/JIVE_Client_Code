/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Sha256_Matrix.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sha256_Matrix::Sha256_Matrix( void ) :
	m_hash_ctx( nullptr )
{
	m_hash_ctx = new psSha256_t;

	BF_ASSERT( m_hash_ctx != nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sha256_Matrix::Init( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	int32_t rc = psSha256Init( m_hash_ctx );

	if ( rc == PS_SUCCESS )
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
PKI_Common::pki_status_t Sha256_Matrix::Update( uint8_t const* data, uint32_t data_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::SUCCESS;

	psSha256Update( m_hash_ctx, const_cast<uint8_t*>( data ),
					data_len );			///< This function does not return any status

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sha256_Matrix::Final( uint8_t* Hash_out )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::SUCCESS;

	psSha256Final( m_hash_ctx, Hash_out );	///< This function does not return any status

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Sha256_Matrix::Size( void )
{
	return ( SHA256_HASH_SIZE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sha256_Matrix::~Sha256_Matrix( void )
{
	if ( m_hash_ctx != nullptr )
	{
		delete( m_hash_ctx );
		m_hash_ctx = nullptr;
	}
}
