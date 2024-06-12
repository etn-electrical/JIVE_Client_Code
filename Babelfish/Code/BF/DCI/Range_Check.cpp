/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Range_Check.h"
#include "StdLib_MV.h"
#include "OS_Tasker.h"
#include "Range_TPlate.hpp"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Macros
 *****************************************************************************************
 */

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
bool Range_Check::Check_Range( DCI_ACCESS_DATA_TD* data_access,
							   DCI_DATA_BLOCK_TD const* data_block, bool fix_value )
{
	bool range_good = false;

	switch ( data_block->datatype )
	{
		case DCI_DTYPE_STRING8:
		case DCI_DTYPE_BYTE:
		case DCI_DTYPE_UINT8:
		case DCI_DTYPE_BOOL:
			range_good = Range_TPlate<uint8_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_SINT8:
			range_good = Range_TPlate<int8_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_WORD:
		case DCI_DTYPE_UINT16:
			range_good = Range_TPlate<uint16_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_SINT16:
			range_good = Range_TPlate<int16_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_DWORD:
		case DCI_DTYPE_UINT32:
			range_good = Range_TPlate<uint32_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_SINT32:
			range_good = Range_TPlate<int32_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_UINT64:
			range_good = Range_TPlate<uint64_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_SINT64:
			range_good = Range_TPlate<int64_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_FLOAT:
			range_good = Range_TPlate<float32_t>::Check( data_access, data_block, fix_value );
			break;

		case DCI_DTYPE_DFLOAT:
			range_good = Range_TPlate<float64_t>::Check( data_access, data_block, fix_value );
			break;

		default:
			break;
	}

	return ( range_good );
}
