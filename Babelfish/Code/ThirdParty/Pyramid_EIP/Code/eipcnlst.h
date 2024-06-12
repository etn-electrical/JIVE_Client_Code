/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** eipCnLst.h
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Connection process list routines
**
*****************************************************************************
*/

#ifndef EIPCNLST_H
#define EIPCNLST_H

#ifdef EIP_LARGE_CONFIG_OPT

typedef enum
{
	CNXN_TIMER_TYPE_PRODUCTION,
	CNXN_TIMER_TYPE_CONN_TIMEOUT,
	CNXN_TIMER_TYPE_INHIBIT,
	CNXN_TIMER_TYPE_CFG_TIMEOUT,
	CNXN_TIMER_TYPE_START_DELAY,
#ifdef CONNECTION_STATS
	CNXN_TIMER_TYPE_STATS_DELAY,
#endif

	MAX_CNXN_TIMER_TYPE

} CNXN_TIMER_TYPE;


extern BOOL connListInitialize();

extern BOOL connListQueueCnxnPollEntry(CONNECTION *pConn);
extern CONNECTION* connListGetNextCnxnPollEntry();
extern UINT32 connListGetCnxnPollCount();

extern BOOL connListQueueCnxnTimer(CONNECTION *pConn, CNXN_TIMER_TYPE eType, UINT32 ulTime);
extern CONNECTION* connListGetExpiredCnxnProductionTimer();
extern CONNECTION* connListGetExpiredCnxnTimeoutTimer();
extern CONNECTION* connListGetExpiredCnxnInhibitTimer();
extern CONNECTION* connListGetExpiredCnxnConfigTimer(CNXN_TIMER_TYPE *peType);
extern void connListCancelCnxnTimers(CONNECTION *pConn);
extern void connListCancelCnxnTimer(CONNECTION *pConn, UINT32 eType);
extern BOOL connListResetCnxnTimer(CONNECTION *pConn, UINT32 eType, UINT32 ulTime);

extern void connListClearCnxnLists(CONNECTION *pConn);
extern void connListMoveCnxnListEntries(CONNECTION *pConn);

extern BOOL connListAddCnxnIdTableEntry(UINT32 CID, UINT32 ipAddr, CONNECTION *pConn);
extern BOOL connListFindEntryInCnxnIdTable(UINT32 ipAddr, UINT32 CID, SOCKET hSocket, CONNECTION **ppConn, INT32 iCount);
extern BOOL connListRemoveCnxnIdTableEntry(UINT32 CID, UINT32 ipAddr, INT32 nInstance);

#endif /* EIP_LARGE_CONFIG_OPT */


extern void connListSetConnectionConfigState(CONNECTION *pConn, UINT32 state, UINT32 delay);
extern void connListTriggerCnxnProduction(CONNECTION *pConn);
extern void connListSetCnxnProductionTimer(CONNECTION *pConn, UINT32 ulTime);
extern void connListSetCnxnInhibitTimer(CONNECTION *pConn, UINT32 ulTime);
extern void connListResetCnxnTimeoutTimer(CONNECTION *pConn, UINT32 ulTime);
extern void connListSetCnxnConfigDelayTimer(CONNECTION *pConn, UINT32 ulTime);

#endif /* __EIPCNLST_H__  */
