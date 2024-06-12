/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "XXTea.h"
#include "Stdlib_MV.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
const uint8_t XXTea::BLANK_ENC_STRING[BLANK_ENC_STRING_LEN] =
{ 0x01U, 0x02U, 0x03U };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool XXTea::Encrypt( uint32_t* data, uint32_t length, uint32_t const* key )
{
	bool decrypt_success = false;
	uint32_t y;
	uint32_t z;
	uint32_t round_ctr;		// Contained in the XXTEA_MX definition.
	uint32_t sum;
	uint32_t rounds;
	uint32_t e;

	if ( length > 1 )
	{
		rounds = 6U + ( 52U / length );
		sum = 0U;
		z = data[length - 1U];

		do
		{
			sum += XXTEA_DELTA;
			e = ( sum >> 2U ) & 3U;
			for ( round_ctr = 0U; round_ctr < ( length - 1U ); round_ctr++ )
			{
				y = data[round_ctr + 1U];
				z = data[round_ctr] += MX( e, y, z, sum, round_ctr, key );
			}
			y = data[0];
			z = data[length - 1U] += MX( e, y, z, sum, round_ctr, key );
		} while ( --rounds );

		decrypt_success = true;
	}

	return ( decrypt_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool XXTea::Decrypt( uint32_t* data, uint32_t length, uint32_t const* key )
{
	bool encrypt_success = false;
	uint32_t y;
	uint32_t z;
	uint32_t round_ctr;		// Contained in the XXTEA_MX definition.
	uint32_t sum;
	uint32_t rounds;
	uint32_t e;

	if ( length > 1 )
	{
		rounds = 6U + ( 52U / length );
		sum = rounds * XXTEA_DELTA;
		y = data[0];
		do
		{
			e = ( sum >> 2 ) & 3;
			for ( round_ctr = length - 1; round_ctr > 0U; round_ctr-- )
			{
				z = data[round_ctr - 1];
				y = data[round_ctr] -= MX( e, y, z, sum, round_ctr, key );
			}
			z = data[length - 1];
			y = data[0] -= MX( e, y, z, sum, round_ctr, key );
		} while ( ( sum -= XXTEA_DELTA ) != 0 );

		encrypt_success = true;
	}

	return ( encrypt_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool XXTea::Encrypt( uint8_t* src_data, uint32_t src_length,
					 uint8_t* encoded_data, uint32_t& encoded_length, uint32_t const* key )
{
	bool decrypt_success = false;
	uint32_t* temp_enc;
	uint8_t* temp_u8_ptr;
	uint32_t enc_word_length;
	uint16_t checksum;

	// Is the encoding string length sufficient to hold the string from the source.
	if ( ( src_length > 0 ) && ( encoded_length >= ( src_length + ENCRYPTED_U8_STRING_BUFF_OVERHEAD ) ) )
	{
		// This creates the alignment with the native memory arrangement.  So a uint32 processor
		// will be aligned on the 4byte boundary.  A uint16 processor will get aligned to 2byte boundaries.
		temp_u8_ptr = reinterpret_cast<uint8_t*>( ( reinterpret_cast<uint32_t>( encoded_data ) +
													~PROC_BASE_ADDRESS_MASK ) & PROC_BASE_ADDRESS_MASK );

		// We will add the original src string length to the encoded data.  With a little round up.
		enc_word_length = ( ( src_length + ENCODED_CHECKSUM_SIZE +
							  ENCODED_LENGTH_STORAGE_SIZE + ALIGNMENT_BYTE_ADJUSTMENT ) >> DIV_BY_4 );

		encoded_length = enc_word_length << MULT_BY_4;

		// Align the string and get the checksum.
		checksum = Smart_Copy_String_Ret_Checksum( temp_u8_ptr, src_data, src_length );

		// Fill in the empty space that is not taken up by the original string.
		// We fill it with a semi-known string.
		checksum += Copy_String_Ret_Checksum( &temp_u8_ptr[src_length],
											  const_cast<uint8_t*>( BLANK_ENC_STRING ),
											  ( encoded_length - src_length -
												ENCODED_CHECKSUM_SIZE - ENCODED_LENGTH_STORAGE_SIZE ) );

		checksum += Copy_String_Ret_Checksum(
			&temp_u8_ptr[encoded_length - ENCODED_CHECKSUM_SIZE - ENCODED_LENGTH_STORAGE_SIZE],
			reinterpret_cast<uint8_t*>( &src_length ), ENCODED_LENGTH_STORAGE_SIZE );

		BF_Lib::Copy_String( &temp_u8_ptr[encoded_length - ENCODED_CHECKSUM_SIZE],
							 reinterpret_cast<uint8_t*>( &checksum ), ENCODED_CHECKSUM_SIZE );

		temp_enc = reinterpret_cast<uint32_t*>( temp_u8_ptr );

		Encrypt( temp_enc, enc_word_length, key );

		// Now we need to move the string back into place if it was originally out of alignment.
		if ( ( ( uint_fast8_t )encoded_data & ~PROC_BASE_ADDRESS_MASK ) != 0 )
		{
			Smart_Copy_String( encoded_data, temp_u8_ptr, encoded_length );
		}

		decrypt_success = true;
	}

	return ( decrypt_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool XXTea::Decrypt( uint8_t* encoded_data, uint32_t encoded_length, uint32_t encoded_buff_size,
					 uint8_t* decoded_data, uint32_t& decoded_length, uint32_t const* key )
{
	bool decrypt_success = false;
	uint8_t* temp_u8_ptr;
	uint16_t encoded_checksum;
	uint16_t embedded_length;
	uint_fast16_t j;

	// The min length of the encrypted string is 8bytes (or two words).
	if ( ( encoded_length >= MIN_U8_ENCODED_STRING_SIZE ) &&
		 ( encoded_buff_size >= ( encoded_length + ALIGNMENT_BYTE_ADJUSTMENT ) ) )
	{
		temp_u8_ptr = ( uint8_t* )( ( reinterpret_cast<uint32_t>( encoded_data ) +
									  ~PROC_BASE_ADDRESS_MASK ) & PROC_BASE_ADDRESS_MASK );

		if ( ( ( uint_fast8_t )encoded_data & ~PROC_BASE_ADDRESS_MASK ) != 0 )
		{
			Smart_Copy_String( temp_u8_ptr, encoded_data, encoded_length );
		}
		Decrypt( reinterpret_cast<uint32_t*>( temp_u8_ptr ), encoded_length >> DIV_BY_4, key );

		BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &encoded_checksum ),
							 &temp_u8_ptr[encoded_length - ENCODED_CHECKSUM_SIZE], ENCODED_CHECKSUM_SIZE );

		j = encoded_length - ENCODED_CHECKSUM_SIZE;
		while ( j-- )
		{
			encoded_checksum -= temp_u8_ptr[j];
		}

		// Did the checksum work out correctly?
		if ( encoded_checksum == 0 )
		{
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &embedded_length ),
								 &temp_u8_ptr[encoded_length - ENCODED_CHECKSUM_SIZE - ENCODED_LENGTH_STORAGE_SIZE],
								 ENCODED_CHECKSUM_SIZE );

			// Does the target string have enough length to take the resulting string?
			// Does the embedded length fit within the expected range?
			// The range is determined by subtracting the frame overhead and alignment space.
			if ( ( decoded_length >= embedded_length ) &&
				 ( embedded_length <= ( encoded_length - ENCODED_LENGTH_STORAGE_SIZE -
										ENCODED_CHECKSUM_SIZE ) ) )
			{
				decrypt_success = true;
				decoded_length = embedded_length;

				Smart_Copy_String( decoded_data, temp_u8_ptr, decoded_length );
			}
		}
	}

	return ( decrypt_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool XXTea::Run_Self_Test( void )
{
	bool success = true;
	uint32_t test_u32_string[PRE_ENC_BLOCK_WLEN];
	uint8_t string_u8[TEST_BLOCK_U8_LENGTH];
	uint8_t decoded_string_u8[TEST_BLOCK_U8_LENGTH];
	uint8_t encoded_u8_string[TEST_BLOCK_U8_LENGTH + ENCRYPTED_U8_STRING_BUFF_OVERHEAD];
	uint32_t encoded_len;
	uint32_t decrypted_len;
	uint32_t string_u8_length;
	uint8_t test_number = 0U;
	uint8_t string_offset;
	uint32_t ram_key[KEY_LENGTH];

	// First see if our encrypted string turns into the correct encrypted output.
	Smart_Copy_String( ( uint8_t* )test_u32_string, ( uint8_t* )TEST_BLOCK, PRE_ENC_BLOCK_WLEN << MULT_BY_4 );
	Encrypt( test_u32_string, PRE_ENC_BLOCK_WLEN, TEST_BLOCK_KEY );
	if ( !String_Compare( ( uint8_t* )test_u32_string, ( uint8_t* )TEST_BLOCK_RESULT,
						  PRE_ENC_BLOCK_WLEN << MULT_BY_4 ) )
	{
		success = false;
	}

	// Verify it comes back out again.
	Decrypt( test_u32_string, PRE_ENC_BLOCK_WLEN, ( uint32_t* )TEST_BLOCK_KEY );
	if ( !String_Compare( ( uint8_t* )test_u32_string, ( uint8_t* )TEST_BLOCK, PRE_ENC_BLOCK_WLEN << MULT_BY_4 ) )
	{
		success = false;
	}



	// Verify the length checking is working.  -> Source length is 0 and target is 0.
	test_number = 0U;
	Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
	string_u8_length = 0U;
	encoded_len = 0U;
	if ( Encrypt( string_u8, string_u8_length, encoded_u8_string, encoded_len, TEST_BLOCK_KEY ) == true )
	{
		success = false;
	}
	test_number++;
	Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
	string_u8_length = 0U;
	encoded_len = 0U;
	if ( Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
				  string_u8, string_u8_length, TEST_BLOCK_KEY ) == true )
	{
		success = false;
	}

	// Verify various lengths of uint8 data.
	uint8_t test_len = 1;

	while ( ( test_len <= TEST_BLOCK_U8_LENGTH ) && ( success == true ) )
	{
		// Verify the length checking is working.  -> target string too small to include length;
		test_number++;
		Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
		string_u8_length = test_len;
		// Make room for string_u8_length then subtract off one byte so it is not big enough.
		encoded_len = ( string_u8_length + ENCRYPTED_U8_STRING_BUFF_OVERHEAD ) - 1;
		if ( Encrypt( string_u8, string_u8_length, encoded_u8_string, encoded_len, TEST_BLOCK_KEY ) == true )
		{
			success = false;
		}

		// Verify the length checking is working.  -> target string too small to include length on a decrypt.
		test_number++;
		Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
		string_u8_length = test_len;
		encoded_len = sizeof( encoded_u8_string );
		if ( Encrypt( string_u8, string_u8_length, encoded_u8_string, encoded_len, TEST_BLOCK_KEY ) == false )
		{
			success = false;
		}
		// Make room for string_u8_length then subtract off one byte so it is not big enough.
		if ( Decrypt( encoded_u8_string, encoded_len,
					  ( ( string_u8_length + ALIGNMENT_BYTE_ADJUSTMENT ) - 1 ),
					  string_u8, string_u8_length, TEST_BLOCK_KEY ) == true )
		{
			success = false;
		}

		if ( success == true )
		{
			test_number++;
			Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
			encoded_len = sizeof( encoded_u8_string );
			success = Encrypt( string_u8, test_len,
							   encoded_u8_string, encoded_len, TEST_BLOCK_KEY );
			if ( success == true )
			{
				decrypted_len = test_len;
				success = Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
								   decoded_string_u8, decrypted_len, TEST_BLOCK_KEY );
				if ( ( decrypted_len != test_len ) ||
					 !String_Compare( decoded_string_u8, string_u8, decrypted_len ) )
				{
					success = false;
				}
			}
		}
		if ( success == true )
		{
			string_offset = 0U;
			while ( ( string_offset < 3U ) && ( success == true ) )
			{
				test_number++;
				Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
				encoded_len = sizeof( encoded_u8_string );
				success = Encrypt( string_u8, test_len,
								   &encoded_u8_string[string_offset], encoded_len, TEST_BLOCK_KEY );
				if ( success == true )
				{
					decrypted_len = test_len;
					success = Decrypt( &encoded_u8_string[string_offset], encoded_len,
									   sizeof( encoded_u8_string ), decoded_string_u8,
									   decrypted_len, TEST_BLOCK_KEY );
					if ( ( decrypted_len != test_len ) ||
						 !String_Compare( decoded_string_u8, string_u8, decrypted_len ) )
					{
						success = false;
					}
				}
				string_offset++;
			}
		}

		if ( success == true )
		{
			string_offset = 0U;
			while ( ( string_offset < 3U ) && ( success == true ) )
			{
				test_number++;
				Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
				encoded_len = sizeof( encoded_u8_string );
				success = Encrypt( string_u8, test_len,
								   encoded_u8_string, encoded_len, TEST_BLOCK_KEY );
				if ( success == true )
				{
					decrypted_len = test_len;
					success = Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
									   &decoded_string_u8[string_offset], decrypted_len, TEST_BLOCK_KEY );
					if ( ( decrypted_len != test_len ) ||
						 !String_Compare( &decoded_string_u8[string_offset],
										  string_u8, decrypted_len ) )
					{
						success = false;
					}
				}
				string_offset++;
			}
		}

		if ( success == true )
		{
			test_number++;
			Copy_Val_To_String( string_u8, test_number, sizeof( string_u8 ) );
			encoded_len = sizeof( encoded_u8_string );
			success = Encrypt( string_u8, test_len,
							   encoded_u8_string, encoded_len, TEST_BLOCK_KEY );
			if ( success == true )
			{
				for ( uint8_t key_index = 0U; key_index < KEY_LENGTH; key_index++ )
				{
					ram_key[key_index] = TEST_BLOCK_KEY[key_index];
				}

				ram_key[0]++;	// break the key.
				decrypted_len = test_len;
				if ( Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
							  decoded_string_u8, decrypted_len,
							  const_cast<uint32_t*>( ram_key ) ) == true )
				{
					success = false;
				}

				for ( uint8_t key_index = 0U; key_index < KEY_LENGTH; key_index++ )
				{
					ram_key[key_index] = TEST_BLOCK_KEY[key_index];
				}
				ram_key[1]++;	// break the key.
				decrypted_len = test_len;
				if ( Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
							  decoded_string_u8, decrypted_len,
							  const_cast<uint32_t*>( ram_key ) ) == true )
				{
					success = false;
				}
				for ( uint8_t key_index = 0U; key_index < KEY_LENGTH; key_index++ )
				{
					ram_key[key_index] = TEST_BLOCK_KEY[key_index];
				}
				ram_key[2]++;	// break the key.
				decrypted_len = test_len;
				if ( Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
							  decoded_string_u8, decrypted_len,
							  const_cast<uint32_t*>( ram_key ) ) == true )
				{
					success = false;
				}

				for ( uint8_t key_index = 0U; key_index < KEY_LENGTH; key_index++ )
				{
					ram_key[key_index] = TEST_BLOCK_KEY[key_index];
				}
				ram_key[3]++;	// break the key.
				decrypted_len = test_len;
				if ( Decrypt( encoded_u8_string, encoded_len, sizeof( encoded_u8_string ),
							  decoded_string_u8, decrypted_len,
							  const_cast<uint32_t*>( ram_key ) ) == true )
				{
					success = false;
				}
			}
		}

		test_len++;
	}

	return ( success );
}
