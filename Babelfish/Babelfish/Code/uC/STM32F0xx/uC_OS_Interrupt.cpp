/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_OS_Interrupt.h"

extern "C"
{
void asmPendSVHandler( void );

void asmSVCHandler( void );

};

/*
 *****************************************************************************
 *		Prototypes
 *****************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_OS_Interrupt::uC_OS_Interrupt( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_OS_Interrupt::Set_OS_Vect( uC_OS_INT_CALLBACK_FUNC* int_callback,
								   IRQn_Type irq_n, BF_Lib::MBASE priority,
								   BF_Lib::MBASE trigger_type )
{
	if ( priority <= static_cast<BF_Lib::MBASE>( uC_OS_INT_PRIORITY_HIGHEST ) )
	{
		priority = static_cast<BF_Lib::MBASE>( uC_OS_INT_PRIORITY_HIGHEST );
	}
	if ( priority > static_cast<BF_Lib::MBASE>( uC_OS_INT_PRIORITY_LOWEST ) )
	{
		priority = static_cast<BF_Lib::MBASE>( uC_OS_INT_PRIORITY_LOWEST );
	}

	return ( uC_Interrupt::Set_Vector( int_callback, irq_n, static_cast<uint16_t>( priority ) ) );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'trigger_type' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OS_Interrupt::Set_SVC_Handler_Vector( void )
{
	// Service Call handler must have a high priority.
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	uC_Interrupt::Set_Vector( &asmSVCHandler, SVCall_IRQn,
							  static_cast<uint8_t>( uC_INT_HW_PRIORITY_HIGHEST ) );

	// OS Tick and pendSV handlers need to be set at the lowest priority
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not
	 * enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	uC_Interrupt::Set_Vector( &asmPendSVHandler, static_cast<IRQn_Type>( PendSV_IRQn ),
							  static_cast<uint16_t>( uC_INT_HW_PRIORITY_LOWEST ) );
}
