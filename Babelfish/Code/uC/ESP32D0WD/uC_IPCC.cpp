/**
 *****************************************************************************************
 * @file : uC_IPCC
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "esp_ipc.h"
#include "uC_IPCC.h"
#include "Includes.h"
#include "FreeRTOS.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_IPCC::uC_IPCC( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_IPCC::~uC_IPCC()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IPCC::Register_Callback_Fun( uint32_t cpu_id, ipcc_func_t fun )
{
	BF_Lib::Unused<uint32_t>::Okay( cpu_id );
	BF_Lib::Unused<ipcc_func_t>::Okay( fun );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_IPCC::Send( uint32_t cpu_id, ipcc_func_t fun,
						void* data, uint32_t wait, void* device )
{
	esp_err_t error = ESP_FAIL;

	BF_Lib::Unused<void*>::Okay( device );
    //Ahmed
    #if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)
	// no wait required so call non blocking call
	if ( 0 == wait )
	{
		error = esp_ipc_call( cpu_id, fun, data );
	}
    else
    {
        // wait required so call blocking call
        error = esp_ipc_call_blocking( cpu_id, fun, data );
    }
    #endif

	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_IPCC::Get_Core_ID( void )
{
	return ( xPortGetCoreID() );
}
