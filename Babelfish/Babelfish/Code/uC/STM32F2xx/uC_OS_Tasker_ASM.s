/*
    FreeRTOS V6.0.2 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
	Change from V4.2.1:

	+ Introduced usage of configKERNEL_INTERRUPT_PRIORITY macro to set the
	  interrupt priority used by the kernel.
*/

/* Ting Yan, modified based on FreeRTOS V6.0.2 */

#include "uC_OS_Interrupt.h"



	RSEG    CODE:CODE(2)
	thumb

	EXTERN OS_TASKER_current_task
	EXTERN OS_TASKER_Switch_Context

	PUBLIC asmPendSVHandler
	PUBLIC asmSetInterruptMask
	PUBLIC asmClearInterruptMask
	PUBLIC asmSVCHandler
	PUBLIC OS_Start_First_Task_Handler


/*-----------------------------------------------------------*/

/*
vSetMSP
	msr msp, r0
	bx lr
*/
Disable_Ints_ASM	MACRO
	mov r0, #MAX_SYSCALL_INTERRUPT_BASE_PRIORITY	//MAX_SYSCALL_INTERRUPT_BASE_PRIORITY
	msr basepri, r0
//	mov r0, #1
//	msr primask, r0
//	CPSID I
	ENDM

Enable_Ints_ASM	MACRO
	mov r0, #0
	msr basepri, r0
//	msr primask, r0
//	CPSIE I
	ENDM

/*-----------------------------------------------------------*/

asmPendSVHandler:
    mrs r0, psp
    ldr	r3, =OS_TASKER_current_task	/* Get the location of the current TCB. */
    ldr	r2, [r3]

    stmdb r0!, {r4-r11}				/* Save the remaining registers. */
    str r0, [r2]        /* Save the new top of stack into TCB first member. */

    stmdb sp!, {r3, r14} /* r3 contains LOCATION of the current TCB */

    /* disable all interrupts with lower priorities */
	Disable_Ints_ASM
//    mov r0, #MAX_SYSCALL_INTERRUPT_PRIORITY
//    msr basepri, r0

    /* changed to call the C++ context switch function */
    ldr r0, =OS_TASKER_Switch_Context
    ldr r0, [r0]
    blx r0
    /* after the function call the OS_TASKER_current_task content changes */

    /* bl OS_TASKER_Switch_Context */
    /* changes end here */

    /* re-enable interrupts */
	Enable_Ints_ASM
//    mov r0, #0
//    msr basepri, r0

    ldmia sp!, {r3, r14}

    ldr r1, [r3]
    ldr r0, [r1]    /* The first item in TCB is task top of stack. */
    ldmia r0!, {r4-r11}				/* Pop the registers. */
    msr psp, r0	/* stack ready for the NEWLY SWAPPED-IN task */
    bx r14


/*-----------------------------------------------------------*/

/* mask/clear only affect interrupts with lower priorities */
asmSetInterruptMask:
	push { r0 }
	Disable_Ints_ASM
//	mov R0, #MAX_SYSCALL_INTERRUPT_PRIORITY
//	msr BASEPRI, R0
	pop { R0 }

	bx r14

/*-----------------------------------------------------------*/

asmClearInterruptMask:
	PUSH { r0 }
//	MOV R0, #0
//	MSR BASEPRI, R0
	Enable_Ints_ASM
	POP	 { R0 }

	bx r14

/*-----------------------------------------------------------*/

asmSVCHandler;
    /* similar to the second half of PendSV handler */
	ldr	r3, =OS_TASKER_current_task
	ldr r1, [r3]
	ldr r0, [r1]
	ldmia r0!, {r4-r11}
	msr psp, r0
	Enable_Ints_ASM
//	mov r0, #0
//	msr	basepri, r0
	orr r14, r14, #13 /* EXEC_RETURN changes from 0xFFFFFFF9 to 0xFFFFFFFD */
	bx r14

/*-----------------------------------------------------------*/

OS_Start_First_Task_Handler
	/* Use the NVIC offset register to locate the stack. */
    /* 0xE000ED08: vector table offset register */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]
	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Call SVC to start the first task. */
	svc 0

	END

