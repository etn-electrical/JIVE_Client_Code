/*
 *****************************************************************************************
 *		@file uC_Crypt_AES_ECB.h
 *
 *		@Author Neena Sharma

 *		@brief Derived class for AES Encryption
 *
 *		@details Defines the interface for performing operations with encryption and Decryption
 *
 *		@copyright 2021 Eaton Corporation. All Rights Reserved.

 *****************************************************************************************
 */
#ifndef uC_Crypt_AES_ECB_H
#define uC_Crypt_AES_ECB_H

#include "Crypt.h"
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_OS_Interrupt.h"
#include "stm32f4xx_hal_cryp.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "Legacy/stm32_hal_legacy.h"
#include "stm32f4xx_hal_rcc.h"
#include "includes.h"
#include "Defines.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *	Variables and Constants
 *****************************************************************************************
 */
enum datatype_t		// Datatype options
{
	BIT_8 = CRYP_DATATYPE_8B,
	BIT_16 = CRYP_DATATYPE_16B,
	BIT_32 = CRYP_DATATYPE_32B,
	BIT_1 = CRYP_DATATYPE_1B
};

/**
 **********************************************************************************************
 * @brief                   CRYPT Class Declaration. The class will handle all the functionalities
 *                          related to CRYPT communication.
 **********************************************************************************************
 */
class uC_Crypt_AES_ECB : public Crypt
{
	public:

		/**
		 * @brief               Constructor to create instance of uC_Crypt_AES_ECB class.
		 * @param[in] key       AES key.
		 * @param[in] key_size  size of the AES key of type aes_key_size_t
		 * @param[in] init_vect init vector.
		 * @param[in] timeout   Tineout parameter; default or user defined value for timeout if its taking longer than
		 * expected to perform encryption.
		 * @param[in] datatype  datatype default to 8-bit, its type of input data default to 8-bit but can be 1-bit,
		 * 16-bit and 32 bit as well.
		 */
		uC_Crypt_AES_ECB( uint32_t key_size, uint8_t* key, uint8_t* init_vect, uint32_t timeout,
						  datatype_t datatype = BIT_8 )
		{
			m_hcryp.CRYP_InitTypeDef.DataType = datatype;
			m_hcryp.CRYP_InitTypeDef.KeySize = key_size;
			m_hcryp.CRYP_InitTypeDef.pKey = key;
			m_hcryp.CRYP_InitTypeDef.pInitVect = init_vect;

			m_timeout = timeout;

			__HAL_RCC_CRYP_CLK_ENABLE();

			// initialize the structure CRYP_HandleTypeDef *hcryp
			HAL_CRYP_Init( &m_hcryp );


		}

		/**
		 * @brief Encrypts a uint8 string.
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying encrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @return Returns encryption status of type crypt_status_t.
		 */

		Crypt::crypt_status_t uC_Crypt_AES_ECB::Encrypt( uint8_t* input, uint32_t input_length,
														 uint8_t* output )
		{
			uint8_t* plain_data = input;
			uint16_t s = input_length;
			uint8_t* cypher_data = output;
			// timeout = Timeout;
			HAL_StatusTypeDef ret_val;

			ret_val = HAL_CRYP_AESECB_Encrypt( &m_hcryp, plain_data, s, cypher_data, m_timeout );
			// check ret val to match with crypt_status_t
			switch ( ret_val )
			{
				case HAL_OK:
					return ( SUCCESS );

				case HAL_ERROR:
					return ( DATA_ERROR );

				case HAL_BUSY:
					return ( BUSY_ERROR );

				case HAL_TIMEOUT:
					return ( TIMEOUT_ERROR );

				default:
					return ( OPERATION_NOT_SUPPORTED );
			}
		}

		/**
		 * @brief Encrypts a uint8 string.
		 * @param input - Source data to encrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the resulting encrypted string.
		 *                 Length of the output buffer should be equal or greater than input buffer
		 * @param output_length - in_out parameter. length of output buffer must be passed in
		 * output_length.Not doing anything with it for now. Refer to Crypt_Aes_ECB_matrix.h for using it.
		 * @return Returns encryption status of type crypt_status_t.
		 */
		Crypt::crypt_status_t uC_Crypt_AES_ECB::Encrypt( uint8_t* input, uint32_t input_length,
														 uint8_t* output, uint32_t* output_length );

		/**
		 * @brief Decrypts a uint8 string.
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the result keying decrypted string.
		 * @return Returns decryption status of type crypt_status_t.
		 */
		Crypt::crypt_status_t uC_Crypt_AES_ECB::Decrypt( uint8_t* input, uint32_t input_length,
														 uint8_t* output )
		{
			uint8_t* cypher_data = input;
			uint16_t s = input_length;
			uint8_t* plain_data = output;
			HAL_StatusTypeDef ret_val;

			ret_val = HAL_CRYP_AESECB_Decrypt( &m_hcryp, cypher_data, s, plain_data, m_timeout );
			switch ( ret_val )
			{
				case HAL_OK:
					return ( SUCCESS );

				case HAL_ERROR:
					return ( DATA_ERROR );

				case HAL_BUSY:
					return ( BUSY_ERROR );

				case HAL_TIMEOUT:
					return ( TIMEOUT_ERROR );

				default:
					return ( OPERATION_NOT_SUPPORTED );
			}
		}

		/**
		 * @brief Decrypt a uint8 string.
		 * @param input - Source data to decrypt.
		 * @param input_length - Length of input data in bytes.
		 * @param output - Place to put the resulting decrypted string.
		 * @param output_length - in_out parameter. length of output buffer must be passed in
		 * output_length.Not doing anything with it for now. Refer to Crypt_Aes_ECB_matrix.h for using it.
		 * @return Returns decryption status of type crypt_status_t.
		 */
		Crypt::crypt_status_t uC_Crypt_AES_ECB::Decrypt( uint8_t* input, uint32_t input_length,
														 uint8_t* output, uint32_t* output_length );

	private:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Crypt_AES_ECB( void )
		{
			HAL_CRYP_DeInit( &m_hcryp );
		}

		uint32_t m_timeout;
		CRYP_HandleTypeDef m_hcryp;

};
#endif