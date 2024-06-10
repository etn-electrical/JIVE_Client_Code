/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPETLNK.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet Link object implementation 
**
*****************************************************************************
*/

#include "eipinc.h"

static UINT16 enetlinkOptionalAttributes[ENETLINK_ATTR_OPT_COUNT] = {
#ifdef EIP_BIG12
	ENETLINK_INST_ATTR_INTERFACE_COUNTERS, ENETLINK_INST_ATTR_MEDIA_COUNTERS,
#endif
	ENETLINK_INST_ATTR_INTERFACE_TYPE, ENETLINK_INST_ATTR_INTERFACE_STATE, ENETLINK_INST_ATTR_ADMIN_STATE, ENETLINK_INST_ATTR_INTERFACE_LABEL
#ifdef EIP_BIG12
	, ENETLINK_INST_ATTR_ETHERNET_ERRORS, ENETLINK_INST_ATTR_LINK_DOWN_COUNTER
#endif
};

/*---------------------------------------------------------------------------
** enetlinkInit( )
**
** Initialize Ethernet Link object
**---------------------------------------------------------------------------
*/

void enetlinkInit( ENETLINK_INST_ATTR* pEnetLinkAttr )
{
	memset(pEnetLinkAttr, 0, sizeof(ENETLINK_INST_ATTR));
}

/*---------------------------------------------------------------------------
** enetlinkSendClassAttrAll( )
**
** GetAttributeAll service for Ethernet Link class is received
**---------------------------------------------------------------------------
*/
static void enetlinkSendClassAttrAll( REQUEST* pRequest )
{
	UINT8 *pData;
	UINT16 iVal, iNumberOfAttributes, i;
	UINT16 *pAttrData;

	iNumberOfAttributes = sizeof(enetlinkOptionalAttributes)/sizeof(UINT16);

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT16)*(7+iNumberOfAttributes)));
	if (pRequest->cfg.objectCfg.pData == NULL)
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET(pData, ENETLINK_CLASS_REVISION);
	pData += sizeof(UINT16);
	iVal = (UINT16)platformGetNumEnetLinkInstances();
	UINT16_SET(pData, iVal);
	pData += sizeof(UINT16);
	iVal = (UINT16)platformGetNumEnetLinkInstances();
	UINT16_SET(pData, iVal);
	pData += sizeof(UINT16);
	UINT16_SET(pData, iNumberOfAttributes);
	pData += sizeof(UINT16);
	for (i = 0, pAttrData = enetlinkOptionalAttributes; 
		i < iNumberOfAttributes; 
		i++, pAttrData++, pData += sizeof(UINT16))
	{
		UINT16_SET(pData, *pAttrData);
	}

	UINT16_SET(pData, 0);
	pData += sizeof(UINT16);
	UINT16_SET(pData, ENETLINK_CLASS_ATTR_MAX_INST_ATTR);
	pData += sizeof(UINT16);
#ifdef EIP_BIG12
	UINT16_SET(pData, ENETLINK_INST_ATTR_LINK_DOWN_COUNTER);
#else
	UINT16_SET(pData, ENETLINK_INST_ATTR_INTERFACE_CAPABILITY);
#endif
	pData += sizeof(UINT16);
}

/*---------------------------------------------------------------------------
** enetlinkSendClassAttrSingle( )
**
** GetAttributeSingle service for Ethernet Link class is received 
**---------------------------------------------------------------------------
*/
static void enetlinkSendClassAttrSingle( REQUEST* pRequest )
{
	UINT16 iVal, iTagSize, iNumberOfAttributes, i;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	UINT8* pData;
	UINT16 *pAttrData;

	switch( iAttribute )
	{
	case ENETLINK_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(ENETLINK_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ENETLINK_CLASS_ATTR_MAX_INSTANCE:
		iVal = (UINT16)ENCAP_TO_HS(platformGetNumEnetLinkInstances());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ENETLINK_CLASS_ATTR_NUM_INSTANCES:
		iVal = (UINT16)ENCAP_TO_HS(platformGetNumEnetLinkInstances());
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ENETLINK_CLASS_ATTR_OPT_ATTR_LIST:
		iNumberOfAttributes = sizeof(enetlinkOptionalAttributes)/sizeof(UINT16);
		EtIP_realloc(&pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)((iNumberOfAttributes+1)*sizeof(UINT16)));

		pData = pRequest->cfg.objectCfg.pData;
		UINT16_SET(pData, iNumberOfAttributes);
		pData += sizeof(UINT16);

		for (i = 0, pAttrData = enetlinkOptionalAttributes;
			i < iNumberOfAttributes;
			i++, pAttrData++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *pAttrData);
		}
		break;
	case ENETLINK_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(ENETLINK_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ENETLINK_CLASS_ATTR_MAX_INST_ATTR:
#ifdef EIP_BIG12
		iVal = ENCAP_TO_HS(ENETLINK_INST_ATTR_LINK_DOWN_COUNTER);
#else
		iVal = ENCAP_TO_HS(ENETLINK_INST_ATTR_INTERFACE_CAPABILITY);
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** enetlinkSendInstanceAttrAll( )
**
** GetAttributeAll service for Ethernet Link instance is received
**---------------------------------------------------------------------------
*/
static void enetlinkSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;
	ENETLINK_INST_ATTR EnetLinkConfig;
	UINT8  bInterfaceLabelSize, i;
	UINT16 iMessageSize;
#ifdef EIP_BIG12
	UINT32 lEthernetErrors = 0;
#endif

	/* dynamically get Ethernet Link attributes */
	platformGetEnetLinkAttributes(iInstance, &EnetLinkConfig);
	bInterfaceLabelSize = (UINT8)strlen((char*)EnetLinkConfig.InterfaceLabel);

    iMessageSize = (UINT16)(6*sizeof(UINT32) + MAC_ADDR_LENGTH + ENETLINK_INTERFACE_COUNTER_SIZE + ENETLINK_MEDIA_COUNTER_SIZE +
							5*sizeof(UINT8) + bInterfaceLabelSize + ENETLINK_HC_INTERFACE_COUNTER_SIZE + ENETLINK_HC_MEDIA_COUNTER_SIZE);
	iMessageSize += (UINT16)(EnetLinkConfig.Capabilities.bSpeedDuplexCount*(sizeof(UINT16)+sizeof(UINT8)));

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iMessageSize);

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		/* Out of memory */
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT32_SET( pData, EnetLinkConfig.lInterfaceSpeed);
	pData += sizeof(UINT32);
	UINT32_SET( pData, EnetLinkConfig.lInterfaceFlags );
	pData += sizeof(UINT32);

	memcpy( pData, EnetLinkConfig.macAddr, MAC_ADDR_LENGTH );
	pData += MAC_ADDR_LENGTH;

#ifdef EIP_BIG12
	for (i = 0; i < ENETLINK_NUM_INTERFACE_COUNTERS; i++)
	{
		UINT32_SET(pData, EnetLinkConfig.aInterfaceCounters[i]);
		pData += sizeof(UINT32);
	}
	for (i = 0; i < ENETLINK_NUM_MEDIA_COUNTERS; i++)
	{
		UINT32_SET(pData, EnetLinkConfig.aMediaCounters[i]);
		pData += sizeof(UINT32);
	}
#else
	/* space holders for unimplemented attributes */
	memset(pData, 0, ENETLINK_INTERFACE_COUNTER_SIZE + ENETLINK_MEDIA_COUNTER_SIZE);
	pData += (ENETLINK_INTERFACE_COUNTER_SIZE + ENETLINK_MEDIA_COUNTER_SIZE);
#endif

#ifdef EIP_INTERFACE_CONTROL_SUPPORT
	UINT16_SET(pData, EnetLinkConfig.iInterfaceControlBits);
	pData += sizeof(UINT16);
	UINT16_SET(pData, EnetLinkConfig.iForcedInterfaceSpeed);
	pData += sizeof(UINT16);
#else
	UINT32_SET(pData, 0);
	pData += sizeof(UINT32);
#endif

	*pData++ = EnetLinkConfig.bInterfaceType;
	*pData++ = EnetLinkConfig.bInterfaceState;
	*pData++ = EnetLinkConfig.bAdminState;
	*pData++ = bInterfaceLabelSize;

	memcpy( pData, EnetLinkConfig.InterfaceLabel, bInterfaceLabelSize);
	pData += bInterfaceLabelSize;

	UINT32_SET(pData, EnetLinkConfig.Capabilities.capabilities);
	pData += sizeof(UINT32);
	*pData++ = EnetLinkConfig.Capabilities.bSpeedDuplexCount;
	for (i = 0; i < EnetLinkConfig.Capabilities.bSpeedDuplexCount; i++)
	{
		UINT16_SET(pData, EnetLinkConfig.Capabilities.aInterfaceSpeed[i]);
		pData += sizeof(UINT16);
		*pData++ = EnetLinkConfig.Capabilities.aInterfaceDuplexMode[i];
	}

	/* pad HC attributes*/
	memset(pData, 0, sizeof(UINT64)*14);
	pData += (sizeof(UINT64)*14);

#ifdef EIP_BIG12
	lEthernetErrors = (EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_IN_DISCARDS] +
										EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_IN_ERRORS] +
										EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_OUT_DISCARDS] +
										EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_OUT_ERRORS]);
	for (i = 0; i < ENETLINK_NUM_MEDIA_COUNTERS; i++)
	{
		lEthernetErrors += EnetLinkConfig.aMediaCounters[i];
	}
	UINT32_SET(pData, lEthernetErrors);
	pData += sizeof(UINT32);
	UINT32_SET(pData, EnetLinkConfig.lLinkDownCounter);
#else
	UINT32_SET(pData, 0);
	pData += sizeof(UINT32);
	UINT32_SET(pData, 0);
#endif
}


/*--------------------------------------------------------------------------------
** enetlinkSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Ethernet Link object instance
**--------------------------------------------------------------------------------
*/
static void enetlinkSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;
	UINT32 lVal;
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	ENETLINK_INST_ATTR EnetLinkConfig;
	UINT8  bInterfaceLabelSize, i;
#ifdef EIP_BIG12
	UINT32 lEthernetErrors = 0;
#endif

	/* dynamically get Ethernet Link attributes */
	platformGetEnetLinkAttributes(iInstance, &EnetLinkConfig);
	bInterfaceLabelSize = (UINT8)strlen((char*)EnetLinkConfig.InterfaceLabel);

	switch( iAttribute )
	{
	case ENETLINK_INST_ATTR_INTERFACE_SPEED:
		lVal = ENCAP_TO_HL(EnetLinkConfig.lInterfaceSpeed);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
	case ENETLINK_INST_ATTR_INTERFACE_FLAGS:
		lVal = ENCAP_TO_HL(EnetLinkConfig.lInterfaceFlags);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
	case ENETLINK_INST_ATTR_PHYSICAL_ADDRESS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, EnetLinkConfig.macAddr, MAC_ADDR_LENGTH );
		break;
#ifdef EIP_BIG12
	case ENETLINK_INST_ATTR_INTERFACE_COUNTERS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, ENETLINK_INTERFACE_COUNTER_SIZE);
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			for (i = 0; i < ENETLINK_NUM_INTERFACE_COUNTERS; i++)
			{
				UINT32_SET(pData, EnetLinkConfig.aInterfaceCounters[i]);
				pData += sizeof(UINT32);
			}
		}
		break;
	case ENETLINK_INST_ATTR_MEDIA_COUNTERS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, ENETLINK_MEDIA_COUNTER_SIZE);
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			for (i = 0; i < ENETLINK_NUM_MEDIA_COUNTERS; i++)
			{
				UINT32_SET(pData, EnetLinkConfig.aMediaCounters[i]);
				pData += sizeof(UINT32);
			}
		}
		break;
#endif
#ifdef EIP_INTERFACE_CONTROL_SUPPORT
	case ENETLINK_INST_ATTR_INTERFACE_CONTROL:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT16)*2));
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
				UINT16_SET(pData, EnetLinkConfig.iInterfaceControlBits);
				pData += sizeof(UINT16);
				UINT16_SET(pData, EnetLinkConfig.iForcedInterfaceSpeed);
		}
		break;
#endif
	case ENETLINK_INST_ATTR_INTERFACE_TYPE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bInterfaceType, sizeof(UINT8));
		break;
	case ENETLINK_INST_ATTR_INTERFACE_STATE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bInterfaceState, sizeof(UINT8));
		break;
	case ENETLINK_INST_ATTR_ADMIN_STATE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &EnetLinkConfig.bAdminState, sizeof(UINT8));
		break;
	case ENETLINK_INST_ATTR_INTERFACE_LABEL:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bInterfaceLabelSize + sizeof(UINT8)));
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			*pData++ = bInterfaceLabelSize;
			memcpy( (char*)pData, EnetLinkConfig.InterfaceLabel, bInterfaceLabelSize);
		}
		break;
	case ENETLINK_INST_ATTR_INTERFACE_CAPABILITY:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT32) + sizeof(UINT8) +
									(EnetLinkConfig.Capabilities.bSpeedDuplexCount*(sizeof(UINT16)+sizeof(UINT8)))));
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			UINT32_SET(pData, EnetLinkConfig.Capabilities.capabilities);
			pData += sizeof(UINT32);
			*pData++ = EnetLinkConfig.Capabilities.bSpeedDuplexCount;
			for (i = 0; i < EnetLinkConfig.Capabilities.bSpeedDuplexCount; i++)
			{
				UINT16_SET(pData, EnetLinkConfig.Capabilities.aInterfaceSpeed[i]);
				pData += sizeof(UINT16);
				*pData++ = EnetLinkConfig.Capabilities.aInterfaceDuplexMode[i];
			}
		}
		break;
#ifdef EIP_BIG12
	case ENETLINK_INST_ATTR_ETHERNET_ERRORS:
		lEthernetErrors = (EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_IN_DISCARDS] +
											EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_IN_ERRORS] +
											EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_OUT_DISCARDS] +
											EnetLinkConfig.aInterfaceCounters[ENETLINK_INTERFACE_COUNTER_OUT_ERRORS]);
		for (i = 0; i < ENETLINK_NUM_MEDIA_COUNTERS; i++)
		{
			lEthernetErrors += EnetLinkConfig.aMediaCounters[i];
		}
		lVal = ENCAP_TO_HL(lEthernetErrors);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
	case ENETLINK_INST_ATTR_LINK_DOWN_COUNTER:
		lVal = ENCAP_TO_HL(EnetLinkConfig.lLinkDownCounter);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
#endif
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*--------------------------------------------------------------------------------
** enetlinkSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Ethernet Link object instance
**--------------------------------------------------------------------------------
*/
static void enetlinkSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT8  bValue;
	UINT8* pData;
	ENETLINK_INST_ATTR NewEnetLinkConfig, OldEnetLinkConfig;
	SESSION*  pSession;
	BOOL bSendData = TRUE;

	switch( iAttribute )
	{
#ifdef EIP_INTERFACE_CONTROL_SUPPORT
	case ENETLINK_INST_ATTR_INTERFACE_CONTROL:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT16)+sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT16)+sizeof(UINT16))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		pData = pRequest->cfg.objectCfg.pData;
		NewEnetLinkConfig.iInterfaceControlBits = UINT16_GET(pData);
		if (NewEnetLinkConfig.iInterfaceControlBits & ~ENETLINK_INTERFACE_CONTROL_BITS_MASK)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		pData += sizeof(UINT16);
		NewEnetLinkConfig.iForcedInterfaceSpeed = UINT16_GET(pData);
		if (NewEnetLinkConfig.iInterfaceControlBits & ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED)
		{
			if (NewEnetLinkConfig.iForcedInterfaceSpeed != 0)
			{
				pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
				break;
			}
		}
		else if ((NewEnetLinkConfig.iForcedInterfaceSpeed != 10) &&
				 (NewEnetLinkConfig.iForcedInterfaceSpeed != 100))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		if (!platformTestSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_INTERFACE_CONTROL), iInstance, &NewEnetLinkConfig, &pRequest->bGeneralError))
			break;

		/* Values have been verified.  Send the response before setting the values,
		   otherwise the originating device may not get the response */
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->nState = REQUEST_RESPONSE_RECEIVED;
		requestService( pRequest, &bSendData );

		/* Set the data */
		platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_INTERFACE_CONTROL), iInstance, &NewEnetLinkConfig);
		break;
#endif
	case ENETLINK_INST_ATTR_ADMIN_STATE:
		if (pRequest->cfg.objectCfg.iDataSize < sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}
		if (pRequest->cfg.objectCfg.iDataSize > sizeof(UINT8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		pData = pRequest->cfg.objectCfg.pData;
		bValue = *pData;
		if ((bValue < 1) || (bValue > 2))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		NewEnetLinkConfig.bAdminState = bValue;

		if (!platformTestSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig, &pRequest->bGeneralError))
			break;

		/* Get the current value of the admin state */
		platformGetEnetLinkAttributes(iInstance, &OldEnetLinkConfig);

		if ((NewEnetLinkConfig.bAdminState == ENETLINK_ADMIN_STATE_DISABLED) &&
			(OldEnetLinkConfig.bAdminState == ENETLINK_ADMIN_STATE_ENABLED))
		{
			/* This ensures the request goes out before the "link" (IP address) is disabled */
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->nState = REQUEST_RESPONSE_RECEIVED;
			requestService( pRequest, &bSendData );

			/* remove all sessions associated with this IP address */
			for( pSession = gpnSessions-1; pSession >= gSessions; pSession-- )
			{
				if (pSession->lHostIPAddr == pRequest->lIPAddress)
					sessionRemove(pSession, FALSE);
			}

			platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig);
			return;
		}

		platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_ADMIN_STATE), iInstance, &NewEnetLinkConfig);
		break;

	case ENETLINK_INST_ATTR_INTERFACE_SPEED:
	case ENETLINK_INST_ATTR_INTERFACE_FLAGS:
	case ENETLINK_INST_ATTR_PHYSICAL_ADDRESS:
	case ENETLINK_INST_ATTR_INTERFACE_TYPE:
	case ENETLINK_INST_ATTR_INTERFACE_STATE:
	case ENETLINK_INST_ATTR_INTERFACE_LABEL:
	case ENETLINK_INST_ATTR_INTERFACE_CAPABILITY:
#ifdef EIP_BIG12
	case ENETLINK_INST_ATTR_INTERFACE_COUNTERS:
	case ENETLINK_INST_ATTR_MEDIA_COUNTERS:
	case ENETLINK_INST_ATTR_ETHERNET_ERRORS:
	case ENETLINK_INST_ATTR_LINK_DOWN_COUNTER:
#endif
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

#ifdef EIP_BIG12
/*--------------------------------------------------------------------------------
** enetlinkSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Ethernet Link object instance
**--------------------------------------------------------------------------------
*/
static void enetlinkGetAndClear( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT32 lVal;
	ENETLINK_INST_ATTR EnetLinkConfig;
	UINT8* pData;
	UINT8 i;

	/* dynamically get Ethernet Link attributes */
	platformGetEnetLinkAttributes(iInstance, &EnetLinkConfig);

	switch( iAttribute )
	{
	case ENETLINK_INST_ATTR_INTERFACE_COUNTERS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, ENETLINK_INTERFACE_COUNTER_SIZE);
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			for (i = 0; i < ENETLINK_NUM_INTERFACE_COUNTERS; i++)
			{
				UINT32_SET(pData, EnetLinkConfig.aInterfaceCounters[i]);
				pData += sizeof(UINT32);
				EnetLinkConfig.aInterfaceCounters[i] = 0; /* Clear the counter */
			}
			platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_INTERFACE_COUNTERS), iInstance, &EnetLinkConfig);
		}
		break;
	case ENETLINK_INST_ATTR_MEDIA_COUNTERS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, ENETLINK_MEDIA_COUNTER_SIZE);
		if ( pRequest->cfg.objectCfg.pData != NULL )
		{
			pData = pRequest->cfg.objectCfg.pData;
			for (i = 0; i < ENETLINK_NUM_MEDIA_COUNTERS; i++)
			{
				UINT32_SET(pData, EnetLinkConfig.aMediaCounters[i]);
				pData += sizeof(UINT32);
				EnetLinkConfig.aMediaCounters[i] = 0; /* Clear the counter */
			}
			platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_MEDIA_COUNTERS), iInstance, &EnetLinkConfig);
		}
		break;
	case ENETLINK_INST_ATTR_LINK_DOWN_COUNTER:
		lVal = ENCAP_TO_HL(EnetLinkConfig.lLinkDownCounter);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );

		EnetLinkConfig.lLinkDownCounter = 0;
		platformSetEnetLinkAttributes((1 << ENETLINK_INST_ATTR_LINK_DOWN_COUNTER), iInstance, &EnetLinkConfig);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_SERVICE_NOT_SUPPORTED_FOR_PATH;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}
#endif

/*---------------------------------------------------------------------------
** enetlinkParseInstanceRequest( )
**
** Respond to the Ethernet Link instance request
**---------------------------------------------------------------------------
*/
static void enetlinkParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	if ((iInstance >= 1) && (iInstance <= platformGetNumEnetLinkInstances()))
	{
		switch( pRequest->cfg.objectCfg.bService )
		{
		case SVC_GET_ATTR_ALL:
			enetlinkSendInstanceAttrAll( pRequest, iInstance );
			break;
		case SVC_GET_ATTR_SINGLE:
			enetlinkSendInstanceAttrSingle( pRequest, iInstance );
			break;
		case SVC_SET_ATTR_SINGLE:
			enetlinkSetInstanceAttrSingle( pRequest, iInstance );
			break;
#ifdef EIP_BIG12
		case ENETLINK_GET_AND_CLEAR:
			enetlinkGetAndClear( pRequest, iInstance );
			break;
#endif
		default:
			pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			break;
		}
	}
	else
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
	}
}

/*---------------------------------------------------------------------------
** enetlinkParseClassRequest( )
**
** Respond to the Ethernet Link class request
**---------------------------------------------------------------------------
*/
static void enetlinkParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		enetlinkSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		enetlinkSendClassAttrSingle( pRequest );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** enetlinkParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void enetlinkParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if ( iInstance == 0 )
		enetlinkParseClassRequest( pRequest );
	else
		enetlinkParseInstanceRequest( pRequest, iInstance );
}
