/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Cert_Store_NV.h"
#include "StdLib_MV.h"
#include "CRC16.h"
#include "FUS_Debug.h"
#include "Babelfish_Assert.h"

using namespace BF_Lib;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Store_NV::Cert_Store_NV( NV_Ctrl* nv_ctrl,
							  const PKI_Common::nv_chip_config_t* mem_config ) :
	m_cert_store_ptr( reinterpret_cast<cert_store_struct_t*>( mem_config->start_address ) ),
	m_cert_store_len( mem_config->length ),
	m_nv_ctrl( nv_ctrl ),
	m_cert_store_ram_ptr( nullptr ),
	m_fvk_cert_len( 0U ),
	m_ivk_cert_len( 0U ),
	m_rvk_key_len( 0U ),
	m_fvk_key_len( 0U ),
	m_fvk_cert( nullptr ),
	m_ivk_cert( nullptr ),
	m_rvk_key( nullptr ),
	m_fvk_key( nullptr ),
#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
	m_ca_priv_key_len( 0U ),
	m_ca_cert_len( 0U ),
	m_device_key_len( 0U ),
	m_ca_priv_key( nullptr ),
	m_ca_cert( nullptr ),
	m_device_key( nullptr ),
#endif
	m_cert_store_exists( false )
{
	if ( m_nv_ctrl != nullptr )
	{
		if ( Cert_Store_Integrity_Check() == PKI_Common::SUCCESS )
		{
			m_cert_store_exists = true;

			if ( Get_Cert_Store_Elements() == PKI_Common::FAILURE )
			{
				FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store Memory Read Operation Failed" );
				BF_ASSERT( false );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_Cert_Store_Elements( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::SUCCESS;
	uint8_t data_len[CERT_LEN_MAX_BYTES] = {0};

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->fvk_cert_len ),
						  CERT_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_fvk_cert_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->ivk_cert_len ),
						  CERT_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_ivk_cert_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->rvk_key_len ),
						  KEY_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_rvk_key_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->fvk_key_len ),
						  KEY_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_fvk_key_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->ca_priv_key_len ),
						  KEY_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_ca_priv_key_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->ca_cert_len ),
						  CERT_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_ca_cert_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}

	if ( m_nv_ctrl->Read( data_len,
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->device_key_len ),
						  KEY_LEN_MAX_BYTES,
						  false ) == NV_Ctrl::SUCCESS )
	{
		m_device_key_len = ( data_len[1] << 8 ) | data_len[0];
	}
	else
	{
		ret = PKI_Common::FAILURE;
	}
#endif

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Cert_Store_Integrity_Check( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;
	bool header_verified = false;

	/* Verify header ID */
	uint16_t header_id = 0U;

	if ( NV_Ctrl::SUCCESS ==
		 m_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &header_id ),
						  reinterpret_cast<uint32_t>( &m_cert_store_ptr->header_id ),
						  sizeof( m_cert_store_ptr->header_id ),
						  false ) )
	{
		if ( header_id == CERT_STORE_HEADER_IDENTIFIER )
		{
			header_verified = true;
		}
	}

	if ( header_verified == true )
	{
		/* Read cert store crc */
		uint16_t cert_store_crc = 0U;
		if ( NV_Ctrl::SUCCESS ==
			 m_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &cert_store_crc ),
							  reinterpret_cast<uint32_t>( &m_cert_store_ptr->crc ),
							  sizeof( m_cert_store_ptr->crc ),
							  false ) )
		{
			/* Calculate the crc of cert store data and compare with read crc */
			uint16_t data_len = sizeof( cert_store_struct_t ) -
				sizeof( m_cert_store_ptr->header_id ) - sizeof( m_cert_store_ptr->crc );
			uint16_t calc_crc = 0U;
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
						reinterpret_cast<uint32_t>( &m_cert_store_ptr->rvk_key_len );
					uint16_t data_offset = 0U;
					uint16_t max_data_len = data_len;
					while ( data_offset < max_data_len )
					{

						if ( NV_Ctrl::SUCCESS ==
							 m_nv_ctrl->Read( data,
											  reinterpret_cast<uint32_t>( data_addr +
																		  data_offset ),
											  read_len, false ) )
						{
							calc_crc = CRC16::Calc_On_String( data, read_len, init_crc );
							init_crc = calc_crc;
						}
						else
						{
							BF_ASSERT( false );
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
					if ( cert_store_crc == calc_crc )
					{
						ret = PKI_Common::SUCCESS;
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC Verified" );
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC: %d", cert_store_crc );
					}
					else
					{
						FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC Verification failed" );
					}
					delete[] data;
				}
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert store header verification Failed" );
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert store integrity check Failed" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_CRC_Buf( uint16_t* crc_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

	if ( cert_info != nullptr )
	{

		uint16_t calculated_crc = 0U;

		calculated_crc =
			CRC16::Calc_On_String( reinterpret_cast<uint8_t*>( &cert_info->rvk_key_len ),
								   ( sizeof( cert_store_struct_t ) -
									 sizeof( cert_info->crc ) -
									 sizeof( cert_info->header_id ) ), 0U );
		*crc_out = calculated_crc;
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "CRC to be updated in Cert Store : %d", *crc_out );
		ret = PKI_Common::SUCCESS;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_RVK_Key( uint8_t** rvk_key_out,
													 uint16_t* rvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_rvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_rvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_rvk_key_len <= RVK_KEY_MAX_SIZE ) )
	{
		if ( m_rvk_key == nullptr )
		{
			m_rvk_key = new uint8_t[m_rvk_key_len];

			if ( m_rvk_key != nullptr )
			{
				if ( m_nv_ctrl->Read( m_rvk_key,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->rvk_key ),
									  m_rvk_key_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*rvk_key_len_out = m_rvk_key_len;
					*rvk_key_out = m_rvk_key;
					FUS_DEBUG_PRINT( DBG_MSG_INFO,
									 "Cert Store Block - RVK key Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "RVK Key Length: %d", m_rvk_key_len );
					ret = PKI_Common::SUCCESS;
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
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store Block - RVK key Read Failure" );
		ret = PKI_Common::DATA_ERROR;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_FVK_Cert( uint8_t** fvk_cert_out,
													  uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_fvk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_fvk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_fvk_cert_len <= FVK_CERT_MAX_SIZE ) )
	{
		if ( m_fvk_cert == nullptr )
		{
			m_fvk_cert = new uint8_t[m_fvk_cert_len];

			if ( m_fvk_cert != nullptr )
			{
				if ( m_nv_ctrl->Read( m_fvk_cert,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->fvk_cert ),
									  m_fvk_cert_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*fvk_cert_len_out = m_fvk_cert_len;
					*fvk_cert_out = m_fvk_cert;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - FVK Cert Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "fvk cert Length: %d", m_fvk_cert_len );
					ret = PKI_Common::SUCCESS;
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
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - FVK Cert Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_IVK_Cert( uint8_t** ivk_cert_out,
													  uint16_t* ivk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_ivk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_ivk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_ivk_cert_len <= IVK_CERT_MAX_SIZE ) )
	{
		if ( m_ivk_cert == nullptr )
		{
			m_ivk_cert = new uint8_t[m_ivk_cert_len];

			if ( m_ivk_cert != nullptr )
			{
				if ( m_nv_ctrl->Read( m_ivk_cert,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->ivk_cert ),
									  m_ivk_cert_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*ivk_cert_len_out = m_ivk_cert_len;
					*ivk_cert_out = m_ivk_cert;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - IVK Cert Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "IVK cert Length: %d", m_ivk_cert_len );
					ret = PKI_Common::SUCCESS;
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
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - IVK Cert Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_FVK_Key( uint8_t** fvk_key_out,
													 uint16_t* fvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_fvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_fvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_fvk_key_len <= FVK_KEY_MAX_SIZE ) )
	{
		if ( m_fvk_key == nullptr )
		{
			m_fvk_key = new uint8_t[m_fvk_key_len];

			if ( m_fvk_key != nullptr )
			{
				if ( m_nv_ctrl->Read( m_fvk_key,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->fvk_key ),
									  m_fvk_key_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*fvk_key_len_out = m_fvk_key_len;
					*fvk_key_out = m_fvk_key;
					FUS_DEBUG_PRINT( DBG_MSG_INFO,
									 "Cert Store Block - fvk key Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "fvk Key Length: %d", m_fvk_key_len );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
		else
		{
			*fvk_key_len_out = m_fvk_key_len;
			*fvk_key_out = m_fvk_key;
			/* fvk key is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store Block - fvk key Read Failure" );
		ret = PKI_Common::DATA_ERROR;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Store_NV::cert_store_struct_t* Cert_Store_NV::Get_Cert_Store_Ram_Ptr( void )
{
	/* ram is allocated only when need to update cert store */
	if ( m_cert_store_ram_ptr == nullptr )
	{
		m_cert_store_ram_ptr = new cert_store_struct_t;

		if ( m_cert_store_ram_ptr != nullptr )
		{
			if ( m_cert_store_exists == true )
			{
				if ( m_nv_ctrl->Read( reinterpret_cast<uint8_t*>( m_cert_store_ram_ptr ),
									  reinterpret_cast<uint32_t>( m_cert_store_ptr ),
									  sizeof( cert_store_struct_t ),
									  FALSE ) != NV_Ctrl::SUCCESS )
				{
					BF_ASSERT( false );
				}
			}
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store Memory Allocation Failed" );
		}
	}
	return ( m_cert_store_ram_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Update_Header_ID( void )
{
	cert_store_struct_t* cert_info = nullptr;

	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	cert_info = Get_Cert_Store_Ram_Ptr();
	if ( cert_info != nullptr )
	{
		cert_info->header_id = CERT_STORE_HEADER_IDENTIFIER;
		ret = PKI_Common::SUCCESS;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Update_CRC_Buf( void )
{

	uint16_t calculated_crc = 0U;
	cert_store_struct_t* cert_info = nullptr;

	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	/* update crc in ram buffer */

	if ( Get_CRC_Buf( &calculated_crc ) == PKI_Common::SUCCESS )
	{
		cert_info = Get_Cert_Store_Ram_Ptr();
		if ( cert_info != nullptr )
		{
			cert_info->crc = calculated_crc;
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
PKI_Common::pki_status_t Cert_Store_NV::Cert_Store_Write( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( Update_Header_ID() == PKI_Common::SUCCESS )
	{
		/* Update_CRC_Buf() will return failure if m_cert_store_ram_ptr  is nullptr */
		if ( Update_CRC_Buf() == PKI_Common::SUCCESS )
		{
			if ( m_nv_ctrl != nullptr )
			{
				m_cert_store_exists = false;
				if ( NV_Ctrl::SUCCESS ==
					 m_nv_ctrl->Erase( reinterpret_cast<uint32_t>( m_cert_store_ptr ),
									   m_cert_store_len ) )
				{
					if ( m_nv_ctrl->Write( reinterpret_cast<uint8_t*>( m_cert_store_ram_ptr ),
										   reinterpret_cast<uint32_t>( m_cert_store_ptr ),
										   sizeof( cert_store_struct_t ),
										   FALSE ) == NV_Ctrl::SUCCESS )
					{
						m_cert_store_exists = true;
						delete m_cert_store_ram_ptr;
						m_cert_store_ram_ptr = nullptr;
						ret = PKI_Common::SUCCESS;
						if ( Get_Cert_Store_Elements() == PKI_Common::FAILURE )
						{
							FUS_DEBUG_PRINT( DBG_MSG_ERROR,
											 "Cert Store Memory Read Operation Failed" );
							BF_ASSERT( false );
						}
					}
				}
				else
				{
					FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store Memory Erase Operation Failed" );
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
bool Cert_Store_NV::Is_RVK_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_rvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_rvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
		{
			ret = true;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "RVK Key Available" );
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "RVK Key Not Available" );
		}
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Update_RVK_Key_Buf( uint8_t* key_mem,
															uint16_t key_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( key_mem_len <= RVK_KEY_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->rvk_key_len, 0U, ( sizeof( cert_info->rvk_key_len ) ) );
			memset( &cert_info->rvk_key, 0U, ( sizeof( cert_info->rvk_key ) ) );

			if ( Copy_Data( cert_info->rvk_key, key_mem_len,
							key_mem, key_mem_len ) == true )
			{
				cert_info->rvk_key_len = key_mem_len;
				ret = PKI_Common::SUCCESS;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "RVK key updated in local cert store buffer" );
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
PKI_Common::pki_status_t Cert_Store_NV::Update_IVK_Cert_Buf( uint8_t* cert_mem,
															 uint16_t cert_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( cert_mem_len <= IVK_CERT_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->ivk_cert_len, 0U, ( sizeof( cert_info->ivk_cert_len ) ) );
			memset( &cert_info->ivk_cert, 0U, ( sizeof( cert_info->ivk_cert ) ) );

			if ( Copy_Data( cert_info->ivk_cert, cert_mem_len,
							cert_mem, cert_mem_len ) == true )
			{
				cert_info->ivk_cert_len = cert_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "IVK Cert updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
PKI_Common::pki_status_t Cert_Store_NV::Update_FVK_Cert_Buf( uint8_t* cert_mem,
															 uint16_t cert_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( cert_mem_len <= FVK_CERT_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->fvk_cert_len, 0U, ( sizeof( cert_info->fvk_cert_len ) ) );
			memset( &cert_info->fvk_cert, 0U, ( sizeof( cert_info->fvk_cert ) ) );

			if ( Copy_Data( cert_info->fvk_cert, cert_mem_len,
							cert_mem, cert_mem_len ) == true )
			{
				cert_info->fvk_cert_len = cert_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "FVK Cert updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
PKI_Common::pki_status_t Cert_Store_NV::Update_FVK_Key_Buf( uint8_t* key_mem,
															uint16_t key_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( key_mem_len <= FVK_KEY_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->fvk_key_len, 0U, ( sizeof( cert_info->fvk_key_len ) ) );
			memset( &cert_info->fvk_key, 0U, ( sizeof( cert_info->fvk_key ) ) );

			if ( Copy_Data( cert_info->fvk_key, key_mem_len,
							key_mem, key_mem_len ) == true )
			{
				cert_info->fvk_key_len = key_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "FVK key updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
PKI_Common::pki_status_t Cert_Store_NV::Free_RVK_Key( void )
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
PKI_Common::pki_status_t Cert_Store_NV::Free_FVK_Cert( void )
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
PKI_Common::pki_status_t Cert_Store_NV::Free_IVK_Cert( void )
{
	if ( m_ivk_cert != nullptr )
	{
		delete[] m_ivk_cert;
		m_ivk_cert = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_CA_Cert( uint8_t** ca_cert_out,
													 uint32_t* ca_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_ca_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_ca_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_ca_cert_len <= CA_CERT_MAX_SIZE ) )
	{

		if ( m_ca_cert == nullptr )
		{
			m_ca_cert = new uint8_t[m_ca_cert_len];

			if ( m_ca_cert != nullptr )
			{
				if ( m_nv_ctrl->Read( m_ca_cert,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->ca_cert ),
									  m_ca_cert_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*ca_cert_len_out = m_ca_cert_len;
					*ca_cert_out = m_ca_cert;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - CA Cert Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "CA cert Length: %d", m_ca_cert_len );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
		else
		{
			*ca_cert_len_out = m_ca_cert_len;
			*ca_cert_out = m_ca_cert;
			/* CA cert is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA Cert Read Failure" );
		ret = PKI_Common::DATA_ERROR;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_CA_Key( uint8_t** ca_key_out,
													uint16_t* ca_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_ca_priv_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_ca_priv_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_ca_priv_key_len <= CA_KEY_MAX_SIZE ) )
	{

		if ( m_ca_priv_key == nullptr )
		{
			m_ca_priv_key = new uint8_t[m_ca_priv_key_len];

			if ( m_ca_priv_key != nullptr )
			{
				if ( m_nv_ctrl->Read( m_ca_priv_key,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->ca_priv_key ),
									  m_ca_priv_key_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*ca_key_len_out = m_ca_priv_key_len;
					*ca_key_out = m_ca_priv_key;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - CA Key Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "CA Key Length: %d", m_ca_priv_key_len );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
		else
		{
			*ca_key_len_out = m_ca_priv_key_len;
			*ca_key_out = m_ca_priv_key;
			/* CA Key is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA Key Read Failure" );
		ret = PKI_Common::DATA_ERROR;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Get_Dev_Key( uint8_t** dev_key_out,
													 uint16_t* dev_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_exists == true ) &&
		 ( m_device_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
		 ( m_device_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
		 ( m_device_key_len <= DEV_KEY_MAX_SIZE ) )
	{
		if ( m_device_key == nullptr )
		{
			m_device_key = new uint8_t[m_device_key_len];

			if ( m_device_key != nullptr )
			{
				if ( m_nv_ctrl->Read( m_device_key,
									  reinterpret_cast<uint32_t>( m_cert_store_ptr->device_key ),
									  m_device_key_len,
									  false ) == NV_Ctrl::SUCCESS )
				{
					*dev_key_len_out = m_device_key_len;
					*dev_key_out = m_device_key;
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - Device Key Read Successful" );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Device Key Length: %d", m_device_key_len );
					ret = PKI_Common::SUCCESS;
				}
			}
		}
		else
		{
			*dev_key_len_out = m_device_key_len;
			*dev_key_out = m_device_key;
			/* CA cert is already available in the buffer */
			ret = PKI_Common::SUCCESS;
		}
	}

	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - Device Key Read Failure" );
		ret = PKI_Common::DATA_ERROR;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Update_CA_Cert_Buf( uint8_t* cert_mem,
															uint32_t cert_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( cert_mem_len <= CA_CERT_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->ca_cert_len, 0U, ( sizeof( cert_info->ca_cert_len ) ) );
			memset( &cert_info->ca_cert, 0U, ( sizeof( cert_info->ca_cert ) ) );

			if ( Copy_Data( cert_info->ca_cert, cert_mem_len,
							cert_mem, cert_mem_len ) == true )
			{
				cert_info->ca_cert_len = cert_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "CA Cert updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
PKI_Common::pki_status_t Cert_Store_NV::Update_CA_Key_Buf( uint8_t* key_mem,
														   uint16_t key_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( key_mem_len <= CA_KEY_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->ca_priv_key_len, 0U, ( sizeof( cert_info->ca_priv_key_len ) ) );
			memset( &cert_info->ca_priv_key, 0U, ( sizeof( cert_info->ca_priv_key ) ) );

			if ( Copy_Data( cert_info->ca_priv_key, key_mem_len,
							key_mem, key_mem_len ) == true )
			{
				cert_info->ca_priv_key_len = key_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "CA Key updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
PKI_Common::pki_status_t Cert_Store_NV::Update_Dev_Key_Buf( uint8_t* key_mem,
															uint16_t key_mem_len )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( key_mem_len <= DEV_KEY_MAX_SIZE )
	{
		cert_store_struct_t* cert_info = Get_Cert_Store_Ram_Ptr();

		if ( cert_info != nullptr )
		{
			memset( &cert_info->device_key_len, 0U, ( sizeof( cert_info->device_key_len ) ) );
			memset( &cert_info->device_key, 0U, ( sizeof( cert_info->device_key ) ) );

			if ( Copy_Data( cert_info->device_key, key_mem_len,
							key_mem, key_mem_len ) == true )
			{
				cert_info->device_key_len = key_mem_len;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Device Key updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
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
bool Cert_Store_NV::Is_CA_Cert_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_ca_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_ca_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
bool Cert_Store_NV::Is_CA_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_ca_priv_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_ca_priv_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
bool Cert_Store_NV::Is_Dev_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_device_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_device_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
PKI_Common::pki_status_t Cert_Store_NV::Free_CA_Cert( void )
{
	if ( m_ca_cert != nullptr )
	{
		delete[] m_ca_cert;
		m_ca_cert = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Free_CA_Key( void )
{
	if ( m_ca_priv_key != nullptr )
	{
		delete[] m_ca_priv_key;
		m_ca_priv_key = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_NV::Free_Dev_Key( void )
{
	if ( m_device_key != nullptr )
	{
		delete[] m_device_key;
		m_device_key = nullptr;
	}
	return ( PKI_Common::SUCCESS );
}

#endif	// PKI_CERT_GEN_SUPPORT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Store_NV::~Cert_Store_NV( void )
{}
