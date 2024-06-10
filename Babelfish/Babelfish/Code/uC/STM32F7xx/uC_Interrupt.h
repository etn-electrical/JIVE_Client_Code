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
#ifndef uC_INTERRUPT_H
   #define uC_INTERRUPT_H

#include "uC_Base.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

// Interrupt Priority Selection
typedef enum
{
	uC_INT_HW_PRIORITY_0,		// Highest priority
	uC_INT_HW_PRIORITY_1,
	uC_INT_HW_PRIORITY_2,
	uC_INT_HW_PRIORITY_3,
	uC_INT_HW_PRIORITY_4,
	uC_INT_HW_PRIORITY_5,
	uC_INT_HW_PRIORITY_6,
	uC_INT_HW_PRIORITY_7,
	uC_INT_HW_PRIORITY_8,
	uC_INT_HW_PRIORITY_9,
	uC_INT_HW_PRIORITY_10,
	uC_INT_HW_PRIORITY_11,
	uC_INT_HW_PRIORITY_12,
	uC_INT_HW_PRIORITY_13,
	uC_INT_HW_PRIORITY_14,
	uC_INT_HW_PRIORITY_15,		// Lowest Priority
	uC_INT_HW_MAX_PRIORITY
} uC_INT_PRIORITY_ENUM;
#define uC_INT_HW_PRIORITY_LOWEST       ( uC_INT_HW_MAX_PRIORITY - 1 )
#define uC_INT_HW_PRIORITY_HIGHEST      uC_INT_HW_PRIORITY_0

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void INT_CALLBACK_FUNC( void );

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Interrupt
{
	public:
		uC_Interrupt( void );
		~uC_Interrupt( void );

		static bool Set_Vector( INT_CALLBACK_FUNC* vector_callback, IRQn_Type vector_number, uint16_t priority );

		static void Clear_Vector( IRQn_Type vector_number );

		static bool Is_Vector_Empty( IRQn_Type vector_number );

		static void Enable_Int( IRQn_Type vector_number )       { NVIC_EnableIRQ( vector_number ); }

		static void Disable_Int( IRQn_Type vector_number )      { NVIC_DisableIRQ( vector_number ); }

		static bool Is_Enabled( IRQn_Type vector_number )       { return ( NVIC_GetActive( vector_number ) ); }

		static void Clear_Int( IRQn_Type vector_number )            { NVIC_ClearPendingIRQ( vector_number ); }

		static void Set_Int( IRQn_Type vector_number )          { NVIC_SetPendingIRQ( vector_number ); }

	private:
};

#endif
