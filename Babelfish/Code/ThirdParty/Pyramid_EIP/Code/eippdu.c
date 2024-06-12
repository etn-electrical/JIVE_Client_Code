/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPDU.c
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Protocol Data Unit object parses and rebuilds router paths
**
*****************************************************************************
*/

#include "eipinc.h"

/*---------------------------------------------------------------------------
** pduInit( )
**
** Initialize the PDU path.
**---------------------------------------------------------------------------
*/
void pduInit( EPATH* pPDU, BOOL bPadded )
{
	memset( pPDU, 0, sizeof(EPATH) );

	pPDU->bPadded = bPadded;
}

/*---------------------------------------------------------------------------
** pduAddTag( )
**
** Adds a tag to the EPATH.
**---------------------------------------------------------------------------
*/
BOOL pduAddTag(EPATH* pPDU, UINT16 iTagType, UINT16 iTagDataSize, ETAG_DATATYPE lData)
{
	if ( pPDU->bNumTags >= MAX_TAGS )
		return FALSE;

	pPDU->Tags[pPDU->bNumTags].iTagType = iTagType;
	pPDU->Tags[pPDU->bNumTags].iTagDataSize = iTagDataSize;
	pPDU->Tags[pPDU->bNumTags].TagData = lData;
	pPDU->bNumTags++;

	return TRUE;
}


/*---------------------------------------------------------------------------
** pduRelease( )
**
** Releases any dynamic memory allocated to store PDU parameters.
**---------------------------------------------------------------------------
*/
void pduRelease( EPATH* pPDU )
{
	INT32 i;
	ETAG* pTag;
	UINT8* p;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduRelease Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags;
		i < pPDU->bNumTags;
		i++, pTag++ )
	{
		switch( pTag->iTagType )
		{
		case TAG_TYPE_LINK_ADDRESS:
		case TAG_TYPE_KEY:
		case TAG_TYPE_SYMBOL:
		case TAG_TYPE_DATA:
		case TAG_TYPE_DATA2:
		case TAG_TYPE_EXT_SYMBOL:
			p = (UINT8*)pTag->TagData;
			EtIP_free( &p, &pTag->iTagDataSize );
			break;

		default:
			break;
	}
	}

	pduInit( pPDU, TRUE );
}

/*---------------------------------------------------------------------------
** pduAllocateCopy( )
**
** Allocate dynamic memory in the copied PDU 
**---------------------------------------------------------------------------
*/
void pduAllocateCopy( EPATH* pDestPDU, EPATH* pSrcPDU )
{
	INT32 i;
	ETAG *pSrcTag, *pDestTag;

	/* Check for any failure to initialize the source pdu structure */
	if ( pSrcPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduAllocateCopy Internal Logic FAILURE pSrcPDU->bNumTags set to %d", pSrcPDU->bNumTags );
		pSrcPDU->bNumTags = 0;
	}

	/* Check for any failure to initialize the source pdu structure */
	if ( pDestPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduAllocateCopy Internal Logic FAILURE pDestPDU->bNumTags set to %d", pDestPDU->bNumTags );
		pDestPDU->bNumTags = 0;
	}
	
	for( i = 0, pSrcTag = pSrcPDU->Tags, pDestTag = pDestPDU->Tags; 
		i < pSrcPDU->bNumTags;
		i++, pSrcTag++, pDestTag++)
		{
			switch( pSrcTag->iTagType )
			{
			case TAG_TYPE_LINK_ADDRESS:
			case TAG_TYPE_KEY:
			case TAG_TYPE_SYMBOL:
			case TAG_TYPE_DATA:
			case TAG_TYPE_DATA2:
			case TAG_TYPE_EXT_SYMBOL:
				pDestTag->TagData = (ETAG_DATATYPE)EtIP_malloc((UINT8*)pSrcTag->TagData, pSrcTag->iTagDataSize);
				pDestTag->iTagDataSize = pSrcTag->iTagDataSize;
				break;

			default:
				break;
		}
	}
}

#ifdef EIP_STACK_MEMORY

/*---------------------------------------------------------------------------
** pduDefragmentMemHeap( )
**
** Check if any PDU pointers needs to be adjusted during heap memory 
** defragmentation.
**---------------------------------------------------------------------------
*/
void pduDefragmentMemHeap( EPATH* pPDU, UINT8* pData, UINT16 iDataSize )
{
	INT32 i;
	ETAG* pTag;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduDefragmentMemHeap Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags; 
		i < pPDU->bNumTags;
		i++, pTag++ )
	{
		switch(pTag->iTagType )
		{
		case TAG_TYPE_LINK_ADDRESS:
		case TAG_TYPE_KEY:
		case TAG_TYPE_SYMBOL:
		case TAG_TYPE_DATA:
		case TAG_TYPE_DATA2:
		case TAG_TYPE_EXT_SYMBOL:
			if ( (UINT8*)pTag->TagData > pData )
				pTag->TagData -= iDataSize;
			break;

		default:
			break;
		}
	}
}

#endif

/*---------------------------------------------------------------------------
** pduConvertToInternalStorage( )
**
** Store EPATH in the stack.
**---------------------------------------------------------------------------
*/
void pduConvertToInternalStorage( EPATH* pExternalPDU, EPATH* pStackPDU )
{
	INT32 i;
	ETAG *pStackTag, *pExternalTag;

	/* Check for any failure to initialize the pdu structure */
	if ( pExternalPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduConvertToInternalStorage Internal Logic FAILURE pExternalPDU->bNumTags set to %d", pExternalPDU->bNumTags );
		pExternalPDU->bNumTags = 0;
	}

	memcpy(pStackPDU, pExternalPDU, sizeof(EPATH));

	for( i = 0, pStackTag = pStackPDU->Tags, pExternalTag = pExternalPDU->Tags;
		i < pStackPDU->bNumTags;
		i++, pStackTag++, pExternalTag++)
	{
		switch( pStackTag->iTagType )
		{
		case TAG_TYPE_LINK_ADDRESS:
		case TAG_TYPE_KEY:
		case TAG_TYPE_SYMBOL:
		case TAG_TYPE_DATA:
		case TAG_TYPE_DATA2:
		case TAG_TYPE_EXT_SYMBOL:
			pStackTag->TagData = (ETAG_DATATYPE)EtIP_malloc((UINT8*)pExternalTag->TagData, pExternalTag->iTagDataSize);
			pStackTag->iTagDataSize = pExternalTag->iTagDataSize;
			break;

		default:
			break;
		}
	}
}

/*---------------------------------------------------------------------------
** pduParse( )
**
** Parse the Protocol Data Unit buffer.
** Based on the data in pPacketPtr buffer fill in the pPDU structure.
** Return total number of bytes parsed.
**---------------------------------------------------------------------------
*/

INT32 pduParse( UINT8* pPacketPtr, EPATH* pPDU, BOOL bPDUHeader, UINT32 nPDUFreeLength )
{
	UINT8* pPacketPtrStart = pPacketPtr;		/* Used to calculate the number of parsed bytes */
	INT32 nTotal;								/* Declared in the header the length of EPATH in bytes */
	INT32 nParsed;
	PDU_HDR pdu_hdr;
	UINT8 bSegment, bSegmentType, bPort, bLinkAddressSize;
	EtIPDeviceID deviceId;
	UINT8 bLogicalType, bLogicalFormat, bNetworkType, bSymbolSize, bSize;
	UINT8 bExtendedSymbol, bExtendedSymbolSize, bExtSymbolSize;
	UINT16 iDataSize;
	ETAG* pPDUTag;

	if (bPDUHeader)
	{
		/* The first 2 bytes is PDU header */
		memcpy( &pdu_hdr, pPacketPtr, PDU_HDR_SIZE );
		pPacketPtr += PDU_HDR_SIZE;

		/* PDU Length not including the PDU header */
		nTotal = pdu_hdr.bSize * 2 + 2;
		if ((UINT32)nTotal > nPDUFreeLength)
		{
			/* We have an invalid length field, don't parse the PDU */
			return 0;
		}

		nParsed = PDU_HDR_SIZE;
	}
	else
	{
		/* No PDU header */
		nTotal = nPDUFreeLength;
		nParsed = 0;
	}

	pduRelease( pPDU );

	/* Just the header */
	if ( nTotal == 0 )
		return nParsed;
	pPDUTag = &pPDU->Tags[pPDU->bNumTags];

	while (( nParsed < nTotal ) &&
			(pPDU->bNumTags < MAX_TAGS))
	{
		if ( nParsed == (nTotal - 1) )  /* Ignore one last char - it's the pad byte. */
		{
			nParsed++;
			break;
		}

		bSegment = *pPacketPtr;
		bSegmentType = (UINT8)(bSegment & SEGMENT_TYPE_MASK);

		pPacketPtr++;

		/* Segment type determine the data length and structure that follows */
		switch( bSegmentType )
		{
		case PATH_SEGMENT:
			bPort = (UINT8)(bSegment & PORT_ID_MASK);

			if ( bSegment & EXTENDED_LINK_MASK )
			{
				bLinkAddressSize = *pPacketPtr;
				pPacketPtr++;
			}
			else
				bLinkAddressSize = 1;

			/* Storing the Port number */
			if ( bPort == EXTENDED_PORT_FLAG )
			{
				pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
				pPDUTag->iTagDataSize = sizeof(UINT16);
				pPacketPtr += sizeof(UINT16);
			}
			else
			{
				pPDUTag->TagData = (ETAG_DATATYPE)bPort;
				pPDUTag->iTagDataSize = 1;
			}

			pPDUTag->iTagType = TAG_TYPE_PORT;
			pPDU->bNumTags++; pPDUTag++;

			if (pPDU->bNumTags >= MAX_TAGS)
				break;

			/* Extended Link address (e.g. the target IP address in a string representation) */
			if ( bLinkAddressSize > 1 )
			{
				pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc(pPacketPtr, bLinkAddressSize);
				pPDUTag->iTagDataSize = bLinkAddressSize;
				pPDUTag->iTagType =  TAG_TYPE_LINK_ADDRESS;

				pPacketPtr += bLinkAddressSize;

				nParsed = (INT32)(pPacketPtr - pPacketPtrStart);
				if ( (bLinkAddressSize % 2) && (nParsed < (nTotal - 1)) )
				{
					/* Padded format */
					if ( *pPacketPtr == 0 )
					{
						pPDU->bPadded = TRUE;
						pPacketPtr++;			/* Skip the pad byte */
					}
					else
						pPDU->bPadded = FALSE;
				}
			}
			/* Destination slot number in a rack */
			else if ( bLinkAddressSize == 1 )
			{
				pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
				pPDUTag->iTagDataSize = 1;
				pPDUTag->iTagType = TAG_TYPE_SLOT;
				pPacketPtr++;
			}
			pPDU->bNumTags++; pPDUTag++;
			break;

		case LOGICAL_SEGMENT:
			bLogicalType = (UINT8)(bSegment & LOGICAL_SEG_TYPE_MASK);
			bLogicalFormat = (UINT8)(bSegment & LOGICAL_SEG_FORMAT_MASK);

			switch( bLogicalType )
			{
			case LOGICAL_SEG_CLASS:
				switch(bLogicalFormat)
				{
				case LOGICAL_SEG_8_BIT:		/* Class ID is 1 byte long */
					pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
					pPDUTag->iTagDataSize = 1;
					pPacketPtr++;
					break;
				case LOGICAL_SEG_16_BIT:	/* Class ID is 2 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
					pPDUTag->iTagDataSize = sizeof(UINT16);
					pPacketPtr += sizeof(UINT16);
					break;
				case LOGICAL_SEG_32_BIT:	/* Class ID is 4 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)UINT32_GET(pPacketPtr);
					pPDUTag->iTagDataSize = sizeof(UINT32);
					pPacketPtr += sizeof(UINT32);
					break;
				default:					/* Class ID must be either 1, 2 or 4 bytes long */
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;
				}

				pPDUTag->iTagType = TAG_TYPE_CLASS;
				pPDU->bNumTags++; pPDUTag++;
				break;

			case LOGICAL_SEG_INSTANCE:
				switch (bLogicalFormat)
				{
				case LOGICAL_SEG_8_BIT:		/* Instance ID is 1 byte long */
					pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
					pPDUTag->iTagDataSize = 1;
					pPacketPtr++;
					break;
				case LOGICAL_SEG_16_BIT:	/* Instance ID is 2 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
					pPDUTag->iTagDataSize = sizeof(UINT16);
					pPacketPtr += sizeof(UINT16);
					break;
				case LOGICAL_SEG_32_BIT:	/* Instance ID is 4 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)UINT32_GET(pPacketPtr);
					pPDUTag->iTagDataSize = sizeof(UINT32);
					pPacketPtr += sizeof(UINT32);
					break;
				default:					/* Instance ID must be either 1, 2 or 4 bytes long */
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;
				}

				pPDUTag->iTagType = TAG_TYPE_INSTANCE;
				pPDU->bNumTags++; pPDUTag++;
				break;

			case LOGICAL_SEG_ATTRIBUTE:
				switch (bLogicalFormat)
				{
				case LOGICAL_SEG_8_BIT:		/* Attribute ID is 1 byte long */
					pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
					pPDUTag->iTagDataSize = 1;
					pPacketPtr++;
					break;
				case LOGICAL_SEG_16_BIT:	/* Attribute ID is 2 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
					pPDUTag->iTagDataSize = sizeof(UINT16);
					pPacketPtr += sizeof(UINT16);
					break;
				case LOGICAL_SEG_32_BIT:	/* Attribute ID is 4 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)UINT32_GET(pPacketPtr);
					pPDUTag->iTagDataSize = sizeof(UINT32);
					pPacketPtr += sizeof(UINT32);
					break;
				default:					/* Attribute ID must be either 1, 2 or 4 bytes long */
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;

				}

				pPDUTag->iTagType = TAG_TYPE_ATTRIBUTE;
				pPDU->bNumTags++; pPDUTag++;
				break;

			case LOGICAL_SEG_MEMBER:
				switch (bLogicalFormat)
				{
				case LOGICAL_SEG_8_BIT:		/* Member ID is 1 byte long */
					pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
					pPDUTag->iTagDataSize = 1;
					pPacketPtr++;
					break;
				case LOGICAL_SEG_16_BIT:	/* Member ID is 2 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
					pPDUTag->iTagDataSize = sizeof(UINT16);
					pPacketPtr += sizeof(UINT16);
					break;
				case LOGICAL_SEG_32_BIT:	/* Member ID is 4 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;		/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)UINT32_GET(pPacketPtr);
					pPDUTag->iTagDataSize = sizeof(UINT32);
					pPacketPtr += sizeof(UINT32);
					break;
				default:					/* Member ID must be either 1, 2 or 4 bytes long */
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;
				}

				pPDUTag->iTagType = TAG_TYPE_MEMBER;
				pPDU->bNumTags++; pPDUTag++;
				break;

			/* Parse Originator->Target and/or Target->Originator conection points */
			case LOGICAL_SEG_CONN_POINT:
				switch(bLogicalFormat)
				{
				case LOGICAL_SEG_8_BIT:
					pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
					pPDUTag->iTagDataSize = 1;
					pPacketPtr++;
					break;
				case LOGICAL_SEG_16_BIT:		/* Connection point is 2 bytes long */
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;			/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)(UINT16_GET(pPacketPtr));
					pPDUTag->iTagDataSize = sizeof(UINT16);
					pPacketPtr += sizeof(UINT16);
					break;
				case LOGICAL_SEG_32_BIT:
					if ( *pPacketPtr != 0 )
						pPDU->bPadded = FALSE;

					if ( pPDU->bPadded )
						pPacketPtr++;			/* Skip the pad byte */

					pPDUTag->TagData = (ETAG_DATATYPE)UINT32_GET(pPacketPtr);
					pPDUTag->iTagDataSize = sizeof(UINT32);
					pPacketPtr += sizeof(UINT32);
					break;
				default:				/* Connection point must be either 1, 2  or 4 bytes long */
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;
				}

				pPDUTag->iTagType = TAG_TYPE_CONN_POINT;
				pPDU->bNumTags++; pPDUTag++;
				break;

			case LOGICAL_SEG_KEY:
				/* Parse segment key. Values other than 0 indicate that keying is enabled */
				if ( (*pPacketPtr) == COMMON_KEY_FORMAT )
				{
					pPacketPtr++;
					memcpy( &deviceId, pPacketPtr, sizeof(EtIPDeviceID) );
					ENCAP_CVT_HS(deviceId.iVendorID);
					ENCAP_CVT_HS(deviceId.iProductCode);
					ENCAP_CVT_HS(deviceId.iProductType);
					pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc((UINT8*)&deviceId, sizeof(EtIPDeviceID));
					pPDUTag->iTagDataSize = sizeof(EtIPDeviceID);
					pPDUTag->iTagType =  TAG_TYPE_KEY;
					pPacketPtr += sizeof(EtIPDeviceID);
					pPDU->bNumTags++; pPDUTag++;
				}
				else
				{
					DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
					return 0;
				}
				break;

			default:
				/* Segment type was not recognized */
				DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid logical segment specifier: 0x%x", bSegment);
				return 0;
			}
			break;

		case NETWORK_SEGMENT:
			bNetworkType = (UINT8)(bSegment & NET_SEGMENT_TYPE_MASK);

			/* Only handling Production Inhibit Network Segment */
			if ( bNetworkType == PRODUCTION_INHIBIT_TYPE )
			{
				pPDUTag->TagData = (ETAG_DATATYPE)(*pPacketPtr);
				pPDUTag->iTagDataSize = 1;
				pPDUTag->iTagType = TAG_TYPE_INHIBIT_INTERVAL;
				pPDU->bNumTags++; pPDUTag++;
			}

			pPacketPtr++;
			break;

		case SYMBOLIC_SEGMENT:
			bSymbolSize = (UINT8)(bSegment & SYM_SEGMENT_SIZE_MASK);
			if ( bSymbolSize == 0 )
			{
				bExtendedSymbol = *pPacketPtr;
				pPacketPtr++;

				bExtendedSymbolSize = (UINT8)(bExtendedSymbol & SYM_SEGMENT_SIZE_MASK);

				switch ( bExtendedSymbol )
				{
				case EXTENDED_SYM_TYPE_DOUBLEBYTE:
					bSymbolSize = (UINT8)(bExtendedSymbolSize*2);
					break;
				case EXTENDED_SYM_TYPE_TRIPLEBYTE:
					bSymbolSize = (UINT8)(bExtendedSymbolSize*3);
					break;
				case EXTENDED_SYM_TYPE_SPECIAL:
					switch ( bExtendedSymbolSize )
					{
					case 6:
						bSymbolSize = sizeof(UINT8);
						break;
					case 7:
						bSymbolSize = sizeof(UINT16);
						break;
					case 8:
						bSymbolSize = sizeof(UINT32);
						break;
					default:
						bSymbolSize = 0;
						break;
					}
					break;
				default:
					bSymbolSize = 0;
					break;
				}
			}

			if ( bSymbolSize )
			{
				pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc(pPacketPtr, bSymbolSize);
				pPDUTag->iTagDataSize = bSymbolSize;
				pPDUTag->iTagType = TAG_TYPE_SYMBOL;
				pPDU->bNumTags++; pPDUTag++;
			}
			pPacketPtr += bSymbolSize;

			nParsed = (INT32)(pPacketPtr - pPacketPtrStart);
			if ( (nParsed % 2) && (nParsed < (nTotal - 1)) )
			{
				if ( *pPacketPtr == 0 )			/* Padded format */
				{
					pPDU->bPadded = TRUE;
					pPacketPtr++;				/* Skip the pad byte */
				}
				else
					pPDU->bPadded = FALSE;
			}
			break;

		case DATA_SEGMENT:
			bSize = *pPacketPtr;
			pPacketPtr++;

			if ( bSegment == SIMPLE_DATA_SEGMENT )
			{
				iDataSize = (UINT16)(bSize*2);
				pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc(pPacketPtr, iDataSize);
				pPDUTag->iTagDataSize = iDataSize;
				pPDUTag->iTagType = TAG_TYPE_DATA;
				pPDU->bNumTags++; pPDUTag++;

				pPacketPtr += iDataSize;
			}
			else if ( bSegment == EXT_SYMBOL_SEGMENT )
			{
				bExtSymbolSize = bSize;
				pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc(pPacketPtr, bExtSymbolSize);
				pPDUTag->iTagDataSize = bExtSymbolSize;
				pPDUTag->iTagType = TAG_TYPE_EXT_SYMBOL;
				pPDU->bNumTags++; pPDUTag++;

				pPacketPtr += bExtSymbolSize;

				nParsed = (INT32)(pPacketPtr - pPacketPtrStart);
				if ( (bExtSymbolSize % 2) && (nParsed < (nTotal - 1)) )
				{
					/* Padded format */
					if ( *pPacketPtr == 0 )
					{
						pPDU->bPadded = TRUE;
						pPacketPtr++;			/* Skip the pad byte */
					}
					else
						pPDU->bPadded = FALSE;
				}
			}
			break;

		default:
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_MSG, 0, 0, "pduParse invalid segment specifier: 0x%x", bSegment);
			return 0;
		}

		nParsed = (INT32)(pPacketPtr - pPacketPtrStart);
	}

	return nParsed;
}

/*---------------------------------------------------------------------------
** pduGetTagByType( )
**
** Return the tag data and size based on the tag type and the 0 based type
** occurence index (e.g. to get the first class tag the type will be
** TAG_TYPE_CLASS and index will be 0, to get the second class index will 
** be 1, etc).
**---------------------------------------------------------------------------
*/
ETAG_DATATYPE pduGetTagByType( EPATH* pPDU, UINT16 iTagType, INT32 nIndex, UINT16* piTagDataSize )
{
	INT32  nCount = 0;
	INT32  i;
	ETAG *pTag;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduGetTagByType Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags; 
		i < pPDU->bNumTags;
		i++, pTag++)
	{
		if ( pTag->iTagType == iTagType )
		{
			if ( nCount == nIndex )
			{
				*piTagDataSize = pTag->iTagDataSize;
				return pTag->TagData;
			}
			nCount++;
		}
	}

	*piTagDataSize = 0;
	return 0;
}

/*---------------------------------------------------------------------------
** pduGetTagByIndex( )
**
** Return the tag data and size based on the 0 based tag index
**---------------------------------------------------------------------------
*/
ETAG_DATATYPE pduGetTagByIndex( EPATH* pPDU, UINT16* piTagType, INT32 nIndex, UINT16* piTagDataSize )
{
	if ( nIndex < pPDU->bNumTags )
	{
		*piTagType = pPDU->Tags[nIndex].iTagType;
		*piTagDataSize = pPDU->Tags[nIndex].iTagDataSize;
		return pPDU->Tags[nIndex].TagData;
	}

	*piTagDataSize = 0;
	return 0;
}

/*---------------------------------------------------------------------------
** pduGetIndexByType( )
**
** Return the index based on the tag type and the 0 based type 
** occurence index (e.g. to get the first index the type will be
** TAG_TYPE_CLASS and index will be 0, to get the second class index will 
** be 1, etc).
**---------------------------------------------------------------------------
*/
INT32 pduGetIndexByType(EPATH* pPDU, UINT16 iTagType, INT32 nIndex)
{
	INT32  nCount = 0;
	INT32  i;
	ETAG *pTag;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduGetIndexByType Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags; 
		i < pPDU->bNumTags;
		i++, pTag++)
	{
		if ( pTag->iTagType == iTagType )
		{
			if ( nCount == nIndex )
				return i;

			nCount++;
		}
	}

	return -1;
}


/*---------------------------------------------------------------------------
** pduRemoveTagByIndex( )
**
** Remove the tag based on the tag index
**---------------------------------------------------------------------------
*/
BOOL pduRemoveTagByIndex( EPATH* pPDU, INT32 nIndex )
{
	INT32 i;
	ETAG *pTag;
	UINT8* p;

	if ( nIndex >= pPDU->bNumTags )
		return FALSE;

	pTag = &pPDU->Tags[nIndex];

	switch( pTag->iTagType )
	{
	case TAG_TYPE_LINK_ADDRESS:
	case TAG_TYPE_KEY:
	case TAG_TYPE_SYMBOL:
	case TAG_TYPE_DATA:
	case TAG_TYPE_DATA2:
	case TAG_TYPE_EXT_SYMBOL:
		p = (UINT8*)pTag->TagData;
		EtIP_free(&p, &pTag->iTagDataSize );
		break;

	default:
		break;
	}

	pPDU->bNumTags--;

	for( i = nIndex;
		i < pPDU->bNumTags;
		i++, pTag++)
	{
		memcpy(pTag, pTag+1, sizeof(ETAG) );
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
** pduBuild( )
**
** Build the Protocol Data Unit buffer based on the PDU structure.
** Return total number of bytes in the populated buffer.
**---------------------------------------------------------------------------
*/
INT32 pduBuild( EPATH* pPDU, UINT8* pPacketPtr )
{
	UINT8* pData = pPacketPtr;
	INT32 i;
	EtIPDeviceID deviceId;
	ETAG* pTag;
	UINT16 iSize;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduBuild Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags; 
		i < pPDU->bNumTags;
		i++, pTag++ )
	{
		switch ( pTag->iTagType )
		{
		case TAG_TYPE_CLASS:
			switch (pTag->iTagDataSize)
			{
			case 1:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT; /* One byte class logical segment */
				*pData++ = (UINT8)(pTag->TagData);
				break;
			case 2:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_16_BIT; /* Two byte class logical segment */

				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT16_SET(pData, (UINT16)(pTag->TagData));
				pData += sizeof(UINT16);
				break;
			case 4:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_32_BIT; /* Four byte class logical segment */

				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT32_SET(pData, (UINT32)(pTag->TagData));
				pData += sizeof(UINT32);
				break;
			}
			break;

		case TAG_TYPE_INSTANCE:
			switch (pTag->iTagDataSize)
			{
			case 1:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT; /* One byte instance logical segment */
				*pData++ = (UINT8)(pTag->TagData);
				break;
			case 2:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_16_BIT; /* Two byte instance logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT16_SET(pData, (UINT16)(pTag->TagData));
				pData += sizeof(UINT16);
				break;
			case 4:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_32_BIT; /* Four byte instance logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT32_SET(pData, (UINT32)(pTag->TagData));
				pData += sizeof(UINT32);
				break;
			}
			break;

		case TAG_TYPE_ATTRIBUTE:
			switch (pTag->iTagDataSize)
			{
			case 1:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_ATTRIBUTE | LOGICAL_SEG_8_BIT; /* One byte attribute logical segment */
				*pData++ = (UINT8)(pTag->TagData);
				break;
			case 2:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_ATTRIBUTE | LOGICAL_SEG_16_BIT; /* Two byte attribute logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT16_SET(pData, (UINT16)(pTag->TagData));
				pData += sizeof(UINT16);
				break;
			case 4:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_ATTRIBUTE | LOGICAL_SEG_32_BIT; /* Four byte attribute logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT32_SET(pData, (UINT32)(pTag->TagData));
				pData += sizeof(UINT32);
				break;
			}
			break;

		case TAG_TYPE_MEMBER:
			switch (pTag->iTagDataSize)
			{
			case 1:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_MEMBER | LOGICAL_SEG_8_BIT; /* One byte member logical segment */
				*pData++ = (UINT8)(pTag->TagData);
				break;
			case 2:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_MEMBER | LOGICAL_SEG_16_BIT; /* Two byte member logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT16_SET(pData, (UINT16)(pTag->TagData));
				pData += sizeof(UINT16);
				break;
			case 4:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_MEMBER | LOGICAL_SEG_32_BIT; /* Four byte member logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT32_SET(pData, (UINT32)(pTag->TagData));
				pData += sizeof(UINT32);
				break;
			}
			break;

		case TAG_TYPE_CONN_POINT:
			switch (pTag->iTagDataSize)
			{
			case 1:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CONN_POINT | LOGICAL_SEG_8_BIT; /* One byte connection point logical segment */
				*pData++ = (UINT8)(pTag->TagData);
				break;
			case 2:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CONN_POINT | LOGICAL_SEG_16_BIT; /* Two byte connection point logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT16_SET(pData, (UINT16)(pTag->TagData));
				pData += sizeof(UINT16);
				break;
			case 4:
				*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_CONN_POINT | LOGICAL_SEG_32_BIT; /* Four byte connection point logical segment */
				if ( pPDU->bPadded )
					*pData++ = 0;			/* Insert a pad byte */
				UINT32_SET(pData, (UINT32)(pTag->TagData));
				pData += sizeof(UINT32);
				break;
			}
			break;

		case TAG_TYPE_EXT_SYMBOL:
			*pData++ = EXT_SYMBOL_SEGMENT;
			*pData++ = (UINT8)(pTag->iTagDataSize);

			memcpy(pData, (UINT8*)(pTag->TagData), pTag->iTagDataSize);
			pData += pTag->iTagDataSize;

			if ( pPDU->bPadded && (pTag->iTagDataSize % 2) ) /* add a pad byte */
				*pData++ = 0;
			break;

		case TAG_TYPE_PORT:
			if ( pTag->iTagDataSize == sizeof(UINT8))		/* One byte port number */
			{
				/* If the next segment is Extended Link then we have to include that flag here */
				if ( i < (pPDU->bNumTags-1) && (pTag+1)->iTagType == TAG_TYPE_LINK_ADDRESS )
					*pData++ = (UINT8)(PATH_SEGMENT | EXTENDED_LINK_MASK | ((UINT8)pTag->TagData));
				else
					*pData++ = (UINT8)(PATH_SEGMENT | ((UINT8)pTag->TagData));
			}
			else if (pTag->iTagDataSize == sizeof(UINT16))	/* Two byte port number */
			{
				/* If the next segment is Extended Link then we have to include that flag here */
				if ( i < (pPDU->bNumTags-1) && (pTag+1)->iTagType == TAG_TYPE_LINK_ADDRESS )
					*pData++ = PATH_SEGMENT | EXTENDED_LINK_MASK | EXTENDED_PORT_FLAG;
				else
					*pData++ = PATH_SEGMENT | EXTENDED_PORT_FLAG;
			}

			/* If the next segment is Extended Link then we have to include the length here */
			if ( i < (pPDU->bNumTags-1) && (pTag+1)->iTagType == TAG_TYPE_LINK_ADDRESS )
				*pData++ = (UINT8)(pTag+1)->iTagDataSize;

			if ( pTag->iTagDataSize == sizeof(UINT16))		/* Two byte port number */
			{
				UINT16_SET( pData, (UINT16)(pTag->TagData) );
				pData += sizeof(UINT16);
			}
			break;

		case TAG_TYPE_SLOT:
			*pData++ = (UINT8)(pTag->TagData);
			break;

		case TAG_TYPE_LINK_ADDRESS:
			memcpy(pData, (UINT8*)(pTag->TagData), pTag->iTagDataSize);
			pData += pTag->iTagDataSize;

			if ( pPDU->bPadded && (pTag->iTagDataSize % 2) ) /* add a pad byte */
				*pData++ = 0;
			break;

		case TAG_TYPE_KEY:
			*pData++ = LOGICAL_SEGMENT | LOGICAL_SEG_KEY;
			*pData++ = COMMON_KEY_FORMAT;
			memcpy( &deviceId, (UINT8*)(pTag->TagData), sizeof(EtIPDeviceID) );
			ENCAP_CVT_HS(deviceId.iVendorID);
			ENCAP_CVT_HS(deviceId.iProductCode);
			ENCAP_CVT_HS(deviceId.iProductType);
			memcpy( pData, &deviceId, sizeof(EtIPDeviceID) );
			pData += sizeof(EtIPDeviceID);
			break;

		case TAG_TYPE_INHIBIT_INTERVAL:
			*pData++ = NETWORK_SEGMENT | PRODUCTION_INHIBIT_TYPE;
			*pData++ = (UINT8)pTag->TagData;
			break;

		case TAG_TYPE_SYMBOL:
			*pData++ = (UINT8)(SYMBOLIC_SEGMENT | (UINT8)(pTag->iTagDataSize));

			memcpy(pData, (UINT8*)(pTag->TagData), pTag->iTagDataSize);
			pData += pTag->iTagDataSize;

			if ( pPDU->bPadded && !(pTag->iTagDataSize % 2) ) /* add a pad byte */
				*pData++ = 0;
			break;

		case TAG_TYPE_DATA:
		case TAG_TYPE_DATA2:
			*pData++ = SIMPLE_DATA_SEGMENT;

			iSize = pTag->iTagDataSize;
			/* Join 2 subsequent data segment together if any */
			if ((i < (pPDU->bNumTags-1)) &&
				(((pTag+1)->iTagType == TAG_TYPE_DATA) || ((pTag+1)->iTagType == TAG_TYPE_DATA2)))
				iSize += (pTag+1)->iTagDataSize;

			*pData++ = (UINT8)( (iSize%2) ? (iSize+1)/2 : iSize/2 );

			memcpy(pData, (UINT8*)(pTag->TagData), pTag->iTagDataSize);
			pData += pTag->iTagDataSize;

			/* Join 2 subsequent data segment together */
			if ((i < (pPDU->bNumTags-1)) &&
				(((pTag+1)->iTagType == TAG_TYPE_DATA) || ((pTag+1)->iTagType == TAG_TYPE_DATA2)))
			{
				memcpy(pData, (UINT8*)((pTag+1)->TagData), (pTag+1)->iTagDataSize);
				pData += (pTag+1)->iTagDataSize;
				i++, pTag++;
			}

			/* Add the pad byte */
			if ( iSize%2 )
				*pData++ = 0;

			break;

		default:
			break;
		}
	}

	return (INT32)(pData - pPacketPtr);
}

/*---------------------------------------------------------------------------
** pduGetBuildLen( )
**
** Return number of bytes that PDU will take in the buffer.
**---------------------------------------------------------------------------
*/
INT32 pduGetBuildLen( EPATH* pPDU )
{
	INT32 nCount = 0;
	INT32 i;
	ETAG* pTag;
	UINT16 iSize;

	/* Check for any failure to initialize the pdu structure */
	if ( pPDU->bNumTags > MAX_TAGS )
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "pduGetBuildLen Internal Logic FAILURE pPDU->bNumTags set to %d", pPDU->bNumTags );
		pPDU->bNumTags = 0;
	}

	for( i = 0, pTag = pPDU->Tags;
		 i < pPDU->bNumTags;
		 i++, pTag++)
	{
		switch ( pTag->iTagType )
		{
		case TAG_TYPE_CLASS:
		case TAG_TYPE_INSTANCE:
		case TAG_TYPE_MEMBER:
		case TAG_TYPE_CONN_POINT:
		case TAG_TYPE_ATTRIBUTE:
			switch(pTag->iTagDataSize)
			{
			case 1:
				nCount += 2;
				break;
			case 2:
				if ( pPDU->bPadded )
					nCount += 4;
				else
					nCount += 3;
				break;
			case 4:
				if ( pPDU->bPadded )
					nCount += 6;
				else
					nCount += 5;
				break;
			}
			break;

		case TAG_TYPE_EXT_SYMBOL:
		case TAG_TYPE_SYMBOL:
			nCount += (2 + pTag->iTagDataSize);
			if ( pPDU->bPadded && (pTag->iTagDataSize % 2) ) /* add a pad byte */
				nCount++;
			break;

		case TAG_TYPE_PORT:
			if ( pTag->iTagDataSize == 2 )
				nCount += 3;
			else
				nCount += 1;
			if ( i < (pPDU->bNumTags-1) && (pTag+1)->iTagType == TAG_TYPE_LINK_ADDRESS )
				nCount++;
			break;

		case TAG_TYPE_SLOT:
			nCount++;
			break;

		case TAG_TYPE_LINK_ADDRESS:
			nCount += pTag->iTagDataSize;
			if ( pPDU->bPadded && (pTag->iTagDataSize % 2) ) /* add a pad byte */
				nCount++;
			break;

		case TAG_TYPE_KEY:
			nCount += (2 + sizeof(EtIPDeviceID));
			break;

		case TAG_TYPE_INHIBIT_INTERVAL:
			nCount += 2;
			break;

		case TAG_TYPE_DATA:
		case TAG_TYPE_DATA2:
			nCount += 2;

			iSize = pTag->iTagDataSize;
			/* Join 2 subsequent data segment together if any */
			if ((i < (pPDU->bNumTags-1)) &&
				(((pTag+1)->iTagType == TAG_TYPE_DATA) || ((pTag+1)->iTagType == TAG_TYPE_DATA2)))
			{
				iSize += (pTag+1)->iTagDataSize;
				i++; pTag++;
			}

			nCount += (iSize%2) ? (iSize+1) : iSize;
			break;

		default:
			break;
		}
	}

	return nCount;
}

/*---------------------------------------------------------------------------
** pduParseNetworkPath( )
**
** Store extended network path in PDU format.
** Return TRUE if network path is valid, FALSE otherwise.
**---------------------------------------------------------------------------
*/
BOOL pduParseNetworkPath( char* networkPath, EPATH* pPDU )
{
	char szNetworkPath[MAX_NETWORK_PATH_SIZE];
	UINT32 nLen = (UINT32)strlen( networkPath );
	UINT16 i;
	BOOL  bPortFilled = FALSE;
	INT32 nPort = 0;
	UINT8  szPort[64];
	UINT8 *pPort = szPort;
	INT32 nNode;
	UINT8  szNode[128];
	INT32 nNodeSize = 0;
	char *pNetworkPath;
	BOOL bLocal = TRUE;
	ETAG* pPDUTag = &pPDU->Tags[pPDU->bNumTags];

	/* Add trailing 0 if missing */
	memcpy(szNetworkPath, networkPath, nLen);
	szNetworkPath[nLen] = 0;
	nLen++;

	for( i = 0, pNetworkPath = szNetworkPath; i < nLen; i++, pNetworkPath++)
	{
		if ( bLocal )
		{
			if ( *pNetworkPath == ',' || *pNetworkPath == 0 )
			{
				if ( *pNetworkPath == ',' )
				{
					if ( szNetworkPath[nLen-1] != ',' )    /* If the last char is not ',' add it now */
					{
						szNetworkPath[nLen-1] = ',';
						szNetworkPath[nLen] = 0;
					}

					bLocal = FALSE;
					networkPath[i] = 0;
				}

				pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc((UINT8*)szNetworkPath, i);
				pPDUTag->iTagDataSize = i;
				pPDUTag->iTagType = TAG_TYPE_LINK_ADDRESS;
				pPDU->bNumTags++;
				pPDUTag++;
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

					pPDUTag->TagData = (ETAG_DATATYPE)nPort;
					pPDUTag->iTagDataSize = (UINT16)(nPort > 0xff ? 2 : 1);
					pPDUTag->iTagType = TAG_TYPE_PORT;
					pPDU->bNumTags++; pPDUTag++;
				}
				else
				{
					if ( nNodeSize == 0 )				/* 2 commas in a row are not allowed */
					    return FALSE;
					szNode[nNodeSize] = 0;
					/* Check if this is one byte presentation of the slot or any other network path */
					/* If it is one byte representation the length is limited to 3 numeric chars */
					if ( ( nNodeSize == 1 && szNode[0] >= '0' && szNode[0] <= '9' ) ||
						 ( nNodeSize == 2 && szNode[0] >= '0' && szNode[0] <= '9' && szNode[1] >= '0' && szNode[1] <= '9' ) ||
						 ( nNodeSize == 3 && szNode[0] >= '0' && szNode[0] <= '9' && szNode[1] >= '0' && szNode[1] <= '9' && szNode[2] >= '0' && szNode[2] <= '9' ) )
					{
						nNode = atol( (const char*)szNode );
						if ( nNode < 0 || nNode > 0xff ) /* Numeric node is limited to 8 bits */
							return FALSE;

						pPDUTag->TagData = (ETAG_DATATYPE)nNode;
						pPDUTag->iTagDataSize = 1;
						pPDUTag->iTagType = TAG_TYPE_SLOT;
						pPDU->bNumTags++; pPDUTag++;
					}
					else			/* We have a string presentation of the network path */
					{
						if ( platformAddrFromPath( (const char*)szNode, (INT32)strlen((const char*)szNode) ) == (UINT32)ERROR_STATUS )
							return FALSE;

						szNode[nNodeSize] = 0;
						pPDUTag->TagData = (ETAG_DATATYPE)EtIP_malloc((UINT8*)szNode, (UINT16)nNodeSize);
						pPDUTag->iTagDataSize = (UINT16)nNodeSize;
						pPDUTag->iTagType =  TAG_TYPE_LINK_ADDRESS;
						pPDU->bNumTags++; pPDUTag++;
					}

					nNodeSize = 0;
					pPort = szPort;
					bPortFilled = FALSE;
				}
			}
		}
	}

	return TRUE;
}
