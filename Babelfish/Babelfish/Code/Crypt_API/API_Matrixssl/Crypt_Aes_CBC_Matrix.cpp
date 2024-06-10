/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Crypt_Aes_CBC_Matrix.h"
#include "Ram.h"
#include "Stdlib_MV.h"
#include "Babelfish_Assert.h"

const uint8_t Crypt_Aes_CBC_Matrix::BLANK_ENC_STRING[BLANK_ENC_STRING_LEN] =
{ 0x01U, 0x02U, 0x03U, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt_Aes_CBC_Matrix::Crypt_Aes_CBC_Matrix( uint8_t const* key, aes_key_size_t key_size,
											uint8_t const* iv, aes_iv_size_t iv_size ) :
	Crypt(),
	m_key( key ),
	m_key_size( key_size ),
	m_iv( iv ),
	m_ctx( nullptr )
{
	if ( ( ( key_size == AES_128_KEY ) || ( key_size == AES_256_KEY ) ) &&
		 ( iv_size == AES_IV_SIZE_KEY ) )
	{
		m_ctx = reinterpret_cast<psAesCbc_t*>( Ram::Allocate(
												   static_cast<size_t>( sizeof( psAesCbc_t ) ) ) );
		BF_ASSERT( m_ctx != nullptr );
	}
	else
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt_Aes_CBC_Matrix::~Crypt_Aes_CBC_Matrix( void )
{
	if ( m_ctx != nullptr )
	{
		Ram::De_Allocate( m_ctx );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt::crypt_status_t Crypt_Aes_CBC_Matrix::Encrypt( uint8_t* input, uint32_t input_length,
													 uint8_t* output )
{
	crypt_status_t status = DATA_ERROR;

	if ( ( input_length > 0U ) && ( ( input_length & CRYPT_AES_CBC_BLOCK_LENGTH_MASK ) == 0U ) )
	{
		psAesInitCBC( m_ctx, m_iv, m_key, m_key_size, PS_AES_ENCRYPT );
		psAesEncryptCBC( m_ctx, input, output, input_length );

		status = SUCCESS;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt::crypt_status_t Crypt_Aes_CBC_Matrix::Decrypt( uint8_t* input, uint32_t input_length,
													 uint8_t* output )
{
	crypt_status_t status = DATA_ERROR;

	if ( ( input_length > 0 ) && ( ( input_length & CRYPT_AES_CBC_BLOCK_LENGTH_MASK ) == 0U ) )
	{
		psAesInitCBC( m_ctx, m_iv, m_key, m_key_size, PS_AES_DECRYPT );
		psAesDecryptCBC( m_ctx, input, output, input_length );

		status = SUCCESS;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt::crypt_status_t Crypt_Aes_CBC_Matrix::Encrypt( uint8_t* input, uint32_t input_length,
													 uint8_t* output,
													 uint32_t* output_length )
{
	crypt_status_t decrypt_success = crypt_status_t::DATA_ERROR;
	uint32_t min_output_len = MIN_U8_ENCODED_AES_CBC_STRING_SIZE( input_length );
	uint8_t* temp_u8_ptr = output;
	uint16_t checksum;

	// Is the encoding string length sufficient to hold the string from the source.
	if ( ( input_length > 0 ) && ( *output_length >= min_output_len ) )
	{
		// Align the string and get the checksum.
		checksum = BF_Lib::Smart_Copy_String_Ret_Checksum( temp_u8_ptr, input, input_length );

		// Fill in the empty space that is not taken up by the original string.
		// We fill it with a semi-known string.
		checksum += BF_Lib::Copy_String_Ret_Checksum( &temp_u8_ptr[input_length],
													  const_cast<uint8_t*>( BLANK_ENC_STRING ),
													  ( min_output_len - ( input_length
																		   +
																		   CRYPT_ENCODED_CHECKSUM_SIZE
																		   +
																		   CRYPT_ENCODED_LENGTH_STORAGE_SIZE ) ) );

		checksum += BF_Lib::Copy_String_Ret_Checksum(
			&temp_u8_ptr[min_output_len - CRYPT_ENCODED_CHECKSUM_SIZE -
						 CRYPT_ENCODED_LENGTH_STORAGE_SIZE],
			reinterpret_cast<uint8_t*>( &input_length ), CRYPT_ENCODED_LENGTH_STORAGE_SIZE );

		BF_Lib::Copy_String( &temp_u8_ptr[min_output_len - CRYPT_ENCODED_CHECKSUM_SIZE],
							 reinterpret_cast<uint8_t*>( &checksum ), CRYPT_ENCODED_CHECKSUM_SIZE );

		Encrypt( temp_u8_ptr, min_output_len, temp_u8_ptr );

		decrypt_success = SUCCESS;
	}

	return ( decrypt_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Crypt::crypt_status_t Crypt_Aes_CBC_Matrix::Decrypt( uint8_t* input, uint32_t input_length,
													 uint8_t* output,
													 uint32_t* output_length )
{
	Crypt::crypt_status_t status;
	uint8_t* temp_u8_ptr = input;
	uint16_t encoded_checksum = 0U;
	uint32_t embedded_length = 0U;
	uint_fast16_t j;

	status = Decrypt( input, input_length, temp_u8_ptr );

	if ( status == SUCCESS )
	{
		BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &encoded_checksum ),
							 &temp_u8_ptr[input_length - CRYPT_ENCODED_CHECKSUM_SIZE],
							 CRYPT_ENCODED_CHECKSUM_SIZE );

		j = input_length - CRYPT_ENCODED_CHECKSUM_SIZE;
		while ( j-- )
		{
			encoded_checksum -= temp_u8_ptr[j];
		}

		// Did the checksum work out correctly ?
		if ( encoded_checksum == 0U )
		{
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &embedded_length ),
								 &temp_u8_ptr[input_length - CRYPT_ENCODED_CHECKSUM_SIZE -
											  CRYPT_ENCODED_LENGTH_STORAGE_SIZE],
								 CRYPT_ENCODED_LENGTH_STORAGE_SIZE );

			// Does the target string have enough length to take the resulting string?
			// Does the embedded length fit within the expected range?
			// The range is determined by subtracting the frame overhead.
			if ( ( *output_length >= embedded_length ) &&
				 ( embedded_length <= ( input_length - CRYPT_ENCODED_LENGTH_STORAGE_SIZE -
										CRYPT_ENCODED_CHECKSUM_SIZE ) ) )
			{
				*output_length = embedded_length;
				BF_Lib::Smart_Copy_String( output, temp_u8_ptr, embedded_length );
			}
			else
			{
				status = DATA_ERROR;
			}
		}
		else
		{
			status = DATA_ERROR;
		}
	}

	return ( status );
}
