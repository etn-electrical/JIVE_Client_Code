/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCLIEN.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Library main API
**
*****************************************************************************
*/

#include "eipinc.h"
#include "Trusted_IP_Filter_C_Connector.h"

BOOL				gbTerminated;			/* Indicates whether client is stopped at this moment */
BOOL				gbReallyTerminated;		/* Flag indicating Ethernet/IP thread is done */
PLATFORM_MUTEX_TYPE	ghClientMutex;			/* Used to protect integrity of the client calls */

UINT32 gdwTickCount;						/* Stored system tick count in msecs */

UINT32 gpClientAppProcClasses[MAX_CLIENT_APP_PROC_CLASS_NBR];			/* Class number array that will be processed by the client application */
UINT32 glClientAppProcClassNbr;											/* Number of classes registered for client application processing */
UINT32 gpClientAppProcServices[MAX_CLIENT_APP_PROC_SERVICE_NBR];		/* Service number array that will be processed by the client application */
UINT32 glClientAppProcServiceNbr;										/* Number of services registered for client application processing */
EtIPObjectRegisterType  gpClientAppProcObjects[MAX_CLIENT_APP_PROC_OBJECT_NBR];		/* Object handler array that will be processed by the client application */
UINT32 glClientAppProcObjNbr;											/* Number of specific object handlers registered for client application processing */

#ifndef EIP_NO_ORIGINATE_UCMM
INT32 gnCurrentRequestGroupIndex;										/* Currently populated request group Id or (-1) if no group is currently being populated */
#endif

BOOL gbRunMode;				/* Indicate whether Run or Idle state will be sent when producing connection data */
static UINT32 gclientPPSResetTick;

/*---------------------------------------------------------------------------
** clientStart( )
**
** Start executing the client.
**---------------------------------------------------------------------------
*/
BOOL clientStart(void)
{
	gbTerminated = FALSE;
	gbRunMode = FALSE;
#if defined EIP_FILE_OBJECT
	gbFileObjectEnabled = FALSE;
#endif

	gclientPPSResetTick = gdwTickCount = platformGetCurrentTickCount();

	utilInit();

#ifndef EIP_NO_ORIGINATE_UCMM
	gnCurrentRequestGroupIndex = INVALID_REQUEST_INDEX;
#endif

#ifdef EIP_LARGE_CONFIG_OPT
	connListInitialize();
#endif

#ifdef TRACE_OUTPUT
	gTraceFlag = TRACE_LEVEL_ALL;
	gTraceTypeFlag = TRACE_TYPE_ALL;
#endif

#ifdef TRACE_FILE_OUTPUT
	/* Open the debug dump file if defug file output is requested */
	InitializeDump();
#endif /* #ifdef TRACE_FILE_OUTPUT */

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketInit();
#endif

	/* Initialize sockets interface */
	socketInit();

	/* Make sure that any of the API calls running in the context of a client thread
	   are not executed at the same time as another API call or the EML thread is executed */
	ghClientMutex = platformInitMutex("EtIPClientMutex");
	if(ghClientMutex == NULL)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Could not initialize mutex.  Stack is not initialized");
		notifyEvent( NM_ERROR_CREATING_SEMAPHORE, 0, 0 );
		return FALSE;
	}

	platformReleaseMutex(ghClientMutex);

	idInit();						/* Initialize identity information */
	sessionInit();					/* Initialize session array */
	connectionInit();				/* Initialize connection array */
	connmgrInit();					/* Initialize connection manager stats */
	requestInit();					/* Init requests array */
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	requestGroupInit();				/* Init request groups array */
#endif
	assemblyInit();					/* Init assembly tables */
	notifyInit();					/* Initialize event array */
	ucmmInit();						/* Initialize broadcast array */
	glClientAppProcClassNbr = 0;	/* Number of classes registered for client app processing */
	glClientAppProcServiceNbr = 0;	/* Number of services registered for client app processing */
	glClientAppProcObjNbr = 0;		/* Number of object request handlers registered for client app processing */

	tcpipInit();

#if defined EIP_FILE_OBJECT
	eipFileInit();
#endif

#ifdef ET_IP_MODBUS
	if (!modbusInit(&eipModbusCallbackFunction))
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Could not initialize Modbus.");
		notifyEvent( NM_ERROR_UNABLE_START_MODBUS, 0, 0 );
		return FALSE;
	}
#endif

	DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "EtherNet/IP stack succesfully initialized");
	return TRUE;
}

/*---------------------------------------------------------------------------
** clientStop( )
**
** Stop running the client.
**---------------------------------------------------------------------------
*/

void clientStop(void)
{
	INT32 i;
#ifndef EIP_NO_ORIGINATE_UCMM
	SESSION* pSession;
#endif
	CONNECTION* pConnection;

	/* Stack is already stopped - return */
	if ( gbTerminated )
		return;

	DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "clientStop called");

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	/* Close all connections */
	for( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
		connListSetConnectionConfigState(pConnection, ConfigurationClosing, 0);

	platformReleaseMutex(ghClientMutex);

	/* Wait for the closing to complete within clientMainTask */
	for( i = 0; i < 100; i++ )
	{
		if ( !connectionGetNumOrigEstOrClsing() )
			break;
		platformSleep( 5 );
	}

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

#ifndef EIP_NO_ORIGINATE_UCMM
	/* Close all sessions */
	for( pSession= gSessions; pSession < gpnSessions; pSession++ )
	{
		/* If we are the ones who initiate a connection drop - issue Forward Close */
		if ( !pSession->bIncoming && pSession->lState == OpenSessionEstablished )
		{
			/* Send Unregister Session request to the server */
			ucmmIssueUnRegisterSession( pSession );
		}
	}
#endif

	connectionRemoveAll();
	requestRemoveAll();
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	requestGroupRemoveAll();
#endif
	sessionRemoveAll();
	
#ifdef ET_IP_MODBUS
	modbusRelease();
#endif

	tcpipSocketCleanupAll();

#ifdef TRACE_FILE_OUTPUT
	/* Close debug dump file */
	FinishDump();
#endif /* #ifdef TRACE_FILE_OUTPUT */

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketCleanup();
#endif

	platformReleaseMutex(ghClientMutex);

	clientRelease();
}

/*---------------------------------------------------------------------------
** clientRelease( )
**
** Release all resources
**---------------------------------------------------------------------------
*/

void clientRelease(void)
{
	INT32 nLoops = 10 * 30;		/* Wait up to 30s */
	gbTerminated = TRUE;		/* Set the flag to stop the main thread */
	platformSleep(100);

	while( !gbReallyTerminated && (nLoops-- > 0) )
		platformSleep(100); 

	platformDiscardMutex( ghClientMutex );
}

/*---------------------------------------------------------------------------
** clientMainTask( )
**
** Main processing thread. Is executed once a millisecond.
** Running until nothing else to do. Then sleeps the rest of the time slice.
**---------------------------------------------------------------------------
*/
void clientMainTask(void)
{
	INT32 nConnProcessedTotal;
#if defined EIP_LIMIT_NUM_CONNECTIONS_PROCESSED && !defined (EIP_LARGE_CONFIG_OPT)
	static CONNECTION* pNextConnectionToProcess = gConnections;
#endif
#ifdef EIP_LIMIT_NUM_SESSIONS_PROCESSED
	static SESSION* pNextSessionToProcess = gSessions;
	UINT32 nSessionProcessedTotal = 0;
#endif
#ifdef EIP_REQUEST_SEND_RATE
	static REQUEST* pNextRequestToProcess = gRequests;
#endif
#if defined(EIP_LIMIT_NUM_CONNECTIONS_PROCESSED) || defined(EIP_LIMIT_NUM_SESSIONS_PROCESSED) || defined (EIP_LIMIT_NUM_IO_PACKETS_RECEIVED) || defined (EIP_REQUEST_SEND_RATE)
	BOOL bStartFromBegin;
#endif
	INT32 nReceivedIOPackets = 0;
	static UINT32 lTickLast = 0;
	static UINT32 lTickIncrement = 0;
	UINT32 i;
	INT32 lDataReceived = 0;
	SESSION* pSession;
	REQUEST* pRequest;
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	REQUEST_GROUP* pRequestGroup;
#endif
#if !defined EIP_LARGE_CONFIG_OPT || defined CONNECTION_STATS
	CONNECTION* pConnection;
#endif
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	SOCKET* pSocket;
	BOOL bDataSent;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	/* Getting hi-res tick counter is expensive in terms of CPU usage - store it for all other routines */
	gdwTickCount = platformGetCurrentTickCount();

#ifdef EIP_ACD		
	/*EATON_EIP_Changes -YM : Code changed to fix ODVA conformance issue on ACD Test
	Currently the argument to platformIsDuplicateIPAddress is not used, so it does 
	not make sense to call this for all IP address*/
	if (platformIsDuplicateIPAddress(0U))
	{
		connectionNotifyNetLEDChange();
		/* Check for duplicate IP addresses */
		for( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount;
		i++, pTcpipInterface++)
		{
			if (platformIsDuplicateIPAddress(pTcpipInterface->InterfaceConfig.lIpAddr))
			{
				/* remove all sessions associated with this IP address */
				for( pSession = gpnSessions-1; pSession >= gSessions; pSession-- )
				{
					if (pSession->lHostIPAddr == pTcpipInterface->InterfaceConfig.lIpAddr)
						sessionRemove(pSession, FALSE);
				}
				connectionNotifyNetLEDChange();
			}
		}
	}
#endif

#ifdef EIP_LARGE_CONFIG_OPT
	/* process connection states and timers */
	connectionServiceConnectionPolls();
	nConnProcessedTotal = connectionServiceConnectionTimers();
#ifdef CONNECTION_STATS
	/* Handle calculating PPS for all connections here */
	for( pConnection = gConnections; pConnection < gpnConnections; pConnection++)
	{
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
	}
#endif
#else
	/* Send connection data and check if timed out */
	nConnProcessedTotal = 0;
#ifdef EIP_LIMIT_NUM_CONNECTIONS_PROCESSED
	if (pNextConnectionToProcess >=  gpnConnections)
		pNextConnectionToProcess = gConnections;
	bStartFromBegin = (pNextConnectionToProcess == gConnections) ? TRUE : FALSE;
	for( pConnection = pNextConnectionToProcess; pConnection < gpnConnections; )
#else
	for( pConnection = gConnections; pConnection < gpnConnections; )
#endif
	{
		if (!connectionService( pConnection, &bDataSent) )
		{
			/* If the connection hasn't be removed, move to the next one */
			pConnection++;

			if (bDataSent)
			{
				nConnProcessedTotal++;
#ifdef EIP_LIMIT_NUM_CONNECTIONS_PROCESSED
				if (nConnProcessedTotal >= EIP_LIMIT_NUM_CONNECTIONS_PROCESSED)
				{
					pNextConnectionToProcess = pConnection;
					break;
				}
#endif
			}
		}

#ifdef EIP_LIMIT_NUM_CONNECTIONS_PROCESSED
		if (pConnection >= gpnConnections)
		{
			if (bStartFromBegin)
			{
				/* Went through all connections */
				pNextConnectionToProcess = gConnections;
				break;
			}
			else
			{
				/* Reset "connection counter" to service more connections */
				bStartFromBegin = TRUE;
				pConnection = pNextConnectionToProcess = gConnections;
			}
		}
#endif
	}
#endif  /* not EIP_LARGE_CONFIG_OPT */

	/* Check incoming I/O connections */
	nReceivedIOPackets = 0;
	for( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		 i < gnClaimedHostIPAddrCount;
		 i++, pTcpipInterface++)
	{

#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
		bStartFromBegin = (pTcpipInterface->pNextClass1SocketToProcess == pTcpipInterface->alClass1Socket) ? TRUE : FALSE;
		for(pSocket = pTcpipInterface->pNextClass1SocketToProcess; pSocket < &pTcpipInterface->alClass1Socket[NUM_CONNECTION_GROUPS];)
#else
		for(pSocket = pTcpipInterface->alClass1Socket; pSocket < &pTcpipInterface->alClass1Socket[NUM_CONNECTION_GROUPS]; pSocket++)
#endif
		{
			if (*pSocket != (SOCKET)INVALID_SOCKET )
			{
				do 
				{
					lDataReceived = socketClass1Recv(*pSocket, pTcpipInterface->InterfaceConfig.lIpAddr);
					if (lDataReceived == MALFORMED_PACKET_ERROR)
					{
						/* Garbage found, so try again.  It will either yield a good result or 
						 * clear out all of garbage.
						 */
						continue;
					}

					if (lDataReceived != SOCKET_ERROR)
					{
						nReceivedIOPackets++;
#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
						if (nReceivedIOPackets >= EIP_LIMIT_NUM_IO_PACKETS_RECEIVED)
						{
							pTcpipInterface->pNextClass1SocketToProcess = pSocket;
							break;
						}
#endif
					}
				}
				while( lDataReceived > 0 );

#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
				/* Can't break do/while and for loop in single break */
				if (nReceivedIOPackets >= EIP_LIMIT_NUM_IO_PACKETS_RECEIVED)
					break;
#endif
			}

#ifdef EIP_LIMIT_NUM_IO_PACKETS_RECEIVED
			pSocket++;
			if (pSocket >= &pTcpipInterface->alClass1Socket[NUM_CONNECTION_GROUPS])
			{
				if (bStartFromBegin)
				{
					/* Went through all sockets */
					pTcpipInterface->pNextClass1SocketToProcess = pTcpipInterface->alClass1Socket;
					break;
				}
				else
				{
					/* Reset "socket counter" to service more connections */
					bStartFromBegin = TRUE;
					pSocket = pTcpipInterface->pNextClass1SocketToProcess = pTcpipInterface->alClass1Socket;
				}
			}
#endif
		}

		/* Reset the non-CIP encapsulation count every second */
		if (IS_TICK_GREATER_OR_EQUAL( gdwTickCount, gclientPPSResetTick ))
		{
			pTcpipInterface->lNonCIPEncapCountPrev = pTcpipInterface->lNonCIPEncapCount;
			pTcpipInterface->lNonCIPEncapCount = 0;
		}
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	configCheckWatchdogTimer();
#endif

	/* Process TCP requests at least once in TCP_MINIMUM_RATE msec. Process more if amount of I/O traffic
	   allows. Decrease CONN_PROCESSED_LIMIT constant to give I/O more priority, or
	   increase to give more priority to TCP traffic */
	if ( ( gdwTickCount/TCP_MINIMUM_RATE != lTickLast/TCP_MINIMUM_RATE ) || ( nConnProcessedTotal < CONN_PROCESSED_LIMIT ) )
	{
#ifdef EIP_CHECK_BROADCAST_RATE
		if ((lTickIncrement % EIP_CHECK_BROADCAST_RATE) == 0)
		{
#endif
			/* Check for the incoming broadcast service requests */
			socketGetBroadcasts();
#ifdef EIP_CHECK_BROADCAST_RATE
		}
#endif

#ifdef EIP_CHECK_INCOMING_SESSION_RATE
		if ((lTickIncrement % EIP_CHECK_INCOMING_SESSION_RATE) == 0)
		{
#endif
			/* Check for the incoming TCP connection requests from other devices */
			socketCheckIncomingSession();

#ifdef EIP_CHECK_INCOMING_SESSION_RATE
		}
#endif

		/* Service all TCP sessions */
#ifdef EIP_LIMIT_NUM_SESSIONS_PROCESSED
		if (pNextSessionToProcess >=  gpnSessions)
			pNextSessionToProcess = gSessions;
		bStartFromBegin = (pNextSessionToProcess == gSessions) ? TRUE : FALSE;
		for( pSession = pNextSessionToProcess; pSession < gpnSessions; )
#else
		for( pSession = gSessions; pSession < gpnSessions;)
#endif
		{
			/* If the session hasn't be removed, move to the next one */
			if (sessionService(pSession))
			{
				pSession++;
#ifdef EIP_LIMIT_NUM_SESSIONS_PROCESSED
				nSessionProcessedTotal++;
				if (nSessionProcessedTotal >= EIP_LIMIT_NUM_SESSIONS_PROCESSED)
				{
					pNextSessionToProcess = pSession;
					break;
				}
#endif

			}


#ifdef EIP_LIMIT_NUM_SESSIONS_PROCESSED
			if (pSession >= gpnSessions)
			{
				if (bStartFromBegin)
				{
					/* Went through all sessions */
					pNextSessionToProcess = gSessions;
					break;
				}
				else
				{
					/* Reset "session counter" to service more sessions */
					bStartFromBegin = TRUE;
					pSession = pNextSessionToProcess = gSessions;
				}
			}
#endif
		}

		/* Service all requests */
#ifdef EIP_REQUEST_SEND_RATE
		if (pNextRequestToProcess >=  gpnRequests)
			pNextRequestToProcess = gRequests;
		bStartFromBegin = (pNextRequestToProcess == gRequests) ? TRUE : FALSE;
		for( pRequest = pNextRequestToProcess; pRequest < gpnRequests; )
#else
		for( pRequest = gRequests; pRequest < gpnRequests; )
#endif
		{
			/* If the request hasn't be removed, move to the next one */
#ifdef EIP_REQUEST_SEND_RATE
			bDataSent = ((lTickIncrement % EIP_REQUEST_SEND_RATE) == 0) ? TRUE : FALSE;
#else
			bDataSent = TRUE;
#endif
			if (requestService(pRequest, &bDataSent))
				pRequest++;

#ifdef EIP_REQUEST_SEND_RATE
			if (bDataSent)
			{
				pNextRequestToProcess = pRequest;
				break;
			}

			if (pRequest >= gpnRequests)
			{
				if (bStartFromBegin)
				{
					/* Went through all requests */
					pNextRequestToProcess = gRequests;
					break;
				}
				else
				{
					/* Reset "request counter" to service more requests */
					bStartFromBegin = TRUE;
					pRequest = pNextRequestToProcess = gRequests;
				}
			}
#endif			
		}

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
		/* Service all request groups */
		for( pRequestGroup = gpnRequestGroups-1; pRequestGroup >= gRequestGroups; pRequestGroup-- )
			requestGroupService( pRequestGroup );
#endif

#ifdef ET_IP_MODBUS
		modbusService(gdwTickCount);
#endif

		lTickLast = gdwTickCount;
	}

	if (IS_TICK_GREATER_OR_EQUAL( gdwTickCount, gclientPPSResetTick ))
	{
		/* Reset the UCMM count every second */
#ifdef CONNECTION_STATS
		gSystemStats.iNumRecvUCMMMessagesPPSPrev = gSystemStats.iNumRecvUCMMMessagesPPS;
		gSystemStats.iNumRecvUCMMMessagesPPS = 0;
		gSystemStats.iNumSendUCMMMessagesPPSPrev = gSystemStats.iNumSendUCMMMessagesPPS;
		gSystemStats.iNumSendUCMMMessagesPPS = 0;
#endif
		/* Need to reset for TCP/IP stats as well */
		gclientPPSResetTick = gdwTickCount + 1000;
	}

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketService();
#endif

	lTickIncrement++;

	platformReleaseMutex(ghClientMutex);

	/* Now that we released the mutex, we can send any events logged to the client app */
	notifyService();
}

/*---------------------------------------------------------------------------
** clientRegisterEventCallBack( )
**
** Register client application callback function
**---------------------------------------------------------------------------
*/
void clientRegisterEventCallBack( LogEventCallbackType *pfnLogEvent )
{
	gfnLogEvent = pfnLogEvent;
}

/*---------------------------------------------------------------------------
** clientRegisterMessageCallBack( )
**
** Register client application log message function
** This is only utilized with TRACE_OUTPUT defined
**---------------------------------------------------------------------------
*/
void clientRegisterMessageCallBack( LogMessageCallbackType *pfnLogMessage )
{
#ifdef TRACE_OUTPUT
	gfnLogMessage = pfnLogMessage;
#endif
}

/*---------------------------------------------------------------------------
** clientSetDebuggingLevel( )
**
** Sets the level of debug statements written to a file/debug window if
** tracing is turned on.  The two filters are ANDed to determine if message
** should be logged.  See EtIPApi.h for filter macros (TRACE_LEVEL_* and TRACE_TYPE_*)
**---------------------------------------------------------------------------
*/
void clientSetDebuggingLevel(UINT8 iDebugLevel, UINT16 iDebugType)
{
#ifdef TRACE_OUTPUT
	gTraceFlag = iDebugLevel;
	gTraceTypeFlag = iDebugType;
#else
	iDebugLevel = iDebugLevel;
	iDebugType = iDebugType;
#endif
}

/*---------------------------------------------------------------------------
** clientAddAssemblyInstance( )
**
** Add a new assembly object. nAssemblyInstance must be unique among assembly
** object collection. Connection points and configuration connection instance
** for the new connection - all fall in the category of the assembly instance.
** iType must be Combination of a type from EtIPAssemblyAccessType with a type
** from EtIPAssemblyDirectionType and, optionally, one of the types from 
** tagEtIPAssemblyAdditionalFlag defined in EtIPApi.h.
** nOffset will determine offset in bytes of this particular assembly in the assembly
** buffer. nOffset may be passed as INVALID_OFFSET - in this case it will be
** automatically assigned by the stack from the unalocated assembly space.
** nLength is the assembly object size in bytes.
** szAssemblyName is the optional tag name associated with the assembly
** instance. It may be specified by the incoming Forward Open instead of the
** assembly instance. Can be passed as NULL if there is no associated tag name.
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
INT32 clientAddAssemblyInstance(UINT32 iAssemblyInstance, UINT16 iType, INT32 nOffset, UINT16 iLength, char* szAssemblyName)
{
	INT32 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyNew(iAssemblyInstance, nOffset, iLength, iType, szAssemblyName);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientRemoveAssemblyInstance( )
**
** Removes previosly configured assembly object. nAssemblyInstance is the
** unique instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
INT32 clientRemoveAssemblyInstance(UINT32 iAssemblyInstance)
{
	INT32 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyRemove(iAssemblyInstance);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientRemoveAllAssmInstances( )
**
** Removes all previosly configured assembly objects.
** Returns 0 in case of success or ERR_ASSEMBLY_INSTANCE_USED if
** there is an active or pending connection that is configured to use
** this instance.
**---------------------------------------------------------------------------
*/
INT32 clientRemoveAllAssmInstances(void)
{
	CONNECTION* pConnection;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	/* Make sure there are no class0/1 connections present */
	for( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ((pConnection->cfg.bOriginator == FALSE) &&
			connectionIsIOConnection(pConnection))
		{
			platformReleaseMutex(ghClientMutex);
			return ERR_ASSEMBLY_INSTANCE_USED;
		}
	}

	assemblyRemoveAll();

	platformReleaseMutex(ghClientMutex);

	return 0;
}



/*---------------------------------------------------------------------------
** clientAddAssemblyMember( )
**
** Add a new member for the assembly object. If members are used, this function
** will be called multiple times after calling EtIPAddAssemblyInstance.
** The first call will be for offset 0. All sebsequent ones will be for the
** offset starting where the previous EtIPAddAssemblyMember ended.
** EtIPAddAssemblyMember should cover all data size allocated for this
** assembly instance. If a few bits should be skipped EtIPAddAssemblyMember
** will be called with nMember set to INVALID_MEMBER. There is no byte or word
** padding.
** iAssemblyInstance must be a valid instance previously allocated with a
** EtIPAddAssemblyInstance successful call. iMember is a member id associated
** with this particular member. iBitSize is the member size in bits.
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
INT32 clientAddAssemblyMember(UINT32 iAssemblyInstance, UINT32 iMember, UINT16 iBitSize)
{
	INT32 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyAddMember(iAssemblyInstance, iMember, iBitSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientRemoveAllAssemblyMembers( )
**
** Removes previosly configured assembly object members. Gives the client
** application a chance to repopulate member array if needed. There should
** be no active or pending connections using this assembly instance for the
** call to succeed.
** iAssemblyInstance is previously configured instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if
** there is an active or pending connection that is configured to use
** this instance.
**---------------------------------------------------------------------------
*/
INT32 clientRemoveAllAssemblyMembers(UINT32 iAssemblyInstance)
{
	INT32 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyRemoveAllMembers(iAssemblyInstance);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientGetAssemblyInstances( )
**
** Returns total number of configured assemblies.
** pnAssemblyInstanceArray should point to the array of MAX_ASSEMBLIES integers.
** It will receive the assembly instances.
** Example:
**
** USHORT AssemblyInstanceArray[MAX_ASSEMBLIES];
** INT32 nNumAssemblies = clientGetAssemblyInstances(AssemblyInstanceArray);
**
** After return AssemblyInstanceArray[0] will have the first assembly instance,
** AssemblyInstanceArray[1] will have the second assembly instance ...
** AssemblyInstanceArray[nNumAssemblies-1] will have the last assembly
** instance.
**---------------------------------------------------------------------------
*/
INT32 clientGetAssemblyInstances(INT32 *pnAssemblyInstanceArray)
{
	INT32 nCount = 0;
	ASSEMBLY* pAssembly;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	for ( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
	{
		*pnAssemblyInstanceArray = (INT32)pAssembly->iInstance;
		pnAssemblyInstanceArray++;
		nCount++;
	}

	platformReleaseMutex(ghClientMutex);

	return nCount;
}

/*---------------------------------------------------------------------------
** clientGetAssemblyConfig( )
**
** Return configuration for the particular assembly instance.
** nAssemblyInstance is the assembly instance obtained by calling 
** EtIPGetAssemblyInstances().
** pAssemblyConfig is the pointer to the EtIPAssemblyConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_ASSEMBLY_INVALID_INSTANCE
** if assembly with this instance does not exist.
**---------------------------------------------------------------------------
*/
INT32 clientGetAssemblyConfig(INT32 nAssemblyInstance, EtIPAssemblyConfig* pAssemblyConfig)
{
	ASSEMBLY* pAssembly;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pAssembly = assemblyGetByInstance( (UINT16)nAssemblyInstance );

	if ( pAssembly == NULL )
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_ASSEMBLY_INVALID_INSTANCE;
	}

	pAssemblyConfig->nInstance = (INT32)pAssembly->iInstance;
	pAssemblyConfig->nOffset = pAssembly->nOffset;
	pAssemblyConfig->nSize = (INT32)pAssembly->iSize;
	pAssemblyConfig->nType = (INT32)pAssembly->iType;
	pAssemblyConfig->nNumMembers = (INT32)(pAssembly->pnMembers-pAssembly->Members);
	memcpy(pAssemblyConfig->Members, pAssembly->Members, pAssemblyConfig->nNumMembers*sizeof(EtIPAssemblyMemberConfig) );
	memcpy(pAssemblyConfig->szTagName, pAssembly->pTagName, pAssembly->iTagSize);
	pAssemblyConfig->szTagName[pAssembly->iTagSize] = 0;
#ifdef CONNECTION_STATS
	pAssemblyConfig->lLastUpdateTimestamp = pAssembly->lLastUpdateTimestamp;
#else
	pAssemblyConfig->lLastUpdateTimestamp = 0;
#endif
	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*-------------------------------------------------------------------------------
** clientGetAssemblyMemberData( )
**
** Fills the provided buffer with the assembly data for the particular
** member of an assembly instance. pData is the pointer to the buffer to
** be fiiled in. iSize is the allocated buffer length in bytes.
** iAssemblyInstance and iMember identify the target member of an assembly object.
** Returns the number of bytes populated in pData, which will be the least of 
** the member size and the passed buffer length.
** Returns 0 if iAssemblyInstance or iMember has not been configured.
**--------------------------------------------------------------------------------
*/
UINT16 clientGetAssemblyMemberData( UINT32 iAssemblyInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ) 
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyGetMemberData(iAssemblyInstance, iMember, pData, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientSetAssemblyMemberData( )
**
** Fills the assembly member with the data from the passed data buffer.
** iAssemblyInstance and iMember identify the target member.
** pData is the pointer to the buffer to copy data from.
** iSize is the buffer length in bytes. If member size is less than
** iSize only member size bits will be copied from the pData.
** Returns the actual size of copied data in bytes, which will be the
** least of the member size and the passed buffer length.
** Returns 0 if iAssemblyInstance or iMember has not been configured.
**---------------------------------------------------------------------------
*/
UINT16 clientSetAssemblyMemberData( UINT32 iAssemblyInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ) 
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblySetMemberData(iAssemblyInstance, iMember, pData, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*------------------------------------------------------------------------------
** clientGetAssemblyInstanceData( )
**
** Fills the provided buffer with the assembly data for the particular
** assembly instance. pData is the pointer to the buffer. iSize is the
** buffer length in bytes. Returns the actual size of copied data which will be 
** the least of assembly object size and the passed buffer length.
** Returns 0 if iAssemblyInstance has not been configured.
**------------------------------------------------------------------------------
*/
UINT16 clientGetAssemblyInstanceData( UINT32 iAssemblyInstance, UINT8* pData, UINT16 iSize )
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyGetInstanceData(iAssemblyInstance, pData, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientSetAssemblyInstanceData( )
**
** Fills the assembly object with the passed buffer data. pData is the 
** pointer to the buffer. iSize is the buffer length in bytes. 
** Returns the actual size of copied data, which may be less than iSize
** if the assembly object size is less than iSize.
** Returns 0 if iAssemblyInstance has not been configured.
**---------------------------------------------------------------------------
*/
UINT16 clientSetAssemblyInstanceData( UINT32 iAssemblyInstance, UINT8* pData, UINT16 iSize )
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblySetInstanceData(iAssemblyInstance, pData, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientSetAssemblyInstanceDataOffset( )
**EATON_EIP_Changes:MAV - Added the ability to set an offset into the assembly.
**
** Fills the assembly object with the passed buffer data. pData is the
** pointer to the buffer. iSize is the buffer length in bytes.
** Returns the actual size of copied data, which may be less than iSize
** if the assembly object size is less than iSize.
** Returns 0 if iAssemblyInstance has not been configured.
**---------------------------------------------------------------------------
*/
UINT16 clientSetAssemblyInstanceDataOffset( UINT32 iAssemblyInstance,
									UINT8* pData, UINT16 iOffset, UINT16 iSize )
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblySetInstanceDataOffset(iAssemblyInstance, pData, iOffset, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*-------------------------------------------------------------------------------
** clientGetAssemblyCombinedData( )
**
** Fills the provided buffer with the assembly data for one or more
** assembly instances. pData is the pointer to the buffer. iSize is the
** number of bytes to copy. nOffset is the start copy offset in the assembly
** buffer - buffer of ASSEMBLY_SIZE bytes where data for all instances is stored.
** Return the number of bytes copied in the provided buffer, which will be iSize
** if successful or 0 if nOffset + iSize exceed the assembly buffer size.
**-------------------------------------------------------------------------------
*/
UINT16 clientGetAssemblyCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblyGetCombinedData(pData, nOffset, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientSetAssemblyCombinedData( )
**
** Fills one or more assembly instances with the passed buffer data. pData is 
** the pointer to the buffer. iSize is the number of bytes to copy. 
** nOffset is the start copy offset in the assembly buffer  - buffer 
** of ASSEMBLY_SIZE bytes where data for all instances is stored.
** Return the number of bytes copied from the provided buffer, which will be 
** iSize if successful or 0 if nOffset + iSize exceed the assembly buffer size.
**---------------------------------------------------------------------------
*/
UINT16 clientSetAssemblyCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	UINT16 nRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	nRet = assemblySetCombinedData(pData, nOffset, iSize);

	platformReleaseMutex(ghClientMutex);

	return nRet;
}

/*---------------------------------------------------------------------------
** clientAutomaticRunIdleEnforcement( )
**
** Enforces the presense of the 32-bit header in the O->T direction and enforces
** the lack of a 32-bit header in the T->O direction for ExclusiveOwner I/O 
** connections.  Enforces no 32-bit header for either direction on InputOnly or
** ListenOnly connections.
**---------------------------------------------------------------------------
*/
void clientAutomaticRunIdleEnforcement(BOOL bEnforce)
{
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbAutomaticRunIdleEnforcement = bEnforce;
	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientGetNumConnections( )
**
** Returns total number of connections.
**---------------------------------------------------------------------------
*/
INT32 clientGetNumConnections(void)
{
	INT32 iNumberOfConnections;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	iNumberOfConnections = (INT32)(gpnConnections-gConnections);
	platformReleaseMutex(ghClientMutex);

	return iNumberOfConnections;
}

/*---------------------------------------------------------------------------
** clientGetConnectionInstances( )
**
** Returns total number of connections in the state other than
** ConnectionNonExistent.
** pnConnectionIDArray should point to the array of MAX_CONNECTIONS integers.
** It will receive the connection instances.
** Example:
**
** INT32 ConnectionInstanceArray[MAX_CONNECTIONS];
** INT32 nNumConnections = EtIPGetConnectionInstances(ConnectionInstanceArray);
**
** After return ConnectionInstanceArray[0] will have the first connection instance,
** ConnectionInstanceArray[1] will have the second connection instance ...
** ConnectionInstanceArray[nNumConnections-1] will have the last connection 
** instance.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionInstances(INT32 *pnConnectionInstanceArray)
{
	INT32 nCount = 0;
	CONNECTION* pConnection;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		*pnConnectionInstanceArray = pConnection->cfg.nInstance;
		pnConnectionInstanceArray++;
		nCount++;
	}

	platformReleaseMutex(ghClientMutex);

	return nCount;
}


/*---------------------------------------------------------------------------
** clientGetConnectionState( )
**
** Return the state of the particular connection from the EtIPConnectionState
** enumeration.
** nConnectionInstance is the connection instance obtained by calling 
** EtIPGetConnectionInstances() and should be between 1 and MAX_CONNECTIONS.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionState(INT32 nConnectionInstance)
{
	CONNECTION*  pConnection;
	INT32  nState = ConnectionNonExistent;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) != NULL )
		nState = pConnection->lConnectionState;

	platformReleaseMutex(ghClientMutex);

	return nState;
}

/*---------------------------------------------------------------------------
** clientGetConnectionRunIdleFlag( )
**
** Should be called for incoming I/O connections only to get the Run/Idle 
** flag that came with the run/idle header in the I/O message.
** Returns a value from the EtIPConnectionRunIdleFlag enumeration.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionRunIdleFlag(INT32 nConnectionInstance)
{
	CONNECTION*  pConnection;
	INT32 nFlag;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) != NULL )
	{
#ifdef ET_IP_SCANNER
		if (pConnection->cfg.bOriginator)
			nFlag = (gbRunMode ? RunFlag : IdleFlag);
		else
#endif
		nFlag = (INT32)pConnection->bRunIdleFlag;
	}
	else
	{
		nFlag = UnknownRunIdleFlag;
	}

	platformReleaseMutex(ghClientMutex);
	return nFlag;
}

/*---------------------------------------------------------------------------
** clientGetConnectionConfig( )
**
** Return configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling
** EtIPGetConnectionInstances().
** pConfig is the pointer to the EtIPConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionConfig(INT32 nConnectionInstance, EtIPConnectionConfig* pConfig)
{
	CONNECTION*  pConnection;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL)
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	connectionConvertFromInternal( &pConnection->cfg, pConfig );

	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientGetConnectionInternalCfg( )
**
** Return internal configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling
** clientGetConnectionInstances().
** pConfig is the pointer to the EtIPInternalConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionInternalCfg(INT32 nConnectionInstance, EtIPInternalConnectionConfig* pConfig)
{
	CONNECTION*	pConnection;
	SESSION* pSession;
	UINT16 iOutputDataSize, iInputDataSize;
	ASSEMBLY *pProduceAssembly, *pConsumeAssembly;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL)
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	pConfig->lProducingCID = pConnection->lProducingCID;
	pConfig->lConsumingCID = pConnection->lConsumingCID;
	pConfig->iConnectionSerialNbr = pConnection->iConnectionSerialNbr;
	pConfig->iOrigVendorID = pConnection->iVendorID;
	pConfig->lOrigDeviceSerialNbr = pConnection->lDeviceSerialNbr;

	if (connectionIsIOConnection(pConnection))
	{
#ifdef ET_IP_SCANNER
		if (pConnection->cfg.bOriginator)
		{
			iOutputDataSize = (UINT16)(pConnection->cfg.bOutputRunProgramHeader ? (pConnection->cfg.iOutputScannerSize + sizeof(UINT32)) : pConnection->cfg.iOutputScannerSize);
			iInputDataSize = (UINT16)(pConnection->cfg.bInputRunProgramHeader ? (pConnection->cfg.iInputScannerSize + sizeof(UINT32)) : pConnection->cfg.iInputScannerSize);
		}
		else
		{
#endif
			if (( pConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint ) && 
				( pConnection->cfg.iTargetProducingConnPoint != gListenOnlyConnPoint ))
			{
				pProduceAssembly = assemblyGetByInstance(pConnection->cfg.iTargetProducingConnPoint);
				if (pProduceAssembly == NULL)
				{
					/* This should never happen */
					platformReleaseMutex(ghClientMutex);
					return ERR_ASSEMBLY_INVALID_INSTANCE;
				}
				
				iOutputDataSize = (UINT16)(pConnection->cfg.bOutputRunProgramHeader ? (pProduceAssembly->iSize + sizeof(UINT32)) : pProduceAssembly->iSize);
			}
			else
			{
				iOutputDataSize = (UINT16)(pConnection->cfg.bOutputRunProgramHeader ? sizeof(UINT32) : 0);			
			}
			
			if ((pConnection->cfg.iTargetConsumingConnPoint != gHeartbeatConnPoint) &&
				(pConnection->cfg.iTargetConsumingConnPoint != gListenOnlyConnPoint))
			{
				pConsumeAssembly = assemblyGetByInstance(pConnection->cfg.iTargetConsumingConnPoint);
				if (pConsumeAssembly == NULL)
				{
					/* This should never happen */
					platformReleaseMutex(ghClientMutex);
					return ERR_ASSEMBLY_INVALID_INSTANCE;
				}

				iInputDataSize = (UINT16)(pConnection->cfg.bInputRunProgramHeader ? (pConsumeAssembly->iSize + sizeof(UINT32)) : pConsumeAssembly->iSize);
			}
			else
			{
				iInputDataSize = (UINT16)(pConnection->cfg.bInputRunProgramHeader ? sizeof(UINT32) : 0);				
			}

#ifdef ET_IP_SCANNER
		}
#endif
		pConfig->lSocket = pConnection->lClass1Socket;
	}
	else
	{
		/* Size for class3 connections */
		iOutputDataSize = pConnection->cfg.iCurrentProducingSize;
		iInputDataSize = pConnection->cfg.iCurrentConsumingSize;

		pSession = sessionGetBySessionId(pConnection->nSessionId);
		pConfig->lSocket = (pSession != NULL) ? pSession->lSocket : 0;
	}

	if ((iOutputDataSize > MAX_REG_FWD_OPEN_DATA_SIZE) || (iInputDataSize > MAX_REG_FWD_OPEN_DATA_SIZE))
	{
		/* Large Forward Open used */

							/* Add 2 bytes for data sequence count that is increased when data changes */
		pConfig->lProducingConnectionParams = ((UINT32)iOutputDataSize + ((pConnection->cfg.bTransportClass != Class0) ? DATA_SEQUENCE_COUNT_SIZE : 0)) |
							/* Exclusive owner? */
							((pConnection->cfg.bRedundantOwner) ? CM_CP_LARGE_REDUNDANT_OWNER : ((UINT32)CM_CP_EXCLUSIVE_OWNER << 16)) |
							/* variable size? */
							(((pConnection->cfg.bClass1VariableProducer) || (pConnection->cfg.bTransportClass == Class3)) ? 
									((UINT32)CM_CP_VARIABLE_SIZE << 16) : ((UINT32)CM_CP_FIXED_SIZE << 16)) |
							((UINT32)pConnection->cfg.iProducingConnectionType << 16) |
							((UINT32)pConnection->cfg.iProducingPriority << 16);

							/* Add 2 bytes for data sequence count that is increased when data changes */
		pConfig->lConsumingConnectionParams = ((UINT32)iInputDataSize + ((pConnection->cfg.bTransportClass != Class0) ? DATA_SEQUENCE_COUNT_SIZE : 0)) |
							/* variable size? */
							(((pConnection->cfg.bClass1VariableConsumer) || (pConnection->cfg.bTransportClass == Class3)) ? 
									((UINT32)CM_CP_VARIABLE_SIZE << 16) : ((UINT32)CM_CP_FIXED_SIZE << 16)) |
							((UINT32)pConnection->cfg.iConsumingConnectionType << 16) |
							((UINT32)pConnection->cfg.iConsumingPriority << 16);
	}
	else
	{
							/* Add 2 bytes for data sequence count that is increased when data changes */
		pConfig->lProducingConnectionParams = (UINT16)((iOutputDataSize + ((pConnection->cfg.bTransportClass != Class0) ? DATA_SEQUENCE_COUNT_SIZE : 0)) |
							/* Exclusive owner? */
							((pConnection->cfg.bRedundantOwner) ? CM_CP_REDUNDANT_OWNER : CM_CP_EXCLUSIVE_OWNER) |
							/* variable size? */
							(((pConnection->cfg.bClass1VariableProducer) || (pConnection->cfg.bTransportClass == Class3)) ? 
									CM_CP_VARIABLE_SIZE : CM_CP_FIXED_SIZE) |
							pConnection->cfg.iProducingConnectionType |
							pConnection->cfg.iProducingPriority);

							/* Add 2 bytes for data sequence count that is increased when data changes */
		pConfig->lConsumingConnectionParams = (UINT16)((iInputDataSize + ((pConnection->cfg.bTransportClass != Class0) ? DATA_SEQUENCE_COUNT_SIZE : 0)) |
							/* variable size? */
							(((pConnection->cfg.bClass1VariableConsumer) || (pConnection->cfg.bTransportClass == Class3)) ? 
							CM_CP_VARIABLE_SIZE : CM_CP_FIXED_SIZE) |
							pConnection->cfg.iConsumingConnectionType |
							pConnection->cfg.iConsumingPriority);
	}

	pConfig->lProducingIP = platformGetInternetAddress(pConnection->sTransmitAddr.sin_addr);
	pConfig->lProducingPort = pConnection->sTransmitAddr.sin_port;
	pConfig->lConsumingIP = platformGetInternetAddress(pConnection->sReceiveAddr.sin_addr);
	pConfig->lConsumingPort = pConnection->sReceiveAddr.sin_port;

	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientGetConnectionStats( )
**
** Return connection stats for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** pStats is the pointer to the EtIPConnectionStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
INT32 clientGetConnectionStats(INT32 nConnectionInstance, EtIPConnectionStats* pStats, BOOL bClear)
{
#ifdef CONNECTION_STATS
	CONNECTION* pConnection;
	UINT32  lTickCount;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL )
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	if ( !pConnection->bCollectingStats )
	{
		platformReleaseMutex(ghClientMutex);
		return 1;
	}

	pStats->lLostPackets = pConnection->lLostPackets;
	pStats->lMaxRcvdDelay = pConnection->lMaxRcvdDelay;
	pStats->lMaxSendDelay = pConnection->lMaxSendDelay;
	pStats->lRcvdPackets = pConnection->lTotalRcvdPackets;
	pStats->lSendPackets = pConnection->lTotalSendPackets;
	pStats->iSendPacketError = pConnection->iSendPacketError;
	pStats->iRcvdPacketError = pConnection->iRcvdPacketError;
	pStats->iNumTimeouts = pConnection->iNumTimeouts;
	pStats->iNumErrors = pConnection->iNumErrors;
	pStats->lLastRcvdTimestamp = pConnection->lLastRcvdTick;
	pStats->lLastSendTimestamp = pConnection->lLastSendTick;
	pStats->lRcvdBytes = pConnection->lTotalRcvdBytes;
	pStats->lSendBytes = pConnection->lTotalSendBytes;

	lTickCount = gdwTickCount;

	if ( pStats->lRcvdPackets )
	{
		if ( lTickCount > pConnection->lStartRcvdTick )
			pStats->fRcvdAPI = ((float)(lTickCount-pConnection->lStartRcvdTick))/pStats->lRcvdPackets;
		else
			pStats->fRcvdAPI = ((float)((0xffffffff-pConnection->lStartRcvdTick)+lTickCount))/pStats->lRcvdPackets;
	}
	else
		pStats->fRcvdAPI = 0;

	if ( pStats->lSendPackets )
	{
		if ( lTickCount > pConnection->lStartSendTick )
			pStats->fSendAPI = ((float)(lTickCount-pConnection->lStartSendTick))/pStats->lSendPackets;
		else
			pStats->fSendAPI = ((float)((0xffffffff-pConnection->lStartSendTick)+lTickCount))/pStats->lSendPackets;
	}
	else
		pStats->fSendAPI = 0;

	if (bClear)
	{
		pConnection->lLostPackets = 0;
		pConnection->lMaxRcvdDelay = 0;
		pConnection->lMaxSendDelay = 0; 
		pConnection->lTotalRcvdPackets = 0;    
		pConnection->lTotalSendPackets = 0;
		pConnection->iSendPacketError = 0;
		pConnection->iRcvdPacketError = 0;
		pConnection->iNumTimeouts = 0;
		pConnection->iNumErrors = 0;
		pConnection->lTotalSendBytes = 0;
		pConnection->lTotalRcvdBytes = 0;
	}


	platformReleaseMutex(ghClientMutex);

	return 0;
#else
	return -1;
#endif /*CONNECTION_STATS */

}

/*---------------------------------------------------------------------------
** clientSetConnectionProducedSize( )
**
** Set the producing size for a variable sized class1 connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** iSize is the new size sent on the I/O connection. The size must be equal to or 
** lesser than the original connection size
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state. Or 
** ERR_INVALID_VARIABLE_CONNECTION_SIZE if the size specified is too large.
**---------------------------------------------------------------------------
*/
INT32 clientSetConnectionProducedSize(INT32 nConnectionInstance, UINT16 iSize)
{
	CONNECTION* pConnection;
	ASSEMBLY* pAssembly;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL )
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	if ((connectionIsIOConnection(pConnection) == FALSE) ||
		(pConnection->cfg.bClass1VariableProducer != TRUE))
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_CONNECTION_NOT_VARIABLE_TYPE;
	}

#ifdef ET_IP_SCANNER
	if (pConnection->cfg.bOriginator)
	{
		if (iSize > pConnection->cfg.iOutputScannerSize)
		{
			platformReleaseMutex(ghClientMutex);
			return ERR_INVALID_VARIABLE_CONNECTION_SIZE;
		}

		pConnection->cfg.iCurrentProducingSize = iSize;
	}
	else
	{
#endif
		pAssembly = assemblyGetByInstance(pConnection->cfg.iTargetProducingConnPoint);
		if ( pAssembly == NULL )
		{
			/* Should never happen */
			platformReleaseMutex(ghClientMutex);
			return ERR_ASSEMBLY_INVALID_INSTANCE;
		}

		if (iSize > pAssembly->iSize)
		{
			platformReleaseMutex(ghClientMutex);
			return ERR_INVALID_VARIABLE_CONNECTION_SIZE;
		}

		pConnection->cfg.iCurrentProducingSize = iSize;
#ifdef ET_IP_SCANNER
	}
#endif

	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientSetNewConnectionInstance( )
**
** Set a new connection ID for a particular connection.  This is aimed at applications
** organizing their target connections because the EIP stack initially assigned it an ID.
** nOldConnectionInstance is the existing connection instance
** nNewConnectionInstance is the new connection instance
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nOldConnectionInstance is out of the connection instance range or doesn't exist.
** Also returns ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED if nNewConnectionInstance is
** already in use.
**---------------------------------------------------------------------------
*/
INT32 clientSetNewConnectionInstance(INT32 nOldConnectionInstance, INT32 nNewConnectionInstance)
{
	CONNECTION *pConnection;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nOldConnectionInstance)) == NULL )
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	if ((pConnection->cfg.bOriginator == FALSE) &&
		(pConnection->lConnectionState != ConnectionEstablished))
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_CONNECTION_CONFIGURATION_INVALID;
	}

	if (connectionGetFromInstance(nNewConnectionInstance) != NULL )
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	pConnection->cfg.nInstance = nNewConnectionInstance;

	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientSendConnectionResponse( )
**
** Send a response to a potential connection.
** nConnectionInstance is the connection instance passed by NM_CONNECTION_VERIFICATION notification
** pErrorInfo is the response from the application.  If bGeneralStatus = 0, connection is approved/successful
** bAppSize is the size of Application Data (in words) appended to the ForwardOpen reply (typically 0)
** pAppData is the Application Data (in words) appended to the ForwardOpen reply
** See CIP specification for details on error codes.
**---------------------------------------------------------------------------
*/
INT32 clientSendConnectionResponse(INT32 nConnectionInstance, EtIPErrorInfo* pErrorInfo, UINT8 bAppSize, UINT8* pAppData)
{
	CONNECTION* pConnection;
	REQUEST* pRequest;
	UINT16 iModuleConfigSize, *pResponseErrorData;
	UINT8 i, *pModuleConfigData, *pRequestErrorData;
	FWD_OPEN FwdOpen;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL)
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	if ( !Is_Remote_IP_Trusted( pConnection->cfg.lIPAddress, htons( pConnection->sReceiveAddr.sin_port ) ) )
	{
		connectionRemove( pConnection, FALSE );
		platformReleaseMutex(ghClientMutex);          
		return NM_ERROR_USING_WINSOCK;
	}
	if ((pRequest = requestGetByRequestId(pConnection->nRequestId)) == NULL)
	{
		/* If we can't find the request, remove the connection so it doesn't lie around
		   preventing other connections from being established. */
		connectionRemove( pConnection, FALSE );
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	if (pErrorInfo->bGeneralStatus == 0)
	{
		/* This connection has been approved */

#ifdef ET_IP_MODBUS
		if ((pConnection->cfg.modbusCfg.iModbusConsumeSize > 0) ||
			(pConnection->cfg.modbusCfg.iModbusProduceSize > 0))
		{
			connmgrPrepareConnection(pConnection, pRequest, 0, NULL);
		}
		else
		{
#endif
			/* Set module configuration data (if applicable) */
			pModuleConfigData = (UINT8*)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_DATA, 0, &iModuleConfigSize );
			assemblySetInstanceData(pConnection->cfg.iTargetConfigConnInstance, pModuleConfigData, iModuleConfigSize);

			connmgrPrepareConnection(pConnection, pRequest, bAppSize, pAppData);
#ifdef ET_IP_MODBUS
		}
#endif
	}
	else
	{
		/* remove the remnents of the Forward Open request */
		connmgrInitFwdOpenTypeFromBuf(pRequest->cfg.objectCfg.pData, &FwdOpen, pRequest->cfg.objectCfg.bService == LARGE_FWD_OPEN_CMD_CODE);
		connmgrPrepareFwdOpenErrorReply(pRequest, &FwdOpen, pErrorInfo->bGeneralStatus, pErrorInfo->iExtendedStatus);

		connmgrStats.iOpenOtherRejects++;
		connectionRemove( pConnection, FALSE );

		/* add the additional status information */
		pRequest->iExtendedErrorDataSize = (UINT16)(pErrorInfo->iExtendedStatusDataSize*2);
		pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
		if ( pRequest->iExtendedErrorDataSize && pRequest->pExtendedErrorData == NULL )
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			platformReleaseMutex(ghClientMutex);
			return NM_OUT_OF_MEMORY;
		}

		for (i = 0, pResponseErrorData = pErrorInfo->extendedStatusData, pRequestErrorData = pRequest->pExtendedErrorData; 
			 i < pErrorInfo->iExtendedStatusDataSize; 
			 i++, pResponseErrorData++, pRequestErrorData += sizeof(UINT16))
		{
			UINT16_SET(pRequestErrorData, *pResponseErrorData);
		}

		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
	}

	platformReleaseMutex(ghClientMutex);

	return 0;
}


/*---------------------------------------------------------------------------
** clientAbortAllConnections( )
**
** Remove all connections.  This will close all originated and abort
** all target connections regardless of class.
**---------------------------------------------------------------------------
*/
void clientAbortAllConnections()
{
	CONNECTION* pLoopConnection;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
	{
		/* if we originated the connection - close it */
		if (pLoopConnection->cfg.bOriginator)
		{
			DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pLoopConnection->cfg.lHostIPAddr, pLoopConnection->cfg.lIPAddress, "Closing connection");
			connListSetConnectionConfigState(pLoopConnection, ConfigurationClosing, 0);

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			/* Reset signature to 0 to indicate that configuration has changed */
			glEditSignature = 0;
#endif
		}

		/* if this is a target connection we can't close it, so just abort it */
		else
		{
			DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pLoopConnection->cfg.lHostIPAddr, pLoopConnection->cfg.lIPAddress, "Aborting target connection");
			connListSetConnectionConfigState(pLoopConnection, ConfigurationAbortConnection, 0);
		}
	}

	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientSetInputAndListenOnlyConnPts( )
**
** Set the values to be used for InputOnly and ListenOnly connections points.
** This is intended for devices/applications that can't (or don't want to) change
** the HEARTBEAT_CONN_POINT and LISTEN_ONLY_CONN_POINT macros and provides backwards
** compatibility since HEARTBEAT_CONN_POINT and LISTEN_ONLY_CONN_POINT 
** have been changed
** 
**---------------------------------------------------------------------------
*/
void clientSetInputAndListenOnlyConnPts(UINT32 lInputOnlyConnPt, UINT32 lListenOnlyConnPt)
{
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gHeartbeatConnPoint = lInputOnlyConnPt;
	gListenOnlyConnPoint = lListenOnlyConnPt;
	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientGetRunMode()
**
** Returns Run mode setting. Initially set to TRUE. May be modified by
** calling clientSetRunMode() function.
** The Run mode is being sent as the first bit of the 32-bit I/O header,
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
BOOL clientGetRunMode(void)
{
	BOOL bRunMode;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	bRunMode = gbRunMode;

	platformReleaseMutex(ghClientMutex);

	return bRunMode;
}

/*---------------------------------------------------------------------------
** clientSetRunMode()
**
** Set Run mode by passing TRUE, or Idle mode by pasing FALSE as a parameter.
** The state is being sent as the first bit of the 32-bit I/O header, 
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
void clientSetRunMode( BOOL bRunMode )
{
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	/* Update the data sequence count to indicate new data when the run mode changes */
	if ( gbRunMode != bRunMode )
		connectionUpdateAllSeqCount();

	gbRunMode = bRunMode;

	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientStartRequestGroup( )
**
** Starts the unconnected request group used to send multiple service 
** message. All subsequent clientSendUnconnectedRequest and 
** clientSendEPATHUnconnectedRequest calls issued before next clientStopRequestGroup
** is called are grouped into a single multiple service request. It will be sent
** after clientStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
INT32 clientStartRequestGroup(void)
{
#ifndef EIP_NO_ORIGINATE_UCMM
	REQUEST_GROUP* pGroup = requestGroupNewOutgoing( OBJECT_REQUEST );

	if ( pGroup == NULL )
	{
		/* No more space for request groups */
		notifyEvent( NM_PENDING_REQUEST_GROUPS_LIMIT_REACHED, 0, 0 );
		return NM_PENDING_REQUEST_GROUPS_LIMIT_REACHED;
	}

	gnCurrentRequestGroupIndex = pGroup->nIndex;
	return 0;
#else
	return ERR_SENDING_UCMMS_NOT_SUPPORTED;
#endif
}

/*---------------------------------------------------------------------------
** clientStopRequestGroup( )
**
** Stops forming the unconnected request group used to send multiple service
** message. All clientSendUnconnectedRequest and clientSendEPATHUnconnectedRequest
** calls issued between clientStartRequestGroup and clientStopRequestGroup
** are grouped into a single multiple service request. It will be sent
** after clientStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
void clientStopRequestGroup(void)
{
#ifndef EIP_NO_ORIGINATE_UCMM
	gnCurrentRequestGroupIndex = INVALID_REQUEST_INDEX;
#endif
}

/*---------------------------------------------------------------------------
** clientSendUnconnectedRequest( )
**
** Send either UCMM or Unconnected Send depending on whether szNetworkPath
** specifies local or remote target.
** clientGetUnconnectedResponse will be used to get the response.
** pRequest points to a structure containing the request parameters.
** Check the return value. If it's greater than or equal to REQUEST_INDEX_BASE,
** then it is a request Id to be used when calling EtIPGetResponse.
** If it is less than REQUEST_INDEX_BASE then it's one of the 
** _EtIPNotificationMessages_ errors.
**---------------------------------------------------------------------------
*/
INT32 clientSendUnconnectedRequest( EtIPObjectRequest* pRequest )
{
#ifndef EIP_NO_ORIGINATE_UCMM
	INT32 nRequest;
	REQUEST* pReq;
	REQUEST_GROUP* pReqGroup;
	BOOL bLocalRequest;
	char szExtendedPath[MAX_NETWORK_PATH_SIZE];
	UINT16 iExtendedPathLen;
	char szNetworkPath[MAX_NETWORK_PATH_SIZE];

	strcpy( szNetworkPath, (const char*)pRequest->networkPath );

	if ( !utilParseNetworkPath( szNetworkPath, &bLocalRequest, szExtendedPath, &iExtendedPathLen ) )
		return NM_REQUEST_FAILED_INVALID_NETWORK_PATH;

	/* If the user did not specify Request Host IPAddress - use the first one */
	if ((pRequest->lHostIPAddr == INADDR_NONE) || (pRequest->lHostIPAddr == INADDR_ANY))
		pRequest->lHostIPAddr = gsTcpIpInstAttr[0].InterfaceConfig.lIpAddr;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( bLocalRequest )
	{
		/* Schedule UCMM */
		nRequest = requestNewUnconnectedSend( (unsigned char *)szNetworkPath, pRequest, NULL, 0 );
	}
	else
	{
		/* Schedule Unconnected Send */
		nRequest = requestNewUnconnectedSend( (unsigned char *)szNetworkPath, pRequest, szExtendedPath, iExtendedPathLen );
	}

	if ( gnCurrentRequestGroupIndex != INVALID_REQUEST_INDEX )
	{
		pReq = requestGetByRequestId( nRequest );
		if ( pReq )
		{
			pReqGroup = requestGroupGetById( gnCurrentRequestGroupIndex );
			if ( pReqGroup )
			{
				if ( pReqGroup->lIPAddress == 0 )
				{
					pReqGroup->lIPAddress = pReq->lIPAddress;
					pReqGroup->lHostIPAddr = pReq->lHostIPAddr;
				}
				else if ( pReqGroup->lIPAddress != pReq->lIPAddress || pReqGroup->lHostIPAddr != pReq->lHostIPAddr )
				{
					requestRemove( pReq );
					platformReleaseMutex(ghClientMutex);
					return ERR_REQUEST_GROUP_MISMATCH;
				}
			}

			pReq->nRequestGroupIndex = gnCurrentRequestGroupIndex;
		}
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, 0, platformAddrFromPath(szNetworkPath, (INT32)strlen(szNetworkPath)),
					"Internal error: created and lost request %d", nRequest);
	}

	platformReleaseMutex(ghClientMutex);

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, platformAddrFromPath(szNetworkPath, (INT32)strlen(szNetworkPath)),
		"Sending explicit request %d", nRequest);

	return nRequest;
#else
	return ERR_SENDING_UCMMS_NOT_SUPPORTED;
#endif
}

/*---------------------------------------------------------------------------
** clientSendEPATHUnconnectedRqst( )
**
** Same as clientSendUnconnectedRequest above, but EPATH is populated by the
** user. May be used in cases where more complicated EPATH should be used.
** Any available segment type mey be include in EPATH or the user may
** specify word size for parameters like class, instance, or member.
**---------------------------------------------------------------------------
*/
INT32 clientSendEPATHUnconnectedRqst( EtIPEPATHObjectRequest* pEPATHRequest )
{
#ifndef EIP_NO_ORIGINATE_UCMM
	INT32          nRequest;
	REQUEST*       pReq;
	BOOL           bLocalRequest;
	char           szExtendedPath[MAX_NETWORK_PATH_SIZE];
	UINT16         iExtendedPathLen;
	char           szNetworkPath[MAX_NETWORK_PATH_SIZE];
	REQUEST_GROUP* pReqGroup;
    
	strcpy( szNetworkPath, (const char*)pEPATHRequest->networkPath );

	if ( !utilParseNetworkPath( szNetworkPath, &bLocalRequest, szExtendedPath, &iExtendedPathLen ) )
	    return NM_REQUEST_FAILED_INVALID_NETWORK_PATH;

	/* If the user did not specify Request Host IPAddress - use the first one */
	if ((pEPATHRequest->lHostIPAddr == INADDR_NONE) || (pEPATHRequest->lHostIPAddr == INADDR_ANY))
	    pEPATHRequest->lHostIPAddr = gsTcpIpInstAttr[0].InterfaceConfig.lIpAddr;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	if ( bLocalRequest )
	{
		/* Schedule UCMM */
		nRequest = requestNewEPATHUnconnectedSend( (unsigned char *)szNetworkPath, pEPATHRequest, NULL, 0 );
	}
	else
	{
		/* Schedule Unconnected Send */
		nRequest = requestNewEPATHUnconnectedSend( (unsigned char *)szNetworkPath, pEPATHRequest, szExtendedPath, iExtendedPathLen );
	}

	if ( gnCurrentRequestGroupIndex != INVALID_REQUEST_INDEX )
	{
		pReq = requestGetByRequestId( nRequest );
		if ( pReq )
		{
			pReqGroup = requestGroupGetById( gnCurrentRequestGroupIndex );
			if ( pReqGroup )
			{
				if ( pReqGroup->lIPAddress == 0 )
				{
					pReqGroup->lIPAddress = pReq->lIPAddress;
					pReqGroup->lHostIPAddr = pReq->lHostIPAddr;
				}
				else if ( pReqGroup->lIPAddress != pReq->lIPAddress || pReqGroup->lHostIPAddr != pReq->lHostIPAddr )
				{
					requestRemove( pReq );
					platformReleaseMutex(ghClientMutex);
					return ERR_REQUEST_GROUP_MISMATCH;
				}
			}

			pReq->nRequestGroupIndex = gnCurrentRequestGroupIndex;
		}
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, 0, platformAddrFromPath(szNetworkPath, (INT32)strlen(szNetworkPath)),
					"Internal error: created and lost request %d", nRequest);
	}

	platformReleaseMutex(ghClientMutex);

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pEPATHRequest->lHostIPAddr, platformAddrFromPath(szNetworkPath, (INT32)strlen(szNetworkPath)),
		"Sending explicit EPATH request %d", nRequest);

	return nRequest;
#else
	return ERR_SENDING_UCMMS_NOT_SUPPORTED;
#endif
}


/*---------------------------------------------------------------------------
** clientGetUnconnectedResponse( )
**
** Get a response for the previously sent unconnected message.
** nRequestId is a request Id returned from the previously sent
** EtIPSendUCMMRequest message. 
** Returns 0 in case of success or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 clientGetUnconnectedResponse(INT32 nRequestId, EtIPObjectResponse* pResponse)
{
#ifndef EIP_NO_ORIGINATE_UCMM
	REQUEST* pRequest;
	UINT8 i, *pRequestErrorData;
	UINT16 *pResponseErrorData;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	if ( pRequest->nState != REQUEST_RESPONSE_RECEIVED )
	{
		platformReleaseMutex(ghClientMutex);
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"clientGetUnconnectedResponse RequestId = 0x%x, state = %d - response not ready yet", nRequestId, pRequest->nState);
		/* Request is still pending without a response */
		return ERR_NO_AVAILABLE_OBJECT_RESPONSE;
	}

	pResponse->bGeneralStatus = pRequest->bGeneralError;
	pResponse->iExtendedStatus = pRequest->iExtendedError;
	pResponse->iDataSize = pRequest->cfg.objectCfg.iDataSize;
	memcpy( pResponse->responseData, pRequest->cfg.objectCfg.pData, pResponse->iDataSize );
	pResponse->iExtendedStatusDataSize = (UINT16)(pRequest->iExtendedErrorDataSize/2);
	for (i = 0, pResponseErrorData = pResponse->extendedStatusData, pRequestErrorData = pRequest->pExtendedErrorData; 
		 i < pResponse->iExtendedStatusDataSize; 
		 i++, pResponseErrorData++, pRequestErrorData += sizeof(UINT16))
	{
		*pResponseErrorData = UINT16_GET(pRequestErrorData);
	}

	if (pRequest->iExtendedErrorDataSize%2)
	{
		*pResponseErrorData = (UINT16)(*pRequestErrorData);
		pResponse->iExtendedStatusDataSize++;
	}

	if ( pResponse->bGeneralStatus )
		notifyGetCIPErrorDescription( pResponse->bGeneralStatus, pResponse->iExtendedStatus, (unsigned char*)pResponse->errorDescription );
	else
		pResponse->errorDescription[0] = 0;

	requestRemove( pRequest );
	platformReleaseMutex(ghClientMutex);
	return 0;
#else
	return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
#endif
}


/*---------------------------------------------------------------------------
** clientRegObjectsForClientProc( )
**
** Provides the list of the objects that will be processed by the client
** application. When the request is received for one of these objects
** the NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be 
** sent to the client app. The client application can use clientGetClientRequest()
** and clientSendClientResponse() after that to get the request and
** send the response.
** pClassNumberList is the pointer to the integer array. Each integer represent
** the class number of the object that will be proccessed by the client
** application.
** nNumberOfClasses is the number of classes in the list.
** pServiceNumberList is the pointer to the integer array. Each integer represent
** the service that will be proccessed by the client application.
** nNumberOfServices is the number of the services in the list.
**---------------------------------------------------------------------------
*/
void clientRegObjectsForClientProc(UINT32* pClassNumberList, UINT32 nNumberOfClasses, 
										  UINT32* pServiceNumberList, UINT32 nNumberOfServices)
{
	UINT32 i;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	for( i = 0; i < nNumberOfClasses; i++ )
		gpClientAppProcClasses[i] = pClassNumberList[i];

	glClientAppProcClassNbr = (UINT16)nNumberOfClasses;

	for( i = 0; i < nNumberOfServices; i++ )
		gpClientAppProcServices[i] = pServiceNumberList[i];

	glClientAppProcServiceNbr = nNumberOfServices;

	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientRegRequestsForClientProc( )
**
** Provides the list of specific explicit requests that will be processed by the client
** application.  The intent is to override or extend existing functionality of the
** objects already in the EtherNet/IP library without modifying the source directly.
** When the request is received for one of these objects the
** NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be
** sent to the client app. The client application can use clientGetClientRequest()
** and clientSendClientResponse() after that to get the request and
** send the response.
** pSpecificRequests is the pointer to an array of specific services within objects
** already included in the EtherNet/IP stack. Each "specific object" represents
** the service/class/instance/attribute combination that will be proccessed
** by the client application.
** nNumberOfSpecificObjects is the number of the specific object requests in the list.
**---------------------------------------------------------------------------
*/
void clientRegRequestsForClientProc(EtIPObjectRegisterType* pSpecificRequests, UINT32 nNumberOfSpecificObjects)
{
	UINT32 i;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	for( i = 0; i < nNumberOfSpecificObjects; i++ )
		gpClientAppProcObjects[i] = pSpecificRequests[i];

	glClientAppProcObjNbr = nNumberOfSpecificObjects;

	platformReleaseMutex(ghClientMutex);
}


/*---------------------------------------------------------------------------
** EtIPGetClientRequest( )
**
** Gets the pending request that should be processed by the client
** application. The objects for the client application processing must
** be registered in advance by calling EtIPRegisterObjectsForClientProcessing()
** function.
** The client application will get NM_CLIENT_OBJECT_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** EtIPGetClientRequest() function.
** pRequest is the pointer to the EtIPObjectRequest structure that will have
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no
** pending request with this Id.
**---------------------------------------------------------------------------
*/
INT32 clientGetClientRequest( INT32 nRequestId, EtIPObjectRequest* pObjectRequest )
{
	REQUEST* pRequest;
	UINT16 iTagSize;
	UINT32 iClass, iInstance, iAttribute, iMember;
	ETAG_DATATYPE pData;
	EPATH  pdu;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	/* Special handling for "internal" UnconnectedSends */
	if ((pRequest->nType == UNCONNECTED_SEND_REQUEST) ||
		((pRequest->nType == CLASS3_REQUEST) && (pRequest->iExtendedPathSize > 0)))
	{
		pduInit( &pdu, TRUE );
		pduParse(pRequest->pExtendedPath, &pdu, TRUE, pRequest->iExtendedPathSize);

		utilBuildNetworkPath(pObjectRequest->networkPath, pRequest->lHostIPAddr, &pdu);

		pduRelease(&pdu);
		EtIP_free(&pRequest->pExtendedPath, &pRequest->iExtendedPathSize);

		pObjectRequest->lExplicitMessageTimeout = pRequest->lRequestTimeoutTick-gdwTickCount;
	}
	else
	{
		strcpy((char*)pObjectRequest->networkPath, (const char*)socketGetAddressString(pRequest->lIPAddress));
	}

	pObjectRequest->bService = pRequest->cfg.objectCfg.bService;

	iClass = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_CLASS, 0, &iTagSize);
	pObjectRequest->iClass = iTagSize ? iClass : INVALID_CLASS;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	pObjectRequest->iInstance = iTagSize ? iInstance : INVALID_INSTANCE;

	iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	pObjectRequest->iAttribute = iTagSize ? iAttribute : INVALID_ATTRIBUTE;

	iMember = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize);
	pObjectRequest->iMember = iTagSize ? iMember : INVALID_MEMBER;

	pData = pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_EXT_SYMBOL, 0, &iTagSize);
	if ( iTagSize )
	{
		pObjectRequest->iTagSize = iTagSize;
		memcpy(pObjectRequest->requestTag, (UINT8*)pData, iTagSize );
		pObjectRequest->requestTag[iTagSize] = 0;
	}
	else
		pObjectRequest->iTagSize = 0;

	pObjectRequest->iDataSize = pRequest->cfg.objectCfg.iDataSize;
	memcpy(pObjectRequest->requestData, pRequest->cfg.objectCfg.pData, pObjectRequest->iDataSize);

	pObjectRequest->lHostIPAddr = pRequest->lHostIPAddr;

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
			"clientGetClientRequest new client request = 0x%x", pRequest->nIndex);

	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientGetEPATHClientRequest( )
**
** Same as EtIPGetClientRequest above, but EPATH may be used in cases where
** more complicated EPATH may be used.
** Any available segment type may be include in EPATH including duplicate
** occurences of the same type.
**---------------------------------------------------------------------------
*/
INT32 clientGetEPATHClientRequest( INT32 nRequestId, EtIPEPATHObjectRequest* pObjectRequest )
{
	REQUEST* pRequest;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	strcpy((char*)pObjectRequest->networkPath, (const char*)socketGetAddressString(pRequest->lIPAddress));
	pObjectRequest->bService = pRequest->cfg.objectCfg.bService;

	memcpy( &pObjectRequest->ePATH, &pRequest->cfg.objectCfg.PDU, sizeof(EPATH) );

	pObjectRequest->iDataSize = pRequest->cfg.objectCfg.iDataSize;
	memcpy(pObjectRequest->requestData, pRequest->cfg.objectCfg.pData, pObjectRequest->iDataSize);

	pObjectRequest->lHostIPAddr = pRequest->lHostIPAddr;

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
			"clientGetEPATHClientRequest new client request = 0x%x", pRequest->nIndex);

	platformReleaseMutex(ghClientMutex);

	return 0;
}

#ifdef EIP_PCCC_SUPPORT
/*---------------------------------------------------------------------------
** clientGetPCCCRequest( )
**
** Gets the pending PCCC request that should be processed by the client
** application. 
** The client application will get NM_CLIENT_PCCC_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** clientGetPCCCRequest() function.
** pRequest is the pointer to the EtIPPCCCRequest structure that will have
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
INT32 clientGetPCCCRequest(INT32 nRequestId, EtIPPCCCRequest* pPCCCRequest)
{
	REQUEST* pRequest;
	UINT8 *pData;
	UINT8 bExtCommand = 0, bLength = 0;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	switch(pRequest->nType)
	{
#ifdef SUPPORT_CSP
	case PCCC_REQUEST:
		cspGetRequest(pRequest, pPCCCRequest);
		break;
#endif
	case OBJECT_REQUEST:
	case CLASS3_REQUEST:
		pData = pRequest->cfg.objectCfg.pData;

		if (pRequest->cfg.objectCfg.bService == PCCC_EXECUTE_SERVICE)
		{
			pPCCCRequest->bHasRequestorIDData = TRUE;

			bLength = *pData;
			pData++;

			/* get the Vendor ID and serial number */
			pPCCCRequest->iVendorId = UINT16_GET(pData);  pData += 2;
			pPCCCRequest->lSerialNumber = UINT32_GET(pData); pData += 4;

			/* get any product specific data */
			if (bLength > 7)
			{
				pPCCCRequest->iRequesterIDDataSize = (UINT8)(bLength-7);
				memcpy(pPCCCRequest->requesterIDData, pData, pPCCCRequest->iRequesterIDDataSize);
			}
		}
		else
		{
			pPCCCRequest->bHasRequestorIDData = FALSE;
		}

		pPCCCRequest->bCommand = *pData;
		if (pcccIsExtentionCommandList(pPCCCRequest->bCommand))
		{
			bExtCommand = 1;
			pPCCCRequest->bExtCommand = *(pData+4);
		}
		else
		{
			pPCCCRequest->bExtCommand = 0;
		}

		pPCCCRequest->iDataSize = (UINT8)(pRequest->cfg.objectCfg.iDataSize-bLength-4-bExtCommand);
		memcpy(pPCCCRequest->requestData, pData+4+bExtCommand, pPCCCRequest->iDataSize);

#ifdef SUPPORT_CSP
		pPCCCRequest->iAddressDataSize = 0;
#endif
		break;
	}

	pPCCCRequest->lNetworkAddr = pRequest->lIPAddress;
	pPCCCRequest->lHostIPAddr = pRequest->lHostIPAddr;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
		"clientGetPCCCRequest new client request = 0x%x", pRequest->nIndex);

	platformReleaseMutex(ghClientMutex);

	return 0;
}
#endif	/* EIP_PCCC_SUPPORT */

/*---------------------------------------------------------------------------
** clientSendClientResponse( )
**
** Send a response to the previously received request registered to be
** processed by the client application.
** The client application previously polled the request by calling
** EtIPGetClientRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
INT32 clientSendClientResponse(INT32 nRequestId, EtIPObjectResponse* pResponse)
{
	REQUEST* pRequest;
	CONNECTION* pConnection;
	UINT8 i, *pRequestErrorData;
	UINT16 *pResponseErrorData;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	pRequest->bGeneralError = pResponse->bGeneralStatus;
	pRequest->iExtendedError = pResponse->iExtendedStatus;
	pRequest->cfg.objectCfg.iDataSize = pResponse->iDataSize;
	pRequest->cfg.objectCfg.pData = EtIP_malloc(pResponse->responseData, pResponse->iDataSize);

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		requestRemove( pRequest );
		platformReleaseMutex(ghClientMutex);
		return NM_OUT_OF_MEMORY;
	}

	/* prepend the extended error data */
	pRequest->iExtendedErrorDataSize = (UINT16)(pResponse->iExtendedStatusDataSize*2);
	pRequest->pExtendedErrorData = EtIP_malloc(NULL, pRequest->iExtendedErrorDataSize);
	if ( pRequest->iExtendedErrorDataSize && pRequest->pExtendedErrorData == NULL )
	{
		requestRemove( pRequest );
		platformReleaseMutex(ghClientMutex);
		return NM_OUT_OF_MEMORY;
	}

	for (i = 0, pResponseErrorData = pResponse->extendedStatusData, pRequestErrorData = pRequest->pExtendedErrorData; 
		 i < pResponse->iExtendedStatusDataSize; 
		 i++, pResponseErrorData++, pRequestErrorData += sizeof(UINT16))
	{
		UINT16_SET(pRequestErrorData, *pResponseErrorData);
	}

	/* Save the connected response in the connection object */
	if (pRequest->nType == CLASS3_REQUEST)
	{
		pConnection = connectionGetFromSerialNumber( pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );

		if ( pConnection != NULL )
		{
			EtIP_realloc( &pConnection->pResponseData, &pConnection->iResponseDataSize, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );
			pConnection->bGeneralStatus = pRequest->bGeneralError;    /* Store general status. Should be 0 in case of success */
			pConnection->iExtendedStatus = pRequest->iExtendedError;    /* Store extended status. */
			EtIP_realloc( &pConnection->pExtendedErrorData, &pConnection->iExtendedErrorDataSize, pRequest->pExtendedErrorData, pRequest->iExtendedErrorDataSize);
		}
	}

	pRequest->nState = REQUEST_RESPONSE_RECEIVED;

	platformReleaseMutex(ghClientMutex);

	return 0;
}

#ifdef EIP_PCCC_SUPPORT
/*---------------------------------------------------------------------------
** clientSendPCCCResponse( )
**
** Send a response to the previously received PCCC request.
** The client application previously polled the request by calling
** clientGetPCCCRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
INT32 clientSendPCCCResponse(int nRequestId, EtIPPCCCResponse* pResponse)
{
	REQUEST* pRequest;
	UINT8* pData;
	UINT16 iDataSize;
	UINT8 bCommand, 
		  bLength,
		  bExtStatus = 0;
	UINT8 PCCCCache[PCCC_CACHE_SIZE];
#ifdef SUPPORT_CSP
	INT32 iCSPReturn;
#endif

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pRequest = requestGetByRequestId( nRequestId );

	if ( pRequest == NULL )
	{
		/* Request Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	switch(pRequest->nType)
	{
#ifdef SUPPORT_CSP
	case PCCC_REQUEST:
		iCSPReturn = cspProcessSendResponse(pRequest, pResponse);
		if (iCSPReturn != 0)
		{
			requestRemove( pRequest );
			platformReleaseMutex(ghClientMutex);
			return iCSPReturn;
		}
		break;
#endif
	case OBJECT_REQUEST:
	case CLASS3_REQUEST:
		pData = pRequest->cfg.objectCfg.pData;
		if (pRequest->cfg.objectCfg.bService == PCCC_EXECUTE_SERVICE)
		{
			/* Ignore client id */
			bLength = *pData; 
			pData += bLength;
		}
		else
		{
			bLength = 0;
		}

		bCommand = *pData;
		if (pResponse->bExtStatus)
			bExtStatus = 1;

		iDataSize = (UINT16)(4+bLength);
		memcpy(PCCCCache, pRequest->cfg.objectCfg.pData, iDataSize);

		EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, 
					NULL, (UINT16)(iDataSize+pResponse->iDataSize+bExtStatus));

		if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
		{
			requestRemove( pRequest );    /* We are out of memory */
			platformReleaseMutex(ghClientMutex);
			return NM_OUT_OF_MEMORY;
		}

		memcpy(pRequest->cfg.objectCfg.pData, PCCCCache, iDataSize);

		pData = pRequest->cfg.objectCfg.pData+bLength;

		/* set the reply bit in the command */
		(*pData) |= PCCC_COMMAND_REPLY_MASK;

		/* set the error code */
		*(pData+1) = pResponse->bError;

		/* set extended status */
		if (bExtStatus)
			*(pData+4) = pResponse->bExtStatus;

		memcpy(pData+4+bExtStatus, pResponse->responseData, pResponse->iDataSize);
		break;
	}

	pRequest->nState = REQUEST_RESPONSE_RECEIVED;

	platformReleaseMutex(ghClientMutex);

	return 0;
}
#endif /* EIP_PCCC_SUPPORT */

/*---------------------------------------------------------------------------
** clientGetIdentityInfo( )
**
** Returns identity information.
** The client application is responsible for allocating EtIPIdentityInfo
** structure and passing its pointer with the EtIPGetIdentityInfo function
** call. On return the structure fill be filled with identity information.
**---------------------------------------------------------------------------
*/
INT32 clientGetIdentityInfo(UINT32 iInstance, EtIPIdentityInfo* pInfo)
{
	if ((iInstance > MAX_IDENTITY_INSTANCE) || (iInstance < 1))
		return ERR_IDENTITY_INVALID_INSTANCE;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	memcpy( pInfo, &gIDInfo[iInstance-1], sizeof(EtIPIdentityInfo) );
	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientSetIdentityInfo( )
**
** Updates identity information.
** pInfo is the pointer to the EtIPIdentityInfo structure with the new
** identity information.
** To change only some of the identity parameters the client application
** can call EtIPGetIdentityInfo, modify only appropriate structure members
** and then call EtIPSetIdentityInfo.
**---------------------------------------------------------------------------
*/
INT32 clientSetIdentityInfo(UINT32 iInstance, EtIPIdentityInfo* pInfo)
{
	if ((iInstance > MAX_IDENTITY_INSTANCE) || (iInstance < 1))
		return ERR_IDENTITY_INVALID_INSTANCE;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	memcpy( &gIDInfo[iInstance-1], pInfo, sizeof(EtIPIdentityInfo) );
	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientSetIdentityStatus( )
**
** Explicitly sets the Identity Status so it is not determined by the stack.
** iInstance is the Identity Instance number to set the status
** iStatus is the current status.
** To change only some of the identity parameters the client application
** can call clientGetIdentityInfo, modify only appropriate structure members
** and then call clientSetIdentityInfo.
**---------------------------------------------------------------------------
*/
INT32 clientSetIdentityStatus(UINT32 iInstance, UINT16 iStatus)
{
	if ((iInstance > MAX_IDENTITY_INSTANCE) || (iInstance < 1))
		return ERR_IDENTITY_INVALID_INSTANCE;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gIDInfo[iInstance-1].iStatus = iStatus;
	gIDInfo[iInstance-1].bUseApplicationStatus = TRUE;
	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientClearIdentityStatus( )
**
** Clears application's setting of Identity status so it can be determined 
** by the stack.
** iInstance is the Identity Instance number to clear the status
**---------------------------------------------------------------------------
*/
INT32 clientClearIdentityStatus(UINT32 iInstance)
{
	if ((iInstance > MAX_IDENTITY_INSTANCE) || (iInstance < 1))
		return ERR_IDENTITY_INVALID_INSTANCE;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gIDInfo[iInstance-1].bUseApplicationStatus = FALSE;
	platformReleaseMutex(ghClientMutex);

	return 0;
}

/*---------------------------------------------------------------------------
** clientGetHostIPAddress( )
**
** Returns an array of the host IP addresses as Internet unsigned long numbers
** alHostIPAddresses should point to the preallocated unsigned long array 
** where IP address should be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the obtained IP addresses or iNumHostIPAddressesAllowed
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
INT32 clientGetHostIPAddress( UINT32* alHostIPAddresses, UINT32 iNumHostIPAddressesAllowed)
{
	UINT32 i, Count;
	UINT32 *pInHostIP, *pOutHostIP;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	platformSocketInitTargetData();
	Count = PLATFORM_MIN(gnHostIPAddrCount, iNumHostIPAddressesAllowed);

	for ( i = 0, pInHostIP = galHostIPAddr, pOutHostIP = alHostIPAddresses;
		i < Count;
		i++, pInHostIP++, pOutHostIP++)
	{
		*pOutHostIP = *pInHostIP;
	}

	platformReleaseMutex(ghClientMutex);

	return Count;
}

/*---------------------------------------------------------------------------
** clientClaimHostIPAddress( )
**
** Claim one of the host IP addresses returned by clientGetHostIPAddress().
** It can be called repeatedly for each claimed IP address.  A TCP port can also
** be specified with the IP address.  If 0 is specified, the default TCP port
** 44818 (ENCAP_SERVER_PORT) will be used.  Note that each IP address can only
** use one port.
** Returned TRUE if claimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
INT32 clientClaimHostIPAddress( UINT32 lHostIPAddress, UINT16 iPort )
{
	UINT32 i;
	UINT32* pIP;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	BOOL bRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	/* Check first that it's not already claimed */
	for ( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		i < gnClaimedHostIPAddrCount;
		i++, pTcpipInterface++)
	{
		if (pTcpipInterface->InterfaceConfig.lIpAddr == lHostIPAddress )
			break;
	}

	if ( i < gnClaimedHostIPAddrCount )
	{
		platformReleaseMutex(ghClientMutex);
		return TRUE;
	}

	/* Check that it's in the host address list */
	for ( i = 0, pIP = galHostIPAddr;
		i < gnHostIPAddrCount;
		i++, pIP++ )
	{
		if ((*pIP) == lHostIPAddress )
			break;
	}

	if ( i == gnHostIPAddrCount )
	{
		platformReleaseMutex(ghClientMutex);
		return FALSE;
	}

	bRet = tcpipAddClaimedIPAddress(lHostIPAddress, iPort);
	if (bRet)
		connectionNotifyNetLEDChange();

	platformReleaseMutex(ghClientMutex);
	return bRet;
}

/*---------------------------------------------------------------------------
** clientGetClaimedHostIPAddress( )
**
** Returns an array with the claimed host IP addresses.
** alClaimedHostIPAddresses should point to the preallocated unsigned long array
** where the claimed IP address will be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the claimed IP addresses or iNumHostIPAddressesAllowed 
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
INT32 clientGetClaimedHostIPAddress( UINT32* alClaimedHostIPAddresses, UINT32 iNumHostIPAddressesAllowed)
{
	UINT32 i;
	UINT32* pIP;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	UINT32 lClaimed;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	lClaimed = PLATFORM_MIN(gnClaimedHostIPAddrCount, iNumHostIPAddressesAllowed);

	for ( i = 0, pTcpipInterface = gsTcpIpInstAttr, pIP = alClaimedHostIPAddresses;
		i < lClaimed;
		i++, pTcpipInterface++, pIP++)
	{
		(*pIP) = pTcpipInterface->InterfaceConfig.lIpAddr;
	}

	platformReleaseMutex(ghClientMutex);

	return lClaimed;
}

/*---------------------------------------------------------------------------
** clientClaimAllHostIPAddress( )
**
** Claims the ownership of all available host IP addresses.  All IP addresses
** will use TCP port 44818 (ENCAP_SERVER_PORT)
** Function should be called only once after the EtIPAdapterStart() call.
**---------------------------------------------------------------------------
*/
void clientClaimAllHostIPAddress(void)
{
	UINT32 i;
	UINT32* pIP;

	for ( i = 0, pIP = galHostIPAddr;
		  i < gnHostIPAddrCount;
		  i++, pIP++)
	{
		clientClaimHostIPAddress(*pIP, 0);
	}
}

/*---------------------------------------------------------------------------
** clientUnclaimHostIPAddress( )
**
** Unclaim one of the host IP addresses returned by clientGetHostIPAddress().
** Will be called repeatedly for each claimed IP address.
** Returned TRUE if claimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
INT32 clientUnclaimHostIPAddress( UINT32 lHostIPAddress )
{
	BOOL bRet;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	bRet = tcpipRemoveClaimedIPAddress(lHostIPAddress);
	if (bRet)
		connectionNotifyNetLEDChange();

	platformReleaseMutex(ghClientMutex);
	return bRet;
}

/*---------------------------------------------------------------------------
** clientUnclaimAllHostIPAddress( )
**
** Removes the ownership of all available claimed host IP addresses.
**---------------------------------------------------------------------------
*/
void clientUnclaimAllHostIPAddress(void)
{
	UINT32 iNumClaimedIPAddresses;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	iNumClaimedIPAddresses = gnClaimedHostIPAddrCount;

	if (gnClaimedHostIPAddrCount > 0)
	{
		for (pTcpipInterface = &gsTcpIpInstAttr[gnClaimedHostIPAddrCount-1]; 
			pTcpipInterface >= gsTcpIpInstAttr; 
			pTcpipInterface--)
		{
			if (tcpipRemoveClaimedIPAddress(pTcpipInterface->InterfaceConfig.lIpAddr))
				connectionNotifyNetLEDChange();
		}
	}

	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientModbusGetCOMPorts( )
**
** Returns an array of the COM port numbers
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array 
** where COM ports should be stored.
** Returns the number of the obtained COM ports
**---------------------------------------------------------------------------
*/
INT32 clientModbusGetCOMPorts(UINT32* alCOMPorts)
{
#ifdef ET_IP_MODBUS_SERIAL
	INT32 iNumPorts;
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	iNumPorts = modbusGetCOMPorts(alCOMPorts);
	platformReleaseMutex(ghClientMutex);
	return iNumPorts;
#else
	return 0;
#endif
}

/*---------------------------------------------------------------------------
** clientModbusClaimCOMPort( )
**
** Claim one of the COM ports returned by clientModbusGetCOMPorts().
** Will be called repeatedly for each claimed COM port.
** Only used if ET_IP_MODBUS_SERIAL is defined
** Returned TRUE if claimed successfully, FALSE if the COM port is not in 
** the host list.
**---------------------------------------------------------------------------
*/
INT32 clientModbusClaimCOMPort(UINT32 lCOMPort)
{
#ifdef ET_IP_MODBUS_SERIAL
	INT32 bRet;
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	bRet = modbusClaimCOMPort(lCOMPort);
	platformReleaseMutex(ghClientMutex);
	return bRet;
#else
	return FALSE;
#endif
}

/*---------------------------------------------------------------------------
** clientModbusGetClaimedCOMPorts( )
**
** Returns an array with the claimed COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array where the 
** claimed COM ports will be stored.
** Returns the number of the claimed COM ports
**---------------------------------------------------------------------------
*/
INT32 clientModbusGetClaimedCOMPorts(UINT32* alCOMPorts)
{
#ifdef ET_IP_MODBUS_SERIAL
	INT32 iNumPorts;
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	iNumPorts = modbusGetClaimedCOMPorts(alCOMPorts);
	platformReleaseMutex(ghClientMutex);
	return iNumPorts;
#else
	return 0;
#endif
}

/*---------------------------------------------------------------------------
** clientModbusClaimAllCOMPorts( )
**
** Claims the ownership of all available COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
**---------------------------------------------------------------------------
*/
void clientModbusClaimAllCOMPorts()
{
#ifdef ET_IP_MODBUS_SERIAL
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	modbusClaimAllCOMPorts();
	platformReleaseMutex(ghClientMutex);
#endif
}

/*--------------------------------------------------------------------------------
** clientGetTickCount()
**
** Returns the system tick count. 
**---------------------------------------------------------------------------------
*/
UINT32 clientGetTickCount(void)
{
	return gdwTickCount;
}

/*-------------------------------------------------------------------------------
** clientSetKeepTCPOpenForUDP()
**
** This flag is set to TRUE by default. Use this call to change 
** KeepTCPOpenForActiveCorrespondingUDP flag. 
** When KeepTCPOpenForActiveCorrespondingUDP flag is set to TRUE the TCP 
** socket that was used to establish a I/O connection is kept open
** for the duration of theI/O connection.
** If this flag is set to False - TCP socket that was used to establish a
** Class0/1 connection is closed if there is no other activity on this TCP 
** socket for more than 1 minute after connection is opened. Setting this flag
** to FALSE lets you save some resources in lueu of possible incompatibility 
** with some hardware/software. Incompatible hardware includes Rockwell devices. 
** When communicating with Rockwell hardware this flag should be kept as TRUE.
** Even when this flag is set to TRUE the other side of the I/O connection
** may close the TCP socket. This will be handled and I/O connection will be 
** kept open regardless what the bKeepTCPOpenForActiveCorrespondingUDP flag is.
** nConnectionInstance may be set to a particular connection instance and in this 
** case the flag will only apply to that connection. It may also be set to 
** INVALID_INSTANCE and it will apply to all connections already opened or will 
** be opened in the future that did not receive the same call for their instance 
** number.
**--------------------------------------------------------------------------------
*/
int clientSetKeepTCPOpenForUDP( BOOL bKeepTCPOpenForActiveCorrespondingUDP, INT32 nConnectionInstance )
{
	CONNECTION* pConnection;
	
	if ( ((UINT32)nConnectionInstance) == INVALID_INSTANCE || nConnectionInstance == 0 )
	{
		if ( bKeepTCPOpenForActiveCorrespondingUDP )
			gnKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateTrue;
		else
			gnKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateFalse;
		return 0; 
	}
	
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);   

	if ( (pConnection = connectionGetFromInstance(nConnectionInstance)) == NULL)
	{
		platformReleaseMutex(ghClientMutex);
		return ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED;
	}

	if ( bKeepTCPOpenForActiveCorrespondingUDP )
		pConnection->nKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateTrue;
	else
		pConnection->nKeepTCPOpenForActiveCorrespondingUDP = KeepTCPOpenStateFalse;

	platformReleaseMutex(ghClientMutex);
	return 0;
}

/*---------------------------------------------------------------------------
** clientSetSTRINGI( )
**
** Convenient utility function to set a STRINGI to a data buffer
**---------------------------------------------------------------------------
*/
void clientSetSTRINGI(UINT8* pDataBuffer, STRINGI* pString)
{
	/* there is only one string */
	*pDataBuffer = 1; pDataBuffer++;

	/* Set the language */
	memcpy(pDataBuffer, pString->aLang, 3);
	pDataBuffer += 3;

	/* Set the EPATH */
	*pDataBuffer = pString->bEPATH; pDataBuffer++;

	/* Set the character set */
	UINT16_SET(pDataBuffer, pString->iCharacterSet);
	pDataBuffer += sizeof(UINT16);

	/* Set the length */
	*pDataBuffer = pString->bLength; pDataBuffer++;

	/* Set the string */
	memcpy(pDataBuffer, pString->stringData, pString->bLength);
}

/*---------------------------------------------------------------------------
** clientGetSTRINGI( )
**
** Convenient utility function to get a STRINGI from a data buffer
** Returns 0 for success, ERR_STRINGI_BAD_SIZE if iBufferSize is too small
**---------------------------------------------------------------------------
*/
INT32 clientGetSTRINGI(STRINGI* pString, UINT8* pDataBuffer, UINT32 iBufferSize)
{
	if (*pDataBuffer != 1)
		return ERR_STRINGI_BAD_SIZE;

	if (iBufferSize < 8)
		return ERR_STRINGI_BAD_SIZE;

	pDataBuffer++;

	/* Set the language */
	memcpy(pString->aLang, pDataBuffer, 3);
	pDataBuffer += 3;

	/* Set the EPATH */
	pString->bEPATH = *pDataBuffer++;

	/* Set the character set */
	pString->iCharacterSet = UINT16_GET(pDataBuffer);
	pDataBuffer += sizeof(UINT16);

	/* Set the length */
	pString->bLength = *pDataBuffer++;

	if (iBufferSize < (UINT32)(pString->bLength+8))
		return ERR_STRINGI_BAD_SIZE;

	/* Set the string */
	memcpy(pString->stringData, pDataBuffer, pString->bLength);

	return 0;
}

/*---------------------------------------------------------------------------
** clientLogMessage( )
**
** Logs the provided NULL terminated message in the stack's log file with a TRACE_LEVEL_USER filter bit
**---------------------------------------------------------------------------
*/
void clientLogMessage(char *szMsg)
{
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);   
	DumpStr0(TRACE_LEVEL_USER, TRACE_TYPE_MSG, 0, 0, szMsg);
	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientGetNetworkStatus( )
**
** Returns the state of the network indicator (LED).  If multiple IP addresses
** are used and EIP_NSLED_PER_IP is defined, lHostIP is the "claimed" IP address
** to retrieve the NS LED status.  Otherwise lHostIP is ignored.
**---------------------------------------------------------------------------
*/
EtIPNetworkStatus clientGetNetworkStatus(UINT32 lHostIP)
{
	EtIPNetworkStatus eStatus;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
#ifdef EIP_NSLED_PER_IP
	eStatus = connectionGetCurrentNetLEDState(lHostIP);
#else
	eStatus = connectionGetCurrentNetLEDState(0);
#endif
	platformReleaseMutex(ghClientMutex);

	return eStatus;
}

/*---------------------------------------------------------------------------
** clientSendBroadcastListIdentity( )
**
** Sends out a List Identity broadcast message
** clientGetListIdentityResponse will be used to get the response(s)
** lIPAddress is the host address used to send the message
** iMaxDelay is the maximum delay allowed by targets to respond
** Returns TRUE if messages was successfully sent
**---------------------------------------------------------------------------
*/
BOOL clientSendBroadcastListIdentity(UINT32 lIPAddress, UINT16 iMaxDelay)
{
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig;
	BOOL bRet;
	ENCAPH Encap;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);   

	if ((lIPAddress == INADDR_NONE) || (lIPAddress == INADDR_ANY))
		lIPAddress = gsTcpIpInstAttr[0].InterfaceConfig.lIpAddr;

	pTcpIpConfig = tcpipFindClaimedHostAddress(lIPAddress);
	if (pTcpIpConfig == NULL)
	{
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, lIPAddress, 0,
			"clientSendBroadcastListIdentity Host Address has not been claimed");

		platformReleaseMutex(ghClientMutex);
		return FALSE;
	}

	if (!(IS_TICK_GREATER(gdwTickCount, pTcpIpConfig->lListIdentityTimeout)))
	{
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, lIPAddress, 0,
			"clientSendBroadcastListIdentity still waiting on previous broadcast");

		platformReleaseMutex(ghClientMutex);
		return FALSE;
	}

	/* reset the number of list identities for this host address */
	ucmmClearListIdentities(pTcpIpConfig->InterfaceConfig.lIpAddr);

	/* Set up the encapsulated message */
	memset(&Encap, 0, ENCAPH_SIZE);
	Encap.iCommand = ENCAP_CMD_LISTIDENTITY;
	ENCAP_CVT_PS(Encap.iCommand);
	Encap.lContext1 = iMaxDelay;
	ENCAP_CVT_PL(Encap.lContext1);

	bRet = socketSendBroadcasts(pTcpIpConfig, &Encap);

	/* set the timeout appropriately */
	pTcpIpConfig->lListIdentityTimeout = bRet ?  gdwTickCount+iMaxDelay+LIST_IDENTITY_TIMEOUT : gdwTickCount-1;

	platformReleaseMutex(ghClientMutex);

	return bRet;
}

/*---------------------------------------------------------------------------
** clientGetListIdentityResponse( )
**
** Get a response for the previously sent List Identity broadcast.
** nId is a Id returned from the NM_NEW_LIST_IDENTITY_RESPONSE notification 
** Returns 0 in case of success or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 clientGetListIdentityResponse(INT32 nId, EtIPListIdentityInfo* pInfo)
{
	EtIPListIdentityInfo* pListIdInfo;

	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	pListIdInfo = ucmmGetByListIdentityId( nId );

	if ( pListIdInfo == NULL )
	{
		/* Id was not found */
		platformReleaseMutex(ghClientMutex);
		return ERR_OBJECT_REQUEST_UNKNOWN_INDEX;
	}

	memcpy(pInfo, pListIdInfo, sizeof(EtIPListIdentityInfo));

	ucmmRemoveListIdentity( pListIdInfo );
	platformReleaseMutex(ghClientMutex);
	return 0;
}

/*---------------------------------------------------------------------------
** clientSetAppVerifyConnection( )
**
** Determines whether the application will verify connections before they
** are accepted.  If the application verifies connections it will recieve a
** NM_CONNECTION_VERIFICATION notification
**---------------------------------------------------------------------------
*/
void clientSetAppVerifyConnection(BOOL bVerify)
{
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbAppVerifyConnection = bVerify;
	platformReleaseMutex(ghClientMutex);
}

/*---------------------------------------------------------------------------
** clientSetBackplaneSupport( )
**
** Determines whether the application support requests to the "backplane"
** port.  Requests targeted at the "device" slot will be handled by the 
** EtherNet/IP stack.  Requests for slots outside of the "device" will 
** receive a NM_BACKPLANE_REQUEST_RECEIVED notification.  The client 
** application can use clientGetClientRequest() and clientSendClientResponse() 
** after that to get the request and send the response.
** bEnable enables/disables support for responding to backplane requests
** iSlot is the slot for the EtherNet/IP stack "device"
**---------------------------------------------------------------------------
*/
void clientSetBackplaneSupport(BOOL bEnable, UINT16 iSlot)
{
#ifndef EIP_NO_CIP_ROUTING
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbBackplaneSupport = bEnable;
	giBackplaneSlot = iSlot;
	platformReleaseMutex(ghClientMutex);
#endif
}

/*---------------------------------------------------------------------------
** clientGetSystemStats( )
**
** Return system connection stats.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetSystemStats(EtIPSystemStats* pStats, BOOL bClear)
{
#ifdef CONNECTION_STATS
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);

	memcpy(pStats, &gSystemStats.systemStats, sizeof(gSystemStats.systemStats));

	/* Fill in the gaps of "statistics" already being collected by other means */
	pStats->iNumCurrentTCPConnections = (UINT16)(gpnSessions-gSessions);

	if (bClear)
		memset(&gSystemStats.systemStats, 0, sizeof(gSystemStats.systemStats));

	platformReleaseMutex(ghClientMutex);
	return 0;
#else
	return -1;
#endif
}

/*---------------------------------------------------------------------------
** clientSetSystemStats( )
**
** Set system connection stats.  The intent is to reset specific members
** of the EtIPSystemStats structure to 0.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. 
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
INT32 clientSetSystemStats(EtIPSystemStats* pStats)
{
#ifdef CONNECTION_STATS
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	memcpy(&gSystemStats.systemStats, pStats, sizeof(EtIPSystemStats));
	platformReleaseMutex(ghClientMutex);
	return 0;
#else
	return -1;
#endif
}

/*---------------------------------------------------------------------------
** clientFileObjectAddInstance( )
**
** Add an instance to the File Object
**---------------------------------------------------------------------------
*/
INT32 clientFileObjectAddInstance(INT32 iInstance, EtIPFileInstance* pInstance)
{
	BOOL ret = FALSE;
#if defined EIP_FILE_OBJECT
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbFileObjectEnabled = TRUE;
	ret = eipFileAddInstance(iInstance, pInstance);
	platformReleaseMutex(ghClientMutex);
#endif
	return ret;
}

/*---------------------------------------------------------------------------
** clientFileObjectRemoveInstance( )
**
** Remove an instance to the File Object
**---------------------------------------------------------------------------
*/
BOOL clientFileObjectRemoveInstance(INT32 iInstance)
{
	BOOL ret = FALSE;
#if defined EIP_FILE_OBJECT
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbFileObjectEnabled = TRUE;
	ret = eipFileRemoveInstance(iInstance);
	platformReleaseMutex(ghClientMutex);
#endif
	return ret;
}

/*---------------------------------------------------------------------------
** clientFileObjectGetInstanceProperties( )
**
** Get information about a File object instance
**---------------------------------------------------------------------------
*/
BOOL clientFileObjectGetInstanceProperties(INT32 iInstance, EtIPFileInstance* pInstance)
{
	BOOL ret = FALSE;
#if defined EIP_FILE_OBJECT
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbFileObjectEnabled = TRUE;
	ret = eipFileGetInstanceProperties(iInstance, pInstance);
	platformReleaseMutex(ghClientMutex);
#endif
	return ret;
}

/*---------------------------------------------------------------------------
** clientFileObjectSetBaseDirectory( )
**
** Set the directory used by the File object for its files
**---------------------------------------------------------------------------
*/
BOOL clientFileObjectSetBaseDirectory(char* strDirectory)
{
	BOOL ret = FALSE;
#if defined EIP_FILE_OBJECT
	platformWaitMutex(ghClientMutex, MUTEX_TIMEOUT);
	gbFileObjectEnabled = TRUE;
	ret = eipFileSetBaseDirectory(strDirectory);
	platformReleaseMutex(ghClientMutex);
#endif
	return ret;
}

/*---------------------------------------------------------------------------
** clientComputeFileSizeAndChecksum( )
**
** Compute the file size and checksum for a File Object file
**---------------------------------------------------------------------------
*/
BOOL clientComputeFileSizeAndChecksum(char* pstrFilename, UINT32* pFileSize, INT16* piChecksum)
{
	BOOL ret = FALSE;
#if defined EIP_FILE_OBJECT
	ret = utilComputeFileSizeAndChecksum(pstrFilename, pFileSize, piChecksum);
#endif
	return ret;
}
