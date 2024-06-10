/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CERT_SIGNED_SS_MATRIX_H
#define CERT_SIGNED_SS_MATRIX_H

#include "Cert_Conf_Lib.h"
#include "Cert_Signed.h"
#include "PKI_Common.h"
#include "Key_Lib.h"

class Cert_Signed_SS_Matrix : public Cert_Signed
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Cert_Signed_SS_Matrix
		 * class.
		 * @param[in] key: Key class handle.
		 * @param[in] cert: Certificate class handle.
		 * @return None
		 */
		Cert_Signed_SS_Matrix( Key_Lib* key, Cert_Conf_Lib* cert );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Cert_Signed_SS_Matrix goes out of scope or
		 * deleted.
		 *  @return None
		 */
		~Cert_Signed_SS_Matrix( void );

		/**
		 * @brief Signing of certificate - calling Key and Certificate generation APIs.
		 * @param[out] key_mem: private key location.
		 * @param[out] key_mem_len: key length.
		 * @param[out] cert_mem: certificate location.
		 * @param[out] cert_mem_len: certificate length.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Sign( uint8_t** key_mem, uint16_t* key_mem_len,
											   uint8_t** cert_mem, uint32_t* cert_mem_len );

	private:

		Key_Lib* m_key_handle;
		Cert_Conf_Lib* volatile m_cert_handle;
};

#endif	// CERT_SIGNED_SS_MATRIX_H
