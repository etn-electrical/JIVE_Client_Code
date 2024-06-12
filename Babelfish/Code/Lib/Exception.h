/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module contains the implementation for the internal exceptions handling.
 *
 *	@details The exceptions used are handled through this module. While we use
 *	the internal exceptions and are not using the external exceptions (e.g. bad memory
 *	allocation in C++), this module is useful.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "Device_Defines.h"
#include "Exception_List.h"

namespace BF_Lib
{
#ifdef PROC_SPEC_HARD_FAULT_Exception_Stop_Here
#define Exception_Stop_Here()   Disable_Interrupts();while ( true ){};
#else
#ifdef DEBUG
#define Exception_Stop_Here()                Disable_Interrupts();while ( true ){ Debug_Break(); }
#define Exception_Condition( condition )    do{Disable_Interrupts(); \
											   while ( ( condition ) == TRUE ){ uC_Watchdog_HW::Kick_Dog(); } \
}while( 0 )

#define Exception_Assert( x ) do{ \
		if ( !( x ) ){ \
			Disable_Interrupts(); \
			while ( TRUE ){}; \
		} \
}while( 0 )
#else
#define Exception_Stop_Here()                Disable_Interrupts();while ( true ){};
#define Exception_Condition( condition )    do{Disable_Interrupts();while ( ( condition ) == TRUE ){};}while( 0 )

#define Exception_Assert( x ) do{ \
}while( 0 )
#endif
#endif
// #ifndef ARCH_EXCEPTION_OVERRIDES
// #define InvalidRamPtr(x) (0)
// #endif
/**
 ****************************************************************************************
 * @brief This is the Exception class containing the Exception Defines for debug purposes.
 *
 * @details
 * @n @b Usage: While we want stop as an unintended condition is reached e.g.
 * a DCID entered is invalid.
 *
 ****************************************************************************************
 */
class Exception
{
	public:

		/**
		 *  @brief Init function for the Exception module.
		 *  @details Currently this does nothing.
		 */
		static void Init( void );

		/**
		 *  @brief This is the internal exception handling implementation.
		 *  @details This disables all the interrupts and stops the further operation.
		 *  @n @b Usage: In case if the memory allocation was not successful and incorrect condition is
		 *  thus reached, we handle such situations by calling Stop_Here.
		 *  @n @b Usage: If the destructor is not supported inside a particular module, we handle
		 *  such situations by calling Destructor_Not_Supported.
		 *  @returns None
		 */
		static inline void Stop_Here( void )
		{
			Disable_Interrupts();
			for (;;)
			{
				Debug_Break();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Destructor_Not_Supported( void )
		{
			Disable_Interrupts();
			// coverity[no_escape]
			for (;;)
			{
				Debug_Break();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Enter_HEL( void )
		{
			Disable_Interrupts();
			for (;;)
			{
				Debug_Break();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Enter_SEL( void )
		{
			Disable_Interrupts();
			for (;;)
			{
				Debug_Break();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Assert( bool condition )
		{
			if ( condition == false )
			{
				Enter_HEL();
			}
		}

	private:
		/**
		 * Constructor, Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Exception( void );
		~Exception( void );
		Exception( const Exception& rhs );
		Exception & operator =( const Exception& object );

};

}
#endif	/* EXCEPTION_H*/
