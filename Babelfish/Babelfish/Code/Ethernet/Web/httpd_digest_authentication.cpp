/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/tcp.h"
#include "MD5_Digest.h"
#include "httpd_digest_authentication.h"
#include "ASCII_Conversion.h"
#include "uC_Rand_Num.h"
#include "StdLib_MV.h"
/*
 *****************************************************************************************
 *		Static Variable
 *****************************************************************************************
 */
const uint8_t httpd_digest_authentication::m_low_hex_array[17] = "0123456789abcdef";
char_t httpd_digest_authentication::OPAQUE[OPAQUE_LENGTH] = {'a', 0, 0, 0, 0, 'f'};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool httpd_digest_authentication::Verify_User_Digest( Digest_data* p, uint8_t* password,
													  uint8_t password_len )
{
	bool return_val = false;
	uint8_t offset = 0U;
	uint8_t password_size = password_len;
	uint8_t md5_src_buf1[MD5_SRC_BUF_LENGTH] = { 0U };
	uint8_t md5_dest_buf_main1[MD5_DEST_BUF_LENGTH] = { 0U };
	uint8_t md5_dest_buf_aux1[MD5_DEST_BUF_LENGTH] = { 0U };

	memcpy( &md5_src_buf1[offset], p->rx_user_name, p->rx_user_name_length );	///< Copy the user name
	offset = p->rx_user_name_length;
	md5_src_buf1[offset] = 0x3AU;	///< Add colon ':'
	offset++;

	memcpy( &md5_src_buf1[offset], WEB_REALM, WEB_REALM_LENGTH );	///< Copy the realm
	offset += WEB_REALM_LENGTH;
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	password_size = strlen( reinterpret_cast<char_t*>( password ) );
	if ( password_size < password_len )
	{
		memcpy( &md5_src_buf1[offset], password, password_size );	///< copy password
		offset += password_size;
	}
	else
	{
		memcpy( &md5_src_buf1[offset], password, password_len );
		offset += password_len;
	}

	MD5_Digest::Perform_Digest( md5_src_buf1, offset, md5_dest_buf_main1 );	///< Calculate HA1
	MD5_Digest::Perform_Digest( p->rx_md5_uri, p->rx_md5_uri_length, md5_dest_buf_aux1 );///< calculate HA2

	offset = 0U;
	memcpy( &md5_src_buf1[offset], md5_dest_buf_main1, MD5_DEST_BUF_LENGTH );	///< Copy HA1
	offset += MD5_DEST_BUF_LENGTH;
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	From_16Bytes_To_32Hex( p->rx_nonce_buf, &md5_src_buf1[offset] );	///< Convert and copy nonce
	offset += OUTPUT_NONCE_LENGTH;
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	memcpy( &md5_src_buf1[offset], p->rx_nc_response, NC_LENGTH );	///< Copy nc
	offset += NC_LENGTH;
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	memcpy( &md5_src_buf1[offset], p->rx_cnonce_response, p->rx_cnonce_length );	///< Copy cnonce
	offset += p->rx_cnonce_length;
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	memcpy( &md5_src_buf1[offset], AUTH_QOP, AUTH_QOP_LENGTH );	///< Copy qop
	offset += STRLEN( AUTH_QOP );
	md5_src_buf1[offset] = 0x3AU;
	offset++;

	memcpy( &md5_src_buf1[offset], md5_dest_buf_aux1, MD5_DEST_BUF_LENGTH );	///< Copy HA2
	offset += MD5_DEST_BUF_LENGTH;

	MD5_Digest::Perform_Digest( md5_src_buf1, offset, md5_dest_buf_main1 );

	if ( 0U == ( memcmp( p->rx_responce_encode, md5_dest_buf_main1, MD5_DEST_BUF_LENGTH ) ) )
	{
		return_val = true;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void httpd_digest_authentication::From_16Bytes_To_32Hex( const uint8_t pInputByte[16],
														 uint8_t pOutputHex[32U] )
{
	uint8_t output_index = 0U;

	for ( uint8_t input_index = 0U; input_index < 16U; input_index++ )
	{
		pOutputHex[output_index] = m_low_hex_array[pInputByte[input_index] >> 4U];
		output_index++;
		pOutputHex[output_index] = m_low_hex_array[pInputByte[input_index] & 0x0FU];
		output_index++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t httpd_digest_authentication::From_32Hex_To_16Bytes( const uint8_t pInputHex[32],
														   uint8_t pOutputByte[16U] )
{
	bool_t loop_break = false;
	bool_t return_val = true;
	uint8_t input_index = 0U;
	uint8_t temp8H = 0U;
	uint8_t temp8L = 0U;
	uint8_t input_hex = 0U;

	for ( uint8_t output_index = 0U; ( ( output_index < 16U ) && ( false == loop_break ) );
		  output_index++ )
	{
		input_hex = pInputHex[input_index];
		input_index++;
		if ( ( 0x30U <= input_hex ) && ( 0x39U >= input_hex ) )	/* 0..9 */
		{
			temp8H = input_hex - 0x30U;	/* 0 ASCII */
		}
		else if ( ( 0x61U <= input_hex ) && ( 0x66U >= input_hex ) )/* a..f ASCII */
		{
			temp8H = ( input_hex - 0x61U ) + 10U;	/* a.. ASCII */
		}
		else
		{
			return_val = false;
			loop_break = true;
		}
		if ( false == loop_break )
		{
			input_hex = pInputHex[input_index];
			input_index++;
			if ( ( 0x30U <= input_hex ) && ( 0x39U >= input_hex ) )	/* 0..9*/
			{
				temp8L = input_hex - 0x30U;
			}
			else if ( ( 0x61U <= input_hex ) && ( 0x66U >= input_hex ) )/* a..f*/
			{
				temp8L = ( input_hex - 0x61U ) + 10U;	/* a*/
			}
			else
			{
				return_val = false;
				loop_break = true;
			}
			if ( false == loop_break )
			{
				pOutputByte[output_index] = ( static_cast<uint8_t>( temp8H << 4U ) ) | temp8L;
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void httpd_digest_authentication::Generate_Opaque()
{
	uint8_t opaque[OPAQUE_LENGTH + OPAQUE_LENGTH] = {0};

	uC_Rand_Num::Get( opaque, OPAQUE_LENGTH );
	Convert_To_String( opaque, opaque, DCI_DTYPE_UINT32, 0U, false );
	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( httpd_digest_authentication::OPAQUE + 1 ), &opaque[2],
						 OPAQUE_LENGTH - 2 );

}
