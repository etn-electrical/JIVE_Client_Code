/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPCCC.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** PCCC object implementation 
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef EIP_PCCC_SUPPORT

static const UINT8 PCCC_EXTENSION_COMMAND_LIST[] = {0x06, 0x07, 0x0E, 0x0F};

/*---------------------------------------------------------------------------
** pcccIsExtentionCommandList( )
**
** Is command a supported "extended" PCCC command
**---------------------------------------------------------------------------
*/
BOOL pcccIsExtentionCommandList(UINT8 bCommand)
{
	INT32 i;

	for (i = 0; i < (INT32)sizeof(PCCC_EXTENSION_COMMAND_LIST); i++)
	{
		if ((bCommand & PCCC_COMMAND_COMMAND_MASK) == PCCC_EXTENSION_COMMAND_LIST[i])
			return TRUE;
	}

	return FALSE;
}

/*---------------------------------------------------------------------------
** pcccSendClassAttrAll( )
**
** GetAttributeAll service for PCCC class is received
**---------------------------------------------------------------------------
*/
static void pcccSendClassAttrAll( REQUEST* pRequest )
{
	PCCC_CLASS_ATTRIBUTE pcccAttr;

	pcccAttr.iRevision = ENCAP_TO_HS(PCCC_CLASS_REVISION);
	pcccAttr.iMaxInstance = ENCAP_TO_HS(1);
	pcccAttr.iNumInstance = ENCAP_TO_HS(1);
	pcccAttr.iMaxClassAttr = ENCAP_TO_HS(PCCC_CLASS_ATTR_MAX_INST_ATTR);
	pcccAttr.iMaxInstanceAttr = ENCAP_TO_HS(0);

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pcccAttr, sizeof(PCCC_CLASS_ATTRIBUTE) );
}

/*--------------------------------------------------------------------------------
** idSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the PCCC class
**--------------------------------------------------------------------------------
*/
static void pcccSendClassAttrSingle( REQUEST* pRequest)
{
	UINT16 iVal;
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch( iAttribute )
	{
	case PCCC_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(PCCC_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PCCC_CLASS_ATTR_MAX_INSTANCE:
		iVal = ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PCCC_CLASS_ATTR_NUM_INSTANCES:
		iVal = ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PCCC_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(PCCC_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PCCC_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(0);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** pcccExecuteService( )
**
** Handle Execute PCCC service
**---------------------------------------------------------------------------
*/
static BOOL pcccExecuteService( REQUEST* pRequest)
{
	UINT8* pData = pRequest->cfg.objectCfg.pData;
	UINT8 bLength;

	if (pRequest->cfg.objectCfg.iDataSize < 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return TRUE;
	}

	bLength = *pData;
	if ((bLength > pRequest->cfg.objectCfg.iDataSize) ||
		(pRequest->cfg.objectCfg.iDataSize - bLength < 4))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
	notifyEvent( NM_CLIENT_PCCC_REQUEST_RECEIVED, pRequest->nIndex, 0 );
	return FALSE;
}

/*---------------------------------------------------------------------------
** pcccExecuteLocalPCCC( )
**
** Handle Execute Local PCCC service
**---------------------------------------------------------------------------
*/
static BOOL pcccExecuteLocalPCCC( REQUEST* pRequest)
{
	if (pRequest->cfg.objectCfg.iDataSize < 4)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	pRequest->nState = REQUEST_WAITING_FOR_RESPONSE;
	notifyEvent( NM_CLIENT_PCCC_REQUEST_RECEIVED, pRequest->nIndex, 0 );
	return FALSE;
}

/*---------------------------------------------------------------------------
** pcccParseClassRequest( )
**
** Respond to the class request
**---------------------------------------------------------------------------
*/
static void pcccParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		pcccSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		pcccSendClassAttrSingle( pRequest);
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** pcccParseInstanceRequest( )
**
** Respond to the instance request
**---------------------------------------------------------------------------
*/
static BOOL pcccParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	if (iInstance != 1)
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return TRUE;
	}

	switch( pRequest->cfg.objectCfg.bService )
	{
	case PCCC_EXECUTE_SERVICE:
		return pcccExecuteService(pRequest);
	case PCCC_EXECUTE_LOCAL_PCCC:
		return pcccExecuteLocalPCCC(pRequest);
	}

	pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
	return TRUE;
}

/*---------------------------------------------------------------------------
** pcccParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
BOOL pcccParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if ( iInstance == 0 )
	{
		pcccParseClassRequest( pRequest );
		return TRUE;
	}

	return pcccParseInstanceRequest( pRequest, iInstance );

}
#endif
