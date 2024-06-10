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
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task::OS_Task( OS_TASK_FUNC* task_func, uint16_t stack_size_in_words,
				  uint8_t priority, uint8_t const* task_name, OS_TASK_PARAM init_task_param ) :
	m_task( nullptr )
{
	m_word_stack_size = stack_size_in_words;
	xTaskCreate( task_func, reinterpret_cast<const char*>( task_name ), stack_size_in_words, init_task_param,
				 priority, &m_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_Task::~OS_Task( void )
{
	m_task = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t OS_Task::Stack_Usage_Percent( void )
{
	int32_t difference;
	uint32_t percent_used;
	uint8_t return_result;

#if defined ESP32_SETUP || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
	// Note that this is the correct form when called from an ISR
	UBaseType_t usStackHighWaterMark;

	// Use the handle to obtain stack usage of the task.
	usStackHighWaterMark = uxTaskGetStackHighWaterMark( m_task );

	difference = static_cast<uint32_t>( m_word_stack_size - usStackHighWaterMark );

#else
	TaskStatus_t xTaskDetails;

	// Use the handle to obtain further information about the task.
	vTaskGetInfo( m_task,
				  &xTaskDetails,
				  pdTRUE,	// Include the high water mark in xTaskDetails.
				  eInvalid );	// Include the task state in xTaskDetails.

	difference = static_cast<int32_t>( m_word_stack_size - xTaskDetails.usStackHighWaterMark );
#endif	// #ifdef ESP32_SETUP
	percent_used = difference * 100;
	percent_used = percent_used / m_word_stack_size;
	return_result = static_cast<uint8_t>( percent_used & 0xFFU );

	return ( return_result );
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
