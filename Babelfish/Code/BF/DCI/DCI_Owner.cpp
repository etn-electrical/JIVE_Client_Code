/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Owner.h"
#include "StdLib_MV.h"
#include "OS_Tasker.h"
#include "DCI.h"
#include "DCI_NV_Mem.h"
#include "DCI_Source_IDs.h"
#include "Bit.hpp"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
Change_Tracker* DCI_Owner::m_change_tracker = reinterpret_cast<Change_Tracker*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Owner::DCI_Owner( DCI_ID_TD data_id, DCI_DATA_PASS_TD* data_ptr ) :
	m_data_block( reinterpret_cast<DCI_DATA_BLOCK_TD*>( 0U ) )
#ifdef DEBUG_DCI_DATA
	,
	m_data( reinterpret_cast<BF_Lib::SUPER_DATA_TYPE*>( 0U ) )
#endif
{
	Init_Owner( data_id, data_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Owner::DCI_Owner( DCI_ID_TD data_id ) :
	m_data_block( reinterpret_cast<DCI_DATA_BLOCK_TD*>( 0U ) )
#ifdef DEBUG_DCI_DATA
	,
	m_data( reinterpret_cast<BF_Lib::SUPER_DATA_TYPE*>( 0U ) )
#endif
{
	Init_Owner( data_id, static_cast<DCI_DATA_PASS_TD*>( nullptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Owner::Init_Owner( DCI_ID_TD data_id, DCI_DATA_PASS_TD* data_ptr )
{
	uint8_t* u8_data = static_cast<uint8_t*>( data_ptr );
	bool test_default_val;
	bool test_rw_default_val;

	BF_ASSERT( data_id < DCI_TOTAL_DCIDS );

	m_data_block = DCI::Get_Data_Block( data_id );

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_READ_ONLY ) )
	{
		BF_ASSERT( m_data_block->ram_ptr != reinterpret_cast<DCI_RAM_DATA_TD*>( nullptr ) );
	}
	else	// Only allocate space for a value that is not predefined as a constant.
	{
		// Don't allocate space if the value already has a data loc assigned to it.
		BF_ASSERT( m_data_block->ram_ptr != reinterpret_cast<DCI_RAM_DATA_TD*>( nullptr ) );
		// Gotta decide what to do in this condition.
		// Essentially we had multiple requests for ownership of one ID value.
		// This is not necessarily a good thing.  It should work itself out somehow.
		// Gotta think about it more.
		if ( data_ptr != reinterpret_cast<DCI_DATA_PASS_TD*>( nullptr ) )
		{
			DCI::Copy_Data( m_data_block->ram_ptr,
							reinterpret_cast<DCI_RAM_DATA_TD*>( u8_data ), *m_data_block->length_ptr );
		}
	}

#ifdef DEBUG_DCI_DATA
	m_data =
		reinterpret_cast<BF_Lib::SUPER_DATA_TYPE*>( reinterpret_cast<void*>( m_data_block->
																			 ram_ptr ) );
#endif

	// If the initial value is available - load that into the ram ptr.
	// Else if the initial value is not present but the default is then we take the default and
	// load it into ram.
	// No point loading the default value if it is going to change later because it is ram based.
	// Else we will stick with the zero values that WERE loaded by the ram allocation.
	test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
	test_rw_default_val = !BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
											  DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		if ( Check_Out_Init( m_data_block->ram_ptr ) == false )		// If NV read failed
		{
			if ( test_default_val && test_rw_default_val )
			{
				( void ) Check_Out_Default( m_data_block->ram_ptr );
			}
		}
	}
	else if ( test_default_val && test_rw_default_val )
	{
		( void ) Check_Out_Default( m_data_block->ram_ptr );
	}
	else
	{
		// do nothing.
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Owner::~DCI_Owner( void )
{
	m_data_block = reinterpret_cast<DCI_DATA_BLOCK_TD*>( 0U );
#ifdef DEBUG_DCI_DATA
	m_data = reinterpret_cast<BF_Lib::SUPER_DATA_TYPE*>( 0U );
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( DCI_DATA_PASS_TD* ram_data ) const
{
	DCI::Copy_Data( reinterpret_cast<DCI_RAM_DATA_TD*>( ram_data ),
					m_data_block->ram_ptr, *m_data_block->length_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( DCI_DATA_PASS_TD const* ram_data ) const
{
	bool change = false;

	change = DCI::Copy_Data_Check_For_Change( m_data_block->ram_ptr,
											  ram_data, *m_data_block->length_ptr );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Offset( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD offset,
								  DCI_LENGTH_TD length ) const
{
	bool success = false;

	if ( length == ACCESS_LENGTH_GET_ALL )
	{
		length = ( *m_data_block->length_ptr );
	}

	if ( ( offset + length ) <= ( *m_data_block->length_ptr ) )
	{
		DCI::Copy_Data( ( ram_data ), &( ( m_data_block->ram_ptr )[offset] ), length );
		success = true;
	}
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Offset( DCI_DATA_PASS_TD const* ram_data,
								 DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	bool change = false;
	bool success = false;

	if ( length == ACCESS_LENGTH_SET_ALL )
	{
		length = ( *m_data_block->length_ptr );
	}

	if ( ( offset + length ) <= ( *m_data_block->length_ptr ) )
	{
		change = DCI::Copy_Data_Check_For_Change( &( ( m_data_block->ram_ptr )[offset] ),
												  ( ram_data ), length );

		Change_Tracker::ID_Change( m_data_block->dcid, change,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Index( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD index ) const
{
	DCI_LENGTH_TD offset;
	DCI_DATATYPE_SIZE_TD datatype_size;

	datatype_size = DCI::Get_Datatype_Size( m_data_block->datatype );

	BF_ASSERT( ( index * datatype_size ) < *m_data_block->length_ptr );

	switch ( datatype_size )
	{
		case 1U:
			offset = index;
			DCI::Copy_8BIT( ram_data, &( m_data_block->ram_ptr )[offset] );
			break;

		case 2U:
			offset = index << MULT_BY_2;
			DCI::Copy_16BIT( ram_data, &( m_data_block->ram_ptr )[offset] );
			break;

		case 4U:
			offset = index << MULT_BY_4;
			DCI::Copy_32BIT( ram_data, &( m_data_block->ram_ptr )[offset] );
			break;

		case 8U:		// 64Bit access does not exist yet.
			offset = index << MULT_BY_8;
			DCI::Copy_64BIT( ram_data, &( m_data_block->ram_ptr )[offset] );
			break;

		default:
			BF_ASSERT( false );
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule: 0-1-1
			 * @n PCLint: Warning 527: Unreachable code
			 * @n Justification: We don't return from the BF_ASSERT(false) call
			 *                   by design. The break is still needed for complete code.
			 */
			break;	/*lint !e527 */
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Index( DCI_DATA_PASS_TD const* ram_data, DCI_LENGTH_TD index ) const
{
	bool change = false;
	DCI_LENGTH_TD offset;
	DCI_DATATYPE_SIZE_TD datatype_size;

	datatype_size = DCI::Get_Datatype_Size( m_data_block->datatype );

	BF_ASSERT( ( index * datatype_size ) < *m_data_block->length_ptr );
	switch ( datatype_size )
	{
		case 1:
			offset = index;
			change = DCI::Copy_8BIT_Check_For_Change( &( m_data_block->ram_ptr )[offset],
													  ram_data );
			break;

		case 2:
			offset = index << MULT_BY_2;
			change = DCI::Copy_16BIT_Check_For_Change( &( m_data_block->ram_ptr )[offset],
													   ram_data );
			break;

		case 4:
			offset = index << MULT_BY_4;
			change = DCI::Copy_32BIT_Check_For_Change( &( m_data_block->ram_ptr )[offset],
													   ram_data );
			break;

		case 8:		// 64Bit access does not exist yet.
			offset = index << MULT_BY_8;
			change = DCI::Copy_64BIT_Check_For_Change( &( m_data_block->ram_ptr )[offset],
													   ram_data );
			break;

		default:
			BF_ASSERT( false );
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule: 0-1-1
			 * @n PCLint: Warning 527: Unreachable code
			 * @n Justification: We don't return from the BF_ASSERT(false) call
			 *                   by design. The break is still needed for complete code.
			 */
			break;	/*lint !e527 */
	}
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Null_String( char_t const* ram_data ) const
{
	bool change = false;
	DCI_LENGTH_TD temp_length = *m_data_block->length_ptr;

	change = DCI::Copy_Null_String_Data_Check_For_Change( m_data_block->ram_ptr, ram_data,
														  &temp_length );

	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Null_String( DCI_DATA_PASS_TD* ram_data, DCI_LENGTH_TD* length ) const
{
	DCI_LENGTH_TD temp_length = 0U;

	if ( ( length == NULL ) || ( *length > *m_data_block->length_ptr ) )
	{
		temp_length = *m_data_block->length_ptr;
	}

	DCI::Copy_Null_String_Data( ram_data, m_data_block->ram_ptr, &temp_length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( bool ram_data ) const
{
	bool change = false;
	dci_bool_t dci_bool;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == DCI_DTYPE_BOOL_SIZE );
#endif
	if ( ram_data == true )
	{
		dci_bool = DCI_BOOL_TRUE;
	}
	else
	{
		dci_bool = DCI_BOOL_FALSE;
	}

	change = DCI::Copy_8BIT_Check_For_Change( m_data_block->ram_ptr,
											  reinterpret_cast<DCI_RAM_DATA_TD*>( &dci_bool ) );

	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( uint8_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint8_t ) );

#endif
	change =
		DCI::Copy_8BIT_Check_For_Change( m_data_block->ram_ptr,
										 reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( uint16_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint16_t ) );
#endif

	change =
		DCI::Copy_16BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );

	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( uint32_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint32_t ) );
#endif

	change =
		DCI::Copy_32BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( uint64_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint64_t ) );
#endif

	change =
		DCI::Copy_64BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( int8_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int8_t ) );
#endif

	change =
		DCI::Copy_8BIT_Check_For_Change( m_data_block->ram_ptr,
										 reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( int16_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int16_t ) );
#endif

	change =
		DCI::Copy_16BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( int32_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int32_t ) );
#endif

	change =
		DCI::Copy_32BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( int64_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int64_t ) );
#endif

	change =
		DCI::Copy_64BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( float32_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( float32_t ) );
#endif

	change =
		DCI::Copy_32BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( BF_Lib::float64_t ram_data ) const
{
	bool change = false;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check in a datatype that
	/// is bigger than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( BF_Lib::float64_t ) );
#endif

	change =
		DCI::Copy_64BIT_Check_For_Change( m_data_block->ram_ptr,
										  reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ) );
	Change_Tracker::ID_Change( m_data_block->dcid, change,
							   Change_Tracker::RAM_ATTRIB, m_change_tracker );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In( void ) const
{
	Change_Tracker::ID_Change( m_data_block->dcid, true, Change_Tracker::RAM_ATTRIB,
							   m_change_tracker );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Init( void ) const
{
	Change_Tracker::ID_Change( m_data_block->dcid, true, Change_Tracker::NV_ATTRIB,
							   m_change_tracker );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( bool& ram_data ) const
{
	dci_bool_t dci_bool;

#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( bool ) );
#endif

	DCI::Copy_8BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &dci_bool ), m_data_block->ram_ptr );

	if ( dci_bool != DCI_BOOL_FALSE )
	{
		ram_data = true;
	}
	else
	{
		ram_data = false;
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( uint8_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint8_t ) );
#endif
	DCI::Copy_8BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( uint16_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint16_t ) );
#endif
	DCI::Copy_16BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( uint32_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint32_t ) );
#endif
	DCI::Copy_32BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( uint64_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( uint64_t ) );
#endif
	DCI::Copy_64BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( int8_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int8_t ) );
#endif
	DCI::Copy_8BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( int16_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int16_t ) );
#endif
	DCI::Copy_16BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( int32_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int32_t ) );
#endif
	DCI::Copy_32BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( int64_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatype that
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( int64_t ) );
#endif
	DCI::Copy_64BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( float32_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatyp
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( float32_t ) );
#endif
	DCI::Copy_32BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out( BF_Lib::float64_t& ram_data ) const
{
#ifdef DEBUG
	/// If you hit this exception then someone is trying to check out a datatyp
	/// is a different size than the target data.
	BF_ASSERT( DCI::Get_Datatype_Size( m_data_block->datatype ) == sizeof( BF_Lib::float64_t ) );
#endif
	DCI::Copy_64BIT( reinterpret_cast<DCI_RAM_DATA_TD*>( &ram_data ), m_data_block->ram_ptr );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Init_Offset( DCI_DATA_PASS_TD const* ram_data,
									  DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	bool success = false;
	bool data_changed;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		// NV_Mem::Write( ram_data, m_data_block->nv_loc, *m_data_block->length_ptr, true );
		success = DCI_NV_Mem::Owner_Write( ram_data, m_data_block,
										   &data_changed, offset, length );
		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::NV_ATTRIB, m_change_tracker );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Init_Offset( DCI_DATA_PASS_TD* ram_data,
									   DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	bool success = false;
	bool test_default_val = false;

	if ( length == ACCESS_LENGTH_GET_ALL )
	{
		length = *m_data_block->length_ptr;
	}

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		// ( NV_Mem::Read( ram_data, m_data_block->nv_loc, *m_data_block->length_ptr, true );
		success = DCI_NV_Mem::Owner_Read( ram_data, m_data_block,
										  static_cast<bool*>( nullptr ), offset, length );
		test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
											  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		if ( ( success != true ) && test_default_val &&
			 ( m_data_block->default_val !=
			   reinterpret_cast<DCI_DEF_DATA_TD const*>( nullptr ) ) )
		{
			DCI::Copy_Data( ram_data, &m_data_block->default_val[offset], length );
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
bool DCI_Owner::Check_In_Default_Offset( DCI_DATA_PASS_TD const* default_data,
										 DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	bool success = false;
	DCI_RAM_DATA_TD* dest_ptr;
	bool test_default_val;
	bool test_rw_default_val;
	bool data_changed;

	test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
	test_rw_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
											 DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
	if ( test_default_val && test_rw_default_val )
	{
		if ( offset < ( *m_data_block->length_ptr ) )
		{
			Push_TGINT();
			if ( ( length + offset ) > ( *m_data_block->length_ptr ) )
			{
				length = ( *m_data_block->length_ptr ) - offset;
			}
			// This is done deliberately to get at the default which we have determined must by
			// in the ram space (otherwise the macro is broken).

			dest_ptr = const_cast<DCI_RAM_DATA_TD*>( m_data_block->default_val );
			data_changed =
				DCI::Copy_Data_Check_For_Change( &dest_ptr[offset], default_data, length );
			Pop_TGINT();
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::DEFAULT_ATTRIB, m_change_tracker );
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
bool DCI_Owner::Check_Out_Default_Offset( DCI_DATA_PASS_TD* default_data,
										  DCI_LENGTH_TD offset, DCI_LENGTH_TD length ) const
{
	bool success = false;


	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		if ( offset < ( *m_data_block->length_ptr ) )
		{
			if ( ( length + offset ) > ( *m_data_block->length_ptr ) )
			{
				length = ( *m_data_block->length_ptr ) - offset;
			}
			// casting is necessary to ensure we get the byte offset
			DCI::Copy_Data( default_data, &m_data_block->default_val[offset], length );
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
bool DCI_Owner::Check_Out_Default( DCI_DATA_PASS_TD* default_data ) const
{
	bool success = false;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		DCI::Copy_Data( default_data, m_data_block->default_val, *m_data_block->length_ptr );
		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Default( DCI_DATA_PASS_TD const* default_data ) const
{
	bool success = false;
	DCI_RAM_DATA_TD* dest_ptr;
	bool test_default_val;
	bool test_rw_default_val;
	bool data_changed;

	test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
	test_rw_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
											 DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
	if ( test_default_val && test_rw_default_val )
	{
		Push_TGINT();
		dest_ptr = const_cast<DCI_RAM_DATA_TD*>( m_data_block->default_val );
		data_changed = DCI::Copy_Data_Check_For_Change( dest_ptr, default_data,
														*m_data_block->length_ptr );
		Pop_TGINT();
		Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
								   Change_Tracker::DEFAULT_ATTRIB, m_change_tracker );
		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Init( DCI_DATA_PASS_TD* ram_data ) const
{
	bool success = false;
	bool test_default_val;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		// ( NV_Mem::Read( ram_data, m_data_block->nv_loc, *m_data_block->length_ptr, true );
		success = DCI_NV_Mem::Owner_Read( ram_data, m_data_block );
		test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
											  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );

		if ( ( success != true ) && test_default_val &&
			 ( m_data_block->default_val !=
			   reinterpret_cast<DCI_DEF_DATA_TD const*>( nullptr ) ) )
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
bool DCI_Owner::Check_In_Init( DCI_DATA_PASS_TD const* ram_data ) const
{
	bool success = false;
	bool data_changed;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		success = DCI_NV_Mem::Owner_Write( ram_data, m_data_block, &data_changed );	// NV_Mem::Write(
																					// ram_data,
																					// m_data_block->nv_loc,
																					// *m_data_block->length_ptr,
																					// true );
		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::NV_ATTRIB, m_change_tracker );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Min( DCI_DATA_PASS_TD* min ) const
{
	bool success = false;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	success = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
								 DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) && ( range_block != nullptr );

	BF_ASSERT( success );

	Push_TGINT();
	DCI::Copy_Data( min, range_block->min, *m_data_block->length_ptr );
	Pop_TGINT();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Max( DCI_DATA_PASS_TD* max ) const
{
	bool success = false;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	success = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
								 DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) && ( range_block != nullptr );
	BF_ASSERT( success );

	Push_TGINT();
	DCI::Copy_Data( max, range_block->max, *m_data_block->length_ptr );
	Pop_TGINT();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Min( DCI_DATA_PASS_TD const* min ) const
{
	bool success = false;
	DCI_RAM_DATA_TD* dest_ptr;
	bool test_range_val;
	bool test_rw_range_val;
	bool data_changed;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	test_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
	test_rw_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										   DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL );
	success = test_range_val && test_rw_range_val && ( range_block != nullptr );

	BF_ASSERT( success );

	Push_TGINT();
	dest_ptr = const_cast<DCI_RAM_DATA_TD*>( range_block->min );
	data_changed = DCI::Copy_Data_Check_For_Change( dest_ptr, min, *m_data_block->length_ptr );
	Pop_TGINT();
	Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
							   Change_Tracker::MIN_ATTRIB, m_change_tracker );
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Max( DCI_DATA_PASS_TD const* max ) const
{
	bool success = false;
	DCI_RAM_DATA_TD* dest_ptr;
	bool test_range_val;
	bool test_rw_range_val;
	bool data_changed;
	DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );

	test_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
	test_rw_range_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										   DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL );

	success = test_range_val && test_rw_range_val && ( range_block != nullptr );
	BF_ASSERT( success );

	Push_TGINT();
	dest_ptr = const_cast<DCI_RAM_DATA_TD*>( range_block->max );
	data_changed = DCI::Copy_Data_Check_For_Change( dest_ptr, max, *m_data_block->length_ptr );
	Pop_TGINT();
	Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
							   Change_Tracker::MAX_ATTRIB, m_change_tracker );
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Enum_Element_Count( DCI_LENGTH_TD* element_count ) const
{
	bool success = false;
	DCI_RANGE_BLOCK const* range_block = nullptr;
	DCI_ENUM_COUNT_TD enum_count = 0U;

	if ( m_data_block->range_block_index != DCI::INVALID_INDEX )
	{
		range_block = DCI::Get_Range_Block( m_data_block->range_block_index );
		if ( range_block != nullptr )
		{
			enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
			if ( ( enum_count != 0xFFFF ) &&
				 BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_ENUM_VAL ) )
			{
				*element_count = enum_count;
				success = true;
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
bool DCI_Owner::Check_Out_Enum( DCI_ENUM_LIST_TD* enum_dest, DCI_LENGTH_TD element_offset,
								DCI_LENGTH_TD element_count ) const
{
	bool success = false;
	DCI_DATATYPE_SIZE_TD datatype_size;
	DCI_RANGE_BLOCK const* range_block = nullptr;
	DCI_ENUM_COUNT_TD enum_count = 0U;
	DCI_ENUM_BLOCK const* enum_block = nullptr;

	if ( m_data_block->range_block_index != DCI::INVALID_INDEX )
	{
		range_block = DCI::Get_Range_Block( m_data_block->range_block_index );
		if ( range_block != nullptr )
		{
			enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
			enum_block = DCI::Get_Enum_Block( range_block->enum_block_index );
			if ( ( enum_count != 0xFFFF ) && ( enum_block != nullptr ) &&
				 BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_ENUM_VAL ) )
			{
				if ( ( ( element_offset + element_count ) <= enum_count ) )
				{
					success = true;
					datatype_size = DCI::Get_Datatype_Size( m_data_block->datatype );
					DCI::Copy_Data( enum_dest, &enum_block->enum_list[element_offset * datatype_size], element_count );
				}
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
bool DCI_Owner::Check_In_Enum( DCI_ENUM_LIST_TD const* enum_list,
							   DCI_LENGTH_TD element_count ) const
{
	bool success = false;

	/*
	    This functionality is deprecated and it is no more available. So if this function is called it will go in an
	       exception
	 */
	BF_ASSERT( false );
	// bool test_enum_val;
	// bool data_changed = false;
	// DCI_ENUM_LIST_TD** dest_ptr;
	// DCI_LENGTH_TD* element_count_ptr;
	// DCI_RANGE_BLOCK const* range_block = DCI::Get_Range_Block( m_data_block->range_block_index );
	// DCI_ENUM_COUNT_TD enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
	// DCI_ENUM_BLOCK const* enum_block = DCI::Get_Enum_Block( range_block->enum_block_index );
	//
	// test_enum_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
	// DCI_OWNER_ATTRIB_INFO_ENUM_VAL );
	//
	// if ( test_enum_val && ( range_block != nullptr ) )
	// {
	// Push_TGINT();
	//// This assumes that the value is actually writable even though declare it as const.
	// element_count_ptr =
	// reinterpret_cast<DCI_LENGTH_TD*>( &enum_count );
	// *element_count_ptr = element_count;
	// dest_ptr =
	// const_cast<DCI_ENUM_LIST_TD**>( &enum_block->enum_list );
	// *dest_ptr = const_cast<DCI_ENUM_LIST_TD*>( enum_list );
	// DCI::Copy_Data( reinterpret_cast<DCI_DATA_PASS_TD**>( &*dest_ptr[0] ),
	// enum_list,
	// element_count*m_data_block->datatype );
	// Pop_TGINT();
	// Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
	// Change_Tracker::ENUM_ATTRIB, m_change_tracker );
	// success = true;
	// }
	// else
	// {
	// BF_ASSERT(false);
	// }

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_Out_Length( DCI_LENGTH_TD* length ) const
{

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
bool DCI_Owner::Check_In_Length( DCI_LENGTH_TD const* length ) const
{
	bool success = false;
	DCI_LENGTH_TD* length_dest;
	bool data_changed = false;

	success = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RW_LENGTH );
	BF_ASSERT( success );

	Push_TGINT();
	length_dest = const_cast<DCI_LENGTH_TD*>( m_data_block->length_ptr );
	if ( *length_dest != *length )
	{
		data_changed = true;
	}
	*length_dest = *length;
	Pop_TGINT();
	Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
							   Change_Tracker::LENGTH_ATTRIB, m_change_tracker );
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Check_In_Length( DCI_LENGTH_TD length ) const
{
	bool success = false;
	DCI_LENGTH_TD* length_dest;
	bool data_changed = false;

	success = BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RW_LENGTH );
	BF_ASSERT( success );
	Push_TGINT();
	length_dest = const_cast<DCI_LENGTH_TD*>( m_data_block->length_ptr );
	if ( *length_dest != length )
	{
		data_changed = true;
	}
	*length_dest = length;
	Pop_TGINT();
	Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
							   Change_Tracker::LENGTH_ATTRIB, m_change_tracker );
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_LENGTH_TD DCI_Owner::Get_Length( void ) const
{
	DCI_LENGTH_TD return_length;

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
DCI_DATATYPE_TD DCI_Owner::Get_Datatype( void ) const
{
	return ( m_data_block->datatype );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Attach_Callback( DCI_CBACK_FUNC_TD* cback_func,
								 DCI_CBACK_PARAM_TD cback_param, DCI_CB_MSK_TD cback_mask ) const
{
	bool success = false;
	DCI_CBACK_BLOCK_TD* cback_block = DCI::Get_CBACK_Block( m_data_block->cback_block_index );

	BF_ASSERT( cback_block != nullptr );
	Push_TGINT();
	cback_block->cback = cback_func;
	cback_block->cback_param = cback_param;
	cback_block->cback_mask = cback_mask;
	Pop_TGINT();

	if ( ( cback_mask &
		   ( DCI_ACCESS_POST_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_INIT_CMD_MSK ) ) > 0U )
	{
		Enable_Cleanup_Tasks();
		m_change_tracker->Track_ID( m_data_block->dcid );
	}
	success = true;
	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Owner::Enable_Cleanup_Tasks( void )
{
	if ( m_change_tracker == NULL )
	{
		m_change_tracker = new Change_Tracker( &Post_Access_Func,
											   static_cast<Change_Tracker::cback_param_t>( nullptr ),
											   true );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Owner::Post_Access_Func( Change_Tracker::cback_param_t param,
								  DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
{
	DCI_DATA_BLOCK_TD const* data_block;
	DCI_ACCESS_ST_TD access_struct;

	if ( param == 0U )
	{}

	data_block = DCI::Get_Data_Block( dci_id );
	DCI_CBACK_BLOCK_TD* cback_block = DCI::Get_CBACK_Block( data_block->cback_block_index );

	BF_ASSERT( cback_block != nullptr );
	if ( ( cback_block->cback_mask &
		   ( DCI_ACCESS_POST_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_INIT_CMD_MSK ) ) != 0U )
	{
		BF_ASSERT( cback_block->cback != nullptr );

		access_struct.source_id = DCI_OWNER_SOURCE_ID;
		access_struct.data_id = dci_id;
		access_struct.data_access.data = data_block->ram_ptr;
		access_struct.data_access.offset = ACCESS_OFFSET_NONE;
		access_struct.data_access.length = *data_block->length_ptr;
		if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::RAM_ATTRIB ) &&
			 ( ( cback_block->cback_mask & DCI_ACCESS_POST_SET_RAM_CMD_MSK ) !=
			   0U ) )
		{
			access_struct.command = DCI_ACCESS_POST_SET_RAM_CMD;
			( *cback_block->cback )( cback_block->cback_param,
									 &access_struct );
		}
		if ( BF_Lib::Bit::Test( attribute_mask, Change_Tracker::NV_ATTRIB ) &&
			 ( ( cback_block->cback_mask & DCI_ACCESS_POST_SET_INIT_CMD_MSK ) !=
			   0U ) )
		{
			access_struct.command = DCI_ACCESS_POST_SET_INIT_CMD;
			( *cback_block->cback )( cback_block->cback_param,
									 &access_struct );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Load_Current_To_Init( void ) const
{
	bool success = false;
	bool data_changed;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		success =
			DCI_NV_Mem::Owner_Write( m_data_block->ram_ptr, m_data_block, &data_changed );
		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::NV_ATTRIB, m_change_tracker );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Load_Default_To_Current( void ) const
{
	bool success = false;
	bool data_changed;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL ) )
	{
		data_changed = DCI::Copy_Data_Check_For_Change( m_data_block->ram_ptr,
														m_data_block->default_val,
														*m_data_block->length_ptr );

		Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Load_Default_To_Init( void ) const
{
	bool success = false;
	bool data_changed;
	bool test_default_val;
	bool test_init_val;

	test_default_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
										  DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
	test_init_val = BF_Lib::Bit::Test( m_data_block->owner_attrib_info,
									   DCI_OWNER_ATTRIB_INFO_INIT_VAL );
	if ( test_default_val && test_init_val )
	{
		success = DCI_NV_Mem::Owner_Write( m_data_block->default_val,
										   m_data_block, &data_changed );
		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::NV_ATTRIB, m_change_tracker );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Load_Init_To_Current( void ) const
{
	bool success = false;
	bool data_changed;

	if ( BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
	{
		success = DCI_NV_Mem::Owner_Read( m_data_block->ram_ptr, m_data_block, &data_changed );

		if ( success == true )
		{
			Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
									   Change_Tracker::RAM_ATTRIB, m_change_tracker );
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Clear_Current_Val( void ) const
{
	bool success = false;
	bool change = false;

	if ( !BF_Lib::Bit::Test( m_data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_READ_ONLY ) )
	{
		change = DCI::Clear_Data( m_data_block->ram_ptr, *m_data_block->length_ptr );
		Change_Tracker::ID_Change( m_data_block->dcid, change,
								   Change_Tracker::RAM_ATTRIB, m_change_tracker );
		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Owner::Lock( DCI_LOCK_TYPE_TD lock_type ) const
{
	DCI::Lock( m_data_block->dcid, lock_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Owner::Unlock( DCI_LOCK_TYPE_TD lock_type ) const
{
	DCI::Unlock( m_data_block->dcid, lock_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Is_Locked( DCI_LOCK_TYPE_TD lock_type ) const
{
	return ( DCI::Is_Locked( m_data_block->dcid, lock_type ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Owner::Is_Created( DCI_ID_TD data_id )
{
	DCI_DATA_BLOCK_TD const* data_block;
	bool created = false;

	// this function is deprecated and should not be used as all the DCID are now statically created
	BF_ASSERT( false );
	data_block = DCI::Get_Data_Block( data_id );

	if ( data_block->ram_ptr != reinterpret_cast<DCI_RAM_DATA_TD*>( nullptr ) )
	{
		created = true;
	}

	return ( created );
}
