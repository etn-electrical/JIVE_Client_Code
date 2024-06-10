/**
 **********************************************************************************************
 * @file			Prod_Spec_LTK_ESP32.h
 *
 * @brief			This file contain macro used to enable different code sets with different
 *                  functionality on ESP32
 *
 * @details			These macros has been used in test_main.cpp file to call specific
 *                  functions
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPEC_LTK_ESP32_H
#define PROD_SPEC_LTK_ESP32_H

/*
 *****************************************************************************************
 *		Defines for Enabling required module testing
 *
 *		ENABLE ONLY ONE DEFINE AT A TIME
 *
 *****************************************************************************************
 */

/*
 *	As of now ESP32 Sample App supports only one protocol at a time from Ethernet and Wifi
 *	If both were enabled, Code will disable ETHERNET_SETUP by default
 *
 *	@note: All Features level macro definition is
 *	       moved to project CMakeLists.txt.
 *	       To enable features, enable macro from project CMakeLists.txt
 */

/*
 ********************* TO ENABLE FAULT CATCHER **********************************************

    Required changes in SDK_Config
        1. Component config -> Core dump-
                            >> data destination-Flash.
                            >> Core dump data format-ELF format.
                            >> Core dump data integrity check- CRC32
        2. Component config -> ESP System Settings
                            >> Panic handler behavior – Print Registers and reboot.

     Confluence link: https://confluence-prod.tcc.etn.com/display/LTK/ESP32+Fault+Catcher+Setup+Guide

 *****************************************************************************************
 */

#endif	// #ifndef APP_IO_CONFIG_ESP32_H
