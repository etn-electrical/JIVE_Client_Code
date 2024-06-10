/**
 *****************************************************************************************
 * @file       platform_app.cpp
 * @details    Platform dependent declarations and definitions.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */


#ifndef PLATFORM_APP_H
#define PLATFORM_APP_H

#include "includes.h"

/*********************** Platform Specific Includes ************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "OS_Tasker.h"
#include "OS_Queue.h"
#include "OS_Mutex.h"

#include "EthConfig.h"
#include "Eth_Port.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------
 */
class Platform_App
{
	public:
		/**
		 **********************************************************************************************
		 * @brief                    Constructor to create instance of Platform_App
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		Platform_App( void )
		{}

		/**
		 **********************************************************************************************
		 * @brief                    Destructor to delete instance of Platform_App
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		~Platform_App( void )
		{}

		/**
		 **********************************************************************************************
		 * @brief                    Initializes EIP callback
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void EIP_CALLBACK_Init( void );

	private:
		/**
		 **********************************************************************************************
		 * @brief                    Verify config capability
		 * @param[in] pCallback      none
		 * @return[out] bool_t       returns true
		 * @n
		 **********************************************************************************************
		 */
		static bool_t VerifyConfigCapability( void );

		/**
		 **********************************************************************************************
		 * @brief                    Verify IP address control from EIP
		 * @param[in] pCallback      none
		 * @return[out] bool_t       returns true
		 * @n
		 **********************************************************************************************
		 */
		static bool_t VerifyIpAdrCntrlFromEip( void );

		/**
		 **********************************************************************************************
		 * @brief                    Verify IP address Setteble from ethernet
		 * @param[in] pCallback      none
		 * @return[out] bool_t       returns true
		 * @n
		 **********************************************************************************************
		 */
		static bool_t VerifyIpAdrSettebleFromEth( void );

};


#endif	/* #ifndef PLATFORM_APP_H */
