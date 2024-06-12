/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPRQGRP.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Request Group array is used for servicing Multiple Service requests
**
*****************************************************************************
*/

#ifndef EIPRQGRP_H
#define EIPRQGRP_H

#ifndef EIP_NO_GROUP_REQUEST_SUPPORT

/* Structure used to contain request group properties */
typedef struct tagREQUEST_GROUP
{
	INT32 nIndex;						/* Unique request identifier */
	INT32 nType;						/* From the RequestType enumeration */
	UINT16 nNumberServices;				/* Number of requests in the group */
	UINT32 lRequestTimeoutTick;			/* When the request group should time out on the lack of response */
#ifndef EIP_NO_ORIGINATE_UCMM
	BOOL bIncoming;						/* Request Group can be originated by another device (incoming) or by the client application (outgoing) */
#endif
	UINT32 lIPAddress;					/* IP address of the target device for the outgoing request */
	UINT32 lHostIPAddr;					/* Host IPAddress in UINT32 format for the opened session */
	UINT8 bOpenPriorityTickTime;		/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8 bOpenTimeoutTicks;			/* Number of ticks to timeout */
	union
	{
		REQUEST_CFG objectCfg;			/* UCMM/Class3 Request configuration data */
		CLASS3_REQUEST_CFG class3Cfg;	/* Class3 only Request configuration data */
#ifdef SUPPORT_CSP
		PCCC_REQUEST_CFG pcccCfg;		/* PCCC Request configuration data */
#endif
	} cfg;

	UINT32 lContext1;					/* Sender Context information */
	UINT32 lContext2;					/* Sender Context information */
	UINT8  bGeneralError;				/* General request status. 0 indicates a success */
	UINT16 iExtendedError;				/* Extended request status. Used only if General Status is not equal to 0 */
	INT32  nSessionId;					/* Unique session identifier */
}
REQUEST_GROUP;

#define MAX_REQUEST_GROUPS			32	/* Maximum number of outstanding Multiple Service requests */

#define REQUEST_GROUP_INDEX_BASE	1	/* Starting index of the request group collection */

#ifdef EIP_LARGE_BUF_ON_HEAP
extern REQUEST_GROUP* gRequestGroups;	/* Request Group array */
#else
extern REQUEST_GROUP gRequestGroups[MAX_REQUEST_GROUPS];	/* Request Group array */
#endif
extern REQUEST_GROUP* gpnRequestGroups;			/* Pointer to the tail of populated request group array */

extern void requestGroupInit(void);
#ifndef EIP_NO_ORIGINATE_UCMM
extern REQUEST_GROUP* requestGroupNewOutgoing( INT32 nType );
#endif
extern REQUEST_GROUP* requestGroupNewIncoming( INT32 nType, INT32 nSessionId);
extern void requestGroupRemove( REQUEST_GROUP* pRequestGroup );
extern void requestGroupRemoveAll(void);
extern void requestGroupService( REQUEST_GROUP* pRequestGroup );
extern REQUEST_GROUP* requestGroupGetById( INT32 nRequestGroupId );
extern UINT16 requestGroupGetServiceNumber( REQUEST_GROUP* pRequestGroup );

#endif /* #ifndef EIP_NO_GROUP_REQUEST_SUPPORT	*/

#endif /* #ifndef REQUEST_GROUP_H */
