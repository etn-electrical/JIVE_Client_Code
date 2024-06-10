/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI.h"
#include "StdLib_MV.h"
#include "Exception.h"
#include "OS_Tasker.h"
#include "NV_Mem.h"
#include "DCI_NV_Mem.h"
#include "Bit.hpp"
#include "DCI_Constants.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
const DCI_DATATYPE_SIZE_TD DCI::DATATYPE_SIZES[DCI_DTYPE_MAX_TYPES] =
{
	DCI_DTYPE_BOOL_SIZE,						// DCI_DTYPE_BOOL,
	DCI_DTYPE_UINT8_SIZE,						// DCI_DTYPE_UINT8,
	DCI_DTYPE_SINT8_SIZE,						// DCI_DTYPE_SINT8,
	DCI_DTYPE_UINT16_SIZE,						// DCI_DTYPE_UINT16,
	DCI_DTYPE_SINT16_SIZE,						// DCI_DTYPE_SINT16,
	DCI_DTYPE_UINT32_SIZE,						// DCI_DTYPE_UINT32,
	DCI_DTYPE_SINT32_SIZE,						// DCI_DTYPE_SINT32,
	DCI_DTYPE_UINT64_SIZE,						// DCI_DTYPE_UINT64,
	DCI_DTYPE_SINT64_SIZE,						// DCI_DTYPE_SINT64,
	DCI_DTYPE_FLOAT_SIZE,						// DCI_DTYPE_FLOAT,
	DCI_DTYPE_LFLOAT_SIZE,						// DCI_DTYPE_LFLOAT,
	DCI_DTYPE_DFLOAT_SIZE,						// DCI_DTYPE_DFLOAT,
	DCI_DTYPE_BYTE_SIZE,						// DCI_DTYPE_BYTE,
	DCI_DTYPE_WORD_SIZE,						// DCI_DTYPE_WORD,
	DCI_DTYPE_DWORD_SIZE,						// DCI_DTYPE_DWORD,
	DCI_DTYPE_STRING8_SIZE						// DCI_DTYPE_STRING8,
};

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define Callback_Attrib( attrib )           ( ( attrib ) & ( 1 << DC_ATTRIB_INFO_CALLBACK ) )
DCI_DATA_BLOCK_TD const* const DCI::UNDEFINED_DCID =
	reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
BF::DCI_Lock* DCI::m_dci_lock;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Init( void )
{
	new BF::System_Reset( &Reset_Req_Handler,
						  static_cast<BF::System_Reset::cback_param_t>( nullptr ) );

	m_dci_lock = new BF::DCI_Lock( BF::DCI_Lock::READ_WRITE );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_Data( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source, DCI_LENGTH_TD length )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );
	uint_fast16_t length_cntr = static_cast<uint_fast16_t>( length );

	Push_TGINT();
	while ( length_cntr > 0U )
	{
		*dest_cp = *source_cp;
		dest_cp++;
		source_cp++;
		length_cntr--;
	}
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_Data_Check_For_Change( DCI_DATA_PASS_TD* dest,
									  DCI_DATA_PASS_TD const* source, DCI_LENGTH_TD length )
{
	uint8_t* dest_cp = reinterpret_cast<uint8_t*>( dest );
	uint8_t const* source_cp = reinterpret_cast<uint8_t const*>( source );
	uint_fast16_t length_cntr = static_cast<uint_fast16_t>( length );
	bool data_changed = false;

	Push_TGINT();
	while ( ( length_cntr > 0U ) && ( *dest_cp == *source_cp ) )
	{
		dest_cp++;
		source_cp++;
		length_cntr--;
	}
	if ( length_cntr != 0U )
	{
		data_changed = true;
		while ( length_cntr > 0U )		// This second while is to finish off the string copy if the
										// value is truly different.
		{
			*dest_cp = *source_cp;
			dest_cp++;
			source_cp++;
			length_cntr--;
		}
	}
	Pop_TGINT();

	return ( data_changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_Null_String_Data( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source,
								 DCI_LENGTH_TD* length )
{
	uint8_t* dest_cp = reinterpret_cast<uint8_t*>( dest );
	uint8_t const* source_cp = reinterpret_cast<uint8_t const*>( source );
	uint_fast16_t length_cntr = static_cast<uint_fast16_t>( *length ) - 1U;

	if ( *length > 0U )
	{
		Push_TGINT();

		while ( ( length_cntr > 0U ) && ( *source_cp != static_cast<uint8_t>( nullptr ) ) )
		{
			*dest_cp = *source_cp;
			dest_cp++;
			source_cp++;
			length_cntr--;
		}
		if ( *source_cp == static_cast<uint8_t>( nullptr ) )
		{
			*dest_cp = *source_cp;
		}
		*length = *length - static_cast<DCI_LENGTH_TD>( length_cntr );

		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_Null_String_Data_Check_For_Change( DCI_DATA_PASS_TD* dest,
												  DCI_DATA_PASS_TD const* source,
												  DCI_LENGTH_TD* length )
{
	uint8_t* dest_cp = reinterpret_cast<uint8_t*>( dest );
	uint8_t const* source_cp = reinterpret_cast<uint8_t const*>( source );
	uint_fast16_t length_cntr = static_cast<uint_fast16_t>( *length ) - 1U;
	bool changed = false;

	if ( *length > 0U )
	{
		Push_TGINT();

		while ( ( length_cntr > 0U ) && ( *source_cp != static_cast<uint8_t>( nullptr ) ) )
		{
			if ( *dest_cp != *source_cp )
			{
				changed = true;
			}
			*dest_cp = *source_cp;
			dest_cp++;
			source_cp++;
			length_cntr--;
		}
		if ( *source_cp == static_cast<uint8_t>( nullptr ) )
		{
			if ( *dest_cp != static_cast<uint8_t>( nullptr ) )
			{
				changed = true;
			}
			*dest_cp = *source_cp;
		}
		*length = *length - static_cast<DCI_LENGTH_TD>( length_cntr );

		Pop_TGINT();
	}

	return ( changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_8BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );

	Push_TGINT();

	*dest_cp = *source_cp;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_8BIT_Check_For_Change( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );
	bool data_changed = false;

	Push_TGINT();

	if ( dest_cp[0] != source_cp[0] )
	{
		dest_cp[0] = source_cp[0];
		data_changed = true;
	}

	Pop_TGINT();

	return ( data_changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_16BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );

	Push_TGINT();

	dest_cp[0] = source_cp[0];
	dest_cp[1] = source_cp[1];

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_16BIT_Check_For_Change( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );
	bool data_changed = false;

	Push_TGINT();

	if ( ( dest_cp[0] != source_cp[0] ) || ( dest_cp[1] != source_cp[1] ) )
	{
		dest_cp[0] = source_cp[0];
		dest_cp[1] = source_cp[1];
		data_changed = true;
	}

	Pop_TGINT();

	return ( data_changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_32BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );

	Push_TGINT();

	dest_cp[0] = source_cp[0];
	dest_cp[1] = source_cp[1];
	dest_cp[2] = source_cp[2];
	dest_cp[3] = source_cp[3];

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_32BIT_Check_For_Change( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );

	bool data_changed = false;

	Push_TGINT();

	if ( ( dest_cp[0] != source_cp[0] ) || ( dest_cp[1] != source_cp[1] ) ||
		 ( dest_cp[2] != source_cp[2] ) || ( dest_cp[3] != source_cp[3] ) )
	{
		dest_cp[0] = source_cp[0];
		dest_cp[1] = source_cp[1];
		dest_cp[2] = source_cp[2];
		dest_cp[3] = source_cp[3];
		data_changed = true;
	}

	Pop_TGINT();

	return ( data_changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI::Copy_64BIT( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );

	Push_TGINT();

	dest_cp[0] = source_cp[0];
	dest_cp[1] = source_cp[1];
	dest_cp[2] = source_cp[2];
	dest_cp[3] = source_cp[3];
	dest_cp[4] = source_cp[4];
	dest_cp[5] = source_cp[5];
	dest_cp[6] = source_cp[6];
	dest_cp[7] = source_cp[7];

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Copy_64BIT_Check_For_Change( DCI_DATA_PASS_TD* dest, DCI_DATA_PASS_TD const* source )
{
	uint8_t* dest_cp = static_cast<uint8_t*>( dest );
	uint8_t const* source_cp = static_cast<uint8_t const*>( source );
	bool data_changed = false;

	Push_TGINT();

	if ( ( dest_cp[0] != source_cp[0] ) || ( dest_cp[1] != source_cp[1] ) ||
		 ( dest_cp[2] != source_cp[2] ) || ( dest_cp[3] != source_cp[3] ) ||
		 ( dest_cp[4] != source_cp[4] ) || ( dest_cp[5] != source_cp[5] ) ||
		 ( dest_cp[6] != source_cp[6] ) || ( dest_cp[7] != source_cp[7] ) )
	{
		dest_cp[0] = source_cp[0];
		dest_cp[1] = source_cp[1];
		dest_cp[2] = source_cp[2];
		dest_cp[3] = source_cp[3];
		dest_cp[4] = source_cp[4];
		dest_cp[5] = source_cp[5];
		dest_cp[6] = source_cp[6];
		dest_cp[7] = source_cp[7];
		data_changed = true;
	}

	Pop_TGINT();

	return ( data_changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Reset_Req_Handler( BF::System_Reset::cback_param_t handle,
							 BF::System_Reset::reset_select_t reset_req )
{
	bool success = false;
	bool val_changed = false;
	bool test_init_value = false;
	bool test_default_value = false;
	bool test_app_param = false;

	if ( handle == static_cast<BF::System_Reset::cback_param_t>( 0U ) )
	{}

	switch ( reset_req )
	{
		case BF::System_Reset::FACTORY_RESET:
			for ( DCI_ID_TD i = 0U; i < DCI_TOTAL_DCIDS; i++ )
			{
				test_init_value = BF_Lib::Bit::Test( dci_data_blocks[i].owner_attrib_info,
													 DCI_OWNER_ATTRIB_INFO_INIT_VAL );
				test_default_value = BF_Lib::Bit::Test( dci_data_blocks[i].owner_attrib_info,
														DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
				if ( test_init_value && test_default_value )
				{
					success = DCI_NV_Mem::Owner_Write( dci_data_blocks[i].default_val,
													   &dci_data_blocks[i], &val_changed );
					// write_success = DCI_NV_Mem::Owner_Write(
					// dci_data_blocks[i].default_block->def, &dci_data_blocks[i], &val_changed );
					// if ( ( write_success == true ) && ( val_changed == true ) )		//Removed
					// because I am not sure if I want to notify the change tracker that a factory
					// reset has occurred since we will likely reset from here.
					// {
					// Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
					// Change_Tracker::NV_ATTRIB, m_change_tracker );
					// }
				}
			}
			break;

		case BF::System_Reset::APP_PARAM_RESET:
			for ( DCI_ID_TD i = 0U; i < DCI_TOTAL_DCIDS; i++ )
			{
				test_init_value = BF_Lib::Bit::Test( dci_data_blocks[i].owner_attrib_info,
													 DCI_OWNER_ATTRIB_INFO_INIT_VAL );
				test_default_value = BF_Lib::Bit::Test( dci_data_blocks[i].owner_attrib_info,
														DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
				test_app_param = BF_Lib::Bit::Test( dci_data_blocks[i].owner_attrib_info,
													DCI_OWNER_ATTRIB_INFO_APP_PARAM );
				if ( test_init_value && test_default_value && test_app_param )
				{
					success = DCI_NV_Mem::Owner_Write( dci_data_blocks[i].default_val,
													   &dci_data_blocks[i] );
					// write_success = DCI_NV_Mem::Owner_Write(
					// dci_data_blocks[i].default_block->def, &dci_data_blocks[i] );
					// if ( ( write_success == true ) && ( val_changed == true ) )		//Removed
					// because I am not sure if I want to notify the change tracker that a factory
					// reset has occurred since we will likely reset from here.
					// {
					// Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
					// Change_Tracker::NV_ATTRIB, m_change_tracker );
					// }
				}
			}
			break;

		default:
			break;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Factory_Reset_DCID( DCI_ID_TD dci_id )
{
	bool success = false;
	bool val_changed;
	bool test_init_value = false;
	bool test_default_value = false;

	if ( dci_id < DCI_TOTAL_DCIDS )
	{
		test_init_value = BF_Lib::Bit::Test( dci_data_blocks[dci_id].owner_attrib_info,
											 DCI_OWNER_ATTRIB_INFO_INIT_VAL );
		test_default_value = BF_Lib::Bit::Test( dci_data_blocks[dci_id].owner_attrib_info,
												DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
		if ( test_init_value && test_default_value )
		{
			success = DCI_NV_Mem::Owner_Write( dci_data_blocks[dci_id].default_val,
											   &dci_data_blocks[dci_id], &val_changed );
			// if ( ( success == true ) && ( val_changed == true ) )		//Removed because I am not
			// sure if I want to notify the change tracker that a factory reset has occurred since we
			// will likely reset from here.
			// {
			// Change_Tracker::ID_Change( m_data_block->dcid, data_changed,
			// Change_Tracker::NV_ATTRIB, m_change_tracker );
			// }
		}
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI::Clear_Data( DCI_DATA_PASS_TD* dest, DCI_LENGTH_TD length )
{
	uint8_t* dest_cp = reinterpret_cast<uint8_t*>( dest );
	bool data_changed = false;

	Push_TGINT();
	while ( ( length > 0U ) && ( *dest_cp == CLEAR_DATA_VALUE ) )
	{
		dest_cp++;
		length--;
	}
	if ( length != 0U )
	{
		data_changed = true;
		while ( length > 0U )		// This second while is to finish off the string copy if the
									// value is truly different.
		{
			*dest_cp = CLEAR_DATA_VALUE;
			dest_cp++;
			length--;
		}
	}
	Pop_TGINT();

	return ( data_changed );
}
