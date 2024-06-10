/**
 *****************************************************************************************
 * @file       platform_app.cpp
 * @details    Platform dependent declarations and definitions.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "eipinc.h"
#include "EIP_if.h"
#include "platform_app.h"
#include "Eth_if.h"
#include "Eth_Port.h"
#include "STDlib_MV.h"
#include "Exception.h"
#include "lwip/inet.h"
/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */


/*
 *******************************************************************************
 *		Prototypes
 *******************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Platform_App::EIP_CALLBACK_Init( void )
{
	TcpIpConfigCapabilityCallback( &VerifyConfigCapability );
	IpAdrCntrlFromEipCallback( &VerifyIpAdrCntrlFromEip );
	IpAdrSettebleFromEthCallback( &VerifyIpAdrSettebleFromEth );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Platform_App::VerifyConfigCapability( void )
{
	bool_t return_status = true;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Platform_App::VerifyIpAdrCntrlFromEip( void )
{
	bool_t return_status = true;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Platform_App::VerifyIpAdrSettebleFromEth( void )
{
	bool_t return_status = true;

	return ( return_status );
}
