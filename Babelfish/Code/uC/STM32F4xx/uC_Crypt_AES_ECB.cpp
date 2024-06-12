/**
 *****************************************************************************************
 *	@file uC_Crypt_AES_ECB.cpp
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Crypt_AES_ECB.h"

/*
 *****************************************************************************************
 * See header file for function definitions
 *****************************************************************************************
 */
uC_Crypt_AES_ECB::Crypt::crypt_status_t uC_Crypt_AES_ECB::Encrypt( uint8_t* input, uint32_t input_length,
																   uint8_t* output, uint32_t* output_length )
{

	uint8_t* plain_data = input;
	uint16_t s = input_length;
	uint8_t* cypher_data = output;


	// doing checksum on output_length

	uint32_t min_output_len = MIN_U8_ENCODED_AES_ECB_STRING_SIZE( s );
	uint8_t* temp_u8_ptr = cypher_data;
	uint16_t checksum;

	// Is the encoding string length sufficient to hold the string from the source.
	if ( ( s > 0 ) && ( *output_length >= min_output_len ) )
	{
		// Align the string and get the checksum.
		checksum = BF_Lib::Smart_Copy_String_Ret_Checksum( temp_u8_ptr, plain_data, s );

		// Fill in the empty space that is not taken up by the original string.
		// We fill it with a semi-known string.
		checksum += BF_Lib::Copy_String_Ret_Checksum( &temp_u8_ptr[s],
													  const_cast<uint8_t*>( BLANK_ENC_STRING ),
													  ( min_output_len - ( s
																		   +
																		   CRYPT_ENCODED_CHECKSUM_SIZE
																		   +
																		   CRYPT_ENCODED_LENGTH_STORAGE_SIZE ) ) );

		checksum += BF_Lib::Copy_String_Ret_Checksum(
			&temp_u8_ptr[min_output_len - CRYPT_ENCODED_CHECKSUM_SIZE -
						 CRYPT_ENCODED_LENGTH_STORAGE_SIZE],
			reinterpret_cast<uint8_t*>( &s ), CRYPT_ENCODED_LENGTH_STORAGE_SIZE );

		BF_Lib::Copy_String( &temp_u8_ptr[min_output_len - CRYPT_ENCODED_CHECKSUM_SIZE],
							 reinterpret_cast<uint8_t*>( &checksum ), CRYPT_ENCODED_CHECKSUM_SIZE );

	}
	HAL_StatusTypeDef ret_val;

	ret_val = HAL_CRYP_AESECB_Encrypt( &m_hcryp, plain_data, min_output_length, cypher_data, m_timeout );

	switch ( ret_val )
	{
		case HAL_OK:
			return ( SUCCESS );

		case HAL_ERROR:
			return ( DATA_ERROR );

		case HAL_BUSY:
			return ( BUSY_ERROR );

		case HAL_TIMEOUT:
			return ( TIMEOUT_ERROR );

		default:
			return ( OPERATION_NOT_SUPPORTED );
	}

}

/*
 *****************************************************************************************
 * See header file for function definitions
 *****************************************************************************************
 */

uC_Crypt_AES_ECB::Crypt::crypt_status_t uC_Crypt_AES_ECB::Decrypt( uint8_t* input, uint32_t input_length,
																   uint8_t* output, uint32_t* output_length )
{
	uint8_t* cypher_data = input;
	uint16_t s = input_length;
	uint8_t* plain_data = output;
	HAL_StatusTypeDef ret_val;

	ret_val = HAL_CRYP_AESECB_Decrypt( &m_hcryp, cypher_data, s, plain_data, m_timeout );


	// calculations for output_length
	// Crypt::crypt_status_t status;
	// uint8_t* temp_u8_ptr = input;
	uint16_t encoded_checksum = 0U;
	uint32_t embedded_length = 0U;
	uint_fast16_t j;

	if ( ret_val == HAL_OK )
	{
		BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &encoded_checksum ),
							 &cypher_data[s - CRYPT_ENCODED_CHECKSUM_SIZE],
							 CRYPT_ENCODED_CHECKSUM_SIZE );

		j = s - CRYPT_ENCODED_CHECKSUM_SIZE;
		while ( j-- )
		{
			encoded_checksum -= cypher_data[j];
		}

		// Did the checksum work out correctly?
		if ( encoded_checksum == 0U )
		{
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &embedded_length ),
								 &cypher_data[input_length - CRYPT_ENCODED_CHECKSUM_SIZE -
											  CRYPT_ENCODED_LENGTH_STORAGE_SIZE],
								 CRYPT_ENCODED_LENGTH_STORAGE_SIZE );

			// Does the target string have enough length to take the resulting string?
			// Does the embedded length fit within the expected range ?
			// The range is determined by subtracting the frame overhead.
			if ( ( *output_length >= embedded_length ) &&
				 ( embedded_length <= ( s - CRYPT_ENCODED_LENGTH_STORAGE_SIZE -
										CRYPT_ENCODED_CHECKSUM_SIZE ) ) )
			{
				*output_length = embedded_length;
				BF_Lib::Smart_Copy_String( plain_data, cypher_data, embedded_length );
			}
			else
			{
				ret_val = HAL_ERROR;
			}
		}
		else
		{
			ret_val = HAL_ERROR;
		}
	}

	switch ( ret_val )
	{
		case HAL_OK:
			return ( SUCCESS );

		case HAL_ERROR:
			return ( DATA_ERROR );

		case HAL_BUSY:
			return ( BUSY_ERROR );

		case HAL_TIMEOUT:
			return ( TIMEOUT_ERROR );

		default:
			return ( OPERATION_NOT_SUPPORTED );

	}
}
