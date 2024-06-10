/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Blink_Example.h"
#include "Test_Debug.h"
#include "Toolkit_Revision.h"
#include "Test_CR_Task.h"
#include "esp_idf_version.h"


#ifdef CONFIG_IDF_TARGET_ESP32
// As of now there is no default LED available on ESP32-Mini-1 hardware.
// If LED has been connected some other port pin on Mini-1 hardware, please change here
#define BLINK_GPIO 2
#endif

void Blink_LED_Example( void )
{
	/* Configure the IOMUX register for pad BLINK_GPIO (some pads are
	   muxed to GPIO on reset already, but some default to other
	   functions and need to be switched to GPIO. Consult the
	   Technical Reference for a list of pads and their default
	   functions.)
	 */
	gpio_pad_select_gpio( BLINK_GPIO );

	/* Set the GPIO as a push/pull output */
	gpio_set_direction( ( gpio_num_t )BLINK_GPIO, GPIO_MODE_OUTPUT );

	while ( 1 )
	{
		TEST_CODE_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "IDF Version Major: %u, Minor: %u, Build: %u", ESP_IDF_VERSION_MAJOR,
							   ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH );
		TEST_CODE_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "Babelfish version: Major: %u, Minor: %u, Build: %u", CONST_LTK_MAJOR,
							   CONST_LTK_MINOR, CONST_LTK_BUILD );

		/* Blink off (output low) */
		TEST_CODE_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "Turning off LED" );
		// printf("Turning off the LED\n");
		gpio_set_level( ( gpio_num_t )BLINK_GPIO, 0 );
		vTaskDelay( 3000 / portTICK_PERIOD_MS );

		/* Blink on (output high) */
		TEST_CODE_DEBUG_PRINT( BF_DBG_MSG_DEBUG, "Turning on the LED" );
		// printf("Turning on the LED\n");
		gpio_set_level( ( gpio_num_t )BLINK_GPIO, 1 );

		vTaskDelay( 1000 / portTICK_PERIOD_MS );
	}
}
