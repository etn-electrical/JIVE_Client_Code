/*
 **********************************************************************************************
 * @file            Prod_Spec_LTK_ESP32.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the ethernet/EMAC, NVRAM,  modbus, OS etc here
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "Test_Example.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "Debug_Manager_Config.h"
#include "Shell_Includes.h"
#if ( defined ( BF_DBG_IF_UART ) && defined( DEBUG_MANAGER ) )
#include "BF_Shell_Main.h"
#endif

#ifdef ETHERNET_SETUP
#include "Prod_Spec_Ethernet.h"
#endif
#ifdef DEBUG_MANAGER
#include "Prod_Spec_Debug_Manager.h"
#endif
#if defined ( WIFI_STATION_MODE ) || defined ( WIFI_ACCESS_POINT_MODE )
#include "Prod_Spec_Wifi.h"
#endif
#ifdef BLE_GATTS
#include "Prod_Spec_Ble.h"
#endif
#ifdef RTC_INTERFACE
#include "Prod_Spec_RTC.h"
#endif
#ifdef IOT_C2D_TIMING_TEST
#include "IOT_Test_Task.h"
#endif
#ifdef PKI_SUPPORT
#include "Prod_Spec_PKI.h"
#endif
#include "Prod_Spec_FUS.h"
#ifdef EIP_SETUP
#include "Prod_Spec_EIP.h"
#endif

#include "Prod_Spec_Services.h"
#if defined ( BACNET_MSTP_SETUP ) || defined ( BACNET_IP_SETUP )
#include "Prod_Spec_BACNET.h"
#endif
#ifdef PX_GREEN_IOT_SUPPORT
#include "Prod_Spec_IOT.h"
#endif	// #ifdef PX_GREEN_IOT_SUPPORT

//Ahmed
#include "Breaker.h"
extern DRAM_ATTR Device_Info_str DeviceInfo;

/**
 **********************************************************************************************
 * @brief                     Product Specific Target Main Init
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_Main_Init( void )
{
//Ahmed

    //Intiate EE
    InitEE();
	PROD_SPEC_SERVICES_Setup();
#ifdef RTC_INTERFACE
	Prod_Spec_RTC_Init();
#endif

#if ( defined ( BF_DBG_IF_UART ) && defined( DEBUG_MANAGER ) )
	Shell::BF_Shell_Initialize0();
#endif

#ifdef DEBUG_MANAGER
	PROD_SPEC_DEBUG_MANAGER_Init();
#endif

#ifdef ETHERNET_SETUP
	PROD_SPEC_ETH_Init();
#endif	// #ifdef ETHERNET_SETUP

#if defined ( WIFI_STATION_MODE ) || defined ( WIFI_ACCESS_POINT_MODE )
       //Ahmed
	//PROD_SPEC_WIFI_Init();
    if (DeviceInfo.DeviceInNetworkStatus == DEVICE_IS_IN_A_NETWORK)
    {
        WiFi_Connect();
    } 
#endif

#if ( defined( RTC_INTERFACE ) && defined( SNTP_SETUP ) )
	SNTP_Init();		///< SNTP Init
#endif

#ifdef PKI_SUPPORT
	// Code sign init, dev cert init, cert store interface init
	PROD_SPEC_PKI_Init();
#endif

	// Initialize the FUS functionality
	PROD_SPEC_FUS_Init();

#ifdef PX_GREEN_IOT_SUPPORT
	//Ahmed PROD_SPEC_IOT_Init();
#endif

#ifdef BLE_GATTS
	PROD_SPEC_BLE_Init();
#endif	// #ifdef BLE_GATTS

#ifdef EIP_SETUP
	PROD_SPEC_EIP_Init();
#endif

#if defined ( BACNET_MSTP_SETUP ) || defined ( BACNET_IP_SETUP )
	PROD_SPEC_BACNET_Init();
#endif

}
