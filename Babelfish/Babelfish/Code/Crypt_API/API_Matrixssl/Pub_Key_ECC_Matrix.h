/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the ECC Key class and APIs for the Key generation using ECC.
 *
 *	@details Pub_Key_ECC_Matrix contains the necessary methods for key generation using
 *	 ECC algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef PUB_KEY_ECC_MATRIX_H
#define PUB_KEY_ECC_MATRIX_H

#include "Pub_Key_Lib.h"

class Pub_Key_ECC_Matrix : public Pub_Key_Lib
{
	public:

		/**
		 * @brief Constructor.
		 * @details This does basic initialization of the data members for Pub_Key_ECC_Matrix class.
		 * @param[in] key_mem : Pointer to public key location.
		 * @param[in] key_mem_len : Key length.
		 * @param[in] password : Not used.
		 * @param[out] ret_status : Status of requested operation.
		 * @return None.
		 */
		Pub_Key_ECC_Matrix( uint8_t const* key_mem, uint16_t key_mem_len,
							PKI_Common::pki_status_t* ret_status,
							char_t* password = nullptr );

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] key : Pointer to the public key structure.
		 * @param[out] key_mem : Pointer to (in memory) the public key location.
		 * @param[out] key_mem_len : Pointer to the key length.
		 * @return Status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key( void** key, uint8_t** key_mem,
												  uint16_t* key_mem_len );

		/**
		 * @brief Verify signature.
		 * @details Verify signature of FVK public key or FW pack using ECC algorithm.
		 * @param[in] sig : Pointer to signature location.
		 * @param[in] sig_len : Pointer to signature length.
		 * @param[in] calculated_hash : Pointer to final calculated hash.
		 * @param[in] hash_size : Sha256 hash size.
		 * @return Status of signature verification.
		 */
		virtual PKI_Common::pki_status_t Verify_Hash( uint8_t* sig, uint16_t sig_len,
													  uint8_t* calculated_hash,
													  uint32_t hash_size );

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Pub_Key_ECC_Matrix goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		~Pub_Key_ECC_Matrix( void );

	private:

		psPubKey_t* volatile m_key;
		uint8_t const* m_key_mem;
		uint16_t m_key_mem_len;
};

#endif	// PUB_KEY_ECC_MATRIX_H
