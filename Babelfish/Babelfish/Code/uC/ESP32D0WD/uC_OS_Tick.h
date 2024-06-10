/**
 *****************************************************************************************
 * @file		uC_OS_Tick.h
 *
 * @brief		This file provides the access for the os tick functionalities
 *
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OS_TICK_H
   #define OS_TICK_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *****************************************************************************************
 *		Declarations
 *****************************************************************************************
 */
extern void _xt_tick_divisor_init( void );

/**
 ****************************************************************************************
 * @brief						The uC_OS_Tick class contains implementations required to the
 * @n							access of the functionalities of OS tick.
 * @details						It provides functionalities to start OS tick timer, tick handling.
 ****************************************************************************************
 */
class uC_OS_Tick
{
	public:
		/**
		 ****************************************************************************************
		 * @brief	Typedef for call back function for tick handler
		 *
		 ****************************************************************************************
		 */
		typedef void CALLBACK_FUNC ( void );

		/**
		 * @brief					Constructor to create an instance of uC_OS_Tick class.
		 * @n						Typically we should call the constructor and assign the lowest
		 * @n						priority to SysTick, and then call Start_OS_Tick when the OS is
		 * @n						set up properly and ready to go
		 * @param[in] tick_handler	Tick handler or an call back function pointer
		 */
		uC_OS_Tick( CALLBACK_FUNC* tick_handler );

		/**
		 * @brief				Destructor to delete the uC_OS_Tick instance when it goes out of scope
		 * @return				None
		 */
		~uC_OS_Tick( void );

		/**
		 * @brief				The function to start the OS tick timer
		 * @return				None
		 */
		static void Start_OS_Tick( void );

	private:
		/**
		 * @brief				The function provides the OS tick handling functionality
		 * @return				None
		 */
		static void OS_Tick_Handler( void );

		static CALLBACK_FUNC* m_tick_handler;	///< Which contains the OS tick handler details
};

#ifdef __cplusplus
}
#endif

#endif
