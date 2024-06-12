/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "Base64_Rest.h"
#ifdef ESP32_SETUP
#include "Etn_Types.h"
#endif

const uint8_t Base64::enc_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/*
   {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z', '0', '1', '2', '3',
   '4', '5', '6', '7', '8', '9', '+', '/',
   0
   };
 */

#ifdef BASE64_DECODING_USE_TABLE_LOOKUP

const uint8_t Base64::dec_table[256] =
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
};
#endif	// BASE64_DECODING_USE_TABLE_LOOKUP
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Base64::Encode( const uint8_t* pOriginal, uint32_t uInputLength, uint8_t* pRes,
						 uint32_t offset )
{
	uint32_t res_length = 0U;

	// advance input pointer according to the offset
	pOriginal += offset;

	while ( uInputLength > 0U )
	{
		// 1st encoded character
		pRes[res_length] = enc_table[pOriginal[0] >> 2];
		res_length++;

		// 2nd, 3rd and 4th encoded character
		switch ( uInputLength )
		{
			case 1:
				pRes[res_length] =
					enc_table[( static_cast<uint8_t>( ( static_cast<uint8_t>( pOriginal[0] & 0x3U ) )
													  << 4U ) )];
				res_length++;
				pRes[res_length] = 0x3DU;	/*..'=' */
				res_length++;
				pRes[res_length] = 0x3DU;	/*..'=' */
				res_length++;
				break;

			case 2:
				pRes[res_length] =
					enc_table[( static_cast<uint8_t>( ( static_cast<uint8_t>( ( pOriginal[0] & 0x3U )
																			  << 4U ) )
													  | ( static_cast<uint8_t>( pOriginal[1] >> 4U ) ) ) )];
				res_length++;
				pRes[res_length] =
					enc_table[( static_cast<uint8_t>( ( pOriginal[1] & 0xFU ) << 2U ) )];
				res_length++;
				pRes[res_length] = 0x3DU;	/*..'=' */
				res_length++;
				break;

			default:
				pRes[res_length] =
					enc_table[( static_cast<uint8_t>( ( static_cast<uint8_t>( ( pOriginal[0] & 0x3U )
																			  << 4U ) )
													  | ( static_cast<uint8_t>( pOriginal[1] >> 4U ) ) ) )];
				res_length++;
				pRes[res_length] =
					enc_table[( static_cast<uint8_t>( ( static_cast<uint8_t>( ( pOriginal[1] & 0xFU )
																			  << 2U ) ) |
													  ( static_cast<uint8_t>( pOriginal[2] >> 6U ) ) ) )];
				res_length++;
				pRes[res_length] = enc_table[( static_cast<uint8_t>( pOriginal[2] & 0x3FU ) )];
				res_length++;
		}

		if ( uInputLength > 3U )
		{
			uInputLength -= 3U;
			pOriginal += 3U;
		}
		else
		{
			uInputLength = 0U;
		}
	}
	return ( res_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Base64::Decode( const uint8_t* pEncoded, uint32_t uInputLength, uint8_t* pDecoded,
						uint32_t* uOutputLength, uint32_t offset )
{
	uint8_t c = 0U;
	uint8_t i = 0U;
	uint8_t buf[4] = { 0U };
	uint8_t padding = static_cast<uint8_t>( PADDING_0 );
	uint8_t return_val = 0U;
	bool_t function_end = false;

	*uOutputLength = 0U;

	// advance input pointer according to the offset
	pEncoded += offset;

	// input length has to be a multiple of 4
	if ( ( uInputLength < 4U ) || ( 0U != ( uInputLength & 0x3U ) ) )
	{
		return_val = static_cast<uint8_t>( BASE64_DECODING_ERR_WRONG_INPUT_LENGTH );
	}
	else
	{
		if ( 0x3DU == pEncoded[uInputLength - 1U] )	/*..'=' */
		{
			if ( 0x3DU == pEncoded[uInputLength - 2U] )	/*..'=' */
			{
				// last two encoded characters are both '='
				padding = static_cast<uint8_t>( PADDING_2 );
			}
			else
			{
				// last one encoded character is '='
				padding = static_cast<uint8_t>( PADDING_1 );
			}
		}
		bool_t continue_flag = false;
		bool_t break_flag = false;
		while ( ( 4U <= uInputLength ) && ( false == function_end ) )
		{

			// check the validity of input encoded characters and
			// fill buffer for the intermediate step of decoding
			for ( i = 0U; ( ( i < 4U ) && ( false == break_flag ) ); i++ )
			{
				c = pEncoded[i];

#ifdef BASE64_DECODING_USE_TABLE_LOOKUP
				buf[i] = dec_table[c];
				if ( ( false == continue_flag ) && ( 0U == ( buf[i] & 0xC0U ) ) )
				{
					continue_flag = true;
				}
#else
				if ( ( false == continue_flag ) && ( ( 0x41U <= c ) && ( 0x5AU >= c ) ) )	/* A..Z */
				{
					buf[i] = ( c - 0x41U );	/* A */
					continue_flag = true;
				}
				if ( ( false == continue_flag ) && ( ( 0x61U <= c ) && ( 0x7AU >= c ) ) )	/* a..z */
				{
					buf[i] = ( c - 0x61U ) + 26U;	/* a */
					continue_flag = true;
				}
				if ( ( false == continue_flag ) && ( ( 0x30U <= c ) && ( 0x39U >= c ) ) )	/* 0..9 */
				{
					buf[i] = ( c - 0x30U ) + 52U;	/* 0 */
					continue_flag = true;
				}
				if ( ( false == continue_flag ) && ( 0x2BU == c ) )	/* + */
				{
					buf[i] = 62U;
					continue_flag = true;
				}
				if ( ( false == continue_flag ) && ( 0x2FU == c ) )	/* / */
				{
					buf[i] = 63U;
					continue_flag = true;
				}
#endif	// BASE64_DECODING_USE_TABLE_LOOKUP
				if ( ( false == continue_flag ) && ( 0x3DU == c ) )	/* = */
				{
					if ( 4U >= uInputLength )
					{
						// has to be the last two encoded characters
						if ( 2U == i )
						{
							if ( ( static_cast<uint8_t>( PADDING_2 ) ) == padding )
							{
								break_flag = true;
							}
							else
							{
								return_val =
									static_cast<uint8_t>( BASE64_DECODING_ERR_WRONG_INPUT_CHAR );
								function_end = true;
							}
						}
						else if ( 3U == i )
						{
							// only last encoded character is '='
							break_flag = true;
						}
						else
						{
							// '=' cannot appear other than the last two encoded characters
							return_val =
								static_cast<uint8_t>( BASE64_DECODING_ERR_WRONG_INPUT_CHAR );
							function_end = true;
						}
					}
					else
					{
						// '=' cannot appear other than the last two encoded characters
						return_val = static_cast<uint8_t>( BASE64_DECODING_ERR_WRONG_INPUT_CHAR );
						function_end = true;
					}
				}
				continue_flag = false;
			}

			if ( false == function_end )
			{
				// start filling the decoded octets
				// first decoded octet
				pDecoded[( *uOutputLength )] =
					static_cast<uint8_t>( static_cast<uint8_t>( buf[0] << 2 ) | static_cast<uint8_t>( buf[1] >> 4 ) );
				( *uOutputLength )++;

				if ( ( 4U >= uInputLength ) && ( ( static_cast<uint8_t>( PADDING_2 ) ) == padding ) )
				{
					// stop here, do nothing further
				}
				else
				{
					// not the last four encoded characters, or PADDING_0 or PADDING_1
					// second decoded octet
					pDecoded[( *uOutputLength )] =
						static_cast<uint8_t>( static_cast<uint8_t>( ( ( buf[1] & 0xFU ) << 4 ) ) |
											  static_cast<uint8_t>( buf[2] >> 2 ) );
					( *uOutputLength )++;

					if ( ( 4U >= uInputLength ) && ( ( static_cast<uint8_t>( PADDING_1 ) ) == padding ) )
					{
						// stop here, do nothing further
					}
					else
					{
						// not the last four encoded characters, or PADDING_0
						if ( ( 4U < uInputLength ) || ( ( static_cast<uint8_t>( PADDING_0 ) ) == padding ) )
						{
							// third decoded octet
							pDecoded[( *uOutputLength )] =
								static_cast<uint8_t>( static_cast<uint8_t>( ( ( buf[2] & 0x3U ) << 6 ) ) | buf[3] );
							( *uOutputLength )++;
						}
						else
						{
							// uInputLength <= 4 and PADDING_0!= padding
							// should not happen, defensive programming
							return_val = static_cast<uint8_t>( BASE64_DECODING_ERR_WRONG_INPUT_CHAR );
							function_end = true;
						}
					}
				}
				if ( false == function_end )
				{
					uInputLength -= 4U;
					pEncoded += 4U;
				}
			}
		}
		if ( false == function_end )
		{

			return_val = static_cast<uint8_t>( BASE64_DECODING_ERR_OK );
		}
	}
	return ( return_val );
}
