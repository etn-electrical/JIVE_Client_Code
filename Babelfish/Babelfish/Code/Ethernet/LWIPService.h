/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LWIP_SERVICE_H
#define LWIP_SERVICE_H

#include "sntp_if.h"

typedef struct
{
	unsigned long IPAddr;
	unsigned long NetMask;
	unsigned long GWAddr;
	unsigned long IPMode;
} IP_CONFIG;

// *****************************************************************************
//
// IP Address Acquisition Modes
//
// *****************************************************************************
#define IPADDR_USE_STATIC       0
#define IPADDR_USE_DHCP         1
#define IPADDR_USE_AUTOIP       2

#define IP2LONG( a, b, c, d ) ( ( ( ( a ) << 24 ) & 0xFF000000 ) | \
								( ( ( b ) << 16 ) & 0x00FF0000 ) | \
								( ( ( c ) << 8 ) & 0x0000FF00 ) | \
								( ( ( d ) << 0 ) & 0x000000FF ) )

// *****************************************************************************
//
//! Prototypes for the APIs.
//
// *****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

void LWIPServiceTaskInit( void* pvParameters );

void LWIPServiceGetIPDef( void* pvParameters );

void LWIP_Service_TCPIP_Thread_CallBack( void* );

void Sntp_If_Init( SNTP_If* handle );

void LWIPServiceBringStaticNetifDown( struct netif* netif );

void LWIPServiceBringStaticNetifUp( struct netif* netif );

void LWIPServiceStaticLinkUp( struct netif* netif );

void Lwip_Update_Active_Ip_Config( void );

/**
 * @brief  : Function to return lwip_errno
 * @details: This function is used to return the value of Lwip_errno which contains the value of
 * last error that occurred for the calling thread.
 * @return error number of type int32.
 */

int32_t Get_LWIP_Errno( void );

#ifdef __cplusplus
}
#endif

#endif	// LWIP_SERVICE_H
