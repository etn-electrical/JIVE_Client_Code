/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the ECC Key class and APIs for the Key generation using ECC.
 *
 *	@details Key_ECC_Matrix contains the necessary methods for key generation using
 *	the Elliptic Curve Cryptography.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef KEY_ECC_MATRIX_H
#define KEY_ECC_MATRIX_H

#include "Key_Lib.h"

class Key_ECC_Matrix : public Key_Lib
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Key_ECC_Matrix class.
		 * @param[in] key_size.
		 * @param[in] curve_type: Elliptic Curve type for private key.
		 * @return None
		 */
		Key_ECC_Matrix( PKI_Common::key_size_ecc_t key_size,
						PKI_Common::key_curve_type_t curve_type );

		Key_ECC_Matrix( uint8_t* key_mem, uint16_t key_mem_len, char_t* password = nullptr );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Key_ECC_Matrix goes out of scope or deleted.
		 *  @return None
		 */
		~Key_ECC_Matrix( void );

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] pointer to the private key.
		 * @param[out] pointer to the public key location.
		 * @param[out] cert_conf: pointer to the key length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key( void** key, uint8_t** key_mem,
												  uint16_t* key_mem_len );

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] pointer to the private key.
		 * @param[out] pointer to the public key location.
		 * @param[out] cert_conf: pointer to the key length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Set_Key( uint8_t* key_mem, uint16_t key_mem_len );

	private:

		uint16_t m_key_size = 0U;
		psPubKey_t* volatile m_key = nullptr;
		uint8_t* m_key_mem = nullptr;
		uint16_t m_key_mem_len = 0U;
		uint8_t m_curve_type;
		volatile bool m_key_mem_rec = false;

		/**
		 * @brief Generate key.
		 * @return status of requested operation.
		 */
		PKI_Common::pki_status_t Generate( void );

};

#endif	// KEY_ECC_MATRIX_H
