/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPUTIL.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** General utilities.
**
*****************************************************************************
*/

#include "eipinc.h"
#include "Ram_C_Connector.h"

#ifdef EIP_STACK_MEMORY
/* Force byte-array alignment to 4-byte alignment via special typedef */
static MEMORYPOOL gMemoryPool;
static UINT8* gpMemoryPool;
#endif

extern UINT16 GetResetCounter( void );
/*---------------------------------------------------------------------------
** utilInit( )
**
** Initializes the memory pool
**---------------------------------------------------------------------------
*/
void utilInit(void)
{
#ifdef EIP_STACK_MEMORY

#ifdef EIP_LARGE_BUF_ON_HEAP
	gMemoryPool.au8_array = EtIP_mallocHeap(MEMORY_POOL_SIZE * sizeof(UINT8));
	if(gMemoryPool.au8_array == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gMemoryPool to %d bytes\n", MEMORY_POOL_SIZE);
		return;
	}
#endif

	gpMemoryPool = gMemoryPool.au8_array;
#endif
}


#ifdef EIP_LARGE_BUF_ON_HEAP
/*---------------------------------------------------------------------------
** EtIP_mallocHeap( )
**
** Allocates a buffer from the native heap
**---------------------------------------------------------------------------
*/
UINT8* EtIP_mallocHeap( UINT16 iDataSize )
{
	UINT8* p;

	if (iDataSize == 0)
		return NULL;

	p = (UINT8*)malloc(iDataSize);
	if (p == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Out of memory for %d bytes\n", iDataSize);
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		return NULL;
	}

	memset(p, 0, iDataSize);

	return p;
}
#endif

/*---------------------------------------------------------------------------
** EtIP_malloc( )
**
** Stores passed in data in the memory pool.
** If data is NULL then memory is reserved, but not filled in.
** Returns the offset to the memory pool the data is stored at.
**---------------------------------------------------------------------------
*/
UINT8* EtIP_malloc( UINT8* data, UINT16 iDataSize )
{
#ifndef EIP_STACK_MEMORY
	UINT8* p;

	if (iDataSize == 0)
		return NULL;

    p = (UINT8*)Ram_Allocate( iDataSize );    //making use of Ram::Allocate API to get proper heap usage
	if (p == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Out of memory for %d bytes\n", iDataSize);
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		return NULL;
	}

	if (data)
		memcpy(p, data, iDataSize);

	return p;

#else
	UINT8* iOffset = gpMemoryPool;

	if ( iDataSize == 0)		/* If there is no data, do not allocate anything */
		return NULL;

	/* Check if we have enough room */
	if ( iDataSize > (&gMemoryPool.au8_array[MEMORY_POOL_SIZE] - gpMemoryPool) )
	{
		DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "utilAddToMemoryPool ran out of memory: Requested: %hd, Available: %hd",
			iDataSize, (&gMemoryPool.au8_array[MEMORY_POOL_SIZE] - gpMemoryPool));
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		return NULL;
	}

	if ( data )					/* If data pointer is not NULL then store the data in the dynamic pool */
		memcpy( gpMemoryPool, data, iDataSize );

	/* Fix to avoid unaligned pointers (force alignment to 32bit!) */
	if ( iDataSize&0x03 )
	{
		/* round size here to multiple of 4 to avoid unaligned pointers */
		iDataSize = (iDataSize - (iDataSize%4)) +4; /* round up*/
	}

	gpMemoryPool += iDataSize;	/* Adjust the pool ceiling offset */

	return iOffset;				/* The data is stored at this offset */

#endif /* EIP_STACK_MEMORY */
}

/*---------------------------------------------------------------------------
** EtIP_free( )
**
** Removes the specified data block from the memory pool.
**---------------------------------------------------------------------------
*/
void EtIP_free( UINT8** piData, UINT16* piDataSize )
{
#ifndef EIP_STACK_MEMORY
	/* Check that memory block has valid offset and size */
	if ( !(*piDataSize) || (*piData == NULL))
		return;

    Ram_De_Allocate( *piData ); // making use of Ram::De_Allocate
#else

	CONNECTION* pConnection;
	REQUEST* pRequest;
	REQUEST_GROUP* pRequestGroup;
	SESSION* pSession;
	ASSEMBLY* pAssembly;
	UINT16 iDataSize = *piDataSize;
	INT32 nLen = (INT32)(gpMemoryPool - *piData - iDataSize);

	/* Check that memory block has valid offset and size */
	if ( !iDataSize || *piData == NULL)
		return;

	if ( iDataSize&0x03 )
	{
		/* Data trunk size is implicitly rounded up by EtIP_malloc: Also round size here
		 * to multiple of 4. */
		iDataSize = (iDataSize - (iDataSize%4)) +4; /* round up*/
		nLen = gpMemoryPool - *piData - iDataSize;
	}

	/* Quick garbage collection is done every time the memory is released, instead of doing the
	   full pool unfragmentation when needed */
	memmove(*piData, *piData + iDataSize, nLen);

	gpMemoryPool -= iDataSize;    /* Adjust the pool ceiling offset */

	/* Adjust the offsets */
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if ( pConnection->cfg.iConnectionNameSize != 0 && pConnection->cfg.pConnectionName > *piData )
			pConnection->cfg.pConnectionName -= iDataSize;
		if ( pConnection->cfg.iTargetConsumingConnTagSize != 0 && pConnection->cfg.targetConsumingConnTag > *piData )
			pConnection->cfg.targetConsumingConnTag -= iDataSize;
		if ( pConnection->cfg.iTargetProducingConnTagSize != 0 && pConnection->cfg.targetProducingConnTag > *piData )
			pConnection->cfg.targetProducingConnTag -= iDataSize;
		if ( pConnection->requestCfg.pData != NULL && pConnection->requestCfg.pData > *piData )
			pConnection->requestCfg.pData -= iDataSize;
		pduDefragmentMemHeap( &pConnection->cfg.PDU, *piData, iDataSize );
		pduDefragmentMemHeap( &pConnection->requestCfg.PDU, *piData, iDataSize );
#ifdef ET_IP_SCANNER
		if (pConnection->cfg.nullForwardOpenCfg.iModuleConfig1Size != 0 && pConnection->cfg.nullForwardOpenCfg.moduleConfig1Data > *piData )
			pConnection->cfg.nullForwardOpenCfg.moduleConfig1Data -= iDataSize;
		if (pConnection->cfg.nullForwardOpenCfg.iModuleConfig2Size != 0 && pConnection->cfg.nullForwardOpenCfg.moduleConfig2Data > *piData )
			pConnection->cfg.nullForwardOpenCfg.moduleConfig2Data -= iDataSize;

		if ( pConnection->CCOcfg.cfg.iConnectionNameSize != 0 && pConnection->CCOcfg.cfg.pConnectionName > *piData )
			pConnection->CCOcfg.cfg.pConnectionName -= iDataSize;
		if ( pConnection->CCOcfg.cfg.iTargetConsumingConnTagSize != 0 && pConnection->CCOcfg.cfg.targetConsumingConnTag > *piData )
			pConnection->CCOcfg.cfg.targetConsumingConnTag -= iDataSize;
		if ( pConnection->CCOcfg.cfg.iTargetProducingConnTagSize != 0 && pConnection->CCOcfg.cfg.targetProducingConnTag > *piData )
			pConnection->CCOcfg.cfg.targetProducingConnTag -= iDataSize;
		pduDefragmentMemHeap( &pConnection->CCOcfg.ExtraPDU, *piData, iDataSize );

#endif
		if (pConnection->pResponseData != NULL && pConnection->pResponseData > *piData )
			pConnection->pResponseData -= iDataSize;
		if (pConnection->pExtendedErrorData != NULL && pConnection->pExtendedErrorData > *piData )
			pConnection->pExtendedErrorData -= iDataSize;
#ifdef ET_IP_SCANNER
		if (pConnection->pApplicationData != NULL && pConnection->pApplicationData > *piData )
			pConnection->pApplicationData -= iDataSize;
#endif
#ifdef ET_IP_MODBUS
		if ( pConnection->pModbusConsumeData != NULL && pConnection->pModbusConsumeData > *piData)
			pConnection->pModbusConsumeData -= iDataSize;
#endif
	}

	for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
	{
		if ( pRequest->pExtendedPath != NULL && pRequest->pExtendedPath > *piData )
			pRequest->pExtendedPath -= iDataSize;
		if ( pRequest->pExtendedErrorData != NULL && pRequest->pExtendedErrorData > *piData )
			pRequest->pExtendedErrorData -= iDataSize;
		if ( pRequest->cfg.objectCfg.pData != NULL && pRequest->cfg.objectCfg.pData > *piData )
			pRequest->cfg.objectCfg.pData -= iDataSize;
		pduDefragmentMemHeap( &pRequest->cfg.objectCfg.PDU, *piData, iDataSize );
	}

	for( pRequestGroup = gRequestGroups; pRequestGroup < gpnRequestGroups; pRequestGroup++ )        /* Find our request group */
	{
		if ( pRequestGroup->cfg.objectCfg.pData != NULL && pRequestGroup->cfg.objectCfg.pData > *piData )
			pRequestGroup->cfg.objectCfg.pData -= iDataSize;
		pduDefragmentMemHeap( &pRequestGroup->cfg.objectCfg.PDU, *piData, iDataSize );
	}

	for (pSession = gSessions; pSession < gpnSessions; pSession++)
	{
		if ((pSession->pPartialRecvPacket != NULL) && (pSession->pPartialRecvPacket > *piData))
			pSession->pPartialRecvPacket -= iDataSize;

		if ((pSession->pPartialSendPacket != NULL) && (pSession->pPartialSendPacket > *piData))
			pSession->pPartialSendPacket -= iDataSize;
	}

	for (pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++)
	{
		if ((pAssembly->pTagName != NULL) && (pAssembly->pTagName > *piData))
			pAssembly->pTagName -= iDataSize;
	}

#endif /* EIP_STACK_MEMORY */

	/* Make sure the same memory block is not released twice */
	*piDataSize = 0;
	*piData = NULL;
}

/*---------------------------------------------------------------------------
** EtIP_realloc( )
**
** Removes the old data block from the memory pool and replaces it with the 
** new one with different length.
**---------------------------------------------------------------------------
*/
void EtIP_realloc( UINT8** piData, UINT16* piDataSize, UINT8* data, UINT16 iDataSize )
{
#ifdef EIP_STACK_MEMORY
	UINT16 iAlignedDataSize;

	if ((piData) &&			/* Make sure piData and piDataSize exist */
		(*piData) &&
		(piDataSize) &&
		(*piData<data) &&			/* The shift will only happen to memory "below */
		(data >= gpMemoryPool) &&			/* pdata must be from gpMemoryPool */
		(data <= &gMemoryPool.au8_array[MEMORY_POOL_SIZE]))
	{
		iAlignedDataSize = *piDataSize;
		if ( iAlignedDataSize&0x03 ) 
		{
			/* Data trunk size is implicitly rounded up by EtIP_malloc: Also round size here
			 * to multiple of 4. */
			iAlignedDataSize = (iAlignedDataSize - (iAlignedDataSize%4)) +4;
		}
		/* Memory refed by *data will be moved. Re-target the data ptr before
		   its modified by EtIP_free */
		data -= iAlignedDataSize;
	}
#endif

	EtIP_free( piData, piDataSize );
	*piDataSize = iDataSize;
	*piData = EtIP_malloc( data, iDataSize );
}

/*---------------------------------------------------------------------------
** utilParseNetworkPath( )
**
** Determine if network path is local or remote. 
** If remote extract the extended part and convert it to the Extended Port
** segment representation.
** Return TRUE if network path is valid, FALSE otherwise.
**---------------------------------------------------------------------------
*/
BOOL utilParseNetworkPath( char* szNetworkPath, BOOL* pbLocal, char* extendedPath, UINT16* piExtendedPathLen )
{
	UINT32 nLen = (UINT32)strlen(szNetworkPath);
	UINT32 i;
	BOOL  bPortFilled = FALSE;
	INT32 nPort = 0;
	UINT8  szPort[64];
	UINT8 *pPort = szPort;
	INT32 nNode;
	UINT8  szNode[128];
	INT32 nNodeSize = 0;
	char *pExtendedPath = extendedPath;
	char *pNetworkPath;

	*pbLocal = TRUE;
	*piExtendedPathLen = 0;

	for( i = 0, pNetworkPath = szNetworkPath; i < nLen; i++, pNetworkPath++)
	{
		if ( *pbLocal )
		{
			if ( *pNetworkPath == ',' )
			{
				if ( szNetworkPath[nLen-1] != ',' )		/* If the last char is not ',' add it now */
				{
					szNetworkPath[nLen++] = ',';
					szNetworkPath[nLen] = 0;
				}
				*pbLocal = FALSE;
				*pNetworkPath = 0;
			}
		}
		else
		{
			if ( *pNetworkPath != ',' )
			{
				if ( !bPortFilled )
				{
					if ( *pNetworkPath < '0' || *pNetworkPath > '9' )
						return FALSE;
					*pPort++ = *pNetworkPath;
				}
				else
					szNode[nNodeSize++] = *pNetworkPath;
			}
			else
			{
				if ( !bPortFilled )						/* Convert the port to integer and store in nPort */
				{
					if ( pPort == szPort )				/* 2 commas in a row are not allowed */
						return FALSE;
					*pPort = 0;
					nPort = atol( (const char*)szPort );
					if ( nPort < 0 || nPort > 0xffff ) /* Port is limited to 16 bits */
						return FALSE;
					bPortFilled = TRUE;
				}
				else
				{
					if ( nNodeSize == 0 )				/* 2 commas in a row are not allowed */
						return FALSE;
					szNode[nNodeSize] = 0;
					/* Check if this is one byte presentation of the slot or any other network path */
					/* If it is one byte representation the length is limited to 3 numeric chars */
					if ( nNodeSize <= 3 && szNode[0] >= '0' && szNode[0] <= '9' &&
						( nNodeSize < 2 || (szNode[1] >= '0' && szNode[1] <= '9') ) &&
						( nNodeSize < 3 || (szNode[2] >= '0' && szNode[2] <= '9') ) )
					{
						nNode = atol( (const char*)szNode );
						if ( nNode < 0 || nNode > 0xff ) /* Numeric node is limited to 8 bits */
							return FALSE;
						if ( nPort < 0xf )						/* Put it in the segment byte */
							*pExtendedPath++ = (UINT8)(PATH_SEGMENT | (UINT8)nPort);
						else									/* Add separate 16-bit port value */
						{
							*pExtendedPath++ = PATH_SEGMENT | 0xf;
							UINT16_SET( pExtendedPath, nPort );
							pExtendedPath += sizeof(UINT16);
						}
						*pExtendedPath++ = (UINT8)nNode;		/* Add numeric node */
					}
					else		/* We have a string presentation of the network path */
					{
						if ( platformAddrFromPath( (const char*)szNode, (INT32)strlen((const char*)szNode) ) == (UINT32)ERROR_STATUS )
							return FALSE;
						if ( nPort < 0xf )						/* Put it in the segment byte */
							*pExtendedPath++ = (UINT8)(PATH_SEGMENT | EXTENDED_LINK_MASK | (UINT8)nPort);
						else									/* Add separate 16-bit port value */
						{
							*pExtendedPath++ = PATH_SEGMENT | EXTENDED_LINK_MASK | 0xf;
							UINT16_SET( pExtendedPath, nPort );
							pExtendedPath += sizeof(UINT16);
						}
						if ( nNodeSize % 2 )
							szNode[nNodeSize++] = 0;			/* Add pad byte to make node length even */
						*pExtendedPath++ = (UINT8)nNodeSize;
						memcpy( pExtendedPath, szNode, nNodeSize );
						pExtendedPath += (UINT16)nNodeSize;
					}
					nNodeSize = 0;
					pPort = szPort;
					bPortFilled = FALSE;
				}
			}
		}
	}

	*piExtendedPathLen = (UINT16)(pExtendedPath - extendedPath);
	return TRUE;
}

/*---------------------------------------------------------------------------
** utilBuildNetworkPath( )
**
** Build network path from an IP address and an extended path. 
** Return TRUE if network path is valid, FALSE otherwise.
**---------------------------------------------------------------------------
*/
BOOL utilBuildNetworkPath( char* szNetworkPath, UINT32 lIPAddr, EPATH* pPDU )
{
	struct in_addr sTmpAddr;
	INT32 i, currentLength;
	UINT16 iTagSize;
	UINT16 iTagType;
	ETAG_DATATYPE pTag;
	char szTemp[MAX_NETWORK_PATH_SIZE];

	szNetworkPath[0] = 0;

	if ( lIPAddr )
	{
		platformGetInternetAddress(sTmpAddr) = lIPAddr;
		strcpy( szNetworkPath, platformInetNtoa(sTmpAddr) );
	}

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "utilBuildNetworkPath Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0; i < pPDU->bNumTags; i++ )
	{
		pTag = pduGetTagByIndex( pPDU, &iTagType, i, &iTagSize );
		if ( iTagSize == 0 )
			break;

		if ( iTagType == TAG_TYPE_PORT || iTagType == TAG_TYPE_SLOT )
		{
			sprintf( szTemp, ",%u", ((UINT32)pTag));
			strncat( szNetworkPath, szTemp, MAX_NETWORK_PATH_SIZE-1-strlen(szNetworkPath));
		}
		else if ( iTagType == TAG_TYPE_LINK_ADDRESS )
		{
			currentLength = (INT32)strlen(szNetworkPath);
			szTemp[0] = ',';
			if (iTagSize > MAX_NETWORK_PATH_SIZE-currentLength-2)
				iTagSize = MAX_NETWORK_PATH_SIZE-currentLength-2;

			memcpy( &szTemp[1], (void*)pTag, iTagSize );
			szTemp[iTagSize+1] = 0;
			strncat( szNetworkPath, szTemp, MAX_NETWORK_PATH_SIZE-(currentLength+2));
		}
	}

	return TRUE;
}

/*---------------------------------------------------------------------------
** utilCheckNetworkPath( )
**
** Determine if network path is invalid from the get go. 
** Return 0 if network path looks valid, one of the errors otherwise.
**---------------------------------------------------------------------------
*/
INT32 utilCheckNetworkPath(const char* networkPath )
{
	UINT32 lIPAddress;
	BOOL bLocalRequest;
	char szExtendedPath[MAX_NETWORK_PATH_SIZE];
	UINT16 iExtendedPathLen;
	char szNetworkPath[MAX_NETWORK_PATH_SIZE];

	strcpy( szNetworkPath, networkPath );

	if ( !utilParseNetworkPath( szNetworkPath, &bLocalRequest, szExtendedPath, &iExtendedPathLen ) )
		return NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH;

	lIPAddress = platformAddrFromPath(szNetworkPath, (INT32)strlen(szNetworkPath));
	if ( lIPAddress == (UINT32)ERROR_STATUS )
		return NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH;

	return 0;
}

/*---------------------------------------------------------------------------
** utilCheckHostIPAddress( )
**
** Determine if host IP address is invalid from the get go.
** Return 0 if host IP address looks valid, one of the errors otherwise.
**---------------------------------------------------------------------------
*/
INT32 utilCheckHostIPAddress( UINT32 lHostIPAddr )
{
	/* If set to INADDR_NONE - assign the first network card address */
	if ((lHostIPAddr == INADDR_NONE) || (lHostIPAddr == INADDR_ANY))
	{
		if ( gnClaimedHostIPAddrCount == 0 )
			return ERR_NO_CLAIMED_HOST_ADDRESSES;
	}
	else if ( tcpipFindClaimedHostAddress(lHostIPAddr) == NULL)
		return ERR_INVALID_OR_NOT_CLAIMED_HOST_ADDRESS;

	return 0;
}

/*---------------------------------------------------------------------------
** utilGetUniqueID( )
**
** Return a UINT32 value unique between lSessionID, lSessionTag for all sessions 
** and between lConsumingCID, lProducingCID, iConnectionSerialNbr for all 
** connections.
**---------------------------------------------------------------------------
*/
UINT32 utilGetUniqueID()
{
    //UINT32 lVal = gdwTickCount;    /* Take a guess */
 	//EATON_EIP_Changes:
    //Added this new function call to get new value on every reset. To fix conformance issue.
	UINT32 lVal = (UINT32)GetResetCounter();    /* Take a guess */
		  
    for(;;)
    {        
        /* Skip 0xffff and 0xffffffff allocated to mark invalid ids */
        if ( lVal != 0xffff && lVal != 0xffffffff )
                if ( utilIsIDUnique( lVal ) )    
                    return lVal;
        
        lVal++;
    }

    //return (UINT32)ERROR_STATUS;            /* We should never get here */
}

/*---------------------------------------------------------------------------
** utilIsIDUnique( )
**
** Checks if passed value is unique between lSessionID, lSessionTag for all 
** sessions and between lConsumingCID, lProducingCID, iConnectionSerialNbr 
** for all connections.
**---------------------------------------------------------------------------
*/
BOOL utilIsIDUnique(UINT32 lVal)
{
	SESSION*  pSession;
	CONNECTION* pConnection;

	/* Check if unique between lSessionID, lSessionTag for all sessions */
	for( pSession = gSessions; pSession < gpnSessions; pSession++ )
	{
		if ( pSession->lSessionTag == lVal )
			return FALSE;
	}

	/* Check if unique between lConsumingCID, lProducingCID, iConnectionSerialNbr for all connections */
	for ( pConnection = gConnections; pConnection <= gpnConnections; pConnection++ )
	{
		if ((pConnection->lConsumingCID == lVal) ||
			(pConnection->lProducingCID == lVal) ||
			(pConnection->iConnectionSerialNbr == (UINT16)lVal))
				return FALSE;
	}

	return TRUE;
}

UINT32 utilCalculatePseudoRandomDelay(UINT32 lMaxWaitTime)
{
	UINT8 R256;
	UINT32 delayValue;
	ENETLINK_INST_ATTR enetLink;

	/* Use the MAC address of the first instance of the Ethernet Link 
	 * object for the RNG algorithm
	 */
	platformGetEnetLinkAttributes(1, &enetLink);

	R256 = enetLink.macAddr[0];
	R256 = (R256 << 1) ^ enetLink.macAddr[1];
	R256 = (R256 << 1) ^ enetLink.macAddr[2];
	R256 = (R256 << 1) ^ enetLink.macAddr[3];
	R256 = (R256 << 1) ^ enetLink.macAddr[4];
	R256 = (R256 << 1) ^ enetLink.macAddr[5];
	R256 %= 256;

	delayValue = lMaxWaitTime * R256 / 256;
	return delayValue;

}

#if defined EIP_FILE_OBJECT
BOOL utilComputeFileSizeAndChecksum(char* pstrFilename, UINT32* pFileSize, INT16* piChecksum)
{
	PLATFORM_FILE_HANDLE pFile;
	UINT16 iChecksum = 0;
	int i;

	if ((pFile = platformFileOpen(pstrFilename, "rb")) == NULL)
		return FALSE;

	*pFileSize = 0;
	while ((i = platformFileGetC(pFile)) != EOF)
	{
		iChecksum += i;
		(*pFileSize)++;
	}

	*piChecksum = (UINT16)(~iChecksum) + 1;
	return TRUE;
}
#endif
