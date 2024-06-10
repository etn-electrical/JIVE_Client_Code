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
#ifndef OS_INTERRUPT_ASM_H
   #define OS_INTERRUPT_ASM_H

;*****************************************************************************************
;* ----- Include Files -------------------------------------------------------------------
;*****************************************************************************************
#include "uC_Base_HW_IDs.h"


;*****************************************************************************************
;* ----- Constants -----------------------------------------------------------------------
;*****************************************************************************************
#define OS_INT_current_stack_ptr		OS_TASKER_current_task


;*****************************************************************************************
;* ----- Macro Header --------------------------------------------------------------------
;* ---------------------------------------------------------------------------------------
;*****************************************************************************************

OS_Context_Save_Macro	MACRO

	; Push R0 as we are going to use the register.
	STMDB	SP!, {R0}		//iSP -=4; *iSP = R0;

	; Set R0 to point to the task stack pointer.
	STMDB	SP, {SP}^		//iSP[-4] = tSP;
	NOP
	SUB		SP, SP, #4		//iSP -= 4;
	LDMIA	SP!, {R0}		//R0 = *iSP; iSP += 4;

	; Push the return address onto the stack.
	STMDB	R0!, {LR}		//R0(tSP) -= 4; *R0(tSP) = LR;

	; Now we have saved LR we can use it instead of R0.
	MOV		LR, R0			//LR = R0(tSP);

	; Pop R0 so we can save it onto the system mode stack.
	LDMIA	SP!, {R0}		//R0 = *iSP; iSP += 4;

	; Push all the system mode registers onto the task stack.
	STMDB	LR, {R0-LR}^	//*LR(tSP)[-4*15(60)] = tR0-tLR;
	NOP
	SUB		LR, LR, #60		//LR(tSP) -= 60;

	; Push the SPSR onto the task stack.
	MRS		R0, SPSR		//R0(tSP) = SPSR;
	STMDB	LR!, {R0}

	LDR		R0, =OS_INT_critical_nesting
	LDR		R1, [R0]
	STMDB	LR!, {R1}

	MOV		R1, #32000		//Update the OS_INT_critical_nesting with a high value in case someone tries to use critical sections.
	STR		R1, [R0]

	; Store the new top of stack for the task.
	LDR		R1, =OS_INT_current_stack_ptr
	LDR		R0, [R1]
	STR		LR, [R0]

	ENDM


;*****************************************************************************************
;* ----- Macro Header --------------------------------------------------------------------
;* ---------------------------------------------------------------------------------------
;*****************************************************************************************

OS_Context_Restore_Macro	MACRO

	; Set the LR to the task stack.
	LDR		R1, =OS_INT_current_stack_ptr
	LDR		R0, [R1]
	LDR		LR, [R0]

	; The critical nesting depth is the first item on the stack.
	; Load it into the ulCriticalNesting variable.
	LDR		R0, =OS_INT_critical_nesting
	LDMFD	LR!, {R1}
	STR		R1, [R0]

	; Get the SPSR from the stack.
	LDMFD	LR!, {R0}
	MSR		SPSR_cxsf, R0

	; Restore all system mode registers for the task.
	LDMFD	LR, {R0-R14}^
	NOP

	; Restore the return address.
	LDR		LR, [LR, #+60]

	; And return - correcting the offset in the LR to obtain the
	; correct address.
	SUBS	PC, LR, #4

	ENDM



#endif
