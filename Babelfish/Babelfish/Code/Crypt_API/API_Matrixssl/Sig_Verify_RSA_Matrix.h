/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the RSA class and APIs for the Device Certificate using RSA.
 *
 *	@details Sig_Verify_RSA_Matrix contains the necessary methods for device certificate generation
 * using
 *	the RSA algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef SIG_VERIFY_RSA_MATRIX_H
#define SIG_VERIFY_RSA_MATRIX_H

#include "Pub_Key_Lib.h"
#include "Sha_Lib.h"
#include "Sig_Verify.h"

/* A function pointer for callback function */
typedef void ( * callback_t )( void );

class Sig_Verify_RSA_Matrix : public Sig_Verify
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Sig_Verify_RSA_Matrix
		 * class.
		 * @param[in] hash_ctx: Pointer to Sha_Lib struct.
		 * @param[in] rsa_pub_key: Pointer to RSA public key.
		 * @param[in] max_num_bytes: Maximum number of bytes to be updated at a time during
		 * signature
		 * verification. The dynamic buffer of size max_num_bytes is allocated to do the read
		 * operation when firmware
		 * is located in external memory
		 * @param[in] callback_func_t: Pointer to a function to be called after every update of
		 * max_num_bytes.
		 * @return None
		 */
		Sig_Verify_RSA_Matrix( Sha_Lib* hash_ctx, Pub_Key_Lib* rsa_pub_key, uint32_t max_num_bytes,
							   callback_t cb );

		/**
		 * @brief This verifies the FVK public key and FW pack using RSA public key.
		 * @param[in] data: Location of FVK public key or FW pack.
		 * @param[in] data_len: Length of FVK public key or FW pack.
		 * @param[in] sig: Location of the signature for FVK or FW pack.
		 * @param[in] sig_len: Length of the signature for FVK or FW pack.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify( uint8_t const* data, uint32_t data_len,
												 uint8_t* sig,
												 uint16_t sig_len );

		/**
		 * @brief
		 * @details This verifies the FVK public key and FW pack using RSA public key when firmware
		 * is located
		 * in external memory.
		 * @param[in] nv_handle: Pointer to NV_Ctrl.
		 * @param[in] data_nv_config: Address and length of the FVK public key or FW pack.
		 * @param[in] sig_nv_config: Address and length of the signature for FVK or FW pack.
		 * @return Success or failure status.
		 */
		virtual PKI_Common::pki_status_t Verify( NV_Ctrl* nv_handle,
												 const PKI_Common::nv_chip_config_t* data_nv_config,
												 const PKI_Common::nv_chip_config_t* sig_nv_config );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Sig_Verify_RSA_Matrix goes out of scope or
		 * deleted.
		 *  @return None
		 */
		~Sig_Verify_RSA_Matrix( void );

	private:
		Sha_Lib* m_hash;
		Pub_Key_Lib* m_rsa_pub_key;
		uint32_t m_max_ram_buff_size;
		callback_t m_callback_func;

};

#endif	// SIG_VERIFY_RSA_MATRIX_H
