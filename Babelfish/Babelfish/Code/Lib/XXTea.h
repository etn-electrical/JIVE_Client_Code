/**
 *****************************************************************************************
 *	@file
 *
 *	@brief XXTea is an Extremely small encryption algorithm for embedded environments.
 *
 *	@details In cryptography, Corrected Block TEA (often referred to as XXTEA) is a
 *	block cipher designed to correct weaknesses in the original Block TEA.
 *	Formally speaking, XXTEA is a consistent incomplete source-heavy heterogeneous UFN
 *	(unbalanced Feistel network) block cipher. XXTEA operates on variable-length blocks
 *	that are some arbitrary multiple of 32 bits in size (minimum 64 bits). The number of
 *	full cycles depends on the block size, but there are at least six (rising to 32
 *	for small block sizes). The original Block TEA applies the XTEA round function to
 *	each word in the block and combines it additively with its leftmost neighbour.
 *	Slow diffusion rate of the decryption process was immediately exploited to break the
 *	cipher. Corrected Block TEA uses a more involved round function which makes use of
 *	both immediate neighbours in processing each word in the block.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef XXTEA_H
	#define XXTEA_H

#include "Includes.h"

/**
 ****************************************************************************************
 * @brief This class is responsible for encrypting and decrypting XXTea strings.
 * It will encrypt uint8 strings and return the original length.  It is unique because
 * it can encrypt strings that are not aligned by the min encryption size (uint32_t).
 *
 * The code is riddled with magic numbers which defy description and are rooted in
 * encryption algorithmic magic.  Beware the magic numbers of an encryption scheme.
 *
 * @details See details in the class member descriptions below.
 *
 ****************************************************************************************
 */
class XXTea
{
	public:
		/**
		 * @brief The length of a key.  This key is four 32bit values.
		 */
		static const uint8_t KEY_LENGTH = 4U;

		/**
		 * @brief The following two constants are for the size of the length and checksum
		 * which is inserted into the encoded string to get data back out again correctly.
		 */
		static const uint8_t ENCODED_LENGTH_STORAGE_SIZE = sizeof( uint32_t );
		static const uint8_t ENCODED_CHECKSUM_SIZE = sizeof( uint16_t );
		static const uint8_t ALIGNMENT_BYTE_ADJUSTMENT = sizeof( uint32_t ) - 1U;

		/**
		 * @brief The minimum encrypted string size based on uint32 alignment for encryption.
		 * It is a function of the length, checksum, alignment and single byte of data.
		 */
		static const uint8_t MIN_U8_ENCODED_STRING_SIZE =
			( ENCODED_LENGTH_STORAGE_SIZE + ENCODED_CHECKSUM_SIZE +
			  ALIGNMENT_BYTE_ADJUSTMENT + 1U ) & ( ~0x3U );

		/**
		 * @brief If you have a buffer of a certain size (X) it is highly recommended that your
		 * encrypted destination string be the at least that X + MIN_ENCRYPTED_STRING_OVERHEAD.
		 * This overhead includes the size of the original string length, checksum, and uint32
		 * alignment overhead.
		 * Made up of = Min Encoded Size ( Length_Size + Checksum_Size ) + +
		 * adjustment_for_data_being_less_than_4bytes + mem_alignment_compensation.
		 */
		static const uint8_t ENCRYPTED_U8_STRING_BUFF_OVERHEAD =
			ENCODED_LENGTH_STORAGE_SIZE + ENCODED_CHECKSUM_SIZE +
			( MIN_U8_ENCODED_STRING_SIZE -
			  ( ENCODED_LENGTH_STORAGE_SIZE + ENCODED_CHECKSUM_SIZE ) ) +
			sizeof( uint32_t );

		/**
		 * @brief Encrypts a uint8 string.  The length of the encoded data string must be
		 * sufficient to hold the resulting string and the resultant length stored within.
		 * It is very important that 4bytes (uint32) of space be left in the dest string to store
		 * actual uint8 source string length.  It is best to take the max source string length
		 * and add ENCRYPTED_U8_STRING_BUFF_OVERHEAD.
		 * @param src_data - Source data to encrypt.
		 * @param src_length - Length of data in bytes.
		 * @param encoded_data - Place to put the resulting encrypted string.
		 * @param encoded_length - On function call this length parameter should contain the
		 * max length of the encoded data buffer.  The function will fill in the length on
		 * return to indicate how much of the string was used.
		 * @param key - The key must be passed in.  It is a 128bit key made up of four 32bit words.
		 * @return Returns encryption success or failure.  Failure could be due to an incorrect
		 * target data buffer.
		 */
		static bool Encrypt( uint8_t* src_data, uint32_t src_length,
							 uint8_t* encoded_data, uint32_t& encoded_length, uint32_t const* key );

		/**
		 * @brief Encrypts a uint8 string.  The length of the encoded data string must be
		 * sufficient to hold the resulting string.
		 * @param encoded_data - Place to extract the encrypted string from.
		 * @param encoded_length - The length of the encrypted string.
		 * @param encoded_buff_size - The actual size of the encrypted buffer.  It is important
		 * that this string can handle the alignment movement overhead.  Which means the length
		 * has to be encoded data plus 3bytes.
		 * @param decoded_data - Destination for the decrypted string.
		 * @param decoded_length - The length of the decoded data.  The string length shall be
		 * passed in indicating the max size of the destination buffer.  The length will
		 * be returned indicating how much data was decoded.
		 * @param key - The key must be passed in.  It is a 128bit key made up of four 32bit words.
		 * @return Returns decryption success or failure.  Failure could be due to an incorrect
		 * target data buffer.
		 */
		static bool Decrypt( uint8_t* encoded_data, uint32_t encoded_length,
							 uint32_t encoded_buff_size, uint8_t* decoded_data,
							 uint32_t& decoded_length, uint32_t const* key );

		/**
		 * @brief This version of encrypt will encode a uint32 string.
		 * @param data - The uint32 data string to be encrypted.
		 * @param length - The amount of data words to be encrypted.
		 * @param key - The 128bit key in 32bit format.
		 */
		static bool Encrypt( uint32_t* data, uint32_t length, uint32_t const* key );

		static bool Decrypt( uint32_t* data, uint32_t length, uint32_t const* key );

		// Returns true if self test is good.
		static bool Run_Self_Test( void );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		XXTea( void );
		~XXTea( void );
		XXTea( const XXTea& rhs );
		XXTea & operator =( const XXTea& object );

		static const uint32_t XXTEA_DELTA = 0x9e3779b9;
		static const uint32_t PROC_BASE_ADDRESS_MASK = ~0x03U;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline uint32_t MX( uint32_t e, uint32_t y, uint32_t z,
								   uint32_t sum, uint32_t round_ctr, uint32_t const* key )
		{
			return ( ( ( z >> 5 ^ y << 2 ) + ( y >> 3 ^ z << 4 ) ) ^ ( ( sum ^ y ) +
																	   ( key[( round_ctr & 3 ) ^ e] ^ z ) ) );
		}

		static const uint8_t BLANK_ENC_STRING_LEN = ALIGNMENT_BYTE_ADJUSTMENT;
		static const uint8_t BLANK_ENC_STRING[BLANK_ENC_STRING_LEN];

		static const uint8_t TEST_BLOCK_U32_LENGTH = 8U;
		static const uint8_t TEST_BLOCK_U8_LENGTH = TEST_BLOCK_U32_LENGTH * sizeof( uint32_t );

		static const uint8_t PRE_ENC_BLOCK_WLEN = 2U;
		static const uint32_t TEST_BLOCK[PRE_ENC_BLOCK_WLEN];
		static const uint32_t TEST_BLOCK_KEY[KEY_LENGTH];
		static const uint32_t TEST_BLOCK_RESULT[PRE_ENC_BLOCK_WLEN];

};

#endif
