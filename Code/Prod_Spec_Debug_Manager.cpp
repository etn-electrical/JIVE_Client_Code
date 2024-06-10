/*
 **********************************************************************************************
 * @file            Prod_Spec_Debug_Print.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the Debug print functionality
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include "Includes.h"
#include "CR_Tasker.h"
#include <string.h>
#include "Prod_Spec_LTK_ESP32.h"
#include "Babelfish_Assert.h"
#include "Babelfish_Debug.h"
#include "Debug_Manager.h"
#include "Debug_Interface_Term_IO.h"
#include "Prod_Spec_Debug.h"
#include "Config/Debug_Manager_Config.h"

#ifdef BF_DBG_IF_UART
#include "Debug_Interface_UART.h"
#endif

/*
 * Debug Manager configuration parameters
 * DEBUG_MANAGER_CR_TASK_PRIORITY: Task priority. Highest priority is not recommended for DEbug
 * Manager task
 * 								   as this might affect other functionality in system
 * DEBUG_MANAGER_CR_TASK_TIME_SLICE_MS: Milli second time in which Debug Manager task should get
 * triggered.
 * DEBUG_MANAGER_CIRCULAR_BUFFER_SIZE_BYTES: Size of circular buffer configured to store debug
 * messages before printing
 * 											 them on selected interface.
 */
static const uint8_t DBG_MANAGER_CR_TASK_PRIORITY = CR_TASKER_PRIORITY_2;
static const uint16_t DBG_MANAGER_CR_TASK_TIME_SLICE_MS = 50U;
static const uint16_t DBG_MANAGER_CIRCULAR_BUFFER_SIZE_BYTES = 2048U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_DEBUG_MANAGER_Init( void )
{
	Debug_Interface* dbg_if_ptr = nullptr;

#ifdef BF_DBG_IF_UART
	dbg_if_ptr = new Debug_Interface_UART();
	BF_ASSERT( dbg_if_ptr );
#elif defined BF_DBG_IF_TERM_IO
	dbg_if_ptr = new Debug_Interface_Term_IO();
	BF_ASSERT( dbg_if_ptr );
#else
	// #error "Debug Interface is not enabled"
#endif

	// If Debug interface is initialized then only initialize and configure Debug Manager
	if ( nullptr != dbg_if_ptr )
	{
		PROD_SPEC_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "Initializing Debug Manager task" );
		dbg_if_ptr->Init();
		// coverity[leaked_storage]
		new Debug_Manager( dbg_if_ptr,
						   DBG_MANAGER_CR_TASK_PRIORITY,
						   DBG_MANAGER_CR_TASK_TIME_SLICE_MS,
						   DBG_MANAGER_CIRCULAR_BUFFER_SIZE_BYTES );
	}
}
