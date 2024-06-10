\example  OS_Tasker An example code for OS Tasker Class
\brief    The sample code explains use of different APIs of OS_Tasker Class.
\par Board Specific Settings  

 \details Not Applicable.
 
 \par Pin Configurations 
 \details Not Applicable

 \par Dependency
 \details Not Applicable
 
 \par Sample Code Listing

~~~~{.cpp}
#include "OS_Task_TEST.h"
#include "OS_Tasker.h"

OS_Task* testTask1;
OS_Task* testTask2;
OS_Task* testTask3;
OS_Task* testTask4;
OS_Task* testTask5;
OS_Task* testTask6;
OS_Task* testTask7;
OS_Task* testTask9;



void OS_Callback_1( OS_TASK_PARAM param );
void OS_Callback_2( OS_TASK_PARAM param );
void OS_Callback_3( OS_TASK_PARAM param );
void OS_Callback_4( OS_TASK_PARAM param );
void OS_Callback_5( OS_TASK_PARAM param );
void OS_Callback_6( OS_TASK_PARAM param );
void OS_Callback_7( OS_TASK_PARAM param );
void OS_Callback_9( OS_TASK_PARAM param );



BOOL Init_OS_Task_test( void )
{
  //OS_Task_TEST_1 - Tests Create_Task() & Is_Task_Suspended();
  //If task is not suspended then it should return false
  testTask1 = OS_Tasker::Create_Task( OS_Callback_1, 64, OS_TASK_PRIORITY_1, "TEST Task1", NULL);  
  testTask2 = OS_Tasker::Create_Task( OS_Callback_2, 64, OS_TASK_PRIORITY_1, "TEST Task2", NULL);
  
  //OS_Task_TEST_2 - Test Is_Task_Suspended(); if task is suspended.
  //If task is  suspended then it should return true
  testTask3 = OS_Tasker::Create_Task( OS_Callback_3, 64, OS_TASK_PRIORITY_1, "TEST Task3", NULL);  
  testTask4 = OS_Tasker::Create_Task( OS_Callback_4, 64, OS_TASK_PRIORITY_1, "TEST Task4", NULL);
  
  //OS_Task_TEST_3 - Test Resume()
  //Suspend Task 5, then check from task6 if task 5 is suspended or not (it should be suspended)
  //then resume the task5 from 6 and check if it is resumed back
  testTask5 = OS_Tasker::Create_Task( OS_Callback_5, 64, OS_TASK_PRIORITY_1, "TEST Task5", NULL);  
  testTask6 = OS_Tasker::Create_Task( OS_Callback_6, 64, OS_TASK_PRIORITY_1, "TEST Task6", NULL);
  
  //OS_Task_TEST_4 - Test Get_Current_Task()
  //Gets the current task, inspect it through debugger that pointer getting assigned with actual task values
  testTask7 = OS_Tasker::Create_Task( OS_Callback_7, 64, OS_TASK_PRIORITY_1, "TEST Task7", NULL);  
 
  
  //OS_Task_TEST_5 - Test Get_Tick_Count()
  testTask9 = OS_Tasker::Create_Task( OS_Callback_9, 64, OS_TASK_PRIORITY_1, "TEST Task9", NULL);  
  
  
  return true;
}

//OS_Task_TEST_1-----------------------------------------
void OS_Callback_1( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
while (1)
    {
        test1++;
    }
}
// OS_Task_TEST_1:Is Task Suspended
void OS_Callback_2( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
BOOL IsTaskSuspended = TRUE;
while (1)
    {
      IsTaskSuspended = OS_Tasker::Is_Task_Suspended( testTask1 );
      test1++;
    }
}

//OS_Task_TEST_2-----------------------------------------
void OS_Callback_3( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
while (1)
    {
        test1++;
        OS_Tasker::Suspend();
    }
}

void OS_Callback_4( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
BOOL IsTaskSuspended = FALSE;
while (1)
    {
      IsTaskSuspended = OS_Tasker::Is_Task_Suspended( testTask3 );
      test1++;
    }
}

//OS_Task_TEST_3-----------------------------------------
void OS_Callback_5( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
while (1)
    {
        test1++;
        OS_Tasker::Suspend();
    }
}

void OS_Callback_6( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
BOOL IsTaskSuspended = FALSE;
while (1)
    {
      test1++;
      IsTaskSuspended = OS_Tasker::Is_Task_Suspended( testTask5 );
      OS_Tasker::Resume( testTask5 );
      IsTaskSuspended = OS_Tasker::Is_Task_Suspended( testTask5 );

    }
}

//OS_Task_TEST_4-----------------------------------------
void OS_Callback_7( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
OS_Task *thisTask;
while (1)
    {
        test1++;
        thisTask = OS_Tasker::Get_Current_Task();
    }
}


//OS_Task_TEST_5-----------------------------------------
void OS_Callback_9( OS_TASK_PARAM param )
{
uint8_t test1 = 1;
volatile OS_TICK_TD count1;
while (1)
    {
        test1++;
        count1 = OS_Tasker::Get_Tick_Count();
        count1 = 0;    }
}



~~~~


