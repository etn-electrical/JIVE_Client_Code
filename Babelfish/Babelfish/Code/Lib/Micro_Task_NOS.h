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
 *   @deprecated : The micro_task is no longer used and is deprecated.
 *****************************************************************************************
 */
#ifndef MICRO_TASK_H
   #define MICRO_TASK_H

#define OPERATING_SYSTEM
/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "uC_Watchdog.h"
#ifdef OPERATING_SYSTEM
	#include "OS_Tasker.h"
#else
	#include "uC_Interrupt.h"
	#define OS_TASK_PARAM   void*
#endif

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define         MICRO_TASK_DEFAULT_OS_STACK_SIZE        100

typedef void* MICRO_TASK_FUNC_PARAM_TD;
typedef void (MICRO_TASK_FUNC_TD)( MICRO_TASK_FUNC_PARAM_TD param );

typedef struct task
{
	MICRO_TASK_FUNC_TD* task_func;
	MICRO_TASK_FUNC_PARAM_TD task_param;
	struct task* next;
} MICRO_TASK_STRUCT_TD;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Micro_Task
{
	public:
		Micro_Task( uint16_t micro_task_os_stack_size = MICRO_TASK_DEFAULT_OS_STACK_SIZE );
		~Micro_Task();

		static void Init( void );

		static void Add_Task( MICRO_TASK_FUNC_TD* task_to_add, MICRO_TASK_FUNC_PARAM_TD task_param = NULL );

		static uint8_t Destroy_Task( MICRO_TASK_FUNC_TD* task_to_destroy, MICRO_TASK_FUNC_PARAM_TD task_param = NULL );

		static void Shutdown( void );

		static bool Is_uTask_Running( void );

		static uint8_t Get_uTask_Stack_Usage( void );

#ifndef OPERATING_SYSTEM
		static void Run( OS_TASK_PARAM param );

#endif

	private:
#ifdef OPERATING_SYSTEM
		static void Run( OS_TASK_PARAM param );

#endif
		static void Run_Fallback( OS_TASK_PARAM param );		// This is run if the Run task gets suspended.

		static void Last_Task( MICRO_TASK_FUNC_PARAM_TD task_param );

		static void Dummy_Task( MICRO_TASK_FUNC_PARAM_TD task_param )   {}

		static uint8_t Find_Task( MICRO_TASK_FUNC_TD* task_to_find, MICRO_TASK_FUNC_PARAM_TD task_param = NULL );

		static bool m_tasks_running;
		static MICRO_TASK_STRUCT_TD m_tasks[];
		static uint8_t m_task_count;

		static uC_Watchdog* m_task_watchdog_bone;
		static uint8_t m_task_max;
#ifdef OPERATING_SYSTEM
		static OS_Task* m_os_task;
		static OS_Task* m_os_fallback_task;				// This is fallen into if the Run Task suspends.
#endif
};


#endif
