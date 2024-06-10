/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPSESSN.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Session collection declaration
**
*****************************************************************************
*/


#ifndef EIPSESSN_H
#define EIPSESSN_H

#define INVALID_SESSION_INDEX					(-1)

typedef enum tagOpenSessionState
{
	SessionNonExistent							= 0x0,	/* Session is not instantiated */
	OpenSessionLogged							= 0x1,	/* Open Session request has just arrived */
	OpenSessionWaitingForTCPConnection			= 0x2,	/* Waiting for the TCP connection to be established to the target */
	OpenSessionTCPConnectionEstablished			= 0x3,	/* TCP connection to the target has been successfully established */
	OpenSessionWaitingForHandshake				= 0x4,	/* Waiting for the response for the Resister Session sent */
	OpenSessionWaitingForRegisterResponse		= 0x5,	/* Waiting for the response for the Resister Session sent */
	OpenSessionWaitingForRegister				= 0x6,	/* Waiting for the Resister Session on the incoming session */
	OpenSessionEstablished						= 0x7	/* Session is opened */
}
OpenSessionState;

typedef struct tagSession					/* Session object data */
{
	INT32 nSessionId;						/* Unique session identifier */

	SOCKET lSocket;							/* TCP socket identifier */

	UINT32 lSessionTag;						/* Session Tag is returned in response to the Register Session request and must be included in all subsequent requests to the same server */

	UINT32 lState;							/* One of the values of the OpenSessionState enumeration */

	UINT32 lWaitingForOpenSessionTimeout;	/* When the session should time out if there is no response */
	UINT32 lInactivityTimeout;				/* Number of milliseconds of inactivity before session is closed. */

#ifndef EIP_NO_ORIGINATE_UCMM
	BOOL bIncoming;							/* TRUE if the session was initiated by some other device, FALSE if it was us who sent Register Session request */
#endif

	UINT32 lHostIPAddr;						/* Host IPAddress in UINT32 format for the opened session */

	UINT32 lClientIPAddr;					/* Network peer IP address TCP session is opened with */

	UINT16 iClientEncapProtocolVersion;
	UINT16 iClientEncapProtocolFlags;

	UINT8* pPartialRecvPacket;				/* Temporary store any partial packets we received for this session */
	UINT16 iPartialRecvPacketSize;			/* Size of the partial recv packet */
	UINT32 lPartialRecvPacketTimeoutTick;	/* When the partial recv packet should be discarded as old */

	UINT8* pPartialSendPacket;				/* Temporary store any partial packets we need to send for this session */
	UINT16 iPartialSendPacketSize;			/* Size of the partial send packet */

} SESSION;

#ifdef EIP_LARGE_BUF_ON_HEAP
extern SESSION* gSessions;					/* Session objects array */
#else
extern SESSION gSessions[MAX_SESSIONS];		/* Session object array */
#endif
extern SESSION* gpnSessions;				/* Number of open sessions */

extern void  sessionInit(void);
extern SESSION* sessionNew( UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming);
extern void sessionRemove( SESSION* pSession, BOOL bRelogOutgoingRequests );
extern void sessionRemoveAll(void);
#ifndef EIP_NO_ORIGINATE_UCMM
extern SESSION* sessionFindOutgoing(UINT32 lIPAddress, UINT32 lHostIPAddr);
#endif
extern BOOL sessionService( SESSION* pSession );
extern BOOL sessionIsIdle(SESSION* pSession, INT32 nConnectionIdExclude, INT32 nRequestIdExclude );
extern void sessionResetTimeout( SESSION* pSession );
extern SESSION* sessionFindAddressEstablished(UINT32 lIPAddress, UINT32 lHostIPAddr, BOOL bIncoming);
extern SESSION* sessionGetBySessionId(INT32 nInstance);

#endif /* #ifndef EIPSESSN_H */
