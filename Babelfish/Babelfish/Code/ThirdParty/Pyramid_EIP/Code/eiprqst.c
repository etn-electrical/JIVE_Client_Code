/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPRQST.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Explicit message request handling
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef EIP_LARGE_BUF_ON_HEAP
REQUEST* gRequests;					/* Request array */
#else
REQUEST  gRequests[MAX_REQUESTS];	/* Request array */
#endif

REQUEST* gpnRequests;

INT32 gnIndex;						/* Index to be assigned to the next opened request */

/*---------------------------------------------------------------------------
** requestInit( )
**
** Initialize request array
**---------------------------------------------------------------------------
*/
void requestInit(void)
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	gRequests = (REQUEST *)EtIP_mallocHeap(MAX_REQUESTS * sizeof(REQUEST ));
	if (gRequests == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gRequests to %d entries\n", MAX_REQUESTS);
		return;
	}
#endif

	gpnRequests = gRequests;
	gnIndex = REQUEST_INDEX_BASE;
}


/*---------------------------------------------------------------------------
** requestInitialize( )
**
** Initialize a request
**---------------------------------------------------------------------------
*/
static void requestInitialize( REQUEST* pRequest )
{
	memset( pRequest, 0, sizeof(REQUEST) );
	pduInit(&pRequest->cfg.objectCfg.PDU, TRUE);

	pRequest->nIndex = INVALID_REQUEST_INDEX;
	pRequest->nRequestGroupIndex = INVALID_REQUEST_INDEX;
	pRequest->nSessionId = INVALID_SESSION_INDEX;
	pRequest->lIPAddress = INADDR_NONE;
	pRequest->lFromIPAddress = INADDR_NONE;
	pRequest->lHostIPAddr = INADDR_NONE;
	pRequest->bOpenPriorityTickTime = PRIORITY_TICK_TIME;
	pRequest->bOpenTimeoutTicks = TIMEOUT_TICKS;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** requestNewOutgoing( )
**
** Allocate and initialize new request in the array for outgoing requests
**---------------------------------------------------------------------------
*/
REQUEST* requestNewOutgoing( INT32 nType, BOOL bDeleteOnSend, UINT32 lHostIPAddr )
{
	REQUEST* pRequest;

	if ( gpnRequests >= &gRequests[MAX_REQUESTS] )
		return NULL;						/* Request limit has been reached */

	pRequest = gpnRequests;
	gpnRequests++;

	requestInitialize( pRequest );

	pRequest->nIndex = gnIndex++;
	pRequest->lContext1 = lHostIPAddr;
	pRequest->lContext2 = (UINT32)pRequest->nIndex;
	pRequest->nType = nType;
	pRequest->bIncoming = FALSE;
	pRequest->bDeleteOnSend = bDeleteOnSend;
	pRequest->lHostIPAddr = lHostIPAddr;

	if ( gnIndex == INDEX_END )				/* Wrap the unique request identifier if need to */
		gnIndex = REQUEST_INDEX_BASE;

	return pRequest;
}
#endif

/*---------------------------------------------------------------------------
** requestNewIncoming( )
**
** Allocate and initialize new request in the array for incoming requests
**---------------------------------------------------------------------------
*/
REQUEST* requestNewIncoming( INT32 nType, BOOL bDeleteOnSend, INT32 nSessionId )
{
	REQUEST* pRequest;
	SESSION* pSession = NULL;
	
	if (nType != LIST_IDENTITY_REQUEST)
	{
		pSession = sessionGetBySessionId( nSessionId );
		if (pSession == NULL)
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0,
				"requestNewIncoming INTERNAL ERROR: session with Id %d pulled from under us", nSessionId);
			return NULL;
		}
	}

	if ( gpnRequests >= &gRequests[MAX_REQUESTS] )
		return NULL;				/* Request limit has been reached */

	pRequest = gpnRequests;
	gpnRequests++;

	requestInitialize( pRequest );

	pRequest->nIndex = gnIndex++;
	switch(nType)
	{
	case LIST_IDENTITY_REQUEST:
		pRequest->lContext1 = 0;
		pRequest->lHostIPAddr = 0;
		break;
	default:
		pRequest->lContext1 = pSession->lHostIPAddr;
		pRequest->lHostIPAddr = pSession->lHostIPAddr;
		break;
	}
	pRequest->lContext2 = (UINT32)pRequest->nIndex;
	pRequest->nType = nType;
#ifndef EIP_NO_ORIGINATE_UCMM
	pRequest->bIncoming = TRUE;
#endif
	pRequest->bDeleteOnSend = bDeleteOnSend;
	pRequest->nSessionId = nSessionId;

	if ( gnIndex == INDEX_END )				/* Wrap the unique request identifier if need to */
		gnIndex = REQUEST_INDEX_BASE;

	return pRequest;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** requestInitializeFromObjectRqst( )
**
** Initialize parameters from EtIPObjectRequest structure.
** Return 0 in case of success, or one of the error codes.
**---------------------------------------------------------------------------
*/
static INT32 requestInitializeFromObjectRqst( REQUEST* pRequest, EtIPObjectRequest* pObjectRequest )
{
	if ( pObjectRequest->lExplicitMessageTimeout == 0 )
		pObjectRequest->lExplicitMessageTimeout = DEFAULT_EXPLICIT_REQUEST_TIMEOUT;
	pRequest->lRequestTimeoutTick = gdwTickCount + pObjectRequest->lExplicitMessageTimeout;

	pRequest->cfg.objectCfg.bService = pObjectRequest->bService;

	pduRelease( &pRequest->cfg.objectCfg.PDU );

	if ( pObjectRequest->iClass != INVALID_CLASS )
		pduAddTag(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_CLASS, (UINT16)(IS_WORD(pObjectRequest->iClass) ? (IS_BYTE(pObjectRequest->iClass) ? 1 : 2) : 4),
					pObjectRequest->iClass);

	if ( pObjectRequest->iInstance != INVALID_INSTANCE )
		pduAddTag(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, (UINT16)(IS_WORD(pObjectRequest->iInstance) ? (IS_BYTE(pObjectRequest->iInstance) ? 1 : 2) : 4),
					pObjectRequest->iInstance);

	if ( pObjectRequest->iAttribute != INVALID_ATTRIBUTE )
		pduAddTag(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, (UINT16)(IS_WORD(pObjectRequest->iAttribute) ? (IS_BYTE(pObjectRequest->iAttribute) ? 1 : 2) : 4),
					pObjectRequest->iAttribute);

	if ( pObjectRequest->iMember != INVALID_MEMBER )
		pduAddTag(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, (UINT16)(IS_WORD(pObjectRequest->iMember) ? (IS_BYTE(pObjectRequest->iMember) ? 1 : 2) : 4),
					pObjectRequest->iMember);

	if ( pObjectRequest->iTagSize != 0 )
		pduAddTag(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_EXT_SYMBOL, pObjectRequest->iTagSize,
					(ETAG_DATATYPE)(EtIP_malloc((UINT8*)pObjectRequest->requestTag, pObjectRequest->iTagSize)));

	pRequest->cfg.objectCfg.pData = EtIP_malloc( pObjectRequest->requestData, pObjectRequest->iDataSize );
	pRequest->cfg.objectCfg.iDataSize = pObjectRequest->iDataSize;

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		requestRemove( pRequest );
		return NM_OUT_OF_MEMORY;
	}

	return 0;
}

/*---------------------------------------------------------------------------
** requestInitFromEPATHObjectRqst( )
**
** Initialize parameters from EtIPEPATHObjectRequest structure. 
** Return 0 in case of success, or one of the error codes.
**---------------------------------------------------------------------------
*/
static INT32 requestInitFromEPATHObjectRqst( REQUEST* pRequest, EtIPEPATHObjectRequest* pEPATHObjectRequest )
{
	if ( pEPATHObjectRequest->lExplicitMessageTimeout == 0 )
		pEPATHObjectRequest->lExplicitMessageTimeout = DEFAULT_EXPLICIT_REQUEST_TIMEOUT;

	pRequest->lRequestTimeoutTick = gdwTickCount + pEPATHObjectRequest->lExplicitMessageTimeout;

	pRequest->cfg.objectCfg.bService = pEPATHObjectRequest->bService;

	pduConvertToInternalStorage( &pEPATHObjectRequest->ePATH, &pRequest->cfg.objectCfg.PDU );

	pRequest->cfg.objectCfg.pData = EtIP_malloc( pEPATHObjectRequest->requestData, pEPATHObjectRequest->iDataSize );
	pRequest->cfg.objectCfg.iDataSize = pEPATHObjectRequest->iDataSize;

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		requestRemove( pRequest );
		return NM_OUT_OF_MEMORY;
	}

	return 0;
}

/*---------------------------------------------------------------------------
** requestNewUnconnectedSend( )
**
** Queue new Unconnected Send request message. 
** Return the request index or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 requestNewUnconnectedSend( UINT8* szIPAddr, EtIPObjectRequest* pObjectRequest,
									char* extendedPath, UINT16 iExtendedPathLen)
{
	REQUEST* pRequest;
	INT32 ret;

	/* Schedule new Unconnected Send request */
	pRequest = requestNewOutgoing(UNCONNECTED_SEND_REQUEST, FALSE, pObjectRequest->lHostIPAddr);

	if ( pRequest == NULL )
	{
		/* No more space for requests */
		notifyEvent( NM_PENDING_REQUESTS_LIMIT_REACHED, 0, 0 );
		return NM_PENDING_REQUESTS_LIMIT_REACHED;
	}

	ret = requestInitializeFromObjectRqst( pRequest, pObjectRequest );
	if (ret != 0 )
	{
		requestRemove( pRequest );
		return NM_OUT_OF_MEMORY;
	}

	if ( iExtendedPathLen != 0 )
	{
		pRequest->iExtendedPathSize = iExtendedPathLen;
		pRequest->pExtendedPath = EtIP_malloc( (unsigned char*)extendedPath, iExtendedPathLen );
		if ( pRequest->iExtendedPathSize && pRequest->pExtendedPath == NULL )
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			return NM_OUT_OF_MEMORY;
		}

		/* For UnconnectedSends, compute the time tick based on the EtIPObjectRequest timeout */
		pRequest->bOpenTimeoutTicks = (UINT8)((pObjectRequest->lExplicitMessageTimeout+((1 << (PRIORITY_TICK_TIME & PRIORITY_TICK_TIME_MASK))-1)) >> (PRIORITY_TICK_TIME & PRIORITY_TICK_TIME_MASK));
	}

	pRequest->lIPAddress = platformAddrFromPath( (const char*)szIPAddr, (INT32)strlen((const char*)szIPAddr) );
	if ( pRequest->lIPAddress == (UINT32)ERROR_STATUS )
	{
		requestRemove( pRequest );
		return NM_REQUEST_FAILED_INVALID_NETWORK_PATH;
	}

	pRequest->nState = REQUEST_LOGGED;
	return pRequest->nIndex;
}

/*---------------------------------------------------------------------------
** requestNewEPATHUnconnectedSend( )
**
** Queue new Unconnected Send request message based on the already filled
** EPATH. 
** Return the request index or one of the error codes.
**---------------------------------------------------------------------------
*/
INT32 requestNewEPATHUnconnectedSend( UINT8* szIPAddr, EtIPEPATHObjectRequest* pEPATHObjectRequest, 
										char* extendedPath, UINT16 iExtendedPathLen)
{
	REQUEST* pRequest;
	INT32 ret;

	/* Schedule new Unconnected Send request */
	pRequest = requestNewOutgoing( UNCONNECTED_SEND_REQUEST, FALSE, pEPATHObjectRequest->lHostIPAddr);

	if ( pRequest == NULL )
	{
		/* No more space for requests */
		notifyEvent( NM_PENDING_REQUESTS_LIMIT_REACHED, 0, 0 );
		return NM_PENDING_REQUESTS_LIMIT_REACHED;
	}

	ret = requestInitFromEPATHObjectRqst( pRequest, pEPATHObjectRequest );
	if (ret != 0 )
		return ret;

	if ( iExtendedPathLen != 0 )
	{
		pRequest->iExtendedPathSize = iExtendedPathLen;
		pRequest->pExtendedPath = EtIP_malloc( (unsigned char*)extendedPath, iExtendedPathLen );
		if ( pRequest->iExtendedPathSize && pRequest->pExtendedPath == NULL )
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			return NM_OUT_OF_MEMORY;
		}

		/* For UnconnectedSends, compute the time tick based on the EtIPObjectRequest timeout */
		pRequest->bOpenTimeoutTicks = (UINT8)((pEPATHObjectRequest->lExplicitMessageTimeout+((1 << (PRIORITY_TICK_TIME & PRIORITY_TICK_TIME_MASK))-1)) >> (PRIORITY_TICK_TIME & PRIORITY_TICK_TIME_MASK));
	}

	pRequest->lIPAddress = platformAddrFromPath( (const char*)szIPAddr, (INT32)strlen((const char*)szIPAddr) );
	if ( pRequest->lIPAddress == (UINT32)ERROR_STATUS )
	{
		requestRemove( pRequest );
		return NM_REQUEST_FAILED_INVALID_NETWORK_PATH;
	}

	pRequest->nState = REQUEST_LOGGED;
	return pRequest->nIndex;
}
#endif

/*---------------------------------------------------------------------------
** requestRemove( )
**
** Remove the request from the array
**---------------------------------------------------------------------------
*/
void requestRemove( REQUEST* pRequest )
{
	REQUEST* pLoopRequest;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
		"requestRemove %d", pRequest->nIndex);

	switch( pRequest->nType )
	{
	case OBJECT_REQUEST:
	case UNCONNECTED_SEND_REQUEST:
		pduRelease( &pRequest->cfg.objectCfg.PDU );
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;

	case CLASS3_REQUEST:
		pduRelease( &pRequest->cfg.class3Cfg.cfg.PDU );
		EtIP_free( &pRequest->cfg.class3Cfg.cfg.pData, &pRequest->cfg.class3Cfg.cfg.iDataSize );
		break;
#ifdef SUPPORT_CSP
	case PCCC_REQUEST:
		cspRemoveRequest(pRequest);
		break;
#endif
	}

	EtIP_free( &pRequest->pExtendedPath, &pRequest->iExtendedPathSize );
	EtIP_free( &pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize);

	gpnRequests--;

	/* Shift the requests with the bigger index to fill in the void */
	for( pLoopRequest = pRequest; pLoopRequest < gpnRequests; pLoopRequest++ )
		memcpy( pLoopRequest, pLoopRequest+1, sizeof(REQUEST) );
}

/*---------------------------------------------------------------------------
** requestRemoveAll( )
**
** Remove all requests from the array
**---------------------------------------------------------------------------
*/
void requestRemoveAll(void)
{
	REQUEST* pRequest;

	for( pRequest = gpnRequests-1; pRequest >= gRequests; pRequest-- )
	{
		switch( pRequest->nType )
		{
		case OBJECT_REQUEST:
		case UNCONNECTED_SEND_REQUEST:
			pduRelease( &pRequest->cfg.objectCfg.PDU );
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			break;

		case CLASS3_REQUEST:
			pduRelease( &pRequest->cfg.class3Cfg.cfg.PDU );
			EtIP_free( &pRequest->cfg.class3Cfg.cfg.pData, &pRequest->cfg.class3Cfg.cfg.iDataSize );
			break;
#ifdef SUPPORT_CSP
		case PCCC_REQUEST:
			cspRemoveRequest(pRequest);
			break;
#endif
		}
		EtIP_free( &pRequest->pExtendedPath, &pRequest->iExtendedPathSize );
		EtIP_free( &pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize);
	}

	gpnRequests = gRequests;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** requestSend( )
**
** Send the request.  Return FALSE if the request has been removed
**---------------------------------------------------------------------------
*/
static BOOL requestSend( REQUEST* pRequest )
{
	switch(pRequest->nType)
	{
	case OBJECT_REQUEST:
		if ( !ucmmSendUnconnectedRequest( pRequest ) )		/* there was an error and the request was deleted */
			return FALSE;
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		break;
	case UNCONNECTED_SEND_REQUEST:
		if ( pRequest->iExtendedPathSize )
		{
			if ( !ucmmPassUnconnectedSendRequest( pRequest ) )
				return FALSE;
		}
		else
		{
			if ( !ucmmPassUncnctSendRqstFinalLeg( pRequest ) )
				return FALSE;
		}
		pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
		break;
	case CLASS3_REQUEST:
		break;
	}

	if ( pRequest->bDeleteOnSend )
	{
		requestRemove(pRequest);
		return FALSE;
	}

	return TRUE;
}
#endif

/*---------------------------------------------------------------------------
** requestService( )
**
** Service a particular request.  Return FALSE if the request is removed
**---------------------------------------------------------------------------
*/
BOOL requestService( REQUEST* pRequest, BOOL* pDataSent )
{
#ifndef EIP_NO_ORIGINATE_UCMM
	SESSION* pSession;
	BOOL bHandleIncomingRequest = TRUE;
#endif
	BOOL bSendData = *pDataSent;
	
	/* No data sent yet */
	*pDataSent = FALSE;

	switch( pRequest->nState )
	{
	case REQUEST_LOGGED:
#ifndef EIP_NO_ORIGINATE_UCMM
		if (pRequest->bIncoming)
		{
#endif
			switch(pRequest->nType)
			{
			case OBJECT_REQUEST:
			case CLASS3_REQUEST:
				routerProcessObjectRequest( pRequest );
				break;
			case LIST_IDENTITY_REQUEST:
				if (IS_TICK_GREATER_OR_EQUAL(gdwTickCount, pRequest->lRequestTimeoutTick))
				{
					ucmmRespondListIdentity( NULL, pRequest->lHostIPAddr, &pRequest->sTOTagAddr, (UINT16)pRequest->nSessionId);
					requestRemove(pRequest);
					*pDataSent = TRUE;
				}
				break;
#ifndef EIP_NO_ORIGINATE_UCMM
			default:
				bHandleIncomingRequest = FALSE;
				break;
#endif
			}
#ifndef EIP_NO_ORIGINATE_UCMM
			if (bHandleIncomingRequest)
				break;
		}

		/* Check if the session with this server has already been established */
		pSession = sessionFindOutgoing( pRequest->lIPAddress, pRequest->lHostIPAddr);

		if ( pSession == NULL )
		{
			pSession = sessionNew(pRequest->lIPAddress, pRequest->lHostIPAddr, FALSE);

			if ( pSession == NULL )    /* Maximum number of open sessions has been reached */
			{
				requestRemove(pRequest);
				notifyEvent(NM_SESSION_COUNT_LIMIT_REACHED, 0, 0);
				return FALSE;
			}

			/* Session will timeout when request times out */
			pSession->lWaitingForOpenSessionTimeout = pRequest->lRequestTimeoutTick;

			pRequest->nSessionId = pSession->nSessionId;
		}
		else
		{
			pRequest->nSessionId = pSession->nSessionId;

			if ( pSession->lState == OpenSessionEstablished )
			{
				if ( pRequest->nRequestGroupIndex == INVALID_REQUEST_INDEX )
				{
					if (bSendData)
					{
						*pDataSent = TRUE;
						return requestSend( pRequest );
					}

					return TRUE;
				}

				pRequest->nState = REQUEST_READY_FOR_GROUP_SEND;
				return TRUE;
			}
		}

		/* Session has not been successfully established yet */
		pRequest->nState = REQUEST_WAITING_FOR_SESSION;
		break;

	case REQUEST_WAITING_FOR_SESSION:
		if ( !pRequest->bIncoming || pRequest->nType != UNCONNECTED_SEND_REQUEST )
			pSession = sessionGetBySessionId( pRequest->nSessionId );
		else
		{
			pSession = sessionFindOutgoing(pRequest->lIPAddress, pRequest->lHostIPAddr);
		}
		if (pSession == NULL)
		{
			notifyEvent( NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0 );
			requestRemove( pRequest );
			return FALSE;
		}
		if (IS_TICK_GREATER(gdwTickCount, pRequest->lRequestTimeoutTick ) )
		{
			notifyEvent(  NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0 );
			requestRemove( pRequest );
			return FALSE;
		}

		if ( pSession->lState == OpenSessionEstablished )
		{
			if ( pRequest->nRequestGroupIndex == INVALID_REQUEST_INDEX )
			{
				if (bSendData)
				{
					*pDataSent = TRUE;
					return requestSend( pRequest );
				}

				return TRUE;
			}

			pRequest->nState = REQUEST_READY_FOR_GROUP_SEND;
		}
#endif /* EIP_NO_ORIGINATE_UCMM */
		break;

	case REQUEST_WAITING_FOR_RESPONSE:
		if ( IS_TICK_GREATER(gdwTickCount, pRequest->lRequestTimeoutTick ) )
		{
#ifndef EIP_NO_ORIGINATE_UCMM
			if ( !pRequest->bIncoming )
			{
				notifyEvent(NM_REQUEST_TIMED_OUT, pRequest->nIndex, 0);

				pSession = sessionGetBySessionId( pRequest->nSessionId );
				if (pSession)
				{
					/* Check if need to drop the session that went offline */
					if ( sessionIsIdle(pSession, INVALID_CONN_INSTANCE, pRequest->nIndex ) )
					{
						sessionRemove( pSession, TRUE );
						return FALSE;
					}
				}
			}
#endif
			requestRemove(pRequest);
			return FALSE;
		}
		break;

	case REQUEST_RESPONSE_RECEIVED:
#ifndef EIP_NO_ORIGINATE_UCMM
		/* If request is an incoming one send the response, otherwise check for the client read timeout */
		if ( pRequest->bIncoming )
		{
#endif
			/* If part of the multiple service request - do not send response now - wait until all responses are ready */  
			if ( pRequest->nRequestGroupIndex == INVALID_REQUEST_INDEX )
			{
				if (bSendData)
				{
					switch(pRequest->nType)
					{
					case OBJECT_REQUEST:
					case CLASS3_REQUEST:
					case UNCONNECTED_SEND_REQUEST:
						if (!ucmmSendObjectResponse( pRequest ))
							return FALSE;
						*pDataSent = TRUE;
						break;
#ifdef SUPPORT_CSP
					case PCCC_REQUEST:
						cspSendResponse( pRequest );
						*pDataSent = TRUE;
						break;
#endif
					}
					requestRemove( pRequest );
					return FALSE;
				}
			}
#ifndef EIP_NO_ORIGINATE_UCMM
		}
		else if ( IS_TICK_GREATER(gdwTickCount, pRequest->lRequestTimeoutTick ) )
		{
			requestRemove(pRequest);
			return FALSE;
		}
#endif
		break;
	}

	return TRUE;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** requestResponseReceived( )
**
** Received the response
**---------------------------------------------------------------------------
*/
void requestResponseReceived( REQUEST* pRequest, UINT32 nResponseEvent)
{
	/* Set the timeout (10 seconds) for the client application to get the results */
	pRequest->lRequestTimeoutTick = gdwTickCount + DEFAULT_EXPLICIT_REQUEST_TIMEOUT;
	pRequest->nState = REQUEST_RESPONSE_RECEIVED;

	notifyEvent(nResponseEvent, pRequest->nIndex, 0);
}
#endif

/*---------------------------------------------------------------------------
** requestGetByRequestId( )
**
** Find a request based on the request Id
**---------------------------------------------------------------------------
*/

REQUEST* requestGetByRequestId( INT32 nRequestId )
{
	REQUEST* pRequest;

	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )		/* Find our request */
	{
		if ( pRequest->nIndex == nRequestId )
			return pRequest;
	}

	return NULL;
}

