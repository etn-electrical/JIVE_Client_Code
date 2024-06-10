/**
 *****************************************************************************************
 * @file       EIP_if_C_Connector.h
 * @details    This file is similar to "EIP_if.h". Only difference is, it doesn't have
               dependency on C++ source files. This file is created specifically for Pyramid
               EIP stack in order to compile it with C compiler(Auto extension in IAR).
 * @copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef EIP_IF_C_CONNECTOR_H
#define EIP_IF_C_CONNECTOR_H


/*********************** Platform Specific Includes ************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "EtherNet_IP_Config.h"
#include "Etn_types.h"
#include "Device_Defines.h"

/**
 * @brief Used for boolean type data.
 */
#define TRUE    true
#define FALSE   false


#define INT8            char
// #define UINT8           unsigned char
#define INT16           signed short
// #define UINT16          unsigned short
#define INT32           signed int
// #define UINT32          unsigned int
#define SOCKET          int
// #define BOOL            bool
// #define UINT64          unsigned long




extern int32_t lwip_errno;

/* Can be cast to UINT8, UINT16, UINT32 or UINT8* depending on the type, so make sure
   pointer size is appropriate for platform */
#define ETAG_DATATYPE   UINT32

#define ET_IP_CALLBACK

#define INVALID_SOCKET      ( -1 )

/* IS_BYTE macro should return TRUE if x is a single byte and FALSE if its a 16 bit value */
#define IS_BYTE( x )  ( ( ( ( UINT16 )x & 0xFF00 ) == 0U ) ? TRUE : FALSE )
/* IS_WORD macro should return TRUE if x is a single word (16 bit value) and FALSE if its a 32 bit value */
#define IS_WORD( x )  ( ( ( ( UINT32 )x & 0xFFFF0000 ) == 0U ) ? TRUE : FALSE )

/* MIN and MAX macros return the higher or lower of two values */
#define PLATFORM_MIN( x, y ) ( ( x < y ) ? x : y )
#define PLATFORM_MAX( x, y ) ( ( x > y ) ? x : y )

#define PLATFORM_MUTEX_TYPE     QueueHandle_t
#define MUTEX_TIMEOUT           1	/* Maximum time to wait for the mutex before timing out */

#define EIP_WOULDBLOCK EWOULDBLOCK

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for OS_Tasker::Delay( uint32_t ).
 *  @param[in]    x, delay count.
 *  @return    None.
 */
void OS_Tasker_Delay( uint32_t x );

/**
 *  @brief    This is C/C++ bridge function for OS_Tasker::Get_Tick_Count().
 *  @param[in]    None.
 *  @return    Tick count.
 */
UINT32 OS_Tasker_Get_Tick_Count( void );

void platformSetSockaddrIn( struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );

void platformSocketLibInit( void );

void platformSocketInitTargetData( void );

UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen );

void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );

void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

void platformSocketLibCleanup();

// #ifdef EIP_ACD
BOOL platformIsDuplicateIPAddress( UINT32 lIPAddress );

BOOL platformDidIPAddressConflict( UINT32 lIPAddress );

// #endif

#ifdef __cplusplus
}
#endif

/* Redefinition of system mutex handling functions */
#define platformInitMutex( strMutexName )         xSemaphoreCreateMutex()
#define platformDiscardMutex( xEvent )
#define platformReleaseMutex( mutex )             xSemaphoreGive( mutex )
#define platformWaitMutex( mutex, wait )          xSemaphoreTake( mutex, wait )

#define platformSleep( x )                        OS_Tasker_Delay( x )		/* Sleep for x number of milliseconds */
#define platformGetCurrentTickCount()             OS_Tasker_Get_Tick_Count()

/* Redefinition of normal BSD style socket functions to named macros */
#define platformIoctl( sock, cmnd, argp )           ioctlsocket( sock, cmnd, ( unsigned long* )argp )
#define platformSetSockBufSize( p1, p2, p3, p4, p5 ) setsockopt( p1, p2, p3, p4, p5 )
#define platformSetSockOpt( s, l, o, d, dl )          setsockopt( s, l, o, d, dl )
#define platformCloseSocket( sock )               closesocket( sock )
#define platformGetLastSocketError( socket )      platformGetLastError()
// #define platformGetLastError()                  platformGetLastError()
#define platformGetLastError()                  ( errno )
#define platformAccept( s, a, l )                 accept( s, a, ( socklen_t* )l )
#define platformBind( s, a, l )                   bind( s, a, l )
#define platformConnect( s, a, al )                 connect( s, a, al )
#define platformGetPeerName( socket, name, len )  getpeername( socket, name, ( socklen_t* )len )
#define platformListen( s, i )                    listen( s, i )
#define platformRecv( s, b, l, f )                   recv( s, b, l, f )
#define platformRecvFrom( s, b, l, f, a, al )          recvfrom( s, b, l, f, a, ( socklen_t* )al )
#define platformSelect( n, i, o, e, t )           select( n, i, o, e, t )
#define platformSend( s, b, l, f )                   send( s, ( const char* )b, l, f )
#define platformSendTo( s, b, l, f, a, al, p )            sendto( s, ( const char* )b, l, f, a, al )


// #k: new from V4.4.4-----Start----------
/* This is only used to determine which IP address a UDP broadcast
 * message came in on.  If a device only supports a single IP address
 * this can just be set to ERROR, which will just use the first/only IP address
 */
#define platformGetSockName( s, n, l )            getsockname( s, n, l )
// #k: from V4.4.4-----End ----------
#define platformSocket( x, y, z )                 socket( x, y, z )
#define platformGetInternetAddress( addr )        addr.s_addr
#define platformInetNtoa( in )                    inet_ntoa( in )


#endif	// EIP_IF_C_CONNECTOR_H
