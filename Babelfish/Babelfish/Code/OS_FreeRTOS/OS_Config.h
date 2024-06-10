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
#ifndef OS_CONFIG_H
#define OS_CONFIG_H


#ifndef FREERTOS_V10
#error Missing Definition: FREERTOS_V10 must be defined
#endif

#include "Device_Config.h"
/**
 * The following define should include some code to verify that the stack has not been
 * corrupted before and after a context switch.  It also includes some additional defines
 * which can help measure and diagnose the RTOS and CR_Tasker.  It is recommended that you
 * define these in the Device_Config.h file.  This will do it just for you.
 */
#ifdef OS_TASKER_DEBUG
/**
 * @brief Selects .
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
// #define OS_TASKER_VERIFY_TASK_STACK
/**
 * @brief Selects .
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
// #define OS_TASK_MEASURE_RUN_TIME

/**
 *  @brief This will print the priority level reports to the terminal.
 */
// #define OSCR_MON_PRINT_PRIORITY_LEVEL_TIMES

/**
 *  @brief The following define will enable a per task timing reporting in OSCR_MON.  Otherwise if you init
 *  OSCR_Mon you will just get the per priority printout of timing every 10seconds.
 */
// #define OSCR_MON_PRINT_TASK_DETAILS

/**
 * @brief The following define will only function if OSCR_MON_PRINT_TASK_DETAILS is defined.
 * This define will enable more information regarding the tasks including total times run
 * the pointer assigned to the task as well as the min run time.
 */
// #define OSCR_MON_PRINT_ADVANCED_TASK_DETAILS

/**
 * @brief Will enable a trace log of tasks running.  Every task switch, the task
 * gets added to a log list.  If the same task is re-activated then it will count how many
 * times it gets reactivated.
 * @details Defined enables feature, undefined disables feature.
 */
// #define OS_TASK_MON_USE_LOGGING
#endif

/**
 * @brief Frequency of CPU clock in hertz.
 */
#define CONFIG_CPU_CLOCK_HZ             MASTER_CLOCK_FREQUENCY

/**
 * @brief Tick frequency in hertz.
 */
#define OS_TICK_FREQUENCY_HERTZ         1000U

/**
 * @brief Tick period in milliseconds
 */
#define OS_TICK_RATE_MS                 ( 1000U / OS_TICK_FREQUENCY_HERTZ )

/**
 * @brief Selects whether to use 16-bit tick counter.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_USE_16_BIT_TICKS         0U

/**
 * @brief Selects whether to use tick hook function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_USE_TICK_HOOK            1U

/**
 * @brief Selects whether idle should yield.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
// #define CONFIG_IDLE_SHOULD_YIELD  1

/**
 * @brief Selects whether to use mutex feature.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
// if CONFIG_USE_RECURSIVE_MUTEXES is set as 1,
// CONFIG_USE_MUTEXES MUST be set as 1 too
#define CONFIG_USE_MUTEXES              1U
/**
 * @brief Selects whether to use recursive mutex feature.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_USE_RECURSIVE_MUTEXES    1U

/**
 * @brief Selects whether to use counting semaphores.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_USE_COUNTING_SEMAPHORES  1U

/**
 * @brief Selects whether to use task priority set function..
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_PRIORITY_SET    1U
/**
 * @brief Selects whether to use task priority get function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_PRIORITY_GET    1U
/**
 * @brief Selects whether to use task delete function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_DELETE          1U
/**
 * @brief Selects whether to use task suspend function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_SUSPEND         1U
/**
 * @brief Selects whether to use task delay function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_DELAY           1U
/**
 * @brief Selects whether to use task delay until function.
 * @details Defined as any non-zero value enables feature, undefined or zero value disables feature.
 */
#define CONFIG_INCLUDE_TASK_DELAY_UNTIL     1U

/**
 * @brief Default minimum stack size (number of 4-byte words).
 * @details Used for the Idle Task
 */
#if 0	// Increase stack size to min 256 if RAM_TERM_PRINT is enabled
	#define CONFIG_MINIMAL_STACK_SIZE           256U
#else
	#define CONFIG_MINIMAL_STACK_SIZE           70U
#endif

/**
 * @brief Maximum length of task name string.
 */
#define CONFIG_MAX_TASK_NAME_LEN            16U

/**
 * @brief Maximum Priority of task.
 */
#define CONFIG_MAX_PRIORITIES                   8U

#endif	// OS_CONFIG_H
