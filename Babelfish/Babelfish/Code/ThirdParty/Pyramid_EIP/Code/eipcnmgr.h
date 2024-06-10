/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCNMGR.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Connection Manager class header file
**
*****************************************************************************
*/

#ifndef EIPCNMGR_H
#define EIPCNMGR_H

#define DATA_SIZE_MASK				0x01FF	/* Data size mask in the connection parameters word */
#define TRANSPORT_CLASS_MASK		0x0F	/* Transport class mask in the transport type byte */
#define TRIGGER_CLASS_MASK			0xF0	/* Trigger class mask in the transport type byte */
#define CLASS_TYPE_MASK				0x70	/* Trigger type mask in the transport type byte */
#define SERVER_CLASS_TYPE_MASK		0x80	/* Server type mask in the transport type byte */
#define DATA_SEQUENCE_COUNT_SIZE	2		/* Size of seqeunce count value within a connected packet */

#define CONNMGR_CLASS				6		/* Connection Manager class Id */
#define CONNMGR_CLASS_REVISION		1		/* Connection Manager class revision */

/* Class and instance attribute numbers */
#define CONNMGR_CLASS_ATTR_REVISION				1
#define CONNMGR_CLASS_ATTR_MAX_INSTANCE			2
#define CONNMGR_CLASS_ATTR_NUM_INSTANCES		3
#define CONNMGR_CLASS_ATTR_OPTIONAL_ATTR		4
#define CONNMGR_CLASS_ATTR_MAX_CLASS_ATTR		6
#define CONNMGR_CLASS_ATTR_MAX_INST_ATTR		7

#define CONNMGR_ATTR_OPEN_REQUESTS				1
#define CONNMGR_ATTR_OPEN_FORMAT_REJECTS		2
#define CONNMGR_ATTR_OPEN_RESOURCE_REJECTS		3
#define CONNMGR_ATTR_OPEN_OTHER_REJECTS			4
#define CONNMGR_ATTR_CLOSE_REQUEST				5
#define CONNMGR_ATTR_CLOSE_FORMAT_REJECTS		6
#define CONNMGR_ATTR_CLOSE_OTHER_REJECTS		7
#define CONNMGR_ATTR_CONNECTION_TIMEOUTS		8
#ifdef EIP_BIG12
#define CONNMGR_ATTR_CPU_UTILIZATION			11
#endif

#define CONNMGR_ATTR_IO_PPS						15
#define CONNMGR_ATTR_PERCENT_IO_UTILIZATION		16
#define CONNMGR_ATTR_EXPLICIT_PPS				17
#define CONNMGR_ATTR_MISSED_IO_PACKETS			18
#define CONNMGR_ATTR_IO_CONN_COUNT				19
#define CONNMGR_ATTR_EXPLICIT_CONN_COUNT		20

#ifdef EIP_BIG12
#define CONNMGR_ATTR_OPT_COUNT					9
#else
#define CONNMGR_ATTR_OPT_COUNT					8
#endif

#define FWD_OPEN_CMD_CODE			0x54		/* Open Connection request */
#define LARGE_FWD_OPEN_CMD_CODE		0x5B		/* Large Forward Open Connection request */
#define FWD_CLOSE_CMD_CODE			0x4E		/* Close Connection request */
#define UNCONNECTED_SEND_CMD_CODE	0x52		/* Unconnected Send request */
#define GET_CONNECTION_OWNER_CODE	0x5A		/* Get Connection Owner request */

#define PRIORITY_TICK_TIME			0x0A
#define TIMEOUT_TICKS				0xF0
#define PRIORITY_TICK_TIME_MASK		0x0F


#define KEY_COMPATIBILITY_FLAG		0x80
#define MAJOR_REVISION_MASK			0x7F

#define LAST_ACTION_NO_OWNER		0
#define LAST_ACTION_IDLE			1
#define LAST_ACTION_RUN				2


typedef struct tagFWD_OPEN			/* Forward open parameters structure */
{
	UINT8 bOpenPriorityTickTime;	/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8 bOpenTimeoutTicks;		/* Number of ticks to timeout */

	UINT32 lLeOTConnId;				/* Originator -> Target connection id */
	UINT32 lLeTOConnId;				/* Target -> Originator connection id */

	UINT16 iLeOrigConnectionSn;		/* Unique number identifying this connection */
	UINT16 iLeOrigVendorId;			/* Unique number identifying the originator vendor */
	UINT32 lLeOrigDeviceSn;			/* Unique number identifying the originator device serial number */

	UINT8 bTimeoutMult;				/* Timeout multiplier determines timeout interval on I/O receive */

	UINT8 bReserved;				/* Must be 0 */
	UINT16 iReserved;				/* Must be 0 */

	UINT32 lLeOTRpi;				/* Originator -> Target requested packet interval */

	UINT16 iLeOTConnParams;			/* Originator -> Target connection parameters */
	UINT32 lLargeLeOTConnParams;	/* Originator -> Target connection parameters for the Large Forward Open */

	UINT32 lLeTORpi;				/* Target -> Originator requested packet interval */

	UINT16 iLeTOConnParams;			/* Target -> Originator connection parameters */
	UINT32 lLargeLeTOConnParams;	/* Target -> Originator connection parameters for the Large Forward Open */

	UINT8 bClassTrigger;			/* Transport class and type */
	UINT8 bConnPathSize;			/* connection path size in words */

}
FWD_OPEN;

#define FWD_OPEN_SIZE  36
#define LARGE_FWD_OPEN_SIZE 40
#define MAX_REG_FWD_OPEN_DATA_SIZE 509

typedef struct tagFWD_OPEN_REPLY
{
	UINT32 lLeOTConnId;				/* Originator -> Target connection id */
	UINT32 lLeTOConnId;				/* Target -> Originator connection id */

	UINT16 iLeOrigConnectionSn;		/* Unique number identifying this connection */
	UINT16 iLeOrigVendorId;			/* Unique number identifying the originator vendor */
	UINT32 lLeOrigDeviceSn;			/* Unique number identifying the originator device serial number */

	UINT32 lLeOTApi;				/* Originator -> Target actual packet interval */
	UINT32 lLeTOApi;				/* Target -> Originator actual packet interval */
	UINT8 bAppReplySize;			/* Number of 16 bit words in the Application Reply field. */
	UINT8 bReserved;
	UINT8* pAppReply;				/* Application Reply data */
}
FWD_OPEN_REPLY;

#define FWD_OPEN_REPLY_SIZE   26

typedef struct tagFWD_CLOSE
{
	UINT8 bOpenPriorityTickTime;	/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8 bOpenTimeoutTicks;		/* Number of ticks to timeout */

	UINT16 iLeOrigConnectionSn;		/* Unique number identifying this connection */
	UINT16 iLeOrigVendorId;			/* Unique number identifying the originator vendor */
	UINT32 lLeOrigDeviceSn;			/* Unique number identifying the originator device serial number */

	UINT8 bClosePathSize;			/* close path size in words */
	UINT8 bReserved;
}
FWD_CLOSE;

#define FWD_CLOSE_SIZE  12

typedef struct tagFWD_CLOSE_REPLY
{
	UINT16 iLeOrigConnectionSn;		/* Unique number identifying this connection */
	UINT16 iLeOrigVendorId;			/* Unique number identifying the originator vendor */
	UINT32 lLeOrigDeviceSn;			/* Unique number identifying the originator device serial number */
	UINT8 bPathSize;				/* Message Path size in words */
	UINT8 bReserved;
}
FWD_CLOSE_REPLY;

#define FWD_CLOSE_REPLY_SIZE		10

typedef struct tagCONNMGR_STATS
{
	UINT16 iOpenRequests;
	UINT16 iOpenFormatRejects;
	UINT16 iOpenResourceRejects;
	UINT16 iOpenOtherRejects;
	UINT16 iCloseRequests;
	UINT16 iCloseFormatRejects;
	UINT16 iCloseOtherRejects;
	UINT16 iConnectionTimeouts;

} CONNMGR_STATS;

#define CONNMGR_STATS_SIZE			16


extern void connmgrInitFwdOpenTypeFromBuf( UINT8* buf, FWD_OPEN* fwdOpenType, BOOL bLargeFwdOpen );
extern void connmgrInitBufFromFwdOpenType( UINT8* buf, FWD_OPEN* fwdOpenType, BOOL bLargeFwdOpen );

extern void connmgrInitFwdOpenReplyFromBuf( UINT8* buf, FWD_OPEN_REPLY* fwdOpenTypeReply );

extern void connmgrInitBufFromFwdCloseType( UINT8* buf, FWD_CLOSE* fwdCloseType );

extern void connmgrInitFwdCloseReplyFromBuf( UINT8* buf, FWD_CLOSE_REPLY* fwdCloseTypeReply );

extern void connmgrPrepareConnection(CONNECTION* pConnection, REQUEST* pRequest, UINT8 bAppSize, UINT8* pAppData);
extern void connmgrPrepareFwdOpenErrorReply(REQUEST* pRequest, FWD_OPEN* pFwdOpen, UINT8 bGeneralError, UINT16 iExtendedError);

extern void connmgrInit(void);
extern BOOL connmgrParseClassInstanceRqst( REQUEST* pRequest );
extern void connmgrReset(UINT8 bReset);

extern CONNMGR_STATS connmgrStats;
extern BOOL gbAppVerifyConnection;
extern BOOL gbAutomaticRunIdleEnforcement;
#ifndef EIP_NO_CIP_ROUTING
extern BOOL gbBackplaneSupport;
extern UINT16 giBackplaneSlot;
#endif

#ifdef EIP_BIG12
UINT16 platformGetCPUUtilization(void);
#endif

#endif /* #ifndef EIPCNMGR_H */
