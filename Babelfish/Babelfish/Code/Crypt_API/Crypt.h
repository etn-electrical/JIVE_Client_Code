/**
 *****************************************************************************************
 *	@file Crypt.h
 *
 *	@brief Base class for Encryption
 *
 *	@details Defines the interface for performing operations with encryption and Decryption
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef _CRYPT_H_
#define _CRYPT_H_

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/**
 * @brief The following two constants are for the size of the length and checksum
 * which is inserted into the encoded string to get data back out again correctly.
 */
#define CRYPT_ENCODED_LENGTH_STORAGE_SIZE ( sizeof( uint32_t ) )
#define CRYPT_ENCODED_CHECKSUM_SIZE ( sizeof( uint16_t ) )

/**
 ****************************************************************************************
 * @brief The Crypt class contains implementations required for the Cryptography operations.
 *
 * @details It provides functionalities to encrypt and decrypt the input data.
 *
 ****************************************************************************************
 */
class Crypt
{
	public:
		/**
		 * @brief Return Status for crypt operations.
		 */
		enum crypt_status_t
		{
			SUCCESS,
			DATA_ERROR,
			BUSY_ERROR,
			OPERATION_NOT_SUPPORTED,
			PENDING_CALL_BACK,
			TIMEOUT_ERROR
		};

		enum aes_key_size_t
		{
			AES_INVALID_KEY = 0U,
			AES_128_KEY = 16U,
			AES_256_KEY = 32U,
		};

		enum aes_iv_size_t
		{
			AES_IV_SIZE_KEY = 16U
		};

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Crypt goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Crypt( void )
		{}

		/**
		 * @brief Encrypts a uint8 string.
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying encrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @return Returns encryption status of type crypt_status_t.
		 */
		virtual crypt_status_t Encrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Decrypts a uint8 string.
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying decrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @return Returns decryption status of type crypt_status_t.
		 */
		virtual crypt_status_t Decrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

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
		 * MIN_U8_ENCODED_XXX_YYY_STRING_SIZE in respective class headers.
		 * on successful encryption, value at ouput_length ptr is updated with length of encrypted
		 * output data.
		 * @return Returns encryption status of type crypt_status_t.
		 */
		virtual crypt_status_t Encrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output, uint32_t* output_length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

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
		virtual crypt_status_t Decrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output, uint32_t* output_length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes for specific devices.
		 */
		Crypt()
		{}

};

#endif	// _CRYPT_H_
