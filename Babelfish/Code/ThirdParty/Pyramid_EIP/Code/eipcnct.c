/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCNCT.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** This module contains the implementation of the Connection collection
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef EIP_LARGE_BUF_ON_HEAP
CONNECTION* gConnections;
#else
CONNECTION  gConnections[MAX_CONNECTIONS];
#endif
CONNECTION* gpnConnections;
#ifdef CONNECTION_STATS
INTERNAL_STATS gSystemStats;
#endif

INT32  gnKeepTCPOpenForActiveCorrespondingUDP;		/* A global variable that will be used if the same variable is not set for a particular connection */
UINT32 gHeartbeatConnPoint;
UINT32 gListenOnlyConnPoint;
#ifndef EIP_NSLED_PER_IP
EtIPNetworkStatus geSavedNetworkStatus;				/* Current Network LED status (used to detect state changes) */
#endif

/*---------------------------------------------------------------------------
** connectionInit( )
**
** Initialize connection array
**---------------------------------------------------------------------------
*/
void connectionInit(void)
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	gConnections = (CONNECTION *)EtIP_mallocHeap((UINT16)(MAX_CONNECTIONS * sizeof(CONNECTION )));
	if (gConnections == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gConnections for %d entries\n", MAX_CONNECTIONS);
		return;
	}
#endif

	gpnConnections = gConnections;
#ifdef CONNECTION_STATS
	memset(&gSystemStats, 0, sizeof(gSystemStats));
#endif

	gnKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateTrue;
	gHeartbeatConnPoint = HEARTBEAT_CONN_POINT;
	gListenOnlyConnPoint = LISTEN_ONLY_CONN_POINT;
#ifdef EIP_NSLED_PER_IP
	geSavedNetworkStatus = NetworkStatusOff;
#endif
}

/*---------------------------------------------------------------------------
** connectionInitialize( )
**
** Initialize connection object
**---------------------------------------------------------------------------
*/
static void connectionInitialize( CONNECTION* pConnection )
{
	memset( (void *)pConnection, 0, sizeof(CONNECTION) );

	pConnection->lConnectionState = ConnectionNonExistent;

	pConnection->lClass1Socket = (SOCKET)INVALID_SOCKET;
	pConnection->bRunIdleFlag = UnknownRunIdleFlag;

	pConnection->iInDataSeqNbr = 0xffff;
	pConnection->iOutDataSeqNbr = 1;

	pConnection->nKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateUnknown;

	pduInit( &pConnection->cfg.PDU, TRUE );
	pduInit( &pConnection->requestCfg.PDU, TRUE );

	pConnection->sReceiveAddr.sin_family =
	pConnection->sTransmitAddr.sin_family = AF_INET;
}

/*---------------------------------------------------------------------------
** connectionNew( )
**
** Add new connection to the connection array
**---------------------------------------------------------------------------
*/
CONNECTION* connectionNew(void)
{
	CONNECTION* pConnection;

	if ( gpnConnections >= &gConnections[MAX_CONNECTIONS] )
		return NULL;

	pConnection = gpnConnections++;

	connectionInitialize( pConnection );

	pConnection->cfg.iWatchdogTimeoutAction = TimeoutDelayAutoReset;
	pConnection->cfg.lWatchdogTimeoutReconnectDelay = DEFAULT_WATCHDOG_TIMEOUT_RECONNECT_DELAY;

	return pConnection;
}

/*---------------------------------------------------------------------------
** connectionRemoveAll( )
**
** Clean up connection array
**---------------------------------------------------------------------------
*/
void connectionRemoveAll(void)
{
	CONNECTION* pConnection;

	for( pConnection = gpnConnections-1; pConnection >= gConnections; pConnection-- )
	{
		connectionRemove( pConnection, FALSE );
	}

	gpnConnections = gConnections;
}

#ifndef EIP_LARGE_CONFIG_OPT
/*---------------------------------------------------------------------------
** connectionIsTimedOut( )
**
** Return TRUE if connection is timed out, FALSE otherwise
**---------------------------------------------------------------------------
*/
static BOOL connectionIsTimedOut( CONNECTION* pConnection )
{
	if ((pConnection->cfg.lConsumingDataRate > 0) &&
		(IS_TICK_GREATER(gdwTickCount, pConnection->lTimeoutTick)))
	{
		DumpStr3(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"connectionIsTimedOut TIMED OUT Connection %d CurrentTick %d Timing out at %d",
			pConnection->cfg.nInstance, gdwTickCount, pConnection->lTimeoutTick);
		return TRUE;
	}
	else
		return FALSE;
}

/*---------------------------------------------------------------------------
** connectionService( )
**
** Service a particular connection. Return TRUE if connection was removed
** If data was sent, pDataSend becomes true
**---------------------------------------------------------------------------
*/
BOOL connectionService( CONNECTION* pConnection, BOOL* pDataSent )
{
	CONNECTION* pMulticastProducer = NULL;
	BOOL bConnectionRemove;
#ifdef ET_IP_SCANNER
	SESSION* pSession = NULL;
#endif

	/* No data sent yet */
	*pDataSent = FALSE;

	if ( pConnection->lConfigurationState == ConfigurationAbortConnection )
	{
		connectionRemove( pConnection, FALSE);
		return TRUE;
	}

	/* Check if this connection was scheduled for closing */
	if (pConnection->lConfigurationState == ConfigurationClosing &&
		pConnection->lConnectionState != ConnectionClosing )
	{
		/* If we are the ones who initiate a connection drop - issue Forward Close */
		if ( pConnection->cfg.bOriginator && pConnection->lConnectionState == ConnectionEstablished )
		{
			pConnection->lStartTick = gdwTickCount;
			pConnection->lConnectionState = ConnectionClosing;
		}
		else
		{
			connectionRemove( pConnection, FALSE );
			return TRUE;
		}
	}

	switch( pConnection->lConnectionState )
	{
#ifdef ET_IP_SCANNER
	case ConnectionConfiguring:
	{
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationLogged:
		{
			/* If incoming connection scheduled by CC object, but not opened yet - leave it alone */
			if ( !pConnection->cfg.bOriginator )
				return FALSE;

			/* Check if there is a delay on opening a connection  or
			   if another connection is being configured on the same IP address. If Yes, wait until it's finished. */
			if ( IS_TICK_GREATER( pConnection->lStartTick, gdwTickCount ) ||
				connectionAnotherPending( pConnection ) )
				return FALSE;

				/* Check if the session with this server has already been established */
					pSession = sessionFindOutgoing( pConnection->cfg.lIPAddress, pConnection->cfg.lHostIPAddr, FALSE);

					if (pSession == NULL )
					{
						pSession = sessionNew( pConnection->cfg.lIPAddress, pConnection->cfg.lHostIPAddr, FALSE, FALSE );

						if ( pSession == NULL )
						{
							/* Maximum number of open sessions has been reached */
							connectionRemove( pConnection, FALSE );
							notifyEvent(NM_SESSION_COUNT_LIMIT_REACHED, 0, 0);
							return TRUE;
						}

						DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
							"connectionService new session logged - Id %d", pSession->nSessionId);

						/* Session will timeout when ForwardOpen request times out */
						pSession->lWaitingForOpenSessionTimeout = gdwTickCount + (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));

						pConnection->nSessionId = pSession->nSessionId;
					}
					else
					{
						pConnection->nSessionId = pSession->nSessionId;
						if (pSession->lState == OpenSessionEstablished)
						{
							if ( scanmgrOutgoingConnection(pConnection ) )
							{
								pConnection->lConfigurationState = ConfigurationWaitingForForwardOpenResponse;
								pConnection->lConfigurationTimeoutTick = gdwTickCount + (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));
							}
							return FALSE;
						}
					}

				/* Session has not been successfully established yet */
				pConnection->lConfigurationState = ConfigurationWaitingForSession;
				pConnection->lConfigurationTimeoutTick = gdwTickCount + (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));
			}
			break;

			case ConfigurationWaitingForSession:
				pSession = sessionGetBySessionId( pConnection->nSessionId );

				/* If the session has failed to open or it takes too long for it to open */
				if (((pSession == NULL))
					|| IS_TICK_GREATER( gdwTickCount, pConnection->lConfigurationTimeoutTick ) )
				{
					pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
					gSystemStats.systemStats.iNumFailedConnections++;
					pConnection->iNumErrors++;
#endif
					notifyEvent( NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH, pConnection->cfg.nInstance, 0 );
					return connectionTimedOut(pConnection);
				}
				else if (((pSession != NULL) && (pSession->lState == OpenSessionEstablished)))
				{
					if ( scanmgrOutgoingConnection(pConnection ) )
					{
						pConnection->lConfigurationState = ConfigurationWaitingForForwardOpenResponse;
						pConnection->lConfigurationTimeoutTick = gdwTickCount + (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));
					}
				}
			break;

			case ConfigurationWaitingForForwardOpenResponse:
			{
				if ( IS_TICK_GREATER( gdwTickCount, pConnection->lConfigurationTimeoutTick ) )
				{
					pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
					gSystemStats.systemStats.iNumFailedConnections++;
					pConnection->iNumErrors++;
#endif
					notifyEvent( NM_CONN_CONFIG_FAILED_NO_RESPONSE, pConnection->cfg.nInstance, 0 );
					return connectionTimedOut( pConnection);
				}
			}
			break;

			default:
				break;
			}
		}
		break;
#endif /* ET_IP_SCANNER */
	case ConnectionEstablished:
	{
		/* Check if it's time to produce on connection or ChangeOfState */
		if (connectionIsIOConnection(pConnection) || pConnection->cfg.bOriginator)
		{
			if ( ((pConnection->cfg.lProducingDataRate > 0) && (IS_TICK_GREATER_OR_EQUAL(gdwTickCount, pConnection->lProduceTick))) ||
				( pConnection->bTransferImmediately && !(connectionIsIOConnection(pConnection) && IS_TICK_GREATER(pConnection->lInhibitExpireTick, gdwTickCount) ) ) )
			{
				if (connectionIsIOConnection(pConnection)) /* For using UDP packets to transport I/O */
				{
#ifdef ET_IP_MODBUS
					if ((pConnection->cfg.modbusCfg.iModbusConsumeSize == 0) &&
						(pConnection->cfg.modbusCfg.iModbusProduceSize == 0))
					{
#endif
						pMulticastProducer = connectionGetFirstMultiProducer( pConnection );
						ioSendIOPacket( pConnection, pMulticastProducer );
						connectionResetProducingTicks( pConnection );
#ifdef ET_IP_MODBUS
					}
					else
					{
						pMulticastProducer = connectionGetFirstMultiProducer( pConnection );
						eipmbsioSendReadRequest( pConnection, pMulticastProducer);
						connectionResetProducingTicks( pConnection );
					}
#endif
				}
#ifdef ET_IP_SCANNER
				else if ( pConnection->cfg.bTransportClass == Class3 && /* For Class 3 using TCP packets to transport object request */
							pConnection->cfg.bOriginator &&
							!pConnection->bOutstandingClass3Request)
				{
					ucmmSendConnectedRequest( pConnection );
					connectionResetProducingTicks( pConnection );
					pConnection->bOutstandingClass3Request = TRUE;
				}
#endif
				*pDataSent = TRUE;
				pConnection->bTransferImmediately = FALSE;
			}
		}

		if ( connectionIsTimedOut( pConnection ) )
		{
			pConnection->bGeneralStatus = ROUTER_ERROR_FAILURE;
			pConnection->iExtendedStatus = ROUTER_EXT_ERR_CONNECTION_TIMED_OUT;
#ifdef CONNECTION_STATS
			pConnection->iNumTimeouts++;
			gSystemStats.systemStats.iNumTimedOutConnections++;
#endif
			notifyEvent( NM_CONNECTION_TIMED_OUT, pConnection->cfg.nInstance, 0 );
			pConnection->bOutstandingClass3Request = FALSE;
			bConnectionRemove = connectionTimedOut( pConnection);
			connectionNotifyNetLEDChange();
			return bConnectionRemove;
		}

#ifdef CONNECTION_STATS
		if ( pConnection->bCollectingStats == FALSE &&
			IS_TICK_GREATER( gdwTickCount, pConnection->lStartCollectStatsTick ) )
		{
			pConnection->bCollectingStats = TRUE;
			pConnection->lPPSResetTick = gdwTickCount; /* Reset count ASAP */
		}

		if ( pConnection->bCollectingStats )
		{
			if (IS_TICK_GREATER_OR_EQUAL( gdwTickCount, pConnection->lPPSResetTick ))
			{
				pConnection->lRcvdPPSCountPrev = pConnection->lRcvdPPSCount;
				pConnection->lRcvdPPSCount = 0;
				pConnection->lSendPPSCountPrev = pConnection->lSendPPSCount;
				pConnection->lSendPPSCount = 0;
				pConnection->lPPSResetTick = gdwTickCount + 1000;
			}
		}
#endif /*CONNECTION_STATS */
	}
	break;
#ifdef ET_IP_SCANNER
	case ConnectionClosing:
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationClosing:

			/* Check if there is a delay on closing a connection  or
			   if another connection is being configured on the same IP address. If Yes, wait until it's finished. */
			if ( IS_TICK_GREATER( pConnection->lStartTick, gdwTickCount ) ||
				connectionAnotherPending( pConnection ) )
				return FALSE;

			pConnection->lConfigurationTimeoutTick = gdwTickCount + FORWARD_CLOSE_TIMEOUT;
			pConnection->bOutstandingClass3Request = FALSE;
			scanmgrIssueFwdClose( pConnection );
			pConnection->lConfigurationState = ConfigurationWaitingForForwardCloseResponse;
			break;

		case ConfigurationWaitingForForwardCloseResponse:
		case ConfigurationClosingTimeOut:
			if ( IS_TICK_GREATER(gdwTickCount, pConnection->lConfigurationTimeoutTick ) )
			{
				/* Forward Close response did not come - close now */
				connectionRemove( pConnection, FALSE );
				return TRUE;
			}
			break;

		default:
			break;
		}
		break;
#endif

	default:
		break;
	}

	return FALSE;
}
#endif /* EIP_LARGE_CONFIG_OPT */

/*---------------------------------------------------------------------------
** connectionTimedOut( )
**
** The peer stopped communicating. Process based on the iWatchdogTimeoutAction
** member value.
** Return TRUE if pConnection is removed.
**---------------------------------------------------------------------------
*/
BOOL connectionTimedOut(CONNECTION* pConnection)
{ 
	/* increment number of connection timeouts */
	connmgrStats.iConnectionTimeouts++;
#ifdef CONNECTION_STATS
	pConnection->bCollectingStats = FALSE;
#endif

#ifdef EIP_LARGE_CONFIG_OPT
	connListClearCnxnLists(pConnection);  /* cancel any active timers or polls for the connection */
	connListRemoveCnxnIdTableEntry(pConnection->lConsumingCID, pConnection->cfg.lIPAddress, pConnection->cfg.nInstance);
#endif

	/* if we're in the process of closing, don't mess with the timeout action, otherwise
	 * we may end up erasing the close state and reopening the connection */
	if (pConnection->lConfigurationState == ConfigurationClosing)
	{
		pConnection->lConnectionState = ConnectionTimedOut;
		connListSetConnectionConfigState(pConnection, ConfigurationClosing, 0);
		return FALSE;
	}

	switch( pConnection->cfg.iWatchdogTimeoutAction )
	{
	case TimeoutAutoDelete:
	case TimeoutDeferredDelete:
#ifdef ET_IP_SCANNER
		if (pConnection->cfg.bOriginator)
		{
			/* Give some time to the client to get error info */
			pConnection->lConnectionState = ConnectionClosing;
			pConnection->lConfigurationState = ConfigurationClosingTimeOut;
#ifdef EIP_LARGE_CONFIG_OPT
			connListQueueCnxnTimer(pConnection, CNXN_TIMER_TYPE_CFG_TIMEOUT, FORWARD_OPEN_ERROR_TIMEOUT);
#else
			pConnection->lConfigurationTimeoutTick = gdwTickCount + FORWARD_OPEN_ERROR_TIMEOUT;
#endif
		}
		/* Target connection to scanner assemblies */
		else if (pConnection->bCCConfigured)
		{
			connectionGoOffline( pConnection, FALSE );
			pConnection->lConnectionState = ConnectionConfiguring;
			connListSetConnectionConfigState(pConnection, ConfigurationLogged, 0);
		}
		else
		{
#endif 
			connectionRemove(pConnection, TRUE);
			return TRUE;
#ifdef ET_IP_SCANNER
		}
		break;
#endif 

	case TimeoutAutoReset:
		connectionGoOffline( pConnection, TRUE );
		pConnection->lConnectionState = ConnectionConfiguring;
		connListSetConnectionConfigState(pConnection, ConfigurationLogged, 0);
		break;

	case TimeoutDelayAutoReset:
		connectionGoOffline( pConnection, TRUE );
		pConnection->lConnectionState = ConnectionConfiguring;
		connListSetConnectionConfigState(pConnection, ConfigurationLogged, pConnection->cfg.lWatchdogTimeoutReconnectDelay);
		break;

	case TimeoutManualReset:
		connectionGoOffline( pConnection, TRUE );
		pConnection->lConnectionState = ConnectionTimedOut;
		break;

	default:
		break;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** connectionAssignSocketConnGroup( )
**
** Assign a "socket group" for a class1 connection
**---------------------------------------------------------------------------
*/
static BOOL connectionAssignSocketConnGroup( CONNECTION* pConnection )
{
	UINT32 i;
	CONNECTION* pLoopConnection;
	INT32 iGroupCount = 0;

	for (i = 0; i < NUM_CONNECTION_GROUPS; i++)
	{
		for(pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++)
		{
			if ((pLoopConnection->lClass1SocketConnGroup == i) &&
				(pLoopConnection != pConnection))
			{
				iGroupCount++;
				if (iGroupCount > PLATFORM_MAX_CONNECTIONS_PER_SOCKET)
				{
					/* Try the next group */
					iGroupCount = 0;
					break;
				}
			}
		}

		if (pLoopConnection == gpnConnections)
		{
			/* Found an available group */
			pConnection->lClass1SocketConnGroup = i;
			return TRUE;
		}
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** connectionAssignClass1Socket( )
**
** Assign UDP socket based on the connection instance.
**---------------------------------------------------------------------------
*/
BOOL connectionAssignClass1Socket( CONNECTION* pConnection )
{
	SOCKET* pSocket;
	UINT8 nTTL;
#ifdef EIP_QOS
	INT32 lDSCPRet;
#endif

	/* find Host IPAddress index */
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = tcpipFindClaimedHostAddress(pConnection->cfg.lHostIPAddr);

	if ( pTcpipInterface == NULL )
	{
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"connectionAssignClass1Socket Host Address has not been claimed");
		return FALSE;
	}

	if (!connectionAssignSocketConnGroup(pConnection))
	{
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"connectionAssignClass1Socket could not assign class1 socket");
		return FALSE;
	}

	/* Grab the indexed socket */
	pSocket = &pTcpipInterface->alClass1Socket[pConnection->lClass1SocketConnGroup]; 

	if (*pSocket == (SOCKET)INVALID_SOCKET )
	{
		*pSocket = socketClass1Init( FALSE, COMMON_BUFFER_SIZE, pTcpipInterface->InterfaceConfig.lIpAddr,
								htons(pConnection->sReceiveAddr.sin_port),
								IS_MULTICAST(platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr)));

#ifdef EIP_QOS
		/* Set QoS based on connection producing priority */
		switch(pConnection->cfg.iProducingPriority)
		{
		case UrgentPriority:
			lDSCPRet = platformSetDSCPValue(*pSocket, pTcpipInterface->QoSConfig.bDSCPUrgent);
			break;
		case ScheduledPriority:
			lDSCPRet = platformSetDSCPValue(*pSocket, pTcpipInterface->QoSConfig.bDSCPScheduled);
			break;
		case HighPriority:
			lDSCPRet = platformSetDSCPValue(*pSocket, pTcpipInterface->QoSConfig.bDSCPHigh);
			break;
		case LowPriority:
			lDSCPRet = platformSetDSCPValue(*pSocket, pTcpipInterface->QoSConfig.bDSCPLow);
			break;
		default:
			lDSCPRet = SOCKET_ERROR;
			break;
		}

		if (lDSCPRet == SOCKET_ERROR)
		{
			notifyEvent(NM_ERROR_USING_WINSOCK, 0, 0);        
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, "connectionAssignClass1Socket error calling platformSetDSCPValue function");
		}
#endif
	}

	/* assign the socket to the corresponding connection */
	pConnection->lClass1Socket = *pSocket;

	/* Set multicast options for this new socket */
	if (pConnection->cfg.iProducingConnectionType == Multicast)
	{
		/* The multicast packet will be propagated for up to TTL_SUBNET_LEVEL subnetworks */
		nTTL = pTcpipInterface->InterfaceConfig.bTTLValue;
		platformSetSockOpt(pConnection->lClass1Socket, IPPROTO_IP, IP_MULTICAST_TTL, &nTTL, sizeof(nTTL));

		platformSetSockOpt(pConnection->lClass1Socket, IPPROTO_IP, IP_MULTICAST_IF, 
				(char *)&pConnection->cfg.lHostIPAddr, sizeof(pConnection->cfg.lHostIPAddr));
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** connectionReleaseClass1Socket( )
**
** Drop from the multicast group and close the socket if needed.
**---------------------------------------------------------------------------
*/
void connectionReleaseClass1Socket( CONNECTION* pConnection )
{
	INT32 i;
	SOCKET* pSocket;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = tcpipFindClaimedHostAddress(pConnection->cfg.lHostIPAddr);
	CONNECTION* pLoopConnection;

	/* Check if already been released */
	if ((pTcpipInterface == NULL) ||
		(connectionIsIOConnection(pConnection) == FALSE) ||
		(pConnection->lClass1Socket == (SOCKET)INVALID_SOCKET))
		return;

	/* Check if this was the only connection using this Class1 socket */
	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((pLoopConnection != pConnection) &&
			(pLoopConnection->lClass1Socket != INVALID_SOCKET) &&
			(pLoopConnection->lClass1Socket == pConnection->lClass1Socket) &&
			 (pLoopConnection->lConnectionState == ConnectionEstablished || pLoopConnection->lConnectionState == ConnectionClosing) )             
				break;
	}

	if ( pLoopConnection == gpnConnections )
	{
		/* This was the last open connection for this Host Address/Conn Group combination - close the socket */
#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
		/* Take us from the multicast receive list */
		if ( IS_MULTICAST(platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr)))
			socketDropMulticastGroup(pConnection);
#endif

		DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"Closing socket 0x%x", pConnection->lClass1Socket);

		platformCloseSocket(pConnection->lClass1Socket);

		/* Clear all connection groups of the socket */
		for(i = 0, pSocket = pTcpipInterface->alClass1Socket; 
			i < NUM_CONNECTION_GROUPS; 
			i++, pSocket++)
		{
			if (pConnection->lClass1Socket == *pSocket)
				*pSocket = (SOCKET)INVALID_SOCKET;
		}
	}

	pConnection->lClass1Socket = (SOCKET)INVALID_SOCKET;
}

/*---------------------------------------------------------------------------
** connectionGoOffline( )
**
** Outgoing connections are taken offline instead of removing them.
**---------------------------------------------------------------------------
*/

void connectionGoOffline( CONNECTION* pConnection, BOOL bTimeOut )
{
	REQUEST* pRequest;
	CONNECTION *pLoopConnection, *pROLoopConnection;
#ifndef EIP_CIPSAFETY
	SESSION* pSession;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig;
#endif
	UINT32 lSaveConnectionState;

	if (pConnection->lConnectionState != ConnectionEstablished && pConnection->lConnectionState != ConnectionClosing)
		return;

#ifdef CONNECTION_STATS
	if (connectionIsIOConnection(pConnection))
		gSystemStats.systemStats.iNumCurrentIOConnections--;
	else if (pConnection->cfg.bTransportClass == Class3)
		gSystemStats.systemStats.iNumCurrentExplicitConnections--;
#endif

	/* Sync sequence count so application can provide a new request */
	if ((pConnection->cfg.bTransportClass == Class3) &&
		(pConnection->cfg.bOriginator == TRUE))
	{
		pConnection->iInDataSeqNbr = pConnection->iOutDataSeqNbr;
	}

	connectionReleaseClass1Socket( pConnection );

	/* Remove Class3 requests for this connection */
	for( pRequest = (gpnRequests - 1); pRequest >= gRequests; pRequest-- )
	{
		if (pRequest->cfg.class3Cfg.iConnectionSerialNbr == pConnection->iConnectionSerialNbr &&
			pRequest->cfg.class3Cfg.iVendorID == pConnection->iVendorID &&
			pRequest->cfg.class3Cfg.lDeviceSerialNbr == pConnection->lDeviceSerialNbr )
		{
			requestRemove( pRequest );
		}
	}

	/* If this is incoming I/O connection - check if need to close any dependent input or listen only connections */
	if ((connectionIsIOConnection(pConnection)) && 
		(!pConnection->cfg.bOriginator) &&
		(pConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint))
	{
		/* Check if Exclusive Owner - close any input and listen only connections for the same producing conn point */
		if ( connectionIsDataConnPoint(pConnection->cfg.iTargetConsumingConnPoint) )
		{
			if (pConnection->cfg.iProducingConnectionType == Multicast)
			{
				for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
				{
					/* Remove input (only in case of time out and not explicit close) and listen only 
					 * incoming connections for the same multicast producing conn point 
					 * Logic follows:
					 *  - not the same connection
					 *  - connection is established
					 *  - class 1
					 *  - target
					 *  - same producing connection point
					 *  - either a listen only connection or this is a timeout
					 *  - both connections are not redundant owners (don't close your co-owner)
					 *  */
					if ( pLoopConnection != pConnection && 
						 pLoopConnection->lConnectionState == ConnectionEstablished &&
						 connectionIsIOConnection(pLoopConnection) && 
						 !pLoopConnection->cfg.bOriginator &&
 						 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
						 (pLoopConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint || bTimeOut ) &&
						 (pConnection->cfg.iTargetProducingConnPoint == pLoopConnection->cfg.iTargetProducingConnPoint) &&
						 !(pConnection->cfg.bRedundantOwner && pLoopConnection->cfg.bRedundantOwner))
					{
						/* Ensure there isn't another RedundantOwner connection for ListenOnly connection
						   to remain attached to */
						if ((pLoopConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint) &&
							(pConnection->cfg.bRedundantOwner))
						{
							for ( pROLoopConnection = gConnections; pROLoopConnection < gpnConnections; pROLoopConnection++ )
							{
								if ((pROLoopConnection != pConnection) && (pROLoopConnection != pLoopConnection) &&
									(pROLoopConnection->lConnectionState == ConnectionEstablished) &&
									(!pROLoopConnection->cfg.bOriginator) &&
									(pROLoopConnection->cfg.bRedundantOwner) &&
									(pROLoopConnection->cfg.iTargetConfigConnInstance == pConnection->cfg.iTargetConfigConnInstance) &&
									(pROLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
									(pROLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
								{
									/* Found another RO connection, don't close ListenOnly */
									break;
								}

							}

							if (pROLoopConnection >= gpnConnections)
							{
								connListSetConnectionConfigState(pLoopConnection, ConfigurationClosing, 0);
							}
						}
						else
						{
							connListSetConnectionConfigState(pLoopConnection, ConfigurationClosing, 0);
						}
					}
				}
			}
		}
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		else if (assemblyIsScannerInputInstance(pConnection->cfg.iTargetConsumingConnPoint) || 
				 assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
		{
			for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
			{
				/* Remove input (only in case of time out and not explicit close) and listen only incoming connections for the same producing conn point */
				if ( pLoopConnection != pConnection && 
					 pLoopConnection->lConnectionState == ConnectionEstablished &&
					 connectionIsIOConnection(pLoopConnection) && 
					 !pLoopConnection->cfg.bOriginator &&
					 (pLoopConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint || bTimeOut ) &&
					 pConnection->cfg.iTargetProducingConnPoint == pLoopConnection->cfg.iTargetProducingConnPoint &&
#ifdef EIP_LARGE_MEMORY
					 pConnection->cfg.nOutputScannerTable == pLoopConnection->cfg.nOutputScannerTable &&
#endif
					 pConnection->cfg.nOutputScannerOffset == pLoopConnection->cfg.nOutputScannerOffset)
				{
					connListSetConnectionConfigState(pLoopConnection, ConfigurationClosing, 0);
				}
			}
		}
#endif
		/* If the last input only connection - remove any listen only connections for the same producing conn point */
		else if ( pConnection->cfg.iTargetConsumingConnPoint == gHeartbeatConnPoint )
		{	
			if ( connectionNotListenOnlyCnctCnt( pConnection ) == 0 )	/* Make sure this is the last and only input only connection for that producing conn point */
			{
				for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
				{
					/* Remove input and listen only incoming connections for the same producing conn point */
					if (pLoopConnection != pConnection && 
						pLoopConnection->lConnectionState == ConnectionEstablished &&
						connectionIsIOConnection(pLoopConnection) && 
						!pLoopConnection->cfg.bOriginator &&
						pConnection->cfg.iTargetProducingConnPoint == pLoopConnection->cfg.iTargetProducingConnPoint &&
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
						((!assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint)) || 
							((pConnection->cfg.nOutputScannerOffset == pLoopConnection->cfg.nOutputScannerOffset)
#ifdef EIP_LARGE_MEMORY
							&& (pConnection->cfg.nOutputScannerTable == pLoopConnection->cfg.nOutputScannerTable)
#endif
							)) &&
#endif
						pLoopConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint )
					{
						connListSetConnectionConfigState(pLoopConnection, ConfigurationClosing, 0);
					}
				}    
			}
		}		
	}

#ifndef EIP_CIPSAFETY
	/* If this is the only target connection on the session, close the socket */
	pTcpIpConfig = tcpipFindClaimedHostAddress(pConnection->cfg.lHostIPAddr);

	if ((pTcpIpConfig != NULL) && 
		((bTimeOut)
#ifdef EIP_QUICK_CONNECT
		|| (pTcpIpConfig->InterfaceConfig.bQuickConnect == TCPIP_QUICK_CONNECT_ENABLED)
#endif
		) &&
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
			/* no other connections found */
			pSession = sessionGetBySessionId( pConnection->nSessionId );
			if (pSession != NULL)
			{
				/* Make sure the connection isn't removed twice by forcing it
				   to be considered timed out */
				lSaveConnectionState = pConnection->lConnectionState;
				pConnection->lConnectionState = ConnectionTimedOut;
				sessionRemove(pSession, FALSE);
				pConnection->lConnectionState = lSaveConnectionState;
			}
		}
	}
#endif

	/* Check if the redundant master needs to change */
	if (pConnection->bIsRedundantMaster)
	{
		/* Ensure this connection isn't repicked */
		pConnection->cfg.bClaimOutputOwnership = 0;
		pConnection->cfg.bReadyOwnershipOutputs = 0;
		connectionFindNewRedundantMaster(pConnection);
	}
}

/*---------------------------------------------------------------------------
** connectionCfgRelease( )
**
** Release dynamic memory from the connection config structure
**---------------------------------------------------------------------------
*/
void connectionCfgRelease(CONNECTION_CFG* pcfg)
{
	/* All dynamic memory pointers are stored in PDU and copied in config parameters */
	pduRelease( &pcfg->PDU );

	EtIP_free( &pcfg->pConnectionName, &pcfg->iConnectionNameSize );
	
	EtIP_free( &pcfg->targetConsumingConnTag, &pcfg->iTargetConsumingConnTagSize );
	EtIP_free( &pcfg->targetProducingConnTag, &pcfg->iTargetProducingConnTagSize );
}

/*---------------------------------------------------------------------------
** connectionRemove( )
**
** Remove a connection from the connection array
**---------------------------------------------------------------------------
*/
void connectionRemove( CONNECTION* pConnection, BOOL bTimeOut )
{
	CONNECTION* pLoopConnection;
	INT32 nInstance = pConnection->cfg.nInstance;
	UINT32 lCloseEvent = NM_CONNECTION_CLOSED;

	connectionGoOffline( pConnection, bTimeOut );

	/* Clean up dynamic data */
	connectionCfgRelease(&pConnection->cfg);
#ifdef ET_IP_SCANNER
	connectionCfgRelease(&pConnection->CCOcfg.cfg);
	pduRelease(&pConnection->CCOcfg.ExtraPDU);
#endif

	EtIP_free( &pConnection->pResponseData, &pConnection->iResponseDataSize );
	EtIP_free(&pConnection->pExtendedErrorData, &pConnection->iExtendedErrorDataSize);
#ifdef ET_IP_SCANNER
	EtIP_free(&pConnection->pApplicationData, &pConnection->iApplicationDataSize);
#endif
#ifdef ET_IP_MODBUS
	EtIP_free( &pConnection->pModbusConsumeData, &pConnection->iModbusConsumeDataSize);
#endif

	EtIP_free( &pConnection->requestCfg.pData, &pConnection->requestCfg.iDataSize );
	pduRelease( &pConnection->requestCfg.PDU );

	/* Check that we are not in the early stages of establishing a connection when there was an error */
	if ( nInstance != (INT32)INVALID_CONN_INSTANCE && nInstance != 0 && pConnection->lConnectionState != ConnectionNonExistent )
	{
		notifyEvent( lCloseEvent, nInstance, 0 );
	}

#ifdef EIP_LARGE_CONFIG_OPT
	connListClearCnxnLists(pConnection);    /* kill all active poll and timer entries */
	connListRemoveCnxnIdTableEntry(pConnection->lConsumingCID, pConnection->cfg.lIPAddress, pConnection->cfg.nInstance);
#endif

	if ( gpnConnections > gConnections )
		gpnConnections--;

	DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"connectionRemove with Instance %d, total now %d", nInstance, gpnConnections-gConnections);

	/* Shift the connections with the higher index to fill in the void */
	for( pLoopConnection = pConnection; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		memcpy( pLoopConnection, pLoopConnection+1, sizeof(CONNECTION) );
#ifdef EIP_LARGE_CONFIG_OPT
		/* fix the timer queue entries for the moved connection */
		connListMoveCnxnListEntries(pLoopConnection);

		/* fix the hash table entry for the moved connection */
		connListRemoveCnxnIdTableEntry(pLoopConnection->lConsumingCID, pLoopConnection->cfg.lIPAddress, pLoopConnection->cfg.nInstance);
		connListAddCnxnIdTableEntry(pLoopConnection->lConsumingCID, pLoopConnection->cfg.lIPAddress, pLoopConnection);
#endif
	}
	
	connectionNotifyNetLEDChange();
}

/*---------------------------------------------------------------------------
** connectionRecalcTimeouts( )
**
** Recalculate timeout and inhibit intervals based on the connection settings
**---------------------------------------------------------------------------
*/
void connectionRecalcTimeouts( CONNECTION* pConnection )
{
	ETAG_DATATYPE data;
	UINT16 iTagSize;

	/* By default, inhibit interval is the quarter of the producing I/O rate */
	if ( pConnection->cfg.bOriginator )
	{
		if (pConnection->cfg.bProductionOTInhibitInterval == 0)
			pConnection->lInhibitInterval = 0;
		else if (pConnection->cfg.bProductionOTInhibitInterval < pConnection->cfg.lProducingDataRate)
			pConnection->lInhibitInterval = pConnection->cfg.bProductionOTInhibitInterval;
		else
			pConnection->lInhibitInterval = pConnection->cfg.lProducingDataRate >> 2;
	}
	else
	{
		data = pduGetTagByType(&pConnection->cfg.PDU, TAG_TYPE_INHIBIT_INTERVAL, 0, &iTagSize);
		if ((iTagSize) && (((UINT8)data) < pConnection->cfg.lProducingDataRate))
			pConnection->lInhibitInterval = (UINT8)data;
		else
			pConnection->lInhibitInterval = pConnection->cfg.lProducingDataRate >> 2;
	}

	/* Timeout multiplier of 0 indicates that timeout should be 4 times larger than producing
	   data rate, 1 - 8 times larger, 2 - 16 times, and so on */
	pConnection->lTimeoutInterval = pConnection->cfg.lConsumingDataRate << (2+pConnection->cfg.bTimeoutMultiplier);
	DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"connectionRecalcTimeouts Time Out set to %d for connection %d", pConnection->lTimeoutInterval, pConnection->cfg.nInstance);
}

/*---------------------------------------------------------------------------
** connectionResetAllTicks( )
**
** Reset produce and timeout ticks
**---------------------------------------------------------------------------
*/
void connectionResetAllTicks( CONNECTION* pConnection )
{
	if ( pConnection->cfg.lProducingDataRate < CONNECTION_FIRST_SEND_MAX_DELAY )
	{
		if (pConnection->cfg.lProducingDataRate == 1)
		{
			connListSetCnxnProductionTimer(pConnection, pConnection->cfg.lProducingDataRate);
		}
		else
		{
			connListSetCnxnProductionTimer(pConnection, pConnection->cfg.lProducingDataRate - 1);
		}
	}
	else
	{
		connListSetCnxnProductionTimer(pConnection, CONNECTION_FIRST_SEND_MAX_DELAY - 1);
	}
	if ( pConnection->cfg.lConsumingDataRate > 0 ) /* don't set the timeout if the consuming rate is 0 */
		connListResetCnxnTimeoutTimer(pConnection, CONNECTION_FIRST_PACKET_TIMEOUT);

	connListSetCnxnInhibitTimer(pConnection, pConnection->lInhibitInterval);
}

/*---------------------------------------------------------------------------
** connectionResetProducingTicks( )
**
** Reset produce tick for next production
**---------------------------------------------------------------------------
*/
void connectionResetProducingTicks( CONNECTION* pConnection )
{
	connListSetCnxnProductionTimer(pConnection, pConnection->cfg.lProducingDataRate);
	connListSetCnxnInhibitTimer(pConnection, pConnection->lInhibitInterval);
}

/*---------------------------------------------------------------------------
** connectionResetConsumingTicks( )
**
** I/O has just been received, reset the timeout tick
**---------------------------------------------------------------------------
*/

void connectionResetConsumingTicks( CONNECTION* pConnection )
{
	connListResetCnxnTimeoutTimer(pConnection, pConnection->lTimeoutInterval);
}

/*---------------------------------------------------------------------------
** connectionGetFromInstance( )
**
** Returns connection pointer based on the connection instance.
** Returns NULL if the connection instance was not found.
**---------------------------------------------------------------------------
*/
CONNECTION* connectionGetFromInstance(INT32 nInstance)
{
	CONNECTION* pConnection;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ( pConnection->cfg.nInstance == nInstance ||
			pConnection->nCCProducingInstance == nInstance
#ifdef ET_IP_SCANNER
			|| pConnection->CCOcfg.cfg.nInstance == nInstance
#endif
			)
		{
			return pConnection;
		}
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** connectionGetFromAssmInstance( )
**
** Returns connection that is using specified assembly instance.
** Returns NULL if specified assembly instance is not used.
**---------------------------------------------------------------------------
*/
CONNECTION* connectionGetFromAssmInstance(UINT32 iInstance)
{
	CONNECTION* pLoopConnection;

	/* Check both active and pending connections */
	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((!pLoopConnection->cfg.bOriginator) && connectionIsIOConnection(pLoopConnection))
		{
			if ( pLoopConnection->cfg.iTargetConfigConnInstance == iInstance || pLoopConnection->cfg.iTargetConsumingConnPoint == iInstance ||
				 pLoopConnection->cfg.iTargetProducingConnPoint == iInstance )
				return pLoopConnection;
		}
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** connectionGetNumOrigEstOrClsing( )
**
** Returns the number of the originating connections with the status
** ConnectionEstablished or ConnectionClosing
**---------------------------------------------------------------------------
*/
INT32 connectionGetNumOrigEstOrClsing()
{
	CONNECTION* pConnection;
	INT32 nEstablished = 0;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ( pConnection->cfg.bOriginator &&
			 (pConnection->lConnectionState == ConnectionEstablished || pConnection->lConnectionState == ConnectionClosing) )
			nEstablished++;
	}

	return nEstablished;
}

#ifdef CONNECTION_STATS
/*---------------------------------------------------------------------------
** connectionTotalLostIOPackets( )
**
** Sum up all lost packets from all I/O connections
**---------------------------------------------------------------------------
*/
UINT32 connectionTotalLostIOPackets(void)
{
	UINT32 lLostPackets = 0;

	CONNECTION* pConnection;
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ((pConnection->lConnectionState == ConnectionEstablished) &&
			(connectionIsIOConnection(pConnection)))
		{
			lLostPackets += pConnection->lLostPackets;
		}
	}

	return lLostPackets;
}

/*---------------------------------------------------------------------------
** connectionTotalLostIOPackets( )
**
** Clear lost packet count from all I/O connections
**---------------------------------------------------------------------------
*/
void connectionClearLostIOPackets(void)
{
	CONNECTION* pConnection;
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (connectionIsIOConnection(pConnection))
		{
			pConnection->lLostPackets = 0;
		}
	}
}

/*---------------------------------------------------------------------------
** connectionTotalIOPPS( )
**
** Sum up previous second's I/O packet count from all I/O connections
**---------------------------------------------------------------------------
*/
UINT32 connectionTotalIOPPS(void)
{
	UINT32 lioPPS = 0;

	CONNECTION* pConnection;
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ((pConnection->lConnectionState == ConnectionEstablished) &&
			(connectionIsIOConnection(pConnection)) &&
			pConnection->bCollectingStats)
		{
			lioPPS += pConnection->lRcvdPPSCountPrev;
			lioPPS += pConnection->lSendPPSCountPrev;
		}
	}

	return lioPPS;
}

/*---------------------------------------------------------------------------
** connectionTotalExplicitPPS( )
**
** Sum up previous second's message count from all explicit connections and UCMMs
**---------------------------------------------------------------------------
*/
UINT32 connectionTotalExplicitPPS(void)
{
	UINT32 lExplicitPPS = 0;

	CONNECTION* pConnection;
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ((pConnection->lConnectionState == ConnectionEstablished) &&
			(pConnection->cfg.bTransportClass == Class3) &&
			pConnection->bCollectingStats)
		{
			lExplicitPPS += pConnection->lRcvdPPSCountPrev;
			lExplicitPPS += pConnection->lSendPPSCountPrev;
		}
	}

	lExplicitPPS += gSystemStats.iNumRecvUCMMMessagesPPSPrev;
	lExplicitPPS += gSystemStats.iNumSendUCMMMessagesPPSPrev;
	return lExplicitPPS;
}

#endif

/*---------------------------------------------------------------------------
** connectionGetConnDeviceStatus( )
**
** Returns the connection status for the device as a whole
**---------------------------------------------------------------------------
*/
UINT16 connectionGetConnDeviceStatus()
{
	CONNECTION* pConnection;
	INT32  nEstablished = 0;
	BOOL   bRunMode = FALSE;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (connectionIsIOConnection(pConnection))
		{
			if (pConnection->lConnectionState == ConnectionEstablished)
			{
				nEstablished++;

#ifdef ET_IP_SCANNER
				if ( pConnection->cfg.bOriginator )
				{
					if ( gbRunMode )
						bRunMode = TRUE;
				}
				else
#endif			
				if ( pConnection->bRunIdleFlag )
					bRunMode = TRUE;
			}
			else if (pConnection->lConnectionState == ConnectionTimedOut)
			{
				return ID_STATUS_COMM_FAULT;
			}
		}
	}

	if ( nEstablished == 0 )
		return ID_STATUS_NOT_CONNECTED;
	
	if (bRunMode)
		return ID_STATUS_CONNECTED_RUN_MODE;

	return ID_STATUS_CONNECTED_IDLE_MODE;
}

/*---------------------------------------------------------------------------
** connectionGetUnusedInstance( )
**
** Returns the first unused instance in the connection array.
**---------------------------------------------------------------------------
*/
INT32 connectionGetUnusedInstance(void)
{
	CONNECTION* pConnection;
	INT32  nInstance = 1;

	for( nInstance = 1; nInstance <= MAX_CONNECTIONS; nInstance++ )
	{
		pConnection = connectionGetFromInstance( nInstance );

		if ( pConnection == NULL)
			return nInstance;
	}

	return ERROR_STATUS;
}


/*---------------------------------------------------------------------------
** connectionAnotherPending( )
**
** Return TRUE if there is another pending connection with the same IP
** address that's in the process of opening or closing.
**---------------------------------------------------------------------------
*/
BOOL connectionAnotherPending( CONNECTION* pConnection )
{
	CONNECTION*  pLoopConnection;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* Find another connection with the same IP address */
		if ( pLoopConnection != pConnection && pConnection->cfg.lIPAddress == pLoopConnection->cfg.lIPAddress )
		{
			/* Check if the connection is being opened */
			if ( pLoopConnection->lConnectionState == ConnectionConfiguring &&
				( pLoopConnection->lConfigurationState == ConfigurationWaitingForSession ||
				  pLoopConnection->lConfigurationState == ConfigurationWaitingForForwardOpenResponse ) )
			{
				return TRUE;
			}

			/* Check if the connection is being closed */
			if ( pLoopConnection->lConnectionState == ConnectionClosing &&
				 pLoopConnection->lConfigurationState == ConfigurationWaitingForForwardCloseResponse )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** connectionDelayPending( )
**
** Postpone the configuration of other connections with the same IP 
** address for CONNECTION_CONFIGURATION_DELAY period.
**---------------------------------------------------------------------------
*/
void connectionDelayPending( CONNECTION* pConnection )
{
	CONNECTION*  pLoopConnection;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* Find another connection with the same IP address */
		if ( pLoopConnection != pConnection && pConnection->cfg.lIPAddress == pLoopConnection->cfg.lIPAddress )
		{
			/* Check if the connection is waiting to be opened or closed */
			if (( pLoopConnection->lConnectionState == ConnectionConfiguring && pLoopConnection->lConfigurationState == ConfigurationLogged ) ||
				( (pLoopConnection->lConnectionState == ConnectionClosing || pLoopConnection->lConnectionState == ConnectionEstablished) && pLoopConnection->lConfigurationState == ConfigurationClosing ) )
			{
				connListSetCnxnConfigDelayTimer(pLoopConnection, CONNECTION_CONFIGURATION_DELAY);
#ifdef EIP_LARGE_CONFIG_OPT
				/* break out - we only need to trigger the next connection, the others will be triggered
				 * after it is completed.  
				 */
				break;
#endif
			}
		}
	}
}

/*---------------------------------------------------------------------------
** connectionGetFromSerialNumber( )
**
** Returns index in the connection array based on the connection serial number.
**---------------------------------------------------------------------------
*/
CONNECTION* connectionGetFromSerialNumber(UINT16 iConnectionSerialNbr, UINT16 iVendorID, UINT32 lDeviceSerialNbr)
{
	CONNECTION* pConnection;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (pConnection->iConnectionSerialNbr == iConnectionSerialNbr &&
			pConnection->iVendorID == iVendorID &&
			pConnection->lDeviceSerialNbr == lDeviceSerialNbr )
		{
			return pConnection;
		}
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** connectionGetConnectedCount( )
**
** Returns number of active connections
**---------------------------------------------------------------------------
*/
UINT16 connectionGetConnectedCount(void)
{
	CONNECTION* pConnection;
	UINT16  nCount = 0;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ( pConnection->lConnectionState == ConnectionEstablished )
		{
			nCount++;
		}
	}

	return nCount;
}

UINT16 connectionGetIOConnectionCount(UINT32 lIPAddress)
{
	CONNECTION* pConnection;
	UINT16  nCount = 0;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (( pConnection->lConnectionState == ConnectionEstablished )
			&& (connectionIsIOConnection(pConnection)
			&& ((pConnection->cfg.lHostIPAddr == lIPAddress) || (lIPAddress == 0))))
			nCount++;
	}

	return nCount;
}

/*---------------------------------------------------------------------------
** connectionUpdateAllSeqCount( )
**
** Update the data sequence count to indicate new data for all connections
** when, for example, the run mode changes
**---------------------------------------------------------------------------
*/
void connectionUpdateAllSeqCount(void)
{
	CONNECTION* pConnection;

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ((pConnection->lConnectionState == ConnectionEstablished) &&
			connectionIsIOConnection(pConnection))
		{
			pConnection->iOutDataSeqNbr++;

			if (((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ChangeOfState ) ||
				((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ApplicationTriggered ))
				connListTriggerCnxnProduction(pConnection);
		}
	}
}

/*---------------------------------------------------------------------------
** connectionGetFirstMultiProducer( )
**
** Returns the connection index for the first connection producing to the
** same multicast as pConnection does 
**---------------------------------------------------------------------------
*/
CONNECTION* connectionGetFirstMultiProducer(CONNECTION* pConnection)
{
	CONNECTION* pLoopConnection;

	/* Is not applicable if not producing Multicast */
	if (pConnection->cfg.iProducingConnectionType != Multicast )
		return NULL;

	for ( pLoopConnection = gConnections; pLoopConnection < pConnection; pLoopConnection++ )
	{
		if ( pLoopConnection->lConnectionState == ConnectionEstablished &&
			 !pLoopConnection->cfg.bOriginator &&
			 connectionIsIOConnection(pLoopConnection) &&
			 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
			 pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint &&
			 pLoopConnection->cfg.lHostIPAddr == pConnection->cfg.lHostIPAddr
#ifdef ET_IP_MODBUS
			&& pLoopConnection->cfg.modbusCfg.iModbusProduceSize == pConnection->cfg.modbusCfg.iModbusProduceSize
#endif
			)
				return pLoopConnection;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** connectionGetAnyMultiProducer( )
**
** Returns the connection index for any connection producing to the
** same multicast as nConnection does 
**---------------------------------------------------------------------------
*/
CONNECTION* connectionGetAnyMultiProducer(CONNECTION* pConnection)
{
	CONNECTION* pLoopConnection;

	/* Is not applicable if Originator or if T->O is PointToPoint */
	if (pConnection->cfg.bOriginator ||
		pConnection->cfg.iProducingConnectionType != Multicast )
		return NULL;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ( pLoopConnection != pConnection &&
			 !pLoopConnection->cfg.bOriginator &&
			 pLoopConnection->lConnectionState == ConnectionEstablished &&
			 connectionIsIOConnection(pLoopConnection) &&
			 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
			 pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint
#ifdef ET_IP_MODBUS
			&& pLoopConnection->cfg.modbusCfg.iModbusProduceSize == pConnection->cfg.modbusCfg.iModbusProduceSize
#endif
			)
				return pLoopConnection;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** connectionGetMulticastProducers( )
**
** Returns the number of all other connections producing to the 
** same multicast as nConnection does 
**---------------------------------------------------------------------------
*/
INT32 connectionGetMulticastProducers(CONNECTION* pConnection)
{
	CONNECTION* pLoopConnection;
	INT32  nCount = 0;

	/* Is not applicable if Originator or if T->O is PointToPoint */
	if ( pConnection->cfg.bOriginator ||
		 pConnection->cfg.iProducingConnectionType != Multicast )
		return 0;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ( pLoopConnection != pConnection &&
			 !pLoopConnection->cfg.bOriginator &&
			 pLoopConnection->lConnectionState == ConnectionEstablished &&
			 connectionIsIOConnection(pLoopConnection) &&
			 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
			 pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint )
			nCount++;
	}

	return nCount;
}

/*---------------------------------------------------------------------------------
** connectionNotListenOnlyCnctCnt( )
**
** Returns the number of all not listen only connections other than the passed one
** that are produced for the same producing connection point as the passed
** connection.
**---------------------------------------------------------------------------------
*/
INT32 connectionNotListenOnlyCnctCnt(CONNECTION* pConnection)
{
	CONNECTION* pLoopConnection;
	INT32  nCount = 0;

	/* Is not applicable if Originator or if T->O is PointToPoint */
	if ( pConnection->cfg.bOriginator ||
		 pConnection->cfg.iProducingConnectionType != Multicast )
		return 0;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ( pLoopConnection != pConnection &&
			 !pLoopConnection->cfg.bOriginator &&
			 pLoopConnection->lConnectionState == ConnectionEstablished &&
			 connectionIsIOConnection(pLoopConnection) &&
			 pLoopConnection->cfg.iProducingConnectionType == Multicast &&
			 pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint &&
			 pLoopConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint )
			nCount++;
	}

	return nCount;
}

static void connectNotifyNewRedundantMaster(CONNECTION* pConnection)
{
	CONNECTION *pLoopConnection;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((!pLoopConnection->cfg.bOriginator) &&
			(pLoopConnection->cfg.bRedundantOwner) &&
			connectionIsIOConnection(pLoopConnection) &&
			(pLoopConnection->cfg.iTargetConfigConnInstance == pConnection->cfg.iTargetConfigConnInstance) &&
			(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
		{
			notifyEvent(NM_INCOMING_CONNECTION_REDUNDANT_MASTER_CHANGED, pConnection->cfg.nInstance, pLoopConnection->cfg.nInstance);
		}
	}
}

static void connectNotifyNoRedundantMaster(CONNECTION* pConnection)
{
	CONNECTION *pLoopConnection;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((!pLoopConnection->cfg.bOriginator) &&
			(pLoopConnection->cfg.bRedundantOwner) &&
			connectionIsIOConnection(pLoopConnection) &&
			(pLoopConnection->cfg.iTargetConfigConnInstance == pConnection->cfg.iTargetConfigConnInstance) &&
			(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
		{
			notifyEvent(NM_INCOMING_CONNECTION_NO_REDUNDANT_MASTER, pLoopConnection->cfg.nInstance, 0);
		}
	}
}

/*---------------------------------------------------------------------------
** connectionSetNewRedundantMaster( )
**
** Set connection to redundant "master".  Set all other similar connections
** to "not master"
**---------------------------------------------------------------------------
*/
void connectionSetNewRedundantMaster(CONNECTION* pConnection)
{
	CONNECTION* pLoopConnection;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((pLoopConnection != pConnection) &&
			(!pLoopConnection->cfg.bOriginator) &&
			(pLoopConnection->cfg.bRedundantOwner) &&
			(connectionIsIOConnection(pLoopConnection)) &&
			(pLoopConnection->cfg.iTargetConfigConnInstance == pConnection->cfg.iTargetConfigConnInstance) &&
			(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
		{
			pLoopConnection->bIsRedundantMaster = FALSE;
		}
	}

	pConnection->bIsRedundantMaster = TRUE;
	connectNotifyNewRedundantMaster(pConnection);
}

/*---------------------------------------------------------------------------
** connectionFindNewRedundantMaster( )
**
** Connection no longer master.  Find a new one based on last connection to
** transition its COO bit or the highest ROO value
** Return TRUE if new master is determined
**---------------------------------------------------------------------------
*/
BOOL connectionFindNewRedundantMaster(CONNECTION* pConnection)
{
	CONNECTION *pLoopConnection, *pMasterConnection = NULL;

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		if ((!pLoopConnection->cfg.bOriginator) &&
			(pLoopConnection->cfg.bRedundantOwner) &&
			(connectionIsIOConnection(pLoopConnection)) &&
			(pLoopConnection->cfg.iTargetConfigConnInstance == pConnection->cfg.iTargetConfigConnInstance) &&
			(pLoopConnection->cfg.iTargetProducingConnPoint == pConnection->cfg.iTargetProducingConnPoint) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint == pConnection->cfg.iTargetConsumingConnPoint))
		{
			if (pLoopConnection->cfg.bClaimOutputOwnership)
			{
				/* Find the connection who was the last to transition its COO bit */
				if ((pMasterConnection == NULL) || (!pMasterConnection->cfg.bClaimOutputOwnership))
					pMasterConnection = pLoopConnection;
				else
				{
					if (IS_TICK_GREATER(pMasterConnection->lRedundantMasterTimestamp, pLoopConnection->lRedundantMasterTimestamp))
						pMasterConnection = pLoopConnection;
				}
			}
			else
			{
				/* Settle for highest non-zero ROO value */
				if (((pMasterConnection == NULL) && 
					 (pLoopConnection->cfg.bReadyOwnershipOutputs > 0)) || 
					((pMasterConnection != NULL) && 
					 (!pMasterConnection->cfg.bClaimOutputOwnership) && 
					 (pLoopConnection->cfg.bReadyOwnershipOutputs > pMasterConnection->cfg.bReadyOwnershipOutputs)))
					pMasterConnection = pLoopConnection;
			}
		}
	}


	if (pMasterConnection == NULL)
	{
		/* No master at all */
		pConnection->bIsRedundantMaster = FALSE;
		connectNotifyNoRedundantMaster(pConnection);
		return TRUE;
	}
	
	if (pMasterConnection == pConnection)
	{
		/* Same master */
		return FALSE;
	}

	connectionSetNewRedundantMaster(pMasterConnection);
	return TRUE;
}

#ifdef ET_IP_SCANNER
/*---------------------------------------------------------------------------
** connectionSetConnectionFlag( )
**
** Set connection flag member based on the configuration parameters
**---------------------------------------------------------------------------
*/

void connectionSetConnectionFlag(CONNECTION* pConnection)
{
	pConnection->cfg.iConnectionFlag = (UINT16)(( pConnection->cfg.bOriginator ) ?
				ORIGINATOR_CONNECTION_FLAG : TARGET_CONNECTION_FLAG);

#ifdef ET_IP_SCANNER
	if ( pConnection->cfg.bOriginator )
	{
		if ( pConnection->cfg.iOutputScannerSize == 0)
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_HEARTBEAT;
		else if ( pConnection->cfg.bOutputRunProgramHeader )
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_RUN_IDLE;
		else
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_PURE_DATA;

		if ( pConnection->cfg.iInputScannerSize == 0)
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_HEARTBEAT;
		else if ( pConnection->cfg.bInputRunProgramHeader )
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_RUN_IDLE;
		else
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_PURE_DATA;
	}
	else /* If Target  */
	{
#endif
		if (( pConnection->cfg.iTargetConsumingConnPoint == gHeartbeatConnPoint ) ||
			( pConnection->cfg.iTargetConsumingConnPoint == gListenOnlyConnPoint ))
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_HEARTBEAT;
		else if ( pConnection->cfg.bOutputRunProgramHeader )
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_RUN_IDLE;
		else
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_PURE_DATA;

		if (( pConnection->cfg.iTargetProducingConnPoint == gHeartbeatConnPoint ) ||
			( pConnection->cfg.iTargetProducingConnPoint == gListenOnlyConnPoint ))
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_TO_FORMAT_HEARTBEAT;
		else if ( pConnection->cfg.bInputRunProgramHeader )
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_RUN_IDLE;
		else
			pConnection->cfg.iConnectionFlag |= CONFIG_TYPE_OT_FORMAT_PURE_DATA;
#ifdef ET_IP_SCANNER
	}
#endif
}


/*---------------------------------------------------------------------------
** connectionConvertToInternal( )
**
** Convert configuration structure to the internal storage presentation
** where part of the data is stored in the dynamic memory pool.
**---------------------------------------------------------------------------
*/
BOOL connectionConvertToInternal(EtIPConnectionConfig* pcfg, CONNECTION_CFG* pConfig)
{
	UINT8* pModuleData;
	UINT8* pConnectionTag;

	memset( pConfig, 0, sizeof(CONNECTION_CFG) );

	pConfig->nInstance = pcfg->nInstance;
	pConfig->bOriginator = pcfg->bOriginator;
	pConfig->iConnectionFlag = pcfg->iConnectionFlag;
	pConfig->iTargetConfigConnInstance = pcfg->iTargetConfigConnInstance;
	pConfig->iTargetConsumingConnPoint = pcfg->iTargetConsumingConnPoint;
	pConfig->iTargetProducingConnPoint = pcfg->iTargetProducingConnPoint;
	pConfig->iCurrentProducingSize = pcfg->iCurrentProducingSize;
	pConfig->iCurrentConsumingSize = pcfg->iCurrentConsumingSize;
	pConfig->lProducingDataRate = pcfg->lProducingDataRate;
	pConfig->lConsumingDataRate = pcfg->lConsumingDataRate;
	pConfig->bProductionOTInhibitInterval = pcfg->bProductionOTInhibitInterval;
	pConfig->bProductionTOInhibitInterval = pcfg->bProductionTOInhibitInterval;
	pConfig->bOutputRunProgramHeader = pcfg->bOutputRunProgramHeader;
	pConfig->bInputRunProgramHeader = pcfg->bInputRunProgramHeader;
	pConfig->iProducingConnectionType = pcfg->iProducingConnectionType;
	pConfig->iConsumingConnectionType = pcfg->iConsumingConnectionType;
	pConfig->iProducingPriority = pcfg->iProducingPriority;
	pConfig->iConsumingPriority = pcfg->iConsumingPriority;
	pConfig->bTransportClass = pcfg->bTransportClass;
	pConfig->bTransportType = pcfg->bTransportType;
	pConfig->bTimeoutMultiplier = pcfg->bTimeoutMultiplier;
	pConfig->iWatchdogTimeoutAction = pcfg->iWatchdogTimeoutAction;
	pConfig->lWatchdogTimeoutReconnectDelay = pcfg->lWatchdogTimeoutReconnectDelay;
	pConfig->bRedundantOwner = pcfg->bRedundantOwner;
	pConfig->bReadyOwnershipOutputs = pcfg->bReadyOwnershipOutputs;
	pConfig->bClaimOutputOwnership = pcfg->bClaimOutputOwnership;
	pConfig->bClass1VariableConsumer = pcfg->bClass1VariableConsumer;
	pConfig->bClass1VariableProducer = pcfg->bClass1VariableProducer;
	memcpy(&pConfig->deviceId, &pcfg->deviceId, sizeof(EtIPDeviceID));

	/* request timeout values based on the config */
	pConfig->bOpenPriorityTickTime = (UINT8)((pcfg->bOpenPriorityTickTime == 0) ? PRIORITY_TICK_TIME : pcfg->bOpenPriorityTickTime);
	pConfig->bOpenTimeoutTicks = (UINT8)((pcfg->bOpenTimeoutTicks == 0) ? TIMEOUT_TICKS : pcfg->bOpenTimeoutTicks);

	pConfig->iConnectionNameSize = pcfg->iConnectionNameSize;
	pConfig->pConnectionName = EtIP_malloc( (unsigned char*)pcfg->connectionName, pConfig->iConnectionNameSize );
	if ((pConfig->iConnectionNameSize > 0) && (pConfig->pConnectionName == NULL))
		return FALSE;

	/* If set to INADDR_NONE - assign the first network card address */
	if( pcfg->lHostIPAddr == htonl(INADDR_NONE) && gnClaimedHostIPAddrCount > 0 )
		pConfig->lHostIPAddr = gsTcpIpInstAttr[0].InterfaceConfig.lIpAddr;
	else
		pConfig->lHostIPAddr = pcfg->lHostIPAddr;

	pConfig->nOutputScannerOffset = pcfg->nOutputScannerOffset;
	pConfig->iOutputScannerSize = (UINT16)pcfg->nOutputScannerSize;
	pConfig->nInputScannerOffset = pcfg->nInputScannerOffset;
	pConfig->iInputScannerSize = (UINT16)pcfg->nInputScannerSize;
#ifdef EIP_LARGE_MEMORY
	pConfig->nInputScannerTable = pcfg->nInputScannerTable;
	pConfig->nOutputScannerTable = pcfg->nOutputScannerTable;
#endif

	connectionGenerateConnPath((char*)pcfg->networkPath, pConfig);

	if ( pcfg->iTargetConsumingConnTagSize )
	{
		pConfig->iTargetConsumingConnTagSize = pcfg->iTargetConsumingConnTagSize;
		pConfig->targetConsumingConnTag = EtIP_malloc( (UINT8*)&pcfg->targetConsumingConnTag, pcfg->iTargetConsumingConnTagSize );
		if ( pConfig->targetConsumingConnTag == NULL )
			return FALSE;

		pConnectionTag = EtIP_malloc( (UINT8*)&pcfg->targetConsumingConnTag, pcfg->iTargetConsumingConnTagSize );
		if ( !pduAddTag(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, pcfg->iTargetConsumingConnTagSize, (ETAG_DATATYPE)pConnectionTag) )
			return FALSE;
	}

	if ( pcfg->iTargetProducingConnTagSize )
	{
		pConfig->iTargetProducingConnTagSize = pcfg->iTargetProducingConnTagSize;
		pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)&pcfg->targetProducingConnTag, pcfg->iTargetProducingConnTagSize );
		if ( pConfig->targetProducingConnTag == NULL )
			return FALSE;

		pConnectionTag = EtIP_malloc( (UINT8*)&pcfg->targetProducingConnTag, pcfg->iTargetProducingConnTagSize );
		if ( !pduAddTag(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, pcfg->iTargetProducingConnTagSize, (ETAG_DATATYPE)pConnectionTag) )
			return FALSE;
	}

	/* Add tags for the config data */
	if ( pcfg->iModuleConfig1Size )
	{
		pModuleData = EtIP_malloc((UINT8*)&pcfg->moduleConfig1, pcfg->iModuleConfig1Size);
		if (pModuleData == NULL)
			return FALSE;

		if (!pduAddTag(&pConfig->PDU, TAG_TYPE_DATA, pcfg->iModuleConfig1Size, (ETAG_DATATYPE)pModuleData))
			return FALSE;
	}

	if ( pcfg->iModuleConfig2Size )
	{
		pModuleData = EtIP_malloc((UINT8*)&pcfg->moduleConfig2, pcfg->iModuleConfig2Size);
		if (pModuleData == NULL)
			return FALSE;

		if (!pduAddTag(&pConfig->PDU, TAG_TYPE_DATA2, pcfg->iModuleConfig2Size, (ETAG_DATATYPE)pModuleData))
			return FALSE;
	}

	return TRUE;
}

BOOL connectionConvertEPATHToInternal(UINT8* pByteArrayEPATH, INT32 iEPATHSize, EtIPEPATHConnectionConfig* pcfg, CONNECTION_CFG* pConfig, INT32* piError)
{
	ETAG_DATATYPE eTag;
	UINT16 iTagSize;

	memset( pConfig, 0, sizeof(CONNECTION_CFG) );

	pConfig->nInstance = pcfg->nInstance;
	pConfig->bOriginator = pcfg->bOriginator;
	pConfig->iConnectionFlag = pcfg->iConnectionFlag;
	pConfig->iCurrentProducingSize = pcfg->iCurrentProducingSize;
	pConfig->iCurrentConsumingSize = pcfg->iCurrentConsumingSize;
	pConfig->lProducingDataRate = pcfg->lProducingDataRate;
	pConfig->lConsumingDataRate = pcfg->lConsumingDataRate;
	pConfig->bOutputRunProgramHeader = pcfg->bOutputRunProgramHeader;
	pConfig->bInputRunProgramHeader = pcfg->bInputRunProgramHeader;
	pConfig->iProducingConnectionType = pcfg->iProducingConnectionType;
	pConfig->iConsumingConnectionType = pcfg->iConsumingConnectionType;
	pConfig->iProducingPriority = pcfg->iProducingPriority;
	pConfig->iConsumingPriority = pcfg->iConsumingPriority;
	pConfig->bTransportClass = pcfg->bTransportClass;
	pConfig->bTransportType = pcfg->bTransportType;
	pConfig->bTimeoutMultiplier = pcfg->bTimeoutMultiplier;
	pConfig->iWatchdogTimeoutAction = pcfg->iWatchdogTimeoutAction;
	pConfig->lWatchdogTimeoutReconnectDelay = pcfg->lWatchdogTimeoutReconnectDelay;
	pConfig->bRedundantOwner = pcfg->bRedundantOwner;
	pConfig->bReadyOwnershipOutputs = pcfg->bReadyOwnershipOutputs;
	pConfig->bClaimOutputOwnership = pcfg->bClaimOutputOwnership;
	pConfig->bClass1VariableConsumer = pcfg->bClass1VariableConsumer;
	pConfig->bClass1VariableProducer = pcfg->bClass1VariableProducer;

	/* request timeout values based on the config */
	pConfig->bOpenPriorityTickTime = (UINT8)((pcfg->bOpenPriorityTickTime == 0) ? PRIORITY_TICK_TIME : pcfg->bOpenPriorityTickTime);
	pConfig->bOpenTimeoutTicks = (UINT8)((pcfg->bOpenTimeoutTicks == 0) ? TIMEOUT_TICKS : pcfg->bOpenTimeoutTicks);

	pConfig->iConnectionNameSize = pcfg->iConnectionNameSize;
	pConfig->pConnectionName = EtIP_malloc( (unsigned char*)pcfg->connectionName, pConfig->iConnectionNameSize );
	if ((pConfig->iConnectionNameSize > 0) && (pConfig->pConnectionName == NULL))
	{
		*piError = NM_OUT_OF_MEMORY;
		return FALSE;
	}

	/* If set to INADDR_NONE - assign the first network card address */
	if( pcfg->lHostIPAddr == htonl(INADDR_NONE) && gnClaimedHostIPAddrCount > 0 )
		pConfig->lHostIPAddr = gsTcpIpInstAttr[0].InterfaceConfig.lIpAddr;
	else
		pConfig->lHostIPAddr = pcfg->lHostIPAddr;

	pConfig->lIPAddress = pcfg->lTargetIP;

	pConfig->nOutputScannerOffset = pcfg->nOutputScannerOffset;
	pConfig->iOutputScannerSize = (UINT16)pcfg->nOutputScannerSize;
	pConfig->nInputScannerOffset = pcfg->nInputScannerOffset;
	pConfig->iInputScannerSize = (UINT16)pcfg->nInputScannerSize;
#ifdef EIP_LARGE_MEMORY
	pConfig->nInputScannerTable = pcfg->nInputScannerTable;
	pConfig->nOutputScannerTable = pcfg->nOutputScannerTable;
#endif

	/* Build connection path PDU */
	if (pduParse(pByteArrayEPATH, &pConfig->PDU, FALSE, iEPATHSize) != iEPATHSize)
	{
		*piError = ERR_CONNECTION_CONFIGURATION_INVALID;
		return FALSE;
	}

	/* Extract the pieces of the EPATH that are individual members of CONNECTION_CFG */
	eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	pConfig->iTargetConfigConnInstance = (iTagSize != 0) ? (UINT32)eTag : INVALID_INSTANCE;

	eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 0, &iTagSize);
	if (iTagSize != 0)
	{
		pConfig->iTargetConsumingConnPoint = (UINT32)eTag;

		eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 1, &iTagSize);
		if (iTagSize != 0)
			pConfig->iTargetProducingConnPoint = (UINT32)eTag;
	}
	else
	{
		eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 0, &iTagSize);
		if (iTagSize != 0)
			pConfig->iTargetProducingConnPoint = (UINT32)eTag;
	}

	eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, 0, &pConfig->iTargetConsumingConnTagSize);
	if (pConfig->iTargetConsumingConnTagSize != 0)
	{
		pConfig->targetConsumingConnTag = EtIP_malloc((UINT8*)eTag, pConfig->iTargetConsumingConnTagSize );

		eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, 1, &pConfig->iTargetProducingConnTagSize);
		if (pConfig->iTargetProducingConnTagSize != 0)
			pConfig->targetProducingConnTag = EtIP_malloc((UINT8*)eTag, pConfig->iTargetProducingConnTagSize );
	}
	else
	{
		eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, 0, &pConfig->iTargetProducingConnTagSize);
		if (pConfig->iTargetProducingConnTagSize != 0)
			pConfig->targetProducingConnTag = EtIP_malloc((UINT8*)eTag, pConfig->iTargetProducingConnTagSize );
	}

	eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INHIBIT_INTERVAL, 0, &iTagSize);
	pConfig->bProductionTOInhibitInterval = (UINT8)((iTagSize != 0) ? (UINT8)eTag : 0);

	eTag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_KEY, 0, &iTagSize);
	if (iTagSize != 0)
	{
		memcpy(&pConfig->deviceId, (char*)eTag, iTagSize);
	}
	else
	{
		memset(&pConfig->deviceId, 0, sizeof(EtIPDeviceID));
	}

	return TRUE;
}

#endif

/*---------------------------------------------------------------------------
** connectionParseConnectionPath
**
** Parse Connection Path and store in CONNECTION_CFG structure members.
**---------------------------------------------------------------------------
*/
void connectionParseConnectionPath( CONNECTION_CFG* pConfig )    
{
	UINT16 iTagSize, iAssembly1TagSize, iAssembly2TagSize;
	ETAG_DATATYPE data, assembly1Tag = 0, assembly2Tag = 0;
	ASSEMBLY* pAssemblyTag1 = NULL;
	ASSEMBLY* pAssemblyTag2 = NULL;
	ASSEMBLY* pAssemblyPoint1 = NULL;
	ASSEMBLY* pAssemblyPoint2 = NULL;

	pConfig->iTargetConsumingConnPoint = INVALID_CONN_POINT;
	pConfig->iTargetProducingConnPoint = INVALID_CONN_POINT;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ( iTagSize )
		pConfig->iTargetConfigConnInstance = (UINT32)data;
	else
		pConfig->iTargetConfigConnInstance = INVALID_INSTANCE;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_INHIBIT_INTERVAL, 0, &iTagSize);
	if ( iTagSize )
		pConfig->bProductionTOInhibitInterval = (UINT8)data;

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_KEY, 0, &iTagSize);
	if ( iTagSize == sizeof(EtIPDeviceID) )
		memcpy( &pConfig->deviceId, (UINT8*)data, sizeof(EtIPDeviceID));

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 0, &iTagSize);
	if ( iTagSize )
	{
		pConfig->iTargetConsumingConnPoint = (UINT32)data;
		if ((pConfig->iTargetConsumingConnPoint != gHeartbeatConnPoint) &&
			(pConfig->iTargetConsumingConnPoint != gListenOnlyConnPoint)
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			&& (!assemblyIsScannerInputInstance(pConfig->iTargetConsumingConnPoint))
#endif
			)
		{
			pAssemblyPoint1 = assemblyGetByInstance( pConfig->iTargetConsumingConnPoint );
			if ( pAssemblyPoint1 == NULL )
			{																/* Connection points must be valid to succeed */
				pConfig->iTargetConsumingConnPoint = INVALID_CONN_POINT;
				return;
			}
		}
	}

	data = pduGetTagByType(&pConfig->PDU, TAG_TYPE_CONN_POINT, 1, &iTagSize);
	if ( iTagSize )
	{
		pConfig->iTargetProducingConnPoint = (UINT32)data;
		if ((pConfig->iTargetProducingConnPoint != gHeartbeatConnPoint)
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			&& (!assemblyIsScannerOutputInstance(pConfig->iTargetProducingConnPoint))
#endif
			)
		{
			pAssemblyPoint2 = assemblyGetByInstance( pConfig->iTargetProducingConnPoint );		
			if ( pAssemblyPoint2 == NULL )
			{																/* Connection points must be valid to succeed */
				pConfig->iTargetProducingConnPoint = INVALID_CONN_POINT;
				return;
			}
		}
	}

	assembly1Tag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, 0, &iAssembly1TagSize);
	if ( iAssembly1TagSize )
		pAssemblyTag1 = assemblyGetByTagName( (UINT8*)assembly1Tag, iAssembly1TagSize );

	assembly2Tag = pduGetTagByType(&pConfig->PDU, TAG_TYPE_EXT_SYMBOL, 1, &iAssembly2TagSize);
	if ( iAssembly2TagSize )
		pAssemblyTag2 = assemblyGetByTagName( (UINT8*)assembly2Tag, iAssembly2TagSize );

	/* If tag name(s) specified instead of connection points */
	if ( pConfig->iTargetConsumingConnPoint == INVALID_CONN_POINT || pConfig->iTargetProducingConnPoint == INVALID_CONN_POINT )
	{
		/* If no connection points specified */
		if ( pConfig->iTargetConsumingConnPoint == INVALID_CONN_POINT && pConfig->iTargetProducingConnPoint == INVALID_CONN_POINT )
		{
			if ( pAssemblyTag1 != NULL && pAssemblyTag2 != NULL )	/* Two tag names specified */
			{
				/* Extract the connection points */
				if ( (pAssemblyTag1->iType & AssemblyConsuming) && (pAssemblyTag2->iType & AssemblyProducing) )
				{
					pConfig->iTargetConsumingConnPoint = pAssemblyTag1->iInstance;
					pConfig->iTargetConsumingConnTagSize = iAssembly1TagSize;
					pConfig->targetConsumingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );

					pConfig->iTargetProducingConnPoint = pAssemblyTag2->iInstance;
					pConfig->iTargetProducingConnTagSize = iAssembly2TagSize;
					pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)assembly2Tag, iAssembly2TagSize );
				}
			}
			else if ( pAssemblyTag1 != NULL && pAssemblyTag2 == NULL )	/* One tag specified */
			{
				/* If the second tag is specified - it must be valid. Otherwise reject. */
				if ( iAssembly2TagSize == 0 )
				{
					/* In case of both producing and consuming assembly and only one tag and no connection point - always default to producing */
					if ( pAssemblyTag1->iType & AssemblyProducing )
					{
						pConfig->iTargetConsumingConnPoint = gHeartbeatConnPoint;
						pConfig->iTargetProducingConnPoint = pAssemblyTag1->iInstance;
						pConfig->iTargetProducingConnTagSize = iAssembly1TagSize;
						pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );
					}
					else if ( pAssemblyTag1->iType & AssemblyConsuming )
					{
						pConfig->iTargetConsumingConnPoint = pAssemblyTag1->iInstance;
						pConfig->iTargetConsumingConnTagSize = iAssembly1TagSize;
						pConfig->targetConsumingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );
						pConfig->iTargetProducingConnPoint = gHeartbeatConnPoint;
					}
				}
			}
		}
		else if ( pConfig->iTargetConsumingConnPoint != INVALID_CONN_POINT ) /* One connection point specified */
		{
			/* A tag specified */
			if ( pAssemblyTag1 != NULL )
			{
				if ( pConfig->iTargetConsumingConnPoint == gHeartbeatConnPoint )
				{
					if ( pAssemblyTag1->iType & AssemblyProducing )
					{
						pConfig->iTargetProducingConnPoint = pAssemblyTag1->iInstance;
						pConfig->iTargetProducingConnTagSize = iAssembly1TagSize;
						pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );
					}
					else if ( pAssemblyTag1->iType & AssemblyConsuming )
					{
						pConfig->iTargetProducingConnPoint = gHeartbeatConnPoint;
						pConfig->iTargetConsumingConnPoint = pAssemblyTag1->iInstance;
						pConfig->iTargetConsumingConnTagSize = iAssembly1TagSize;
						pConfig->targetConsumingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );						
					} 
				}
				else if ( pConfig->iTargetConsumingConnPoint == gListenOnlyConnPoint )
				{
					if ( pAssemblyTag1->iType & AssemblyProducing )
					{
						pConfig->iTargetProducingConnPoint = pAssemblyTag1->iInstance;
						pConfig->iTargetProducingConnTagSize = iAssembly1TagSize;
						pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );
					}
				}
				else if ( (pAssemblyPoint1->iType & AssemblyConsuming) && (pAssemblyTag1->iType & AssemblyProducing) )
				{
					pConfig->iTargetProducingConnPoint = pAssemblyTag1->iInstance;
					pConfig->iTargetProducingConnTagSize = iAssembly1TagSize;
					pConfig->targetProducingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );
				}
				else if ( (pAssemblyPoint1->iType & AssemblyProducing) && (pAssemblyTag1->iType & AssemblyConsuming) )
				{
					pConfig->iTargetProducingConnPoint = pConfig->iTargetConsumingConnPoint;
					pConfig->iTargetConsumingConnPoint = pAssemblyTag1->iInstance;
					pConfig->iTargetConsumingConnTagSize = iAssembly1TagSize;
					pConfig->targetConsumingConnTag = EtIP_malloc( (UINT8*)assembly1Tag, iAssembly1TagSize );						
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------
** connectionGenerateConnPath
**
** Build Connection Path from the configuration parameters.
**---------------------------------------------------------------------------
*/
void connectionGenerateConnPath(char* networkPath, CONNECTION_CFG* pConfig)
{
	UINT8* ptr;
	ETAG_DATATYPE  pAddr;
	UINT16 iTagSize, iTagType;
	char szNetworkPath[MAX_NETWORK_PATH_SIZE];

	/* Build connection path PDU */
	pduRelease( &pConfig->PDU );

	strcpy( szNetworkPath, networkPath );
	pduParseNetworkPath( szNetworkPath, &pConfig->PDU );

	/* Check for any failure to initialize the pdu structure */
	if ( pConfig->PDU.bNumTags > MAX_TAGS )
	{
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConfig->lHostIPAddr, pConfig->lIPAddress,
			"connectionGenerateConnPath Internal Logic FAILURE pConfig->PDU.bNumTags set to %d for connection instance %d",
			pConfig->PDU.bNumTags, pConfig->nInstance );
		pConfig->PDU.bNumTags = 0;
	}
	
	/* Remove any ports and the first IP address. Store IP address separately in Config structure */
	while( pConfig->PDU.bNumTags > 0 )
	{
		pAddr = pduGetTagByIndex(&pConfig->PDU, &iTagType, 0, &iTagSize );
		if ( iTagType == TAG_TYPE_LINK_ADDRESS )
		{
			pConfig->lIPAddress = platformAddrFromPath( (const char*)pAddr, (INT32)iTagSize );
			pduRemoveTagByIndex(&pConfig->PDU, 0);
			break;
		}
		else
			pduRemoveTagByIndex(&pConfig->PDU, 0);
	}

	/* Add key segment */
	ptr = EtIP_malloc((UINT8*)&pConfig->deviceId, sizeof(EtIPDeviceID));
	pduAddTag(&pConfig->PDU, TAG_TYPE_KEY, sizeof(EtIPDeviceID), (ETAG_DATATYPE)ptr);

	if ( pConfig->bProductionTOInhibitInterval )
		pduAddTag(&pConfig->PDU, TAG_TYPE_INHIBIT_INTERVAL, 1, (ETAG_DATATYPE)pConfig->bProductionTOInhibitInterval);

	if ((pConfig->bTransportClass == Class1) || (pConfig->bTransportClass == Class0))
	{
		if ( (pConfig->iTargetConsumingConnPoint != INVALID_CONN_POINT && pConfig->iTargetConsumingConnPoint != 0) ||
			 (pConfig->iTargetProducingConnPoint != INVALID_CONN_POINT && pConfig->iTargetProducingConnPoint != 0) )
		{
			pduAddTag(&pConfig->PDU, TAG_TYPE_CLASS, 1, (ETAG_DATATYPE)ASSEMBLY_CLASS);
		}
	}
	else
	{
		pduAddTag(&pConfig->PDU, TAG_TYPE_CLASS, 1, (ETAG_DATATYPE)ROUTER_CLASS);
	}

	if (pConfig->iTargetConfigConnInstance != INVALID_INSTANCE && pConfig->iTargetConfigConnInstance != 0 &&
		(((pConfig->bTransportClass != Class1) && (pConfig->bTransportClass != Class0)) ||
			(pConfig->iTargetConsumingConnPoint != INVALID_CONN_POINT && pConfig->iTargetConsumingConnPoint != 0) ||
			(pConfig->iTargetProducingConnPoint != INVALID_CONN_POINT && pConfig->iTargetProducingConnPoint != 0) ) )
		{
			pduAddTag(&pConfig->PDU, TAG_TYPE_INSTANCE, (UINT16)(IS_WORD(pConfig->iTargetConfigConnInstance) ? (IS_BYTE(pConfig->iTargetConfigConnInstance) ? 1 : 2) : 4),
						pConfig->iTargetConfigConnInstance);
		}

	if ( pConfig->iTargetConsumingConnPoint != INVALID_CONN_POINT && pConfig->iTargetConsumingConnPoint != 0 )
	{
		pduAddTag(&pConfig->PDU, TAG_TYPE_CONN_POINT, (UINT16)(IS_WORD(pConfig->iTargetConsumingConnPoint) ? (IS_BYTE(pConfig->iTargetConsumingConnPoint) ? 1 : 2) : 4),
					pConfig->iTargetConsumingConnPoint);
	}
	if ( pConfig->iTargetProducingConnPoint != INVALID_CONN_POINT && pConfig->iTargetProducingConnPoint != 0 )
	{
		pduAddTag(&pConfig->PDU, TAG_TYPE_CONN_POINT, (UINT16)(IS_WORD(pConfig->iTargetProducingConnPoint) ? (IS_BYTE(pConfig->iTargetProducingConnPoint) ? 1 : 2) : 4),
					pConfig->iTargetProducingConnPoint);
	}
}


/*---------------------------------------------------------------------------
** connectionConvertFromInternal( )
**
** Convert configuration structure from the internal storage presentation
** where part of the data is stored in the dynamic memory pool to the 
** client recognizable structure.
**---------------------------------------------------------------------------
*/
void connectionConvertFromInternal(CONNECTION_CFG* pcfg, EtIPConnectionConfig* pConfig)
{
	UINT8* pModuleConfigData;

	memset( pConfig, 0, sizeof(EtIPConnectionConfig) );

	pConfig->nInstance = pcfg->nInstance;
	pConfig->bOriginator = pcfg->bOriginator;
	pConfig->iConnectionFlag = pcfg->iConnectionFlag;

	utilBuildNetworkPath((char*)pConfig->networkPath, pcfg->lIPAddress, &pcfg->PDU);

	pConfig->iTargetConfigConnInstance = pcfg->iTargetConfigConnInstance;
	pConfig->iTargetConsumingConnPoint = pcfg->iTargetConsumingConnPoint;
	pConfig->iTargetProducingConnPoint = pcfg->iTargetProducingConnPoint;
	pConfig->iCurrentProducingSize = pcfg->iCurrentProducingSize;
	pConfig->iCurrentConsumingSize = pcfg->iCurrentConsumingSize;
	pConfig->bClass1VariableConsumer = pcfg->bClass1VariableConsumer;
	pConfig->bClass1VariableProducer = pcfg->bClass1VariableProducer;
	pConfig->bRedundantOwner = pcfg->bRedundantOwner;
	pConfig->bReadyOwnershipOutputs = pcfg->bReadyOwnershipOutputs;
	pConfig->bClaimOutputOwnership = pcfg->bClaimOutputOwnership;

	pConfig->lProducingDataRate = pcfg->lProducingDataRate;
	pConfig->lConsumingDataRate = pcfg->lConsumingDataRate;
	pConfig->bProductionOTInhibitInterval = pcfg->bProductionOTInhibitInterval;
	pConfig->bProductionTOInhibitInterval = pcfg->bProductionTOInhibitInterval;
	pConfig->bOutputRunProgramHeader = pcfg->bOutputRunProgramHeader;
	pConfig->bInputRunProgramHeader = pcfg->bInputRunProgramHeader;
	pConfig->iProducingConnectionType = pcfg->iProducingConnectionType;
	pConfig->iConsumingConnectionType = pcfg->iConsumingConnectionType;
	pConfig->iProducingPriority = pcfg->iProducingPriority;
	pConfig->iConsumingPriority = pcfg->iConsumingPriority;
	pConfig->bTransportClass = pcfg->bTransportClass;
	pConfig->bTransportType = pcfg->bTransportType;
	pConfig->bTimeoutMultiplier = pcfg->bTimeoutMultiplier;
	pConfig->iWatchdogTimeoutAction = pcfg->iWatchdogTimeoutAction;
	pConfig->lWatchdogTimeoutReconnectDelay = pcfg->lWatchdogTimeoutReconnectDelay;
	memcpy(&pConfig->deviceId, &pcfg->deviceId, sizeof(EtIPDeviceID));

	pConfig->bOpenPriorityTickTime = pcfg->bOpenPriorityTickTime;
	pConfig->bOpenTimeoutTicks = pcfg->bOpenTimeoutTicks;

	pConfig->iConnectionNameSize = pcfg->iConnectionNameSize;
	if ( pcfg->iConnectionNameSize )
	{
		memcpy(pConfig->connectionName, pcfg->pConnectionName, pcfg->iConnectionNameSize);
		pConfig->connectionName[pcfg->iConnectionNameSize] = 0;
	}

	pConfig->iTargetConsumingConnTagSize = pcfg->iTargetConsumingConnTagSize;
	if ( pcfg->iTargetConsumingConnTagSize )
	{
		memcpy(pConfig->targetConsumingConnTag, pcfg->targetConsumingConnTag, pcfg->iTargetConsumingConnTagSize);
		pConfig->targetConsumingConnTag[pcfg->iTargetConsumingConnTagSize] = 0;
	}

	pConfig->iTargetProducingConnTagSize = pcfg->iTargetProducingConnTagSize;
	if ( pcfg->iTargetProducingConnTagSize )
	{
		memcpy(pConfig->targetProducingConnTag, pcfg->targetProducingConnTag, pcfg->iTargetProducingConnTagSize);
		pConfig->targetProducingConnTag[pcfg->iTargetProducingConnTagSize] = 0;
	}

	pModuleConfigData = (UINT8*)pduGetTagByType( &pcfg->PDU, TAG_TYPE_DATA, 0, &pConfig->iModuleConfig1Size);
	if (pConfig->iModuleConfig1Size > 0)
		memcpy(pConfig->moduleConfig1, pModuleConfigData, pConfig->iModuleConfig1Size);

	pModuleConfigData = (UINT8*)pduGetTagByType( &pcfg->PDU, TAG_TYPE_DATA, 1, &pConfig->iModuleConfig2Size);
	if (pConfig->iModuleConfig2Size > 0)
	{
		memcpy(pConfig->moduleConfig2, pModuleConfigData, pConfig->iModuleConfig2Size);
	}
	else
	{
		pModuleConfigData = (UINT8*)pduGetTagByType( &pcfg->PDU, TAG_TYPE_DATA2, 0, &pConfig->iModuleConfig2Size);
		if (pConfig->iModuleConfig2Size > 0)
			memcpy(pConfig->moduleConfig2, pModuleConfigData, pConfig->iModuleConfig2Size);
	}

	pConfig->lHostIPAddr = pcfg->lHostIPAddr;

#ifdef ET_IP_SCANNER
	pConfig->nOutputScannerOffset = pcfg->nOutputScannerOffset;
	pConfig->nOutputScannerSize = pcfg->iOutputScannerSize;
	pConfig->nInputScannerOffset = pcfg->nInputScannerOffset;
	pConfig->nInputScannerSize = pcfg->iInputScannerSize;

#ifdef EIP_LARGE_MEMORY
	pConfig->nInputScannerTable = pcfg->nInputScannerTable;
	pConfig->nOutputScannerTable = pcfg->nOutputScannerTable;
#endif
#endif

#ifdef ET_IP_MODBUS
	pConfig->iModbusConsumeSize = pcfg->modbusCfg.iModbusConsumeSize;
	pConfig->iModbusProduceSize = pcfg->modbusCfg.iModbusProduceSize;
	if (pcfg->modbusCfg.iModbusAddressSize > 0)
		memcpy(pConfig->modbusNetworkPath, pcfg->modbusCfg.pModbusAddress, pcfg->modbusCfg.iModbusAddressSize);

	pConfig->modbusNetworkPath[pcfg->modbusCfg.iModbusAddressSize] = 0;
#else
	pConfig->iModbusConsumeSize = 0;
	pConfig->iModbusProduceSize = 0;
	pConfig->modbusNetworkPath[0] = 0;
#endif
}

/*---------------------------------------------------------------------------
** connectionIsDataConnPoint( )
**
** Return TRUE if passed instance should correspond to a connection
** point with some data, FALSE otherwise
**---------------------------------------------------------------------------
*/
BOOL connectionIsDataConnPoint( UINT32 iInstance )
{
	if ( iInstance != gHeartbeatConnPoint &&
		 iInstance != gListenOnlyConnPoint &&
		 iInstance != 0 &&
		 iInstance != INVALID_CONN_POINT && 
		 iInstance != INVALID_INSTANCE
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		 && (!assemblyIsScannerInputInstance(iInstance)) &&
			(!assemblyIsScannerOutputInstance(iInstance))
#endif
		 )
		 return TRUE;

	return FALSE;
}

/*---------------------------------------------------------------------------
** connectionIsIOConnection( )
**
** Return TRUE if connection is transport class 0 or 1, FALSE otherwise
**---------------------------------------------------------------------------
*/
BOOL connectionIsIOConnection(CONNECTION* pConnection)
{
	if ((pConnection->cfg.bTransportClass == Class1) || 
		(pConnection->cfg.bTransportClass == Class0))
		return TRUE;

	return FALSE;
}


/*---------------------------------------------------------------------------
** connectionNotifyNetLEDChange( )
**
** Determine if Network LED status has changed and notify the application
**---------------------------------------------------------------------------
*/
void connectionNotifyNetLEDChange()
{
	EtIPNetworkStatus eCurrentState;
#ifdef EIP_NSLED_PER_IP
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	for (i = 0, pTcpipInterface = gsTcpIpInstAttr;
		 i < gnClaimedHostIPAddrCount;
		 i++, pTcpipInterface++)
	{
		eCurrentState = connectionGetCurrentNetLEDState(pTcpipInterface->InterfaceConfig.lIpAddr);
		if (eCurrentState != pTcpipInterface->eSavedNetworkStatus)
		{
			pTcpipInterface->eSavedNetworkStatus = eCurrentState;
			notifyEvent(NM_NETWORK_LED_CHANGE, eCurrentState, pTcpipInterface->InterfaceConfig.lIpAddr);
		}
	}
#else
	eCurrentState = connectionGetCurrentNetLEDState(0);
	if (eCurrentState != geSavedNetworkStatus)
	{
		geSavedNetworkStatus = eCurrentState;
		notifyEvent(NM_NETWORK_LED_CHANGE, eCurrentState, 0);
	}
#endif
}

/*---------------------------------------------------------------------------
** connectionGetCurrentNetLEDState( )
**
** Determine the current state of the Network LED
**---------------------------------------------------------------------------
*/
EtIPNetworkStatus connectionGetCurrentNetLEDState(UINT32 lIPAddress)
{
	CONNECTION *pLoopConnection, *pLoop2Connection;
	EtIPNetworkStatus status;
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	BOOL bIPFound = FALSE;

//EATON_EIP_Changes:Code changed to fix ODVA conformance issue on ACD
#ifdef EIP_ACD
	if (platformIsDuplicateIPAddress(0))
	{
		/* Check for duplicate IP addresses */
		for(i = 0, pTcpipInterface = gsTcpIpInstAttr;
			i < gnClaimedHostIPAddrCount;
			i++, pTcpipInterface++)
		{
			if ((pTcpipInterface->InterfaceConfig.lIpAddr == lIPAddress) ||
				(lIPAddress == 0))
			{
				bIPFound = TRUE;
				if (platformIsDuplicateIPAddress(pTcpipInterface->InterfaceConfig.lIpAddr))
				{
					/* Duplicate found, automatically solid red LED */
					pTcpipInterface->lInterfaceStatus |= TCPIP_STATUS_ACD_STATUS;
				}
			}
		}
		return NetworkStatusRed;
	}
#endif
	//EATON_EIP_Changes: Following code added because of above fix ODVA conformance issue
	//on ACD by Shekhar - 31082016
	//We need to flash NS LED in green if there is no connections. Before this NS LED is always solid Green
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount; 
		i++, pTcpipInterface++)
	{	
		if ((pTcpipInterface->InterfaceConfig.lIpAddr == lIPAddress) ||
			(lIPAddress == 0))
		{
			bIPFound = TRUE;
		}
	}

	if (bIPFound == FALSE)
	{
		/* no IP address */
		return NetworkStatusOff;
	}

	/* initialize Network LED to "no connection" status */
	status = NetworkStatusFlashingGreen;
	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* any established connection (orig/targ, class1/class3) will turn 
		   the Network LED solid green */
		if ((pLoopConnection->lConnectionState == ConnectionEstablished) &&
			((lIPAddress == 0) ||
			 ((lIPAddress == pLoopConnection->cfg.lHostIPAddr) && (pLoopConnection->cfg.bOriginator == TRUE)) ||
			 ((lIPAddress == pLoopConnection->cfg.lIPAddress) && (pLoopConnection->cfg.bOriginator == FALSE))))
			status = NetworkStatusGreen;

		if ((pLoopConnection->cfg.bOriginator == FALSE) &&
			(connectionIsIOConnection(pLoopConnection)) &&
			(pLoopConnection->lConnectionState == ConnectionTimedOut) &&
			((lIPAddress == 0) || (pLoopConnection->cfg.lIPAddress == lIPAddress)) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint != gHeartbeatConnPoint) &&
			(pLoopConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint))
		{
			if (pLoopConnection->cfg.bRedundantOwner)
			{
				/* Check for active RO connection with same connection points */
				for ( pLoop2Connection = gConnections; pLoop2Connection < gpnConnections; pLoop2Connection++ )
				{
					if ((connectionIsIOConnection(pLoop2Connection)) &&
						(pLoop2Connection->cfg.iTargetConsumingConnPoint == pLoopConnection->cfg.iTargetConsumingConnPoint) &&
						(pLoop2Connection->cfg.iTargetProducingConnPoint == pLoopConnection->cfg.iTargetProducingConnPoint) &&
						(pLoop2Connection->cfg.iTargetConfigConnInstance == pLoopConnection->cfg.iTargetConfigConnInstance) &&
						(pLoop2Connection->lConnectionState == ConnectionEstablished) &&
						(pLoop2Connection->cfg.bOriginator == FALSE) &&
						((lIPAddress == 0) || (pLoopConnection->cfg.lIPAddress == lIPAddress)))
					{
						/* found a redundant owner match */
						break;
					}
				}

				if (pLoop2Connection >= gpnConnections)
				{
					/* There are no other Redundant Owner connections to control the output */
					status = NetworkStatusFlashingRed;
					break;
				}
			}
			else
			{
				/* have a timed out Exclusive Owner target connection, overrides other LED states */
				status = NetworkStatusFlashingRed;
				break;
			}
		}
	}

	return status;
}

#ifdef EIP_LARGE_CONFIG_OPT
static void ConnectionProduce(CONNECTION *pConnection)
{
	CONNECTION* pMulticastProducer = NULL;

	if (connectionIsIOConnection(pConnection)) /* For Class 0/1 using UDP packets to transport I/O */
	{
#ifdef ET_IP_MODBUS
		if ((pConnection->cfg.modbusCfg.iModbusConsumeSize == 0) &&
			(pConnection->cfg.modbusCfg.iModbusProduceSize == 0))
		{
#endif
			pMulticastProducer = connectionGetFirstMultiProducer( pConnection );
			ioSendIOPacket( pConnection, pMulticastProducer );
			connectionResetProducingTicks( pConnection );
#ifdef ET_IP_MODBUS
		}
		else
		{
			pMulticastProducer = connectionGetFirstMultiProducer( pConnection );
			eipmbsioSendReadRequest( pConnection, pMulticastProducer);
			connectionResetProducingTicks( pConnection );
		}
#endif
	}
	else if ( pConnection->cfg.bTransportClass == Class3 && /* For Class 3 using TCP packets to transport object request */
			  pConnection->cfg.bOriginator &&
			  !pConnection->bOutstandingClass3Request)
	{
		ucmmSendConnectedRequest( pConnection );
		connectionResetProducingTicks( pConnection );
		pConnection->bOutstandingClass3Request = TRUE;
	}

	pConnection->bTransferImmediately = FALSE;
}

static BOOL OnConnectionProductionTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
	case ConnectionEstablished:
		ConnectionProduce(pConnection);
		return TRUE;

	default :
		break;
	}

	return FALSE;
}

static void OnConnectionInhibitTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
	case ConnectionEstablished:
		/* if there has been a production trigger sometime during the inhibit
		 * period, then produce
		 */
		if (pConnection->bTransferImmediately)
		{
			ConnectionProduce(pConnection);
		}
		break;

	default :
		break;
	}
}

static void OnConnectionTimeoutTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
	case ConnectionEstablished:
		DumpStr3(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"OnConnectionTimeoutTimer TIMED OUT Connection %d CurrentTick %d",
			pConnection->cfg.nInstance, gdwTickCount, 0);

		pConnection->bGeneralStatus = ROUTER_ERROR_FAILURE;
		pConnection->iExtendedStatus = ROUTER_EXT_ERR_CONNECTION_TIMED_OUT;
#ifdef CONNECTION_STATS
		pConnection->iNumTimeouts++;
		gSystemStats.systemStats.iNumTimedOutConnections++;
#endif
		notifyEvent( NM_CONNECTION_TIMED_OUT, pConnection->cfg.nInstance, 0 );
		pConnection->bOutstandingClass3Request = FALSE;
		connectionTimedOut( pConnection);
		connectionNotifyNetLEDChange();
		break;

	default :
		break;
	}
}

static void OnConnectionConfigTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
#ifdef ET_IP_SCANNER
	case ConnectionConfiguring:
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationWaitingForSession:		/* timed out waiting for session to open */
			pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedConnections++;
			pConnection->iNumErrors++;
#endif
			connectionDelayPending(pConnection);	/* trigger any pending with the same IP */

			notifyEvent( NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH, pConnection->cfg.nInstance, 0 );
			connectionTimedOut(pConnection);
			break;

		case ConfigurationWaitingForForwardOpenResponse:	/* timed out waiting for fwd_open response */
			pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedConnections++;
			pConnection->iNumErrors++;
#endif
			/* trigger any pending with the same IP */
			connectionDelayPending(pConnection);

			notifyEvent( NM_CONN_CONFIG_FAILED_NO_RESPONSE, pConnection->cfg.nInstance, 0 );
			connectionTimedOut( pConnection);
			break;
		default:
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"OnConnectionConfigTimer: Invalid config state under ConnectionConfiguring");
			break;
		}
		break;
#endif

#ifdef ET_IP_SCANNER
	case ConnectionClosing:
		switch( pConnection->lConfigurationState )
		{
			case ConfigurationWaitingForForwardCloseResponse:	/* timed out waiting for fwd_close response */
			case ConfigurationClosingTimeOut:					/* delay after timeout */
				/* wait for a bit and trigger other connections that are on the same IP */
				connectionDelayPending( pConnection );

				connectionRemove( pConnection, FALSE );
				break;
			default:
				DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"OnConnectionConfigTimer: Invalid config state under ConnectionClosing");
				break;
		}
		break;
#endif
	default:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"OnConnectionConfigTimer: Invalid connection state");
		break;
	}

}

static void OnConnectionDelayTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
#ifdef ET_IP_SCANNER
	case ConnectionConfiguring:
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationLogged: /* delay before open */
			connListQueueCnxnPollEntry(pConnection);
			break;
		default:
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"OnConnectionDelayTimer: Invalid config state under ConnectionConfiguring");
			break;
		}
		break;
#endif

#ifdef ET_IP_SCANNER
	case ConnectionEstablished:
	case ConnectionClosing:
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationClosing:
			/* delay before close */
			connListQueueCnxnPollEntry(pConnection);
			break;
		default:
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"OnConnectionDelayTimer: Invalid config state under ConnectionClosing");
			break;
		}
		break;
#endif
	default:
		DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"OnConnectionDelayTimer: Invalid connection state, conn %d, state %d/%d", pConnection->cfg.nInstance, pConnection->lConnectionState, pConnection->lConfigurationState);
		break;
	}
}

#ifdef CONNECTION_STATS
static void OnConnectionStatsDelayTimer(CONNECTION *pConnection)
{
	switch( pConnection->lConnectionState )
	{
	case ConnectionEstablished:
		/* turn on stats collection after the initial delay */
		pConnection->bCollectingStats = TRUE;
		pConnection->lPPSResetTick = gdwTickCount; /* Reset count ASAP */
		break;

	default:
		DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"OnConnectionStatsDelayTimer: Invalid connection state, conn %d, state %d/%d\n", pConnection->cfg.nInstance, pConnection->lConnectionState, pConnection->lConfigurationState);
		break;
	}
}
#endif

/*
 * Service connection timers that have expired on the various queues.
 * Returns the number of connections that produced data.
 */
int connectionServiceConnectionTimers()
{
#define MAX_TIMER_PER_TASK  15

	CONNECTION *pConnection;
	CNXN_TIMER_TYPE eTimerType;
	UINT32 ulCount;
	int prodCount = 0;

	ulCount = 0;
	do
	{
		if ((pConnection = connListGetExpiredCnxnProductionTimer()) != NULL)
		{
			if (OnConnectionProductionTimer(pConnection))
				prodCount++;
		}
	} while(pConnection && ++ulCount < MAX_TIMER_PER_TASK
#ifdef EIP_LIMIT_NUM_CONNECTIONS_PROCESSED
			&& ulCount < EIP_LIMIT_NUM_CONNECTIONS_PROCESSED
#endif
			);

	ulCount = 0;
	do
	{
		if ((pConnection = connListGetExpiredCnxnTimeoutTimer()) != NULL)
		{
			OnConnectionTimeoutTimer(pConnection);
		}
	} while(pConnection && ++ulCount < MAX_TIMER_PER_TASK);

	ulCount = 0;
	do
	{
		if ((pConnection = connListGetExpiredCnxnInhibitTimer()) != NULL)
		{
			OnConnectionInhibitTimer(pConnection);
		}
	} while(pConnection && ++ulCount < MAX_TIMER_PER_TASK);

	ulCount = 0;
	do
	{
		if ((pConnection = connListGetExpiredCnxnConfigTimer(&eTimerType)) != NULL)
		{
			switch (eTimerType)
			{
				case CNXN_TIMER_TYPE_CFG_TIMEOUT :
					OnConnectionConfigTimer(pConnection);
					break;
				case CNXN_TIMER_TYPE_START_DELAY :
					OnConnectionDelayTimer(pConnection);
					break;
#ifdef CONNECTION_STATS
				case CNXN_TIMER_TYPE_STATS_DELAY :
					OnConnectionStatsDelayTimer(pConnection);
					break;
#endif
				default:
					DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
						"connectionServiceConnectionTimers: Unknown timer type %d", eTimerType);
					break;
			}
		}
	} while(pConnection && ++ulCount < MAX_TIMER_PER_TASK);

	return prodCount;
}

void ProcessConnectionPoll(CONNECTION *pConnection)
{
#ifdef ET_IP_SCANNER
	SESSION* pSession;
#endif

	if ( pConnection->lConfigurationState == ConfigurationAbortConnection )
	{
		connectionRemove( pConnection, FALSE);
		return;
	}

	/* Check if this connection was scheduled for closing */
	if ( pConnection->lConfigurationState == ConfigurationClosing &&
		 pConnection->lConnectionState != ConnectionClosing )
	{
		/* If we are the ones who initiate a connection drop - issue Forward Close (in the switch below) */
		if ( pConnection->cfg.bOriginator && pConnection->lConnectionState == ConnectionEstablished )
		{
			/* if another connection is being configured on the same IP address leave this
			 * connection alone until the other one is done */
			if ( connectionAnotherPending( pConnection ) )
			{
				connListQueueCnxnPollEntry(pConnection);
				return;
			}

			pConnection->lConnectionState = ConnectionClosing;
		}
		else
		{
			/* trigger any other connections on same IP that are waiting on us */
			connectionDelayPending( pConnection );
			connectionRemove( pConnection, FALSE );
			return;
		}
	}

	switch( pConnection->lConnectionState )
	{
#ifdef ET_IP_SCANNER
	case ConnectionConfiguring:
	{
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationLogged:
		{
			/* If incoming connection scheduled by CC object, but not opened yet - leave it alone */
			if ( !pConnection->cfg.bOriginator )
				break;

			/* if another connection is being configured on the same IP address. wait until it's finished. */
			if ( connectionAnotherPending( pConnection ) )
			{
				/* do nothing - the fwd-open response, or timeout, will trigger a
				 * config delay to start up the close
				 */
				connListQueueCnxnPollEntry(pConnection);
				break;
			}

			/* Check if the session with this server has already been established */
			pSession = sessionFindOutgoing(pConnection->cfg.lIPAddress, pConnection->cfg.lHostIPAddr);

			if (pSession == NULL )  /* need to create a new session */
			{
				pSession = sessionNew(pConnection->cfg.lIPAddress, pConnection->cfg.lHostIPAddr, FALSE);

				if ( pSession == NULL )    /* Maximum number of open sessions has been reached */
				{
					connectionRemove( pConnection, FALSE );
					notifyEvent(NM_SESSION_COUNT_LIMIT_REACHED, 0, 0);
					break;
				}

				DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"connectionService new session logged - Id %d", pSession->nSessionId);

				/* Session will timeout when ForwardOpen request times out */
				pSession->lWaitingForOpenSessionTimeout = gdwTickCount + (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK));

				pConnection->nSessionId = pSession->nSessionId;
			}
			else    /* session exists */
			{
				pConnection->nSessionId = pSession->nSessionId;
			}

			if (pSession->lState == OpenSessionEstablished)
			{
				if ( scanmgrOutgoingConnection(pConnection ) )
				{
					pConnection->lConfigurationState = ConfigurationWaitingForForwardOpenResponse;
					connListQueueCnxnTimer(pConnection, CNXN_TIMER_TYPE_CFG_TIMEOUT, (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK)));
				}
				else
				{
					connectionRemove( pConnection, FALSE );
				}
			}
			else    /* wait for Session to be established */
			{
				connListSetConnectionConfigState(pConnection, ConfigurationWaitingForSession, 0);
			}
		}
		break;

		case ConfigurationWaitingForSession:
		{
			pSession = sessionGetBySessionId( pConnection->nSessionId );

			if ( pSession == NULL)  /* session failed and was removed */
			{
				pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
				gSystemStats.systemStats.iNumFailedConnections++;
				pConnection->iNumErrors++;
#endif
				connectionDelayPending(pConnection);    /* trigger any pending with the same IP */

				notifyEvent( NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH, pConnection->cfg.nInstance, 0 );
				connectionTimedOut(pConnection);
			}

			else if (pSession->lState == OpenSessionEstablished )   /* session has opened */
			{
				connListCancelCnxnTimer(pConnection, CNXN_TIMER_TYPE_CFG_TIMEOUT);

				if ( scanmgrOutgoingConnection(pConnection ) )
				{
					pConnection->lConfigurationState = ConfigurationWaitingForForwardOpenResponse;
					connListQueueCnxnTimer(pConnection, CNXN_TIMER_TYPE_CFG_TIMEOUT, (pConnection->cfg.bOpenTimeoutTicks << (pConnection->cfg.bOpenPriorityTickTime & PRIORITY_TICK_TIME_MASK)));
				}
				else
				{
					connectionRemove( pConnection, FALSE );
				}
			}

			else    /* keep polling for a session status change */
			{
				connListQueueCnxnPollEntry(pConnection);
			}
		}
		break;

		case ConfigurationWaitingForForwardOpenResponse:
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"ProcessConnectionPoll: Invalid config state: ConfigurationWaitingForForwardOpenResponse - should be handled in timer");
			break;

		default:
			break;
		}
	}
	break;

	case ConnectionClosing:
		switch( pConnection->lConfigurationState )
		{
		case ConfigurationClosing:
			/* if another connection is being configured on the same IP address, wait until it's finished. */
			if ( connectionAnotherPending( pConnection ) )
			{
				/* do nothing - the fwd-close response, or timeout, will trigger a
				 * config delay to start up the close
				 */
				connListQueueCnxnPollEntry(pConnection);
			}
			else
			{
				connListQueueCnxnTimer(pConnection, CNXN_TIMER_TYPE_CFG_TIMEOUT, FORWARD_CLOSE_TIMEOUT);
				pConnection->bOutstandingClass3Request = FALSE;
				scanmgrIssueFwdClose( pConnection );
				pConnection->lConfigurationState = ConfigurationWaitingForForwardCloseResponse;
			}
			break;

		case ConfigurationWaitingForForwardCloseResponse:
		case ConfigurationClosingTimeOut:
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"ProcessConnectionPoll: Invalid config state: ConfigurationWaitingForForwardCloseResponse - should be handled in timer");
			break;

		default:
			break;
		}
		break;
#endif
	case ConnectionEstablished:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
			"ProcessConnectionPoll: Invalid connection state: Established - should be handled in timer");
		break;

	default:
		break;
	}
}

void connectionServiceConnectionPolls()
{
#define MAX_POLL_PER_TASK 15

	CONNECTION *pConnection;
	UINT32 ulCount = connListGetCnxnPollCount();

	/* limit the number handled per task iteration */
	if (ulCount > MAX_POLL_PER_TASK)
		ulCount = MAX_POLL_PER_TASK;

	if (ulCount)
	{
		pConnection = connListGetNextCnxnPollEntry();
		while (pConnection)
		{
			ProcessConnectionPoll(pConnection);

			/* only process the entries that were on the queue when we
			 * came in.  we don't want to start processing entries that
			 * were added while in here or we may never leave.
			 */
			if (--ulCount == 0)
				break;

			pConnection = connListGetNextCnxnPollEntry();
		}
	}
}

#endif /* EIP_LARGE_CONFIG_OPT */
