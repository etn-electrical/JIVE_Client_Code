/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the ECC class and APIs for the Device Certificate using ECC.
 *
 *	@details Sig_Verify_ECC_Matrix contains the necessary methods for device certificate generation
 * using
 *	the ECC algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef SIG_VERIFY_ECC_MATRIX_H
#define SIG_VERIFY_ECC_MATRIX_H

#include "Pub_Key_Lib.h"
#include "Sha_Lib.h"
#include "Sig_Verify.h"
#include "Cert_Matrix.h"
#include "Fw_Store.h"

class Sig_Verify_ECC_Matrix : public Sig_Verify
{
	public:

		/**
		 * @brief Constructor to be called when public key is available in code_sign_info
		 * @details This does basic initialization of the data members for Sig_Verify_ECC_Matrix
		 * class.
		 * @param[in] hash_ctx: Pointer to Sha_Lib struct.
		 * @param[in] ecc_pub_key: Pointer to ECC public key.
		 * @return None
		 */
		Sig_Verify_ECC_Matrix( Fw_Store* fw_ctrl, Pub_Key_Lib* ecc_pub_key );

		/**
		 * @brief Constructor to be called when public key certificate is available in
		 * code_sign_info
		 * @details This does basic initialization of the data members for Sig_Verify_ECC_Matrix
		 * class.
		 * @param[in] hash_ctx: Pointer to Sha_Lib struct.
		 * @param[in] cert_handle: Pointer to pub_key_cert.
		 * @param[in] max_num_bytes: Maximum number of bytes to be updated at a time during
		 * signature verification. The dynamic buffer of size max_num_bytes is allocated to do the
		 * read operation when firmware is located in external memory
		 * @return None
		 */
		Sig_Verify_ECC_Matrix( Fw_Store* fw_ctrl, Cert_Matrix* cert_handle );

		/**
		 * @brief This verifies the FW pack (FW signature) using FVK in code sign info block.
		 * @return Success or failure status.
		 */
		PKI_Common::pki_status_t Verify( void );

		/**
		 * @brief This verifies the FW pack (trusted FW signature) using trusted FVK in cross sign
		 * info block.
		 * @return Success or failure status.
		 */
		PKI_Common::pki_status_t Verify_Trusted_Fw_Sign( void );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Sig_Verify_ECC_Matrix goes out of scope or
		 * deleted.
		 *  @return None
		 */
		~Sig_Verify_ECC_Matrix( void );

	private:
		Pub_Key_Lib* m_ecc_pub_key;
		Fw_Store* m_fw_ctrl;
		Cert_Matrix* m_cert_handle;	// pointer to key certificate
		bool m_is_cert;	// flag to indicate cert or key, cert:true, key:false

};

#endif	// SIG_VERIFY_ECC_MATRIX_H
