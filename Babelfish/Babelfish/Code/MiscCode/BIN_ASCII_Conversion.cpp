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

#define BYTE_TO_LO_NIBBLE( byte ) ( ( byte ) & 0xFU )
#define BYTE_TO_HI_NIBBLE( byte ) ( ( ( byte ) & 0xF0U ) >> 4 )
#define NIBBLE_TO_CHAR( nibble ) ( ( ( nibble ) <= 9U )? ( ( nibble ) + 0x30U ) : ( ( ( nibble ) - 10U ) + 0x41U ) )/*
																													   '0'..'A'
																													 */
#define BYTE_LO_NIBBLE_TO_CHAR( byte ) ( NIBBLE_TO_CHAR( BYTE_TO_LO_NIBBLE( static_cast<uint8_t>( byte ) ) ) )
#define BYTE_HI_NIBBLE_TO_CHAR( byte ) ( NIBBLE_TO_CHAR( BYTE_TO_HI_NIBBLE( static_cast<uint8_t>( byte ) ) ) )

#define SET_HEX_PREFIX( str, offset ) \
	do { \
		( str )[( offset )] = 0x30U; \
		( str )[1U + ( offset )] = 0x78U; \
	} while ( 0 );
/* 0x30U ..for 0 0x78 ..for x */
#define SET_HEX_DIGIT( str, offset, byte ) \
	do { \
		( str )[( offset )] = BYTE_HI_NIBBLE_TO_CHAR( byte ); \
		( str )[( offset ) + 1U] = BYTE_LO_NIBBLE_TO_CHAR( byte ); \
	} while ( 0 );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t BIN_ASCII_Conversion::byte_to_str( const uint8_t* pBytes, uint8_t* p_ascii )
{
	SET_HEX_PREFIX( p_ascii, 0 );
	SET_HEX_DIGIT( p_ascii, 2U, pBytes[0] );
	return ( static_cast<uint8_t>( ( ( sizeof ( "0x" ) ) - 1U )
								   + ( static_cast<uint8_t>( sizeof( uint8_t ) << 1 ) ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t BIN_ASCII_Conversion::word_to_str( const uint8_t* pBytes, uint8_t* p_ascii )
{
	SET_HEX_PREFIX( p_ascii, 0U );
	SET_HEX_DIGIT( p_ascii, 2U, pBytes[1] );
	SET_HEX_DIGIT( p_ascii, 4U, pBytes[0] );
	return ( static_cast<uint8_t>( ( ( sizeof ( "0x" ) ) - 1 ) + ( sizeof( uint16_t ) << 1 ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t BIN_ASCII_Conversion::dword_to_str( const uint8_t* pBytes, uint8_t* p_ascii )
{
	SET_HEX_PREFIX( p_ascii, 0 );
	SET_HEX_DIGIT( p_ascii, 2, pBytes[3] );
	SET_HEX_DIGIT( p_ascii, 4, pBytes[2] );
	SET_HEX_DIGIT( p_ascii, 6, pBytes[1] );
	SET_HEX_DIGIT( p_ascii, 8, pBytes[0] );
	return ( static_cast<uint8_t>( ( ( sizeof ( "0x" ) ) - 1 ) + ( sizeof( uint32_t ) << 1 ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t BIN_ASCII_Conversion::float_to_str( const uint8_t* pBytes, uint8_t* p_ascii )
{
	return ( dword_to_str( pBytes, p_ascii ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t BIN_ASCII_Conversion::dfloat_to_str( const uint8_t* pBytes, uint8_t* p_ascii )
{
	SET_HEX_PREFIX( p_ascii, 0 );
	SET_HEX_DIGIT( p_ascii, 2, pBytes[7] );
	SET_HEX_DIGIT( p_ascii, 4, pBytes[6] );
	SET_HEX_DIGIT( p_ascii, 6, pBytes[5] );
	SET_HEX_DIGIT( p_ascii, 8, pBytes[4] );
	SET_HEX_DIGIT( p_ascii, 10, pBytes[3] );
	SET_HEX_DIGIT( p_ascii, 12, pBytes[2] );
	SET_HEX_DIGIT( p_ascii, 14, pBytes[1] );
	SET_HEX_DIGIT( p_ascii, 16, pBytes[0] );
	return ( static_cast<uint8_t>( ( ( sizeof ( "0x" ) - 1U ) + ( sizeof( uint64_t ) << 1 ) ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BIN_ASCII_Conversion::byte_to_str_no_prefix( const uint32_t pBytes, uint8_t* p_ascii )
{
	SET_HEX_DIGIT( p_ascii, 0U, pBytes );
}