/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the RSA Key class and APIs for the Key generation using RSA.
 *
 *	@details Key_RSA_Matrix contains the necessary methods for key generation using
 *	the RSA algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef KEY_RSA_MATRIX_H
#define KEY_RSA_MATRIX_H

#include "Key_Lib.h"

class Key_RSA_Matrix : public Key_Lib
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Key_RSA_Matrix class.
		 * @param[in] key_size.
		 * @return None
		 */
		Key_RSA_Matrix( PKI_Common::key_size_rsa_t key_size );

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Key_RSA_Matrix class.
		 * @param[in] key_mem.
		 * @param[in] key_mem_len.
		 * @param[in] password.
		 * @return None
		 */
		Key_RSA_Matrix( uint8_t* key_mem, uint16_t key_mem_len, char_t* password = nullptr );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Key_RSA_Matrix goes out of scope or deleted.
		 *  @return None
		 */
		~Key_RSA_Matrix( void );

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] pointer to the private key.
		 * @param[out] pointer to the public key location.
		 * @param[out] cert_conf: pointer to the key length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key( void** key, uint8_t** key_mem,
												  uint16_t* key_mem_len );

	private:

		uint16_t m_key_size = 0U;
		psPubKey_t* m_key = nullptr;
		uint8_t* m_key_mem = nullptr;
		uint16_t m_key_mem_len = 0U;
		bool m_construct_failed = false;
		bool m_key_mem_rec = false;

		static const uint32_t E_VALUE = 65537U;

		/**
		 * @brief Generate key.
		 * @return status of requested operation.
		 */
		PKI_Common::pki_status_t Generate( void );

};

#endif	// KEY_RSA_MATRIX_H
