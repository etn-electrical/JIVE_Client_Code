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
#ifndef uC_OS_DEVICE_DEFINES_H
#define uC_OS_DEVICE_DEFINES_H

// configuration is mostly done here:
#include "OS_Config.h"

#ifdef __cplusplus
extern "C" {
#endif
// simpler than critical section entry/exit, does not track nesting
// mostly used in interrupts
// only masks interrupts w/ lower priorities than MAX_SYSCALL_INTERRUPT_PRIORITY
extern void asmSetInterruptMask();

extern void asmClearInterruptMask();

#ifdef __cplusplus
}
#endif


#define OS_Disable_Interrupts()         asmSetInterruptMask()
#define OS_Enable_Interrupts()          asmClearInterruptMask()
#define OS_In_Exception()               ( __get_IPSR() != 0 )

// used when a higher priority task is ready in an ISR
#define END_SWITCHING_ISR( xSwitchRequired ) \
	do {if ( xSwitchRequired )OS_Tasker::Yield();} while ( 0 )

/*
 *****************************************************************************
 *		Define checks
 *****************************************************************************
 */
#ifndef CONFIG_USE_TICK_HOOK
#error Missing Definition: CONFIG_USE_TICK_HOOK must be defined
#endif

/*
 #ifndef CONFIG_IDLE_SHOULD_YIELD
 #define CONFIG_IDLE_SHOULD_YIELD 0
 #endif
 */

#ifndef CONFIG_USE_MUTEXES
#define CONFIG_USE_MUTEXES 0
#endif

#ifndef CONFIG_USE_RECURSIVE_MUTEXES
#define CONFIG_USE_RECURSIVE_MUTEXES 0
#endif

#ifndef CONFIG_USE_COUNTING_SEMAPHORES
#define CONFIG_USE_COUNTING_SEMAPHORES 0
#endif

#ifndef CONFIG_USE_ALTERNATIVE_API
#define CONFIG_USE_ALTERNATIVE_API 0
#endif

#ifndef CONFIG_INCLUDE_TASK_PRIORITY_SET
#error Missing Definition: CONFIG_INCLUDE_TASK_PRIORITY_SET must be defined
#endif

#ifndef CONFIG_INCLUDE_TASK_PRIORITY_GET
#error Missing Definition: CONFIG_INCLUDE_TASK_PRIORITY_GET must be defined
#endif

#ifndef CONFIG_INCLUDE_TASK_DELETE
#error Missing Definition: CONFIG_INCLUDE_TASK_DELETE must be defined
#endif

#ifndef CONFIG_INCLUDE_TASK_SUSPEND
#error Missing Definition: CONFIG_INCLUDE_TASK_SUSPEND must be defined
#endif

#ifndef CONFIG_INCLUDE_TASK_DELAY
#error Missing Definition: CONFIG_INCLUDE_TASK_DELAY must be defined
#endif

#ifndef CONFIG_INCLUDE_TASK_DELAY_UNTIL
#error Missing Definition: CONFIG_INCLUDE_TASK_DELAY_UNTIL must be defined
#endif

#ifndef CONFIG_USE_16_BIT_TICKS
#error Missing Definition: CONFIG_USE_16_BIT_TICKS must be defined
#endif

#if ( CONFIG_USE_MUTEXES == 0 ) && ( CONFIG_USE_RECURSIVE_MUTEXES == 1 )
#error if CONFIG_USE_RECURSIVE_MUTEXES==1, CONFIG_USE_MUTEXES must be 1 too
#endif

#if CONFIG_USE_16_BIT_TICKS == 1
typedef uint16_t OS_TICK_TD;
#else
typedef uint32_t OS_TICK_TD;
#endif

/// Processor optimized typedefs.
#if PROCESSOR_REGISTER_BASE_SIZE == 4
typedef uint32_t OS_STACK_TD;
#elif PROCESSOR_REGISTER_BASE_SIZE == 2
typedef uint16_t OS_STACK_TD;
#elif PROCESSOR_REGISTER_BASE_SIZE == 1
typedef uint8_t OS_STACK_TD;
#endif

//// these needs to be included wherever the customized new/delete is used
// #include "Ram_Handler.h"
// #include "Ram.h"
// #include "Ram_Malloc.h"


#endif
