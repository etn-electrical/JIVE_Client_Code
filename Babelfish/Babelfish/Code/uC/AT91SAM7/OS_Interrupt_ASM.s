;*****************************************************************************************
;*
;*		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
;*
;*****************************************************************************************



;*****************************************************************************************
;*		Includes
;*****************************************************************************************
#include "OS_Interrupt_ASM.h"

;*****************************************************************************************
;* ----- Function Header -----------------------------------------------------------------
;* ---------------------------------------------------------------------------------------
;*****************************************************************************************
    SECTION .text:CODE:NOROOT(2)
	CODE32

	EXTERN OS_TASKER_current_task		; Data
	EXTERN OS_INT_critical_nesting		; Data
	EXTERN OS_INT_array				; Data

	EXTERN OS_TASKER_Switch_Context	; Code
	EXTERN OS_TASKER_Tick_Handler		; Code

	PUBLIC OS_OS_Int_Wrapper
	PUBLIC OS_Yield_Int_Handler
	PUBLIC OS_Tick_Int_Handler

OS_OS_Int_Wrapper:

	OS_Context_Save_Macro

	;****************************
	; Put crap here.
	;----------------------------
	;call OS_Interrupt_Array[AIC int Source Id];
	LDR		R0, =OS_INT_array
	LDR		R1, =AT91C_AIC_ISR
	LDR		R1, [R1]
	ADDS	R0, R0, R1, LSL #+2
	LDR		R0, [R0]
	MOV		LR, PC
	BX		R0

	LDR 	R0, =AT91C_AIC_EOICR	; Mark the End of Interrupt on the AIC
    STR		R0, [R0]

	;----------------------------
	; End crap here.
	;****************************

	OS_Context_Restore_Macro
	;Should never pass the above point because the context switch does the calling.

;*****************************************************************************************
;* ----- Function Header -----------------------------------------------------------------
;* ---------------------------------------------------------------------------------------
;*****************************************************************************************
OS_Yield_Int_Handler:
	ADD		LR, LR, #4			; Add 4 to the LR to make the LR appear exactly
								; as if the context was saved during the IRQ
								; handler.

	OS_Context_Save_Macro		; Save the context of the current task...
								; before selecting the next task to execute.

	LDR 	R0, =OS_TASKER_Switch_Context
	LDR 	R0, [R0]
	MOV     LR, PC
	BX 		R0

	OS_Context_Restore_Macro
	;Should never pass the above point because the context switch does the calling.


;*****************************************************************************************
;* ----- Function Header -----------------------------------------------------------------
;* ---------------------------------------------------------------------------------------
;*****************************************************************************************
OS_Tick_Int_Handler:
	OS_Context_Save_Macro				; Save the context of the current task.

	LDR 	R0, =OS_TASKER_Tick_Handler 	; Increment the tick count - this may wake a task.
	LDR 	R0, [R0]
	MOV 	LR, PC
	BX 		R0
	LDR 	R0, =OS_TASKER_Switch_Context ; Select the next task to execute.
	LDR 	R0, [R0]
	MOV 	LR, PC
	BX 		R0

	LDR		R14, =AT91C_PITC_PIVR	; Clear the PIT interrupt
	LDR		R0, [R14]

	LDR 	R14, =AT91C_AIC_EOICR	; Mark the End of Interrupt on the AIC
    STR		R14, [R14]

	OS_Context_Restore_Macro
	;Should never pass the above point because the context switch does the calling.

	;This is the end of this.
	END
