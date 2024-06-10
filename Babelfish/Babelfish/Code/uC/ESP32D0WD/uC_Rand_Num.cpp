/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Rand_Num.h"
#include "Exception.h"
#include "bootloader_random.h"


/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Rand_Num::uC_Rand_Num( void )
{
	// The hardware RNG is fully functional whenever an RF subsystem is running (ie Bluetooth or WiFi is enabled).
	// If the RF subsystem is not used, the function bootloader_random_enable() must be called to enable an hardware
	// RNG.
	// TODO_ESP32_REWORK: Add similar macro for BLE definition
	#ifndef WIFI_STATION_MODE
	bootloader_random_enable();
	#endif

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Rand_Num::Get( uint8_t* rand_data, uint16_t length )
{
	BF_Lib::SPLIT_UINT32 temp_rand;
	uint_fast8_t rand_index = sizeof( temp_rand );

	for ( uint_fast16_t i = 0U; i < length; i++ )
	{
		if ( rand_index >= sizeof( temp_rand ) )
		{
			rand_index = 0U;

			temp_rand.u32 = esp_random();
		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
}
