/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BIN_ASCII_CONVERSION_H
#define BIN_ASCII_CONVERSION_H


class BIN_ASCII_Conversion
{
	public:
		static uint8_t byte_to_str( const uint8_t* pBytes, uint8_t* p_byte );

		static uint8_t word_to_str( const uint8_t* pBytes, uint8_t* p_word );

		static uint8_t dword_to_str( const uint8_t* pBytes, uint8_t* p_dword );

		static uint8_t float_to_str( const uint8_t* pBytes, uint8_t* p_dword );

		static uint8_t dfloat_to_str( const uint8_t* pBytes, uint8_t* p_dword );

		/**
		 * @brief Convert hex value to string.
		 * @details This function converts the hex value to it's string form
		 * e.g. 0x0a = "0A"
		 * @param[in] pBytes: value in hex format.
		 * @param[out] p_ascii: value in string format.
		 * @return None.
		 */
		static void byte_to_str_no_prefix( const uint32_t pBytes, uint8_t* p_ascii );

};


#endif	// BIN_ASCII_CONVERSION_H
