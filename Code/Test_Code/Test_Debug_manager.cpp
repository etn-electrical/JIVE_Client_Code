/* GPIO Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include "Prod_Spec_Debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Test_debug_manager.h"


void Test_Debug_manager_app_main( void )
{
	uint16_t cnt = 0;

	while ( 1 )
	{
		if ( cnt < 21 )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Testing debug message cnt: %d\r\n", cnt++ );

			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "test Error message \r\n" );
		}
		else
		{
			cnt = 0;
		}
		vTaskDelay( 500 / portTICK_RATE_MS );
	}

}

#ifdef __cplusplus
}
#endif


