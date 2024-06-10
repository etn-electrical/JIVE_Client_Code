/**
 **************************************************************************************************
 *  @file
 *
 *	@details Fw_Store_NV.h provides required information from firmware and code
 * sign information block stored in External memory
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef FW_STORE_NV_H
#define FW_STORE_NV_H

#include "Fw_Store.h"

class Fw_Store_NV : public Fw_Store
{
	public:
		/**
		 * @brief Constructor.
		 * @param[in]  mem_config->start_address : Pointer to start address of the firmware store
		 * block (sector/page) in external memory
		 * mem_config->length : size of the firmware
		 * @param[in] fw_nv_ctrl : NV control to access firmware and code signing information stored
		 * in external memory
		 * @param[out] integrity_status : Integrity status of code sign information block
		 * @param[in] cb : User callback function pointer
		 * @return None.
		 */
		Fw_Store_NV( const PKI_Common::nv_chip_config_t* mem_config, NV_Ctrl* fw_nv_ctrl,
					 callback_t cb, bool* integrity_status );

		/**
		 * @brief Get pointer to FVK public key certificate from code sign info block
		 * @param[out] fvk_cert_out: Pointer to FVK key certificate.
		 * @param[out] fvk_cert_len_out: length of the FVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Cert( uint8_t** fvk_cert_out,
											   uint16_t* fvk_cert_len_out );

		/**
		 * @brief Get pointer to FVK public key from code sign info block
		 * @param[out] fvk_key_out: Pointer to FVK key.
		 * @param[out] fvk_key_len_out: length of the FVK key.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Key( uint8_t** fvk_key_out,
											  uint16_t* fvk_key_len_out );

		/**
		 * @brief Get pointer to IVK public key certificate from code sign info block
		 * @param[out] ivk_cert_out: Pointer to IVK key certificate.
		 * @param[out] ivk_cert_len_out: Length of the IVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_IVK_Cert( uint8_t** ivk_cert_out,
											   uint16_t* ivk_cert_len_out );

		/**
		 * @brief Get pointer to firmware signature from code sign info block
		 * @param[out] fw_sign_out: Pointer to firmware signature.
		 * @param[out] fw_sign_len_out: Length of firmware signature.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Fw_Sign( uint8_t** fw_sign_out,
											  uint16_t* fw_sign_len_out );

		/**
		 * @brief Get pointer to RVK public key certificate from cross sign info block
		 * @param[out] rvk_cert_out: Pointer to RVK key certificate.
		 * @param[out] rvk_cert_len_out: Length of the RVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_RVK_Key( uint8_t** rvk_key_out,
											  uint16_t* rvk_key_len_out );

		/**
		 * @brief Calculate hash of the firmware and return the same
		 * @param[out] calculated_hash: Hash of the firmware.
		 * @param[out] hash_size_out: sha256 hash size.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Hash( uint8_t** calculated_hash, uint32_t* hash_size_out );

		/**
		 * @brief Calculate hash of the firmware (including components in cross sign info block) and
		 * return the same
		 * @param[out] calculated_hash: Hash of the firmware.
		 * @param[out] hash_size_out: sha256 hash size.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Hash_Trusted( uint8_t** calculated_hash,
												   uint32_t* hash_size_out );

		/**
		 * @brief Get pointer to trusted FVK certificate from cross sign info block
		 * @param[out] fvk_cert_out: Pointer to trusted FVK certificate.
		 * @param[out] fvk_cert_len_out: Length of the trusted FVK certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Trusted_FVK_Cert( uint8_t** fvk_cert_out,
													   uint16_t* fvk_cert_len_out );

		/**
		 * @brief Get pointer to Trusted firmware signature from cross sign info block
		 * @param[out] fw_sign_out: Pointer to Trusted firmware signature.
		 * @param[out] fw_sign_len_out: Length of the Trusted firmware signature.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Trusted_Fw_Sign( uint8_t** fw_sign_out,
													  uint16_t* fw_sign_len_out );

		/**
		 * @brief deallocate the memory allocated while reading trusted FVK cert from external
		 * memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_Trusted_FVK_Cert( uint8_t* cert );

		/**
		 * @brief deallocate the memory allocated while reading trusted Fw signature from external
		 * memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_Trusted_Fw_Sign( uint8_t* sign );

		/**
		 * @brief Verify the CRC of extended code sign information block(cross signing)
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Validate_Cross_Sign_CRC( void );

		/**
		 * @brief deallocate the memory allocated while calculating hash of the firmware
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_Hash( uint8_t* calculated_hash );

		/**
		 * @brief deallocate the memory allocated while reading FVK cert from external memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_FVK_Cert( void );

		/**
		 * @brief deallocate the memory allocated while reading FVK key from external memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_FVK_Key( void );

		/**
		 * @brief deallocate the memory allocated while reading IVK cert from external memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_IVK_Cert( void );

		/**
		 * @brief deallocate the memory allocated while reading FW signature from external memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_Fw_Sign( void );

		/**
		 * @brief deallocate the memory allocated while reading RVK key from external memory
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_RVK_Key( void );

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Fw_Store_Control goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		~Fw_Store_NV( void );

	private:

		/**
		 * @brief Validate the code sign information block
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Code_Sign_Info_Integrity_Check( void );

		/**
		 * @brief Get trusted fvk cert length
		 * @param[out] fvk_cert_len_out: Trusted fvk cert length.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Trusted_FVK_Cert_Len( uint16_t* fvk_cert_len_out );

		/**
		 * @brief Get trusted fvk cert length and trusted fw signature length
		 * @param[out] fvk_cert_len_out: Trusted fvk cert length.
		 * @param[out] fw_sign_len_out: Trusted fw signature length.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Trusted_Lenghts( uint16_t* fvk_cert_len_out,
													  uint16_t* fw_sign_len_out );

		/**
		 * @brief Verify the CRC of code sign information block
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Validate_Code_Sign_Info_CRC( void );

		/**
		 * @brief Get pointer to start of the firmware location from fw store
		 * @param[out] fw_location_out: Pointer to fw location
		 * @param[out] fw_package_len_out: length of the fw
		 * @return status of requested operation
		 */
		inline PKI_Common::pki_status_t Get_Fw_Location( uint8_t** fw_location_out,
														 uint32_t* fw_package_len_out );

		/**
		 * @brief Get pointer to start of the firmware location from fw store
		 * @param[out] fw_location_out: Pointer to fw location
		 * @param[out] fw_package_len_out: length of the fw till trusted fw signature
		 * @return status of requested operation
		 */
		inline PKI_Common::pki_status_t Get_Fw_Location_Trusted( uint8_t** fw_location_out,
																 uint32_t* fw_package_len_out );

		/**
		 * @brief Get the cross signing status, it will be set when RVK key in code sign info block
		 * is new
		 * @return status of requested operation
		 */
		bool Get_Cross_Sign_Status( void );

		/**
		 * @brief Pointer to start of firmware storage area
		 */
		real_fw_header* m_fw_config_addr;

		/**
		 * @brief Maximum length of the firmware storage area
		 */
		uint32_t m_fw_config_len;

		/**
		 * @brief pointer to code signing information
		 */
		code_sign_struct_t* m_code_sign_ptr;

		/**
		 * @brief NV control to access firmware and code signing information stored in external
		 * memory
		 */
		NV_Ctrl* m_fw_nv_ctrl;

		/**
		 * @brief Pointer to a function to be called after every update of
		 * max_num_bytes.
		 */
		callback_t m_callback_func;

		/**
		 * @brief variables to store length of certificates, keys and signature read from external
		 * memory
		 */
		uint16_t m_fvk_cert_len;
		uint16_t m_fvk_key_len;
		uint16_t m_ivk_cert_len;
		uint16_t m_rvk_key_len;
		uint16_t m_fw_sign_len;

		/**
		 * @brief Pointer to ram buffer for certificates, keys and signature
		 */
		uint8_t* volatile m_fvk_cert;
		uint8_t* volatile m_fvk_key;
		uint8_t* volatile m_ivk_cert;
		uint8_t* volatile m_rvk_key;
		uint8_t* volatile m_fw_sign;
};


#endif	// FW_STORE_NV_H
