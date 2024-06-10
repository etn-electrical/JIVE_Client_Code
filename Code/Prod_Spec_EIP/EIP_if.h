/**
 *****************************************************************************************
 * @file       EIP_if.h
 * @details    This file contains declaration and Macros related to EIP interface.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef EIP_IF_H
#define EIP_IF_H

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
#include "EtherNet_IP_Config.h"
/******************** Platform Specific Definitions ************************/



#define INT8            char
#define UINT8           unsigned char
#define INT16           signed short
#define UINT16          unsigned short
#define INT32           signed int
#define UINT32          unsigned int
#define SOCKET          int
#define BOOL            bool

/* Can be cast to UINT8, UINT16, UINT32 or UINT8* depending on the type, so make sure
   pointer size is appropriate for platform */
#define ETAG_DATATYPE   UINT32

#define ET_IP_CALLBACK

#define INVALID_SOCKET      ( -1 )

/* IS_BYTE macro should return TRUE if x is a single byte and FALSE if its a 16 bit value */
#define IS_BYTE( x )  ( ( ( ( UINT16 )x & 0xFF00 ) == 0U ) ? TRUE : FALSE )
/* IS_WORD macro should return TRUE if x is a single word (16 bit value) and FALSE if its a 32 bit value */
#define IS_WORD( x )  ( ( ( ( UINT32 )x & 0xFFFF0000 ) == 0U ) ? TRUE : FALSE )

#if !defined( _T )
#define _T
#endif

/* MIN and MAX macros return the higher or lower of two values */

#define PLATFORM_MIN( x, y ) ( ( x < y ) ? x : y )
#define PLATFORM_MAX( x, y ) ( ( x > y ) ? x : y )

// eo #define PLATFORM_MUTEX_TYPE HANDLE  /* Native system mutex type */
// #define PLATFORM_MUTEX_TYPE OS_Mutex*	/* Native system mutex type */

// eo #define EWOULDBLOCK WSAEWOULDBLOCK /* Native socket system "would block" value */

// #define MUTEX_TIMEOUT    OS_MUTEX_INDEFINITE_TIMEOUT	/* Maximum time to wait for the mutex before timing out */

#define EIP_WOULDBLOCK EWOULDBLOCK

inline int strnicmp( const char* pStr1, const char* pStr2, size_t strLen )
{
	char c1, c2;
	int diff;

	if ( strLen == 0U )
	{
		return ( 0 );
	}

	do
	{
		c1 = *pStr1++;
		c2 = *pStr2++;
		diff = ( UINT8 ) tolower( c1 ) - ( UINT8 ) tolower( c2 );
	} while ( ( diff == 0U ) && ( c1 != '\0' ) && ( --strLen > 0 ) );

	return ( diff );
}

/*************************** Forward Declaration ******************************/

/********************************* Typedefs/externs ***********************************/

// Callback APIs for application
typedef bool_t (* TCPIP_APP_CALLBACK) ( void );

static TCPIP_APP_CALLBACK isTcpIpConfigCapabilityOn;
static TCPIP_APP_CALLBACK isIpAdrCntrlFromEipOn;
static TCPIP_APP_CALLBACK isIpAddrSettebleFromEthernet;

extern const Eth_Port::eth_dcid_t Eth_Port_Config[TOTAL_ETH_PORTS_IN_USE];

/************************ Public Function Prototypes **************************/
/**
 **********************************************************************************************
 * @brief                     EIP Set DCID
 * @param[in] dcid            dcid to be updated
 * @param[in] data            dcid data
 * @return[out] DCI_ERROR_TD  returns dci data error
 * @n
 **********************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Set_DCID( DCI_ID_TD dcid, uint8_t* data );

/**
 **********************************************************************************************
 * @brief                     EIP Get DCID
 * @param[in] dcid            dcid to be updated
 * @param[in] data            dcid data
 * @return[out] DCI_ERROR_TD  returns dci data error
 * @n
 **********************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID( DCI_ID_TD dcid, uint8_t* data );

/**
 **********************************************************************************************
 * @brief                     EIP Set DCID
 * @param[in] dcid            dcid to be updated
 * @param[in] data            dcid data
 * @return[out] DCI_ERROR_TD  returns dci data error
 * @n
 **********************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Set_DCID( DCI_ID_TD dcid, uint16_t* data );

/**
 **********************************************************************************************
 * @brief                     EIP Get DCID
 * @param[in] dcid            dcid to be accessed
 * @param[in] data            dcid data
 * @return[out] DCI_ERROR_TD  returns dci data error
 * @n
 **********************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID( DCI_ID_TD dcid, uint16_t* data );

/**
 **********************************************************************************************
 * @brief                      EIP get dcid length
 * @param[in] dcid             dcid to be accessed
 * @param[in] length           length to get dcid length
 * @return[out] DCI_ERROR_TD   returns dci data error
 * @n
 **********************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID_Len( DCI_ID_TD dcid, DCI_LENGTH_TD* length );

/**
 **********************************************************************************************
 * @brief                         Set callback for ip address from ethernet
 * @param[in] TCPIP_APP_CALLBACK  pCallback assigned which needs to be executed
 * @return[out] none              none
 * @n
 **********************************************************************************************
 */
void IpAdrSettebleFromEthCallback( TCPIP_APP_CALLBACK pCallback );

/**
 **********************************************************************************************
 * @brief                         Set callback for ip address control from EIP
 * @param[in] TCPIP_APP_CALLBACK  pCallback assigned which needs to be executed
 * @return[out] none              none
 * @n
 **********************************************************************************************
 */
void IpAdrCntrlFromEipCallback( TCPIP_APP_CALLBACK pCallback );

/**
 **********************************************************************************************
 * @brief                         Set callback for tcp/ip config capability
 * @param[in] TCPIP_APP_CALLBACK  pCallback assigned which needs to be executed
 * @return[out] none              none
 * @n
 **********************************************************************************************
 */
void TcpIpConfigCapabilityCallback( TCPIP_APP_CALLBACK pCallback );

#ifdef __cplusplus
extern "C" {
#endif
/**
 **********************************************************************************************
 * @brief                Creates a log file.
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformInitLogFile( void );

/**
 **********************************************************************************************
 * @brief                Closes the log file
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformCloseLogFile( void );

/**
 **********************************************************************************************
 * @brief                Writes a record to the log file
 * @param[in] UINT8      iSeverity
 * @param[in] UINT16     iType
 * @param[in] UINT832    lSrcIp
 * @param[in] UINT832    lDstIp
 * @param[in] char*      szBuf
 * @param[in] BOOL       bTimeStamp
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformWriteLog( UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char* szBuf,
					   BOOL bTimeStamp );

/**
 **********************************************************************************************
 * @brief                Initialize the sockets library.
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformSocketLibInit( void );

/**
 **********************************************************************************************
 * @brief                Unload Winsock
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformSocketLibCleanup();

/**
 **********************************************************************************************
 * @brief                Initialize galHostAddr array. It is passed as a response to the
 *                       List Targets UCMM request.
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformSocketInitTargetData( void );

/**
 **********************************************************************************************
 * @brief                Return current msec counter using a high-res performance timer.
 * @param[in] void       none
 * @return[out] UINT32   current msec counter using a high-res performance timer.
 * @n
 **********************************************************************************************
 */
UINT32 platformGetCurrentTickCount();

/**
 **********************************************************************************************
 * @brief                Get an IP address from the network path representation.
 * @param[in] szAddr     szAddr is usually an IP string (i.e."216.233.160.129").
 * @param[in] nLen       nLen to copy
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
UINT32 platformAddrFromPath( const char* szAddr, INT32 nLen );

/**
 **********************************************************************************************
 * @brief                Populates a sockaddr_in structure as an AF_INET type.  Used to
 *                       handle platform specific definitions of sockaddr_in
 *
 * @param[in] pSockaddr  Pointer to sockaddr_in structure to be populated
 * @param[in] iPort      Host order port number to be placed within this address structure
 * @param[in] lIpAddress Network order IP Address to be placed within this address
 * @return[out] none     none
 *
 **********************************************************************************************
 */
void platformSetSockaddrIn( struct sockaddr_in* pSockaddr, UINT16 iPort, UINT32 lIpAddress );

/**
 **********************************************************************************************
 * @brief                Populates an EIP "sockaddr_in" buffer with a sockaddr_in structure.
 * @param[in] void       none
 * @return[out] none     none
 * @n
 **********************************************************************************************
 */
void platformCopySockAddrInToBuffer( UINT8* pBuffer, struct sockaddr_in* pSockAddr );

/**
 **********************************************************************************************
 * @brief                  Populates a sockaddr_in structure with an EIP "sockaddr_in" buffer from
 *                         a packet.
 * @param[in] sockaddr_in* pSockAddr
 * @param[in] UINT8*       pBuffer
 * @return[out] none       none
 * @n
 **********************************************************************************************
 */
void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr, UINT8* pBuffer );

/**
 **********************************************************************************************
 * @brief                Sets the DSCP value for each socket to have QoS functionality
 * @param[in] SOCKET     Socket
 * @param[in] UINT8      bDSCPValue
 * @return[out] INT32    Returns socket error
 * @n
 **********************************************************************************************
 */
INT32 platformSetDSCPValue( SOCKET s, UINT8 bDSCPValue );

/**
 **********************************************************************************************
 * @brief                Determines if the current IP address used by the EtherNet/IP stack is a
 *                       duplicate IP address on the network.  Requires Address Conflict Detection
 *                       support (#define EIP_ACD) from the platform
 * @param[in] UINT32     lIPAddress
 * @return[out] BOOL     Returns operation status
 * @n
 **********************************************************************************************
 */
BOOL platformIsDuplicateIPAddress( UINT32 lIPAddress );

/**
 **********************************************************************************************
 * @brief                The IP address duplicate detection detected a collision but was either
 *                       able to defeat the other device or retreat.  This merely indicates that
 *                       a collision occurred.  Whether we retreated or not is in the following
 *                       function
 * @param[in] UINT32     lIPAddress
 * @return[out] BOOL     Returns operation status
 * @n
 **********************************************************************************************
 */
BOOL platformDidIPAddressConflict( UINT32 lIPAddress );

#ifdef __cplusplus
}
#endif
/********************************** Macros ************************************/

// #define platformSleep( x )                        OS_Tasker::Delay( x )		/* Sleep for x number of milliseconds */
// #define platformGetCurrentTickCount()           OS_Tasker::Get_Tick_Count()

/* Redefinition of system mutex handling functions */
// extern OS_Mutex* m_EipApiMutex;
// #define platformInitMutex( strMutexName )         m_EipApiMutex

// #define platformDiscardMutex( xEvent )

// #define platformReleaseMutex( mutex )             if ( mutex != NULL ){ ( mutex->Release() ); }
// #define platformWaitMutex( mutex, wait )          if ( mutex != NULL ){ ( mutex->Acquire( wait ) ); }

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
#define platformSocket( x, y, z )                   socket( x, y, z )
#define platformGetInternetAddress( addr )        addr.s_addr
#define platformInetNtoa( in )                    inet_ntoa( in )



/********************************** Prototypes ************************************/
/**
 **********************************************************************************************
 * @brief                     EIP platform initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void EIP_PLATFORM_Init( void );


#endif	/* #ifndef PLATFORM_H */
