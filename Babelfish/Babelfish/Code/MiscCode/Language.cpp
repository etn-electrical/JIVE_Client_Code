/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Language.h"
#include "Includes.h"
#include "StdLib_MV.h"
#include "Language_Debug.h"
#include "Fw_Store.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Language::Language( NV_Ctrl* lg_nv_ctrl ) :
	m_lg_ptr( nullptr ),
	m_lg_nv_ctrl( lg_nv_ctrl ),
	m_lg_list( nullptr ),
	m_lg_total( nullptr )
{
	if ( m_lg_nv_ctrl != nullptr )
	{
		NV_Ctrl::mem_range_info_t mem_range = {0U};
		m_lg_nv_ctrl->Get_Memory_Info( &mem_range );

		/* Get the start address of language Pack */
		m_lg_ptr =
			reinterpret_cast<lg_spec_struct_t*>( mem_range.start_address +
												 sizeof( crc_info ) +
												 sizeof( fw_header ) +
												 sizeof( prodinfo_for_boot_s ) );
		LANG_DEBUG_PRINT( DBG_MSG_INFO, "Start address of language Pack is %d",
						  m_lg_ptr );
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR,
						  "NV control to access language pack storage memory is not configured" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Language::Language_Info_Init( void )
{
	if ( m_lg_ptr != nullptr )
	{
		uint8_t total_lang_cnt = 0U;
		lg_spec_struct_t* addr = m_lg_ptr;
		uint32_t offset = 0U;
		NV_Ctrl::mem_range_info_t mem_range = {0U};
		m_lg_nv_ctrl->Get_Memory_Info( &mem_range );
		uint32_t total_mem_range =
			( ( mem_range.end_address - mem_range.start_address ) + 1 );

		/* Get the count of total number of languages, if next_lg_offset of language header found
		   null then stop counting */
		while ( ( addr != nullptr ) && ( total_lang_cnt < LG_MAX ) )
		{
			/* Validate langauge spec and get next language address offset */
			if ( true == Validate_Lg_Spec( reinterpret_cast<uint32_t>( addr ), &offset, total_mem_range ) )
			{
				total_lang_cnt++;
				if ( offset == 0U )
				{
					addr = nullptr;
				}
				else
				{
					addr =
						reinterpret_cast<lg_spec_struct_t*>( ( reinterpret_cast<uint32_t>( addr ) +
															   offset ) );
				}
			}
			else
			{
				break;
			}
		}
		m_lg_total = total_lang_cnt;
		LANG_DEBUG_PRINT( DBG_MSG_INFO,
						  "Total number of languages available in device memory is %d",
						  m_lg_total );

		if ( m_lg_list != nullptr )
		{
			delete[] m_lg_list;
			m_lg_list = nullptr;
		}
		if ( m_lg_total > 0U )
		{
			m_lg_list = new lg_metadata[m_lg_total];
		}

		/* Save language IDs and addresses in a buffer */
		if ( m_lg_list != nullptr )
		{
			addr = m_lg_ptr;
			uint8_t i;

			for ( i = 0U; ( ( i < m_lg_total ) && ( addr != nullptr ) ); i++ )
			{
				m_lg_list[i].lg_addr = reinterpret_cast<uint32_t>( addr );
				if ( m_lg_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &m_lg_list[i].lg_id ),
										 reinterpret_cast<uint32_t>( &addr->id ),
										 sizeof( addr->id ), false ) == NV_Ctrl::SUCCESS )
				{
					LANG_DEBUG_PRINT( DBG_MSG_INFO, "%d. Language ID : %d , address : %x", i,
									  m_lg_list[i].lg_id, m_lg_list[i].lg_addr );

					if ( m_lg_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &offset ),
											 reinterpret_cast<uint32_t>( &addr->next_lg_offset ),
											 sizeof( addr->next_lg_offset ),
											 false ) == NV_Ctrl::SUCCESS )
					{
						addr =
							reinterpret_cast<lg_spec_struct_t*>( ( reinterpret_cast<uint32_t>( addr )
																   + offset ) );
					}
					else
					{
						LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Next Language offset read failed" );
						break;
					}
				}
				else
				{
					LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Language ID read failed" );
					break;
				}
			}
		}
		else
		{
			LANG_DEBUG_PRINT( DBG_MSG_ERROR,
							  "Memory is not allocated for storing list of language IDs and addresses (m_lg_list)" );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Validate_Lg_Spec( uint32_t addr, uint32_t* offset, uint32_t total_mem_range )
{
	bool ret_status = false;
	lg_spec_struct_t header;

	header.spec_version = 0U;

	if ( m_lg_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &header ),
							 reinterpret_cast<uint32_t>( addr ),
							 sizeof( lg_spec_struct_t ), false ) == NV_Ctrl::SUCCESS )
	{
		if ( ( header.spec_version <= LG_MAX_SPEC_VER ) && ( header.spec_version != 0U ) )
		{
			if ( ( header.length > 0U ) && ( header.length < total_mem_range ) )
			{
				if ( header.next_lg_offset < total_mem_range )
				{
					*offset = header.next_lg_offset;
					ret_status = true;
				}
				else
				{
					LANG_DEBUG_PRINT( DBG_MSG_ERROR,
									  "Next language address offset of %s language is invalid : %d Bytes",  header.name,
									  header.next_lg_offset );
				}
			}
			else
			{
				LANG_DEBUG_PRINT( DBG_MSG_ERROR,
								  " Block length of %s language is invalid : %d Bytes ",
								  header.name, header.length );
			}
		}
		else
		{
			LANG_DEBUG_PRINT( DBG_MSG_ERROR, " Spec version of %s language is invalid : %d ",
							  header.name, header.spec_version );
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Language header read failed" );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Get_Total_Lang_Count( uint8_t* lg_cnt )
{
	bool ret_status = false;

	if ( m_lg_ptr != nullptr )
	{
		if ( m_lg_total > 0U )
		{
			*lg_cnt = m_lg_total;
			ret_status = true;
		}
		else
		{
			LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Total language count is zero" );
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR,
						  "Pointer to language pack is null, cannot provide language count" );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Get_Lang_Id( uint8_t index, uint8_t* id )
{
	bool ret_status = false;

	if ( m_lg_list != nullptr )
	{
		if ( index < m_lg_total )
		{
			*id = m_lg_list[index].lg_id;
			ret_status = true;
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "List of language IDs is empty" );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Get_Lang_Addr( uint32_t* addr, uint8_t id )
{
	bool ret_status = false;
	uint8_t i;

	if ( m_lg_list != nullptr )
	{
		/* Get language index
		   Language IDs are stored in m_lg_list[], search for an index whoes id is matched. */
		for ( i = 0U; i < m_lg_total; i++ )
		{
			if ( m_lg_list[i].lg_id == id )
			{
				*addr = m_lg_list[i].lg_addr + sizeof( lg_spec_struct_t );
				ret_status = true;
			}
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "List of language start addresses is empty" );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Is_Lang_Id_Valid( uint8_t id )
{
	bool is_id_valid = false;
	uint8_t i;

	if ( m_lg_list != nullptr )
	{
		for ( i = 0U; ( ( i < m_lg_total ) && ( is_id_valid == false ) ); i++ )
		{
			if ( id == m_lg_list[i].lg_id )
			{
				is_id_valid = true;
			}
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "List of language IDs is empty" );
	}

	if ( is_id_valid == false )
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR,
						  "language ID  %d  is invalid, requested language is not available in database \n Ensure to have language code added in upported Languages Paths in Language_Config.h",
						  id );
	}
	return ( is_id_valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Get_Lang_Header( uint8_t lg_id, lg_spec_struct_t* lg_header )
{
	bool ret_status = false;

	if ( m_lg_list != nullptr )
	{
		uint8_t i;
		uint16_t header_size = sizeof( lg_spec_struct_t );
		lg_spec_struct_t header;
		header.id = 0U;

		for ( i = 0U; ( ( i < m_lg_total ) && ( ret_status == false ) ); i++ )
		{
			if ( m_lg_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &header ),
									 reinterpret_cast<uint32_t>( m_lg_list[i].lg_addr ),
									 header_size, false ) == NV_Ctrl::SUCCESS )
			{
				if ( lg_id == header.id )
				{
					if ( BF_Lib::Copy_Data( reinterpret_cast<uint8_t*>( lg_header ),
											header_size,
											reinterpret_cast<uint8_t*>( &header ),
											header_size ) == true )
					{
						ret_status = true;
					}
				}
			}
			else
			{
				LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Language header information read failed" );
				break;
			}
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "List of language start addresses is empty" );
	}
	return ( ret_status );
}

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Match_Lg_Rest_Path( uint8_t* lg_id, const uint8_t* data, uint32_t length )
{
	bool ret_status = false;

	if ( m_lg_list != nullptr )
	{
		uint8_t i;
		uint16_t header_size = sizeof( lg_spec_struct_t );
		lg_spec_struct_t header;

		for ( i = 0U; ( ( i < m_lg_total ) && ( ret_status == false ) ); i++ )
		{
			if ( m_lg_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &header ),
									 reinterpret_cast<uint32_t>( m_lg_list[i].lg_addr ),
									 header_size, false ) == NV_Ctrl::SUCCESS )
			{
				if ( BF_Lib::Compare_Data( reinterpret_cast<uint8_t*>( header.code ),
										   const_cast<uint8_t*>( data ),
										   length ) == true )
				{
					*lg_id = header.id;
					ret_status = true;
				}
			}
			else
			{
				LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Language header information read failed" );
				break;
			}
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR, "List of language start addresses is empty" );
	}
	return ( ret_status );
}

# endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Language::Lang_Read( uint8_t* data, uint32_t addr, uint32_t offset, uint32_t read_len )
{
	bool ret_status = false;

	if ( m_lg_ptr != nullptr )
	{
		if ( m_lg_nv_ctrl->Read( data, ( addr + offset ), read_len, false ) == NV_Ctrl::SUCCESS )
		{
			ret_status = true;
		}
		else
		{
			LANG_DEBUG_PRINT( DBG_MSG_ERROR, "Language data read failed" );
		}
	}
	else
	{
		LANG_DEBUG_PRINT( DBG_MSG_ERROR,
						  "Pointer to language pack is null, cannot read language data" );
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Language::~Language( void )
{
	if ( m_lg_list != nullptr )
	{
		delete[] m_lg_list;
		m_lg_list = nullptr;
	}
}
