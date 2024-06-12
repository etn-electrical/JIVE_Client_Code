/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "BIN_ASCII_Conversion.h"
#include "INT_ASCII_Conversion.h"
#include "ASCII_Conversion.h"
#include <stdio.h>

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Convert_To_String( uint8_t* data, uint8_t* dest_data, DCI_DATATYPE_TD datatype,
							uint16_t offset, bool output_format )
{
	uint16_t num_of_char = 0U;

	if ( output_format == TRUE )
	{
		switch ( datatype )
		{
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_BOOL:
			case DCI_DTYPE_UINT8:
				num_of_char =
					INT_ASCII_Conversion::uint32_to_str( data[offset],
														 dest_data );
				break;

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
				uint16_t* u_tmp16;
				u_tmp16 =
					reinterpret_cast<uint16_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::uint32_to_str( u_tmp16[0U],
																   dest_data );
				break;

			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_UINT32:
				uint32_t* u_tmp32;
				u_tmp32 =
					reinterpret_cast<uint32_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::uint32_to_str( u_tmp32[0U],
																   dest_data );
				break;

			case DCI_DTYPE_FLOAT:
				float32_t* float_val;
				float_val = reinterpret_cast<float32_t*>( &data[offset] );
				num_of_char = snprintf( reinterpret_cast<char*>( dest_data ),
										( FLOAT_PREC + EXTRA_BYTES ), "%.*g",
										FLOAT_PREC, float_val[0] );
				break;

			case DCI_DTYPE_DFLOAT:
				float64_t* double_val;
				double_val = reinterpret_cast<float64_t*>( &data[offset] );
				num_of_char = snprintf( reinterpret_cast<char*>( dest_data ),
										( DOUBLE_PREC + EXTRA_BYTES ), "%.*g",
										DOUBLE_PREC, double_val[0] );
				break;

			case DCI_DTYPE_SINT8:
				int8_t* tmp8;
				tmp8 = reinterpret_cast<int8_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::sint32_to_str( tmp8[0U],
																   dest_data );
				break;

			case DCI_DTYPE_SINT16:
				int16_t* tmp16;
				tmp16 = reinterpret_cast<int16_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::sint32_to_str( tmp16[0U],
																   dest_data );
				break;

			case DCI_DTYPE_SINT32:
				int32_t* tmp32;
				tmp32 = reinterpret_cast<int32_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::sint32_to_str( tmp32[0U],
																   dest_data );
				break;

			case DCI_DTYPE_UINT64:
				uint64_t* u_tmp64;
				u_tmp64 =
					reinterpret_cast<uint64_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::uint64_to_str( u_tmp64[0U],
																   dest_data );
				break;

			case DCI_DTYPE_SINT64:
				int64_t* tmp64;
				tmp64 = reinterpret_cast<int64_t*>( &data[offset] );
				num_of_char = INT_ASCII_Conversion::sint64_to_str( tmp64[0U],
																   dest_data );
				break;

			default:
				break;
		}
	}
	else
	{
		switch ( datatype )
		{
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_BOOL:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_SINT8:
				num_of_char =
					BIN_ASCII_Conversion::byte_to_str( &data[offset],
													   dest_data );
				break;

			case DCI_DTYPE_UINT16:
			case DCI_DTYPE_SINT16:
			case DCI_DTYPE_WORD:
				num_of_char = BIN_ASCII_Conversion::word_to_str( &data[offset],
																 dest_data );
				break;

			case DCI_DTYPE_UINT32:
			case DCI_DTYPE_SINT32:
			case DCI_DTYPE_DWORD:
				num_of_char = BIN_ASCII_Conversion::dword_to_str( &data[offset],
																  dest_data );
				break;

			case DCI_DTYPE_FLOAT:
				num_of_char =
					BIN_ASCII_Conversion::float_to_str( &data[offset],
														dest_data );
				break;

			case DCI_DTYPE_UINT64:
			case DCI_DTYPE_SINT64:
			case DCI_DTYPE_DFLOAT:
				num_of_char =
					BIN_ASCII_Conversion::dfloat_to_str(
						&data[offset],
						dest_data );
				break;

			default:
				break;
		}
	}
	return ( num_of_char );
}
