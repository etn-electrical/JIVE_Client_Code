@example  CR_Tasker
@brief    The sample code initializes the CR_Tasker. It adds tasks for scheduling and calls the task cyclically.
@n		   CR_tasker comes up with functionalities like adding task, suspending task, resuming task
@n        We have added the example codes for 
@n        1. Timed Pending
@n        2. Multiple Pending
@n        3. CR Timers
@n        4. Priority Usage
@n        5. Semaphores using queue
 
@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run successfully the sample code
@n        1. STM32F207 Card is fitted on the board.
 
@par Pin Configurations 
@details   Not Applicable
@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "CR_Tasker.h"
#include "uC_Tick.h"
#include "Ram.h"
#include "Ram_Static.h"
#include "Timers.h"
#include "Prod_Spec_Debug.h"

void CR_Task_Create( void );
void Init_CR_Timers( void );
void Start_CR_Timers( void );
void CR_Tasker_Init( void );
void CallBackFunc_CR_Timer0( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );
void CallBackFunc_CR_Timer1( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );
void CallBackFunc_CR_Timer2( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );
void CallBackFunc_CR_Timer3( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );
void TEST_Task5_Wrapper( CR_Tasker* cr_task, CR_TASKER_PARAM param );
CR_Tasker* task0; // CR task 0 instance 
CR_Tasker* task1; // CR task 1 instance 
CR_Tasker* task2; // CR task 2 instance 
CR_Tasker* task3; // CR task 3 instance 
CR_Tasker* task4; // CR task 4 instance 
CR_Tasker* task5; // CR task 5 instance
CR_Tasker* task6; // CR task 6 instance 
CR_Tasker* task7; // CR task 7 instance 
CR_Queue* queue;  // CR Queue
CR_Queue* Binary_Sem = nullptr;
// CR_Queue instance "Bin_Sem" is created for synchtonization between two tasks.
Timers* timer0;   // CR timer 0 instance 
Timers* timer1;   // CR timer 1 instance 
Timers* timer2;   // CR timer 2 instance 
Timers* timer3;   // CR timer 3 instance 
volatile static uint8_t task_enable_count = 0U;
volatile static uint8_t timer_count = 0U;
// CR_Tasker tick interval in Hz. So tick will be 1/Hz msec
const uint32_t CR_TASKER_TICK_FREQUENCY = 1000U;
// Lets declare a simple class who's member function shall be configured under CR_Tasker
class TestExample_Class
{
		int u_count;

	public:
		TestExample_Class( void )
		{
			u_count = 0;
		}   // Constructor of the class
		~TestExample_Class( void )
		{
		}  // Destructor of the class
		static void TEST_Task0( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task3( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task4( void* param0 );
		void TEST_Task5( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task6( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static void TEST_Task7( CR_Tasker* cr_task, CR_TASKER_PARAM param );
		static uint8_t tick_count;
};
uint8_t TestExample_Class::tick_count = 0U;
TestExample_Class tclass;

/*
 * brief                    CR tasker Initialization
 * note                     The below function does the following
 *                          1. Initialization of CR_Tasker Scheduler
 *                          2. Sets tick of CR_tasker Scheduler 
 *                          3. Starts CR tasker Scheduler
*/
void CR_Tasker_Init( void )
{
	queue = new CR_Queue();
	// Create a CR_tasker Queue for sharing synchronization between task1 and task3.
	//As the default argument is passed to constructor, it is a single item queue.
	//This single item  queue will be used for semaphore purpose. Thus queue can be empty or full.
	//Tasks using the queue don't care what the queue holds - they only want to know if the queue
	//is empty or full. This mechanism is used for task synchronization

	Binary_Sem = new CR_Queue();
	Binary_Sem->Send(NULL, 10U);
	/*
	 * CR Queue instance called "Bin_Sem" has been created to do synchronization between
	 * Task 6 and Task 7. A Null message is sent to the queue to make the binary semaphore
	 * available initially.
 	 */	
	new uC_Tick( CR_Tasker::Tick_Event, CR_TASKER_TICK_FREQUENCY, uC_INT_HW_PRIORITY_7 );
	// Configure the Tick of the CR_Tasker. Attach tick callbackfunction of CR_Tasker
	//Give frequency of tick in Hz. In above case CR_TASKER_TICK_FREQUENCY = 1000Hz
	//So Scheduler tick will occur at every 1/1000 = 1ms interval
	uC_Tick::Start_Tick();
	// Start Tick generating timer
	CR_Tasker::Scheduler( NULL );
	// Run the CR_Tasker Scheduler. Scheduling of tasks starts from here
}

/*
 * brief                    Adding the tasks in CR tasker Scheduler
 * note                     The below function does the following
 *                          1. It adds prioritized Tasks in CR_tasker Scheduler
*/
void CR_Task_Create( void )
{
	task0 = new CR_Tasker( TestExample_Class::TEST_Task0, NULL, CR_TASKER_PRIORITY_3, "TASK0" );
	// Add Priority 3 Task0 in CR_Tasker
	task1 = new CR_Tasker( TestExample_Class::TEST_Task1, NULL, CR_TASKER_PRIORITY_0, "TASK1" );
	// Add Idle priority/Priority 0 Task1 in CR_Tasker
	task2 = new CR_Tasker( TestExample_Class::TEST_Task2, NULL, CR_TASKER_PRIORITY_1, "TASK2" );
	// Add Priority 1 Task2 in CR_Tasker
	task3 = new CR_Tasker( TestExample_Class::TEST_Task3, NULL, CR_TASKER_PRIORITY_2, "TASK3" );
	// Add Priority 2 Task3 in CR_Tasker
	task4 = new CR_Tasker( TestExample_Class::TEST_Task4, NULL, "TASK4" );
	// Add Idle Priority Task4 in CR_Tasker. Constructor 2 is called in this case.
	// So priority of this task is 0/Idle Priority task
	task5 = new CR_Tasker( TEST_Task5_Wrapper, NULL, CR_TASKER_PRIORITY_4, "TASK5" );
	// For Adding the nonmember function of class, you need to add the corresponding wrapper function in CR_tasker
        // This is because CR Tasker constructor prototype do not accept nonstatic member function to be configured
        // under CR tasker
	task6 = new CR_Tasker( TestExample_Class::TEST_Task6, NULL, "TASK6" );
	/*
	 * Add Idle Priority Task6 in CR_Tasker. Constructor 2 is called in this case.
	 * So priority of this task is 0/Idle Priority task
	 */
	task7 = new CR_Tasker( TestExample_Class::TEST_Task7, NULL, "TASK7" );
	/*
	 * Add Idle Priority Task7 in CR_Tasker. Constructor 2 is called in this case.
	 * So priority of this task is 0/Idle Priority task
	 */
}

/*
 * brief                    Initializing the CoRoutine Timers
 * note                     The below function does the following
 *                          1. It initializes the co routine timers 0 through 3
*/
void Init_CR_Timers( void )
{
	timer0 = new Timers( CallBackFunc_CR_Timer0, NULL );
	//Configure CR Timer0 and attach call back function
	timer1 = new Timers( CallBackFunc_CR_Timer1, NULL );
	//Configure CR Timer1 and attach call back function
	timer2 = new Timers( CallBackFunc_CR_Timer2, NULL );
	//Configure CR Timer2 and attach call back function
	timer3 = new Timers( CallBackFunc_CR_Timer3, NULL );
	//Configure CR Timer3 and attach call back function
}

/*
 * brief                    Starting the Coroutine timers
 * note                     The below function does the following
 *                          1. Sets the ticks at which timer callback function 
 *                             is called
 *                          2. Starts the CR timers 0 through 3
 */
void Start_CR_Timers( void )
{
	timer0->Start( 100U, true );
	// The above statement starts the timer and calls the callback function after 10Ticks
	// The second argument is "true" signifying it is configured in auto-reload mode
	timer1->Start( 1U, true );
	// The above statement starts the timer and calls the callback function after 20Ticks
	// The second argument is "true" signifying it is configured in auto-reload mode
	timer2->Start( 12U, false );
	// The above statement starts the timer and calls the callback function after 30Ticks
	// The second argument is "false" signifying it is not configured in auto-reload mode
	timer3->Start( 20U, false );
	// The above statement starts the timer and calls the callback function after 40Ticks
	// The second argument is "false" signifying it is not configured in auto-reload mode
}

/*
 * brief                    Test Task 0 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.The below example code also shows how to yield execution
 *                            to another task.
 *                          3.The below example also shows the use of static variable 
 *                            while using CR Tasker
*/
void TestExample_Class::TEST_Task0( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	static uint8_t static_counter;
	static uint8_t local_counter;
	volatile static CR_QUEUE_STATUS queue_status;
	uint32_t queue_transmit;
	CR_Tasker_Begin( cr_task );
	
	for ( ;; )
	{
		static_counter++;
		local_counter++;
		CR_Tasker_Yield( cr_task );
		// Yield execution to another ready to run task.
		// CR tasker scheduler will decide the task to yield to.
		// Here CR_Tasker_Yield generates branch instruction. When control returns back from
		// another task the values of local variables are lost, as local variables always gets
		// generated on stack. Hence use static variables if you want to preserve variable values
		// In our case the value of local_counter can never be more than 1
		static_counter++;
		local_counter++;
		if ( 0x14 == static_counter )
		{
			// Sends data to the queue
			CR_Queue_Send( cr_task, queue, ( CR_TX_QUEUE_DATA ) & queue_transmit, 10U,
				queue_status );
			// A semaphore is signaled by calling CR_Queue_Send. Releasing the semaphore
			// This task releases the semaphore when he dont require resource.
			// above we have assumed that if static_counter ==14 task1 does not require resource and
			// it release the semaphore by posting information on queue
			// It waits for 10 number of ticks to put semaphore on a queue in case there is no room.
			// Even after 10 ticks room is not available on queue, it fills queue_status = CR_QUEUE_FAIL
			static_counter = 0x00;
		}
	}
	CR_Tasker_End();
	
}

/*
 * brief                    Test Task 1 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.The below example code shows how to yield execution to 
 *                            another tasks until a a specified condition is not satisfied.
 *                          3.This example alos shows the use of static variable 
 *                            static_counter_task1 
*/
void TestExample_Class::TEST_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t static_counter_task1;

	CR_Tasker_Begin( cr_task );

	for ( ;; )
	{
		static_counter_task1++;
		CR_Tasker_Yield_Until( cr_task, timer_count >= 5 );
		// Yield execution to another ready to run task until timer_count > 5.
		// CR tasker scheduler will decide the task to yield to.
		// This will keep coming back at the same priority.
		// That means we could end up blocking lower priority tasks until this is complete.
		// This feature shall be used in lower priority tasks or used smartly with higher priority tasks
		// Hence here we have configured priority of this task as 0
		timer_count = 0U;

		static_counter_task1++;
		static_counter_task1++;
	}
	CR_Tasker_End();
	
}

/*
 * brief                    Test Task 2 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.This is an example of multiple time pending. 
 *                            The task is delayed/blocked multiple times
 *                          3.The use of static variables is also demonstrated
*/
void TestExample_Class::TEST_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t static_counter_task2 = 0x00U;
	CR_Tasker_Begin( cr_task );

	for ( ;; )
	{
		static_counter_task2++;
		CR_Tasker_Delay( cr_task, 1U );
		// Pend the task for 1 tick
		static_counter_task2++;
		// The use of static variable is critical, as below function blocks this task
		// and when this task resumes then we may need values variables were holding at the time of pending
		CR_Tasker_Delay( cr_task, 4U );
		// Pend the task for 4 ticks
		static_counter_task2++;
		CR_Tasker_Delay( cr_task, 5U );
		// Pend the task for 5 ticks
		static_counter_task2++;
	}
	CR_Tasker_End();
	
}

/*
 * brief                    Test Task 3 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.This task is an example of how it receives semaphore from 
 *                            another task
*/
void TestExample_Class::TEST_Task3( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t static_counter_task3 = 0U;
	volatile static CR_QUEUE_STATUS queue_status;
	volatile static uint32_t queue_receive;

	CR_Tasker_Begin( cr_task );

	for ( ;; )
	{
		static_counter_task3++;
		// Receive the data from queue
		CR_Queue_Receive( cr_task, queue, ( CR_RX_QUEUE_DATA ) & queue_receive, 1000,
			queue_status );
		// Pend. The task waits until a task signals the semaphore or the specified timeout expires.
		// Task3 is waiting for a semaphore. This task is in a blocked state until it receives semaphore
		// from task1 and queue_status = CR_QUEUE_BLOCKED. Once it receives the semaphore
		// from task1 then queue_status = CR_QUEUE_PASS. There is a timeout of 1000 ticks. If even after
		// 1000 ticks task3 do not receive semaphore then queue_status = CR_QUEUE_FAIL.
		// Do processing only if we receive semaphore
		if ( CR_QUEUE_PASS == queue_status )
		{
			static_counter_task3++;
			tick_count++;
			//Incrementing class specific static variable
			CR_Tasker_Delay( cr_task, 3U );
			// Pend the task for 3 ticks
			static_counter_task3++;
			tick_count++;
			//Incrementing class specific static variable
			CR_Tasker_Delay( cr_task, 10U );
			// Pend the task for 1000 ticks
		}
        
		CR_Tasker_Delay( cr_task, 100U );
	}
	CR_Tasker_End();
	
}

/*
 * brief                    Test Task 4 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.This is an example of no pending.
*/
void TestExample_Class::TEST_Task4( void* param0 )
{
	volatile static uint8_t static_counter_task4 = 0x00U;

	if ( static_counter_task4 < 240U )
	{
		static_counter_task4++;
	}
	// Increase the variable till its value becomes 240
}

/*
 * brief                    Test Task 5 logic
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application
 *                          2.This is a nonstatic function under TestExample_Class.
 *                          3.It is also an example of time pending as this task is 
 *                            delayed for 5 ticks.
*/
void TestExample_Class::TEST_Task5( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	static uint8_t static_local_variable;
	CR_Tasker_Begin( cr_task );

	for ( ;; )
	{
		u_count++;
		static_local_variable++;
		CR_Tasker_Delay( cr_task, 5U );
		// Delay the execution of this task by 5 ticks
		u_count++;
		static_local_variable++;
	}

	CR_Tasker_End();

}

/*
 * brief                    Wrapper function for Test_task5
 * note                     1.Wrapper function for TEST_Task5, as TASK5 is nonstatic function
*/
void TEST_Task5_Wrapper( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	tclass.TEST_Task5( cr_task, param );
}

/*
 * brief                    Test Task 6 Logic
 * note                     1.This task along with Test task 7 is used to demonstrate the usage of CR_Queue
 *                            as semaphores for synchronization.r
 *                          2.Both the tasks have a critical section guarded by semaphores. These critical 
 *                            section consists of two debug print statements with the delay between them.
 *                          3. This delay is introduced to yield the control to other tasks. 
 *                          4. Though Task 6 yield the control, Task 7 can not run it's critical section 
 *                             as it waits for the semaphore held by Task 6.
*/
void TestExample_Class::TEST_Task6( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Receive( cr_task, Binary_Sem, nullptr, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task6: Got the message" );
		}
		CR_Tasker_Delay( cr_task, 100U );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task6: Print after delay" );
		Binary_Sem->Send( NULL, 10U );
		CR_Tasker_Delay( cr_task, 100U );
	}
	CR_Tasker_End();
}

/*
 * brief                    Test Task 7 Logic
 * note                     1.This task along with Test task 6 is used to demonstrate the usage of CR_Queue
 *                            as semaphores for synchronization.r
 *                          2.Both the tasks have a critical section guarded by semaphores. These critical 
 *                            section consists of two debug print statements with the delay between them.
 *                          3. This delay is introduced to yield the control to other tasks. 
 *                          4. Though Task 7 yield the control, Task 6 can not run it's critical section 
 *                             as it waits for the semaphore held by Task 7.
*/
void TestExample_Class::TEST_Task7( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Receive( cr_task, Binary_Sem, nullptr, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task7: Got the message" );
		}
		CR_Tasker_Delay( cr_task, 100U );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Task7: Print after delay" );
		Binary_Sem->Send( NULL, 10U );
		CR_Tasker_Delay( cr_task, 100U );
	}
	CR_Tasker_End();
}

/*
 * brief                    CR Timer 0 Call back function
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application. e.g. In below code we are suspnding the 
 *                            task3 for 100ms and resuming it for 100ms
 *                          2.This example code also explains about how to use CR_tasker 
 *                            functions like suspend and Resume().
 *                          3.The below code also shows how to use CR Timer
*/
void CallBackFunc_CR_Timer0( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	task_enable_count++;
	// Count 100ms
	if ( 10U == task_enable_count )
	{
		task3->Suspend();
		// Suspending the task from CR_Tasker
	}
	if ( 20U == task_enable_count )
	{
		task3->Resume();
		// Resuming the task in CR_Tasker
		task_enable_count = 0U;
	}
}

/*
 * brief                    CR Timer 1 Call back function
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application.
*/
void CallBackFunc_CR_Timer1( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	// User Logic e.g.
	timer_count++;
}

/*
 * brief                    CR Timer 2 Call back function
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application.
*/
void CallBackFunc_CR_Timer2( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	timer2->Start( 5U, false );
	// CR Timer 2 is not configured in autoreload mode. So Start it manually
	// to generate 5ms tick again
}

/*
 * brief                    CR Timer 3 Call back function
 * note                     1.Integrator can implement any logic in this function as per
 *                            his application.
*/
 void CallBackFunc_CR_Timer3( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	timer3->Start( 4U, false );
	// CR Timer 3 is not configured in autoreload mode. So Start it manually
	// to generate 4ms tick again
	// User Logic
}

~~~~
