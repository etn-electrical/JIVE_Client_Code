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
#ifndef uC_OS_INTERRUPT_H
	#define uC_OS_INTERRUPT_H


/// The following is the start of the OS Interrupt priorities that are safe to use.
/// Right now it is set for the middle.  So higher priority interrupts must not use the OS.
#define MAX_SYSCALL_INTERRUPT_PRIORITY          8

/// This is the actual register value put into the base priority register which is only
/// the top 4bits.
#define MAX_SYSCALL_INTERRUPT_BASE_PRIORITY     ( MAX_SYSCALL_INTERRUPT_PRIORITY << 4 )

#ifdef __IAR_SYSTEMS_ICC__		// This makes sure that the assembly compile will not include the
								// following.

#include "uC_Interrupt.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
enum
{
	uC_OS_INT_PRIORITY_0 = MAX_SYSCALL_INTERRUPT_PRIORITY,	// Highest priority
	uC_OS_INT_PRIORITY_1,
	uC_OS_INT_PRIORITY_2,
	uC_OS_INT_PRIORITY_3,
	uC_OS_INT_PRIORITY_4,
	uC_OS_INT_PRIORITY_5,
	uC_OS_INT_PRIORITY_6,
	uC_OS_INT_PRIORITY_7,		// Lowest Priority
	uC_OS_INT_MAX_PRIORITY
};

#define uC_OS_INT_PRIORITY_LOWEST       ( ( uC_OS_INT_MAX_PRIORITY ) - 1 )
#define uC_OS_INT_PRIORITY_HIGHEST      uC_OS_INT_PRIORITY_0

#if uC_OS_INT_PRIORITY_LOWEST >= 0x10
	#error \
	The max OS Interrupt priority (lowest priority) is beyond the reach of the priority descriptor.
#endif

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
typedef void (uC_OS_INT_CALLBACK_FUNC)( void );


/*
 *****************************************************************************
 *  Class Declaration
   -----------------------------------------------------------------------------
 */
class uC_OS_Interrupt
{
	public:
		uC_OS_Interrupt( void );
		~uC_OS_Interrupt( void );

		static bool Set_OS_Vect( uC_OS_INT_CALLBACK_FUNC* int_callback,
								 IRQn_Type irq_n, BF_Lib::MBASE priority,
								 BF_Lib::MBASE trigger_type = 0 );

		static void Set_SVC_Handler_Vector( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Enable_Int( IRQn_Type vector_number )
		{
			uC_Interrupt::Enable_Int( vector_number );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Disable_Int( IRQn_Type vector_number )
		{
			uC_Interrupt::Disable_Int( vector_number );
		}

		// static bool Is_Enabled( IRQn_Type vector_number )		{ return( uC_Interrupt::Is_Enabled(
		// vector_number ) ); }
		static void Clear_Int( IRQn_Type vector_number )
		{
			uC_Interrupt::Clear_Int( vector_number );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Set_Int( IRQn_Type vector_number )
		{
			uC_Interrupt::Set_Int( vector_number );
		}

	private:
};

#endif		// __IAR_SYSTEMS_ICC__

#endif
