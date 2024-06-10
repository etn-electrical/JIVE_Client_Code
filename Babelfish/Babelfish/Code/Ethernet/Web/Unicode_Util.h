/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef _UNICODE_UTIL_H
#define _UNICODE_UTIL_H
#include "Etn_Types.h"

#define UTF16_MIN_SURROGATE 0xD800U
#define UTF16_MAX_SURROGATE 0xDFFFU
#define UTF16_MAX_HI_SURROGATE 0xDBFFU
#define UTF16_MIN_LO_SURROGATE 0xDC00U

#define UNICODE_MAX_CODE_POINT 0x10FFFFUL

#define UTF8_1_BYTE_MASK            0x80U	// 0b10000000
#define UTF8_1_BYTE_PATTERN         0x00U	// 0b00000000

#define UTF8_OTHER_BYTE_MASK        0xC0U	// 0b11000000
#define UTF8_OTHER_BYTE_PATTERN     0x80U	// 0b10000000

#define UTF8_BYTE_1_OF_2_MASK       0xE0U	// 0b11100000
#define UTF8_BYTE_1_OF_2_PATTERN    0xC0U	// 0b11000000

#define UTF8_BYTE_1_OF_3_MASK       0xF0U	// 0b11110000
#define UTF8_BYTE_1_OF_3_PATTERN    0xE0U	// 0b11100000

#define UTF8_BYTE_1_OF_4_MASK       0xF8U	// 0b11111000
#define UTF8_BYTE_1_OF_4_PATTERN    0xF0U	// 0b11110000

#define UTF8_AX_BX_MASK             0xE0U	// 0b11100000
#define UTF8_AX_BX_PATTERN          0xA0U	// 0b10100000

#define UTF8_1_BYTE_MAX_CODE_POINT 0x0000007FUL
#define UTF8_2_BYTE_MAX_CODE_POINT 0x000007FFUL
#define UTF8_3_BYTE_MAX_CODE_POINT 0x0000FFFFUL
#define UTF8_4_BYTE_MAX_CODE_POINT 0x001FFFFFUL



static const uint8_t UTF8_FORMAT_ERROR = 0U;
static const uint8_t UTF8_1_BYTE = 1U;
static const uint8_t UTF8_2_BYTES = 2U;
static const uint8_t UTF8_3_BYTES = 3U;
static const uint8_t UTF8_4_BYTES = 4U;
static const uint8_t UTF8_MAX_BYTES = UTF8_4_BYTES;



static const uint8_t CODEPOINT_UTF8_ERR_OK = 0U;
static const uint8_t CODEPOINT_UTF8_ERR_RANGE_ERROR = 1U;
static const uint8_t CODEPOINT_UTF8_ERR_OUTPUT_INADEQUATE_SPACE = 2U;

static const uint8_t UTF16_UTF8_ERR_OK = 0U;
static const uint8_t UTF16_UTF8_WRONG_INPUT_LENGTH = 1U;
static const uint8_t UTF16_UTF8_WRONG_INPUT_RANGE = 2U;
static const uint8_t UTF16_UTF8_WRONG_INPUT_FORMAT = 3U;
static const uint8_t UTF16_UTF8_ERR_OUTPUT_TOO_LONG = 4U;
static const uint8_t UTF16_UTF8_ERR_OTHER_ERRORS = 5U;

static const uint8_t UTF16_ERR_OK = 0U;
static const uint8_t UTF16_ERR_INPUT_LESS_THAN_2_BYTES = 1U;
static const uint8_t UTF16_ERR_INPUT_LESS_THAN_4_BYTES = 2U;
static const uint8_t UTF16_ERR_FIRST_SURROGATE_NOT_HI = 3U;
static const uint8_t UTF16_ERR_SECOND_SURROGATE_NOT_LO = 4U;


static const uint8_t UTF8_CODEPOINT_ERR_OK = 0U;
static const uint8_t UTF8_CODEPOINT_ERR_FORMAT_ERROR = 1U;


class Unicode_Util
{
	public:

#if 1
		inline static uint8_t UTF16StringToUTF8( const uint8_t* pInput, uint32_t uInputLength, uint8_t* pOutput,
												 uint32_t uMaxOutputLength, uint32_t* pOutputLength,
												 bool isLittleEndian, uint32_t offset = 0 );

#endif

		// input pByte[offset..offset+1]
		inline static uint16_t UTF16ToCodeUnit( const uint8_t* pByte, uint32_t offset, bool isLittleEndian = true )
		{
			return ( isLittleEndian ? UTF16ToCodeUnitLittleEndian( pByte, offset ) : UTF16ToCodeUnitBigEndian( pByte,
																											   offset ) );
		}

		// check whether the code point <= 0x10FFFF, maximum for Unicode
		inline static bool IsValidUnicodeCodePoint( uint32_t codepoint )
		{
			if ( codepoint > UNICODE_MAX_CODE_POINT )
			{
				return ( false );
			}
			if ( ( UTF16_MIN_SURROGATE <= codepoint ) && ( UTF16_MAX_SURROGATE >= codepoint ) )
			{
				return ( false );
			}
			return ( true );
		}

		// returns the number of bytes needed for UTF-8 encoding of the codepoint
		inline static uint8_t CodePointNumberOfUTF8Bytes( uint32_t codepoint )
		{
			if ( !IsValidUnicodeCodePoint( codepoint ) )
			{
				return ( UTF8_FORMAT_ERROR );
			}
			return ( ( codepoint <= UTF8_1_BYTE_MAX_CODE_POINT ) ? UTF8_1_BYTE : \
					 ( ( codepoint <= UTF8_2_BYTE_MAX_CODE_POINT ) ? UTF8_2_BYTES : \
					   ( ( codepoint <= UTF8_3_BYTE_MAX_CODE_POINT ) ? UTF8_3_BYTES : \
						 ( ( codepoint <= UNICODE_MAX_CODE_POINT ) ? UTF8_4_BYTES :
						   UTF8_FORMAT_ERROR ) ) ) );
		}

		// available input in pByte[offset_start .. offset_beyond-1]
		// returns UTF16_ERR_INPUT_LESS_THAN_2_BYTES if not surrogate but < 2 bytes
		// UTF16_ERR_INPUT_LESS_THAN_4_BYTES if surrogate but < 4 bytes
		// UTF16_ERR_FIRST_SURROGATE_NOT_HI if first surrogate not in HI surrogate range
		// UTF16_ERR_SECOND_SURROGATE_NOT_LO if second surrogate not in LO surrogate range
		// UTF16_ERR_OK if successful and stores number of bytes in *pBytesUsed if provided
		static uint8_t UTF16ToCodePoint( const uint8_t* pByte, uint32_t offset_start, uint32_t offset_beyond,
										 bool isLittleEndian, uint32_t* pRes, uint8_t* pBytesUsed = NULL );

		// convert codepoint to UTF8 in pByte[offset_start..offset_beyond-1]
		// returns CODEPOINT_UTF8_ERR_RANGE_ERROR if codepoint > U+10FFFF, or is surrogate
		// CODEPOINT_UTF8_ERR_OUTPUT_INADEQUATE_SPACE if not enough space to store the result bytes
		// CODEPOINT_UTF8_ERR_OK if successful, and stores number of bytes in *pBytesUsed if provided
		static uint8_t CodePointToUTF8( uint32_t codepoint, uint8_t* pByte, uint32_t offset_start,
										uint32_t offset_beyond, uint8_t* pBytesUsed = NULL );

		// decode the first UTF-8 codepoint in pInput[offset_start..offset_beyond-1]
		// returns UTF8_CODEPOINT_ERR_FORMAT_ERROR if input format error
		// UTF8_CODEPOINT_ERR_OK if successful, save result in *pCodepoint, and number of bytes used in *pBytesUsed
		static uint8_t UTF8ToCodePoint( const uint8_t* pInput, uint32_t offset_start, uint32_t offset_beyond,
										uint32_t* pCodepoint, uint8_t* pBytesUsed = NULL );

		// check the first byte to see how many bytes in the UTF-8, return 1, 2, 3, 4, or UTF8_FORMAT_ERROR if not valid
		inline static uint8_t PeekUTF8NumberOfBytes( uint8_t first_byte )
		{
			return ( ( UTF8_1_BYTE_PATTERN == ( UTF8_1_BYTE_MASK & first_byte ) ) ? UTF8_1_BYTE:
					 ( ( UTF8_BYTE_1_OF_2_PATTERN == ( UTF8_BYTE_1_OF_2_MASK & first_byte ) ) ? UTF8_2_BYTES :
					   ( ( UTF8_BYTE_1_OF_3_PATTERN == ( UTF8_BYTE_1_OF_3_MASK & first_byte ) ) ? UTF8_3_BYTES :
						 ( ( UTF8_BYTE_1_OF_4_PATTERN == ( UTF8_BYTE_1_OF_4_MASK & first_byte ) ) ? UTF8_4_BYTES :
						   UTF8_FORMAT_ERROR ) ) ) );
			/*
			   if (UTF8_1_BYTE_PATTERN == (UTF8_1_BYTE_MASK & first_byte) )
			   {
			   // 0b0xxx xxxx
			   return UTF8_1_BYTE;
			   }
			   if (UTF8_BYTE_1_OF_2_PATTERN == (UTF8_BYTE_1_OF_2_MASK & first_byte) )
			   {
			   // 0b110x xxxx
			   return UTF8_2_BYTES;
			   }
			   if (UTF8_BYTE_1_OF_3_PATTERN == (UTF8_BYTE_1_OF_3_MASK & first_byte) )
			   {
			   // 0b1110 xxxx
			   return UTF8_3_BYTES;
			   }
			   if (UTF8_BYTE_1_OF_4_PATTERN == (UTF8_BYTE_1_OF_4_MASK & first_byte) )
			   {
			   // 0b1111 0xxx
			   return UTF8_4_BYTES;
			   }
			   return UTF8_FORMAT_ERROR;
			 */
		}

		// format 0b10xx xxxx
		inline static bool IsValidUTF8OtherByte( uint8_t other_byte )
		{
			return ( ( UTF8_OTHER_BYTE_PATTERN == ( UTF8_OTHER_BYTE_MASK & other_byte ) ) ? true : false );
		}

		// [U+D800, U+DFFF]
		inline static bool CodeUnitIsSurrogate( uint16_t code_unit )
		{
			return ( ( UTF16_MIN_SURROGATE <= code_unit ) && ( UTF16_MAX_SURROGATE >= code_unit ) );
		}

	private:

		// when codepoint <= U+007F, direct copy
		static void CodePointTo1ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset );

		// for codepoint between [U+0080, U+07FF]
		static void CodePointTo2ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset );

		// for codepoint between [U+08FF, U+FFFF]
		static void CodePointTo3ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset );

		// for codepoint between [U+10000, U+1FFFFF]
		static void CodePointTo4ByteUTF8( uint32_t codepoint, uint8_t* pOutput, uint32_t offset );

		// [U+D800, U+DBFF]
		inline static bool CodeUnitIsHiSurrogate( uint16_t code_unit )
		{
			return ( ( UTF16_MIN_SURROGATE <= code_unit ) && ( UTF16_MAX_HI_SURROGATE >= code_unit ) );
		}

		// [U+DC00, U+DFFF]
		inline static bool CodeUnitIsLoSurrogate( uint16_t code_unit )
		{
			return ( ( UTF16_MIN_LO_SURROGATE <= code_unit ) && ( UTF16_MAX_SURROGATE >= code_unit ) );
		}

		// byte 0 LSB, byte 1 MSB
		inline static uint16_t UTF16ToCodeUnitLittleEndian( const uint8_t* pByte, uint32_t offset )
		{
			return ( ( ( uint16_t ) pByte[offset] ) | ( ( ( uint16_t ) pByte[offset + 1] ) << 8 ) );
		}

		// byte 0 MSB, byte 1 LSB
		inline static uint16_t UTF16ToCodeUnitBigEndian( const uint8_t* pByte, uint32_t offset )
		{
			return ( ( ( uint16_t ) pByte[offset + 1] ) | ( ( ( uint16_t ) pByte[offset] ) << 8 ) );
		}

};

#endif	// _UNICODE_UTIL_H
