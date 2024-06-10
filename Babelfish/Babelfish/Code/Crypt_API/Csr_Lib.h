/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the certificate signing request.
 *
 *	@details Csr_Lib contains the necessary structure and methods for CSR generation and to store
 *	the handles of key and cert_conf class.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CSR_LIB_H
#define CSR_LIB_H

#include "Includes.h"
#include "PKI_Common.h"
#include "Key_Lib.h"
#include "Cert_Conf_Lib.h"

class Csr_Lib
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Csr_Lib goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Csr_Lib()
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
		 * @brief Set the values of the desired parameters.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Set( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Set the values of the desired parameters.
		 * @param[out] dn: distinguished name.
		 * @param[out] pub_key: public key.
		 * @param[out] req_ext: required extension.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get( void** dn, void** pub_key, void** req_ext )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Return the handle of Key_Lib class.
		 * @param[out] key: handle of key_lib class.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Key_Handle( Key_Lib** key )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Return the handle of Cert_Conf_Lib class.
		 * @param[out] cert_conf_handle: handle of Cert_Conf_Lib class.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Cert_Conf_Handle( Cert_Conf_Lib** cert_conf_handle )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Csr_Lib()
		{}

};

#endif	// CSR_LIB_H
