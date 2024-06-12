/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Cert_Store_Flash.h"
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
Cert_Store_Flash::Cert_Store_Flash( NV_Ctrl* nv_ctrl,
									const PKI_Common::nv_chip_config_t* mem_config ) :
	m_cert_store_ptr( reinterpret_cast<cert_store_struct_t*>( mem_config->start_address ) ),
	m_cert_store_len( mem_config->length ),
	m_nv_ctrl( nv_ctrl ),
	m_cert_store_ram_ptr( nullptr ),
	m_cert_store_exists( false )
{
	if ( ( m_cert_store_ptr != nullptr ) && ( m_nv_ctrl != nullptr ) )
	{
		if ( Cert_Store_Integrity_Check() == PKI_Common::SUCCESS )
		{
			m_cert_store_exists = true;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Cert_Store_Integrity_Check( void )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_nv_ctrl != nullptr ) )
	{
		/* Verify header ID */
		if ( m_cert_store_ptr->header_id == CERT_STORE_HEADER_IDENTIFIER )
		{
			/* Calculate the crc of cert store data and compare with cert store crc */
			uint16_t calculated_crc = 0U;

			calculated_crc =
				CRC16::Calc_On_String( reinterpret_cast<uint8_t*>( &m_cert_store_ptr->rvk_key_len ),
									   ( sizeof( cert_store_struct_t ) -
										 sizeof( m_cert_store_ptr->crc ) -
										 sizeof( m_cert_store_ptr->header_id ) ), 0U );


			if ( m_cert_store_ptr->crc == calculated_crc )
			{
				ret = PKI_Common::SUCCESS;
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC verified" );
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC: %d", m_cert_store_ptr->crc );
			}
			else
			{
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert store CRC verification failed" );
			}

		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert store header verification Failed" );
		}
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
PKI_Common::pki_status_t Cert_Store_Flash::Get_CRC_Buf( uint16_t* crc_out )
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
PKI_Common::pki_status_t Cert_Store_Flash::Get_RVK_Key( uint8_t** rvk_key_out,
														uint16_t* rvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->rvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->rvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->rvk_key_len <= RVK_KEY_MAX_SIZE ) )
		{
			*rvk_key_out = m_cert_store_ptr->rvk_key;
			*rvk_key_len_out = m_cert_store_ptr->rvk_key_len;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - RVK Key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			/* rvk key is not available and return error */
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - Not available" );
			ret = PKI_Common::DATA_ERROR;
		}
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - RVK Key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Get_FVK_Cert( uint8_t** fvk_cert_out,
														 uint16_t* fvk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->fvk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->fvk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->fvk_cert_len <= FVK_CERT_MAX_SIZE ) )
		{
			*fvk_cert_len_out = m_cert_store_ptr->fvk_cert_len;
			*fvk_cert_out = m_cert_store_ptr->fvk_cert;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - FVK Cert Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - Not available" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Get_IVK_Cert( uint8_t** ivk_cert_out,
														 uint16_t* ivk_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->ivk_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ivk_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ivk_cert_len <= IVK_CERT_MAX_SIZE ) )
		{
			*ivk_cert_len_out = m_cert_store_ptr->ivk_cert_len;
			*ivk_cert_out = m_cert_store_ptr->ivk_cert;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - IVK Cert Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - Not available" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Get_FVK_Key( uint8_t** fvk_key_out,
														uint16_t* fvk_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->fvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->fvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->fvk_key_len <= FVK_KEY_MAX_SIZE ) )
		{
			*fvk_key_len_out = m_cert_store_ptr->fvk_key_len;
			*fvk_key_out = m_cert_store_ptr->fvk_key;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - FVK key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
	}
	if ( ret != PKI_Common::SUCCESS )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - FVK key Read Failure" );
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Store_Flash::cert_store_struct_t* Cert_Store_Flash::Get_Cert_Store_Ram_Ptr( void )
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_Header_ID( void )
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_CRC_Buf( void )
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
PKI_Common::pki_status_t Cert_Store_Flash::Cert_Store_Write( void )
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
				/* Erase Length should be equal to the sector size */
				if ( NV_Ctrl::SUCCESS ==
					 m_nv_ctrl->Erase( reinterpret_cast<uint32_t>( m_cert_store_ptr ), ( m_cert_store_len + 1U ) ) )
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
					}
				}
				else
				{
					FUS_DEBUG_PRINT( DBG_MSG_ERROR,
									 "Cert Store Memory Erase Operation Failed" );
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
bool Cert_Store_Flash::Is_RVK_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_cert_store_ptr->rvk_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->rvk_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_RVK_Key_Buf( uint8_t* key_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "RVK key updated in local cert store buffer" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_IVK_Cert_Buf( uint8_t* cert_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "IVK Cert updated in local cert store buffer" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_FVK_Cert_Buf( uint8_t* cert_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "FVK Cert updated in local cert store buffer" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_FVK_Key_Buf( uint8_t* key_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "FVK key updated in local cert store buffer" );
				ret = PKI_Common::SUCCESS;
			}
		}
	}
	return ( ret );
}

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Get_CA_Cert( uint8_t** ca_cert_out,
														uint32_t* ca_cert_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->ca_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_cert_len <= CA_CERT_MAX_SIZE ) )
		{

			*ca_cert_out = m_cert_store_ptr->ca_cert;
			*ca_cert_len_out = static_cast<uint32_t>( m_cert_store_ptr->ca_cert_len );
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - CA Cert Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			ret = PKI_Common::DATA_ERROR;
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA Cert Read Failure" );
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Get_CA_Key( uint8_t** ca_key_out,
													   uint16_t* ca_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->ca_priv_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_priv_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_priv_key_len <= CA_KEY_MAX_SIZE ) )
		{

			*ca_key_out = m_cert_store_ptr->ca_priv_key;
			*ca_key_len_out = m_cert_store_ptr->ca_priv_key_len;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - CA key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			ret = PKI_Common::DATA_ERROR;
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - CA key Read Failure" );
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Get_Dev_Key( uint8_t** dev_key_out,
														uint16_t* dev_key_len_out )
{
	PKI_Common::pki_status_t ret = PKI_Common::FAILURE;

	if ( ( m_cert_store_ptr != nullptr ) && ( m_cert_store_exists == true ) )
	{
		if ( ( m_cert_store_ptr->device_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->device_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) &&
			 ( m_cert_store_ptr->device_key_len <= DEV_KEY_MAX_SIZE ) )
		{
			/* device key is available and return the same */
			*dev_key_out = m_cert_store_ptr->device_key;
			*dev_key_len_out = m_cert_store_ptr->device_key_len;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Cert Store - Device Key Read Successful" );
			ret = PKI_Common::SUCCESS;
		}
		else
		{
			/* device key is not available and return error */
			ret = PKI_Common::DATA_ERROR;
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Cert Store - Device Key Read Failure" );
		}
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Cert_Store_Flash::Update_CA_Cert_Buf( uint8_t* cert_mem,
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_CA_Key_Buf( uint8_t* key_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "CA key updated in local cert store buffer" );
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
PKI_Common::pki_status_t Cert_Store_Flash::Update_Dev_Key_Buf( uint8_t* key_mem,
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
				FUS_DEBUG_PRINT( DBG_MSG_INFO,
								 "Device Key updated in local cert store buffer" );
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
bool Cert_Store_Flash::Is_CA_Cert_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_cert_store_ptr->ca_cert_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_cert_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
bool Cert_Store_Flash::Is_CA_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_cert_store_ptr->ca_priv_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->ca_priv_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
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
bool Cert_Store_Flash::Is_Dev_Key_Available( void )
{
	bool ret = false;

	if ( m_cert_store_exists == true )
	{
		if ( ( m_cert_store_ptr->device_key_len != FLASH_MEMORY_DEFAULT_VALUE_U16 ) &&
			 ( m_cert_store_ptr->device_key_len != FLASH_MEMORY_ZERO_VALUE_U16 ) )
		{
			ret = true;
		}
	}
	return ( ret );
}

#endif  // PKI_CERT_GEN_SUPPORT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Cert_Store_Flash::~Cert_Store_Flash( void )
{}
