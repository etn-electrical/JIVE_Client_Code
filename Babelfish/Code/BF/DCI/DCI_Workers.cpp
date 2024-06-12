/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DCI_Workers.h"
#include "DCI_Source_Ids.h"
#include "Range_Check.h"

#include "DCI.h"
#include "DCI_NV_Mem.h"
#include "Bit.hpp"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Workers::DCI_Workers( bool correct_out_of_range_sets, bool owner_access,
						  Change_Tracker const* change_tracker_to_avoid,
						  BF::DCI_Lock* dci_lock ) :
	m_change_tracker( change_tracker_to_avoid ),
	m_owner_access( owner_access ),
	m_correct_out_of_range_sets( correct_out_of_range_sets ),
	m_source_id( DCI_SOURCE_IDS_Get_Privileged() ),
	m_dci_lock( dci_lock )
{
	if ( m_dci_lock == reinterpret_cast<BF::DCI_Lock*>( nullptr ) )
	{
		m_dci_lock = new BF::DCI_Lock( BF::DCI_Lock::READ_ONLY );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Workers::~DCI_Workers()
{
	m_change_tracker = reinterpret_cast<Change_Tracker const*>( nullptr );
	delete m_dci_lock;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Workers::Change_Permissions( bool owner_access )
{
	m_owner_access = owner_access;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Check_Length( DCI_DATA_BLOCK_TD const* data_block,
										DCI_ACCESS_DATA_TD* data_struct,
										DCI_ACCESS_CMD_TD command ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_RANGE_BLOCK const* range_block = nullptr;
	DCI_ENUM_COUNT_TD enum_count = 0U;

	if ( data_block->range_block_index != DCI::INVALID_INDEX )
	{
		range_block = DCI::Get_Range_Block( data_block->range_block_index );
		if ( range_block != nullptr )
		{
			enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
		}
	}

	if ( data_struct->length == 0U )
	{
		data_struct->offset = ACCESS_OFFSET_NONE;
		switch ( command )
		{
			case DCI_ACCESS_GET_LENGTH_CMD:
				data_struct->length = static_cast<uint16_t>( sizeof( DCI_LENGTH_TD ) );
				break;

			case DCI_ACCESS_GET_ENUM_LIST_LEN_CMD:
				data_struct->length = static_cast<uint16_t>( sizeof( uint16_t ) );
				break;

			case DCI_ACCESS_GET_ENUM_CMD:
				if ( ( range_block != nullptr ) && ( enum_count != 0xFFFF ) )
				{
					data_struct->length = static_cast<DCI_LENGTH_TD>( enum_count *
																	  DCI::Get_Datatype_Size( data_block->datatype ) );
				}
				break;

			case DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD:
				data_struct->length = static_cast<uint16_t>( sizeof( DCI_PATRON_ATTRIB_TD ) );
				break;

			default:
				data_struct->length = *data_block->length_ptr;
				break;
		}
	}
	else
	{
		if ( ( data_struct->length + data_struct->offset ) >
			 *data_block->length_ptr )
		{
			return_status = DCI_ERR_INVALID_DATA_LENGTH;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Check_Access( DCI_DATA_BLOCK_TD const* data_block,
										DCI_ACCESS_CMD_TD command ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;

	if ( m_owner_access == true )
	{
		switch ( command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			case DCI_ACCESS_CLEAR_RAM_CMD:
				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_READ_ONLY ) )
				{
					return_status = DCI_ERR_RAM_READ_ONLY;
				}
				break;

			case DCI_ACCESS_GET_INIT_CMD:
			case DCI_ACCESS_SET_INIT_CMD:
				if ( !BF_Lib::Bit::Test( data_block->owner_attrib_info,
										 DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
				{
					return_status = DCI_ERR_NV_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_DEFAULT_CMD:
				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
				{
					if ( !BF_Lib::Bit::Test( data_block->owner_attrib_info,
											 DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL ) )
					{
						return_status = DCI_ERR_DEFAULT_NOT_WRITABLE;
					}
				}
				else
				{
					return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_MIN_CMD:
			case DCI_ACCESS_SET_MAX_CMD:
				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) )
				{
					if ( !BF_Lib::Bit::Test( data_block->owner_attrib_info,
											 DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL ) )
					{
						return_status = DCI_ERR_RANGE_NOT_WRITABLE;
					}
				}
				else
				{
					return_status = DCI_ERR_RANGE_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_LENGTH_CMD:
				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_RW_LENGTH ) )
				{
					return_status = DCI_ERR_DATA_LENGTH_NOT_WRITABLE;
				}
				break;

			case DCI_ACCESS_GET_DEFAULT_CMD:
				if ( !BF_Lib::Bit::Test( data_block->owner_attrib_info,
										 DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
				{
					return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_GET_MIN_CMD:
			case DCI_ACCESS_GET_MAX_CMD:
				if ( !BF_Lib::Bit::Test( data_block->owner_attrib_info,
										 DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) )
				{
					return_status = DCI_ERR_RANGE_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_GET_LENGTH_CMD:
			case DCI_ACCESS_GET_RAM_CMD:
				break;

			default:
				return_status = DCI_ERR_INVALID_COMMAND;
				break;
		}
	}
	else
	{
		switch ( command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			case DCI_ACCESS_CLEAR_RAM_CMD:
				if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
										DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
				{
					return_status = DCI_ERR_RAM_READ_ONLY;
				}
				break;

			case DCI_ACCESS_SET_INIT_CMD:
				if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
										DCI_PATRON_ATTRIB_NVMEM_DATA ) )
				{
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
					{
						return_status = DCI_ERR_NV_READ_ONLY;
					}
				}
				else
				{
					return_status = DCI_ERR_NV_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_DEFAULT_CMD:
				if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
										DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
				{
					return_status = DCI_ERR_DEFAULT_NOT_WRITABLE;
				}
				else
				{
					return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_MIN_CMD:
			case DCI_ACCESS_SET_MAX_CMD:
				if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
										DCI_PATRON_ATTRIB_RANGE_DATA ) )
				{
					return_status = DCI_ERR_RANGE_NOT_WRITABLE;
				}
				else
				{
					return_status = DCI_ERR_RANGE_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_SET_LENGTH_CMD:
				return_status = DCI_ERR_DATA_LENGTH_NOT_WRITABLE;
				break;

			case DCI_ACCESS_GET_INIT_CMD:
				if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
										 DCI_PATRON_ATTRIB_NVMEM_DATA ) )
				{
					return_status = DCI_ERR_NV_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_GET_DEFAULT_CMD:
				if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
										 DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
				{
					return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_GET_MIN_CMD:
			case DCI_ACCESS_GET_MAX_CMD:
				if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
										 DCI_PATRON_ATTRIB_RANGE_DATA ) )
				{
					return_status = DCI_ERR_RANGE_NOT_AVAILABLE;
				}
				break;

			case DCI_ACCESS_GET_LENGTH_CMD:
			case DCI_ACCESS_GET_RAM_CMD:
				break;

			default:
				return_status = DCI_ERR_INVALID_COMMAND;
				break;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Data_Access( DCI_ACCESS_ST_TD* access_struct ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_CB_RET_TD cback_return = DCI_CBACK_RET_PROCESS_NORMALLY;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( access_struct->data_id );
	BF_ASSERT( data_block != nullptr );
	DCI_CBACK_BLOCK_TD* cback_block = DCI::Get_CBACK_Block( data_block->cback_block_index );

	if ( data_block == reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		return_status = Check_Length( data_block, &access_struct->data_access,
									  access_struct->command );
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		if ( cback_block != nullptr )
		{
			if ( ( cback_block->cback != nullptr ) )
			{
				if ( ( DCI_Access_Cmd_Mask( access_struct->command ) &
					   cback_block->cback_mask ) != 0U )
				{
					cback_return =
						( *cback_block->cback )( cback_block->cback_param,
												 access_struct );
				}
			}
		}
		if ( cback_return == DCI_CBACK_RET_PROCESS_NORMALLY )
		{
			if ( data_block->ram_ptr != nullptr )
			{
				return_status = Check_Access( data_block, access_struct->command );
				if ( return_status == DCI_ERR_NO_ERROR )
				{
					return_status = Execute_Operation( data_block, access_struct );
				}
			}
			else
			{
				return_status = DCI_ERR_ACCESS_VIOLATION;
			}
		}
		else if ( ( cback_return == DCI_CBACK_RET_NO_FURTHER_PROC_VAL_CHANGE ) ||
				  ( cback_return == DCI_CBACK_RET_NO_FURTHER_PROC_NO_VAL_CHANGE ) )
		{
			bool data_changed;

			if ( cback_return == DCI_CBACK_RET_NO_FURTHER_PROC_VAL_CHANGE )
			{
				data_changed = true;
			}
			else
			{
				data_changed = false;
			}

			Change_Tracker::attrib_t change_tracker_attrib;

			change_tracker_attrib = Change_Tracker_Attrib( access_struct->command );

			Change_Tracker::ID_Change( access_struct->data_id, data_changed,
									   change_tracker_attrib, m_change_tracker );
			switch ( access_struct->command )
			{
				case DCI_ACCESS_SET_RAM_CMD:
				case DCI_ACCESS_DEFAULT_TO_RAM_CMD:
				case DCI_ACCESS_INIT_TO_RAM_CMD:
					Change_Tracker::ID_Change( access_struct->data_id, data_changed,
											   Change_Tracker::RAM_ATTRIB, m_change_tracker );
					break;

				case DCI_ACCESS_SET_INIT_CMD:
				case DCI_ACCESS_RAM_TO_INIT_CMD:
				case DCI_ACCESS_DEFAULT_TO_INIT_CMD:
					Change_Tracker::ID_Change( access_struct->data_id, data_changed,
											   Change_Tracker::NV_ATTRIB, m_change_tracker );
					break;

				default:
					break;
			}
		}
		else if ( ( cback_return & DCI_CBACK_RET_ERROR_ENCODED ) != 0U )
		{
			return_status = DCI_CBack_Decode_Error( cback_return );
			// return_status = static_cast<DCI_ERROR_TD>( cback_return &
			// static_cast<DCI_CB_RET_TD>(~static_cast<DCI_CB_RET_TD>(DCI_CBACK_RET_ERROR_ENCODED))
			// );
		}
		else
		{
			// do nothing
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Change_Tracker::attrib_t DCI_Workers::Change_Tracker_Attrib( DCI_ACCESS_CMD_TD command ) const
{
	Change_Tracker::attrib_t attrib;

	switch ( command )
	{
		case DCI_ACCESS_SET_LENGTH_CMD:
			attrib = Change_Tracker::LENGTH_ATTRIB;
			break;

		case DCI_ACCESS_CLEAR_RAM_CMD:
		case DCI_ACCESS_SET_RAM_CMD:
			attrib = Change_Tracker::RAM_ATTRIB;
			break;

		case DCI_ACCESS_SET_INIT_CMD:
			attrib = Change_Tracker::NV_ATTRIB;
			break;

		case DCI_ACCESS_SET_DEFAULT_CMD:
			attrib = Change_Tracker::DEFAULT_ATTRIB;
			break;

		case DCI_ACCESS_SET_MIN_CMD:
			attrib = Change_Tracker::MIN_ATTRIB;
			break;

		case DCI_ACCESS_SET_MAX_CMD:
			attrib = Change_Tracker::MIN_ATTRIB;
			break;

		default:
			attrib = Change_Tracker::TOTAL_ATTRIBS;
			break;
	}

	return ( attrib );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Execute_Operation( DCI_DATA_BLOCK_TD const* data_block,
											 DCI_ACCESS_ST_TD* access_struct ) const
{
	DCI_ERROR_TD return_status;

	return_status = Check_Access( data_block, access_struct->command );

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_GET_RAM_CMD:
				return_status = Get_RAM_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_RAM_CMD:
				return_status = Set_RAM_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_GET_LENGTH_CMD:
				return_status = Get_Length_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_LENGTH_CMD:
				return_status = Set_Length_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_GET_INIT_CMD:
				return_status = Get_Init_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_INIT_CMD:
				return_status = Set_Init_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_GET_DEFAULT_CMD:
				return_status = Get_Default_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_DEFAULT_CMD:
				return_status = Set_Default_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_GET_MIN_CMD:
				return_status = Get_Min_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_MIN_CMD:
				return_status = Set_Min_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_GET_MAX_CMD:
				return_status = Get_Max_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_SET_MAX_CMD:
				return_status = Set_Max_Data( access_struct, data_block );
				break;

			case DCI_ACCESS_CLEAR_RAM_CMD:
				return_status = Clear_Ram_Data( access_struct, data_block );
				break;

			default:
				return_status = DCI_ERR_INVALID_COMMAND;
				break;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	uint8_t* src_ptr;

	src_ptr = data_block->ram_ptr;
	DCI::Copy_Data( access_struct->data_access.data, &src_ptr[access_struct->data_access.offset],
					access_struct->data_access.length );

	return ( DCI_ERR_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	uint8_t* dest_ptr;
	DCI_ACCESS_DATA_TD post_set_range_fix;
	bool change = false;

	dest_ptr = data_block->ram_ptr;
	if ( Ignore_Range( data_block ) )
	{
		change = DCI::Copy_Data_Check_For_Change( &dest_ptr[access_struct->data_access.offset],
												  access_struct->data_access.data,
												  access_struct->data_access.length );
	}
	else
	{
		// If the value is whole then we can check the whole range right away
		// and error out.
		if ( access_struct->data_access.length == *data_block->length_ptr )
		{
			if ( Range_Check::Check_Range( &access_struct->data_access, data_block,
										   m_correct_out_of_range_sets ) )
			{
				change = DCI::Copy_Data_Check_For_Change( dest_ptr,
														  access_struct->data_access.data,
														  access_struct->data_access.length );
			}
			else
			{
				return_status = DCI_ERR_EXCEEDS_RANGE;
			}
		}
		else
		{
			change = DCI::Copy_Data_Check_For_Change( &dest_ptr[access_struct->data_access.offset],
													  access_struct->data_access.data,
													  access_struct->data_access.length );

			post_set_range_fix.data = data_block->ram_ptr;
			post_set_range_fix.length = *data_block->length_ptr;
			post_set_range_fix.offset = ACCESS_OFFSET_NONE;
			if ( !Range_Check::Check_Range( &post_set_range_fix, data_block, true ) )
			{
				return_status = DCI_ERR_EXCEEDS_RANGE;
			}
		}
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, change,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Length_Data( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI::Copy_Data( access_struct->data_access.data, data_block->length_ptr,
					static_cast<DCI_LENGTH_TD>( sizeof( DCI_LENGTH_TD ) ) );

	return ( DCI_ERR_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Length_Data( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_LENGTH_TD* length_dest;

	length_dest = const_cast<DCI_LENGTH_TD*>( data_block->length_ptr );

	DCI::Copy_Data( length_dest, access_struct->data_access.data,
					static_cast<DCI_LENGTH_TD>( sizeof( DCI_LENGTH_TD ) ) );

	return ( DCI_ERR_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Clear_Ram_Data( DCI_ACCESS_ST_TD* access_struct,
										  DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	bool nv_read_error;
	bool data_changed = false;

	// If we have an Init value load that.
	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		nv_read_error = DCI_NV_Mem::Owner_Read( data_block->ram_ptr, data_block,
												&data_changed );
		if ( nv_read_error == true )
		{
			return_status = DCI_ERR_NV_ACCESS_FAILURE;
		}
	}	// If we have a default value then use that.
	else if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
								 DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		data_changed = DCI::Copy_Data_Check_For_Change( data_block->ram_ptr,
														data_block->default_val,
														*data_block->length_ptr );
	}
	else	// Use a clear value which is defined by the DCI as zero.
	{
		data_changed = DCI::Clear_Data( data_block->ram_ptr, *data_block->length_ptr );
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, data_changed,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Init_Data( DCI_ACCESS_ST_TD* access_struct,
										 DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;

	if ( !DCI_NV_Mem::Owner_Read( access_struct->data_access.data, data_block,
								  ( static_cast<bool*>( nullptr ) ),
								  access_struct->data_access.offset,
								  access_struct->data_access.length ) )
	{
		return_status = DCI_ERR_NV_ACCESS_FAILURE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Init_Data( DCI_ACCESS_ST_TD* access_struct,
										 DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	bool data_changed;

	if ( Ignore_Range( data_block ) )
	{
		if ( !DCI_NV_Mem::Owner_Write( access_struct->data_access.data, data_block, &data_changed,
									   access_struct->data_access.offset,
									   access_struct->data_access.length ) )
		{
			return_status = DCI_ERR_NV_ACCESS_FAILURE;
		}
	}
	else
	{
		return_status = DCI_NV_Mem::Patron_Write( &access_struct->data_access,
												  data_block, m_correct_out_of_range_sets,
												  &data_changed );
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, data_changed,
								   Change_Tracker::NV_ATTRIB, m_change_tracker );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Default_Data( DCI_ACCESS_ST_TD* access_struct,
											DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI::Copy_Data( access_struct->data_access.data,
					&data_block->default_val[access_struct->data_access.offset],
					access_struct->data_access.length );

	return ( DCI_ERR_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Default_Data( DCI_ACCESS_ST_TD* access_struct,
											DCI_DATA_BLOCK_TD const* data_block ) const
{
	uint8_t* dest_ptr;
	bool change;

	dest_ptr = const_cast<DCI_RAM_DATA_TD*>( data_block->default_val );
	change = DCI::Copy_Data_Check_For_Change( &dest_ptr[access_struct->data_access.offset],
											  access_struct->data_access.data,
											  access_struct->data_access.length );
	Change_Tracker::ID_Change( access_struct->data_id, change, Change_Tracker::DEFAULT_ATTRIB,
							   m_change_tracker );

	return ( DCI_ERR_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Min_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD status = DCI_ERR_RANGE_NOT_AVAILABLE;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( range_block != nullptr )
	{
		DCI::Copy_Data( access_struct->data_access.data,
						&range_block->min[access_struct->data_access.offset],
						access_struct->data_access.length );
		status = DCI_ERR_NO_ERROR;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Min_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	uint8_t* dest_ptr;
	bool change;
	DCI_ERROR_TD status = DCI_ERR_RANGE_NOT_AVAILABLE;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( range_block != nullptr )
	{
		dest_ptr = const_cast<DCI_RAM_DATA_TD*>( range_block->min );
		change = DCI::Copy_Data_Check_For_Change(
			&dest_ptr[access_struct->data_access.offset], access_struct->data_access.data,
			access_struct->data_access.length );
		Change_Tracker::ID_Change( access_struct->data_id, change,
								   Change_Tracker::MIN_ATTRIB, m_change_tracker );
		status = DCI_ERR_NO_ERROR;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Max_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD status = DCI_ERR_RANGE_NOT_AVAILABLE;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( range_block != nullptr )
	{
		DCI::Copy_Data( access_struct->data_access.data,
						&range_block->max[access_struct->data_access.offset],
						access_struct->data_access.length );
		status = DCI_ERR_NO_ERROR;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Max_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	uint8_t* dest_ptr;
	bool change;
	DCI_ERROR_TD status = DCI_ERR_RANGE_NOT_AVAILABLE;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( range_block != nullptr )
	{
		dest_ptr = const_cast<DCI_RAM_DATA_TD*>( range_block->max );
		change = DCI::Copy_Data_Check_For_Change(
			&dest_ptr[access_struct->data_access.offset], access_struct->data_access.data,
			access_struct->data_access.length );
		Change_Tracker::ID_Change( access_struct->data_id, change,
								   Change_Tracker::MAX_ATTRIB, m_change_tracker );
		status = DCI_ERR_NO_ERROR;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Workers::Ignore_Range( DCI_DATA_BLOCK_TD const* data_block ) const
{
	bool ignore = false;

	if ( ( BF_Lib::Bit::Test( data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) ==
		   false ) || m_owner_access )
	{
		ignore = true;
	}

	return ( ignore );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Current( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
									   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Current( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
									   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = data;
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Init( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
									DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_INIT_CMD;
	access_struct.data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Init( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
									DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_INIT_CMD;
	access_struct.data_access.data = data;
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Default( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
									   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_DEFAULT_CMD;
	access_struct.data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Default( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
									   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_DEFAULT_CMD;
	access_struct.data_access.data = data;
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Min( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
								   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_MIN_CMD;
	access_struct.data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Min( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
								   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_MIN_CMD;
	access_struct.data_access.data = data;
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Max( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
								   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_MAX_CMD;
	access_struct.data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Max( DCI_ID_TD dcid, DCI_DATA_PASS_TD* data,
								   DCI_LENGTH_TD length, DCI_LENGTH_TD offset ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_MAX_CMD;
	access_struct.data_access.data = data;
	access_struct.data_access.offset = offset;
	access_struct.data_access.length = length;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Set_Length( DCI_ID_TD dcid, DCI_LENGTH_TD length ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_SET_LENGTH_CMD;
	access_struct.data_access.data = &length;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Length( DCI_ID_TD dcid, DCI_LENGTH_TD* length ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		*length = *data_block->length_ptr;
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Datatype( DCI_ID_TD dcid, DCI_DATATYPE_TD* datatype ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		*datatype = data_block->datatype;
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Initialize_Current( DCI_ID_TD dcid ) const
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_CLEAR_RAM_CMD;
	access_struct.data_access.data = reinterpret_cast<DCI_DATA_PASS_TD*>( nullptr );
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;

	return ( Data_Access( &access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Owner_Attrib( DCI_ID_TD dcid, DCI_OWNER_ATTRIB_TD* attrib ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		*attrib = data_block->owner_attrib_info;
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Get_Patron_Attrib( DCI_ID_TD dcid, DCI_PATRON_ATTRIB_TD* attrib ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		*attrib = data_block->patron_attrib_info;
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Range_Check( DCI_ID_TD dcid, DCI_DATA_PASS_TD const* data,
									   bool correct_out_of_range, DCI_LENGTH_TD length,
									   DCI_LENGTH_TD offset ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_ACCESS_DATA_TD data_struct;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block == reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	data_struct.length = length;
	data_struct.offset = offset;
	data_struct.data = const_cast<DCI_DATA_PASS_TD*>( data );

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		return_status = Check_Length( data_block, &data_struct, DCI_ACCESS_SET_RAM_CMD );
	}

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		if ( data != reinterpret_cast<DCI_DATA_PASS_TD const*>( nullptr ) )
		{
			return_status = Check_Access( data_block, DCI_ACCESS_GET_MIN_CMD );
			if ( return_status == DCI_ERR_NO_ERROR )
			{
				if ( !Range_Check::Check_Range( &data_struct, data_block,
												correct_out_of_range ) )
				{
					return_status = DCI_ERR_EXCEEDS_RANGE;
				}
			}
		}
		else
		{
			return_status = DCI_ERR_ACCESS_VIOLATION;
		}
	}

	return ( return_status );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'length' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Is_Locked( DCI_ID_TD dcid, bool* locked, DCI_LOCK_TYPE_TD lock_type ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		*locked = m_dci_lock->Locked( dcid, lock_type );
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Workers::Lock( DCI_ID_TD dcid, bool lock, DCI_LOCK_TYPE_TD lock_type ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( dcid );

	if ( data_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
	{
		if ( lock == true )
		{
			m_dci_lock->Lock( dcid, lock_type );
		}
		else
		{
			m_dci_lock->Unlock( dcid, lock_type );
		}
	}
	else
	{
		return_status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( return_status );
}
