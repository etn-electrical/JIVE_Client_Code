/**
 **************************************************************************************************
 *  @file
 *  @brief OS_Task_Priority.h This file provides OS task priority defined value.
 *
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef OS_TASK_PRIORITY_H
#define OS_TASK_PRIORITY_H


// The higher the task priority number the higher the priority.
static const uint8_t OS_TASK_PRIORITY_LOW_IDLE_TASK = 0U;
static const uint8_t OS_TASK_PRIORITY_1 = 1U;
static const uint8_t OS_TASK_PRIORITY_2 = 2U;
static const uint8_t OS_TASK_PRIORITY_3 = 3U;
static const uint8_t OS_TASK_PRIORITY_4 = 4U;
static const uint8_t OS_TASK_PRIORITY_5 = 5U;
static const uint8_t OS_TASK_PRIORITY_6 = 6U;
static const uint8_t OS_TASK_PRIORITY_7 = 7U;
static const uint8_t OS_TASK_PRIORITY_MAX = CONFIG_MAX_PRIORITIES;
// Not a valid task priority.

#ifdef ESP32_SETUP
// This defines was in ThirdParty lwipopts.h but we are using esp_idf lwipopts.h that's why added here for esp32
#define BIP_THREAD_NAME              "bip_thread"
#define BIP_THREAD_STACKSIZE          2560
#define BIP_THREAD_PRIO               OS_TASK_PRIORITY_1
#endif

#endif	// OS_TASK_PRIORITY_H
