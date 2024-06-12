/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPORT.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
*
** Port object implementation 
**
*****************************************************************************
*/

#include "eipinc.h"

#ifndef EIP_NO_CIP_ROUTING
/* Communication objects for Ethernet (IP) port instances */
static UINT8 portAssociatedEIPPortObjects[] = { 0x03,	/* Number of objects */
												/* TCP/IP object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												TCPIP_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												0, /* Instance to be populated */
												/* QoS object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												ENETLINK_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												1, /* Only 1 QoS instance currently supported */
												/* Ethernet Link object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												ENETLINK_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												1 /* Instance if TCP/IP object doesn't have its own link path */
												};
#ifdef ET_IP_MODBUS
/* Communication objects for Modbus port instances */
static UINT8 portAssociatedModbusPortObjects[] = { 0x04,	/* Number of objects */
												/* QoS object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												MODBUS_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												1, /* Only 1 Modbus object instance currently supported */
												/* TCP/IP object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												TCPIP_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												0, /* Instance to be populated */
												/* QoS object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												QOS_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												1, /* Only 1 QoS instance currently supported */
												/* Ethernet Link object */
												0x02,  /* EPATH size */
												(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT),
												ENETLINK_CLASS,
												(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT),
												1 /* Instance if TCP/IP object doesn't have its own link path */
												};
#endif

/*---------------------------------------------------------------------------
** portSendClassAttrAll( )
**
** GetAttributeAll service for Port object class is received 
**---------------------------------------------------------------------------
*/
static void portSendClassAttrAll( REQUEST* pRequest )
{
	UINT8* pData;
	UINT16 iEntryPort;
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;
#ifdef ET_IP_MODBUS
	UINT16 iMemSize = (UINT16)(sizeof(UINT16)*(8+(gnClaimedHostIPAddrCount*4)));
#else
	UINT16 iMemSize = (UINT16)(sizeof(UINT16)*(8+(gnClaimedHostIPAddrCount*2)));
#endif

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iMemSize);

	pData = pRequest->cfg.objectCfg.pData;
	UINT16_SET(pData, PORT_CLASS_REVISION);
	pData += sizeof(UINT16);
#ifdef ET_IP_MODBUS
	UINT16_SET(pData, ((gnClaimedHostIPAddrCount*2)+1));
#else
	UINT16_SET(pData, (gnClaimedHostIPAddrCount+1));
#endif
	pData += sizeof(UINT16);
#ifdef ET_IP_MODBUS
	UINT16_SET(pData, ((gnClaimedHostIPAddrCount*2)+1));
#else
	UINT16_SET(pData, (gnClaimedHostIPAddrCount+1));
#endif
	pData += sizeof(UINT16);

	iEntryPort = 1;
	for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
		 i++, pTcpipConfig++)
	{
		if (pTcpipConfig->InterfaceConfig.lIpAddr == pRequest->lHostIPAddr)
		{
#ifdef ET_IP_MODBUS
			iEntryPort = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+((pTcpipConfig-gsTcpIpInstAttr)*2));
#else
			iEntryPort = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+(pTcpipConfig-gsTcpIpInstAttr));
#endif
			break;
		}
	}

	UINT16_SET(pData, iEntryPort);
	pData += sizeof(UINT16);

	/* The values at index 1 (offset 0) and any non-instantiated instances shall be zero. */
	UINT32_SET(pData, 0);
	pData += sizeof(UINT32);

	/* Backplane Port */
	UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_TYPE);
	pData += sizeof(UINT16);

	UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_NUMBER);
	pData += sizeof(UINT16);


	for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
		 i++, pTcpipConfig++)
	{
		UINT16_SET(pData, TCP_IP_PORT_TYPE);
		pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i*2));
#else
		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i));
#endif
		pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
		UINT16_SET(pData, TCP_IP_MODBUS_TCP_PORT_TYPE);
		pData += sizeof(UINT16);

		UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+(i*2)+1));
		pData += sizeof(UINT16);
#endif
	}
}

/*---------------------------------------------------------------------------
** portSendClassAttrSingle( )
**
** GetAttributeSingle service for Port object class is received 
**---------------------------------------------------------------------------
*/
static void portSendClassAttrSingle( REQUEST* pRequest )
{
	UINT16 iVal;
	UINT8* pData;
	UINT16 iTagSize;
	UINT32 i;
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig;

	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	switch( iAttribute )
	{
	case PORT_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(PORT_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PORT_CLASS_ATTR_MAX_INSTANCE:

#ifdef ET_IP_MODBUS
		iVal = (UINT16)ENCAP_TO_HS(((gnClaimedHostIPAddrCount*2)+1));
#else
		iVal = (UINT16)ENCAP_TO_HS(gnClaimedHostIPAddrCount+1);
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PORT_CLASS_ATTR_NUM_INSTANCES:
#ifdef ET_IP_MODBUS
		iVal = (UINT16)ENCAP_TO_HS(((gnClaimedHostIPAddrCount*2)+1));
#else
		iVal = (UINT16)ENCAP_TO_HS(gnClaimedHostIPAddrCount+1);
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PORT_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(PORT_CLASS_ATTR_ALL_PORTS);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PORT_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(PORT_INST_ATTR_ASSOC_COMMUNICATION_OBJECTS);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case PORT_CLASS_ATTR_ENTRY_PORT:
		iVal = 1;
		for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
			 i++, pTcpipConfig++)
		{
			if (pTcpipConfig->InterfaceConfig.lIpAddr == pRequest->lHostIPAddr)
			{
#ifdef ET_IP_MODBUS
				iVal = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+((pTcpipConfig-gsTcpIpInstAttr)*2));
#else
				iVal = (UINT16)(TCP_IP_STARTING_PORT_NUMBER+(pTcpipConfig-gsTcpIpInstAttr));
#endif
				break;
			}
		}
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16));
		UINT16_SET(pRequest->cfg.objectCfg.pData, iVal);
		break;
	case PORT_CLASS_ATTR_ALL_PORTS:
#ifdef ET_IP_MODBUS
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT32)+(sizeof(UINT16)*2*((gnClaimedHostIPAddrCount*2)+1))));
#else
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(sizeof(UINT32)+(sizeof(UINT16)*2*(gnClaimedHostIPAddrCount+1))));
#endif

		pData = pRequest->cfg.objectCfg.pData;

		/* The values at index 1 (offset 0) and any non-instantiated instances shall be zero. */
		UINT32_SET(pData, 0);
		pData += sizeof(UINT32);

		/* Backplane Port */
		UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_TYPE);
		pData += sizeof(UINT16);

		UINT16_SET(pData, TCP_IP_BACKPLANE_PORT_NUMBER);
		pData += sizeof(UINT16);

		for (i = 0, pTcpipConfig = gsTcpIpInstAttr; i < gnClaimedHostIPAddrCount;
			 i++, pTcpipConfig++)
		{
			UINT16_SET(pData, TCP_IP_PORT_TYPE);
			pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
			UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i*2));
#else
			UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+i));
#endif
			pData += sizeof(UINT16);

#ifdef ET_IP_MODBUS
			UINT16_SET(pData, TCP_IP_MODBUS_TCP_PORT_TYPE);
			pData += sizeof(UINT16);

			UINT16_SET(pData, (TCP_IP_STARTING_PORT_NUMBER+(i*2)+1));
			pData += sizeof(UINT16);
#endif
		}
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** portSendInstanceAttrAll( )
**
** GetAttributeAll service for Port instance is received
**---------------------------------------------------------------------------
*/
static void portSendInstanceAttrAll( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8* pData;
	UINT8  bPortNameSize;
	UINT16 iPortType, iPortNodeAddressLength, ipStrLength = 0, ipStrLengthPad = 0;
	char* ipStr = "";
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );

	if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		bPortNameSize = (UINT8)strlen(PORT_BACKPLANE_PORT_NAME);
		iPortType = TCP_IP_BACKPLANE_PORT_TYPE;
		iPortNodeAddressLength = 2;
	}
#ifdef ET_IP_MODBUS
	else if (iInstance%2 == 1)
	{
		/* Modbus/TCP port */
		bPortNameSize = (UINT8)strlen(PORT_MODBUS_TCP_PORT_NAME);
		iPortType = TCP_IP_MODBUS_TCP_PORT_TYPE;
	}
#endif
	else
	{
		/* Normal EtherNet/IP port */
		bPortNameSize = (UINT8)strlen(PORT_PORT_NAME);
		iPortType = TCP_IP_PORT_TYPE;
	}

	if (iInstance != TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		pTcpipInterface = &gsTcpIpInstAttr[(iInstance-TCP_IP_STARTING_PORT_NUMBER)/2];

		/* Get length of IP address string */
		ipStr = socketGetAddressString(pTcpipInterface->InterfaceConfig.lIpAddr);
		ipStrLength = (UINT16)strlen(ipStr);
		ipStrLengthPad = (UINT16)(((ipStrLength+1)/2)*2);
		iPortNodeAddressLength = 2+ipStrLengthPad;
	}

	pRequest->cfg.objectCfg.iDataSize = (UINT16)( sizeof(UINT16) +		/* port type */
										sizeof(UINT16) +				/* port number */
										6 +								/* port EPATH */
										1 + bPortNameSize +				/* length of port name + port name */
										iPortNodeAddressLength);		/* port node address */

	pRequest->cfg.objectCfg.pData = EtIP_malloc( NULL, pRequest->cfg.objectCfg.iDataSize );

	if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL )
	{
		notifyEvent( NM_OUT_OF_MEMORY, 0, 0 );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;

	UINT16_SET( pData, iPortType );
	pData += sizeof(UINT16);

	UINT16_SET( pData, iInstance );
	pData += sizeof(UINT16);

	if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		/* Modbus/TCP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
		*pData++ = 0x64;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
		*pData++ = TCP_IP_BACKPLANE_PORT_NUMBER;

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_BACKPLANE_PORT_NAME, bPortNameSize );  
		pData += bPortNameSize;
	}
#ifdef ET_IP_MODBUS
	else if (iInstance%2 == 1)
	{
		/* Modbus/TCP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
		*pData++ = TCPIP_CLASS;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
		*pData++ = (UINT8)(iInstance/2);

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_MODBUS_TCP_PORT_NAME, sizeof(PORT_MODBUS_TCP_PORT_NAME) );  
		pData += bPortNameSize;

	}
#endif
	else
	{
		/* Normal EtherNet/IP port */
		*pData++ = 2;
		*pData++ = 0;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
		*pData++ = TCPIP_CLASS;
		*pData++ = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
#ifdef ET_IP_MODBUS
		*pData++ = (UINT8)(iInstance/2);
#else
		*pData++ = (UINT8)(iInstance-1);
#endif

		*pData++ = bPortNameSize;
		memcpy( (char*)pData, PORT_PORT_NAME, sizeof(PORT_PORT_NAME) );  
		pData += bPortNameSize;
	}

	if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
	{
		*pData++ = 0x01;
		*pData++ = 0;
	}
	else
	{
		*pData++ = (UINT8)(0x10 | iInstance);
		*pData++ = (UINT8)ipStrLengthPad;
		memcpy( (char*)pData, ipStr, ipStrLength);
		pData += ipStrLength;
		if (ipStrLength & 1)
			*pData++ = 0;	/* Pad to even number bytes */
	}
}


/*--------------------------------------------------------------------------------
** portSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Port object instance
**--------------------------------------------------------------------------------
*/
static void portSendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT8  bNameLen;
	UINT16 iVal;
	UINT32 iVal32;
	UINT8* pData;
	UINT8  bVal[6];
	UINT16 iTagSize, ipStrLength, ipStrLengthPad;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipInterface;
	char* ipStr;

	switch( iAttribute )
	{
	case PORT_INST_ATTR_PORT_TYPE:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			iVal = ENCAP_TO_HS(TCP_IP_BACKPLANE_PORT_TYPE);
		}
#ifdef ET_IP_MODBUS
		else if (iInstance%2 == 1)
		{
			iVal = ENCAP_TO_HS(TCP_IP_MODBUS_TCP_PORT_TYPE);
		}
#endif
		else
		{
			iVal = ENCAP_TO_HS(TCP_IP_PORT_TYPE);
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;

	case PORT_INST_ATTR_PORT_NUMBER:
		iVal = (UINT16)ENCAP_TO_HS(iInstance);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;

	case PORT_INST_ATTR_PORT_OBJECT:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			bVal[0] = 2;
			bVal[1] = 0;
			bVal[2] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
			bVal[3] = 0x64;
			bVal[4] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
			bVal[5] = TCP_IP_BACKPLANE_PORT_NUMBER;
		}
#ifdef ET_IP_MODBUS
		else if (iInstance%2 == 1)
		{
			bVal[0] = 2;
			bVal[1] = 0;
			bVal[2] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
			bVal[3] = TCPIP_CLASS;
			bVal[4] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
			bVal[5] = (UINT8)(iInstance/2);
		}
#endif
		else
		{
			bVal[0] = 2;
			bVal[1] = 0;
			bVal[2] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_CLASS | LOGICAL_SEG_8_BIT);
			bVal[3] = TCPIP_CLASS;
			bVal[4] = (UINT8)(LOGICAL_SEGMENT | LOGICAL_SEG_INSTANCE | LOGICAL_SEG_8_BIT);
#ifdef ET_IP_MODBUS
			bVal[5] = (UINT8)(iInstance/2);
#else
			bVal[5] = (UINT8)(iInstance-1);
#endif
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, bVal, 6);
		break;

	case PORT_INST_ATTR_PORT_NAME:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			bNameLen = (UINT8)strlen(PORT_BACKPLANE_PORT_NAME);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bNameLen + sizeof(UINT8)) );
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			}
			else
			{
				pData = pRequest->cfg.objectCfg.pData;
				*pData++ = bNameLen;
				memcpy( (char*)pData, PORT_BACKPLANE_PORT_NAME, bNameLen);
			}
		}
#ifdef ET_IP_MODBUS
		else if (iInstance%2 == 1)
		{
			bNameLen = (UINT8)strlen(PORT_MODBUS_TCP_PORT_NAME);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bNameLen + sizeof(UINT8)) );
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			}
			else
			{
				pData = pRequest->cfg.objectCfg.pData;
				*pData++ = bNameLen;
				memcpy( (char*)pData, PORT_MODBUS_TCP_PORT_NAME, bNameLen );
			}
		}
#endif
		else
		{
			bNameLen = (UINT8)strlen(PORT_PORT_NAME);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(bNameLen + sizeof(UINT8)) );
			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			}
			else
			{
				pData = pRequest->cfg.objectCfg.pData;
				*pData++ = bNameLen;
				memcpy( (char*)pData, PORT_PORT_NAME, bNameLen );
			}
		}
		break;
	case PORT_INST_ATTR_PORT_NODE_ADDRESS:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			bVal[0] = 0x01;
			bVal[1] = 0;
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, bVal, 2);
		}
		else
		{
			pTcpipInterface = &gsTcpIpInstAttr[(iInstance-TCP_IP_STARTING_PORT_NUMBER)/2];

			/* Get length of IP address string */
			ipStr = socketGetAddressString(pTcpipInterface->InterfaceConfig.lIpAddr);
			ipStrLength = (UINT16)strlen(ipStr);
			ipStrLengthPad = (UINT16)(((ipStrLength+1)/2)*2);
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(2 + ipStrLengthPad) );

			if ( pRequest->cfg.objectCfg.pData == NULL )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			}
			else
			{
				pData = pRequest->cfg.objectCfg.pData;
				*pData++ = (UINT8)(0x10 | iInstance);
				*pData++ = (UINT8)ipStrLengthPad;
				memcpy( (char*)pData, ipStr, ipStrLength);
				pData += ipStrLength;
				if (ipStrLength & 1)
					*pData++ = 0;	/* Pad to even number bytes */
			}
		}
		break;
	case PORT_INST_ATTR_PORT_ROUTE_CAPABILITY:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			iVal32 = PORT_ROUTE_INCOMING_UCMM_SUPPORT | PORT_ROUTE_INCOMING_IO_SUPPORT | PORT_ROUTE_INCOMING_CLASS3_SUPPORT;
		}
#ifdef ET_IP_MODBUS
		else if (iInstance%2 == 1)
		{
			iVal32 = PORT_ROUTE_INCOMING_UCMM_SUPPORT | PORT_ROUTE_INCOMING_IO_SUPPORT | PORT_ROUTE_INCOMING_CLASS3_SUPPORT;
		}
#endif
		else
		{
			iVal32 = PORT_ROUTE_INCOMING_UCMM_SUPPORT | PORT_ROUTE_OUTGOING_UCMM_SUPPORT;
		}
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal32, sizeof(UINT32) );
		break;
	case PORT_INST_ATTR_ASSOC_COMMUNICATION_OBJECTS:
		if (iInstance == TCP_IP_BACKPLANE_PORT_NUMBER)
		{
			/* There are no objects affected by backplane port */
			bVal[0] = 0;
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, bVal, sizeof(UINT8) );
		}
#ifdef ET_IP_MODBUS
		else if (iInstance%2 == 1)
		{
			pTcpipInterface = &gsTcpIpInstAttr[(iInstance-TCP_IP_STARTING_PORT_NUMBER)/2];
			if (pTcpipInterface->InterfaceConfig.iLinkObjPathSize == 0)
			{
				/* Ethernet Link object instance 1 */
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, portAssociatedModbusPortObjects, sizeof(portAssociatedModbusPortObjects) );
			}
			else
			{
				/* Replace Ethernet Link of with path from link attribute from TCP/IP object */
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(portAssociatedModbusPortObjects)-4+pTcpipInterface->InterfaceConfig.iLinkObjPathSize );
				pData = pRequest->cfg.objectCfg.pData;
				memcpy(pData, portAssociatedModbusPortObjects, sizeof(portAssociatedModbusPortObjects));

				/* use offset of Ethernet Link object to change instance */
				pData[16] = pTcpipInterface->InterfaceConfig.iLinkObjPathSize/2;
				memcpy(&pData[17], pTcpipInterface->InterfaceConfig.LinkObjPath, pTcpipInterface->InterfaceConfig.iLinkObjPathSize);
			}

			/* use offset of TCP/IP object to change instance */
			pData[5] = (UINT8)(iInstance/2);
		}
#endif
		else
		{
			pTcpipInterface = &gsTcpIpInstAttr[(iInstance-TCP_IP_STARTING_PORT_NUMBER)/2];
			if (pTcpipInterface->InterfaceConfig.iLinkObjPathSize == 0)
			{
				/* Ethernet Link object instance 1 */
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, portAssociatedEIPPortObjects, sizeof(portAssociatedEIPPortObjects) );
				pData = pRequest->cfg.objectCfg.pData;
			}
			else
			{
				/* Replace Ethernet Link of with path from link attribute from TCP/IP object */
				EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(portAssociatedEIPPortObjects)-4+pTcpipInterface->InterfaceConfig.iLinkObjPathSize );
				pData = pRequest->cfg.objectCfg.pData;
				memcpy(pData, portAssociatedEIPPortObjects, sizeof(portAssociatedEIPPortObjects));

				/* use offset of Ethernet Link object to change instance */
				pData[11] = pTcpipInterface->InterfaceConfig.iLinkObjPathSize/2;
				memcpy(&pData[12], pTcpipInterface->InterfaceConfig.LinkObjPath, pTcpipInterface->InterfaceConfig.iLinkObjPathSize);
			}

			/* use offset of TCP/IP object to change instance */
			pData[5] = (UINT8)(iInstance-1);
		}
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** portParseClassRequest( )
**
** Respond to the Port class request
**---------------------------------------------------------------------------
*/
static void portParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
	case SVC_GET_ATTR_ALL:
		portSendClassAttrAll( pRequest );
		break;
	case SVC_GET_ATTR_SINGLE:
		portSendClassAttrSingle( pRequest );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*---------------------------------------------------------------------------
** portParseInstanceRequest( )
**
** Respond to the Port object instance request
**---------------------------------------------------------------------------
*/
static void portParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
#ifdef ET_IP_MODBUS
	if ( iInstance <= gnClaimedHostIPAddrCount*2+1)
#else
	if ( iInstance <= gnClaimedHostIPAddrCount+1)
#endif
	{
		switch( pRequest->cfg.objectCfg.bService )
		{
		case SVC_GET_ATTR_ALL:
			portSendInstanceAttrAll( pRequest, iInstance );
			break;
		case SVC_GET_ATTR_SINGLE:
			portSendInstanceAttrSingle( pRequest, iInstance );
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
** portParseClassInstanceRequest( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void portParseClassInstanceRequest( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize );

	if ( iInstance == 0 )
		portParseClassRequest( pRequest );
	else
		portParseInstanceRequest( pRequest, iInstance );
}

#endif

