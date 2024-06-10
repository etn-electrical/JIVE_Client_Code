/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Sig_Verify_ECC_Matrix.h"

#include "StdLib_MV.h"
#include "PKI_Debug.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sig_Verify_ECC_Matrix::Sig_Verify_ECC_Matrix( Fw_Store* fw_ctrl, Pub_Key_Lib* ecc_pub_key ) :
	m_fw_ctrl( fw_ctrl ),
	m_ecc_pub_key( ecc_pub_key ),
	m_cert_handle( nullptr ),
	m_is_cert( false )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sig_Verify_ECC_Matrix::Sig_Verify_ECC_Matrix( Fw_Store* fw_ctrl, Cert_Matrix* cert_handle ) :
	m_fw_ctrl( fw_ctrl ),
	m_ecc_pub_key( nullptr ),
	m_cert_handle( cert_handle ),
	m_is_cert( true )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sig_Verify_ECC_Matrix::Verify( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	uint8_t* calculated_hash = nullptr;
	uint8_t* sign = nullptr;
	uint16_t sign_len = 0U;
	uint32_t hash_size = 0U;

	if ( m_fw_ctrl->Get_Hash( &calculated_hash, &hash_size ) == PKI_Common::SUCCESS )
	{
		if ( m_fw_ctrl->Get_Fw_Sign( &sign, &sign_len ) == PKI_Common::SUCCESS )
		{
			if ( true == m_is_cert )
			{
				if ( m_cert_handle->Verify_Hash( sign, sign_len, calculated_hash,
												 hash_size ) == PKI_Common::SUCCESS )
				{
					ret_status = PKI_Common::SUCCESS;
				}
			}
			else
			{
				if ( m_ecc_pub_key->Verify_Hash( sign, sign_len, calculated_hash,
												 hash_size ) == PKI_Common::SUCCESS )
				{
					ret_status = PKI_Common::SUCCESS;
				}
			}
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR,
							 "Data hash calculation failed during signature verification" );
		}
	}
	if ( m_fw_ctrl->Free_Hash( calculated_hash ) != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR,
						 "calculated_hash memory deallocation failed" );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sig_Verify_ECC_Matrix::Verify_Trusted_Fw_Sign( void )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	uint8_t* calculated_hash = nullptr;
	uint8_t* sign = nullptr;
	uint16_t sign_len = 0U;
	uint32_t hash_size = 0U;

	if ( m_fw_ctrl->Get_Hash_Trusted( &calculated_hash, &hash_size ) == PKI_Common::SUCCESS )
	{
		if ( m_fw_ctrl->Get_Trusted_Fw_Sign( &sign, &sign_len ) == PKI_Common::SUCCESS )
		{
			if ( m_cert_handle->Verify_Hash( sign, sign_len, calculated_hash,
											 hash_size ) == PKI_Common::SUCCESS )
			{
				ret_status = PKI_Common::SUCCESS;
			}
		}
		if ( m_fw_ctrl->Free_Trusted_Fw_Sign( sign ) != PKI_Common::SUCCESS )
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR,
							 "Fw Signature memory deallocation failed" );
		}
	}
	else
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Data hash calculation failed during signature verification" );
	}
	if ( m_fw_ctrl->Free_Hash( calculated_hash ) != PKI_Common::SUCCESS )
	{
		PKI_DEBUG_PRINT( DBG_MSG_ERROR,
						 "calculated_hash memory deallocation failed" );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sig_Verify_ECC_Matrix::~Sig_Verify_ECC_Matrix( void )
{}
