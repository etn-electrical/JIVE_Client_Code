/**
 **********************************************************************************************
 * @file			adc1_example_main.cpp
 * @brief			ADC1 Example This example code is in the Public Domain (or CC0 licensed, at your option.)
 * @n				Unless required by applicable law or agreed to in writing, this
 * @n				software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * @n				CONDITIONS OF ANY KIND, either express or implied.
 **********************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "adc1_example_main.h"
#include "uC_AtoD.h"
#include "uC_Base_HW_IDs_ESP32.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t DEFAULT_VREF = 1100;			///< Use adc2_vref_to_gpio() to obtain a better estimate
static const uint16_t NO_OF_SAMPLES = 64;			///< Multi sampling
static const adc_channel_t channel = ADC_CHANNEL_6;	///< GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;	///< Initialized with 0DB Attenuation configuration
static const uC_BASE_ATOD_CTRL_ENUM unit = uC_BASE_ATOD_CTRL_1;	///< ADC1 is configured
static const uint16_t OS_TICL_DELAY = 1000;			///< OS tick delay details

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static esp_adc_cal_characteristics_t* adc_chars;

/**
 * @brief					Function Checks the efuse voltage
 * @return Void				None
 */
static void check_efuse()
{
	// Check TP is burned into eFuse
	if ( esp_adc_cal_check_efuse( ESP_ADC_CAL_VAL_EFUSE_TP ) == ESP_OK )
	{
		printf( "eFuse Two Point: Supported\n" );
	}
	else
	{
		printf( "eFuse Two Point: NOT supported\n" );
	}

	// Check Vref is burned into eFuse
	if ( esp_adc_cal_check_efuse( ESP_ADC_CAL_VAL_EFUSE_VREF ) == ESP_OK )
	{
		printf( "eFuse Vref: Supported\n" );
	}
	else
	{
		printf( "eFuse Vref: NOT supported\n" );
	}
}

/**
 * @brief					Function used to print the Characterized details
 * @return Void				None
 */
static void print_char_val_type( esp_adc_cal_value_t val_type )
{
	if ( val_type == ESP_ADC_CAL_VAL_EFUSE_TP )
	{
		printf( "Characterized using Two Point Value\n" );
	}
	else if ( val_type == ESP_ADC_CAL_VAL_EFUSE_VREF )
	{
		printf( "Characterized using eFuse Vref\n" );
	}
	else
	{
		printf( "Characterized using Default Vref\n" );
	}
}

/**
 * @brief					Function used to test the ADC1
 * @return Void				None
 */
void test_ADC_app_main()
{
	uint32_t adc_reading;

	// Check if Two Point or Vref are burned into eFuse
	check_efuse();

	// Configure ADC
	uC_AtoD* atod_ctrl;

	atod_ctrl = new uC_AtoD( uC_ATOD_RESOLUTION_ENUM( ADC_RESOLUTION_BIT_12 ), channel, unit );

	adc_chars = static_cast<esp_adc_cal_characteristics_t*>
		( calloc( 1, sizeof( esp_adc_cal_characteristics_t ) ) );
	esp_adc_cal_value_t val_type = atod_ctrl->ADC_Characterization( unit, atten, ADC_RESOLUTION_BIT_12,
																	DEFAULT_VREF, adc_chars );

	// Characterize ADC
	print_char_val_type( val_type );

	// Continuously sample ADC1
	while ( 1 )
	{
		adc_reading = 0;
		// Multi sampling
		for ( int i = 0; i < NO_OF_SAMPLES; i++ )
		{
			if ( unit == uC_BASE_ATOD_CTRL_1 )
			{
				atod_ctrl->Start_Scan();
				adc_reading += atod_ctrl->Get_Channel( uint8_t( channel ), unit );
			}
		}
		adc_reading /= NO_OF_SAMPLES;

		// Convert adc_reading to voltage in mV
		uint32_t voltage = esp_adc_cal_raw_to_voltage( adc_reading, adc_chars );
		printf( "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage );
		vTaskDelay( pdMS_TO_TICKS( OS_TICL_DELAY ) );
	}
}

#ifdef __cplusplus
}
#endif
