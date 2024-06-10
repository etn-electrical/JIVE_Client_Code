/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the Device Certificate operations.
 *
 *	@details Cert_Lib contains the necessary structure and methods for device certificate
 *	configuration.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_LIB_H
#define CERT_LIB_H

#include "Includes.h"
#include "PKI_Common.h"

class Cert_Lib
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Cert_Lib goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Cert_Lib()
		{}

		/**
		 * @brief Validates the device certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Validate( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Return the parsed certificate.
		 * @param[out] cert: pointer to the device certificate area.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Cert( void** cert )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Cert_Lib()
		{}

};

#endif	// CERT_LIB_H
