/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPRQGRP.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Request Group array used for servicing Multiple Service requests
**
*****************************************************************************
*/

#include "eipinc.h"

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT

#ifdef EIP_LARGE_BUF_ON_HEAP
REQUEST_GROUP* gRequestGroups;							/* Request Group array */
#else
REQUEST_GROUP  gRequestGroups[MAX_REQUEST_GROUPS];		/* Request Group array */
#endif

REQUEST_GROUP* gpnRequestGroups;

static INT32 gnRequestGroupIndex;						/* Index to be assigned to the next opened request group */

/*---------------------------------------------------------------------------
** requestGroupInit( )
**
** Initialize request group array
**---------------------------------------------------------------------------
*/

void requestGroupInit(void)
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	gRequestGroups = (REQUEST_GROUP *)EtIP_mallocHeap(MAX_REQUEST_GROUPS * sizeof(REQUEST_GROUP ));
	if (gRequestGroups == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gRequestGroups to %d entries\n", MAX_REQUEST_GROUPS);
		return;
	}
#endif

	gpnRequestGroups = gRequestGroups;
	gnRequestGroupIndex = REQUEST_GROUP_INDEX_BASE;
}


/*---------------------------------------------------------------------------
** requestGroupInitialize( )
**
** Initialize a request group
**---------------------------------------------------------------------------
*/

void requestGroupInitialize( REQUEST_GROUP* pRequestGroup )
{
	memset( pRequestGroup, 0, sizeof(REQUEST_GROUP) );

	pRequestGroup->nIndex = INVALID_REQUEST_INDEX;
	pRequestGroup->nSessionId = INVALID_SESSION_INDEX;
	pRequestGroup->bOpenPriorityTickTime = PRIORITY_TICK_TIME;
	pRequestGroup->bOpenTimeoutTicks = TIMEOUT_TICKS;
}

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** requestGroupNewOutgoing( )
**
** Allocate and initialize new request group for outgoing requests
**---------------------------------------------------------------------------
*/
REQUEST_GROUP* requestGroupNewOutgoing( INT32 nType )
{
	REQUEST_GROUP* pRequestGroup;

	if ( gpnRequestGroups >= &gRequestGroups[MAX_REQUEST_GROUPS] )
		return NULL;					/* Request group limit has been reached */

	pRequestGroup = gpnRequestGroups;
	gpnRequestGroups++;

	requestGroupInitialize( pRequestGroup );

	pRequestGroup->nIndex = gnRequestGroupIndex++;
	pRequestGroup->lContext1 = galHostIPAddr[0];
	pRequestGroup->lContext2 = (UINT32)pRequestGroup->nIndex;
	pRequestGroup->nType = nType;
	pRequestGroup->bIncoming = FALSE;

	pRequestGroup->lRequestTimeoutTick = gdwTickCount + DEFAULT_EXPLICIT_REQUEST_TIMEOUT;

	pRequestGroup->cfg.objectCfg.bService = SVC_MULTIPLE_SVC_REQUEST;
	pRequestGroup->cfg.objectCfg.PDU.bNumTags = 2;

	pRequestGroup->cfg.objectCfg.PDU.Tags[0].iTagDataSize = 1;
	pRequestGroup->cfg.objectCfg.PDU.Tags[0].iTagType = TAG_TYPE_CLASS;
	pRequestGroup->cfg.objectCfg.PDU.Tags[0].TagData = ROUTER_CLASS;

	pRequestGroup->cfg.objectCfg.PDU.Tags[1].iTagDataSize = 1;
	pRequestGroup->cfg.objectCfg.PDU.Tags[1].iTagType = TAG_TYPE_INSTANCE;
	pRequestGroup->cfg.objectCfg.PDU.Tags[1].TagData = 1;

	if ( gnRequestGroupIndex == INDEX_END )					/* Wrap the unique request identifier if need to */
		gnRequestGroupIndex = REQUEST_GROUP_INDEX_BASE;

	return pRequestGroup;
}
#endif

/*---------------------------------------------------------------------------
** requestGroupNewIncoming( )
**
** Allocate and initialize new request group for incoming requests
**---------------------------------------------------------------------------
*/
REQUEST_GROUP* requestGroupNewIncoming( INT32 nType, INT32 nSessionId )
{
	REQUEST_GROUP* pRequestGroup;
	SESSION* pSession = sessionGetBySessionId( nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0,
			"requestGroupNewIncoming INTERNAL ERROR: session with Id %d pulled from under us", nSessionId);
		return NULL; 
	}

	if ( gpnRequestGroups >= &gRequestGroups[MAX_REQUEST_GROUPS] )
		return NULL;				/* Request group limit has been reached */

	pRequestGroup = gpnRequestGroups;
	gpnRequestGroups++;

	requestGroupInitialize( pRequestGroup );

	pRequestGroup->nIndex = gnRequestGroupIndex++;
	pRequestGroup->nType = nType;
	#ifndef EIP_NO_ORIGINATE_UCMM
	pRequestGroup->bIncoming = TRUE;
	#endif

	pRequestGroup->lIPAddress = pSession->lClientIPAddr;
	pRequestGroup->lHostIPAddr = pSession->lHostIPAddr;
	pRequestGroup->nSessionId = nSessionId;

	pRequestGroup->lRequestTimeoutTick = gdwTickCount + DEFAULT_EXPLICIT_REQUEST_TIMEOUT;

	if ( gnRequestGroupIndex == INDEX_END )					/* Wrap the unique request identifier if need to */
		gnRequestGroupIndex = REQUEST_GROUP_INDEX_BASE;

	return pRequestGroup;
}

/*---------------------------------------------------------------------------
** requestGroupRemove( )
**
** Remove the request group from the array
**---------------------------------------------------------------------------
*/

void requestGroupRemove( REQUEST_GROUP* pRequestGroup )
{
	REQUEST_GROUP* pLoopRequestGroup;

	switch( pRequestGroup->nType )
	{
	case OBJECT_REQUEST:
	case UNCONNECTED_SEND_REQUEST:
		pduRelease( &pRequestGroup->cfg.objectCfg.PDU );
		EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize );
		break;

	case CLASS3_REQUEST:
		pduRelease( &pRequestGroup->cfg.class3Cfg.cfg.PDU );
		EtIP_free( &pRequestGroup->cfg.class3Cfg.cfg.pData, &pRequestGroup->cfg.class3Cfg.cfg.iDataSize );
		break;
	}

	gpnRequestGroups--;

	/* Shift the requests with the bigger index to fill in the void */
	for( pLoopRequestGroup = pRequestGroup; pLoopRequestGroup < gpnRequestGroups; pLoopRequestGroup++ )
		memcpy( pLoopRequestGroup, pLoopRequestGroup+1, sizeof(REQUEST_GROUP) );
}

/*---------------------------------------------------------------------------
** requestGroupRemoveAll( )
**
** Remove all request groups from the array
**---------------------------------------------------------------------------
*/

void requestGroupRemoveAll(void)
{
	REQUEST_GROUP* pRequestGroup;

	for( pRequestGroup = gRequestGroups; pRequestGroup < gpnRequestGroups; pRequestGroup++ )		/* Find our request group */
	{
		switch( pRequestGroup->nType )
		{
		case OBJECT_REQUEST:
		case UNCONNECTED_SEND_REQUEST:
			pduRelease( &pRequestGroup->cfg.objectCfg.PDU );
			EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize );
			break;

		case CLASS3_REQUEST:
			pduRelease( &pRequestGroup->cfg.class3Cfg.cfg.PDU );
			EtIP_free( &pRequestGroup->cfg.class3Cfg.cfg.pData, &pRequestGroup->cfg.class3Cfg.cfg.iDataSize );
			break;
		}
	}

	gpnRequestGroups = gRequestGroups;
}

/*---------------------------------------------------------------------------
** requestGroupService( )
**
** Service a particular request group
**---------------------------------------------------------------------------
*/

void requestGroupService( REQUEST_GROUP* pRequestGroup )
{
	REQUEST* pRequest;
	INT32 nTotal = 0, nTotalComplete = 0;

	if ( IS_TICK_GREATER(gdwTickCount, pRequestGroup->lRequestTimeoutTick ) )
	{
		requestGroupRemove( pRequestGroup );
		return;
	}
	
#ifndef EIP_NO_ORIGINATE_UCMM
	/* If this is incoming multiple service request */
	if( pRequestGroup->bIncoming )
	{
#endif
		/* Service all incoming requests in the group */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				nTotal++;
				if ( pRequest->nState == REQUEST_RESPONSE_RECEIVED )
					nTotalComplete++;
			}
		}

		if ( nTotal == 0 && pRequestGroup->bGeneralError == 0 )		/* Orphan group - all requests timed out - remove */
		{
			requestGroupRemove( pRequestGroup );
		}
		else if ( nTotal == nTotalComplete )		/* All ready to be responded */
		{
			switch ( pRequestGroup->nType )
			{
				case OBJECT_REQUEST:
				case UNCONNECTED_SEND_REQUEST:
					ucmmSendUnconnectedGroupResponse( pRequestGroup );
					break;
				
				case CLASS3_REQUEST:
					ucmmSendConnectedGroupResponse( pRequestGroup );
					break;
			}

			/* Remove all incoming requests in the group */
			for( pRequest = gpnRequests-1; pRequest >= gRequests; pRequest-- )
			{
				if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
				{
					requestRemove( pRequest );
				}
			}
			requestGroupRemove( pRequestGroup );
		}
#ifndef EIP_NO_ORIGINATE_UCMM
	}
	else
	{
		/* Do not process if still being populated */
		if ( pRequestGroup->nIndex == gnCurrentRequestGroupIndex )
			return;

		/* Service all outgoing requests in the group */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				nTotal++;
				if ( pRequest->nState == REQUEST_READY_FOR_GROUP_SEND )
				{
					pRequestGroup->nSessionId = pRequest->nSessionId;
					nTotalComplete++;
				}
			}
		}

		if ( nTotal > 0 && nTotal == nTotalComplete )	/* All ready to be sent */
		{
			if ( pRequestGroup->nType == OBJECT_REQUEST )
			{
				/* Send multiple service request */
				if ( ucmmSendUnconnectedGroupRequest( pRequestGroup ) )
				{
					/* If successful, mark requests as sent */
					for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
					{
						if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
						{
							pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
						}
					}
				}
			}
		}
	}
#endif
}

/*---------------------------------------------------------------------------
** requestGroupGetById( )
**
** Find a request group based on the request group Id
**---------------------------------------------------------------------------
*/

REQUEST_GROUP* requestGroupGetById( INT32 nRequestGroupId )
{
	REQUEST_GROUP* pRequestGroup;

	for( pRequestGroup = gRequestGroups; pRequestGroup < gpnRequestGroups; pRequestGroup++ )		/* Find our request group */
	{
		if ( pRequestGroup->nIndex == nRequestGroupId )
			return pRequestGroup;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** requestGroupGetServiceNumber( )
**
** Returns number of requests assigned to a request group
**---------------------------------------------------------------------------
*/

UINT16 requestGroupGetServiceNumber( REQUEST_GROUP* pRequestGroup )
{
	REQUEST* pRequest;
	UINT16 iServices = 0;
	
	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
	{
		if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
		{
			iServices++;
		}
	}

	return iServices;
}

#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT */
