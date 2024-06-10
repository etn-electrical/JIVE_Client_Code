/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPTCPIP.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** TCP/IP object 
**
*****************************************************************************
*/


#ifndef EIPTCPIP_H
#define EIPTCPIP_H

#include "EtherNet_IP_Config.h"


#define TCPIP_CLASS								0xF5		/* TCP/IP class identifier */
#define TCPIP_CLASS_REVISION					4			/* TCP/IP class revision */

/* Class and instance attribute numbers */
#define TCPIP_CLASS_ATTR_REVISION				1
#define TCPIP_CLASS_ATTR_MAX_INSTANCE			2
#define TCPIP_CLASS_ATTR_NUM_INSTANCES			3
#define TCPIP_CLASS_ATTR_OPT_ATTR_LIST			4
#define TCPIP_CLASS_ATTR_OPT_SVC_LIST			5
#define TCPIP_CLASS_ATTR_MAX_CLASS_ATTR			6
#define TCPIP_CLASS_ATTR_MAX_INST_ATTR			7

#define TCPIP_INST_ATTR_INTERFACE_STATUS		1
#define TCPIP_INST_ATTR_CONFIG_CAPABILITY		2
#define TCPIP_INST_ATTR_CONFIG_CONTROL			3
#define TCPIP_INST_ATTR_LINK_PATH				4
#define TCPIP_INST_ATTR_INTERFACE_CONFIG		5
#define TCPIP_INST_ATTR_HOST_NAME				6
#ifdef EIP_CIPSAFETY
#define TCPIP_INST_ATTR_SAFETY_NETWORK_NUMBER	7
#endif
#define TCPIP_INST_ATTR_TTL_VALUE				8
#define TCPIP_INST_ATTR_MCAST_CONFIG			9
#ifdef EIP_ACD
#define TCPIP_INST_ATTR_ACD_MODE				10
#define TCPIP_INST_ATTR_ACD_LAST_CONFLICT		11
#endif
#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
#define TCPIP_INST_ATTR_QUICK_CONNECT			12
#endif
#define TCPIP_INST_ATTR_ENCAP_TIMEOUT			13
#define TCPIP_INST_ATTR_IANA_PORT_ADMIN			14
#define TCPIP_INST_ATTR_IANA_PROTOCOL_ADMIN		15
#define TCPIP_INST_ATTR_ACTIVE_TCP_CONN			16
#define TCPIP_INST_ATTR_NON_CIP_ENCAP			17

#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
#define REDUCED_COUNT	0
#else
#define REDUCED_COUNT	1
#endif

#if defined (EIP_ACD) && defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
#define TCPIP_ATTR_OPT_COUNT				7 - REDUCED_COUNT
#elif defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
#define TCPIP_ATTR_OPT_COUNT				5 - REDUCED_COUNT
#elif defined EIP_ACD
#define TCPIP_ATTR_OPT_COUNT				6 - REDUCED_COUNT
#else
#define TCPIP_ATTR_OPT_COUNT				4 - REDUCED_COUNT
#endif

#define TCPIP_SET_PORT_ADMIN_STATE_SERVICE		0x4C
#define TCPIP_SET_PROTOCOL_ADMIN_STATE_SERVICE	0x4D


/* Attribute data lengths */
#define TCPIP_MAX_DOMAIN_LENGTH		48
#define TCPIP_MAX_HOST_LENGTH		64

/* Status attribute bit-map */
#define TCPIP_STATUS_NOT_CONFIGURED			0x00000000
#define TCPIP_STATUS_VALID_CONFIGURATION	0x00000001
#define TCPIP_STATUS_VALID_CONFIG_HARDWARE	0x00000002
#define TCPIP_STATUS_MCAST_PENDING			0x00000010
#define TCPIP_STATUS_INTERFACE_CONFIG_PENDING	0x00000020
#define TCPIP_STATUS_ACD_STATUS				0x00000040


/* Capability attribute bit-map */
#define TCPIP_CAPABILITY_BOOTP				0x00000001
#define TCPIP_CAPABILITY_DNS				0x00000002
#define TCPIP_CAPABILITY_DHCP				0x00000004
#define TCPIP_CAPABILITY_DHCP_DNS			0x00000008
#define TCPIP_CAPABILITY_SETTABLE_CONFIG	0x00000010
#define TCPIP_CAPABILITY_HARDWARE_CONFIG	0x00000020
#define TCPIP_CAPABILITY_IP_RESET_NEEDED	0x00000040
#define TCPIP_CAPABILITY_ACD				0x00000080

#define TCPIP_SETTABLE_CAPABILITY_MASK		0x0000001F

/* Control attribute bit-map */
#define TCPIP_CONTROL_CONFIG_BITMASK		0x00000007
#define TCPIP_CONTROL_USE_STORED_CONFIG		0x00000000
#define TCPIP_CONTROL_BOOTP					0x00000001
#define TCPIP_CONTROL_DHCP					0x00000002
#define TCPIP_CONTROL_DNS_ENABLE			0x00000010

#define TCPIP_MAX_LINK_PATH_LENGTH			12
#define TCPIP_MAX_ARP_PDU_LENGTH			28

#define TCPIP_SAFETY_NETWORK_NUMBER_SIZE	6

#define TCPIP_QUICK_CONNECT_ENABLED				1
#define TCPIP_QUICK_CONNECT_DISABLED			0

#define TCPIP_ENCAP_TIMEOUT_DEFAULT			120

/* Class attribute structure */
typedef struct tagTCPIP_CLASS_ATTR
{
	UINT16 iRevision;
	UINT16 iMaxInstance;
	UINT16 iNumInstances;
	UINT16 iMaxClassAttr;
	UINT16 iMaxInstanceAttr;
}
TCPIP_CLASS_ATTR;

#define TCPIP_CLASS_ATTR_SIZE				10
#define TCPIP_INTERFACE_CONFIG_SIZE			20
#define TCPIP_INTERFACE_MCAST_SIZE			8

typedef struct tagTCPIP_MCAST_DATA
{
	UINT8 bAllocControl;
	UINT8 bReserved;
	UINT16 iNumMcast;
	UINT32 lMcastStartAddr;
}
TCPIP_MCAST_DATA;

#define TCPIP_MAX_NUM_ADMIN_PORTS				10
#define TCPIP_ADMIN_PORT_NAME_SIZE				32			/* port name length including terminating 0 */

#define TCPIP_ADMIN_PROTOCOL_TCP				6
#define TCPIP_ADMIN_PROTOCOL_UDP				17

#define TCPIP_ADMIN_STATE_PORT_OPEN				1
#define TCPIP_ADMIN_STATE_PORT_CLOSE			0

#define TCPIP_ADMIN_CAP_FLAG_CONFIGURABLE		0x01
#define TCPIP_ADMIN_CAP_FLAG_RESET_REQUIRED		0x02

typedef struct tagTCPIP_ADMIN_PORT
{
	UINT8 portName[TCPIP_ADMIN_PORT_NAME_SIZE];
	UINT16 iPortNumber;
	UINT8 bProtocol;
	UINT8 bState;
	UINT8 bCapability;
}
TCPIP_ADMIN_PORT;


/* TCP/IP Configuration DATA */
typedef struct tagTCPIP_CONFIG_DATA
{
	UINT32 lConfigCapability;
	UINT32 lConfigControl;
	UINT16 iLinkObjPathSize;
	UINT8 LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
	UINT32 lIpAddr;
	UINT32 lSubnetMask;
	UINT32 lGatewayAddr;
	UINT32 lNameServer;
	UINT32 lNameServer2;
	UINT8 szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
	UINT8 szHostName[TCPIP_MAX_HOST_LENGTH+1];
#ifdef EIP_CIPSAFETY
	UINT8 SafetyNetworkNumber[TCPIP_SAFETY_NETWORK_NUMBER_SIZE];
#endif
	UINT8 bTTLValue;
	TCPIP_MCAST_DATA MulticastData;
#ifdef EIP_ACD
	UINT8 bACDMode;
	UINT8 bACDLCD_Phase;
	UINT8 ACDLCD_MacID[MAC_ADDR_LENGTH];
	UINT8 ACDLCD_ARPPdu[TCPIP_MAX_ARP_PDU_LENGTH];
#endif
#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
	UINT8 bQuickConnect;
#endif
	UINT16 iEncapTimeout;
}
TCPIP_CONFIG_DATA;

typedef struct tagTCPIP_INTERFACE_INSTANCE_ATTRIBUTES
{
	UINT32 lInterfaceStatus;
	TCPIP_CONFIG_DATA InterfaceConfig;
#ifdef EIP_NSLED_PER_IP
	EtIPNetworkStatus eSavedNetworkStatus;
#endif
#ifdef EIP_QOS
	QOS_INST_ATTR QoSConfig;
#endif
	struct sockaddr_in MulticastBaseAddr;				/* Multicast address base */
	UINT32 lMulticastAddrCount;							/* Number of used Multicast addresses */
	SOCKET alClass1Socket[NUM_CONNECTION_GROUPS];		/* UDP socket handling Class1 connections */
#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
	SOCKET* pNextClass1SocketToProcess;
#endif
	SOCKET lBroadcastSocket;							/* broadcast sockets */
	SOCKET lPassiveWaitSocket;							/* listening sockets array */
#ifdef TRACE_SOCKET_OUTPUT
	SOCKET lLogSrvSock;									/* Socket for sending log messages */
#endif
	UINT32 lListIdentityTimeout;						/* when to stop listening for list identity broadcase responses */
	UINT16 iPort;										/* TCP port used to send/recv (typically ENCAP_SERVER_PORT) */
	UINT32 lNonCIPEncapCount;							/* Number of non-CIP encapsulation messages received this second  */
	UINT32 lNonCIPEncapCountPrev;						/* Number of non-CIP encapsulation messages received last second */
}
TCPIP_INTERFACE_INSTANCE_ATTRIBUTES;

extern TCPIP_INTERFACE_INSTANCE_ATTRIBUTES gsTcpIpInstAttr[MAX_HOST_ADDRESSES];

extern void tcpipInit(void);
extern void tcpipSocketCleanupAll(void);
extern BOOL tcpipAddClaimedIPAddress(UINT32 lAddress, UINT16 iPort);
extern BOOL tcpipRemoveClaimedIPAddress(UINT32 lAddress);
extern TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* tcpipFindClaimedHostAddress( UINT32 lHostIPAddress );
extern void tcpipParseClassInstanceRequest( REQUEST* pRequest );
extern void tcpipReset(UINT8 bReset);

#ifdef __cplusplus
extern "C" {
#endif
  
BOOL platformGetTcpIpCfgData(TCPIP_CONFIG_DATA *psCfg);
void platformSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP);
BOOL platformTestSetTcpIpCfgData(UINT32 lMask, TCPIP_CONFIG_DATA *psCfg, UINT32 lCurrentIP, UINT8* pbGeneralError);

#ifdef __cplusplus
}
#endif
#ifdef EIP_QOS
extern void platformGetQoSAttributes( TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig );
extern BOOL platformSetQoSAttributes(UINT32 lMask, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig );
#endif

#endif /* #ifndef EIPTCPIP_H */


