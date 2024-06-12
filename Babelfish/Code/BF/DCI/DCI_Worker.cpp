/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DCI_Worker.h"
#include "Range_Check.h"
#include "StdLib_MV.h"

#include "OS_Tasker.h"
#include "DCI.h"
#include "DCI_NV_Mem.h"
#include "DCI_Source_IDs.h"
#include "Bit.hpp"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Worker::DCI_Worker( DCI_ID_TD dcid, worker_permission_t privileges ) :
	m_privileges( privileges ),
	m_data_block( static_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
{
	BF_ASSERT( dcid < DCI_TOTAL_DCIDS );
	m_data_block = DCI::Get_Data_Block( dcid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Worker::~DCI_Worker()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Worker::Change_Permissions( DCI_Worker::worker_permission_t permissions )
{
	m_privileges = permissions;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out( DCI_DATA_PASS_TD* ram_data ) const
{
	BF_ASSERT( m_data_block->ram_ptr != NULL );
	DCI::Copy_Data( ram_data, m_data_block->ram_ptr,
					*m_data_block->length_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In( DCI_DATA_PASS_TD const* data,
						   Change_Tracker const* change_tracker_to_avoid )
{
	bool status = false;
	DCI_ACCESS_DATA_TD data_access;
	bool change = false;
	bool test_range_trusted = false;
	bool test_info_range_val = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	test_range_trusted = !BF_Lib::Bit::Test( m_privileges, RANGE_TRUSTED );
	test_info_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
	if ( test_range_trusted && test_info_range_val )
	{
		data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
		data_access.length = *m_data_block->length_ptr;
		data_access.offset = ACCESS_OFFSET_NONE;
		if ( Range_Check::Check_Range( &data_access, m_data_block, false ) )
		{
			status = true;
			change = DCI::Copy_Data_Check_For_Change( m_data_block->ram_ptr,
													  data, *m_data_block->length_ptr );
		}
	}
	else
	{
		status = true;
		change = DCI::Copy_Data_Check_For_Change( m_data_block->ram_ptr, data,
												  *m_data_block->length_ptr );
	}
	if ( status == true )
	{
		Change_Tracker::ID_Change( m_data_block->dcid, change,
								   Change_Tracker::RAM_ATTRIB, change_tracker_to_avoid );
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Init( DCI_DATA_PASS_TD const* data,
								Change_Tracker const* change_tracker_to_avoid )
{
	bool status = false;
	DCI_ACCESS_DATA_TD data_access;
	bool data_changed;
	bool test_info_init_val = false;
	bool test_init_write = false;
	bool test_range_trusted = false;
	bool test_info_range_val = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	test_info_init_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL );
	test_init_write = BF_Lib::Bit::Test( m_privileges, INIT_WRITE );
	if ( test_info_init_val && test_init_write )
	{
		test_range_trusted = !BF_Lib::Bit::Test( m_privileges, RANGE_TRUSTED );
		test_info_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
		if ( test_range_trusted && test_info_range_val )
		{
			data_access.data = const_cast<DCI_DATA_PASS_TD*>( data );
			data_access.length = *m_data_block->length_ptr;
			data_access.offset = ACCESS_OFFSET_NONE;
			if ( Range_Check::Check_Range( &data_access, m_data_block, false ) )
			{
				status = DCI_NV_Mem::Owner_Write( data, m_data_block, &data_changed );
				if ( status == true )
				{
					Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
											   Change_Tracker::NV_ATTRIB, change_tracker_to_avoid );
				}
			}
		}
		else
		{
			status = DCI_NV_Mem::Owner_Write( data, m_data_block, &data_changed );
			if ( status == true )
			{
				Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
										   Change_Tracker::NV_ATTRIB, change_tracker_to_avoid );
			}
		}
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Init( DCI_DATA_PASS_TD* ram_data ) const
{
	bool success = false;
	bool test_info_default_val = false;
	bool test_info_init_val;

	test_info_init_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL );
	if ( ( m_data_block->ram_ptr != NULL ) && test_info_init_val )
	{
		success = DCI_NV_Mem::Owner_Read( ram_data, m_data_block );	// ( NV_Mem::Read( ram_data, m_data_block->nv_loc,
																	// *m_data_block->length_ptr, true );
		test_info_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		if ( ( success != true ) && test_info_default_val &&
			 ( m_data_block->default_val != reinterpret_cast<DCI_DEF_DATA_TD const*>( nullptr ) ) )
		{
			DCI::Copy_Data( ram_data, m_data_block->default_val, *m_data_block->length_ptr );
			success = true;
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Default( DCI_DATA_PASS_TD const* ram_data,
								   Change_Tracker const* change_tracker_to_avoid )
{
	bool status = false;
	bool perform_checkin = false;
	bool change = false;
	DCI_DEF_DATA_TD* dest_ptr;
	DCI_ACCESS_DATA_TD data_access;
	bool test_info_default_val = false;
	bool test_info_rw_default_val = false;
	bool test_default_write = false;
	bool test_range_trusted = false;
	bool test_info_range_val = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	test_info_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
	test_info_rw_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
												  DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
	test_default_write = BF_Lib::Bit::Test( m_privileges, DEFAULT_WRITE );
	if ( test_info_default_val && test_info_rw_default_val && test_default_write )
	{
		test_range_trusted = !BF_Lib::Bit::Test( m_privileges, RANGE_TRUSTED );
		test_info_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
		if ( test_range_trusted && test_info_range_val )
		{
			data_access.data = const_cast<DCI_DATA_PASS_TD*>( ram_data );
			data_access.length = *m_data_block->length_ptr;
			data_access.offset = ACCESS_OFFSET_NONE;
			if ( Range_Check::Check_Range( &data_access, m_data_block, false ) )
			{
				perform_checkin = true;
			}
		}
		else
		{
			perform_checkin = true;
		}

		if ( true == perform_checkin )
		{
			Push_TGINT();
			dest_ptr = const_cast<DCI_DEF_DATA_TD*>( m_data_block->default_val );
			change = DCI::Copy_Data_Check_For_Change( dest_ptr,
													  ram_data, *m_data_block->length_ptr );
			Pop_TGINT();
			status = true;
			Change_Tracker::ID_Change( m_data_block->dcid, change,
									   Change_Tracker::DEFAULT_ATTRIB, change_tracker_to_avoid );
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Offset( DCI_DATA_PASS_TD const* ram_data,
								  Change_Tracker const* change_tracker_to_avoid, DCI_LENGTH_TD offset,
								  DCI_LENGTH_TD length )
{
	uint8_t* dest_ptr;
	bool status = false;
	bool change;

	// TODO:  These permissions need to be fixed up.
	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, RANGE_TRUSTED ) )
	{
		if ( length == ACCESS_LENGTH_SET_ALL )
		{
			length = ( *m_data_block->length_ptr );
		}

		if ( ( offset + length ) <= ( *m_data_block->length_ptr ) )
		{
			dest_ptr = reinterpret_cast<uint8_t*>( m_data_block->ram_ptr );
			change = DCI::Copy_Data_Check_For_Change( &dest_ptr[offset], ram_data, length );

			Change_Tracker::ID_Change( m_data_block->dcid, change,
									   Change_Tracker::RAM_ATTRIB, change_tracker_to_avoid );

			status = true;
		}
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Offset( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD offset,
								   DCI_LENGTH_TD length ) const
{
	uint8_t* src_ptr;
	bool success = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( length == ACCESS_LENGTH_GET_ALL )
	{
		length = ( *m_data_block->length_ptr );
	}

	if ( ( offset + length ) <= ( *m_data_block->length_ptr ) )
	{
		src_ptr = reinterpret_cast<uint8_t*>( m_data_block->ram_ptr );
		DCI::Copy_Data( ram_data, &src_ptr[offset], length );
		success = true;
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Init_Offset( DCI_DATA_PASS_TD const* ram_data,
									   Change_Tracker const* change_tracker_to_avoid, DCI_LENGTH_TD offset,
									   DCI_LENGTH_TD length )
{
	bool status = false;
	bool changed_value = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, INIT_WRITE ) )
	{
		if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
		{
			status = DCI_NV_Mem::Owner_Write( ram_data, m_data_block, &changed_value, offset,
											  length );
			if ( status == true )
			{
				Change_Tracker::ID_Change( m_data_block->dcid, changed_value,
										   Change_Tracker::NV_ATTRIB, change_tracker_to_avoid );
			}
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Init_Offset( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD offset,
										DCI_LENGTH_TD length ) const
{
	bool success = false;
	bool test_info_default_val = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( length == ACCESS_LENGTH_GET_ALL )
	{
		length = *m_data_block->length_ptr;
	}

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		success = DCI_NV_Mem::Owner_Read( ram_data, m_data_block, static_cast<bool*>( nullptr ), offset, length );
		test_info_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
												   DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		if ( ( success != true ) && test_info_default_val &&
			 ( m_data_block->default_val != reinterpret_cast<DCI_DEF_DATA_TD const*>( nullptr ) ) )
		{
			DCI::Copy_Data( ram_data, m_data_block->default_val, *m_data_block->length_ptr );
			success = true;
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Default_Offset( DCI_DATA_PASS_TD const* default_data,
										  Change_Tracker const* change_tracker_to_avoid, DCI_LENGTH_TD offset,
										  DCI_LENGTH_TD length )
{
	bool status = false;
	bool change = false;
	bool test_info_default_val = false;
	bool test_info_rw_default_val = false;
	uint8_t* dest_ptr;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, DEFAULT_WRITE ) )
	{
		test_info_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
												   DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		test_info_rw_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
													  DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
		if ( test_info_default_val && test_info_rw_default_val )
		{
			if ( length == ACCESS_LENGTH_SET_ALL )
			{
				length = *m_data_block->length_ptr;
			}

			if ( ( length + offset ) <= ( *m_data_block->length_ptr ) )
			{
				Push_TGINT();
				// This is done deliberately to get at the default which we have determined must by
				// in the ram space (otherwise the macro is broken).
				dest_ptr = const_cast<uint8_t*>( m_data_block->default_val );
				change = DCI::Copy_Data_Check_For_Change( &dest_ptr[offset],
														  default_data, length );
				Pop_TGINT();
				status = true;
				Change_Tracker::ID_Change( m_data_block->dcid, change,
										   Change_Tracker::DEFAULT_ATTRIB, change_tracker_to_avoid );
			}
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Default( DCI_DATA_PASS_TD* default_data ) const
{
	bool success = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		Push_TGINT();
		DCI::Copy_Data( default_data, m_data_block->default_val,
						*m_data_block->length_ptr );
		Pop_TGINT();
		success = true;
	}

	return ( success );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Default_Offset( DCI_DATA_PASS_TD* default_data,
										   DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	uint8_t const* src_ptr;
	bool status = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		if ( ACCESS_LENGTH_SET_ALL == length )
		{
			length = *m_data_block->length_ptr;
		}
		if ( ( length + offset ) <= ( *m_data_block->length_ptr ) )
		{
			Push_TGINT();
			src_ptr = reinterpret_cast<uint8_t const*>( m_data_block->default_val );
			DCI::Copy_Data( default_data, &src_ptr[offset], length );
			Pop_TGINT();
			status = true;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Min( DCI_DATA_PASS_TD const* min,
							   Change_Tracker const* change_tracker_to_avoid )
{
	bool status = false;
	DCI_RANGE_DATA_TD* dest_ptr;
	bool test_info_range_val = false;
	bool test_info_rw_range_val = false;
	bool data_changed;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, RANGE_WRITE ) )
	{
		test_info_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
		test_info_rw_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
													DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL );
		if ( test_info_range_val && test_info_rw_range_val && ( range_block != nullptr ) )
		{
			dest_ptr = const_cast<DCI_RANGE_DATA_TD*>( range_block->min );
			data_changed = DCI::Copy_Data_Check_For_Change( dest_ptr, min, *m_data_block->length_ptr );
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::MIN_ATTRIB, change_tracker_to_avoid );
			status = true;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_In_Max( DCI_DATA_PASS_TD const* max,
							   Change_Tracker const* change_tracker_to_avoid )
{
	bool status = false;
	DCI_RANGE_DATA_TD* dest_ptr;
	bool test_info_range_val = false;
	bool test_info_rw_range_val = false;
	bool data_changed;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, RANGE_WRITE ) )
	{
		test_info_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
		test_info_rw_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
													DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL );
		if ( test_info_range_val && test_info_rw_range_val && ( range_block != nullptr ) )
		{
			dest_ptr = const_cast<DCI_RANGE_DATA_TD*>( range_block->max );
			data_changed = DCI::Copy_Data_Check_For_Change( dest_ptr, max,
															*m_data_block->length_ptr );
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::MAX_ATTRIB, change_tracker_to_avoid );
			status = true;
		}
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Min( DCI_DATA_PASS_TD* min ) const
{
	bool success = false;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
							DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) && ( range_block != nullptr ) )
	{
		DCI::Copy_Data( min, range_block->min, *m_data_block->length_ptr );
		success = true;
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Max( DCI_DATA_PASS_TD* max ) const
{
	bool success = false;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
							DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) && ( range_block != nullptr ) )
	{
		DCI::Copy_Data( max, range_block->max, *m_data_block->length_ptr );
		success = true;
	}
	return ( success );
}

// TODO: Start back up here.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Check_Out_Length( DCI_LENGTH_TD* length ) const
{

	BF_ASSERT( m_data_block->ram_ptr != NULL );

#ifndef DCI_ALL_LENGTHS_CONST
	Push_TGINT();
#endif

	*length = *m_data_block->length_ptr;

#ifndef DCI_ALL_LENGTHS_CONST
	Pop_TGINT();
#endif
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_LENGTH_TD DCI_Worker::Get_Length( void ) const
{
	DCI_LENGTH_TD return_length = 0U;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
#ifndef DCI_ALL_LENGTHS_CONST
	Push_TGINT();
#endif
	return_length = *m_data_block->length_ptr;
#ifndef DCI_ALL_LENGTHS_CONST
	Pop_TGINT();
#endif

	return ( return_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_DATATYPE_TD DCI_Worker::Get_Datatype( void ) const
{
	BF_ASSERT( m_data_block->ram_ptr != NULL );

	return ( m_data_block->datatype );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Load_Current_To_Init( Change_Tracker const* change_tracker_to_avoid ) const
{
	bool success = false;
	bool changed_value = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, INIT_WRITE ) )
	{
		if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
		{
			success = DCI_NV_Mem::Owner_Write( m_data_block->ram_ptr, m_data_block,
											   &changed_value );
			if ( success == true )
			{
				Change_Tracker::ID_Change( m_data_block->dcid, changed_value,
										   Change_Tracker::NV_ATTRIB, change_tracker_to_avoid );
			}
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Load_Default_To_Current( Change_Tracker const* change_tracker_to_avoid ) const
{
	bool success = false;
	bool data_changed = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		data_changed = DCI::Copy_Data_Check_For_Change( m_data_block->ram_ptr,
														m_data_block->default_val, *m_data_block->length_ptr );

		Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
								   Change_Tracker::RAM_ATTRIB, change_tracker_to_avoid );

		success = true;
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Load_Default_To_Init( Change_Tracker const* change_tracker_to_avoid ) const
{
	bool success = false;
	bool changed_data;
	bool test_info_default_val = false;
	bool test_info_init_val = false;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_privileges, INIT_WRITE ) )
	{
		test_info_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
												   DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		test_info_init_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL );
		if ( test_info_default_val && test_info_init_val )
		{
			success = DCI_NV_Mem::Owner_Write( m_data_block->default_val, m_data_block,
											   &changed_data );
			if ( success == true )
			{
				Change_Tracker::ID_Change( m_data_block->dcid, changed_data,
										   Change_Tracker::NV_ATTRIB, change_tracker_to_avoid );
			}
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Load_Init_To_Current( Change_Tracker const* change_tracker_to_avoid ) const
{
	bool success = false;
	bool data_changed;

	BF_ASSERT( m_data_block->ram_ptr != NULL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		success = DCI_NV_Mem::Owner_Read( m_data_block->ram_ptr, m_data_block,
										  &data_changed );

		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::RAM_ATTRIB, change_tracker_to_avoid );
		}
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Worker::Clear_Current_Val( Change_Tracker const* change_tracker_to_avoid ) const
{
	bool success = false;
	bool change;

	BF_ASSERT( m_data_block->ram_ptr != NULL );

	if ( !BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_READ_ONLY ) )
	{
		change = DCI::Clear_Data( m_data_block->ram_ptr, *m_data_block->length_ptr );
		Change_Tracker::ID_Change( m_data_block->dcid, change,
								   Change_Tracker::RAM_ATTRIB, change_tracker_to_avoid );
		success = true;
	}

	return ( success );
}
