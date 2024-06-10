/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef INT_ASCII_CONVERSION_H
#define INT_ASCII_CONVERSION_H

#include "stdint.h"

#define UINT8_MAX_STRING    "255"

#define UINT16_MAX_STRING   "65535"
#define UINT32_MAX_STRING   "4294967295"
#define SINT32_MIN_STRING   "-2147483648"
#define UINT64_MAX_STRING   "18446744073709551615"


#define STRLEN( x ) ( static_cast<uint16_t>( sizeof( x ) - 1U ) )

static const uint16_t ERR_PARSING_OK = 0U;
static const uint16_t ERR_PARSING_NON_DIGIT = 1U;
static const uint16_t ERR_PARSING_INPUT_ERROR = 2U;
static const uint16_t ERR_PARSING_LEADING_ZERO = 3U;
static const uint16_t ERR_PARSING_BEYOND_BOOL = 4U;
static const uint16_t ERR_PARSING_BEYOND_UINT8 = 5U;
static const uint16_t ERR_PARSING_BEYOND_SINT8 = 6U;
static const uint16_t ERR_PARSING_BEYOND_UINT16 = 7U;
static const uint16_t ERR_PARSING_BEYOND_SINT16 = 8U;
static const uint16_t ERR_PARSING_BEYOND_UINT32 = 9U;
static const uint16_t ERR_PARSING_BEYOND_SINT32 = 10U;
static const uint16_t ERR_PARSING_BEYOND_UINT64 = 11U;
static const uint16_t ERR_PARSING_BEYOND_SINT64 = 12U;
static const uint16_t ERR_PARSING_BEYOND_BYTE = 13U;
static const uint16_t ERR_PARSING_BEYOND_WORD = 14U;
static const uint16_t ERR_PARSING_BEYOND_DWORD = 15U;
static const uint16_t ERR_PARSING_BEYOND_FLOAT = 16U;
static const uint16_t ERR_PARSING_BEYOND_DFLOAT = 17U;
static const uint16_t ERR_PARSING_BEYOND_8BYTES = 18U;

typedef struct U32_PAIR_TD
{
	uint32_t lo;
	uint32_t hi;
} U32_PAIR_TD;

class INT_ASCII_Conversion
{
	public:

		// for smaller data types, call uint32_to_str, or sint32_to_str

		// returns number of digits in the result, not including '\0'
		// p_max_u32_str must be at least a char[sizeof(UINT32_MAX_STRING)]
		static uint8_t uint32_to_str( uint32_t u32, uint8_t* p_max_u32_str );

		// returns number of digits in the result, not including '\0'
		// p_max_u32_str_plus_1 must be at least a char[sizeof(UINT32_MAX_STRING)+1]
		static uint8_t sint32_to_str( int32_t s32, uint8_t* p_max_u32_str_plus_1 );

		// returns number of digits in the result, not including '\0'
		// p_max_u64_str must be at least a char[sizeof(UINT64_MAX_STRING)]
		static uint8_t uint64_to_str( uint64_t u64, uint8_t* p_max_u64_str );

		// returns number of digits in the result, not including '\0'
		// p_max_u64_str_plus_1 must be at least a char[sizeof(UINT64_MAX_STRING)+1]
		static uint8_t sint64_to_str( int64_t u64, uint8_t* p_max_u64_str_plus_1 );

		// parse a uint64 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint64 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_UINT64   - if result > 2^64-1
		static uint8_t str_to_uint64( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint64_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a sint64 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint64 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char other than the first '-' or '+' is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_SINT64   - if result not in [-2^63, 2^63-1]
		static uint8_t str_to_sint64( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, int64_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a uint32 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint32 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_UINT32   - if result > 2^32-1
		static uint8_t str_to_uint32( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint32_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a sint32 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint32 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char other than the first '-' or '+' is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_SINT64   - if result not in [-2^31, 2^31-1]
		static uint8_t str_to_sint32( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, int32_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a uint16 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint16 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_UINT16   - if result > 2^16-1
		static uint8_t str_to_uint16( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint16_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a sint16 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the sint16 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_SINT16   - if result not in [-2^15, 2^15-1]
		static uint8_t str_to_sint16( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, int16_t* pRes,
									  bool_t leading0Allowed = false );

		// parse a uint8 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint8 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_UINT8   - if result > 2^8-1
		static uint8_t str_to_uint8( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint8_t* pRes,
									 bool_t leading0Allowed = false );


		// parse a bool (0 or 1) from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the uint8 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any uint8_t is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_BOOL     - if result > 1
		static uint8_t str_to_bool( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint8_t* pRes,
									bool_t leading0Allowed = false );

		// parse a sint8 number from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the sint8 number if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any uint8_t is not in '0'..'9'
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_SINT8   - if result not in [-2^7, 2^7-1]
		static uint8_t str_to_sint8( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, int8_t* pRes,
									 bool_t leading0Allowed = false );

		// parse a bool from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the byte if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if hasPrefix0X; or
		// if first two char are not "0x" or "0X" if hasPrefix0X
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_BOOL     - if input more than 2 hex digits
		static uint8_t str_hex_to_bool( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint8_t* pRes,
										bool_t hasPrefix0x, bool_t leading0Allowed = true );

		// parse a byte from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the byte if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if hasPrefix0X; or
		// if first two char are not "0x" or "0X" if hasPrefix0X
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_BYTE     - if input more than 2 hex digits
		static uint8_t str_hex_to_byte( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint8_t* pRes,
										bool_t hasPrefix0x, bool_t leading0Allowed = true );

		// parse a word from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the word if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if "0x" is allowed
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_WORD     - if input more than 4 hex digits
		static uint8_t str_hex_to_word( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint16_t* pRes,
										bool_t hasPrefix0x, bool_t leading0Allowed = true );

		// parse a dword from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the dword if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if "0x" is allowed
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_DWORD    - if input more than 8 hex digits
		static uint8_t str_hex_to_dword( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint32_t* pRes,
										 bool_t hasPrefix0x, bool_t leading0Allowed = true );

		// parse a float (32-bit IEEE754 single precision from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the float if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if "0x" is allowed
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_FLOAT    - if input more than 8 hex digits
		static uint8_t str_hex_to_float( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint32_t* pRes,
										 bool_t hasPrefix0x, bool_t leading0Allowed = true );

		// parse a dfloat (64-bit IEEE754 double precision from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the float if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if "0x" is allowed
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_DFLOAT   - if input more than 16 hex digits
		static uint8_t str_hex_to_dfloat( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint64_t* pRes,
										  bool_t hasPrefix0x, bool_t leading0Allowed = true );

		static uint8_t str_hex_to_Bytes( const uint8_t* str, uint32_t first_digit,
										 uint32_t last_digit, uint8_t* pRes );

		// returns number of digits of an unsigned 16-bit integer
		inline static uint8_t UINT16_Number_Digits( uint16_t u16 )
		{
			return ( ( u16 < 10 ) ? 1 : (
						 ( u16 < 100 ) ? 2 : (
							 ( u16 < 1000 ) ? 3 : (
								 ( u16 < 10000 ) ? 4 : 5 ) ) ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static bool_t has0xPrefix( const uint8_t* pInput, uint32_t start, uint32_t end )
		{
			if ( start + STRLEN( "0x" ) - 1 > end )
			{
				return ( false );
			}
			if ( ( '0' == pInput[start] ) && ( ( 'x' == pInput[start + 1] ) || ( 'X' == pInput[start + 1] ) ) )
			{
				return ( true );
			}
			return ( false );
		}

		// parse a 8-byte hex from str[first_digit..last_digit]
		// returns ERR_PARSING_OK, and put the float if successful in pRes
		// otherwise, returns:
		// ERR_PARSING_NON_DIGIT       - if any char is not in '0'..'9', or 'a'..'f', or 'A'..'F', except for "0x"
		// ERR_PARSING_INPUT_ERROR     - if first_digit > last_digit, or first_digt + 2 > last_digit, if "0x" is allowed
		// ERR_PARSING_LEADING_ZERO    - if there is leading zero (except for "0")
		// ERR_PARSING_BEYOND_8BYTES   - if input more than 16 hex digits
		static uint8_t str_hex_to_8Bytes( const uint8_t* str, uint32_t first_digit, uint32_t last_digit, uint64_t* pRes,
										  bool_t hasPrefix0x, bool_t leading0Allowed = true );

		/**
		 * @brief Function that tells whether the given string is numeric or not
		 * @param[in] source_str: Source string
		 * @param[in] length: Max length of the source string
		 * @return True if string is numeric, else returns false
		 */
		static bool Is_String_Numeric( const uint8_t* source_str, uint32_t length );

	private:
		static const U32_PAIR_TD comp_table[sizeof( UINT64_MAX_STRING ) - 1][9];
		static const uint8_t SINT8_MAX = 0x7FU;
		static const uint16_t SINT16_MAX = 0x7FFFU;
		static const uint32_t SINT32_MAX = 0x7FFFFFFFL;
		static const uint64_t SINT64_MAX = 0x7FFFFFFFFFFFFFFFULL;
};


#endif	// INT_ASCII_CONVERSION_H
