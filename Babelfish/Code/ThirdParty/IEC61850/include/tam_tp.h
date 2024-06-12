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
 *  Declarations for Transport Layer Interface.
 *
 *  This file should not require modification.
 */

#ifndef _TMW_TP_H
#define _TMW_TP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tam.h"

#define DR_REASONS 1
/* #define TMW_TP_USE_DEF_INDICATIONS */

typedef enum {
    TMW_Thread_Idle,
    TMW_Thread_Running,
    TMW_Thread_Stopping,
    TMW_Thread_Stopped,
    TMW_Thread_Quitting
} TMW_ThreadState;

typedef enum {
    EventTask_Idle,
    EventTask_Running,
    EventTask_Stopping,
    EventTask_Stopped
} EventTaskState;

typedef struct TMW_TP_Connection *TMW_TP_ConnectionPtr;
typedef struct TMW_TP_Context *TMW_TP_ContextPtr;
typedef struct TMW_TP_Config *TMW_TP_ConfigPtr;
typedef struct TMW_TP_Interface *TMW_TP_InterfacePtr;

typedef unsigned long (*TMW_DL_User_Func)(void *arg);

typedef struct TMW_TP_Config {
    TMW_Address local_tsap;
    TMW_Address local_nsap;
    TMW_Mac_Address local_mac;
    unsigned int max_tsdu_size;
    TMW_Media media;
    TamFifoIndex input_queue_len;
    TamFifoIndex output_queue_len;
    void *link_dependant;
} TMW_TP_Config;


/* NOTE WELL - These must stay in correspondence with their counterparts in RFC1006.H */
typedef int TMW_TP_ConnState;
#define TP_Idle         0
#define TP_WaitingTP0   1
#define TP_Connected    2
#define TP_Disconnected 3
#define TP_Failed       4

typedef unsigned int TMW_TP_RefNum;
typedef int TMW_TP_DR_Class;

/*
 ******************************************************************************
 */
typedef struct TMW_TP_Queue { /* Used for I/O, per connection */
    TamFifo fifo; /* Circular FIFO for index mgmt */
    TamQueue queue; /* Maintains list of queue entries */
    int eots; /* Count of completed msgs in queue */
} TMW_TP_Queue;


/*
 ******************************************************************************
 */
typedef struct TMW_TP_QueueEntry *TMW_TP_QueueEntryPtr;
typedef struct TMW_TP_QueueEntry {
    TamQueueEntry queue_header; /* Used by TMW_Queue routines */
    TMW_TP_QueueEntryPtr next; /* Used for other linked lists */
    TMW_Packet packet; /* Packed being queued */
    unsigned long seq_num; /* Seq number of this entry */
    int num_transmissions; /* For retrans processing */
    char eot; /* TRUE->last segment of msgs */
} TMW_TP_QueueEntry;

/*
 ******************************************************************************
 */
typedef struct TMW_TP_QueuePoolEntry {
    TamPoolEntry pool_data; /* Used by Tamarack pool fcns */
    TMW_TP_QueueEntry queue_data; /* Data allocated from pool */
} TMW_TP_QueuePoolEntry;

/*
 ******************************************************************************
 */
typedef struct TMW_TP_Connection {
    TMW_Address local_nsap;
    TMW_Address local_tsap;
    TMW_Mac_Address remote_mac;
    TMW_Address remote_nsap;
    TMW_Address remote_tsap;
    TMW_TP_ConnState state; /* State of the connection */
    int data_available; /* count of full msgs queued */
    TMW_TP_Queue input_queue;
    TMW_TP_Queue output_queue;
    unsigned int neg_tpdu_size; /* Negotiated by TP0          */
    TMW_TP_RefNum lclRefNum; /* Our TP0 refrence number    */
    TMW_TP_RefNum rmtRefNum; /* Their TP0 reference number */
    void *userData; /* Used by user of TMW_TP    */
    TMW_TP_ConnectionPtr next; /* Used for linked lists */
    TMW_TP_ConfigPtr config; /* Pointer to overall config */
    TMW_TP_ContextPtr cntxt; /* Pointer to overall context */
} TMW_TP_Connection;

/*
 ******************************************************************************
 */
typedef struct TMW_TP_ConnPoolEntry {
    TamPoolEntry pool_data;
    TMW_TP_Connection conn_data;
} TMW_TP_ConnPoolEntry;


/*
 * These prototypes define the standardized entry points into
 * an arbitrary transport layer.  These are used to hide the
 * transport (RFC1006) from the user code.
 ******************************************************************************
 */
typedef int (*TMW_TP_Configure)(char *name, TMW_TP_Config *config);

typedef void (*TMW_TP_Initialize_Context)(TMW_TP_ContextPtr cntxt, TMW_TP_Config *config);

typedef void (*TMW_TP_Add_Connection)(TMW_TP_ConnPoolEntry *conn, TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Open)(TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Close)(TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Service)(TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Receive)(unsigned char *buffer, unsigned int length, TMW_TP_Connection *conn, TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Send)(unsigned char *buffer, unsigned int len, TMW_TP_Connection *conn, TMW_TP_ContextPtr cntxt);

typedef int (*TMW_TP_Can_Send)(unsigned int length, TMW_TP_Connection *conn, TMW_TP_ContextPtr context);

typedef void (*TMW_TP_User_Indication)(TMW_TP_Connection *conn, TMW_TP_ContextPtr context);

typedef int (*TMW_TP_Connect_Indication)(TMW_TP_Connection *conn, TMW_TP_ContextPtr context);

typedef void (*TMW_TP_Disconnect_Indication)(TMW_TP_Connection *conn, TMW_TP_ContextPtr context, TMW_TP_DR_Class dr_class, int dr_reason);

typedef void (*TMW_TP_Abort_Indication)(TMW_TP_Connection *conn, TMW_TP_ContextPtr context);

typedef TMW_TP_Connection * (*TMW_TP_Connect)(TMW_Address *called_nsap, TMW_Address *called_tsap, TMW_Address *calling_tsap, int port, int secure_connect, TMW_TP_ContextPtr context);

typedef int (*TMW_TP_Disconnect)(TMW_TP_Connection *conn, TMW_TP_ContextPtr context);

/*
 ******************************************************************************
 */
typedef struct TMW_TP_Interface {
    TMW_TP_ContextPtr cntxt;
    TMW_TP_Config *config;
    TMW_TP_InterfacePtr next;
    TamFifoIndex seq_num_range;
    TamFifoIndex default_queue_len;
    TamQueuePolicy queue_policy;
    TMW_TP_Configure TamConfigure;
    TMW_TP_Initialize_Context TamInitialize_context;
    TMW_TP_Add_Connection TamAdd_connection;
    TMW_TP_Open TamOpen;
    TMW_TP_Close TamClose;
    TMW_TP_Service TamService;
    TMW_TP_Receive TamReceive;
    TMW_TP_Send TamSend;
    TMW_TP_Can_Send TamCan_send;
    TMW_TP_User_Indication TamUser_indication;
    TMW_TP_Connect_Indication TamConnect_indication;
    TMW_TP_Disconnect_Indication TamDisconnect_indication;
    TMW_TP_Abort_Indication TamAbort_indication;
    TMW_TP_Connect TamConnect;
    TMW_TP_Disconnect TamDisconnect;
} TMW_TP_Interface;


typedef int (*TMW_TP_EnrollConnect)(TMW_TP_Connection *connect,
                                    void *handle);
/*
 ******************************************************************************
 */
typedef struct TMW_TP_Context {
    TMW_TP_ContextPtr next; /* Chains protocol contexts */
    TMW_TP_Interface tp_def;
    TMW_TP_Connection *conn_list; /* List of active connections */
    TMW_Address local_tsap;
    TMW_Address local_nsap;
    unsigned int max_tsdu_size;
    TamPool connections; /* Pooled TMW_TP_Connections */
    TamPool queue_entries; /* Pooled TMW_Queue_Entries */
    TamPool *buffers; /* Pooled buffers */
#if defined(USER_DEFAULT_TRANSPORT_LOGGER)
    void *logFile;
#endif

    TmwTargSem_t sem;

    TMW_TARG_EVENT eve;

    void *userData; /* Used by user of TMW_TP */
#if defined(TMW_TP_USE_TASKS)
    TMW_TP_EnrollConnect enrollConnect;
    void *enrollHandle;
#endif
} TMW_TP_Context;


typedef struct TMW_DL_Reference *TMW_DL_ReferencePtr;
typedef struct Rfc1006EventTaskParams *Rfc1006EventTaskParamsPtr;
typedef struct TMW_EventTaskParams *TMW_EventTaskParamsPtr;
typedef struct TMW_DL_Enrollment *TMW_DL_EnrollmentPtr;
typedef struct DlEventTaskParams *DlEventTaskParamsPtr;

/*
 ******************************************************************************
 */
typedef struct TMW_DL_Reference {
    TMW_DL_ReferencePtr Next;
    TMW_DL_Context *dlContext;
    int RefCount;
} TMW_DL_Reference;

/*
 ******************************************************************************
 */
typedef struct Rfc1006EventTaskParams {
    EventTaskState State;
    unsigned long Timeout;
    void *TpContext;
    Rfc1006EventTaskParamsPtr Next;
    TMW_EventTaskParamsPtr GlobParams;
    TMW_TARG_EVENT WakeUpEvent;
    int NumNetworkEvents;
    void *NetworkConnections[TMW_TARG_MAX_CONN];
    int   NetworkConnectionsDeleted[TMW_TARG_MAX_CONN];
    TmwTargSem_t RfcSemaphore;
    TMW_TARG_RFC1006_EVENT_PARAMS
} Rfc1006EventTaskParams;


/*
 ******************************************************************************
 */
typedef struct TMW_EventTaskParams {
    EventTaskState State;
    TmwTargSem_t EtSemaphore;
    unsigned long Timeout;
    TMW_TARG_EVENT WakeUpEvent;
    TMW_DL_Reference *dlReference;
    Rfc1006EventTaskParams *rfc1006Params;
    TmwThreadId_t ThreadId;
#if defined(TMW_USE_CLIENT)
    void *clientContext;
#endif
} TMW_EventTaskParams;

/*
 ******************************************************************************
 */
typedef struct TMW_DL_Enrollment {
    TMW_TARG_EVENT WakeUpEvent;
    TmwTargSem_t DlEnSemaphore;
    TMW_DL_User_Func Func;
    void *FuncArg;
    EventTaskState State;
    unsigned long Timeout;
    TmwThreadId_t ThreadId;
    TMW_EventTaskParamsPtr globParams;
    TMW_DL_EnrollmentPtr Next;
} TMW_DL_Enrollment;

/*
 ******************************************************************************
 */
typedef struct DlEventTaskParams {
    EventTaskState State;
    unsigned long Timeout;
    void *DlContext;
    DlEventTaskParamsPtr Next;
    TMW_EventTaskParamsPtr GlobParams;
    TMW_TARG_EVENT WakeUpEvent;
    TmwTargSem_t DlSemaphore;
    TMW_DL_Enrollment *Enrolled;
    TMW_TARG_DL_EVENT_PARAMS
} DlEventTaskParams;

/*
 ******************************************************************************
 */
typedef struct TMW_ThreadParams {
    TMW_ThreadState State;
    TMW_TARG_EVENT WakeUpEvent;
    unsigned long InitialTimeout;
    unsigned long Timeout;
#if defined(TMW_USE_THREADS)
    TmwTargTaskFunction_t ThreadFunction;
#endif
    void *ThreadFunctionParams;
    TmwThreadId_t ThreadId;
} TMW_ThreadParams;


#if defined(USER_DEFAULT_TRANSPORT_LOGGER)
#define TMW_LOG_CONNECT     0
#define TMW_LOG_SEND        1
#define TMW_LOG_RECEIVE     2
#define TMW_LOG_DISCONNECT  3
#define TMW_LOG_ABORT       4
#endif

int TMW_TP_configure(TMW_TP_Context *cntxt, char *name, TMW_TP_Config *config);
void TMW_TP_initialize_context(TMW_TP_Context *cntxt, TMW_TP_Config *config);

void TMW_TP_cleanup_context(TMW_TP_Context *cntxt);

void TMW_TP_initialize_connection(TMW_TP_Connection *conn,
                                  TMW_TP_Context *cntxt);

void TMW_TP_add_connection(TMW_TP_ConnPoolEntry *conn, TMW_TP_Context *cntxt);

int TMW_TP_open(TMW_TP_Context *cntxt);

int TMW_TP_close(TMW_TP_Context *cntxt);

int TMW_TP_service(TMW_TP_Context *cntxt);

int TMW_TP_receive(unsigned char *buffer, unsigned int length,
                   TMW_TP_Connection *conn, TMW_TP_Context *cntxt);

int TMW_TP_send(unsigned char *buffer, unsigned int len,
                TMW_TP_Connection *conn, TMW_TP_Context *cntxt);

int TMW_TP_can_send(unsigned int length, TMW_TP_Connection *conn,
                    TMW_TP_Context *context);

void TMW_TP_I_user_indication(TMW_TP_Connection *conn, TMW_TP_Context *context);

int TMW_TP_I_connect_indication(TMW_TP_Connection *conn,
                                TMW_TP_Context *context);

void TMW_TP_I_disconnect_indication(TMW_TP_Connection *conn,
                                    TMW_TP_Context *context,
                                    TMW_TP_DR_Class dr_class, int dr_reason);

void TMW_TP_I_abort_indication(TMW_TP_Connection *conn, TMW_TP_Context *context);


/* User space indication routines
 ******************************************************************************
 */
void TMW_TP_user_indication(TMW_TP_Connection *conn, TMW_TP_Context *context);

int TMW_TP_connect_indication(TMW_TP_Connection *conn, TMW_TP_Context *context);

void TMW_TP_disconnect_indication(TMW_TP_Connection *conn,
                                  TMW_TP_Context *context,
                                  TMW_TP_DR_Class dr_class, int dr_reason);

void TMW_TP_abort_indication(TMW_TP_Connection *conn, TMW_TP_Context *context);

TMW_TP_Connection *TMW_TP_connect(TMW_Address *called_nsap,
                                  TMW_Address *called_tsap,
                                  TMW_Address *calling_tsap, int port,
                                  int secure_connect, TMW_TP_Context *cntxt);

int TMW_TP_disconnect(TMW_TP_Connection *conn, TMW_TP_Context *cntxt);

void MMSd_user_transport_logger(unsigned char *buffer, unsigned int length,
                                void *passedConn, void *passedCntxt,
                                int indType);

#ifdef __cplusplus
}
;
#endif

#endif  /* TMW_TP_H */
