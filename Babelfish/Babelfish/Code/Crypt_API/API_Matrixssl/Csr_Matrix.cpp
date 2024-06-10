/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Csr_Matrix.h"

#include "Includes.h"
#include "PKI_Common.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Csr_Matrix::Csr_Matrix( Key_Lib* key, Cert_Conf_Lib* cert_conf ) :
	m_key( key ),
	m_cert_conf( cert_conf ),
	m_csr_mem( nullptr ),
	m_csr_mem_len( 0 )
{
	if ( ( key == nullptr ) || ( cert_conf == nullptr ) )
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Csr_Matrix::Csr_Matrix( uint8_t* csr_mem, uint16_t csr_mem_len ) :
	m_key( nullptr ),
	m_cert_conf( nullptr ),
	m_csr_mem( csr_mem ),
	m_csr_mem_len( csr_mem_len )
{
	if ( ( csr_mem == nullptr ) || ( csr_mem_len == nullptr ) )
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Csr_Matrix::Generate( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	psPool_t* pool = nullptr;
	int32_t ret = PS_FAILURE;
	psPubKey_t* key = nullptr;
	uint8_t* key_mem = nullptr;
	uint16_t key_mem_len = 0;
	psCertConfig_t* cert_config = nullptr;

	if ( ( m_key != nullptr ) && ( m_cert_conf != nullptr ) )
	{
		ret_status = m_key->Get_Key( reinterpret_cast<void**>( &key ), &key_mem, &key_mem_len );

		if ( ret_status == PKI_Common::SUCCESS )
		{
			ret_status = m_cert_conf->Get_Cert_Config( reinterpret_cast<void**>( &cert_config ) );

			if ( ret_status == PKI_Common::SUCCESS )
			{
				ret =
					psWriteCertReqMem( pool, key, cert_config, &m_csr_mem,
									   reinterpret_cast<int32_t*>( &m_csr_mem_len ) );
			}
		}
	}
	if ( ret == PS_SUCCESS )
	{
		m_del_csr_mem = true;
		ret_status = PKI_Common::SUCCESS;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Csr_Matrix::Get( void** dn, void** pub_key, void** req_ext )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	psPool_t* pool = nullptr;

	int32_t ret = PS_FAILURE;

	x509DNattributes_t* DN = nullptr;
	psPubKey_t* key = nullptr;
	x509v3extensions_t* reqExt = nullptr;

	if ( m_csr_mem == nullptr )
	{
		Generate();
	}

	if ( m_csr_mem != nullptr )
	{
		ret = psParseCertReqBuf( pool, m_csr_mem, m_csr_mem_len, &DN, &key, &reqExt );

		if ( ret == PS_SUCCESS )
		{
			*dn = DN;
			*pub_key = key;
			*req_ext = reqExt;
			ret_status = PKI_Common::SUCCESS;
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
Csr_Matrix::~Csr_Matrix( void )
{
	if ( m_key != nullptr )
	{
		m_key->~Key_Lib();
		delete ( m_key );
		m_key = nullptr;
	}

	if ( m_cert_conf != nullptr )
	{
		m_cert_conf->~Cert_Conf_Lib();
		delete ( m_cert_conf );
		m_cert_conf = nullptr;
	}

	if ( ( m_del_csr_mem == true ) && ( m_csr_mem != nullptr ) )
	{
		psFree( m_csr_mem, nullptr );
		m_csr_mem = nullptr;
		// Set to false to allow execution only once in case of optimization
		m_del_csr_mem = false;
	}
}
