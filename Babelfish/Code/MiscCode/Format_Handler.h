/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module is responsible for data coversion fron DCI raw data to formated data.vice versa.
 *
 *	@details DCI module will contain raw data which has minimum memory foor print. Other modules will covert
 *	 raw data to formatted data using this module.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FORMAT_HANDLER_H
#define FORMAT_HANDLER_H

#include "includes.h"
#include "DCI_Constants.h"
#include "INT_ASCII_Conversion.h"

#define REST_SERVER_FORMAT_HANDLING
/**
 * @brief Check if input string contains only decimal numbers e.g. '0' to '9'
 * @param[in] str: Input String
 * @return 	Status
 */
inline bool Is_Decimal_String( char_t* str )
{
	bool status = true;

	while ( *str )
	{
		if ( ( *str < '0' ) || ( *str > '9' ) )
		{
			/// Break loop since error condition
			status = false;
			break;
		}
		str++;
	}
	return ( status );
}

/**
 * @brief Check if input string contains only hex numbers e.g. '0' to 'F'
 * @param[in] str: Input String
 * @return 	Status
 */
inline bool Is_Hex_String( char_t* str )
{
	bool status = false;

	while ( *str )
	{
		if ( ( ( *str >= '0' ) && ( *str <= '9' ) ) ||
			 ( ( *str >= 'A' ) && ( *str <= 'F' ) ) )
		{
			status = true;
		}
		else if ( ( *str >= 'a' ) && ( *str <= 'f' ) )
		{
			// Lower to upper case conversion
			*str -= 0x20;
			status = true;
		}
		else
		{
			// Error: Other than hex number found
			status = false;
			break;
		}
		str++;
	}
	return ( status );
}

/**
 * @brief Strtok_Search simulates strtok standard api. This function is added since IAR supported strtok()
 * 		 is not working as per standard. It returns base address of input string in following condition:
 *		 1. When delimiter not found ( Not following standard )
 *		 2. When first token found  ( As per standard )
 *		 Note:
 *		 1. When delimiter found then we will replace delimiter with '\0' null character.
 *		 2. When "element" is passed as nullptr then this function starts parsing from old address(token) if any.
 *			Strtok_Search( nullptr, ',' );
 * @param[in] element: Input string
 * @param[in] delimiter: Work as string separator
 * @return 	Returns
 *			1. nullptr : When delimiter not found
 *			2. address : When delimiter found
 */
char_t* Strtok_Search( char_t* element, const char_t* delimiter );

typedef uint8_t ( * format_validation_handle_t ) ( char_t*, void* );

/* Delimiter used to separate values of array element */
#define ARRAY_DELIMITER ","
#define ARRAY_DELIMITER_CHAR ','
#define IP_DELIMITER "."
#define IP_DELIMITER_CHAR '.'
#define MAC_DELIMITER ":"
#define MAC_DELIMITER_CHAR ':'
#define MAX_8BIT_NUMBER 255
#define ARR_TO_STR_DELIMITER "-"

enum conv_type_t
{
	RAW_TO_STRING,	/// > Conversion Raw data to String
	STRING_TO_RAW	/// > Conversion String to Raw data
};

/**
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
typedef uint16_t (formatter_callback_t)( void* data, const uint16_t data_buf_size, uint8_t* string,
										 const uint16_t str_buf_size, conv_type_t dir, void* config );

struct formatter_struct_t
{
	const char_t* format_name;	/// > format name
#ifdef REST_SERVER_FORMAT_HANDLING
	formatter_callback_t* formatter_cback;	/// > format handler
	void* config_struct;	/// > configuration parameters required for conversion
#endif
};

/**
 * @brief Handler for MAC_ADDRESS()
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_MAC_ADDRESS( void* data, const uint16_t data_buf_size, uint8_t* string,
									 const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for IPV4_BIG_ENDIAN()
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_IPV4_BIG_ENDIAN( void* data, const uint16_t data_buf_size, uint8_t* string,
										 const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for IPV4_BIG_ENDIAN_U8()
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_IPV4_BIG_ENDIAN_U8( void* data, const uint16_t data_buf_size, uint8_t* string,
											const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for WAIVER(). WAIVER is not a format but we added this handler to be consistant with format handler
 * design.
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_WAIVER( void* data, const uint16_t data_buf_size, uint8_t* string,
								const uint16_t str_buf_size, conv_type_t dir, void* config );

/**
 * @brief Handler for HOST_ADDRESS(). This format can accept Host name and IP address as string
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = Just read from the DCI and send for the GET request
 * 				 STRING_TO_RAW = Just write the value entered by the user to DCID for the PUT request
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_HOST_ADDRESS( void* data, const uint16_t data_buf_size, uint8_t* string,
									  const uint16_t str_buf_size, conv_type_t dir, void* config );

/**
 * @brief Handler for FW_VERSION()
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_FW_VERSION( void* data, const uint16_t data_buf_size, uint8_t* string,
									const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for CRED(). Currently We do not support any format conversion for cred type data.
 * We handle separately in rest.cpp for PUT  and GET command.
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] length: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_CRED( void* data, const uint16_t data_buf_size, uint8_t* string,
							  const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler to convert uint8 array to hex string
 * @param[in] data: pointer to raw data
 * @param[in] data_buf_size: Max raw data length
 * @param[in] string: formatted data
 * @param[in] str_buf_size: Max formatted string length
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * 				 STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_UINT8_ARR_TO_HEX_STR( void* data, const uint16_t data_buf_size, uint8_t* string,
											  const uint16_t str_buf_size, conv_type_t dir, void* config );

/**
 * @brief Parse_Formatted_String is used to parse comma seperated elements. e.g."element1,element2,element3"
 * @param[in] str: pointer to string which needs to validate.
 * @param[in] format_validation_handle points to a function which is going to parse individual elements. e.g."element1"
 * @param[in] raw_data: string to raw converted output will be stored here.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t Parse_Formatted_String( char_t* str, format_validation_handle_t format_validation_handle, void* raw_data );

/**
 * @brief Convert Firmware Revision number to ASCII
 * @param[in] firm_rev: Firmware revision number in uint32 format.
 * @param[out] firm_rev_ascii: Converted ASCII firmware version will be stored here.
 * @return 	None
 */
void Convert_Rev_Num_To_Ascii( uint32_t firm_rev, uint8_t firm_rev_ascii[] );

/**
 * @brief Convert Firmware Revision ASCII to number
 * @param[in] firm_rev_ascii: Firmware revision number in ASCII format.
 * @param[out] fw_ver: Converted decimal number format will be stored here.
 * @return 	None
 */
void Convert_Rev_Ascii_To_Num( char* firm_rev_ascii, fw_file_version* fw_ver );

/**
 * @brief Convert Firmware Revision ASCII to number
 * @param[in] firm_rev_ascii: Firmware revision number in ASCII format.
 * @return 	fw_ver: Converted Hexadecimal number format
 */
uint32_t Convert_Rev_Ascii_To_HEX( char_t* firm_rev_ascii );

/**
 * @brief Check if string contains only IP address
 * @param[in] ip_add: Input String which needs to be validate
 * @param[in] raw_data: Output raw data will get stored here
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint8_t Validate_And_Parse_IPv4_Address( char_t* ip_add, void* raw_data );

/**
 * @brief Check if string contains only MAC address
 * @param[in] mac_add: Input String which needs to be validate
 * @param[in] raw_data: Output raw data will get stored here
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint8_t Validate_And_Parse_Mac_Address( char_t* mac_add, void* raw_data );

#endif	// FORMAT_HANDLER_H
