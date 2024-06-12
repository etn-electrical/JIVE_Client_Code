/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/def.h"

#include "Unicode_Util.h"

#define UTF16_ENCODING_OFFSET 0x10000UL
#define UTF16_SURROGATE_MASK 0x3FFU

#define UTF8_OTHER_BYTE_BASE 0x00000080UL

#define UTF8_LOW_6_BIT_MASK 0x0000003FUL
#define UTF8_SHIFT_COUNT 6U

/******************************************************************************/
/*
   Unicode code point range U+0000 to U+10FFFF

   (however U+D800~U+DFFF, the surrogates, are not supposed to be encoded in UTF-8

   UTF-8 encoding (maxismum 4 bytes):
   Code point range          Byte 1        Byte 2        Byte 3        Byte 4
   U+0000~U+007F    (0~7 bits)    0b0xxxxxxx
   U+0080~U+07FF    (8~11 bits)   0b110xxxxx 0b10xxxxxx
   U+0800~U+FFFF    (12~16 bits)  0b1110xxxx 0b10xxxxxx 0b10xxxxxx
   U+10000~U+1FFFFF (17~21 bits)  0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx

   Note that the maximum valid code point for Unicode is U+10FFFF

   Example:

   Code point 000wwwzz zzzzyyyy yyxxxxxx (msb to lsb)
   => UTF-8 bytes: 0b11110www, 0b10zzzzzz, 0b10yyyyyy, 0b10xxxxxx

   UTF-16 encoding (maximum two 16-bit code units, or a surrogate pair) :
   Code point range          Code unit
   U+0000  ~  U+D7FF         Identical
   U+D800  ~  U+DFFF         CANNOT BE ENCODED, USED IN SURROGATE PAIR ONLY
   U+E000  ~  U+FFFF         Identical
   U+10000 ~  U+1FFFFF       See below, into high surrogate (first code unit) and
   low surrogate (second code unit)

   First code unit range (D800~DBFF); second code unit range (DC00~DFFF)

   Procedure:
   1)  code point -= 0x10000, result reange 0~0xFFFFF, 20 bits
   2)  first code unit (high surrogate) = top ten bits added to 0xD800
   3)  second code unit (low surrogate) = low ten bits added to 0xDC00

   Example:
   Code point U+1D11E (G clef):
   1)  0x1D11E - 0x10000 = 0x0D11E = 0b0000110100 0100011110
   2)  first code unit (high surrogate) = 0xD800 + 0x0034 = 0xD834
   3)  second code unit (low surrogate) = 0xDC00 + 0x011E = 0xDD1E

   in bytes if big endian: (lower address first) - 0xDB, 0x34, 0xDD, 0x1E
   in bytes if little endian (lower address first) - 0x34, 0xDB, 0x1E, 0xDD

   Convert U+1D11E (G clef) into UTF-8:
   Code point 00000001 11010001 00011110
   => UTF-8 bytes: 0b11110000, 0b10011101, 0b10000100, 0b10011110,
   or 0xF0, 0x9D, 0x84, 0x9E

 */
/******************************************************************************/

void Unicode_Util::CodePointTo1ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset )
{
	// direct copy, no extra conversion needed
	// from 0b0xxx xxxx (from msb to lsb)
	// to   0b0xxx xxxx (one byte total)
	pOutput[offset] = static_cast<uint8_t>( codepoint & UTF8_1_BYTE_MAX_CODE_POINT );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Unicode_Util::CodePointTo2ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset )
{
	// from 0b0000 0yyy yyxx xxxx (from msb to lsb)
	// to   0b110y yyyy (first byte)
	// and  0b10xx xxxx (second byte)
	codepoint &= static_cast<uint32_t>( UTF8_2_BYTE_MAX_CODE_POINT );
	pOutput[offset + 1U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset] = static_cast<uint8_t>( UTF8_BYTE_1_OF_2_PATTERN | codepoint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Unicode_Util::CodePointTo3ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset )
{
	// from 0bzzzz yyyy yyxx xxxx (from msb to lsb)
	// to   0b1110 zzzz (first byte)
	// and  0b10yy yyyy (second byte)
	// and  0b10xx xxxx (third byte)
	codepoint &= static_cast<uint32_t>( UTF8_3_BYTE_MAX_CODE_POINT );
	pOutput[offset + 2U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset + 1U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset] = static_cast<uint8_t>( UTF8_BYTE_1_OF_3_PATTERN | codepoint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Unicode_Util::CodePointTo4ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset )
{
	// from 0b000w wwzz zzzz yyyy yyxx xxxx (from msb to lsb)
	// to   0b1111 0www (first byte)
	// and  0b10zz zzzz (second byte)
	// and  0b10yy yyyy (third byte)
	// and  0b10xx xxxx (fourth byte)
	codepoint &= static_cast<uint32_t>( UTF8_4_BYTE_MAX_CODE_POINT );
	pOutput[offset + 3U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset + 2U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset + 1U] = static_cast<uint8_t>( UTF8_OTHER_BYTE_BASE
												 | ( codepoint & UTF8_LOW_6_BIT_MASK ) );
	codepoint >>= UTF8_SHIFT_COUNT;
	pOutput[offset] = static_cast<uint8_t>( UTF8_BYTE_1_OF_4_PATTERN | codepoint );
}

// convert codepoint to UTF8 in pByte[offset_start..offset_beyond-1]
uint8_t Unicode_Util::CodePointToUTF8( uint32_t codepoint, uint8_t* pByte, uint32_t offset_start,
									   uint32_t offset_beyond, uint8_t* pBytesUsed )
{

	uint8_t return_val = 0U;
	bool_t function_end = false;
	uint8_t num_of_utf8_bytes = 0U;

	if ( false == IsValidUnicodeCodePoint( codepoint ) )
	{
		return_val = static_cast<uint8_t>( CODEPOINT_UTF8_ERR_RANGE_ERROR );
	}
	else
	{

		num_of_utf8_bytes = CodePointNumberOfUTF8Bytes( codepoint );

		if ( ( static_cast<uint8_t>( ( UTF8_FORMAT_ERROR ) ) == num_of_utf8_bytes ) ||
			 ( ( static_cast<uint8_t>( UTF8_MAX_BYTES ) ) < num_of_utf8_bytes ) )
		{
			// should not happen, defensive programming
			return_val = static_cast<uint8_t>( CODEPOINT_UTF8_ERR_RANGE_ERROR );
		}
		else if ( ( offset_start + num_of_utf8_bytes ) > offset_beyond )
		{
			return_val = static_cast<uint8_t>( CODEPOINT_UTF8_ERR_OUTPUT_INADEQUATE_SPACE );
		}
		else
		{

			switch ( num_of_utf8_bytes )
			{
				case UTF8_1_BYTE:
					CodePointTo1ByteUTF8( codepoint, pByte, offset_start );
					break;

				case UTF8_2_BYTES:
					CodePointTo2ByteUTF8( codepoint, pByte, offset_start );
					break;

				case UTF8_3_BYTES:
					CodePointTo3ByteUTF8( codepoint, pByte, offset_start );
					break;

				case UTF8_4_BYTES:
					CodePointTo4ByteUTF8( codepoint, pByte, offset_start );
					break;

				default:
					return_val = static_cast<uint8_t>( CODEPOINT_UTF8_ERR_RANGE_ERROR );
					function_end = true;
			}
			if ( false == function_end )
			{
				if ( NULL != pBytesUsed )
				{
					*pBytesUsed = num_of_utf8_bytes;
				}

				return_val = static_cast<uint8_t>( CODEPOINT_UTF8_ERR_OK );
			}
		}
	}
	return ( return_val );

}

// available input in pByte[offset_start .. offset_beyond-1]
uint8_t Unicode_Util::UTF16ToCodePoint( const uint8_t* pByte, uint32_t offset_start,
										uint32_t offset_beyond, bool_t isLittleEndian, uint32_t* pRes,
										uint8_t* pBytesUsed )
{
	uint8_t return_val = 0U;
	uint16_t code_unit_0 = 0U;
	uint16_t code_unit_1 = 0U;

	if ( ( offset_start + 2U ) > offset_beyond )
	{
		return_val = static_cast<uint8_t>( UTF16_ERR_INPUT_LESS_THAN_2_BYTES );
	}
	else
	{
		if ( isLittleEndian )
		{
			code_unit_0 = UTF16ToCodeUnitLittleEndian( pByte, offset_start );
		}
		else
		{
			// big endian
			code_unit_0 = UTF16ToCodeUnitBigEndian( pByte, offset_start );
		}
		if ( false == CodeUnitIsSurrogate( code_unit_0 ) )
		{
			// if not surrogate, no conversion needed
			*pRes = static_cast<uint32_t>( code_unit_0 );
			if ( NULL != pBytesUsed )
			{
				*pBytesUsed = 2U;
			}
			return_val = static_cast<uint8_t>( UTF16_ERR_OK );
		}
		else
		{
			// the first code unit (code_unit_0) is surrogate
			if ( false == CodeUnitIsHiSurrogate( code_unit_0 ) )
			{
				return_val = static_cast<uint8_t>( UTF16_ERR_FIRST_SURROGATE_NOT_HI );
			}
			else if ( ( offset_start + 4U ) > offset_beyond )
			{
				return_val = static_cast<uint8_t>( UTF16_ERR_INPUT_LESS_THAN_4_BYTES );
			}
			else
			{
				if ( isLittleEndian )
				{
					code_unit_1 = UTF16ToCodeUnitLittleEndian( pByte, ( offset_start + 2U ) );
				}
				else
				{
					// big endian
					code_unit_1 = UTF16ToCodeUnitBigEndian( pByte, ( offset_start + 2U ) );
				}
				if ( false == CodeUnitIsLoSurrogate( code_unit_1 ) )
				{
					return_val = static_cast<uint8_t>( UTF16_ERR_SECOND_SURROGATE_NOT_LO );
				}
				else
				{

					// now data valid, do conversion
					*pRes = ( ( ( ( static_cast<uint32_t>( code_unit_0 ) ) & UTF16_SURROGATE_MASK )
								<< 10 )
							  | ( ( static_cast<uint32_t>( code_unit_1 ) ) & UTF16_SURROGATE_MASK ) );
					*pRes += static_cast<uint32_t>( UTF16_ENCODING_OFFSET );

					if ( NULL != pBytesUsed )
					{
						*pBytesUsed = 4U;
					}

					return_val = static_cast<uint8_t>( UTF16_ERR_OK );
				}
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Unicode_Util::UTF8ToCodePoint( const uint8_t* pInput, uint32_t offset_start,
									   uint32_t offset_beyond, uint32_t* pCodepoint, uint8_t* pBytesUsed )
{
	uint8_t return_val = 0U;
	bool_t loop_break = false;
	uint8_t i = 0U;
	uint8_t num_of_bytes;
	uint32_t result = 0U;

	num_of_bytes = PeekUTF8NumberOfBytes( pInput[offset_start] );

	if ( ( UTF8_FORMAT_ERROR == num_of_bytes ) || ( UTF8_MAX_BYTES < num_of_bytes ) ||
		 ( ( offset_start + num_of_bytes ) > offset_beyond ) )
	{
		return_val = static_cast<uint8_t>( UTF8_CODEPOINT_ERR_FORMAT_ERROR );
	}
	else
	{

		for ( i = 1U; ( ( i < num_of_bytes ) && ( false == loop_break ) ); i++ )
		{
			if ( true != IsValidUTF8OtherByte( pInput[offset_start + i] ) )
			{
				return_val = static_cast<uint8_t>( UTF8_CODEPOINT_ERR_FORMAT_ERROR );
				loop_break = true;
			}
		}
		if ( false == loop_break )
		{
			switch ( num_of_bytes )
			{
				case UTF8_1_BYTE:
					result = static_cast<uint32_t>( pInput[offset_start] );
					break;

				case UTF8_2_BYTES:
					result =
						static_cast<uint32_t>( static_cast<uint32_t>( pInput[offset_start]
																	  & ( static_cast<uint32_t>( static_cast<uint8_t>( ~
																													   UTF8_BYTE_1_OF_2_MASK ) ) ) ) );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 1U] & UTF8_LOW_6_BIT_MASK );
					break;

				case UTF8_3_BYTES:
					result =
						static_cast<uint32_t>( pInput[offset_start]
											   & ( static_cast<uint32_t>( static_cast<uint8_t>( ~UTF8_BYTE_1_OF_3_MASK ) ) ) );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 1U] & UTF8_LOW_6_BIT_MASK );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 2U] & UTF8_LOW_6_BIT_MASK );
					break;

				case UTF8_4_BYTES:
					result =
						static_cast<uint32_t>( pInput[offset_start]
											   & ( static_cast<uint32_t>( static_cast<uint8_t>( ~UTF8_BYTE_1_OF_4_MASK ) ) ) );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 1U] & UTF8_LOW_6_BIT_MASK );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 2U] & UTF8_LOW_6_BIT_MASK );
					result <<= UTF8_SHIFT_COUNT;
					result |=
						static_cast<uint32_t>( pInput[offset_start + 3U] & UTF8_LOW_6_BIT_MASK );
					break;

				default:
					return_val = static_cast<uint8_t>( UTF8_CODEPOINT_ERR_FORMAT_ERROR );
					loop_break = true;
			}
			if ( false == loop_break )
			{

				if ( !IsValidUnicodeCodePoint( result ) )
				{
					return_val = static_cast<uint8_t>( UTF8_CODEPOINT_ERR_FORMAT_ERROR );
				}
				else
				{

					*pCodepoint = result;
					if ( NULL != pBytesUsed )
					{
						*pBytesUsed = num_of_bytes;
					}
					return_val = static_cast<uint8_t>( UTF8_CODEPOINT_ERR_OK );
				}
			}
		}
	}
	return ( return_val );
}
