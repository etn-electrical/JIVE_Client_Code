/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
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
		uC_Rand_Num( void );
		~uC_Rand_Num() {}


		static void Get( uint8_t* rand_data, uint16_t length );

	private:
};


#endif



