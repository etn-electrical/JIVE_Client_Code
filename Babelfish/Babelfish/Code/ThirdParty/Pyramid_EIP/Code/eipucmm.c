/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPUCMM.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Services unconnected messages requests and repsonses
**
*****************************************************************************
*/

#include "eipinc.h"
#include "Trusted_IP_Filter_C_Connector.h"

/* Response data for the List Services command */
static ENCAP_OBJ_SERVICE list_services_data = {{ENCAP_TO_PS( ENCAP_OBJTYPE_SERVICE_COMM ), 
												ENCAP_TO_PS( ENCAP_SERVICE_SIZE )},
												{ENCAP_TO_PS( ENCAP_COMMSERVICE_VERSION ),
												ENCAP_TO_PS( ENCAP_COMMSERVICE_CIP_DIRECT | ENCAP_COMMSERVICE_IO_CONNECT ),
												ENCAP_COMMSERVICE_NAME}};

static ENCAP_RC_DATA register_data = {
	ENCAP_TO_PS( ENCAP_PROTOCOL_VERSION ),
	ENCAP_TO_PS( 0 )
};

static EtIPListIdentityInfo* gpnListIdentities;					/* Number outstanding List Identity responses*/
#ifdef EIP_LARGE_BUF_ON_HEAP
static EtIPListIdentityInfo* gListIdentities;					/* List Identity responses array */
#else
static EtIPListIdentityInfo  gListIdentities[MAX_SESSIONS];		/* List Identity responses array */
#endif
static INT32 gnListIdentityIndex;

/*---------------------------------------------------------------------------
** ucmmInit( )
**
** Initialize broadcast response arrays
**---------------------------------------------------------------------------
*/
void ucmmInit()
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	gListIdentities = (EtIPListIdentityInfo *)EtIP_mallocHeap(MAX_SESSIONS * sizeof(EtIPListIdentityInfo ));
	if(gListIdentities == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gListIdentities to %d entries\n", MAX_SESSIONS);
		return;
	}
#endif

	gpnListIdentities = gListIdentities;
	gnListIdentityIndex = LIST_IDENTITY_INDEX_BASE;
}

/*---------------------------------------------------------------------------
** ucmmClearListIdentities( )
**
** Resets number of List Identity responses from a broadcast
**---------------------------------------------------------------------------
*/
void ucmmClearListIdentities(UINT32 lHostAddress)
{
	EtIPListIdentityInfo* pInfo;

	for (pInfo = gListIdentities; pInfo < gpnListIdentities;)
	{
		if (pInfo->lHostAddress == lHostAddress)
		{
			ucmmRemoveListIdentity(pInfo);
		}
		else
		{
			pInfo++;
		}
	}
}

/*---------------------------------------------------------------------------
** ucmmRemoveListIdentity( )
**
** Get Identity Identity response by ID
**---------------------------------------------------------------------------
*/
EtIPListIdentityInfo* ucmmGetByListIdentityId(INT32 nId)
{
	EtIPListIdentityInfo* pInfo;

	for (pInfo = gListIdentities; pInfo < gpnListIdentities; pInfo++)
	{
		if (pInfo->lID == nId)
			return pInfo;
	}

	return NULL;
}


/*---------------------------------------------------------------------------
** ucmmRemoveListIdentity( )
**
** Remove the request from the array
**---------------------------------------------------------------------------
*/
void ucmmRemoveListIdentity(EtIPListIdentityInfo* pInfo)
{
	EtIPListIdentityInfo* pLoopInfo;

	gpnListIdentities--;

	/* Shift the requests with the bigger index to fill in the void */
	for( pLoopInfo = pInfo; pLoopInfo < gpnListIdentities; pLoopInfo++ )
		memcpy( pLoopInfo, pLoopInfo+1, sizeof(EtIPListIdentityInfo) );
}



#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** ucmmIssueRegisterSession( )
**
** Open a new session with a server by issuing a Register Session request
**---------------------------------------------------------------------------
*/
BOOL ucmmIssueRegisterSession( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	ENCAP_RC_DATA* rc = (ENCAP_RC_DATA*)&gmsgBuf[ENCAPH_SIZE];

	/* Store the new packet in the gmsgBuf buffer */
	memset( gmsgBuf, 0, (ENCAPH_SIZE + ENCAP_RC_DATA_SIZE) );

	pHdr->iCommand = ENCAP_CMD_REGISTERSESSION;
	pHdr->iLength = ENCAP_RC_DATA_SIZE;

	rc->iRc_version = ENCAP_TO_HS(pSession->iClientEncapProtocolVersion);
	rc->iRc_flags = ENCAP_TO_HS(pSession->iClientEncapProtocolFlags);

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"Sending ENCAP_CMD_REGISTERSESSION cmd on session %d", pSession->nSessionId);

	return socketEncapSendData( pSession );
}

/*---------------------------------------------------------------------------
** ucmmParseRegisterSessionResp( )
**
** Parse a response to our Register Session request.
** Return FALSE if pSession is removed. 
**---------------------------------------------------------------------------
*/
static BOOL ucmmParseRegisterSessionResp( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	ENCAP_RC_DATA* rc = (ENCAP_RC_DATA*)&gmsgBuf[ENCAPH_SIZE];

	if ( pHdr->iLength != ENCAP_RC_DATA_SIZE )
	{
		sessionRemove( pSession, FALSE );
		return FALSE;
	}

	/* Extract and store Session ID for future use */
	pSession->lSessionTag = pHdr->lSession;

	pSession->iClientEncapProtocolVersion = ENCAP_TO_HS( rc->iRc_version );
	pSession->iClientEncapProtocolFlags = ENCAP_TO_HS( rc->iRc_flags );
	pSession->lState = OpenSessionEstablished;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
				"Received good ENCAP_CMD_REGISTERSESSION response on session %d",
				pSession->nSessionId);

	return TRUE;
}
#endif

/*---------------------------------------------------------------------------
** ucmmRespondRegisterSession( )
**
** Respond to the incoming Register Session request. Act as a server.
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
static BOOL ucmmRespondRegisterSession( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];
	ENCAP_RC_DATA* rc = (ENCAP_RC_DATA*)pData;
	UINT16 iVersion, iFlags;
	BOOL bRet = TRUE;

	if ( pHdr->iLength != ENCAP_RC_DATA_SIZE )			/* Invalid header data */
		return ucmmErrorReply( pSession, ENCAP_E_BAD_LENGTH, 0 );

	iVersion = ENCAP_TO_HS( rc->iRc_version );
	iFlags = ENCAP_TO_HS( rc->iRc_flags );

	memcpy( pData, &register_data, sizeof( register_data ) );

	/* Verify that versions of the protocol match and
		that server can support the requested capabilities */
	if (( iVersion != ENCAP_PROTOCOL_VERSION ) ||
		( iFlags != 0 ))
	{
		return ucmmErrorReply( pSession, ENCAP_E_UNSUPPORTED_REV, sizeof(register_data));
	}

	/* Check that status is 0 */
	if ( pHdr->lStatus == ENCAP_E_SUCCESS )
	{
		pHdr->iLength = sizeof(register_data);

		/* Only 1 Register Session per socket is allowed */
		if ( pSession->lState == OpenSessionEstablished )
			return ucmmErrorReply( pSession, ENCAP_E_UNHANDLED_CMD, sizeof(register_data));

		pSession->lSessionTag = utilGetUniqueID(); /* Choose Session Tag */
		pHdr->lSession = pSession->lSessionTag;

		bRet = socketEncapSendData( pSession );

		pSession->lState = OpenSessionEstablished;
	}

	return bRet;
}


/*---------------------------------------------------------------------------
** ucmmIssueUnRegisterSession( )
**
** Terminate a session with the server by issuing Unregister Session request
**---------------------------------------------------------------------------
*/
void ucmmIssueUnRegisterSession( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_UNREGISTERSESSION;
	pHdr->lSession = pSession->lSessionTag;

	DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
		"Sending ENCAP_CMD_UNREGISTERSESSION cmd on session %d", pSession->nSessionId);

	socketEncapSendData( pSession );
}


/*---------------------------------------------------------------------------
** ucmmRespondUnRegisterSession( )
**
** Received Unregister Session request. Cleanup appropriate session object.
**---------------------------------------------------------------------------
*/
static void ucmmRespondUnRegisterSession( SESSION* pSession )
{
	sessionRemove( pSession, FALSE );
}

/*---------------------------------------------------------------------------
** ucmmRespondListServices( )
**
** Respond to the List Services request
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmRespondListServices( SESSION* pSession )
{
	ENCAPH*  pHdr = ( ENCAPH * )gmsgBuf;
	UINT8*   pData = &gmsgBuf[ENCAPH_SIZE];

	if ( pHdr->iLength != 0 )		/* Invalid header data */
	{
		/* pSession will be NULL for broadcasts */
		if ( pSession != NULL )
			return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
	}

	/* Check that status is 0 */
	if ( pHdr->lStatus == ENCAP_E_SUCCESS )
	{
		pHdr->iLength = sizeof(UINT16) + sizeof(list_services_data);

		UINT16_SET(pData, 1);		/* object count */

		pData += sizeof(UINT16);
		memcpy( pData, &list_services_data, sizeof( list_services_data ) );

		if ( pSession != NULL )
			return socketEncapSendData( pSession );
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmRespondListInterfaces( )
**
** Respond to the List Interfaces request
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmRespondListInterfaces( SESSION* pSession )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	UINT8* pData= &gmsgBuf[ENCAPH_SIZE];

	if ( pHdr->iLength != 0 )
	{
		/* Invalid header data */
		if ( pSession != NULL )
			return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
	}

	if ( pHdr->lStatus == ENCAP_E_SUCCESS )
	{
		pHdr->iLength = sizeof(UINT16);

		UINT16_SET(pData, 0);				/* object count */

		if ( pSession != NULL )
			return socketEncapSendData( pSession );
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmRespondListIdentity( )
**
** Respond to the List Identity reaquest
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmRespondListIdentity( SESSION* pSession, UINT32 lHostIPAddr,
								struct sockaddr_in* pSrcAddr, UINT16 EncapSession)
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];
	UINT16 iItemLen;
	UINT8 bProductNameSize = (UINT8)strlen((char*)gIDInfo[0].productName);		/* Product name size */
	struct sockaddr_in Addr;
	UINT16 iPort;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpip;

	if ((pSession != NULL) && (pHdr->iLength != 0))				/* Invalid header data */
		return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );

	iItemLen = (UINT16)( sizeof(UINT16) +			/* Protocol Version */
						CPF_TAG_SOCKADDR_SIZE +		/* Socket address */
						sizeof(UINT16) +			/* Vendor ID */
						sizeof(UINT16) +			/* Product Type */
						sizeof(UINT16) +			/* Product Code */
						sizeof(UINT16) +			/* Revision */
						sizeof(UINT16) +			/* Device Status */
						sizeof(UINT32) +			/* Serial Number */
						sizeof(UINT8) +				/* Product name size */
						bProductNameSize +			/* Product name */
						sizeof(UINT8) );			/* Target State */

	pHdr->iLength = (UINT16)( sizeof(UINT16) +		/* Object Count size */
					sizeof(UINT16) +				/* Target code size */
					sizeof(UINT16) +				/* Target length size */
					iItemLen );						/* Target data size */

	/* Object count */
	UINT16_SET(pData, 1);
	pData += sizeof(UINT16);

	UINT16_SET(pData, IDENTITY_CIP_TARGET);			/* Target code */
	pData += sizeof(UINT16);

	UINT16_SET(pData, iItemLen);					/* Target length */
	pData += sizeof(UINT16);

	UINT16_SET(pData, ENCAP_PROTOCOL_VERSION);		/* Protocol Version */
	pData += sizeof(UINT16);

	pTcpip = tcpipFindClaimedHostAddress( lHostIPAddr );
	if (pTcpip == NULL)
	{
		/* Shouldn't happen */
		if (pSession != NULL)
			return ucmmErrorReply( pSession, ENCAP_E_BADDATA, 0 );
		else
			return TRUE;
	}

	iPort = pTcpip->iPort;

	platformSetSockaddrIn( &Addr, iPort, lHostIPAddr );

	platformCopySockAddrInToBuffer(pData, &Addr);	/* Socket address */
	pData += CPF_TAG_SOCKADDR_SIZE;

	UINT16_SET(pData, gIDInfo[0].iVendor );
	pData += sizeof(UINT16);

	UINT16_SET(pData, gIDInfo[0].iProductType);
	pData += sizeof(UINT16);

	UINT16_SET(pData, gIDInfo[0].iProductCode);
	pData += sizeof(UINT16);

	*pData++ = gIDInfo[0].bMajorRevision;
	*pData++ = gIDInfo[0].bMinorRevision;

	UINT16_SET(pData, gIDInfo[0].iStatus);
	pData += sizeof(UINT16); 

	UINT32_SET(pData, gIDInfo[0].lSerialNumber);
	pData += sizeof(UINT32);

	*pData++ = bProductNameSize;

	memcpy( pData, gIDInfo[0].productName, bProductNameSize );
	pData += bProductNameSize;

	*pData++ = OPERATIONAL_DEVICE_STATE;			/* Device State */

	if ( pSession != NULL )
		return socketEncapSendData( pSession );

	/* This is a broadcast request, so make sure the encapsulation
	 * header is properly set up
	 */
	pHdr->iCommand = ENCAP_CMD_LISTIDENTITY;
	pHdr->lSession = EncapSession;
	pHdr->lStatus = 0;
	pHdr->lContext1 = 0;
	pHdr->lContext2 = 0;
	pHdr->lOpt = 0;

	socketSendBroadcast(pHdr, pTcpip, pSrcAddr);
	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmParseListIdentityResp( )
**
** Parse and process List Identity broadcase response.
**---------------------------------------------------------------------------
*/
void ucmmParseListIdentityResp(INT32 lBytesReceived, UINT32 lSrcAddress, UINT32 lHostAddress)
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];
	UINT8 bProductNameSize;
	UINT16 i, iNumberOfObjects, iTargetCode, iObjectLength, iProtocolVersion;
	struct sockaddr_in sTmpAddr;
	EtIPListIdentityInfo* pListIdInfo;

	if (pHdr->iLength != lBytesReceived-ENCAPH_SIZE)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
			"ENCAP_CMD_LISTIDENTITY response doesn't have correct size");

		return;
	}

	iNumberOfObjects = UINT16_GET(pData);
	pData += sizeof(UINT16);

	for (i = 0; i < iNumberOfObjects; i++)
	{
		iTargetCode = UINT16_GET(pData);
		pData += sizeof(UINT16);

		iObjectLength = UINT16_GET(pData);
		pData += sizeof(UINT16);

		if (iObjectLength+(pData-gmsgBuf) > lBytesReceived)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
				"ENCAP_CMD_LISTIDENTITY iObjectLength too big");
			return;
		}

		if (iTargetCode != IDENTITY_CIP_TARGET)
		{
			/* skip this object */
			pData += iObjectLength;
			continue;
		}

		if (iObjectLength < (UINT16)(sizeof(UINT16) +		/* Protocol Version */
								CPF_TAG_SOCKADDR_SIZE +		/* Socket address */
								sizeof(UINT16) +			/* Vendor ID */
								sizeof(UINT16) +			/* Product Type */
								sizeof(UINT16) +			/* Product Code */
								sizeof(UINT16) +			/* Revision */
								sizeof(UINT16) +			/* Device Status */
								sizeof(UINT32) +			/* Serial Number */
								sizeof(UINT8)))				/* Product name size */
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
				"ENCAP_CMD_LISTIDENTITY iObjectLength too small for identity information");
		}

		iProtocolVersion = UINT16_GET(pData);

		if (iProtocolVersion != ENCAP_PROTOCOL_VERSION)
		{
			pData += iObjectLength;
			continue;
		}

		pData += sizeof(UINT16);

		if (gpnListIdentities >= &gListIdentities[MAX_SESSIONS])
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
				"ENCAP_CMD_LISTIDENTITY too many outstanding List Identity responses");
			return;
		}

		/* start filling out the the List Identity Response to send to the application */
		pListIdInfo = gpnListIdentities;

		pListIdInfo->lID = gnListIdentityIndex++;
		pListIdInfo->lHostAddress = lHostAddress;

		platformCopySockAddrInFromBuffer(&sTmpAddr, pData);
		pListIdInfo->lIPAddress = platformGetInternetAddress(sTmpAddr.sin_addr);
		pData += CPF_TAG_SOCKADDR_SIZE;

		pListIdInfo->deviceId.iVendorID = UINT16_GET(pData);
		pData += sizeof(UINT16);
		pListIdInfo->deviceId.iProductType = UINT16_GET(pData);
		pData += sizeof(UINT16);
		pListIdInfo->deviceId.iProductCode = UINT16_GET(pData);
		pData += sizeof(UINT16);
		pListIdInfo->deviceId.bMajorRevision = *pData++;
		pListIdInfo->deviceId.bMinorRevision = *pData++;
		pListIdInfo->iStatus = UINT16_GET(pData);
		pData += sizeof(UINT16);
		pListIdInfo->lSerialNumber = UINT32_GET(pData);
		pData += sizeof(UINT32);

		bProductNameSize = *pData++;

		if ((bProductNameSize > MAX_PRODUCT_NAME_SIZE-1) ||
			(bProductNameSize+(pData-gmsgBuf) > lBytesReceived))
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
				"ENCAP_CMD_LISTIDENTITY bProductNameSize too big");
			return;
		}

		memcpy(pListIdInfo->productName, pData, bProductNameSize);
		pListIdInfo->productName[bProductNameSize] = 0;
		pData += bProductNameSize;

		if (pData-gmsgBuf >= lBytesReceived)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, lSrcAddress, lHostAddress,
				"ENCAP_CMD_LISTIDENTITY not enough data");
			return;

		}

		pListIdInfo->bTargetState = *pData++;

		/* successfully parsed object, let the application know */
		gpnListIdentities++;
		notifyEvent( NM_NEW_LIST_IDENTITY_RESPONSE, pListIdInfo->lID, 0 );
	}
}

/*---------------------------------------------------------------------------
** ucmmErrorReply( )
**
** Send encapsulated message response indicating an error.
** lStatus contains error code.
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmErrorReply( SESSION* pSession, UINT32 lStatus, UINT16 iLength )
{
	ENCAPH*   pHdr = ( ENCAPH * )gmsgBuf;

	pHdr->lStatus =  lStatus;
	pHdr->iLength = iLength;

	if (pSession != NULL)
		return socketEncapSendData( pSession );

	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmProcessEncapMsg( )
**
** Parse and process TCP packet.
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmProcessEncapMsg( SESSION* pSession )
{
	ENCAPH *psEncapHdr = ( ENCAPH * )gmsgBuf;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	SESSION* pLoopSession;
	UINT32 i;

	for( i = 0, pTcpipInterface = gsTcpIpInstAttr;
		 i < gnClaimedHostIPAddrCount;
		 i++, pTcpipInterface++)
	{
		if (pTcpipInterface->InterfaceConfig.lIpAddr == pSession->lHostIPAddr)
			break;
	}

	/*EATON_EIP_Changes:Commented as the TCP/IP preconformance test was failing.*/
	//if (i >= gnClaimedHostIPAddrCount)
	//{
		// Should not happen
		//return FALSE; 
	//}

	/* Discard No Operation commands */
	if ( psEncapHdr->iCommand == ENCAP_CMD_NOOP )
	{
		//pTcpipInterface->lNonCIPEncapCount++;
		return TRUE;
	}

	/* Discard messages with non-zero Encapsulation option field */
	if ( psEncapHdr->lOpt != 0)
	{
		//pTcpipInterface->lNonCIPEncapCount++;
		return TRUE;
	}

	switch ( psEncapHdr->iCommand )
	{
	/* List Services command */
	case ENCAP_CMD_LISTSERVICES:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_LISTSERVICES cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		return ucmmRespondListServices( pSession );

	/* List Interfaces command */
	case ENCAP_CMD_LISTINTERFACES:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_LISTINTERFACES cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		return ucmmRespondListInterfaces( pSession );

	/* List Identity command */
	case ENCAP_CMD_LISTIDENTITY:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_LISTIDENTITY cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		return ucmmRespondListIdentity( pSession, pSession->lHostIPAddr, NULL, 0);

	/* Register Session command */
	case ENCAP_CMD_REGISTERSESSION:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_REGISTERSESSION cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		/* Determine whether this is a request or a response */
#ifndef EIP_NO_ORIGINATE_UCMM
		if (pSession->bIncoming )
#endif
			return ucmmRespondRegisterSession( pSession );

#ifndef EIP_NO_ORIGINATE_UCMM
		return ucmmParseRegisterSessionResp( pSession );
#endif
	/* UnRegister Session command */
	case ENCAP_CMD_UNREGISTERSESSION:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_UNREGISTERSESSION cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		ucmmRespondUnRegisterSession( pSession );
		return FALSE;

	/* Indicate Status command received */
	case ENCAP_CMD_INDICATESTATUS:
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr,
			"encapProcessEncapMsg rcvd ENCAP_CMD_INDICATESTATUS cmd");
		//pTcpipInterface->lNonCIPEncapCount++;
		break;

	/* UCMM or Class 3 message received */
	case ENCAP_CMD_SEND_RRDATA:			/* UCMM */
	case ENCAP_CMD_SEND_UNITDATA:		/* Class 3 */
		if ( pSession->lSessionTag != psEncapHdr->lSession )
		{
			for( pLoopSession = gSessions; pLoopSession < gpnSessions; pLoopSession++ )
			{
				if ( pLoopSession->lSessionTag == psEncapHdr->lSession )
				{
					pSession = pLoopSession;
					break;
				}
			}
			/* If could not match session identifier - ignore */
			if ( pLoopSession == gpnSessions )
				return TRUE;
		}

		if (psEncapHdr->iCommand == ENCAP_CMD_SEND_RRDATA)
		{
			DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "encapProcessEncapMsg rcvd ENCAP_CMD_SEND_RRDATA cmd");
		}
		else if (psEncapHdr->iCommand == ENCAP_CMD_SEND_UNITDATA)
		{
			DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_SESSION, pSession->lHostIPAddr, pSession->lClientIPAddr, "encapProcessEncapMsg rcvd ENCAP_CMD_SEND_UNITDATA cmd");
		}
		return routerDataTransferReceived( pSession );

	/* Command not handled.  Return with error. */
	default:
		//pTcpipInterface->lNonCIPEncapCount++;
		return ucmmErrorReply( pSession, ENCAP_E_UNHANDLED_CMD, 0 );
	}

	return TRUE;
}

#ifdef ET_IP_SCANNER
/*---------------------------------------------------------------------------
** ucmmSendConnectedRequest( )
**
** Send Class 3 message
**---------------------------------------------------------------------------
*/
void ucmmSendConnectedRequest( CONNECTION* pConnection )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];			/* Store response packet in the gmsgBuf buffer */
	ENCAP_DT_HDR dataHdr;
	PDU_HDR pdu_hdr;
	UINT16 iPDUSize = (UINT16)pduGetBuildLen(&pConnection->requestCfg.PDU);		/* Request PDU */
	UINT16 iSize;
	SESSION* pSession = sessionGetBySessionId( pConnection->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendConnectedRequest Session terminated from under us: session Id %d", pConnection->nSessionId);
		return;
	}

#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		pConnection->lLastRcvdTick = gdwTickCount;
		if ( !pConnection->lStartSendTick )
			pConnection->lStartSendTick = gdwTickCount;

		pConnection->lTotalSendPackets++;
		pConnection->lSendPPSCount++;
	}
#endif /* CONNECTION_STATS */

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_UNITDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +				/* Required data header */
								sizeof(UINT16) +				/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +				/* Two tag headers */
								sizeof(UINT32) +				/* Class 3 Connection ID */
								DATA_SEQUENCE_COUNT_SIZE +		/* Connection sequence count */
								iPDUSize +						/* EPATH */
								pConnection->requestCfg.iDataSize );	/* Class 3 data length */

	if (pConnection->requestCfg.bNoService == FALSE) /* EPATH size + Service Code */
		pHdr->iLength += 2;

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );			/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, 2);									/* Add tag count */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_CONNECTED);		/* Indicate that this is Class 3 message */
	tag.iTag_length = ENCAP_TO_HS(sizeof(UINT32));

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	UINT32_SET( pData, pConnection->lProducingCID );		/* Supply connection ID */
	pData += sizeof(UINT32);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_CONNECTED);
	if (pConnection->requestCfg.bNoService == FALSE)
		iSize = (UINT16)(4 + iPDUSize + pConnection->requestCfg.iDataSize);
	else
		iSize = (UINT16)(2 + iPDUSize + pConnection->requestCfg.iDataSize);
	tag.iTag_length = ENCAP_TO_HS(iSize);

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	UINT16_SET( pData, pConnection->iOutDataSeqNbr );		/* Supply the sequence count */
	pData += DATA_SEQUENCE_COUNT_SIZE;

	if (pConnection->requestCfg.bNoService == FALSE)
	{
		pdu_hdr.bService = pConnection->requestCfg.bService; 
		pdu_hdr.bSize = (UINT8)(iPDUSize/2);				/* Path size in words */

		memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );			/* Add Protocol Data Unit */
		pData += PDU_HDR_SIZE;
	}
	else if (iPDUSize > 0)
	{
		(*pData) = (UINT8)(iPDUSize/2);				/* Path size in words */
		pData++;
	}

	pduBuild( &pConnection->requestCfg.PDU, pData );
	pData += iPDUSize;

	memcpy( pData, pConnection->requestCfg.pData, pConnection->requestCfg.iDataSize );

	DumpStr1(TRACE_LEVEL_DATA, TRACE_TYPE_CONNECTION, pConnection->cfg.lIPAddress, pConnection->cfg.lHostIPAddr,
					"Sending output data for connection %d", pConnection->cfg.nInstance);

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.systemStats.iNumSentClass3Messages++;
#endif
}
#endif
#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** ucmmSendUnconnectedRequest( )
**
** Send UCMM request
**---------------------------------------------------------------------------
*/
BOOL ucmmSendUnconnectedRequest( REQUEST* pRequest )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];			/* Store response packet in the gmsgBuf buffer */
	PDU_HDR pdu_hdr;
	ENCAP_DT_HDR dataHdr;
	UINT16 iPDUSize = (UINT16)pduGetBuildLen(&pRequest->cfg.objectCfg.PDU);		/* Request PDU */
	UINT16 iTagCount = 2;
	SESSION* pSession = sessionGetBySessionId( pRequest->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendUnconnectedRequest Session terminated from under us with session Id %d", pRequest->nSessionId);
		notifyEvent(NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0);
		requestRemove( pRequest );
		return FALSE;
	}

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequest->lContext1;
	pHdr->lContext2 = pRequest->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +			/* Required data header */
								sizeof(UINT16) +			/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +			/* Two tag headers */
								iPDUSize + 2 +				/* EPATH + size and Service*/
								pRequest->cfg.objectCfg.iDataSize );	/* UCMM data request length */

	if ((platformGetInternetAddress(pRequest->sOTTagAddr.sin_addr)) ||
		(pRequest->sOTTagAddr.sin_port))
	{
		iTagCount++;
		pHdr->iLength += (CPF_TAG_SIZE + CPF_TAG_SOCKADDR_SIZE);
	}

	if ((platformGetInternetAddress(pRequest->sTOTagAddr.sin_addr)) ||
		(pRequest->sTOTagAddr.sin_port))
	{
		iTagCount++;
		pHdr->iLength += (CPF_TAG_SIZE + CPF_TAG_SOCKADDR_SIZE);
	}

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );		/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);						/* Specify how many tags do follow */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);		/* Indicates local target */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);		/* UCMM tag header */
	tag.iTag_length = (UINT16)(ENCAP_TO_HS(2 + iPDUSize + pRequest->cfg.objectCfg.iDataSize));

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	pdu_hdr.bService = pRequest->cfg.objectCfg.bService; 
	pdu_hdr.bSize = (UINT8)(iPDUSize/2);				/* Path size in words */

	memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );			/* Add Protocol Data Unit */
	pData += PDU_HDR_SIZE;

	pduBuild( &pRequest->cfg.objectCfg.PDU, pData );
	pData += iPDUSize;

	memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );		/* Add request data if provided */
	pData += pRequest->cfg.objectCfg.iDataSize;

	if ((platformGetInternetAddress(pRequest->sOTTagAddr.sin_addr)) ||
		(pRequest->sOTTagAddr.sin_port))
	{
		tag.iTag_type = ENCAP_TO_HS(CPF_TAG_SOCKADDR_OT);
		tag.iTag_length = ENCAP_TO_HS(CPF_TAG_SOCKADDR_SIZE);

		memcpy(pData, &tag, CPF_TAG_SIZE);
		pData += CPF_TAG_SIZE;

		platformCopySockAddrInToBuffer(pData, &pRequest->sOTTagAddr);
		pData += CPF_TAG_SOCKADDR_SIZE;
	}

	if ((platformGetInternetAddress(pRequest->sTOTagAddr.sin_addr)) ||
		(pRequest->sTOTagAddr.sin_port))
	{
		tag.iTag_type = ENCAP_TO_HS(CPF_TAG_SOCKADDR_TO);
		tag.iTag_length = ENCAP_TO_HS(CPF_TAG_SOCKADDR_SIZE);

		memcpy(pData, &tag, CPF_TAG_SIZE);
		pData += CPF_TAG_SIZE;

		platformCopySockAddrInToBuffer(pData, &pRequest->sTOTagAddr);
		pData += CPF_TAG_SOCKADDR_SIZE;
	}

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif

	return TRUE;
}

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
/*---------------------------------------------------------------------------
** ucmmSendUnconnectedGroupRequest( )
**
** Send unconnected multiple service request
**---------------------------------------------------------------------------
*/
BOOL ucmmSendUnconnectedGroupRequest( REQUEST_GROUP* pRequestGroup )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	REQUEST* pRequest;
	UINT16 iServices = 0;
	UINT16 iOffset;
	CPF_TAG tag;
	UINT8* pData = &gmsgBuf[ENCAPH_SIZE];		/* Store response packet in the gmsgBuf buffer */
	PDU_HDR pdu_hdr;
	ENCAP_DT_HDR dataHdr;
	UINT16 iPDUSize = (UINT16)pduGetBuildLen(&pRequestGroup->cfg.objectCfg.PDU);		/* Request PDU */
	UINT16 iRequestPDUSize;
	UINT16 iTagCount = 2;
	UINT16 iTagSize;
	SESSION* pSession = sessionGetBySessionId( pRequestGroup->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendUnconnectedGroupRequest Session terminated from under us with session Id %d", pRequestGroup->nSessionId);

		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				notifyEvent(NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0);
				requestRemove( pRequest );
			}
		}
		return FALSE;
	}

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequestGroup->lContext1;
	pHdr->lContext2 = pRequestGroup->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +		/* Required data header */
								sizeof(UINT16) +		/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +		/* Two tag headers */
								2 +						/* Service + EPATH size */
								iPDUSize );				/* EPATH */

	iTagSize = (UINT16)(2 + iPDUSize);

	/* Service all requests */
	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
	{
		if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
		{
			iServices++;

			iRequestPDUSize = (UINT16)pduGetBuildLen(&pRequest->cfg.objectCfg.PDU);		/* Request PDU */

			pHdr->iLength += (2 + iRequestPDUSize + pRequest->cfg.objectCfg.iDataSize);
			iTagSize += (2 + iRequestPDUSize + pRequest->cfg.objectCfg.iDataSize);
		}
	}

	/* Check that the biggest tag does not exceed MAX_UCMM_SIZE bytes */
	if ( iTagSize > MAX_UCMM_SIZE )
	{
		/* Mark requests as response received with error Request Too Large */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				/* Free up the request data because there was an error */
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

				pRequest->bGeneralError = ROUTER_ERROR_REQUEST_TOO_LARGE;
				pRequest->iExtendedError = 0;
				requestResponseReceived(pRequest, NM_REQUEST_RESPONSE_RECEIVED);
			}
		}

		/* Remove the group, the requests will be removed when they are retrieved
		   by the application */
		requestGroupRemove( pRequestGroup );
		return FALSE;
	}

	/* Add 2 bytes of the service count and 2 bytes per offset */
	pHdr->iLength += 2 + 2*iServices;
	iTagSize += 2 + 2*iServices;

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );		/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);						/* Specify how many tags do follow */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);		/* Indicates local target */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);		/* UCMM tag header */
	tag.iTag_length = (UINT16)(ENCAP_TO_HS(iTagSize));

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	pdu_hdr.bService = pRequestGroup->cfg.objectCfg.bService;
	pdu_hdr.bSize = (UINT8)(iPDUSize/2);				/* Path size in words */

	memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );			/* Add Protocol Data Unit */
	pData += PDU_HDR_SIZE;

	pduBuild( &pRequestGroup->cfg.objectCfg.PDU, pData );
	pData += iPDUSize;

	UINT16_SET( pData, iServices );
	pData += 2;

	/* First reply offset is always right after the offset value array */
	iOffset = (UINT16)(2 + 2*iServices);

	UINT16_SET( pData, iOffset );
	pData += 2;

	/* Loop through all requests that belong to this group */
	for( pRequest = gRequests; pRequest < (gpnRequests-1); pRequest++ )
	{
		if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
		{
			iRequestPDUSize = (UINT16)pduGetBuildLen(&pRequest->cfg.objectCfg.PDU); 
			iOffset += (2 + iRequestPDUSize + pRequest->cfg.objectCfg.iDataSize);

			UINT16_SET( pData, iOffset );
			pData += 2;
		}
	}

	/* Now put the actual request pdu & data */
	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
	{
		if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
		{
			iRequestPDUSize = (UINT16)pduGetBuildLen(&pRequest->cfg.objectCfg.PDU); 
			pdu_hdr.bService = pRequest->cfg.objectCfg.bService; 
			pdu_hdr.bSize = (UINT8)(iRequestPDUSize/2);		/* Path size in words */

			memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );		/* Add Protocol Data Unit */
			pData += PDU_HDR_SIZE;

			pduBuild( &pRequest->cfg.objectCfg.PDU, pData );
			pData += iRequestPDUSize;

			memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );		/* Add request data if provided */
			pData += pRequest->cfg.objectCfg.iDataSize;
		}
	}

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif

	return TRUE;
}
#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT */
#endif /* #ifndef EIP_NO_ORIGINATE_UCMM */

/*---------------------------------------------------------------------------
** ucmmSendConnectedObjectResponse( )
**
** Send Class 3 message response
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
static BOOL ucmmSendConnectedObjectResponse( REQUEST* pRequest )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag = {0};
	UINT8* pData;
	ENCAP_DT_HDR dataHdr = {0};
	REPLY_HEADER replyHdr = {0};
	UINT16 iTagCount = 2, iTmpLength;
	CONNECTION* pConnection;
	SESSION* pSession = sessionGetBySessionId( pRequest->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendConnectedObjectResponse Session terminated from under us: session Id %d", pRequest->nSessionId);
		return TRUE;
	}

	pConnection = connectionGetFromSerialNumber( pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );

	if ( pConnection == NULL )
	{
		DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0, "ucmmSendConnectedObjectResponse Connection closed from under us: Connection Serial Number 0x%hx, Vendor Id 0x%hx, Device Serial Number 0x%x",
					pRequest->cfg.class3Cfg.iConnectionSerialNbr, pRequest->cfg.class3Cfg.iVendorID, pRequest->cfg.class3Cfg.lDeviceSerialNbr );
		return TRUE;
	}

	/* Make sure the response data can fit on the connection */
	if (pConnection->cfg.iCurrentProducingSize < pRequest->cfg.objectCfg.iDataSize + pRequest->iExtendedErrorDataSize)
	{
		pRequest->bGeneralError = ROUTER_ERROR_REPLY_DATA_TOO_LARGE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		EtIP_free( &pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize );
	}


#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		pConnection->lLastRcvdTick = 0;

		if ( !pConnection->lStartSendTick )
			pConnection->lStartSendTick = gdwTickCount;

		pConnection->lTotalSendPackets++;
		pConnection->lSendPPSCount++;
	}
#endif /* CONNECTION_STATS */

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_UNITDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequest->lContext1;
	pHdr->lContext2 = pRequest->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +						/* Required data header */
								sizeof(UINT16) +						/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +						/* Two tag headers */
								sizeof(UINT32) +						/* Class 3 Connection ID */
								DATA_SEQUENCE_COUNT_SIZE +				/* Connection sequence count */
								pRequest->cfg.objectCfg.iDataSize );	/* Class 3 data length */

	if (pConnection->requestCfg.bNoService == FALSE)
	{
		pHdr->iLength += REPLY_HEADER_SIZE;				/* Reply header */
		if (pRequest->bGeneralError != 0)
		{
			/* Extended status data is in word length */
			pHdr->iLength += ( (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize + (pRequest->iExtendedErrorDataSize%2));
		}
	}

	pData = &gmsgBuf[ENCAPH_SIZE];						/* Store response packet in the gmsgBuf buffer */

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );		/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);						/* Add tag count */
	pData += sizeof(iTagCount);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_CONNECTED);	/* Indicate that this is Class 3 message */
	tag.iTag_length = ENCAP_TO_HS(sizeof(UINT32));

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	UINT32_SET( pData, pConnection->lProducingCID );	/* Supply connection ID */
	pData += sizeof(UINT32);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_CONNECTED);
	if (pConnection->requestCfg.bNoService == FALSE)
	{
		iTmpLength = (UINT16)(sizeof(UINT16) + REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize +
								(pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize + (pRequest->iExtendedErrorDataSize%2));
		tag.iTag_length = ENCAP_TO_HS(iTmpLength);
	}
	else
		tag.iTag_length = (UINT16)(ENCAP_TO_HS(sizeof(UINT16) + pRequest->cfg.objectCfg.iDataSize));

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	UINT16_SET( pData, pRequest->cfg.class3Cfg.iConnSequence );			/* Supply the sequence count */
	pData += DATA_SEQUENCE_COUNT_SIZE;

	if (pConnection->requestCfg.bNoService == FALSE)
	{
		replyHdr.bService = (UINT8)(pRequest->cfg.objectCfg.bService | REPLY_BIT_MASK);		/* Populate Reply header */
		replyHdr.bReserved = 0;
		replyHdr.bGenStatus = pRequest->bGeneralError;					/* 0 indicates a success */
		replyHdr.bObjStatusSize = (UINT8)((pRequest->iExtendedError ? 1 : 0) + ((pRequest->iExtendedErrorDataSize+1)/2)); /* If there is an error provide extended error information */

		memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);					/* Add reply header */
		pData += REPLY_HEADER_SIZE;

		if (pRequest->bGeneralError != 0)
		{
			if (pRequest->iExtendedError)
			{
				UINT16_SET(pData, pRequest->iExtendedError);			/* Add extended error */
				pData += sizeof(UINT16);
			}

			memcpy(pData, pRequest->pExtendedErrorData, pRequest->iExtendedErrorDataSize);
			pData += pRequest->iExtendedErrorDataSize;

			/* Pad to word boundary */
			if (pRequest->iExtendedErrorDataSize%2)
			{
				*pData = 0;
				pData++;
			}
		}
	}

	memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );

	if (!socketEncapSendData( pSession ))
		return FALSE;

#ifdef CONNECTION_STATS
	gSystemStats.systemStats.iNumSentClass3Messages++;
#endif
	return TRUE;
}

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
/*---------------------------------------------------------------------------
** ucmmSendConnectedGroupResponse( )
**
** Send multiple service response over Class3 connection
**---------------------------------------------------------------------------
*/
void ucmmSendConnectedGroupResponse( REQUEST_GROUP* pRequestGroup )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	REQUEST* pRequest;
	UINT16 iServices = 0;
	UINT16 iOffset = 0;
	CPF_TAG tag = {0};
	UINT8* pData;
	ENCAP_DT_HDR dataHdr = {0};
	REPLY_HEADER replyHdr = {0};
	UINT16 iTagCount = 2;
	UINT16 iExtError;
	CONNECTION* pConnection;
	UINT16 iTagSize = 0, iDefaultLength;
	SESSION* pSession = sessionGetBySessionId( pRequestGroup->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendConnectedGroupResponse Session terminated from under us: session Id %d", pRequestGroup->nSessionId);
		return;
	}

	pConnection = connectionGetFromSerialNumber( pRequestGroup->cfg.class3Cfg.iConnectionSerialNbr, pRequestGroup->cfg.class3Cfg.iVendorID, pRequestGroup->cfg.class3Cfg.lDeviceSerialNbr );

	if ( pConnection == NULL )
	{
		DumpStr3(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0, "ucmmSendConnectedGroupResponse Connection closed from under us: Connection Serial Number 0x%hx, Vendor Id 0x%hx, Device Serial Number 0x%x",
					pRequestGroup->cfg.class3Cfg.iConnectionSerialNbr, pRequestGroup->cfg.class3Cfg.iVendorID, pRequestGroup->cfg.class3Cfg.lDeviceSerialNbr );
		return;
	}

#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		pConnection->lLastRcvdTick = 0;

		if ( !pConnection->lStartSendTick )
			pConnection->lStartSendTick = gdwTickCount;

		pConnection->lTotalSendPackets++;
		pConnection->lSendPPSCount++;
	}
#endif /* CONNECTION_STATS */

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_UNITDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequestGroup->lContext1;
	pHdr->lContext2 = pRequestGroup->lContext2;
	iDefaultLength = (UINT16)( ENCAP_DT_HDR_SIZE +		/* Required data header */
							sizeof(UINT16) +			/* Tag (or object) count */
							CPF_TAG_SIZE * 2 +			/* Two tag headers */
							sizeof(UINT32) +			/* Class 3 Connection ID */
							DATA_SEQUENCE_COUNT_SIZE +	/* Connection sequence count */
							REPLY_HEADER_SIZE );		/* Reply header size */
	pHdr->iLength = iDefaultLength;

	iTagSize = 2 + REPLY_HEADER_SIZE;

	if ((pRequestGroup->bGeneralError) && /* We have an error */
		(pRequestGroup->bGeneralError != ROUTER_ERROR_SERVICE_ERROR))
	{
		if (pRequestGroup->iExtendedError != 0)
		{
			iExtError = ENCAP_TO_HS(pRequestGroup->iExtendedError);
			EtIP_realloc( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize, (UINT8*)&iExtError, sizeof(UINT16) );

			if ( pRequestGroup->cfg.objectCfg.iDataSize && pRequestGroup->cfg.objectCfg.pData == NULL )
			{
				notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
				return;
			}

			pHdr->iLength += 2;
			iTagSize += 2;
		}
		else
		{
			EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize);
		}
	}
	else
	{
		/* Service all requests */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				iServices++;

				if (pRequest->bGeneralError)
				{

					/* Multiple Service Response must have error if any individual service has an error */
					pRequestGroup->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
				}

				pHdr->iLength += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize + (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize);
				iTagSize += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize + (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize);
			}
		}

		/* Add 2 bytes of the service count and 2 bytes per offset */
		pHdr->iLength += 2 + 2*iServices;
		iTagSize += 2 + 2*iServices;

		/* Check that the biggest tag does not exceed MAX_DATA_FIELD_SIZE bytes */
		if ( iTagSize > MAX_DATA_FIELD_SIZE )
		{
			/* Mark the entire message as too big */
			iTagSize = REPLY_HEADER_SIZE;
			pHdr->iLength = iDefaultLength;
			pRequestGroup->bGeneralError = ROUTER_ERROR_RESPONSE_TOO_LARGE;
			EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize);        

			/* Free the request data */
			for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
			{
				if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
				{
					EtIP_free(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize);
				}
			}
		}
	}

	pData = &gmsgBuf[ENCAPH_SIZE];

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );			/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);							/* Add tag count */
	pData += sizeof(iTagCount);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_CONNECTED);		/* Indicate that this is Class 3 message */
	tag.iTag_length = ENCAP_TO_HS(sizeof(UINT32));

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	UINT32_SET( pData, pConnection->lProducingCID );		/* Supply connection ID */
	pData += sizeof(UINT32);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_CONNECTED);
	tag.iTag_length = (UINT16)(ENCAP_TO_HS(iTagSize));

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	UINT16_SET( pData, pRequestGroup->cfg.class3Cfg.iConnSequence );		/* Supply the sequence count */
	pData += DATA_SEQUENCE_COUNT_SIZE;

	/* Add reply header that has the same service code as the UCMM request and uses
	   REPLY_BIT_MASK to indicate a response */
	replyHdr.bService = (UINT8)( SVC_MULTIPLE_SVC_REQUEST | REPLY_BIT_MASK);
	replyHdr.bReserved = 0;
	replyHdr.bGenStatus = pRequestGroup->bGeneralError;						/* 0 indicates a success */
	replyHdr.bObjStatusSize = (UINT8)( pRequestGroup->iExtendedError ? 1 : 0);	/* If there is an error provide one word long extended error information */

	memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);
	pData += REPLY_HEADER_SIZE;

	/* We have a error unrelated to an individual request */
	if ((pRequestGroup->bGeneralError) &&
		(pRequestGroup->bGeneralError != ROUTER_ERROR_SERVICE_ERROR))
	{
		memcpy( pData, pRequestGroup->cfg.objectCfg.pData, pRequestGroup->cfg.objectCfg.iDataSize );    /* Add UCMM data */
		pData += pRequestGroup->cfg.objectCfg.iDataSize;
	}
	else
	{
		UINT16_SET( pData, iServices );
		pData += sizeof(UINT16);

		/* First reply offset is always right after the offset value array */
		iOffset = (UINT16)(2 + 2*iServices);

		UINT16_SET( pData, iOffset );
		pData += sizeof(UINT16);

		/* Loop through all requests that belong to this group */
		for( pRequest = gRequests; pRequest < (gpnRequests-1); pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				iOffset += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize);

				UINT16_SET( pData, iOffset );
				pData += sizeof(UINT16);
			}
		}

		/* Now put the actual replies */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				/* Add reply header that has the same service code as the UCMM request and uses REPLY_BIT_MASK to indicate a response */
				replyHdr.bService = (UINT8)( pRequest->cfg.objectCfg.bService | REPLY_BIT_MASK);
				replyHdr.bReserved = 0;
				replyHdr.bGenStatus = pRequest->bGeneralError;							/* 0 indicates a success */
				replyHdr.bObjStatusSize = (UINT8)( pRequest->iExtendedError ? 1 : 0);	/* If there is an error provide one word long extended error information */

				memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);
				pData += REPLY_HEADER_SIZE;

				if (pRequest->iExtendedError)
				{
					UINT16_SET(pData, pRequest->iExtendedError);						/* Add extended error */
					pData += sizeof(UINT16);
				}

				memcpy(pData, pRequest->pExtendedErrorData, pRequest->iExtendedErrorDataSize);
				pData += pRequest->iExtendedErrorDataSize;

				/* Pad to word boundary */
				if (pRequest->iExtendedErrorDataSize%2)
				{
					*pData = 0;
					pData++;
				}

				memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );		/* Add reply data */
				pData += pRequest->cfg.objectCfg.iDataSize;
			}
		}
	}

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.systemStats.iNumSentClass3Messages++;
#endif
}
#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT	*/


/*---------------------------------------------------------------------------
** ucmmSendUnconnectedObjectResp( )
**
** Send UCMM response
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
static BOOL ucmmSendUnconnectedObjectResp( REQUEST* pRequest )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag = {0};
	UINT8* pData;
	ENCAP_DT_HDR dataHdr = {0};
	REPLY_HEADER replyHdr = {0};
	UINT16 iTagCount = 2, iTmpLength;
	SESSION* pSession = sessionGetBySessionId( pRequest->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendUnconnectedObjectResp Session terminated from under us: session Id %d", pRequest->nSessionId);
		return TRUE;
	}
	
	/* Trusted IP whitelist */
	if ( !Is_Remote_IP_Trusted( pRequest->lIPAddress, ENCAP_SERVER_PORT ) )
	{      
		return TRUE;
	}

	if (pRequest->cfg.objectCfg.iDataSize + pRequest->iExtendedErrorDataSize > MAX_UCMM_SIZE)
	{
		/* Request is too big for the send buffer */
		pRequest->bGeneralError = ROUTER_ERROR_REPLY_DATA_TOO_LARGE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		EtIP_free( &pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize );
	}

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequest->lContext1;
	pHdr->lContext2 = pRequest->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +			/* Required data header */
								sizeof(UINT16) +			/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +			/* Two tag headers */
								REPLY_HEADER_SIZE +			/* Reply header size */
								((pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize + (pRequest->iExtendedErrorDataSize%2)) +	/* Extended status data is in word length */
								pRequest->cfg.objectCfg.iDataSize );		/* UCMM data length */

	if ((platformGetInternetAddress(pRequest->sOTTagAddr.sin_addr)) ||
		(pRequest->sOTTagAddr.sin_port))
	{
		iTagCount++;
		pHdr->iLength += (CPF_TAG_SIZE + CPF_TAG_SOCKADDR_SIZE);
	}

	if ((platformGetInternetAddress(pRequest->sTOTagAddr.sin_addr)) ||
		(pRequest->sTOTagAddr.sin_port))
	{
		iTagCount++;
		pHdr->iLength += (CPF_TAG_SIZE + CPF_TAG_SOCKADDR_SIZE);
	}

	pData = &gmsgBuf[ENCAPH_SIZE];

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );	/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);					/* Add tag count */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);	/* Indicate that the target is local */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);	/* UCMM tag */
	iTmpLength = (UINT16)(REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize +
						  (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize + (pRequest->iExtendedErrorDataSize%2));
	tag.iTag_length = ENCAP_TO_HS(iTmpLength);

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	if (pRequest->cfg.objectCfg.bNoService == FALSE)
	{
		/* Add reply header that has the same service code as the UCMM request and uses
		   REPLY_BIT_MASK to indicate a response */
		replyHdr.bService = (UINT8)( pRequest->cfg.objectCfg.bService | REPLY_BIT_MASK);
		replyHdr.bReserved = 0;
		replyHdr.bGenStatus = pRequest->bGeneralError;		/* 0 indicates a success */
		replyHdr.bObjStatusSize = (UINT8)((pRequest->iExtendedError ? 1 : 0) + ((pRequest->iExtendedErrorDataSize+1)/2)); /* If there is an error provide extended error information */

		memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);
		pData += REPLY_HEADER_SIZE;

		if (pRequest->iExtendedError)
		{
			UINT16_SET(pData, pRequest->iExtendedError);	/* Add extended error */
			pData += sizeof(UINT16);
		}

		memcpy(pData, pRequest->pExtendedErrorData, pRequest->iExtendedErrorDataSize);
		pData += pRequest->iExtendedErrorDataSize;

		/* Pad to word boundary */
		if (pRequest->iExtendedErrorDataSize%2)
		{
			*pData = 0;
			pData++;
		}
	}

	memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );	/* Add UCMM data */
	pData += pRequest->cfg.objectCfg.iDataSize;

	if ((platformGetInternetAddress(pRequest->sOTTagAddr.sin_addr)) ||
		(pRequest->sOTTagAddr.sin_port))
	{
		tag.iTag_type = ENCAP_TO_HS(CPF_TAG_SOCKADDR_OT);
		tag.iTag_length = ENCAP_TO_HS(CPF_TAG_SOCKADDR_SIZE);

		memcpy(pData, &tag, CPF_TAG_SIZE);
		pData += CPF_TAG_SIZE;

		platformCopySockAddrInToBuffer(pData, &pRequest->sOTTagAddr);
		pData += CPF_TAG_SOCKADDR_SIZE;
	}

	if ((platformGetInternetAddress(pRequest->sTOTagAddr.sin_addr)) ||
		(pRequest->sTOTagAddr.sin_port))
	{
		tag.iTag_type = ENCAP_TO_HS(CPF_TAG_SOCKADDR_TO);
		tag.iTag_length = ENCAP_TO_HS(CPF_TAG_SOCKADDR_SIZE);

		memcpy(pData, &tag, CPF_TAG_SIZE);
		pData += CPF_TAG_SIZE;

		platformCopySockAddrInToBuffer(pData, &pRequest->sTOTagAddr);
		pData += CPF_TAG_SOCKADDR_SIZE;
	}

	if (!socketEncapSendData( pSession ))
		return FALSE;

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif

	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmSendObjectResponse( )
**
** Send UCMM or Class 3 message response
** Return FALSE if pSession is removed.
**---------------------------------------------------------------------------
*/
BOOL ucmmSendObjectResponse( REQUEST* pRequest )
{
	if (pRequest->nType == CLASS3_REQUEST)
		return ucmmSendConnectedObjectResponse( pRequest );
	else
		return ucmmSendUnconnectedObjectResp( pRequest );
}

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
/*---------------------------------------------------------------------------
** ucmmSendUnconnectedGroupResponse( )
**
** Send multiple service response
**---------------------------------------------------------------------------
*/
void ucmmSendUnconnectedGroupResponse( REQUEST_GROUP* pRequestGroup )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	REQUEST* pRequest;
	UINT16 iServices = 0, iOffset = 0, iTagCount = 2, iTagSize, iDefaultLength, iExtError;
	CPF_TAG tag = {0};
	UINT8* pData;
	ENCAP_DT_HDR dataHdr;
	REPLY_HEADER replyHdr = {0};
	SESSION* pSession = sessionGetBySessionId( pRequestGroup->nSessionId );

	if ( pSession == NULL )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, 0, 0, "ucmmSendUnconnectedGroupResponse Session terminated from under us: session Id %d", pRequestGroup->nSessionId);
		return;
	}

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequestGroup->lContext1;
	pHdr->lContext2 = pRequestGroup->lContext2;
	iDefaultLength = (UINT16)( ENCAP_DT_HDR_SIZE +		/* Required data header */
								sizeof(UINT16) +		/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +		/* Two tag headers */
								REPLY_HEADER_SIZE );	/* Reply header size */
	pHdr->iLength = iDefaultLength;
	iTagSize = REPLY_HEADER_SIZE;

	if ((pRequestGroup->bGeneralError) && /* We have an error */
		(pRequestGroup->bGeneralError != ROUTER_ERROR_SERVICE_ERROR))
	{
		if (pRequestGroup->iExtendedError != 0)
		{
			iExtError = ENCAP_TO_HS(pRequestGroup->iExtendedError);
			EtIP_realloc( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize, (UINT8*)&iExtError, sizeof(UINT16) );

			if ( pRequestGroup->cfg.objectCfg.iDataSize && pRequestGroup->cfg.objectCfg.pData == NULL )
			{
				notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
				return;
			}

			pHdr->iLength += 2;
			iTagSize += 2;
		}
		else
		{
			EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize);
		}
	}
	else
	{
		/* Service all requests */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				iServices++;

				if (pRequest->bGeneralError)
				{

					/* Multiple Service Response must have error if any individual service has an error */
					pRequestGroup->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
				}

				pHdr->iLength += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize + (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize);
				iTagSize += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize + (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize);
			}
		}

		/* Add 2 bytes of the service count and 2 bytes per offset */
		pHdr->iLength += 2 + 2*iServices;
		iTagSize += 2 + 2*iServices;

		/* Check that the biggest tag does not exceed MAX_UCMM_SIZE bytes */
		if ( iTagSize > MAX_UCMM_SIZE )
		{
			/* Mark the entire message as too big */
			iTagSize = REPLY_HEADER_SIZE;
			pHdr->iLength = iDefaultLength;
			pRequestGroup->bGeneralError = ROUTER_ERROR_RESPONSE_TOO_LARGE;
			EtIP_free( &pRequestGroup->cfg.objectCfg.pData, &pRequestGroup->cfg.objectCfg.iDataSize);

			/* Free the request data */
			for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
			{
				if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
				{
					EtIP_free(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize);
					EtIP_free(&pRequest->pExtendedErrorData, &pRequest->iExtendedErrorDataSize);
				}
			}
		}
	}

	pData = &gmsgBuf[ENCAPH_SIZE];

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );		/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, iTagCount);						/* Add tag count */
	pData += sizeof(iTagCount);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);		/* Indicate that the target is local */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);		/* UCMM tag */
	tag.iTag_length = (UINT16)(ENCAP_TO_HS(iTagSize));

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	/* Add reply header that has the same service code as the UCMM request and uses
	   REPLY_BIT_MASK to indicate a response */
	replyHdr.bService = (UINT8)( SVC_MULTIPLE_SVC_REQUEST | REPLY_BIT_MASK);
	replyHdr.bReserved = 0;
	replyHdr.bGenStatus = pRequestGroup->bGeneralError;		/* 0 indicates a success */
	replyHdr.bObjStatusSize = (UINT8)( pRequestGroup->iExtendedError ? 1 : 0);		/* If there is an error provide one word long extended error information */

	memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);
	pData += REPLY_HEADER_SIZE;

	if ((pRequestGroup->bGeneralError) &&				/* We have a error unrelated to an individual request */
		(pRequestGroup->bGeneralError != ROUTER_ERROR_SERVICE_ERROR))
	{
		memcpy( pData, pRequestGroup->cfg.objectCfg.pData, pRequestGroup->cfg.objectCfg.iDataSize );	/* Add UCMM data */
		pData += pRequestGroup->cfg.objectCfg.iDataSize;
	}
	else
	{
		UINT16_SET( pData, iServices );
		pData += sizeof(UINT16);

		/* First reply offset is always right after the offset value array */
		iOffset = (UINT16)(2 + 2*iServices);

		UINT16_SET( pData, iOffset );
		pData += sizeof(UINT16);

		/* Loop through all requests that belong to this group */
		for( pRequest = gRequests; pRequest < (gpnRequests-1); pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				iOffset += (REPLY_HEADER_SIZE + pRequest->cfg.objectCfg.iDataSize + (pRequest->iExtendedError ? 2 : 0) + pRequest->iExtendedErrorDataSize);

				UINT16_SET( pData, iOffset );
				pData += sizeof(UINT16);
			}
		}

		/* Now put the actual replies */
		for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
		{
			if ( pRequest->nRequestGroupIndex == pRequestGroup->nIndex )
			{
				/* Add reply header that has the same service code as the UCMM request and uses REPLY_BIT_MASK to indicate a response */
				replyHdr.bService = (UINT8)( pRequest->cfg.objectCfg.bService | REPLY_BIT_MASK);
				replyHdr.bReserved = 0;
				replyHdr.bGenStatus = pRequest->bGeneralError;				/* 0 indicates a success */
				replyHdr.bObjStatusSize = (UINT8)((pRequest->iExtendedError ? 1 : 0) + ((pRequest->iExtendedErrorDataSize+1)/2)); /* If there is an error provide extended error information */

				memcpy(pData, &replyHdr, REPLY_HEADER_SIZE);
				pData += REPLY_HEADER_SIZE;

				if (pRequest->iExtendedError)
				{
					UINT16_SET(pData, pRequest->iExtendedError);			/* Add extended error */
					pData += sizeof(UINT16);
				}

				memcpy(pData, pRequest->pExtendedErrorData, pRequest->iExtendedErrorDataSize);
				pData += pRequest->iExtendedErrorDataSize;

				/* Pad to word boundary */
				if (pRequest->iExtendedErrorDataSize%2)
				{
					*pData = 0;
					pData++;
				}

				memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );	/* Add UCMM data */
				pData += pRequest->cfg.objectCfg.iDataSize;
			}
		}
	}

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif
}
#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT */

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** ucmmPassUnconnectedSendRequest( )
**
** Send Unconnected Send request
**---------------------------------------------------------------------------
*/
BOOL ucmmPassUnconnectedSendRequest( REQUEST* pRequest )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag;
	UINT8* pData;
	PDU_HDR pdu_hdr;
	ENCAP_DT_HDR dataHdr;
	unsigned char unconnectedSendPath[] = { UNCONNECTED_SEND_CMD_CODE, 0x02,
											(UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT), CONNMGR_CLASS,
											(UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT), 0x01 };
	UINT16 iExtendedPathSize = pRequest->iExtendedPathSize;
	UINT16 iPDUSize = (UINT16)pduGetBuildLen(&pRequest->cfg.objectCfg.PDU);		/* Request PDU */
	UINT16 iSize;
	SESSION* pSession;

	pSession = sessionFindAddressEstablished( pRequest->lIPAddress, pRequest->lHostIPAddr, FALSE);

	if ( pSession == NULL )
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pRequest->lHostIPAddr, pRequest->lIPAddress, "ucmmPassUnconnectedSendRequest Session terminated from under us");
		if ( !pRequest->bIncoming )
			notifyEvent(NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0);
		requestRemove( pRequest );
		return FALSE;
	}

	if ( iExtendedPathSize % 2 )
		iExtendedPathSize++;

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequest->lContext1;
	pHdr->lContext2 = pRequest->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +			/* Required data header */
							sizeof(UINT16) +				/* Tag (or object) count */
							CPF_TAG_SIZE * 2 +				/* Two tag headers */
							sizeof(unconnectedSendPath)+	/* Unconnected Send path segment */
							sizeof(UINT16) +				/* Priority & tick time and number of ticks */
							sizeof(UINT16) +				/* The size of the embedded UCMM message */
							2 +								/* Service + EPATH size */
							iPDUSize +						/* EPATH */
							pRequest->cfg.objectCfg.iDataSize +		/* Data length */
							sizeof(UINT16) +				/* Extended path length */
							iExtendedPathSize );			/* Intermediate and final leg paths */

	if ( pRequest->cfg.objectCfg.iDataSize % 2 )			/* Count the pad byte */
		pHdr->iLength++;

	pData = &gmsgBuf[ENCAPH_SIZE];							/* Store message in the gmsgBuf buffer */

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	/* Add data header */
	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, 2);									/* Tag count = 2 */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);			/* Indicates local target */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);				/* UCMM tag header */
	tag.iTag_length = (UINT16)( sizeof(unconnectedSendPath)+	/* Unconnected Send path segment */
							sizeof(UINT16) +					/* Priority & tick time and number of ticks */
							sizeof(UINT16) +					/* The size of the embedded UCMM message */
							2 +									/* Service + EPATH size */
							iPDUSize +							/* EPATH */
							pRequest->cfg.objectCfg.iDataSize +	/* Data length */
							sizeof(UINT16) +					/* Extended path length */
							iExtendedPathSize );				/* Intermediate and final leg paths */

	if (pRequest->cfg.objectCfg.iDataSize % 2 )					/* Count the pad byte */
		tag.iTag_length++;

	ENCAP_CVT_HS(tag.iTag_length);

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	memcpy( pData, unconnectedSendPath, sizeof(unconnectedSendPath) );
	pData += sizeof(unconnectedSendPath);

	*pData++ = pRequest->bOpenPriorityTickTime;
	*pData++ = pRequest->bOpenTimeoutTicks;

	iSize = (UINT16)(2 + iPDUSize + pRequest->cfg.objectCfg.iDataSize);
	UINT16_SET(pData, iSize);
	pData += sizeof(UINT16);

	pdu_hdr.bService = pRequest->cfg.objectCfg.bService;
	pdu_hdr.bSize = (UINT8)(iPDUSize/2);				/* Path size in words */

	memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );			/* Add Protocol Data Unit */
	pData += PDU_HDR_SIZE;

	pduBuild( &pRequest->cfg.objectCfg.PDU, pData );
	pData += iPDUSize;

	if ( pRequest->cfg.objectCfg.iDataSize )
	{
		/* Add request data if provided */
		memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );
		pData += pRequest->cfg.objectCfg.iDataSize;

		/* Insert the pad byte if data length is odd */
		if ( pRequest->cfg.objectCfg.iDataSize % 2 )
			*pData++ = 0;
	}

	*pData++ = (UINT8)(iExtendedPathSize/2);
	*pData++ = 0;

	memcpy( pData, pRequest->pExtendedPath, pRequest->iExtendedPathSize);
	pData += pRequest->iExtendedPathSize;

	/* Put the pad byte in if necessary */
	if ( pRequest->iExtendedPathSize % 2 )
		*pData = 0;

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif

	return TRUE;
}

/*---------------------------------------------------------------------------
** ucmmPassUncnctSendRqstFinalLeg( )
**
** Send the final leg of the Unconnected Send request as a UCMM message
**---------------------------------------------------------------------------
*/
BOOL ucmmPassUncnctSendRqstFinalLeg( REQUEST* pRequest )
{
	ENCAPH* pHdr = ( ENCAPH * )gmsgBuf;
	CPF_TAG tag;
	UINT8* pData;
	PDU_HDR pdu_hdr;
	ENCAP_DT_HDR dataHdr;
	UINT16 iPDUSize = (UINT16)(pduGetBuildLen(&pRequest->cfg.objectCfg.PDU)); /* Request PDU */
	UINT16 iSize;
	SESSION* pSession;

	pSession = sessionFindAddressEstablished(pRequest->lIPAddress, pRequest->lHostIPAddr, FALSE);

	if ( pSession == NULL )
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_SESSION, pRequest->lHostIPAddr, pRequest->lIPAddress, "ucmmPassUncnctSendRqstFinalLeg Session terminated from under us");
		if ( !pRequest->bIncoming )
			notifyEvent(NM_REQUEST_FAILED_INVALID_NETWORK_PATH, pRequest->nIndex, 0);
		requestRemove( pRequest );
		return FALSE;
	}

	memset( pHdr, 0, ENCAPH_SIZE );

	pHdr->iCommand = ENCAP_CMD_SEND_RRDATA;
	pHdr->lSession = pSession->lSessionTag;
	pHdr->lContext1 = pRequest->lContext1;
	pHdr->lContext2 = pRequest->lContext2;
	pHdr->iLength = (UINT16)( ENCAP_DT_HDR_SIZE +		/* Required data header */
								sizeof(UINT16) +		/* Tag (or object) count */
								CPF_TAG_SIZE * 2 +		/* Two tag headers */
								2 +						/* Service + EPATH size */
								iPDUSize +				/* EPATH */
								pRequest->cfg.objectCfg.iDataSize );	/* UCMM data request length */

	pData = &gmsgBuf[ENCAPH_SIZE];					/* Store message in the gmsgBuf buffer */

	dataHdr.lTarget = ENCAP_TO_HL(INTERFACE_HANDLE);
	dataHdr.iTimeout = ENCAP_TO_HS(ENCAPSULATION_TIMEOUT);

	memcpy( pData, &dataHdr, ENCAP_DT_HDR_SIZE );	/* Add data header */
	pData += ENCAP_DT_HDR_SIZE;

	UINT16_SET(pData, 2);							/* Tag count = 2 */
	pData += sizeof(UINT16);

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_ADR_NULL);	/* Indicates local target */
	tag.iTag_length = 0;

	memcpy(pData, &tag, CPF_TAG_SIZE);
	pData += CPF_TAG_SIZE;

	tag.iTag_type = ENCAP_TO_HS(CPF_TAG_PKT_UCMM);	/* UCMM tag header */
	iSize = (UINT16)(2 + iPDUSize + pRequest->cfg.objectCfg.iDataSize);
	tag.iTag_length = ENCAP_TO_HS(iSize);

	memcpy( pData, &tag, CPF_TAG_SIZE );
	pData += CPF_TAG_SIZE;

	pdu_hdr.bService = pRequest->cfg.objectCfg.bService;
	pdu_hdr.bSize = (UINT8)(iPDUSize/2);			/* Path size in words */

	/* Add Protocol Data Unit */
	memcpy( pData, &pdu_hdr, PDU_HDR_SIZE );
	pData += PDU_HDR_SIZE;

	pduBuild( &pRequest->cfg.objectCfg.PDU, pData );
	pData += iPDUSize;

	/* Add request data if provided */
	if ( pRequest->cfg.objectCfg.iDataSize )
		memcpy( pData, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize );

	socketEncapSendData( pSession );

#ifdef CONNECTION_STATS
	gSystemStats.iNumSendUCMMMessagesPPS++;
	gSystemStats.systemStats.iNumSentUCMMMessages++;
#endif
	return TRUE;
}
#endif
