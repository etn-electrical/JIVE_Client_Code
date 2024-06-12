/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MD5_DIGEST_H
#define MD5_DIGEST_H

#define A_INIT 0x67452301U
#define B_INIT 0xefcdab89U
#define C_INIT 0x98badcfeU
#define D_INIT 0x10325476U



// this implementation is based on RFC 1321 with the following limitations
// 1) byte based MD5, so only applicable to where (number of input bits)%8==0
// 2) input byte number is limited to a 32 bit unsigned integer (0~2^32-1)
// it is designed to be fast although more memory is used
class MD5_Digest
{
	public:
		static void Perform_Digest( const uint8_t* pInput, uint32_t input_byte_number, uint8_t* pOutputHex );

		inline static const uint8_t* Get_Hex_Array() {return ( hex_array );}

	private:
		static uint32_t A, B, C, D;
		static uint32_t AA, BB, CC, DD;
		static uint32_t X[16];
		static const uint8_t hex_array[17];

		static void store_word_in_hex( uint32_t word, uint8_t* pOutputHex, uint8_t output_offset );

		static void store_output_in_hex( uint8_t* pOutputHex );

		static void init_buffers() {A = A_INIT;B = B_INIT;C = C_INIT;D = D_INIT;}

};

#endif	// MD5_DIGEST_H
