/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDBG.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Socket Debug object 
**
*****************************************************************************
*/

#ifndef EIP_DBG_H
#define EIP_DBG_H

#ifdef TRACE_SOCKET_OUTPUT

#define LOGGING_PORT 50001

extern void traceSocketInit(void);
extern void traceSocketService(void);
extern void traceSocketInitServerSock(SOCKET* pLogSocket, UINT32 lClaimedHostIPAddr);
extern void traceSocketCleanup(void);
extern void traceSocketWriteLogData(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char* pBuf, int nLen);
extern void traceSocketWriteLogString(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char *szBuf);

/* This retrieves time in seconds from Jan 1, 1970 and current millisecond (0-1000) */
extern void platformGetCurrentAbsTime(UINT32* plSecTime, UINT16* puMillisecTime);


typedef struct tagDebugDataHdr
{
	UINT16 iType;
	UINT32 lTime;
	UINT16 iMsecs;
	UINT16 iLen;
	UINT32 lSrcIp;
	UINT32 lDstIp;
}
sDebugDataHdr;

#endif /* TRACE_SOCKET_OUTPUT */

#endif 
