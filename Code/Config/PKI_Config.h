/**
 **************************************************************************************************
 *  @file
 *
 *	@details PKI_Config.h contains common variables and macro definitions necessary for code signing and cert generation
 * using RSA and ECC algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef PKI_CONFIG_H
#define PKI_CONFIG_H

#include "Includes.h"
#include "PKI_Common.h"

/**
 * @brief  variables to check availability of cert store components.
 */
static const uint8_t FLASH_MEMORY_DEFAULT_VALUE_U8 = 0xFF;
static const uint16_t FLASH_MEMORY_DEFAULT_VALUE_U16 = 0xFFFF;
static const uint16_t FLASH_MEMORY_ZERO_VALUE_U16 = 0x0000;
static const uint32_t FLASH_MEMORY_DEFAULT_VALUE_U32 = 0xFFFFFFFF;
static const uint32_t FLASH_MEMORY_ZERO_VALUE_U32 = 0x00000000;

/* This macro specifies certificate generation method */
#define CERT_GEN_ECC_CA_DEV
// #define CERT_GEN_RSA_CA_DEV
// #define CERT_GEN_RSA_CA_ECC_DEV

static const uint32_t SIGNATURE_MAX_LENGTH = 2048U;
static const uint32_t CODE_SIGN_HEADER_VERSION = 0x4598FE21;

#ifdef CERT_GEN_RSA_CA_ECC_DEV
static const uint16_t CA_CERT_MAX_SIZE = 1022;
static const uint16_t CA_KEY_MAX_SIZE = 1298;
static const uint16_t DEV_KEY_MAX_SIZE = 254;

#elif defined CERT_GEN_RSA_CA_DEV
static const uint16_t CA_CERT_MAX_SIZE = 1022;
static const uint16_t CA_KEY_MAX_SIZE = 1298;
static const uint16_t DEV_KEY_MAX_SIZE = 1298;

#elif defined CERT_GEN_ECC_CA_DEV
static const uint16_t CA_CERT_MAX_SIZE = 766;
static const uint16_t CA_KEY_MAX_SIZE = 254;
static const uint16_t DEV_KEY_MAX_SIZE = 254;

#else
#error "Certificate generation method not defined"
#endif

static const uint16_t FVK_CERT_MAX_SIZE = 1022;		///< Maximum size ( in bytes ) of FVK certificate (FVK key + FVK
///< signature)
static const uint16_t IVK_CERT_MAX_SIZE = 1534;		///< Maximum size of IVK certificate (IVK key + IVK signature)

static const uint16_t FVK_KEY_MAX_SIZE = 162;		/// FVK key maximum size in bytes
static const uint16_t IVK_KEY_MAX_SIZE = 162;		/// IVK key maximum size in bytes
static const uint16_t RVK_KEY_MAX_SIZE = 162;		/// RVK key maximum size in bytes

static const uint16_t FVK_KEY_SIGN_MAX_SIZE = 142;	/// RVK key maximum size in bytes
static const uint16_t IVK_KEY_SIGN_MAX_SIZE = 142;	/// RVK key maximum size in bytes
static const uint16_t FW_SIGN_MAX_SIZE = 142;		///< Maximum firmware signature size
static const uint16_t CERT_LEN_MAX_BYTES = 2;		///< Maximum bytes of cert len
static const uint16_t KEY_LEN_MAX_BYTES = 2;		///< Maximum bytes of key len
static const uint16_t SIGN_LEN_MAX_BYTES = 2;		///< Maximum bytes of signature len
static const uint16_t RESERVED_BYTES = 2U;

static const uint32_t MAX_ALLOCATE_BYTES = 256U;	///< update 256Bytes data at a time while generating hash.

#ifndef BOOTLOADER_BUILD
/**
 **************************************************************************************************
 *  Defines used by old code sign method.
 **************************************************************************************************
 */

static const uint8_t FVK_PUB_KEY_SIZE_LEN = 2U;		///< number of bytes to be read to get fvk_pub_key_len from external
///< memory.
static const uint32_t FVK_VER_LENGTH = 4U;
static const uint32_t FVK_SIZE_LENGTH = 2U;
static const uint32_t RESERVED_WORD = 2U;
static const uint32_t FVK_SIGN_SIZE_LENGTH = 2U;
static const uint32_t FIRMWARE_SIGN_SIZE_LENGTH = 2U;

#define FVK_PUB_KEY_LEN_ADDR( fw_location, \
							  fw_size )                        ( fw_location + fw_size + \
																 FVK_VER_LENGTH )

#define FVK_PUB_KEY_ADDR( fw_location, \
						  fw_size )                            ( fw_location + fw_size + \
																 FVK_VER_LENGTH + \
																 FVK_SIZE_LENGTH + \
																 RESERVED_WORD )

#define FVK_SIGNATURE_SIZE_ADDR( fvk_pub_key, \
								 fvk_pub_key_len )             ( fvk_pub_key + fvk_pub_key_len )

#define FVK_SIGNATURE_ADDR( fvk_pub_key, \
							fvk_pub_key_len )                  ( fvk_pub_key + fvk_pub_key_len + \
																 FVK_SIGN_SIZE_LENGTH )

#define FW_SIGNATURE_SIZE_ADDR( fvk_signature, \
								fvk_signature_size )         ( fvk_signature + fvk_signature_size )

#define FW_SIGNATURE_ADDR( fvk_signature, \
						   fvk_signature_size )              ( fvk_signature + fvk_signature_size + \
															   FIRMWARE_SIGN_SIZE_LENGTH )

#define FW_PACKAGE_SIZE( fw_size, fvk_pub_key_len, \
						 fvk_signature_size ) ( fw_size + FVK_VER_LENGTH + FVK_SIZE_LENGTH + \
												RESERVED_WORD + \
												fvk_pub_key_len + \
												FVK_SIGN_SIZE_LENGTH + \
												fvk_signature_size )

#else
/* CODE_SIGN macro is used to,
   1. enable RVK/IVK/FVK upgrade
   2. enable application signature verification on power up if APP_CODE_SIGN_CHECK is defined*/
#define CODE_SIGN
// Ahmed changing the code from local signing to server signing

#ifdef CODE_SIGN
/* SERVER_SIGN macro enables CCoE server signing method, this will write rvk available in
   server_sign_rvk.c into RVK region. during firmware upgrade, firmware should be verified by CCoE
   server keys */
#define SERVER_SIGN

/* LOCAL_SIGN macro enables local signing method, this will write rvk available in
   local_sign_rvk.c into RVK region. during firmware upgrade, firmware should be verified by locally
   generated keys. Local signing is used for development purpose */
//#define LOCAL_SIGN

#if defined ( LOCAL_SIGN ) && defined ( SERVER_SIGN )
#error "Defined multiple code signing methods"	///< Either LOCAL_SIGN or SERVER_SIGN is supported.
#endif

/* If APP_CODE_SIGN_CHECK is defined then application signature is verified on powerup */
// #define APP_CODE_SIGN_CHECK
#endif	// CODE_SIGN

#endif	// BOOTLOADER_BUILD

#endif	// PKI_CONFIG_H
