/**
 *****************************************************************************************
 *	@file		Test_App_multiple_HAL.cpp
 *	@brief		See header file for module overview.
 *****************************************************************************************
 */

#include <stdio.h>
#include "Includes.h"

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "DCI_Ram_Owner_Patron_Example.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uC_Flash.h"
#include "uC_AtoD.h"
#include "uC_CAN.h"
#include "uC_Base.h"
#include "uC_PWM.h"
#include "uC_IO_Config.h"
#include "uC_Base_HW_IDs_ESP32.h"
#include "driver/twai.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/adc_channel.h"
#include "Etn_Types.h"
#include "pwm_test_example.h"
#include "Test_App_multiple_HAL.h"
#include "sdkconfig.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
 */

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t DEFAULT_VREF = 1100U;						///< Use adc2_vref_to_gpio() to obtain a better
// estimate
static const uint16_t NO_OF_SAMPLES = 64U;						///< Multi sampling
static const adc_channel_t channel = ADC_CHANNEL_6;				///< GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;				///< Initialized with 0DB Attenuation configuration
static const uC_BASE_ATOD_CTRL_ENUM unit = uC_BASE_ATOD_CTRL_1;	///< ADC1 is configured
static const uint16_t OS_TICL_DELAY = 1000U;					///< OS tick delay details
static const uint16_t ADC_WAIT_COUNT = 10;						///< Waiting time for ADC value in GPIO Pin
static const uint16_t PCAN_WAIT_COUNTER = 10;					///< Waiting time for Duty cycle value from PCAN
static const uint32_t MSG_ID = 0x555;							///< 11 bit standard format ID
static const gpio_num_t BLINK_GPIO = GPIO_NUM_5;				///< GPIO Pin for LED blink
static const uint32_t BLINK_LED_COUNTER = 10U;					///< LED blink time
static const uint16_t NO_OF_MSGS = 1U;
static const uint16_t FLASH_SECTOR_NUM = 273U;
static const uint8_t SET_ZERO = 0U;
static const uint16_t NO_OF_ITERS = 1U;
static const char* EXAMPLE_TAG = "CAN";
static const uint8_t TEST_VALUE1 = 52U;
static const char* PWM_TAG = "PWM";

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static esp_adc_cal_characteristics_t* adc_chars;

enum CAN_Test_BaudRate_Def
{
	CAN_TEST_BAUD_RATE_1000KBPS,	///< 1MBPS baud rate selection
	CAN_TEST_BAUD_RATE_800KBPS,		///< 800KBPS baud rate selection
	CAN_TEST_BAUD_RATE_500KBPS,		///< 500KBPS baud rate selection
	CAN_TEST_BAUD_RATE_250KBPS,		///< 250KBPS baud rate selection
	CAN_TEST_BAUD_RATE_125KBPS,		///< 125KBPS baud rate selection
	CAN_TEST_BAUD_RATE_100KBPS,		///< 100KBPS baud rate selection
	CAN_TEST_BAUD_RATE_50KBPS,		///< 50KBPS baud rate selection
	CAN_TEST_BAUD_RATE_25KBPS,		///< 25KBPS baud rate selection
	CAN_MAX_TEST_NO_BAUD_RATE
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void DCI_Basic_Test_cases( void )
{
	DCI_CheckIn_CheckOut_Data_Variables_Test();
	DCI_Test_Owner_Attributes();
	DCI_owner_Test_Lock_Unlock();
	DCI_Patron_access_test();
}

/*
 * LED blink OS task
 */
void Blink_LED( void )
{
	volatile uint32_t blink_count = SET_ZERO;

	uC_IO_Config* IO_Config;

	IO_Config = new uC_IO_Config();

	printf(
		"GPIO pin %d set  for blinking for %d Seconds.( If Wrover kit RGB LED for Green :GPIO 2,Blue :GPIO 4, Red:GPIO 0 ) \n",
		BLINK_GPIO, BLINK_LED_COUNTER / 5 );

	/* Set the GPIO as a push/pull output */
	IO_Config->Set_GPIO_Direction( BLINK_GPIO, GPIO_MODE_OUTPUT );

	while ( 1 )
	{
		/* Blink off (output low) */
		IO_Config->Set_GPIO_Level( BLINK_GPIO, 0 );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );

		/* Blink on (output high) */
		IO_Config->Set_GPIO_Level( BLINK_GPIO, 1 );
		vTaskDelay( 1000 / portTICK_PERIOD_MS );

		blink_count++;

		if ( blink_count == BLINK_LED_COUNTER )
		{
			printf( "LED blinking for %d Seconds finished", BLINK_LED_COUNTER / 5 );
			break;
		}
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void MCPWM_PWM_Set_Config( MCPWM_CONFIG pwm_config )
{


	uC_PWM* example_pwm;
	bool pwm_status = false;

	uC_PERIPH_IO_STRUCT port_io;

	port_io.mode = pwm_config.mode;
	port_io.io_num = pwm_config.io_num;

	// create uC_PWM instance, select timer and PWM channel
	example_pwm = new uC_PWM( pwm_config.mcpwm_unit, pwm_config.pwm_channel, &port_io );

	// configure polarity, counter_mode and frequency
	example_pwm->Configure( pwm_config.polarity, pwm_config.counter_mode, pwm_config.frequency );

	// Set PWM duty cycle
	example_pwm->Set_Duty_Cycle_Pct( pwm_config.duty_percent );

	// turn Off PWM Mode
	example_pwm->Turn_Off();

	// check if PWM is turned off
	if ( pwm_status == false )
	{
		// turn On PWM
		example_pwm->Turn_On();
	}

	// read PWM status
	pwm_status = example_pwm->Is_On();

	if ( pwm_status )
	{
		ESP_LOGE( PWM_TAG,
				  "PWM signal started [ MCPWM unit: %d | GPIO pin: %d | Channel :%d | Duty Cycle :%.1f%% | Frequency :%d Hz | polarity : %d | Mode: %d ] \n",
				  pwm_config.mcpwm_unit, pwm_config.io_num, pwm_config.pwm_channel, pwm_config.duty_percent, pwm_config.frequency, pwm_config.polarity,
				  pwm_config.counter_mode );
	}
	else
	{
		ESP_LOGE( PWM_TAG, "NOT able to Start PWM signal" );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Read_PCAN_Value_Set_PWM_Duty_cycle()
{
	volatile bool receive_flag = false;
	volatile uint32_t pcan_wait_count = SET_ZERO;
	MCPWM_CONFIG pwm_config;

	printf( "\n --------- Read  CAN Duty Cycle Transmitted value and set it to PWM  ------\n  " );
	printf( "Started CAN communication of 25kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;
	CAN_MessageBuf rx_message;

	CAN_FilterConfTypeDef CAN_Filter = {.filter_config = CAN_FILTER_CONFIG_ACCEPT_ALL()};

	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_25KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	CAN_Object->Configure_CAN_filter( &CAN_Filter );

	// Start CAN Driver for this iteration
	CAN_Object->Enable_CAN();

	while ( !receive_flag )
	{

		vTaskDelay( 50 );

		if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
		{
			printf( "Message received\n" );

			// Process received message
			if ( rx_message.flags & CAN_MSG_FLAG_EXTD )
			{
				printf( "CAN Message Received in Extended Format\n" );
			}
			else
			{
				printf( "CAN Message Received in Standard Format\n" );
			}

			printf( "ID is %X\n", rx_message.identifier );

			if ( !( rx_message.flags & CAN_MSG_FLAG_RTR ) )
			{
				for ( int j = 0; j < rx_message.data_length_code; j++ )
				{
					printf( "Received Data for PWM Duty Cycle= %d\n", rx_message.data[j] );
					CAN_Object->CAN_Stop();
					vTaskDelay( 500 );
					receive_flag = true;
				}
				break;
			}
		}
		else
		{
			printf( " Waiting for data from PCAN for %d Seconds \n", ( PCAN_WAIT_COUNTER - pcan_wait_count ) );
			vTaskDelay( 100 );
			pcan_wait_count++;

			if ( pcan_wait_count == PCAN_WAIT_COUNTER )
			{
				CAN_Object->CAN_Stop();
				vTaskDelay( 500 );
				receive_flag = true;
				printf( "PCAN Duty Cycle not received \n" );
			}

		}

	}
	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );

	// PWM Configuration
	pwm_config.pwm_channel = uC_BASE_PWM_CHANNEL_1;
	pwm_config.counter_mode = MCPWM_UP_COUNTER;
	pwm_config.frequency = 100;
	pwm_config.io_num = GPIO_NUM_19;
	pwm_config.mcpwm_unit = 0;
	pwm_config.mode = GPIO_MODE_OUTPUT;
	pwm_config.polarity = MCPWM_DUTY_MODE_0;

	if ( ( rx_message.data[0] > 0 ) && ( rx_message.data[0] <= 100 ) )
	{
		printf( "Received Data for PWM Duty Cycle= %d\n", rx_message.data[0] );
		pwm_config.duty_percent = ( float32_t )rx_message.data[0];
	}
	else
	{
		pwm_config.duty_percent = 25;
		printf( " Duty percent value :%d is out of limits. Setting 25 percent as default  \n",
				rx_message.data[0] );
	}

	MCPWM_PWM_Set_Config( pwm_config );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Send_ADC_Voltage_Value_to_CAN( uint32_t can_send_voltage )
{

	uint32_t can_receive_voltage = SET_ZERO;
	can_timing_config_t timing_config;

	printf( "Start CAN communication for 25kbps \n" );

	uC_CAN* CAN_Object = new uC_CAN();

	// Create tasks and synchronization primitives
	CAN_MessageBuf tx_msg = {0, 0, 0, 0};
	CAN_MessageBuf rx_msg = {0, 0, 0, 0};

	tx_msg.data_length_code = 4;
	tx_msg.identifier = MSG_ID;
	tx_msg.flags = CAN_MSG_FLAG_SELF;

	printf( "CAN data to send (recieved from ADC :%d mV \n", can_send_voltage );

	*( uint32_t* )& tx_msg.data = can_send_voltage;

	printf(
		"CAN tx_msg.data[0] :%02X \nCAN tx_msg.data[1] :%02X \nCAN tx_msg.data[2] :%02X \nCAN tx_msg.data[3] :%02X \n",
		tx_msg.data[0], tx_msg.data[1], tx_msg.data[2], tx_msg.data[3] );

	// Install CAN driver
	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_25KBPS ), &timing_config );
	CAN_Object->Initialize_CAN_Bus( &timing_config );

	for ( uint8_t iter = 0; iter < NO_OF_ITERS; iter++ )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();

		for ( uint8_t i = 0; i < NO_OF_MSGS; i++ )
		{
			CAN_Object->CAN_Data_Transmit( &tx_msg );
			vTaskDelay( pdMS_TO_TICKS( 10 ) );

			if ( CAN_Object->CAN_Receive_Data( &rx_msg ) )
			{
				can_receive_voltage = *( uint32_t* )&rx_msg.data;
				ESP_LOGI( EXAMPLE_TAG,
						  "Message received--- \nCAN rx_msg.Data[0] = %02X \nCAN rx_msg.Data[1] = %02X \nCAN rx_msg.Data[2] = %02X \nCAN rx_msg.Data[3] = %02X",
						  rx_msg.data[0], rx_msg.data[1], rx_msg.data[2], rx_msg.data[3] );
				ESP_LOGI( EXAMPLE_TAG, "Message converted to Voltage = %d mV ", can_receive_voltage );
			}
		}

		CAN_Object->CAN_Stop();									///< Stop the CAN Driver
		vTaskDelay( pdMS_TO_TICKS( OS_TICL_DELAY ) );			///< Delay then start next iteration
	}

	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Get_ADC1_IO_Number( adc1_channel_t channel, gpio_num_t* gpio_num )
{

	switch ( channel )
	{
		case ADC1_CHANNEL_0:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_0_GPIO_NUM );
			break;

		case ADC1_CHANNEL_1:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_1_GPIO_NUM );
			break;

		case ADC1_CHANNEL_2:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_2_GPIO_NUM );
			break;

		case ADC1_CHANNEL_3:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_3_GPIO_NUM );
			break;

		case ADC1_CHANNEL_4:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_4_GPIO_NUM );
			break;

		case ADC1_CHANNEL_5:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_5_GPIO_NUM );
			break;

		case ADC1_CHANNEL_6:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_6_GPIO_NUM );
			break;

		case ADC1_CHANNEL_7:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_7_GPIO_NUM );
			break;

		default:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_0_GPIO_NUM );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Read_ADC_Send_to_CAN_Test( void )
{

	uint32_t adc_reading;
	uint32_t voltage_adc = SET_ZERO;
	uint32_t adc_wait_count = SET_ZERO;
	gpio_num_t gpio_num;

	printf( "\n\n --------- Read ADC1 voltage and send it as CAN Message ------\n  " );

	// Configure ADC
	uC_AtoD* atod_ctrl;

	atod_ctrl = new uC_AtoD( uC_ATOD_RESOLUTION_ENUM( ADC_RESOLUTION_BIT_12 ), channel, unit );

	Get_ADC1_IO_Number( ( adc1_channel_t ) channel, &gpio_num );

	adc_chars = static_cast<esp_adc_cal_characteristics_t*>
		( calloc( 1, sizeof( esp_adc_cal_characteristics_t ) ) );

	atod_ctrl->ADC_Characterization( unit, atten, ADC_RESOLUTION_BIT_12,
									 DEFAULT_VREF, adc_chars );
	// Continuously sample ADC1
	while ( 1 )
	{
		adc_reading = SET_ZERO;

		for ( int i = 0; i < NO_OF_SAMPLES; i++ )
		{
			if ( unit == uC_BASE_ATOD_CTRL_1 )
			{
				atod_ctrl->Start_Scan();
				adc_reading += atod_ctrl->Get_Channel( uint8_t( channel ), unit );
			}
		}
		adc_reading /= NO_OF_SAMPLES;

		// Convert adc_reading to voltage_adc in mV
		voltage_adc = esp_adc_cal_raw_to_voltage( adc_reading, adc_chars );

		if ( ( adc_wait_count == ADC_WAIT_COUNT ) || ( adc_reading > 0 ) )
		{
			printf( "Value to be send through CAN :  Raw: %d\tVoltage: %dmV\n", adc_reading, voltage_adc );
			break;
		}
		else
		{
			printf( "Raw: %d\tVoltage: %dmV waiting %d seconds for voltage value (Raw) in GPIO Pin=%d ...\n",
					adc_reading, voltage_adc, ( ADC_WAIT_COUNT - adc_wait_count ), gpio_num );
		}

		vTaskDelay( pdMS_TO_TICKS( OS_TICL_DELAY ) );
		adc_wait_count++;
	}

	Send_ADC_Voltage_Value_to_CAN( voltage_adc );
}

#ifdef __cplusplus
extern "C"
{
#endif



namespace NV_Ctrls
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Read_from_Flash_write_to_RAM_vice_versa_Test( void )
{
	uint32_t val_read = SET_ZERO;
	uint32_t sector_flash = FLASH_SECTOR_NUM;
	uint8_t test_value1 = SET_ZERO;
	uint32_t val = 0x20;

	printf( "\n\n--------Read Some Value from DCI Flash and Place into DCI RAM --------\n" );

	DCI_Owner* uint8_data_test_owner;
	uC_Flash* Temp_uC_Flash = new uC_Flash( nullptr, nullptr, true, NULL, NULL, false );

	uint8_data_test_owner = new DCI_Owner( DCI_BLE_USER1_ROLE_DCID );

	// Write & Read value from Flash

	if ( Temp_uC_Flash->Erase_Sector( sector_flash ) != uC_FLASH_COMPLETE )
	{
		printf( "Erase failed\r\n" );
	}

	uint8_data_test_owner->Check_In_Init( ( uint8_t* ) &val );
	uint8_data_test_owner->Check_Out_Init( ( uint8_t* ) &val_read );

	if ( val_read != val )
	{
		printf( "Flash : Read invalid value=%08x of sector=%d\r\n", val_read, sector_flash );
	}
	else
	{
		printf( "Flash :Read value=%08x of SECTOR =%d\r\n", val_read, sector_flash );
	}

	// Write Value to RAM & read the same to print

	uint8_data_test_owner->Check_In( static_cast<uint8_t>( val_read ) );
	uint8_data_test_owner->Check_Out( test_value1 );
	printf( "RAM: Read Value : %08x\n", test_value1 );

	printf( "\n-----------Read Value from DCI RAM and Place into  Flash ----------------\n" );
	uint8_data_test_owner->Check_In( static_cast<uint8_t>( TEST_VALUE1 ) );
	uint8_data_test_owner->Check_Out( test_value1 );
	printf( "RAM: Read Value : %08x\n", test_value1 );

	if ( Temp_uC_Flash->Erase_Sector( sector_flash ) != uC_FLASH_COMPLETE )
	{
		printf( "Erase failed\r\n" );
	}

	uint8_data_test_owner->Check_In_Init( ( uint8_t* ) &test_value1 );
	uint8_data_test_owner->Check_Out_Init( ( uint8_t* ) &val_read );

	if ( val_read != test_value1 )
	{
		printf( "Flash : Read invalid value=%08x of sector=%d\r\n", val_read, sector_flash );

	}
	else
	{
		printf( "Flash :Read value=%08x of SECTOR =%d\r\n", val_read, sector_flash );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_App_Multiple_HAL_Main()
{

	DCI_Basic_Test_cases();

	DCI_Read_from_Flash_write_to_RAM_vice_versa_Test();

	Read_ADC_Send_to_CAN_Test();

	Read_PCAN_Value_Set_PWM_Duty_cycle();

	// Blink_LED();
}

}
#ifdef __cplusplus
}
#endif

