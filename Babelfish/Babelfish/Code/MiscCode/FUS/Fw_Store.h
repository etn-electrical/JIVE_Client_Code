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
#ifndef FW_STORE_H
#define FW_STORE_H

#include "Includes.h"
#include "PKI_Config.h"
#include "Uberloader_Data.h"
#include "Sha256_Matrix.h"

static const uint16_t VERSION_INFO_SIZE = 16U;
static const uint16_t PRODUCT_CODE_SIZE = 32U;
static const uint16_t FW_HDR_RESERVED_BYTES = 54U;
static const uint16_t FW_STORE_RESERVED_BYTES = 100U;
static const uint16_t CODE_SIGN_INFO_MAX_SIZE = ( 4 * 1024U );
static const uint32_t CODE_SIGN_INFO_IDENTITY_MARK = 0x4598FE21;
static const uint16_t CODE_SIGN_INFO_HEADER_VERSION = 0x0001;
static const uint8_t ECC_CURVE_TYPE = 0x17;	///< curve type is SECP256R1
static const uint8_t KEY_SIZE = 0x01;	///< 256 bits
static const uint8_t HASH_SIZE = 0x02;	///< SHA256


/* A function pointer for callback function */
typedef void ( * callback_t )( void );

/**
 * @brief Structure containing Firmware header components and firmware start location
 */
typedef struct fw_header_s
{
	/** @brief Magic number header; also helps define endianness. */
	uint32_t magic_endian;

	/** @brief Length of the data including FW header and FW */
	uint32_t data_length;

	/** @brief nv address for boot loader to receive message from application. */
	uint32_t msg_for_boot_nvadd;

	/** @brief Version of this header format; right now only one is defined.
	 * Any field past this one is defined by this format.*/
	uint16_t header_format_version;

	/** @brief id of fw images (application, web, host, etc) */
	uint16_t image_guid;

	/** @brief Version of this FW file. */
	uint8_t version_info[VERSION_INFO_SIZE];

	/** @brief Product for which FW is intended. Note-aligned to 8 bytes. */
	uint8_t product_code[PRODUCT_CODE_SIZE];

	/** @brief Compatibility number of the image, all images in system should have same
	   compatibility number. */
	uint16_t compatibility_num;

	/** @brief Reserved header space */
	uint8_t reserved[58];

} fw_header;///< total header size(including crc at start) becomes 128 bytes.

typedef struct crc_info_s
{
	uint16_t crc;
	uint16_t reserved;
} crc_info;

typedef struct real_fw_header_s
{
	crc_info fw_crc;	///< crc to validate firmware package components
	fw_header f;
	prodinfo_for_boot_s prod;
	/** @brief Payload data. */
	uint8_t data[4];
} real_fw_header;


class Fw_Store
{
	public:

		/**
		 * @brief Code signing information structure
		 */

		struct code_sign_struct_t
		{
			uint32_t identity_mark;
			uint16_t header_version_num;///< Version of code sign information header
			uint8_t cross_sign_status;	///< Status indication New RVK is available

			uint8_t algo_type;			///< Algorithm type enum ECC or RSA ( 0 - RSA, 1 - ECC )

			uint8_t ecc_curve_type;		///< ECC curve type ( e.g. ECP256R1/PRIME - 20 )

			uint8_t key_size;			///< key size enum ( e.g. 0 - 128, 1 - 256 )

			uint8_t hash_size;			///< Provides the hash type enum (SHAx) used in the code
			///< signing (e.g. sha256, sha384) (2 - SHA256)
			uint8_t reserved;

			uint8_t fvk_key[FVK_KEY_MAX_SIZE];		///< firmware verification key only
			uint8_t fvk_key_len[KEY_LEN_MAX_BYTES];		///< length of firmware verification key

			uint8_t fvk_cert[FVK_CERT_MAX_SIZE];	///< Firmware verification key certificate
			///< (key+ signature), used to verify the FW
			///< signature.
			uint8_t fvk_cert_len[CERT_LEN_MAX_BYTES];///< Length of firmware verification key cert

			uint8_t ivk_cert[IVK_CERT_MAX_SIZE];	///< Intermediate verification key certificate,
			///< used to verify fvk
			uint8_t ivk_cert_len[CERT_LEN_MAX_BYTES];///< Length of intermediate verification key
			///< cert

			uint8_t rvk_key[RVK_KEY_MAX_SIZE];
			uint8_t rvk_key_len[KEY_LEN_MAX_BYTES];

			uint8_t fw_sign[FW_SIGN_MAX_SIZE];		///< FW signature ( signature of the data
			///< including fw header, fw code and
			///< code_sign_info till ivk_cert_len )
			uint8_t fw_sign_len[SIGN_LEN_MAX_BYTES];///< Length of firmware signature
			uint16_t reserved_align;
			uint16_t crc;
		};

		/**
		 * @brief Get pointer to FVK key from code sign info block
		 * @param[out] fvk_key_out: Pointer to FVK key.
		 * @param[out] fvk_key_len_out: length of the FVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_FVK_Key( uint8_t** fvk_key_out,
													  uint16_t* fvk_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to FVK public key certificate from code sign info block
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
		 * @brief Get pointer to IVK public key certificate from code sign info block
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
		 * @brief Get pointer to firmware signature from code sign info block
		 * @param[out] fw_sign_out: Pointer to firmware signature.
		 * @param[out] fw_sign_len_out: Length of firmware signature.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Fw_Sign( uint8_t** fw_sign_out,
													  uint16_t* fw_sign_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to RVK key from code sign info block
		 * @param[out] rvk_cert_out: Pointer to RVK key.
		 * @param[out] rvk_cert_len_out: Length of the RVK key.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_RVK_Key( uint8_t** rvk_key_out,
													  uint16_t* rvk_key_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Calculate hash of the firmware and return the same
		 * @param[out] calculated_hash: Hash of the firmware.
		 * @param[out] hash_size_out: sha256 hash size.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Hash( uint8_t** calculated_hash,
												   uint32_t* hash_size_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get the cross signing status, it will be set when RVK key in code sign info block
		 * is new
		 * @return status of requested operation
		 */
		virtual bool Get_Cross_Sign_Status( void )
		{
			return ( false );
		}

		/**
		 * @brief Calculate hash of the firmware (including components in cross sign info block) and
		 * return the same
		 * @param[out] calculated_hash: Hash of the firmware.
		 * @param[out] hash_size_out: sha256 hash size.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Hash_Trusted( uint8_t** calculated_hash,
														   uint32_t* hash_size_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to trusted FVK certificate from cross sign info block
		 * @param[out] fvk_cert_out: Pointer to trusted FVK certificate.
		 * @param[out] fvk_cert_len_out: Length of the trusted FVK certificate.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Trusted_FVK_Cert( uint8_t** fvk_cert_out,
															   uint16_t* fvk_cert_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Get pointer to Trusted firmware signature from cross sign info block
		 * @param[out] fw_sign_out: Pointer to Trusted firmware signature.
		 * @param[out] fw_sign_len_out: Length of the Trusted firmware signature.
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Get_Trusted_Fw_Sign( uint8_t** fw_sign_out,
															  uint16_t* fw_sign_len_out )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief deallocate the memory allocated while reading trusted FVK cert from external
		 * memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_Trusted_FVK_Cert( uint8_t* cert )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief deallocate the memory allocated while reading trusted Fw signature from external
		 * memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_Trusted_Fw_Sign( uint8_t* sign )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Verify the CRC of extended code sign information block(cross signing)
		 * @param None
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Validate_Cross_Sign_CRC( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief deallocate the memory allocated while calculating hash of the firmware
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_Hash( uint8_t* calculated_hash )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief deallocate the memory allocated while reading FVK cert from external memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_FVK_Cert( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while reading FVK key from external memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_FVK_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while reading IVK cert from external memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_IVK_Cert( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while reading FW signature from external memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_Fw_Sign( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 * @brief deallocate the memory allocated while reading RVK key from external memory
		 * @return status of requested operation
		 */
		virtual PKI_Common::pki_status_t Free_RVK_Key( void )
		{
			return ( PKI_Common::SUCCESS );
		}

		/**
		 *  @brief Destructor.
		 *  @details It will get called when object of Fw_Store_Control goes out of scope or
		 * deleted.
		 *  @return None.
		 */
		virtual ~Fw_Store( void )
		{}

	protected:
		/**
		 * @brief Constructor.
		 * @return None.
		 */
		Fw_Store( void )
		{}

};


#endif	// FW_STORE_H
