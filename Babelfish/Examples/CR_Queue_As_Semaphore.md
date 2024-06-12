@example  CR_Queue_As_Semaphore
@brief    The sample code shows the usage of CR_Queue as Semaphore. The semaphore API like macros are 
@n		  used for guarding critical sections on two tasks. The semaphore token is made available from
@n		 Timer ISR after some time delay.
 
@par Board Specific Settings  

@details  This example is tested on Nucleo-429ZI board. 
@n		  This example uses LED_EXAMPLE_TIMER. So the user needs to disable the use of this timer
@n		  for LED blinking. To run this example, code in this example can be copied to Prod_Spec_APP.cpp. 
@n		  The debug output for this program is given below.
@n
@n		  4 Callback_Function Timer ISR: No Task is woken by Bin_Sem2 Post 
@n		  4 Callback_Function Timer ISR: Binary_Sem2 Taken 
@n		  4 Callback_Function Timer ISR: No Task is woken by Bin_Sem2 Post 
@n		  4 Callback_Function Timer ISR: Binary_Sem2 Taken 
@n		  4 Callback_Function Timer ISR: No Task is woken by Bin_Sem2 Post 
@n		  4 Callback_Function Timer ISR: Binary_Sem2 Taken 
@n		  4 Callback_Function Timer ISR: No Task is woken by Bin_Sem2 Post 
@n		  4 Callback_Function Timer ISR: Binary_Sem2 Taken 
@n		  4 Callback_Function Timer ISR: No Task is woken by Bin_Sem2 Post 
@n		  4 Callback_Function Timer ISR: Binary_Sem2 Taken 
@n		  4 TestExample_Class::TEST_Task1 Task1: Got the message
@n		  4 TestExample_Class::TEST_Task1 Task1: Print after delay
@n		  4 TestExample_Class::TEST_Task2 Task2: Got the message
@n		  4 TestExample_Class::TEST_Task2 Task2: Print after delay

@par Sample Code Listing
 
~~~~{.cpp}

#include "CR_Tasker.h"
#include "uC_Tick.h"
#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "uC_Single_Timer.h"
#include "CR_Queue.h"
#include "Prod_Spec_Debug.h"

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
 
/*
 * CR_Queue pointer "Bin_Sem1" is used for synchtonization between two tasks.
 * And "Bin_Sem2" is used to demonstrate the following macros.
 * CR_Queue_Post_From_ISR()
 * CR_Queue_Pend_From_ISR()
 */
CR_Queue* Binary_Sem1 = nullptr;
CR_Queue* Binary_Sem2 = nullptr;

CR_Tasker* task1;	// CR task 1 instance
CR_Tasker* task2;	// CR task 2 instance
uC_Single_Timer* timer1 = nullptr;	// Hardware timer instance
CR_Tasker* task1;	// CR task 1 instance
CR_Tasker* task2;	// CR task 2 instance
uC_Single_Timer* timer1 = nullptr;	// Hardware timer instance

// Lets declare a simple class who's member function shall be configured under CR_Tasker
class TestExample_Class
{
	int u_count;

	public:
		TestExample_Class( void )
		{
			u_count = 0;
		}	// Constructor of the class

		~TestExample_Class( void )
		{}	// Destructor of the class

		static void TEST_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		static void TEST_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM param );
};

TestExample_Class tclass;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
void Callback_Function( uC_Single_Timer::cback_param_t param );

void CR_Task_Create( void );
void Init_Start_Hardware_Timer( void );
void Create_Semaphore( void );

/*
 * @brief                   Semaphore Creation
 * @details                 Two CR Queue instances are created
 *                          which can be used as binary Semaphores.
 */
void Create_Semaphore( void )
{
	Binary_Sem1 = new CR_Queue();
	Binary_Sem2 = new CR_Queue();
}

/*
 * @brief                   Adding the tasks in CR tasker Scheduler
 * @details                 The below function does the following
 *                          1. It adds prioritized Tasks in CR_tasker Scheduler
 */
void CR_Task_Create( void )
{

	task1 = new CR_Tasker( TestExample_Class::TEST_Task1, NULL, CR_TASKER_PRIORITY_0, "TASK1" );
	// Add Idle Priority Task1 in CR_Tasker. Constructor 2 is called in this case.
	// So priority of this task is 0/Idle Priority task
	task2 = new CR_Tasker( TestExample_Class::TEST_Task2, NULL, CR_TASKER_PRIORITY_0, "TASK2" );
	// Add Idle Priority Task2 in CR_Tasker. Constructor 2 is called in this case.
	// So priority of this task is 0/Idle Priority task
}

/*
 * @brief                   Initializing and starting the hardware Timer.
 * @details                 This function makes use of phyical LED_EXAMPLE_TIMER
 *                          for testing the CR_Queue_Post_From_ISR() and CR_Queue_Pend_From_ISR().
 */
void Init_Start_Hardware_Timer( void )
{

	/* create instance for uC_timers. */
	timer1 = new uC_Single_Timer( LED_EXAMPLE_TIMER );

	/* configure the interrupts and set its priority */
	timer1->Set_Callback( &Callback_Function, NULL, uC_INT_HW_PRIORITY_7 );
	timer1->Set_Timeout( 1U, TRUE );
	/* Start the timer and the count */
	timer1->Start_Timeout();
}

/*
 * @brief                    Test Task 1 Logic
 * @details                 1.This task along with Test task 2 is used to demonstrate the usage of CR_Queue
 *                            as semaphores for synchronization.
 *                          2.Both the tasks have a critical section guarded by semaphores. These critical
 *                            section consists of two debug print statements with the delay between them.
 *                          3. This delay is introduced to yield the control to other tasks.
 *                          4. Though Task 1 yield the control, Task 2 can not run it's critical section
 *                             as it waits for the semaphore held by Task 1.
 * @param[in] cr_task       CR_Tasker handle
 * @param[in] cr_param      CR_Tasker Parameter
 */
void TestExample_Class::TEST_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Pend( cr_task, Binary_Sem1, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task1: Got the message" );
		}
		CR_Tasker_Delay( cr_task, 100U );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task1: Print after delay" );
		CR_Queue_Post( Binary_Sem1, 10U, result );
		CR_Tasker_Delay( cr_task, 100U );
	}
	CR_Tasker_End();
}

/*
 * @brief                    Test Task 2 Logic
 * @details                 1.This task along with Test task 1 is used to demonstrate the usage of CR_Queue
 *                            as semaphores for synchronization.
 *                          2.Both the tasks have a critical section guarded by semaphores. These critical
 *                            section consists of two debug print statements with the delay between them.
 *                          3. This delay is introduced to yield the control to other tasks.
 *                          4. Though Task 2 yield the control, Task 1 can not run it's critical section
 *                             as it waits for the semaphore held by Task 2.
 * @param[in] cr_task       CR_Tasker handle
 * @param[in] cr_param      CR_Tasker Parameter
 */
void TestExample_Class::TEST_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Pend( cr_task, Binary_Sem1, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task2: Got the message" );
		}
		CR_Tasker_Delay( cr_task, 100U );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task2: Print after delay" );
		CR_Queue_Post( Binary_Sem1, 10U, result );
		CR_Tasker_Delay( cr_task, 100U );
	}
	CR_Tasker_End();
}

/* **********************************************************************************************
 * @brief                             Timer Interrupt call back Routine
 * @details  A callback function when the timer interrupt occurs.
 * The ISR has been written to demonstrate the CR_Queue_Post_From_ISR() and CR_Queue_Pend_From_ISR()
 * for the first 10 seconds. Within the first 10 seconds, we get the debug messages based on the
 * return values of CR_Queue_Post_From_ISR() and CR_Queue_Pend_From_ISR().  Then, we get the
 * debug messages pertaining to the inter CR_Task synchronization between Task 1 and Task 2.
 * @param[in] param                 Timer call back parameter
 * @n
 **********************************************************************************************
 */
void Callback_Function( uC_Single_Timer::cback_param_t param )
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	static bool counting = true;
	bool result;
	bool task_woken;

	// Code to Post the NULL message to CR_Queue once after some time delay.
	if ( counting == true )
	{
		i++;
		j++;
		if ( 1U == j )
		{
			CR_Queue_Post_From_ISR( Binary_Sem2, false, task_woken );
			if ( task_woken == true )
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Timer ISR: Binary_Sem2 posted and waiting task is woken" );
			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Timer ISR: No Task is woken by Bin_Sem2 Post " );
			}
		}
		if ( 2U == j )
		{
			CR_Queue_Pend_From_ISR( Binary_Sem2, &task_woken, result );
			if ( result == true )
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Timer ISR: Binary_Sem2 Taken " );
			}
			j = 0U;
		}
	}
	if ( i == 10 )
	{
		CR_Queue_Post_From_ISR( Binary_Sem1, false, task_woken );
		if ( task_woken == true )
		{
			// Successfully woken up the waiting task; Stop counting.
			counting = false;
		}
		else
		{
			// Not able to wake up the task.Retry again after 1s.
			i = 9;
		}
	}
}

**
 **********************************************************************************************
 * @brief                     Function to start this example
 *                            application
 * @param[in] void            none
 * @return[out] none          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_APP_Init( void )
{
	Create_Semaphore();
	CR_Task_Create();
	Init_Start_Hardware_Timer();
}

~~~~
