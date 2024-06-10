/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides APIs for the firmware verification.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CODE_SIGN_RSA_MATRIX_H
#define CODE_SIGN_RSA_MATRIX_H

#include "Includes.h"
#include "Sig_Verify_RSA_Matrix.h"

class Code_Sign_RSA_Matrix
{
	public:

		/**
		 * @brief This does firmware verification when firmware is located in internal flash memory
		 * @details This function first verifies Firmware verification key (FVK) then Firmware
		 * package
		 * Following is the sequence of FVK and Firmware components.
		 *	-------------------------------------------------------------------------------------->
		 *      Firmware |   fvk     |  fvk pub   | Reserved  |  fvk public   |  fvk signature  |
		 *               |	ver	     | key length |  word	  |	  key		  |    size		    |
		 *				 |  4 bytes  | 2 Bytes    | 2 Bytes	  |               |    2 Bytes      |
		 *	------------------------------------------------------------------------------------->
		 *	>--------------------------------------------------
		 *	   fvk      |  firmware         |  firmware		|
		 *	  signature	| signature size	|  signature	|
		 *              |  2 bytes          |               |
		 *	>--------------------------------------------------
		 *
		 * @param[in] fw_location : Pointer to start of the firmware.
		 * @param[in] fw_size : size of the firmware.
		 * @param[in] rvk_pub_key_loc : RVK public key location in memory
		 * @param[in] rvk_pub_key_len : Length of the RVK public key
		 * @param[in] code_sign_user_callback : User callback function pointer
		 * @return Status of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Signature_RSA( uint8_t const* fw_location,
															  uint32_t const fw_size,
															  const uint8_t* rvk_pub_key_loc,
															  const uint32_t rvk_pub_key_len,
															  callback_t callback );

		/**
		 * @brief This does firmware verification when firmware is located in external memory.
		 * @details This function first verifies Firmware verification key (FVK) then Firmware
		 * package
		 * Following is the sequence of FVK and Firmware components.
		 *	-------------------------------------------------------------------------------------->
		 *      Firmware |   fvk     |  fvk pub   | Reserved  |  fvk public   |  fvk signature  |
		 *               |	ver	     | key length |  word	  |	  key		  |    size		    |
		 *				 |  4 bytes  | 2 Bytes    | 2 Bytes	  |               |    2 Bytes      |
		 *	------------------------------------------------------------------------------------->
		 *	>--------------------------------------------------
		 *	   fvk      |  firmware         |  firmware		|
		 *	  signature	| signature size	|  signature	|
		 *              |  2 bytes          |               |
		 *	>--------------------------------------------------
		 *
		 * @param[in] nv_handle : Pointer to signature location.
		 * @param[in] data_nv_config :
		 * data_nv_config->start_address is a pointer to start of the firmware.
		 * data_nv_config->length is a size of the firmware.
		 * @param[in] rvk_pub_key_loc : RVK public key location in memory
		 * @param[in] rvk_pub_key_len : Length of the RVK public key
		 * @param[in] code_sign_user_callback : User callback function pointer
		 * @return Status of requested operation.
		 */
		static PKI_Common::pki_status_t Verify_Signature_RSA_NV( NV_Ctrl* nv_handle,
																 PKI_Common::nv_chip_config_t* data_nv_config,
																 const uint8_t* rvk_pub_key_loc,
																 const uint32_t rvk_pub_key_len,
																 callback_t callback );

};

#endif	// CODE_SIGN_RSA_MATRIX_H
