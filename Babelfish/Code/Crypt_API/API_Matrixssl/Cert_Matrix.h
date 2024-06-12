/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides APIs to parse x509 certificate and get its components.
 *
 *	@details Cert_Matrix contains the necessary methods for device certificate generation using
 *	the ECC algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_MATRIX_H
#define CERT_MATRIX_H

#include "Cert_Lib.h"

class Cert_Matrix : public Cert_Lib
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Cert_Matrix class and
		 * parse the certificate.
		 * @param[in] cert_mem: certificate memory location.
		 * @param[in] cert_mem_len: length of the certificate.
		 * @param[in] ret_status : certificate parsing success or failure status.
		 * @return None
		 */
		Cert_Matrix( uint8_t* cert_mem, uint16_t cert_mem_len,
					 PKI_Common::pki_status_t* ret_status );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Cert_Matrix goes out of scope or deleted.
		 *  @return None
		 */
		~Cert_Matrix( void );

		/**
		 * @brief Validates the device certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Authenticate( psX509Cert_t* cert );

		/**
		 * @brief get pointer to parsed certificate
		 * @param[out] cert: pointer to parsed certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Cert( void** cert );

		/**
		 * @brief get public key bin
		 * @param[out] pub_key : pointer to public key bin
		 * @param[out] pub_key_len : public key bin len
		 * @return Status of requested operation.
		 */
		PKI_Common::pki_status_t Get_Pub_Key( uint8_t** pub_key, uint16_t* pub_key_len );

		/**
		 * @brief get signature of the key
		 * @param[out] signature : pointer to signature of key
		 * @param[out] signature_len : signature length
		 * @return Status of requested operation.
		 */
		PKI_Common::pki_status_t Get_Signature( uint8_t** signature, uint16_t* signature_len );

		/**
		 * @brief get hash of the key
		 * @param[out] key_hash : key hash
		 * @param[out] key_hash_len : key hash length
		 * @return Status of requested operation.
		 */
		PKI_Common::pki_status_t Get_Cert_Hash( uint8_t** key_hash, uint16_t* key_hash_len );

		/**
		 * @brief Verify signature of key.
		 * @details This function is called when code_sign_info contains FVK/IVK/RVK in the form of
		 * cert
		 * @param[in] sig : Pointer to signature location.
		 * @param[in] sig_len : signature length.
		 * @param[in] hash : Pointer to the hash of data ( it could be hash of fw or key ).
		 * @param[in] hash_size : Sha256 hash size.
		 * @return Status of signature verification.
		 */
		PKI_Common::pki_status_t Verify_Hash( uint8_t* sig, uint16_t sig_len,
											  uint8_t* hash,
											  uint32_t hash_size );

	private:

		uint8_t* m_cert_mem;
		uint16_t m_cert_mem_len;
		psX509Cert_t* volatile m_cert;
};

#endif	// CERT_MATRIX_H
