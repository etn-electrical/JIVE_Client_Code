/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This Module contains functionalities to copy the string considering the
 *	different possibilities.
 *
 *	@details The string could be constant, null, constant null, the checksum return could
 *	be expected, string comparison such different scenarios over strings considers and
 *	implemented in this module.
 *
 *  @n @b Usage: Operations using strings - copy, compare, align, copy null string, copy
 *  constant string, copy single value across, copy with CRC return.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef STDLIB_MV_H
#define STDLIB_MV_H

namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Functions
 *****************************************************************************************
 */

/**
 *  This tells others that the copy string will not run over its own data if
 *  the copy string is operating on itself.
 */
#define STDLIB_MV_SMART_COPY_STRING

/**
 *  @brief A standard uint8_t copy string.
 *  @details A simple standard to copy the source into the destination string.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return None
 */
void Copy_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief Copy string with a constant source.
 *  @details A simple standard to copy the constant source into the destination string.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return None
 */
void Copy_Const_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief Copy string from source to destination.
 *  @details Copy string with a 16bit addition type checksum returned.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return The 16bit checksum value.
 */
uint16_t Copy_String_Ret_Checksum( uint8_t* dest, uint8_t const* source, uint32_t length );

/// The CRC Initialized value of the copy string for CCITT CRC algorithm.
const uint16_t COPY_STRING_CRC_INIT = 0xFFFFU;

/**
 *  @brief Copy string from source to destination.
 *  @details Copy string using the CCITT CRC algorithm.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @param[in] init_crc: The CRC Initialized value.
 *  @return The CRC value is returned.
 */
uint16_t Copy_String_Ret_CRC( uint8_t* dest, uint8_t const* source,
							  uint32_t length, uint16_t init_crc = COPY_STRING_CRC_INIT );

/**
 *  @brief Copy string from source to destination.
 *  @details Copy string will copy until a NULL is encountered. A marriage of strlen and strcopy.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return The length of the string copied is returned.
 */
uint16_t Copy_Null_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief Copy string from source to destination.
 *  @details Copy string will copy until a NULL is encountered and here using a const source.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return The length of the string copied is returned.
 */
uint16_t Copy_Const_Null_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief String comparison.
 *  @details Does a comparison of string1 and string2.  If the strings are different then a false is returned.
 *  Otherwise if the strings are the same true is returned.
 *  @param[in] string1: The pointer to the first string.
 *  @param[in] string2: The pointer to the second string.
 *  @param[in] length: The length to be compared.
 *  @return The status after comparision.
 *  @retval true: The strings are same.
 *  @retval false: The strings are different.
 */
bool String_Compare( uint8_t const* string1, uint8_t const* string2, uint32_t length );

/**
 *  @brief A standard uint8_t copy string which will allow you to copy internally to a string.
 *  @details What this means is that if you need to shift a string forward or back within itself
 *  this will do it without writing over the top of itself.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return None.
 */
void Smart_Copy_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief A standard uint8_t copy const string which will allow you to copy internally to a string.
 *  @details What this means is that if you need to shift a string forward or back within itself
 *  this will do it without writing over the top of itself.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return None.
 */
void Smart_Copy_Const_String( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 *  @brief A standard uint8_t copy string which will allow you to copy internally to a string.
 *  @details What this means is that if you need to shift a string forward or back within itself
 *  this will do it without writing over the top of itself.
 *  @param[in] dest: The pointer to the destination string.
 *  @param[in] source: The pointer to the source string.
 *  @param[in] length: The length to be copied.
 *  @return 16bit addition type checksum returned.
 */
uint16_t Smart_Copy_String_Ret_Checksum( uint8_t* dest, uint8_t const* source, uint32_t length );

/**
 * @brief
 * @details Takes a uint8_t string and aligns it to UINT32 memory.
 * @n @b Usage: This would be used if you were receiving a uint8_t string full of UINT32 data.
 * You want to use it but need to copy it to the proper alignment so you can use it natively.
 * @param[in] src: The pointer to the destination string.
 * @param[in] dest: The pointer to the source string.
 * @param[in] byte_length: The length to be copied.
 * @return The length in words of the string aligned.
 */
uint32_t Align_String_U8_To_U32( uint32_t* dest, uint8_t const* src, uint32_t byte_length );

/**
 * @brief
 * @details Takes a uint8_t string and aligns it to uint16_t memory.
 * @n @b Usage: This would be used if you were receiving a uint8_t string full of UINT32 data.
 * You want to use it but need to copy it to the proper alignment so you can use it natively.
 * @param[in] src: The pointer to the destination string.
 * @param[in] dest: The pointer to the source string.
 * @param[in] byte_length: The length to be copied.
 * @return The length in words of the string aligned.
 */
uint32_t Align_String_U8_To_U16( uint16_t* dest, uint8_t const* src, uint32_t byte_length );

/**
 * @brief
 * @details Copy a single value across an entire string with a 16bit addition type
 * checksum returned.
 * @param[in] dest: The pointer to the destination string.
 * @param[in] value: The pointer to the source string.
 * @param[in] length: The length to be copied.
 * @return The 16bit addition type checksum.
 */
uint16_t Copy_Val_To_String_Ret_Checksum( uint8_t* dest, uint8_t value, uint32_t length );

/**
 * @brief
 * @details Copy a single value across an entire string without a 16bit addition type
 * checksum returned.
 * @param[in] dest: The pointer to the destination string.
 * @param[in] value: The pointer to the source string.
 * @param[in] length: The length to be copied.
 * @return None
 */
void Copy_Val_To_String( uint8_t* dest, uint8_t value, uint32_t length );


/**
 * @brief
 * @details Copy a single value across an entire string without a 16bit addition type
 * checksum returned.
 * @param[in] dest: The pointer to the destination string.
 * @param[in] value: The pointer to the source string.
 * @param[in] length: The length to be Compared.
 * @return None
 */
bool_t Compare_Data( uint8_t const* dest, uint8_t const* source, uint32_t length );

/**
 * @brief
 * @details Return string length if NULL is found in first str_size characters
 * or zero if NULL char not found in first buffer_size char or str is null
 * @param[in] str: The pointer to the string.
 * @param[in] buffer_size: The size of the str .
 * @return length of the str or zero if NULL char not found in first buffer_size char or str is null
 */
uint16_t Get_String_Length( uint8_t* str, uint16_t buffer_size );

/**
 * @brief
 * @details Copy data from source string to destination string performing bound checking
 * @param[in] src: The pointer to the source string.
 * @param[in] src_len: length of source string .
 * @param[in] dst: The pointer to the destination string.
 * @param[in] dst_buf_size: size of the destination string .
 * @return status of successful or failure on copy of data
 */
bool_t Copy_Data( uint8_t* dst, uint16_t dst_buf_size, const uint8_t* src, uint16_t src_len );

/**
 * @brief
 * @details Remove specific character from string
 * @param[in] str: The pointer to the string.
 * @param[in] buffer_size: size of the string .
 * @param[in] c: character needs to be replaced
 * @return returns number of characters removed
 */
uint32_t Remove_Char_From_String( uint8_t* str, const uint32_t buffer_size, uint8_t c );

}

#endif	/* STDLIB_MV_H*/
