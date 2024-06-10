/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the Key generation operations.
 *
 *	@details Pub_Key_Lib contains the necessary methods for device Key generation.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef PUB_KEY_LIB_H_
#define PUB_KEY_LIB_H_

#include "Includes.h"
#include "PKI_Common.h"

class Pub_Key_Lib
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Pub_Key_Lib goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Pub_Key_Lib()
		{}

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] key : Pointer to the public key structure.
		 * @param[out] key_mem : Pointer to (in memory) the public key location.
		 * @param[out] key_mem_len : Pointer to the key length.
		 * @return Status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key( void** key, uint8_t** key_mem,
												  uint16_t* key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Decrypt hash from signature and compare with final calculated hash of the data.
		 * @details Decrypt hash from signature of FVK public key or FW pack using sha256 algorithm.
		 * @param[in] sig : Pointer to signature location.
		 * @param[in] sig_len : Pointer to signature length.
		 * @param[out] calculated_hash : Pointer to final calculated hash of the data.
		 * @param[in] hash_size : Sha256 hash size.
		 * @return Status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Verify_Hash( uint8_t* sig, uint16_t sig_len,
													  uint8_t* calculated_hash,
													  uint32_t hash_size )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Pub_Key_Lib()
		{}

};

#endif	// PUB_KEY_LIB_H_
