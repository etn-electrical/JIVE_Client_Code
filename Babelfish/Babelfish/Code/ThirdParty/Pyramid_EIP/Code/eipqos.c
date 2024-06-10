/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPQOS.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** QoS object implementation
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef EIP_QOS

/*---------------------------------------------------------------------------
** qosInit( )
**
** Initialize QoS object
**---------------------------------------------------------------------------
*/
void qosInit(QOS_INST_ATTR* pQOSAttr)
{
	memset(pQOSAttr, 0, sizeof(QOS_INST_ATTR));
}

/*---------------------------------------------------------------------------
** qosSendClassAttrSingle( )
**
** GetAttributeSingle service for QoS class is received
**---------------------------------------------------------------------------
*/
static void qosSendClassAttrSingle( REQUEST* pRequest )
{
	UINT16 iVal, iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);

	switch( iAttribute )
	{
	case QOS_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(QOS_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case QOS_CLASS_ATTR_MAX_INSTANCE:
		iVal = (UINT16)ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case QOS_CLASS_ATTR_NUM_INSTANCES:
		iVal = (UINT16)ENCAP_TO_HS(1);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case QOS_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(QOS_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case QOS_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(QOS_INST_ATTR_DSCP_EXPLICIT);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*--------------------------------------------------------------------------------
** qosSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the QoS object instance
**--------------------------------------------------------------------------------
*/
static void qosSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize);
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface = &gsTcpIpInstAttr[iInstance-1];
	QOS_INST_ATTR* pAttr = &pTcpipInterface->QoSConfig;

	switch( iAttribute )
	{
	case QOS_INST_ATTR_8021Q_TAG_ENABLE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->b8021QTagEnable, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_PTP_EVENT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPPTPEvent, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_PTP_GENERAL:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPPTPGeneral, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_URGENT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPUrgent, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_SCHEDULED:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPScheduled, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_HIGH:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPHigh, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_LOW:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPLow, sizeof(UINT8));
		break;
	case QOS_INST_ATTR_DSCP_EXPLICIT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pAttr->bDSCPExplicit, sizeof(UINT8));
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*--------------------------------------------------------------------------------
** qosSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the QoS object instance
**--------------------------------------------------------------------------------
*/
static void qosSetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT8  bValue;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig = &gsTcpIpInstAttr[iInstance-1];
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES TempConfig;

	switch( iAttribute )
	{
	case QOS_INST_ATTR_8021Q_TAG_ENABLE:
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

		bValue = *pRequest->cfg.objectCfg.pData;
		if ((bValue != 0) && (bValue != 1))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.b8021QTagEnable = bValue;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_8021Q_TAG_ENABLE), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_PTP_EVENT:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}


		TempConfig.QoSConfig.bDSCPPTPEvent = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_PTP_EVENT), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_PTP_GENERAL:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPPTPGeneral = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_PTP_GENERAL), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_URGENT:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPUrgent = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_URGENT), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_SCHEDULED:
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
		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPScheduled = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_SCHEDULED), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_HIGH:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPHigh = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_HIGH), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_LOW:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPLow = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_LOW), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	case QOS_INST_ATTR_DSCP_EXPLICIT:
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

		if (*pRequest->cfg.objectCfg.pData > QOS_MAX_DSCP_VALUE)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}

		TempConfig.QoSConfig.bDSCPExplicit = *pRequest->cfg.objectCfg.pData;
		TempConfig.InterfaceConfig.lIpAddr = pTcpipConfig->InterfaceConfig.lIpAddr;

		if (!platformSetQoSAttributes((1 << QOS_INST_ATTR_DSCP_EXPLICIT), &TempConfig))
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			break;
		}
		break;

	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*---------------------------------------------------------------------------
** qosParseInstanceRequest( )
**
** Respond to the QoS instance request
**---------------------------------------------------------------------------
*/
static void qosParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	if ((iInstance >= 1) && (iInstance <= gnClaimedHostIPAddrCount))
	{
		switch( pRequest->cfg.objectCfg.bService )
		{
		case SVC_GET_ATTR_SINGLE:
			qosSendInstanceAttrSingle( pRequest, iInstance );
			break;
		case SVC_SET_ATTR_SINGLE:
			qosSetInstanceAttrSingle( pRequest, iInstance );
			break;
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
** qosParseClassRequest( )
**
** Respond to the QoS class request
**---------------------------------------------------------------------------
*/
static void qosParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_SINGLE:
		qosSendClassAttrSingle( pRequest );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** qosParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void qosParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);

	if ( iInstance == 0 )
		qosParseClassRequest( pRequest );
	else
		qosParseInstanceRequest( pRequest, iInstance );
}

#endif /* #ifdef EIP_QOS */
