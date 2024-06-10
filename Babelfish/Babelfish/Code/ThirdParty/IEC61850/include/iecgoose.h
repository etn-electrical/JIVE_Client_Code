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
 *  Declarations for IEC 61850 Goose Interface.
 *
 *  This file should not require modification.
 */

#ifndef _IECGOOSE_H
#define _IECGOOSE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TMW_USE_90_5)
#include "spp.h"        /* 90-5 Extensiom */
#endif

/* There are three styles of buffer allocation supported:

 IEC_GOOSE_STATIC_ALLOCATION   - same as old style, can still be overridded with RFC1006_ENTRY_POOL_DEFS, etc.
 IEC_GOOSE_DYNAMIC_ALLOCATION  - boot time malloc, so that multiple transport contexts can be opened
 IEC_GOOSE_UNBOUND_ALLOCATION  - run-time mallocs, with no limits (used for workstation type applications)

 */

/* No more than one can be defined in the package */
/* This should generate an error unless at most one is defined */
#if defined(IEC_GOOSE_STATIC_ALLOCATION)
#define IEC_GOOSE_ALLOCATION 1
#endif
#if defined(IEC_GOOSE_DYNAMIC_ALLOCATION)
#define IEC_GOOSE_ALLOCATION 2
#endif
#if defined(IEC_GOOSE_UNBOUND_ALLOCATION)
#define IEC_GOOSE_ALLOCATION 3
#endif

/* If none of them defined, assume old-style static or overrides */
#if !defined(IEC_GOOSE_ALLOCATION)
#define IEC_GOOSE_STATIC_ALLOCATION
#endif


/* #define TEST_GOOSE */
#if !defined(IEC_GOOSE_MAX_ELEMENTS)
#define IEC_GOOSE_MAX_ELEMENTS 300
#endif

/*   Default VLAN pars */
#define IEC_DEF_VLAN_GOOSE_VID      0
#define IEC_DEF_VLAN_GOOSE_APPID    0
#define IEC_DEF_VLAN_GOOSE_PRI      4
#define IEC_DEF_VLAN_GSE_VID        0
#define IEC_DEF_VLAN_GSE_APPID      1
#define IEC_DEF_VLAN_GSE_PRI        4

typedef enum {
    IEC_GOOSE_SUCCESS,
    IEC_GOOSE_ERR_LINK,
    IEC_GOOSE_ERR_SIZE,
    IEC_GOOSE_ERR_NO_BUFFERS,
    IEC_GOOSE_ERR_PARAMS,
    IEC_GOOSE_ERR_SIGNATURE
}
IEC_Goose_Error;

typedef struct IEC_Goose_Context *IEC_Goose_ContextPtr;

#define IEC_GOOSE_MAX_DATA_LEN  1518
#define IEC_GOOSE_LSAP  0xF5

#define IEC_GOOSE_IDENT_SIZE 128 // added macro
#define MAX_IEC_GOOSE_PROFILES 30 //added macro

#if !defined(IEC_GOOSE_IDENT_SIZE )
#define IEC_GOOSE_IDENT_SIZE MMSD_OBJECT_NAME_LEN
#endif

typedef struct TMW_LanHeader {
    int etherType;
    int lsap;
    int VLAN_Priority;
    unsigned int VLAN_VID;
    int ApplID;
    unsigned char   simulateBit;
    int lengthOfExtension;
} TMW_LanHeader;

typedef struct IEC_GooseHeader {
    char             gocbRef[IEC_GOOSE_IDENT_SIZE];
    unsigned long    HoldTim;
    char             datSet[IEC_GOOSE_IDENT_SIZE];
    char             goID[IEC_GOOSE_IDENT_SIZE];
    MMSd_UtcBTime    t;
    unsigned long    StNum;
    unsigned long    SqNum;
    unsigned char    test;
    long             confRev;
    unsigned char    ndsCom;
    unsigned long    numDataSetEntries;
    unsigned char    fixedOffs;
    unsigned int     VLAN_AppID;
    unsigned int     VLAN_VID;
    unsigned char    VLAN_Priority;
    unsigned char    simulateBit;
    unsigned char    *pSecurity;
    int              nSecurityLen;
    int             lengthOfExtension;
    unsigned char *pPrivateSecurityData;
    int           nPrivateSecurityDataLen;
    TMW_61850_VLAN_MakeSignatureFcn     genVlanSignature;       /* Goose outputs only */
    TMW_61850_VLAN_CheckSignatureFcn    checkVlanSignature;     /* Goose inputs only */
    void                                *pCryptoContext;        /* Goose inputs and outputs */
    void                                *pVlanSignatureData;    /* Goose inputs and outputs */
} IEC_GooseHeader;

typedef unsigned long IEC_GooseInterval;

/* Since its not standardized yet, we include various options for
 * specifying the GOOSE time sequence:
 *
 *  Arithmetic  - next interval = last_interval * multiplier
 *  Profiles    - user-specified intervals, after which one of the other
 *              two types are used.
 */
typedef enum {
    IEC_GOOSE_Disabled,
    IEC_GOOSE_Arithmetic,
    IEC_GOOSE_Profile
} IEC_GooseType;

typedef struct { /* GOOSE_Arithmetic */
    unsigned long        multiplier;
    IEC_GooseInterval   firstInterval;
    IEC_GooseInterval   maxInterval;
} IEC_GooseAriPars;

typedef struct { /* GOOSE_Profile */
    int                 numProfiles;
    int                 currentIndex;
    IEC_GooseInterval  profiles[MAX_IEC_GOOSE_PROFILES];
} IEC_GooseProPars;

typedef struct {
    IEC_GooseType      strategyType;
    IEC_GooseAriPars   ari;
    IEC_GooseProPars   pro;
} IEC_Goose_Strategy;

typedef enum {
    tmwGooseProtocol_Normal,
    tmwGooseProtocol_UDP
} tmwGooseProtocol_t; /* 90-5 Extension */

typedef struct IEC_GooseStreamStatus {
    TMW_ms_time         timeStarted;
    TMW_ms_time         timeLastSent;
    TMW_ms_time         timeNext;
    IEC_GooseInterval   currentInterval;
    IEC_GooseInterval   nextInterval;
    unsigned long       StNum;
    unsigned long       SqNum;
    int                 have_received;
    int                 initial_wait;
    unsigned long       HoldTim;
} IEC_GooseStreamStatus;

/* NOTE:
    The simulate and test functionality for input is simple - they are simply
    delivered to the user in the Goose header.  For output, however, it is a
    little trickier.  We do not want to change the value of the test and simulate
    flags during retransmission - the retransmission should always contain the
    data that was last published. This means we have to maintain the test/simulate
    flags in the GooseStatue (below), and copy the value into the header ONLY when
    we are publishing new data, not when retransmitting */

typedef struct IEC_GooseStatus {
    IEC_GooseStreamStatus   normal;
    IEC_GooseStreamStatus   simulated;
    unsigned char           simulateNextPublished;
    unsigned char           testNextPublished;
    TMW_Mac_Address         address;
    TMW_Mac_Address         srcAddress;
    IEC_Goose_Strategy      strategy;
    IEC_GooseHeader         header;
    tmwGooseProtocol_t      protocol;           /* 90-5 Extension */
#if defined(TMW_USE_90_5)
    TMW_SPP_Data            sppData;            /* 90-5 Extension */
#endif
    IEC_Goose_ContextPtr    goose_context;
    unsigned char           goose_data[ IEC_GOOSE_MAX_DATA_LEN ];
    int                     goose_data_len;
    void                    *handle;
} IEC_GooseStatus;


typedef enum {
    Unknown_Mgmt_Req,
    GSE_Elements_Req,
    GOOSE_Elements_Req,
    GSE_Elements_Res,
    GOOSE_Elements_Res,
    GSE_References_Req,
    GOOSE_References_Req,
    GSE_References_Res,
    GOOSE_References_Res,
    GOOSE_Unsupported_Res,
} IEC_PrimitiveType;

typedef enum {
    IEC_GblError_other,
    IEC_GblError_unknownControlBlock,
    IEC_GblError_responseTooLarge,
    IEC_GblError_controlBlockConfigurationError
} IEC_GlbErrors;

typedef enum {
    IEC_ErrorReason_other,
    IEC_ErrorReason_notFound
} IEC_ErrorReason;

typedef enum {
    GSEMGMT_Error_Other,
    GSEMGMT_Error_UnknownReference,
    GSEMGMT_Error_UnableToSegmentResponse
} GSEMGMT_ErrorCode;

/* Goose indication callback function pointer */
typedef void (*IEC_Goose_Indication)(void *handle, void *param, IEC_GooseHeader *header, unsigned char *dataPtr, MMSd_length dataLength, int timed_out);

typedef int (*IEC_Goose_GetReferenceReq_Indication)(IEC_Goose_ContextPtr gseCntxt, void *mms_cntxt, void *param, char *gocbRef, unsigned char *dataPtr, MMSd_length dataLen, unsigned char *securityPtr,
        MMSd_length securityLen, char *dataSetName, TMW_Packet *packet, long *pConfRev, int *pGblError);

typedef void (*IEC_Goose_GetReferenceRes_Indication)(void *param, unsigned char *dataPtr, MMSd_length dataLen, unsigned char *securityPtr, MMSd_length securityLen, unsigned long stateId,
        char *gocbRef, char *dataSetName, unsigned long confRev, int nGblError);

typedef int (*IEC_Goose_GetElementNumReq_Indication)(IEC_Goose_ContextPtr gseCntxt, void *mms_cntxt, void *param, char *gocbRef, unsigned char *dataPtr, MMSd_length dataLen,
        unsigned char *securityPtr, MMSd_length securityLen, char *dataSetName, TMW_Packet *packet, long *pConfRev, int *pGblError);

typedef void (*IEC_Goose_GetElementNumRes_Indication)(void *param, unsigned char *dataPtr, MMSd_length dataLen, unsigned char *securityPtr, MMSd_length securityLen, unsigned long stateId,
        char *gocbRef, char *dataSetName, unsigned long confRev, int nGblError);

typedef void (*IEC_Goose_NotSupportedRes_Indication)(void *param, unsigned long stateId, unsigned char *securityPtr, MMSd_length securityLen);

typedef struct IEC_Goose_Subscription *IEC_Goose_SubscriptionPtr;

#if defined(USE_MMSD_IN_IEC_GOOSE)
#define MAX_LGOS_INSTANCES 13
typedef struct MMSd_lgos_t {
    MMSd_ObjectName * LGOS_GoCBRef;
    MMSd_Boolean * LGOS_Status;
    MMSd_Integer * LGOS_StNum;
    MMSd_Integer * LGOS_RxConf;
} MMSd_lgos_t;
#else
#define MMSd_lgos_t void
#endif

typedef struct IEC_Goose_Subscription {
    char                        goID[IEC_GOOSE_IDENT_SIZE];
    char                        gocbRef[IEC_GOOSE_IDENT_SIZE];
    char                        datSet[IEC_GOOSE_IDENT_SIZE];
    void                       *handle;
    int                         goID_sum;
    unsigned int                VLAN_AppID;
    unsigned int                VLAN_VID;
    unsigned char               VLAN_Priority;
    IEC_Goose_Indication        call_back;
    void                       *call_back_data;
    unsigned long               maxInterval;
    IEC_GooseStatus             status;
    TMW_61850_VLAN_MakeSignatureFcn     genVlanSignature;       /* Goose outputs only */
    TMW_61850_VLAN_CheckSignatureFcn    checkVlanSignature;     /* Goose inputs only */
    void                                *pCryptoContext;        /* Goose inputs and outputs */
    void                                *pVlanSignatureData;    /* Goose inputs and outputs */
    MMSd_lgos_t            * lgos;
    IEC_Goose_SubscriptionPtr   next;
} IEC_Goose_Subscription;

typedef void (*IEC_Goose_Send_Ind)(void *handle, IEC_Goose_Subscription *subscr, int retrans);

typedef void (*IEC_Goose_PreSend_Ind)( IEC_GooseStatus * status );

typedef void (*IEC_Goose_SqStats_Ind)(void *handle, IEC_Goose_Subscription *subscr, IEC_GooseHeader *header);

typedef enum {
    IEC_GOOSE_SEND_allData11
} IEC_GooseSendType;


typedef struct IEC_Goose_Context {
    IEC_Goose_Subscription *avail_pool;
    IEC_Goose_Subscription *active_in;
    IEC_Goose_Subscription *active_out;
    char adapter_name[129];
    TMW_DL_Context *link_context;
    IEC_Goose_Send_Ind send_indication;
    IEC_Goose_PreSend_Ind presend_indication;
    IEC_Goose_SqStats_Ind SqStats_indication;
    IEC_Goose_GetReferenceReq_Indication getReferenceReqFcn;
    void *getReferenceReqParam;
    IEC_Goose_GetReferenceRes_Indication getReferenceResFcn;
    void *getReferenceResParam;
    IEC_Goose_GetElementNumReq_Indication getElementNumReqFcn;
    void *getElementNumReqParam;
    IEC_Goose_GetElementNumRes_Indication getElementNumResFcn;
    void *getElementNumResParam;
    IEC_Goose_NotSupportedRes_Indication notSupportedResFcn;
    void *notSupportedResParam;
    TmwTargSem_t sem;
    TMW_TARG_EVENT eve;
    /* NOTE: These are not used if USE_MMSD_IN_IEC_GOOSE is not defined */
    void *directory;
    int directorySize;
    IEC_GooseSendType sendtype;
} IEC_Goose_Context;

int _IEC_gooseParse( IEC_Goose_Context *cntxt, TMW_Mac_Address *srcAddr, TMW_LanHeader *lanAddress,
					 unsigned char *buffer, int len, IEC_GooseHeader *header, unsigned char **dataPtr,
                     MMSd_length *dataLen);

/* ------------------------------------------- */
/* User-callable GOOSE Administrative Routines */
/* ------------------------------------------- */

/* Initializes internal goose database */
/* -- Returns TRUE if initialization succeeded */
TMW_CLIB_API int IEC_Goose_Initialize(IEC_Goose_Context *cntxt);

TMW_CLIB_API void IEC_Goose_Close(IEC_Goose_Context *cntxt);

/* Drives the retransmission and timeouts - call it often */
unsigned long IEC_Goose_Service_Short(IEC_Goose_Context *cntxt);

void IEC_Goose_Service(IEC_Goose_Context *cntxt);

/* Called when a message is received over 802.3 which might be Goose */
/* -- Returns TRUE if valid goose message */
int IEC_Goose_Evaluate(TMW_DL_Context *dl_cntxt, unsigned char *buffer, int length, void *vcntxt);

int TMW_decode_VLAN_header(unsigned char *buffer, int bufferLength, int *offset, int *length, TMW_LanHeader *header);
int TMW_encode_VLAN_header(TMW_Packet *pPacket, TMW_LanHeader *header);

/* ---------------------------------- */
/* User-callable GOOSE Input Routines */
/* ---------------------------------- */

/* Subscribes to a goose input stream */
/* -- Returns TRUE if subscription succeeded */
TMW_CLIB_API int IEC_Goose_Subscribe(IEC_Goose_Context *cntxt, char *goID, char *gocbRef, TMW_Mac_Address *mac, unsigned int AppID, void *handle, IEC_Goose_Indication callBack, void *callBackData);

TMW_CLIB_API int IEC_Goose_SubscribeSecure(IEC_Goose_Context *cntxt, char *goID, char *gocbRef, TMW_Mac_Address *mac, unsigned int AppID, void *handle, IEC_Goose_Indication callBack, void *callBackData,
                              TMW_61850_VLAN_CheckSignatureFcn checkVlanSignature, void *pCryptoContext, void *pVlanSignatureData);

TMW_CLIB_API int IEC_Goose_Subscribe_Common( IEC_Goose_Context      *cntxt,
								char                   *goID,
								char                   *gocbRef,
								TMW_Mac_Address        *mac,
								unsigned int           appID,
								void                   *handle,
								IEC_Goose_Indication   callBack,
								void                   *callBackData,
								tmwGooseProtocol_t     protocol		);

#if defined(TMW_USE_90_5)
TMW_CLIB_API int IEC_Goose_SubscribeUDP( IEC_Goose_Context *cntxt, char *goID, char *gocbRef, TMW_Mac_Address *address, int port, unsigned int nAppID, void *handle, IEC_Goose_Indication callBack,
                            void *callBackData);

#endif

/* Cancels a goose subscription */
/* -- Returns TRUE if unsubscribe succeeded */
TMW_CLIB_API int IEC_Goose_Unsubscribe(IEC_Goose_Context *cntxt, void *handle);

TMW_CLIB_API int IEC_Goose_SetInitialWait(IEC_Goose_Context *cntxt, void *handle, unsigned long timeToWait);

int IEC_Goose_GetOutputTestMode(IEC_Goose_Context *cntxt, void *handle, unsigned char *testMode);

int IEC_Goose_SetOutputTestMode(IEC_Goose_Context *cntxt, void *handle, unsigned char testMode);

int IEC_Goose_GetOutputSimulateMode(IEC_Goose_Context *cntxt, void *handle, unsigned char *testMode);

int IEC_Goose_SetOutputSimulateMode(IEC_Goose_Context *cntxt, void *handle, unsigned char testMode);

/* ----------------------------------- */
/* User-callable GOOSE Output Routines */
/* ----------------------------------- */

/* Establishes a goose output stream */
/* -- Returns IEC_GOOSE_SUCCESS if creation succeeded */
IEC_Goose_Error IEC_Goose_Create(
    IEC_Goose_Context *cntxt,         /* context            */
    char              *goID,          /* goID               */
    char              *gocbRef,       /* goose control      */
    char              *dsRef,         /* DataSet            */
    TMW_Mac_Address   *mac,           /* multicast address  */
    IEC_Goose_Strategy *strategy,     /* retrans strategy   */
    long              confRev,        /* dataset config     */
    int               numDataSetEntries, /* objects in data */
    unsigned char     bFixedOffs,     /* fixed goose encoding option */
    unsigned char     *buffer,        /* initial data       */
    MMSd_length       length,         /* length of data     */
    void              *handle,        /* user handle        */
    unsigned int      VLAN_AppID,     /* VLAN App ID        */
    unsigned int      VLAN_VID,       /* VLAN Virtual LAN ID*/
    unsigned char     VLAN_Priority );/* VLAN Priority      */


IEC_Goose_Error IEC_Goose_CreateOut( IEC_Goose_Context *cntxt,
                                     char              *goID,
                                     char              *gocbRef,
                                     char              *dsRef,
                                     TMW_Mac_Address   *mac,
                                     IEC_Goose_Strategy *strategy,
                                     long              confRev,
                                     unsigned char     ndsCom,
                                     unsigned char     test,
                                     int               numDataSetEntries,
                                     unsigned char     bFixedOffs,     /* fixed goose encoding option */
                                     unsigned char     *buffer,
                                     MMSd_length       length,
                                     void              *handle,
                                     unsigned int      VLAN_AppID,
                                     unsigned int      VLAN_VID,
                                     unsigned char     VLAN_Priority );

/* The makeSignature function is called to generate security signature */
/*  See src\tam\tam.h for prototype (common to both Goose and Sampled Values */
TMW_CLIB_API  IEC_Goose_Error IEC_Goose_CreateSecure( IEC_Goose_Context               *cntxt,
                                        char                            *goID,
                                        char                            *gocbRef,
                                        char                            *dsRef,
                                        TMW_Mac_Address                 *mac,
                                        IEC_Goose_Strategy              *strategy,
                                        long                            confRev,
                                        unsigned char                   ndsCom,
                                        unsigned char                   test,
                                        unsigned char                   simulateBit,
                                        int                             numDataSetEntries,
                                        unsigned char                   bFixedOffs,     /* fixed goose encoding option */
                                        unsigned char                   *buffer,
                                        MMSd_length                     length,
                                        void                            *handle,
                                        unsigned int                    VLAN_AppID,
                                        unsigned int                    VLAN_VID,
                                        unsigned char                   VLAN_Priority,
                                        unsigned char                   *privateSecurityData,
                                        int                             privateSecurityDataLen,
                                        TMW_61850_VLAN_MakeSignatureFcn makeVlanSignature,
                                        void                            *pCryptoContext,
                                        void                            *pVlanSignatureData );

/* Deletes a goose output stream */
/* -- Returns TRUE if deletion succeeded */
TMW_CLIB_API int IEC_Goose_Delete( IEC_Goose_Context *cntxt,         /* context            */
                      void *handle );                   /* user handle        */

/* Publishes new data and restarts the retransmission scheme */
/* -- Returns TRUE if publish succeeded */
TMW_CLIB_API int IEC_Goose_Publish( IEC_Goose_Context *cntxt,        /* context            */
                       void              *handle,       /* user handle        */
                       IEC_GooseHeader   *header,       /* Returns header     */
                       int               numDataSetEntries, /* objects in data*/
                       unsigned char     *buffer,       /* current data       */
                       int               length );      /* data length        */


IEC_Goose_Subscription *IEC_Goose_find_handle(void *handle, IEC_Goose_Subscription **pool);
IEC_Goose_Subscription *IEC_Goose_find_entry(TMW_Mac_Address *mac, char *goID, char *gcref, int appID, IEC_Goose_Subscription **pool);

/* ------------------------------------ */
/* User-callable DataSet Query Routines */
/* ------------------------------------ */

#if defined(TMW_USE_90_5)
IEC_Goose_Error IEC_Goose_CreateUDP( IEC_Goose_Context *cntxt,
                                     char *goID,
                                     char *gocbRef,
                                     char *dsRef,
                                     TMW_Mac_Address *mac,
                                     int port,
                                     IEC_Goose_Strategy *strategy,
                                     long confRev,
                                     unsigned char ndsCom,
                                     unsigned char test,
                                     unsigned char simulateBit,
                                     int numDataSetEntries,
                                     unsigned char bFixedOffs,
                                     unsigned char *buffer,
                                     MMSd_length length,
                                     void *handle,
                                     unsigned int VLAN_AppID );
#endif

MMSd_length GSE_Encode_Request(IEC_PrimitiveType idType, char *goID, char *dataSetName, long confRev, unsigned long stateId, int errCode, MMSd_length length, TMW_Packet *pPacket);

IEC_Goose_Error IEC_Goose_GetReference(IEC_Goose_Context *cntxt, void *handle, unsigned long stateId, int *pElements, int numElements);

IEC_Goose_Error IEC_Goose_GetElementNumber(IEC_Goose_Context *cntxt, void *handle, unsigned long stateId, char **Elnamelist);

/* UTC Time additions for IEC Goose */

#if defined(USE_MMSD_IN_IEC_GOOSE)
#ifdef _CASM_H
/* ***************************************************************** */
/*
   The IEC Goose Data Buffer can safely hold this much data.  If the encoded
   data set exceeds this, it is possible that the final encoded message (with
   header) could exceed the overall maximum size.  Note that this number
   reserves the following:
FIXME!! : check these, first two should now be 129?
        GoCbRef     65 bytes
        DatSet      65 bytes
        GoID        32 bytes
        ConfRev      4 bytes

    If you can guarantee that these are smaller than the max, you can add
    a little more to the data length.
*/
#define MAX_IEC_GOOSE_DATASET_BUFFER 1265

/* ***************************************************************** */
/* This should have been in CASM.C ! */
int MMSd_ObjectNameString( MMSd_ObjectName *pObj, char *outStr );

char * MMSd_GooseControlGetGcbName( void *handle );

void * MMSd_GooseControlGetHandle( MMSd_context *pMmsContext, char *GcbName );

void * MMSd_GooseControlGetContext( void *handle );

int MMSd_GooseControlSetContext( void *handle, /* IEC_Goose_Context */void *pContext );

void * MMSd_GooseControlGetStrategy( void *handle );

int MMSd_GooseControlSetStrategy( void *handle, /* IEC_Goose_Strategy */void *pStrategy );

TMW_CLIB_API int MMSd_GooseControlGetGoEna( void *handle );

TMW_CLIB_API int MMSd_GooseControlSetGoEna(void *handle, int newState);

TMW_CLIB_API int MMSd_GooseControlSetConfRev(void *handle, unsigned long confRev);

TMW_CLIB_API unsigned long MMSd_GooseControlGetConfRev(void *handle);

TMW_CLIB_API int MMSd_GooseControlGetDataSet(void *handle, char *DatSet);

TMW_CLIB_API int MMSd_GooseControlSetDataSet(void *handle, char *DatSet);

int MMSd_GooseControlGetLanPars( void *handle,
                                 TMW_Mac_Address *pAddress,
                                 unsigned int *VLAN_AppID,
                                 unsigned int *VLAN_VID,
                                 unsigned char *VLAN_Priority );

int MMSd_GooseControlSetLanPars( void *handle,
                                 TMW_Mac_Address *pAddress,
                                 unsigned int VLAN_AppID,
                                 unsigned int VLAN_VID,
                                 unsigned char VLAN_Priority );

int MMSd_GooseControlScanData( void *handle,
                               unsigned char *buffer,
                               int length );

int MMSd_GooseControlStart( MMSd_context *pMmsContext,
                            void *handle,
                            unsigned char *buffer,
                            int bufferSize );

int MMSd_GooseControlStop( void *handle );

int MMSd_GooseControlReport( void *handle,
                             unsigned char *buffer,
                             int length );

int MMSd_GooseControlReadDatSet( void *handle,
                                 unsigned char *buffer,
                                 int bufferSize,
                                 int *dataOffset,
                                 int *dataLength );

void MMSd_GooseControlSetAllContext( MMSd_context *pMmsContext,
                                     /* IEC_Goose_Context */void *pContext );

void MMSd_GooseControlStartAll( MMSd_context *pMmsContext, unsigned char *buffer, int bufferSize );

void MMSd_GooseControlStopAll( MMSd_context *pMmsContext );

void MMSd_GooseControlScanAllData( MMSd_context *pMmsContext, unsigned char *buffer, int length );

/* Retrieves the current value of the simulate bit for the Goose
 output stream referenced by (handle) */
int TMW_GooseControlGetSimulate( void *handle, unsigned char *pSimulateMode );

/* Sets Goose output stream referenced by (handle) to transmit
 with the simulate bit set to nSimulateMode.  */
TMW_CLIB_API int TMW_GooseControlSetSimulate(void *handle, unsigned char nSimulateMode);

/* Sets all Goose streams controlled by Control Blocks within the Logcal Device (pLdName)
 to transmit with the simulate bit set to nSimulateMode.  If pLdName is NULL, it will
 set ALL Goose output streams controlled by Control Blocks to transmit with the simulate
 bit set to nSimulateMode.  */
void MMSd_GooseControlSetSimulateLD( MMSd_context *pMmsContext, char *pLdName, unsigned char nSimulateMode );

void MMSd_GooseControlSetAllSimulate( MMSd_context *pMmsContext, unsigned char nSimulateMode );

int TMW_GooseControlGetTest( void *handle, unsigned char *pTestMode );

TMW_CLIB_API int TMW_GooseControlSetTest(void *handle, unsigned char nTestMode);

void MMSd_GooseControlSetTestLD( MMSd_context *pMmsContext, char *pLdName, unsigned char nTestMode );

void MMSd_GooseControlSetupTest( MMSd_context *pMmsContext );
void MMSd_GooseControlSetupSimulate( MMSd_context *pMmsContext );


#endif  /* _CASM_H */
#endif  /* USE_MMSD_IN_IEC_GOOSE */

#ifdef __cplusplus
};
#endif

#endif  /* _IECGOOSE_H */
