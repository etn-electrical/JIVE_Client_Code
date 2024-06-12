/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPTCPIP.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** TCP/IP object implementation 
**
*****************************************************************************
*/

#include "eipinc.h"

TCPIP_INTERFACE_INSTANCE_ATTRIBUTES gsTcpIpInstAttr[MAX_HOST_ADDRESSES];

static UINT16 tcpipOptionalAttributes[TCPIP_ATTR_OPT_COUNT] = {TCPIP_INST_ATTR_TTL_VALUE, TCPIP_INST_ATTR_MCAST_CONFIG
#ifdef EIP_ACD
	, TCPIP_INST_ATTR_ACD_MODE, TCPIP_INST_ATTR_ACD_LAST_CONFLICT
#endif
#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
	, TCPIP_INST_ATTR_QUICK_CONNECT
#endif
	, TCPIP_INST_ATTR_ACTIVE_TCP_CONN, 
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
	TCPIP_INST_ATTR_NON_CIP_ENCAP
#endif
};

/*---------------------------------------------------------------------------
** tcpipInit( )
**
** Initialize TCP/IP object
**---------------------------------------------------------------------------
*/
void tcpipInit(void)
{
	gnClaimedHostIPAddrCount = 0;
}	

static void tcpipInitConfig(INT32 iIndex)
{
	SOCKET *pSocket;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig = &gsTcpIpInstAttr[iIndex];

	memset( pTcpipConfig, 0, sizeof(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES) );

	pTcpipConfig->InterfaceConfig.lIpAddr = INADDR_NONE;
	pTcpipConfig->lInterfaceStatus = TCPIP_STATUS_VALID_CONFIGURATION;
#ifdef EIP_NSLED_PER_IP
	pTcpipConfig->eSavedNetworkStatus = NetworkStatusOff;
#endif

	pTcpipConfig->lMulticastAddrCount = 0;

	/* Initialize the Class1 sockets array */
	for (pSocket = pTcpipConfig->alClass1Socket; pSocket < &pTcpipConfig->alClass1Socket[NUM_CONNECTION_GROUPS]; pSocket++)
		(*pSocket) = (SOCKET)INVALID_SOCKET;
#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
	pTcpipConfig->pNextClass1SocketToProcess = pTcpipConfig->alClass1Socket;
#endif

	/* initialize broadcast socket */
	pTcpipConfig->lBroadcastSocket = (SOCKET)INVALID_SOCKET;

	/* Initialize the listening socket */
	pTcpipConfig->lPassiveWaitSocket = (SOCKET)INVALID_SOCKET;

#ifdef TRACE_SOCKET_OUTPUT
	/* Initialize the log server socket */
	pTcpipConfig->lLogSrvSock = (SOCKET)INVALID_SOCKET;
#endif

	pTcpipConfig->iPort = ENCAP_SERVER_PORT;
}

/*---------------------------------------------------------------------------
** tcpipSocketCleanup( )
**
** Cleanup sockets on TCP/IP interface
**---------------------------------------------------------------------------
*/
static void tcpipSocketCleanup(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig)
{
	SOCKET* pSocket;

	/* close broadcast socket */
	if (pTcpipConfig->lBroadcastSocket != (SOCKET)INVALID_SOCKET)
	{
		platformCloseSocket(pTcpipConfig->lBroadcastSocket);
		pTcpipConfig->lBroadcastSocket = (SOCKET)INVALID_SOCKET;
	}

	/* close listening socket */
	if (pTcpipConfig->lPassiveWaitSocket != (SOCKET)INVALID_SOCKET)
	{
		platformCloseSocket(pTcpipConfig->lPassiveWaitSocket);
		pTcpipConfig->lPassiveWaitSocket = (SOCKET)INVALID_SOCKET;
	}

	/* close class1 sockets */
	for (pSocket = pTcpipConfig->alClass1Socket; pSocket < &pTcpipConfig->alClass1Socket[NUM_CONNECTION_GROUPS]; pSocket++)
	{
		if (*pSocket != (SOCKET)INVALID_SOCKET)
		{
			platformCloseSocket(*pSocket);
			(*pSocket) = (SOCKET)INVALID_SOCKET;
		}
	}

#ifdef TRACE_SOCKET_OUTPUT
	/* close logging socket */
	if (pTcpipConfig->lLogSrvSock != (SOCKET)INVALID_SOCKET)
	{
		platformCloseSocket(pTcpipConfig->lLogSrvSock);
		pTcpipConfig->lLogSrvSock = (SOCKET)INVALID_SOCKET;
	}
#endif
	
}

/*---------------------------------------------------------------------------
** tcpipSocketCleanupAll( )
**
** Cleanup sockets on all interfaces
**---------------------------------------------------------------------------
*/
void tcpipSocketCleanupAll(void)
{
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;
	UINT32 i;

	for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
		 i++, pTcpipConfig++)
	{
		tcpipSocketCleanup(pTcpipConfig);
	}

	/* Unload socket library */
	platformSocketLibCleanup();
}

/*---------------------------------------------------------------------------
** tcpipFindClaimedHostAddress( )
**
** Return the TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* for the particular claimed Host IP Address
**---------------------------------------------------------------------------
*/
TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* tcpipFindClaimedHostAddress( UINT32 lHostIPAddress )
{
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIp;

	for(i = 0, pTcpIp = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount;
		i++, pTcpIp++)
	{
		if(pTcpIp->InterfaceConfig.lIpAddr == lHostIPAddress )
		return pTcpIp;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** tcpipGetMcastBaseAddr( )
**
** Prepare the starting multicast address based on our IP address and subnet
** mask.
**---------------------------------------------------------------------------
*/
static void tcpipGetMcastBaseAddr(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig)
{
	UINT32 host;
	UINT32 ma;

	host = pTcpipConfig->InterfaceConfig.lIpAddr & (~pTcpipConfig->InterfaceConfig.lSubnetMask);

	/* Use the host number to determine base address */
	ma = CIP_MCAST_BASE + (((htonl(host) - 1) & CIP_MCAST_HMASK) << CIP_MCAST_BSHIFT);

	/* Store starting multicast address in the gMulticastBaseAddr variable */
	platformSetSockaddrIn( &pTcpipConfig->MulticastBaseAddr, CLASS1_UDP_PORT, htonl(ma) );

	pTcpipConfig->lMulticastAddrCount = 0;
}

/*---------------------------------------------------------------------------
** tcpipAddClaimedIPAddress( )
**
** Start using an IP address on EtherNet/IP
**---------------------------------------------------------------------------
*/
BOOL tcpipAddClaimedIPAddress(UINT32 lAddress, UINT16 iPort)
{
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIp;

	tcpipInitConfig(gnClaimedHostIPAddrCount);

	pTcpIp = &gsTcpIpInstAttr[gnClaimedHostIPAddrCount];
	pTcpIp->InterfaceConfig.lIpAddr = lAddress;
	if (iPort > 0)
		pTcpIp->iPort = iPort;

	if (!platformGetTcpIpCfgData(&pTcpIp->InterfaceConfig))
		return FALSE;

#ifdef ET_IP_MODBUS
	if (!modbusClaimIPAddress(lAddress))
		return FALSE;
#endif

	/* store the multicast base address for future use */
	tcpipGetMcastBaseAddr(pTcpIp);

	/* Setup the listening socket */
	socketPostListen(&pTcpIp->lPassiveWaitSocket, pTcpIp->InterfaceConfig.lIpAddr, pTcpIp->iPort);
	if (pTcpIp->lPassiveWaitSocket == (SOCKET)INVALID_SOCKET)
		return FALSE;

	/* Setup the broadcast socket */
	pTcpIp->lBroadcastSocket = socketClass1Init(TRUE, SMALL_BUFFER_SIZE, pTcpIp->InterfaceConfig.lIpAddr, ENCAP_SERVER_PORT, FALSE);
	if (pTcpIp->lBroadcastSocket == (SOCKET)INVALID_SOCKET)
		return FALSE;

#ifdef TRACE_SOCKET_OUTPUT
	/* Setup the logging socket */
	traceSocketInitServerSock(&pTcpIp->lLogSrvSock, pTcpIp->InterfaceConfig.lIpAddr);
#endif

#ifdef EIP_QOS
	platformGetQoSAttributes( pTcpIp );
#endif

	gnClaimedHostIPAddrCount++;
	return TRUE;
}

/*---------------------------------------------------------------------------
** tcpipRemoveClaimedIPAddress( )
**
** Stop using an IP address on EtherNet/IP
**---------------------------------------------------------------------------
*/
BOOL tcpipRemoveClaimedIPAddress(UINT32 lAddress)
{
	UINT32 i;
	SESSION* pSession;
	CONNECTION* pConnection;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES *pTcpipConfig, *pLoopTcpipConfig;

	for(i = 0, pTcpipConfig = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount;
		i++, pTcpipConfig++)
	{
		if (pTcpipConfig->InterfaceConfig.lIpAddr == lAddress)
			break;
	}

	/* IP address not found */
	if (i == gnClaimedHostIPAddrCount)
		return FALSE;

	/* remove all sessions associated with this IP address */
	for( pSession = gpnSessions-1; pSession >= gSessions; pSession-- )
	{
		if (pSession->lHostIPAddr == lAddress)
			sessionRemove(pSession, FALSE);
	}

	/* remove all connections associated with this IP address */
	for( pConnection = gpnConnections-1; pConnection >= gConnections; pConnection-- )
	{
		if (pConnection->cfg.lHostIPAddr == lAddress)
			connectionRemove(pConnection, FALSE);
	}

	tcpipSocketCleanup(pTcpipConfig);

#ifdef ET_IP_MODBUS
	modbusUnclaimIPAddress(lAddress);
#endif

	/* Shift the TCP/IP configurations with the higher index to fill in the void */
	for( pLoopTcpipConfig = pTcpipConfig; pLoopTcpipConfig < &gsTcpIpInstAttr[gnClaimedHostIPAddrCount]; pLoopTcpipConfig++)
		memcpy( pLoopTcpipConfig, pLoopTcpipConfig+1, sizeof(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES));

	gnClaimedHostIPAddrCount--;
	return TRUE;
}

/*---------------------------------------------------------------------------
** tcpipIsValidIPAddress( )
**
** Ensures the IP address is in the Class A, Class B or Class C range
**---------------------------------------------------------------------------
*/
static BOOL tcpipIsValidIPAddress(UINT32 lIPAddress)
{
	if ((htonl(lIPAddress) < 0xE0000000) &&	  /* Class A, B, or C */
		(htonl(lIPAddress) != 0x7F000001)) /* localhost */
	{
		return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** tcpipIsValidSubnetMask( )
**
** Ensures subnet mask is made of consecutive 1s, followed by consecutive 0s.
**---------------------------------------------------------------------------
*/
static BOOL tcpipIsValidSubnetMask(UINT32 subnetMask)
{
	/* The subnet mask must contiguous 1's, from 0.0.0.0 to 255.255.255.252 */
	UINT32 testBit = 0x80000000;   /* start from the left most bit */
	UINT32 test2Bits = 0x00000003;   
	INT32 bitTested = 0;

	/* Maximum 30 1's */
	if (subnetMask & test2Bits)
		return FALSE;

	/* Check the contiguous 1's */
	while (bitTested < 32)
	{
		if (!(subnetMask & testBit))
			break;

		testBit = testBit >> 1;
		bitTested++;
	}

	while (bitTested < 30)
	{
		if (subnetMask & testBit)
			return FALSE;

		testBit = testBit >> 1;
		bitTested++;
	}

	return TRUE;
}

static void tcpipUpdateIPAddress(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig, UINT32 lNewIPAddress)
{
#ifdef ET_IP_MODBUS
	UINT32 lOldIPAddress = pTcpipConfig->InterfaceConfig.lIpAddr;
#endif
	SESSION* pSession;
	
	/* remove all sessions associated with this IP address */
	for( pSession = gpnSessions-1; pSession >= gSessions; pSession-- )
	{
		if (pSession->lHostIPAddr == pTcpipConfig->InterfaceConfig.lIpAddr)
			sessionRemove(pSession, FALSE);
	}

	tcpipSocketCleanup(pTcpipConfig);

	pTcpipConfig->InterfaceConfig.lIpAddr = lNewIPAddress;
	
	if (!platformGetTcpIpCfgData(&pTcpipConfig->InterfaceConfig))
		return;

#ifdef ET_IP_MODBUS
	modbusUpdateClaimIPAddress(lOldIPAddress, lNewIPAddress);
#endif

	/* store the multicast base address for future use */
	tcpipGetMcastBaseAddr(pTcpipConfig);

	/* Setup the listening socket */
	socketPostListen(&pTcpipConfig->lPassiveWaitSocket, pTcpipConfig->InterfaceConfig.lIpAddr, pTcpipConfig->iPort);

	/* Setup the broadcast socket */
	pTcpipConfig->lBroadcastSocket = socketClass1Init(TRUE, SMALL_BUFFER_SIZE, pTcpipConfig->InterfaceConfig.lIpAddr, ENCAP_SERVER_PORT, FALSE);

#ifdef TRACE_SOCKET_OUTPUT
	/* Setup the logging socket */
	traceSocketInitServerSock(&pTcpipConfig->lLogSrvSock, pTcpipConfig->InterfaceConfig.lIpAddr);
#endif
}

/*---------------------------------------------------------------------------
** tcpipSendClassAttrAll( )
**
** GetAttributeAll service for TCP/IP class is received 
**---------------------------------------------------------------------------
*/
static void tcpipSendClassAttrAll( REQUEST* pRequest )
{
	UINT8 *pData;
	UINT16 i, *pAttrData;

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (sizeof(UINT16)*(5+TCPIP_ATTR_OPT_COUNT+2)));
	if (pRequest->cfg.objectCfg.pData == NULL)
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, TCPIP_CLASS_REVISION);
	pData += sizeof(UINT16);
	UINT16_SET(pData, ((UINT16)gnClaimedHostIPAddrCount));
	pData += sizeof(UINT16);
	UINT16_SET(pData, ((UINT16)gnClaimedHostIPAddrCount));
	pData += sizeof(UINT16);

	/* optional attributes */
	UINT16_SET(pData, TCPIP_ATTR_OPT_COUNT);
	pData += sizeof(UINT16);

	for (i = 0, pAttrData = tcpipOptionalAttributes; 
		i < TCPIP_ATTR_OPT_COUNT; 
		i++, pAttrData++, pData += sizeof(UINT16))
	{
		UINT16_SET(pData, *pAttrData);
	}

	/* no optional services */
	UINT16_SET(pData, 0);
	pData += sizeof(UINT16);

	UINT16_SET(pData, TCPIP_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
	UINT16_SET(pData, TCPIP_INST_ATTR_NON_CIP_ENCAP);
#else
	UINT16_SET(pData, TCPIP_INST_ATTR_ACTIVE_TCP_CONN);
#endif
}

/*---------------------------------------------------------------------------
** tcpipSendClassAttrSingle( )
**
** GetAttributeSingle service for TCP/IP class is received 
**---------------------------------------------------------------------------
*/
static void tcpipSendClassAttrSingle( REQUEST* pRequest )
{
	UINT16 iVal, iTagSize, i;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT8* pData;
	UINT16 *pAttrData;

	switch( iAttribute )
	{
	case TCPIP_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(TCPIP_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case TCPIP_CLASS_ATTR_MAX_INSTANCE:
		iVal = ENCAP_TO_HS((UINT16)gnClaimedHostIPAddrCount);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case TCPIP_CLASS_ATTR_NUM_INSTANCES:
		iVal = ENCAP_TO_HS((UINT16)gnClaimedHostIPAddrCount);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case TCPIP_CLASS_ATTR_OPT_ATTR_LIST:
		EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((TCPIP_ATTR_OPT_COUNT+1)*sizeof(UINT16)));

		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, TCPIP_ATTR_OPT_COUNT);
		pData += sizeof(UINT16);

		for (i = 0, pAttrData = tcpipOptionalAttributes;
			i < TCPIP_ATTR_OPT_COUNT;
			i++, pAttrData++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *pAttrData);
		}
		break;
	case TCPIP_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(TCPIP_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case TCPIP_CLASS_ATTR_MAX_INST_ATTR:
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
		iVal = ENCAP_TO_HS(TCPIP_INST_ATTR_NON_CIP_ENCAP);
#else
		iVal = ENCAP_TO_HS(TCPIP_INST_ATTR_ACTIVE_TCP_CONN);
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}
}

static UINT16 tcpipGetActiveTCPConnections(UINT32 lIPAddress)
{
	SESSION* pSession;
	UINT16 iNumActiveConn = 0;
	for( pSession = gSessions; pSession < gpnSessions; pSession++)
	{
		if (pSession->lHostIPAddr == lIPAddress)
			iNumActiveConn++;
	}

	return iNumActiveConn;
}

/*---------------------------------------------------------------------------
** tcpipSendInstanceAttrAll( )
**
** GetAttributeAll service for TCP/IP instance is received
**---------------------------------------------------------------------------
*/
static void tcpipSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = &gsTcpIpInstAttr[iInstance-1];
	UINT16 iHostNameLen, iDomainNameLen, iNumActiveConn;
	UINT32 lVal;

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	/* get the latest TCP/IP information */
	platformGetTcpIpCfgData(&pTcpipInterface->InterfaceConfig);

	iDomainNameLen = (UINT16)strlen((const char*)pTcpipInterface->InterfaceConfig.szDomainName);
	iHostNameLen = (UINT16)strlen((const char*)pTcpipInterface->InterfaceConfig.szHostName);

	pRequest->cfg.objectCfg.iDataSize = (UINT16)( sizeof(UINT32) * 3 + sizeof(UINT16) + pTcpipInterface->InterfaceConfig.iLinkObjPathSize + TCPIP_INTERFACE_CONFIG_SIZE +
		iHostNameLen + sizeof(UINT16) + iDomainNameLen + sizeof(UINT16) + sizeof(UINT8)*7 + TCPIP_INTERFACE_MCAST_SIZE
		+ (sizeof(UINT8)*2)+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH /* ACD */
		+ sizeof(UINT8) /* Quick connect */
		+ sizeof(UINT16) /* Encapsulation Inactivity Timeout */
		+ sizeof(UINT16) /* Active TCP Connections */
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
		+ sizeof(UINT32) /* Non-CIP Encapsulation Messages Per Second */
#endif
		);

	if ( iDomainNameLen & 1 )
		pRequest->cfg.objectCfg.iDataSize++;

	if ( iHostNameLen & 1 )
		pRequest->cfg.objectCfg.iDataSize++;

	pRequest->cfg.objectCfg.pData = EtIP_malloc( NULL, pRequest->cfg.objectCfg.iDataSize );

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT32_SET( pData, pTcpipInterface->lInterfaceStatus );
	pData += sizeof(UINT32);

	UINT32_SET( pData, pTcpipInterface->InterfaceConfig.lConfigCapability );
	pData += sizeof(UINT32);

	UINT32_SET( pData, pTcpipInterface->InterfaceConfig.lConfigControl );
	pData += sizeof(UINT32);

	UINT16_SET( pData, pTcpipInterface->InterfaceConfig.iLinkObjPathSize/2 );
	pData += sizeof(UINT16);

	if ( pTcpipInterface->InterfaceConfig.iLinkObjPathSize )
	{
		memcpy( pData, pTcpipInterface->InterfaceConfig.LinkObjPath, pTcpipInterface->InterfaceConfig.iLinkObjPathSize );
		pData += pTcpipInterface->InterfaceConfig.iLinkObjPathSize;
	}

	/*************************************************************
	* Address data is expected to be IP's network order (big endian)
	* Because CIP expects data to be in host order (little endian), the bytes 
	* must be swapped
	**************************************************************/

	lVal = ntohl(pTcpipInterface->InterfaceConfig.lIpAddr);
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

	lVal = ntohl(pTcpipInterface->InterfaceConfig.lSubnetMask);
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

	lVal = ntohl(pTcpipInterface->InterfaceConfig.lGatewayAddr);
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

	lVal = ntohl(pTcpipInterface->InterfaceConfig.lNameServer);
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

	lVal = ntohl(pTcpipInterface->InterfaceConfig.lNameServer2);
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

	UINT16_SET( pData, iDomainNameLen );
	pData += sizeof(UINT16);

	strncpy( (char*)pData, (const char*)pTcpipInterface->InterfaceConfig.szDomainName, iDomainNameLen );
	pData += iDomainNameLen;

	if ( iDomainNameLen & 1 )
		*pData++ = 0;

	UINT16_SET( pData, iHostNameLen );
	pData += sizeof(UINT16);

	strncpy( (char*)pData, (const char*)pTcpipInterface->InterfaceConfig.szHostName, iHostNameLen );
	pData += iHostNameLen;

	if ( iHostNameLen & 1 )
		*pData++ = 0;

	/* Attribute 7 is only used in CIP-Safety */
#ifdef EIP_CIPSAFETY
	memcpy(pData, pTcpipInterface->InterfaceConfig.SafetyNetworkNumber, TCPIP_SAFETY_NETWORK_NUMBER_SIZE);
#else
	memset(pData, 0, TCPIP_SAFETY_NETWORK_NUMBER_SIZE);
#endif
	pData += TCPIP_SAFETY_NETWORK_NUMBER_SIZE;

	/* TTL Value */
	*pData = pTcpipInterface->InterfaceConfig.bTTLValue;
	pData++;

	/* Multicast Configuration */
	*pData = pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl;
	pData++;
	*pData = 0;		/* Reserved */
	pData++;
	UINT16_SET(pData, pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast);
	pData += sizeof(UINT16);

	switch(pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl)
	{
	case 0:
		lVal = ntohl(platformGetInternetAddress(pTcpipInterface->MulticastBaseAddr.sin_addr));
		break;
	case 1:
		lVal = ntohl(pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr);
		break;
	default:
		lVal = 0;
		break;
	}
	UINT32_SET( pData, lVal);
	pData += sizeof(UINT32);

#ifdef EIP_ACD
	*pData = pTcpipInterface->InterfaceConfig.bACDMode;
	pData++;
	*pData = pTcpipInterface->InterfaceConfig.bACDLCD_Phase;
	pData++;
	memcpy(pData, pTcpipInterface->InterfaceConfig.ACDLCD_MacID, MAC_ADDR_LENGTH);
	pData += MAC_ADDR_LENGTH;
	memcpy(pData, pTcpipInterface->InterfaceConfig.ACDLCD_ARPPdu, TCPIP_MAX_ARP_PDU_LENGTH);
	pData += TCPIP_MAX_ARP_PDU_LENGTH;
#else
	/* Need to pad ACD attributes */
	memset(pData, 0, 2 + MAC_ADDR_LENGTH + TCPIP_MAX_ARP_PDU_LENGTH);
	pData += (2 + MAC_ADDR_LENGTH + TCPIP_MAX_ARP_PDU_LENGTH);
#endif

#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
	*pData = pTcpipInterface->InterfaceConfig.bQuickConnect;
#else
	*pData = 0;
#endif
	pData++;

	UINT16_SET( pData, pTcpipInterface->InterfaceConfig.iEncapTimeout );
	pData += sizeof(UINT16);

	/* Calculate the number of active TCP connections for this IP address */
	iNumActiveConn = tcpipGetActiveTCPConnections(pTcpipInterface->InterfaceConfig.lIpAddr);
	UINT16_SET(pData, iNumActiveConn);
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
	pData += sizeof(UINT16);
	UINT32_SET( pData, pTcpipInterface->lNonCIPEncapCountPrev);
#endif
}


/*--------------------------------------------------------------------------------
** tcpipSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the TCP/IP object instance
**--------------------------------------------------------------------------------
*/
static void tcpipSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;
	UINT32 lVal;
	UINT16 iDataSize, iLen, iTagSize, iVal;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = &gsTcpIpInstAttr[iInstance-1];

	/* get the latest TCP/IP information */
	platformGetTcpIpCfgData(&pTcpipInterface->InterfaceConfig);

	switch( iAttribute )
	{
	case TCPIP_INST_ATTR_INTERFACE_STATUS:
		lVal = ENCAP_TO_HL(pTcpipInterface->lInterfaceStatus);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;

	case TCPIP_INST_ATTR_CONFIG_CAPABILITY:
		lVal = ENCAP_TO_HL(pTcpipInterface->InterfaceConfig.lConfigCapability);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;

	case TCPIP_INST_ATTR_CONFIG_CONTROL:
		lVal = ENCAP_TO_HL(pTcpipInterface->InterfaceConfig.lConfigControl);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;

	case TCPIP_INST_ATTR_LINK_PATH:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT16) + pTcpipInterface->InterfaceConfig.iLinkObjPathSize) );
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			UINT16_SET( pRequest->cfg.objectCfg.pData, pTcpipInterface->InterfaceConfig.iLinkObjPathSize/2 );

			if ( pTcpipInterface->InterfaceConfig.iLinkObjPathSize )
				memcpy( pRequest->cfg.objectCfg.pData+sizeof(UINT16), pTcpipInterface->InterfaceConfig.LinkObjPath, pTcpipInterface->InterfaceConfig.iLinkObjPathSize );
		}
		break;

	case TCPIP_INST_ATTR_INTERFACE_CONFIG:
		iLen = (UINT16)strlen((const char*)pTcpipInterface->InterfaceConfig.szDomainName);
		iDataSize = (UINT16)( TCPIP_INTERFACE_CONFIG_SIZE + iLen + sizeof(UINT16) );
		if ( iDataSize & 1)  /* Pad with 0 if domain name length is odd */
			iDataSize++;

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iDataSize );
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;

			/*************************************************************
			* Address data is expected to be IP's network order (big endian)
			* Because CIP expects data to be in host order (little endian), the bytes
			* must be swapped
			**************************************************************/
			lVal = ntohl(pTcpipInterface->InterfaceConfig.lIpAddr);
			UINT32_SET( pData, lVal);
			pData += sizeof(UINT32);

			lVal = ntohl(pTcpipInterface->InterfaceConfig.lSubnetMask);
			UINT32_SET( pData, lVal);
			pData += sizeof(UINT32);

			lVal = ntohl(pTcpipInterface->InterfaceConfig.lGatewayAddr);
			UINT32_SET( pData, lVal);
			pData += sizeof(UINT32);

			lVal = ntohl(pTcpipInterface->InterfaceConfig.lNameServer);
			UINT32_SET( pData, lVal);
			pData += sizeof(UINT32);

			lVal = ntohl(pTcpipInterface->InterfaceConfig.lNameServer2);
			UINT32_SET( pData, lVal);
			pData += sizeof(UINT32);

			UINT16_SET( pData, iLen );
			pData += sizeof(UINT16);

			strncpy( (char*)pData, (const char*)pTcpipInterface->InterfaceConfig.szDomainName, iLen );
			pData += iLen;

			if ( iLen & 1)  /* Pad with 0 if domain name length is odd */
				*pData++ = 0;
		}
		break;

	case TCPIP_INST_ATTR_HOST_NAME:
		iLen = (UINT16)strlen((const char*)pTcpipInterface->InterfaceConfig.szHostName);
		iDataSize = (UINT16)( iLen + sizeof(UINT16) );
		if ( iDataSize & 1)  /* Pad with 0 if host name length is odd */
			iDataSize++;
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iDataSize );
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			UINT16_SET( pData, iLen );
			pData += sizeof(UINT16);
			strncpy( (char*)pData, (const char*)pTcpipInterface->InterfaceConfig.szHostName, iLen );
			if ( iLen & 1)  /* Pad with 0 if domain name length is odd */
			{
				pData += iLen;
				*pData = 0;
			}
		}
		break;
#ifdef EIP_CIPSAFETY
	case TCPIP_INST_ATTR_SAFETY_NETWORK_NUMBER:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize,
						pTcpipInterface->InterfaceConfig.SafetyNetworkNumber, TCPIP_SAFETY_NETWORK_NUMBER_SIZE);
		break;
#endif
	case TCPIP_INST_ATTR_TTL_VALUE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pTcpipInterface->InterfaceConfig.bTTLValue, sizeof(UINT8) );
		break;
	case TCPIP_INST_ATTR_MCAST_CONFIG:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, TCPIP_INTERFACE_MCAST_SIZE);
		if ( pRequest->cfg.objectCfg.pData == NULL)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			break;
		}

		pData = pRequest->cfg.objectCfg.pData;
		*pData = pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl;
		pData++;
		*pData = pTcpipInterface->InterfaceConfig.MulticastData.bReserved;
		pData++;
		UINT16_SET( pData, pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast);
		pData += sizeof(UINT16);
		switch(pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl)
		{
		case 0:
			lVal = ntohl(platformGetInternetAddress(pTcpipInterface->MulticastBaseAddr.sin_addr));
			break;
		case 1:
			lVal = ntohl(pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr);
			break;
		default:
			lVal = 0;
			break;
		}
		UINT32_SET( pData, lVal);
		break;
#ifdef EIP_ACD
	case TCPIP_INST_ATTR_ACD_MODE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pTcpipInterface->InterfaceConfig.bACDMode, sizeof(UINT8) );
		break;
	case TCPIP_INST_ATTR_ACD_LAST_CONFLICT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 1+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH);
		if ( pRequest->cfg.objectCfg.pData == NULL)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			break;
		}

		pData = pRequest->cfg.objectCfg.pData;
		*pData = pTcpipInterface->InterfaceConfig.bACDLCD_Phase;
		pData++;
		memcpy(pData, pTcpipInterface->InterfaceConfig.ACDLCD_MacID, MAC_ADDR_LENGTH);
		pData += MAC_ADDR_LENGTH;
		memcpy(pData, pTcpipInterface->InterfaceConfig.ACDLCD_ARPPdu, TCPIP_MAX_ARP_PDU_LENGTH);

		break;
#endif
#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
	case TCPIP_INST_ATTR_QUICK_CONNECT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pTcpipInterface->InterfaceConfig.bQuickConnect, sizeof(UINT8) );
		break;
#endif
	case TCPIP_INST_ATTR_ENCAP_TIMEOUT:
		iVal = ENCAP_TO_HS(pTcpipInterface->InterfaceConfig.iEncapTimeout);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case TCPIP_INST_ATTR_ACTIVE_TCP_CONN:
		iVal = ENCAP_TO_HS(tcpipGetActiveTCPConnections(pTcpipInterface->InterfaceConfig.lIpAddr));
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
	case TCPIP_INST_ATTR_NON_CIP_ENCAP:
		lVal = ENCAP_TO_HL(pTcpipInterface->lNonCIPEncapCountPrev);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
#endif
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** tcpipSetInstanceAttrAll( )
**
** SetAttributeAll service for TCP/IP instance is received.
** Only interface config data is transferred.
**---------------------------------------------------------------------------
*/
static void tcpipSetInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = &gsTcpIpInstAttr[iInstance-1];
	UINT16 iLen;
	UINT32 lVal;
	TCPIP_CONFIG_DATA TempConfigData;
	BOOL bSendData = TRUE;

	if ((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_HARDWARE_CONFIG) &&
		((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_SETTABLE_CAPABILITY_MASK) == 0))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize < 26)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iInstance = iInstance;
	TempConfigData.lConfigControl = UINT32_GET( pData );
	pData += sizeof(UINT32);

	/* Check at least if its a valid value for the platform */
	if ((TempConfigData.lConfigControl & TCPIP_CONTROL_CONFIG_BITMASK) > TCPIP_CONTROL_DHCP)
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/*************************************************************
	* Address data is expected to be IP's network order (big endian)
	* Because CIP expects data to be in host order (little endian), the bytes
	* must be swapped
	**************************************************************/
	lVal = UINT32_GET(pData);
	TempConfigData.lIpAddr = ntohl(lVal);
	if (!tcpipIsValidIPAddress(TempConfigData.lIpAddr))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	pData += sizeof(UINT32);

	lVal = UINT32_GET(pData);
	TempConfigData.lSubnetMask = ntohl(lVal);
	/* Subnet mask doesn't need to be validated */
	pData += sizeof(UINT32);

	/* IP addresses with the host ID set to all ones or all zeros are invalid */
	if ((!tcpipIsValidIPAddress(TempConfigData.lIpAddr)) ||
		((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == 0) ||
		((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask) ||
		((TempConfigData.lIpAddr != 0) && 
		 (TempConfigData.lIpAddr != pTcpipInterface->InterfaceConfig.lIpAddr) &&
		 (tcpipFindClaimedHostAddress(TempConfigData.lIpAddr) != NULL)))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	lVal = UINT32_GET(pData);
	TempConfigData.lGatewayAddr = ntohl(lVal);
	if ((!tcpipIsValidIPAddress(TempConfigData.lGatewayAddr)) ||
		((TempConfigData.lGatewayAddr) != 0 &&
		 (((TempConfigData.lIpAddr & TempConfigData.lSubnetMask) != (TempConfigData.lGatewayAddr & TempConfigData.lSubnetMask)) ||
		 ((TempConfigData.lGatewayAddr & ~TempConfigData.lSubnetMask) == 0) ||
		 ((TempConfigData.lGatewayAddr & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	pData += sizeof(UINT32);

	lVal = UINT32_GET(pData);
	TempConfigData.lNameServer = ntohl(lVal);
	if ((!tcpipIsValidIPAddress(TempConfigData.lNameServer)) ||
		((TempConfigData.lNameServer != 0) && (((TempConfigData.lNameServer & ~TempConfigData.lSubnetMask) == 0) ||
		((TempConfigData.lNameServer & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	pData += sizeof(UINT32);

	lVal = UINT32_GET(pData);
	TempConfigData.lNameServer2 = ntohl(lVal);
	if ((!tcpipIsValidIPAddress(TempConfigData.lNameServer2)) ||
		((TempConfigData.lNameServer2 != 0) &&
		  (((TempConfigData.lNameServer2 & ~TempConfigData.lSubnetMask) == 0) ||
		   ((TempConfigData.lNameServer2 & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	pData += sizeof(UINT32);

	iLen = UINT16_GET(pData);
	if ( iLen > TCPIP_MAX_DOMAIN_LENGTH )
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	pData += sizeof(UINT16);

	if (((pData-pRequest->cfg.objectCfg.pData) + ((iLen+1) & 0xfffe)) > pRequest->cfg.objectCfg.iDataSize)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (((pData-pRequest->cfg.objectCfg.pData) + ((iLen+1) & 0xfffe)) < pRequest->cfg.objectCfg.iDataSize)
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	memcpy(TempConfigData.szDomainName, pData, iLen);
	TempConfigData.szDomainName[iLen] = 0;

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	/* Make sure we can set the TCP/IP parameters */
	if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_CONFIG_CONTROL) + (1 << TCPIP_INST_ATTR_INTERFACE_CONFIG), 
										&TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr, 
										&pRequest->bGeneralError))
	{
		return;
	}

	/* Send the response before setting the IP parameters */
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	requestService( pRequest, &bSendData );

	/* Set the TCP/IP information */
	platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_INTERFACE_CONFIG) + (1 << TCPIP_INST_ATTR_CONFIG_CONTROL), 
							&TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
	pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_VALID_CONFIGURATION;

	if (((TempConfigData.lIpAddr != 0) && (TempConfigData.lIpAddr != pTcpipInterface->InterfaceConfig.lIpAddr)) ||
			(TempConfigData.lConfigControl != pTcpipInterface->InterfaceConfig.lConfigControl))
	{
		if (((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_IP_RESET_NEEDED) != TCPIP_CAPABILITY_IP_RESET_NEEDED))
		{
			if (TempConfigData.lIpAddr == 0)
				platformGetTcpIpCfgData(&TempConfigData);

			tcpipUpdateIPAddress(pTcpipInterface, TempConfigData.lIpAddr);
		}
		else
		{
			pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_INTERFACE_CONFIG_PENDING;
		}
	}
}


/*--------------------------------------------------------------------------------
** tcpipSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the TCP/IP object instance
**--------------------------------------------------------------------------------
*/
static void tcpipSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT16 iLen, iTagSize;
	UINT32 lVal;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	CONNECTION* pConnection;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = &gsTcpIpInstAttr[iInstance-1];
	TCPIP_CONFIG_DATA TempConfigData = {0};
	BOOL bSendData = TRUE;

	switch( iAttribute )
	{
	case TCPIP_INST_ATTR_CONFIG_CONTROL:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT32))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT32))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		if (!(pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_SETTABLE_CONFIG))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}

		TempConfigData.lConfigControl = UINT32_GET( pData );

		/* Check at least if its a valid value for the platform */
		if ((TempConfigData.lConfigControl & TCPIP_CONTROL_CONFIG_BITMASK) > TCPIP_CONTROL_DHCP)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Make sure we can set the TCP/IP parameters */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_CONFIG_CONTROL), &TempConfigData,
								pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
		{
			break;
		}

		/* Send the response before setting the parameter because the effect
		 * could kill the current socket */
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		requestService( pRequest, &bSendData );

		/* Set the TCP/IP information */
		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_CONFIG_CONTROL), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);

		if (TempConfigData.lConfigControl != pTcpipInterface->InterfaceConfig.lConfigControl)
		{
			if ((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_IP_RESET_NEEDED) != TCPIP_CAPABILITY_IP_RESET_NEEDED)
			{
				platformGetTcpIpCfgData(&TempConfigData);
				tcpipUpdateIPAddress(pTcpipInterface, TempConfigData.lIpAddr);
			}
			else
			{
				pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_INTERFACE_CONFIG_PENDING;
			}
		}
		break;

	case TCPIP_INST_ATTR_INTERFACE_CONFIG:
		if (pRequest->cfg.objectCfg.iDataSize < (sizeof(UINT32)*5+sizeof(UINT16)))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		if ((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_HARDWARE_CONFIG) &&
			((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_SETTABLE_CAPABILITY_MASK) == 0))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}

		/*************************************************************
		* Address data is expected to be IP's network order (big endian)
		* Because CIP expects data to be in host order (little endian), the bytes
		* must be swapped
		**************************************************************/
		lVal = UINT32_GET(pData);
		TempConfigData.lIpAddr = ntohl(lVal);
		if ((!tcpipIsValidIPAddress(TempConfigData.lIpAddr)) ||
			((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == 0) ||
			((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask) ||
			((TempConfigData.lIpAddr != 0) &&
			 (TempConfigData.lIpAddr != pTcpipInterface->InterfaceConfig.lIpAddr) &&
			 (tcpipFindClaimedHostAddress(TempConfigData.lIpAddr) != NULL)))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		pData += sizeof(UINT32);

		lVal = UINT32_GET(pData);
		TempConfigData.lSubnetMask = ntohl(lVal);
		/* Subnet mask doesn't need to be validated */
		pData += sizeof(UINT32);

		/* IP addresses with the host ID set to all ones or all zeros are invalid */
		if (((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == 0) ||
			((TempConfigData.lIpAddr & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask) ||
			(!tcpipIsValidSubnetMask(lVal)))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		lVal = UINT32_GET(pData);
		TempConfigData.lGatewayAddr = ntohl(lVal);
		if ((!tcpipIsValidIPAddress(TempConfigData.lGatewayAddr)) ||
			((TempConfigData.lGatewayAddr) != 0 &&
			 (((TempConfigData.lIpAddr & TempConfigData.lSubnetMask) != (TempConfigData.lGatewayAddr & TempConfigData.lSubnetMask)) ||
			 ((TempConfigData.lGatewayAddr & ~TempConfigData.lSubnetMask) == 0) ||
			 ((TempConfigData.lGatewayAddr & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		pData += sizeof(UINT32);

		lVal = UINT32_GET(pData);
		TempConfigData.lNameServer = ntohl(lVal);
		if ((!tcpipIsValidIPAddress(TempConfigData.lNameServer)) ||
			((TempConfigData.lNameServer != 0) && (((TempConfigData.lNameServer & ~TempConfigData.lSubnetMask) == 0) ||
			((TempConfigData.lNameServer & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		pData += sizeof(UINT32);

		lVal = UINT32_GET(pData);
		TempConfigData.lNameServer2 = ntohl(lVal);
		if ((!tcpipIsValidIPAddress(TempConfigData.lNameServer2)) ||
			((TempConfigData.lNameServer2 != 0) && (((TempConfigData.lNameServer2 & ~TempConfigData.lSubnetMask) == 0) ||
			((TempConfigData.lNameServer2 & ~TempConfigData.lSubnetMask) == ~TempConfigData.lSubnetMask))))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		pData += sizeof(UINT32);

		iLen = UINT16_GET(pData);
		if ( iLen > TCPIP_MAX_DOMAIN_LENGTH )
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		pData += sizeof(UINT16);

		if (((pData-pRequest->cfg.objectCfg.pData) + ((iLen+1) & 0xfffe)) > pRequest->cfg.objectCfg.iDataSize)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		if (((pData-pRequest->cfg.objectCfg.pData) + ((iLen+1) & 0xfffe)) < pRequest->cfg.objectCfg.iDataSize)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		strncpy( (char*)TempConfigData.szDomainName, (const char*)pData, iLen );
		TempConfigData.szDomainName[iLen] = 0;

		/* Make sure we can set the TCP/IP parameters */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_INTERFACE_CONFIG), &TempConfigData,
								pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
		{
			break;
		}

		/* Send the response before setting the IP parameters */
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		requestService( pRequest, &bSendData );

		/* Set the TCP/IP information */
		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_INTERFACE_CONFIG), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_VALID_CONFIGURATION;

		if ((TempConfigData.lIpAddr != 0) &&
			(TempConfigData.lIpAddr != pTcpipInterface->InterfaceConfig.lIpAddr))
		{
			if ((pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_IP_RESET_NEEDED) != TCPIP_CAPABILITY_IP_RESET_NEEDED)
				tcpipUpdateIPAddress(pTcpipInterface, TempConfigData.lIpAddr);
			else
				pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_INTERFACE_CONFIG_PENDING;
		}
		break;

	case TCPIP_INST_ATTR_HOST_NAME:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		if (!(pTcpipInterface->InterfaceConfig.lConfigCapability & TCPIP_CAPABILITY_SETTABLE_CONFIG))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}

		iLen = UINT16_GET( pData );
		pData += sizeof(UINT16);
		if ((iLen > TCPIP_MAX_HOST_LENGTH) ||
			(((iLen+3) & 0xfffe) < pRequest->cfg.objectCfg.iDataSize))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		if (((iLen+3) & 0xfffe) > pRequest->cfg.objectCfg.iDataSize)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		if (iLen == 0)
		{
			/* Empty the string */
			TempConfigData.szHostName[0] = 0;
		}
		else
		{
			memcpy(TempConfigData.szHostName, pData, iLen);
			TempConfigData.szHostName[iLen] = 0;
		}

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_HOST_NAME), &TempConfigData,
								pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		/* value has been verified */
		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_HOST_NAME), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		strcpy((char*)pTcpipInterface->InterfaceConfig.szHostName, (const char*)TempConfigData.szHostName);
		break;
	case TCPIP_INST_ATTR_TTL_VALUE:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		TempConfigData.bTTLValue = *pRequest->cfg.objectCfg.pData;
		if (TempConfigData.bTTLValue == 0)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Cannot set value if there is an existing I/O multicast connection */
		for( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
		{
			if ((connectionIsIOConnection(pConnection)) &&
				(pConnection->lConnectionState != ConnectionTimedOut) &&
				((pConnection->cfg.iProducingConnectionType == Multicast) ||
				 (pConnection->cfg.iConsumingConnectionType == Multicast)))
			{
				pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
				break;
			}
		}

		/* Found class1 connection */
		if (pConnection < gpnConnections)
			break;

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_TTL_VALUE), &TempConfigData,
										pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_TTL_VALUE), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		pTcpipInterface->InterfaceConfig.bTTLValue = TempConfigData.bTTLValue;
		break;
	case TCPIP_INST_ATTR_MCAST_CONFIG:
		if (pRequest->cfg.objectCfg.iDataSize < TCPIP_INTERFACE_MCAST_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > TCPIP_INTERFACE_MCAST_SIZE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		//EATON_EIP_Changes:MAV - Moved from below so we reflect the fact that the value is read only.
		/* Test Set the TCP/IP information */ 
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_MCAST_CONFIG), &TempConfigData,
										 pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
		{
			break;
			//#k: Added this 'if' statement on 23Octo13. For more info See "RedBall_EDC_SRS_V1.0.13.doc" 
			//&/or mail "RE: BCM DIP switch mapping".
		}
		TempConfigData.MulticastData.bAllocControl = *pData++;
		if (TempConfigData.MulticastData.bAllocControl >= 2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfigData.MulticastData.bReserved = *pData++;
		if (TempConfigData.MulticastData.bReserved != 0)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Cannot set value if there is an existing I/O multicast connection */
		for( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
		{
			if ((connectionIsIOConnection(pConnection)) &&
				(pConnection->lConnectionState != ConnectionTimedOut) &&
				((pConnection->cfg.iProducingConnectionType == Multicast) ||
				 (pConnection->cfg.iConsumingConnectionType == Multicast)))
			{
				pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
				break;
			}
		}

		/* Found class1 connection */
		if (pConnection < gpnConnections)
			break;

		TempConfigData.MulticastData.iNumMcast = UINT16_GET(pData);
		pData += sizeof(UINT16);
		lVal = UINT32_GET(pData);
		TempConfigData.MulticastData.lMcastStartAddr = ntohl(lVal);

		if (TempConfigData.MulticastData.bAllocControl == 0)
		{
			/* Verify parameters are zeroed out if alloc control wants
			   to use default algorithm */
			if ((TempConfigData.MulticastData.iNumMcast != 0) ||
				(TempConfigData.MulticastData.lMcastStartAddr != 0))
			{
				pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
				break;
			}

			TempConfigData.MulticastData.iNumMcast = CIP_MAX_NUM_MULTICAST_ADDRESS;
			TempConfigData.MulticastData.lMcastStartAddr = CIP_MCAST_BASE;
		}
		else if (TempConfigData.MulticastData.bAllocControl == 1) 
		{
			if ( (TempConfigData.MulticastData.iNumMcast == 0) || (TempConfigData.MulticastData.lMcastStartAddr == 0)) 
			{
				pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
				break;
			}
		}


		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		//EATON_EIP_Changes:MAV - Moved the test up so we can pass conformance testing.
		//Ori-if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_MCAST_CONFIG), &TempConfigData,
		//Ori-								 pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
		//Ori-	break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_MCAST_CONFIG), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		memcpy(&pTcpipInterface->InterfaceConfig.MulticastData, &TempConfigData.MulticastData, sizeof(pTcpipInterface->InterfaceConfig.MulticastData));
		break;
#ifdef EIP_ACD
	case TCPIP_INST_ATTR_ACD_MODE:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		TempConfigData.bACDMode = *pData++;
		if (TempConfigData.bACDMode >= 2)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ACD_MODE), &TempConfigData,
							pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ACD_MODE), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		break;
	case TCPIP_INST_ATTR_ACD_LAST_CONFLICT:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8)+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH)
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8)+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH)
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		/* Data must be all 0s */
		for (iLen = 0; iLen < 1+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH; iLen++, pData++)
		{
			if (*pData != 0)
			{
				pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
				break;
			}
		}

		/* Check if error was generated (nested break) */
		if (iLen < 1+MAC_ADDR_LENGTH+TCPIP_MAX_ARP_PDU_LENGTH)
			break;

		/* reset data pointer */
		pData = pRequest->cfg.objectCfg.pData;

		/* set the attribute structure */
		TempConfigData.bACDLCD_Phase = *pData++;
		memcpy(TempConfigData.ACDLCD_MacID, pData, MAC_ADDR_LENGTH);
		pData += MAC_ADDR_LENGTH;
		memcpy(TempConfigData.ACDLCD_ARPPdu, pData, TCPIP_MAX_ARP_PDU_LENGTH);

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ACD_LAST_CONFLICT), &TempConfigData,
							pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ACD_LAST_CONFLICT), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		break;
#endif
#if defined (EIP_QUICK_CONNECT) && !defined (EIP_CIPSAFETY)
	case TCPIP_INST_ATTR_QUICK_CONNECT:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		TempConfigData.bQuickConnect = *pData++;
		if (TempConfigData.bQuickConnect > TCPIP_QUICK_CONNECT_ENABLED)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_QUICK_CONNECT), &TempConfigData,
									pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_QUICK_CONNECT), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		pTcpipInterface->InterfaceConfig.bQuickConnect = TempConfigData.bQuickConnect;
		break;
#endif
	case TCPIP_INST_ATTR_ENCAP_TIMEOUT:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}
		TempConfigData.iEncapTimeout = UINT16_GET(pData);
		if (TempConfigData.iEncapTimeout > 3600)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		/* Ensure the IP address is set in the temp config to uniquely ID the interface */
		TempConfigData.lIpAddr = pTcpipInterface->InterfaceConfig.lIpAddr;

		/* Set the TCP/IP information */
		if (!platformTestSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ENCAP_TIMEOUT), &TempConfigData,
									pTcpipInterface->InterfaceConfig.lIpAddr, &pRequest->bGeneralError))
			break;

		platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_ENCAP_TIMEOUT), &TempConfigData, pTcpipInterface->InterfaceConfig.lIpAddr);
		pTcpipInterface->InterfaceConfig.iEncapTimeout = TempConfigData.iEncapTimeout;
		break;
	case TCPIP_INST_ATTR_INTERFACE_STATUS:
	case TCPIP_INST_ATTR_CONFIG_CAPABILITY:
	case TCPIP_INST_ATTR_LINK_PATH:
	case TCPIP_INST_ATTR_ACTIVE_TCP_CONN:
#ifdef ENABLE_NON_CIP_MESSAGE_COUNTING
	case TCPIP_INST_ATTR_NON_CIP_ENCAP:
#endif	
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*--------------------------------------------------------------------------------
** tcpipReset( )
**
** SetAttributeSingle request has been received for the TCP/IP object instance
**--------------------------------------------------------------------------------
*/
void tcpipReset(UINT8 bReset)
{
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;
	UINT32 i;

	for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
		 i++, pTcpipConfig++)
	{
		if (bReset > 0)
		{

			/* get the latest TCP/IP information */
			platformGetTcpIpCfgData(&pTcpipConfig->InterfaceConfig);

			/* Reset parameters */
			pTcpipConfig->InterfaceConfig.bTTLValue = TTL_SUBNET_LEVEL;
			pTcpipConfig->InterfaceConfig.MulticastData.bAllocControl = 0;
			pTcpipConfig->InterfaceConfig.MulticastData.iNumMcast = CIP_MAX_NUM_MULTICAST_ADDRESS;
			pTcpipConfig->InterfaceConfig.MulticastData.lMcastStartAddr = CIP_MCAST_BASE;

			pTcpipConfig->InterfaceConfig.iEncapTimeout = TCPIP_ENCAP_TIMEOUT_DEFAULT;

			/* Save the data */
			platformSetTcpIpCfgData((1 << TCPIP_INST_ATTR_TTL_VALUE) | (1 << TCPIP_INST_ATTR_MCAST_CONFIG) | (1 << TCPIP_INST_ATTR_ENCAP_TIMEOUT),
										&pTcpipConfig->InterfaceConfig, pTcpipConfig->InterfaceConfig.lIpAddr);
#ifdef EIP_QOS
			/* Reset the QoS values back to default */
			pTcpipConfig->QoSConfig.b8021QTagEnable = FALSE;
			pTcpipConfig->QoSConfig.bDSCPPTPEvent = QOS_DEFAULT_VALUE_DSCP_PTP_EVENT;
			pTcpipConfig->QoSConfig.bDSCPPTPGeneral = QOS_DEFAULT_VALUE_DSCP_PTP_GENERAL;
			pTcpipConfig->QoSConfig.bDSCPUrgent = QOS_DEFAULT_VALUE_DSCP_URGENT;
			pTcpipConfig->QoSConfig.bDSCPScheduled = QOS_DEFAULT_VALUE_DSCP_SCHEDULED;
			pTcpipConfig->QoSConfig.bDSCPHigh = QOS_DEFAULT_VALUE_DSCP_HIGH;
			pTcpipConfig->QoSConfig.bDSCPLow = QOS_DEFAULT_VALUE_DSCP_LOW;
			pTcpipConfig->QoSConfig.bDSCPExplicit = QOS_DEFAULT_VALUE_DSCP_EXPLICIT;
			platformSetQoSAttributes(QOS_MASK_ALL_ATTRIBUTES, pTcpipConfig);
#endif
		}
		else
		{
#ifdef EIP_QOS
			/* Refresh the QoS attribute */
			platformGetQoSAttributes( pTcpipConfig );
#endif
		}

		pTcpipConfig->lMulticastAddrCount = 0;
	}
}

/*---------------------------------------------------------------------------
** tcpipParseInstanceRequest( )
**
** Respond to the TCP/IP instance request
**---------------------------------------------------------------------------
*/
static void tcpipParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	if ( iInstance >= 1 && iInstance <= gnClaimedHostIPAddrCount )
	{
		switch( pRequest->cfg.objectCfg.bService )
		{
		case SVC_GET_ATTR_ALL:
			tcpipSendInstanceAttrAll( pRequest, iInstance );
			break;
		case SVC_GET_ATTR_SINGLE:
			tcpipSendInstanceAttrSingle( pRequest, iInstance );
			break;
		case SVC_SET_ATTR_ALL:
			tcpipSetInstanceAttrAll( pRequest, iInstance );
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			break;
		case SVC_SET_ATTR_SINGLE:
			tcpipSetInstanceAttrSingle( pRequest, iInstance );
			break;
		default:
			pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			break;
		}
	}
	else
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
	}
}

/*---------------------------------------------------------------------------
** tcpipParseClassRequest( )
**
** Respond to the TCP/IP class request
**---------------------------------------------------------------------------
*/
static void tcpipParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		tcpipSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		tcpipSendClassAttrSingle( pRequest );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** tcpipParseClassInstanceRequest( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void tcpipParseClassInstanceRequest( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize );

	if ( iInstance == 0 )
		tcpipParseClassRequest( pRequest );
	else
		tcpipParseInstanceRequest( pRequest, iInstance );
}
