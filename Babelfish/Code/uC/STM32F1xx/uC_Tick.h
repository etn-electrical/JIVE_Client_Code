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
#ifndef uC_TICK_H
	#define uC_TICK_H

#include "uC_Interrupt.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */

/*
 *****************************************************************************
 *		Macros
 *****************************************************************************
 */

/*
 *****************************************************************************
 *		Typedefs
 *****************************************************************************
 */
typedef void (* uC_TICK_CB)( void );

/*
 *****************************************************************************
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class uC_Tick
{
	public:
		// The following function is used if you want to skip over the normal OS
		// stuff and just use the uC_OS_Tick for a general tick behavior.
		uC_Tick( uC_TICK_CB tick_cback,
				 uint32_t tick_frequency_in_hertz, uint8_t tick_interrupt_priority );
		~uC_Tick( void );

		static void Start_Tick( void );

	private:
		static void Tick_Handler( void );

		static uC_TICK_CB m_tick_cb;
};

#endif
