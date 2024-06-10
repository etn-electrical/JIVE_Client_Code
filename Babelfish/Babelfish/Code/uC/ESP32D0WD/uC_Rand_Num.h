/**
 *****************************************************************************************
 *	@file	uC_Rand_Num.h Header File for RNG Driver implementation.
 *
 *	@brief	The file contains uC_Rand_Num Class for generation of random number.
 *
 *	@details	This class provides a random number generator. In ESP32,the hardware RNG is
 *	fully functional whenever an RF subsystem is running (ie Bluetooth or WiFi is enabled).
 *	For random values, call this function after WiFi or Bluetooth are started.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_RAND_NUM_H
   #define uC_RAND_NUM_H

#include "uC_Base.h"

/*
 ***********************************
 *	Variables and Constants
 ***********************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Rand_Num
{
	public:

		/**
		 * @brief                   Constructor to create instance of uC_Rand_Num class.
		 */
		uC_Rand_Num( void );

		/**
		 * @brief                   Destructor to delete the uc_Rand_Num instance when it goes out of
		 *                          scope.
		 */
		~uC_Rand_Num() {}

		/**
		 * @brief                   Get a buffer with random number from hardware RNG
		 *
		 * @param[in] rand_data 	Pointer to buffer to fill with random numbers.
		 * @param[in] length 		Length of buffer in bytes
		 */
		static void Get( uint8_t* rand_data, uint16_t length );

	private:
};


#endif



