/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPSESSN.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Session collection implementation
**
*****************************************************************************
*/

#include "eipinc.h"

SESSION* gpnSessions;				/* Number of the open sessions */

#ifdef EIP_LARGE_BUF_ON_HEAP
SESSION* gSessions;					/* Session objects array */
#else
SESSION  gSessions[MAX_SESSIONS];	/* Session objects array */
#endif

static INT32 gnSessionIndex;		/* Id to be assigned to the next opened session */

/*---------------------------------------------------------------------------
** sessionInit( )
**
** Initialize the session array
**---------------------------------------------------------------------------
*/
void sessionInit(void)
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	gSessions = (SESSION *)EtIP_mallocHeap(MAX_SESSIONS * sizeof(SESSION ));
	if (gSessions == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gSessions to %d entries\n", MAX_SESSIONS);
		return;
	}
#endif

	gpnSessions = gSessions;
	gnSessionIndex = 1;
}

/*---------------------------------------------------------------------------
** sessionInitialize( )
**
** Initialize the session object
**---------------------------------------------------------------------------
*/
static void sessionInitialize( SESSION* pSession )
{
	memset( pSession, 0, sizeof(SESSION) );

	pSession->lSocket = (SOCKET)INVALID_SOCKET;
	pSession->nSessionId = INVALID_SESSION_INDEX;

	pSession->iClientEncapProtocolVersion = ENCAP_PROTOCOL_VERSION;
	pSession->iClientEncapProtocolFlags = 0;

	pSession->lHostIPAddr = INADDR_NONE;
	pSession->lClientIPAddr = INADDR_NONE;
}

/*---------------------------------------------------------------------------
** sessionNew( )
**
** Allocate and initialize a new session object in the array
**---------------------------------------------------------------------------
*/
SESSION* sessionNew( UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming )
{
	UINT32 i;
	SESSION* pSession;
	char szDirection[9];
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	if ( gpnSessions >= &gSessions[MAX_SESSIONS] )		/* Limit of the open sessions reached */
		return NULL;

	pSession = gpnSessions++;

	sessionInitialize( pSession );
	
	/* Make sure we don't have a very old session that uses our Id */
	while (sessionGetBySessionId(gnSessionIndex) != NULL)
	{
		if ( ++gnSessionIndex == INDEX_END )			/* Wrap the unique session identifier if need to */
			gnSessionIndex = 1;
	}

	pSession->nSessionId = gnSessionIndex++;
	if ( gnSessionIndex == INDEX_END )					/* Wrap the unique session identifier if need to */
		gnSessionIndex = 1;

	if ( bIncoming )
	{
		pSession->lState = OpenSessionWaitingForRegister;
		strcpy(szDirection, "incoming");
	}
	else
	{
		pSession->lClientIPAddr = lIPAddress;
		pSession->lState = OpenSessionLogged;
		strcpy(szDirection,"outgoing");
	}

#ifndef EIP_NO_ORIGINATE_UCMM
	pSession->bIncoming = bIncoming;
#endif
	pSession->lHostIPAddr = lHostIPAddr;

	/* Find the Encapsulation (session) timeout for this IP address */
	for ( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount;
		i++, pTcpipInterface++)
	{
		if (pTcpipInterface->InterfaceConfig.lIpAddr == lHostIPAddr )
		{
			pSession->lInactivityTimeout = pTcpipInterface->InterfaceConfig.iEncapTimeout*1000;
			break;
		}
	}

	DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, lHostIPAddr, lIPAddress,
				"Creating %s session %d", szDirection, pSession->nSessionId); 

	return pSession;
}

/*---------------------------------------------------------------------------
** sessionRemove( )
**
** Remove the session from the array
**---------------------------------------------------------------------------
*/
void sessionRemove( SESSION* pSessionRemove, BOOL bRelogOutgoingRequests )
{
	SESSION* pSession;
	REQUEST* pRequest;
	CONNECTION* pConnection;
	INT32 nSessionId = pSessionRemove->nSessionId;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSessionRemove->lHostIPAddr, pSessionRemove->lClientIPAddr,
			"sessionRemove %d", pSessionRemove->nSessionId);

	if ( gpnSessions > gSessions)
		gpnSessions--;									/* Adjust the total number of open sessions */

	if ( pSessionRemove->lSocket != (SOCKET)INVALID_SOCKET )
	{
		platformCloseSocket(pSessionRemove->lSocket);	/* Close Winsock TCP socket */
		pSessionRemove->lSocket = (SOCKET)INVALID_SOCKET;
	}

	if ( pSessionRemove->pPartialRecvPacket != NULL )
		EtIP_free(&pSessionRemove->pPartialRecvPacket, &pSessionRemove->iPartialRecvPacketSize );

	if ( pSessionRemove->pPartialSendPacket != NULL )
		EtIP_free( &pSessionRemove->pPartialSendPacket, &pSessionRemove->iPartialSendPacketSize );

	/* Shift the sessions with the higher index to fill in the void */
	for( pSession = pSessionRemove; pSession < gpnSessions; pSession++ )
		memcpy( pSession, pSession+1, sizeof(SESSION) );

	for( pConnection = gpnConnections-1; pConnection >= gConnections; pConnection-- )
	{
		if ( pConnection->nSessionId == nSessionId )
		{
			pConnection->nSessionId = INVALID_SESSION_INDEX;

			if (pConnection->lConnectionState == ConnectionEstablished)
			{
				/* only close class3 connections */
				if (pConnection->cfg.bTransportClass == Class3)
				{
					pConnection->bGeneralStatus = ROUTER_ERROR_FAILURE;
					pConnection->iExtendedStatus = ROUTER_EXT_ERR_CONNECTION_TIMED_OUT;
#ifdef CONNECTION_STATS
					pConnection->iNumTimeouts++;
					gSystemStats.systemStats.iNumTimedOutConnections++;
#endif
					notifyEvent( NM_CONNECTION_TIMED_OUT, pConnection->cfg.nInstance, 0 );
					connectionTimedOut( pConnection);
					connectionNotifyNetLEDChange();
				}
			}
#ifdef ET_IP_SCANNER
			else if ( pConnection->lConnectionState == ConnectionConfiguring && pConnection->cfg.bOriginator &&
					( pConnection->lConfigurationState == ConfigurationLogged || pConnection->lConfigurationState == ConfigurationWaitingForSession ||
						pConnection->lConfigurationState == ConfigurationWaitingForForwardOpenResponse ) )
			{
				pConnection->lConfigurationState = ConfigurationFailedInvalidNetworkPath;
#ifdef CONNECTION_STATS
				gSystemStats.systemStats.iNumFailedConnections++;
#endif
				notifyEvent( NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH, pConnection->cfg.nInstance, 0 );
				connectionTimedOut( pConnection);
			}
#endif
		}
	}

	for( pRequest = (gpnRequests - 1); pRequest >= gRequests; pRequest-- )
	{
		if ( pRequest->nSessionId == nSessionId )
		{
			pRequest->nSessionId = INVALID_SESSION_INDEX;

#ifndef EIP_NO_ORIGINATE_UCMM
			if ( pRequest->bIncoming )
				requestRemove( pRequest );
			else if ( bRelogOutgoingRequests && pRequest->nState != REQUEST_RESPONSE_RECEIVED )			/* Relog outstanding explicit requests if appropriate */
				pRequest->nState = REQUEST_LOGGED;
#endif
		}
	}
}

/*---------------------------------------------------------------------------
** sessionRemoveAll( )
**
** Close and remove all open sessions
**---------------------------------------------------------------------------
*/
void sessionRemoveAll(void)
{
	SESSION* pSession;

	for( pSession = gpnSessions - 1; pSession >= gSessions; pSession-- )
	{
		sessionRemove( pSession, FALSE );
	}

	gpnSessions = gSessions;
}

/*---------------------------------------------------------------------------
** sessionService( )
**
** Service a particular session.  Return FALSE if session is removed
**---------------------------------------------------------------------------
*/
BOOL sessionService( SESSION* pSession )
{
#if !defined(SYNCHRONOUS_CONNECTION) && !defined(EIP_NO_ORIGINATE_UCMM)
	BOOL bTCPConnectionSuccessfullyCompleted = FALSE;
#endif
	BOOL bHandleMoreData = TRUE;
	INT32 iRetSocketRecv;

	switch( pSession->lState )
	{

#ifndef EIP_NO_ORIGINATE_UCMM

/* if there is a separate thread for establishing a new TCP session */
#if !defined(SYNCHRONOUS_CONNECTION)

	case OpenSessionLogged:
		if (!socketStartTCPConnection(pSession))
		{
			sessionRemove(pSession, FALSE);
			return FALSE;
		}

		pSession->lState = OpenSessionWaitingForTCPConnection;
		break;

	case OpenSessionWaitingForTCPConnection:
		if (!socketContinueTCPConnection(pSession, &bTCPConnectionSuccessfullyCompleted))
		{
			sessionRemove(pSession, FALSE);
			return FALSE;
		}

		if ( bTCPConnectionSuccessfullyCompleted )
			pSession->lState = OpenSessionTCPConnectionEstablished;
		else if ( IS_TICK_GREATER(gdwTickCount, pSession->lWaitingForOpenSessionTimeout ) )
		{
			sessionRemove( pSession, FALSE );
			return FALSE;
		}
		break;

#else	/* if there is no separate thread for establishing a new TCP session */

	case OpenSessionLogged:
	case OpenSessionWaitingForTCPConnection:
		if ( IS_TICK_GREATER(gdwTickCount, pSession->lWaitingForOpenSessionTimeout ) )
		{
			sessionRemove( pSession, FALSE );
			return FALSE;
		}
		break;
#endif

	case OpenSessionTCPConnectionEstablished:
		if ((pSession->lInactivityTimeout > 0) &&
			(IS_TICK_GREATER(gdwTickCount, pSession->lWaitingForOpenSessionTimeout )) )
		{
			sessionRemove( pSession, FALSE );
			return FALSE;
		}

		bHandleMoreData = ucmmIssueRegisterSession( pSession );
		if (bHandleMoreData)
			pSession->lState = OpenSessionWaitingForRegisterResponse;
		break;
#endif /* EIP_NO_ORIGINATE_UCMM */

	case OpenSessionWaitingForRegisterResponse:
	case OpenSessionWaitingForRegister:
		if ((pSession->lInactivityTimeout > 0) &&
			(IS_TICK_GREATER(gdwTickCount, pSession->lWaitingForOpenSessionTimeout )) )
		{
			sessionRemove( pSession, FALSE );
			return FALSE;
		}

		/* break statement is intentionally missing */

	case OpenSessionEstablished:
		if ( pSession->iPartialSendPacketSize )
			bHandleMoreData = socketEncapSendPartial( pSession );

		while (bHandleMoreData)
		{
			iRetSocketRecv = socketEncapRecv( pSession );
			if (iRetSocketRecv <= 0 )
			{
				if (iRetSocketRecv == ERROR_STATUS)
					return FALSE;

				break;
			}

			sessionResetTimeout(pSession);
			bHandleMoreData = ucmmProcessEncapMsg( pSession );    /* Process TCP data */
		}

		/* ensure the session wasn't removed from under us */
		if (bHandleMoreData)
		{
			if ((pSession->lInactivityTimeout > 0) &&
				(IS_TICK_GREATER(gdwTickCount, pSession->lWaitingForOpenSessionTimeout )) )
			{
				if (sessionIsIdle(pSession, INVALID_CONN_INSTANCE, INVALID_REQUEST_INDEX))
				{
					/* session has timed out from inactivity */
					ucmmIssueUnRegisterSession(pSession);
					sessionRemove(pSession, FALSE );
					return FALSE;
				}
				else
				{
					/* Give the session a little more time with any outstanding requests
					   or if a connection is open */
					sessionResetTimeout(pSession);
				}
			}
		}

		return bHandleMoreData;

	default:
		break;
	}

	return TRUE;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** sessionFindOutgoing( )
**
** Return the session index for the particular client IP address
**---------------------------------------------------------------------------
*/
SESSION* sessionFindOutgoing(UINT32 lIPAddress, UINT32 lHostIPAddr)
{
	SESSION* pSession;

	for( pSession = gSessions; pSession < gpnSessions; pSession++ )
	{
		if ( pSession->lClientIPAddr == lIPAddress &&
			pSession->lHostIPAddr == lHostIPAddr &&
			!pSession->bIncoming)
			return pSession;
	}
	return NULL;
}
#endif

/*---------------------------------------------------------------------------
** sessionIsIdle( )
**
** Return TRUE if there no active connections or requests for the specified
** session. Otherwise return FALSE.
**---------------------------------------------------------------------------
*/
BOOL sessionIsIdle(SESSION* pSession, INT32 nConnectionIdExclude, INT32 nRequestIdExclude )
{
	REQUEST* pRequest;
	CONNECTION* pConnection;

	for( pConnection = gpnConnections-1; pConnection >= gConnections; pConnection-- )
	{
		if ( pConnection->nSessionId == pSession->nSessionId &&
			( pConnection->cfg.bTransportClass == Class3 ||
			 pConnection->nKeepTCPOpenForActiveCorrespondingUDP == KeepTCPOpenStateTrue ||
			( pConnection->nKeepTCPOpenForActiveCorrespondingUDP == KeepTCPOpenStateUnknown &&
			 gnKeepTCPOpenForActiveCorrespondingUDP == KeepTCPOpenStateTrue ) ) &&
			( nConnectionIdExclude == (INT32)INVALID_CONN_INSTANCE || pConnection->cfg.nInstance != nConnectionIdExclude ) )
				return FALSE;
	}

	for( pRequest = gpnRequests-1; pRequest >= gRequests; pRequest-- )
	{
		if ((pRequest->nSessionId == pSession->nSessionId) &&
			((nRequestIdExclude == INVALID_REQUEST_INDEX) || (pRequest->nIndex != nRequestIdExclude )))
				 return FALSE;
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** sessionFindAddressEstablished( )
**
** Return the session index for the particular client IP address if the
** session is in the established state
**---------------------------------------------------------------------------
*/

SESSION* sessionFindAddressEstablished(UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming)
{
	SESSION* pSession;

	for( pSession = gSessions; pSession < gpnSessions; pSession++ )
	{
		if ( pSession->lClientIPAddr == lIPAddress &&
			pSession->lHostIPAddr == lHostIPAddr &&
#ifndef EIP_NO_ORIGINATE_UCMM
			(pSession->bIncoming  == bIncoming) &&
#endif
			pSession->lState == OpenSessionEstablished )
				return pSession;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** sessionGetBySessionId( )
**
** Returns session pointer based on the session id.
** Returns NULL if the session id was not found.
**---------------------------------------------------------------------------
*/
SESSION* sessionGetBySessionId(INT32 nId)
{
	SESSION* pSession;

	if ( nId == INVALID_SESSION_INDEX )
		return NULL;

	for ( pSession = gSessions; pSession < gpnSessions; pSession++ )
	{
		if ( pSession->nSessionId == nId )
			return pSession;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** sessionResetTimeout( )
**
** Resets the timeout for the TCP connection for a session when there is
** activity.  If the timeout is reached, the session is removed
**---------------------------------------------------------------------------
*/
void sessionResetTimeout( SESSION* pSession )
{
	pSession->lWaitingForOpenSessionTimeout = gdwTickCount+pSession->lInactivityTimeout;
}
