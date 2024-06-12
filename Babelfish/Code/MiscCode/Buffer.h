/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Buffer allocates some block of memory for moving the data.
 *
 *	@details While moving the data, the data will be placed in the buffer, then placed in its
 *  final destination. This is not every time.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BUFFER_H
#define BUFFER_H

namespace BF_Misc
{
/*
 **************************************************************************************************
 *  @brief The Buffer contains the functionality to allocate a space in the memory that can
 *  be used and is called a buffer space.
 *  @details While moving the data, the data will be placed in the buffer, then placed in its
 *  final destination. This is not every time.
 *  @n @b Usage:
 *  @n @b 1. Contains methods to initialize the buffer space, read the data from the buffer,
 *  flush, check the status of the buffer if it is free, full, empty.
 **************************************************************************************************
 */
class Buffer
{
	public:

		/**
		 *  @brief Constructor.
		 *  @details This basically initializes the buffer start data member with the address from the Ram along
		 *  with initializing other data members.
		 *  @param[in] buffer_size: The size of the buffer to be created.
		 */
		Buffer( int16_t buffer_size );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Buffer goes out of scope or deleted.
		 *  @return None
		 */
		~Buffer()
		{}

		/**
		 *  @brief Used to get the copy of the buffer data.
		 *  @details It copies the buffer data into the destination address till the passed in length.
		 *  In case if the copy is not successful, control goes into the internal exception handling.
		 *  @param[in] dest: The destination address.
		 *  @param[in] length: The length of the data to be copied.
		 */
		void Get_String( uint8_t* dest, int_fast16_t length );

		/**
		 *  @brief Used to write the data into the buffer.
		 *  @details It copies the source string data into the destination buffer till the passed in length.
		 *  In case if the copy is not successful, control goes into the internal exception handling.
		 *  @param[in] src: The source address from where the data is to be read.
		 *  @param[in] length: The length of the data to be copied.
		 */
		void Set_String( uint8_t* src, int_fast16_t length );

		/**
		 *  @brief Used to write in m_deque, the pointer to buffer data value appropriately.
		 *  @details Writes an appropriate value into the data member 'm_deque' which points to the buffer data value,
		 *  depending on the length, start and end of the buffer.
		 *  @param[in] length: The length of the data to be copied.
		 */
		void Flush( int_fast16_t length );

		/**
		 *  @brief This is used to get the value at the present address of the buffer.
		 *  @details In case if the free buffer value is not appropriate, control goes into the internal exception
		 * handling.
		 *  @return The value at the address of the buffer.
		 */
		uint8_t Get( void );

		/**
		 *  @brief This is used to write into the present address of the buffer.
		 *  @details In case if the free buffer value is zero, i.e. not appropriate, control goes into the
		 *  internal exception handling.
		 *  @param[in] data: The value to be written into the buffer address.
		 *  @return None
		 */
		void Set( uint8_t data );

		/**
		 *  @brief This is used to know if the buffer is empty or not.
		 *  @details The buffer free value is determined as the difference between the start and the end address values.
		 *  @return The status if the buffer is empty or not.
		 *  @retval true: Buffer is empty.
		 *  @retval false: Buffer is not yet empty.
		 */
		bool Is_Empty( void ) const;

		/**
		 *  @brief This is used to know if the buffer is full or not.
		 *  @details The buffer is full when the difference between the start and the end address values will be zero.
		 *  @return The status if the buffer is full or not.
		 *  @retval true: Buffer is full.
		 *  @retval false: Buffer is not yet full.
		 */
		bool Is_Full( void ) const;

		/**
		 *  @brief This provides the value of the free space in the buffer, depending on the total buffer size.
		 *  @return The value of the free buffer data.
		 */
		uint16_t FIFO_Free( void ) const;

		/**
		 *  @brief This operator provides the buffer address corresponding to the passed in index.
		 *  @param[in] index: The array index of the buffer array.
		 *  @return The buffer address.
		 */
		uint8_t & operator []( uint16_t index )
		{
			if ( ( index + m_enque ) >= m_buffer_end )
			{
				index = index - ( m_enque - m_buffer_end );
			}
			return ( m_buffer_start[index] );
		}

		/**
		 *  @brief This basically attaches the passed in string with the buffer by initializing the passed
		 *  in string length and the address.
		 *  @details The data members for buffer are initialized w.r.t the passed in string.
		 *  @param[in] string: The array index of the passed in array.
		 *  @param[in] length: The length of the string.
		 *  @return None
		 */
		void Attach_String( uint8_t* string, int16_t length );

	private:
		Buffer( const Buffer& );	///< Private copy constructor
		Buffer & operator =( const Buffer & );	///< Private copy assignment operator

		uint8_t* m_buffer_start;
		uint8_t* m_buffer_end;
		uint8_t* m_enque;
		uint8_t* m_deque;
		int_fast16_t m_buffer_free;
};

}

#endif
