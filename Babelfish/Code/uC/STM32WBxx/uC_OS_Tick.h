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
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class uC_OS_Tick
{
	public:
		typedef void CALLBACK_FUNC ( void );
		// Typically we should call the constructor and assign the lowest
		// priority to SysTick, and then call Start_OS_Tick when the OS is
		// set up properly and ready to go
		uC_OS_Tick( CALLBACK_FUNC* tick_handler );

		~uC_OS_Tick( void );

		/**
		 * @brief  : Function start FreeRTOS Systick timer
		 * @details: This function initialize Systick timer required for FreeRTOS functionality
		 * @return : Void
		 */
		static void Start_OS_Tick( void );

		/**
		 * @brief  : Function to Suspend OS Tick
		 * @details: This function will suspend Systick when get called
		 * @Note   : This function will disable FreeRTOS and CR task functionality hence should be
		 * 			 used with precautions.
		 * @return : Void
		 */
		static void Suspend_OS_Tick( void );

		/**
		 * @brief  : Function to Resume OS Tick
		 * @details: This function will Resume Systick when get called
		 * @Note   : This function will Enable FreeRTOS and CR task functionality hence should be
		 * 			 used with precautions.
		 * @return : Void
		 */
		static void Resume_OS_Tick( void );

	private:
		static void OS_Tick_Handler( void );

		static CALLBACK_FUNC* m_tick_handler;
};

#endif
