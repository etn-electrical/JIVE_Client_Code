/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_DIGEST_AUTHENTICATION_H__
#define __HTTPD_DIGEST_AUTHENTICATION_H__

#include "httpd_common.h"

#define WEB_REALM "webusers@ethernet"
#define REALM_PARAM_STR "realm"
#define NONCE_PARAM_STR "nonce"
#define QOP_PARAM_STR "qop"
#define ALGORITHM_STR "algorithm"
#define MD5_STR "MD5"
#define AUTH_QOP "auth"
#define OPAQUE_PARAM_STR "opaque"

#define USERNAME_PARAM_STR "username"
#define URI_PARAM_STR "uri"
#define NC_PARAM_STR "nc"
#define CNONCE_PARAM_STR "cnonce"
#define RESPONSE_PARAM_STR "response"

enum
{
	USER_NAME_FIELD_RX = 0U,
	NONCE_FIELD_RX,
	REALM_FIELD_RX,
	URI_FIELD_RX,
	ALGORITHM_FIELD_RX,
	RESPONSE_FIELD_RX,
	OPAQUE_FIELD_RX,
	QOP_FIELD_RX,
	NC_FIELD_RX,
	CNONCE_FIELD_RX
};

static const uint16_t DIGEST_DATA_MASK = ( ( 1U << USER_NAME_FIELD_RX ) |
										   ( 1U << NONCE_FIELD_RX ) |
										   ( 1U << REALM_FIELD_RX ) |
										   ( 1U << URI_FIELD_RX ) |
										   ( 1U << ALGORITHM_FIELD_RX ) |
										   ( 1U << RESPONSE_FIELD_RX ) |
										   ( 1U << OPAQUE_FIELD_RX ) |
										   ( 1U << QOP_FIELD_RX ) |
										   ( 1U << NC_FIELD_RX ) |
										   ( 1U << CNONCE_FIELD_RX ) );	///< 0b0000001111111111
/*
 * +--------+----+-----+--------+----------+------+-----+-------+-------+-----------+
 * | cnonce | nc | qop | opaque | response | algo | uri | realm | nonce | user name |
 * +--------+----+-----+--------+----------+------+-----+-------+-------+-----------+
 */
static const uint8_t AUTH_BUF_FIELD_LENGTH = 32U;
static const uint8_t AUTH_BUF_URI_FIELD_LENGTH = 80U;
static const uint8_t USERNAME_LENGTH = 32U;
static const uint8_t WEB_REALM_LENGTH = STRLEN( WEB_REALM );
static const uint8_t OUTPUT_NONCE_LENGTH = 32U;
static const uint8_t NONCE_LENGTH = 16U;
static const uint8_t OPAQUE_LENGTH = 6U;
/* cnonce length varies based on client request (Linux: 44U, Windows: 32U),
   keep the maximum required length of cnonce */
static const uint8_t CNONCE_LENGTH = 44U;
static const uint8_t NC_LENGTH = 8U;
static const uint8_t AUTH_QOP_LENGTH = STRLEN( AUTH_QOP );
static const uint8_t RESPONSE_LENGTH = 32U;
static const uint8_t ALGORITHM_LENGTH = STRLEN( MD5_STR );
static const uint8_t MD5_DEST_BUF_LENGTH = 32U;
static const uint8_t COLON_LENGTH = 5U;		///< HA1:nonce:nc:cnonce:qop:HA2
static const uint8_t MD5_SRC_BUF_LENGTH = MD5_DEST_BUF_LENGTH + OUTPUT_NONCE_LENGTH + NC_LENGTH +
	CNONCE_LENGTH + AUTH_QOP_LENGTH + MD5_DEST_BUF_LENGTH + COLON_LENGTH;

typedef struct
{
	uint8_t rx_nonce_buf[NONCE_LENGTH];
	uint8_t rx_user_name[USERNAME_LENGTH];
	uint8_t rx_responce_encode[RESPONSE_LENGTH];
	uint8_t rx_nc_response[NC_LENGTH];
	uint8_t rx_cnonce_response[CNONCE_LENGTH];
	uint8_t rx_md5_uri[AUTH_BUF_URI_FIELD_LENGTH];
	/* Currently there is URI length limit of 64
	   For uri length greater than this the digest authentication will fail
	 */

	uint8_t rx_user_name_length;
	uint8_t rx_response_length;
	uint8_t rx_cnonce_length;
	uint8_t rx_md5_uri_length;

	union received_flags_t
	{
		struct
		{
			uint16_t user_name : 1;
			uint16_t nonce : 1;
			uint16_t realm : 1;
			uint16_t uri : 1;
			uint16_t algo : 1;
			uint16_t response : 1;
			uint16_t opaque : 1;
			uint16_t qop : 1;
			uint16_t nc : 1;
			uint16_t cnonce : 1;
		} flag;
		uint16_t digest_all_flags;
	} digest_flags;

	uint8_t state;
} Digest_data;

class httpd_digest_authentication
{
	public:
		static char_t OPAQUE[OPAQUE_LENGTH];

		/**
		 * @brief Function to verify the data packet's authenticity.
		 * @details This function calculates the MD5 sum hash and matches it with the response from
		 * the client.
		 * @param[in] p: Digest data buffer.
		 * @param[in] password: password of the user.
		 * @param[in] password_len: password length.
		 * @return true if valid else false.
		 */
		static bool Verify_User_Digest( Digest_data* p, uint8_t* password, uint8_t password_len );

		/**
		 * @brief Function to convert 16 bytes to 32 byte hex value.
		 * @details This function converts 16 bytes hex value to 32 byte human-readable form.
		 * byte with lower index shows up earlier in the hex array within a byte,
		 * the higher nibble shows up earlier in the hex array.
		 * @param[in] pInputByte: Input buffer.
		 * @param[out] pOutputHex: Output buffer.
		 * @return none.
		 */
		static void From_16Bytes_To_32Hex( const uint8_t pInputByte[16], uint8_t pOutputHex[32] );

		/**
		 * @brief Function to convert 32 bytes to 16 byte hex value.
		 * @details This function converts 32 bytes values to 16 byte hex. Byte with lower index
		 * shows up earlier in the hex array within a byte, the higher nibble shows up earlier in
		 * the
		 * hex array.
		 * @param[in] pInputHex: Input buffer.
		 * @param[out] pOutputByte: Output buffer.
		 * @return true if success, false if unsuccessful..
		 */
		static bool_t From_32Hex_To_16Bytes( const uint8_t pInputHex[32], uint8_t pOutputByte[16] );

		/**
		 * @brief Function to generate random opaque value.
		 * @details This function generates a random value and stores it in an array in Hex format.
		 * @return void
		 */
		static void Generate_Opaque();

	private:
		static const uint8_t m_low_hex_array[17U];

};

#endif	/* __HTTPD_DIGEST_AUTHENTICATION_H__ */
