/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPROUTR.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Message Router object and explict message routing behavior
**
*****************************************************************************
*/

#ifndef EIPROUTR_H
#define EIPROUTR_H

#define ROUTER_CLASS						2	/* Message Router class Id */
#define ROUTER_CLASS_REVISION				1	/* Message Router class revision */
#define ROUTER_CLASS_MAX_INSTANCE			1	/* Max instance */

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
#define SCANNER_OBJECT_COUNT			1
#else
#define SCANNER_OBJECT_COUNT			0
#endif

#ifdef EIP_QOS
#define QOS_OBJECT_COUNT				1
#else
#define QOS_OBJECT_COUNT				0
#endif

#ifdef EIP_PCCC_SUPPORT
#define PCCC_OBJECT_COUNT				1
#else
#define PCCC_OBJECT_COUNT				0
#endif

/* Number of classes supported by the router */
#define ROUTER_OBJECT_COUNT				(7+SCANNER_OBJECT_COUNT+QOS_OBJECT_COUNT+PCCC_OBJECT_COUNT)

/* Class and instance attribute numbers */
#define ROUTER_CLASS_ATTR_REVISION				1
#define ROUTER_CLASS_ATTR_MAX_INSTANCE			2
#define ROUTER_CLASS_ATTR_NUM_INSTANCES			3
#define ROUTER_CLASS_ATTR_OPT_ATTR_LIST			4
#define ROUTER_CLASS_ATTR_OPT_SVC_LIST			5
#define ROUTER_CLASS_ATTR_MAX_CLASS_ATTR		6
#define ROUTER_CLASS_ATTR_MAX_INST_ATTR			7

#define ROUTER_ATTR_CLASS_LIST					1
#define ROUTER_ATTR_MAX_CONNECTIONS				2
#define ROUTER_ATTR_OPEN_CONNECTIONS			3
#define ROUTER_ATTR_OPEN_CONNECTION_IDS			4

extern UINT16 routerObjects[ROUTER_OBJECT_COUNT];	/* Array used to service ROUTER_ATTR_CLASS_LIST attribute */


#define CPF_TAG_ADR_NULL			0		/* Null Address Tag */
#define CPF_TAG_ADR_LOCAL			0x81	/* Local Address Tag */
#define CPF_TAG_ADR_OFFLINK			0x82	/* OffLink Address Tag */
#define CPF_TAG_ADR_PCCC			0x85	/* PCCC Address Tag */
#define CPF_TAG_PKT_PCCC			0x91	/* PCCC Packet Tag */
#define CPF_TAG_ADR_CONNECTED		0xa1	/* Connected (CIP) Address Tag */
#define CPF_TAG_ADR_SEQUENCED		0x8002	/* Connected (CIP) Sequenced Tag */
#define CPF_TAG_PKT_UCMM			0xb2	/* Unconnected Messaging Packet Tag */
#define CPF_TAG_PKT_CONNECTED		0xb1	/* Connected Messaging Packet Tag */
#define CPF_TAG_SOCKADDR_OT			0x8000	/* Socket address, originator->target */
#define CPF_TAG_SOCKADDR_TO			0x8001	/* Socket address, target->originator */

#define CPF_INVALID_TAG_TYPE		(-1)


/* Definitions for Common Packet Format Encoding.  The data portion of a
 * SendPacket command has a common general-purpose format used for
 * encapsulation.  The format consists of a set of tagged fields
 * preceeded by a two byte count field.  The count field indicates
 * the number of tags in the packet.
 */

/* All tags have a common format header. */

typedef struct cpf_tag 
{
	UINT16 iTag_type;		/* Type of tag */
	UINT16 iTag_length;		/* Length of field (excludes tag) */
}
CPF_TAG;

#define CPF_TAG_SIZE		4	/* Gets around alignment */


/* Define format for the currently defined and supported tag data.  These definitions
 * DO NOT include the common header (type, length) which precedes each tag.
 */

/* NULL ADDRESS TAG consists of the header. */

/* CONNECTED ADDRESS TAG */
typedef struct cpf_adr_connected 
{
	UINT32 lCid;				/* Connection identifier */
}
CPF_ADR_CONNECTED;

typedef struct cpf_adr_sequenced 
{
	UINT32 lCid;				/* Connection identifier */
	UINT32 lSeq;				/* Connection sequence numbered */
}
CPF_ADR_SEQUENCED;

/* UCMM PACKET TAG consists of the header */

/* CONNECTED PACKET TAG consists of the header */

/* Define Generic Address Tag */
typedef struct cpf_adr_tag 
{
	CPF_TAG sTag;				/* Common Tag */
	union
	{
		UINT8 g[1];				/* Generic data */
		CPF_ADR_CONNECTED sC;	/* Connected address */
		CPF_ADR_SEQUENCED sS;	/* Sequenced address */
	} data;
	UINT8 pad[1];
}
CPF_ADR_TAG;

#define CPF_ADR_TAG_SIZE sizeof(CPF_ADR_TAG)

/* Define Generic Packet Tag */
typedef struct cpf_pkt_tag 
{
	CPF_TAG sTag;				/* Common Tag */
	UINT8* pPacketPtr;
	BOOL bConnected;
	CONNECTION* pConnection;
	UINT16 iConnSequence;
}
CPF_PKT_TAG;

/* Define Generic PCCC Address Tag */
typedef struct cpf_pccc_adr_tag 
{
	CPF_TAG sTag;				/* Common Tag */
	UINT8* pPacketPtr;
}
CPF_PCCC_ADR_TAG;

/* Define Generic Socket Address Tag */
typedef struct cpf_sockaddr_tag 
{
	CPF_TAG sTag;				/* Common Tag */
	struct sockaddr_in sAddr;
}
CPF_SOCKADDR_TAG;

/* Gets around alignment and TCP stacks that may have a different sockaddr_in structure */
#define CPF_TAG_SOCKADDR_SIZE		16


#define REPLY_BIT_MASK				0x80		/* Is set to indicate a response as opposed to a request */



/* CIP Reply header */
typedef struct tagREPLY_HEADER
{
	UINT8 bService;
	UINT8 bReserved;

	UINT8 bGenStatus;					/* 0 indicates successful reply, any other value indicates a failure */
	UINT8 bObjStatusSize;				/* size in words of the extended error that will follow the Reply Header in case of a failure */
}
REPLY_HEADER;

#define REPLY_HEADER_SIZE		4

#define ROUTER_ERROR_BASE		0x10000

#define ROUTER_ATTR_OPTIONAL_COUNT			3	/* Number of optional attributes implemented as defined by the CIP spec */
#define ROUTER_SVC_OPTIONAL_COUNT			1	/* Number of optional services implemented as defined by the CIP spec */

extern BOOL routerDataTransferReceived( SESSION* pSession );
extern void routerProcessObjectRequest( REQUEST* pRequest );
extern void routerParseObjectRequest( INT32 nSessionId, UINT32 lClientAddr, CPF_PKT_TAG* pPacketTag, UINT8* pData, UINT16 iDataSize, INT32 nGroupIndex, CPF_SOCKADDR_TAG* otTag, CPF_SOCKADDR_TAG* toTag, UINT32 lContext1, UINT32 lContext2);

#endif /* #ifndef EIPROUTR_H */
