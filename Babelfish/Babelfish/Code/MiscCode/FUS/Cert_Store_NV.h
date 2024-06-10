/**
 **************************************************************************************************
 *  @file
 *  @brief This file provides APIs to access certificate storage area located in external memory
 *
 *	@details This file has cert_store_struct_t structure and APIs to access this structure and
 * manage the certificate storage area located in external memory
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_STORE_NV_H
#define CERT_STORE_NV_H

#include "Cert_Store.h"

#include "Includes.h"
#include "PKI_Config.h"
#include "uC_Flash.h"
#include "NV_Ctrl.h"

class Cert_Store_NV : public Cert_Store
{
	public:
		/**
		 * @brief Constructor.
		 * @param[in] nv_ctrl : NV control to read/write certificate storage area located external
		 * flash
		 * @param[in] mem_config->start_address : Pointer to start address of the certificate store
		 * block (sector/page) in external memory
		 * mem_config->length : size of the sector/page where certificate store is
		 * located
		 * @return None.
		 */
		Cert_Store_NV( NV_Ctrl* nv_ctrl,
					   const PKI_Common::nv_chip_config_t* mem_config );

		/**
		 * @brief Get pointer to RVK public key from cert store
		 * @param[out] rvk_key_out: Pointer to RVK key.
		 * @param[out] rvk_key_len_out: Length of the RVK key.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_RVK_Key( uint8_t** rvk_key_out, uint16_t* rvk_key_len_out );

		/**
		 * @brief Get pointer to FVK public key certificate from cert store
		 * @param[out] fvk_cert_out: Pointer to FVK key certificate.
		 * @param[out] fvk_cert_len_out: length of the FVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Cert( uint8_t** fvk_cert_out, uint16_t* fvk_cert_len_out );

		/**
		 * @brief Get pointer to IVK public key certificate from cert store
		 * @param[out] ivk_cert_out: Pointer to IVK key certificate.
		 * @param[out] ivk_cert_len_out: Length of the IVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_IVK_Cert( uint8_t** ivk_cert_out, uint16_t* ivk_cert_len_out );

		/**
		 * @brief Get pointer to FVK public key certificate from cert store
		 * @param[out] fvk_key_out: Pointer to FVK key certificate.
		 * @param[out] fvk_key_len_out: Length of the FVK key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_FVK_Key( uint8_t** fvk_key_out, uint16_t* fvk_key_len_out );

		/**
		 * @brief Write cert store from local buffer to cert store in flash
		 * @details To update cert store components, read the cert store into local buffer, update
		 * individual components then write local buffer into cert store in flash
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Cert_Store_Write( void );

		/**
		 * @brief deallocate the memory allocated while reading rvk key
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_RVK_Key( void );

		/**
		 * @brief deallocate the memory allocated to firmware verification key cert
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_FVK_Cert( void );

		/**
		 * @brief deallocate the memory allocated to intermediate verification key cert
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_IVK_Cert( void );

		/**
		 * @brief Function to check whether RVK Available in cert Store
		 * @return status of requested operation
		 */
		bool Is_RVK_Key_Available( void );

		/**
		 * @brief Update RVK key  in local cert store buffer
		 * @param[in] key_mem: Pointer to RVK key.
		 * @param[in] key_mem_len: Length of the RVK key.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_RVK_Key_Buf( uint8_t* key_mem, uint16_t key_mem_len );

		/**
		 * @brief Update IVK cert  in local cert store buffer
		 * @param[in] cert_mem: Pointer to IVK cert.
		 * @param[in] cert_mem_len: Length of the IVK cert.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_IVK_Cert_Buf( uint8_t* cert_mem, uint16_t cert_mem_len );

		/**
		 * @brief Update FVK cert  in local cert store buffer
		 * @param[in] cert_mem: Pointer to FVK cert.
		 * @param[in] cert_mem_len: Length of the FVK cert.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_FVK_Cert_Buf( uint8_t* cert_mem, uint16_t cert_mem_len );

		/**
		 * @brief Update FVK key in local cert store buffer
		 * @param[in] key_mem: Pointer to FVK key.
		 * @param[in] key_mem_len: Length of the FVK key.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_FVK_Key_Buf( uint8_t* key_mem, uint16_t key_mem_len );

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
		/**
		 * @brief Get pointer to CA certificate from cert store
		 * @param[out] ca_cert_out: Pointer to CA certificate.
		 * @param[out] ca_cert_len_out: Length of the certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_CA_Cert( uint8_t** ca_cert_out, uint32_t* ca_cert_len_out );

		/**
		 * @brief Get pointer to CA private key certificate from cert store
		 * @param[out] ca_key_out: Pointer to CA private key.
		 * @param[out] ca_key_len_out: length of the key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_CA_Key( uint8_t** ca_key_out, uint16_t* ca_key_len_out );

		/**
		 * @brief Get pointer to device private key certificate from cert store
		 * @param[out] dev_key_out: Pointer to device private key.
		 * @param[out] dev_key_len_out: Length of the key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Dev_Key( uint8_t** dev_key_out, uint16_t* dev_key_len_out );

		/**
		 * @brief Update CA certificate in local cert store buffer
		 * @param[in] cert_mem: Pointer to CA certificate.
		 * @param[in] cert_mem_len: length of the CA certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_CA_Cert_Buf( uint8_t* cert_mem, uint32_t cert_mem_len );

		/**
		 * @brief Update CA private key certificate in local cert store buffer
		 * @param[in] key_mem: Pointer to CA private key certificate.
		 * @param[in] key_mem_len: Length of the CA private key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_CA_Key_Buf( uint8_t* key_mem, uint16_t key_mem_len );

		/**
		 * @brief Update device certificate in local cert store buffer
		 * @param[in] key_mem: Pointer to device private key certificate.
		 * @param[in] key_mem_len: Length of the device private key certificate.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_Dev_Key_Buf( uint8_t* key_mem, uint16_t key_mem_len );

		/**
		 * @brief Function to check whether CA Cert is available in cert store
		 * @return status of requested operation
		 */
		bool Is_CA_Cert_Available( void );

		/**
		 * @brief Function to check whether CA private key is available in cert store
		 * @return status of requested operation
		 */
		bool Is_CA_Key_Available( void );

		/**
		 * @brief Function to check whether device private key is available in cert store
		 * @return status of requested operation
		 */
		bool Is_Dev_Key_Available( void );

		/**
		 * @brief deallocate the memory allocated while generating CA certificate
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_CA_Cert( void );

		/**
		 * @brief deallocate the memory allocated while generating CA private key
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_CA_Key( void );

		/**
		 * @brief deallocate the memory allocated while generating device private key
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Free_Dev_Key( void );

#endif
		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Pub_Key_ECC_Matrix goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		~Cert_Store_NV( void );

	private:

		/**
		 * @brief Validate the certificate storage block
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Cert_Store_Integrity_Check( void );

		/**
		 * @brief Read length of the cert and keys in class member variables
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_Cert_Store_Elements( void );

		/**
		 * @brief Get pointer to the crc of local cert store buffer
		 * @details Provides crc to be updated in cert store in flash
		 * @param[out] crc_out: pointer to calculated crc.
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Get_CRC_Buf( uint16_t* crc_out );

		/**
		 * @brief Update CRC in local cert store buffer
		 * @details This function calculates the crc of local buffer and updates the same in local
		 * buffer, this function should be called after updating all required cert store comonents
		 * in local buffer
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_CRC_Buf( void );

		/**
		 * @brief Update Header ID
		 * @details This function updates header ID in local buffer
		 * @param None
		 * @return status of requested operation
		 */
		PKI_Common::pki_status_t Update_Header_ID( void );

		/**
		 * @brief Header Identifier for Cert Store, initial value will be 0xE201,
		 * If Header structure changes, Header ID should be increased
		 */
		static const uint16_t CERT_STORE_HEADER_IDENTIFIER = 0xE201;

		/**
		 * @brief Allocate a memory to get certificate store in local buffer
		 * @return pointer to local RAM buffer
		 */
		cert_store_struct_t* Get_Cert_Store_Ram_Ptr( void );

		/**
		 * @brief pointer to code signing information structure
		 */
		cert_store_struct_t* m_cert_store_ptr;

		/**
		 * @brief minimum required erasable page/sector size
		 */
		uint32_t m_cert_store_len;

		/**
		 * @brief NV control to access cert store in external memory
		 */
		NV_Ctrl* m_nv_ctrl;

		/**
		 * @brief pointer to local buffer in which certificate store is copied
		 */
		cert_store_struct_t* m_cert_store_ram_ptr;

		/**
		 * @brief Flag to indicate whether cert store components are valid or not
		 */
		bool m_cert_store_exists;

		/**
		 * @brief variables to store length of certificates and keys read from external
		 * memory
		 */
		uint16_t m_fvk_cert_len;
		uint16_t m_ivk_cert_len;
		uint16_t m_rvk_key_len;
		uint16_t m_fvk_key_len;

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
		uint16_t m_ca_priv_key_len;
		uint16_t m_ca_cert_len;
		uint16_t m_device_key_len;
#endif

		/**
		 * @brief Pointer to ram buffer for certificates and keys
		 */
		uint8_t* m_fvk_cert;
		uint8_t* m_ivk_cert;
		uint8_t* m_rvk_key;
		uint8_t* m_fvk_key;

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
		uint8_t* m_ca_priv_key;
		uint8_t* m_ca_cert;
		uint8_t* m_device_key;
#endif

};


#endif	// CERT_STORE_NV_H
