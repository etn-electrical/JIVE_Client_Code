/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Includes.h"
#include "uC_Watchdog.h"
#include "uC_Interrupt.h"
#include "uC_Reset.h"
#include "uC_IO_Config.h"
#include "DCI_Constants.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_Mem.h"
#include "Prod_Spec.h"
#include "Exception.h"

#ifdef PX_GREEN_IOT_SUPPORT
#include "Prod_Spec_IOT.h"
#endif	// #ifdef PX_GREEN_IOT_SUPPORT

//Ahmed
#include "Breaker.h"

extern DRAM_ATTR Device_Info_str DeviceInfo;
static uint8_t JustPoweredUpNoConnection = true;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t main( void )
{
	// Watchdog not yet supported for ESP32
	// uC_Watchdog::Init( WATCHDOG_TIMEOUT );

	// RAM initialization
	PROD_SPEC_RAM_Init();

	// Get uC Reset reason
	// coverity[leaked_storage]
	new uC_Reset();

	// Interrupt_tick_counts starts after OS / CR Tasker Init. osdepTimeOpen()
	// is called during PROD_SPEC_CERT_ECC_CA_Init setup which returns success only when
	// Interrupt_tick_counts is started,hence initializing Main_Init after OS init.
	// FUS over IOT is tested with this change.
	// Initialize ESP32 supported features

	//Ahmed
	//Initiate certifications before intitate CR task to avoid reseting in the case of empty flash
	PROD_SPEC_Main_Init();

	// Initialize CR Task functionality and scheduler
	PROD_SPEC_OS_Init();

    //Ahmed
//    SW_Init();	// Jonathan commented this out

    if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK)
    {
        JustPoweredUpNoConnection = false;
        PROD_SPEC_IOT_Init();
        DeviceInfo.PxInitiated = true;
    }

#ifdef  DEBUG_ESP32_DISABLE
	// For run-time flash integrity check
	/*	new BF_Misc::Mem_Check(
	        reinterpret_cast<uint8_t*>( uC_App_Real_Fw_Start_Address() ),
	        ( uC_App_Fw_End_Address() - ( uC_App_Real_Fw_Start_Address()) ) + 1,
	        reinterpret_cast<uint16_t*>( uC_App_Fw_Crc_Start_Address() ) );
	    Enable_Interrupts();*/
#endif
	/*
	 * Inside this function we are doing OSCR_Dog initialization.
	 * In ESP32, Scheduler is getting started before entering into main.
	 * i.e. in start_cpu0() present in cpu_start.c
	 * So, we are not starting scheduler again.
	 */
	PROD_SPEC_Start_OS();

	return ( 0 );
}


void PxGreenOneSecondTimer()
{

    if (JustPoweredUpNoConnection == true)
    {
        if  ( (DeviceInfo.WiFiConnected == true) && (DeviceInfo.ConnectionStringSaved == true))
        {
            JustPoweredUpNoConnection = false;
#if defined ( PX_GREEN_IOT_SUPPORT ) && defined ( WIFI_STATION_MODE )
            PROD_SPEC_IOT_Init();
#endif
            DeviceInfo.PxInitiated = true;
        }
    }
}
