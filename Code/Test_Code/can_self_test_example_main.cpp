/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 **********************************************************************************************
 * @file			can_self_test_example.cpp
 * @brief			The following example demonstrates the self testing capabilities of the CAN
 * @n				peripheral by utilizing the No Acknowledgment Mode and Self Reception Request
 * @n				capabilities. This example can be used to verify that the CAN peripheral and
 * @n				its connections to the external transceiver operates without issue. The example
 * @n				will execute multiple iterations, each iteration will do the following:
 * @n				1) Start the CAN driver
 * @n				2) Transmit and receive 100 messages using self reception request
 * @n				3) Stop the CAN driver
 **********************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/twai.h"
#include "can_self_test_example_main.h"
#include "uC_CAN.h"
#include "uC_Base.h"

/*
 *****************************************************************************************
 *		Defines for Enabling required configuration testing with 1000KBPS enable only one Macro at time
 *****************************************************************************************
 */
// #define SELF_TEST				///< To perform the self testing of CAN (Looping)
// #define ONLY_RX_FROM_PCAN		///< To perform the only receive opertaion from PCAN(Data sent from PC RXed by
// ESP32)
#define BOTH_TX_RX_FROM_PCAN	///< To perform the both RX and TX opertaions from PCAN

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t NO_OF_MSGS = 100;
static const uint16_t NO_OF_ITERS = 3;
static const uint32_t MSG_ID = 0x555;	///< 11 bit standard format ID
static const char* EXAMPLE_TAG = "CAN Self Test";

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

/**
 * @brief					Function used to perform self testing of CAN for 25kbps
 * @return Void				None
 */
void can_self_test_app_main()
{
	can_timing_config_t timing_config;

	printf( "Start Self test of CAN communication for 25kbps \n" );
	uC_CAN* CAN_Object = new uC_CAN();

	// Create tasks and synchronization primitives
	CAN_MessageBuf tx_msg;
	CAN_MessageBuf rx_message;

	tx_msg.data_length_code = 1;
	tx_msg.identifier = MSG_ID;
	tx_msg.flags = CAN_MSG_FLAG_SELF;

	// Install CAN driver
	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_25KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	for ( uint8_t iter = 0; iter < NO_OF_ITERS; iter++ )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();

		for ( uint8_t i = 0; i < NO_OF_MSGS; i++ )
		{
			// Transmit messages using self reception request
			tx_msg.data[0] = i;
			CAN_Object->CAN_Data_Transmit( &tx_msg );
			vTaskDelay( pdMS_TO_TICKS( 10 ) );

			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				ESP_LOGI( EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0] );
			}
		}

		CAN_Object->CAN_Stop();					///< Stop the CAN Driver
		vTaskDelay( pdMS_TO_TICKS( 100 ) );		///< Delay then start next iteration
	}

	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );
}

/**
 * @brief					Function used to perform self testing of CAN with 250kbps
 * @return Void				None
 */
void can_self_test_app_main_250kbps()
{

	printf( "Start Self test of CAN communication for 250kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;

	// Create tasks and synchronization primitives
	CAN_MessageBuf tx_msg;
	CAN_MessageBuf rx_message;

	tx_msg.data_length_code = 1;
	tx_msg.identifier = MSG_ID;
	tx_msg.flags = CAN_MSG_FLAG_SELF;

	// Install CAN driver
	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_250KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	for ( uint8_t iter = 0; iter < NO_OF_ITERS; iter++ )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();

		for ( uint8_t i = 0; i < NO_OF_MSGS; i++ )
		{
			// Transmit messages using self reception request
			tx_msg.data[0] = i;
			CAN_Object->CAN_Data_Transmit( &tx_msg );
			vTaskDelay( pdMS_TO_TICKS( 10 ) );

			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				ESP_LOGI( EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0] );
			}
		}

		CAN_Object->CAN_Stop();					///< Stop the CAN Driver
		vTaskDelay( pdMS_TO_TICKS( 100 ) );		///< Delay then start next iteration
	}

	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );
}

/**
 * @brief					Function used to perform self testing of CAN with 500kbps
 * @return Void				None
 */
void can_self_test_app_main_500kbps()
{

	printf( "Start Self test of CAN communication for 500kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;

	// Create tasks and synchronization primitives
	CAN_MessageBuf tx_msg;
	CAN_MessageBuf rx_message;

	tx_msg.data_length_code = 1;
	tx_msg.identifier = MSG_ID;
	tx_msg.flags = CAN_MSG_FLAG_SELF;

	// Install CAN driver
	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_500KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	for ( uint8_t iter = 0; iter < NO_OF_ITERS; iter++ )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();

		for ( uint8_t i = 0; i < NO_OF_MSGS; i++ )
		{
			// Transmit messages using self reception request
			tx_msg.data[0] = i;
			CAN_Object->CAN_Data_Transmit( &tx_msg );
			vTaskDelay( pdMS_TO_TICKS( 10 ) );

			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				ESP_LOGI( EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0] );
			}
		}

		CAN_Object->CAN_Stop();					///< Stop the CAN Driver
		vTaskDelay( pdMS_TO_TICKS( 100 ) );		///< Delay then start next iteration
	}

	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );
}

/**
 * @brief					Function used to perform self testing of CAN with 1000kbps
 * @return Void				None
 */
void can_self_test_app_main_1000kbps()
{
#ifdef SELF_TEST
	printf( "Start Self test of CAN communication for 1000kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;

	CAN_MessageBuf tx_msg;
	CAN_MessageBuf rx_message;

	tx_msg.data_length_code = 1;
	tx_msg.identifier = MSG_ID;
	tx_msg.flags = CAN_MSG_FLAG_SELF;

	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_1000KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	for ( uint8_t iter = 0; iter < NO_OF_ITERS; iter++ )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();

		for ( uint8_t i = 0; i < NO_OF_MSGS; i++ )
		{
			// Transmit messages using self reception request
			tx_msg.data[0] = i;
			CAN_Object->CAN_Data_Transmit( &tx_msg );
			vTaskDelay( pdMS_TO_TICKS( 10 ) );

			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				ESP_LOGI( EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0] );
			}
		}

		CAN_Object->CAN_Stop();					///< Stop the CAN Driver
		vTaskDelay( pdMS_TO_TICKS( 100 ) );		///< Delay then start next iteration
	}

	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );
#else
	can_test_with_PCAN_View();
#endif
}

#ifdef ONLY_RX_FROM_PCAN
/**
 * @brief					Function used to perform CAN testing by receive the data from PCAN view or PC
 * @return Void				None
 */
static void can_test_task0( void* arg )
{

	printf( "Start Self test of CAN communication for 1000kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;
	CAN_MessageBuf rx_message;

	CAN_FilterConfTypeDef CAN_Filter = {.filter_config = CAN_FILTER_CONFIG_ACCEPT_ALL()};


	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_1000KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	CAN_Object->Configure_CAN_filter( &CAN_Filter );

	while ( 1 )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();
		vTaskDelay( 50 );

		for ( int itr = 0; itr < 100; itr++ )
		{
			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				printf( "Message received\n" );

				// Process received message
				if ( rx_message.flags & CAN_MSG_FLAG_EXTD )
				{
					printf( "Message is in Extended Format\n" );
				}
				else
				{
					printf( "Message is in Standard Format\n" );
				}

				printf( "ID is %d\n", rx_message.identifier );

				if ( !( rx_message.flags & CAN_MSG_FLAG_RTR ) )
				{
					for ( int j = 0; j < rx_message.data_length_code; j++ )
					{
						printf( "Data byte %d = %d\n", j, rx_message.data[j] );
					}
				}
			}
			else
			{
				printf( " start sending data from PCAN \n" );
				vTaskDelay( 50 );
			}
		}

		CAN_Object->CAN_Stop();
		vTaskDelay( 500 );
	}
	printf( " Driver uninstall is triggered \n" );
	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );

	vTaskDelete( NULL );
}

#endif

/**
 * @brief					Function used to perform CAN testing by receiving the data from PCAN view or PC and
 * @n						Transmitting same data back to PCAN
 * @return Void				None
 */
static void can_test_task1( void* arg )
{
	printf( "Start Self test of CAN communication for 1000kbps\n" );
	uC_CAN* CAN_Object = new uC_CAN();

	can_timing_config_t timing_config;
	CAN_MessageBuf tx_msg;
	CAN_MessageBuf rx_message;

	CAN_FilterConfTypeDef CAN_Filter = {.filter_config = CAN_FILTER_CONFIG_ACCEPT_ALL()};


	CAN_Object->Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def( CAN_TEST_BAUD_RATE_1000KBPS ), &timing_config );

	CAN_Object->Initialize_CAN_Bus( &timing_config );

	CAN_Object->Configure_CAN_filter( &CAN_Filter );

	// for ( uint8_t i = 0; i < 4; i++ )
	while ( 1 )
	{
		// Start CAN Driver for this iteration
		CAN_Object->Enable_CAN();
		vTaskDelay( 50 );

		for ( int itr = 0; itr < 100; itr++ )
		{
			if ( CAN_Object->CAN_Receive_Data( &rx_message ) )
			{
				printf( "Message received\n" );

				// Process received message
				if ( rx_message.flags & CAN_MSG_FLAG_EXTD )
				{
					printf( "Message is in Extended Format\n" );
				}
				else
				{
					printf( "Message is in Standard Format\n" );
				}

				printf( "ID is %d\n", rx_message.identifier );

				if ( !( rx_message.flags & CAN_MSG_FLAG_RTR ) )
				{
					for ( int j = 0; j < rx_message.data_length_code; j++ )
					{
						printf( "Data byte %d = %d\n", j, rx_message.data[j] );
					}
				}

				// Transmit the recived message from PC
				vTaskDelay( pdMS_TO_TICKS( 500 ) );

				for ( uint32_t Index = 0; Index < rx_message.data_length_code; Index++ )
				{
					tx_msg.data[Index] = rx_message.data[Index];
				}

				tx_msg.data_length_code = rx_message.data_length_code;
				tx_msg.identifier = rx_message.identifier;
				tx_msg.flags = rx_message.flags;
				CAN_Object->CAN_Data_Transmit( &tx_msg );
			}
			else
			{
				printf( " start sending data from PCAN \n" );
				vTaskDelay( 50 );
			}
		}

		CAN_Object->CAN_Stop();
		vTaskDelay( 500 );
	}
	printf( " Driver uninstall is triggered \n" );
	// Uninstall CAN driver
	ESP_ERROR_CHECK( CAN_Object->CAN_Driver_Uninstall() );
	ESP_LOGI( EXAMPLE_TAG, "Driver uninstalled" );

	vTaskDelete( NULL );
}

void can_test_with_PCAN_View()
{
#ifdef ONLY_RX_FROM_PCAN
	xTaskCreate( can_test_task0, "can_test_task_0", 4096, ( void* )0, 10, NULL );
#endif

#ifdef BOTH_TX_RX_FROM_PCAN
	xTaskCreate( can_test_task1, "can_test_task_0", 4096, ( void* )0, 10, NULL );
#endif
}

#ifdef __cplusplus
}
#endif
