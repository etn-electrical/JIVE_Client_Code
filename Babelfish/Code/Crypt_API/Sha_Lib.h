/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the Device Certificate operations.
 *
 *	@details Sha_Lib contains the necessary structure and methods for device certificate
 *	configuration.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef SHA_LIB_H_
#define SHA_LIB_H_

#include "Includes.h"
#include "PKI_Common.h"

class Sha_Lib
{
	public:

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object Sha_Lib goes out of scope or deleted.
		 *  @return None.
		 */
		virtual ~Sha_Lib()
		{}

		/**
		 * @brief Sha256 initialization.
		 * @details Initialize psSha256_t structure.
		 * @param None.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Init( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update the data.
		 * @details Update the data using sha256 algorithm for calculating hash.
		 * @param[in] data : Pointer to data (FVK public key or FW pack).
		 * @param[in] data_len : Data length.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Update( uint8_t const* data, uint32_t data_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Calculate the final hash.
		 * @details Calculate the final hash using sha256 algorithm.
		 * @param[out] Hash_out : Calculated hash.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Final( uint8_t* Hash_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get sha256 hash size.
		 * @return hash size.
		 */
		virtual uint32_t Size( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Sha_Lib()
		{}

};

#endif	// Sha_Lib_H_
