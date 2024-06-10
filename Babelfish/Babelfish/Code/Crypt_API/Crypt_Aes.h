/**
 *****************************************************************************************
 *	@file Crypt_Aes.h
 *
 *	@brief Base class for AES Encryption
 *
 *	@details Defines the interface for performing operations with encryption and Decryption
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef _CRYPT_AES_H_
#define _CRYPT_AES_H_

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief
 * @details
 *
 ****************************************************************************************
 */
class Crypt_Aes
{
	public:
		/**
		 * @brief Return Status for NV_Ctrl operations
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
		virtual ~Crypt_Aes( void )
		{}

		/**
		 * @brief Encrypts a uint8 string.  The length of the input buffer must be in multiple of 16 bytes.
		 * @param key - The key must be passed in.  It is a 128bit key passed as uint8_t pointer
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of data in bytes.
		 * @param output - (optional) Place to put the resultkeying encrypted string.
		 *                 when passed as null, input buffer is used to store encrypted data.
		 *                 Length of the encrypted data always equal to input length.
		 * @return Returns encryption success or failure.  Failure could be due to an incorrect
		 * target data buffer or size.
		 */
		virtual crypt_status_t Encrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output, uint32_t output_length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief decrypts a uint8 encrypted input string.  The length of the input buffer must be in multiple of 16
		 * bytes.
		 * @param key - The key must be passed in.  It is a 128bit key passed as uint8_t pointer
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of data in bytes.
		 * @param output - (optional) Place to put the resulting decrypted string.
		 *                 when passed as null, input buffer is used to store decrypted data.
		 *                 Length of the decrypted data always equal to input length.
		 * @return Returns decryption success or failure.  Failure could be due to an incorrect
		 * target data buffer or size.
		 */
		virtual crypt_status_t Decrypt( uint8_t* input, uint32_t input_length,
										uint8_t* output, uint32_t output_length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes for specific devices.
		 */
		Crypt_Aes()
		{}

};

#endif	// _CRYPT_AES_H_
