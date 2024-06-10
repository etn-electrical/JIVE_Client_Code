/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCNMGR.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** This module contains the implementation of the Connection Manager class
**
*****************************************************************************
*/

#include "eipinc.h"


CONNMGR_STATS	connmgrStats;			/* Connection manager statistics (instance attributes) */
BOOL gbAppVerifyConnection;				/* Determines if client applications must verify all connections */
BOOL gbAutomaticRunIdleEnforcement;		/* Enforces including a O->T Run/Idle header and not a T->O Run/Idle header */
#ifndef EIP_NO_CIP_ROUTING
BOOL gbBackplaneSupport;				/* Determines if the client application wants to handle "backplane" requests */
UINT16 giBackplaneSlot;					/* Slot of the EtherNet/IP "device" in the backplane */
#endif

static UINT16 connmgrAttributes[CONNMGR_ATTR_OPT_COUNT] = {CONNMGR_ATTR_OPEN_REQUESTS, CONNMGR_ATTR_OPEN_FORMAT_REJECTS,
													 CONNMGR_ATTR_OPEN_RESOURCE_REJECTS, CONNMGR_ATTR_OPEN_OTHER_REJECTS,
													 CONNMGR_ATTR_CLOSE_REQUEST, CONNMGR_ATTR_CLOSE_FORMAT_REJECTS,
													 CONNMGR_ATTR_CLOSE_OTHER_REJECTS, CONNMGR_ATTR_CONNECTION_TIMEOUTS
#ifdef EIP_BIG12
													 , CONNMGR_ATTR_CPU_UTILIZATION
#endif
};

/*---------------------------------------------------------------------------
** connmgrInit( )
**
** Initialized Connection Manager (reset stats)
**---------------------------------------------------------------------------
*/
void connmgrInit(void)
{
	memset(&connmgrStats, 0, sizeof(CONNMGR_STATS));
	gbAppVerifyConnection = FALSE;
#ifndef EIP_NO_CIP_ROUTING
	gbBackplaneSupport = FALSE;
#endif
	gbAutomaticRunIdleEnforcement = FALSE;
}

/*---------------------------------------------------------------------------
** connmgrNULLConnection( )
**
** Incoming "NULL" connection request received parse "NULL Forward Open"
**---------------------------------------------------------------------------
*/
static BOOL connmgrNULLConnection(REQUEST* pRequest, CONNECTION* pConnection, FWD_OPEN* pFwdOpen)
{
	CONNECTION *pLoopConnection, *pOwnerConnection = NULL;
	UINT16 iModuleConfigSize;
	UINT8* pModuleConfigData;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* Check if we already have a connection with the same serial numbers,
			then it's a duplicate Forward Open */
		if ( pLoopConnection != pConnection && pLoopConnection->lConnectionState == ConnectionEstablished &&
			 !pLoopConnection->cfg.bOriginator &&
			 pLoopConnection->iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
			 pLoopConnection->iVendorID == pConnection->iVendorID &&
			 pLoopConnection->lDeviceSerialNbr == pConnection->lDeviceSerialNbr)
		{
			pOwnerConnection = pLoopConnection;
			break;
		}
	}

	if (pOwnerConnection == NULL)
	{
		/* Couldn't find the matching connection, connection will be rejected */
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if ((pConnection->cfg.bTransportClass != pOwnerConnection->cfg.bTransportClass) ||
		(pConnection->cfg.bTransportType != pOwnerConnection->cfg.bTransportType) ||
		(pConnection->cfg.iTargetConfigConnInstance != pOwnerConnection->cfg.iTargetConfigConnInstance) ||
		(pConnection->cfg.iTargetProducingConnPoint != pOwnerConnection->cfg.iTargetProducingConnPoint) ||
		(pConnection->cfg.iTargetConsumingConnPoint != pOwnerConnection->cfg.iTargetConsumingConnPoint) ||
		(pConnection->cfg.lProducingDataRate != pOwnerConnection->cfg.lProducingDataRate) ||
		(pConnection->cfg.lConsumingDataRate != pOwnerConnection->cfg.lConsumingDataRate) ||
		(pConnection->cfg.bProductionOTInhibitInterval != pOwnerConnection->cfg.bProductionOTInhibitInterval) ||
		(pConnection->cfg.bProductionTOInhibitInterval != pOwnerConnection->cfg.bProductionTOInhibitInterval) ||
		(pConnection->cfg.iProducingPriority != pOwnerConnection->cfg.iProducingPriority) ||
		(pConnection->cfg.iConsumingPriority != pOwnerConnection->cfg.iConsumingPriority) ||
		(pConnection->cfg.bTimeoutMultiplier != pOwnerConnection->cfg.bTimeoutMultiplier))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, pFwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if (gbAppVerifyConnection)
	{
		pConnection->cfg.nInstance = connectionGetUnusedInstance();
		pConnection->nRequestId = pRequest->nIndex;
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		notifyEvent( NM_CONNECTION_VERIFICATION, pConnection->cfg.nInstance, 0 );
		return FALSE;
	}

	/* Set module configuration data (if applicable) */
	pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );
	if ( assemblyNotifyInstanceChange(pConnection->cfg.iTargetConfigConnInstance, pModuleConfigData, iModuleConfigSize, FALSE) )
	{
		assemblyNotifyMembersChange(pConnection->cfg.iTargetConfigConnInstance, INVALID_MEMBER, pModuleConfigData, iModuleConfigSize );
		assemblySetInstanceData(pConnection->cfg.iTargetConfigConnInstance, pModuleConfigData, iModuleConfigSize );    /* Store data in assembly object */
	}

	connmgrPrepareConnection(pConnection, pRequest, 0, NULL);
	return TRUE;
}

/*---------------------------------------------------------------------------
** connmgrVerifyRedundantOwnerRequest( )
**
** Check redundant owner request against existing connection to see if they
** are compatible
**---------------------------------------------------------------------------
*/
static BOOL connmgrVerifyRedundantOwnerRequest(CONNECTION *pNewConn, CONNECTION *pExistConn)
{
	UINT8 *pNewConnConfigData, *pExistConnConfigData;
	UINT16 iNewConnConfigSize, iExistConnConfigSize;

	if (
		/* Check RPI, Connection parameters, and Transport Class/Trigger */
		(pNewConn->cfg.lProducingDataRate != pExistConn->cfg.lProducingDataRate) ||
		(pNewConn->cfg.lConsumingDataRate != pExistConn->cfg.lConsumingDataRate) ||

		(pNewConn->cfg.iProducingConnectionType != pExistConn->cfg.iProducingConnectionType) ||
		(pNewConn->cfg.iProducingPriority != pExistConn->cfg.iProducingPriority) ||
		(pNewConn->cfg.bClass1VariableProducer != pExistConn->cfg.bClass1VariableProducer) ||

		(pNewConn->cfg.iConsumingConnectionType != pExistConn->cfg.iConsumingConnectionType) ||
		(pNewConn->cfg.iConsumingPriority != pExistConn->cfg.iConsumingPriority) ||
		(pNewConn->cfg.bClass1VariableConsumer != pExistConn->cfg.bClass1VariableConsumer) ||

		(pNewConn->cfg.bTransportClass != pExistConn->cfg.bTransportClass) ||
		(pNewConn->cfg.bTransportType != pExistConn->cfg.bTransportType) ||

		/* Check Inhibit production */
		(pNewConn->cfg.bProductionOTInhibitInterval != pExistConn->cfg.bProductionOTInhibitInterval) ||
		(pNewConn->cfg.bProductionTOInhibitInterval != pExistConn->cfg.bProductionTOInhibitInterval) ||

		/* Check electronic key */
		(memcmp(&pNewConn->cfg.deviceId, &pExistConn->cfg.deviceId, sizeof(EtIPDeviceID)) != 0) ||

		/* Check connection path */
		(pNewConn->cfg.iTargetProducingConnPoint != pExistConn->cfg.iTargetProducingConnPoint) ||
		(pNewConn->cfg.iTargetConsumingConnPoint != pExistConn->cfg.iTargetConsumingConnPoint) ||
		(pNewConn->cfg.iTargetConfigConnInstance != pExistConn->cfg.iTargetConfigConnInstance)

		)
	{
		return FALSE;
	}

	/* Check configuration data */
	pNewConnConfigData = (UINT8*)pduGetTagByType( &pNewConn->cfg.PDU, TAG_TYPE_DATA, 0, &iNewConnConfigSize);
	pExistConnConfigData = (UINT8*)pduGetTagByType( &pExistConn->cfg.PDU, TAG_TYPE_DATA, 0, &iExistConnConfigSize);
	if ((iNewConnConfigSize != iExistConnConfigSize) ||
		(memcmp(pNewConnConfigData, pExistConnConfigData, iNewConnConfigSize) != 0))
	{
		return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** connmgrInitBufFromFwdOpenReply()
**
** Populate the buffer from the provided Forward Open Reply structure
**---------------------------------------------------------------------------
*/
static void connmgrInitBufFromFwdOpenReply( UINT8* pBuffer, FWD_OPEN_REPLY* fwdOpenTypeReply )
{
	UINT32_SET(pBuffer, fwdOpenTypeReply->lLeOTConnId);
	pBuffer += sizeof(UINT32);
	UINT32_SET(pBuffer, fwdOpenTypeReply->lLeTOConnId);
	pBuffer += sizeof(UINT32);

	UINT16_SET(pBuffer, fwdOpenTypeReply->iLeOrigConnectionSn);
	pBuffer += sizeof(UINT16);
	UINT16_SET(pBuffer, fwdOpenTypeReply->iLeOrigVendorId);
	pBuffer += sizeof(UINT16);
	UINT32_SET(pBuffer, fwdOpenTypeReply->lLeOrigDeviceSn);
	pBuffer += sizeof(UINT32);

	UINT32_SET(pBuffer, fwdOpenTypeReply->lLeOTApi);
	pBuffer += sizeof(UINT32);
	UINT32_SET(pBuffer, fwdOpenTypeReply->lLeTOApi);
	pBuffer += sizeof(UINT32);

	*pBuffer = fwdOpenTypeReply->bAppReplySize;
	pBuffer++;
	*pBuffer = fwdOpenTypeReply->bReserved;
	pBuffer++;
	memcpy(pBuffer, fwdOpenTypeReply->pAppReply, fwdOpenTypeReply->bAppReplySize*2);
}

/*---------------------------------------------------------------------------
** connmgrPrepareFwdOpenReply( )
**
** Send successful Forward Open Reply
**---------------------------------------------------------------------------
*/
static void connmgrPrepareFwdOpenReply(CONNECTION* pConnection, REQUEST* pRequest, UINT8 bAppSize, UINT8* pAppData)
{
	FWD_OPEN_REPLY FwdReply;
	UINT32 lProducingRate = pConnection->cfg.lProducingDataRate*1000; /* On the wire it's in microseconds */
	UINT32 lConsumingRate = pConnection->cfg.lConsumingDataRate*1000; /* On the wire it's in microseconds */

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	pRequest->cfg.objectCfg.iDataSize = (UINT16)(FWD_OPEN_REPLY_SIZE + (bAppSize*2));
	pRequest->cfg.objectCfg.pData = EtIP_malloc( NULL, pRequest->cfg.objectCfg.iDataSize );

	FwdReply.lLeOTConnId = pConnection->lConsumingCID;
	FwdReply.lLeTOConnId = pConnection->lProducingCID;

	FwdReply.iLeOrigConnectionSn = pConnection->iConnectionSerialNbr;
	FwdReply.iLeOrigVendorId = pConnection->iVendorID;
	FwdReply.lLeOrigDeviceSn = pConnection->lDeviceSerialNbr;
	FwdReply.lLeOTApi = lConsumingRate;
	FwdReply.lLeTOApi = lProducingRate;
	FwdReply.bAppReplySize = bAppSize;
	FwdReply.bReserved = 0;
	FwdReply.pAppReply = pAppData;

	connmgrInitBufFromFwdOpenReply( pRequest->cfg.objectCfg.pData, &FwdReply );

	if (connectionIsIOConnection(pConnection))
	{
		if (pConnection->cfg.iConsumingConnectionType == PointToPoint)
		{
			memcpy(&pRequest->sOTTagAddr, &pConnection->sReceiveAddr, sizeof(struct sockaddr_in));
			/* It is "recommended" that the IP address be set to 0 for PointToPoint connections
			   Certain scanners make it "required" */
			platformGetInternetAddress(pRequest->sOTTagAddr.sin_addr) = 0;
		}

		if (pConnection->cfg.iProducingConnectionType == Multicast)
			memcpy(&pRequest->sTOTagAddr, &pConnection->sTransmitAddr, sizeof(struct sockaddr_in));
		else
			memset(&pRequest->sTOTagAddr, 0, sizeof(struct sockaddr_in));
	}
}

/*---------------------------------------------------------------------------
** connmgrIncomingConnection( )
**
** Incoming connection request received - store addresses and parse
** Forward Open
**---------------------------------------------------------------------------
*/
BOOL connmgrIncomingConnection( REQUEST* pRequest, BOOL bLargeFwdOpen )
{
	CONNECTION  *pConnection, *pLoopConnection, *pOTConnection = NULL, *pTOConnection = NULL;
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	CONNECTION* pOrigTOConnection;
#endif
	CONNECTION* pMulticastProducer;
	FWD_OPEN FwdOpen;
	INT32 nPathSize;
	UINT8* pPacketPtr = pRequest->cfg.objectCfg.pData;
	INT32 pdu_len;
	PDU_HDR pdu_hdr;
	UINT16 iTagSize, iClassTagSize, iInstanceTagSize, iFwdOpenSize;
	UINT32 iClass, iInstance;
#ifndef EIP_NO_CIP_ROUTING
	UINT32 lPort, iSlot = INVALID_SLOT_INDEX;
	ETAG_DATATYPE pAddr;
#endif
#ifdef ET_IP_MODBUS
	UINT32 lIPAddress;
#endif
	ASSEMBLY *pInputAssembly, *pOutputAssembly, *pConfigAssembly;
	UINT16 iInputDataSize, iOutputDataSize, iModuleConfigSize, iLoopModuleConnectionSize;
	UINT8 *pModuleConfigData, *pTmpData, *pLoopModuleConfigData;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	EtIPDeviceID* pDeviceID;
	UINT16 usPort;

	/* increment number of forward open attempts */
	connmgrStats.iOpenRequests++; 

	if ( pRequest->cfg.objectCfg.PDU.bNumTags != 2 )
	{
		connmgrStats.iOpenFormatRejects++;
#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumFailedConnections++;
#endif
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pRequest->lIPAddress, pRequest->lHostIPAddr,
				"connmgrIncomingConnection ERROR: segments other than Class and Instance were specified");
		return TRUE;
	}

	if ( pRequest->cfg.objectCfg.iDataSize < (FWD_OPEN_SIZE + 4) )
	{
		connmgrStats.iOpenFormatRejects++;
#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumFailedConnections++;
#endif
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	/* Parse Forward Open parameters */
	connmgrInitFwdOpenTypeFromBuf( pPacketPtr, &FwdOpen, bLargeFwdOpen );

	pConnection = connectionNew();

	if ( pConnection == NULL )
	{
		/* Can not exceed the maximum number of concurent connections limit */
		connmgrStats.iOpenResourceRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_CONNECTION_LIMIT_REACHED);
		notifyEvent( NM_CONNECTION_COUNT_LIMIT_REACHED, 0, 0);
		return TRUE;
	}

	pConnection->nSessionId = pRequest->nSessionId;
	pConnection->cfg.lIPAddress = pRequest->lIPAddress;
	pConnection->cfg.lHostIPAddr = pRequest->lHostIPAddr;

	pConnection->cfg.bOriginator = FALSE;
	pConnection->cfg.iWatchdogTimeoutAction = TimeoutAutoDelete;

	pConnection->cfg.bOpenPriorityTickTime = FwdOpen.bOpenPriorityTickTime;
	pConnection->cfg.bOpenTimeoutTicks = FwdOpen.bOpenTimeoutTicks;

	pConnection->cfg.bTimeoutMultiplier = FwdOpen.bTimeoutMult;
	pConnection->cfg.lProducingDataRate = FwdOpen.lLeTORpi/1000;	/* In the Forward Open it's in microseconds */
	pConnection->cfg.lConsumingDataRate = FwdOpen.lLeOTRpi/1000;	/* In the Forward Open it's in microseconds */

	/* If Consuming Connection Id was not specified in the Forward Open or duplicate, provide new one in the reply */
	if ( FwdOpen.lLeOTConnId == 0 || !utilIsIDUnique(FwdOpen.lLeOTConnId) )
		pConnection->lConsumingCID = utilGetUniqueID();
	else
		pConnection->lConsumingCID = FwdOpen.lLeOTConnId;

	/* If Producing Connection Id was not specified in the Forward Open, provide it in the reply */
	if ( FwdOpen.lLeTOConnId == 0 )
		pConnection->lProducingCID = utilGetUniqueID();
	else
		pConnection->lProducingCID = FwdOpen.lLeTOConnId;

	if ( bLargeFwdOpen )
	{
		iInputDataSize  = (UINT16)FwdOpen.lLargeLeOTConnParams;
		iOutputDataSize = (UINT16)FwdOpen.lLargeLeTOConnParams;

		pConnection->cfg.bRedundantOwner = ((FwdOpen.lLargeLeOTConnParams & CM_CP_LARGE_REDUNDANT_OWNER) == CM_CP_LARGE_REDUNDANT_OWNER) ? TRUE : FALSE;

		pConnection->cfg.iConsumingConnectionType = (UINT16)(((FwdOpen.lLargeLeOTConnParams>>16)&0xFFFF) & CM_CP_TYPE_MASK);
		pConnection->cfg.iProducingConnectionType = (UINT16)(((FwdOpen.lLargeLeTOConnParams>>16)&0xFFFF) & CM_CP_TYPE_MASK);

		pConnection->cfg.iConsumingPriority = (UINT16)(((FwdOpen.lLargeLeOTConnParams>>16)&0xFFFF) & CM_CP_PRIORITY_MASK);
		pConnection->cfg.iProducingPriority = (UINT16)(((FwdOpen.lLargeLeTOConnParams>>16)&0xFFFF) & CM_CP_PRIORITY_MASK);

		pConnection->cfg.bClass1VariableConsumer = ((FwdOpen.lLargeLeOTConnParams & CM_CP_LARGE_VARIABLE_SIZE) == CM_CP_LARGE_VARIABLE_SIZE) ? TRUE : FALSE;
		pConnection->cfg.bClass1VariableProducer = ((FwdOpen.lLargeLeTOConnParams & CM_CP_LARGE_VARIABLE_SIZE) == CM_CP_LARGE_VARIABLE_SIZE) ? TRUE : FALSE;

	}
	else
	{
		iInputDataSize  = (UINT16)( FwdOpen.iLeOTConnParams & DATA_SIZE_MASK);
		iOutputDataSize = (UINT16)( FwdOpen.iLeTOConnParams & DATA_SIZE_MASK);

		pConnection->cfg.bRedundantOwner = ((FwdOpen.iLeOTConnParams & CM_CP_REDUNDANT_OWNER) == CM_CP_REDUNDANT_OWNER) ? TRUE : FALSE;

		pConnection->cfg.iConsumingConnectionType = (UINT16)(FwdOpen.iLeOTConnParams & CM_CP_TYPE_MASK);
		pConnection->cfg.iProducingConnectionType = (UINT16)(FwdOpen.iLeTOConnParams & CM_CP_TYPE_MASK);

		pConnection->cfg.iConsumingPriority = (UINT16)( FwdOpen.iLeOTConnParams & CM_CP_PRIORITY_MASK);
		pConnection->cfg.iProducingPriority = (UINT16)( FwdOpen.iLeTOConnParams & CM_CP_PRIORITY_MASK);

		pConnection->cfg.bClass1VariableConsumer = ((FwdOpen.iLeOTConnParams & CM_CP_VARIABLE_SIZE) == CM_CP_VARIABLE_SIZE) ? TRUE : FALSE;
		pConnection->cfg.bClass1VariableProducer = ((FwdOpen.iLeTOConnParams & CM_CP_VARIABLE_SIZE) == CM_CP_VARIABLE_SIZE) ? TRUE : FALSE;
	}

	pConnection->cfg.bTransportClass = (UINT8)( FwdOpen.bClassTrigger & TRANSPORT_CLASS_MASK);
	pConnection->cfg.bTransportType = (UINT8)( FwdOpen.bClassTrigger & TRIGGER_CLASS_MASK);

	/* Remove 2 bytes for data sequence count that is used when data changes */
	if (pConnection->cfg.bTransportClass != Class0)
	{
		iInputDataSize -= DATA_SEQUENCE_COUNT_SIZE;
		iOutputDataSize -= DATA_SEQUENCE_COUNT_SIZE;
	}

	/* no redundant owner or fixed sized support for class 3 */
	if (pConnection->cfg.bTransportClass == Class3)
	{
		if (pConnection->cfg.bRedundantOwner)
		{
			connmgrStats.iOpenFormatRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_REDUNDANT_OWNER);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (pConnection->cfg.bClass1VariableConsumer == FALSE)
		{
			connmgrStats.iOpenFormatRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_FIXED_VARIABLE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (pConnection->cfg.bClass1VariableProducer == FALSE)
		{
			connmgrStats.iOpenFormatRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_FIXED_VARIABLE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	/* Connection Serial number, Vendor Id and Device Serial Number combination
	   is unique and will identify this connection */
	pConnection->iConnectionSerialNbr = FwdOpen.iLeOrigConnectionSn;
	pConnection->iVendorID = FwdOpen.iLeOrigVendorId;
	pConnection->lDeviceSerialNbr = FwdOpen.lLeOrigDeviceSn;

	if ( bLargeFwdOpen )
		iFwdOpenSize = LARGE_FWD_OPEN_SIZE;
	else
		iFwdOpenSize = FWD_OPEN_SIZE;

	pPacketPtr += (iFwdOpenSize - 2);

	memcpy( &pdu_hdr, pPacketPtr, PDU_HDR_SIZE );	/* The first 2 bytes is PDU header */
	pdu_len = pdu_hdr.bSize * 2;					/* PDU Length not including the PDU header */

	/* Check if not enough data */
	if ( pRequest->cfg.objectCfg.iDataSize < (iFwdOpenSize + pdu_len) )
	{
		connmgrStats.iOpenFormatRejects++;
#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumFailedConnections++;
#endif
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* Check if too much data */
	if ( pRequest->cfg.objectCfg.iDataSize > (iFwdOpenSize + pdu_len) )
	{
		connmgrStats.iOpenFormatRejects++;
#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumFailedConnections++;
#endif
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* Parse Message Router Protocol Data Unit part which is part of the connection path */
	nPathSize = pduParse( pPacketPtr, &pConnection->cfg.PDU, TRUE, iFwdOpenSize + pdu_len);
	if ( nPathSize == 0 )
	{
		connmgrStats.iOpenFormatRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

#ifndef EIP_NO_CIP_ROUTING
	/* check for an extended link address */
	lPort = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_PORT, 0, &iTagSize );
	if (iTagSize != 0)
	{
		/* Check the port to see if we support it */
#ifdef ET_IP_MODBUS
		if (((lPort != TCP_IP_BACKPLANE_PORT_NUMBER) && (lPort < TCP_IP_STARTING_PORT_NUMBER)) ||
			(lPort > (TCP_IP_STARTING_PORT_NUMBER+(gnClaimedHostIPAddrCount*2)-1)))
#else
		if (lPort != TCP_IP_BACKPLANE_PORT_NUMBER)
#endif
		{
			connmgrStats.iOpenFormatRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_PORT_NOT_AVAILABLE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* Assign the host addressed based on the port */
		if (lPort != TCP_IP_BACKPLANE_PORT_NUMBER)
			pConnection->cfg.lHostIPAddr = gsTcpIpInstAttr[(lPort-TCP_IP_STARTING_PORT_NUMBER)/2].InterfaceConfig.lIpAddr;

		/* Get the first IP address */
		pAddr = pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_LINK_ADDRESS, 0, &iTagSize );
		if (iTagSize == 0)
		{
			/* if it's not an address, maybe it's a slot */
			iSlot = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_SLOT, 0, &iTagSize );

			if (iTagSize == 0)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_LINK_ADDRESS);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
#ifdef ET_IP_MODBUS
		else
		{
			lIPAddress = platformAddrFromPath( (const char*)pAddr, iTagSize);

			if ( lIPAddress != 0 )
			{
				/* Check to see if the connection will be routed through a ModbusTCP gateway */
				lPort = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_PORT, 1, &iTagSize );
				if (iTagSize != 0)
				{
					if (lPort != MODBUS_TCP_TO_SL_PORT_NUMBER)
					{
						connmgrStats.iOpenFormatRejects++;
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_PORT_NOT_AVAILABLE);
						connectionRemove( pConnection, FALSE );
						return TRUE;
					}

					/* a slot must follow */
					iSlot = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_SLOT, 0, &iTagSize );
					if (iTagSize == 0)
					{
						connmgrStats.iOpenFormatRejects++;
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_LINK_ADDRESS);
						connectionRemove( pConnection, FALSE );
						return TRUE;
					}
				}

				return eipmbsParseCIPModbusFORequest(pRequest, pConnection, &FwdOpen, bLargeFwdOpen);
			}
			else
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_LINK_ADDRESS);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
#endif
	}
#endif /* #ifndef EIP_NO_CIP_ROUTING */

	/* Support only Class 0, 1 and 3 connections */
	if ((connectionIsIOConnection(pConnection) == FALSE) && (pConnection->cfg.bTransportClass != Class3))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_TRANSPORT_CLASS_NOT_SUPPORTED);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* Check for supported trigger types */
	if ((((pConnection->cfg.bTransportType & SERVER_CLASS_TYPE_MASK) == 0) &&
		  ((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) != Cyclic) &&
		  ((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) != ChangeOfState) &&
		  ((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) != ApplicationTriggered)))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_PRODUCTION_TRIGGER_NOT_SUPPORTED);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if (pConnection->cfg.bTransportClass == Class3)
	{
		/* Class3 must be application triggered */
		if ((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) != ApplicationTriggered)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_CLASS_TRIGGER_INVALID);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* and a server */
		if ((pConnection->cfg.bTransportType & SERVER_CLASS_TYPE_MASK) != SERVER_CLASS_TYPE_MASK)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_DIRECTION_NOT_SUPPORTED);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	connectionParseConnectionPath( &pConnection->cfg );

	/* Check if keying is being used, then it must match the target Ids */
	pDeviceID = (EtIPDeviceID*)pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_KEY, 0, &iTagSize);
	if (iTagSize != 0 )
	{
#ifndef EIP_NO_CIP_ROUTING
		if ((!gbBackplaneSupport) ||
			(iSlot == INVALID_SLOT_INDEX) ||
			(iSlot == giBackplaneSlot))
		{
#endif
			if (!idCheckElectronicKey(pDeviceID, &pConnection->bGeneralStatus, &pConnection->iExtendedStatus))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, pConnection->bGeneralStatus, pConnection->iExtendedStatus);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
#ifndef EIP_NO_CIP_ROUTING
		}
#endif
	}

	iClass = (UINT32)pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_CLASS, 0, &iClassTagSize);
	iInstance = (UINT32)pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_INSTANCE, 0, &iInstanceTagSize);

	/* Check for some basic path conformance. The path should reference an object or a tag or an object/tag */
	if (iClassTagSize == 0)
	{
		if ( !(iClassTagSize == 0 && iInstanceTagSize == 0 ) )
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}
	else if ( ( connectionIsIOConnection(pConnection) && iClass != ASSEMBLY_CLASS ) ||
			( pConnection->cfg.bTransportClass == Class3 && (iClass != ROUTER_CLASS || iInstance != 1)) )
	{
		/* NULLForwardOpen used as a ping, to query the identity object to see if a device
		   is there.  This is a special case with special handling */
		if ((connectionIsIOConnection(pConnection)) && (iClass == ID_CLASS) &&
			(pConnection->cfg.iConsumingConnectionType == NullConnType) && 
			(pConnection->cfg.iProducingConnectionType == NullConnType))
		{
			pConnection->lConsumingCID = FwdOpen.lLeOTConnId; /* Keep the same O->T connection ID */
			connmgrPrepareFwdOpenReply(pConnection, pRequest, 0, NULL);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* Sub-millisecond RPIs not supported */
	if ((pConnection->cfg.lProducingDataRate == 0) ||
		(pConnection->cfg.lConsumingDataRate == 0))
	{
		connmgrStats.iOpenResourceRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE);

		/* add the additional status information needed with an invalid RPI */
		pRequest->iExtendedErrorDataSize = 10;
		pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
		pTmpData = pRequest->pExtendedErrorData;
		*pTmpData = (UINT8)((pConnection->cfg.lConsumingDataRate == 0) ? CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE : CONNMGR_EXT_ERR_RPI_ACCEPTABLE);
		pTmpData++;
		*pTmpData = (UINT8)((pConnection->cfg.lProducingDataRate == 0) ? CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE : CONNMGR_EXT_ERR_RPI_ACCEPTABLE);
		pTmpData++;
		UINT32_SET(pTmpData, ((pConnection->cfg.lConsumingDataRate == 0) ? 1000 : (pConnection->cfg.lConsumingDataRate*1000)));
		pTmpData += sizeof(UINT32);
		UINT32_SET(pTmpData, ((pConnection->cfg.lProducingDataRate == 0) ? 1000 : (pConnection->cfg.lProducingDataRate*1000)));
		pTmpData += sizeof(UINT32);
		connectionRemove( pConnection, FALSE);
		return TRUE;
	}

	if (pConnection->cfg.bProductionTOInhibitInterval > pConnection->cfg.lProducingDataRate)
	{
		/* Inhibit time isn't real useful if it's greater than RPI */
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_RPI_SMALLER_THAN_INHIBIT);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}


	/* get the module config data */
	pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );

	if (pConnection->cfg.bTransportClass == Class3)
	{
		/* must connected to instance 1 of the message router */
		if (pConnection->cfg.iTargetConfigConnInstance != 1)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* Verify the class3 explicit message can fit */
		if (iInputDataSize > MAX_DATA_FIELD_SIZE)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_SIZE);
			pRequest->iExtendedErrorDataSize = 2;
			pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
			UINT16_SET(pRequest->pExtendedErrorData, (UINT16)(MAX_DATA_FIELD_SIZE+DATA_SEQUENCE_COUNT_SIZE));
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (iOutputDataSize > MAX_DATA_FIELD_SIZE)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_SIZE);
			pRequest->iExtendedErrorDataSize = 2;
			pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
			UINT16_SET(pRequest->pExtendedErrorData, (UINT16)(MAX_DATA_FIELD_SIZE+DATA_SEQUENCE_COUNT_SIZE));
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		/* Set the max size to the size requested in the fwd_open */
		pConnection->cfg.iCurrentConsumingSize = iInputDataSize;
		pConnection->cfg.iCurrentProducingSize = iOutputDataSize;
	}

	if (connectionIsIOConnection(pConnection))
	{
		/* Make sure for received either connection points or valid tag name(s) */
		if (pConnection->cfg.iTargetConsumingConnPoint == INVALID_CONN_POINT)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONSUMING_PATH);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (pConnection->cfg.iTargetProducingConnPoint == INVALID_CONN_POINT)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_PRODUCING_PATH);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}

		if (iModuleConfigSize > 0)
		{
			pConfigAssembly = assemblyGetByInstance( pConnection->cfg.iTargetConfigConnInstance );
			if (( pConfigAssembly == NULL ) || !(pConfigAssembly->iType & AssemblyConfiguration))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIGURATION_PATH);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* Check for size. If the size is an odd number we will receive the length, which is one 
			   byte larger since the data segment data size is specified in words. */
			if ((pConfigAssembly->iSize != iModuleConfigSize) || 
				((pConfigAssembly->iSize%2) && (pConfigAssembly->iSize != (iModuleConfigSize-1))))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONFIGURATION_SIZE);

				/* add the additional status information needed with a bad configuration size */
				pRequest->iExtendedErrorDataSize = 2;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				UINT16_SET(pRequest->pExtendedErrorData, ((pConfigAssembly->iSize+1)/2));
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}

		if ((pConnection->cfg.iTargetConsumingConnPoint != gHeartbeatConnPoint) &&
			(pConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint))
		{
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			if (assemblyIsScannerInputInstance(pConnection->cfg.iTargetConsumingConnPoint))
			{
				/* Check if there is reserved space for a target connection */
				if (!configCheckTargetConsumeConnPt(&pConnection->cfg, iInputDataSize))
				{
					connmgrStats.iOpenOtherRejects++;
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONSUMING_PATH);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
			}
			else
			{
#endif
			pInputAssembly = assemblyGetByInstance( pConnection->cfg.iTargetConsumingConnPoint );
			if (( pInputAssembly == NULL ) || !(pInputAssembly->iType & AssemblyConsuming))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONSUMING_PATH);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			if ( pInputAssembly->iSize == iInputDataSize )
			{
				pConnection->cfg.bInputRunProgramHeader = FALSE;
				if (pConnection->cfg.bRedundantOwner || gbAutomaticRunIdleEnforcement)
				{
					/* Redundant owner connection and Run/Idle "enforcement" must have 32-bit header */
					connmgrStats.iOpenOtherRejects++;
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_SIZE);
					pRequest->iExtendedErrorDataSize = 2;
					pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
					if (pConnection->cfg.bTransportClass == Class0)
					{
						UINT16_SET(pRequest->pExtendedErrorData, (pInputAssembly->iSize+4));
					}
					else
					{
						UINT16_SET(pRequest->pExtendedErrorData, (pInputAssembly->iSize+4+DATA_SEQUENCE_COUNT_SIZE));
					}
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
			}
			else if ( pInputAssembly->iSize == (iInputDataSize - 4) )
			{
				pConnection->cfg.bInputRunProgramHeader = TRUE;
			}
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_SIZE);
				pRequest->iExtendedErrorDataSize = 2;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				/* Assume a 32-bit header is involved */
				if (pConnection->cfg.bTransportClass == Class0)
				{
					UINT16_SET(pRequest->pExtendedErrorData, (pInputAssembly->iSize+4));
				}
				else
				{
					UINT16_SET(pRequest->pExtendedErrorData, (pInputAssembly->iSize+4+DATA_SEQUENCE_COUNT_SIZE));
				}
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			if ((pConnection->cfg.bRedundantOwner) &&
				(pConnection->cfg.iProducingConnectionType != Multicast))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_CONNECTION_TYPE);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* Set the max size to the size requested in the fwd_open this will be the max in a class 1 variable size connection */
			pConnection->cfg.iCurrentConsumingSize = pInputAssembly->iSize;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			}
#endif
		}
		else
		{
			if (pConnection->cfg.bRedundantOwner)
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_CONSUMING_PATH);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* Allow for Run/Idle header for heartbeat connection if not "enforced" */
			if ((iInputDataSize == 4) && (gbAutomaticRunIdleEnforcement == FALSE))
			{
				pConnection->cfg.bInputRunProgramHeader = TRUE;
			}
			else if (iInputDataSize == 0)
				pConnection->cfg.bInputRunProgramHeader = FALSE;
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_SIZE);
				pRequest->iExtendedErrorDataSize = 2;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				/* Assume "heartbeat data" is involved */
				if (pConnection->cfg.bTransportClass == Class0)
				{
					UINT16_SET(pRequest->pExtendedErrorData, 0);
				}
				else
				{
					UINT16_SET(pRequest->pExtendedErrorData, DATA_SEQUENCE_COUNT_SIZE);
				}

				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}

		if ( pConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint )
		{
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)

			if (assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
			{
				/* Check if there is reserved space for a target connection */
				if (!configCheckTargetProduceConnPt(&pConnection->cfg, iOutputDataSize))
				{
					connmgrStats.iOpenOtherRejects++;
	 				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_PRODUCING_PATH);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
			}
			else
			{
#endif
			pOutputAssembly = assemblyGetByInstance( pConnection->cfg.iTargetProducingConnPoint );
			if (( pOutputAssembly == NULL ) || !(pOutputAssembly->iType & AssemblyProducing))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_PRODUCING_PATH);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			if ( pOutputAssembly->iSize == iOutputDataSize )
			{
				pConnection->cfg.bOutputRunProgramHeader = FALSE;
			}
			else if ((pOutputAssembly->iSize == (iOutputDataSize - 4) ) && (gbAutomaticRunIdleEnforcement == FALSE))
			{
				pConnection->cfg.bOutputRunProgramHeader = TRUE;
			}
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_SIZE);
				pRequest->iExtendedErrorDataSize = 2;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				/* Assume "modeless data" is involved */
				if (pConnection->cfg.bTransportClass == Class0)
				{
					UINT16_SET(pRequest->pExtendedErrorData, (UINT16)(pOutputAssembly->iSize));
				}
				else
				{
					UINT16_SET(pRequest->pExtendedErrorData, (UINT16)(pOutputAssembly->iSize+DATA_SEQUENCE_COUNT_SIZE));
				}
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* Set the max size to the size requested in the fwd_open this will be the max in a class 1 variable size connection */
			pConnection->cfg.iCurrentProducingSize = pOutputAssembly->iSize;
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			}
#endif
		}
		else
		{
			/* Allow for Run/Idle header for heartbeat connection */
			if ((iOutputDataSize == 4) && (gbAutomaticRunIdleEnforcement == FALSE))
				pConnection->cfg.bOutputRunProgramHeader = TRUE;
			else if (iOutputDataSize == 0)
				pConnection->cfg.bOutputRunProgramHeader = FALSE;
			else
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_SIZE);
				pRequest->iExtendedErrorDataSize = 2;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				/* Assume "heartbeat data" is involved */
				if (pConnection->cfg.bTransportClass == Class0)
				{
					UINT16_SET(pRequest->pExtendedErrorData, 0);
				}
				else
				{
					UINT16_SET(pRequest->pExtendedErrorData, DATA_SEQUENCE_COUNT_SIZE);
				}
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}
	}
	else
	{
		pConnection->cfg.bInputRunProgramHeader = pConnection->cfg.bOutputRunProgramHeader = FALSE;
	}

	if ((pConnection->cfg.iConsumingConnectionType == NullConnType ) && 
		(pConnection->cfg.iProducingConnectionType == NullConnType))
	{
		/* NULL forward open */
		pConnection->lConsumingCID = FwdOpen.lLeOTConnId; /* Keep the same O->T connection ID */
		return connmgrNULLConnection(pRequest, pConnection, &FwdOpen);
	}

	if ((pConnection->cfg.iConsumingConnectionType != PointToPoint)
#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
		&& (pConnection->cfg.iConsumingConnectionType != Multicast)
#endif
		)
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_OT_CONNECTION_TYPE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	if ((pConnection->cfg.iProducingConnectionType != PointToPoint) && (pConnection->cfg.iProducingConnectionType != Multicast))
	{
		connmgrStats.iOpenOtherRejects++;
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_INVALID_TO_CONNECTION_TYPE);
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* Check if we already have a connection with the same serial numbers,
		   then it's a duplicate Forward Open */
		if ( pLoopConnection != pConnection && pLoopConnection->lConnectionState == ConnectionEstablished &&
			 pLoopConnection->iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
			 pLoopConnection->iVendorID == pConnection->iVendorID &&
			 pLoopConnection->lDeviceSerialNbr == pConnection->lDeviceSerialNbr)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_DUPLICATE_FWD_OPEN);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	/* Reset timeout ticks based on the connection settings */
	connectionRecalcTimeouts( pConnection );

	if (connectionIsIOConnection(pConnection))
	{
		/* Check what incoming connection are already configured for these connection points */    
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			if ((pLoopConnection->cfg.bOriginator) || 
				((pLoopConnection->lConnectionState != ConnectionConfiguring) && (pLoopConnection->lConnectionState != ConnectionEstablished)) ||
				(pLoopConnection == pConnection))
				continue;

			if ( (connectionIsDataConnPoint(pLoopConnection->cfg.iTargetConsumingConnPoint) 
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
				  || (assemblyIsScannerInputInstance(pConnection->cfg.iTargetConsumingConnPoint))
#endif
				  ) && (pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
				pOTConnection = pLoopConnection;

			if ( pLoopConnection->cfg.iTargetProducingConnPoint != 0 &&
				 pLoopConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint && 
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
				(!assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint)) &&
#endif
				 (pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
				 (pLoopConnection->cfg.iProducingConnectionType == Multicast) &&
				 (pConnection->cfg.iProducingConnectionType == Multicast))
				pTOConnection = pLoopConnection;

			/* configuration instance can't be shared with existing consuming connection */
			if ((iModuleConfigSize > 0) && 
				(pConnection->cfg.iTargetConfigConnInstance == pLoopConnection->cfg.iTargetConsumingConnPoint))
			{
				connmgrStats.iOpenOtherRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_OWNERSHIP_CONFLICT);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* If another connection specified this configuration application path (and specified configuration data) */
			if ((iModuleConfigSize > 0) &&
				(pConnection->cfg.iTargetConfigConnInstance == pLoopConnection->cfg.iTargetConfigConnInstance))
			{
				/* If  this configuration doesn't match the other connection configuration, return ownership conflict error */
				pLoopModuleConfigData = (UINT8*)pduGetTagByType( &pLoopConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iLoopModuleConnectionSize);
				if ((iLoopModuleConnectionSize > 0) &&
					((iModuleConfigSize != iLoopModuleConnectionSize) ||
					(memcmp(pModuleConfigData, pLoopModuleConfigData, iModuleConfigSize) != 0)))
				{
					connmgrStats.iOpenOtherRejects++;
					pRequest->bGeneralError = ROUTER_ERROR_FAILURE;
					pRequest->iExtendedError = ROUTER_EXT_ERR_OWNERSHIP_CONFLICT;
					connectionRemove( pConnection, FALSE );
					EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
					return TRUE;
				}
			}
		}
	}

	/* If a new instance */
	if ( pOTConnection == NULL && pTOConnection == NULL )
	{
		pConnection->cfg.nInstance = connectionGetUnusedInstance();

#ifdef ET_IP_SCANNER
		/* Forward Open was not preceeded with the CC config request for an incoming connection */
		pConnection->bCCConfigured = FALSE;
		connectionSetConnectionFlag( pConnection );
#ifndef EIP_NO_CCO
		glEditSignature = 0;
#endif
#endif
	}
	else
	{
		if ( pOTConnection != NULL )	/* the incoming request is attempting to re-use a connection point */
		{
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			if (pConnection->bCCConfigured)
			{
				if (assemblyIsScannerInputInstance(pConnection->cfg.iTargetConsumingConnPoint))
				{
					if (configCheckConnection(pConnection) == NULL)
					{
						connmgrStats.iOpenOtherRejects++;
						connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_OWNERSHIP_CONFLICT);
						connectionRemove( pConnection, FALSE );
						return TRUE;
					}
				}

				pConnection->cfg.nInstance = pOTConnection->cfg.nInstance;
				memcpy( &pConnection->cfg.deviceId, &pOTConnection->cfg.deviceId, sizeof(EtIPDeviceID) );
				memcpy( &pConnection->cfg.proxyDeviceId, &pOTConnection->cfg.proxyDeviceId, sizeof(EtIPDeviceID) );
				memcpy( &pConnection->cfg.remoteDeviceId, &pOTConnection->cfg.remoteDeviceId, sizeof(EtIPDeviceID) );
				if ( pOTConnection->cfg.iConnectionNameSize )
				{
					pConnection->cfg.pConnectionName = EtIP_malloc(pOTConnection->cfg.pConnectionName, pOTConnection->cfg.iConnectionNameSize);
					pConnection->cfg.iConnectionNameSize = pOTConnection->cfg.iConnectionNameSize;
				}
				if ( pOTConnection->cfg.iTargetConsumingConnTagSize )
				{
					pConnection->cfg.targetConsumingConnTag = EtIP_malloc(pOTConnection->cfg.targetConsumingConnTag, pOTConnection->cfg.iTargetConsumingConnTagSize);
					pConnection->cfg.iTargetConsumingConnTagSize = pOTConnection->cfg.iTargetConsumingConnTagSize;
				}
				if ( pOTConnection->cfg.iTargetProducingConnTagSize )
				{
					pConnection->cfg.targetProducingConnTag = EtIP_malloc(pOTConnection->cfg.targetProducingConnTag, pOTConnection->cfg.iTargetProducingConnTagSize);
					pConnection->cfg.iTargetProducingConnTagSize = pOTConnection->cfg.iTargetProducingConnTagSize;
				}
				pConnection->cfg.iConnectionFlag = pOTConnection->cfg.iConnectionFlag;

				if ( pTOConnection != NULL)
					pConnection->nCCProducingInstance = pTOConnection->cfg.nInstance;
				else
					pConnection->nCCProducingInstance = INVALID_INSTANCE;
			}
			else
			{
#endif
					/* If the incoming connection is not a redundant owner request, 
						then we've got an ownership conflict */
				if ((!pConnection->cfg.bRedundantOwner) ||
					(!pOTConnection->cfg.bRedundantOwner))
				{
					connmgrStats.iOpenOtherRejects++;
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_OWNERSHIP_CONFLICT);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}

				/* if the incoming request is for a redundant owner, check to see if it's allowed and set it up */
				if (!connmgrVerifyRedundantOwnerRequest(pConnection, pOTConnection))
				{
					connmgrStats.iOpenOtherRejects++;
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_REDUNDANT_CONNECTION_MISMATCH);
					connectionRemove( pConnection, FALSE );
					return TRUE;
				}
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			}
#endif
		}
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		else
		{
			if ( pTOConnection->lConnectionState != ConnectionConfiguring || pTOConnection->lConfigurationState != ConfigurationLogged )
			{
				if ((pConnection->cfg.iProducingConnectionType == Multicast) &&
					(pTOConnection->cfg.iProducingConnectionType == Multicast) &&
					(pConnection->cfg.lProducingDataRate == pTOConnection->cfg.lProducingDataRate))
				{
					pConnection->cfg.nInstance = connectionGetUnusedInstance();
					pConnection->bCCConfigured = pTOConnection->bCCConfigured;
				}
			}
			else
			{
				pConnection->cfg.nInstance = pTOConnection->cfg.nInstance;
				pConnection->nCCProducingInstance = INVALID_INSTANCE;
			}
			pConnection->cfg.iConnectionFlag = pTOConnection->cfg.iConnectionFlag;
		}
#endif

		if ((pConnection->cfg.iProducingConnectionType == Multicast) &&
			(pTOConnection->cfg.iProducingConnectionType == Multicast))
		{
			if (pConnection->cfg.lProducingDataRate != pTOConnection->cfg.lProducingDataRate )
			{
				connmgrStats.iOpenResourceRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE);

				/* add the additional status information needed with an invalid RPI */
				pRequest->iExtendedErrorDataSize = 10;
				pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
				pTmpData = pRequest->pExtendedErrorData;
				*pTmpData = CONNMGR_EXT_ERR_RPI_ACCEPTABLE;
				pTmpData++;
				*pTmpData = CONNMGR_EXT_ERR_RPI_REQUIRED;
				pTmpData++;
				UINT32_SET(pTmpData, (pConnection->cfg.lConsumingDataRate*1000));
				pTmpData += sizeof(UINT32);
				UINT32_SET(pTmpData, (pTOConnection->cfg.lProducingDataRate*1000));
				pTmpData += sizeof(UINT32);
				connectionRemove( pConnection, FALSE);
				return TRUE;
			}

			/* For multicasting data, both connections must be fixed or variable */
			if (pConnection->cfg.bClass1VariableProducer != pTOConnection->cfg.bClass1VariableProducer)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_MISMATCH_TO_FIXED_VAR);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* For multicasting data, both connections must have same priority */
			if (pConnection->cfg.iProducingPriority != pTOConnection->cfg.iProducingPriority)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_MISMATCH_TO_CONN_PRIORITY);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* For multicasting data, both connections must have same transport class */
			if (pConnection->cfg.bTransportClass != pTOConnection->cfg.bTransportClass)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_MISMATCH_TRANSPORT_CLASS);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* For multicasting data, both connections must have same production trigger */
			if (pConnection->cfg.bTransportType != pTOConnection->cfg.bTransportType)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_TRIGGER);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}

			/* For multicasting data, both connections must have same production trigger */
			if (pConnection->cfg.bProductionTOInhibitInterval != pTOConnection->cfg.bProductionTOInhibitInterval)
			{
				connmgrStats.iOpenFormatRejects++;
				connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_INHIBIT);
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
		}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (pConnection->bCCConfigured)
		{
			pOrigTOConnection = pTOConnection;

			if ( pOTConnection != NULL )
			{
				/* We can remove now nOTConnection connection since we created a new one at nConnection */
				pOTConnection->cfg.nInstance = INVALID_CONN_INSTANCE;
				connectionRemove( pOTConnection, FALSE );        

				if ( pOTConnection < pConnection )
					pConnection--;

				if ( pTOConnection != NULL && pOTConnection < pTOConnection )
					pTOConnection--;
			}

			if ( pTOConnection != NULL && pOTConnection != pOrigTOConnection && 
				 pTOConnection->lConnectionState == ConnectionConfiguring && 
				 pTOConnection->lConfigurationState == ConfigurationLogged )
			{
				if (!assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
				{
					/* We can remove now nOTConnection connection since we created a new one at nConnection */
					pTOConnection->cfg.nInstance = INVALID_CONN_INSTANCE;
					connectionRemove( pTOConnection, FALSE );        

					if ( pTOConnection < pConnection )
						pConnection--;
				}
			}
		}
		else if (pConnection->cfg.nInstance == 0)
		{
#endif
			pConnection->cfg.nInstance = connectionGetUnusedInstance();
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		}
#endif
	}

	pTcpipInterface = tcpipFindClaimedHostAddress( pRequest->lHostIPAddr );
	if (pTcpipInterface == NULL)
	{
		/* This shouldn't happen */
		connectionRemove( pConnection, FALSE );
		return TRUE;
	}

	/* For Class 0/1 using UDP packets to transport I/O 
	 * For Class 3 using TCP packets to transport data */
	usPort = (UINT16)(connectionIsIOConnection(pConnection) ? CLASS1_UDP_PORT : pTcpipInterface->iPort);

	/* Populate the sReceiveAddr */
#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
	if ( pConnection->cfg.iConsumingConnectionType != Multicast )
	{
#endif
		platformSetSockaddrIn( &pConnection->sReceiveAddr, usPort, pConnection->cfg.lHostIPAddr );
#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
	}
	else
	{
		memcpy( &pConnection->sReceiveAddr, &pRequest->sOTTagAddr, sizeof(struct sockaddr_in) );
	}
#endif

	/* Populate the sTransmitAddr (IP Address is actually set below) */
	platformSetSockaddrIn( &pConnection->sTransmitAddr, usPort, htonl( INADDR_ANY ) );

	if ( pConnection->cfg.iProducingConnectionType != Multicast )
	{
		/* Always use default transmit address */
		platformGetInternetAddress( pConnection->sTransmitAddr.sin_addr ) = pConnection->cfg.lIPAddress;
		if (pRequest->sTOTagAddr.sin_port != 0)
			pConnection->sTransmitAddr.sin_port = pRequest->sTOTagAddr.sin_port;
	}
	else
	{
		/* Determine multicast address to use based on Ethernet Link attribute */
		switch(pTcpipInterface->InterfaceConfig.MulticastData.bAllocControl)
		{
		case 0:
			/* multicast algorithm */
			platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr) = platformGetInternetAddress(pTcpipInterface->MulticastBaseAddr.sin_addr) + htonl(pTcpipInterface->lMulticastAddrCount%CIP_MAX_NUM_MULTICAST_ADDRESS);
			break;
		case 1:
			/* "static" multicast address */
			platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr) = pTcpipInterface->InterfaceConfig.MulticastData.lMcastStartAddr + htonl(pTcpipInterface->lMulticastAddrCount%pTcpipInterface->InterfaceConfig.MulticastData.iNumMcast);
			break;
		}

		pTcpipInterface->lMulticastAddrCount++;

		/* Check if this is a multicast connection hooking up to one of the previously
		established multicast connections. */
		pMulticastProducer = connectionGetAnyMultiProducer( pConnection );

		if ( pMulticastProducer != NULL)
		{
			pConnection->lProducingCID = pMulticastProducer->lProducingCID;
			pConnection->sTransmitAddr = pMulticastProducer->sTransmitAddr;
			pConnection->lOutAddrSeqNbr = pMulticastProducer->lOutAddrSeqNbr;
		}
	}

	/* Check if this is a listen only connection hooking up to one of the previously 
	   established multicast connections to the same assembly instance. */
	if ( !pConnection->cfg.bOriginator &&
		 connectionIsIOConnection(pConnection) && 
		 pConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint )
	{
		if ( pConnection->cfg.iProducingConnectionType == Multicast )
		{
			/* Find exclusive owner */
			for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
			{
				if ( pLoopConnection != pConnection &&
					 !pLoopConnection->cfg.bOriginator &&
					 pLoopConnection->lConnectionState == ConnectionEstablished &&
					 connectionIsIOConnection(pLoopConnection) &&
					 pConnection->cfg.bTransportClass == pLoopConnection->cfg.bTransportClass &&
					 pConnection->cfg.lProducingDataRate == pLoopConnection->cfg.lProducingDataRate &&
					 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
					 pConnection->cfg.bTransportType == pLoopConnection->cfg.bTransportType &&
					 pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint &&
					 pLoopConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint )
				{
					/* This is a listen-only incoming connection. Make sure that we specify
					   the same Connection ID and the IP address to listen on. */
					pConnection->lProducingCID = pLoopConnection->lProducingCID;
					pConnection->sTransmitAddr = pLoopConnection->sTransmitAddr;
					break;
				}
			}
		}

		/* Couldn't find an owner for the listen only connection */
		if (pConnection->cfg.iProducingConnectionType != Multicast || pLoopConnection == gpnConnections)
		{
			connmgrStats.iOpenOtherRejects++;
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION);
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	if (gbAppVerifyConnection)
	{
		pConnection->nRequestId = pRequest->nIndex;
		/* Let the library know this connection is still "in progress" */
		pConnection->lConnectionState = ConnectionConfiguring;
		pConnection->lConfigurationState = ConfigurationLogged;
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		notifyEvent( NM_CONNECTION_VERIFICATION, pConnection->cfg.nInstance, 0 );
		return FALSE;
	}

	/* Set module configuration data (if applicable) */
	pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );
	if ( assemblyNotifyInstanceChange(pConnection->cfg.iTargetConfigConnInstance, pModuleConfigData, iModuleConfigSize, FALSE) )
	{
		assemblyNotifyMembersChange(pConnection->cfg.iTargetConfigConnInstance, INVALID_MEMBER, pModuleConfigData, iModuleConfigSize );
		assemblySetInstanceData(pConnection->cfg.iTargetConfigConnInstance, pModuleConfigData, iModuleConfigSize );    /* Store data in assembly object */
	}

	connmgrPrepareConnection(pConnection, pRequest, 0, NULL);
	return TRUE;
}

/*---------------------------------------------------------------------------
** connmgrPrepareConnection( )
** 
** Prepare connection after it has been successfully verified
**---------------------------------------------------------------------------
*/
void connmgrPrepareConnection(CONNECTION* pConnection, REQUEST* pRequest, UINT8 bAppSize, UINT8* pAppData)
{
	CONNECTION* pLoopConnection, *pOwnerConnection = NULL;
	FWD_OPEN    FwdOpen;
	BOOL bSendData = TRUE;

	if ((pConnection->cfg.iConsumingConnectionType == NullConnType ) && 
		(pConnection->cfg.iProducingConnectionType == NullConnType))
	{
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			/* Check if we already have a connection with the same serial numbers,
			   then it's a duplicate Forward Open */
			if ( pLoopConnection != pConnection && pLoopConnection->lConnectionState == ConnectionEstablished &&
				 !pLoopConnection->cfg.bOriginator &&
				 pLoopConnection->iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
				 pLoopConnection->iVendorID == pConnection->iVendorID &&
				 pLoopConnection->lDeviceSerialNbr == pConnection->lDeviceSerialNbr)
			{
				pOwnerConnection = pLoopConnection;
				break;
			}
		}

		if (pOwnerConnection == NULL)
		{
			/* Couldn't find the matching connection, connection will be rejected */
			connmgrStats.iOpenOtherRejects++;

			connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);
			connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION);
		}
		else
		{
			notifyEvent( NM_CONNECTION_RECONFIGURED, pOwnerConnection->cfg.nInstance, 0 );

			/* Send Forward Open reply */
			connmgrPrepareFwdOpenReply(pConnection, pRequest, bAppSize, pAppData);
		}

		/* remove temporary connection */
		connectionRemove(pConnection, FALSE);
	}
	else
	{
		/* Send Forward Open reply */
		connmgrPrepareFwdOpenReply(pConnection, pRequest, bAppSize, pAppData);

		/* Reset timeout and producing ticks */
		connectionResetAllTicks( pConnection );

#ifdef EIP_LARGE_CONFIG_OPT
		/* add to the ID hash table */
		connListAddCnxnIdTableEntry(pConnection->lConsumingCID, pConnection->cfg.lIPAddress, pConnection);
#endif

		/* Postpone the configuration of all other connections with the same IP address for CONNECTION_CONFIGURATION_DELAY period */
		connectionDelayPending( pConnection );

		if (connectionIsIOConnection(pConnection))
		{
			/* Check if we are already listening to this address on another active connection */
			for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ ) 
			{
				if ((pLoopConnection != pConnection) &&
					(connectionIsIOConnection(pLoopConnection)) &&
					(pLoopConnection->lConnectionState == ConnectionEstablished) &&
					(pLoopConnection->cfg.lHostIPAddr == pConnection->cfg.lHostIPAddr) &&
					(platformGetInternetAddress(pLoopConnection->sReceiveAddr.sin_addr) == platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr)) &&
					(pLoopConnection->cfg.bTransportClass == pConnection->cfg.bTransportClass) &&
					(pLoopConnection->sReceiveAddr.sin_port == pConnection->sReceiveAddr.sin_port) &&
					(pLoopConnection->cfg.iProducingConnectionType == pConnection->cfg.iProducingConnectionType) &&
					(pLoopConnection->cfg.iConsumingConnectionType == pConnection->cfg.iConsumingConnectionType))
				{
					pConnection->lClass1Socket = pLoopConnection->lClass1Socket;
					break;
				}
			}

			if ( pLoopConnection == gpnConnections )		/* Not found identical UDP configuration */
			{
				if (!connectionAssignClass1Socket(pConnection))
				{
					connmgrStats.iOpenOtherRejects++;

					connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);
					connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_CONNECTION_LIMIT_REACHED);
					connectionRemove(pConnection, FALSE);
					return;
				}
#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
				if ( IS_MULTICAST(platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr) ) )
					socketJoinMulticastGroup( pConnection );
#endif
			}

			/* Make sure an Exclusive Owner I/O connection sticks around until explicitly closed or 
			   reset so the correct LED state can be reported */
			if ((pConnection->cfg.iTargetConsumingConnPoint != gHeartbeatConnPoint) &&
				(pConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint))
				pConnection->cfg.iWatchdogTimeoutAction = TimeoutManualReset;

			/* Remove any timed out connections with the same connection points
			   At this point the connection has been "healed" and the network
			   LED may change */
			for ( pLoopConnection = gpnConnections-1; pLoopConnection >= gConnections;) 
			{
				if ((pLoopConnection != pConnection) &&
					(connectionIsIOConnection(pLoopConnection)) &&
					(pLoopConnection->lConnectionState == ConnectionTimedOut) &&
					(pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint) &&
					(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint))
				{
					connectionRemove(pLoopConnection, FALSE);
					if (pLoopConnection < pConnection)
						pConnection--;
				}
				else
				{
					pLoopConnection--;
				}
			}
		}

#ifdef CONNECTION_STATS
		pConnection->bCollectingStats = FALSE;
#ifdef EIP_LARGE_CONFIG_OPT
		connListQueueCnxnTimer(pConnection, CNXN_TIMER_TYPE_STATS_DELAY, COLLECTING_STATS_DELAY);
#else
		pConnection->lStartCollectStatsTick = gdwTickCount + COLLECTING_STATS_DELAY;
#endif

		if (connectionIsIOConnection(pConnection))
		{
			gSystemStats.systemStats.iNumCurrentIOConnections++;
			if (gSystemStats.systemStats.iMaxActiveIOConnections < gSystemStats.systemStats.iNumCurrentIOConnections)
				gSystemStats.systemStats.iMaxActiveIOConnections = gSystemStats.systemStats.iNumCurrentIOConnections;
		}
		else if (pConnection->cfg.bTransportClass == Class3)
		{
			gSystemStats.systemStats.iNumCurrentExplicitConnections++;
			if (gSystemStats.systemStats.iMaxActiveExplicitConnections < gSystemStats.systemStats.iNumCurrentExplicitConnections)
				gSystemStats.systemStats.iMaxActiveExplicitConnections = gSystemStats.systemStats.iNumCurrentExplicitConnections;
		}
#endif
		pConnection->lConnectionState = ConnectionEstablished;
		/* Reset whether or not we've received a packet */
		pConnection->bReceivedIOPacket = FALSE;

		notifyEvent( NM_CONNECTION_ESTABLISHED, pConnection->cfg.nInstance, 0 );
		connectionNotifyNetLEDChange();

		/* This connection can't be the master at this point because no I/O
		   data has been exchanged, but this check will notify the application
		   if there isn't already an existing redundant "master" for this
		   set of connection points */
		if (pConnection->cfg.bRedundantOwner)
			connectionFindNewRedundantMaster(pConnection);
	}

	/* This ensures that the forward open reply is sent out before the first class 1 packet */
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	requestService( pRequest, &bSendData );
}

/*---------------------------------------------------------------------------
** connmgrInitBufFromFwdCloseReply()
**
** Populate the buffer from the provided Forward Close Reply structure
**---------------------------------------------------------------------------
*/
static void connmgrInitBufFromFwdCloseReply( UINT8* pBuffer, FWD_CLOSE_REPLY* fwdCloseTypeReply )
{
	UINT16_SET(pBuffer, fwdCloseTypeReply->iLeOrigConnectionSn);
	pBuffer += sizeof(UINT16);
	UINT16_SET(pBuffer, fwdCloseTypeReply->iLeOrigVendorId);
	pBuffer += sizeof(UINT16);

	UINT32_SET(pBuffer, fwdCloseTypeReply->lLeOrigDeviceSn);
	pBuffer += sizeof(UINT32);

	*pBuffer = fwdCloseTypeReply->bPathSize;
	pBuffer++;
	*pBuffer = fwdCloseTypeReply->bReserved;
}

/*---------------------------------------------------------------------------
** connmgrPrepareFwdCloseReply( )
**
** Prepare successful Forward Close reply
**---------------------------------------------------------------------------
*/
static void connmgrPrepareFwdCloseReply( CONNECTION* pConnection, REQUEST* pRequest )
{
	UINT8* pPacketPtr;
	FWD_CLOSE_REPLY FwdCloseReply;

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, FWD_CLOSE_REPLY_SIZE );
	pPacketPtr = pRequest->cfg.objectCfg.pData;

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		/* we are out of memory */
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	FwdCloseReply.lLeOrigDeviceSn = pConnection->lDeviceSerialNbr;
	FwdCloseReply.iLeOrigVendorId = pConnection->iVendorID;
	FwdCloseReply.iLeOrigConnectionSn = pConnection->iConnectionSerialNbr;
	FwdCloseReply.bPathSize = 0;
	FwdCloseReply.bReserved = 0;

	connmgrInitBufFromFwdCloseReply( pPacketPtr, &FwdCloseReply );
}

/*---------------------------------------------------------------------------
** connmgrPrepareFwdCloseErrorReply( )
**
** Prepare failed Forward Close reply
**---------------------------------------------------------------------------
*/
static void connmgrPrepareFwdCloseErrorReply(REQUEST* pRequest, FWD_CLOSE* pFwdClose, UINT8 bGeneralError, UINT16 iExtendedError)
{
	UINT8* pData;

	/* Setup the error values */
	pRequest->bGeneralError = bGeneralError;
	pRequest->iExtendedError = iExtendedError;

	/* A Forward Close error reply contains the extended error as well as the
	   connection serial number, Originator's Vendor ID and serial number
	   Attach the values to the reply */
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (sizeof(UINT16)*2)+sizeof(UINT32)+2);
	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, pFwdClose->iLeOrigConnectionSn);
	pData += sizeof(UINT16);
	UINT16_SET(pData, pFwdClose->iLeOrigVendorId);
	pData += sizeof(UINT16);
	UINT32_SET(pData, pFwdClose->lLeOrigDeviceSn);
	pData += sizeof(UINT32);

	/* Remaining path size */
	*pData = pFwdClose->bClosePathSize;
	pData++;

	/* Shall be zero */
	*pData = 0;
}

/*---------------------------------------------------------------------------
** connmgrInitFwdCloseTypeFromBuf()
**
** Populate Forward Close structure from the provided buffer
**---------------------------------------------------------------------------
*/
static void connmgrInitFwdCloseTypeFromBuf( UINT8* pBuffer, FWD_CLOSE* fwdCloseType )
{
	fwdCloseType->bOpenPriorityTickTime = *pBuffer++;
	fwdCloseType->bOpenTimeoutTicks = *pBuffer++;

	fwdCloseType->iLeOrigConnectionSn = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdCloseType->iLeOrigVendorId = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdCloseType->lLeOrigDeviceSn = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdCloseType->bClosePathSize = *pBuffer++;
	fwdCloseType->bReserved = *pBuffer;
}

/*---------------------------------------------------------------------------
** connmgrProcessFwdClose( )
**
** Parse Forward Close request
**---------------------------------------------------------------------------
*/
static void connmgrProcessFwdClose(  REQUEST* pRequest )
{
	FWD_CLOSE     FwdClose;
	CONNECTION*  pConnection;
	UINT8*       pPacketPtr = pRequest->cfg.objectCfg.pData;
#ifdef EIP_QUICK_CONNECT
	CONNECTION*  pLoopConnection;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig;
	BOOL bSendData = TRUE;
#endif

	/* increment number of forward clsoe attempts */
	connmgrStats.iCloseRequests++;

	if ( pRequest->cfg.objectCfg.PDU.bNumTags != 2 )
	{
		connmgrStats.iCloseFormatRejects++;
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pRequest->lIPAddress, pRequest->lHostIPAddr,
				"connmgrProcessFwdClose ERROR: segments other than Class and Instance were specified");
		return;
	}

	if ( pRequest->cfg.objectCfg.iDataSize < FWD_CLOSE_SIZE )
	{
		connmgrStats.iCloseFormatRejects++;
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pRequest->lIPAddress, pRequest->lHostIPAddr,
					"connmgrProcessFwdClose ERROR: not enough data specified");
		return;
	}

	/* Parse Forward Close parameters */
	connmgrInitFwdCloseTypeFromBuf( pPacketPtr, &FwdClose );

	/* Find the connection that should be closed */
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ( FwdClose.iLeOrigConnectionSn == pConnection->iConnectionSerialNbr &&
			 FwdClose.iLeOrigVendorId == pConnection->iVendorID &&
			 FwdClose.lLeOrigDeviceSn == pConnection->lDeviceSerialNbr &&
			 pConnection->lConnectionState == ConnectionEstablished )
		{
			/* Only allow Originator IP address to close connection */
			if (pConnection->cfg.lIPAddress != pRequest->lIPAddress)
			{
				connmgrStats.iCloseOtherRejects++;
				connmgrPrepareFwdCloseErrorReply(pRequest, &FwdClose, ROUTER_ERROR_PERMISSION_DENIED, 0);
				return;
			}

			break;
		}
	}

	/* Connection not found - return an error */
	if ( pConnection >= gpnConnections )
	{
		connmgrStats.iCloseOtherRejects++;
		connmgrPrepareFwdCloseErrorReply(pRequest, &FwdClose, ROUTER_ERROR_FAILURE, ROUTER_EXT_ERR_CONNECTION_NOT_FOUND);
		return;
	}

	/* Prepare successful Forward Close reply */
	connmgrPrepareFwdCloseReply( pConnection, pRequest  );

#ifdef EIP_QUICK_CONNECT
	/* If this is the only target connection on the session the socket will be closed,
	   so make sure the response goes out */
	pTcpIpConfig = tcpipFindClaimedHostAddress(pConnection->cfg.lHostIPAddr);

	if ((pTcpIpConfig != NULL) && 
		(pTcpIpConfig->InterfaceConfig.bQuickConnect == TCPIP_QUICK_CONNECT_ENABLED) &&
		(!pConnection->cfg.bOriginator))
	{
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			if ( pLoopConnection != pConnection && 
				 pLoopConnection->lConnectionState == ConnectionEstablished &&
				 pLoopConnection->nSessionId == pConnection->nSessionId)
			{
				break;
			}
		}

		if (pLoopConnection >= gpnConnections)
		{
			/* no other connections found, send the request immediately */
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			requestService( pRequest, &bSendData );
		}
	}
#endif

#ifdef ET_IP_SCANNER
	if ( pConnection->bCCConfigured )
	{
		connectionGoOffline( pConnection, FALSE );

		if ( pConnection->cfg.nInstance != (INT32)INVALID_CONN_INSTANCE && pConnection->cfg.nInstance != 0 )            /* Instance would be 0 if we were in the early stages of establishing a connection when there was an error */
		{
			notifyEvent( NM_CONNECTION_CLOSED, pConnection->cfg.nInstance, 0  );
			connectionNotifyNetLEDChange();
#ifdef CONNECTION_STATS
			if (connectionIsIOConnection(pConnection))
				gSystemStats.systemStats.iNumCurrentIOConnections--;
			else if (pConnection->cfg.bTransportClass == Class3)
				gSystemStats.systemStats.iNumCurrentExplicitConnections--;
#endif
		}

		pConnection->lConnectionState = ConnectionConfiguring;
		connListSetConnectionConfigState(pConnection, ConfigurationLogged, 0);
	}
	else
#endif

	connectionRemove( pConnection, FALSE );
}

/*---------------------------------------------------------------------------
** connmgrPrepareFwdOpenErrorReply( )
**
** Prepare failed Forward Open reply
**---------------------------------------------------------------------------
*/
void connmgrPrepareFwdOpenErrorReply(REQUEST* pRequest, FWD_OPEN* pFwdOpen, UINT8 bGeneralError, UINT16 iExtendedError)
{
	UINT8* pData;

	DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pRequest->lHostIPAddr, pRequest->lIPAddress,
			"Incoming ForwardOpen failed: GeneralError = 0x%02x, ExtendedError = 0x%04x", bGeneralError, iExtendedError);

	/* Setup the error values */
	pRequest->bGeneralError = bGeneralError;
	pRequest->iExtendedError = iExtendedError;

	/* A Forward Open error reply contains the connection serial number, Originator's Vendor ID and serial number
	   Attach the values to the reply */
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (sizeof(UINT16)*2)+sizeof(UINT32)+2);
	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, pFwdOpen->iLeOrigConnectionSn);
	pData += sizeof(UINT16);
	UINT16_SET(pData, pFwdOpen->iLeOrigVendorId);
	pData += sizeof(UINT16);
	UINT32_SET(pData, pFwdOpen->lLeOrigDeviceSn);
	pData += sizeof(UINT32);

	/* Remaining path size */
	*pData = pFwdOpen->bConnPathSize;
	pData++;

	/* Shall be zero */
	*pData = 0;

#ifdef CONNECTION_STATS
	/* Keep track of failure, it builds character */
	gSystemStats.systemStats.iNumFailedConnections++;
#endif

}

/*---------------------------------------------------------------------------
** connmgrInitFwdOpenTypeFromBuf()
**
** Populate Forward Open structure from the provided buffer
**---------------------------------------------------------------------------
*/
void connmgrInitFwdOpenTypeFromBuf( UINT8* pBuffer, FWD_OPEN* fwdOpenType, BOOL bLargeFwdOpen )
{
	fwdOpenType->bOpenPriorityTickTime = *pBuffer++;
	fwdOpenType->bOpenTimeoutTicks = *pBuffer++;

	fwdOpenType->lLeOTConnId = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);
	fwdOpenType->lLeTOConnId = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdOpenType->iLeOrigConnectionSn = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdOpenType->iLeOrigVendorId = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdOpenType->lLeOrigDeviceSn = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdOpenType->bTimeoutMult = *pBuffer++;

	fwdOpenType->bReserved = *pBuffer++;
	fwdOpenType->iReserved = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);

	fwdOpenType->lLeOTRpi = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	if ( bLargeFwdOpen )
	{
		fwdOpenType->lLargeLeOTConnParams = UINT32_GET(pBuffer);
		pBuffer += sizeof(UINT32);

		fwdOpenType->lLeTORpi = UINT32_GET(pBuffer);
		pBuffer += sizeof(UINT32);
		fwdOpenType->lLargeLeTOConnParams = UINT32_GET(pBuffer);
		pBuffer += sizeof(UINT32);
	}
	else
	{
		fwdOpenType->iLeOTConnParams = UINT16_GET(pBuffer);
		pBuffer += sizeof(UINT16);

		fwdOpenType->lLeTORpi = UINT32_GET(pBuffer);
		pBuffer += sizeof(UINT32);
		fwdOpenType->iLeTOConnParams = UINT16_GET(pBuffer);
		pBuffer += sizeof(UINT16);
	}

	fwdOpenType->bClassTrigger = *pBuffer++;
	fwdOpenType->bConnPathSize = *pBuffer;
}

/*---------------------------------------------------------------------------
** connmgrInitBufFromFwdOpenType()
**
** Populate the buffer from the provided Forward Open structure
**---------------------------------------------------------------------------
*/
void connmgrInitBufFromFwdOpenType( UINT8* pBuffer, FWD_OPEN* fwdOpenType, BOOL bLargeFwdOpen )
{
	*pBuffer = fwdOpenType->bOpenPriorityTickTime;
	pBuffer++;
	*pBuffer = fwdOpenType->bOpenTimeoutTicks;
	pBuffer++;

	UINT32_SET(pBuffer, fwdOpenType->lLeOTConnId);
	pBuffer += sizeof(UINT32);
	UINT32_SET(pBuffer, fwdOpenType->lLeTOConnId);
	pBuffer += sizeof(UINT32);

	UINT16_SET(pBuffer, fwdOpenType->iLeOrigConnectionSn);
	pBuffer += sizeof(UINT16);
	UINT16_SET(pBuffer, fwdOpenType->iLeOrigVendorId);
	pBuffer += sizeof(UINT16);
	UINT32_SET(pBuffer, fwdOpenType->lLeOrigDeviceSn);
	pBuffer += sizeof(UINT32);

	*pBuffer = fwdOpenType->bTimeoutMult;
	pBuffer++;

	*pBuffer = fwdOpenType->bReserved;
	pBuffer++;
	UINT16_SET(pBuffer, fwdOpenType->iReserved);
	pBuffer += sizeof(UINT16);

	UINT32_SET(pBuffer, fwdOpenType->lLeOTRpi);
	pBuffer += sizeof(UINT32);

	if ( bLargeFwdOpen )
	{
		UINT32_SET(pBuffer, fwdOpenType->lLargeLeOTConnParams);
		pBuffer += sizeof(UINT32);

		UINT32_SET(pBuffer, fwdOpenType->lLeTORpi);
		pBuffer += sizeof(UINT32);
		UINT32_SET(pBuffer, fwdOpenType->lLargeLeTOConnParams);
		pBuffer += sizeof(UINT32);
	}
	else
	{
		UINT16_SET(pBuffer, fwdOpenType->iLeOTConnParams);
		pBuffer += sizeof(UINT16);

		UINT32_SET(pBuffer, fwdOpenType->lLeTORpi);
		pBuffer += sizeof(UINT32);
		UINT16_SET(pBuffer, fwdOpenType->iLeTOConnParams);
		pBuffer += sizeof(UINT16);
	}

	*pBuffer = fwdOpenType->bClassTrigger;
	pBuffer++;
	*pBuffer = fwdOpenType->bConnPathSize;
}

/*---------------------------------------------------------------------------
** connmgrInitFwdOpenReplyFromBuf()
**
** Populate Forward Open Reply structure from the provided buffer
**---------------------------------------------------------------------------
*/
void connmgrInitFwdOpenReplyFromBuf( UINT8* pBuffer, FWD_OPEN_REPLY* fwdOpenTypeReply )
{
	fwdOpenTypeReply->lLeOTConnId = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);
	fwdOpenTypeReply->lLeTOConnId = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdOpenTypeReply->iLeOrigConnectionSn = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdOpenTypeReply->iLeOrigVendorId = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdOpenTypeReply->lLeOrigDeviceSn = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdOpenTypeReply->lLeOTApi = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);
	fwdOpenTypeReply->lLeTOApi = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdOpenTypeReply->bAppReplySize = *pBuffer++;
	fwdOpenTypeReply->bReserved = *pBuffer++;
	if (fwdOpenTypeReply->bAppReplySize > 0)
		fwdOpenTypeReply->pAppReply = pBuffer;
}

/*---------------------------------------------------------------------------
** connmgrInitBufFromFwdCloseType()
**
** Populate the buffer from the provided Forward Close structure
**---------------------------------------------------------------------------
*/
void connmgrInitBufFromFwdCloseType( UINT8* pBuffer, FWD_CLOSE* fwdCloseType )
{
	*pBuffer = fwdCloseType->bOpenPriorityTickTime;
	pBuffer++;
	*pBuffer = fwdCloseType->bOpenTimeoutTicks;
	pBuffer++;

	UINT16_SET(pBuffer, fwdCloseType->iLeOrigConnectionSn);
	pBuffer += sizeof(UINT16);
	UINT16_SET(pBuffer, fwdCloseType->iLeOrigVendorId);
	pBuffer += sizeof(UINT16);
	UINT32_SET(pBuffer, fwdCloseType->lLeOrigDeviceSn);
	pBuffer += sizeof(UINT32);

	*pBuffer = fwdCloseType->bClosePathSize;
	pBuffer++;
	*pBuffer = fwdCloseType->bReserved;
}

/*---------------------------------------------------------------------------
** connmgrInitFwdCloseReplyFromBuf()
**
** Populate Forward Close Reply structure from the provided buffer
**---------------------------------------------------------------------------
*/
void connmgrInitFwdCloseReplyFromBuf( UINT8* pBuffer, FWD_CLOSE_REPLY* fwdCloseTypeReply )
{
	fwdCloseTypeReply->iLeOrigConnectionSn = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);
	fwdCloseTypeReply->iLeOrigVendorId = UINT16_GET(pBuffer);
	pBuffer += sizeof(UINT16);

	fwdCloseTypeReply->lLeOrigDeviceSn = UINT32_GET(pBuffer);
	pBuffer += sizeof(UINT32);

	fwdCloseTypeReply->bPathSize = *pBuffer++;
	fwdCloseTypeReply->bReserved = *pBuffer;
}

/*---------------------------------------------------------------------------
** connmgrSendClassAttrAll( )
**
** GetAttributeAll service for Connection Manager class is received 
**---------------------------------------------------------------------------
*/
static void connmgrSendClassAttrAll( REQUEST* pRequest )
{
	UINT8* pData;

	EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(4*sizeof(UINT16)));

	pData = pRequest->cfg.objectCfg.pData;

	/* class revision */
	UINT16_SET(pData, CONNMGR_CLASS_REVISION);
	pData += sizeof(UINT16);
	/* Max instance */
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);

	/* Max class attribute */
	UINT16_SET(pData, CONNMGR_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
	/* Max instance attribute */
#ifdef EIP_BIG12
	UINT16_SET(pData, CONNMGR_ATTR_CPU_UTILIZATION);
#else
	UINT16_SET(pData, CONNMGR_ATTR_CONNECTION_TIMEOUTS);
#endif
}

/*--------------------------------------------------------------------------------
** connmgrSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the Connection Manager class
**--------------------------------------------------------------------------------
*/
static void connmgrSendClassAttrSingle( REQUEST* pRequest)
{
	UINT16 iVal, iTagSize, iNumberOfAttributes, i;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	UINT8* pData;
	UINT16 *pAttrData;

	switch (iAttribute)
	{
	case CONNMGR_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(CONNMGR_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_MAX_INSTANCE:
	case CONNMGR_CLASS_ATTR_NUM_INSTANCES:
		iVal = ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_OPTIONAL_ATTR:
		iNumberOfAttributes = sizeof(connmgrAttributes)/sizeof(UINT16);
		EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((iNumberOfAttributes+1)*sizeof(UINT16)));

		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, iNumberOfAttributes);
		pData += sizeof(UINT16);

		for (i = 0, pAttrData = connmgrAttributes; 
			i < iNumberOfAttributes; 
			i++, pAttrData++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *pAttrData);
		}
		break;
	case CONNMGR_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(CONNMGR_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_CLASS_ATTR_MAX_INST_ATTR:
#ifdef EIP_BIG12
		iVal = ENCAP_TO_HS(CONNMGR_ATTR_CPU_UTILIZATION);
#else
		iVal = ENCAP_TO_HS(CONNMGR_ATTR_CONNECTION_TIMEOUTS);
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** connmgrSendInstanceAttrAll( )
**
** GetAttributeAll service for Connection Manager instance is received
**---------------------------------------------------------------------------
*/
static void connmgrSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;

	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL,
					(UINT16)(sizeof(UINT16)*8));

	pData = pRequest->cfg.objectCfg.pData;

	/* open requests */
	UINT16_SET(pData, connmgrStats.iOpenRequests);
	pData += sizeof(UINT16);
	/* open format rejects*/
	UINT16_SET(pData, connmgrStats.iOpenFormatRejects);
	pData += sizeof(UINT16);
	/* open resource rejects */
	UINT16_SET(pData, connmgrStats.iOpenResourceRejects);
	pData += sizeof(UINT16);
	/* open other rejects */
	UINT16_SET(pData, connmgrStats.iOpenOtherRejects);
	pData += sizeof(UINT16);
	/* close requests */
	UINT16_SET(pData, connmgrStats.iCloseRequests);
	pData += sizeof(UINT16);
	/* close format rejects*/
	UINT16_SET(pData, connmgrStats.iCloseFormatRejects);
	pData += sizeof(UINT16);
	/* close other requests */
	UINT16_SET(pData, connmgrStats.iCloseOtherRejects);
	pData += sizeof(UINT16);
	/* connection timeouts */
	UINT16_SET(pData, connmgrStats.iConnectionTimeouts);
}

/*--------------------------------------------------------------------------------
** connmgrSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Connection Manager object instance
**--------------------------------------------------------------------------------
*/
static void connmgrSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iVal;
	UINT16 iTagSize;
	UINT32 iAttribute;
#ifdef CONNECTION_STATS
	UINT32 lVal32;
#endif

	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch (iAttribute)
	{
	case CONNMGR_ATTR_OPEN_REQUESTS:
		iVal = ENCAP_TO_HS(connmgrStats.iOpenRequests);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_OPEN_FORMAT_REJECTS:
		iVal = ENCAP_TO_HS(connmgrStats.iOpenFormatRejects);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_OPEN_RESOURCE_REJECTS:
		iVal = ENCAP_TO_HS(connmgrStats.iOpenResourceRejects);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_OPEN_OTHER_REJECTS:
		iVal = ENCAP_TO_HS(connmgrStats.iOpenOtherRejects);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_CLOSE_REQUEST:
		iVal = ENCAP_TO_HS(connmgrStats.iCloseRequests);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_CLOSE_FORMAT_REJECTS:
		iVal = ENCAP_TO_HS(connmgrStats.iCloseFormatRejects);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_CLOSE_OTHER_REJECTS:
		iVal = ENCAP_TO_HS(connmgrStats.iCloseOtherRejects);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case CONNMGR_ATTR_CONNECTION_TIMEOUTS:
		iVal = ENCAP_TO_HS(connmgrStats.iConnectionTimeouts);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
#ifdef EIP_BIG12
	case CONNMGR_ATTR_CPU_UTILIZATION:
		iVal = ENCAP_TO_HS(platformGetCPUUtilization());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
#endif
#ifdef CONNECTION_STATS
	case CONNMGR_ATTR_IO_PPS:
		lVal32 = ENCAP_TO_HS(connectionTotalIOPPS());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal32, sizeof(UINT32) );
		break;
	case CONNMGR_ATTR_EXPLICIT_PPS:
		lVal32 = ENCAP_TO_HS(connectionTotalExplicitPPS());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal32, sizeof(UINT32) );
		break;
		break;
	case CONNMGR_ATTR_MISSED_IO_PACKETS:
		lVal32 = ENCAP_TO_HS(connectionTotalLostIOPackets());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal32, sizeof(UINT32) );
		break;
	case CONNMGR_ATTR_IO_CONN_COUNT:
		lVal32 = ENCAP_TO_HS(gSystemStats.systemStats.iNumCurrentIOConnections);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal32, sizeof(UINT32) );
		break;
	case CONNMGR_ATTR_EXPLICIT_CONN_COUNT:
		lVal32 = ENCAP_TO_HS(gSystemStats.systemStats.iNumCurrentExplicitConnections);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal32, sizeof(UINT32) );
		break;
#endif

	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** connmgrSetInstanceAttrAll( )
**
** SetAttributeAll service for Connection Manager instance is received.
**---------------------------------------------------------------------------
*/
static void connmgrSetInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;

	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize < CONNMGR_STATS_SIZE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	if (pRequest->cfg.objectCfg.iDataSize > CONNMGR_STATS_SIZE)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	connmgrStats.iOpenRequests = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iOpenFormatRejects = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iOpenResourceRejects = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iOpenOtherRejects = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iCloseRequests = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iCloseFormatRejects = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iCloseOtherRejects = UINT16_GET(pData);
	pData += sizeof(UINT16);
	connmgrStats.iConnectionTimeouts = UINT16_GET(pData);
	pData += sizeof(UINT16);

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*--------------------------------------------------------------------------------
** connmgrSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Connection Manager object instance
**--------------------------------------------------------------------------------
*/
static void connmgrSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iTagSize;
	UINT32 iAttribute;
	UINT8* pData;

	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	pData = pRequest->cfg.objectCfg.pData;

	switch (iAttribute)
	{
	case CONNMGR_ATTR_OPEN_REQUESTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iOpenRequests = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_OPEN_FORMAT_REJECTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iOpenFormatRejects = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_OPEN_RESOURCE_REJECTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iOpenResourceRejects = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_OPEN_OTHER_REJECTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iOpenOtherRejects = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_CLOSE_REQUEST:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iCloseRequests = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_CLOSE_FORMAT_REJECTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iCloseFormatRejects = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_CLOSE_OTHER_REJECTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iCloseOtherRejects = UINT16_GET(pData);
		break;
	case CONNMGR_ATTR_CONNECTION_TIMEOUTS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		connmgrStats.iConnectionTimeouts = UINT16_GET(pData);
		break;
#ifdef CONNECTION_STATS
	case CONNMGR_ATTR_MISSED_IO_PACKETS:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT32))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		else if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT32))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		if (UINT32_GET(pData) != 0)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
			break;
		}

		connectionClearLostIOPackets();
		break;
#endif
#ifdef EIP_BIG12
	case CONNMGR_ATTR_CPU_UTILIZATION:
#endif
#ifdef CONNECTION_STATS
	case CONNMGR_ATTR_IO_PPS:
	case CONNMGR_ATTR_EXPLICIT_PPS:
	case CONNMGR_ATTR_IO_CONN_COUNT:
	case CONNMGR_ATTR_EXPLICIT_CONN_COUNT:
#endif
#if defined CONNECTION_STATS || defined EIP_BIG12
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		break;
#endif
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

#ifndef EIP_NO_CIP_ROUTING
/*---------------------------------------------------------------------------
** connmgrProcessUnconnectedSend( )
**
** Parse Unconnected Send request 
**---------------------------------------------------------------------------
*/
static void connmgrProcessUnconnectedSend( REQUEST* pRequest )
{
	EPATH pdu;
	INT32 nPDUSize;
	UINT32 lPort;
	ETAG_DATATYPE pAddr;
	UINT16 iTagSize, iSlot = 0xFFFF;
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	INT32 nMsgSize;
	INT32 nPathSize;
	REQUEST* pNewRequest;

	if ( pRequest->cfg.objectCfg.iDataSize < sizeof(UINT32) )
	{
		pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	pduInit( &pdu, TRUE );

	pNewRequest = requestNewIncoming(UNCONNECTED_SEND_REQUEST,
										FALSE,
										pRequest->nSessionId);	/* Schedule new Unconnected Send request */

	if ( pNewRequest == NULL)
	{
		/* No more space for requests */
		notifyEvent( NM_PENDING_REQUESTS_LIMIT_REACHED, 0, 0 );
		return;
	}

	pNewRequest->nFromSessionId = pRequest->nSessionId;
	pNewRequest->lFromIPAddress = pRequest->lIPAddress;

	pNewRequest->lContext1 = pRequest->lContext1;
	pNewRequest->lContext2 = pRequest->lContext2;

	pNewRequest->bOpenPriorityTickTime = *pData++;
	pNewRequest->bOpenTimeoutTicks = *pData++;

	/* Set the request timeout tick */
	pNewRequest->lRequestTimeoutTick = gdwTickCount + (pNewRequest->bOpenTimeoutTicks << (pNewRequest->bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));

	nMsgSize = UINT16_GET(pData);
	pData += sizeof(UINT16);

	pNewRequest->cfg.objectCfg.bService = *pData;

	/* Parse the next part of the packet that must be Router PDU (Protocol Data Unit) */
	nPDUSize = pduParse( pData, &pNewRequest->cfg.objectCfg.PDU, TRUE, pRequest->cfg.objectCfg.iDataSize-2);
	if ( nPDUSize == 0 )
	{
		requestRemove( pNewRequest );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		return;
	}

	pData += nPDUSize;
	pNewRequest->cfg.objectCfg.iDataSize = (UINT16)(nMsgSize - nPDUSize);
	pNewRequest->cfg.objectCfg.pData = EtIP_malloc( pData, pNewRequest->cfg.objectCfg.iDataSize );

	if ( pNewRequest->cfg.objectCfg.iDataSize && pNewRequest->cfg.objectCfg.pData == NULL )
	{
		requestRemove( pNewRequest );
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	pData += pNewRequest->cfg.objectCfg.iDataSize;

	/* Skip the pad byte if data length is odd */
	if ( pNewRequest->cfg.objectCfg.iDataSize % 2 )
		pData++;

	nPathSize = UINT16_GET(pData) * 2;						/* Path size is quoted in words */
	*(pData+1) = (UINT8)(PLATFORM_MIN(nPathSize/2, 2));		/* Make it parse only the first path segment */

	nPDUSize = (INT32)pduParse( pData, &pdu, TRUE, (UINT32)(pRequest->cfg.objectCfg.iDataSize-(pData-pRequest->cfg.objectCfg.pData)));
	if ( nPDUSize == 0 )
	{
		requestRemove( pNewRequest );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		return;
	}

	pNewRequest->lIPAddress = 0;

	/* Check for any failure to initialize the pdu structure */
	if ( pdu.bNumTags > MAX_TAGS )
	{
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequest->lIPAddress, pRequest->lHostIPAddr,
			"connmgrProcessUnconnectedSend Internal Logic FAILURE pdu.bNumTags set to %d for request index %d",
			pdu.bNumTags, pRequest->nIndex );
		pdu.bNumTags = 0;
	}

	/* Check the port to see if we support it */
	lPort = (UINT32)pduGetTagByType( &pdu, TAG_TYPE_PORT, 0, &iTagSize );
	if ((iTagSize == 0) ||
		(lPort < TCP_IP_STARTING_PORT_NUMBER) || 
#ifdef ET_IP_MODBUS
		( (lPort > (TCP_IP_STARTING_PORT_NUMBER+(gnClaimedHostIPAddrCount*2)-1))
#ifdef ET_IP_MODBUS_SERIAL
		 && (lPort < MODBUS_SL_STARTING_PORT_NUMBER)) 
		 || (lPort > (UINT32)((MODBUS_SL_STARTING_PORT_NUMBER + ((gpnClaimedHostCOMPorts-galClaimedHostCOMPorts)-1)))
#endif
		)
#else
		(lPort > (TCP_IP_STARTING_PORT_NUMBER+gnClaimedHostIPAddrCount-1))
#endif
		)
	{
		if ((gbBackplaneSupport == TRUE) && (lPort == TCP_IP_BACKPLANE_PORT_NUMBER))
		{
			/* Get the slot number */
			iSlot = (UINT16)pduGetTagByType( &pdu, TAG_TYPE_SLOT, 0, &iTagSize );
			if (iTagSize != 0)
			{
				if (giBackplaneSlot == iSlot)
				{
					/* This is a request to ourselves */
					pNewRequest->nType = OBJECT_REQUEST;
					routerProcessObjectRequest(pNewRequest);
				}
				else
				{
					/* Get the extended path */
					pNewRequest->iExtendedPathSize = (UINT16)nPDUSize;
					pNewRequest->pExtendedPath = EtIP_malloc(pData, pNewRequest->iExtendedPathSize );

					pNewRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
					notifyEvent( NM_BACKPLANE_REQUEST_RECEIVED, pNewRequest->nIndex, 0 );
				}
			}

			pduRelease( &pdu );
			requestRemove( pRequest );
			return;
		}

		pduRelease( &pdu );
		requestRemove( pNewRequest );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	pData += nPDUSize;

	/* Assign the host addressed based on the port */
#ifdef ET_IP_MODBUS
	pNewRequest->lHostIPAddr = gsTcpIpInstAttr[(lPort-TCP_IP_STARTING_PORT_NUMBER)/2].InterfaceConfig.lIpAddr;
#else
	pNewRequest->lHostIPAddr = gsTcpIpInstAttr[lPort-TCP_IP_STARTING_PORT_NUMBER].InterfaceConfig.lIpAddr;
#endif


#ifdef ET_IP_MODBUS_SERIAL
	if ((lPort >= MODBUS_SL_STARTING_PORT_NUMBER) &&
		(lPort < (UINT32)((MODBUS_SL_STARTING_PORT_NUMBER + ((gpnClaimedHostCOMPorts-galClaimedHostCOMPorts)-1)))))
	{
		iSlot = (UINT16)pduGetTagByType( &pdu, TAG_TYPE_SLOT, 0, &iTagSize );
		if (iTagSize == 0)
		{
			pduRelease( &pdu );
			requestRemove( pNewRequest );
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
			return;
		}

		if ((iSlot == 0) || (iSlot > 0xFF))
		{
			pduRelease( &pdu );
			requestRemove( pNewRequest );
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
			return;
		}

		pNewRequest->bModbusSerialAddress = (UINT8)iSlot;
	}
	else
	{
#endif
		/* Get the first IP address */
		pAddr = pduGetTagByType( &pdu, TAG_TYPE_LINK_ADDRESS, 0, &iTagSize );
		if (iTagSize == 0)
		{
			pduRelease( &pdu );
			requestRemove( pNewRequest );
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
			return;
		}

		pNewRequest->lIPAddress = platformAddrFromPath( (const char*)pAddr, (INT32)iTagSize );
#ifdef ET_IP_MODBUS_SERIAL
	}
#endif

#ifdef ET_IP_MODBUS
#ifdef ET_IP_MODBUS_SERIAL

	if ((lPort >= MODBUS_SL_STARTING_PORT_NUMBER) &&
		(lPort < (UINT32)((MODBUS_SL_STARTING_PORT_NUMBER + ((gpnClaimedHostCOMPorts-galClaimedHostCOMPorts)-1)))))
	{
		if (iSlot != 0xFFFF)
		{
			if ( nPathSize > (nPDUSize - 2) )
			{
				/* Can't have any routing information after serial address 
				   because Modbus has no way to route it */
				pduRelease( &pdu );
				requestRemove( pNewRequest );
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
				return;
			}
			
			eipmbsParseCIPModbusRequest(pNewRequest);
		}
	}
	else
#endif /* ET_IP_MODBUS_SERIAL */
	if (lPort%2)	/* Modbus port are odd */
	{
		if ( pNewRequest->lIPAddress )
		{
			if ( nPathSize > (nPDUSize - 2) )
			{
				pNewRequest->iExtendedPathSize = (UINT16)(nPathSize - nPDUSize + 2);
				pNewRequest->pExtendedPath = EtIP_malloc( pData, pNewRequest->iExtendedPathSize );

				/* verify the extended path */
				if (pNewRequest->iExtendedPathSize != 2)
				{
					pduRelease( &pdu );
					requestRemove( pNewRequest );
					pRequest->bGeneralError = ROUTER_ERROR_BAD_PATH_SIZE;
					return;
				}

				/* Ensure port 1 is used to route to ModbusTCP -> ModbusSL */
				if (*pNewRequest->pExtendedPath != MODBUS_TCP_TO_SL_PORT_NUMBER)
				{
					pduRelease( &pdu );
					requestRemove( pNewRequest );
					pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
					return;
				}
			}
			else
				pNewRequest->iExtendedPathSize = 0;

			eipmbsParseCIPModbusRequest(pNewRequest);
		}
	}
	else
	{
#endif /* ET_IP_MODBUS */

		if ( pNewRequest->lIPAddress )
		{
			if (pNewRequest->lIPAddress == pNewRequest->lHostIPAddr)
			{
				/* This is a request to ourselves */
				pNewRequest->nType = OBJECT_REQUEST;
				routerProcessObjectRequest(pNewRequest);
			}
			else
			{
#ifndef EIP_NO_ORIGINATE_UCMM
				if ( nPathSize > (nPDUSize - 2) )
				{
					pNewRequest->iExtendedPathSize = (UINT16)(nPathSize - nPDUSize + 2);
					pNewRequest->pExtendedPath = EtIP_malloc( pData, pNewRequest->iExtendedPathSize );
				}
				else
					pNewRequest->iExtendedPathSize = 0;

				pNewRequest->nState = REQUEST_LOGGED;
#else
				pduRelease( &pdu );
				requestRemove( pNewRequest );
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
				return;
#endif
			}
		}
#ifdef ET_IP_MODBUS
	}
#endif

	pduRelease( &pdu );
	requestRemove( pRequest );
}
#endif

/*---------------------------------------------------------------------------
** connmgrGetConnectionOwner( )
**
** Parse Get Connection Owner request
**---------------------------------------------------------------------------
*/
static void connmgrGetConnectionOwner( REQUEST* pRequest )
{
	UINT8 bPathSize, bNumConnections = 0, bNumCOOConnections = 0,
			bNumROOConnections = 0, bLastAction = LAST_ACTION_NO_OWNER;
	UINT16 iPDUParseSize, iOwnerConnSerialNumber = 0, iOwnerOrigVendorID = 0;
	UINT32 lOwnerOrigSerialNumber = 0;
	CONNECTION_CFG ConnConfig;
	CONNECTION* pConnection;
	UINT8* pData;

	if (pRequest->cfg.objectCfg.iDataSize < 2)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
	
	/* First byte shall be 0 */
	if (*pRequest->cfg.objectCfg.pData != 0)
	{
		pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	bPathSize = pRequest->cfg.objectCfg.pData[1];

	/* Parse out the connection path */
	pduInit(&ConnConfig.PDU, TRUE);
	iPDUParseSize = (UINT16)pduParse(pRequest->cfg.objectCfg.pData, &ConnConfig.PDU, TRUE, pRequest->cfg.objectCfg.iDataSize);

	if (bPathSize*2 < (iPDUParseSize-PDU_HDR_SIZE))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pduRelease(&ConnConfig.PDU);
		return;
	}

	if (bPathSize*2 > (iPDUParseSize-PDU_HDR_SIZE))
	{
		pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pduRelease(&ConnConfig.PDU);
		return;
	}

	/* Initialize the "tags" as they may be part of the connection path */
	ConnConfig.iTargetConsumingConnTagSize = 0;
	ConnConfig.iTargetProducingConnTagSize = 0;

	connectionParseConnectionPath(&ConnConfig);

	/* Free the tags as they have been associated with an assembly instance */
	if (ConnConfig.iTargetConsumingConnTagSize > 0)
		EtIP_free(&ConnConfig.targetConsumingConnTag, &ConnConfig.iTargetConsumingConnTagSize);
	if (ConnConfig.iTargetProducingConnTagSize > 0)
		EtIP_free(&ConnConfig.targetProducingConnTag, &ConnConfig.iTargetProducingConnTagSize);


	/* Got all of the information, now find the connections that match */
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ ) 
	{
		if ((connectionIsIOConnection(pConnection)) &&
			(pConnection->cfg.bOriginator == FALSE) &&
			(pConnection->cfg.bRedundantOwner == TRUE) &&
			(pConnection->cfg.iTargetConfigConnInstance == ConnConfig.iTargetConfigConnInstance) &&
			(pConnection->cfg.iTargetConsumingConnPoint == ConnConfig.iTargetConsumingConnPoint) &&
			(pConnection->cfg.iTargetProducingConnPoint == ConnConfig.iTargetProducingConnPoint) &&
			(pConnection->lConnectionState == ConnectionEstablished))
		{
			bNumConnections++;

			if (pConnection->cfg.bClaimOutputOwnership > 0)
				bNumCOOConnections++;

			if (pConnection->cfg.bReadyOwnershipOutputs > 0)
				bNumROOConnections++;

			if (pConnection->bIsRedundantMaster)
			{
				bLastAction = (UINT8)((pConnection->bRunIdleFlag == TRUE) ? LAST_ACTION_RUN : LAST_ACTION_IDLE);
				iOwnerConnSerialNumber = pConnection->iConnectionSerialNbr;
				iOwnerOrigVendorID = pConnection->iVendorID;
				lOwnerOrigSerialNumber = pConnection->lDeviceSerialNbr;
			}
		}
	}

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 4+sizeof(UINT16)*2+sizeof(UINT32));
	if (pRequest->cfg.objectCfg.pData == NULL)
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;
	*pData = bNumConnections;
	pData++;
	*pData = bNumCOOConnections;
	pData++;
	*pData = bNumROOConnections;
	pData++;
	*pData = bLastAction;
	pData++;
	UINT16_SET(pData, iOwnerConnSerialNumber);
	pData += sizeof(UINT16);
	UINT16_SET(pData, iOwnerOrigVendorID);
	pData += sizeof(UINT16);
	UINT32_SET(pData, lOwnerOrigSerialNumber);
}

/*---------------------------------------------------------------------------
** connmgrParseClassRequest( )
**
** Respond to the class request
**---------------------------------------------------------------------------
*/
static void connmgrParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		connmgrSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		connmgrSendClassAttrSingle( pRequest);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** connmgrParseInstanceRequest( )
**
** Respond to the instance request
**---------------------------------------------------------------------------
*/
static BOOL connmgrParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	BOOL bInternal = TRUE;

	if ( iInstance != 1 )
	{
		/* only Instance 1 is supported at this moment */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		connmgrSendInstanceAttrAll( pRequest, iInstance);
		break;

	case SVC_GET_ATTR_SINGLE:
		connmgrSendInstanceAttrSingle( pRequest, iInstance );
		break;

	case SVC_SET_ATTR_ALL:
		connmgrSetInstanceAttrAll( pRequest, iInstance);
		break;

	case SVC_SET_ATTR_SINGLE:
		connmgrSetInstanceAttrSingle( pRequest, iInstance );
		break;

	case FWD_OPEN_CMD_CODE:
		bInternal = connmgrIncomingConnection( pRequest, FALSE );
		break;

	case LARGE_FWD_OPEN_CMD_CODE:
		bInternal = connmgrIncomingConnection( pRequest, TRUE );
		break;

	case FWD_CLOSE_CMD_CODE:
		connmgrProcessFwdClose( pRequest );
		break;

#ifndef EIP_NO_CIP_ROUTING
	case UNCONNECTED_SEND_CMD_CODE:
		connmgrProcessUnconnectedSend( pRequest );
		break;
#endif
	case GET_CONNECTION_OWNER_CODE:
		connmgrGetConnectionOwner( pRequest );
		break;

	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}

	return bInternal;
}

/*---------------------------------------------------------------------------
** connmgrParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/

BOOL connmgrParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT32 iInstance;
	UINT16 iTagSize;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ( iTagSize == 0 )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	if ( iInstance == 0 )
	{
		connmgrParseClassRequest( pRequest );
		return TRUE;
	}

	return connmgrParseInstanceRequest( pRequest, iInstance );
}

/*---------------------------------------------------------------------------
** connmgrReset( )
**
** Reset connection counters
**---------------------------------------------------------------------------
*/
void connmgrReset(UINT8 bReset)
{
	if (bReset > 0)
	{
		memset(&connmgrStats, 0, sizeof(CONNMGR_STATS));
	}
}
