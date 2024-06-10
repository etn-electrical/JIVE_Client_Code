/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the Key generation operations.
 *
 *	@details Key_Lib contains the necessary methods for device Key generation.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef KEY_LIB_H
#define KEY_LIB_H

#include "Includes.h"
#include "PKI_Common.h"

class Key_Lib
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Key_Lib goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Key_Lib()
		{}

		/**
		 * @brief Call the Generate function for the private key generation and return pointer.
		 * @param[out] key: pointer to the private key.
		 * @param[out] key_mem: pointer to the public key location.
		 * @param[out] key_mem_len: pointer to the key length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key( void** key, uint8_t** key_mem, uint16_t* key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Store the key value at the desired location.
		 * @param[in] key_mem: pointer to the private key.
		 * @param[in] key_mem_len: length of the key.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Set_Key( uint8_t* key_mem, uint16_t key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Key_Lib()
		{}

};

#endif	// KEY_LIB_H
