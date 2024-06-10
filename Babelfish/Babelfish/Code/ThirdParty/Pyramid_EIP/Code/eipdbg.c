/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDBG.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet socket debug interface.
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef TRACE_SOCKET_OUTPUT

static SOCKET lLogCliSock;

static UINT8 gSocketTraceFlag;			/* Filtering as determined by client */
static UINT16 gSocketTraceTypeFlag;

/*---------------------------------------------------------------------------
** traceSocketInit( )
**
** Initializes the debug module.
**---------------------------------------------------------------------------
*/
void traceSocketInit(void)
{
	lLogCliSock = (SOCKET)INVALID_SOCKET;
	gSocketTraceFlag = TRACE_LEVEL_NONE;
	gSocketTraceTypeFlag = TRACE_TYPE_NONE;
}

/*---------------------------------------------------------------------------
** dbgCloseSock()
**
** Closes a socket and marks it as invalid
**---------------------------------------------------------------------------
*/
static void dbgCloseSock(SOCKET *pSock)
{
	if(*pSock != (SOCKET)INVALID_SOCKET)
	{
		platformCloseSocket(*pSock);
		*pSock = (SOCKET)INVALID_SOCKET;
	}
}

/*---------------------------------------------------------------------------
** traceSocketInitServerSock( )
**
** Creates a server socket and puts it in listen mode.
**---------------------------------------------------------------------------
*/
void traceSocketInitServerSock(SOCKET* pLogSocket, UINT32 lClaimedHostIPAddr)
{

	UINT32 lFlagTrue = (UINT32)~0;
	struct sockaddr_in sSrv_addr;

	if(((*pLogSocket) = platformSocket(PF_INET,SOCK_STREAM,0)) == (SOCKET)INVALID_SOCKET)
	{
		return;
	}

	/* Configure the socket to listen to any potential client coming on the Logging port */
	platformSetSockaddrIn( &sSrv_addr, LOGGING_PORT, lClaimedHostIPAddr );

	/* Keepalives are being sent */
	if( platformSetSockOpt(*pLogSocket,SOL_SOCKET,SO_KEEPALIVE, (char*)&lFlagTrue,sizeof(lFlagTrue)) == SOCKET_ERROR )
	{
		dbgCloseSock(pLogSocket);
		return;
	}

	/* Set socket mode to non-blocking */

	lFlagTrue = 1;

	if( platformIoctl(*pLogSocket, FIONBIO, &lFlagTrue) == SOCKET_ERROR )
	{
		/* Non-blocking mode is absolutely required */
		dbgCloseSock(pLogSocket);
		return;
	}

	/* Bind the server to Ethernet/IP TCP port */
	if( platformBind(*pLogSocket,(struct sockaddr*)&sSrv_addr,sizeof(sSrv_addr)) == SOCKET_ERROR )
	{
		dbgCloseSock(pLogSocket);
		return;
	}

	/* Go to listening mode */
	if( platformListen(*pLogSocket,5) == SOCKET_ERROR )
	{
		dbgCloseSock(pLogSocket);
		return;
	}

}

/*---------------------------------------------------------------------------
** traceSocketCleanup()
**
** Closes the debug server socket and any open connections
**---------------------------------------------------------------------------
*/
void traceSocketCleanup(void)
{
	if(lLogCliSock != (SOCKET)INVALID_SOCKET)
	{
		dbgCloseSock(&lLogCliSock);
	}
}
  
/*---------------------------------------------------------------------------
** dbgCheckServerSock( )
**
** Checks if we have any new debug connection requests
**---------------------------------------------------------------------------
*/
static void dbgCheckServerSock(void)
{
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;
	UINT32 i, lAddrLen;

	for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
		 i++, pTcpipConfig++)
	{
		if (pTcpipConfig->lLogSrvSock == (SOCKET)INVALID_SOCKET)
			continue;

		/* Check for the connect request from the client */
		lLogCliSock = platformAccept(pTcpipConfig->lLogSrvSock, NULL,NULL);

		/* Found a socket */
		if (lLogCliSock != (SOCKET)INVALID_SOCKET)
			break;
	}

	/* didn't find any */
	if (i == gnClaimedHostIPAddrCount)
		return;

	/* Set socket mode to non-blocking */

	lAddrLen = 1;

	if( platformIoctl(lLogCliSock, FIONBIO, &lAddrLen) == SOCKET_ERROR )
	{
		/* Non-blocking mode is absolutely required */
		dbgCloseSock(&lLogCliSock);
	}

}

/*---------------------------------------------------------------------------
** dbgCheckCliSock( )
**
** Check for commands from the client debugger.
**---------------------------------------------------------------------------
*/
static void dbgCheckCliSock(void)
{
	UINT8 aCliCmd[3];
	int iRecd;

	iRecd = platformRecv(lLogCliSock, (char*)aCliCmd, sizeof(aCliCmd),0);

	if(iRecd < 0)
	{
		if(platformGetLastSocketError(lLogCliSock) != EWOULDBLOCK)
		{
			dbgCloseSock(&lLogCliSock);
		}

		return;
	}

	switch(iRecd)
	{
	case 0:
		dbgCloseSock(&lLogCliSock);
		break;    /* Nothing to read */
	case 1:
		/* We must have a 1 byte command from the client debugger. */
	case 2:
		 /* We must have 2 byte command from the client debugger, but we need 3, so
			just take the first one */
		gSocketTraceFlag = aCliCmd[0];
		break;
	case 3:
		gSocketTraceFlag = aCliCmd[0];
		gSocketTraceTypeFlag = UINT16_GET(&aCliCmd[1]);
		break;
	}

}

/*---------------------------------------------------------------------------
** traceSocketService( )
**
** Services the debugger "task".
**---------------------------------------------------------------------------
*/
void traceSocketService(void)
{
	/* We only support a single client at a time.  If we aleady have one,
	 * check it for new incoming commands.  Othewise, check the server socket
	 * to see if there's a client out there who wants to connect to us.
	 */

	if(lLogCliSock != (SOCKET)INVALID_SOCKET)
	{
		dbgCheckCliSock();
	}
	else
	{
		dbgCheckServerSock();
	}

}

/*---------------------------------------------------------------------------
** traceSocketWriteLogData()
**
** Writes a debug log entry to the client logging socket
**---------------------------------------------------------------------------
*/
void traceSocketWriteLogData(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char* pBuf, int nLen)
{
	static char aBuf[sizeof(sDebugDataHdr)+MAX_PACKET_SIZE];
	char *p = aBuf;
	UINT32 lSecTime;
	UINT16 lMillisecTime;

	if((!((iSeverity & gSocketTraceFlag) || (iType & gSocketTraceTypeFlag))) || (lLogCliSock == (SOCKET)INVALID_SOCKET))
		return;

	platformGetCurrentAbsTime(&lSecTime, &lMillisecTime);

	UINT16_SET(p, iType);
	p += sizeof(UINT16);
	UINT32_SET(p, lSecTime);
	p += sizeof(UINT32);
	UINT16_SET(p, lMillisecTime);
	p += sizeof(UINT16);
	UINT16_SET(p, nLen);
	p += sizeof(UINT16);
	UINT32_SET(p, lSrcIp);
	p += sizeof(UINT32);
	UINT32_SET(p, lDstIp);
	p += sizeof(UINT32);

	/* copy to a single buffer because socket performance is better on a single message */
	memcpy(p, pBuf, nLen);

	if(platformSend(lLogCliSock, aBuf, (INT32)(nLen+p-aBuf),0) != (nLen+p-aBuf))
	{
		dbgCloseSock(&lLogCliSock);
		return;
	}
}

void traceSocketWriteLogString(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf)
{
	static char aBuf[sizeof(sDebugDataHdr)+TRACE_BUFFER_SIZE];
	char *p = aBuf;
	UINT32 lSecTime;
	UINT16 lMillisecTime;
	UINT16 nLen;

	if((!((iSeverity & gSocketTraceFlag) || (iType & gSocketTraceTypeFlag))) || (lLogCliSock == (SOCKET)INVALID_SOCKET))
		return;

	platformGetCurrentAbsTime(&lSecTime, &lMillisecTime);

	UINT16_SET(p, iType);
	p += sizeof(UINT16);
	UINT32_SET(p, lSecTime);
	p += sizeof(UINT32);
	UINT16_SET(p, lMillisecTime);
	p += sizeof(UINT16);
	nLen = (UINT16)(strlen(szBuf)+1);
	UINT16_SET(p, nLen);
	p += sizeof(UINT16);
	UINT32_SET(p, lSrcIp);
	p += sizeof(UINT32);
	UINT32_SET(p, lDstIp);
	p += sizeof(UINT32);

	/* Ensure string is NULL terminated */
	szBuf[nLen-1] = 0;

	/* copy to a single buffer because socket performance is better on a single message */
	memcpy(p, szBuf, nLen);

	if(platformSend(lLogCliSock, aBuf, (INT32)(nLen+p-aBuf),0) != (nLen+p-aBuf))
	{
		dbgCloseSock(&lLogCliSock);
		return;
	}
}

#endif /* TRACE_SOCKET_OUTPUT */
