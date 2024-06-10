/**
 *****************************************************************************************
 *	@file
 *
 *	@brief FIFO is a method for organizing a data buffer of memory, in which the
 *	oldest entry is processed first.
 *
 *	@details My_FIFO primarily consists of a set of read and write pointers,
 *	storage and control logic
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MY_FIFO_H
#define MY_FIFO_H

namespace BF_Misc
{
/*
 **************************************************************************************************
 *  @brief The My_FIFO contains the functionality to allocate a space in the memory that can
 *  be used in the First In First Out way.
 *  @details First In First Out means that the oldest entry is processed first.
 *  @n @b Usage:
 *  @n @b 1. Contains methods to initialize the FIFO space, read the data from the FIFO,
 *  check the status of the buffer if it is free, full, empty.
 **************************************************************************************************
 */
class My_FIFO
{
	public:

		/**
		 *  @brief Constructor.
		 *  @details This basically initializes the FIFO start data member with the address from the Ram along
		 *  with initializing other data members. In case the passed in buffer size is zero, it calls for an
		 *  internal exception handling.
		 *  @param[in] buffer_size: The size of the buffer to be created.
		 */
		My_FIFO( uint16_t buffer_size );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object My_FIFO goes out of scope or deleted.
		 *  @return None
		 */
		~My_FIFO()
		{}

		/**
		 *  @brief Used to get the copy of the buffer data.
		 *  @details It copies the buffer data into the destination address till the passed in length.
		 *  In case if the copy is not successful, control goes into the internal exception handling.
		 *  @param[in] dest: The destination address.
		 *  @param[in] size: The size of the data to be copied.
		 */
		void Get_String( uint8_t* dest, uint16_t size );

		/**
		 *  @brief Used to write the data into the buffer.
		 *  @details It copies the source string data into the destination buffer till the passed in length.
		 *  In case if the copy is not successful, control goes into the internal exception handling.
		 *  @param[in] src: The source address from where the data is to be read.
		 *  @param[in] size: The size of the data to be copied.
		 */
		void Set_String( uint8_t* src, uint16_t size );

		/**
		 *  @brief This is used to get the byte value at the present address of the buffer.
		 *  @details In case if the buffer value is not appropriate, control goes into the internal exception handling.
		 *  @return The value at the address of the buffer.
		 */
		uint8_t Get_Byte( void );

		/**
		 *  @brief This is used to write into the present address of the buffer.
		 *  @details In case if the free buffer value is zero, i.e. not appropriate, control goes into the
		 *  internal exception handling.
		 *  @param[in] data: The value to be written into the buffer address.
		 *  @return None
		 */
		void Set_Byte( uint8_t data );


		/**
		 *  @brief This is used to get the size of FIFO
		 *  @details This function calculates the number of elements queued in FIFO
		 *  @param[in] None
		 *  @return Number of elements in FIFO
		 */
		uint16_t FIFO_Size( void );

		/**
		 *  @brief This is used to know if the FIFO space is empty or not.
		 *  @details The buffer free value is determined as the difference between the start and the end address values.
		 *  @return The status if the FIFO space is empty or not.
		 *  @retval true: FIFO space is empty.
		 *  @retval false: FIFO space is not yet empty.
		 */
		bool Is_Empty( void ) const;

		/**
		 *  @brief This is used to know if the FIFO space is full or not.
		 *  @details The FIFO space is full when the difference between the start and the end address values will be
		 * zero.
		 *  @return The status if the FIFO space is full or not.
		 *  @retval true: FIFO space is full.
		 *  @retval false: FIFO space is not yet full.
		 */
		bool Is_Full( void ) const;

		/**
		 *  @brief This provides the value of the free space in the FIFO space, depending on the total buffer size.
		 *  @return The value of the free FIFO data.
		 */
		uint16_t FIFO_Free( void ) const;

	private:
		uint8_t* m_buffer_start;
		uint8_t* m_buffer_end;
		uint16_t m_buffer_free;
		uint8_t* m_enque;
		uint8_t* m_deque;
		uint16_t m_buffer_max_size;
};

}

#endif
