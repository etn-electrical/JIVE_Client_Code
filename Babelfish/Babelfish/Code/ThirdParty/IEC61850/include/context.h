/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1994-2015 */
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
 *   Context Types for MMS
 *
 *  This file should not require modification.
 */

#ifndef _CONTEXT_H
#define _CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif


#include "ber.h"

typedef enum {
    MMSd_Authentication_None,
    MMSd_Authentication_Weak,
    MMSd_Authentication_Strong
}
MMSd_AuthenticationType;

typedef struct MMSd_ObjectListEntry {
#ifndef _use_uchar
    MMSd_short domainIndex; /* Index to domain */
    MMSd_dictionaryIndex dictionaryIndex; /* Item index in dictionary */
#else
    MMSd_char domainIndex; /* Index to domain */
    MMSd_uchar dictionaryIndex; /* Item index in dictionary */
#endif
    MMSd_descriptor object; /* NAME of object manipulated */
    MMSd_descriptor data; /* DATA for WRITE operation */
    MMSd_descriptor altAccess; /* AlternateAccess pointer */
} MMSd_ObjectListEntry;

typedef enum {
    /* for checking confirmed services while parsing */
    no_check,
    check_confirmed_service,
    check_unconfirmed_service
} MMSd_svcCheck;

typedef int SvcInfoType;
#define SvcErrInfo_obtainFile           0
#define SvcErrInfo_start                1
#define SvcErrInfo_stop                 2
#define SvcErrInfo_resume               3
#define SvcErrInfo_reset                4
#define SvcErrInfo_deleteVarAccess      5
#define SvcErrInfo_deleteNamedVarList   6
#define SvcErrInfo_deleteNamedType      7
#define SvcErrInfo_defineEventEnr       8
#define SvcErrInfo_fileRename           9
#define SvcErrInfo_additionalSvc        10

typedef int SvcFileAdditionalInfo;
#define FileError_source                0
#define FileError_destination           1

typedef int PrgInvState;
#define PrgInvState_non_existent        0
#define PrgInvState_unrunnable          1
#define PrgInvState_idle                2
#define PrgInvState_running             3
#define PrgInvState_stopped             4
#define PrgInvState_starting            5
#define PrgInvState_stopping            6
#define PrgInvState_resuming            7
#define PrgInvState_resetting           8

typedef struct MMSd_SvcSpecificInfo {
    SvcInfoType infoType;
    union {
        SvcFileAdditionalInfo obtainFile; /* ObtainFile-Error */
        PrgInvState start; /* ProgramInvState */
        PrgInvState stop; /* ProgramInvState */
        PrgInvState resume; /* ProgramInvState */
        PrgInvState reset; /* ProgramInvState */
        unsigned long deleteVarAccess; /* Unsigned32 */
        unsigned long deleteNamedVarList; /* Unsigned32 */
        unsigned long deleteNamedType; /* Unsigned32 */
        /* defineEventEnr-Error (unimplemented)                    ObjectName */
        SvcFileAdditionalInfo fileRename; /* FileRename-Error */
        /* additionalSvc        (unimplemented)                                     */
    } u;
} MMSd_SvcSpecificInfo;

typedef void *TMW_TrackingHandle;

typedef struct TMW_TrackingContext {
    int trackingCdcOptions;
    TMW_TrackingHandle trackingHandle;
    void *pUpdateCache; /* MMSd_UpdateCache       */
    int changed;
} TMW_TrackingContext;

#define MMSD_MAX_AUTH_LEVELS    3
typedef struct MMSd_context {

    MMSd_descriptor Y; /* Fetch descriptor for request */
    MMSd_descriptor Emit; /* Emit descriptor for response */

    short objectCount; /* Index, count for Obj List */
    short dataCount; /* Index, count for Data List */
    MMSd_svcCheck token_check; /* Enables service neg. checking */

    LENGTH_TYPE L; /* Used for checking CLOSES */

    void *connection; /* Info about association */
#if defined(MMSD_MAX_AA_NVLS)
    void *aa_connections; /* aa-specific NVLs */
#endif
    void *pStackContext; /* stack context */
    MMSd_descriptor Alt; /* Descriptor for alternate acc. */

    void *directory; /* Base of system directory */
    int directorySize; /* Size of system directory */

    /* -- The following two pointers are used during I/O operations */

    boolean isNVL; /* I/O for vars or NVL? */
    void *NVL; /* Pointer to NVL */

    int iCurDomain;
    int iCurObject;

    TMW_TrackingContext trackingContext;

#if defined(TMW_REPORT_ALL_CLIENT_WRITES)
    void *pWriteUpdateCache;
#endif

    /* Note - 'mini' context stops here */

    MMSd_ObjectListEntry objectList[MMSD_MAX_OBJECT_LIST_COUNT];
    MMSd_InvokeIdType invokeID; /* Request Invoke ID */
    int domainIndex; /* Used in processing objectName */
    short isClientRequest; /* Controls dictionary accesses */
    MMSd_AuthenticationType authenticationType; /* Authentication req'd or type */
    char auth_pw[MMSD_MAX_AUTH_LEVELS][33]; /* Passwords */

    /* Domain Up/Down loading state machines */
    /* These are void * to avoid having to carry around all of the
     include files */
    void *dls_avail;
    void *dls_active;

    void *pFreeList;
    void *client_context;
    void *serverInterface;
#if defined(TMW_USE_THREADS)
    TmwTargSem_t sem;
#endif
    char edition;
} MMSd_context;



/* The definition for the type is in user.h */
#define SERVER_INTERFACE( ctx )  ((MMSd_ServerInterfaceTable *) (ctx)->serverInterface)

/* File Attributes
 --
 -- Used for MMS file services.
 --------------------------------------------------------------
 */

typedef struct MMSd_FileAttributes {
    unsigned long size;
    char *lastModified;
} MMSd_FileAttributes;

TMW_CLIB_API int MMSd_fileAttrEncode(MMSd_FileAttributes * attribs, MMSd_descriptor * E);

#ifdef __cplusplus
};
#endif

#endif /* _CONTEXT_H */
