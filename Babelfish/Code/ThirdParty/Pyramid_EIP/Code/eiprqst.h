/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPRQST.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Request array is used for scheduling outgoing object requests
**
*****************************************************************************
*/

#ifndef EIPRQST_H
#define EIPRQST_H

#define INVALID_REQUEST_INDEX				(-1)

typedef enum tagRequestState
{
	REQUEST_LOGGED							= 1,	/* Request has just arrived */
	REQUEST_WAITING_FOR_SESSION				= 2,	/* Waiting for the the TCP connection to be established to the target */
	REQUEST_WAITING_FOR_RESPONSE			= 3,	/* Waiting for the response */
	REQUEST_RESPONSE_RECEIVED				= 4,	/* Response received */
	REQUEST_READY_FOR_GROUP_SEND			= 5		/* Response received */
}
RequestState;

typedef enum tagRequestType
{
	OBJECT_REQUEST = 0,					/* For UCMM requests */
	CLASS3_REQUEST,						/* For Class3 requests */
	UNCONNECTED_SEND_REQUEST,			/* For Unconnected Sends */
	PCCC_REQUEST,						/* For PCCC requests */
	LIST_IDENTITY_REQUEST				/* For (incoming) ListIdentity requests */
}
RequestType;

/* Request configuration data */
typedef struct tagREQUEST_CFG
{
	UINT8 bService;					/* Service code */
	BOOL bNoService;				/* Don't include service code in request */
	EPATH PDU;						/* EPATH describes the target object */
	UINT8* pData;					/* Request and response data */
	UINT16 iDataSize;				/* Data size in bytes */
}
REQUEST_CFG;

/* Class 3Request configuration data */
typedef struct tagCLASS3_REQUEST_CFG
{
	REQUEST_CFG cfg;
	UINT16 iConnectionSerialNbr;	/* Connection serial number for the Class 3 responses */
	UINT16 iVendorID;				/* Connection vendor Id for the Class 3 responses */
	UINT32 lDeviceSerialNbr;		/* Connection device serial number for the Class 3 responses */
	UINT16 iConnSequence;			/* Connection sequence for the Class 3 responses */
}
CLASS3_REQUEST_CFG;

/* PCCC Request configuration data */
typedef struct tagPCCC_REQUEST_CFG
{
	UINT8 bCommand;						/* PCCC Command */
	UINT8 bError;						/* PCCC Error */
	UINT16 iTransactionNumber;			/* PCCC Transaction Number */
	UINT8 bExtCommand;					/* PCCC Extended Command/Extended Status (where applicable) */
	UINT8* pData;						/* Request and response data */
	UINT16 iDataSize;					/* Data size in bytes */
	UINT8* pAddrData;					/* Address Data */
	UINT16 iAddrDataSize;				/* Address Data size in bytes */
}
PCCC_REQUEST_CFG;

/* Structure used to pass around UCMM or Class3 information when parsing the request */
typedef struct tagREQUEST
{
	INT32 nType;						/* From the RequestType enumeration */
	INT32 nIndex;						/* Unique request identifier */
	UINT32 lRequestTimeoutTick;			/* When the request should time out on the lack of response */
	INT32 nState;						/* From the RequestState enumeration */
#ifndef EIP_NO_ORIGINATE_UCMM
	BOOL   bIncoming;					/* Request can be originated by another device (incoming) or by the client application (outgoing) */
#endif
	BOOL bDeleteOnSend;					/* Whether the request should be immediately deleted on send */
	UINT32 lIPAddress;					/* IP address of the target device for the outgoing request */
	UINT32 lFromIPAddress;				/* IP address of the originator of the Unconnected Send */
	UINT32 lHostIPAddr;					/* Host IPAddress in UINT32 format for the opened session */
	UINT8  bOpenPriorityTickTime;		/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8  bOpenTimeoutTicks;			/* Number of ticks to timeout */
	UINT8* pExtendedPath;				/* Extended path describe the path for the target bridge to use to pass forward the request */
	UINT16 iExtendedPathSize;			/* Size of the extended path in bytes */
	union
	{
		REQUEST_CFG objectCfg;			/* UCMM/Class3 Request configuration data */
		CLASS3_REQUEST_CFG class3Cfg;	/* Class3 only Request configuration data */
#ifdef SUPPORT_CSP
		PCCC_REQUEST_CFG pcccCfg;		/* PCCC Request configuration data */
#endif
	} cfg;								/* Request configuration data */

	UINT32 lContext1;					/* Sender Context information */
	UINT32 lContext2;					/* Sender Context information */
	UINT8  bGeneralError;				/* General request status. 0 indicates a success */
	UINT16 iExtendedError;				/* Extended request status. Used only if General Status is not equal to 0 */
	UINT8* pExtendedErrorData;			/* Error related data.  Appended with Extended Error only if General Status is not equal to 0 */
	UINT16 iExtendedErrorDataSize;		/* Size of error related data in bytes. */
	struct sockaddr_in sOTTagAddr;		/* Store Originator to Target socket address from the Forward Open request */
	struct sockaddr_in sTOTagAddr;		/* Store Target to Originator socket address from the Forward Open request */
	INT32  nSessionId;					/* Unique session identifier */
	INT32  nFromSessionId;				/* Unique session identifier of the originator of the Unconnected Send */
	INT32  nRequestGroupIndex;			/* Index of the Multiple Service Request Group this requests belongs to or INVALID_REQUEST_INDEX if standalone request */

#ifdef ET_IP_MODBUS
	INT32 nModbusRequestID;				/* Request ID used to track a sent Modbus request */
	INT32 nModbusContext;				/* Used when multiple Modbus message are needed for a single CIP message */
#ifdef ET_IP_MODBUS_SERIAL
	UINT8 bModbusSerialAddress;			/* Serial address for ModbusSL translation */
#endif
#endif
}
REQUEST;

#define INDEX_END			0x7fffffff	/* Should wrap at this point */


#ifdef EIP_LARGE_BUF_ON_HEAP
extern REQUEST* gRequests;					/* Request array */
#else
extern REQUEST gRequests[MAX_REQUESTS];		/* Requests array */
#endif
extern INT32 gnIndex;						/* Index to be assigned to the next request */
extern REQUEST* gpnRequests;

extern void  requestInit(void);
extern REQUEST* requestNewIncoming( INT32 nType, BOOL bDeleteOnSend, INT32 nSessionId );
#ifndef EIP_NO_ORIGINATE_UCMM
extern REQUEST* requestNewOutgoing( INT32 nType, BOOL bDeleteOnSend, UINT32 lHostIPAddr );
extern INT32 requestNewUnconnectedSend( UINT8* szIPAddr, EtIPObjectRequest* pObjectRequest, 
										char* extendedPath, UINT16 iExtendedPathLen);
extern INT32 requestNewEPATHUnconnectedSend( UINT8* szIPAddr, EtIPEPATHObjectRequest* pEPATHObjectRequest, 
											char* extendedPath, UINT16 iExtendedPathLen);
#endif
extern void  requestRemove( REQUEST* pRequest );
extern void  requestRemoveAll(void);
extern BOOL requestService( REQUEST* pRequest, BOOL* pDataSent );
#ifndef EIP_NO_ORIGINATE_UCMM
extern void requestResponseReceived( REQUEST* pRequest, UINT32 nResponseEvent);
#endif
extern REQUEST* requestGetByRequestId( INT32 nRequestId );

#endif /* #ifndef EIPRQST_H */
