/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Sig_Verify_RSA_Matrix.h"

#include "StdLib_MV.h"
#include "uC_Watchdog.h"
#include "PKI_Debug.h"

#define WATCHDOG_TIMEOUT_SIGNATURE_VERIFICATION 3000U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sig_Verify_RSA_Matrix::Sig_Verify_RSA_Matrix( Sha_Lib* hash_ctx, Pub_Key_Lib* rsa_pub_key,
											  uint32_t max_num_bytes,
											  callback_t cb ) :
	m_hash( hash_ctx ),
	m_rsa_pub_key( rsa_pub_key ),
	m_max_ram_buff_size( max_num_bytes ),
	m_callback_func( cb )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sig_Verify_RSA_Matrix::Verify( uint8_t const* data, uint32_t data_len,
														uint8_t* sig,
														uint16_t sig_len )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( PKI_Common::SUCCESS == m_hash->Init() )
	{
		uint32_t hash_size = m_hash->Size();
		uint32_t update_buff_size = m_max_ram_buff_size;
		uint32_t remaining_data_len = 0U;
		uint32_t i = 0U;
		bool update_failed_flag = false;
		uint8_t* calculated_hash = new uint8_t[hash_size];

		if ( calculated_hash != nullptr )
		{
			if ( data_len < update_buff_size )
			{
				update_buff_size = data_len;
			}
			else
			{
				update_buff_size = m_max_ram_buff_size;
			}
			while ( ( i < data_len ) && ( update_buff_size != 0U ) )
			{
				if ( PKI_Common::SUCCESS ==
					 m_hash->Update( ( const_cast<uint8_t*>( data + i ) ), update_buff_size ) )
				{
					m_callback_func();
					i = i + update_buff_size;
					remaining_data_len = data_len - i;
					if ( remaining_data_len < update_buff_size )
					{
						update_buff_size = remaining_data_len;
					}
				}
				else
				{
					update_failed_flag = true;
					PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Data Update Failed during Signature Verification " );
					break;
				}
			}
			if ( update_failed_flag == false )
			{
				uC_Watchdog::Init( WATCHDOG_TIMEOUT_SIGNATURE_VERIFICATION );
				if ( PKI_Common::SUCCESS == m_hash->Final( calculated_hash ) )
				{
					if ( PKI_Common::SUCCESS ==
						 m_rsa_pub_key->Verify_Hash( sig, sig_len, calculated_hash, hash_size ) )
					{
						ret_status = PKI_Common::SUCCESS;
					}
				}
				else
				{
					PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Data Hash Calculation Failed during Signature Verification" );
				}
				uC_Watchdog::Init( WATCHDOG_TIMEOUT );
			}
		}
		else
		{
			PKI_DEBUG_PRINT( DBG_MSG_ERROR, "Memory Allocation Failed to Store Calculated Hash" );
		}
		if ( calculated_hash != nullptr )
		{
			delete[] calculated_hash;
		}
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Sig_Verify_RSA_Matrix::Verify( NV_Ctrl* nv_handle,
														const PKI_Common::nv_chip_config_t* data_nv_config,
														const PKI_Common::nv_chip_config_t* sig_nv_config )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	if ( PKI_Common::SUCCESS == m_hash->Init() )
	{
		uint32_t hash_size = m_hash->Size();
		uint32_t update_buff_size = m_max_ram_buff_size;
		uint32_t remaining_data_len = 0U;
		uint32_t i = 0U;
		bool update_failed_flag = false;

		uint8_t* calculated_hash = new uint8_t[hash_size];
		uint8_t* data = new uint8_t[m_max_ram_buff_size];
		uint8_t* sig = new uint8_t[sig_nv_config->length];

		if ( ( calculated_hash != nullptr ) && ( data != nullptr ) && ( sig != nullptr ) )
		{
			if ( data_nv_config->length < update_buff_size )
			{
				update_buff_size = data_nv_config->length;
			}
			else
			{
				update_buff_size = m_max_ram_buff_size;
			}

			while ( ( i < data_nv_config->length ) && ( update_buff_size != 0U ) )
			{
				if ( NV_Ctrl::SUCCESS ==
					 nv_handle->Read( data, data_nv_config->start_address + i, update_buff_size,
									  false ) )
				{
					if ( PKI_Common::SUCCESS ==
						 m_hash->Update( ( const_cast<uint8_t*>( data ) ), update_buff_size ) )
					{
						m_callback_func();
						i = i + update_buff_size;
						remaining_data_len = data_nv_config->length - i;
						if ( remaining_data_len < update_buff_size )
						{
							update_buff_size = remaining_data_len;
						}
					}
					else
					{
						update_failed_flag = true;
						break;
					}
				}
			}

			if ( update_failed_flag == false )
			{

				if ( PKI_Common::SUCCESS == m_hash->Final( calculated_hash ) )
				{
					if ( NV_Ctrl::SUCCESS ==
						 nv_handle->Read( sig, sig_nv_config->start_address, sig_nv_config->length,
										  false ) )
					{

						if ( PKI_Common::SUCCESS ==
							 m_rsa_pub_key->Verify_Hash( sig, sig_nv_config->length,
														 calculated_hash, hash_size ) )
						{
							ret_status = PKI_Common::SUCCESS;
						}
					}
				}
			}
		}
		if ( calculated_hash != nullptr )
		{
			delete[] calculated_hash;
		}
		if ( data != nullptr )
		{
			delete[] data;
		}
		if ( sig != nullptr )
		{
			delete[] sig;
		}
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Sig_Verify_RSA_Matrix::~Sig_Verify_RSA_Matrix( void )
{}
