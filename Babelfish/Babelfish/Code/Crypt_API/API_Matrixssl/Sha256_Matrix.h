/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the RSA class and APIs for the Device Certificate using RSA.
 *
 *	@details Sha256_Matrix contains the necessary methods for device certificate generation using
 *	the RSA algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef SHA256_MATRIX_H
#define SHA256_MATRIX_H

#include "Sha_Lib.h"

/**
 **************************************************************************************************
 * @brief This class is responsible for handling Sha256 library functions.
 **************************************************************************************************
 */
class Sha256_Matrix : public Sha_Lib
{
	public:

		/**
		 * @brief Constructor.
		 * @details This does basic initialization of the data members for Sha256_Matrix class.
		 * @param[out] ret_status : Success or failure status.
		 * @return None.
		 */
		Sha256_Matrix( void );

		/**
		 * @brief Sha256 initialization.
		 * @details Initialize psSha256_t structure.
		 * @param None.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Init( void );

		/**
		 * @brief Update the data.
		 * @details Update the data using sha256 algorithm for calculating hash.
		 * @param[in] data : Pointer to data (FVK public key or FW pack).
		 * @param[in] data_len : Data length.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Update( uint8_t const* data, uint32_t data_len );

		/**
		 * @brief Calculate the final hash.
		 * @details Calculate the final hash using sha256 algorithm.
		 * @param[out] Hash_out : Calculated hash.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Final( uint8_t* Hash_out );

		/**
		 * @brief Get sha256 hash size.
		 * @return hash size.
		 */
		virtual uint32_t Size( void );

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Sha256_Matrix goes out of scope or deleted.
		 *  @return None.
		 */
		~Sha256_Matrix( void );

	private:
		psSha256_t* volatile m_hash_ctx;
};

#endif	// SHA256_MATRIX_H
