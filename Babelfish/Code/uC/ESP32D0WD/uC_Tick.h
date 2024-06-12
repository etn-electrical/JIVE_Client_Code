/**
 *****************************************************************************************
 * @file		uC_Tick.h
 *
 * @brief		This file provides the access for the tick functionalities
 *
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_TICK_H
   #define uC_TICK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Interrupt.h"

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void (* uC_TICK_CB)( void );

/*
 *****************************************************************************************
 *		Declarations
 *****************************************************************************************
 */
extern void _xt_tick_divisor_init( void );

/**
 ****************************************************************************************
 * @brief						The uC_Tick class contains implementations required to the
 * @n							access of the functionalities of tick.
 * @details						It provides functionalities to start tick timer, tick handling.
 ****************************************************************************************
 */
class uC_Tick
{
	public:
		/**
		 * @brief								Constructor to create an instance of uC_Tick class.
		 * @n									The following function is used if you want to skip over the normal OS
		 * @n									stuff and just use the uC_OS_Tick for a general tick behavior.
		 * @param[in] tick_cback				Tick handler or an call back function pointer
		 * @param[in] tick_frequency_in_hertz	Tick frequency details in hertz
		 * @param[in] tick_interrupt_priority	Tick Interrupt priority details
		 */
		uC_Tick( uC_TICK_CB tick_cback,
				 uint32_t tick_frequency_in_hertz, uint8_t tick_interrupt_priority );

		/**
		 * @brief				Destructor to delete the uC_Tick instance when it goes out of scope
		 * @return				None
		 */
		~uC_Tick( void );

		/**
		 * @brief				The function to start the tick timer
		 * @return				None
		 */
		static void Start_Tick( void );

	private:
		/**
		 * @brief				The function provides the tick handling functionality
		 * @return				None
		 */
		static void Tick_Handler( void );

		static uC_TICK_CB m_tick_cb;	///< Which contains the tick handler details
};

#ifdef __cplusplus
}
#endif

#endif
