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

/*
 *
 *  This file contains
 *  Declarations for Stack Integration.
 *
 *  This file should not require modification.
 */

#ifndef _STACK_H
#define _STACK_H

#ifdef __cplusplus
extern "C" {
#endif

#define TMW_TP_MAX_PDU_SIZE (MMSD_MAX_PDU_SIZE + 50)

typedef enum {
    TMW_StackIdle,
    TMW_StackRunning,
    TMW_StackStopping
} TMW_StackState;

#define TMW_StackError_MemoryAllocation 0x0001
#define TMW_StackError_ServiceThreads   0x0002
#define TMW_StackError_Rfc1006Open      0x0004
#define TMW_StackError_Rfc1006Thread    0x0008
#define TMW_StackError_IecGooseOpen     0x0010
#define TMW_StackError_IecGooseThread   0x0020
#define TMW_StackError_ReportingInit    0x0040
#define TMW_StackError_ReportingThread  0x0080
#define TMW_StackError_UcaGooseOpen     0x0100
#define TMW_StackError_UcaGooseThread   0x0200
#define TMW_StackError_XmlOpen          0x0400
#define TMW_StackError_XmlThread        0x0800
#define TMW_StackError_SavOpen          0x1000
#define TMW_StackError_SavThread        0x2000

typedef struct TMW_MMS_ServerListEntry *TMW_MMS_ServerListEntryPtr;
typedef struct TMW_TP_ContextListEntry *TMW_TP_ContextListEntryPtr;

typedef struct TMW_MMS_ServerListEntry {
    TMW_TP_ContextListEntryPtr pTpContextListEntry;
    MMSd_Server *pMmsServer;
    TMW_MMS_ServerListEntryPtr pNext;
} TMW_MMS_ServerListEntry;

typedef struct TMW_TP_ContextListEntry {
    TMW_TP_Context *pTpContext;
    int nRefCount;
    TMW_Stack_Address nsap_called;
    TMW_Stack_Address t_sel_called;
    int port;
#if defined(TMW_USE_SECURITY)
    TMW_StackSecurityConfiguration_t securityConfiguration;
#endif
#if defined(TMW_USE_THREADS)
    TmwTargServiceThreadHandle_t ServiceThreadHandle;
#endif
    TMW_MMS_ServerListEntry *pMmsServerList;
    TMW_TP_ContextListEntryPtr pNext;
} TMW_TP_ContextListEntry;

TMW_CLIB_API void TMW_InitializeFullStackAddressing(void);

TMW_CLIB_API void TMW_TerminateFullStackAddressing(void);

TMW_CLIB_API TMW_TP_ContextListEntry *TMW_find_tp_context_list(TMW_TP_Context *pTpContext);

TMW_MMS_ServerListEntry *TMW_find_t_selector(
        TMW_TP_ContextListEntry *pTpContextListEntry, TMW_Address *pTSelector);

TMW_CLIB_API TMW_MMS_ServerListEntry *TMW_find_full_address_mms_context(
        MMSd_Stack_Addresses *pMmsStackAddresses,
        TMW_TP_ContextListEntry *pTpContextList);

TMW_CLIB_API void TMW_InitializeServerAddress(MMSd_Stack_Addresses *serverAddress);

typedef int TMW_StackError;
int TMW_start_full_address_tp_context(TMW_TP_ContextListEntry *pFullAddressTpContext, TMW_StackError *nErrorFlags);

typedef enum {
    TMW_ConfirmedErrorIndication,
    TMW_RejectIndication,
    TMW_AbortReponseIndication
} TMW_ClientIndicationType;

typedef void (*TMW_ClientIndicationMessageFunc)(void *pStackContext, const char *message, TMW_ClientIndicationType type);

typedef enum {
    TMW_TransportUserIndication,
    TMW_TransportAbortIndication,
    TMW_TransportConnectIndication,
    TMW_TransportDisconnectIndication,
    TMW_General,
    TMW_NewConnection,
    TMW_CloseMmsConnectionIndication,
} TMW_StackMessageType;

typedef void (*TMW_StackIndicationFunc)(void *pStackContext, void *mmsdConnection, TMW_StackMessageType type);

#define TMW_MAX_FILENAME_LEN 260

typedef struct TMW_StackContext {
    TMW_StackState nState;
    TMW_StackError nErrorFlags;
    TmwTargTraceMessageFunc messageFunc;
    TMW_StackIndicationFunc indicationFunc;
    TMW_StackIndicationFunc newConnectionIndicationFunc; /* Called when server (or combined client/server) receives connection */
    void *pNewConnectionIndicationData;
    TMW_TP_Context *pTpContext;
    MMSd_context *pMmsContext;
    MMSd_Server *pMmsServer;
    TMW_TP_ContextListEntry *pFullAddressTpContext;

    void *pIecGooseContext; /* Void for apps not including iecgoose.h */
    char *udpMulticastAdaptorIP; /* local IP address to select adaptor for udp multicast send */
    void *pXmlContext; /* Void for apps not including xml.h */
    void *pSavContext; /* Void for apps not including sav.h */
    MMSd_EventReportStatus *pEventReportStatus;
    int nEventReportStatus;
    int nPeer2PeerConnections;
    void *pClientContext; /* Void for apps not including cliapi.h */
    TMW_ClientIndicationMessageFunc clientIndicationFunc;
#if defined(TMW_USE_THREADS)
    TmwTargServiceThreadHandle_t ServiceThreadHandle;
    TmwTargThreadHandle_t XmlThreadHandle;
    TmwTargThreadHandle_t IecGooseThreadHandle;
    TmwTargThreadHandle_t DebugThreadHandle;
    TmwTargThreadHandle_t ReportThreadHandle;
    TmwTargThreadHandle_t SimulateThreadHandle;
    TmwTargThreadHandle_t ConfigThreadHandle;
    TmwTargThreadHandle_t ControlsThreadHandle;
    TmwTargSem_t bufferSemaphore;
#endif
    unsigned char inputBuffer[TMW_TP_MAX_PDU_SIZE];
    unsigned char outBuffer[TMW_TP_MAX_PDU_SIZE];
    int bHasServer;
    int bHasClient;
    int bIsSoap;
    void *pUserData;
    char fileRoot[TMW_MAX_FILENAME_LEN];
} TMW_StackContext;

/*
 * The TMW_startup_library function must be called before any other
 * part of the system is called.
 */

TMW_CLIB_API void TMW_startup_library(void);

/*
 * The TMW_shutdown_library function must be called after everything else
 * in the library is finished - just to clean things up.
 */

TMW_CLIB_API void TMW_shutdown_library(void);

/*
 * The TMW_wake_reporting_thread is used to generate reports from
 * the server using the passed MMSd_context.
 */
TMW_CLIB_API void TMW_wake_mms_reporting_thread(MMSd_context *pMmsContext);

/*
 * This routine initializes an existing mmsContext with defaults
 */
TMW_CLIB_API void TMW_InitializeDefaultMMSd(MMSd_context *mmsContext);

/*
 * This routine initializes an existing RFC1006_Config with defaults
 */
TMW_CLIB_API void TMW_InitializeDefaultRFC1006Config(RFC1006_Config *rfcConfig);

/*
 * Used to retrieve the default stack interface structure for an MMS server.
 * The default stack interface is required for calling SCL_RTG_config_all,
 * which initializes the MMS context of full address servers and requires
 * the server interface to build the MMS dictionaries.
 */
TMW_CLIB_API MMSd_ServerInterfaceTable *TMW_GetDefaultServerInterface(void);

/*
 * This routine can be included as the addressSearch function in the MMSd_ServerInterfaceTable.
 * It verifies all of the upper layer OSI addresses (P_Sel, etc.) against the server's address
 * parameters.  The function pointer in the MMSd_ServerInterfaceTable can also be replaced by
 * a custom routine which looks up the addresses in a list of MMS contexts, so that you can have
 * multiple MMS servers at the same IP address.
 *
 * The current one is a dummy that simply displays all of the calling addresses being used for
 * the incoming connection - in the future it will be extended to do the actual searching.
 */
TMW_CLIB_API int TMW_StackAddressSearch(MMSd_context *pMmsContext, MMSd_context **pFoundMmsContext);

/*
 * The TMW_new_stack_context routine starts up a server stack, including all support threads
 * for reporting and servicing the underlying layers.  The RFC1006_Config structure contains
 * the parameters for transport.  If the IecGooseAdapterName  is NULL,
 * those Goose components will not be used. udpMulticastAdaptorIP should be a text string of the IP address
 * of the adaptor to publish UDP multicast (90-5) frames on.
 *
 * The stack should be closed using TMW_delete_stack_context, below.
 *
 * Note that stacks started using this routine ignore all addresses other than the IP address.
 * This requires less field configuration, but does not allow for multiple servers to exist
 * on a single device without using multi-homing (multiple IP addresses).
 *
 * The routine TMW_new_full_address_stack_context (below) can be used to start servers which
 * recognize all seven layers of addressing, and can multiplex over a single IP address.
 */

#if !defined(TMW_USE_SECURITY)
TMW_CLIB_API TMW_StackContext *TMW_new_stack_context( MMSd_context *pMmsContext, RFC1006_Config *pRfcConfig,
													  char *pIecGooseAdapterName, char *udpMulticastAdaptorIP);
#else
TMW_CLIB_API TMW_StackContext *TMW_new_stack_context( MMSd_context *pMmsContext, RFC1006_Config *pRfcConfig,
													  char *pIecGooseAdapterName, char *udpMulticastAdaptorIP,
													  TMW_StackSecurityConfiguration_t *pSecurityConfiguration);
#endif



/*
 * The stack restricts access to only a portion of the device filesystem. The default root path,
 * MMSD_FILE_ROOT  is set in vendor.h. This function allows the root to be set at runtime.
 */
 /*
  *   NOTE WELL:  The files below this path are a potential security vulnerability. 
  */
TMW_CLIB_API void TMW_set_file_root(TMW_StackContext *pStackContext, char * root);


/*
 * TMW_delete_stack_context routine stops all underlying threads and cleans up all memory
 * allocated for the stack. No references to the stack may be issued after calling this routine.
 */
TMW_CLIB_API void TMW_delete_stack_context(TMW_StackContext *pStackContext);

/*
 * The TMW_new_full_address_stack_context routine starts up a server stack,
 * including all support threads for reporting and servicing the underlying layers.
 *
 * This routine differs from TMW_new_stack_context (above), in that the resulting
 * server will check all addresses (not just the IP address).  It allows multiplexing
 * of multiple servers over a single IP address.
 *
 * The MMSd_Server structure can be created directly from an SCL file using the
 * CSCL routine SCL_RTG_config_all.
 *
 * The RFC1006_Config structure contains the parameters for transport.
 * If the IecGooseAdapterName is NULL,
 * those Goose components will not be used. The stack should be closed using
 * TMW_delete_full_address_stack_context, below.
 */
#if !defined(TMW_USE_SECURITY)
TMW_CLIB_API TMW_StackContext *TMW_new_full_address_stack_context( MMSd_Server *pMmsServer, RFC1006_Config *pRfcConfig,
																   char *pIecGooseAdapterName, char *udpMulticastAdaptorIP);
#else
TMW_CLIB_API TMW_StackContext *TMW_new_full_address_stack_context( MMSd_Server *pMmsServer, RFC1006_Config *pRfcConfig,
																   char *pIecGooseAdapterName, char *udpMulticastAdaptorIP,
																   TMW_StackSecurityConfiguration_t *pSecurityConfiguration);
#endif

/*
 * TMW_delete_full_address_stack_context routine stops all underlying threads and cleans up all memory
 * allocated for a stack created with TMW_new_full_address_stack_context. No references to the stack
 * may be issued after calling this routine.
 */
TMW_CLIB_API void TMW_delete_full_address_stack_context(TMW_StackContext *pStackContext);

/*
 * This routine opens transport layer logging file for the stack context. If
 * the fileName is NULL, a file will be created using the IP address of the stack
 * context (xxx_xxx_xxx_xxx.log). The transport library will log all connects,
 * disconnects, and messages until the log file is closed (TMW_close_tp_log_file,
 * below).  The messages will contain the ASCII, which can be decoded and displayed
 * using an off-line utility.
 */
TMW_CLIB_API void TMW_open_tp_log_file(TMW_StackContext *pStackContext, char *fileName);

/*
 * Disables transport logging for a stack and closes the logging file.
 */
TMW_CLIB_API void TMW_close_tp_log_file(TMW_StackContext *pStackContext);

#if defined(TMW_USE_CLIENT) || defined(USE_TASE2_CLIENT)
#if defined(_CLIAPI_H)
/*
 * The TMW_new_client_stack_context routine starts up a client stack, including all suppport
 * threads for servicing the underlying layers.  The RFC1006_Config structure contains
 * the parameters for transport.  If the IecGooseAdapterName is NULL,
 * those Goose components will not be used. The stack should be closed using
 * TMW_delete_stack_context.
 */

#if !defined(TMW_USE_SECURITY)
TMW_CLIB_API TMW_StackContext *TMW_new_client_stack_context( MMSd_context *pMmsContext,
															 RFC1006_Config *pRfcConfig,
															 char *pIecGooseAdapterName,
															 char *udpMulticastAdaptorIP );
#else
TMW_CLIB_API TMW_StackContext *TMW_new_client_stack_context( MMSd_context *pMmsContext,
															 RFC1006_Config *pRfcConfig,
															 char *pIecGooseAdapterName,
															 char *udpMulticastAdaptorIP,
															 TMW_StackSecurityConfiguration_t *pSecurityConfiguration );
#endif

#if !defined(TMW_USE_SECURITY)
TMW_CLIB_API TMW_StackContext *TMW_new_full_address_client_stack_context( 	MMSd_Server *pMmsServer,
																			RFC1006_Config *pRfcConfig,
																			char *pIecGooseAdapterName,
																			char *udpMulticastAdaptorIP );
#else
TMW_CLIB_API TMW_StackContext *TMW_new_full_address_client_stack_context( MMSd_Server *pMmsServer,
																		  RFC1006_Config *pRfcConfig,
																		  char *pIecGooseAdapterName,
																		  char *udpMulticastAdaptorIP,
																		  TMW_StackSecurityConfiguration_t *pSecurityConfiguration );
#endif
/*
 * This routine initializes an existing MMSd_InitiatePars, TMW_UL_Server, TMW_LL_Server with defaults
 */

TMW_CLIB_API void TMW_InitializeDefaultMMSdInitiateParameters(MMSd_InitiatePars *initPars);

TMW_CLIB_API void TMW_InitializeUpperAddress(TMW_UL_Server *serverUpperAddress);

TMW_CLIB_API void TMW_InitializeLowerAddress(TMW_LL_Server *serverLoweverAddress);

/*
 * This routine creates and initializes new remote server connection
 */
TMW_CLIB_API MMSd_RemoteServer *TMW_create_remote_server( TMW_StackContext *pStackContext, 
														  MMSd_InitiatePars *initPars,
														  TMW_UL_Server *serverUpperAddress, 
														  TMW_LL_Server *serverLowerAddress );

/*
 * This routine may be called from the newConnectionIndicationFunc stack context callback function.
 * It is used to allocate a client-type remote server so that the server can make peer-to-peer
 * service requests on the connection (bi-directional connections).  Note that by default the
 * returned MMSd_RemoteServer has been initialized with callbacks that will automatically free
 * if up on concludes and loss of connections - this may be dangerous if the application is
 * making use
 */
TMW_CLIB_API MMSd_RemoteServer *TMW_MakePeer2PeerConnection( MMSd_Connection *pMmsConnection, TMW_StackContext *pStackContext );

TMW_CLIB_API void TMW_RemovePeer2PeerConnection( MMSd_RemoteServer *pRemoteServer, TMW_StackContext *pStackContext );

#endif
#endif

#if defined(USE_XML)
#if defined(_XMLAPI_H)
int TMW_add_xml_context( TMW_StackContext *pStackContext, MMSd_XML_Config *pXmlPassedConfig );
#endif
#endif

#if defined(TMW_USE_SAV)
#if defined(_SAV_H)
TMW_CLIB_API int TMW_add_sav_context( TMW_StackContext *pStackContext, char *pSavAdapterName, TMW_SAV_Start start_call_back_data,
                         void *start_call_back, TMW_SAV_Stop stop_call_back, void *stop_call_back_data );

TMW_CLIB_API int TMW_remove_sav_context( TMW_StackContext *pStackContext );

#define TMW_getSavContext( pStackContext )  \
            ((TMW_SAV_Context *) ( pStackContext == NULL )  \
                                 ? NULL                     \
                                 : (pStackContext)->pSavContext )

#endif

#endif

#if !defined(TMW_USE_THREADS)
/*
 * This routine is only available when the library is built with no threading.
 * It should be called by the user application code periodically.  It handles
 * the TCP/IP sockets, timeouts, and drives the reporting process.  If it is
 * not called often enough, you will not only get very slow response times,
 * but may also get buffer overruns at reporting.
 */

int TMW_stack_service(TMW_StackContext *pStackContext);

#endif

TMW_CLIB_API unsigned long TMW_TempIecGooseService( void *param );

#ifdef __cplusplus
};
#endif

#endif /* _STACK_H */
