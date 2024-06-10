/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCNLST.C
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

#include "eipinc.h"

#ifdef EIP_LARGE_CONFIG_OPT

#define CNXN_POLL_LIST_POOL_SIZE	MAX_CONNECTIONS
#define CNXN_TIMER_LIST_POOL_SIZE	(MAX_CONNECTIONS * 3 + 10)   /* allow for production, timeout and inhibit entries plus some config delays */
#define CNXN_RX_LIST_POOL_SIZE		MAX_CONNECTIONS

typedef struct _GENERAL_LIST_ENTRY 
{
	struct _GENERAL_LIST_ENTRY  *pPrev;
	struct _GENERAL_LIST_ENTRY  *pNext;

} GENERAL_LIST_ENTRY;

typedef struct _CNXN_POLL_LIST_ENTRY
{
	struct _CNXN_POLL_LIST_ENTRY *pPrev;
	struct _CNXN_POLL_LIST_ENTRY *pNext;
	CONNECTION *pConn;

} CNXN_POLL_LIST_ENTRY;

typedef struct _CNXN_TIMER_LIST_ENTRY
{
	struct _CNXN_TIMER_LIST_ENTRY *pPrev;
	struct _CNXN_TIMER_LIST_ENTRY *pNext;
	CONNECTION *pConn;
	UINT32 eType;
	UINT32 ulExpTime;

} CNXN_TIMER_LIST_ENTRY;

typedef struct _CNXN_TIMER_LIST
{
	CNXN_TIMER_LIST_ENTRY *psHead;
	CNXN_TIMER_LIST_ENTRY *psTail;
	CNXN_TIMER_LIST_ENTRY *psCenter;
	int centerMoveCount;

} CNXN_TIMER_LIST;

static CNXN_POLL_LIST_ENTRY asPollBuffers[CNXN_POLL_LIST_POOL_SIZE];
static CNXN_TIMER_LIST_ENTRY asTimerBuffers[CNXN_TIMER_LIST_POOL_SIZE];

static CNXN_POLL_LIST_ENTRY *psPollPool;		/* pool of unused poll buffers */
static CNXN_POLL_LIST_ENTRY *psPollQueueHead;	/* poll queue head */
static CNXN_POLL_LIST_ENTRY *psPollQueueTail;	/* poll queue tail */
static UINT32 ulPollEntryCount;

static CNXN_TIMER_LIST_ENTRY *psTimerPool;		/* pool of unused timer buffers */

static CNXN_TIMER_LIST sProdTimerQueue;			/* production timer queue */
static CNXN_TIMER_LIST sTOTimerQueue;			/* connetion timeout timer queue */
static CNXN_TIMER_LIST sInhTimerQueue;			/* inhibit timer queue */
static CNXN_TIMER_LIST sCfgTimerQueue;			/* config timer queue */

static void LLInsert(GENERAL_LIST_ENTRY *pNewEntry, GENERAL_LIST_ENTRY *pLoc,
						GENERAL_LIST_ENTRY **pListHead, GENERAL_LIST_ENTRY **pListTail);
static void LLInsert_Ordered(CNXN_TIMER_LIST_ENTRY *pNewEntry, CNXN_TIMER_LIST_ENTRY *pLoc, 
								CNXN_TIMER_LIST *psList);
static void LLRemove(GENERAL_LIST_ENTRY *pLoc, GENERAL_LIST_ENTRY **pListHead, GENERAL_LIST_ENTRY **pListTail);
static void LLRemove_Ordered(CNXN_TIMER_LIST_ENTRY *pLoc, CNXN_TIMER_LIST *psList);
static void connListRemoveCnxnPollListEntry(CONNECTION *pConn);
static CNXN_POLL_LIST_ENTRY* connListGetPollEntryFromPool();
static void connListReturnPollEntryToPool(CNXN_POLL_LIST_ENTRY *pEntry);
static CNXN_TIMER_LIST_ENTRY* connListGetTimerEntryFromPool();
static void connListReturnTimerEntryToPool(CNXN_TIMER_LIST_ENTRY *pEntry);
static void connListSetCnxnTimerReference(CONNECTION *pConn, UINT32 eType, void *pTimer);
static CNXN_TIMER_LIST_ENTRY* connListGetCnxnTimerReference(CONNECTION *pConn, UINT32 eType);
static BOOL connListInsertCnxnTimerEntry(CNXN_TIMER_LIST_ENTRY *pEntry);
static void connListCnxnIdTableInit();

/* set up free buffer pools and queue heads and tails for connection lists */
BOOL connListInitialize()
{
	int i;

	/* set up poll pool */
	psPollPool = &asPollBuffers[0];
	for (i=0; i<CNXN_POLL_LIST_POOL_SIZE; i++)
	{
		asPollBuffers[i].pPrev = (0 == i) ? NULL : &asPollBuffers[i-1];
		asPollBuffers[i].pNext = (CNXN_POLL_LIST_POOL_SIZE - 1 == i) ? NULL : &asPollBuffers[i+1];
	}

	/* set up timer pool */
	psTimerPool = &asTimerBuffers[0];
	for (i=0; i<CNXN_TIMER_LIST_POOL_SIZE; i++)
	{
		asTimerBuffers[i].pPrev = (0 == i) ? NULL : &asTimerBuffers[i-1];
		asTimerBuffers[i].pNext = (CNXN_TIMER_LIST_POOL_SIZE - 1 == i) ? NULL : &asTimerBuffers[i+1];
	}

	/* initialize queues to empty */
	psPollQueueHead = NULL;
	psPollQueueTail = NULL;
	ulPollEntryCount = 0;

	sProdTimerQueue.psHead = NULL;
	sProdTimerQueue.psTail = NULL;
	sProdTimerQueue.psCenter = NULL;
	sProdTimerQueue.centerMoveCount = 0;
	sTOTimerQueue.psHead = NULL;
	sTOTimerQueue.psTail = NULL;
	sTOTimerQueue.psCenter = NULL;
	sTOTimerQueue.centerMoveCount = 0;
	sInhTimerQueue.psHead = NULL;
	sInhTimerQueue.psTail = NULL;
	sInhTimerQueue.psCenter = NULL;
	sInhTimerQueue.centerMoveCount = 0;
	sCfgTimerQueue.psHead = NULL;
	sCfgTimerQueue.psTail = NULL;
	sCfgTimerQueue.psCenter = NULL;
	sCfgTimerQueue.centerMoveCount = 0;

	/* init the hash table */
	connListCnxnIdTableInit();

	return TRUE;
}

/* add an entry to the poll queue for the given connection */
BOOL connListQueueCnxnPollEntry(CONNECTION *pConn)
{
	BOOL ret = TRUE;
	CNXN_POLL_LIST_ENTRY *pEntry;

	/* if there's already a poll entry for the connection, we don't need
	* to queue another one.
	*/
	if (NULL == pConn->pPollListEntry)
	{
		/* get a free entry */
		pEntry = connListGetPollEntryFromPool();

		if (NULL == pEntry)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConn->cfg.lHostIPAddr, pConn->cfg.lIPAddress, 
					"QueueCnxnPollEntry: No free Poll entries!");
			ret = FALSE;
		}
		else
		{
			/* fill in entry */
			pEntry->pConn = pConn;

			/* connection reference to poll list entry */
			pConn->pPollListEntry = pEntry;

			/* add to end of queue */
			LLInsert((GENERAL_LIST_ENTRY *)pEntry, (GENERAL_LIST_ENTRY *)psPollQueueTail, 
					(GENERAL_LIST_ENTRY **)&psPollQueueHead, (GENERAL_LIST_ENTRY **)&psPollQueueTail);

			ulPollEntryCount++;
		}
	}

	return ret;
}

/* retreive the next entry from the poll queue.
   returns NULL if there is nothing to poll */
CONNECTION* connListGetNextCnxnPollEntry()
{
	CNXN_POLL_LIST_ENTRY *pEntry;
	CONNECTION *pConn = NULL;

	if (psPollQueueHead)		/* something's on the queue */
	{
		/* save connection pointer */
		pConn = psPollQueueHead->pConn;

		pConn->pPollListEntry = NULL;	/* reset poll list reference in connection */

		/* remove entry from front of queue */
		pEntry = psPollQueueHead;
		LLRemove((GENERAL_LIST_ENTRY *)psPollQueueHead,
					(GENERAL_LIST_ENTRY **)&psPollQueueHead, (GENERAL_LIST_ENTRY **)&psPollQueueTail);

		ulPollEntryCount--;

		/* return buffer */
		connListReturnPollEntryToPool(pEntry);
	}

	return pConn;
}

UINT32 connListGetCnxnPollCount()
{
	return ulPollEntryCount;
}

/* add a connection timer to the timer list */
BOOL connListQueueCnxnTimer(CONNECTION *pConn, CNXN_TIMER_TYPE eType, UINT32 ulTime)
{
	return connListResetCnxnTimer(pConn, eType, ulTime);
}

/* get the next connection timer entry of the specified type that has expired.
 * return the resulting timer type in peResultingType
   returns NULL if no timer of the give type has expired  */
static CONNECTION* connListGetExpiredCnxnTimerType(CNXN_TIMER_TYPE eType, CNXN_TIMER_TYPE *peResultingType)
{
	CONNECTION *pConn = NULL;
	CNXN_TIMER_LIST_ENTRY *pEntry;
	CNXN_TIMER_LIST *psList;

	switch (eType)
	{
	case CNXN_TIMER_TYPE_PRODUCTION:
		psList = &sProdTimerQueue;
		break;
	case CNXN_TIMER_TYPE_CONN_TIMEOUT:
		psList = &sTOTimerQueue;
		break;
	case CNXN_TIMER_TYPE_INHIBIT:
		psList = &sInhTimerQueue;
		break;
	case CNXN_TIMER_TYPE_CFG_TIMEOUT:
	case CNXN_TIMER_TYPE_START_DELAY:
#ifdef CONNECTION_STATS
	case CNXN_TIMER_TYPE_STATS_DELAY:
#endif
		psList = &sCfgTimerQueue;
		break;
	default:
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0,
				"connListGetExpiredCnxnTimerType: Unknown timer type %d", eType);
		return NULL;
		break;
	}
		/* if the list isn't empty and the clock has reached or surpassed the head
			entries expiration time, pull the head entry off the queue and return it */
	if (NULL != psList->psHead &&
		IS_TICK_GREATER_OR_EQUAL(gdwTickCount, (psList->psHead)->ulExpTime))
	{
		pConn = (psList->psHead)->pConn;
		*peResultingType = (psList->psHead)->eType;

		connListSetCnxnTimerReference(pConn, *peResultingType, NULL);  /* clear reference in connection */

		pEntry = psList->psHead;
		LLRemove_Ordered(pEntry, psList);

		connListReturnTimerEntryToPool(pEntry);
	}

	return pConn;
}

/* get the next connection production timer entry that has expired.
   returns NULL if no production timer has expired  */
CONNECTION* connListGetExpiredCnxnProductionTimer()
{
	CNXN_TIMER_TYPE eType;

	return connListGetExpiredCnxnTimerType(CNXN_TIMER_TYPE_PRODUCTION, &eType);
}

/* get the next connection timeout timer entry that has expired.
      returns NULL if no timeout timer has expired  */
CONNECTION* connListGetExpiredCnxnTimeoutTimer()
{
	CNXN_TIMER_TYPE eType;

	return connListGetExpiredCnxnTimerType(CNXN_TIMER_TYPE_CONN_TIMEOUT, &eType);
}

/* get the next connection inhibit timer entry that has expired.
      returns NULL if no inhibit timer has expired  */
CONNECTION* connListGetExpiredCnxnInhibitTimer()
{
	CNXN_TIMER_TYPE eType;

	return connListGetExpiredCnxnTimerType(CNXN_TIMER_TYPE_INHIBIT, &eType);
}

/* get the next connection config timer entry that has expired.
 * returns the specific type of timer in peType
      returns NULL if no config timer has expired  */
CONNECTION* connListGetExpiredCnxnConfigTimer(CNXN_TIMER_TYPE *peType)
{
	return connListGetExpiredCnxnTimerType(CNXN_TIMER_TYPE_CFG_TIMEOUT, peType);
}

/* cancel all timers for the given connection */
void connListCancelCnxnTimers(CONNECTION *pConn)
{
	UINT32 eType;

	for (eType = CNXN_TIMER_TYPE_PRODUCTION; eType<MAX_CNXN_TIMER_TYPE; eType++)
	{
		connListCancelCnxnTimer(pConn, eType);
	}
}

/* cancel the given timer type for the connection */
void connListCancelCnxnTimer(CONNECTION *pConn, UINT32 eType)
{
	CNXN_TIMER_LIST_ENTRY *pEntry;

	pEntry = connListGetCnxnTimerReference(pConn, eType);
	if (pEntry)
	{
		CNXN_TIMER_LIST *psList;

		switch (eType)
		{
		case CNXN_TIMER_TYPE_PRODUCTION:
			psList = &sProdTimerQueue;
			break;
		case CNXN_TIMER_TYPE_CONN_TIMEOUT:
			psList = &sTOTimerQueue;
			break;
		case CNXN_TIMER_TYPE_INHIBIT:
			psList = &sInhTimerQueue;
			break;
		case CNXN_TIMER_TYPE_CFG_TIMEOUT:
		case CNXN_TIMER_TYPE_START_DELAY:
#ifdef CONNECTION_STATS
		case CNXN_TIMER_TYPE_STATS_DELAY:
#endif
			psList = &sCfgTimerQueue;
			break;
		default:
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0,
					"connListCancelCnxnTimer: Unknown timer type %d", eType);
			return;
		}

		LLRemove_Ordered(pEntry, psList);
		connListReturnTimerEntryToPool(pEntry);
		connListSetCnxnTimerReference(pConn, eType, NULL);
	}
}

/* reset the given timer type for the connection to the new time */
BOOL connListResetCnxnTimer(CONNECTION *pConn, UINT32 eType, UINT32 ulTime)
{
	CNXN_TIMER_LIST_ENTRY *pEntry;
	BOOL ret = TRUE;

	pEntry = connListGetCnxnTimerReference(pConn, eType);
	if (pEntry)
	{
		/* pull it out of the list */
		CNXN_TIMER_LIST *psList;

		switch (eType)
		{
		case CNXN_TIMER_TYPE_PRODUCTION:
			psList = &sProdTimerQueue;
			break;
		case CNXN_TIMER_TYPE_CONN_TIMEOUT:
			psList = &sTOTimerQueue;
			break;
		case CNXN_TIMER_TYPE_INHIBIT:
			psList = &sInhTimerQueue;
			break;
		case CNXN_TIMER_TYPE_CFG_TIMEOUT:
		case CNXN_TIMER_TYPE_START_DELAY:
#ifdef CONNECTION_STATS
		case CNXN_TIMER_TYPE_STATS_DELAY:
#endif
			psList = &sCfgTimerQueue;
			break;
		default:
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0,
					"ResetCnxnTimer: Unknown timer type %d", eType);
			return FALSE;
		}

		LLRemove_Ordered(pEntry, psList);

		/* reset the time */
		pEntry->ulExpTime = gdwTickCount + ulTime;
	}
	else
	{
		/* create a new entry */
		pEntry = connListGetTimerEntryFromPool();

		if (NULL == pEntry)
		{
			ret = FALSE;
		}
		else
		{
			pEntry->pConn = pConn;
			pEntry->eType = eType;
			pEntry->ulExpTime = gdwTickCount + ulTime;

			/* set reference in connection */
			connListSetCnxnTimerReference(pConn, eType, pEntry);
		}
	}

	/* insert the updated/new entry into the list */
	if (pEntry)
	{
		ret = connListInsertCnxnTimerEntry(pEntry);
	}

	if (FALSE == ret)
	{
		connListSetCnxnTimerReference(pConn, eType, NULL);  /* clear reference if insert failed */
	}

	return ret;
}

/* remove any entries in the poll or timer queues related to the given connection */
void connListClearCnxnLists(CONNECTION *pConn)
{
	connListRemoveCnxnPollListEntry(pConn);
	connListCancelCnxnTimers(pConn);
}

/* update the poll and timer list entries when a connection structure is moved */
void connListMoveCnxnListEntries(CONNECTION *pConn)
{
	/* if the connection structure has been moved, the pointers in all the
	 * associated list entries needs to be updated
	 */
	if (pConn->pPollListEntry)
		((CNXN_POLL_LIST_ENTRY *)(pConn->pPollListEntry))->pConn = pConn;
	if (pConn->pConfigTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pConfigTimer))->pConn = pConn;
	if (pConn->pDelayTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pDelayTimer))->pConn = pConn;
	if (pConn->pInhibitTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pInhibitTimer))->pConn = pConn;
	if (pConn->pProduceTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pProduceTimer))->pConn = pConn;
	if (pConn->pTimeoutTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pTimeoutTimer))->pConn = pConn;
#ifdef CONNECTION_STATS
	if (pConn->pStatsTimer)
		((CNXN_TIMER_LIST_ENTRY *)(pConn->pStatsTimer))->pConn = pConn;
#endif
}


/* remove entry in the poll queue for the connection */
static void connListRemoveCnxnPollListEntry(CONNECTION *pConn)
{
	CNXN_POLL_LIST_ENTRY *pEntry;

	pEntry = pConn->pPollListEntry;
	if (pEntry)
	{
		LLRemove((GENERAL_LIST_ENTRY *)pEntry,
					(GENERAL_LIST_ENTRY **)&psPollQueueHead, (GENERAL_LIST_ENTRY **)&psPollQueueTail);

		ulPollEntryCount--;
		connListReturnPollEntryToPool(pEntry);

		pConn->pPollListEntry = NULL;
	}
}

/* insert newEntry after the pLoc entry in the queue with the given head and tail.
      if pLoc is NULL the entry will be inserted at the front of the queue */
static void LLInsert(GENERAL_LIST_ENTRY *pNewEntry, GENERAL_LIST_ENTRY *pLoc, 
						GENERAL_LIST_ENTRY **pListHead, GENERAL_LIST_ENTRY **pListTail)
{
	if (pLoc)
	{
		pNewEntry->pNext = pLoc->pNext;
		pNewEntry->pPrev = pLoc;
		pLoc->pNext = pNewEntry;
		if (pNewEntry->pNext)
			pNewEntry->pNext->pPrev = pNewEntry;
		else /* at the end */
			if (pListTail)
				*pListTail = pNewEntry;
	}
	else /* add to front of the queue */
	{
		pNewEntry->pNext = *pListHead;
		pNewEntry->pPrev = NULL;
		if (*pListHead)
			(*pListHead)->pPrev = pNewEntry;
		*pListHead = pNewEntry;
		if (NULL == pNewEntry->pNext)  /* only entry in queue */
			if (pListTail)
				*pListTail = pNewEntry;
	}
}

/* remove the pLoc entry from the queue with the given head and tail */
static void LLRemove(GENERAL_LIST_ENTRY *pLoc, 
						GENERAL_LIST_ENTRY **pListHead, GENERAL_LIST_ENTRY **pListTail)
{
	if (pLoc->pPrev)
		pLoc->pPrev->pNext = pLoc->pNext;
	else /* first entry */
		*pListHead = pLoc->pNext;

	if (pLoc->pNext)
		pLoc->pNext->pPrev = pLoc->pPrev;
	else /* last entry */
		if (pListTail)
			*pListTail = pLoc->pPrev;
}

/* insert newEntry after the pLoc entry in the given queue 
      if pLoc is NULL the entry will be inserted at the front of the queue */
static void LLInsert_Ordered(CNXN_TIMER_LIST_ENTRY *pNewEntry, CNXN_TIMER_LIST_ENTRY *pLoc, 
								CNXN_TIMER_LIST *psList)
{
	if (pLoc)
	{
		pNewEntry->pNext = pLoc->pNext;
		pNewEntry->pPrev = pLoc;
		pLoc->pNext = pNewEntry;
		if (pNewEntry->pNext)
			pNewEntry->pNext->pPrev = pNewEntry;
		else /* at the end */
			psList->psTail = pNewEntry;
	}
	else /* add to front of the queue */
	{
		pNewEntry->pNext = psList->psHead;
		pNewEntry->pPrev = NULL;
		if (psList->psHead)
			psList->psHead->pPrev = pNewEntry;
		psList->psHead = pNewEntry;
		if (NULL == pNewEntry->pNext)  /* only entry in queue */
			psList->psTail = pNewEntry;
	}

	if (psList->psCenter != NULL)
	{
		/* determine which way the center needs to move */
		if (IS_TICK_GREATER_OR_EQUAL(pNewEntry->ulExpTime, psList->psCenter->ulExpTime))
			psList->centerMoveCount++;
		else
			psList->centerMoveCount--;

		/* if the list has changed enough to move the center, the move it */
		if (psList->centerMoveCount == 2)
		{
			psList->psCenter = psList->psCenter->pNext;
			psList->centerMoveCount = 0;
		}
		else if (psList->centerMoveCount == -2)
		{
			psList->psCenter = psList->psCenter->pPrev;
			psList->centerMoveCount = 0;
		}
	}
		/* there wasn't a center - this must be the first element, set the center to the head */
	else
	{
		psList->psCenter = psList->psHead;
		psList->centerMoveCount = 0;
	}
}

/* remove the pLoc entry from the given queue */
static void LLRemove_Ordered(CNXN_TIMER_LIST_ENTRY *pLoc,
								CNXN_TIMER_LIST *psList)
{
	if (pLoc->pPrev)
		pLoc->pPrev->pNext = pLoc->pNext;
	else /* first entry */
		psList->psHead = pLoc->pNext;

	if (pLoc->pNext)
		pLoc->pNext->pPrev = pLoc->pPrev;
	else /* last entry */
		psList->psTail = pLoc->pPrev;

	if (psList->psCenter)
	{
		/* we're removing the center */
		if (pLoc == psList->psCenter)
		{
			/* adjust the center based on which direction the move count is headed */
			if (psList->centerMoveCount > 0)
			{
				psList->psCenter = psList->psCenter->pNext;
				psList->centerMoveCount = 0;
			}
			else
			{
				psList->psCenter = psList->psCenter->pPrev;
				psList->centerMoveCount = 0;
			}
		}
		else
		{
				/* determine which way the center needs to move */
			if (IS_TICK_GREATER(pLoc->ulExpTime, psList->psCenter->ulExpTime))
				psList->centerMoveCount--;
			else
				psList->centerMoveCount++;

			/* if the list has changed enough to move the center, the move it */
			if (psList->centerMoveCount == 2)
			{
				psList->psCenter = psList->psCenter->pNext;
				psList->centerMoveCount = 0;
			}
			else if (psList->centerMoveCount == -2)
			{
				psList->psCenter = psList->psCenter->pPrev;
				psList->centerMoveCount = 0;
			}
		}
	}
}

/* get a poll list entry from the free buffer pool
      returns NULL if the pool is empty */
static CNXN_POLL_LIST_ENTRY *connListGetPollEntryFromPool()
{
	CNXN_POLL_LIST_ENTRY *pEntry = NULL;

	if (psPollPool) /* pool is not empty */
	{
		/* return the head of the pool and remove it from the pool */
		pEntry = psPollPool;
		LLRemove((GENERAL_LIST_ENTRY *)psPollPool, (GENERAL_LIST_ENTRY **)&psPollPool, NULL);
	}

	return pEntry;
}

/* return a poll list entry to the free buffer pool */
static void connListReturnPollEntryToPool(CNXN_POLL_LIST_ENTRY *pEntry)
{
	/* insert the entry at the front of the pool */
	LLInsert((GENERAL_LIST_ENTRY *)pEntry, NULL, (GENERAL_LIST_ENTRY **)&psPollPool, NULL);
}

/* get a timer list entry from the free buffer pool
      returns NULL if the pool is empty */
static CNXN_TIMER_LIST_ENTRY *connListGetTimerEntryFromPool()
{
	CNXN_TIMER_LIST_ENTRY *pEntry = NULL;

	if (psTimerPool) /* pool is not empty */
	{
		/* return the head of the pool and remove it from the pool */
		pEntry = psTimerPool;
		LLRemove((GENERAL_LIST_ENTRY *)psTimerPool,
				(GENERAL_LIST_ENTRY **)&psTimerPool, NULL);
	}

	return pEntry;
}

/* return a timer list entry to the free buffer pool */
static void connListReturnTimerEntryToPool(CNXN_TIMER_LIST_ENTRY *pEntry)
{
	/* insert the entry at the front of the pool */
	LLInsert((GENERAL_LIST_ENTRY *)pEntry, NULL,  (GENERAL_LIST_ENTRY **)&psTimerPool, NULL);
}

/* set the timer entry reference in the connection */
static void connListSetCnxnTimerReference(CONNECTION *pConn, UINT32 eType, void *pTimer)
{
	switch (eType)
	{
	case CNXN_TIMER_TYPE_PRODUCTION :
		pConn->pProduceTimer = pTimer;
		break;
	case CNXN_TIMER_TYPE_CONN_TIMEOUT :
		pConn->pTimeoutTimer = pTimer;
		break;
	case CNXN_TIMER_TYPE_INHIBIT :
		pConn->pInhibitTimer = pTimer;
		break;
	case CNXN_TIMER_TYPE_CFG_TIMEOUT :
		pConn->pConfigTimer = pTimer;
		break;
	case CNXN_TIMER_TYPE_START_DELAY :
		pConn->pDelayTimer = pTimer;
		break;
#ifdef CONNECTION_STATS
	case CNXN_TIMER_TYPE_STATS_DELAY :
		pConn->pStatsTimer = pTimer;
		break;
#endif
	default :
		break;
	}
}

/* retreive the timer entry reference from the connection */
static CNXN_TIMER_LIST_ENTRY *connListGetCnxnTimerReference(CONNECTION *pConn, UINT32 eType)
{
	CNXN_TIMER_LIST_ENTRY *pEntry = NULL;

	switch (eType)
	{
	case CNXN_TIMER_TYPE_PRODUCTION :
		pEntry = pConn->pProduceTimer;
		break;
	case CNXN_TIMER_TYPE_CONN_TIMEOUT :
		pEntry = pConn->pTimeoutTimer;
		break;
	case CNXN_TIMER_TYPE_INHIBIT :
		pEntry = pConn->pInhibitTimer;
		break;
	case CNXN_TIMER_TYPE_CFG_TIMEOUT :
		pEntry = pConn->pConfigTimer;
		break;
	case CNXN_TIMER_TYPE_START_DELAY :
		pEntry = pConn->pDelayTimer;
		break;
#ifdef CONNECTION_STATS
	case CNXN_TIMER_TYPE_STATS_DELAY :
		pEntry = pConn->pStatsTimer;
		break;
#endif
	default :
		break;
	}
	return pEntry;
}

/* insert the timer entry into the sorted list */
static BOOL connListInsertCnxnTimerEntry(CNXN_TIMER_LIST_ENTRY *pEntry)
{
	CNXN_TIMER_LIST_ENTRY *pLoc;
	BOOL ret = TRUE;
	CNXN_TIMER_LIST *psList;
	BOOL walkBackwards;

	switch (pEntry->eType)
	{
	case CNXN_TIMER_TYPE_PRODUCTION:
		psList = &sProdTimerQueue;
		break;
	case CNXN_TIMER_TYPE_CONN_TIMEOUT:
		psList = &sTOTimerQueue;
		break;
	case CNXN_TIMER_TYPE_INHIBIT:
		psList = &sInhTimerQueue;
		break;
	case CNXN_TIMER_TYPE_CFG_TIMEOUT:
	case CNXN_TIMER_TYPE_START_DELAY:
#ifdef CONNECTION_STATS
	case CNXN_TIMER_TYPE_STATS_DELAY:
#endif
		psList = &sCfgTimerQueue;
		break;
	default:
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0,
				"InsertCnxnTimerEntry: Unknown timer type %d", pEntry->eType);
		return FALSE;
	}

	/*
	 * Determine where in the queue the new entry needs to go.
	 * 
	 * The queue is ordered in increasing expiration times from head to tail (i.e. the head is closer to 
	 * expiring than the tail).  Entries that have equal expiration times are inserted in FIFO order so that 
	 * entries placed in the queue first will expire first.
	 * 
	 * The logic that follows attempts to find the insertion point with the least amount of queue walking.
	 * 
	 * Although a binary search algorithm is the most efficient for searching, it is only more efficient in the 
	 * worst case.  Testing showed that the worst case is not typical, and for most cases, using a binary search
	 * actually made the performance worse than a linear search.
	 * 
	 * The logic below is kind of a hybrid approach:
	 *  - If the queue's empty, we obviously know where to insert
	 *  - If the new entry goes after the tail - add it to the end
	 *  - If the new entry goes before the head - add it to the head
	 *  - If the new entry goes on the tail side of the center
	 *      - if the entry is closer to the tail than the center, start walking from the tail
	 *      - if it is closer to the center, start walking from the center toward the tail
	 *  - if the new entry goes on the head side of the center
	 *      - if the entry is closer to the head than the center, start walking from the head
	 *      - if it is closer to the center, start walking from the center toward the head
	 *  - if the center is not set, start walking from the tail.
	 *  
	 *  Since finding the center of a linked list is an expensive function, the center of the list is 
	 *  maintained by the linked list insert and remove routines as items are added and removed.
	 */
	if (psList->psHead == NULL)
	{
		pLoc = NULL;
	}
	else if (IS_TICK_GREATER_OR_EQUAL(pEntry->ulExpTime, psList->psTail->ulExpTime)) /* goes at the tail */
	{
		pLoc = psList->psTail;
	}
	else if (IS_TICK_GREATER(psList->psHead->ulExpTime, pEntry->ulExpTime))     /* goes at the head */
	{
		pLoc = NULL;
	}
	else
	{
		if (psList->psCenter)
		{
			/* is it on the tail side of center */
			if (IS_TICK_GREATER_OR_EQUAL(pEntry->ulExpTime, psList->psCenter->ulExpTime))
			{
				/* is it closer to tail than the center? */
				if (IS_TICK_GREATER((pEntry->ulExpTime - psList->psCenter->ulExpTime), (psList->psTail->ulExpTime - pEntry->ulExpTime)))
				{
					/* tail towards center */
					pLoc = psList->psTail;
					walkBackwards = TRUE;
				}
				/* it's closer to the center */
				else
				{
					/* center towards tail */
					pLoc = psList->psCenter;
					walkBackwards = FALSE;
				}
			}
			/* it's on the head side of center */
			else
			{
				/* is it closer to center than the head? */
				if (IS_TICK_GREATER((pEntry->ulExpTime - psList->psHead->ulExpTime), (psList->psCenter->ulExpTime - pEntry->ulExpTime)))
				{
					/* center towards head */
					pLoc = psList->psCenter;
					walkBackwards = TRUE;
				}
				/* it's closer to the head */
				else
				{
					/* head towards center */
					pLoc = psList->psHead;
					walkBackwards = FALSE;
				}
			}
		}
		/* center is null (this really shouldn't happen unless the queue is empty, but we need to do
		 * somthing for error checking.  We'll just start at the tail */
		else
		{
			pLoc = psList->psTail;
			walkBackwards = TRUE;
		}

		/* walk the list based on the start and direction determined above */
		while (pLoc)
		{
			/* walk backwards toward the head */
			if (walkBackwards)
			{
				if (IS_TICK_GREATER_OR_EQUAL(pEntry->ulExpTime, pLoc->ulExpTime))
				{
					break;
				}
				pLoc = pLoc->pPrev;
			}

			/* walk forwards toward the tail */
			else
			{
				if (IS_TICK_GREATER(pLoc->ulExpTime, pEntry->ulExpTime))
				{
					pLoc = pLoc->pPrev; /* insert before this node */
					break;
				}
				pLoc = pLoc->pNext;
			}
		}
	}

	/* insert the entry */
	LLInsert_Ordered(pEntry, pLoc, psList);

	return ret;
}


/*--------------------------------------------------------------------------
 *  Connection ID hash table
 *  
 *  Connections are identified by 2 peices of information in incoming packets:
 *      Connection ID
 *      Source IP address
 *      
 *  The hash table hashes 64-bit values made up of the concantenation of the CID 
 *  and Address.
 *      
 */

static PLATFORM_HASH_ID cnxnHashTable;

typedef struct
{
	PLATFORM_HASH_NODE   node;  /* linked list node (must be first) */

	UINT32 idKey;			/* key is 64-bits made up of the ID and address */
	UINT32 addrKey;

	CONNECTION *pConn;		/* data is pointer to associated connection */
	INT32 iCount;			/* Number of connections that share same CID and IP address */
	INT32 nInstance;		/* connection ID for connection (used for searching when pConn == NULL) */

} H_NODE_CONNID;

/*
 * Compare 2 hash nodes.
 * 
 * Nodes match if the CID and address are the same.
 * 
 * returns TRUE if nodes match, FALSE otherwise
 */
static BOOL CnxnHashKeyCompareFn(void *node1, void *node2, INT32 arg)
{
	H_NODE_CONNID *pNode1 = (H_NODE_CONNID*)node1,
				  *pNode2 = (H_NODE_CONNID*)node2;

	if ((pNode1->idKey == pNode2->idKey) &&
		(pNode1->addrKey == pNode2->addrKey))
	{
		if (pNode1->iCount < 0)
		{
			if (pNode1->nInstance == pNode2->pConn->cfg.nInstance)
				return TRUE;
		}
		else if (pNode1->iCount == pNode2->iCount)
		{
			return TRUE;
		}
	}

	return FALSE;
}



/*-------------------------------------------------------
 * FNV hash function
 * 
 * Fowler/Noll/Vo Hash
 * 
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 ***
 *
 * To use the recommended 32 bit FNV-1a hash, pass FNV1_32A_INIT as the
 * Fnv32_t hashval argument to fnv_32a_buf() or fnv_32a_str().
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 *  chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy! :-)
 */

#define FNV_32_PRIME ((UINT32 )0x01000193)
#define FNV1_32_INIT ((UINT32 )0x811c9dc5)

/*
 * fnv_32a_buf - perform a 32 bit Fowler/Noll/Vo FNV-1a hash on a buffer
 *
 * input:
 *  buf - start of buffer to hash
 *  len - length of buffer in octets
 *  hval    - previous hash value or 0 if first call
 *
 * returns:
 *  32 bit hash as a static hash type
 *
 * NOTE: use FNV1_32_INIT as the
 *   hval arg on the first call to either fnv_32a_buf() or fnv_32a_str().
 */
static UINT32 fnv_32a_buf(void *buf, int len, UINT32 hval)
{
	unsigned char *bp = (unsigned char *)buf;   /* start of buffer */
	unsigned char *be = bp + len;       /* beyond end of buffer */

	/*
	 * FNV-1a hash each octet in the buffer
	 */
	while (bp < be)
	{
		/* xor the bottom with the current octet */
		hval ^= (UINT32 )*bp++;

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
		hval *= FNV_32_PRIME;
	}

	/* return our new hash value */
	return hval;
}

/*------------------------------------------------------- */

/*
 * Hash the given node.
 * 
 * returns hash index within 0-elemCount
 */
static int CnxnHashFn(int elemCount, void *node, INT32 arg)
{
	H_NODE_CONNID *pNode = (H_NODE_CONNID*)node;
	UINT64 key;

	/* build the 64-bit key value */
	key = ((UINT64 )(pNode->idKey) << 32) | pNode->addrKey;

	key = fnv_32a_buf(&key, 8, FNV1_32_INIT);

	/* xor fold down to elemcount */
	key = ((key >> CNXN_HASH_TABLE_SIZE_LOG2) ^ (key)) & ((1 << CNXN_HASH_TABLE_SIZE_LOG2)-1);

	return (int )key;
}

/*
 * Initialize the connection ID hash table
 */
static void connListCnxnIdTableInit()
{
	cnxnHashTable = platformHashTableCreate(CNXN_HASH_TABLE_SIZE_LOG2,	/* 2^SIZE entries */
									CnxnHashKeyCompareFn,		/* compare function */
									CnxnHashFn,					/* hash function */
									0);							/* key argument */
	if (NULL == cnxnHashTable)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, 
				"CnxnIdTableInit: Unable to create connection ID hash table");
	}
}

/*
 * Add a connection entry to the ID hash table
 * 
 * returns TRUE if node is added successfully
 */
BOOL connListAddCnxnIdTableEntry(UINT32 CID, UINT32 ipAddr, CONNECTION *pConn)
{
	H_NODE_CONNID keyNode;
	H_NODE_CONNID *pNode;
	UINT16 iAllocSize = sizeof(H_NODE_CONNID );

	/* allocate the new node and fill it in */
	pNode = (H_NODE_CONNID *)EtIP_malloc(NULL, iAllocSize);
	if (NULL == pNode)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, 
				"AddCnxnIdTableEntry: Unable to allocate new ID hash table node");
		return FALSE;
	}

	keyNode.idKey = CID;
	keyNode.addrKey = ipAddr;
	keyNode.iCount = 0;
	while (platformHashTableFind(cnxnHashTable, (PLATFORM_HASH_NODE *)&keyNode, 0) != NULL)
		keyNode.iCount++;

	memset(pNode, 0, sizeof(H_NODE_CONNID ));
	pNode->idKey = CID;
	pNode->addrKey = ipAddr;
	pNode->pConn = pConn;
	pNode->iCount = keyNode.iCount;

	/* add it to the table */
	if (platformHashTablePut(cnxnHashTable, (PLATFORM_HASH_NODE *)pNode) != 0)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, 
				"AddCnxnIdTableEntry: Error adding node to hash table");
		EtIP_free((UINT8**)&pNode, &iAllocSize);
		return FALSE;
	}

	return TRUE;
}


/* 
 * find an entry in the table matching the given rx packet parameters
 * If a match if found, return TRUE and set ppConn to the connection
 * If an addr/CID match is found, but to the wrong socket, return TRUE, but leave ppConn NULL
 *      This will indicate that a duplicate packet may have been received on a second NIC
 * */
BOOL connListFindEntryInCnxnIdTable(UINT32 ipAddr, UINT32 CID, SOCKET hSocket, CONNECTION **ppConn, INT32 iCount)
{
	H_NODE_CONNID keyNode;
	H_NODE_CONNID *pNode;
	BOOL ret = FALSE;

	*ppConn = NULL;

	keyNode.idKey = CID;
	keyNode.addrKey = ipAddr;
	keyNode.iCount = iCount;

	pNode = (H_NODE_CONNID *)platformHashTableFind(cnxnHashTable, (PLATFORM_HASH_NODE *)&keyNode, 0);
	if (pNode)
	{
		ret = TRUE;

		if (pNode->pConn->lClass1Socket == hSocket)
		{
			*ppConn = pNode->pConn;
		}
	}

	return ret;
}

/*
 * Remove a connection entry from the ID hash table
 */
BOOL connListRemoveCnxnIdTableEntry(UINT32 CID, UINT32 ipAddr, INT32 nInstance)
{
	H_NODE_CONNID keyNode, *keyRetNode;
	H_NODE_CONNID *pNode;
	INT32 iSaveCount;
	UINT16 iFreeSize = sizeof(H_NODE_CONNID);

	/* find the node in the table */
	keyNode.idKey = CID;
	keyNode.addrKey = ipAddr;
	keyNode.iCount = -1;
	keyNode.nInstance = nInstance;

	pNode = (H_NODE_CONNID *)platformHashTableFind(cnxnHashTable, (PLATFORM_HASH_NODE *)&keyNode, 0);
	if (NULL == pNode)
	{
		return FALSE;
	}

	iSaveCount = pNode->iCount;

	/* remove the node from the table */
	if (platformHashTableRemove(cnxnHashTable, (PLATFORM_HASH_NODE *)pNode)  != 0)
	{
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, 
				"RemoveCnxnIdTableEntry: Error removing node from hash table");
		return FALSE;
	}

	/* Update the connection "count" for this CID/IP address match */
	keyNode.iCount = iSaveCount+1;
	keyNode.nInstance = -1;
	while ((keyRetNode = (H_NODE_CONNID *)platformHashTableFind(cnxnHashTable, (PLATFORM_HASH_NODE *)&keyNode, 0)) != NULL)
	{
		keyRetNode->iCount--;
		keyNode.iCount++;
	}

	/* free up the memory originally allocated for the node */
	EtIP_free((UINT8**)&pNode, &iFreeSize);
	return TRUE;
}

#endif /* EIP_LARGE_CONFIG_OPT */


/*
 * Set the connection configuration state for the given connection.
 * The configuration delay timer will also be set if the delay is non-zero
 */
void connListSetConnectionConfigState(CONNECTION *pConn, UINT32 state, UINT32 delay)
{
	pConn->lConfigurationState = state;

#ifdef EIP_LARGE_CONFIG_OPT
	/* if there is a delay after the state change start a timer, otherwise
	  queue a poll for the connection */
	if (delay)
		connListSetCnxnConfigDelayTimer(pConn, delay);
	else
		connListQueueCnxnPollEntry(pConn);
#else
	pConn->lStartTick = gdwTickCount + delay;
#endif
}

/*
 * Trigger a production for the given connection
 */
void connListTriggerCnxnProduction(CONNECTION *pConn)
{
	pConn->bTransferImmediately = TRUE;

#ifdef EIP_LARGE_CONFIG_OPT
	/* if there's no inhibit timer running, we can trigger the production
	 * immediately by resetting the production timer to 0
	 * otherwise the inhibit timer will handle the production
	 */
	if (NULL == connListGetCnxnTimerReference(pConn, CNXN_TIMER_TYPE_INHIBIT))
	{
	connListResetCnxnTimer(pConn, CNXN_TIMER_TYPE_PRODUCTION, 0);
	}
#endif
}

/*
 * Reset the production timer for the given connection
 */
void connListSetCnxnProductionTimer(CONNECTION *pConn, UINT32 ulTime)
{
#ifdef EIP_LARGE_CONFIG_OPT
	if (ulTime)   /* don't queue a production timer if production rate is 0 */
	{
		connListResetCnxnTimer(pConn, CNXN_TIMER_TYPE_PRODUCTION, ulTime);
	}
#else
	pConn->lProduceTick = gdwTickCount + ulTime;
#endif
}

/*
 * reset the production inhibit timer for the given connection
 */
void connListSetCnxnInhibitTimer(CONNECTION *pConn, UINT32 ulTime)
{
#ifdef EIP_LARGE_CONFIG_OPT
	if (ulTime) /* don't queue an inhibit timer if the inhibit time is 0 */
	{
		connListResetCnxnTimer(pConn, CNXN_TIMER_TYPE_INHIBIT, ulTime);
	}
#else
	pConn->lInhibitExpireTick = gdwTickCount + ulTime;
#endif
}

/*
 * reset the connection timeout timer for the given connection
 */
void connListResetCnxnTimeoutTimer(CONNECTION *pConn, UINT32 ulTime)
{
#ifdef EIP_LARGE_CONFIG_OPT
	if (ulTime) /* don't queue the timeout timer if the consumption rate is 0 */
	{
		connListResetCnxnTimer(pConn, CNXN_TIMER_TYPE_CONN_TIMEOUT, ulTime);
	}
#else
	pConn->lTimeoutTick = gdwTickCount + ulTime;
#endif
}

/*
 * set the configuration delay timer for the given connection
 */
void connListSetCnxnConfigDelayTimer(CONNECTION *pConn, UINT32 ulTime)
{
#ifdef EIP_LARGE_CONFIG_OPT
	/* if the config is being delayed, we need to make sure that there is
	 * no pending polls in the list - it needs to wait for the delay
	 * timer to expire before it can do any processing.
	 */
	connListRemoveCnxnPollListEntry(pConn);

	connListResetCnxnTimer(pConn, CNXN_TIMER_TYPE_START_DELAY, ulTime);
#else
	pConn->lStartTick = gdwTickCount + ulTime;
#endif
}
