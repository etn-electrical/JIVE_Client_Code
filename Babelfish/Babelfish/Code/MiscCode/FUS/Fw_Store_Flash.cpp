/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Fw_Store_Flash.h"
#include "CRC16.h"
#include "FUS_Debug.h"

using namespace BF_Lib;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Fw_Store_Flash::Fw_Store_Flash( const PKI_Common::nv_chip_config_t* mem_config, callback_t cb,
								bool* integrity_status ) :
	m_fw_ctrl( reinterpret_cast<real_fw_header*>( mem_config->start_address ) ),
	m_mem_cfg_len( mem_config->length ),
	m_code_sign_ptr( nullptr ),
	m_callback_func( cb )
{
	*integrity_status = false;

	if ( m_fw_ctrl != nullptr )
	{
		m_code_sign_ptr =
			reinterpret_cast<code_sign_struct_t*>( reinterpret_cast<uint8_t*>( m_fw_ctrl ) +
												   m_fw_ctrl->f.data_length );
		if ( m_code_sign_ptr != nullptr )
		{
			if ( Code_Sign_Info_Integrity_Check() == PKI_Common::SUCCESS )
			{
				*integrity_status = true;
			}
			else
			{
				m_code_sign_ptr = nullptr;
				FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block Validation Failed" );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Code_Sign_Info_Integrity_Check( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	uint8_t algo_type = PKI_Common::PKI_ECC;

	if ( m_code_sign_ptr->identity_mark == CODE_SIGN_INFO_IDENTITY_MARK )
	{
		if ( m_code_sign_ptr->header_version_num == CODE_SIGN_INFO_HEADER_VERSION )
		{
			if ( m_code_sign_ptr->algo_type == algo_type )
			{
				if ( m_code_sign_ptr->ecc_curve_type == ECC_CURVE_TYPE )
				{
					if ( m_code_sign_ptr->key_size == KEY_SIZE )
					{
						if ( m_code_sign_ptr->hash_size == HASH_SIZE )
						{
							if ( Validate_Code_Sign_Info_CRC() == PKI_Common::SUCCESS )
							{
								ret = PKI_Common::SUCCESS;
								FUS_DEBUG_PRINT( DBG_MSG_INFO,
												 "Validation of Code Sign Info Block Successful" );
							}
						}
					}
				}
			}
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Validate_Code_Sign_Info_CRC( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	uint16_t calc_crc = 0U;

	calc_crc =
		CRC16::Calc_On_String(
			reinterpret_cast<uint8_t*>( &m_code_sign_ptr->identity_mark ),
			( sizeof( code_sign_struct_t ) -
			  sizeof( m_code_sign_ptr->crc ) ), 0U );

	if ( m_code_sign_ptr->crc == calc_crc )
	{
		ret = PKI_Common::SUCCESS;
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Code Sign Info Block CRC Verified" );
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block CRC Verification Failed" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Fw_Location( uint8_t** fw_location_out,
														  uint32_t* fw_package_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*fw_location_out = reinterpret_cast<uint8_t*>( m_fw_ctrl );
		*fw_package_len_out = m_code_sign_ptr->fw_sign - reinterpret_cast<uint8_t*>( m_fw_ctrl );
		ret = PKI_Common::SUCCESS;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_FVK_Cert( uint8_t** fvk_cert_out,
													   uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*fvk_cert_len_out = ( m_code_sign_ptr->fvk_cert_len[1] << 8 ) |
			m_code_sign_ptr->fvk_cert_len[0];

		if ( ( *fvk_cert_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *fvk_cert_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *fvk_cert_len_out <= FVK_CERT_MAX_SIZE ) )
		{
			*fvk_cert_out = m_code_sign_ptr->fvk_cert;
			FUS_DEBUG_PRINT( DBG_MSG_INFO,
							 "Code Sign Info Block - FVK Public Key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - FVK Public Key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_FVK_Key( uint8_t** fvk_key_out,
													  uint16_t* fvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*fvk_key_len_out = ( m_code_sign_ptr->fvk_key_len[1] << 8 ) |
			m_code_sign_ptr->fvk_key_len[0];

		if ( ( *fvk_key_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *fvk_key_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *fvk_key_len_out <= FVK_KEY_MAX_SIZE ) )
		{
			*fvk_key_out = m_code_sign_ptr->fvk_key;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Code Sign Info Block - FVK Key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - FVK Key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_IVK_Cert( uint8_t** ivk_cert_out,
													   uint16_t* ivk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*ivk_cert_len_out = ( m_code_sign_ptr->ivk_cert_len[1] << 8 ) |
			m_code_sign_ptr->ivk_cert_len[0];

		if ( ( *ivk_cert_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *ivk_cert_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *ivk_cert_len_out <= IVK_CERT_MAX_SIZE ) )
		{
			*ivk_cert_out = m_code_sign_ptr->ivk_cert;
			FUS_DEBUG_PRINT( DBG_MSG_INFO,
							 "Code Sign Info Block - IVK Public Key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - IVK Public Key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Fw_Sign( uint8_t** fw_sign_out,
													  uint16_t* fw_sign_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*fw_sign_len_out = ( m_code_sign_ptr->fw_sign_len[1] << 8 ) |
			m_code_sign_ptr->fw_sign_len[0];

		if ( ( *fw_sign_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *fw_sign_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *fw_sign_len_out <= FW_SIGN_MAX_SIZE ) )
		{
			*fw_sign_out = m_code_sign_ptr->fw_sign;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Code Sign Info Block - FW Signature Read Successful" );
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Firmware Signature Length: %d", *fw_sign_len_out );
			ret = PKI_Common::SUCCESS;
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_RVK_Key( uint8_t** rvk_key_out,
													  uint16_t* rvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*rvk_key_len_out = ( m_code_sign_ptr->rvk_key_len[1] << 8 ) |
			m_code_sign_ptr->rvk_key_len[0];

		if ( ( *rvk_key_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *rvk_key_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *rvk_key_len_out <= RVK_KEY_MAX_SIZE ) )
		{
			*rvk_key_out = m_code_sign_ptr->rvk_key;
			ret = PKI_Common::SUCCESS;
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Hash( uint8_t** calculated_hash,
												   uint32_t* hash_size_out )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	Sha_Lib* sha256 = new Sha256_Matrix();

	if ( sha256 != nullptr )
	{
		if ( PKI_Common::SUCCESS == sha256->Init() )
		{
			uint32_t hash_size = sha256->Size();
			*hash_size_out = hash_size;
			*calculated_hash = new uint8_t[hash_size];

			if ( *calculated_hash != nullptr )
			{
				uint32_t max_update_buff_size = MAX_ALLOCATE_BYTES;
				uint32_t update_buff_size = max_update_buff_size;
				uint32_t remaining_data_len = 0U;
				uint32_t i = 0U;
				bool update_failed_flag = false;
				uint8_t* data = nullptr;///< start of the firmware/data
				uint32_t data_len = 0U;
				uint8_t* data_buf = new uint8_t[max_update_buff_size];

				if ( data_buf != nullptr )
				{
					if ( Get_Fw_Location( &data, &data_len ) == PKI_Common::SUCCESS )
					{
						if ( data_len < update_buff_size )
						{
							update_buff_size = data_len;
						}
						else
						{
							update_buff_size = max_update_buff_size;
						}
						while ( ( i < data_len ) && ( update_buff_size != 0U ) )
						{
							if ( PKI_Common::SUCCESS ==
								 sha256->Update( ( const_cast<uint8_t*>( data + i ) ),
												 update_buff_size ) )
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
								FUS_DEBUG_PRINT( DBG_MSG_ERROR,
												 "Data update failed during signature verification " );
								break;
							}
						}
						if ( update_failed_flag == false )
						{
							if ( PKI_Common::SUCCESS == sha256->Final( *calculated_hash ) )
							{
								ret_status = PKI_Common::SUCCESS;
							}
							else
							{
								FUS_DEBUG_PRINT( DBG_MSG_ERROR,
												 "Data hash calculation failed during signature verification" );
							}
						}
					}
					delete[] data_buf;
				}
			}
			else
			{
				FUS_DEBUG_PRINT( DBG_MSG_ERROR,
								 "Memory Allocation failed to store calculated Hash" )
			}
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		sha256->~Sha_Lib();
		delete( sha256 );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Fw_Store_Flash::Get_Cross_Sign_Status( void )
{
	bool ret = false;

	if ( m_code_sign_ptr != nullptr )
	{
		if ( m_code_sign_ptr->cross_sign_status == 1U )
		{
			ret = true;
		}
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Hash_Trusted( uint8_t** calculated_hash,
														   uint32_t* hash_size_out )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

	Sha_Lib* sha256 = new Sha256_Matrix();

	if ( sha256 != nullptr )
	{
		if ( PKI_Common::SUCCESS == sha256->Init() )
		{
			uint32_t hash_size = sha256->Size();
			*hash_size_out = hash_size;
			*calculated_hash = new uint8_t[hash_size];

			if ( *calculated_hash != nullptr )
			{
				uint32_t max_update_buff_size = MAX_ALLOCATE_BYTES;
				uint32_t update_buff_size = max_update_buff_size;
				uint32_t remaining_data_len = 0U;
				uint32_t i = 0U;
				bool update_failed_flag = false;
				uint8_t* data = nullptr;///< start of the firmware/data
				uint32_t data_len = 0U;
				uint8_t* data_buf = new uint8_t[max_update_buff_size];

				if ( data_buf != nullptr )
				{
					if ( Get_Fw_Location_Trusted( &data, &data_len ) == PKI_Common::SUCCESS )
					{
						if ( data_len < update_buff_size )
						{
							update_buff_size = data_len;
						}
						else
						{
							update_buff_size = max_update_buff_size;
						}
						while ( ( i < data_len ) && ( update_buff_size != 0U ) )
						{
							if ( PKI_Common::SUCCESS ==
								 sha256->Update( ( const_cast<uint8_t*>( data + i ) ),
												 update_buff_size ) )
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
								FUS_DEBUG_PRINT( DBG_MSG_ERROR,
												 "Data update failed during signature verification " );
								break;
							}
						}
						if ( update_failed_flag == false )
						{
							if ( PKI_Common::SUCCESS == sha256->Final( *calculated_hash ) )
							{
								ret_status = PKI_Common::SUCCESS;
							}
							else
							{
								FUS_DEBUG_PRINT( DBG_MSG_ERROR,
												 "Data hash calculation failed during signature verification" );
							}
						}
					}
					delete[] data_buf;
				}
			}
			else
			{
				FUS_DEBUG_PRINT( DBG_MSG_ERROR,
								 "Memory Allocation failed to store calculated Hash" );
			}
		}
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		sha256->~Sha_Lib();
		delete( sha256 );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Validate_Cross_Sign_CRC( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t calc_crc = 0U;
		uint16_t crc = 0U;
		uint32_t data_len = 0U;
		uint16_t trusted_fvk_cert_len = 0U;
		uint8_t* cross_sign_ptr = nullptr;
		uint16_t trusted_fw_sign_len = 0U;
		uint8_t* trusted_fw_sign_len_ptr = nullptr;
		uint32_t end_address = reinterpret_cast<uint32_t>( m_fw_ctrl ) + m_mem_cfg_len;

		cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );

		trusted_fvk_cert_len = ( ( *( cross_sign_ptr + 1 ) ) << 8 ) | ( *( cross_sign_ptr ) );
		if ( trusted_fvk_cert_len <= FVK_CERT_MAX_SIZE )
		{
			trusted_fw_sign_len_ptr = cross_sign_ptr + CERT_LEN_MAX_BYTES + trusted_fvk_cert_len;
			trusted_fw_sign_len = ( ( *( trusted_fw_sign_len_ptr + 1 ) ) << 8 ) |
				( *( trusted_fw_sign_len_ptr ) );

			if ( trusted_fw_sign_len <= FW_SIGN_MAX_SIZE )
			{
				data_len = CERT_LEN_MAX_BYTES + trusted_fvk_cert_len + SIGN_LEN_MAX_BYTES +
					trusted_fw_sign_len;

				if ( ( reinterpret_cast<uint32_t>( cross_sign_ptr ) >
					   ( reinterpret_cast<uint32_t>( m_fw_ctrl ) + m_fw_ctrl->f.data_length ) ) &&
					 ( reinterpret_cast<uint32_t>( cross_sign_ptr + data_len +
												   sizeof( uint16_t ) ) <=
					   end_address ) )
				{
					crc = *( reinterpret_cast<uint16_t*>( cross_sign_ptr + data_len ) );
					calc_crc =
						CRC16::Calc_On_String( cross_sign_ptr, data_len, 0U );

					if ( crc == calc_crc )
					{
						ret = PKI_Common::SUCCESS;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Extended Code Sign Info Block CRC Verified" );
					}
				}
				else
				{
					FUS_DEBUG_PRINT( DBG_MSG_ERROR,
									 "Extended Code Sign Info Block is out of range" );
				}
			}
		}
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Extended Code Sign Info Block CRC Verification Failed" );
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Fw_Location_Trusted( uint8_t** fw_location_out,
																  uint32_t* fw_package_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t trusted_fvk_cert_len = 0U;
		uint8_t* cross_sign_ptr = nullptr;

		cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );

		trusted_fvk_cert_len = ( ( *( cross_sign_ptr + 1 ) ) << 8 ) | ( *( cross_sign_ptr ) );

		if ( ( trusted_fvk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( trusted_fvk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( trusted_fvk_cert_len <= FVK_CERT_MAX_SIZE ) )
		{
			*fw_package_len_out = ( cross_sign_ptr - reinterpret_cast<uint8_t*>( m_fw_ctrl ) ) +
				CERT_LEN_MAX_BYTES + trusted_fvk_cert_len;
			*fw_location_out = reinterpret_cast<uint8_t*>( m_fw_ctrl );
			ret = PKI_Common::SUCCESS;
		}
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Trusted_FVK_Cert( uint8_t** fvk_cert_out,
															   uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint8_t* cross_sign_ptr = nullptr;

		cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );

		*fvk_cert_len_out = ( ( *( cross_sign_ptr + 1 ) ) << 8 ) | ( *( cross_sign_ptr ) );

		if ( ( *fvk_cert_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( *fvk_cert_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( *fvk_cert_len_out <= FVK_CERT_MAX_SIZE ) )
		{
			*fvk_cert_out = cross_sign_ptr + CERT_LEN_MAX_BYTES;
			FUS_DEBUG_PRINT( DBG_MSG_INFO,
							 "Cross Sign Info Block - Trusted FVK cert Read Successful" );
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted FVK Cert: %d", *fvk_cert_len_out );
			ret = PKI_Common::SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Cross Sign Info Block - Trusted FVK Key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Get_Trusted_Fw_Sign( uint8_t** fw_sign_out,
															  uint16_t* fw_sign_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t trusted_fvk_cert_len = 0U;
		uint8_t* trusted_fw_sign_len_ptr = nullptr;
		uint8_t* cross_sign_ptr = nullptr;

		cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );

		trusted_fvk_cert_len = ( ( *( cross_sign_ptr + 1 ) ) << 8 ) | ( *( cross_sign_ptr ) );

		if ( ( trusted_fvk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( trusted_fvk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( trusted_fvk_cert_len <= FVK_CERT_MAX_SIZE ) )
		{
			trusted_fw_sign_len_ptr = cross_sign_ptr + CERT_LEN_MAX_BYTES + trusted_fvk_cert_len;

			*fw_sign_len_out = ( ( *( trusted_fw_sign_len_ptr + 1 ) ) << 8 ) |
				( *( trusted_fw_sign_len_ptr ) );

			if ( ( *fw_sign_len_out != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
				 ( *fw_sign_len_out != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
				 ( *fw_sign_len_out <= FW_SIGN_MAX_SIZE ) )
			{
				*fw_sign_out = trusted_fw_sign_len_ptr + SIGN_LEN_MAX_BYTES;
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "Cross Sign Info Block - Trusted FW Signature Read Successful" );
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted Firmware Signature Length: %d",
								 *fw_sign_len_out );
				ret = PKI_Common::SUCCESS;
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Cross Sign Info Block - Trusted FW Signature Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_Flash::Free_Hash( uint8_t* calculated_hash )
{
	if ( calculated_hash != nullptr )
	{
		delete[] calculated_hash;
		calculated_hash = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Fw_Store_Flash::~Fw_Store_Flash()
{}
