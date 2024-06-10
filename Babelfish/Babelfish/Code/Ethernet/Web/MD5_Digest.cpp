/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <string.h>

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "MD5_Digest.h"

#define T_1 0xd76aa478U
#define T_2 0xe8c7b756U
#define T_3 0x242070dbU
#define T_4 0xc1bdceeeU
#define T_5 0xf57c0fafU
#define T_6 0x4787c62aU
#define T_7 0xa8304613U
#define T_8 0xfd469501U
#define T_9 0x698098d8U
#define T_10 0x8b44f7afU
#define T_11 0xffff5bb1U
#define T_12 0x895cd7beU
#define T_13 0x6b901122U
#define T_14 0xfd987193U
#define T_15 0xa679438eU
#define T_16 0x49b40821U
#define T_17 0xf61e2562U
#define T_18 0xc040b340U
#define T_19 0x265e5a51U
#define T_20 0xe9b6c7aaU
#define T_21 0xd62f105dU
#define T_22 0x2441453U
#define T_23 0xd8a1e681U
#define T_24 0xe7d3fbc8U
#define T_25 0x21e1cde6U
#define T_26 0xc33707d6U
#define T_27 0xf4d50d87U
#define T_28 0x455a14edU
#define T_29 0xa9e3e905U
#define T_30 0xfcefa3f8U
#define T_31 0x676f02d9U
#define T_32 0x8d2a4c8aU
#define T_33 0xfffa3942U
#define T_34 0x8771f681U
#define T_35 0x6d9d6122U
#define T_36 0xfde5380cU
#define T_37 0xa4beea44U
#define T_38 0x4bdecfa9U
#define T_39 0xf6bb4b60U
#define T_40 0xbebfbc70U
#define T_41 0x289b7ec6U
#define T_42 0xeaa127faU
#define T_43 0xd4ef3085U
#define T_44 0x4881d05U
#define T_45 0xd9d4d039U
#define T_46 0xe6db99e5U
#define T_47 0x1fa27cf8U
#define T_48 0xc4ac5665U
#define T_49 0xf4292244U
#define T_50 0x432aff97U
#define T_51 0xab9423a7U
#define T_52 0xfc93a039U
#define T_53 0x655b59c3U
#define T_54 0x8f0ccc92U
#define T_55 0xffeff47dU
#define T_56 0x85845dd1U
#define T_57 0x6fa87e4fU
#define T_58 0xfe2ce6e0U
#define T_59 0xa3014314U
#define T_60 0x4e0811a1U
#define T_61 0xf7537e82U
#define T_62 0xbd3af235U
#define T_63 0x2ad7d2bbU
#define T_64 0xeb86d391U

#define F( X, Y, Z ) ( ( ( X ) &( Y ) ) | ( ~( X ) &( Z ) ) )
#define G( X, Y, Z ) ( ( ( X ) &( Z ) ) | ( ( Y ) &~( Z ) ) )
#define H( X, Y, Z ) ( ( X ) ^ ( Y ) ^ ( Z ) )
#define I( X, Y, Z ) ( ( Y ) ^ ( ( X ) | ~( Z ) ) )

#define ROT( x, s ) ( ( ( x ) << ( s ) ) | ( ( x ) >> ( 32 - ( s ) ) ) )

#define F_OPERATION( a, b, c, d, k, s, i, x ) \
	/*do*/ {( a ) = \
				static_cast<uint32_t>( ( b ) + ROT( ( a ) + F( ( b ), ( c ), \
															   ( d ) ) + ( ( x )[( k )] ) + T_ ## i, ( s ) ) );}
// while (0);

#define G_OPERATION( a, b, c, d, k, s, i, x ) \
	/*do*/ {( a ) = \
				static_cast<uint32_t>( ( b ) + ROT( ( a ) + G( ( b ), ( c ), \
															   ( d ) ) + ( ( x )[( k )] ) + T_ ## i, ( s ) ) );}
// while (0);

#define H_OPERATION( a, b, c, d, k, s, i, x ) \
	/*do*/ {( a ) = \
				static_cast<uint32_t>( ( b ) + ROT( ( a ) + H( ( b ), ( c ), \
															   ( d ) ) + ( ( x )[( k )] ) + T_ ## i, ( s ) ) );}
// while (0);

#define I_OPERATION( a, b, c, d, k, s, i, x ) \
	/*do*/ {( a ) = \
				static_cast<uint32_t>( ( b ) + ROT( ( a ) + I( ( b ), ( c ), \
															   ( d ) ) + ( ( x )[( k )] ) + T_ ## i, ( s ) ) );}
// while (0);

uint32_t MD5_Digest::A = A_INIT;
uint32_t MD5_Digest::B = B_INIT;
uint32_t MD5_Digest::C = C_INIT;
uint32_t MD5_Digest::D = D_INIT;

uint32_t MD5_Digest::AA = A_INIT;
uint32_t MD5_Digest::BB = B_INIT;
uint32_t MD5_Digest::CC = C_INIT;
uint32_t MD5_Digest::DD = D_INIT;

uint32_t MD5_Digest::X[16] =
{ 0U };

const uint8_t MD5_Digest::hex_array[17] = "0123456789abcdef";

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void MD5_Digest::store_word_in_hex( uint32_t word, uint8_t* pOutputHex, uint8_t output_offset )
{
	pOutputHex[output_offset] = hex_array[( word & 0x000000f0U ) >> 4U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x0000000fU ) >> 0U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x0000f000U ) >> 12U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x00000f00U ) >> 8U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x00f00000U ) >> 20U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x000f0000U ) >> 16U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0xf0000000U ) >> 28U];
	output_offset++;
	pOutputHex[output_offset] = hex_array[( word & 0x0f000000U ) >> 24U];
	output_offset++;
}

// pOutputHex needs to have at least 32 bytes
void MD5_Digest::store_output_in_hex( uint8_t* pOutputHex )
{
	store_word_in_hex( A, pOutputHex, 0U );
	store_word_in_hex( B, pOutputHex, 8U );
	store_word_in_hex( C, pOutputHex, 16U );
	store_word_in_hex( D, pOutputHex, 24U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void MD5_Digest::Perform_Digest( const uint8_t* pInput, uint32_t input_byte_number,
								 uint8_t* pOutputHex )
{
	uint32_t i;
	uint32_t num_rounds;
	uint32_t offset = 0U;

	init_buffers();

	num_rounds = ( ( input_byte_number & 0x3fU ) >= 56U )
					? ( ( input_byte_number >> 6U ) + 2U )
						: ( ( input_byte_number >> 6U ) + 1U );
	for ( i = 0U; i < num_rounds; i++ )
	{
		if ( 0U == ( input_byte_number & 0x3fU ) )
		{
			if ( ( num_rounds - 1U ) == i )
			{
				memset( reinterpret_cast<uint8_t*>( X ), 0, 56U );
				*( reinterpret_cast<uint8_t*>( X ) ) = 0x80U;
			}
			else
			{
				memcpy( reinterpret_cast<uint8_t*>( X ), &pInput[offset], 64U );
				offset += 64U;
			}
		}
		else
		{
			if ( ( num_rounds - 1U ) == i )
			{
				if ( ( input_byte_number & 0x3fU ) >= 56U )
				{
					memset( reinterpret_cast<uint8_t*>( X ), 0, 56U );
				}
				else
				{
					memset( reinterpret_cast<uint8_t*>( X ), 0, 64U );
					memcpy( reinterpret_cast<uint8_t*>( X ),
							&pInput[input_byte_number & 0xffffffc0U], input_byte_number & 0x3fU );
					offset += ( input_byte_number & 0x3fU );
					*( ( reinterpret_cast<uint8_t*>( X ) ) + ( input_byte_number & 0x3fU ) ) =
						0x80U;
				}
			}
			else if ( ( num_rounds - 2U ) == i )
			{
				if ( ( input_byte_number & 0x3fU ) >= 56U )
				{
					// TODO
					memset( reinterpret_cast<uint8_t*>( X ), 0, 64U );
					memcpy( reinterpret_cast<uint8_t*>( X ),
							&pInput[input_byte_number & 0xffffffc0U], input_byte_number & 0x3fU );
					offset += ( input_byte_number & 0x3fU );
					*( ( reinterpret_cast<uint8_t*>( X ) ) + ( input_byte_number & 0x3fU ) ) =
						0x80U;
				}
				else
				{
					memcpy( reinterpret_cast<uint8_t*>( X ), &pInput[offset], 64U );
					offset += 64U;
				}
			}
			else
			{
				memcpy( reinterpret_cast<uint8_t*>( X ), &pInput[offset], 64U );
				offset += 64U;
			}
		}
		if ( ( num_rounds - 1U ) == i )
		{
			X[14] = input_byte_number << 3U;
			X[15] = ( input_byte_number & 0xe0000000U ) >> 29;
		}

		/* now starts the per-512b operation */
		AA = A;
		BB = B;
		CC = C;
		DD = D;

		F_OPERATION( A, B, C, D, 0, 7, 1, X )
		F_OPERATION( D, A, B, C, 1, 12, 2, X )
		F_OPERATION( C, D, A, B, 2, 17, 3, X )
		F_OPERATION( B, C, D, A, 3, 22, 4, X )

		F_OPERATION( A, B, C, D, 4, 7, 5, X )
		F_OPERATION( D, A, B, C, 5, 12, 6, X )
		F_OPERATION( C, D, A, B, 6, 17, 7, X )
		F_OPERATION( B, C, D, A, 7, 22, 8, X )

		F_OPERATION( A, B, C, D, 8, 7, 9, X )
		F_OPERATION( D, A, B, C, 9, 12, 10, X )
		F_OPERATION( C, D, A, B, 10, 17, 11, X )
		F_OPERATION( B, C, D, A, 11, 22, 12, X )

		F_OPERATION( A, B, C, D, 12, 7, 13, X )
		F_OPERATION( D, A, B, C, 13, 12, 14, X )
		F_OPERATION( C, D, A, B, 14, 17, 15, X )
		F_OPERATION( B, C, D, A, 15, 22, 16, X )

		G_OPERATION( A, B, C, D, 1, 5, 17, X )
		G_OPERATION( D, A, B, C, 6, 9, 18, X )
		G_OPERATION( C, D, A, B, 11, 14, 19, X )
		G_OPERATION( B, C, D, A, 0, 20, 20, X )

		G_OPERATION( A, B, C, D, 5, 5, 21, X )
		G_OPERATION( D, A, B, C, 10, 9, 22, X )
		G_OPERATION( C, D, A, B, 15, 14, 23, X )
		G_OPERATION( B, C, D, A, 4, 20, 24, X )

		G_OPERATION( A, B, C, D, 9, 5, 25, X )
		G_OPERATION( D, A, B, C, 14, 9, 26, X )
		G_OPERATION( C, D, A, B, 3, 14, 27, X )
		G_OPERATION( B, C, D, A, 8, 20, 28, X )

		G_OPERATION( A, B, C, D, 13, 5, 29, X )
		G_OPERATION( D, A, B, C, 2, 9, 30, X )
		G_OPERATION( C, D, A, B, 7, 14, 31, X )
		G_OPERATION( B, C, D, A, 12, 20, 32, X )

		H_OPERATION( A, B, C, D, 5, 4, 33, X )
		H_OPERATION( D, A, B, C, 8, 11, 34, X )
		H_OPERATION( C, D, A, B, 11, 16, 35, X )
		H_OPERATION( B, C, D, A, 14, 23, 36, X )

		H_OPERATION( A, B, C, D, 1, 4, 37, X )
		H_OPERATION( D, A, B, C, 4, 11, 38, X )
		H_OPERATION( C, D, A, B, 7, 16, 39, X )
		H_OPERATION( B, C, D, A, 10, 23, 40, X )

		H_OPERATION( A, B, C, D, 13, 4, 41, X )
		H_OPERATION( D, A, B, C, 0, 11, 42, X )
		H_OPERATION( C, D, A, B, 3, 16, 43, X )
		H_OPERATION( B, C, D, A, 6, 23, 44, X )

		H_OPERATION( A, B, C, D, 9, 4, 45, X )
		H_OPERATION( D, A, B, C, 12, 11, 46, X )
		H_OPERATION( C, D, A, B, 15, 16, 47, X )
		H_OPERATION( B, C, D, A, 2, 23, 48, X )

		I_OPERATION( A, B, C, D, 0, 6, 49, X )
		I_OPERATION( D, A, B, C, 7, 10, 50, X )
		I_OPERATION( C, D, A, B, 14, 15, 51, X )
		I_OPERATION( B, C, D, A, 5, 21, 52, X )

		I_OPERATION( A, B, C, D, 12, 6, 53, X )
		I_OPERATION( D, A, B, C, 3, 10, 54, X )
		I_OPERATION( C, D, A, B, 10, 15, 55, X )
		I_OPERATION( B, C, D, A, 1, 21, 56, X )

		I_OPERATION( A, B, C, D, 8, 6, 57, X )
		I_OPERATION( D, A, B, C, 15, 10, 58, X )
		I_OPERATION( C, D, A, B, 6, 15, 59, X )
		I_OPERATION( B, C, D, A, 13, 21, 60, X )

		I_OPERATION( A, B, C, D, 4, 6, 61, X )
		I_OPERATION( D, A, B, C, 11, 10, 62, X )
		I_OPERATION( C, D, A, B, 2, 15, 63, X )
		I_OPERATION( B, C, D, A, 9, 21, 64, X )

		A += AA;
		B += BB;
		C += CC;
		D += DD;
	}

	store_output_in_hex( pOutputHex );
}
