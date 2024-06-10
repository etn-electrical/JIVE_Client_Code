/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides APIs for the firmware verification.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CODE_SIGN_ECC_MATRIX_H
#define CODE_SIGN_ECC_MATRIX_H

#include "Includes.h"
#include "Sig_Verify_ECC_Matrix.h"
#include "PKI_Config.h"
#include "Cert_Store.h"
#include "Fw_Store.h"

class Code_Sign_ECC_Matrix
{
	public:

		/**
		 * @brief constructor
		 */
		Code_Sign_ECC_Matrix( void )
		{}

		/**
		 * @brief This does firmware signature verification
		 * @details This function checks the status of keys( FVK, IVK, RVK ) whether new key is
		 * available or not, based on the status it verifies key signature then firmware signature.
		 * @param[in] cert_store_if : certificate storage area control, required to access fvk, IVK
		 * and RVK from certificate store.
		 * @param[in] fw_ctrl : firmware storage area control, required to access firmware package
		 * which is to be verified
		 * @param[in] callback : User callback function pointer
		 * @return Success or failure of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Signature_ECC( Cert_Store* cert_store_if,
															  Fw_Store* fw_ctrl,
															  bool* key_upgrade_status );

		/**
		 * @brief constructor
		 */
		~Code_Sign_ECC_Matrix( void )
		{}

	private:
		/**
		 * @brief Compare certificate in code_sign_info and certificate in cert store
		 * @details it can compare FVK cert or IVK cert or RVK cert
		 * @param[in] fw_ctrl : control to firmware storage area, required to get certificate from
		 * code sign info
		            (footer of the FW code where fvk and ivk are stored ).
		 * @param[in] key_cert_enum : enum to identify which certificate should be read from cert
		 * store (fvk, ivk, or rvk)
		 * @param[in] cert_store_if : certificate storage area control
		 * @return Success or failure of comparison.
		 */
		static PKI_Common::pki_status_t Compare_Cert( Fw_Store* fw_ctrl,
													  Cert_Store::cert_store_enum_t key_cert_enum,
													  Cert_Store* cert_store_if );

		/**
		 * @brief verify signature of firmware package using FVK
		 * @param[in] fw_ctrl : control to firmware storage area, required to access fw signature
		 * and data
		 * @param[in] cert_handle : Cert_Matrix pointer, required to access fvk key
		 * @return Success or failure of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Fw_Sign( Fw_Store* fw_ctrl,
														Cert_Matrix* cert_handle );

		/**
		 * @brief verify trusted Fw signature using Trusted FVK cert in case of cross signing
		 * @param[in] fw_ctrl : control to firmware storage area, required to access trusted fw
		 * signature and data
		 * @param[in] cert_handle : Cert_Matrix pointer, required to access trusted fvk key
		 * @return Success or failure of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Trusted_Fw_Sign( Fw_Store* fw_ctrl,
																Cert_Matrix* cert_handle );

		/**
		 * @brief verify cross signing i.e. extended code sign block
		 * @param[in] cert_store_if : certificate storage area control, required to access IVK
		 * from certificate store.
		 * @param[in] fw_ctrl : firmware storage area control, required to access extended code sign
		 * block and firmware package which is to be verified
		 * @return Success or failure of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Cross_Sign( Cert_Store* cert_store_if,
														   Fw_Store* fw_ctrl );

};

#endif	// CODE_SIGN_ECC_MATRIX_H
