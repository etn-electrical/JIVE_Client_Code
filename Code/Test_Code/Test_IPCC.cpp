/**
 *****************************************************************************************
 * @file		Test_IPCC.cpp
 *
 * @brief		Contains the test application function prototype for IPCC API's.
 *				IPC-Inter-Processor Communication Control
 * @copyright  	2021 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */

#include "Test_IPCC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uC_IPCC.h"
#include "Test_Example.h"

#if defined ( TEST_IPCC ) && !defined ( CONFIG_FREERTOS_UNICORE )

/**
 * @brief			:	Function executes on other CPU and change value
 * @return Void		:	None
 */
static void Test_IPCC_Call( void* arg )
{
	printf( "Inside Callback Function \n\r" );
	vTaskDelay( 50 );
	uint32_t* val = ( uint32_t* )arg;

	*val = 0xa5a5;
	printf( "Exiting Callback Function \n\r" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_IPCC_App_Main( void )
{
	/*
	 * Test blocking call
	 * set wait = 1 to indicate its a blocking call
	 */
	uint32_t ret = 0;
	uint32_t val = 0x5a5a;
	uint32_t wait = 1;
	uint32_t cpu_id = 0;

	uC_IPCC* uC_IPCC_ptr = new uC_IPCC();

	cpu_id = uC_IPCC_ptr->Get_Core_ID();

	ret = uC_IPCC_ptr->Send( !cpu_id, Test_IPCC_Call, &val, wait, NULL );

	printf( "IPCC_Send status %d \n\r", ret );

	// Value should be changed since we are waiting till function execution
	if ( 0x5a5a != val )
	{
		printf( "API Success\n\r" );
	}
	else
	{
		printf( "Failed to execute blocking call\n\r" );
	}

	/*
	 * Test non blocking call
	 * set wait = 0 to indicate its a non blocking call
	 */
	val = 0x5a5a;
	wait = 0;

	ret = uC_IPCC_ptr->Send( !cpu_id, Test_IPCC_Call, &val, wait, NULL );

	printf( "IPCC_Send status %d \n\r", ret );

	// Value should not be changed since we are not waiting till function execution
	if ( 0x5a5a == val )
	{
		printf( "API Success\n\r" );
	}
	else
	{
		printf( "Failed to execute non blocking call\n\r" );
	}

	delete uC_IPCC_ptr;
}

#endif	/* TEST_IPCC */
