/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Patron.h"
#include "Exception.h"
#include "Range_Check.h"
#include "Change_Tracker.h"
#include "DCI_NV_Mem.h"
#include "Bit.hpp"
#include "DCI_Lock.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
const DCI_CB_MSK_TD DCI_Patron::SKIP_LENGTH_CHECKING_MSK =
	( DCI_ACCESS_GET_LENGTH_CMD_MSK | DCI_ACCESS_GET_ENUM_LIST_LEN_CMD_MSK |
	  DCI_ACCESS_GET_ENUM_CMD_MSK | DCI_ACCESS_RAM_TO_INIT_CMD_MSK |
	  DCI_ACCESS_INIT_TO_RAM_CMD_MSK | DCI_ACCESS_DEFAULT_TO_RAM_CMD_MSK |
	  DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD_MSK | DCI_ACCESS_DEFAULT_TO_INIT_CMD_MSK );



/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Patron::DCI_Patron( bool correct_out_of_range_sets, BF::DCI_Lock* dci_lock,
						Change_Tracker const* change_tracker_to_avoid ) :
	m_change_tracker( change_tracker_to_avoid ),
	m_correct_out_of_range_sets( correct_out_of_range_sets ),
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
DCI_ERROR_TD DCI_Patron::Data_Access( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_CB_RET_TD cback_return = DCI_CBACK_RET_PROCESS_NORMALLY;
	DCI_DATA_BLOCK_TD const* data_block;
	DCI_RANGE_BLOCK const* range_block = nullptr;
	DCI_ENUM_COUNT_TD enum_count = 0U;

	data_block = DCI::Get_Data_Block( access_struct->data_id );
	DCI_CBACK_BLOCK_TD* cback_block = DCI::Get_CBACK_Block( data_block->cback_block_index );

	if ( data_block->range_block_index != DCI::INVALID_INDEX )
	{
		range_block = DCI::Get_Range_Block( data_block->range_block_index );
		if ( range_block != nullptr )
		{
			enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
		}
	}

	if ( data_block != DCI::UNDEFINED_DCID )
	{
		if ( access_struct->data_access.length == 0U )
		{
			access_struct->data_access.offset = ACCESS_OFFSET_NONE;
			switch ( access_struct->command )
			{
				case DCI_ACCESS_GET_LENGTH_CMD:
					access_struct->data_access.length =
						static_cast<uint16_t>( sizeof( DCI_LENGTH_TD ) );
					break;

				case DCI_ACCESS_GET_ENUM_LIST_LEN_CMD:
					access_struct->data_access.length =
						static_cast<uint16_t>( sizeof( uint16_t ) );
					break;

				case DCI_ACCESS_GET_ENUM_CMD:
					if ( ( range_block != nullptr ) && ( enum_count != 0xFFFF ) )
					{
						access_struct->data_access.length =
							static_cast<DCI_LENGTH_TD>
							( enum_count
							  *
							  DCI::Get_Datatype_Size( data_block->datatype ) );
					}
					break;

				case DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD:
					access_struct->data_access.length =
						static_cast<uint16_t>( sizeof( DCI_PATRON_ATTRIB_SUPPORT_TD ) );
					break;

				default:
					access_struct->data_access.length = *data_block->length_ptr;
					break;
			}
		}
		if ( ( ( DCI_Access_Cmd_Mask( access_struct->command ) & SKIP_LENGTH_CHECKING_MSK ) !=
			   0U ) ||
			 ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
			   *data_block->length_ptr ) )
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

					case DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD:
						return_status = Get_Attrib_Support_Data( access_struct, data_block );
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

					case DCI_ACCESS_GET_MIN_CMD:
						return_status = Get_Min_Data( access_struct, data_block );
						break;

					case DCI_ACCESS_GET_MAX_CMD:
						return_status = Get_Max_Data( access_struct, data_block );
						break;

					case DCI_ACCESS_GET_ENUM_LIST_LEN_CMD:
						return_status = Get_Enum_List_Length( access_struct, data_block );
						break;

					case DCI_ACCESS_GET_ENUM_CMD:
						return_status = Get_Enum_Data( access_struct, data_block );
						break;

					case DCI_ACCESS_RAM_TO_INIT_CMD:
						return_status = Load_Current_To_Init( access_struct, data_block );
						break;

					case DCI_ACCESS_INIT_TO_RAM_CMD:
						return_status = Load_Init_To_Current( access_struct, data_block );
						break;

					case DCI_ACCESS_DEFAULT_TO_INIT_CMD:
						return_status = Load_Default_To_Init( access_struct, data_block );
						break;

					case DCI_ACCESS_DEFAULT_TO_RAM_CMD:
						return_status = Load_Default_To_Current( access_struct, data_block );
						break;

					default:
						return_status = DCI_ERR_INVALID_COMMAND;
						break;
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
		else
		{
			return_status = DCI_ERR_INVALID_DATA_LENGTH;
		}
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
DCI_ERROR_TD DCI_Patron::Get_Length( DCI_ID_TD data_id, DCI_LENGTH_TD* length ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( data_id );

	if ( data_block != NULL )
	{
#ifndef DCI_ALL_LENGTHS_CONST
		Push_TGINT();
#endif

		*length = *data_block->length_ptr;

#ifndef DCI_ALL_LENGTHS_CONST
		Pop_TGINT();
#endif
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
DCI_ERROR_TD DCI_Patron::Get_Attribute_Support( DCI_ID_TD data_id,
												DCI_PATRON_ATTRIB_SUPPORT_TD* attrib_support ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( data_id );

	if ( data_block != NULL )
	{
		*attrib_support = data_block->patron_attrib_info;
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
DCI_ERROR_TD DCI_Patron::Get_Datatype( DCI_ID_TD data_id, DCI_DATATYPE_TD* datatype ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;

	if ( data_id < DCI_TOTAL_DCIDS )
	{
		*datatype = dci_data_blocks[data_id].datatype;
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
DCI_ERROR_TD DCI_Patron::Get_Datatype_Length( DCI_ID_TD data_id,
											  DCI_DATATYPE_SIZE_TD* datatype_size ) const
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;

	data_block = DCI::Get_Data_Block( data_id );

	if ( data_block != NULL )
	{
		*datatype_size = DCI::Get_Datatype_Size( data_block->datatype );
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
DCI_ERROR_TD DCI_Patron::Get_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;

	if ( !m_dci_lock->Locked( data_block->dcid, BF::DCI_Lock::LOCK_READ_MASK ) )
	{
		uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
		uint8_t* src_ptr = data_block->ram_ptr;

		DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
						access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_VALUE_LOCKED;
	}
	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Set_RAM_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block )
{
	uint8_t* src_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	uint8_t* dest_ptr = data_block->ram_ptr;
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_DATATYPE_SIZE_TD datatype_size;
	bool changed_value = false;
	bool test_range_data = false;
	bool test_enum_data = false;
	bool good_offset_alignment;
	bool good_length_alignment;

	if ( !m_dci_lock->Locked( data_block->dcid, BF::DCI_Lock::LOCK_WRITE_MASK ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
		{
			test_range_data = BF_Lib::Bit::Test( data_block->owner_attrib_info,
												 DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
			test_enum_data = BF_Lib::Bit::Test( data_block->owner_attrib_info,
												DCI_OWNER_ATTRIB_INFO_ENUM_VAL );
			if ( test_range_data || test_enum_data )
			{
				datatype_size = DCI::Get_Datatype_Size( data_block->datatype );
				good_offset_alignment = Good_Alignment( access_struct->data_access.offset,
														datatype_size );
				good_length_alignment = Good_Alignment( access_struct->data_access.length,
														datatype_size );
				if ( ( good_offset_alignment == true ) && ( good_length_alignment == true ) )
				{
					if ( !Range_Check::Check_Range( &access_struct->data_access, data_block,
													m_correct_out_of_range_sets ) )
					{
						return_error = DCI_ERR_EXCEEDS_RANGE;
					}
					else
					{
						changed_value =
							DCI::Copy_Data_Check_For_Change(
								&dest_ptr[access_struct->data_access.offset], src_ptr,
								access_struct->data_access.length );
					}
				}
				else
				{
					changed_value = DCI::Copy_Data_Check_For_Change(
						&dest_ptr[access_struct->data_access.offset],
						src_ptr, access_struct->data_access.length );

					DCI_ACCESS_DATA_TD data_access;
					data_access.data = dest_ptr;
					data_access.offset = ACCESS_OFFSET_NONE;
					data_access.length = *data_block->length_ptr;

					if ( !Range_Check::Check_Range( &data_access, data_block, true ) )
					{
						return_error = DCI_ERR_EXCEEDS_RANGE;
					}
				}
			}
			else
			{
				changed_value = DCI::Copy_Data_Check_For_Change(
					&dest_ptr[access_struct->data_access.offset],
					src_ptr, access_struct->data_access.length );
			}

			if ( return_error == DCI_ERR_NO_ERROR )
			{
				Change_Tracker::ID_Change( access_struct->data_id, changed_value,
										   Change_Tracker::RAM_ATTRIB, m_change_tracker );
			}
		}
		else
		{
			return_error = DCI_ERR_RAM_READ_ONLY;
		}
	}
	else
	{
		return_error = DCI_ERR_VALUE_LOCKED;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Length_Data( DCI_ACCESS_ST_TD* access_struct,
										  DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	uint16_t const* src_ptr = data_block->length_ptr;

	if ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
		 static_cast<DCI_LENGTH_TD>( sizeof( DCI_LENGTH_TD ) ) )
	{
		DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
						access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_INVALID_DATA_LENGTH;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Attrib_Support_Data( DCI_ACCESS_ST_TD* access_struct,
												  DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	uint16_t const* src_ptr = &data_block->patron_attrib_info;

	if ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
		 static_cast<DCI_LENGTH_TD>( sizeof( DCI_PATRON_ATTRIB_SUPPORT_TD ) ) )
	{
		DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
						access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_INVALID_DATA_LENGTH;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Init_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;

	if ( !m_dci_lock->Locked( data_block->dcid, BF::DCI_Lock::LOCK_READ_MASK ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
		{
			return_error = DCI_NV_Mem::Patron_Read( &access_struct->data_access, data_block );
		}
		else
		{
			return_error = DCI_ERR_NV_NOT_AVAILABLE;
		}
	}
	else
	{
		return_error = DCI_ERR_VALUE_LOCKED;
	}
	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Set_Init_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block )
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	bool changed_value;

	if ( !m_dci_lock->Locked( data_block->dcid, BF::DCI_Lock::LOCK_WRITE_MASK ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
		{
			if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
			{
				return_error = DCI_NV_Mem::Patron_Write( &access_struct->data_access, data_block,
														 m_correct_out_of_range_sets,
														 &changed_value );

				if ( return_error == DCI_ERR_NO_ERROR )
				{
					Change_Tracker::ID_Change( access_struct->data_id, changed_value,
											   Change_Tracker::NV_ATTRIB, m_change_tracker );
				}
			}
			else
			{
				return_error = DCI_ERR_NV_READ_ONLY;
			}
		}
		else
		{
			return_error = DCI_ERR_NV_NOT_AVAILABLE;
		}
	}
	else
	{
		return_error = DCI_ERR_VALUE_LOCKED;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Default_Data( DCI_ACCESS_ST_TD* access_struct,
										   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_RAM_DATA_TD* dest_ptr =
		reinterpret_cast<DCI_RAM_DATA_TD*>( access_struct->data_access.data );
	DCI_RAM_DATA_TD const* src_ptr = data_block->default_val;
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;

	// uint_fast16_t src_index;

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
	{
		// NOTE NOTE NOTE the commented section below is no longer needed because we are stuffing the
		// default array with the
		// the proper amount of data.  This eliminated the need to "fake" the default.  The heavy
		// lifting is done by
		// a VBA macro.
		////We do the following because the default is actually a single value.  This is an issue
		// with array values.
		////The default can technically appear to the outside world as an array.  It should be
		// treated just like
		////the RAM value.  Whether it be an array or not.  In this case it is not.
		// src_index = access_struct->data_access.offset;
		// for ( uint_fast16_t i = 0U; i < access_struct->data_access.length; i++ )
		// {
		// dest_ptr[i] = src_ptr[src_index];
		// src_index++;
		// if ( src_index >= DCI::Get_Datatype_Size( data_block->datatype ) )
		// {
		// src_index = 0U;
		// }
		// }
		DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
						access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_DEFAULT_NOT_AVAILABLE;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Min_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_RAM_DATA_TD* dest_ptr =
		reinterpret_cast<DCI_RAM_DATA_TD*>( access_struct->data_access.data );
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_LENGTH_TD src_index;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
							DCI_PATRON_ATTRIB_RANGE_DATA ) && ( range_block != nullptr ) )
	{
		// We do the following because the range is actually a single value.  This is an issue with
		// array values.
		// The range can technically appear to the outside world as an array.  It should be treated
		// just like
		// the RAM value.  Whether it be an array or not.
		DCI_RAM_DATA_TD const* src_ptr = range_block->min;
		src_index = Trim_Offset_To_Single_Datatype( data_block, access_struct->data_access.offset );
		for ( DCI_LENGTH_TD i = 0U; i < access_struct->data_access.length; i++ )
		{
			dest_ptr[i] = src_ptr[src_index];
			src_index++;
			if ( src_index >= DCI::Get_Datatype_Size( data_block->datatype ) )
			{
				src_index = 0U;
			}
		}
		// DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
		// access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_RANGE_NOT_AVAILABLE;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Max_Data( DCI_ACCESS_ST_TD* access_struct,
									   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_RAM_DATA_TD* dest_ptr =
		reinterpret_cast<DCI_RAM_DATA_TD*>( access_struct->data_access.data );
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_LENGTH_TD src_index;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( data_block->range_block_index );

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
							DCI_PATRON_ATTRIB_RANGE_DATA ) && ( range_block != nullptr ) )
	{
		// We do the following because the range is actually a single value.  This is an issue with
		// array values.
		// The range can technically appear to the outside world as an array.  It should be treated
		// just like
		// the RAM value.  Whether it be an array or not.
		DCI_RAM_DATA_TD const* src_ptr = range_block->max;
		src_index = Trim_Offset_To_Single_Datatype( data_block, access_struct->data_access.offset );
		for ( DCI_LENGTH_TD i = 0U; i < access_struct->data_access.length; i++ )
		{
			dest_ptr[i] = src_ptr[src_index];
			src_index++;
			if ( src_index >= DCI::Get_Datatype_Size( data_block->datatype ) )
			{
				src_index = 0U;
			}
		}
		// DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
		// access_struct->data_access.length );
	}
	else
	{
		return_error = DCI_ERR_RANGE_NOT_AVAILABLE;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Enum_List_Length( DCI_ACCESS_ST_TD* access_struct,
											   DCI_DATA_BLOCK_TD const* data_block ) const
{
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	uint8_t* src_ptr;
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_LENGTH_TD list_byte_length;
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
	if ( ( enum_count != 0xFFFF ) && ( range_block != nullptr ) &&
		 BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_ENUM_DATA ) )
	{
		if ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
			 static_cast<DCI_LENGTH_TD>( sizeof( DCI_LENGTH_TD ) ) )
		{
			list_byte_length = static_cast<DCI_LENGTH_TD>(
				enum_count *
				DCI::Get_Datatype_Size( data_block->datatype ) );
			src_ptr = reinterpret_cast<uint8_t*>( &list_byte_length );
			DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
							access_struct->data_access.length );
		}
		else
		{
			return_error = DCI_ERR_INVALID_DATA_LENGTH;
		}
	}
	else
	{
		return_error = DCI_ERR_ENUM_NOT_AVAILABLE;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Get_Enum_Data( DCI_ACCESS_ST_TD* access_struct,
										DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	uint8_t const* src_ptr;
	DCI_LENGTH_TD list_byte_length;
	DCI_RANGE_BLOCK const* range_block = nullptr;
	DCI_ENUM_COUNT_TD enum_count = 0U;
	DCI_ENUM_BLOCK const* enum_block = nullptr;

	if ( data_block->range_block_index != DCI::INVALID_INDEX )
	{
		range_block = DCI::Get_Range_Block( data_block->range_block_index );
		if ( range_block != nullptr )
		{
			enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
			enum_block = DCI::Get_Enum_Block( range_block->enum_block_index );
		}
	}
	if ( ( enum_count != 0xFFFF ) && ( range_block != nullptr ) &&
		 BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_ENUM_DATA ) )
	{
		list_byte_length = static_cast<DCI_LENGTH_TD>( enum_count *
													   DCI::Get_Datatype_Size( data_block->datatype ) );
		if ( ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
			   list_byte_length ) && ( enum_block != nullptr ) )
		{
			src_ptr = ( enum_block->enum_list );
			DCI::Copy_Data( dest_ptr, &src_ptr[access_struct->data_access.offset],
							access_struct->data_access.length );
		}
		else
		{
			return_error = DCI_ERR_INVALID_DATA_LENGTH;
		}
	}
	else
	{
		return_error = DCI_ERR_ENUM_NOT_AVAILABLE;
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Load_Current_To_Init( DCI_ACCESS_ST_TD* access_struct,
											   DCI_DATA_BLOCK_TD const* data_block )
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_DATA_TD data_access;
	bool data_changed = false;

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
		{
			data_access.data = data_block->ram_ptr;
			data_access.offset = ACCESS_OFFSET_NONE;
			data_access.length = *data_block->length_ptr;
			return_error = DCI_NV_Mem::Patron_Write( &data_access, data_block,
													 m_correct_out_of_range_sets, &data_changed );
		}
		else
		{
			return_error = DCI_ERR_NV_READ_ONLY;
		}
	}
	else
	{
		return_error = DCI_ERR_NV_NOT_AVAILABLE;
	}

	if ( return_error == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, data_changed,
								   Change_Tracker::NV_ATTRIB, m_change_tracker );
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Load_Default_To_Init( DCI_ACCESS_ST_TD* access_struct,
											   DCI_DATA_BLOCK_TD const* data_block )
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_DATA_TD data_access;
	bool data_changed = false;

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
		{
			if ( BF_Lib::Bit::Test( data_block->patron_attrib_info,
									DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
			{
				data_access.data = const_cast<DCI_RAM_DATA_TD*>
					( data_block->default_val );
				data_access.offset = ACCESS_OFFSET_NONE;
				data_access.length = *data_block->length_ptr;
				return_error = DCI_NV_Mem::Patron_Write( &data_access, data_block,
														 m_correct_out_of_range_sets,
														 &data_changed );
			}
			else
			{
				return_error = DCI_ERR_NV_READ_ONLY;
			}
		}
		else
		{
			return_error = DCI_ERR_NV_NOT_AVAILABLE;
		}
	}
	else
	{
		return_error = DCI_ERR_DEFAULT_NOT_AVAILABLE;
	}

	if ( return_error == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, data_changed,
								   Change_Tracker::NV_ATTRIB, m_change_tracker );
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Load_Default_To_Current( DCI_ACCESS_ST_TD* access_struct,
												  DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	bool data_changed;

	if ( access_struct == static_cast<DCI_ACCESS_ST_TD*>( nullptr ) )
	{}

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
	{
		data_changed = DCI::Copy_Data_Check_For_Change( data_block->ram_ptr,
														data_block->default_val,
														*data_block->length_ptr );

		Change_Tracker::ID_Change( data_block->dcid, data_changed,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );
	}
	else
	{
		return_error = DCI_ERR_DEFAULT_NOT_AVAILABLE;
	}

	// This is redundant and forced a change track.  This would be incorrect behavior.
	// if ( return_error == DCI_ERR_NO_ERROR )
	// {
	// Change_Tracker::ID_Change( access_struct->data_id, true,
	// Change_Tracker::RAM_ATTRIB, m_change_tracker );
	// }

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Load_Init_To_Current( DCI_ACCESS_ST_TD* access_struct,
											   DCI_DATA_BLOCK_TD const* data_block ) const
{
	DCI_ERROR_TD return_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_DATA_TD data_access;
	bool data_changed = false;

	if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
	{
		if ( BF_Lib::Bit::Test( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
		{
			data_access.data = data_block->ram_ptr;
			data_access.offset = ACCESS_OFFSET_NONE;
			data_access.length = *data_block->length_ptr;
			return_error = DCI_NV_Mem::Patron_Read( &data_access, data_block, &data_changed );
		}
		else
		{
			return_error = DCI_ERR_RAM_READ_ONLY;
		}
	}
	else
	{
		return_error = DCI_ERR_NV_NOT_AVAILABLE;
	}

	if ( return_error == DCI_ERR_NO_ERROR )
	{
		Change_Tracker::ID_Change( access_struct->data_id, data_changed,	// data_changed was true.
																			// In case issues arise.
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD DCI_Patron::Error_Check_Access( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_DATA_BLOCK_TD const* data_block;
	bool test_range_data = false;
	bool test_enum_data = false;
	DCI_DATATYPE_SIZE_TD datatype_size;
	bool good_offset_alignment;
	bool good_length_alignment;

	data_block = DCI::Get_Data_Block( access_struct->data_id );

	if ( data_block != NULL )
	{
		if ( access_struct->data_access.length == 0U )
		{
			access_struct->data_access.length = *data_block->length_ptr;
			access_struct->data_access.offset = ACCESS_OFFSET_NONE;
		}
		if ( ( access_struct->data_access.length + access_struct->data_access.offset ) <=
			 *data_block->length_ptr )
		{
			switch ( access_struct->command )
			{
				case DCI_ACCESS_GET_RAM_CMD:
					break;

				case DCI_ACCESS_SET_RAM_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
					{
						return_status = DCI_ERR_RAM_READ_ONLY;
					}
					else
					{
						test_range_data = BF_Lib::Bit::Test( data_block->owner_attrib_info,
															 DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
						test_enum_data = BF_Lib::Bit::Test( data_block->owner_attrib_info,
															DCI_OWNER_ATTRIB_INFO_ENUM_VAL );
						if ( test_range_data || test_enum_data )
						{
							datatype_size = DCI::Get_Datatype_Size( data_block->datatype );
							good_offset_alignment = Good_Alignment(
								access_struct->data_access.offset,
								datatype_size );
							good_length_alignment = Good_Alignment(
								access_struct->data_access.length,
								datatype_size );
							if ( ( good_offset_alignment == true ) &&
								 ( good_length_alignment == true ) )
							{
								if ( !Range_Check::Check_Range( &access_struct->data_access,
																data_block, false ) )
								{
									return_status = DCI_ERR_EXCEEDS_RANGE;
								}
							}
						}
					}
					break;

				case DCI_ACCESS_GET_LENGTH_CMD:
					if ( ( access_struct->data_access.length != sizeof( DCI_LENGTH_TD ) ) ||
						 ( access_struct->data_access.offset != 0U ) )
					{
						return_status = DCI_ERR_INVALID_DATA_LENGTH;
					}
					break;

				case DCI_ACCESS_GET_INIT_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						return_status = DCI_ERR_NV_NOT_AVAILABLE;
					}
					break;

				case DCI_ACCESS_RAM_TO_INIT_CMD:
				case DCI_ACCESS_SET_INIT_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						return_status = DCI_ERR_NV_NOT_AVAILABLE;
					}
					else if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
					{
						return_status = DCI_ERR_NV_READ_ONLY;
					}
					else
					{
						// do nothing
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

				case DCI_ACCESS_GET_ENUM_LIST_LEN_CMD:
				case DCI_ACCESS_GET_ENUM_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_ENUM_DATA ) )
					{
						return_status = DCI_ERR_ENUM_NOT_AVAILABLE;
					}
					break;


				case DCI_ACCESS_INIT_TO_RAM_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						return_status = DCI_ERR_NV_NOT_AVAILABLE;
					}
					else if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
					{
						return_status = DCI_ERR_RAM_READ_ONLY;
					}
					else
					{
						// do nothing
					}
					break;

				case DCI_ACCESS_DEFAULT_TO_INIT_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
					{
						return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
					}
					else if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_NVMEM_DATA ) )
					{
						return_status = DCI_ERR_NV_NOT_AVAILABLE;
					}
					else if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
					{
						return_status = DCI_ERR_NV_READ_ONLY;
					}
					else
					{
						// do nothing
					}
					break;

				case DCI_ACCESS_DEFAULT_TO_RAM_CMD:
					if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
											 DCI_PATRON_ATTRIB_DEFAULT_DATA ) )
					{
						return_status = DCI_ERR_DEFAULT_NOT_AVAILABLE;
					}
					else if ( !BF_Lib::Bit::Test( data_block->patron_attrib_info,
												  DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
					{
						return_status = DCI_ERR_RAM_READ_ONLY;
					}
					else
					{
						// do nothing
					}
					break;

				default:
					return_status = DCI_ERR_INVALID_COMMAND;
					break;
			}
		}
		else
		{
			return_status = DCI_ERR_INVALID_DATA_LENGTH;
		}
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
DCI_LENGTH_TD DCI_Patron::Trim_Offset_To_Single_Datatype( DCI_DATA_BLOCK_TD const* data_block,
														  DCI_LENGTH_TD offset_to_trim ) const
{
	DCI_LENGTH_TD trimmed_offset;

	// The following uses "magic" numbers but I figured in this case the size of a datatype is not
	// very magic.  This can be understood.
	switch ( DCI::Get_Datatype_Size( data_block->datatype ) )
	{
		case 2:	// sizeof( uint16_t ):
			trimmed_offset = offset_to_trim & 0x01U;
			break;

		case 4:	// sizeof( uint32_t ):
			trimmed_offset = offset_to_trim & 0x03U;
			break;

		case 8:	// sizeof( uint64_t ):
			trimmed_offset = offset_to_trim & 0x07U;
			break;

		case 1:	// sizeof( uint8_t )
		default:
			trimmed_offset = 0U;	// offset_to_trim & 0x00U;
			break;

	}

	return ( trimmed_offset );
}
