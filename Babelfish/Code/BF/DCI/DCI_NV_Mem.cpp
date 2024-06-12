/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_NV_Mem.h"
#include "NV_Mem.h"
#include "Ram.h"
#include "Stdlib_MV.h"
#include "Range_Check.h"
#include "DCI.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint8_t* DCI_NV_Mem::m_frag_buffer;
uint32_t DCI_NV_Mem::m_buffer_size;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_NV_Mem::Init( uint8_t factory_reset_done_key, uint32_t buffer_size )
{
	uint8_t factory_reset_mem_trig = 0U;
	bool success;

	m_buffer_size = buffer_size;
	m_frag_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( buffer_size ) );

	success = NV::NV_Mem::Read( &factory_reset_mem_trig, DCI_NV_MEM_FACTORY_RESET_TRIG_NVADD,
								sizeof( factory_reset_mem_trig ), NV::NV_Mem::USE_PROTECTION );
	if ( ( factory_reset_mem_trig != factory_reset_done_key ) &&
		 ( success == true ) )
	{
		BF::System_Reset::Reset_Device( BF::System_Reset::FACTORY_RESET );
		DCI::Reset_Req_Handler( static_cast<BF::System_Reset::cback_param_t>( nullptr ),
								BF::System_Reset::FACTORY_RESET );
		factory_reset_mem_trig = factory_reset_done_key;
		NV::NV_Mem::Write( &factory_reset_mem_trig, DCI_NV_MEM_FACTORY_RESET_TRIG_NVADD,
						   sizeof( factory_reset_mem_trig ), NV::NV_Mem::USE_PROTECTION );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_NV_Mem::Patron_Read( DCI_ACCESS_DATA_TD* data_access,
									  DCI_DATA_BLOCK_TD const* data_block, bool* ram_data_changed )
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	bool data_changed = false;
	bool read_success;

	read_success = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( m_frag_buffer ), data_block->nv_loc,
									 *data_block->length_ptr, NV::NV_Mem::USE_PROTECTION );
	if ( ( *data_block->length_ptr <= m_buffer_size ) && read_success )
	{
		data_changed =
			DCI::Copy_Data_Check_For_Change( reinterpret_cast<uint8_t*>( data_access->data ),
											 &m_frag_buffer[data_access->offset], data_access->length );
	}
	else
	{
		return_error = DCI_ERR_NV_ACCESS_FAILURE;
	}

	if ( ram_data_changed != NULL )
	{
		*ram_data_changed = data_changed;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_NV_Mem::Patron_Write( DCI_ACCESS_DATA_TD* data_access,
									   DCI_DATA_BLOCK_TD const* data_block, bool fix_value, bool* nv_data_changed )
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	bool data_changed = false;
	bool test_range = false;
	bool test_enum = false;

	if ( *data_block->length_ptr <= m_buffer_size )	// &&	///The NV_Mem read below is commented out so we can use the
													// below repair routine.
	// ( NV_Mem::Read( m_frag_buffer, data_block->nv_loc, *data_block->length_ptr, NV_Mem::USE_PROTECTION ) ) )
	{
		/// The below is to be put in if the NV write with bad data becomes an issue.  This will allow a write to occur.
		/// The below will write over the bad block.
		if ( !NV::NV_Mem::Read( m_frag_buffer, data_block->nv_loc, *data_block->length_ptr,
								NV::NV_Mem::USE_PROTECTION ) )
		{	// The NV read is invalid.  Load with the default if it exists.  Otherwise zero out.
			if ( BF_Lib::Bit::Test( data_block->owner_attrib_info,
									DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
			{		// Either use the default value
				DCI::Copy_Data( m_frag_buffer, data_block->default_val,
								*data_block->length_ptr );
			}
			else
			{		// Or else just put a zero in there.  Wipe'r clean.
				for ( DCI_LENGTH_TD i = 0U; i < *data_block->length_ptr; i++ )
				{
					m_frag_buffer[i] = 0U;
				}
			}
			DCI::Copy_Data( &m_frag_buffer[data_access->offset],
							reinterpret_cast<uint8_t*>( data_access->data ), data_access->length );
			data_changed = true;
		}
		else
		{
			data_changed = DCI::Copy_Data_Check_For_Change( &m_frag_buffer[data_access->offset],
															reinterpret_cast<uint8_t*>( data_access->data ),
															data_access->length );
		}

		if ( data_changed == true )
		{
			DCI_ACCESS_DATA_TD temp_data_access;
			temp_data_access.length = *data_block->length_ptr;
			temp_data_access.offset = 0U;
			temp_data_access.data = m_frag_buffer;

			test_range = BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_RANGE_DATA );
			test_enum = BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_ENUM_DATA );
			if ( test_range || test_enum )
			{
				if ( !Range_Check::Check_Range( &temp_data_access, data_block, fix_value ) )
				{
					return_error = DCI_ERR_EXCEEDS_RANGE;
				}
			}

			if ( return_error == DCI_ERR_NO_ERROR )
			{
				if ( !NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( m_frag_buffer ),
										 data_block->nv_loc, *data_block->length_ptr, NV::NV_Mem::USE_PROTECTION ) )
				{
					return_error = DCI_ERR_NV_ACCESS_FAILURE;
				}
			}
		}
	}
	else
	{
		return_error = DCI_ERR_NV_ACCESS_FAILURE;
	}

	if ( nv_data_changed != NULL )
	{
		*nv_data_changed = data_changed;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_NV_Mem::Owner_Read( DCI_DATA_PASS_TD* data_ptr,
							 DCI_DATA_BLOCK_TD const* data_block, bool* ram_data_changed,
							 DCI_LENGTH_TD offset, DCI_LENGTH_TD length )
{
	bool success = false;
	bool data_changed = false;
	bool read_success;

	if ( length == ACCESS_LENGTH_GET_ALL )
	{
		length = *data_block->length_ptr;
	}

	if ( ( offset + length ) <= ( *data_block->length_ptr ) )
	{
		read_success = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( m_frag_buffer ), data_block->nv_loc,
										 *data_block->length_ptr, NV::NV_Mem::USE_PROTECTION );
		if ( ( *data_block->length_ptr <= m_buffer_size ) && read_success )
		{
			data_changed = DCI::Copy_Data_Check_For_Change( data_ptr, &m_frag_buffer[offset], length );
			success = true;
		}
	}
	if ( ram_data_changed != NULL )
	{
		*ram_data_changed = data_changed;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_NV_Mem::Owner_Write( DCI_DATA_PASS_TD const* data_ptr,
							  DCI_DATA_BLOCK_TD const* data_block, bool* nv_data_changed,
							  DCI_LENGTH_TD offset, DCI_LENGTH_TD length )
{
	bool success = false;
	bool data_changed = false;

	if ( length == ACCESS_LENGTH_SET_ALL )
	{
		length = *data_block->length_ptr;
	}

	if ( ( offset + length ) <= ( *data_block->length_ptr ) )
	{

		if ( *data_block->length_ptr <= m_buffer_size )	// &&	///The NV_Mem read below is commented out so we can use
														// the below repair routine.
		// ( NV_Mem::Read( m_frag_buffer, data_block->nv_loc, *data_block->length_ptr, NV_Mem::USE_PROTECTION ) ) )
		{
			/// The below is to be put in if the NV write with bad data becomes an issue.  This will allow a write to
			/// occur.
			/// The below will write over the bad block.
			if ( !NV::NV_Mem::Read( m_frag_buffer, data_block->nv_loc, *data_block->length_ptr,
									NV::NV_Mem::USE_PROTECTION ) )
			{	// The NV read is invalid.  Load with the default if it exists.  Otherwise zero out.
				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
				{		// Either use the default value
					DCI::Copy_Data( m_frag_buffer, data_block->default_val, *data_block->length_ptr );
				}
				else
				{		// Or else just put a zero in there.  Wipe'r clean.
					for ( DCI_LENGTH_TD i = 0U; i < *data_block->length_ptr; i++ )
					{
						m_frag_buffer[i] = 0U;
					}
				}
				data_changed = true;
			}
			else
			{
				data_changed = DCI::Copy_Data_Check_For_Change( &m_frag_buffer[offset], data_ptr, length );
			}

			success = true;

			if ( data_changed == true )
			{
				if ( !NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( m_frag_buffer ), data_block->nv_loc,
										 *data_block->length_ptr, NV::NV_Mem::USE_PROTECTION ) )
				{
					success = false;
				}
			}
		}
	}
	if ( nv_data_changed != NULL )
	{
		*nv_data_changed = data_changed;
	}

	return ( success );
}
