/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPSCKT.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet Socket interface.
**
*****************************************************************************
*/

#include "eipinc.h"

UINT32 gnHostIPAddrCount;					/* Number of host addresses */
UINT32 galHostIPAddr[MAX_HOST_ADDRESSES];	/* Host address array */

UINT32 gnClaimedHostIPAddrCount;			/* Number of claimed host addresses */

#ifdef EIP_LARGE_BUF_ON_HEAP
UINT8* gmsgBuf;								/* Buffer storing incoming and outgoing packets */
#else
UINT8 gmsgBuf[MAX_PACKET_SIZE];				/* Buffer storing incoming and outgoing packets */
#endif

/*---------------------------------------------------------------------------
** socketInit( )
**
** Initialize Winsock library and socket object
**---------------------------------------------------------------------------
*/
void socketInit(void)
{
	INT32 i;

#ifdef EIP_LARGE_BUF_ON_HEAP
	gmsgBuf = EtIP_mallocHeap(MAX_PACKET_SIZE * sizeof(UINT8 ));
	if (gmsgBuf == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gmsgBuf to %d bytes\n", MAX_PACKET_SIZE);
		return;
	}
#endif

	/* Initialize network card IP address array */
	for( i = 0; i < MAX_HOST_ADDRESSES; i++ )
	{
		galHostIPAddr[i] = INADDR_NONE;
	}

	platformSocketLibInit();

	platformSocketInitTargetData();					/* Initialize our socket target data */
}

#ifndef EIP_CUSTOM_SOCKET
/*---------------------------------------------------------------------------
** socketPostListen( )
**
** Prepare the claimed TCP listening sockets. It will be reused when 
** waiting for the new TCP incoming connections.
** nIndex is the index of the new claimed host IP address we should 
** use to create a new TCP wait socket.
**---------------------------------------------------------------------------
*/
void socketPostListen(SOCKET* pWaitSocket, UINT32 lClaimedHostIPAddr, UINT16 iPort)
{
	UINT32 lFlagTrue = (UINT32)~0;
	SOCKET lSocket;
	INT32 status;
	struct sockaddr_in sSrv_addr;
	UINT32 lSize_buf;
	UINT32 lAddrLen;

	/* Get a handle for a new TCP socket */
	if ( ( lSocket = platformSocket( PF_INET, SOCK_STREAM, 0 ) ) == (SOCKET)INVALID_SOCKET )
	{
		/* Socket error.  Without a socket we cannot do anything */
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "socketPostListen: failed calling socket function");
		notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );
		return;
	}

	/* Configure the socket to listen to any potential client coming on the Ethernet/IP TCP port */
	platformSetSockaddrIn( &sSrv_addr, iPort, lClaimedHostIPAddr );

	/* The socket may be bound to an address which is already in use */
	status = platformSetSockOpt( lSocket, SOL_SOCKET, SO_REUSEADDR, ( char * ) &lFlagTrue, sizeof( lFlagTrue ) );
	if ( status == SOCKET_ERROR )
	{
		/* While some of the options are non-critical, having them fail this
		 early in the game should be a cause for general alarm */
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "socketPostListen: failed calling platformSetSockOpt SO_REUSEADDR");
	}

	/* Keepalives are being sent */
	status = platformSetSockOpt( lSocket, SOL_SOCKET, SO_KEEPALIVE, ( char * ) &lFlagTrue, sizeof( lFlagTrue ) );
	if ( status == SOCKET_ERROR )
	{
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "socketPostListen: failed calling platformSetSockOpt SO_KEEPALIVE");
	}

	/* Get a size buffer at least large enough for a full packet, and also
	   keep it a power of 2. Keep it at least 4k in size */
	lSize_buf = SMALL_BUFFER_SIZE;
	status = platformSetSockBufSize( lSocket, SOL_SOCKET, SO_SNDBUF, ( char * ) &lSize_buf, sizeof( lSize_buf ) );
	if ( status == SOCKET_ERROR )
	{
		/* While some of the options are non-critical, having them fail this
		   early in the game should be a cause for general alarm */
		status = platformGetLastSocketError(lSocket);
		DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "Could not set send buffer size: error %d", status);
	}

	lSize_buf = SMALL_BUFFER_SIZE;
	status = platformSetSockBufSize( lSocket, SOL_SOCKET, SO_RCVBUF, ( char * ) &lSize_buf, sizeof( lSize_buf ) );
	if ( status == SOCKET_ERROR )
	{
		/* While some of the options are non-critical, having them fail this
		   early in the game should be a cause for general alarm */
		status = platformGetLastSocketError(lSocket);
		DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "Could not set receive buffer size: error %d", status);
	}

	/* Bind the server to Ethernet/IP TCP port */
	if ( platformBind( lSocket, ( struct sockaddr * ) & sSrv_addr, sizeof( sSrv_addr ) ) == SOCKET_ERROR )
	{
		/* If we can't bind, no one can conect, fatal out */
		notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "socketPostListen: failed calling platformBind function");
		platformCloseSocket(lSocket);
		(*pWaitSocket) = INVALID_SOCKET;
		return;
	}

	/* Go to listening mode */
	if ( platformListen( lSocket, 5 ) == SOCKET_ERROR )
	{
		notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "socketPostListen: failed calling platformListen function");
		platformCloseSocket(lSocket);
		(*pWaitSocket) = INVALID_SOCKET;
		return;
	}

	(*pWaitSocket) = lSocket;

	lAddrLen = 1;
	platformIoctl(*pWaitSocket, FIONBIO, &lAddrLen);
}

/*---------------------------------------------------------------------------
** socketCheckIncomingSession( )
**
** Check if we have the new incoming TCP session
**---------------------------------------------------------------------------
*/

void socketCheckIncomingSession()
{
	struct sockaddr_in  sCli_addr;
#ifdef SO_DONTLINGER
	BOOL bLinger = TRUE;
#else
	struct linger sLinger = {FALSE, 0L}; 
#endif
	SOCKET lSocket;
	UINT32 lAddrLen;
	INT32 nStatus;
	SESSION* pSession;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig;

	/* Make sure the number of open sessions is under the limit */
	if ( gpnSessions >= &gSessions[MAX_SESSIONS] )
		return;

	/* check all listening sockets for incomming requests */
	for(pTcpIpConfig = gsTcpIpInstAttr;
		pTcpIpConfig < &gsTcpIpInstAttr[gnClaimedHostIPAddrCount];
		pTcpIpConfig++)
	{
#ifdef EIP_ACD
		if (platformIsDuplicateIPAddress(pTcpIpConfig->InterfaceConfig.lIpAddr))
			continue;
#endif

		/* set only the prepared sockets */
		if(pTcpIpConfig->lPassiveWaitSocket == (SOCKET)INVALID_SOCKET )
			continue;

		platformSetSockaddrIn( &sCli_addr, 0, htonl( INADDR_ANY ) );

		lAddrLen = sizeof( sCli_addr );

		/* Check for the connect request from the client */
		lSocket = platformAccept(pTcpIpConfig->lPassiveWaitSocket, ( struct sockaddr * ) &sCli_addr, &lAddrLen );

		if ( lSocket == (SOCKET)INVALID_SOCKET )
			continue;

#ifdef SO_DONTLINGER
		/* Set the socket options to SO_DONTLINGER for the new socket that we got us */
		nStatus = platformSetSockOpt(lSocket, SOL_SOCKET, SO_DONTLINGER, (char*)&bLinger, sizeof( bLinger ) );
#else
		/* Set the socket options to "don't linger" for the new socket that we got us */
		nStatus = platformSetSockOpt(lSocket, SOL_SOCKET, SO_LINGER, (char*)&sLinger, sizeof(sLinger) );
#endif

		DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, platformGetInternetAddress(sCli_addr.sin_addr),
				"socketCheckIncomingSession opened new incoming socket 0x%x", lSocket);

		/* Set socket mode to non-blocking */
		lAddrLen = 1;
		nStatus = platformIoctl(lSocket, FIONBIO, &lAddrLen);

		if ( nStatus == SOCKET_ERROR )			/* Non-blocking mode is absolutely required */
		{
			notifyEvent(NM_ERROR_SETTING_SOCKET_TO_NONBLOCKING, 0, 0);
			platformCloseSocket(lSocket);
			continue;
		}

#ifdef EIP_QOS
		/* Set QoS value */
		if (platformSetDSCPValue(lSocket, pTcpIpConfig->QoSConfig.bDSCPExplicit) == SOCKET_ERROR)
		{
			notifyEvent(NM_ERROR_USING_WINSOCK, 0, 0);
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pTcpIpConfig->InterfaceConfig.lIpAddr, platformGetInternetAddress(sCli_addr.sin_addr), "socketCheckIncomingSession error calling platformSetDSCPValue function");
		}
#endif

		/* Sleep for a tick - let winsock realize it has a new socket */
		platformSleep(1);

		/* Allocate new session object */
		pSession = sessionNew( platformGetInternetAddress(sCli_addr.sin_addr), pTcpIpConfig->InterfaceConfig.lIpAddr, TRUE);

		if ( pSession == NULL )    /* Exceeded maximum number of open sessions */
		{
			DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, 0, 0, "socketCheckIncomingSession exceeded maximum number of open sessions");
			platformCloseSocket(lSocket);
			continue;
		}

		DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, platformGetInternetAddress(sCli_addr.sin_addr), pTcpIpConfig->InterfaceConfig.lIpAddr,
			"New incoming session %d accepted", pSession->nSessionId);

		/* Session will timeout only after inactivity */
		sessionResetTimeout(pSession);

		/* Save the info for the newly connected socket */
		pSession->lSocket = lSocket;
		pSession->lClientIPAddr = platformGetInternetAddress(sCli_addr.sin_addr);

#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumTCPConnectionsUsed++;
		if (gSystemStats.systemStats.iMaxTCPConnectionsUsed < (gpnSessions-gSessions))
			gSystemStats.systemStats.iMaxTCPConnectionsUsed = (UINT16)(gpnSessions-gSessions);
#endif
	}

}

#endif /* EIP_CUSTOM_SOCKET */

#if !defined SYNCHRONOUS_CONNECTION && !defined EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** socketStartTCPConnection( )
**
** Starts a TCP connection process
**---------------------------------------------------------------------------
*/
BOOL socketStartTCPConnection( SESSION* pSession )
{
	UINT32 lOptSet = 1;
	struct sockaddr_in sCli_addr;
	INT32 nStatus;
	UINT32 lAddrLen;
	UINT32 lSize;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	/* Get a handle to the new TCP socket */
	if ( ( pSession->lSocket = platformSocket( PF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
	{
		notifyEvent(NM_ERROR_USING_WINSOCK, 0, 0);
		return FALSE;
	}

	/* Keepalives are being sent */
	if ( ( nStatus = platformSetSockOpt( pSession->lSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&lOptSet, sizeof(lOptSet) ) ) == SOCKET_ERROR )
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_KEEPALIVE");

	/* Allocate big enough send and receive buffers */
	lSize = SMALL_BUFFER_SIZE;
	if ( ( nStatus = platformSetSockBufSize( pSession->lSocket, SOL_SOCKET, SO_SNDBUF, (char *)&lSize, sizeof(lSize) ) ) == SOCKET_ERROR )
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_SNDBUF");
	if ( ( nStatus = platformSetSockBufSize( pSession->lSocket, SOL_SOCKET, SO_RCVBUF, (char *)&lSize, sizeof(lSize) ) ) == SOCKET_ERROR )
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error setting option SO_RCVBUF");

	/* Bind the socket */
	platformSetSockaddrIn( &sCli_addr, 0, pSession->lHostIPAddr );

	if ( platformBind( pSession->lSocket, ( struct sockaddr * ) & sCli_addr, sizeof( sCli_addr ) ) == SOCKET_ERROR )
	{
		notifyEvent(NM_ERROR_USING_WINSOCK, 0, 0);
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error calling platformBind function");
		platformCloseSocket(pSession->lSocket);
		return FALSE;
	}

	/* Get the IP address to use */
	pTcpipInterface = tcpipFindClaimedHostAddress(pSession->lHostIPAddr);
	if (pTcpipInterface == NULL)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "INTERNAL ERROR: IP address not claimed");
		platformCloseSocket(pSession->lSocket);
		return FALSE;
	}

#ifdef EIP_QOS
	/* Set QoS value */
	if (platformSetDSCPValue(pSession->lSocket, pTcpipInterface->QoSConfig.bDSCPExplicit) == SOCKET_ERROR)
	{
		notifyEvent(NM_ERROR_USING_WINSOCK, 0, 0);
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "socketStartTCPConnection error calling platformSetDSCPValue function");
		platformCloseSocket(pSession->lSocket);
		return FALSE;
	}
#endif

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"socketStartTCPConnection connecting using socket 0x%x", pSession->lSocket);

	/* Set socket mode to non-blocking */
	lAddrLen = 1;
	nStatus = platformIoctl(pSession->lSocket, FIONBIO, &lAddrLen);

	platformSetSockaddrIn( &sCli_addr, pTcpipInterface->iPort, pSession->lClientIPAddr );

	/* Connect to the listening server */
	platformConnect( pSession->lSocket, ( struct sockaddr * )&sCli_addr, sizeof( sCli_addr ));

	return TRUE;

}

/*---------------------------------------------------------------------------
** socketContinueTCPConnection( )
**
** Check the progress of the TCP connection request
**---------------------------------------------------------------------------
*/

BOOL socketContinueTCPConnection( SESSION* pSession, BOOL* bTCPConnectionSuccessfullyCompleted ) 
{
	INT32 nfds;
	struct timeval timeout;
	fd_set writeSet, exceptionSet;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	FD_ZERO( &writeSet );
	FD_ZERO( &exceptionSet );

	FD_SET( pSession->lSocket, &writeSet );
	FD_SET( pSession->lSocket, &exceptionSet );

	nfds = (INT32)(pSession->lSocket + 1);

	platformSelect( nfds, NULL, &writeSet, &exceptionSet, &timeout );

	if ( FD_ISSET( pSession->lSocket, &exceptionSet ) )
		return FALSE;

	*bTCPConnectionSuccessfullyCompleted = (FD_ISSET( pSession->lSocket, &writeSet ) != 0);

#ifdef CONNECTION_STATS
	if (*bTCPConnectionSuccessfullyCompleted)
	{
		gSystemStats.systemStats.iNumTCPConnectionsUsed++;
		if (gSystemStats.systemStats.iMaxTCPConnectionsUsed < (gpnSessions-gSessions))
			gSystemStats.systemStats.iMaxTCPConnectionsUsed = (UINT16)(gpnSessions-gSessions);
	}
#endif

	return TRUE;
}

#endif /* !defined SYNCHRONOUS_CONNECTION && !defined EIP_NO_ORIGINATE_UCMM */

/*---------------------------------------------------------------------------
** socketAdjustEncapHeader( )
**
** Flip the fields in the Encap header if big-endian platform is used.
**---------------------------------------------------------------------------
*/
void socketAdjustEncapHeader(ENCAPH* pHdr)
{
	ENCAP_CVT_HS (pHdr->iLength);
	ENCAP_CVT_HS (pHdr->iCommand);
	ENCAP_CVT_HL (pHdr->lStatus);
	ENCAP_CVT_HL (pHdr->lContext1);
	ENCAP_CVT_HL (pHdr->lContext2);
	ENCAP_CVT_HL (pHdr->lOpt);
}

/*---------------------------------------------------------------------------
** socketEncapRecv( )
**
** Check if we received some data on one of our TCP sockets.
** If yes, read it and return the session index indicating to the main scanner 
** task that we got new TCP data for that session.
**---------------------------------------------------------------------------
*/
INT32 socketEncapRecv( SESSION* pSession )
{
	INT32 lBytesReceived = 0;
	INT32 lBytesRequested;
	INT32 ltmpBytesReceived = 0;
	INT32 lBytesToRead = 0;
	INT32 lOffset = 0;
	UINT16 iCommand;
	INT32 ulSockError;
	ENCAPH* pHdr = (ENCAPH*)gmsgBuf;
	if (pSession->lSocket == INVALID_SOCKET)
	{
		sessionRemove(pSession, FALSE);
		return ERROR_STATUS;
	}

	/* Check for any partial packets received with the previous tick read */
	if ( pSession->pPartialRecvPacket != NULL )
	{
		if ( IS_TICK_GREATER(gdwTickCount, pSession->lPartialRecvPacketTimeoutTick) )
		{
			/* Partial packet is old and is being discarded */

			/* Check first that it is for NOP cmd as for NOP only first ENCAPH_SIZE are saved in dynamic memory */
			if ( pSession->iPartialRecvPacketSize >= 2 )
			{
				iCommand = _SWAPS_(((ENCAPH*)pSession->pPartialRecvPacket)->iCommand);
				if (iCommand == ENCAP_CMD_NOOP)
				{
					pSession->iPartialRecvPacketSize = (UINT16)PLATFORM_MIN(pSession->iPartialRecvPacketSize, ENCAPH_SIZE);
				}
			}
		}
		else
		{
			lBytesReceived = pSession->iPartialRecvPacketSize;

			if ( pSession->iPartialRecvPacketSize >= 2 )
			{
				iCommand = _SWAPS_(((ENCAPH*)pSession->pPartialRecvPacket)->iCommand);
				if (iCommand == ENCAP_CMD_NOOP)
				{
					memcpy( gmsgBuf, pSession->pPartialRecvPacket, PLATFORM_MIN(lBytesReceived, ENCAPH_SIZE));
					pSession->iPartialRecvPacketSize = (UINT16)PLATFORM_MIN(lBytesReceived, ENCAPH_SIZE);
				}
				else
					memcpy( gmsgBuf, pSession->pPartialRecvPacket, lBytesReceived );
			}
			else
			{
				memcpy( gmsgBuf, pSession->pPartialRecvPacket, lBytesReceived );
			}
		}

		EtIP_free(&pSession->pPartialRecvPacket, &pSession->iPartialRecvPacketSize );

	}

	/* The packet should be at least ENCAPH_SIZE bytes long */
	while( lBytesReceived < ENCAPH_SIZE )
	{
		/* Try to read the data. All sockets are non-blocking, so we'll get the result right away */
		ltmpBytesReceived = platformRecv( pSession->lSocket, (char*)( gmsgBuf + lBytesReceived ),
														ENCAPH_SIZE - lBytesReceived, 0 );
		if( ltmpBytesReceived <= 0 )
		{
			ulSockError = platformGetLastSocketError( pSession->lSocket );			/* Error occured - check what caused the error*/

			if( ltmpBytesReceived < 0 && ulSockError == EIP_WOULDBLOCK) /* Nothing more to read for now */
			{
				if ( lBytesReceived && lBytesReceived > pSession->iPartialRecvPacketSize )
				{
					/* Got partial packet - save it for the next tick read */
					EtIP_realloc(&pSession->pPartialRecvPacket,
						&pSession->iPartialRecvPacketSize, gmsgBuf, (UINT16)lBytesReceived );
					pSession->lPartialRecvPacketTimeoutTick = gdwTickCount + PARTIAL_PACKET_TIMEOUT;
					if (pSession->pPartialRecvPacket == NULL)
					{
						DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
									"socketEncapRecv couldn't save partial packet of size %d.  Probably corrupt data",
									lBytesReceived);
						sessionRemove( pSession, TRUE );
						return ERROR_STATUS;
					}
				}
				return 0;			/* Nothing to read */
			}
			else
			{
				DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
							"socketEncapRecv got error %d reading encap header, socket = 0x%x, ltmpBytesReceived = %d",
							ulSockError, pSession->lSocket, ltmpBytesReceived);
				sessionRemove( pSession, TRUE );
				return ERROR_STATUS;
			}
		}

		lBytesReceived += ltmpBytesReceived;			/* Count the data received */
	}

	/* If we got here we have at least the encapsulation header. Now get the packet size from the
	   header and read the rest of the packet from the socket */
	lBytesRequested = (INT32)(_SWAPS_(pHdr->iLength) + ENCAPH_SIZE);
	iCommand = _SWAPS_(pHdr->iCommand);

	/* Looks like we have something we can't handle (probably garbage), just shut the session down */
	if ((iCommand != ENCAP_CMD_NOOP) && (lBytesRequested > MAX_PACKET_SIZE))
	{
		sessionRemove( pSession, TRUE );
		return ERROR_STATUS;
	}

	while ( lBytesReceived < lBytesRequested )		/* Loop until we read the whole packet */
	{
		/* If command is NOP then the message can be as large as 65535 bytes - read it in strides */
		if (iCommand == ENCAP_CMD_NOOP)
		{
			lOffset = ENCAPH_SIZE;
			lBytesToRead = PLATFORM_MIN((lBytesRequested - lBytesReceived), MAX_PACKET_SIZE - ENCAPH_SIZE);
		}
		else
		{
			lOffset = lBytesReceived;
			lBytesToRead = lBytesRequested - lBytesReceived;
		}

		/* Read the data */
		ltmpBytesReceived = platformRecv( pSession->lSocket, (char*)&gmsgBuf[lOffset], lBytesToRead, 0 );
		if( ltmpBytesReceived <= 0 )
		{
			ulSockError = platformGetLastSocketError(pSession->lSocket);	/* Error occured - check what caused the error*/

			if( ulSockError == EIP_WOULDBLOCK)
			{
				if ( lBytesReceived &&
					(lBytesReceived > pSession->iPartialRecvPacketSize))
				{
					/* Got partial packet - save it for the next tick read */
					if (iCommand == ENCAP_CMD_NOOP)
					{
						if (pSession->pPartialRecvPacket == NULL)
						{
							EtIP_realloc( &pSession->pPartialRecvPacket,
								&pSession->iPartialRecvPacketSize, gmsgBuf, (UINT16)ENCAPH_SIZE );
							if (pSession->pPartialRecvPacket == NULL)
							{
								DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
											"socketEncapRecv couldn't save partial packet of size %d.  Probably corrupt data",
											ENCAPH_SIZE);
								sessionRemove( pSession, TRUE );
								return ERROR_STATUS;
							}
						}
						pSession->iPartialRecvPacketSize = (UINT16)lBytesReceived;
					}
					else
					{
						EtIP_realloc( &pSession->pPartialRecvPacket,
							&pSession->iPartialRecvPacketSize, gmsgBuf, (UINT16)lBytesReceived );
						if (pSession->pPartialRecvPacket == NULL)
						{
							DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
										"socketEncapRecv couldn't save partial packet of size %d.  Probably corrupt data",
										lBytesReceived);
							sessionRemove( pSession, TRUE );
							return ERROR_STATUS;
						}
					}
					pSession->lPartialRecvPacketTimeoutTick = gdwTickCount + PARTIAL_PACKET_TIMEOUT;
				}

				return 0;				/* Nothing to read */
			}
			else
			{
				DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
							"socketEncapRecv got error %d", ulSockError);
				sessionRemove( pSession, TRUE );
				return ERROR_STATUS;
			}
		}
		lBytesReceived += ltmpBytesReceived;
	}

	/* If we started with the partial packet - discard it now */
	if ( pSession->pPartialRecvPacket != NULL )
	{
		if (iCommand == ENCAP_CMD_NOOP)
			pSession->iPartialRecvPacketSize = ENCAPH_SIZE;

		EtIP_free( &pSession->pPartialRecvPacket, &pSession->iPartialRecvPacketSize );
	}

	/* Ignore commands with too many bytes of data */
	if ( lBytesRequested > MAX_PACKET_SIZE )
	{
		DumpStr3(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"socketEncapRecv received a command with too many bytes: ERR - received %d bytes on socket 0x%x for session %d", lBytesRequested, pSession->lSocket, pSession-gSessions);
		return 0;
	}

	DumpStr3(TRACE_LEVEL_DATA, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
		"socketEncapRecv received %d bytes on socket 0x%x for session %d", lBytesReceived, pSession->lSocket, pSession->nSessionId);

	DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_TCP_IN, pSession->lClientIPAddr,
		pSession->lHostIPAddr, (char*)gmsgBuf, lBytesReceived);

	/* In the old buffer bytes are in host order. Put them back into network order. Meaningful only for big-endian platforms. */
	socketAdjustEncapHeader((ENCAPH*)gmsgBuf);

	return lBytesReceived;

}

/*---------------------------------------------------------------------------
** socketEncapSend( )
**
** Send TCP packet
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
static BOOL socketEncapSend(SESSION* pSession, INT32 lLen, INT32* plenSent)
{
	INT32 lSent = 0;
	INT32 lSockError;

	if (pSession->lSocket == (SOCKET)INVALID_SOCKET)
	{
		(*plenSent) = ERROR_STATUS;
		return TRUE;
	}

	if (lLen > MAX_PACKET_SIZE)		/* The length is invalid */
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"socketEncapSend encap header is corrupted");
		(*plenSent) = ERROR_STATUS;
		return TRUE;
	}

	/* Send TCP data */
	lSent = platformSend(pSession->lSocket, gmsgBuf, lLen, 0);

	if (lSent <= 0)
	{
		lSockError = platformGetLastSocketError(pSession->lSocket);

		if (lSent < 0 && lSockError != EIP_WOULDBLOCK)		/* session has been broken */
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"socketEncapSend got error %d", lSockError);
			sessionRemove(pSession, TRUE);
			(*plenSent) = ERROR_STATUS;
			return FALSE;
		}

		lSent = 0;
	}

	if ( lSent < lLen )
	{
		if (pSession->pPartialSendPacket != NULL)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, 
					"socketEncapSend socket full - aborting");
			sessionRemove( pSession, TRUE );
			(*plenSent) = ERROR_STATUS;
			return FALSE;
		}

		pSession->iPartialSendPacketSize = (UINT16)(lLen - lSent);
		pSession->pPartialSendPacket = EtIP_malloc( &gmsgBuf[lSent], pSession->iPartialSendPacketSize );
	}

	if ( lSent )
	{
		DumpStr2(TRACE_LEVEL_DATA, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"socketEncapSend sent %d bytes to socket 0x%x", lSent, pSession->lSocket);

		DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_TCP_OUT, pSession->lHostIPAddr,
			pSession->lClientIPAddr, (char*)gmsgBuf, lLen);
	}

	/* Set the number of bytes sent */
	(*plenSent) = lSent;

	/* reset the session timeout */
	sessionResetTimeout(pSession);
	return TRUE;
}

/*---------------------------------------------------------------------------
** socketEncapSendData( )
**
** Send TCP packet
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL socketEncapSendData(SESSION* pSession)
{
	INT32 len, lenSent;
	ENCAPH* pHdr;

	pHdr = (ENCAPH*)gmsgBuf;
	len = ENCAPH_SIZE + pHdr->iLength;

	pHdr->lSession = pSession->lSessionTag;

	socketAdjustEncapHeader(pHdr);

	return socketEncapSend( pSession, len, &lenSent);
}

/*---------------------------------------------------------------------------
** socketEncapSendPartial( )
**
** Send partial TCP packet
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/

BOOL socketEncapSendPartial(SESSION* pSession)
{
	INT32 len, lenSent;

	len = pSession->iPartialSendPacketSize;
	memcpy( gmsgBuf, pSession->pPartialSendPacket, len );

	EtIP_free( &pSession->pPartialSendPacket, &pSession->iPartialSendPacketSize );

	DumpStr2(TRACE_LEVEL_DATA, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"socketEncapSend about to send partial %d bytes to socket 0x%x", len, pSession->lSocket);
	DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_TCP_OUT, pSession->lHostIPAddr,
			pSession->lClientIPAddr, (char*)gmsgBuf, len);

	return socketEncapSend( pSession, len, &lenSent);
}

#ifndef EIP_CUSTOM_SOCKET
/*---------------------------------------------------------------------------
** socketClass1Init( )
**
** Initialize UDP socket used to send and receive Class 1 packets
**---------------------------------------------------------------------------
*/

SOCKET socketClass1Init( BOOL bBroadcast, UINT32 lBufferSize, UINT32 lHostIPAddress, UINT16 iPort, BOOL bConsumeMcast )
{
	SOCKET lSocket;
	struct sockaddr_in sSrvrAddr;
	UINT32 lAddrLen;
	INT32 nStatus;
	UINT32 lFlagTrue = (UINT32)~0;
	INT32 nProtocol = bBroadcast ? IPPROTO_IP : IPPROTO_UDP;

	/* Get a handle to a new UDP socket */
	if ( ( lSocket = platformSocket( PF_INET, SOCK_DGRAM, nProtocol ) ) == (SOCKET)INVALID_SOCKET )
	{
		notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init error calling socket function");
		return INVALID_SOCKET;
	}

	/* The socket may be bound to an address which is already in use */
	nStatus = platformSetSockOpt( lSocket, SOL_SOCKET, SO_REUSEADDR, ( char * ) &lFlagTrue, sizeof( lFlagTrue ) );
	if ( nStatus == SOCKET_ERROR )
	{
		/*
		** While some of the options are non-critical, having them fail this
		** early in the game should be a cause for general alarm
		*/
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init: failed calling platformSetSockOpt SO_REUSEADDR");
	}

#ifdef EIP_BROADCAST_BIND_TO_INADDR_ANY
	platformSetSockaddrIn(&sSrvrAddr, iPort, (bBroadcast || (bConsumeMcast && (gnClaimedHostIPAddrCount == 1))) ? INADDR_ANY : lHostIPAddress);
#else
	platformSetSockaddrIn(&sSrvrAddr, iPort, lHostIPAddress);
#endif

	/* Allocate big enough send and receive buffers */
	if ( ( nStatus = platformSetSockBufSize( lSocket, SOL_SOCKET, SO_SNDBUF, (char *)&lBufferSize, sizeof(UINT32) ) ) == SOCKET_ERROR )
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init error setting option SO_SNDBUF");

	if ( ( nStatus = platformSetSockBufSize( lSocket, SOL_SOCKET, SO_RCVBUF, (char *)&lBufferSize, sizeof(UINT32) ) ) == SOCKET_ERROR )
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init error setting option SO_RCVBUF");

	if (bBroadcast)
	{
		/* Setup the broadcast option */
		lFlagTrue = (UINT32)~0;
		if ( ( nStatus = platformSetSockOpt( lSocket, SOL_SOCKET, SO_BROADCAST, (char*)&lFlagTrue, sizeof( lFlagTrue) ) ) == SOCKET_ERROR )
			DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init error setting option SO_BROADCAST");
	}

	/* Bind the socket to the Ethernet/IP UDP port */
	if ( platformBind( lSocket, (struct sockaddr *)&sSrvrAddr , sizeof(sSrvrAddr) ) == SOCKET_ERROR )
	{
		/* If we can't bind, no one can conect, fatal out */
		notifyEvent( NM_ERROR_USING_WINSOCK, 0, 0 );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init error calling bind function");
		platformCloseSocket(lSocket);
		return INVALID_SOCKET;
	}

	/* Make socket non-blocking */
	lAddrLen = 1;
	nStatus = platformIoctl(lSocket, FIONBIO, &lAddrLen);

	if ( nStatus == SOCKET_ERROR )				/* Non-blocking mode is absolutely required */
	{
		notifyEvent( NM_ERROR_SETTING_SOCKET_TO_NONBLOCKING, 0, 0 );
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init: could not set socket mode to non-blocking");
		platformCloseSocket(lSocket);
		return INVALID_SOCKET;
	}

	DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lHostIPAddress, 0, "socketClass1Init allocated socket %x", lSocket);
	return lSocket;
}

#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)

/*---------------------------------------------------------------------------
** INT32 socketJoinMulticastGroup(CONNECTION* pConnection)
**
** Join multicast group, so we can receive on a connection where the
** transport mode is set to Multicast ( as opposed to PointToPoint )
**---------------------------------------------------------------------------
*/

SOCKET socketJoinMulticastGroup(CONNECTION* pConnection)
{
	struct ip_mreq  sMreq;

	platformGetInternetAddress(sMreq.imr_interface) = pConnection->cfg.lHostIPAddr;
	platformGetInternetAddress(sMreq.imr_multiaddr) = platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr);

	/* Join multicast group that receives on the pConnection->sReceiveAddr.sin_addr.s_addr address */
	if ( platformSetSockOpt( pConnection->lClass1Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
							(char *)&sMreq, sizeof(sMreq) ) == SOCKET_ERROR )
	{
		DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"socketJoinMulticastGroup: error adding membership address %s added to socket 0x%x",
					platformInetNtoa(sMreq.imr_multiaddr),
					pConnection->lClass1Socket ); 
	}
	else
	{
		DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"socketJoinMulticastGroup address %s added to socket 0x%x",
					platformInetNtoa(sMreq.imr_multiaddr),
					pConnection->lClass1Socket );
	}

	return pConnection->lClass1Socket;
}

/*-----------------------------------------------------------------------------
** INT32 socketDropMulticastGroup(CONNECTION* pConnection)
**
** Drop out membership in the multicast group once the connection is terminated
**-----------------------------------------------------------------------------
*/

SOCKET socketDropMulticastGroup(CONNECTION* pConnection)
{
	struct ip_mreq  sMreq;

	platformGetInternetAddress(sMreq.imr_interface) = pConnection->cfg.lHostIPAddr;
	platformGetInternetAddress(sMreq.imr_multiaddr) = platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr);

	/* Drop multicast group that receives on the pConnection->sReceiveAddr.sin_addr.s_addr address */
	if ( platformSetSockOpt( pConnection->lClass1Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
							(char *)&sMreq, sizeof(sMreq) ) == SOCKET_ERROR )
		return ERROR_STATUS;

	DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"socketDropMulticastGroup address %s removed from socket 0x%x",
				platformInetNtoa(sMreq.imr_multiaddr),
				pConnection->lClass1Socket );

	return pConnection->lClass1Socket;
}

#endif /* #if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST) */

/*---------------------------------------------------------------------------
** INT32 socketClass1Recv()
**
** Receive on the UDP socket
**---------------------------------------------------------------------------
*/

INT32 socketClass1Recv( SOCKET lClass1Socket, UINT32 lHostIp )
{
	INT32 lBytesReceived = 0;
	INT32 lDataLength;
	struct sockaddr_in sClientAddr;
	INT32 lSockAddrSize;

	lSockAddrSize = sizeof( struct sockaddr_in );

	/* Try to read from the UDP socket into the gmsgBuf buffer. The socket is set to non-blocking,
	  so we'll know the results right away */
	lBytesReceived = platformRecvFrom( lClass1Socket, (char*)gmsgBuf, MAX_PACKET_SIZE, 0,
										(struct sockaddr *)&sClientAddr, &lSockAddrSize );

	if( lBytesReceived <= 0 )
		return ERROR_STATUS;			/* Nothing to read or error reading */

	DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_CONNECTION, platformGetInternetAddress(sClientAddr.sin_addr), lHostIp,
			"socketClass1Recv receieved %d bytes", lBytesReceived);
	DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_UDP_IN, platformGetInternetAddress(sClientAddr.sin_addr),lHostIp,(char*)gmsgBuf, lBytesReceived);

	/* Parse the message and populate assembly object with the input data */
	lDataLength = ioParseIOPacket( platformGetInternetAddress(sClientAddr.sin_addr), lClass1Socket, gmsgBuf, lBytesReceived);
	return lDataLength;

}

/*---------------------------------------------------------------------------
** INT32 socketGetBroadcasts()
**
** Receive any broadcast messages to port 0xaf12
**---------------------------------------------------------------------------
*/
void socketGetBroadcasts(void)
{
	INT32 lBytesReceived;
	struct sockaddr_in sClientAddr, socketAddress;
	INT32 lSockAddrSize;
    // PX Green Modified - LTK-3872
	UINT32 lSocketAddressSize = sizeof(socketAddress);
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES *pTcpIpConfig, *pTcpIpConfig2, *pTcpIpSocketConfig;
	UINT8 bInternalMsg;

	for(pTcpIpConfig = gsTcpIpInstAttr;
		pTcpIpConfig < &gsTcpIpInstAttr[gnClaimedHostIPAddrCount];
		pTcpIpConfig++)
	{
#ifdef EIP_ACD
		if (platformIsDuplicateIPAddress(pTcpIpConfig->InterfaceConfig.lIpAddr))
			continue;
#endif
		if (pTcpIpConfig->lBroadcastSocket == (SOCKET)INVALID_SOCKET)
			continue;

		lSockAddrSize = sizeof( struct sockaddr_in );

		/* Try to read from the UDP socket into the gmsgBuf buffer. The socket is set to non-blocking,
		  so we'll know the results right away */
		lBytesReceived = platformRecvFrom(pTcpIpConfig->lBroadcastSocket, (char*)gmsgBuf, MAX_PACKET_SIZE, 0,
											( struct sockaddr * )&sClientAddr, &lSockAddrSize );

		/* must have at least an encapsulation header */
		if( lBytesReceived < ENCAPH_SIZE )
			continue;

		pTcpIpSocketConfig = pTcpIpConfig;
		/* if platformGetSockName fails, just use TcpIp config provided */
		if (platformGetSockName(pTcpIpConfig->lBroadcastSocket, (struct sockaddr*)&socketAddress, &lSocketAddressSize) == SOCKET_ERROR)
			platformGetInternetAddress(socketAddress.sin_addr) = 0;

		/* Filter out broadcasts that we initiated */
		bInternalMsg = FALSE;
		for(pTcpIpConfig2 = gsTcpIpInstAttr; 
			pTcpIpConfig2 < &gsTcpIpInstAttr[gnClaimedHostIPAddrCount]; 
			pTcpIpConfig2++)
		{
			if (pTcpIpConfig2->InterfaceConfig.lIpAddr == platformGetInternetAddress(sClientAddr.sin_addr))
			{
				bInternalMsg = TRUE;
				break;
			}
			
			if (pTcpIpConfig2->InterfaceConfig.lIpAddr == platformGetInternetAddress(socketAddress.sin_addr))
				pTcpIpSocketConfig = pTcpIpConfig2;
		}

		if (!bInternalMsg)
		{
			socketHandleBroadcasts(pTcpIpSocketConfig, &sClientAddr, gmsgBuf, lBytesReceived);
		}
	}
}

#endif /* EIP_CUSTOM_SOCKET */

/*---------------------------------------------------------------------------
** void socketHandleBroadcasts()
**
** Process any broadcast messages to port 0xaf12
**---------------------------------------------------------------------------
*/

void socketHandleBroadcasts(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, struct sockaddr_in* pClientAddr, UINT8* pBuf, INT32 lBytesReceived)
{
	ENCAPH* psEncapHdr;
	REQUEST* pRequest;
	UINT16 iMaxDelay;


	DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_MSG, platformGetInternetAddress(pClientAddr->sin_addr), pTcpIpConfig->InterfaceConfig.lIpAddr, "socketHandleBroadcasts receieved %d bytes", lBytesReceived);
	DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_BCAST_IN, 0, 0, (char*)pBuf, lBytesReceived);

	if (pBuf != gmsgBuf)
		memcpy( gmsgBuf, pBuf, lBytesReceived );

	psEncapHdr = (ENCAPH *)gmsgBuf;

	/* Convert the encapsulation header. */
	socketAdjustEncapHeader(psEncapHdr);

	if (psEncapHdr->lStatus != ENCAP_E_SUCCESS)
		return;

	switch (psEncapHdr->iCommand)
	{
	case ENCAP_CMD_LISTIDENTITY:
		if (psEncapHdr->iLength > 0)
		{
			/* if length > 0, this must be a response */
			if (!(IS_TICK_GREATER(gdwTickCount, pTcpIpConfig->lListIdentityTimeout)))
				ucmmParseListIdentityResp(lBytesReceived, platformGetInternetAddress(pClientAddr->sin_addr), pTcpIpConfig->InterfaceConfig.lIpAddr);
		}
		else
		{
			/* As of v1.12 of the Ethernet/IP spec, responses to broadcast ListIdentity
			 * commands need to be delayed to prevent an unintentional ARP storm.
			 * Many TCP/IP stacks can't (easily if at all) distinguish between broadcast
			 * ListIdentity and unicast ListIdentity, so for now both are being
			 * delayed.  Hopefully the spec will be more accomodating to unicast
			 * ListIdentity, since it's really only used in the conformance test
			 */
			pRequest = requestNewIncoming(LIST_IDENTITY_REQUEST, TRUE, psEncapHdr->lSession);
			if (pRequest != NULL)
			{
				pRequest->lHostIPAddr = pTcpIpConfig->InterfaceConfig.lIpAddr;
				/* Save the requestor sockaddr info for the response */
				memcpy(&pRequest->sTOTagAddr, pClientAddr, sizeof(pRequest->sTOTagAddr));
				iMaxDelay = (UINT16)(psEncapHdr->lContext1 & 0xFFFF);

				if ((iMaxDelay == 0) || (iMaxDelay > 2000))
				{
					iMaxDelay = 2000;
				}
				else if (iMaxDelay < 500)
				{
					iMaxDelay = 500;
				}

				/* Use the timeout tick value as a timer expiration for the ListIdentity */
				pRequest->lRequestTimeoutTick = gdwTickCount + utilCalculatePseudoRandomDelay(iMaxDelay);
				pRequest->nState = REQUEST_LOGGED;

			}
		}
		return;

	case ENCAP_CMD_LISTSERVICES:
		ucmmRespondListServices( NULL );
		break;

	case ENCAP_CMD_LISTINTERFACES:
		ucmmRespondListInterfaces( NULL );
		break;

		/* UnRegister Session command */
	case ENCAP_CMD_UNREGISTERSESSION:
		ucmmErrorReply(NULL, ENCAP_E_UNHANDLED_CMD, 0);
		break;

	default:						/* Broadcast not supported - just drop it - no need to return an error */
		return;
	}

	socketSendBroadcast(psEncapHdr, pTcpIpConfig, pClientAddr);
}

void socketSendBroadcast(ENCAPH* psEncapHdr, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, struct sockaddr_in* pClientAddr)
{
	INT32 len = ENCAPH_SIZE + psEncapHdr->iLength, lSent;

	/* Convert the encapsulation header back */
	socketAdjustEncapHeader(psEncapHdr);

	lSent = platformSendTo( pTcpIpConfig->lBroadcastSocket, gmsgBuf, len, 0,
								(struct sockaddr*)pClientAddr, sizeof( struct sockaddr_in ), ENCAP_SERVER_PORT);

	if( lSent <= 0 )
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pTcpIpConfig->InterfaceConfig.lIpAddr, platformGetInternetAddress(pClientAddr->sin_addr), "Error sending broadcast response");
	else
	{
		DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_MSG, pTcpIpConfig->InterfaceConfig.lIpAddr, platformGetInternetAddress(pClientAddr->sin_addr), "socketHandleBroadcasts successfully sent %d bytes", lSent);
		DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_BCAST_OUT, pTcpIpConfig->InterfaceConfig.lIpAddr,
			platformGetInternetAddress(pClientAddr->sin_addr),(char*)gmsgBuf, lSent);
	}
}

BOOL socketSendBroadcasts(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, ENCAPH* encap)
{
	SOCKET lSocket;
	INT32 lSent;
	struct sockaddr_in sSrvrAddr;
	UINT32 lToBeSent;
	UINT8* pData;

	lSocket = pTcpIpConfig->lBroadcastSocket;
	lToBeSent = ENCAPH_SIZE;
	pData = gmsgBuf;
	memcpy(gmsgBuf, encap, ENCAPH_SIZE);

	platformSetSockaddrIn( &sSrvrAddr, ENCAP_SERVER_PORT, INADDR_BROADCAST);

	lSent = platformSendTo(lSocket, pData, lToBeSent, 0,
							(struct sockaddr*)&sSrvrAddr, sizeof( struct sockaddr_in ), ENCAP_SERVER_PORT);

	if( lSent <= 0 )
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, pTcpIpConfig->InterfaceConfig.lIpAddr, 0, "Error sending broadcast message");
		lSent = platformGetLastSocketError(lSocket);
		return FALSE;
	}

	DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_MSG, pTcpIpConfig->InterfaceConfig.lIpAddr, 0, "socketSendBroadcasts successfully sent %d bytes", lSent);

	return TRUE;
}

/*---------------------------------------------------------------------------
** socketClass1Send( )
**
** Transfer on the UDP socket
**---------------------------------------------------------------------------
*/

INT32 socketClass1Send( CONNECTION* pConnection, INT32 len )
{
	INT32 lSent = 0;
	struct sockaddr_in saddr;
	INT32 lSockError;
	SESSION* pSession;

	/* Check that connection is active and fits the profile */
	if ((connectionIsIOConnection(pConnection) == FALSE) ||
		(pConnection->lConnectionState != ConnectionEstablished))
		return ERROR_STATUS;


	memcpy(&saddr, &pConnection->sTransmitAddr, sizeof(struct sockaddr_in));
	/* Send UDP packet, which is already in the gmsgBuf buffer */
	lSent = platformSendTo(pConnection->lClass1Socket,
		gmsgBuf,
		len,
		0,
		(struct sockaddr*)&saddr,
		sizeof(struct sockaddr_in),
		CLASS1_UDP_PORT);
	if( lSent <= 0 )
	{
		lSockError = platformGetLastSocketError(pConnection->lClass1Socket);			/* Error occured - check what caused the error */
		if( lSent < 0 && lSockError != EIP_WOULDBLOCK)
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"socketClass1Send error %d", lSockError);

			/* Terminate all sessions for this socket */
			pSession = sessionGetBySessionId( pConnection->nSessionId );
			if ( pSession )
				sessionRemove( pSession, TRUE );
		}

#ifdef CONNECTION_STATS
		gSystemStats.systemStats.iNumFailedSentIOMessages++;
		pConnection->iSendPacketError++;
#endif

		return ERROR_STATUS;
	}

#ifdef CONNECTION_STATS
	gSystemStats.systemStats.iNumSuccessfulSentIOMessages++;
#endif

	DumpStr2(TRACE_LEVEL_DATA, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr),
				"socketClass1Send sent %d bytes for connection %d", lSent, pConnection->cfg.nInstance);
	DumpBuf(TRACE_LEVEL_DATA, TRACE_TYPE_UDP_OUT, pConnection->cfg.lHostIPAddr, platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr), (char*)gmsgBuf, lSent);

	return lSent;		/* Return the actual number of bytes sent */

}

/*---------------------------------------------------------------------------
** socketGetAddressString( )
**
** Return string representation of the passed IP address
**---------------------------------------------------------------------------
*/
INT8* socketGetAddressString( UINT32 lIPAddress )
{
	struct sockaddr_in sAddr;
	platformGetInternetAddress(sAddr.sin_addr) = lIPAddress;

	return (INT8*)platformInetNtoa(sAddr.sin_addr);
}
