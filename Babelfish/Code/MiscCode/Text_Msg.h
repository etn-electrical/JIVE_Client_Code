/**
 *****************************************************************************************
 *   @file Text_Msg.h
 *
 *   @brief Text Message Structure with functionality to read, write from the buffer arrays.
 *
 *   @details This structure, along with its helper operations can be used to manipulate
 *				the contents of uint8_t arrays up to 65535 bytes in size.
 *
 *   @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef Text_Msg_H
#define Text_Msg_H

#ifdef __cplusplus	/* Can only be used with C++ */

namespace BF_Misc
{

/*
 **************************************************************************************************
 *  @brief The Text_Msg contains the two buffers, one constant read and other write, we can work we them
 *  like add the data, read, compare etc.
 *  @details This structure, along with its helper operations can be used to manipulate
 *	the contents of uint8_t arrays up to 65535 bytes in size.
 *  @n @b Usage:
 *  @n @b 1. It contains public method to write the byte, array, strings, unsigned or signed 32 bit data,
 *  hex values into the write buffer, also read these from the buffer.
 **************************************************************************************************
 */
typedef struct Text_Msg
{
	uint8_t* m_write_buffer;
	const uint8_t* m_read_buffer;
	uint16_t m_length;
	uint16_t m_index;
	bool m_wrap_around;

	/**
	 *  @brief This initializes the data members with appropriate values.
	 *  @details There are two buffers, the write buffer kept to null. Here the read buffer is constant.
	 *  @param[in] buffer: The address of the passed in buffer. Here the read buffer is constant.
	 *  @param[in] length: The length of the buffer.
	 *  @param[in] wrap_around: The status whether to allow wrap around or to simply end.
	 *  @return None
	 */
	void Bind( const uint8_t* buffer, uint16_t length, bool wrap_around );

	/**
	 *  @brief This initializes the data members with appropriate values.
	 *  @param[in] buffer: The address of the passed in buffer.
	 *  @param[in] length: The length of the buffer.
	 *  @param[in] wrap_around: The status whether to allow wrap around or to simply end.
	 *  @return None
	 */
	void Bind( uint8_t* buffer, uint16_t length, bool wrap_around );

	/**
	 *  @brief This is the initialization of the read and write buffer index to zero.
	 *  @return None
	 */
	void Empty( void )
	{
		m_index = 0;
	}

	/**
	 *  @brief This writes the byte into the write buffer.
	 *  @details In case when the wrap around is activated, the buffer array index is reinitialized to zero.
	 *			i.e It is wrap-around buffer safe.
	 *  @param[in] byte: The byte to be written into the buffer array..
	 *  @return None
	 */
	void Add_Byte( uint8_t byte );

	/**
	 *  @brief This writes into the write buffer the passed in array byte by byte.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] bytes: The pointer to the bytes array.
	 *  @param[in] length: The length of bytes to be written.
	 *  @return None
	 */
	void Add_Array( const uint8_t* bytes, uint16_t length );

	/**
	 *  @brief This writes a string into the write buffer. It is wrap-around buffer safe.
	 *  @details The bytes are written after the flags verification into a specific format,
	 *  the format: </the string data>carriage return byte.It is wrap-around buffer safe.
	 *  @param[in] data: The actual data of the string to be written inside the write buffer.
	 *  @param[in] flags: Formatting instructions for the written data.
	 *  @return None
	 */
	void Add_String( const uint8_t* data, uint16_t flags = 0 );

	/**
	 *  @brief This is used to write the unsigned 32 bit value into the write buffer.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] value: The value to be written.
	 *  @param[in] flags: Formatting instructions for the written data.
	 *  @return None
	 */
	void Add_U32( uint32_t value, uint16_t flags = 0 );

	/**
	 *  @brief This is used to write the signed 32 bit value into the write buffer.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] value: The signed value to be written.
	 *  @param[in] flags: Formatting instructions for the written data.
	 *  @return None
	 */
	void Add_S32( int32_t value, uint16_t flags = 0 );

	/**
	 *  @brief This is used to write the float 32 bit value into the write buffer.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] value: The float value to be written.
	 *  @param[in] flags: Formatting instructions for the written data.
	 *  @return None
	 */
	void Add_F32( float32_t value, uint16_t flags = 0 );

	/**
	 *  @brief This adds the Hex byte into the write buffer.
	 *  @details The byte is written in a specific format. E.g. for byte 'F' we write 0 first
	 *			then then F i.e. we write 0x0F. It is wrap-around buffer safe.
	 *  @param[in] byte: The byte to be written.
	 *  @return None
	 */
	void Add_Hex_Byte( uint8_t byte );

	/**
	 *  @brief The extracts the byte from the buffer and writes to the address passed in.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] &byte: The address that will contain the value.
	 *  @return The status whether the byte extracted is successfully written into the passed address or not.
	 *  @retval true: The byte was successfully written.
	 *  @retval false: The byte was not successfully written.
	 */
	bool Extract_Byte( uint8_t& byte );

	/**
	 *  @brief The extracts the unsigned 32 bits from the buffer and writes to the address passed in.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] &u32t: The address that will contain the value.
	 *  @param[in] delimiter: It marks the end of the uint32 data.
	 *  @return The status whether the values extracted are successfully written into the passed u32t address or not.
	 *  @retval true: The values were successfully written.
	 *  @retval false: The values were not successfully written.
	 */
	bool Extract_U32( uint32_t& u32t, uint8_t delimiter );

	/**
	 *  @brief The extracts the unsigned 32 bits from the buffer and writes to the address passed in.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] &f32t: The address that will contain the value.
	 *  @param[in] delimiter: It marks the end of the float32_t data.
	 *  @return The status whether the values extracted are successfully written into the passed u32t address or not.
	 *  @retval true: The values were successfully written.
	 *  @retval false: The values were not successfully written.
	 */
	bool Extract_F32( float32_t& f32t, uint8_t delimiter );

	/**
	 *  @brief The extracts the signed 32bit, the signed value from the buffer and writes to the address passed in.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] &u32t: The address that will contain the value.
	 *  @param[in] delimiter: It marks the end of the int32 data.
	 *  @return The status whether the values extracted are successfully written into the passed s32t address or not.
	 *  @retval true: The values were successfully written.
	 *  @retval false: The values were not successfully written.
	 */
	bool Extract_S32( int32_t& s32t, uint8_t delimiter );

	/**
	 *  @brief This extracts the byte from the constant read buffer and writes into the passed in address.
	 *  @details The byte is extracted nibble by nibble and the ascii value is converted into the decimal equivalent.
	 *				It is wrap-around buffer safe.
	 *  @param[in] &byte: The address that will contain the value.
	 *  @return The status whether the byte extracted is successfully written into the passed address or not.
	 *  @retval true: The byte was successfully written.
	 *  @retval false: The byte was not successfully written.
	 */
	bool Extract_Hex_Byte( uint8_t& byte );

	/**
	 *  @brief Compares the data of the passed in array with the contents of the read buffer.
	 *  @details This compares the data of the read_buffer starting at its zero index to the passed in array.
	 *			As such, it is not wrap-around buffer safe.
	 *  @param[in] bytes: The pointer to the start of the byte array to be compared.
	 *  @param[in] length: The length of the byte array to be compared.
	 *  @param[in] advance: boolean indicating that index should be advanced passed compare value if successful.
	 *  @return
	 */
	bool Compare( const uint8_t* bytes, uint16_t length, bool advance );

	/**
	 *  @brief Advances the index used to access the buffer arrays.
	 *  @details It is wrap-around buffer safe.
	 *  @param[in] length: The number of the bytes to advance the index
	 *  @return None
	 */
	void Advance( uint16_t length );

	/**
	 *  @brief Advances the index past an array of bytes in the buffer.
	 *  @details Routine will advance index to one location past passed-in array (if found) or the end of buffer
	 * contents.
	 *			It is not wrap-around buffer safe.
	 *  @param[in] bytes: The pointer to the start of the byte array to be advanced past.
	 *  @param[in] length: The length of the byte array to be advanced past.
	 *  @returns true if array is found
	 */
	bool Advance_Past( const uint8_t* bytes, int16_t length );

	/**
	 *  @brief Verifies buffer contains only spaces until the delimiter or end of buffer content; starting
	 *			at the present buffer index.
	 *  @details Routine will advance index to one location past delimiter or the end of buffer contents.
	            It is not wrap-around buffer safe.
	 *  @param[in] delimiter: The character the marks the end of the region to be checked.
	 *  @returns true if only spaces found until delimiter or the end of the buffer
	 */
	bool Is_Clear_Until( uint8_t delimiter );


	/**
	 *  @brief Advances the index used to access the read buffer past all space characters.
	 *  @return None
	 */
	void Trim( void );

	/// Flags for Add String, F32, U32, S32, U8 operations
	enum
	{
		SHOW_LEADING_ZEROS = ( 1 << 15 ),
		SHOW_SIGN = ( 1 << 14 ),
		INSERT_PLUS = ( 1 << 13 ),
		INSERT_MINUS = ( 1 << 12 ),
		APPEND_COMMA = ( 1 << 11 ),
		APPEND_LF = ( 1 << 10 ),
		DECIMAL_AT_TENTHS = ( 1 << 4 ),
		DECIMAL_AT_HUNDREDTHS = ( 2 << 4 ),
		DECIMAL_AT_THOUSANDTHS = ( 3 << 4 ),
		DECIMAL_AT_TEN_THOUSANDTHS = ( 4 << 4 ),
		DECIMAL_AT_HUNDRED_THOUSANDTHS = ( 5 << 4 ),
		DECIMAL_AT_MILLIONTHS = ( 6 << 4 ),
		DECIMAL_MASK = ( 0xF << 4 ),
		SHOW_1_DIGIT = ( 1 << 0 ),
		SHOW_2_DIGITS = ( 2 << 0 ),
		SHOW_3_DIGITS = ( 3 << 0 ),
		SHOW_4_DIGITS = ( 4 << 0 ),
		SHOW_5_DIGITS = ( 5 << 0 ),
		SHOW_6_DIGITS = ( 6 << 0 ),
		SHOW_7_DIGITS = ( 7 << 0 ),
		SHOW_8_DIGITS = ( 8 << 0 ),
		SHOW_9_DIGITS = ( 9 << 0 ),
		SHOW_10_DIGITS = ( 10 << 0 ),
		POSITIONS_MASK = ( 0xF << 0 ),

		TAG_NAME = ( 1 << 0 ),
		CLOSE_TAG = ( 1 << 1 ),
		ADD_CR = ( 1 << 2 ),

		LINE_FEED = 10,
		CARRIAGE_RETURN = 13
	};

} TextMsg;

}
#endif	/* #ifdef __cplusplus */

#endif	/* #ifndef __BYTEARRAY_H__ */

/*******************************************************************************
*  End of File
*******************************************************************************/
