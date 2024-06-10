/**
 *****************************************************************************************
 *	@file Cert_Signed.h
 *
 *	@brief Base class for signing certificate
 *
 *	@details Defines the interface for performing operations with certificate signing
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CERT_SIGNED_H
#define CERT_SIGNED_H

#include "Includes.h"
#include "PKI_Common.h"

class Cert_Signed
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Cert_Signed goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Cert_Signed()
		{}

		/**
		 * @brief Signing of certificate - calling Key and Certificate generation APIs.
		 * @param[out] key_mem: private key location.
		 * @param[out] key_mem_len: key length.
		 * @param[out] cert_mem: certificate location.
		 * @param[out] cert_mem_len: certificate length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Sign( uint8_t** key_mem, uint16_t* key_mem_len,
											   uint8_t** cert_mem, uint32_t* cert_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:

		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Cert_Signed()
		{}

};

#endif	// CERT_SIGNED_H
