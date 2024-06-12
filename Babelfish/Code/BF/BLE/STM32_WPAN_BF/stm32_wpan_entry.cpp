/**
 ******************************************************************************
 * @file            stm32_wpan_entry.cpp
 * @brief           Entry application source file for STM32WPAN Middleware
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 * @copyright
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics. All rights reserved.</center></h2>
 * This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; You may not use this
 * file except in compliance with the License. You may obtain a copy of the License at: www.st.com/SLA0044
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "Includes.h"
#include "stm32_wpan_entry.h"
#include "ble.h"
#include "tl.h"
#include "shci_tl.h"
#include "app_conf.h"

#include "CR_Tasker.h"
#include "CR_Queue.h"

#include "stm32wbxx_hal_hsem.h"


/* Private typedef -----------------------------------------------------------*/


/* Private defines -----------------------------------------------------------*/
#define POOL_SIZE ( CFG_TLBLE_EVT_QUEUE_LENGTH * 4U * \
					DIVC( ( sizeof( TL_PacketHeader_t ) + TL_BLE_EVENT_FRAME_SIZE ), 4U ) )


/* Private macros ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
PLACE_IN_SECTION( "MB_MEM2" ) ALIGN( 4 ) static uint8_t EvtPool[POOL_SIZE];
PLACE_IN_SECTION( "MB_MEM2" ) ALIGN( 4 ) static TL_CmdPacket_t SystemCmdBuffer;
PLACE_IN_SECTION( "MB_MEM2" ) ALIGN( 4 ) static uint8_t SystemSpareEvtBuffer[sizeof( TL_PacketHeader_t ) +
																			 TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION( "MB_MEM2" ) ALIGN( 4 ) static uint8_t BleSpareEvtBuffer[sizeof( TL_PacketHeader_t ) +
																		  TL_EVT_HDR_SIZE + 255];

static CR_Queue* shci_user_evt_proc_flag;


/* Private functions prototypes-----------------------------------------------*/
static void appe_Tl_Init( void );

static void APPE_SysStatusNot( SHCI_TL_CmdStatus_t status );

static void APPE_SysUserEvtRx( void* pPayload );

static void shci_user_evt_proc_task( CR_Tasker* cr_task, CR_TASKER_PARAM param );


/*************************************************************
*   Initialization functions
*************************************************************/

/**
 * @brief    Initialize the BLE stack. Call this function from the main program to start the initialization of the
 * stack.
 */
void Stm32_Wpan_Init( void )
{
	appe_Tl_Init();	// Initialize all transport layers
	/**
	 * From now, the application is waiting for the ready event (VS_HCI_C2_Ready) received on the system channel before
	 * starting the Stack This system event is received with APPE_SysUserEvtRx()
	 */
	return;
}

/**
 * @brief    Initialize the transport layers.
 */
static void appe_Tl_Init( void )
{
	TL_MM_Config_t tl_mm_config;
	SHCI_TL_HciInitConf_t SHci_Tl_Init_Conf;

	// Reference table initialization
	TL_Init();

	// System channel initialization
	shci_user_evt_proc_flag = new CR_Queue( 10 );
	new CR_Tasker( shci_user_evt_proc_task, NULL, CR_TASKER_PRIORITY_4, "shci_user_evt_proc_task" );

	SHci_Tl_Init_Conf.p_cmdbuffer = ( uint8_t* )&SystemCmdBuffer;
	SHci_Tl_Init_Conf.StatusNotCallBack = APPE_SysStatusNot;
	shci_init( APPE_SysUserEvtRx, ( void* ) &SHci_Tl_Init_Conf );

	// Memory Manager channel initialization
	tl_mm_config.p_BleSpareEvtBuffer = BleSpareEvtBuffer;
	tl_mm_config.p_SystemSpareEvtBuffer = SystemSpareEvtBuffer;
	tl_mm_config.p_AsynchEvtPool = EvtPool;
	tl_mm_config.AsynchEvtPoolSize = POOL_SIZE;
	tl_mm_config.p_TracesEvtPool = nullptr;
	tl_mm_config.TracesEvtPoolSize = 0;

	TL_MM_Init( &tl_mm_config );

	TL_Enable();

	return;
}

/**
 * @brief    Initialize the BLE stack. This function is automatically called by the stack after the low-level
 * initialization has taken place. Must be implemented in the app space.
 */
__WEAK void APP_BLE_Init( void )
{
	return;
}

/*************************************************************
*   System HCI event callbacks
*************************************************************/

/**
 * @brief    Notification for SHCI events received from the BLE stack.
 */
static void APPE_SysStatusNot( SHCI_TL_CmdStatus_t status )
{
	return;
}

/**
 * @brief    Processes system user events received from the BLE stack.
 * @note The type of the payload for a system user event is tSHCI_UserEvtRxParam. When the system event is both
 *    - a ready event (subevtcode = SHCI_SUB_EVT_CODE_READY)
 *    - reported by the FUS (sysevt_ready_rsp == RSS_FW_RUNNING) the buffer shall not be released (e.g.,
 * ((tSHCI_UserEvtRxParam*)pPayload)->status shall be set to SHCI_TL_UserEventFlow_Disable). When the status is not
 * filled, the buffer is released by default.
 */
static void APPE_SysUserEvtRx( void* pPayload )
{
	// Traces channel initialization
	TL_TRACES_Init();
	APP_BLE_Init();
	return;
}

/**
 * @brief    Receive notifications of SHCI events from the stack. It needs to trigger shci_user_evt_proc to run via the
 * task manager.
 * @remark   The number of concurrent SHCI events is low, the queue can be set to a relatively small size (e.g., 10
 * elements) without worrying to make the queue full.
 */
void shci_notify_asynch_evt( void* pdata )
{
	// coverity[var_deref_model]
	shci_user_evt_proc_flag->Send( NULL, 0 );
	return;
}

/**
 * @brief    Processes SHCI events received from the BLE stack by calling shci_user_evt_proc().
 */
static void shci_user_evt_proc_task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static CR_QUEUE_STATUS status;
	volatile static uint32_t queue_receive;

	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		CR_Queue_Receive( cr_task, shci_user_evt_proc_flag, ( CR_RX_QUEUE_DATA )&queue_receive, 1000, status );
		if ( CR_QUEUE_PASS == status )
		{
			shci_user_evt_proc();
			// TODO check for any DEBUG MACRO and enable accordingly
			// printf("\n shci_user_evt_proc_task" );
		}
	}
	CR_Tasker_End();
}

/**
 * See declaration in hci_tl.h. This functions is weakly implemented in shci_tl.c but it has been reimplemented for
 * better performance.
 */
void shci_cmd_resp_release( uint32_t flag )
{
	HAL_HSEM_Release( SHCI_CMD_RESP_HSEM, 0 );
}

/**
 * See declaration in shci_tl.h. This function must block if the response has not been released. This functions is
 * weakly implemented in shci_tl.c but it has been reimplemented for better performance.
 */
void shci_cmd_resp_wait( uint32_t timeout )
{
	HAL_HSEM_FastTake( SHCI_CMD_RESP_HSEM );
	while ( HAL_HSEM_IsSemTaken( SHCI_CMD_RESP_HSEM ) )
	{
		continue;
	}
}
