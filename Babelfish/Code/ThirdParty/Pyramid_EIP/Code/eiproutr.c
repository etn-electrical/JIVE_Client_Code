/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPROUTR.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*/

#include "eipinc.h"

/* Supported by Message Router Class objects */
static UINT16 routerObjects[ROUTER_OBJECT_COUNT] = { ID_CLASS, ROUTER_CLASS, ASSEMBLY_CLASS, CONNMGR_CLASS,
#ifdef EIP_QOS
	QOS_CLASS,
#endif
#ifdef EIP_CIPSAFETY
	SA_SUP_CLASS,
#endif
#ifdef EIP_PCCC_SUPPORT
	PCCC_CLASS,
#endif
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	CONFIG_CLASS, 
#endif
#ifndef EIP_NO_CIP_ROUTING
	PORT_CLASS,
#endif
TCPIP_CLASS, ENETLINK_CLASS };

static UINT16 routerOptionalAttributes[ROUTER_ATTR_OPTIONAL_COUNT] = {ROUTER_ATTR_CLASS_LIST, ROUTER_ATTR_MAX_CONNECTIONS, ROUTER_ATTR_OPEN_CONNECTIONS};
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
static UINT16 routerOptionalServices[ROUTER_SVC_OPTIONAL_COUNT] = {SVC_MULTIPLE_SVC_REQUEST};
#endif

#ifndef EIP_NO_ORIGINATE_UCMM
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
/*---------------------------------------------------------------------------
** routerLogResponseGroup( )
**
** Response received for the previously sent multiple service request
**---------------------------------------------------------------------------
*/
void routerLogResponseGroup( REQUEST_GROUP* pRequestGroup, CPF_PKT_TAG* pktTag )
{
	REQUEST* pRequest;
	UINT8* pData = pktTag->pPacketPtr+2;				/* Jump to the error status */
	UINT16 nExtErrorLen, nExtErrorDataLen, iServices, iDataSize,
			iReqOffset, iNextReqOffset, iReqDataSize;
	INT32 nReqIndex = 0;

	iServices = requestGroupGetServiceNumber( pRequestGroup );
	if ( iServices == 0 )
	{
		/* Orphaned group - clean up and exit */
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequestGroup->lHostIPAddr, pRequestGroup->lIPAddress,
					"Orphaned group %d", pRequestGroup->nIndex);
		requestGroupRemove( pRequestGroup );
		return;
	}
	
	pRequestGroup->bGeneralError = *pData;					/* Store general status. Should be 0 in case of success */
	pData++;
	nExtErrorLen = (UINT16)((*pData)*2);					/* Extended error length is in words */
	pData++;

	if (pRequestGroup->bGeneralError)
	{
		if ( nExtErrorLen >= 2 && (pktTag->sTag.iTag_length-REPLY_HEADER_SIZE >= 2 ) )
		{
			/* If there is an error check for an extended status code */
			pRequestGroup->iExtendedError = UINT16_GET(pData);
			pData += sizeof(UINT16);
		}
		else
		{
			pRequestGroup->iExtendedError = 0;
		}

		if (pRequestGroup->bGeneralError != ROUTER_ERROR_SERVICE_ERROR)
		{
			for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
			{
				if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
				{
					pRequest->bGeneralError = pRequestGroup->bGeneralError;
					pRequest->iExtendedError = pRequestGroup->iExtendedError;

					nExtErrorDataLen = (UINT16)(PLATFORM_MIN((UINT16)(nExtErrorLen-2), (UINT16)(pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-2)));
					EtIP_realloc(&pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize,
								 pData, nExtErrorDataLen);

					requestResponseReceived( pRequest, NM_REQUEST_RESPONSE_RECEIVED);
				}
			}

			return;
		}
	}

	/* Now at the end of reply header */
	if (pktTag->sTag.iTag_length < REPLY_HEADER_SIZE + nExtErrorLen)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequestGroup->lHostIPAddr, pRequestGroup->lIPAddress,
					"Improperly formatted request for for group %d", pRequestGroup->nIndex);
		requestGroupRemove( pRequestGroup );
		return;
	}

	iDataSize = (UINT16)(pktTag->sTag.iTag_length - REPLY_HEADER_SIZE - nExtErrorLen);

	if ( iServices != UINT16_GET( pData ) )
	{
		/* Could not match number of requests -  log, clean up and exit */
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequestGroup->lHostIPAddr, pRequestGroup->lIPAddress,
					"Could not match number of requests for group %d", pRequestGroup->nIndex);
		requestGroupRemove( pRequestGroup );
		return;
	}

	/* Populate responses for the individual requests */
	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
	{
		if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
		{
			iReqOffset = UINT16_GET( &pData[2 + 2*nReqIndex] );

			if ( nReqIndex < (iServices - 1) )
			{
				iNextReqOffset = UINT16_GET( &pData[REPLY_HEADER_SIZE + 2*nReqIndex] );
				iReqDataSize = (UINT16)(iNextReqOffset - iReqOffset);
			}
			else
			{
				iReqDataSize = (UINT16)(iDataSize - iReqOffset);
			}

			/* Check that specifed offsets do not cause out of bounds error */
			if ( (iReqOffset + iReqDataSize) > iDataSize )
			{
				DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequestGroup->lHostIPAddr, pRequestGroup->lIPAddress,
					"Offsets invalid for group %d", pRequestGroup->nIndex);
				requestGroupRemove( pRequestGroup );
				return;
			}

			/* Store the response in the request object */
			pRequest->bGeneralError = pData[iReqOffset+2];		/* Store general status. Should be 0 in case of success */
			pRequest->iExtendedError = 0;
			nExtErrorLen = (UINT16)(pData[iReqOffset+3]*2);		/* Extended error length is in words */

			if ( pRequest->bGeneralError )						/* If there is an error check for an extended status */
			{
				if ((nExtErrorLen >= 2) && (iReqDataSize >= (REPLY_HEADER_SIZE+2)))
				{
					pRequest->iExtendedError = UINT16_GET(&pData[iReqOffset+REPLY_HEADER_SIZE]);

					/* Get any additional status data */
					nExtErrorDataLen = (UINT16)(PLATFORM_MIN((UINT16)(nExtErrorLen-2), (UINT16)(iReqDataSize-REPLY_HEADER_SIZE-2)));
					EtIP_realloc(&pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize,
								 &pData[iReqOffset+REPLY_HEADER_SIZE+2], nExtErrorDataLen);
					if (nExtErrorDataLen && pRequest->pExtendedErrorData == NULL )
					{
						notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
						requestRemove( pRequest );
						requestGroupRemove( pRequestGroup );
						return;
					}
				}

			}

			if ( iReqDataSize >= (REPLY_HEADER_SIZE + nExtErrorLen) )
			{
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, 
					(UINT8*)(&pData[iReqOffset+REPLY_HEADER_SIZE+nExtErrorLen]), (UINT16)(iReqDataSize-REPLY_HEADER_SIZE-nExtErrorLen) );                                                                        
				
				if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
				{
					notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
					requestRemove( pRequest );    /* Out of memory */
					requestGroupRemove( pRequestGroup );
					return;
				}
			}
			
			requestResponseReceived( pRequest, NM_REQUEST_RESPONSE_RECEIVED);
			nReqIndex++;
		}
	}	

	requestGroupRemove( pRequestGroup );
}
#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT */

/*---------------------------------------------------------------------------
** routerLogResponse( )
**
** Log response received for the previously sent request
**---------------------------------------------------------------------------
*/
void routerLogResponse( REQUEST* pRequest, CPF_PKT_TAG* pktTag )
{
	INT32 nExtErrorLen = 0;
	UINT16 nExtErrorDataLen;
	UINT8* pData = pktTag->pPacketPtr+2;

	/* Store the response in the request object */
	pRequest->bGeneralError = *pData;		/* Store general status. Should be 0 in case of success */
	pData++;
	nExtErrorLen = (*pData)*2;				/* Extended error length is in words */
	pData++;
	if ( nExtErrorLen >= 2 && (pktTag->sTag.iTag_length-REPLY_HEADER_SIZE >= 2 ) )
	{
		/* If there is an error check for an extended status code */
		pRequest->iExtendedError = UINT16_GET(pData);
		pData += sizeof(UINT16);

		/* Get any additional status data */
		nExtErrorDataLen = (UINT16)(PLATFORM_MIN((UINT16)(nExtErrorLen-2), (UINT16)(pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-2)));
		EtIP_realloc(&pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize,
					 pData, nExtErrorDataLen);
		if (nExtErrorDataLen && pRequest->pExtendedErrorData == NULL )
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			return;
		}

		pData += nExtErrorDataLen;
	}
	else
	{
		pRequest->iExtendedError = 0;
	}

	if ((INT32)(pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-nExtErrorLen) >= 0)
	{
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize,
			pData, (UINT16)(pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-nExtErrorLen) );
		if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			return;
		}
	}

	requestResponseReceived( pRequest, NM_REQUEST_RESPONSE_RECEIVED);
}
#endif /* #ifndef EIP_NO_ORIGINATE_UCMM */

#ifdef ET_IP_SCANNER
/*---------------------------------------------------------------------------
** routerLogClass3Response( )
**
** Log response received for originated class3 connection
**---------------------------------------------------------------------------
*/
static void routerLogClass3Response(CONNECTION* pConnection, CPF_PKT_TAG* pktTag )
{
	UINT8  *pData = pktTag->pPacketPtr+2;
	UINT8  bGeneralStatus;
	UINT16 iExtendedStatus;
	UINT16 iDataSize;
	UINT8* pDataStart;
	UINT16 iExtendedErrorDataSize;
	UINT8* pExtendedErrorDataStart;

	bGeneralStatus = *pData;
	pData++;
	iExtendedErrorDataSize = (UINT16)((*pData)*2);		/* Extended error length is in words */
	pData++;

	pExtendedErrorDataStart = pData;
	pData += iExtendedErrorDataSize;

	if (pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-iExtendedErrorDataSize > 0)
		iDataSize = (UINT16)(pktTag->sTag.iTag_length-REPLY_HEADER_SIZE-iExtendedErrorDataSize);
	else
		iDataSize = 0;

	pDataStart = pData;

	if (iExtendedErrorDataSize >= sizeof(UINT16))
	{
		iExtendedStatus = UINT16_GET(pExtendedErrorDataStart);
		pExtendedErrorDataStart += sizeof(UINT16);
		iExtendedErrorDataSize -= sizeof(UINT16);
	}
	else
	{
		iExtendedStatus = 0;
	}

	if ((pktTag->iConnSequence == pConnection->iOutDataSeqNbr) &&
		(pktTag->iConnSequence != pConnection->iInDataSeqNbr))
	{
		pConnection->iInDataSeqNbr = pktTag->iConnSequence;

		/* Store data for application */
		EtIP_realloc( &pConnection->pExtendedErrorData, &pConnection->iExtendedErrorDataSize, pExtendedErrorDataStart, iExtendedErrorDataSize);
		EtIP_realloc( &pConnection->pResponseData, &pConnection->iResponseDataSize, pDataStart, iDataSize );

		pConnection->bGeneralStatus = bGeneralStatus;		/* Store general status. Should be 0 in case of success */
		pConnection->iExtendedStatus = iExtendedStatus;		/* Store extended status. */

		notifyEvent( NM_NEW_CLASS3_RESPONSE, pConnection->cfg.nInstance, 0 );
	}
}
#endif

/*---------------------------------------------------------------------------
** routerDataTransferReceived( )
**
** Message Router parses TCP message and directs the request to the
** appropriate objects for processing
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL routerDataTransferReceived( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	UINT16 len, tag_len;
	CPF_TAG tag;
	CPF_SOCKADDR_TAG toTag;		/* Store Target->Originator address tag that came with the UCMM or Class3 message */
	CPF_SOCKADDR_TAG otTag;		/* Store Originator->Target address tag that came with the UCMM or Class3 message */
	UINT8* pData;
	INT32 i;
#ifndef EIP_NO_ORIGINATE_UCMM
	SESSION* pToSession;
	REQUEST* pRequest;
	REQUEST_GROUP* pRequestGroup;
	BOOL bFoundUCMMRequest = FALSE;
#endif
	ENCAP_DT_HDR dataHdr;
	UINT16  iObjectCount;
	CPF_ADR_TAG addrTag;
	CPF_PKT_TAG  pktTag;
	CPF_PCCC_ADR_TAG pcccAddrPktTag;
	CONNECTION* pConnection;
	BOOL bRet = TRUE;
#ifdef CONNECTION_STATS
	UINT32 lRecvDelay;
#endif

	memset( &otTag, 0, sizeof(otTag) );
	memset( &toTag, 0, sizeof(toTag));
	memset( &addrTag, 0, sizeof(addrTag) );
	memset( &pktTag, 0, sizeof(pktTag) );
	memset( &pcccAddrPktTag, 0, sizeof(pcccAddrPktTag) );

	pData = &gmsgBuf[sizeof(ENCAPH)];				/* Point right outside the header */

	if ( pHdr->lStatus != ENCAP_E_SUCCESS )			/* Status other than 0 indicates failure */
	{
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"routerDataTransferReceived Status is 0x%x for session %d", pHdr->lStatus, pSession->nSessionId);

#ifndef EIP_NO_ORIGINATE_UCMM
		/* Check if there is a request waiting for response for this session, and if yes, mark as failed */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )        /* See if it's a reply to our UCMM request */
		{
			if (pRequest->nSessionId == pSession->nSessionId &&
				!pRequest->bIncoming && pRequest->nState == REQUEST_WAITING_FOR_RESPONSE &&
				pRequest->lContext1 == pHdr->lContext1 &&
				pRequest->lContext2 == pHdr->lContext2 )
			{
				/* Modify the request object appropriately */
				pRequest->bGeneralError = ROUTER_ERROR_ENCAP_PROTOCOL;
				pRequest->iExtendedError = 0;
				pRequest->cfg.objectCfg.iDataSize = 0;
				requestResponseReceived( pRequest, NM_REQUEST_RESPONSE_RECEIVED);
				break;
			}
		}
#endif
		return TRUE;
	}

	len = pHdr->iLength;

	if ( len < ENCAP_DT_HDR_SIZE )			/* Data header is required */
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
					"routerDataTransferReceived invalid packet length");

		/* Only bother to tell sender of the message that it's invalid if a non-zero
			length is provided.  Otherwise it's probably a buggy EtherNet/IP
			implementation and not dropping the message could result in
			infinite loops of encapsulation messages being exchanged */
		if (len == 0)
			return TRUE;

		return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
	}

	/*
	** Grab the parameters from the data transfer header. Adjust the
	** packet pointer and the remaining length to account for the values
	** that were just read.
	*/
	memcpy( &dataHdr, pData, ENCAP_DT_HDR_SIZE );	/* Read the data header */
	ENCAP_CVT_HS( dataHdr.iTimeout );

	pData += ENCAP_DT_HDR_SIZE;						/* Point beyond the data header */
	len -= ENCAP_DT_HDR_SIZE;						/* Adjust the length */

	/*
	** Next, check for a standard object header which should precede the
	** individual header tags.
	*/
	if ( len < sizeof(UINT16) )
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"routerDataTransferReceived invalid length specified in the header");
		return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
	}

	iObjectCount = UINT16_GET( pData );				/* Number of tags (objects) to follow */

	pData += sizeof(UINT16);						/* Point to the data beyond Object count */
	len -= sizeof(UINT16);							/* Remove the object length from the total */

	/*
	** Parse the tags in the packet. Convert the standard tag header to
	** host byte order.
	*/
	for ( i = 0; i < iObjectCount; i++ )
	{
		/*
		** Copy the tag header into a place where it can be converted into
		** host standard byte order.  Then decode the tag to find out where
		** it belongs.  Make sure that the tag length doesn't exceed the
		** amount of data remaining in the packet.
		*/
		if ( len < CPF_TAG_SIZE )
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
					"routerDataTransferReceived invalid packet length");
			return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
		}

		memcpy( &tag, pData, CPF_TAG_SIZE );
		ENCAP_CVT_HS( tag.iTag_type );
		ENCAP_CVT_HS( tag.iTag_length );

		tag_len = tag.iTag_length;

		/* Adjust the data length and data pointer to account for the tag header */
		len -= CPF_TAG_SIZE;
		pData += CPF_TAG_SIZE;

		/* Does the tag length specify more data than the packet contains? */
		if ( tag_len > len )
		{
			return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
		}

		/* Switch out on the TAG types */
		switch ( tag.iTag_type )
		{
		case CPF_TAG_ADR_NULL:				/* Destination address - store in addrTag */
		case CPF_TAG_ADR_LOCAL:
		case CPF_TAG_ADR_OFFLINK:
		case CPF_TAG_ADR_CONNECTED:

			/* The length of the tag can not exceed the storage reserved for it */
			if ( tag_len > sizeof(addrTag.data) )
			{
				DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
							"routerDataTransferReceived The length of the tag exceeds the storage reserved for it");
				return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
			}

			memcpy( &addrTag.sTag, &tag, CPF_TAG_SIZE );
			if ( tag_len )
			{
				memcpy( &addrTag.data.g, pData, tag_len );
				pData += tag_len;
				len -= tag_len;
			}
			break;
		case CPF_TAG_ADR_PCCC:				/* pccc packet address data (pktTag is used for PCCC message) */
			memcpy( &pcccAddrPktTag.sTag, &tag, CPF_TAG_SIZE );
			pcccAddrPktTag.pPacketPtr = pData;

			pData += tag_len;
			len -= tag_len;
			break;
		case CPF_TAG_PKT_PCCC:				/* The packet data - store in pktTag */
		case CPF_TAG_PKT_UCMM:
		case CPF_TAG_PKT_CONNECTED:

			memcpy( &pktTag.sTag, &tag, CPF_TAG_SIZE );
			pktTag.pPacketPtr = pData;

			pData += tag_len;
			len -= tag_len;
			break;

		case CPF_TAG_SOCKADDR_OT:			/* Originator -> Target socket address */

			if ( tag_len != CPF_TAG_SOCKADDR_SIZE)
			{
				DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
							"routerDataTransferReceived the tag length is invalid");
				return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
			}

			memcpy( &otTag.sTag, &tag, CPF_TAG_SIZE );
			platformCopySockAddrInFromBuffer(&otTag.sAddr, pData);
			pData += tag_len;
			len -= tag_len;
			break;

		case CPF_TAG_SOCKADDR_TO:    /* Target -> Originator socket address */

			if ( tag_len != CPF_TAG_SOCKADDR_SIZE)
			{
				DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
						"routerDataTransferReceived the tag length is invalid");
				return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
			}

			memcpy( &toTag.sTag, &tag, CPF_TAG_SIZE );
			platformCopySockAddrInFromBuffer(&toTag.sAddr, pData);
			pData += tag_len;
			len -= tag_len;
			break;

		default:
			/* Return an error on anything we don't know about */
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
					"routerDataTransferReceived the tag type is invalid");
			return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
		}
	}

	if ( len )			/* Should not be anything else in the packet */
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"routerDataTransferReceived unexpected extra data");
		return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
	}

	if ((pktTag.sTag.iTag_length > MAX_DATA_FIELD_SIZE) ||		/* Packet data should not exceed the limit */
		((pktTag.sTag.iTag_type == CPF_TAG_PKT_UCMM) && (pktTag.sTag.iTag_length > MAX_UCMM_SIZE)))
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"routerDataTransferReceived packet is too long");
		return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );
	}

	if ((dataHdr.lTarget != INTERFACE_HANDLE) ||
		((CPF_TAG_PKT_CONNECTED != pktTag.sTag.iTag_type) &&
		(CPF_TAG_PKT_UCMM != pktTag.sTag.iTag_type) &&
		(CPF_TAG_PKT_PCCC != pktTag.sTag.iTag_type)))
	{
		/* This is not a UCMM, PCCC, or Class3 message */
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"routerDataTransferReceived unknown packet structure");
		return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
	}

	if ((( pHdr->iCommand != ENCAP_CMD_SEND_RRDATA ) ||			/* This is UCMM */
		( pktTag.sTag.iTag_type != CPF_TAG_PKT_UCMM ) ||
		( addrTag.sTag.iTag_type != CPF_TAG_ADR_NULL ) )
		&&
		(( pHdr->iCommand != ENCAP_CMD_SEND_RRDATA ) ||			/* This is PCCC */
		( pktTag.sTag.iTag_type != CPF_TAG_PKT_PCCC ) ||
		( addrTag.sTag.iTag_type != CPF_TAG_ADR_NULL ) )
		&&
		( ( pHdr->iCommand != ENCAP_CMD_SEND_UNITDATA ) ||		/* This is Class 3 message */
		( pktTag.sTag.iTag_type != CPF_TAG_PKT_CONNECTED ) ||
		( addrTag.sTag.iTag_type != CPF_TAG_ADR_CONNECTED ) ) )
	{
		/* Did not recognize the packet structure */
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"routerDataTransferReceived unknown packet structure");
		return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
	}

	switch (pktTag.sTag.iTag_type)
	{
	case CPF_TAG_PKT_UCMM:
		pktTag.bConnected = FALSE;
#ifndef EIP_NO_ORIGINATE_UCMM
		if ( (*pktTag.pPacketPtr) == (SVC_MULTIPLE_SVC_REQUEST | REPLY_BIT_MASK) )		/* This is reply to our multiple service request */
		{
			for( pRequestGroup = gRequestGroups; pRequestGroup < gpnRequestGroups; pRequestGroup++ )
			{
				if ( pRequestGroup->lContext1 == pHdr->lContext1 &&
					 pRequestGroup->lContext2 == pHdr->lContext2 )
				{
#ifdef CONNECTION_STATS
					gSystemStats.iNumRecvUCMMMessagesPPS++;
					gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
					routerLogResponseGroup( pRequestGroup, &pktTag );
					bFoundUCMMRequest = TRUE;
					break;
				}
			}

			/* If it's not in the Request Group, check the usual requests */
			if (bFoundUCMMRequest)
				break;
		}

		if ( (*pktTag.pPacketPtr) & REPLY_BIT_MASK )							/* This is some reply */
		{
			for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )		/* See if it's a reply to our UCMM request */
			{
				if ( pRequest->lContext1 == pHdr->lContext1 &&
					 pRequest->lContext2 == pHdr->lContext2 )
				{
					if (pRequest->nSessionId == pSession->nSessionId &&
						!pRequest->bIncoming && pRequest->nState == REQUEST_WAITING_FOR_RESPONSE )
					{
						bFoundUCMMRequest = TRUE;
#ifdef CONNECTION_STATS
						gSystemStats.iNumRecvUCMMMessagesPPS++;
						gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
						routerLogResponse( pRequest, &pktTag );
						break;
					}
					else if ( pRequest->nType == UNCONNECTED_SEND_REQUEST && pRequest->nState == REQUEST_WAITING_FOR_RESPONSE )
					{
						if (pRequest->nSessionId == pSession->nSessionId &&
							(((pRequest->cfg.objectCfg.bService|REPLY_BIT_MASK) == *pktTag.pPacketPtr ) ||
							((UNCONNECTED_SEND_CMD_CODE|REPLY_BIT_MASK) == *pktTag.pPacketPtr ) ) )
						{
							/* If Unconnected Send was originated by the API call */
							if ( !pRequest->bIncoming )
							{
								bFoundUCMMRequest = TRUE;
#ifdef CONNECTION_STATS
								gSystemStats.iNumRecvUCMMMessagesPPS++;
								gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
								routerLogResponse( pRequest, &pktTag );
							}
							else	/* If we received this Unconnected Send from somebody else and were resending it */
							{
								pToSession = sessionGetBySessionId( pRequest->nFromSessionId );
								if ( pToSession != NULL )
									bRet = socketEncapSendData( pToSession );

								requestRemove(pRequest);
							}
							break;
						}
					}
				}
			}

#ifdef ET_IP_SCANNER
			if (((*pktTag.pPacketPtr == (FWD_OPEN_CMD_CODE | REPLY_BIT_MASK)) || (*pktTag.pPacketPtr == (LARGE_FWD_OPEN_CMD_CODE | REPLY_BIT_MASK))) &&		/* This is a Forward Open reply */
				(bFoundUCMMRequest == FALSE))
			{
#ifdef CONNECTION_STATS
				gSystemStats.iNumRecvUCMMMessagesPPS++;
				gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
				return scanmgrProcessFwdOpenReply( pSession->nSessionId, pktTag.pPacketPtr, pktTag.sTag.iTag_length, &otTag, &toTag );
			}

			if ((*pktTag.pPacketPtr == (FWD_CLOSE_CMD_CODE | REPLY_BIT_MASK)) &&	/* This is a Forward Close reply */
				(bFoundUCMMRequest == FALSE))
			{
#ifdef CONNECTION_STATS
				gSystemStats.iNumRecvUCMMMessagesPPS++;
				gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
				scanmgrProcessFwdCloseReply(pktTag.pPacketPtr );
				return TRUE;
			}
#endif /* #ifdef ET_IP_SCANNER */

			if (pRequest < gpnRequests)
				return bRet;

			/* No request was found, reply with "Service not supported"
			   (This was really put in to speed up the conformance test that
			   uses services above 0x7F) */
		}
#endif /* EIP_NO_ORIGINATE_UCMM */

		/* This is a UCMM request to us */
		routerParseObjectRequest(pSession->nSessionId, pSession->lClientIPAddr, &pktTag, pktTag.pPacketPtr, pktTag.sTag.iTag_length, INVALID_REQUEST_INDEX, &otTag, &toTag, pHdr->lContext1, pHdr->lContext2);
#ifdef CONNECTION_STATS
		gSystemStats.iNumRecvUCMMMessagesPPS++;
		gSystemStats.systemStats.iNumRecvUCMMMessages++;
#endif
		return TRUE;

#ifdef SUPPORT_CSP
	case CPF_TAG_PKT_PCCC:
		return cspRequest(pSession, pHdr, &pktTag);
#endif

	case CPF_TAG_PKT_CONNECTED:			/* This is a Class 3 message */
		if ( addrTag.sTag.iTag_length == sizeof(UINT32) )
		{
			ENCAP_CVT_HL( addrTag.data.sC.lCid );
			/* Find the Class 3 connection this message belongs to */
			for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
			{
				if ( pConnection->lConnectionState == ConnectionEstablished &&
					pConnection->cfg.bTransportClass == Class3 &&
					addrTag.data.sC.lCid == pConnection->lConsumingCID )
					break;
			}

			if ( pConnection < gpnConnections )			/* Class 3 connection found */
			{
				pktTag.bConnected = TRUE;
				pktTag.iConnSequence = UINT16_GET( pktTag.pPacketPtr );
				pktTag.sTag.iTag_length -= sizeof(UINT16);
				pktTag.pPacketPtr += sizeof(UINT16);

				/* For Class 3 request reset the last received data timestamp */
				connectionResetConsumingTicks( pConnection );

#ifdef CONNECTION_STATS
				if ( pConnection->bCollectingStats )
				{
					if (pConnection->lLastRcvdTick )
					{
						if ( gdwTickCount >= pConnection->lLastRcvdTick )
							lRecvDelay = gdwTickCount - pConnection->lLastRcvdTick;
						else
							lRecvDelay = (0xffffffff - pConnection->lLastRcvdTick) + gdwTickCount;

						if ( lRecvDelay > pConnection->lMaxRcvdDelay )
							pConnection->lMaxRcvdDelay = lRecvDelay;
					}

					if ( !pConnection->lStartRcvdTick )
						pConnection->lStartRcvdTick = gdwTickCount;

					pConnection->lTotalRcvdPackets++;
					pConnection->lRcvdPPSCount++;
				}
#endif /* CONNECTION_STATS */

				if (pConnection->cfg.bOriginator) /* This should be a reply to our Class 3 request */
				{
#ifdef ET_IP_SCANNER
					pConnection->bOutstandingClass3Request = FALSE;

					if (pConnection->requestCfg.bNoService == TRUE)
					{
						if ((pktTag.iConnSequence == pConnection->iOutDataSeqNbr) &&
							(pktTag.iConnSequence != pConnection->iInDataSeqNbr))
						{
							EtIP_realloc( &pConnection->pResponseData, &pConnection->iResponseDataSize, pktTag.pPacketPtr,  pktTag.sTag.iTag_length );    /* Store data for application */

							pConnection->bGeneralStatus = 0;					/* Status assumed good */
							pConnection->iExtendedStatus = 0;

							notifyEvent( NM_NEW_CLASS3_RESPONSE, pConnection->cfg.nInstance, 0 );
					}

						pConnection->iInDataSeqNbr = pktTag.iConnSequence;
					}
					/* This is a reply to our Class 3 request */
					else if ( (pConnection->requestCfg.bService|REPLY_BIT_MASK) == *pktTag.pPacketPtr )
					{
						routerLogClass3Response(pConnection, &pktTag );
#ifdef CONNECTION_STATS
						gSystemStats.systemStats.iNumRecvClass3Messages++;
#endif

					}
#endif /* #ifdef ET_IP_SCANNER */

					return TRUE;
				}

				/* This is a Class 3 request directed to us */
				pktTag.pConnection = pConnection;
				routerParseObjectRequest(pSession->nSessionId, pSession->lClientIPAddr, &pktTag, pktTag.pPacketPtr, pktTag.sTag.iTag_length, INVALID_REQUEST_INDEX, &otTag, &toTag, pHdr->lContext1, pHdr->lContext2);
#ifdef CONNECTION_STATS
				gSystemStats.systemStats.iNumRecvClass3Messages++;
#endif
				return TRUE;
			}

			/* Could not match a connection, ignore the message */
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pSession->lHostIPAddr, pSession->lClientIPAddr, "Could not match Consuming Id 0x%x for class3 message", addrTag.data.sC.lCid);
			return TRUE;
		}
		break;
	default:
		/* This is not a UCMM, PCCC, or Class3 message */
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"routerDataTransferReceived unknown packet structure");
		return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );

	}

	return bRet;
}

/*---------------------------------------------------------------------------
** routerParseObjectRequest( )
**
** Parse UCMM or Class 3 request
**---------------------------------------------------------------------------
*/

void routerParseObjectRequest( INT32 nSessionId, UINT32 lClientAddr, CPF_PKT_TAG* pPacketTag, UINT8* pData, UINT16 iDataSize, INT32 nGroupIndex, CPF_SOCKADDR_TAG* otTag, CPF_SOCKADDR_TAG* toTag, UINT32 lContext1, UINT32 lContext2)
{
	UINT16 iPDUSize, iTagSize;
	REQUEST* pRequest;
	EtIPDeviceID* pDeviceID;

	pRequest = requestNewIncoming(
					pPacketTag->bConnected ? CLASS3_REQUEST : OBJECT_REQUEST,
					FALSE,
					nSessionId );		/* Register new incoming object request */

	if ( pRequest == NULL )				/* No more space for requests */
	{
		notifyEvent( NM_PENDING_REQUESTS_LIMIT_REACHED, 0, 0 );
		return;
	}

	pRequest->lIPAddress = lClientAddr;
	pRequest->cfg.objectCfg.bService = *pData;

	pRequest->lContext1 = lContext1; /* Save the senders's context to send it back with the response */
	pRequest->lContext2 = lContext2;
	pRequest->nRequestGroupIndex = nGroupIndex;

	if ( pPacketTag->bConnected )
	{
		pRequest->cfg.class3Cfg.iConnectionSerialNbr = pPacketTag->pConnection->iConnectionSerialNbr;
		pRequest->cfg.class3Cfg.iVendorID = pPacketTag->pConnection->iVendorID;
		pRequest->cfg.class3Cfg.lDeviceSerialNbr = pPacketTag->pConnection->lDeviceSerialNbr;
		pRequest->cfg.class3Cfg.iConnSequence = pPacketTag->iConnSequence;

		/* If the sequence number has not changed - just resend the last response data */
		if ((pPacketTag->iConnSequence == pPacketTag->pConnection->iInDataSeqNbr) &&
			(nGroupIndex == INVALID_REQUEST_INDEX)
#ifdef ET_IP_MODBUS
			/* Modbus connections ignore the sequence count and always send
			   the request through */
			&& (pPacketTag->pConnection->cfg.modbusCfg.iModbusAddressSize == 0)
#endif
			)
		{
			pRequest->bGeneralError = pPacketTag->pConnection->bGeneralStatus;
			pRequest->iExtendedError = pPacketTag->pConnection->iExtendedStatus;
			pRequest->cfg.objectCfg.iDataSize = pPacketTag->pConnection->iResponseDataSize;
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pPacketTag->pConnection->pResponseData, pPacketTag->pConnection->iResponseDataSize );
			EtIP_realloc( &pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize, pPacketTag->pConnection->pExtendedErrorData, pPacketTag->pConnection->iExtendedErrorDataSize);
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}
		else
		{
			pPacketTag->pConnection->iInDataSeqNbr = pPacketTag->iConnSequence;
			/* Set the request timeout tick */
			pRequest->lRequestTimeoutTick = gdwTickCount + pPacketTag->pConnection->lTimeoutInterval;
		}
	}
	else
	{
		/* Set the request timeout tick */
		pRequest->lRequestTimeoutTick = gdwTickCount + DEFAULT_EXPLICIT_REQUEST_TIMEOUT;
	}

	/* Parse the next part of the packet that must be Router PDU (Protocol Data Unit) */
	iPDUSize = (UINT16)pduParse( pData, &pRequest->cfg.objectCfg.PDU, TRUE, iDataSize);
	if ( iPDUSize == 0 || iPDUSize > iDataSize )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_SEG_TYPE;
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		return;
	}

	/* Check if there is an electronic key */
	pDeviceID = (EtIPDeviceID*)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_KEY, 0, &iTagSize);
	if ( iTagSize )
	{
		if (!idCheckElectronicKey(pDeviceID, &pRequest->bGeneralError, &pRequest->iExtendedError))
		{
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			return;
		}
	}

	pRequest->cfg.objectCfg.iDataSize = (UINT16)(iDataSize - iPDUSize);
	pRequest->cfg.objectCfg.pData = EtIP_malloc( &pData[iPDUSize], pRequest->cfg.objectCfg.iDataSize );

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		requestRemove( pRequest );
		return;
	}

	if (otTag)
		memcpy( &pRequest->sOTTagAddr, &otTag->sAddr, sizeof(struct sockaddr_in) );
	if (toTag)
		memcpy( &pRequest->sTOTagAddr, &toTag->sAddr, sizeof(struct sockaddr_in) );

	pRequest->nState = REQUEST_LOGGED;
}

/*---------------------------------------------------------------------------
** routerSendClassAttrAll( )
**
** Respond to the GetAttributeAll service request for the Message Router class
**---------------------------------------------------------------------------
*/

void routerSendClassAttrAll( REQUEST* pRequest )
{
	UINT8* pData;
	UINT16 *p16;

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*(5+ROUTER_ATTR_OPTIONAL_COUNT
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	+ ROUTER_SVC_OPTIONAL_COUNT
#endif
	) );

	pData = pRequest->cfg.objectCfg.pData;

	/* Revision */
	UINT16_SET(pData, ROUTER_CLASS_REVISION);
	pData += sizeof(UINT16);
	/* Optional attributes */
	UINT16_SET(pData, ROUTER_ATTR_OPTIONAL_COUNT);
	pData += sizeof(UINT16);
	for (p16 = routerOptionalAttributes; 
		p16 < &routerOptionalAttributes[ROUTER_ATTR_OPTIONAL_COUNT]; 
		p16++, pData += sizeof(UINT16))
	{
		UINT16_SET(pData, *p16);
	}

	/* Optional services */
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	UINT16_SET(pData, ROUTER_SVC_OPTIONAL_COUNT);
	pData += sizeof(UINT16);
	for (p16 = routerOptionalServices; 
		p16 < &routerOptionalServices[ROUTER_SVC_OPTIONAL_COUNT]; 
		p16++, pData += sizeof(UINT16))
	{
		UINT16_SET(pData, *p16);
	}
#else
	UINT16_SET(pData, 0);
	pData += sizeof(UINT16);
#endif

	/* Max class attribute number */
	UINT16_SET(pData, ROUTER_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
	/* Max instance attribute number */
	UINT16_SET(pData, ROUTER_ATTR_OPEN_CONNECTIONS);
}

/*--------------------------------------------------------------------------------
** routerSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the Message Router class
**--------------------------------------------------------------------------------
*/

void routerSendClassAttrSingle( REQUEST* pRequest)
{
	UINT16 iVal;
	UINT16 iTagSize;
	UINT8* pData;
	UINT16 *p16;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch( iAttribute )
	{
	case ROUTER_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(ROUTER_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ROUTER_CLASS_ATTR_MAX_INSTANCE:
		iVal = ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ROUTER_CLASS_ATTR_NUM_INSTANCES:
		iVal = ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ROUTER_CLASS_ATTR_OPT_ATTR_LIST:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*(ROUTER_ATTR_OPTIONAL_COUNT+1));
		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, ROUTER_ATTR_OPTIONAL_COUNT);
		pData += sizeof(UINT16);
		for (p16 = routerOptionalAttributes;
			p16 < &routerOptionalAttributes[ROUTER_ATTR_OPTIONAL_COUNT];
			p16++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *p16);
		}
		break;
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	case ROUTER_CLASS_ATTR_OPT_SVC_LIST:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*(ROUTER_SVC_OPTIONAL_COUNT+1));
		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, ROUTER_SVC_OPTIONAL_COUNT);
		pData += sizeof(UINT16);
		for (p16 = routerOptionalServices;
			p16 < &routerOptionalServices[ROUTER_SVC_OPTIONAL_COUNT];
			p16++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *p16);
		}
		break;
#endif
	case ROUTER_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(ROUTER_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ROUTER_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(ROUTER_ATTR_OPEN_CONNECTIONS);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** routerSendInstanceAttrAll( )
**
** Respond to the GetAttributeAll service request for the Message Router instance
**---------------------------------------------------------------------------
*/

void routerSendInstanceAttrAll( REQUEST* pRequest)
{
	UINT8* pData;
	UINT32* p;
	INT32  i;
	UINT32  iObjectsFound = 0;

	/* Find out how many "true" application objects there are */
	for ( p = gpClientAppProcClasses; p < &gpClientAppProcClasses[glClientAppProcClassNbr]; p++)
	{
		/* Make sure the application isn't overriding one of the native objects */
		for (i = 0; i < ROUTER_OBJECT_COUNT; i++)
		{
			if (routerObjects[i] == *p)
				break;
		}

		if (i >= ROUTER_OBJECT_COUNT)
			iObjectsFound++;
	}

#if defined EIP_FILE_OBJECT
	if (gbFileObjectEnabled)
		iObjectsFound++;
#endif

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((sizeof(UINT16)*(ROUTER_OBJECT_COUNT+iObjectsFound)) + sizeof(UINT16)*3));

	if ( pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		requestRemove( pRequest );
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	/* now set the number of objects */
	UINT16_SET(pData, (ROUTER_OBJECT_COUNT+iObjectsFound));
	pData += sizeof(UINT16);

	/* First the stardard objects */
	for( i = 0; i < ROUTER_OBJECT_COUNT; i++ )
	{
		UINT16_SET( pData, routerObjects[i] );
		pData += sizeof(UINT16);
	}

#if defined EIP_FILE_OBJECT
	if (gbFileObjectEnabled)
	{
		UINT16_SET(pData, FILE_CLASS);
		pData += sizeof(UINT16);
	}
#endif

	/* Now the application objects */
	for ( p = gpClientAppProcClasses; p < &gpClientAppProcClasses[glClientAppProcClassNbr]; p++)
	{
		/* Make sure the application isn't overriding one of the native objects */
		for (i = 0; i < ROUTER_OBJECT_COUNT; i++)
		{
			if (routerObjects[i] == *p)
				break;
		}

		if (i >= ROUTER_OBJECT_COUNT)
		{
			UINT16_SET( pData, *p);
			pData += sizeof(UINT16);
		}
	}

	/* now list max connections */
	UINT16_SET(pData, MAX_CONNECTIONS);
	pData += sizeof(UINT16);
	UINT16_SET(pData, connectionGetConnectedCount());
}

/*---------------------------------------------------------------------------
** routerSendInstanceAttrSingle( )
**
** Respond to the GetAttributeSingle service request to the Message Router
** instance
**---------------------------------------------------------------------------
*/

void routerSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iVal, iTagSize;
	UINT32 iAttribute;
	UINT8* pData;
	UINT32* p;
	INT32  i;
	UINT32  iObjectsFound = 0;

	iInstance = iInstance;
	iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	if ( iTagSize == 0 )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	switch( iAttribute )
	{
	case ROUTER_ATTR_CLASS_LIST:
		/* Find out how many "true" application objects there are */
		for ( p = gpClientAppProcClasses; p < &gpClientAppProcClasses[glClientAppProcClassNbr]; p++)
		{
			/* Make sure the application isn't overriding one of the native objects */
			for (i = 0; i < ROUTER_OBJECT_COUNT; i++)
			{
				if (routerObjects[i] == *p)
					break;
			}

			if (i >= ROUTER_OBJECT_COUNT)
				iObjectsFound++;
		}

#if defined EIP_FILE_OBJECT
		if (gbFileObjectEnabled)
			iObjectsFound++;
#endif

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((sizeof(UINT16)*(ROUTER_OBJECT_COUNT+iObjectsFound)) + sizeof(UINT16)));
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;

			/* now set the number of objects */
			UINT16_SET(pData, (ROUTER_OBJECT_COUNT+iObjectsFound));
			pData += sizeof(UINT16);

			/* First the stardard objects */
			for( i = 0; i < ROUTER_OBJECT_COUNT; i++ )
			{
				UINT16_SET( pData, routerObjects[i] );
				pData += sizeof(UINT16);
			}

#if defined EIP_FILE_OBJECT
			if (gbFileObjectEnabled)
			{
				UINT16_SET(pData, FILE_CLASS);
				pData += sizeof(UINT16);
			}
#endif

			/* Now the application objects */
			for ( p = gpClientAppProcClasses; p < &gpClientAppProcClasses[glClientAppProcClassNbr]; p++)
			{
				/* Make sure the application isn't overriding one of the native objects */
				for (i = 0; i < ROUTER_OBJECT_COUNT; i++)
				{
					if (routerObjects[i] == *p)
						break;
				}

				if (i >= ROUTER_OBJECT_COUNT)
				{
					UINT16_SET( pData, *p);
					pData += sizeof(UINT16);
				}
			}
		}
		else
		{
			notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
			requestRemove( pRequest );
			return;
		}
		break;
	case ROUTER_ATTR_MAX_CONNECTIONS:
		iVal = ENCAP_TO_HS(MAX_CONNECTIONS);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ROUTER_ATTR_OPEN_CONNECTIONS:
		iVal = ENCAP_TO_HS(connectionGetConnectedCount());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:		/* Only attributes supported at this time */
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
/*---------------------------------------------------------------------------
** routerMultipleServiceRequest( )
**
** Respond to the Multiple Service request (handled by Message Router object)
**---------------------------------------------------------------------------
*/
static void routerProcessMultipleServiceRequest( REQUEST* pRequest)
{
	UINT16 i, iNumberOfServices, iRequestDataSize, iRequestDataOffset, iNextRequestDataOffset;
	REQUEST_GROUP*  pRequestGroup;
	SESSION* pSession;
	UINT8* pData = pRequest->cfg.objectCfg.pData,
		 *pOffset;
	UINT16 iDataSize = pRequest->cfg.objectCfg.iDataSize;		/* convenience pointer */
	CPF_PKT_TAG	PacketTag;										/* "placeholder" tag */
	CPF_SOCKADDR_TAG otTag, toTag;								/* "placeholder" tags */
	CONNECTION *pConnection;

	if ( iDataSize < sizeof(UINT16))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iNumberOfServices = UINT16_GET( pData );
	pData += sizeof(UINT16);

	/* Check there is at least one service and there is at least space for all offsets */
	if ( iDataSize < (sizeof(UINT16)*iNumberOfServices) )
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/* Verify that we have enough data. The last request must have at least 4 bytes */
	for (i = 0, pOffset = pData; 
		 i < iNumberOfServices; 
		 i++, pOffset += sizeof(UINT16))
	{
		iRequestDataOffset = UINT16_GET(pOffset);

		if ( iRequestDataOffset >= (iDataSize - 4) )
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
	}

	/* We now have a valid Multiple Group Request */
	pSession = sessionGetBySessionId( pRequest->nSessionId );
	if (pSession == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "Session terminated from under us with session Id %d", pRequest->nSessionId);
		return;
	}

	/* Register new incoming object request group */
	pRequestGroup = requestGroupNewIncoming( pRequest->nType, pSession->nSessionId );
	if ( pRequestGroup == NULL )					/* No more space for request groups */
	{
		notifyEvent( NM_PENDING_REQUEST_GROUPS_LIMIT_REACHED, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	pRequestGroup->nNumberServices = iNumberOfServices;
	pRequestGroup->lIPAddress = pRequest->lIPAddress;

	pRequestGroup->lContext1 = pRequest->lContext1;			/* Save the senders's context to send it back with the response */
	pRequestGroup->lContext2 = pRequest->lContext2;

	switch(pRequest->nType)
	{
	case CLASS3_REQUEST:

		pConnection = connectionGetFromSerialNumber( pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );
		if (pConnection == NULL)
		{
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0, "Connection terminated from under us with session Id %d", pRequest->nSessionId);
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		/* Fill in the "placeholder" tag */
		PacketTag.bConnected = TRUE;
		PacketTag.pConnection = pConnection;
		PacketTag.iConnSequence = pRequest->cfg.class3Cfg.iConnSequence;

		pRequestGroup->cfg.class3Cfg.iConnectionSerialNbr = pRequest->cfg.class3Cfg.iConnectionSerialNbr;
		pRequestGroup->cfg.class3Cfg.iVendorID = pRequest->cfg.class3Cfg.iVendorID;
		pRequestGroup->cfg.class3Cfg.lDeviceSerialNbr = pRequest->cfg.class3Cfg.lDeviceSerialNbr;
		pRequestGroup->cfg.class3Cfg.iConnSequence = pRequest->cfg.class3Cfg.iConnSequence;

		/* Set the request timeout tick */
		pRequestGroup->lRequestTimeoutTick = gdwTickCount + pConnection->lTimeoutInterval;
		break;
	case OBJECT_REQUEST:
		PacketTag.bConnected = FALSE;
		/* Set the request timeout tick */
		pRequestGroup->lRequestTimeoutTick = gdwTickCount + DEFAULT_EXPLICIT_REQUEST_TIMEOUT;
		break;
	}

	memcpy(&otTag.sAddr, &pRequest->sOTTagAddr, sizeof(struct sockaddr_in));
	memcpy(&toTag.sAddr, &pRequest->sTOTagAddr, sizeof(struct sockaddr_in));


	/* Log all embedded requests */
	for (i = 0, pOffset = pData; 
		 i < iNumberOfServices; 
		 i++, pOffset += sizeof(UINT16))
	{
		iRequestDataOffset = UINT16_GET(pOffset);
		
		/* If the last offset */
		if ( i == (pRequestGroup->nNumberServices-1) )
			iRequestDataSize = (UINT16)(iDataSize - iRequestDataOffset);
		else
		{
			iNextRequestDataOffset = UINT16_GET((pOffset+sizeof(UINT16)));
			iRequestDataSize = (UINT16)(iNextRequestDataOffset - iRequestDataOffset);
		}

		routerParseObjectRequest(pSession->nSessionId, pSession->lClientIPAddr, &PacketTag, pRequest->cfg.objectCfg.pData + iRequestDataOffset, iRequestDataSize,
			pRequestGroup->nIndex, &otTag, &toTag, pRequestGroup->lContext1, pRequestGroup->lContext2);
	}

	/* remove the original request */
	requestRemove( pRequest );
}
#endif

/*---------------------------------------------------------------------------
** routerParseClassRequest( )
**
** Parse Message Router Class requests
**---------------------------------------------------------------------------
*/
static void routerParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		routerSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		routerSendClassAttrSingle( pRequest);
		break;
	default:		/* Do not support any other services */
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** routerParseInstanceRequest( )
**
** Parse Message Router Instance requests
**---------------------------------------------------------------------------
*/

static void routerParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	/* Instance 1 is the only instance supported */
	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		routerSendInstanceAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		routerSendInstanceAttrSingle( pRequest, iInstance );
		break;
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
	case SVC_MULTIPLE_SVC_REQUEST:
		routerProcessMultipleServiceRequest( pRequest);
		break;
#endif
	default:	/* Do not support any other services */
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** routerParseClassInstanceRqst( )
**
** Parse Message Router Class and Instance requests
**---------------------------------------------------------------------------
*/
static void routerParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ( iTagSize == 0 )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if ( iInstance == 0 )
		routerParseClassRequest( pRequest );
	else
		routerParseInstanceRequest( pRequest, iInstance );
}

/*---------------------------------------------------------------------------
** routerProcessObjectRequest( )
**
** Process UCMM or Class 3 request
**---------------------------------------------------------------------------
*/
void routerProcessObjectRequest( REQUEST* pRequest )
{
	INT32 nIndex;
	UINT32 *p;
	EtIPObjectRegisterType *pObject;
	UINT16 iTagSize;
	UINT32 iClass, iInstance, iAttribute, iMember;
#ifndef EIP_NO_CIP_ROUTING
	UINT32 lPort, iSlot;
	PDU_HDR pdu_hdr;
#endif
	CONNECTION* pConnection = NULL;
	BOOL bInternalMsg = TRUE, bHandled = FALSE;

	/* Ensure valid request type */
	switch(pRequest->nType)
	{
	case OBJECT_REQUEST:
	case CLASS3_REQUEST:
		break;
	default:
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
								"routerProcessObjectRequest INTERNAL ERROR: Unsupported request type %d", pRequest->nType);
		return;
	}

	if (pRequest->nType == CLASS3_REQUEST)
	{
		pConnection = connectionGetFromSerialNumber( pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );
		if (pConnection != NULL)
		{
#ifndef EIP_NO_CIP_ROUTING
			if (gbBackplaneSupport)
			{
				lPort = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_PORT, 0, &iTagSize );
				if (iTagSize > 0)
				{
					if (lPort != TCP_IP_BACKPLANE_PORT_NUMBER)
					{
						DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
								"routerProcessObjectRequest INTERNAL ERROR: Class3 connections only support the backplane port");
					}
					else
					{
						iSlot = (UINT32)pduGetTagByType( &pConnection->cfg.PDU, TAG_TYPE_SLOT, 0, &iTagSize );
						if (iTagSize > 0)
						{
							if (iSlot != giBackplaneSlot)
							{
								/* Class3 message for one of the slots, format the request with the correct path information */
								pRequest->iExtendedPathSize = (UINT16)(pduGetBuildLen(&pConnection->cfg.PDU)+PDU_HDR_SIZE);
								pRequest->pExtendedPath = EtIP_malloc(NULL, pRequest->iExtendedPathSize);
								pdu_hdr.bSize = (UINT8)(((pRequest->iExtendedPathSize-PDU_HDR_SIZE)/2));
								memcpy(pRequest->pExtendedPath, &pdu_hdr, PDU_HDR_SIZE);
								pduBuild(&pConnection->cfg.PDU, pRequest->pExtendedPath+PDU_HDR_SIZE);

								pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
								notifyEvent(NM_BACKPLANE_REQUEST_RECEIVED, pRequest->nIndex, 0 );
								return;
							}
						}
					}
				}
			}
#endif
#ifdef ET_IP_MODBUS

			/* We have a Modbus connection message */
			if (pConnection->cfg.modbusCfg.iModbusAddressSize > 0)
			{
				eipmbsParseCIPModbusRequest(pRequest);
				return;
			}
#endif
		}
	}

	iClass = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_CLASS, 0, &iTagSize);
	if ( iTagSize )
	{
		for ( p = gpClientAppProcClasses; p < &gpClientAppProcClasses[glClientAppProcClassNbr]; p++ )
		{
			if ( *p == iClass )
			{
				pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
				notifyEvent( NM_CLIENT_OBJECT_REQUEST_RECEIVED, pRequest->nIndex, 0 );
				return;
			}
		}

		/* if its not a whole class, check for specific handlers */
		iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
		if (iTagSize == 0)
			iInstance = INVALID_INSTANCE;
		iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
		if (iTagSize == 0)
			iAttribute = INVALID_ATTRIBUTE;
		iMember = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize);
		if (iTagSize == 0)
			iMember = INVALID_MEMBER;

		for ( pObject = gpClientAppProcObjects; pObject < &gpClientAppProcObjects[glClientAppProcObjNbr]; pObject++ )
		{
			if (((pObject->bService == pRequest->cfg.objectCfg.bService) || (pObject->bService == INVALID_SERVICE)) &&
				(pObject->iClass == iClass) &&
				(pObject->iInstance == iInstance) &&
				((pObject->iAttribute == iAttribute) || (pObject->iAttribute == 0) ||
				 (pObject->iAttribute == INVALID_ATTRIBUTE) || (pObject->bService == INVALID_SERVICE)) &&
				((pObject->iMember == iMember) || ((pObject->iAttribute == 0) && (pObject->iMember == INVALID_MEMBER)) ||
				(pObject->bService == INVALID_SERVICE)))
			{
				pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
				notifyEvent( NM_CLIENT_OBJECT_REQUEST_RECEIVED, pRequest->nIndex, 0 );
				return;
			}
		}

	}
	else
	{
		for ( p = gpClientAppProcServices; p < &gpClientAppProcServices[glClientAppProcServiceNbr]; p++ )
		{
			if (*p == pRequest->cfg.objectCfg.bService )
			{
				pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
				notifyEvent( NM_CLIENT_OBJECT_REQUEST_RECEIVED, pRequest->nIndex, 0 );
				return;
			}
		}
	}

	nIndex = pRequest->nIndex;

	/* If it was a "service only" request, there is no need to parse an object */
	if (!bHandled)
	{
		/* Direct the request to the appropriate object - this is what Message Router does */
		switch( iClass )
		{
		case ID_CLASS:
			idParseClassInstanceRequest( pRequest );
			break;
		case ROUTER_CLASS:
			routerParseClassInstanceRqst( pRequest );
			break;
		case ASSEMBLY_CLASS:
			assemblyParseClassInstanceRqst( pRequest );
			break;
		case CONNMGR_CLASS:
			bInternalMsg = connmgrParseClassInstanceRqst( pRequest );
			break;
#ifdef EIP_PCCC_SUPPORT
		case PCCC_CLASS:
			bInternalMsg = pcccParseClassInstanceRqst( pRequest );
			break;
#endif
#ifdef EIP_CIPSAFETY
		case SA_SUP_CLASS:
			safSuperParseClassInstanceRqst( pRequest );
			break;
		case SA_VAL_CLASS:
			safValidParseClassInstanceRqst( pRequest );
			break;
#endif
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		case CONFIG_CLASS:
			configParseClassInstanceRequest( pRequest );
			break;
#endif /* #ifdef ET_IP_SCANNER */
#ifndef EIP_NO_CIP_ROUTING
		case PORT_CLASS:
			portParseClassInstanceRequest( pRequest );
			break;
#endif
		case TCPIP_CLASS:
			tcpipParseClassInstanceRequest( pRequest );
			break;
		case ENETLINK_CLASS:
			enetlinkParseClassInstanceRqst( pRequest );
			break;
#ifdef EIP_QOS
		case QOS_CLASS:
			qosParseClassInstanceRqst( pRequest );
			break;
#endif
#if defined EIP_FILE_OBJECT
		case FILE_CLASS:
			if (gbFileObjectEnabled)
			{
				eipFileParseClassInstanceRqst(pRequest);
			}
			else
			{
				EtIP_free(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize);
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			}
			break;
#endif

		default:		/* Do not support this class yet */
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			break;
		}
	}

	if (!bInternalMsg)	/* If message is being handled by the application, wait for the response */
		return;

	/* Check that we still have the request; it could have been already completely processed and removed */
	pRequest = requestGetByRequestId( nIndex );
	if ( pRequest == NULL )
		return;				/* Request has been removed while processing */

	/* Save the connected response in the connection object */
	if (pRequest->nType == CLASS3_REQUEST)
	{
		if ( pConnection != NULL )
		{
			EtIP_realloc( &pConnection->pResponseData, &pConnection->iResponseDataSize, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );
			pConnection->bGeneralStatus = pRequest->bGeneralError;    /* Store general status. Should be 0 in case of success */
			pConnection->iExtendedStatus = pRequest->iExtendedError;    /* Store extended status. */
		}
	}

	pRequest->nState = REQUEST_RESPONSE_RECEIVED;
}
