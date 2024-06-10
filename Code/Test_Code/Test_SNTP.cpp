/**
 *****************************************************************************************
 * @file	   	Test_SNTP.cpp

 * @details    	Contains the test application function for SNTP.
 * @copyright  	2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Test_SNTP.h"
#include "Prod_Spec_RTC.h"
#include "Prod_Spec_LTK_ESP32.h"

void Test_SNTP_App_Main( void )
{
#ifdef SNTP_SETUP
	vTaskDelay( 1500 / portTICK_PERIOD_MS );
	int EPOC_Time = Get_Epoch_Time();

	printf( "\nRTC EPOC Time : %d\n", EPOC_Time );
#endif
}
