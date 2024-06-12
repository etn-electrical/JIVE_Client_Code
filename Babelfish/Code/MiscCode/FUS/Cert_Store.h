/**
 **************************************************************************************************
 *  @file
 *
 *	@details Fw_Store.h provides base class and APIs to access firmware and code
 * sign information block
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_STORE_H
#define CERT_STORE_H

#include "Includes.h"
#include "PKI_Config.h"

class Cert_Store
{
	public:

		/**
		 * @brief enum for certificate store components
		 */
		enum cert_store_enum_t
		{
			CERT_STORE_HEADER_ID,
			CERT_STORE_CRC,
			RVK_KEY_LEN,
			RVK_KEY,
			IVK_CERT_LEN,
			IVK_CERT,
			FVK_CERT_LEN,
			FVK_CERT,
			FVK_KEY_LEN,
			FVK_KEY,
#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
			CA_PRIV_KEY_LEN,
			CA_PRIV_KEY,
			CA_CERT_LEN,
			CA_CERT,
			DEVICE_KEY_LEN,
			DEVICE_KEY
#endif
		};

		/**
		 * @brief Structure to define certificate store components
		 * @details cert store holds CA cerificate, private key certificates( key + signature )
		 * used for communication certificates and public key certificates( key + signature ) which
		 * are used as a backup. public key certs in Code sign info block are compared with these
		 * key
		 * certs to identify whether FVK/IVK/RVK is new or corrupted
		 */
		struct cert_store_struct_t
		{
			uint16_t header_id;	///< cert store ID

			uint16_t crc;		///< crc to validate cert store components, crc is
			///< calculated and updated when any of the cert store
			///< component is updated.

			uint16_t rvk_key_len;				///< Length of root verification key
			uint8_t rvk_key[RVK_KEY_MAX_SIZE];	///< root verification key used to verify IVK

			uint16_t ivk_cert_len;				///< Length of intermediate verification key cert
			uint8_t ivk_cert[IVK_CERT_MAX_SIZE];///< Intermediate verification key certificate
												///< backup

			uint16_t fvk_cert_len;				///< Length of firmware verification key cert
			uint8_t fvk_cert[FVK_CERT_MAX_SIZE];///< Firmware verification key certificate backup

			uint16_t fvk_key_len;				///< Length of firmware verification key
			uint8_t fvk_key[FVK_KEY_MAX_SIZE];	///< Firmware verification key backup

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
			uint16_t ca_priv_key_len;			///< Length of CA private key certificate
			uint8_t ca_priv_key[CA_KEY_MAX_SIZE];///< CA key used to sign CA certificate

			uint16_t ca_cert_len;				///< Length of CA cerificate
			uint8_t ca_cert[CA_CERT_MAX_SIZE];	///< CA cerificate used to generate device
												///< certificate for https
			uint16_t device_key_len;			///< Length of device private key certificate
			uint8_t device_key[DEV_KEY_MAX_SIZE];///< Device private key used to sign device
												///< certificate, device certificate is generated
												///< everytime on power up
#endif
		};

		/**
		 * @brief Get pointer to RVK public key from cert store
		 * @param[out] rvk_key_out: Pointer to RVK key.
		 * @param[out] rvk_key_len_out: Length of the RVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_RVK_Key( uint8_t** rvk_key_out,
													  uint16_t* rvk_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to FVK public key certificate from cert store
		 * @param[out] fvk_cert_out: Pointer to FVK key certificate.
		 * @param[out] fvk_cert_len_out: length of the FVK key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_FVK_Cert( uint8_t** fvk_cert_out,
													   uint16_t* fvk_cert_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to IVK public key certificate from cert store
		 * @param[out] ivk_cert_out: Pointer to IVK key certificate.
		 * @param[out] ivk_cert_len_out: Length of the IVK key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_IVK_Cert( uint8_t** ivk_cert_out,
													   uint16_t* ivk_cert_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to FVK key from cert store
		 * @param[out] fvk_key_out: Pointer to FVK key.
		 * @param[out] fvk_key_len_out: Length of the FVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_FVK_Key( uint8_t** fvk_key_out,
													  uint16_t* fvk_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Write cert store from local buffer to cert store in flash
		 * @details To update cert store components, read the cert store into local buffer, update
		 * individual components then write local buffer into cert store in flash
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Cert_Store_Write( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update Header ID
		 * @details This function updates header ID in local buffer
		 * @param None
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_Header_ID( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief deallocate the memory allocated while reading rvk key
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_RVK_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated to firmware verification key cert
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_FVK_Cert( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated to intermediate verification key cert
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_IVK_Cert( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated to fvk key
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_FVK_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief Function to check whether RVK Available in cert Store
		 * @return status of requested operation
		 */
		virtual bool Is_RVK_Key_Available( void )
		{
			return ( false );
		}

		/**
		 * @brief Update RVK key  in local cert store buffer
		 * @param[in] key_mem: Pointer to RVK key.
		 * @param[in] key_mem_len: Length of the RVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_RVK_Key_Buf( uint8_t* key_mem,
															 uint16_t key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update IVK cert  in local cert store buffer
		 * @param[in] cert_mem: Pointer to IVK cert.
		 * @param[in] cert_mem_len: Length of the IVK cert.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_IVK_Cert_Buf( uint8_t* cert_mem,
															  uint16_t cert_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update FVK cert  in local cert store buffer
		 * @param[in] cert_mem: Pointer to FVK cert.
		 * @param[in] cert_mem_len: Length of the FVK cert.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_FVK_Cert_Buf( uint8_t* cert_mem,
															  uint16_t cert_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update FVK key  in local cert store buffer
		 * @param[in] key_mem: Pointer to FVK key.
		 * @param[in] key_mem_len: Length of the FVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_FVK_Key_Buf( uint8_t* key_mem,
															 uint16_t key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
		/**
		 * @brief Get pointer to CA certificate from cert store
		 * @param[out] ca_cert_out: Pointer to CA certificate.
		 * @param[out] ca_cert_len_out: Length of the certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_CA_Cert( uint8_t** ca_cert_out,
													  uint32_t* ca_cert_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to CA private key certificate from cert store
		 * @param[out] ca_key_out: Pointer to CA private key.
		 * @param[out] ca_key_len_out: length of the key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_CA_Key( uint8_t** ca_key_out,
													 uint16_t* ca_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to device private key certificate from cert store
		 * @param[out] dev_key_out: Pointer to device private key.
		 * @param[out] dev_key_len_out: Length of the key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Dev_Key( uint8_t** dev_key_out,
													  uint16_t* dev_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update CA certificate in local cert store buffer
		 * @param[in] cert_mem: Pointer to CA certificate.
		 * @param[in] cert_mem_len: length of the CA certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_CA_Cert_Buf( uint8_t* cert_mem,
															 uint32_t cert_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update CA private key certificate in local cert store buffer
		 * @param[in] key_mem: Pointer to CA private key certificate.
		 * @param[in] key_mem_len: Length of the CA private key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_CA_Key_Buf( uint8_t* key_mem, uint16_t key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Update device certificate in local cert store buffer
		 * @param[in] key_mem: Pointer to device private key certificate.
		 * @param[in] key_mem_len: Length of the device private key certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Update_Dev_Key_Buf( uint8_t* key_mem,
															 uint16_t key_mem_len )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Function to check whether CA Cert is available in cert store
		 * @return status of requested operation
		 */
		virtual bool Is_CA_Cert_Available( void )
		{
			return ( false );
		}

		/**
		 * @brief Function to check whether CA private key is available in cert store
		 * @return status of requested operation
		 */
		virtual bool Is_CA_Key_Available( void )
		{
			return ( false );
		}

		/**
		 * @brief Function to check whether device private key is available in cert store
		 * @return status of requested operation
		 */
		virtual bool Is_Dev_Key_Available( void )
		{
			return ( false );
		}

		/**
		 * @brief deallocate the memory allocated while generating CA certificate
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_CA_Cert( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while generating CA private key
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_CA_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while generating device private key
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_Dev_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

#endif
		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Pub_Key_ECC_Matrix goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		virtual ~Cert_Store( void )
		{}

	protected:
		/**
		 * @brief Constructor.
		 * @return None.
		 */
		Cert_Store( void )
		{}

};

#endif	// CERT_STORE_H
