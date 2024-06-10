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
#ifndef OS_TICK_H
	#define OS_TICK_H

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

/*
 *****************************************************************************
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class uC_OS_Tick
{
	public:
		typedef void CALLBACK_FUNC( void );
		// Typically we should call the constructor and assign the lowest
		// priority to SysTick, and then call Start_OS_Tick when the OS is
		// set up properly and ready to go
		uC_OS_Tick( CALLBACK_FUNC* tick_handler );

		~uC_OS_Tick( void );

		static void Start_OS_Tick( void );

	private:
		static void OS_Tick_Handler( void );

		static CALLBACK_FUNC* m_tick_handler;
};

#endif