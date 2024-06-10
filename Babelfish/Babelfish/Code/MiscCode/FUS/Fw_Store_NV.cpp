/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Fw_Store_NV.h"
#include "CRC16.h"
#include "FUS_Debug.h"
#include "Babelfish_Assert.h"
#ifdef ESP32_SETUP
#include "esp_ota_ops.h"
#include "uC_Fw_Code_Range.h"
#include "uC_Code_Range.h"
#endif

using namespace BF_Lib;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Fw_Store_NV::Fw_Store_NV( const PKI_Common::nv_chip_config_t* mem_config, NV_Ctrl* fw_nv_ctrl,
						  callback_t cb, bool* integrity_status ) :
	m_fw_config_addr( reinterpret_cast<real_fw_header*>( mem_config->start_address ) ),
	m_fw_config_len( mem_config->length ),
	m_fw_nv_ctrl( fw_nv_ctrl ),
	m_code_sign_ptr( nullptr ),
	m_callback_func( cb ),
	m_fvk_cert_len( 0U ),
	m_fvk_key_len( 0U ),
	m_ivk_cert_len( 0U ),
	m_rvk_key_len( 0U ),
	m_fw_sign_len( 0U ),
	m_fvk_cert( nullptr ),
	m_fvk_key( nullptr ),
	m_ivk_cert( nullptr ),
	m_rvk_key( nullptr ),
	m_fw_sign( nullptr )
{
	*integrity_status = false;

	if ( ( m_fw_nv_ctrl != nullptr ) && ( m_fw_config_len != FLASH_MEMORY_ZERO_VALUE_U32 ) )
	{
		real_fw_header* fw_config_addr = m_fw_config_addr;
#ifdef ESP32_SETUP
		/*
		 * Offset for PXGreen header is = 24+ 8 + 256 = 288.
		 * sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)
		 */
		NV_Ctrl::mem_range_info_t mem_range = { };
		m_fw_nv_ctrl->Get_Memory_Info( &mem_range );
		uint32_t active_ota_address = reinterpret_cast<uint32_t>( uC_App_Fw_Start_Address() );
		uint32_t passive_ota_address = reinterpret_cast<uint32_t>( uC_Scratch_Area_Start_Address() );

		if ( ( mem_range.start_address == active_ota_address ) || ( mem_range.start_address == passive_ota_address ) )
		{
			fw_config_addr = reinterpret_cast<real_fw_header*>( reinterpret_cast<uint8_t*>( m_fw_config_addr )
																+ sizeof( esp_image_header_t ) +
																sizeof( esp_image_segment_header_t ) +
																sizeof( esp_app_desc_t ) );
		}
#endif
		uint32_t code_sign_offset = 0U;
		if ( NV_Ctrl::SUCCESS ==
			 m_fw_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &code_sign_offset ),
								 reinterpret_cast<uint32_t>( &fw_config_addr->f.data_length ),
								 sizeof( uint32_t ), false ) )
		{
			m_code_sign_ptr =
				reinterpret_cast<code_sign_struct_t*>( reinterpret_cast<uint8_t*>( m_fw_config_addr ) +
													   code_sign_offset );
			if ( m_code_sign_ptr != nullptr )
			{
				if ( Code_Sign_Info_Integrity_Check() == PKI_Common::SUCCESS )
				{
					*integrity_status = true;
					uint8_t data_len[CERT_LEN_MAX_BYTES] = { 0U };
					PKI_Common::pki_status_t ret = PKI_Common::SUCCESS;

					if ( m_fw_nv_ctrl->Read( data_len,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->
																		 fvk_cert_len ),
											 CERT_LEN_MAX_BYTES,
											 false ) == NV_Ctrl::SUCCESS )
					{
						m_fvk_cert_len = ( data_len[1] << 8 ) | data_len[0];
					}
					else
					{
						ret = PKI_Common::FAILURE;
					}

					if ( m_fw_nv_ctrl->Read( data_len,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->
																		 fvk_key_len ),
											 KEY_LEN_MAX_BYTES,
											 false ) == NV_Ctrl::SUCCESS )
					{
						m_fvk_key_len = ( data_len[1] << 8 ) | data_len[0];
					}
					else
					{
						ret = PKI_Common::FAILURE;
					}

					if ( m_fw_nv_ctrl->Read( data_len,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->
																		 ivk_cert_len ),
											 CERT_LEN_MAX_BYTES,
											 false ) == NV_Ctrl::SUCCESS )
					{
						m_ivk_cert_len = ( data_len[1] << 8 ) | data_len[0];
					}
					else
					{
						ret = PKI_Common::FAILURE;
					}

					if ( m_fw_nv_ctrl->Read( data_len,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->
																		 rvk_key_len ),
											 CERT_LEN_MAX_BYTES,
											 false ) == NV_Ctrl::SUCCESS )
					{
						m_rvk_key_len = ( data_len[1] << 8 ) | data_len[0];
					}
					else
					{
						ret = PKI_Common::FAILURE;
					}

					if ( m_fw_nv_ctrl->Read( data_len,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->
																		 fw_sign_len ),
											 CERT_LEN_MAX_BYTES,
											 false ) == NV_Ctrl::SUCCESS )
					{
						m_fw_sign_len = ( data_len[1] << 8 ) | data_len[0];
					}
					else
					{
						ret = PKI_Common::FAILURE;
					}

					BF_ASSERT( ret != PKI_Common::FAILURE );
				}
				else
				{
					m_code_sign_ptr = nullptr;
					FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block Validation Failed" );
				}
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Code_Sign_Info_Integrity_Check( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	uint8_t algo_type = PKI_Common::PKI_ECC;

	uint16_t data_len = reinterpret_cast<uint8_t*>( &m_code_sign_ptr->reserved ) -
		reinterpret_cast<uint8_t*>( &m_code_sign_ptr->identity_mark );

	code_sign_struct_t* code_sign_data =
		reinterpret_cast<code_sign_struct_t*>( new uint8_t[data_len] );

	if ( code_sign_data != nullptr )
	{
		if ( NV_Ctrl::SUCCESS ==
			 m_fw_nv_ctrl->Read( reinterpret_cast<uint8_t*>( code_sign_data ),
								 reinterpret_cast<uint32_t>( &m_code_sign_ptr->identity_mark ),
								 data_len, false ) )
		{
			if ( code_sign_data->identity_mark == CODE_SIGN_INFO_IDENTITY_MARK )
			{
				if ( code_sign_data->header_version_num == CODE_SIGN_INFO_HEADER_VERSION )
				{
					if ( code_sign_data->algo_type == algo_type )
					{
						if ( code_sign_data->ecc_curve_type == ECC_CURVE_TYPE )
						{
							if ( code_sign_data->key_size == KEY_SIZE )
							{
								if ( code_sign_data->hash_size == HASH_SIZE )
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
		}
		delete[] code_sign_data;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Validate_Code_Sign_Info_CRC( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	/* Read crc of code sign block */
	uint16_t code_sign_block_crc = 0U;

	if ( NV_Ctrl::SUCCESS ==
		 m_fw_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &code_sign_block_crc ),
							 reinterpret_cast<uint32_t>( &m_code_sign_ptr->crc ),
							 sizeof( m_code_sign_ptr->crc ),
							 false ) )
	{
		/* Calculate the crc of code sign block and compare with read crc */
		uint16_t calc_crc = 0U;
		uint16_t data_len = sizeof( code_sign_struct_t ) - sizeof( m_code_sign_ptr->crc );
		if ( data_len > 0U )
		{
			uint16_t read_len = 0U;
			uint16_t init_crc = 0U;

			if ( data_len < MAX_ALLOCATE_BYTES )
			{
				read_len = data_len;
			}
			else
			{
				read_len = MAX_ALLOCATE_BYTES;
			}

			uint8_t* data = new uint8_t[read_len] ();

			if ( data != nullptr )
			{
				uint32_t data_addr =
					reinterpret_cast<uint32_t>( &m_code_sign_ptr->identity_mark );
				uint16_t data_offset = 0U;
				uint16_t max_data_len = data_len;
				while ( data_offset < max_data_len )
				{

					if ( NV_Ctrl::SUCCESS ==
						 m_fw_nv_ctrl->Read( data,
											 reinterpret_cast<uint32_t>( data_addr +
																		 data_offset ),
											 read_len, false ) )
					{
						calc_crc = CRC16::Calc_On_String( data, read_len, init_crc );
						init_crc = calc_crc;
					}
					data_offset += read_len;
					data_len -= read_len;
					if ( data_len < MAX_ALLOCATE_BYTES )
					{
						read_len = data_len;
					}
					else
					{
						read_len = MAX_ALLOCATE_BYTES;
					}
				}
				if ( code_sign_block_crc == calc_crc )
				{
					ret = PKI_Common::SUCCESS;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Code Sign Info Block CRC Verified" );
				}
				delete[] data;
			}
		}
	}
	if ( ret != PKI_Common::SUCCESS )
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
PKI_Common::pki_status_t Fw_Store_NV::Get_Fw_Location( uint8_t** fw_location_out,
													   uint32_t* fw_package_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		*fw_location_out = reinterpret_cast<uint8_t*>( m_fw_config_addr );
		*fw_package_len_out = ( m_code_sign_ptr->fw_sign - reinterpret_cast<uint8_t*>( m_fw_config_addr ) );
		ret = PKI_Common::SUCCESS;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_FVK_Cert( uint8_t** fvk_cert_out,
													uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_fvk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_fvk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_fvk_cert_len <= FVK_CERT_MAX_SIZE ) )
	{
		if ( m_fvk_cert == nullptr )
		{
			if ( m_code_sign_ptr != nullptr )
			{
				m_fvk_cert = new uint8_t[m_fvk_cert_len];

				if ( m_fvk_cert != nullptr )
				{
					if ( m_fw_nv_ctrl->Read( m_fvk_cert,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->fvk_cert ),
											 m_fvk_cert_len,
											 false ) == NV_Ctrl::SUCCESS )
					{
						*fvk_cert_len_out = m_fvk_cert_len;
						*fvk_cert_out = m_fvk_cert;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Code Sign Info Block - fvk cert Read Successful" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "fvk cert Length: %d", m_fvk_cert_len );
						ret = PKI_Common::SUCCESS;
					}
				}
			}
		}
		else
		{
			*fvk_cert_len_out = m_fvk_cert_len;
			*fvk_cert_out = m_fvk_cert;
			/* FVK is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - fvk cert Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_FVK_Key( uint8_t** fvk_key_out,
												   uint16_t* fvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_fvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_fvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_fvk_key_len <= FVK_KEY_MAX_SIZE ) )
	{
		if ( m_fvk_key == nullptr )
		{
			if ( m_code_sign_ptr != nullptr )
			{
				m_fvk_key = new uint8_t[m_fvk_key_len];

				if ( m_fvk_key != nullptr )
				{
					if ( m_fw_nv_ctrl->Read( m_fvk_key,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->fvk_key ),
											 m_fvk_key_len,
											 false ) == NV_Ctrl::SUCCESS )
					{
						*fvk_key_len_out = m_fvk_key_len;
						*fvk_key_out = m_fvk_key;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Code Sign Info Block - fvk key Read Successful" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "fvk key Length: %d", m_fvk_key_len );
						ret = PKI_Common::SUCCESS;
					}
				}
			}
		}
		else
		{
			*fvk_key_len_out = m_fvk_key_len;
			*fvk_key_out = m_fvk_key;
			/* FVK is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - fvk key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_IVK_Cert( uint8_t** ivk_cert_out,
													uint16_t* ivk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_ivk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_ivk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_ivk_cert_len <= IVK_CERT_MAX_SIZE ) )
	{
		if ( m_ivk_cert == nullptr )
		{
			if ( m_code_sign_ptr != nullptr )
			{
				m_ivk_cert = new uint8_t[m_ivk_cert_len];

				if ( m_ivk_cert != nullptr )
				{
					if ( m_fw_nv_ctrl->Read( m_ivk_cert,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->ivk_cert ),
											 m_ivk_cert_len,
											 false ) == NV_Ctrl::SUCCESS )
					{
						*ivk_cert_len_out = m_ivk_cert_len;
						*ivk_cert_out = m_ivk_cert;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Code Sign Info Block - IVK cert Read Successful" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "IVK cert Length: %d", m_ivk_cert_len );
						ret = PKI_Common::SUCCESS;
					}
				}
			}
		}
		else
		{
			*ivk_cert_len_out = m_ivk_cert_len;
			*ivk_cert_out = m_ivk_cert;
			/* ivk is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - IVK cert Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_Fw_Sign( uint8_t** fw_sign_out,
												   uint16_t* fw_sign_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_fw_sign_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_fw_sign_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_fw_sign_len <= FW_SIGN_MAX_SIZE ) )
	{
		if ( m_fw_sign == nullptr )
		{
			if ( m_code_sign_ptr != nullptr )
			{
				m_fw_sign = new uint8_t[m_fw_sign_len];

				if ( m_fw_sign != nullptr )
				{
					if ( m_fw_nv_ctrl->Read( m_fw_sign,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->fw_sign ),
											 m_fw_sign_len,
											 false ) == NV_Ctrl::SUCCESS )
					{
						*fw_sign_len_out = m_fw_sign_len;
						*fw_sign_out = m_fw_sign;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Code Sign Info Block - FW Signature Read Successful" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "Firmware Signature Length: %d",
										 m_fw_sign_len );
						ret = PKI_Common::SUCCESS;
					}
				}
			}
		}
		else
		{
			*fw_sign_len_out = m_fw_sign_len;
			*fw_sign_out = m_fw_sign;
			/* Fw Sign is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - FW Signature Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_RVK_Key( uint8_t** rvk_key_out,
												   uint16_t* rvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_rvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_rvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_rvk_key_len <= RVK_KEY_MAX_SIZE ) )
	{
		if ( m_rvk_key == nullptr )
		{
			if ( m_code_sign_ptr != nullptr )
			{
				m_rvk_key = new uint8_t[m_rvk_key_len];

				if ( m_rvk_key != nullptr )
				{
					if ( m_fw_nv_ctrl->Read( m_rvk_key,
											 reinterpret_cast<uint32_t>( m_code_sign_ptr->rvk_key ),
											 m_rvk_key_len,
											 false ) == NV_Ctrl::SUCCESS )
					{
						*rvk_key_len_out = m_rvk_key_len;
						*rvk_key_out = m_rvk_key;
						FUS_DEBUG_PRINT( DBG_MSG_INFO,
										 "Code Sign Info Block - RVK key Read Successful" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "RVK Key Length: %d", m_rvk_key_len );
						ret = PKI_Common::SUCCESS;
					}
				}
			}
		}
		else
		{
			*rvk_key_len_out = m_rvk_key_len;
			*rvk_key_out = m_rvk_key;
			/* rvk key is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Code Sign Info Block - RVK key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_Hash( uint8_t** calculated_hash, uint32_t* hash_size_out )
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
				uint8_t* data_addr = nullptr;///< start of the firmware/data
				uint32_t data_len = 0U;
				uint8_t* data_buf = new uint8_t[max_update_buff_size] ();

				if ( data_buf != nullptr )
				{
					if ( Get_Fw_Location( &data_addr, &data_len ) == PKI_Common::SUCCESS )
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
							if ( NV_Ctrl::SUCCESS ==
								 m_fw_nv_ctrl->Read( data_buf,
													 reinterpret_cast<uint32_t>( data_addr + i ),
													 update_buff_size, false ) )
							{
								if ( PKI_Common::SUCCESS ==
									 sha256->Update( ( const_cast<uint8_t*>( data_buf ) ),
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
bool Fw_Store_NV::Get_Cross_Sign_Status( void )
{
	bool ret = false;

	if ( m_code_sign_ptr != nullptr )
	{
		uint8_t status = 0U;
		if ( NV_Ctrl::SUCCESS ==
			 m_fw_nv_ctrl->Read( &status,
								 reinterpret_cast<uint32_t>( &m_code_sign_ptr->cross_sign_status ),
								 sizeof( m_code_sign_ptr->cross_sign_status ), false ) )
		{
			if ( status == 1U )
			{
				ret = true;
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
PKI_Common::pki_status_t Fw_Store_NV::Get_Hash_Trusted( uint8_t** calculated_hash,
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
				uint8_t* data_addr = nullptr;///< start of the firmware/data
				uint32_t data_len = 0U;
				uint8_t* data_buf = new uint8_t[max_update_buff_size] ();

				if ( data_buf != nullptr )
				{
					if ( Get_Fw_Location_Trusted( &data_addr, &data_len ) == PKI_Common::SUCCESS )
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
							if ( NV_Ctrl::SUCCESS ==
								 m_fw_nv_ctrl->Read( data_buf,
													 reinterpret_cast<uint32_t>( data_addr + i ),
													 update_buff_size, false ) )
							{
								if ( PKI_Common::SUCCESS ==
									 sha256->Update( ( const_cast<uint8_t*>( data_buf ) ),
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
PKI_Common::pki_status_t Fw_Store_NV::Get_Trusted_FVK_Cert_Len( uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );

		uint8_t data_len[CERT_LEN_MAX_BYTES] = { 0U };
		uint16_t cert_len = 0U;
		if ( m_fw_nv_ctrl->Read( data_len,
								 reinterpret_cast<uint32_t>( cross_sign_ptr ),
								 CERT_LEN_MAX_BYTES,
								 false ) == NV_Ctrl::SUCCESS )
		{
			cert_len = ( data_len[1] << 8 ) | data_len[0];

			if ( ( cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
				 ( cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
				 ( cert_len <= FVK_CERT_MAX_SIZE ) )
			{
				*fvk_cert_len_out = cert_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "Cross Sign Info Block - trusted fvk cert length Read Successful" );
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted fvk cert Length: %d",
								 *fvk_cert_len_out );
				ret = PKI_Common::SUCCESS;
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Cross Sign Info Block - Trusted FVK cert lenght Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_Trusted_Lenghts( uint16_t* fvk_cert_len_out,
														   uint16_t* fw_sign_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
			sizeof( code_sign_struct_t );
		uint8_t data_len[CERT_LEN_MAX_BYTES] = { 0U };
		uint16_t trusted_fvk_cert_len = 0U;

		if ( Get_Trusted_FVK_Cert_Len( &trusted_fvk_cert_len ) == PKI_Common::SUCCESS )
		{
			*fvk_cert_len_out = trusted_fvk_cert_len;
			uint16_t trusted_fw_sign_len = 0U;
			if ( m_fw_nv_ctrl->Read( data_len,
									 reinterpret_cast<uint32_t>( cross_sign_ptr +
																 CERT_LEN_MAX_BYTES +
																 trusted_fvk_cert_len ),
									 SIGN_LEN_MAX_BYTES, false ) == NV_Ctrl::SUCCESS )
			{
				trusted_fw_sign_len = ( data_len[1] << 8 ) | data_len[0];

				if ( ( trusted_fw_sign_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
					 ( trusted_fw_sign_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
					 ( trusted_fw_sign_len <= FW_SIGN_MAX_SIZE ) )
				{
					*fw_sign_len_out = trusted_fw_sign_len;
					FUS_DEBUG_PRINT( DBG_MSG_INFO,
									 "Cross Sign Info Block - trusted FW Signature length Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted FW Signature Length: %d",
									 *fw_sign_len_out );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Cross Sign Info Block - Trusted FW Signature length Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Validate_Cross_Sign_CRC( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t sign_len = 0U;
		uint16_t cert_len = 0U;

		if ( Get_Trusted_Lenghts( &cert_len, &sign_len ) == PKI_Common::SUCCESS )
		{
			uint16_t crc = 0U;
			uint32_t end_address = reinterpret_cast<uint32_t>( m_fw_config_addr ) + m_fw_config_len;

			uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
				sizeof( code_sign_struct_t );

			uint16_t crc_calc_data_len = CERT_LEN_MAX_BYTES + cert_len +
				SIGN_LEN_MAX_BYTES + sign_len;

			if ( ( reinterpret_cast<uint32_t>( cross_sign_ptr ) >
				   reinterpret_cast<uint32_t>( m_fw_config_addr ) ) &&
				 ( reinterpret_cast<uint32_t>( cross_sign_ptr + crc_calc_data_len +
											   sizeof( uint16_t ) ) <=
				   end_address ) )
			{
				if ( NV_Ctrl::SUCCESS ==
					 m_fw_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &crc ),
										 reinterpret_cast<uint32_t>( cross_sign_ptr +
																	 crc_calc_data_len ),
										 sizeof( uint16_t ),
										 false ) )
				{
					/* Calculate the crc of code sign block and compare with read crc */
					uint16_t calc_crc = 0U;
					if ( crc_calc_data_len > 0U )
					{
						uint16_t read_len = 0U;
						uint16_t init_crc = 0U;

						if ( crc_calc_data_len < MAX_ALLOCATE_BYTES )
						{
							read_len = crc_calc_data_len;
						}
						else
						{
							read_len = MAX_ALLOCATE_BYTES;
						}

						uint8_t* data = new uint8_t[read_len] ();

						if ( data != nullptr )
						{
							uint16_t data_offset = 0U;
							uint16_t max_data_len = crc_calc_data_len;
							while ( data_offset < max_data_len )
							{

								if ( NV_Ctrl::SUCCESS ==
									 m_fw_nv_ctrl->Read( data,
														 reinterpret_cast<uint32_t>(
															 cross_sign_ptr +
															 data_offset ),
														 read_len, false ) )
								{
									calc_crc = CRC16::Calc_On_String( data, read_len,
																	  init_crc );
									init_crc = calc_crc;
								}
								data_offset += read_len;
								crc_calc_data_len -= read_len;
								if ( crc_calc_data_len < MAX_ALLOCATE_BYTES )
								{
									read_len = crc_calc_data_len;
								}
								else
								{
									read_len = MAX_ALLOCATE_BYTES;
								}
							}
							if ( crc == calc_crc )
							{
								ret = PKI_Common::SUCCESS;
								FUS_DEBUG_PRINT( DBG_MSG_INFO,
												 "Extended Code Sign Info Block CRC Verified" );
							}
							delete[] data;
						}
					}
				}
			}
			else
			{
				FUS_DEBUG_PRINT( DBG_MSG_ERROR,
								 "Extended Code Sign Info Block is out of range" );
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
PKI_Common::pki_status_t Fw_Store_NV::Get_Fw_Location_Trusted( uint8_t** fw_location_out,
															   uint32_t* fw_package_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t cert_len = 0U;

		if ( Get_Trusted_FVK_Cert_Len( &cert_len ) == PKI_Common::SUCCESS )
		{
			uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
				sizeof( code_sign_struct_t );

			*fw_package_len_out =
				( cross_sign_ptr - reinterpret_cast<uint8_t*>( m_fw_config_addr ) ) +
				CERT_LEN_MAX_BYTES + cert_len;
			*fw_location_out = reinterpret_cast<uint8_t*>( m_fw_config_addr );
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
PKI_Common::pki_status_t Fw_Store_NV::Get_Trusted_FVK_Cert( uint8_t** fvk_cert_out,
															uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t cert_len = 0U;

		if ( Get_Trusted_FVK_Cert_Len( &cert_len ) == PKI_Common::SUCCESS )
		{
			*fvk_cert_out = new uint8_t[cert_len];

			if ( *fvk_cert_out != nullptr )
			{
				uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
					sizeof( code_sign_struct_t );

				if ( m_fw_nv_ctrl->Read( *fvk_cert_out,
										 reinterpret_cast<uint32_t>( cross_sign_ptr +
																	 CERT_LEN_MAX_BYTES ),
										 cert_len,
										 false ) == NV_Ctrl::SUCCESS )
				{
					*fvk_cert_len_out = cert_len;
					FUS_DEBUG_PRINT( DBG_MSG_INFO,
									 "Cross Sign Info Block - trusted fvk cert Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted fvk cert Length: %d",
									 *fvk_cert_len_out );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR,
						 "Cross Sign Info Block - Trusted FVK cert Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Get_Trusted_Fw_Sign( uint8_t** fw_sign_out,
														   uint16_t* fw_sign_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( m_code_sign_ptr != nullptr )
	{
		uint16_t sign_len = 0U;
		uint16_t cert_len = 0U;

		if ( Get_Trusted_Lenghts( &cert_len, &sign_len ) == PKI_Common::SUCCESS )
		{
			*fw_sign_out = new uint8_t[sign_len];

			if ( *fw_sign_out != nullptr )
			{
				uint8_t* cross_sign_ptr = reinterpret_cast<uint8_t*>( m_code_sign_ptr ) +
					sizeof( code_sign_struct_t );
				if ( m_fw_nv_ctrl->Read( *fw_sign_out,
										 reinterpret_cast<uint32_t>( cross_sign_ptr +
																	 CERT_LEN_MAX_BYTES + cert_len +
																	 SIGN_LEN_MAX_BYTES ),
										 sign_len,
										 false ) == NV_Ctrl::SUCCESS )
				{
					*fw_sign_len_out = sign_len;
					FUS_DEBUG_PRINT( DBG_MSG_INFO,
									 "Cross Sign Info Block - trusted FW Signature Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Trusted FW Signature Length: %d",
									 *fw_sign_len_out );
					ret = PKI_Common::SUCCESS;
				}
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
PKI_Common::pki_status_t Fw_Store_NV::Free_Trusted_FVK_Cert( uint8_t* cert )
{
	if ( cert != nullptr )
	{
		delete[] cert;
		cert = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_Trusted_Fw_Sign( uint8_t* sign )
{
	if ( sign != nullptr )
	{
		delete[] sign;
		sign = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_Hash( uint8_t* calculated_hash )
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
PKI_Common::pki_status_t Fw_Store_NV::Free_FVK_Cert( void )
{
	if ( m_fvk_cert != nullptr )
	{
		delete[] m_fvk_cert;
		m_fvk_cert = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_FVK_Key( void )
{
	if ( m_fvk_key != nullptr )
	{
		delete[] m_fvk_key;
		m_fvk_key = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_IVK_Cert( void )
{
	if ( m_ivk_cert != nullptr )
	{
		delete[] m_ivk_cert;
		m_ivk_cert = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_Fw_Sign( void )
{
	if ( m_fw_sign != nullptr )
	{
		delete[] m_fw_sign;
		m_fw_sign = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Fw_Store_NV::Free_RVK_Key( void )
{
	if ( m_rvk_key != nullptr )
	{
		delete[] m_rvk_key;
		m_rvk_key = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Fw_Store_NV::~Fw_Store_NV()
{
	Free_FVK_Cert();
	Free_FVK_Key();
	Free_IVK_Cert();
	Free_RVK_Key();
	Free_Fw_Sign();
}
