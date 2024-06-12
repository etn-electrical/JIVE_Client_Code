/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "Cert_Signed_SS_Matrix.h"
#include "PKI_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Signed_SS_Matrix::Cert_Signed_SS_Matrix( Key_Lib* key_handle, Cert_Conf_Lib* cert_handle ) :
	m_key_handle( key_handle ),
	m_cert_handle( cert_handle )
{
	BF_ASSERT( ( m_key_handle != nullptr ) ||
			   ( m_cert_handle != nullptr ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Signed_SS_Matrix::Sign( uint8_t** key_mem, uint16_t* key_mem_len,
													  uint8_t** cert_mem, uint32_t* cert_mem_len )
{
	PKI_Common::pki_status_t ret_status;
	psPool_t* pool = nullptr;
	int32_t ret;
	psPubKey_t* signing_key = nullptr;
	psCertConfig_t* cert_config = nullptr;

	ret_status = m_key_handle->Get_Key( reinterpret_cast<void**>( &signing_key ), key_mem,
										reinterpret_cast<uint16_t*>( key_mem_len ) );

	if ( ret_status == PKI_Common::SUCCESS )
	{
		ret_status = m_cert_handle->Get_Cert_Config( reinterpret_cast<void**>( &cert_config ) );

		if ( ret_status == PKI_Common::SUCCESS )
		{
			ret = psX509WriteSelfSignedCertMem( pool,
												cert_config,
												signing_key, cert_mem,
												reinterpret_cast<int32_t*>( cert_mem_len ) );
			if ( ret != PS_SUCCESS )
			{
				ret_status = PKI_Common::FAILURE;
				PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Certificate Self signing failed" );
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
Cert_Signed_SS_Matrix::~Cert_Signed_SS_Matrix( void )
{
	if ( m_cert_handle != nullptr )
	{
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		m_cert_handle->~Cert_Conf_Lib();
		delete ( m_cert_handle );
		m_cert_handle = nullptr;
	}
}
