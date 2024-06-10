/**
 **************************************************************************************************
 *  @file
 *  @brief OS_Task_Config.h This file provides OS task related configuration settings
 *
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef OS_TASK_CONFIG_H
#define OS_TASK_CONFIG_H

#include "OS_Task.h"

/*
 * Thread Priority for TLS and EMAC threads .
 * TLS Task priority should be higher than TCP/IP task priority which is 18 to load webui page
 * and initiate firmware upgrade process.
 * To Avoid connection loss EMAC_Rx_Task priority has been kept lower than TLS.
 */
static const uint8_t ESP_OS_TASK_PRIORITY_19 = 19U;
static const uint8_t ESP_OS_TASK_PRIORITY_20 = 20U;

#define HTTPS_PORT                              443

#define uC_EMAC_START_THREAD_OS_STACK_SIZE      5000U						/* Ethernet task
																			   (Ethernet_Start_Thread_Static)
																			   stack size */
#define uC_EMAC_START_THREAD_OS_PRIORITY        ( ESP_OS_TASK_PRIORITY_19 )	/* Ethernet task
																			   (Ethernet_Start_Thread_Static)
																			   priority */

#define TLS_THREAD_NAME                         "tls_server_thread"			/* TLS task (Tls_Server) name */
#define TLS_THREAD_STACKSIZE                    5000U						/* TLS task (Tls_Server) stack size */
#define TLS_THREAD_PRIO                         ( ESP_OS_TASK_PRIORITY_20 )	/* TLS task (Tls_Server) priority */

#endif	// OS_TASK_CONFIG_H
