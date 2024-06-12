/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OS_Tasker.h"
#include "OS_Task.h"
#include "Ram.h"

/*
 *****************************************************************************
 *		Constants
 *****************************************************************************
 */
#define OS_TASK_USE_FAST_STACK_CHECK
#define OS_TASK_STACK_FILL_VAL          0xA5U
#define OS_TASK_STACK_INIT_XPSR         0x01000000U
#ifdef FPU_SUPPORT
#define OS_TASK_INITIAL_EXEC_RETURN     0xfffffffd
#endif

#ifndef OS_TASK_STACK_RAM_BLOCK
	#define OS_TASK_STACK_RAM_BLOCK         MAIN_RAM_BLOCK
#endif

// essentially verify that 8 bytes of the end of the stack have not been written over.
#ifndef OS_TASK_USE_FAST_STACK_CHECK
	#define OS_STACK_VERIFICATION_SAMPLES       8U
#endif

#if PROCESSOR_REGISTER_BASE_SIZE == 1U
const OS_STACK_TD OS_TASK_STACK_CHECK_VALUE = OS_TASK_STACK_FILL_VAL;
#elif PROCESSOR_REGISTER_BASE_SIZE == 2U
const OS_STACK_TD OS_TASK_STACK_CHECK_VALUE =
	( ( OS_TASK_STACK_FILL_VAL << 8 ) | OS_TASK_STACK_FILL_VAL );
#elif PROCESSOR_REGISTER_BASE_SIZE == 4U
const OS_STACK_TD OS_TASK_STACK_CHECK_VALUE =
	( ( static_cast<OS_STACK_TD>
		( OS_TASK_STACK_FILL_VAL ) << 24 ) |
	  ( static_cast<OS_STACK_TD>( OS_TASK_STACK_FILL_VAL ) << 16 ) |
	  ( static_cast<OS_STACK_TD>( OS_TASK_STACK_FILL_VAL ) << 8 ) | static_cast<OS_STACK_TD>
	  ( OS_TASK_STACK_FILL_VAL ) );
#endif

/*
 *****************************************************************************
 *		Variables
 *****************************************************************************
 */
#ifdef OS_TASK_MEASURE_RUN_TIME
uint32_t OS_Task::m_priority_time[OS_TASK_PRIORITY_MAX];
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task::OS_Task( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
				  uint8_t priority, uint8_t const* task_name, OS_TASK_PARAM init_task_param ) :
	m_top_of_stack( nullptr ),
	m_start_of_stack( nullptr ),
	m_stack_threshold( nullptr ),
	m_end_of_stack( nullptr ),
	m_word_stack_size( 0U ),
	m_task_name( nullptr ),
	m_priority( 0U ),
#if CONFIG_USE_MUTEXES == 1U
	m_base_priority( 0U ),
#endif
	m_general_list_item( nullptr ),
	m_event_list_item( nullptr )
#ifdef OS_TASK_MEASURE_RUN_TIME
	,
	m_max_run_time( 0U ),
	m_min_run_time( 0U ),
	m_total_run_time( 0U ),
	m_times_run( 0U ),
	m_max_stack_usage_percent( 0U )
#endif
	,
	m_context_param( nullptr ),
	m_context_cback( nullptr )
{

	m_task_name = task_name;
	Init_Stack_Space( task_func, stack_size_in_words, init_task_param );

	if ( priority >= OS_TASK_PRIORITY_MAX )
	{
		m_priority = static_cast<BF_Lib::MBASE>( OS_TASK_PRIORITY_MAX - 1U );
	}
	else
	{
		m_priority = static_cast<BF_Lib::MBASE>( priority );
	}

#if CONFIG_USE_MUTEXES == 1U
	m_base_priority = m_priority;
#endif

	m_general_list_item = new OS_List_Item();

	m_event_list_item = new OS_List_Item();

	m_general_list_item->Set_Owner( this );
	m_event_list_item->Set_Owner( this );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task::~OS_Task( void )
{
	delete m_general_list_item;
	delete m_event_list_item;

	Ram::De_Allocate( m_end_of_stack );
	m_top_of_stack = nullptr;
	m_start_of_stack = nullptr;
	m_stack_threshold = nullptr;
	m_end_of_stack = nullptr;
	m_task_name = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task::Init_Stack_Space( OS_TASK_FUNC* task_func,
								uint16_t stack_size_in_words, OS_TASK_PARAM init_task_param )
{
	m_word_stack_size = stack_size_in_words;

	m_end_of_stack = reinterpret_cast<OS_STACK_TD*>( Ram::Allocate( ( m_word_stack_size
																	  * sizeof( OS_STACK_TD ) ),
																	true, OS_TASK_STACK_FILL_VAL,
																	static_cast<uint8_t>
																	( OS_TASK_STACK_RAM_BLOCK ) ) );

	// because the processor works from the high end of the stack space we need to send the
	// pointer up to that point instead of the bottom.
	// This is converted into a pointer which means that all the math is now whatever
	// the processor register size is.
	m_start_of_stack = m_end_of_stack + ( m_word_stack_size - 1U );
	m_start_of_stack = reinterpret_cast<OS_STACK_TD*>(
		reinterpret_cast<BF_Lib::MBASE>( m_start_of_stack ) & CORTEX_M_STACK_ALIGNMENT_MASK );

	m_top_of_stack = m_start_of_stack;

	// Setup the initial stack of the task.  The stack is set exactly as
	// expected by the restoring the context in the interrupt.

	// modified for Cortex-M3
	m_top_of_stack--;
	*m_top_of_stack = OS_TASK_STACK_INIT_XPSR;	// xPSR

	m_top_of_stack--;
	*m_top_of_stack = reinterpret_cast<OS_STACK_TD>( task_func );	// PC

	m_top_of_stack--;
	*m_top_of_stack = 0U;	// LR

	m_top_of_stack -= 5U;	// R12, R3, R2 and R1
	*m_top_of_stack = reinterpret_cast<OS_STACK_TD>( init_task_param );

#ifdef FPU_SUPPORT
	/* A save method is being used that requires each task to maintain its
	   own exec return value. */
	m_top_of_stack--;
	*m_top_of_stack = OS_TASK_INITIAL_EXEC_RETURN;
#endif
	m_top_of_stack -= 8U;	// R11, R10, R9, R8, R7, R6, R5 and R4

#ifndef OS_TASK_STACK_THRESHOLD_PERCENT
	m_stack_threshold = m_end_of_stack + 4U;
#else
  #if ( OS_TASK_STACK_THRESHOLD_PERCENT >= 100U )
	m_stack_threshold = m_end_of_stack + 4U;
  #else
	m_stack_threshold = m_end_of_stack + 4U +
		( ( m_word_stack_size * ( 100U - OS_TASK_STACK_THRESHOLD_PERCENT ) ) / 100U );
  #endif
#endif

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t OS_Task::Stack_Usage_Percent( void )
{
	OS_STACK_TD* stack_runner;
	uint8_t test_val;
	int32_t difference;
	uint32_t percent_used;
	uint8_t return_result;

	stack_runner = m_end_of_stack;	// ( m_start_of_stack + 1 ) - m_word_stack_size;

	// if ( ((UINT32)stack_runner < 0x00200000 ) || ( (UINT32)stack_runner > 0x00207FFF ) )  //For
	// capturing task structure errors.
	// {
	// BF_ASSERT(false);
	// }

	test_val = *( reinterpret_cast<uint8_t*>( stack_runner ) );
	while ( ( stack_runner < m_start_of_stack ) && ( test_val == OS_TASK_STACK_FILL_VAL ) )
	{	// ( *stack_runner == OS_TASK_STACK_FILL_VAL ) )
		stack_runner++;
		test_val = *( reinterpret_cast<uint8_t*>( stack_runner ) );
	}

	difference = ( m_start_of_stack + 1 ) - stack_runner;
	percent_used = static_cast<uint32_t>( difference );
	percent_used = percent_used * 100U;

	percent_used = percent_used / m_word_stack_size;

	return_result = static_cast<uint8_t>( percent_used & 0xFFU );
#ifdef OS_TASK_MEASURE_RUN_TIME
	m_max_stack_usage_percent = return_result;
#endif
	return ( return_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool OS_Task::Stack_Good( void ) const
{
	bool stack_valid;

#ifdef OS_TASK_USE_FAST_STACK_CHECK
	if ( ( m_stack_threshold[1] == OS_TASK_STACK_CHECK_VALUE ) &&
		 ( m_stack_threshold[2] == OS_TASK_STACK_CHECK_VALUE ) &&
		 ( m_stack_threshold[3] == OS_TASK_STACK_CHECK_VALUE ) )
	{
		stack_valid = true;
	}
	else
	{
		stack_valid = false;
	}
#else
	uint8_t* read_data;

	read_data = ( uint8_t* )( m_end_of_stack + 1 );

	uint_fast16_t read_index = 0U;
	while ( ( read_index < OS_STACK_VERIFICATION_SAMPLES ) &&
			( read_data[read_index] == OS_TASK_STACK_FILL_VAL ) )
	{
		read_index++;
	}

	if ( read_index != OS_STACK_VERIFICATION_SAMPLES )
	{
		stack_valid = false;
	}
	else
	{
		stack_valid = true;
	}

	////This all gets dicey because the pointers are 4bytes and therefore don't check out correctly.
	// if ( ( m_top_of_stack < ( m_start_of_stack - m_word_stack_size ) ) ||
	// ( m_top_of_stack > m_start_of_stack ) )
	// {
	// stack_valid = false;
	// }
#endif

	return ( stack_valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Task::Context_Action_Cback( context_action_func_t* context_cback,
									context_action_param_t context_param )
{
	m_context_param = context_param;
	m_context_cback = context_cback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef OS_TASK_MEASURE_RUN_TIME
void OS_Task::Process_Run_Time( BF_Lib::Timers_uSec::TIME_TD run_time )
{
	m_times_run++;

	m_total_run_time += run_time;

	if ( static_cast<uint16_t>( run_time ) > m_max_run_time )
	{
		m_max_run_time = static_cast<uint16_t>( run_time );
	}
	if ( static_cast<uint16_t>( run_time ) < m_min_run_time )
	{
		m_min_run_time = static_cast<uint16_t>( run_time );
	}

	m_run_time.Value( run_time );

	m_priority_time[m_priority] += run_time;
}

#endif
