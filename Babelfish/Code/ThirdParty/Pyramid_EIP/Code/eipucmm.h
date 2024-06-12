/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPUCMM.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Services unconnected messages exchange
**
*****************************************************************************
*/

#ifndef EIPUCMM_H
#define EIPUCMM_H

/*
* Values for the COMMAND CODE field of the common header.
*/
#define ENCAP_CMD_NOOP					0		/* No operation */
#define ENCAP_CMD_LISTSERVICES			4		/* List Target Services Command */
#define ENCAP_CMD_LISTIDENTITY			99		/* List Targets */
#define ENCAP_CMD_LISTINTERFACES		100		/* List Communication Interfaces */
#define ENCAP_CMD_REGISTERSESSION		101		/* Register Session */
#define ENCAP_CMD_UNREGISTERSESSION		102		/* Unregister Session */
#define ENCAP_CMD_SEND_RRDATA			111		/* Send Request/Reply Data */
#define ENCAP_CMD_SEND_UNITDATA			112		/* Send Unit Data (no reply) */
#define ENCAP_CMD_INDICATESTATUS		114		/* Indicate Status */
#define ENCAP_CMD_CANCEL				115		/* Cancel Outstanding Request */

#define INTERFACE_HANDLE				0		/* 0 indicates CIP interface */
#define ENCAPSULATION_TIMEOUT			0		/* timeout used by encapsulation layer (ignored for CIP messages) */

#define LIST_IDENTITY_TIMEOUT			250		/* Wait time (in addition to max delay) for list identity broadcast responses */

#define IDENTITY_CIP_TARGET				0x0C
#define CIP_SECURITY_LIST_IDENTITY_ITEM	0x86

#define OPERATIONAL_DEVICE_STATE		0x3

/* Standard encapsulation message header */
typedef struct  tagENCAPH 
{
	UINT16 iCommand;	/* Command code */
	UINT16 iLength;		/* Total transaction length */
	UINT32 lSession;	/* Session identifier */
	UINT32 lStatus;		/* Status code */
	UINT32 lContext1;	/* Context information */
	UINT32 lContext2;	/* Context information */
	UINT32 lOpt;		/* Options flags */
} 
ENCAPH;

/* Base structure for data structures. Can be used to determine the type of data structure which follows. */
typedef struct encap_obj_hdr
{
	UINT16 iObj_type;	/* Structure type code */
	UINT16 iObj_length;	/* Length of remaining structure */
}
ENCAP_OBJ_HDR;

#define ENCAP_OBJ_HDR_SIZE				4		/* Gets around alignment */


/* Size of most named (ASCII) ENCAP objects in the protocol */
#define ENCAP_OBJ_NAME_SIZE				16

/* Structure which provides information about the services supported by a target. */
typedef struct encap_service 
{
	UINT16 iS_version;
	UINT16 iS_flags;
	UINT8 achS_name[ENCAP_OBJ_NAME_SIZE];
}
ENCAP_SERVICE;

#define ENCAP_SERVICE_SIZE				20

/* The definition for a service object */
typedef struct encap_obj_service 
{
	ENCAP_OBJ_HDR sH;		/* Standard object header */
	ENCAP_SERVICE sS;		/* Service information */
}
ENCAP_OBJ_SERVICE;

#define ENCAP_OBJ_SERVICE_SIZE			24

/* Command specific data for the REGISTER SESSION command.  Request and response data have the same format. */
typedef struct encap_rc_data 
{
	UINT16 iRc_version;		/* Requested/supported protocol revision */
	UINT16 iRc_flags;		/* Requested/supported option flags */
}
ENCAP_RC_DATA;

#define ENCAP_RC_DATA_SIZE				4


#define ENCAP_OBJTYPE_SERVICE_COMM		0x0100	/* Service (communications) type code */

/* Bit flags for the capabilities provided by a communications service */
#define ENCAP_COMMSERVICE_CIP_DIRECT	0x0020	/* Supports CIP direct/default */
#define ENCAP_COMMSERVICE_IO_CONNECT	0x0100	/* Supports CIP Class 1 I/O connections */
#define ENCAP_COMMSERVICE_VERSION		1		/* Communications service version */
#define ENCAP_COMMSERVICE_NAME			"Communications"	/* Name for this service */

/* Type codes for the various objects in the protocol. */
#define ENCAP_OBJTYPE_TARGET			0x0001	/* Target type code */
#define ENCAP_OBJTYPE_INTERFACE			0x0002	/* Interface (communications) type code */
#define ENCAP_OBJTYPE_DRIVER			0x0003	/* Driver type code */

/* Current revision for the protocol */
#define ENCAP_PROTOCOL_VERSION			1

/* Command specific data for the data transfer commands (SEND_PACKET, etc). */
typedef struct encap_dt_hdr 
{
	UINT32 lTarget;				/* Communications target identifier */
	UINT16 iTimeout;			/* Requested timeout */
}
ENCAP_DT_HDR;

#define ENCAP_DT_HDR_SIZE		6		/* Gets around alignment */


/* Error codes that may be in the encaph_status field of the fixed header structure */

#define ENCAP_E_SUCCESS			0
#define ENCAP_E_UNHANDLED_CMD	1		/* Command not handled */
#define ENCAP_E_OUTOFMEM		2		/* Memory not available for command*/
#define ENCAP_E_BADDATA			3		/* Poorly formed or incomplete data */

#define ENCAP_E_BAD_SESSIONID	100		/* Invalid session ID */
#define ENCAP_E_BAD_LENGTH		101		/* Invalid length in header */
#define ENCAP_E_UNSUPPORTED_REV	105		/* Requested protocol version not supported */
#define ENCAP_E_BAD_TARGET_ID	106		/* Invalid target ID */

/* Connection parameters masks and values: iOTConnParams & iTOConnParams */
#define CM_CP_EXCLUSIVE_OWNER		0x0000
#define CM_CP_REDUNDANT_OWNER		0x8000
#define CM_CP_LARGE_REDUNDANT_OWNER	0x80000000


#define CM_CP_TYPE_MASK				0x6000

#define CM_CP_TYPE_NULL				0x0000
#define CM_CP_TYPE_MULTICAST		0x2000
#define CM_CP_TYPE_POINT			0x4000


#define CM_CP_PRIORITY_MASK			0x0C00

#define CM_CP_FIXED_VAR_MASK		0x0200

#define CM_CP_VARIABLE_SIZE			0x0200
#define CM_CP_LARGE_VARIABLE_SIZE	0x02000000
#define CM_CP_FIXED_SIZE			0x0000

extern void ucmmInit(void);

extern BOOL ucmmProcessEncapMsg( SESSION* pSession );

#ifndef EIP_NO_ORIGINATE_UCMM
extern BOOL ucmmIssueRegisterSession( SESSION* pSession );
#endif
extern void ucmmIssueUnRegisterSession( SESSION* pSession );

extern BOOL ucmmRespondListServices( SESSION* pSession );

extern BOOL ucmmRespondListInterfaces( SESSION* pSession );
extern BOOL ucmmRespondListIdentity( SESSION* pSession, UINT32 lHostIPAddr, struct sockaddr_in* pSrcAddr, UINT16 EncapSession );
extern void ucmmParseListIdentityResp(INT32 lBytesReceived, UINT32 lSrcAddress, UINT32 lHostAddress);

extern BOOL ucmmErrorReply( SESSION* pSession, UINT32 lStatus, UINT16 iLength );

#ifdef ET_IP_SCANNER
extern void  ucmmSendConnectedRequest( CONNECTION* pConnection );
#endif
#ifndef EIP_NO_ORIGINATE_UCMM
extern BOOL  ucmmSendUnconnectedRequest( REQUEST* pRequest );
#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
extern BOOL ucmmSendUnconnectedGroupRequest( REQUEST_GROUP* pRequestGroup );
#endif
#endif

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT
extern void ucmmSendConnectedGroupResponse( REQUEST_GROUP* pRequestGroup );
extern void ucmmSendUnconnectedGroupResponse( REQUEST_GROUP* pRequestGroup );
#endif

extern BOOL ucmmSendObjectResponse( REQUEST* pRequest );

#ifndef EIP_NO_ORIGINATE_UCMM
extern BOOL ucmmPassUnconnectedSendRequest( REQUEST* pRequest );
extern BOOL ucmmPassUncnctSendRqstFinalLeg( REQUEST* pRequest );
#endif

extern void ucmmClearListIdentities(UINT32 lHostAddress);
extern EtIPListIdentityInfo* ucmmGetByListIdentityId(INT32 nId);
extern void ucmmRemoveListIdentity(EtIPListIdentityInfo* pInfo);

#endif /* #ifndef UCMM_H */
