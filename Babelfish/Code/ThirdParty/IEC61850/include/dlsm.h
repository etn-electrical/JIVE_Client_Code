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

/** @dlsm.h
 *
 *  This file contains
 *  Declarations for Download State Machine Services.
 *
 *  This file should not require modification.
 */

#ifndef _DLSM_H
#define _DLSM_H

#ifdef __cplusplus
extern "C" {
#endif

/* The invoke IDs that we generate must be unique.  Each
 service type that we support will use a different range
 so that we never generate duplicates, but can still
 encode useful information in the rest of the field. */
#define MMSD_DOWNLOAD_MASK 0x10000000
#define MMSD_IS_DOWNLOAD_INVOKE_ID( id ) \
    ( ( id & MMSD_DOWNLOAD_MASK) == MMSD_DOWNLOAD_MASK )

typedef enum {
    MMSd_loadIdle,
    MMSd_loadInited,
    MMSd_loadSegWait,
    MMSd_loadLast,
    MMSd_loadComplete,
    MMSd_loadInitiating,
    MMSd_loading,
    MMSd_loadDone
}
MMSd_LoadingState;

typedef struct MMSd_DLSMachine *MMSd_DLSMachinePtr;

typedef int (*MMSd_DLSMachineGetSegFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, unsigned char *buffer, unsigned int buffSize, int *moreFollows, MMSd_errorFlag *errCode);

typedef void (*MMSd_DLSMachineStartFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, char *name);

typedef void (*MMSd_DLSMachineDoneFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, int expected);

typedef struct MMSd_DLSMachine {
    char name[33];
    int identifier;
    MMSd_LoadingState state;
    MMSd_Connection *connection;
    MMSd_context *mms_ctx;
    MMSd_InvokeIdType last_invokeId;
    void *user_download_data;
    MMSd_DLSMachinePtr next;
    MMSd_DLSMachinePtr next_connection;
    MMSd_DLSMachineStoreFcn store_callback;
    MMSd_DLSMachineEndFcn end_callback;
    MMSd_DLSMachineErrorFcn err_callback;
    MMSd_DLSMachineGetSegFcn getseg_callback;
    MMSd_DLSMachineDoneFcn done_callback;
    MMSd_DLSMachineStartFcn start_callback;
} MMSd_DLSMachine;

/* Prototypes for managing DLS Machines */
MMSd_DLSMachine *MMSd_allocDLSMachine(MMSd_context *ctx);
void MMSd_freeDLSMachine(MMSd_DLSMachine *mach, MMSd_context *ctx);

void MMSd_activateDLSMachine(MMSd_DLSMachine *mach, MMSd_context *ctx);

MMSd_DLSMachine *MMSd_removeDLSMachine(MMSd_DLSMachine *mach, MMSd_DLSMachine **list);

MMSd_DLSMachine *MMSd_findDLSMachine(char *name, MMSd_DLSMachine *list);

MMSd_DLSMachine *MMSd_retrieveDSLMachine(MMSd_context *ctx, MMSd_InvokeIdType invokeid);

void MMSd_removeConDLSMachine(MMSd_DLSMachine *mach, MMSd_Connection *connection);

/* Prototypes for DLS Protocol events */
MMSd_length MMSd_downloadSegmentRequest(MMSd_context *ctx, char *name, unsigned char *buffer, unsigned int size, int *moreFollows, MMSd_errorFlag *errCode);

int MMSd_terminateDownloadIndication(MMSd_context *ctx, char *name);

int MMSd_downloadSegmentIndication(MMSd_context *ctx, unsigned long invokeid, unsigned char *buffer, int length, int moreFollows, MMSd_errorFlag *errCode);

int MMSd_terminateDownloadConfirm(MMSd_context *ctx, unsigned long invokeid, MMSd_errorFlag *errCode);

int MMSd_initiateDownloadIndication(MMSd_context *ctx, char *name, int shareFlag, MMSd_errorFlag *errCode);

/* Prototypes for DLS utility routines */
void MMSd_abortDLSMachines(MMSd_context *ctx, MMSd_Connection *conn);

void MMSd_initDLSMachines(MMSd_context *ctx);

void MMSd_purgeConDLSMachine(MMSd_context *ctx, MMSd_Connection *connection);

void MMSd_cleanupDLSMachine(MMSd_context *ctx, MMSd_Connection *connection, MMSd_DLSMachine *mach);

int MMSd_encodeSegmentRequest(MMSd_context *ctx, MMSd_DLSMachine *mach, unsigned char *buffer, int buffer_length, int *buffer_index);

int MMSd_encodeTerminateDownloadRequest(MMSd_context *ctx, MMSd_DLSMachine *mach, unsigned char *buffer, int buffer_length, int *buffer_index);

#if !defined(MMSD_USER_INITIATE_DOWNLOAD)
/* Use the default (dummy) module */
TMW_CLIB_API int MMSd_initiateDownloadServiceRequest(MMSd_context *ctx, char *name, void **parameter, MMSd_DLSMachineStoreFcn *store_callback, MMSd_DLSMachineEndFcn *end_callback,
                                        MMSd_DLSMachineErrorFcn *err_callback);

#endif  /* MMSD_USER_INITIATE_DOWNLOAD */

#if !defined(MMSD_USER_DOMAIN_ATTR)
TMW_CLIB_API int MMSd_domainAttrServiceRequest(MMSd_context *ctx, MMSd_Connection *mms_conn, char *name, MMSd_DomainAttributes *attributes, MMSd_errorFlag *errCode);
#endif

#ifdef __cplusplus
}
;
#endif

#endif      /* _DLSM_H */
