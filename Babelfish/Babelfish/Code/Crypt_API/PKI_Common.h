/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base structure required for the certificate class.
 *
 *	@details PKI_Common contains the necessary enums, structures and Key masks as the certificate
 *	generation attributes to be passed to the certificate class constructors.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef PKI_COMMON_H
#define PKI_COMMON_H

#include "crypto/cryptoApi.h"
#include "NV_Ctrl.h"

class PKI_Common
{
	public:

		/**
		 * @brief Return Status for crypt/certificate operations.
		 */
		enum pki_status_t
		{
			SUCCESS,
			DATA_ERROR,
			BUSY_ERROR,
			OPERATION_NOT_SUPPORTED,
			PENDING_CALL_BACK,
			FAILURE = -1
		};

		/**
		 * @brief Certificate storage NV address.
		 */
		struct nv_chip_config_t
		{
			NV_CTRL_ADDRESS_TD start_address;
			NV_CTRL_LENGTH_TD length;
		};

		/**
		 * @brief Key algorithm type
		 */
		enum algorithm_type
		{
			PKI_RSA,
			PKI_ECC
		};

		/**
		 * @brief Certificate algorithms.
		 */
		enum hash_alg_t
		{
			SHA1,
			MD5,
			SHA256,
			SHA384,
			SHA512,
			SHA224
		};

		/**
		 * @brief Enum for RSA key sizes.
		 */
		enum key_size_rsa_t
		{
			KEY_RSA_1024 = 1024U,
			KEY_RSA_2048 = 2048U,
			KEY_RSA_4096 = 4096U
		};

		/**
		 * @brief Enum for ECC key sizes.
		 */
		enum key_size_ecc_t
		{
			KEY_ECC_128 = 128U,
			KEY_ECC_256 = 256U
		};

		/**
		 * @brief Enum for Elliptic Curve types.
		 */
		enum key_curve_type_t
		{
			CURVE_SECP192R1,
			CURVE_SECP224R1,
			CURVE_SECP256R1,
			CURVE_SECP384R1,
			CURVE_SECP521R1,
			CURVE_BRAIN256R1,
			CURVE_BRAIN384R1,
			CURVE_BRAIN512R1,
			CURVE_X25519,
			CURVE_X44830U,
			CURVE_BRAIN224R1
		};

		/**
		 * @brief Certificate generation attributes.
		 */
		struct pki_conf_t
		{
			hash_alg_t cert_alg;	///< Certificate algorithm; See enum above
			bool bsc_constraint_ca;	///< Can be true or false
			uint16_t bsc_path_len;	///< Basic constraint path length
			uint16_t key_usage;		///< Multiple values can be passed
			uint8_t ext_key_usage;	///< Multiple values can be passed
		};

		static const uint16_t MAX_STRING_LENGTH = 256U;

		/* Flags for known Key Usage */
		static const uint8_t KEY_USG_CRL_SIGN = 0x02;
		static const uint8_t KEY_USG_KEY_CERT_SIGN = 0x04;
		static const uint8_t KEY_USG_KEY_AGREEMENT = 0x08;
		static const uint8_t KEY_USG_DATA_ENCIPHERMENT = 0x10;
		static const uint8_t KEY_USG_KEY_ENCIPHERMENT = 0x20;
		static const uint8_t KEY_USG_NON_REPUDIATION = 0x40;
		static const uint8_t KEY_USG_DIGITAL_SIGNATURE = 0x80;
		// static const uint16_t KEY_USG_DECIPHER_ONLY = 0x8000;	///< Optional

		/* Flags for known Extended Key Usage */
		static const uint8_t EXT_KEY_USG_TLS_SERVER_AUTH = 0x02;
		static const uint8_t EXT_KEY_USG_TLS_CLIENT_AUTH = 0x04;
		static const uint8_t EXT_KEY_USG_CODE_SIGNING = 0x08;

		/* Key Usage strings */
		#define CRL_SIGN "crlSign"
		#define KEY_CERT_SIGN "keyCertSign"
		#define KEY_AGREEMENT "keyAgreement"
		#define DATA_ENCIPHERMENT "dataEncipherment"
		#define KEY_ENCIPHERMENT "keyEncipherment"
		#define NON_REPUDIATION "nonRepudiation"
		#define DIGITAL_SIGNATURE "digitalSignature"

		/* Extended Key Usage strings */
		#define SERVER_AUTH "serverAuth"
		#define CLIENT_AUTH "clientAuth"
		#define CODE_SIGNING "codeSigning"
};

#endif	// PKI_COMMON_H
