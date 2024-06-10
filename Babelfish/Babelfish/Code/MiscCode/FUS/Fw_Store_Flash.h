/**
 **************************************************************************************************
 *  @file
 *
 *	@details Fw_Store_Flash.h provides required information from firmware and code
 * sign information block stored in Flash
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef FW_STORE_FLASH_H
#define FW_STORE_FLASH_H

#include "Fw_Store.h"

class Fw_Store_Flash : public Fw_Store
{
	public:
		/**
		 * @brief Constructor.
		 * @param[in]  mem_config->start_address : Pointer to start address of the firmware store
		 * block (sector/page) in internal memory
		 * mem_config->length : size of the firmware
		 * @param[in] cb : User callback function pointer
		 * @param[out] integrity_status : Integrity status of code sign information block
		 * @return None.
		 */
		Fw_Store_Flash( const PKI_Common::nv_chip_config_t* mem_config, callback_t cb,
						bool* integrity_status );

		/**
		 * @brief Get pointer to FVK public key from code sign info block
		 * @param[out] fvk_key_out: Pointer to FVK key.
		 * @param[out] fvk_key_len_out: length of the FVK key.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Key( uint8_t** fvk_key_out,
											  uint16_t* fvk_key_len_out );

		/**
		 * @brief Get pointer to FVK public key certificate from code sign info block
		 * @param[out] fvk_cert_out: Pointer to FVK key certificate.
		 * @param[out] fvk_cert_len_out: length of the FVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Cert( uint8_t** fvk_cert_out,
											   uint16_t* fvk_cert_len_out );

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
		 * @brief Get pointer to RVK public key certificate from code sign info block
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
		 * @brief Get the cross signing status, it will be set when RVK key in code sign info block
		 * is new
		 * @return status of requested operation
		 */
		bool Get_Cross_Sign_Status( void );

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
		 *  @brief Destructor.
		 *  @details It will get called when object of Fw_Store_Flash goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		~Fw_Store_Flash( void );

	private:

		/**
		 * @brief Validate the code sign information block
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Code_Sign_Info_Integrity_Check( void );

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
		 * @brief m_fw_ctrl : Pointer to start of the firmware location
		 */
		real_fw_header* m_fw_ctrl;

		/**
		 * @brief m_mem_cfg_len : length of the total flash area available for fw_store
		 */
		uint32_t m_mem_cfg_len;
		/**
		 * @brief pointer to code signing information
		 */
		code_sign_struct_t* m_code_sign_ptr;

		/**
		 * @brief Pointer to a function to be called after every update of
		 * max_num_bytes.
		 */
		callback_t m_callback_func;
};


#endif	// FW_STORE_FLASH_H
