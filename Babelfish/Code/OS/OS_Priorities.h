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
#ifndef OS_PRIORITIES_H
#define OS_PRIORITIES_H

/* Priority 15, or 255 as only the top four bits are implemented.
 * This is the lowest priority. Used by
 * PendSV and SysTick */
/**
 * @brief Kernel interrupt priority.
 */
#define KERNEL_INTERRUPT_PRIORITY   255

/* Priority level 8 (shifted), as only the top four bits are implemented.
 * Any interrupts with a higher priority shall not call "FromISR"
 * functions to protect kernel data structures while enabling
 * interrupt nesting */
// #define MAX_SYSCALL_INTERRUPT_PRIORITY 		0x80		//0xA0

#endif	// OS_PRIORITIES_H
