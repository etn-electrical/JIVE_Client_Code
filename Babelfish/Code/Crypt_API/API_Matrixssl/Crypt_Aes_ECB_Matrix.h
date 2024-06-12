/**
 *****************************************************************************************
 *	@file Crypt_Aes_ECB_Matrix.h
 *
 *	@brief Class for AES cryptography operations using matrix-ssl library
 *
 *	@details Defines the interface for performing AES encryption with ECB mode of operation.
 *	This class uses APIs from matrix ssl library to perform encryption and decryption.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef CRYPT_AES_ECB_MATRIX_H
#define CRYPT_AES_ECB_MATRIX_H

#include "Crypt.h"
#include "matrixssl/matrixsslApi.h"
#include "../crypto/digest/digest.h"



/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/**
 * @brief minimum block size of data
 */
#define CRYPT_AES_ECB_BLOCK_LENGTH ( 16U )

/**
 * @brief mask to validate for minimum block size
 */
#define CRYPT_AES_ECB_BLOCK_LENGTH_MASK ( CRYPT_AES_ECB_BLOCK_LENGTH - 1U )

/**
 * @brief The minimum encrypted string size based on CRYPT_AES_CBC_BLOCK_LENGTH for encryption.
 * It is a function of the length, checksum and size of input data.
 */
#define MIN_U8_ENCODED_AES_ECB_STRING_SIZE( input ) ( CRYPT_AES_ECB_BLOCK_LENGTH * \
													  ( ( CRYPT_ENCODED_LENGTH_STORAGE_SIZE + \
														  CRYPT_ENCODED_CHECKSUM_SIZE + input + \
														  CRYPT_AES_ECB_BLOCK_LENGTH_MASK ) / \
														CRYPT_AES_ECB_BLOCK_LENGTH ) )


/**
 ****************************************************************************************
 * @brief Contain Encryption and Decryption function for AES ECB mode of operation.
 * @details It provides different functions to encrypt and decrypt data with fixed and variable
 * size.
 * This class uses APIs from matrix-ssl library and encrypt and decrypt the
 * data considering ECB mode of operation.
 *
 ****************************************************************************************
 */

class Crypt_Aes_ECB_Matrix : public Crypt
{
	public:

		/**
		 * @brief               Constructor to create instance of Aes_Crypt class.
		 * @param[in] key       AES key.
		 * @param[in] key_size  size of the AES key of type aes_key_size_t
		 */
		Crypt_Aes_ECB_Matrix( uint8_t const* key, aes_key_size_t key_size );

		/**
		 * @brief Encrypts a uint8 string.
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying encrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @return Returns encryption status of type crypt_status_t.
		 */
		virtual Crypt::crypt_status_t Encrypt( uint8_t* input, uint32_t input_length,
											   uint8_t* output );

		/**
		 * @brief Decrypts a uint8 string.
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying decrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @return Returns decryption status of type crypt_status_t.
		 */
		virtual Crypt::crypt_status_t Decrypt( uint8_t* input, uint32_t input_length,
											   uint8_t* output );

		/**
		 * @brief Encrypts a uint8 string.
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the resulting encrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @param output_length - in_out parameter. length of output buffer must be passed in
		 * output_length.
		 * the length should be sufficient enough to hold encrypted output.
		 * This function also stores actual length of input data and checksum in the encrypted data,
		 * therefore length of outout_buffer should be sufficient enough to hold encrypted data.
		 * the maximum length require can be calculated using function
		 * MIN_U8_ENCODED_AES_ECB_STRING_SIZE.
		 * on successful encryption, value at ouput_length ptr is updated with length of encrypted
		 * output data.
		 * @return Returns encryption status of type crypt_status_t.
		 */
		virtual Crypt::crypt_status_t Encrypt( uint8_t* input, uint32_t input_length,
											   uint8_t* output, uint32_t* output_length );

		/**
		 * @brief Decrypt a uint8 string.
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the resulting decrypted string.
		 * @param output_length - in_out parameter. length of output buffer must be passed in
		 * output_length.
		 * the length should be sufficient enough to hold decrypted output.
		 * Apart from decryption this function also validates the checksum of decrypted data against
		 * stored checksum and update the length of actual (decrypted) data in value at ptr
		 * output_length.
		 * on successful decryption, value at ouput_length ptr is updated with length of decrypted
		 * output data.
		 * @return Returns decryption status of type crypt_status_t.
		 */
		virtual Crypt::crypt_status_t Decrypt( uint8_t* input, uint32_t input_length,
											   uint8_t* output, uint32_t* output_length );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		~Crypt_Aes_ECB_Matrix( void );
		Crypt_Aes_ECB_Matrix( const Crypt_Aes_ECB_Matrix& rhs );
		Crypt_Aes_ECB_Matrix & operator =( const Crypt_Aes_ECB_Matrix& object );

		uint8_t const* m_key;
		psAesKey_t* m_ctx;
		aes_key_size_t m_key_size;

		static const uint8_t BLANK_ENC_STRING_LEN = CRYPT_AES_ECB_BLOCK_LENGTH_MASK;
		static const uint8_t BLANK_ENC_STRING[BLANK_ENC_STRING_LEN];
};

#endif	// CRYPT_AES_ECB_MATRIX_H
