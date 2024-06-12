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
 *  User Visible definitions for RFC1006 Interface
 *
 *  This file should not require modification.
 */
/* #define ROUND_ROBIN 1 */

#ifndef _RFC1006_H
#define _RFC1006_H

#ifdef __cplusplus
extern "C" {
#endif


#include "tmwtarg.h"
#if defined(TMW_USE_SECURITY)
#include "cryptovend.h"
#endif

/* There are three styles of buffer allocation supported:

 RFC1006_STATIC_ALLOCATION   - same as old style, can still be overridded with RFC1006_ENTRY_POOL_DEFS, etc.
 RFC1006_DYNAMIC_ALLOCATION  - boot time malloc, so that multiple transport contexts can be opened
 RFC1006_UNBOUND_ALLOCATION  - run-time mallocs, with no limits (used for workstation type applications)

 */

/* No more than one can be defined in the package */
/* This should generate an error unless at most one is defined */
#if defined(RFC1006_STATIC_ALLOCATION)
#define RFC_ALLOCATION 1
#endif
#if defined(RFC1006_DYNAMIC_ALLOCATION)
#define RFC_ALLOCATION 2
#endif
#if defined(RFC1006_UNBOUND_ALLOCATION)
#define RFC_ALLOCATION 3
#endif

/* If none of them defined, assume old-style static or overrides */
#if !defined(RFC_ALLOCATION) && !defined(RFC1006_CUSTOM_ALLOCATION)
#define RFC1006_STATIC_ALLOCATION
#endif

#define CFG_RFC1006_SEGMENT_SIZE        cntxt->gnrc.tp_def.config->buff_segment_size
#define CFG_RFC1006_MAX_BUFFERS         cntxt->gnrc.tp_def.config->num_buffers
#define CFG_RFC1006_MAX_CONNECTIONS     cntxt->gnrc.tp_def.config->num_connections
#define CFG_RFC1006_MAX_ENTRIES         cntxt->gnrc.tp_def.config->num_queue_pool_entries
#define CFG_RFC1006_MAX_INPUT_QUEUE     cntxt->gnrc.tp_def.config->max_input_queue_len
#define CFG_RFC1006_MAX_OUTPUT_QUEUE    cntxt->gnrc.tp_def.config->max_input_queue_len

typedef TmwSocket_t RFC1006_Socket;

typedef struct {
    unsigned char sockdata[20]; /* Used to hold sockaddr struct */
} RFC1006_SocketAddr;

typedef enum {
    DoingNothing,
    DoingLength,
    DoingTp0Length,
    DoingTp0,
    DoingBodyStart,
    DoingBody,
    DoingDiscard
} RFC1006_ParseState;

/* ****************************************************************************************************************** *
 NOTES on state transitions in RFC1006

 The first five states correspond to the active states in the TMW_TP model, and should remain in the same order.  The
 rest of the states are specific to RFC1006. There are two ways of getting started: socket connect() and socket accept().

 A successful socket connect() will cause the state to transition to Waiting_WBOC if the connection is not secured,
 and to Waiting_TLS_Con if it is secured.

 A successful socket accept() will cause the state to transition to WaitingTP0 if the connection is not secured,
 and to Waiting_TLS_Acc if it is secured.

 Idle                - Connection has been allocated, but is inactive.
 WaitingTP0          - Entered when connection accepted (if not secured), expecting CR from peer.  Goes to Connected when CR-CC done.
 Connected           - Completed CR-CC exchange, ready for data.  On receive if DR, goes to ??, on tcp disconnect goes to ??
 Disconnected        - Completed DR-DC or abort, should go to Failed for cleanup (if not secured) or Waiting_TLS_Shut (if secured).
 Failed              - Ready for cleanup, socked should be closed and connection should be freed.
 Waiting_WBOC        - Entered from connect (if no TLS, else Waiting_TLS_Con), triggers sending of CR.  Goes to Waiting_WFCC on success.
 Waiting_WFCC        - Entered when CR has been sent, waits for CC from peer.  Goes to Connected on success.
 Waiting_TLS_Con     - Entered from connect if (if secured), does TlsConnect and goes to Waiting_WBOC on success.
 Waiting_TLS_Acc     - Entered from accept (if secured), does TlsAccept and goes to WaitingTP0 on success.
 Waiting_TLS_Shut    - Entered when DR-DC exchange is complete, does TlsShutdown and goes to Failed on success.
 Sending_DR          - Entered when application disconnects. Sends DR, goes to Waiting_WFDC (success), or Waiting_TLS_Shut (timeout).
 Waiting_WFDC        - Entered from Sending_DR, waits for DC and goes to Disconnected on success or timeout.

 State transitions and actions:

 State                 Entered from             Action    Success              Failure, Timeout    Indications
 Idle                - allocation               - none    - none               - none               - none
 WaitingTP0          - accept() or TLS_Acc      - wait CR - Send_CC            - Sending_DR         - none
 Sending_CC          - WaitingTP0               - send CC - Connected          - Sending_DR         - connect_ind (success)
 Connected           - WFCC or Sending_CC       - running - none               - none               - data_ind
 Disconnected        - Failed (on cleanup)      - none    - free               - free               - none
 Failed              - Disconnected, TLS_Shut   - cleanup - Disconnected       - none               - none
 Waiting_WBOC        - connect() or TlsCon      - send CR - WFCC               - Failed             - none (connect never indicated)
 Waiting_WFCC        - WBOC                     - wait CC - Connected          - Sending_DR         - connect_ind (success)
 Waiting_TLS_Con     - connect()                - TlsCon  - WBOC               - TLS_Shut           - none
 Waiting_TLS_Acc     - accept()                 - TlsAcc  - WaitingTP0         - TLS_Shut           - none
 Waiting_TLS_Shut    - WFDC                     - TlsShut - Failed             - Failed             - disconnect_ind (success or fail)
 Sending_DR          - application disconnect   - send DR - WFDC               - Failed or TLS_Shut - disconnect_ind (if not TLS_Shut)

 TP0 input processing:

 DR  - Failed: ignore, TLS_Shut: ignore
 CR  - WaitingTP0: Send_CC, otherwise Sending_DR
 CC  - Waiting_WFC: go to Connected, otherwise go to Sending_DR
 DT  - If not Connected: go to Sending_DR
 ED  - If not Connected: go to Sending_DR

 User     disconnects and aborts - if Disconnected, Failed, Sending_DC or TLS_Shut: ignore, else go to Sending_DR
 Providor disconnects and aborts - if Disconnected, Failed, Sending_DC or TLS_Shut: ignore, else go to Sending_DR

 * ****************************************************************************************************************** */

typedef enum {
    Idle,
    WaitingTP0,
    Connected,
    Disconnected,
    Failed,
    Waiting_WBOC,
    Waiting_WFCC,
    Waiting_TLS_Con,
    Waiting_TLS_Acc,
    Waiting_TLS_Shut,
    Sending_CC,
    Sending_DR,
    Abort
} RFC1006_ConnState;

typedef enum {
    Indication_none,
    Indication_disconnect,
    Indication_abort
} RFC1006_IndicationType;

typedef unsigned int RFC1006_RefNum;
typedef int RFC1006_DR_Class;

/* Buffer pool definition
 * A pool of these structures is allocated and linked via TMW_insert_pool()
 * TMW_alloc_pool() and TMW_free_pool() are then used to allocate/free
 * entries from the pool (they 'hide' the header, operating on just the
 * buffer area).
 * ************************************************************************* */
typedef struct RFC1006_BuffPoolEntry {
    TamPoolEntry header;
    unsigned char buffer[RFC1006_SEGMENT_SIZE];
} RFC1006_BuffPoolEntry;

/* Queue pool definition
 *  A pool of RFC1006_QueuePoolEntry structures is allocated and linked via
 *  TMW_insert_pool().  The pool routines then 'hide' the pool_info, and the
 *  rest of the code can operate on just the RFC1006_QueueEntry structures.
 *  Likewise, the queue_header data is used by the TMW Queue package to
 *  maintain the entries within the queues.  The remainder of the QueueEntry
 *  structure is the actual data being queued.
 *  They are used to chain buffers (via the TMW_Packet structures) for the
 *  input and output queues per connection.
 * ************************************************************************* */

typedef struct RFC1006_QueueEntry *RFC1006_QueueEntryPtr;
typedef struct RFC1006_QueueEntry {
    TamQueueEntry queue_header; /* Used by TMW_Queue routines */
    RFC1006_QueueEntryPtr next; /* Used for other linked lists */
    TMW_Packet packet; /* Packed being queued */
    unsigned long seq_num; /* Seq number of thsi entry */
    int num_transmissions; /* For retrans processing */
    char eot; /* TRUE->last segment of msgs */
    unsigned char partial; /* TRUE->part has been sent */
} RFC1006_QueueEntry;

typedef struct RFC1006_QueuePoolEntry {
    TamPoolEntry pool_data; /* Used by TMW pool fcns */
    RFC1006_QueueEntry queue_data; /* Data allocated from pool */
} RFC1006_QueuePoolEntry;

typedef struct RFC1006_Queue { /* Used for I/O, per connection */
    TamFifo fifo; /* Circular FIFO for index mgmt */
    TamQueue queue; /* Maintains list of queue entries */
    int eots; /* Count of completed msgs in queue */
} RFC1006_Queue;

/* Connection pool definition
 *  A pool of RFC1006_ConnPoolEntry structures is allocated and linked via
 *  TMW_insert_pool().  The pool routines then 'hide' the pool_info, and the
 *  rest of the code can operate on just the RFC1006_Connection structures.
 *  They are used to maintain the run-time information for each connection.
 * ************************************************************************* */

typedef struct RFC1006_Connection *RFC1006_ConnectionPtr;
typedef struct RFC1006_Context *RFC1006_ContextPtr;
typedef struct RFC1006_Config *RFC1006_ConfigPtr;

/* These are organized into a pool of RFC1006_ConnPoolEntry records */
typedef struct RFC1006_Connection {
    TMW_TP_Connection gnrc;
    RFC1006_Socket data_s; /* Socket for this connection */
    unsigned int neg_tsdu_size; /* TP0 max size               */
    RFC1006_ParseState input_state; /* Used in assembling RFC1006 */
    int input_msg_size; /* Used in parsing RFC1006    */
    int input_tp0_size; /* Used in parsing TP0        */
    int remaining_to_be_read; /* Used in parsing TP0    */
    int parsing_eot; /* Used in parsing TP0        */
    int deferred_disconnect;/* If disc when flow contrled */
    unsigned char header[50]; /* For holding RFC header     */
    TMW_ms_time timer; /* Timing out states          */
    RFC1006_QueueEntry *current_entry;
    RFC1006_IndicationType indicationType; /* when connection is closed  */
    int discard_tpdu; /* length violations          */
#if defined(TMW_USE_SECURITY)
    tmwTlsState_SECURITY_DATA securityData;
#endif
} RFC1006_Connection;

typedef struct RFC1006_ConnPoolEntry {
    TamPoolEntry pool_data; /* Used by Tamarack gnrc pool fcns */
    RFC1006_Connection conn_data;
} RFC1006_ConnPoolEntry;

/* RFC1006 Context
 *  Overall context of RFC1006 (global information).
 * ************************************************************************* */
typedef struct RFC1006_Context {
    TMW_TP_Context gnrc;
    int opened; /* TRUE if active context */
    RFC1006_RefNum refNum; /* Next ref numbr to allocate */
    RFC1006_Socket listen_s; /* TCP Listen sock (port 102) */
    RFC1006_SocketAddr listen_sockaddr; /* TCP Listen address */
    RFC1006_ConfigPtr config;
#if defined(ROUND_ROBIN)
    RFC1006_Connection *roundRobinWrite;
    RFC1006_Connection *roundRobinRead;
#endif
#if defined(TMW_USE_SECURITY)
    tmwTlsState_SECURITY_CONTEXT securityContext;
#endif
    Rfc1006EventTaskParams eventTaskParams;
} RFC1006_Context;

/* RFC1006 Configuration
 *  All configurable parameters for RFC1006 package
 * ************************************************************************* */
typedef struct RFC1006_Config {
    TMW_TP_Config gnrc;
    int buff_segment_size;
    int num_buffers;
    int num_connections;
    int num_queue_pool_entries;
    int use_listen;
    char adapter_name[129];
    int listen_port; /* If zero, use default 102 for unsecured, otherwise 3782 */
#if defined(SIO_KEEPALIVE_VALS)
    int keepaliveTime;
    int keepaliveInterval;
#endif
} RFC1006_Config;

typedef struct RFC1006_Queue_Entry {
    int dummy;
} RFC1006_Queue_Entry;

void RFC1006_initialize_context(RFC1006_Context *cntxt, RFC1006_Config *config);
void RFC1006_add_connection(RFC1006_ConnPoolEntry *conn, RFC1006_Context *cntxt);
void RFC1006_abort_all_connections(RFC1006_Context *cntxt);
int RFC1006_open(RFC1006_Context *cntxt);
int RFC1006_close(RFC1006_Context *cntxt);
int RFC1006_service(RFC1006_Context *cntxt);
int RFC1006_service_short(RFC1006_Context *cntxt);
int RFC1006_receive(char *buffer, int length, RFC1006_Connection *conn, RFC1006_Context *cntxt);
int RFC1006_send(unsigned char *buffer, int len, RFC1006_Connection *conn, RFC1006_Context *cntxt);
int RFC1006_can_send(int length, RFC1006_Connection *conn, RFC1006_Context *context);

void RFC1006_user_indication(RFC1006_Connection *conn, RFC1006_Context *context);
void RFC1006_connect_indication(RFC1006_Connection *conn, RFC1006_Context *context);
#if defined(DR_REASONS)
void RFC1006_disconnect_indication(RFC1006_Connection *conn, RFC1006_Context *context, RFC1006_DR_Class dr_class, int dr_reason);
#else
void RFC1006_disconnect_indication(RFC1006_Connection *conn, RFC1006_Context *context);
#endif
void RFC1006_abort_indication(RFC1006_Connection *conn, RFC1006_Context *context);
void RFC1006_disconnect_connection(RFC1006_Connection *conn, RFC1006_Connection **prev, RFC1006_Context *cntxt);
TMW_CLIB_API int RFC1006_configure(char *name, RFC1006_Config *config);
RFC1006_Connection *RFC1006_connect(TMW_Address *called_nsap, TMW_Address *called_tsap, TMW_Address *calling_tsap, int port, int secure_connect, RFC1006_Context *cntxt);
int RFC1006_disconnect(RFC1006_Connection *conn, RFC1006_Context *cntxt);
int RFC1006_process_input(RFC1006_Connection *conn);
int RFC1006_process_output(RFC1006_Connection *conn);
RFC1006_Connection *RFC1006_process_connection(RFC1006_Context *cntxt);
int RFC1006_check_for_failures(RFC1006_Context *cntxt);
int RFC1006_hasDataToSend(RFC1006_Connection *pConnection);
void RFC1006_abort_connection(RFC1006_Connection *conn, RFC1006_Connection **prev, RFC1006_Context *cntxt);

int MMSd_startup_transport_library(void);
void MMSd_shutdown_transport_library(void);

/* This table defines the operation of the RFC1006 protocol within
 the TMW gnrc Transport scheme.  The only thing that
 should ever be changed by users is the links to context, config,
 and the next protocol (when chaining). */

#define RFC1006_INTERFACE(d, cntxt, cfg) \
const TMW_TP_Interface d = { \
    /* Linkages for this protocol */ \
    (TMW_TP_Context *)      cntxt,                  /* protocol context */ \
    (TMW_TP_Config *)       cfg,                    /* protocol config */ \
    (TMW_TP_Interface *)    NULL,                   /* next protocol */ \
 \
    /* I/O Queue definitions for this protocol */ \
    (TamFifoIndex)          32767,                  /* seq_num_range */ \
    (TamFifoIndex)          RFC1006_MAX_QUEUE_LEN,  /* default_queue_len */ \
    (TamQueuePolicy)        TMW_QueueTime,          /* queue_policy */ \
 \
    /* Interface routines for this protocol */ \
    (TMW_TP_Configure)              &RFC1006_configure, \
    (TMW_TP_Initialize_Context)     &RFC1006_initialize_context, \
    (TMW_TP_Add_Connection)         &RFC1006_add_connection, \
    (TMW_TP_Open)                   &RFC1006_open, \
    (TMW_TP_Close)                  &RFC1006_close, \
    (TMW_TP_Service)                &RFC1006_service, \
    (TMW_TP_Receive)                &RFC1006_receive, \
    (TMW_TP_Send)                   &RFC1006_send, \
    (TMW_TP_Can_Send)               &RFC1006_can_send, \
    (TMW_TP_User_Indication)        &TMW_TP_user_indication, \
    (TMW_TP_Connect_Indication)     &TMW_TP_connect_indication, \
    (TMW_TP_Disconnect_Indication)  &TMW_TP_disconnect_indication, \
    (TMW_TP_Abort_Indication)       &TMW_TP_abort_indication, \
    (TMW_TP_Connect)                &RFC1006_connect, \
    (TMW_TP_Disconnect)             &RFC1006_disconnect \
}

#define RFC1006_INTERFACE_NOCFG(d, cntxt, cfg) \
const TMW_TP_Interface d = { \
    /* Linkages for this protocol */ \
    (TMW_TP_Context *)      cntxt,                  /* protocol context */ \
    (TMW_TP_Config *)       cfg,                    /* protocol config */ \
    (TMW_TP_Interface *)    NULL,                   /* next protocol */ \
 \
    /* I/O Queue definitions for this protocol */ \
    (TamFifoIndex)          32767,                  /* seq_num_range */ \
    (TamFifoIndex)          RFC1006_MAX_QUEUE_LEN,  /* default_queue_len */ \
    (TamQueuePolicy)        TMW_QueueTime,          /* queue_policy */ \
 \
    /* Interface routines for this protocol */ \
    (TMW_TP_Configure)              NULL, \
    (TMW_TP_Initialize_Context)     &RFC1006_initialize_context, \
    (TMW_TP_Add_Connection)         &RFC1006_add_connection, \
    (TMW_TP_Open)                   &RFC1006_open, \
    (TMW_TP_Close)                  &RFC1006_close, \
    (TMW_TP_Service)                &RFC1006_service, \
    (TMW_TP_Receive)                &RFC1006_receive, \
    (TMW_TP_Send)                   &RFC1006_send, \
    (TMW_TP_Can_Send)               &RFC1006_can_send, \
    (TMW_TP_User_Indication)        &TMW_TP_user_indication, \
    (TMW_TP_Connect_Indication)     &TMW_TP_connect_indication, \
    (TMW_TP_Disconnect_Indication)  &TMW_TP_disconnect_indication, \
    (TMW_TP_Abort_Indication)       &TMW_TP_abort_indication, \
    (TMW_TP_Connect)                &RFC1006_connect, \
    (TMW_TP_Disconnect)             &RFC1006_disconnect \
}

typedef int RFC1006_SOCKET_RESULT;
#define RFC_SOCKET_FAILED   -1
#define RFC_SOCKET_SUCCESS   0
#define RFC_SOCKET_BLOCKED   1

#ifdef __cplusplus
}
;
#endif

#endif  /* _RFC1006_H */
