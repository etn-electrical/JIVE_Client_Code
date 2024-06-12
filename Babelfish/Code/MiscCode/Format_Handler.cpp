/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Format_Handler.h"
#include "Data_Align.hpp"
#include "Ram.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_MAC_ADDRESS( void* data, const uint16_t data_buf_size, uint8_t* string,
									 const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	uint8_t* mac_addr = reinterpret_cast<uint8_t*>( data );
	char_t* f_string = reinterpret_cast<char_t*>( string );
	uint16_t byte_count = 0;
	bool loop_break = false;

	// Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		if ( dir == RAW_TO_STRING )
		{
			// Raw to Formatted string conversion required.
			*f_string = '\0';
			uint16_t str_len = 0;
			do
			{
				// loop operation will help to build formatted string when data parameter is array type.
				snprintf( reinterpret_cast<char_t*>( f_string + str_len ), str_buf_size - str_len,
						  "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
						  mac_addr[byte_count + 5U], mac_addr[byte_count + 4U], mac_addr[byte_count + 3U],
						  mac_addr[byte_count + 2U], mac_addr[byte_count + 1U], mac_addr[byte_count] );

				byte_count += 6U;
				str_len = strnlen( f_string, str_buf_size );

				// Raw data buffer and Formatted data buffer overflow check
				if ( ( str_buf_size <= str_len ) || ( data_buf_size < byte_count ) )
				{
					*f_string = '\0';
					byte_count = 0;
					loop_break = true;
				}
				else if ( byte_count < data_buf_size )
				{
					// Attach delimiter for array type dcid
					snprintf( reinterpret_cast<char_t*>( f_string + str_len ), sizeof( ARRAY_DELIMITER ),
							  ARRAY_DELIMITER );
					str_len += strlen( ARRAY_DELIMITER );
				}
				else
				{
					loop_break = true;
				}

			}while( loop_break == false );
		}
		else
		{
			// Formatted string to Raw conversion required
			const uint16_t STRING_LEN = strnlen( f_string, str_buf_size );
			if ( str_buf_size <= STRING_LEN )
			{
				/* Null character not found */
				byte_count = 0;
			}
			else
			{
				// uint8_t mac_addr_temp[data_buf_size] = {0};
				uint8_t* mac_addr_temp = reinterpret_cast<uint8_t*>( Ram::Allocate( data_buf_size ) );
				uint16_t bytes_received = Parse_Formatted_String( f_string, Validate_And_Parse_Mac_Address,
																  mac_addr_temp );
				if ( bytes_received > 0 )
				{
					BF_Lib::Copy_Data( mac_addr, data_buf_size, mac_addr_temp, bytes_received );
					byte_count = bytes_received;
				}
				Ram::De_Allocate( mac_addr_temp );
			}
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_IPV4_BIG_ENDIAN( void* data, const uint16_t data_buf_size, uint8_t* string,
										 const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	return ( FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8( data, data_buf_size, string, str_buf_size, dir, config ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_WAIVER( void* data, const uint16_t data_buf_size, uint8_t* string,
								const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	// This format type will not have array elements. It will be always single value.
	uint8_t* waiver_data = reinterpret_cast<uint8_t*>( data );
	// char_t* f_string = reinterpret_cast<char_t*>( string );
	uint16_t byte_count = 0;

	// Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		if ( dir == RAW_TO_STRING )
		{
			// Raw to Formatted string conversion required.
			*string = '\0';

			// loop operation will help to build formatted string when data parameter is array type.
			snprintf( reinterpret_cast<char_t*>( string ), str_buf_size,
					  "%u", *waiver_data );
			// Indicates that we have read one byte raw data.
			byte_count += 1U;
		}
		else
		{
			uint32_t val = 0;
			sscanf( reinterpret_cast<const char_t*>( string ), "%u", &val );
			if ( val <= MAX_8BIT_NUMBER )
			{
				*waiver_data = static_cast<uint8_t>( val );
				// Indicates that we have written one byte raw data.
				byte_count += 1U;
			}
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_HOST_ADDRESS( void* data, const uint16_t data_buf_size, uint8_t* string,
									  const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	// This format type will have a string buffer (i.e) it can hold IP address or Host Address
	uint8_t* dci_data = reinterpret_cast<uint8_t*>( data );
	uint8_t* user_entry_data = reinterpret_cast<uint8_t*>( string );
	uint16_t byte_count = 0;
	uint16_t bytes_received = 0;

	///< Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		///< Just copy from dci and respond back the get request
		if ( dir == RAW_TO_STRING )
		{
			bytes_received = BF_Lib::Get_String_Length( dci_data, data_buf_size );
			if ( bytes_received > 0 )
			{
				BF_Lib::Copy_Data( user_entry_data, data_buf_size, dci_data, ( bytes_received ) );
				byte_count = bytes_received;
			}
		}
		else
		{
			///< Just copy from user entry to dci for the Put request
			bytes_received = BF_Lib::Get_String_Length( user_entry_data, data_buf_size );
			if ( bytes_received > 0 )
			{
				BF_Lib::Copy_Data( dci_data, data_buf_size, user_entry_data, ( bytes_received ) );
				byte_count = bytes_received;
			}
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8( void* data, const uint16_t data_buf_size, uint8_t* string,
											const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	uint8_t* ip_addr = reinterpret_cast<uint8_t*>( data );
	char_t* f_string = reinterpret_cast<char_t*>( string );
	uint16_t byte_count = 0;
	bool loop_break = false;

	// Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		if ( dir == RAW_TO_STRING )
		{
			// Raw to Formatted string conversion required.
			*f_string = '\0';
			uint16_t str_len = 0;
			do
			{
				// loop operation will help to build formatted string when more than one ip address in data buffer.
				snprintf( reinterpret_cast<char_t*>( f_string + str_len ), str_buf_size - str_len,
						  "%u.%u.%u.%u", ip_addr[byte_count + 3U], ip_addr[byte_count + 2U],
						  ip_addr[byte_count + 1U], ip_addr[byte_count] );

				byte_count += 4U;
				str_len = strnlen( f_string, str_buf_size );

				// Raw data buffer and Formatted data buffer overflow check
				if ( ( str_buf_size <= str_len ) || ( data_buf_size < byte_count ) )
				{
					*f_string = '\0';
					byte_count = 0;
					loop_break = true;
				}
				else if ( byte_count < data_buf_size )
				{
					// Attach delimiter for array type dcid
					snprintf( reinterpret_cast<char_t*>( f_string + str_len ), sizeof( ARRAY_DELIMITER ),
							  ARRAY_DELIMITER );
					str_len += strlen( ARRAY_DELIMITER );
				}
				else
				{
					loop_break = true;
				}

			}while( loop_break == false );
		}
		else
		{
			const uint16_t STRING_LEN = strnlen( f_string, str_buf_size );
			if ( str_buf_size <= STRING_LEN )
			{
				/* Null character not found */
				byte_count = 0;
			}
			else
			{
				uint8_t* ip_addr_temp = reinterpret_cast<uint8_t*>( Ram::Allocate( data_buf_size ) );
				uint16_t bytes_received = Parse_Formatted_String( f_string, Validate_And_Parse_IPv4_Address,
																  ip_addr_temp );
				if ( bytes_received > 0 )
				{
					BF_Lib::Copy_Data( ip_addr, data_buf_size, ip_addr_temp, bytes_received );
					byte_count = bytes_received;
				}
				Ram::De_Allocate( ip_addr_temp );
			}
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_FW_VERSION( void* data, const uint16_t data_buf_size, uint8_t* string,
									const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	uint8_t* dci_data = reinterpret_cast<uint8_t*>( data );
	uint8_t* user_entry_data = reinterpret_cast<uint8_t*>( string );
	uint16_t byte_count = 0;
	uint16_t bytes_received = 0;

	///< Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		///< Just copy from dci and respond back the get request
		if ( dir == RAW_TO_STRING )
		{
			bytes_received = BF_Lib::Get_String_Length( dci_data, data_buf_size );
			if ( bytes_received > 0 )
			{
				uint32_t firm_rev_num = dci_data[0] << 24 | dci_data[1] << 16 | dci_data[3] << 8 | dci_data[2];
				Convert_Rev_Num_To_Ascii( firm_rev_num, ( uint8_t* )user_entry_data );
				byte_count = bytes_received;
			}
		}
		else
		{
			byte_count = 0;	// Don't need
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Parse_Formatted_String( char_t* str, format_validation_handle_t format_validation_handle, void* raw_data )
{
	bool status = false;
	bool loop_break = false;
	char_t* current_element = str;
	uint8_t* raw_data_t = reinterpret_cast<uint8_t*>( raw_data );
	uint16_t bytes_received = 0;
	uint16_t bytes_temp = 0;

	if ( ( str != nullptr ) && ( raw_data != nullptr ) && ( format_validation_handle != nullptr ) )
	{
		while ( loop_break == false )
		{
			char_t* element_end_addr = strchr( current_element, ARRAY_DELIMITER_CHAR );

			// Strtok_Search() will replace token with null character.
			char_t* element_addr = Strtok_Search( current_element, ARRAY_DELIMITER );

			bytes_temp = format_validation_handle( current_element, raw_data_t );
			if ( bytes_temp > 0 )
			{
				bytes_received += bytes_temp;
				raw_data_t += bytes_temp;
				status = true;
			}
			else
			{
				status = false;
				loop_break = true;	// > Validation Failed
			}

			if ( element_addr == nullptr )	// > Last Element found
			{
				loop_break = true;	// > Exit since last element
			}
			else	// > Next Array Element found.
			{
				// First validate current element then point to next element.
				current_element = element_end_addr + 1U;	// > Point to next array element
			}
		}
	}
	if ( status == false )
	{
		bytes_received = 0;
	}

	return ( bytes_received );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Validate_And_Parse_IPv4_Address( char_t* ip_add, void* raw_data )
{
	uint16_t num;
	bool status = false;
	bool loop_break = false;
	bool token_status = true;
	char_t* octet_bk = nullptr;
	uint8_t received_octet = 0;
	const uint8_t MAX_IP_OCTET = 4U;
	uint8_t* raw_ip_data = reinterpret_cast<uint8_t*>( raw_data );

	if ( ( ip_add != nullptr ) && ( raw_data != nullptr ) )
	{
		char_t* cur_ip_addr = reinterpret_cast<char_t*>( ip_add );

		// Read first ip octet from ip address string
		// e.g. Read "192" from "192.168.1.254"
		char_t* octet = Strtok_Search( cur_ip_addr, IP_DELIMITER );

		while ( loop_break == false )
		{
			if ( ( octet != nullptr ) ||								// > Next octet present
				 ( ( octet == nullptr ) && ( received_octet > 0 ) ) )	// > Last octet
			{
				if ( octet == nullptr )
				{
					// We are calculating address of next octet since previous strtok()
					// delimiter search failed due to last tocken entry.
					token_status = false;
					octet = octet_bk + strlen( octet_bk );
					octet++;
				}
				else
				{
					octet_bk = octet;
				}

				num = strtol( octet, nullptr, 10U );			// Covert into decimal number
				if ( ( Is_Decimal_String( octet ) == true ) &&	// Octet should have only digits
					 ( num <= MAX_8BIT_NUMBER ) )				// Octet value should be less than 256
				{
					received_octet++;

					if ( ( ( received_octet < MAX_IP_OCTET ) && ( token_status == true ) ) ||	// Next tocken avaialble
						 ( ( received_octet == MAX_IP_OCTET ) && ( token_status == false ) ) )	// Last tocken
					{
						raw_ip_data[MAX_IP_OCTET - received_octet] = num;

						if ( received_octet == MAX_IP_OCTET )
						{
							// Success: Ip data parsed successfully
							loop_break = true;
							status = true;
						}
						else
						{
							octet = Strtok_Search( nullptr, IP_DELIMITER );
						}
					}
					else
					{
						// Error: Less than or More than 4 octet received
						loop_break = true;
					}
				}
				else
				{
					// Error: Incorrect ip octet value. Valid range 0 to 255.
					loop_break = true;
				}
			}
			else
			{
				// Error: IP Delimiter not found
				loop_break = true;
			}
		}
	}
	if ( status == false )
	{
		received_octet = 0;
	}

	return ( received_octet );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Validate_And_Parse_Mac_Address( char_t* mac_add, void* raw_data )
{
	uint16_t num;
	bool status = false;
	bool loop_break = false;
	bool token_status = true;
	char_t* octet_bk = nullptr;
	uint8_t received_octet = 0;
	const uint8_t MAX_MAC_OCTET = 6U;
	uint8_t* raw_mac_data = reinterpret_cast<uint8_t*>( raw_data );

	if ( ( mac_add != nullptr ) && ( raw_data != nullptr ) )
	{
		char_t* cur_mac_addr = reinterpret_cast<char_t*>( mac_add );

		// Read first mac octet from mac address string
		// e.g. Read "00" from "00:D0:AF:05:04:6B"
		char_t* octet = Strtok_Search( cur_mac_addr, MAC_DELIMITER );

		while ( loop_break == false )
		{
			if ( ( octet != nullptr ) ||								// > Next octet present
				 ( ( octet == nullptr ) && ( received_octet > 0 ) ) )	// > Last octet
			{
				if ( octet == nullptr )
				{
					// We are calculating address of next octet since previous strtok()
					// delimiter search failed due to last tocken entry
					token_status = false;
					octet = octet_bk + strlen( octet_bk );
					octet++;
				}
				else
				{
					octet_bk = octet;
				}

				num = strtol( octet, nullptr, 16U );			// Covert into decimal number
				if ( ( Is_Hex_String( octet ) == true ) &&	// Octet should have only digits
					 ( num <= MAX_8BIT_NUMBER ) )				// Octet value should be less than 256
				{
					received_octet++;

					if ( ( ( received_octet < MAX_MAC_OCTET ) && ( token_status == true ) ) ||	// Next tocken avaialble
						 ( ( received_octet == MAX_MAC_OCTET ) && ( token_status == false ) ) )	// Last tocken
					{
						raw_mac_data[MAX_MAC_OCTET - received_octet] = num;

						if ( received_octet == MAX_MAC_OCTET )
						{
							// Success: Mac data parsed successfully
							loop_break = true;
							status = true;
						}
						else
						{
							octet = Strtok_Search( nullptr, MAC_DELIMITER );
						}
					}
					else
					{
						// Error: Less than or More than 6 octet received
						loop_break = true;
					}
				}
				else
				{
					// Error: Incorrect Mac octet value. Valid range 0 to 255
					loop_break = true;
				}
			}
			else
			{
				// Error: MAC Delimiter not found
				loop_break = true;
			}
		}
	}
	if ( status == false )
	{
		received_octet = 0;
	}

	return ( received_octet );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
char_t* Strtok_Search( char_t* element, const char_t* delimiter )
{
	static char_t* s_token = nullptr;
	char_t* result = nullptr;
	uint16_t i = 0;

	if ( delimiter != nullptr )
	{
		if ( element != nullptr )
		{
			s_token = element;
		}

		if ( s_token != nullptr )
		{
			while ( s_token[i] != '\0' )
			{
				if ( *delimiter == s_token[i] )
				{
					result = s_token;		// > Return token
					s_token[i] = '\0';		// > Write NULL character at delimiter location
					s_token += ( i + 1 );	// > Point to next token
					break;
				}
				i++;
			}
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_CRED( void* data, const uint16_t data_buf_size, uint8_t* string,
							  const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	// As per format handler design, Each format type must have respective format hander.
	// In Babelfish, We have handled credential (CRED) type formats internally.
	// This CRED handler will not get called anytime but defination required to code get compiled.
	return ( 1U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( void* data, const uint16_t data_buf_size, uint8_t* string,
											  const uint16_t str_buf_size, conv_type_t dir, void* config )
{
	uint8_t* arr_addr = reinterpret_cast<uint8_t*>( data );
	char_t* f_string = reinterpret_cast<char_t*>( string );
	uint16_t byte_count = 0;
	bool loop_break = false;

	// Formatted string buffer should be greater than actual data buffer.
	if ( str_buf_size > data_buf_size )
	{
		if ( dir == RAW_TO_STRING )
		{
			// Raw to Formatted string conversion required.
			*f_string = '\0';
			uint16_t str_len = 0;
			do
			{
				// loop operation will help to build formatted string when data parameter is array type.
				snprintf( reinterpret_cast<char_t*>( f_string + str_len ), str_buf_size - str_len,
						  "%2.2X", arr_addr[byte_count] );

				byte_count += 1U;
				str_len = strnlen( f_string, str_buf_size );

				// Raw data buffer and Formatted data buffer overflow check
				if ( ( str_buf_size <= str_len ) || ( data_buf_size < byte_count ) )
				{
					*f_string = '\0';
					byte_count = 0;
					loop_break = true;
				}
				else if ( byte_count < data_buf_size )
				{
					// Attach delimiter for array type dcid
					snprintf( reinterpret_cast<char_t*>( f_string + str_len ), sizeof( ARR_TO_STR_DELIMITER ),
							  ARR_TO_STR_DELIMITER );
					str_len += strlen( ARR_TO_STR_DELIMITER );
				}
				else
				{
					loop_break = true;
				}

			}while( loop_break == false );
		}
		else
		{
			// Formatted string to Raw conversion required
			const uint16_t STRING_LEN = strnlen( f_string, str_buf_size );
			if ( str_buf_size <= STRING_LEN )
			{
				/* Null character not found */
				byte_count = 0;
			}
			else
			{
				// Formatted string to Raw conversion required not currently implemented
				// uint8_t addr_temp[data_buf_size] = {0};
				uint8_t* addr_temp = reinterpret_cast<uint8_t*>( Ram::Allocate( data_buf_size ) );
				uint16_t bytes_received = Parse_Formatted_String( f_string, nullptr,
																  addr_temp );
				if ( bytes_received > 0 )
				{
					BF_Lib::Copy_Data( arr_addr, data_buf_size, addr_temp, bytes_received );
					byte_count = bytes_received;
				}
				Ram::De_Allocate( addr_temp );
			}
		}
	}
	else
	{
		byte_count = 0;
	}
	return ( byte_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Convert_Rev_Num_To_Ascii( uint32_t firm_rev, uint8_t firm_rev_ascii[] )
{
	uint16_t firm_rev_num[3] = {0U};
	uint16_t tmp8 = 0U;
	uint16_t rev = 0U;
	uint8_t j = 0U;
	uint8_t z = 0U;
	uint8_t i = 0U;
	uint8_t no_of_elements_in_version = 3U;

	firm_rev_num[0] = firm_rev >> 24;
	firm_rev_num[1] = ( firm_rev & 0x00FF0000U ) >> 16;
	firm_rev_num[2] = firm_rev;
	while ( i < no_of_elements_in_version )
	{
		uint8_t ascii_temp[5] =
		{ 0 };
		rev = firm_rev_num[i];
		tmp8 = 0U;
		i++;
		do
		{
			tmp8 = ( rev ) / ( 10U );
			ascii_temp[z] = ( rev + 0x30U ) - ( tmp8 * 10U );
			z++;
			rev = tmp8;
		} while ( rev != 0U );

		while ( z > 0U )
		{
			firm_rev_ascii[j] = ( ascii_temp[z - 1] | 0x30U );
			z--;
			j++;
		}

		if ( i < no_of_elements_in_version )
		{
			firm_rev_ascii[j] = 0x2EU;	// '.'
			j++;
		}
	}
	firm_rev_ascii[j] = 0x00U;	// '\0'
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Convert_Rev_Ascii_To_Num( char* firm_rev_ascii, fw_file_version* fw_ver )
{

	uint16_t firm_rev_num[3] = { 0U };
	uint8_t dot_index[4];
	uint8_t j = 1U;
	uint8_t i = 0U;
	uint8_t firm_rev_ascii_size = strlen( firm_rev_ascii );

	/* Conversion function implemented as per version format eg. 255.255.65535 */
	dot_index[0] = 0;
	for ( uint32_t i = 0U; ( i < firm_rev_ascii_size ) && ( j < 3U ); i++ )
	{
		if ( firm_rev_ascii[i] == '.' )
		{
			dot_index[j++] = i;
		}
	}
	dot_index[j] = firm_rev_ascii_size;

	i = 0U;
	{
		if ( ERR_PARSING_OK ==
			 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )&firm_rev_ascii[0],
												  dot_index[i],
												  dot_index[i + 1] - 1U,
												  &firm_rev_num[i], true ) )
		{}
	}
	i++;
	for (; i < j; i++ )
	{
		if ( ERR_PARSING_OK ==
			 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )&firm_rev_ascii[0],
												  dot_index[i] + 1,
												  dot_index[i + 1] - 1,
												  &firm_rev_num[i], true ) )
		{}
	}
	fw_ver->major = firm_rev_num[0];
	fw_ver->minor = firm_rev_num[1];
	fw_ver->build = firm_rev_num[2];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Convert_Rev_Ascii_To_HEX( char_t* firm_rev_ascii )
{
	uint16_t firm_rev_num[3] = {0U};
	uint8_t dot_index[4];
	uint8_t j = 1U;
	uint8_t i = 0U;
	uint32_t version = 0U;
	uint32_t firm_rev = 0U;
	uint8_t firm_rev_ascii_size = strlen( firm_rev_ascii );

	/* Conversion function implemented as per version format eg. 255.255.65535 */
	dot_index[0] = 0;
	for ( uint32_t i = 0U; ( i < firm_rev_ascii_size ) && ( j < 3U ); i++ )
	{
		if ( firm_rev_ascii[i] == '.' )
		{
			dot_index[j++] = i;
		}
	}
	dot_index[j] = firm_rev_ascii_size;
	i = 0U;
	{
		if ( ERR_PARSING_OK ==
			 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )&firm_rev_ascii[0],
												  dot_index[i],
												  dot_index[i + 1] - 1U,
												  &firm_rev_num[i], true ) )
		{}
	}
	i++;
	for (; i < j; i++ )
	{
		if ( ERR_PARSING_OK ==
			 INT_ASCII_Conversion::str_to_uint16( ( const uint8_t* )&firm_rev_ascii[0],
												  dot_index[i] + 1,
												  dot_index[i + 1] - 1,
												  &firm_rev_num[i], true ) )
		{}
	}
	version = firm_rev_num[0] << 24;
	firm_rev = firm_rev_num[1] << 16;
	version = version | ( firm_rev & 0x00FF0000U );
	version = version | firm_rev_num[2];
	return ( version );
}
