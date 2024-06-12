//*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Declarations for journal service.
 *
 *  This file should not require modification.
 */

#ifndef _JOURNAL_H
#define _JOURNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cbuff.h"
#include "ber.h"
#include "context.h"
#include "mmstypes.h"

/* Journal Content Map */
/* Entries are stored in an object-by-object basis:

 Annotations: type[1], id[2], timestamp[6], text
 variable:    type[1], id[2], timestamp[6], varname[*], Data

 The IDs are used to group a set of buffer entries as a single
 journal entry - multiple variables can be journalled as a single
 entry.
 */

typedef enum {
    MMSd_Journal_Err_SUCCESS,
    MMSd_Journal_Err_NOT_FOUND
}
MMSd_JournalError;

typedef struct MMSd_Journal *MMSd_JournalPtr;
typedef struct MMSd_Journal {
    char *name;
    CircBuff circBuff;
    unsigned int nextId;
    MMSd_JournalPtr pNext;
#if defined(TMW_USE_THREADS)
    TmwTargSem_t sem;
#endif
} MMSd_Journal;

typedef enum {
    MMSd_JournalAnnotation,
    MMSd_JournalEvent,
    MMSd_JournalVar,
    MMSd_JournalInfoRpt
} MMSd_JournalEntryType;

typedef struct MMSd_JournalEntryNum {
    CircBuffPass pass; /* Pass around buffer */
    CircBuffIndex index; /* Position within buffer */
} MMSd_JournalEntryNum;

typedef struct MMSd_JournalEntry {
    MMSd_JournalEntryNum entryNum;
    MMSd_JournalEntryType entryType;
    int entryId;
    MMSd_time stamp;
    char *varName; /* Only valid for JournalVar */
    unsigned char *buff;
    CircBuffLen maxLen;
    CircBuffLen storeLen;
} MMSd_JournalEntry;

typedef unsigned char MMSd_JournalLimitFlags;
#define MMSd_JournalFlag_startNum_valid          0x01
#define MMSd_JournalFlag_afterNum_valid          0x02
#define MMSd_JournalFlag_afterTime_valid         0x04
#define MMSd_JournalFlag_numEntries_valid        0x08
#define MMSd_JournalFlag_startTime_valid         0x10
#define MMSd_JournalFlag_endTime_valid           0x20

typedef enum {
    MMSd_Journal_forward,
    MMSd_Journal_backward
} MMSd_JournalOrder;

typedef struct MMSd_JournalLimit {
    MMSd_JournalLimitFlags limitFlags; /* Shows what was parsed */
    MMSd_JournalEntryNum startNum; /* Parsed from request */
    MMSd_JournalEntryNum afterNum; /* Parsed from request */
    MMSd_time afterTime; /* Parsed from request */
    long numEntries; /* Parsed from request */
    MMSd_time startTime; /* Parsed from request */
    MMSd_time endTime; /* Parsed from request */
    CircBuffIndex startIndex; /* Derived from data */
    CircBuffPass startPass; /* Derived from data */
    CircBuffIndex endIndex; /* Derived from data */
    CircBuffPass endPass; /* Derived from data */
    MMSd_time startStamp; /* Fetched from entry */
    MMSd_time endStamp; /* Fetched from entry */
} MMSd_JournalLimit;

typedef struct MMSd_JournalDescriptor {
    CircBuff *circBuff; /* Circular buffer for Journal */
    CircBuffIndex index; /* Current position within buffer */
    CircBuffPass pass; /* Current pass around buffer */
    CircBuffIndex remain; /* Length of msg within buffer */
    MMSd_ushort tokenLength; /* Length of subordinate type */
    MMSd_tag tag; /* ASN1 tag value */
    MMSd_errorFlag errorFlag; /* Cumulative errors */
} MMSd_JournalDescriptor;

typedef struct MMSd_JournalReader {
    MMSd_Journal *journal;
    CircBuffIndex tail;
    CircBuffPass tailPass;
    CircBuffIndex newTail;
    CircBuffPass newTailPass;
} MMSd_JournalReader;

int JournalInit(MMSd_Journal *journal, unsigned char *buffer, unsigned long length);

int JournalAnn(MMSd_Journal *journal, int id, char *str, MMSd_time *stamp);

int JournalVar(MMSd_Journal *journal, int id, char *name, MMSd_time *stamp, unsigned char *pData, int length);

int JournalFetch(MMSd_Journal *journal, CircBuffIndex *tail, CircBuffPass *tailPass, MMSd_JournalEntry *entry);

int JournalEvent(  MMSd_Journal *journal, int id, char *name, int currentState );

CircBuffLen MMSd_JournalIndexDiff(MMSd_JournalDescriptor *Dstart, MMSd_JournalDescriptor *Dnow);

TMW_CLIB_API int MMSd_JournalGetTimeStamp(MMSd_Journal *journal, CircBuffIndex index, CircBuffPass pass, MMSd_time *stamp);

TMW_CLIB_API int MMSd_JournalGetPrevious(MMSd_Journal *pJournal, CircBuffIndex index, CircBuffPass pass, CircBuffIndex *retIndex, CircBuffPass *retPass);

TMW_CLIB_API int MMSd_JournalGetNext(MMSd_Journal *pJournal, CircBuffIndex index, CircBuffPass pass, CircBuffIndex *retIndex, CircBuffPass *retPass);

/* Locate a journal within the MMS context based on descriptor from MMS service */
MMSd_Journal *MMSd_findJournal(MMSd_context *cntxt, MMSd_descriptor *name);

/* Locate a journal within the MMS context based on domain and journal names (NULL domain name implies VMD */
MMSd_Journal *MMSd_findJournalObject(MMSd_context *cntxt, char *domainName, char *pJournalName);

void MMSd_journalError(MMSd_context *cntxt, MMSd_JournalError err);
MMSd_length MMSd_readJournalServiceRequest(MMSd_context *cntxt, MMSd_Journal *journal, MMSd_descriptor *rangeStart, MMSd_descriptor *rangeStop, MMSd_descriptor *varList, MMSd_descriptor *stopAfter);

MMSd_length MMSd_readJournalResponse(MMSd_context *cntxt, MMSd_descriptor *listOfEntry, boolean moreFollows);

MMSd_length MMSd_writeJournalServiceRequest(MMSd_context *cntxt, MMSd_Journal *journal, MMSd_descriptor *listOfJournalEntry);

MMSd_length MMSd_writeJournalResponse(MMSd_context *cntxt);

int MMSd_initializeJournalServiceRequest(MMSd_context *cntxt, MMSd_Journal *journal, MMSd_descriptor *limitSpec);

int MMSd_initializeJournalResponse(MMSd_context *cntxt, unsigned long numDel);

MMSd_length MMSd_reportJournalStatusServiceRequest(MMSd_context *cntxt, MMSd_Journal *journal);

int MMSd_reportJournalStatusResponse(MMSd_context *cntxt, unsigned long numEntries, boolean deletable);

TMW_CLIB_API int MMSd_JournalValidEntry(MMSd_Journal *journal, CircBuffIndex tail, CircBuffPass tailPass);

int MMSd_JournalValidPosition(MMSd_Journal *journal, CircBuffIndex tail, CircBuffPass tailPass);

int MMSd_journalInfoRpt( MMSd_Journal *journal, int id, MMSd_ObjectName *name, MMSd_time *stamp, unsigned char *pData, int length );

int MMSd_parseJournalInfoRpt(MMSd_JournalDescriptor *D, int *useNames, MMSd_JournalDescriptor *inclusion, MMSd_JournalDescriptor *values );

void MMSd_InitializeJournal(MMSd_Journal *journal);

void MMSd_CleanupJournal(MMSd_Journal *journal);

int MMSd_encodeJournalEntry(MMSd_context *cntxt, MMSd_Journal *journal, CircBuffData *cb_if, MMSd_descriptor *varList, MMSd_JournalEntry *entry);

#ifdef __cplusplus
}
;
#endif

#endif  /* _JOURNAL_H */
