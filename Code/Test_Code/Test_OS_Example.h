/*
 **********************************************************************************************
 * @file            Test_OS_Example.h
 *
 * @brief           Test OS functionality, Test Push_TGINT() and Pop_TGINT() for critical section handling.
 * @details         This file contains function prototypes which are used to test ESP32 FreeRTOS
 *                  OS task creation and working.
 *                  For testing critical section handling, enable CRITICAL_SECTION_TEST macro, In test code
 *                  timer interrupt is configured at 10ms interval,
 *                  Timer example reference : \esp-idf\examples\peripherals\timer_group\main
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef TEST_OS_EXAMPLE
#define TEST_OS_EXAMPLE

/**
 * @brief  : Test OS Task functionality
 * @details: This function will create two OS task and will run a While loop in parallel
 *           Once task creation is done, there should be print on serial port from two
 *           task as well as a while loop added in this function
 * @return void.
 */
void Test_OS_Task( void );

/**
 * @brief  : OS Task Example 1 function
 * @details: This function will print a string periodically at every 1sec interval
 * @return void.
 */
void OS_Task_1( void* arg );

/**
 * @brief  : OS Task Example 2 function
 * @details: This function will print a string periodically at every 1sec interval
 * @return void.
 */
void OS_Task_2( void* arg );

#endif	// #ifndef TEST_OS_EXAMPLE
