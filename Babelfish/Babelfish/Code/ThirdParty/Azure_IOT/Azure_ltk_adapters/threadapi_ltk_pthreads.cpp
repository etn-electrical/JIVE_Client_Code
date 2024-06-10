// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define _DEFAULT_SOURCE
#define OS_SSL_TASK_FALLBACK_STACK_SIZE 1000

#include "IOT_Config.h"
#include "iot_device_platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/xlogging.h"
#include <stdint.h>
#include <stdlib.h>
// #include <errno.h>
#include "lwip/sys.h"

#include "CR_Tasker.h"
#include "OS_Tasker.h"
#include "TLS_Ram.h"
// #include <time.h>
#include "Babelfish_Assert.h"


MU_DEFINE_ENUM_STRINGS( THREADAPI_RESULT, THREADAPI_RESULT_VALUES );

/** Holds the pointer to the OS Task that we create for managing the messageThread function. */
OS_Task * m_thread_handle = nullptr;
/** Pointer to the messageThread function, which will be given to us by the Eaton IoT SDK. */
THREAD_START_FUNC m_iot_function = nullptr;
/** Our flag to indicate whether the task is "active", and the messageThread function should be called. */
volatile bool m_task_active = false;
/** Holds the opaque argument to be given to the messageThread function. 
 * (In practice, this is the IoT Connection structure.) */
void * m_task_arg = nullptr;

/** The implementation of the OS Task that manages the IoT SDK's messageThread worker function.
 * This function is a simple loop that calls the messageThread worker function (if it is "active"),
 * then waits for IOT_MESSAGE_SLEEP_MS, and then repeats itself.
 * Once created, this OS Task does not exit.
 * @param params Unused task parameter
 */
void IOT_Task ( void * /* params */ )
{
  while ( 1 )
  {
		if ( ( m_iot_function != nullptr ) && ( m_task_active == true ) )
		{
			m_iot_function ( m_task_arg );
		}
		// Now sleep, unless awoken early.
		platform_message_thread_sleep();
	}
}

/** Interface for Azure and SDK operation intended for pthread-like creation.
 * However, our operation here is
 *   a) To create an OS Task, not a thread
 *   b) To only create this OS Task one time (and never destroy it)
 *   c) To update the argument provided each time, since the messageThread function 
 * 		needs this refreshed upon each new connection (since it points to the Connection structure)
 *   d) If ever this is called again by the Azure code for some other function, do a Stop_Here() Exception.
 * 		(We don't believe that this can ever happen, but guarding against it.)
 * 
 * Will use sys_thread_new(), which resolves to  OS_Tasker::Create_Task().
 * @param[out] threadHandle Holds a pointer to the "thread handle" (aka "ID") that is created;
 * 				this is actually the OS Task instance; it is not used or needed by the IoT SDK.
 * @param func The task function for the new thread (here, points to the messageThread worker function)
 * @param arg An opaque pointer to an argument for the task function, which is updated upon each new connection.
 * @return The result, THREADAPI_OK on success, or (very unlikely) THREADAPI_NO_MEMORY.
 */
THREADAPI_RESULT ThreadAPI_Create( THREAD_HANDLE* threadHandle, THREAD_START_FUNC func, void* arg )
{
	THREADAPI_RESULT result = THREADAPI_OK;

	if ( m_thread_handle == nullptr )
	{
		m_iot_function = func;
		m_thread_handle = reinterpret_cast<OS_Task*>( sys_thread_new( "IoT messageThread", 
													( OS_TASK_FUNC* )IOT_Task, nullptr,
													OS_EIOT_SDK_TASK_STACK_SIZE,
													static_cast<uint8_t>( OS_TASK_PRIORITY_2 ) ) );
		if ( m_thread_handle == NULL )
		{
			result = THREADAPI_NO_MEMORY;
			LogError( "(result = %s)", MU_ENUM_TO_STRING( THREADAPI_RESULT, result ) );
		}
        else
        {
            LogInfo( "Opened messageThread task at 0x%p", m_thread_handle );
        }
	}
	
	if ( func != m_iot_function )
	{
		LogError( "Unexpected attempt by Azure to create a new thread." );
		BF_ASSERT( false );
	}
	else
	{
		// Return this handle, for the sake of form, though the SDK doesn't really need to use it.
		*threadHandle = m_thread_handle;
		// Update our pointer to the connection instance
		m_task_arg = arg;
		// Set the flag that indicates that messageThread function is ready to be called
		m_task_active = true;
	}

	return ( result );
}

/** Join (end) the given task / "thread".
 * But here we merely clear the "active" flag, so the messageThread function won't be called.
 * @param threadHandle (Unused) The thread handle returned by ThreadAPI_Create(). Points to this task instance.
 * @param res Meant to be the thread result. Unused here.
 * @return Result of the Join operation, always THREADAPI_OK.
 */
THREADAPI_RESULT ThreadAPI_Join( THREAD_HANDLE threadHandle, int* res )
{
	THREADAPI_RESULT result = THREADAPI_OK;
	LogInfo( "Setting the IoT OS Task to inactive" );
	m_task_active = false;
	return ( result );
}

/** Exit from "this" task/thread.
 * But here we merely clear the "active" flag, so the messageThread function won't be called.
 * @param res Meant to be the result given as this "thread" exits. Unused here.
 */
void ThreadAPI_Exit( int /* res */ )
{
	LogInfo( "Setting 'this' IoT OS Task to inactive" );
	m_task_active = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ThreadAPI_Sleep( unsigned int milliseconds )
{
	// CR_Tasker_Delay( cr_task, milliseconds );
	OS_Tasker::Delay( milliseconds );
}
