/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "CRC16.h"

namespace BF_Lib
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Copy_String( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	while ( length > 0U )
	{
		*dest = *source;
		dest++;
		source++;
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Smart_Copy_String( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	if ( dest < source )
	{
		while ( length > 0U )
		{
			*dest = *source;
			dest++;
			source++;
			length--;
		}
	}
	else if ( dest > source )
	{
		dest += length;
		source += length;
		while ( length > 0U )
		{
			dest--;
			source--;
			*dest = *source;
			length--;
		}
	}
	else
	{	/* do nothing */
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Copy_Const_String( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	while ( length > 0U )
	{
		*dest = *source;
		dest++;
		source++;
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Copy_String_Ret_Checksum( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	uint_fast16_t checksum = 0U;

	while ( length > 0U )
	{
		checksum += *source;
		*dest = *source;
		dest++;
		source++;
		length--;
	}

	return ( static_cast<uint16_t>( checksum & 0xFFFFU ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Smart_Copy_String_Ret_Checksum( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	uint_fast16_t checksum = 0U;

	if ( dest < source )
	{
		while ( length > 0U )
		{
			checksum += *source;
			*dest = *source;
			dest++;
			source++;
			length--;
		}
	}
	else if ( dest > source )
	{
		dest += length;
		source += length;
		while ( length > 0U )
		{
			dest--;
			source--;
			checksum += *source;
			*dest = *source;
			length--;
		}
	}
	else
	{
		while ( length > 0U )
		{
			checksum += *source;
			source++;
			length--;
		}
	}

	return ( static_cast<uint16_t>( checksum & 0xFFFFU ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Copy_String_Ret_CRC( uint8_t* dest, uint8_t const* source,
							  uint32_t length, uint16_t init_crc )
{
	uint16_t ret_CRC = init_crc;

	while ( length > 0U )
	{
		BF_Lib::CRC16::Calc_On_Byte( *dest, ret_CRC );
		*dest = *source;
		dest++;
		source++;
		length--;
	}

	return ( ret_CRC );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Copy_Null_String( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	uint_fast16_t temp_len = length;

	while ( ( temp_len > 0U ) && ( *source != nullptr ) )
	{
		temp_len--;
		*dest = *source;
		dest++;
		source++;
	}

	*dest = *source;

	return ( static_cast<uint16_t>( length - temp_len ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Copy_Const_Null_String( uint8_t* dest, uint8_t const* source, uint32_t length )
{
	uint_fast16_t temp_len = length;

	while ( ( temp_len > 0U ) && ( *source != nullptr ) )
	{
		temp_len--;
		*dest = *source;
		dest++;
		source++;
	}

	*dest = *source;

	return ( static_cast<uint16_t>( length - temp_len ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Align_String_U8_To_U32( uint32_t* dest, uint8_t const* src, uint32_t byte_length )	// Returns U32 length
{
	uint32_t word_len = ( byte_length + ( sizeof ( dest[0] ) - 1U ) ) / sizeof ( dest[0] );
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( dest );

	while ( byte_length > 0U )
	{
		*dest_ptr = *src;
		dest_ptr++;
		src++;
		byte_length--;
	}

	while ( ( reinterpret_cast<uint32_t>( dest_ptr ) & ( sizeof ( dest[0] ) - 1U ) ) != 0U )
	{
		*dest_ptr = 0U;
		dest_ptr++;
	}

	return ( word_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Align_String_U8_To_U16( uint16_t* dest, uint8_t const* src, uint32_t byte_length )	// Returns U16 length
{
	uint32_t word_len = ( byte_length + ( sizeof ( dest[0] ) - 1U ) ) / sizeof ( dest[0] );
	uint8_t* dest_ptr = reinterpret_cast<uint8_t*>( dest );

	while ( byte_length > 0U )
	{
		*dest_ptr = *src;
		dest_ptr++;
		src++;
		byte_length--;
	}

	while ( ( reinterpret_cast<uint32_t>( dest_ptr ) & ( sizeof ( dest[0] ) - 1U ) ) != 0U )
	{
		*dest_ptr = 0U;
		dest_ptr++;
	}

	return ( word_len );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool String_Compare( uint8_t const* string1, uint8_t const* string2, uint32_t length )
{
	bool same = true;
	uint_fast32_t i = length;

	while ( ( i > 0U ) && ( same == true ) )
	{
		i--;
		if ( *string1 != *string2 )
		{
			same = false;
		}
		string1++;
		string2++;
	}

	return ( same );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Copy_Val_To_String( uint8_t* dest, uint8_t value, uint32_t length )
{
	while ( length > 0U )
	{
		*dest = value;
		dest++;
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Copy_Val_To_String_Ret_Checksum( uint8_t* dest, uint8_t value, uint32_t length )
{
	uint_fast16_t checksum;

	checksum = value * length;
	while ( length > 0U )
	{
		*dest = value;
		dest++;
		length--;
	}

	return ( static_cast<uint16_t>( checksum & 0xFFFFU ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

bool_t Compare_Data( uint8_t const* dest, uint8_t const* source, uint32_t length )
{
	bool_t same = true;
	uint_fast32_t i = length;

	Push_TGINT();
	while ( ( i > 0U ) && ( same == true ) )
	{
		i--;
		if ( *dest != *source )
		{
			same = false;
		}
		dest++;
		source++;
	}
	Pop_TGINT();
	return ( same );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Get_String_Length( uint8_t* str, uint16_t buffer_size )
{
	uint16_t field_cnt = 0;

	if ( str != nullptr )
	{
		for ( field_cnt = 0; field_cnt < buffer_size; field_cnt++ )
		{
			if ( str[field_cnt] == 0 )
			{
				break;
			}
		}
		if ( field_cnt == buffer_size )
		{
			field_cnt = 0;
		}
	}

	return ( field_cnt );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Copy_Data( uint8_t* dst, uint16_t dst_buf_size, const uint8_t* src, uint16_t src_len )
{
	bool_t status = false;

	if ( dst_buf_size >= src_len )
	{
		Copy_Const_String( dst, src, src_len );
		status = true;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Remove_Char_From_String( uint8_t* str, const uint32_t buffer_size, uint8_t c )
{
	uint32_t removed_char_count = 0;
	uint32_t str_size = BF_Lib::Get_String_Length( str, buffer_size );

	if ( buffer_size > str_size )
	{
		uint_fast16_t i = 0;
		while ( i < str_size )
		{
			if ( str[i] == c )
			{
				Copy_String( &str[i], &str[i + 1U], ( str_size - i ) );
				i = 0;
				removed_char_count++;
			}
			else
			{
				i++;
			}
		}
	}
	return ( removed_char_count );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   void Convert_UINT8_To_BCD( uint8_t data, uint8_t* dest )
   {
   BF_Lib::SPLIT_UINT32 result;
   uint8_t counter = 0U;

   result.u32 = data;

   result.u32 = result.u32<<3;
   while ( result.u8[0] != 0 )
   {
   if ( ( result.u8[1] & 0x0F ) >= 0x05 )
   {
   result.u32 += 0x300;
   }
   if ( ( result.u8[1] & 0xF0 ) >= 0x50 )
   {
   result.u32 += 0x3000;
   }
   //		if ( ( result.u8[2] & 0xF0 ) >= 0x50 )
   //		{
   //			result.u32 += 0x30000;
   //		}
   //		if ( ( result.u8[2] & 0xF0 ) >= 0x50 )
   //		{
   //			result.u32 += 0x300000;
   //		}
   result.u32 = result.u32<<1;
   }

   dest[0] = 0U;
   dest[1] = 0U;
   if ( result.u8[2] & 0x0F )
   {
   dest[1] = ( result.u8[2] & 0x0F )
   }
   if ( result.u8[1] & 0x0F )
   {
   dest[0] |= ( result.u8[2] & 0x0F )
   }
   if ( result.u8[1] & 0xF0 )
   {
   dest[0] |= ( result.u8[1] & 0xF0 )
   }

   }
 */
}	/* end namespace BF_Lib for this module*/
