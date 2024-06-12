/*****************************************************************************/
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


/** @user.h
 *
 *  This file provides the ability for users to override the MMS service
 *  functions provided by the library.
 */

#ifndef _USER_H
#define _USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "context.h"
#include "connect.h"


int MMSd_Decode(MMSd_context *Context, MMSd_descriptor *Request, MMSd_descriptor *Response);

/* Initiate Parameter Definitions
 *  --- The following definitions are used to build the default
 *  --- bitstrings for the Initiate service.
 *
 */

/* --- Definitions of ServicesSupported */
/* Byte 0 */
#define MMSD_SERVICE_status                              0x80
#define MMSD_SERVICE_getNameList                         0x40
#define MMSD_SERVICE_identify                            0x20
#define MMSD_SERVICE_rename                              0x10
#define MMSD_SERVICE_read                                0x08
#define MMSD_SERVICE_write                               0x04
#define MMSD_SERVICE_getVarAccessAttr                    0x02
#define MMSD_SERVICE_defineNamedVar                      0x01

/* Byte 1 */
#define MMSD_SERVICE_defineScatteredAccess               0x80
#define MMSD_SERVICE_getScatteredAccessAttr              0x40
#define MMSD_SERVICE_deleteVarAccess                     0x20
#define MMSD_SERVICE_defineNamedVarList                  0x10
#define MMSD_SERVICE_getNamedVarListAttr                 0x08
#define MMSD_SERVICE_deleteNamedVarList                  0x04
#define MMSD_SERVICE_defineNamedType                     0x02
#define MMSD_SERVICE_getNamedTypeAttr                    0x01

/* Byte 2 */
#define MMSD_SERVICE_deleteNamedType                     0x80
#define MMSD_SERVICE_input                               0x40
#define MMSD_SERVICE_output                              0x20
#define MMSD_SERVICE_takeControl                         0x10
#define MMSD_SERVICE_relinquishControl                   0x08
#define MMSD_SERVICE_defineSemaphore                     0x04
#define MMSD_SERVICE_deleteSemaphore                     0x02
#define MMSD_SERVICE_reportSemaphoreStatus               0x01

/* Byte 3 */
#define MMSD_SERVICE_reportPoolSemaphoreStatus           0x80
#define MMSD_SERVICE_reportSemaphoreEntryStatus          0x40
#define MMSD_SERVICE_initiateDownloadSequence            0x20
#define MMSD_SERVICE_downloadSegment                     0x10
#define MMSD_SERVICE_terminateDownloadSequence           0x08
#define MMSD_SERVICE_initiateUploadSequence              0x04
#define MMSD_SERVICE_uploadSegment                       0x02
#define MMSD_SERVICE_terminateUploadSequence             0x01

/* Byte 4 */
#define MMSD_SERVICE_requestDomainDownload               0x80
#define MMSD_SERVICE_requestDomainUpload                 0x40
#define MMSD_SERVICE_loadDomainContent                   0x20
#define MMSD_SERVICE_storeDomainContent                  0x10
#define MMSD_SERVICE_deleteDomain                        0x08
#define MMSD_SERVICE_getDomainAttr                       0x04
#define MMSD_SERVICE_createProgramInvocation             0x02
#define MMSD_SERVICE_deleteProgramInvocation             0x01

/* Byte 5 */
#define MMSD_SERVICE_start                               0x80
#define MMSD_SERVICE_stop                                0x40
#define MMSD_SERVICE_resume                              0x20
#define MMSD_SERVICE_reset                               0x10
#define MMSD_SERVICE_kill                                0x08
#define MMSD_SERVICE_getProgramInvocationAttr            0x04
#define MMSD_SERVICE_obtainFile                          0x02
#define MMSD_SERVICE_defineEventCondition                0x01

/* Byte 6 */
#define MMSD_SERVICE_deleteEventCondition                0x80
#define MMSD_SERVICE_getEventConditionAttr               0x40
#define MMSD_SERVICE_reportEventConditionStatus          0x20
#define MMSD_SERVICE_alterEventConditionMonitoring       0x10
#define MMSD_SERVICE_triggerEvent                        0x08
#define MMSD_SERVICE_defineEventAction                   0x04
#define MMSD_SERVICE_deleteEventAction                   0x02
#define MMSD_SERVICE_getEventActionAttr                  0x01

/* Byte 7 */
#define MMSD_SERVICE_reportEventActionStatus             0x80
#define MMSD_SERVICE_defineEventEnrollment               0x40
#define MMSD_SERVICE_deleteEventEnrollment               0x20
#define MMSD_SERVICE_alterEventEnrollment                0x10
#define MMSD_SERVICE_reportEventEnrollmentStatus         0x08
#define MMSD_SERVICE_getEventEnrollmentAttr              0x04
#define MMSD_SERVICE_acknowledgeEventNotification        0x02
#define MMSD_SERVICE_getAlarmSummary                     0x01

/* Byte 8 */
#define MMSD_SERVICE_getAlarmEnrollmentSummary           0x80
#define MMSD_SERVICE_readJournal                         0x40
#define MMSD_SERVICE_writeJournal                        0x20
#define MMSD_SERVICE_initializeJournal                   0x10
#define MMSD_SERVICE_reportJournalStatus                 0x08
#define MMSD_SERVICE_createJournal                       0x04
#define MMSD_SERVICE_deleteJournal                       0x02
#define MMSD_SERVICE_getCapList                          0x01

/* Byte 9 */
#define MMSD_SERVICE_fileOpen                            0x80
#define MMSD_SERVICE_fileRead                            0x40
#define MMSD_SERVICE_fileClose                           0x20
#define MMSD_SERVICE_fileRename                          0x10
#define MMSD_SERVICE_fileDelete                          0x08
#define MMSD_SERVICE_fileDirectory                       0x04
#define MMSD_SERVICE_unsolicitedStatus                   0x02
#define MMSD_SERVICE_informationReport                   0x01

/* Byte 10 */
#define MMSD_SERVICE_eventNotification                   0x80
#define MMSD_SERVICE_attachToEventCondition              0x40
#define MMSD_SERVICE_attachToSemaphore                   0x20
#define MMSD_SERVICE_conclude                            0x10
#define MMSD_SERVICE_cancel                              0x08

/* --- Definitions of CBBs */

#define MMSD_CBB_str1                                    0x80
#define MMSD_CBB_str2                                    0x40
#define MMSD_CBB_vnam                                    0x20
#define MMSD_CBB_valt                                    0x10
#define MMSD_CBB_vadr                                    0x08
#define MMSD_CBB_vsca                                    0x04
#define MMSD_CBB_tpy                                     0x02
#define MMSD_CBB_vlis                                    0x01

#define MMSD_CBB_real                                    0x80
#define MMSD_CBB_akec                                    0x40
#define MMSD_CBB_cei                                     0x20

extern MMSd_initiateCBBs MMSd_def_CBBs;
extern MMSd_initiateServices MMSd_def_Services;


/* These need to be defined for initializing the default
 (Windows and LINUX) servers.  On many platforms they may
 be changed to be something else via the macros */

#if !defined(MMSD_ABORT_IND_ROUTINE)
void MMSd_acseAbortIndication(MMSd_context *cntxt);
#endif

#if !defined(MMSD_DEFINE_NVL_REQ_ROUTINE)
TMW_CLIB_API int MMSd_defineNVLRequestService(MMSd_context *cntxt, int domainIndex, MMSd_descriptor *D, MMSd_descriptor *E);
#endif

void MMSd_RemoveNvlReferences(MMSd_context *cntxt, MMSd_short domainIndex, MMSd_ushort nvlIndex);

int MMSd_deleteNvlRange(MMSd_context *cntxt, MMSd_short nvlDomainIndex, int bForceControlBlocks, long *numberMatched, long *numberDeleted);

#if !defined(MMSD_DELETE_NVL_REQ_ROUTINE)
TMW_CLIB_API int MMSd_deleteNVLRequestService(MMSd_context *cntxt, int scopeOfDelete, long *numberMatched, long *numberDeleted);
#endif

#if !defined( MMSD_ALLOC_DS_MEM_FUNCTION )
TMW_CLIB_API int MMSd_allocDsMemFunction(void *userData, long size, unsigned char **ptr);
#endif

#if !defined( MMSD_FREE_DS_MEM_FUNCTION )
TMW_CLIB_API void MMSd_freeDsMemFunction(void *userData, unsigned char *ptr);
#endif

/* Status response function ----------------------------------------
 -- The following types define the response codes for the status
 -- and unsolicited status functions.
 ------------------------------------------------------------------
 */

/* Choices to be returned by USER_STATUS_FUNCTION for vmdLogicalStatus
 -------------------------------------------------------------------
 */
typedef enum {
    state_changes_allowed,
    no_state_changes_allowed,
    limited_services_allowed,
    support_services_allowed
} vmdLogicalStatusType;

/* Choices to be returned by USER_STATUS_FUNCTION for vmdPhysicalStatus
 --------------------------------------------------------------------
 */

typedef enum {
    operational,
    partially_operational,
    inoperable,
    needs_comissioning
} vmdPhysicalStatusType;


/* MMSd_InitializeContext
 *  --- Called by user to initialize fields in the context record.
 *  -----------------------------------------------------------------
 */
void MMSd_InitializeContext(MMSd_context *contextPtr, MMSd_Connection *connectPtr);

unsigned int MMSd_maxNegotiatedPDUsize(MMSd_context *contextPtr);

#define MMSd_initDescMaxPduSize( CTX, _F_ )     \
{                                                   \
    MMSd_ushort temp;                               \
    temp = (MMSd_ushort) MMSd_maxNegotiatedPDUsize( CTX );    \
    if (temp < MMSd_DescLen( _F_ ))                 \
    {                                               \
        MMSd_DescLen( _F_ ) = temp;                 \
        MMSd_DescIndex( _F_ ) = (MMSd_ushort) (temp - 1);           \
    }                                               \
}

MMSd_length Emit_presentation_pdu(MMSd_context *context, MMSd_descriptor *E);

void Pres_init_contexts(MMSd_context *_CNTX);

int Pres_store_ctx(MMSd_context *_CNTX, int ctx_id, int *ctx_abstract, int *ctx_transfer, int **pres_abstract_oids, int num_abstract_oids, int **pres_transfer_oids, int num_transfer_oids);

typedef int (*MMSd_InitiateReqInd)(unsigned int *localDetail, int *nestLevel, int *state, void *userData);

typedef int (*MMSd_ConcludeReqInd)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter);

typedef void (*MMSd_StatusReqInd)(void *userData, int derivation, int *logical, int *physical);

/* Called from MMS GRAMMAR */
typedef int (*MMSd_DomainAttrReqInd)(MMSd_context *ctx, MMSd_Connection *mms_conn, char *name, MMSd_DomainAttributes *attributes, MMSd_errorFlag *errCode);

typedef int (*MMSd_DefineNvlReqInd)(MMSd_context *cntxt, int domainIndex, MMSd_descriptor *D, MMSd_descriptor *E);

typedef int (*MMSd_DeleteNvlReqInd)(MMSd_context *cntxt, int scopeOfDelete, long *numberMatched, long *numberDeleted);

typedef int (*MMSd_DLSMachineStoreFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, unsigned char *buffer, int length, int moreFollows, MMSd_errorFlag *errCode);

typedef int (*MMSd_DLSMachineEndFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, int expected, MMSd_errorFlag *errCode);

typedef void (*MMSd_DLSMachineErrorFcn)(MMSd_context *ctx, MMSd_Connection *mms_conn, void *parameter, MMSd_errorFlag errCode);

typedef int (*MMSd_InitiateDownloadReqInd)(MMSd_context *ctx, char *name, void **parameter, MMSd_DLSMachineStoreFcn *store_callback, MMSd_DLSMachineEndFcn *end_callback,
        MMSd_DLSMachineErrorFcn *err_callback);

typedef int (*MMSd_FileOpenReqInd)(MMSd_context *cntxt, unsigned long pos, unsigned long *frsmID, MMSd_FileAttributes *attribs, MMSd_errorFlag *errcode);

typedef int (*MMSd_FileReadReqInd)(MMSd_context *cntxt, unsigned long frsmID, unsigned char *ptr, unsigned int length, int *moreFollows, MMSd_errorFlag *errcode);

typedef int (*MMSd_FileCloseReqInd)(MMSd_context *cntxt, unsigned long frsmID, MMSd_errorFlag *errcode);

typedef int (*MMSd_FileDeleteReqInd)(MMSd_context *cntxt, MMSd_errorFlag *errcode);

typedef int (*MMSd_FileDirectoryReqInd)(MMSd_context *cntxt, MMSd_errorFlag *errcode);

typedef void (*MMSd_ObtainFileReqInd)(MMSd_context *cntxt, int sourceFileIndex, int destFileIndex, int *errcode);

typedef int (*MMSd_AllocDsMemFcn)(void *userData, long size, unsigned char **ptrReturned);

typedef void (*MMSd_FreeDsMemFcn)(void *userData, unsigned char *ptrToFree);

typedef void (*MMSd_WakeReportingFcn)(MMSd_context *pMmsContext);

typedef unsigned long (*MMSd_generateReportFcn)(void *tpConn, void *tpContext, MMSd_context *mms_context, void *pEventReportStatus, int nEventReportStatus, unsigned char *outBuff,
        MMSd_length outBuffLen);

typedef int (*MMSd_AddressSearch)(MMSd_context *cntxt, MMSd_context **pFoundMmsContext);


typedef int (*MMSd_CB_write_callback)( MMSd_context *cntxt, MMSd_ReportControl *reportControl, char * attr, void * value);
typedef int (*MMSd_CB_read_callback)( MMSd_context *cntxt, MMSd_ReportControl *reportControl, char * attr, void * value);
typedef int (*MMSd_report_callback)( MMSd_context *cntxt, MMSd_ReportControl *reportControl, char* name, unsigned char reason, int length );

/* This function pointer is needed for systems using combined client/servers.  It is used
   to check for outstanding client transactions when the server receives an MMS Conclude
   from the remote side.  It will cause the conclude to be refused if there are outbound
   client transactions on the same connection. */

typedef int (* MMSd_CheckForClientServices)( MMSd_context *cntxt, MMSd_Connection *mms_connection );

/* This defines the interfaces to common Server-side indication
   functions.  On many platforms, one or more of these function
   pointers will point to custom functions which will handle the
   indication. */
typedef struct MMSd_ServerInterfaceTable {
    MMSd_InitiateReqInd         initiateReqInd;
    MMSd_ConcludeReqInd         concludeReqInd;
    MMSd_StatusReqInd           statusReqInd;
    MMSd_DomainAttrReqInd       domainAttrReqInd;
    MMSd_DefineNvlReqInd        defineNvlReqInd;
    MMSd_DeleteNvlReqInd        deleteNvlReqInd;
    MMSd_InitiateDownloadReqInd initiateDownloadReqInd;
    MMSd_FileOpenReqInd         fileOpenReqInd;
    MMSd_FileReadReqInd         fileReadReqInd;
    MMSd_FileCloseReqInd        fileCloseReqInd;
    MMSd_FileDeleteReqInd       fileDeleteReqInd;
    MMSd_FileDirectoryReqInd    fileDirectoryReqInd;
    MMSd_ObtainFileReqInd       obtainFileReqInd;
    MMSd_AddressSearch          addressSearch;
    MMSd_AllocDsMemFcn          allocDsMemFcn;
    MMSd_FreeDsMemFcn           freeDsMemFcn;
    char                        *vendorName;
    char                        *modelName;
    int                         revision;
    unsigned char               *paramCbbs;
    unsigned char               *servicesSupported;
    char                        **capabilityList;
    int                         capabilityListSize;
    MMSd_WakeReportingFcn       wakeReporting;
    MMSd_generateReportFcn      generateReport;
    MMSd_CheckForClientServices clientServiceCheck;
    TMW_MakeSignature           makeSignatureFcn;
    TMW_CheckSignature          checkSignatureFcn;
    void                        *pCryptoUserContext;
    void                        *pCryptoUserData;
    MMSd_CB_write_callback		pCB_write_callback;
    MMSd_CB_read_callback       pCB_read_callback;
    MMSd_report_callback        pReport_callback;
} MMSd_ServerInterfaceTable;

#if defined(MMSD_USE_DOMAIN_DOWNLOAD)
#if defined(MMSD_USER_INITIATE_DOWNLOAD)
#define MMSD_SERVER_INITIATE_DOWNLOAD   MMSD_USER_INITIATE_DOWNLOAD
#else
#define MMSD_SERVER_INITIATE_DOWNLOAD   MMSd_initiateDownloadServiceRequest
#endif
#else  /* MMSD_USE_DOMAIN_DOWNLOAD */
#define MMSD_SERVER_INITIATE_DOWNLOAD       NULL
#endif /* MMSD_USE_DOMAIN_DOWNLOAD */

#if defined(MMSD_USE_FILE_SERVICES)
#define MMSD_SERVER_FILE_OPEN       MMSd_fileOpenServiceRequest
#define MMSD_SERVER_FILE_READ       MMSd_fileReadServiceRequest
#define MMSD_SERVER_FILE_CLOSE      MMSd_fileCloseServiceRequest
#define MMSD_SERVER_FILE_DELETE     MMSd_fileDeleteServiceRequest
#define MMSD_SERVER_FILE_DIRECTORY  MMSd_fileDirectoryServiceRequest
#define MMSD_SERVER_OBTAIN_FILE     MMSd_obtainFileServiceRequest
#else
#define MMSD_SERVER_FILE_OPEN       NULL
#define MMSD_SERVER_FILE_READ       NULL
#define MMSD_SERVER_FILE_CLOSE      NULL
#define MMSD_SERVER_FILE_DELETE     NULL
#define MMSD_SERVER_FILE_DIRECTORY  NULL
#define MMSD_SERVER_OBTAIN_FILE     NULL
#endif

#if defined(MMSD_DYNAMIC_NVLS_LOGIC)
#if defined(MMSD_DEFINE_NVL_REQ_ROUTINE)
#define MMSD_SERVER_DEFINE_NVL  MMSD_DEFINE_NVL_REQ_ROUTINE
#else
#define MMSD_SERVER_DEFINE_NVL  MMSd_defineNVLRequestService
#endif

#if defined(MMSD_DELETE_NVL_REQ_ROUTINE)
#define MMSD_SERVER_DELETE_NVL  MMSD_DELETE_NVL_REQ_ROUTINE
#else
#define MMSD_SERVER_DELETE_NVL  MMSd_deleteNVLRequestService
#endif

#if defined(MMSD_ALLOC_DS_MEM_FUNCTION)
#define MMSD_SERVER_ALLOC_DS    MMSD_ALLOC_DS_MEM_FUNCTION
#else
#define MMSD_SERVER_ALLOC_DS    MMSd_allocDsMemFunction
#endif

#if defined(MMSD_FREE_DS_MEM_FUNCTION)
#define MMSD_SERVER_FREE_DS     MMSD_FREE_DS_MEM_FUNCTION
#else
#define MMSD_SERVER_FREE_DS     MMSd_freeDsMemFunction
#endif
#else   /* MMSD_DYNAMIC_NVLS_LOGIC */
#define MMSD_SERVER_DEFINE_NVL      NULL
#define MMSD_SERVER_DELETE_NVL      NULL
#define MMSD_SERVER_ALLOC_DS        NULL
#define MMSD_SERVER_FREE_DS         NULL
#endif  /* MMSD_DYNAMIC_NVLS_LOGIC */

#if defined(MMSD_SERVER_ADDRESS_SEARCH_FUNCTION)
#define MMSD_SERVER_ADDRESS_SEARCH  MMSD_SERVER_ADDRESS_SEARCH_FUNCTION
#else
#define MMSD_SERVER_ADDRESS_SEARCH  NULL
#endif

#if defined(MMSD_SERVER_GENERATE_REPORT_FUNCTION)
#define MMSD_SERVER_GENERATE_REPORT  MMSD_SERVER_GENERATE_REPORT_FUNCTION
#else
#define MMSD_SERVER_GENERATE_REPORT  NULL
#endif


typedef struct MMSd_Server *MMSd_ServerPtr;
typedef struct MMSd_Server {
    /* NOTE: Keep this first to simplify freeing */
    MMSd_context mmsContext;
    char *pIedName;
#if defined(MMSD_ADDR_INTERFACE)
    MMSd_Stack_Addresses stackAddresses;
    TMW_Stack_Address nsap_called;
#endif
    MMSd_ServerPtr pNext;
} MMSd_Server;



void MMSd_PerformClientService(void *pVoidClientContext);

void MMSd_SignalClient(void *pVoidClientContext);

void MMSd_GetClientEvent(void *pVoidClientContext, void *pVoidEvent);


TMW_CLIB_API void *MMSd_MallocAndSave(size_t size, void **pSaveList);

TMW_CLIB_API void MMSd_FreeMallocList(void *pSaveList);

TMW_CLIB_API void MMSd_AppendMallocList(void *pNewListPassed, void **pSaveList);

TMW_CLIB_API void *MMSd_MallocAndSaveContext(size_t size, MMSd_context *pMmsContext);

TMW_CLIB_API void MMSd_FreeMallocListContext(MMSd_context *pMmsContext);

TMW_CLIB_API void MMSd_AppendMallocListContext(void *pNewListPassed, MMSd_context *pMmsContext);

TMW_CLIB_API void MMSd_freeContextAllocations(MMSd_context *pMmsContext);

#ifdef __cplusplus
};
#endif

#endif /* _USER_H */
