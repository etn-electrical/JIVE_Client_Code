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
#ifndef SIG_VERIFY_H_
#define SIG_VERIFY_H_

#include "Includes.h"
#include "PKI_Common.h"

class Sig_Verify
{
	public:

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Sig_Verify goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Sig_Verify()
		{}

		/**
		 * @brief This verifies the FW pack (FW signature) using FVK in code sign info block.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief This verifies the FVK public key or FW pack located in internal flash.
		 * @param[in] data: Start location of the data (FVK public key or FW pack).
		 * @param[in] data_len: Length of the data.
		 * @param[in] sig: Location of the signature of data ( FVK or FW pack ).
		 * @param[in] sig_len: Length of the signature.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify( uint8_t const* data, uint32_t data_len,
												 uint8_t* sig,
												 uint16_t sig_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief This verifies the FVK public key or FW pack located in external memory.
		 * @param[in] nv_handle: Pointer to NV_Ctrl.
		 * @param[in] data_nv_config: Address and length of the data ( FVK or FW pack ).
		 * @param[in] sig_nv_config: Address and length of the signature of data.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify( NV_Ctrl* nv_handle,
												 const PKI_Common::nv_chip_config_t* data_nv_config,
												 const PKI_Common::nv_chip_config_t* sig_nv_config )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief This verifies the FW pack (trusted FW signature) using trusted FVK in cross sign
		 * info block.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify_Trusted_Fw_Sign( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Sig_Verify()
		{}

};

#endif	// SIG_VERIFY_H_
