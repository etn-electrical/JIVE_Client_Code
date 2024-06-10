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
 *  Declarations for Common utility functions.
 *
 *  This file should not require modification.
 */

#ifndef _TMW_H
#define _TMW_H

#ifdef __cplusplus
extern "C" {
#endif
  
/* Turn on for full debug messages */
/* #define DEBUG_TMW */

/* Turn on for full pool checking */
/* #define TMW_DEBUG_POOL */

/* Turn on if you want to see errors from TMW_parse_mac_address()
 and TMW_parse_address() */
/* #define SHOW_TMW_PARSE_ERRORS */

/* Maximum length of all SAP addresses */
#define TMW_MAX_ADDRESS     50

/* Maximum length of all MAC addresses */
#define TMW_MAX_MAC_ADDRESS 6

/* Maximum length of server names and passwords in upper layer OSI addressing */
#define TMW_MAX_NAME_SIZE       81

/* Maximum length of Object Identifiers in upper layer OSI addressing */
#define TMW_MAX_OBJ_ID_SIZE     10

#define TMW_PRINTF printf //FIXME!!

//FIXME!!
/* These were added in V9.2 to facilitate porting to 64-bit platforms       */
/* backward compatibility for customers that haven't needed it before.      */
/* It would be better to use uintptr_t, but it is difficult to detect which */
/* compilers support it.                                                    */

#if !defined(TMW_PTR_TO_INT)
#define TMW_PTR_TO_INT size_t
#endif

#if !defined(TMW_INT_TO_PTR)
#define TMW_INT_TO_PTR size_t
#endif


#define BITS_TO_BYTES( bits )   ( bits > 0 ) \
                                ? ( (bits + 7) >> 3 ) \
                                : ( ( (-bits) + 7) >> 3 )


#if !defined(_UCA_TIME_DEF)
typedef struct {
    long millisecondsSinceMidnight;
    int daysSinceJan1_1984;
#if defined(MMSD_MICROSECONDS_TIMER)
    int microseconds;
#endif
} MMSd_time; 
#define _UCA_TIME_DEF
#endif
#define TMW_Time MMSd_time

/* Thread-safe malloc */
void TMW_initialize_safe_TMW_TARG_MALLOC(void);
void TMW_delete_safe_TMW_TARG_MALLOC(void);
char *TMW_safe_TMW_TARG_MALLOC(size_t size);
void TMW_safe_TMW_TARG_FREE(char *pData);

/* ************************************************************************** */
/* TamFifo - Tamarack Generic Address Structures                              */
/*                                                                            */
/*  These structures are used throughout to represent various types of        */
/*  addresses.  Also included are some utility routines to parse and format.  */
/* ************************************************************************** */

typedef unsigned char TMW_Media;

#define TMW_Unknown         0
#define TMW_Ethernet        1
#define TMW_Rfc1006         2
#define TMW_Adlc            3
#define TMW_Other           4
#define TMW_Xml             5
#define TMW_VLAN            6
//FIXME??

typedef struct TMW_Address { /* Used for most SAPS */
    unsigned char len;
    unsigned char addr[TMW_MAX_ADDRESS];
} TMW_Address;

typedef struct TMW_Mac_Address { /* MAC addresses are usually smaller */
    unsigned char len;
    unsigned char addr[TMW_MAX_MAC_ADDRESS];
    void *userData;
} TMW_Mac_Address;

TMW_Media TMW_parse_media(unsigned char *buff);
TMW_CLIB_API unsigned char *TMW_parse_octet(unsigned char *buff, unsigned char *octet);
void TMW_parse_mac_address(unsigned char *buff, TMW_Mac_Address *dest);
TMW_CLIB_API void TMW_parse_address(unsigned char *buff, TMW_Address *dest);
TMW_CLIB_API void TMW_parse_dot_address(unsigned char *buff, TMW_Address *dest);
TMW_CLIB_API int TMW_parseObjId(char *str, int *oid);
TMW_CLIB_API int TMW_addr2hex(TMW_Address *addr, char *buffer, int len);
TMW_CLIB_API int TMW_addr2dot(TMW_Address *addr, char *buffer, int len);
TMW_CLIB_API void TMW_textObjId(int *oid, char *str, int bufLen);

/* ************************************************************************** */
/* TamFifo - Tamarack Generic Fifo                                            */
/*                                                                            */
/*  The Generic Tamarack Fifo implements FIFO ordering by maintaining a       */
/*  circular ordering of indices.  These indices can be used to index into    */
/*  arbitrary lists, etc.                                                     */
/* ************************************************************************** */

typedef struct TamFifoEntry *TamFifoEntryPtr;

typedef struct TamFifoEntry {
    TamFifoEntryPtr next;
} TamFifoEntry;

typedef long TamFifoIndex; /* Fifos are bit short of long */
#define TMW_NO_SEQ_NUM  0x80000000L

typedef struct TamFifo { /* Base information for Fifo */
    TamFifoIndex ins_index; /* Next index to insert       */
    TamFifoIndex rem_index; /* Next index to remove       */
    TamFifoIndex fifo_len; /* Overall length of fifo    */
    TamFifoIndex max_entries; /* Max members to allow       */
    TamFifoIndex num_entries; /* Current members in fifo   */
} TamFifo;

/* Error codes returned by Fifo routines */
#define TMW_QUEUE_ERROR (-1)
#define TMW_QUEUE_EMPTY (-2)
#define TMW_QUEUE_FULL  (-3)
#define TMW_QUEUE_OK(status)    (status >= 0)

#define TMW_QUEUE_ORDER (-4)
#define TMW_QUEUE_FAILURE (-5)
#define TMW_QUEUE_DUPLICATE (-6)

/* ************************************************************************** */
/* TamPool - Tamarack Generic Buffer Pool                                     */
/*                                                                            */
/*  The Generic Tamarack Pool is implemented by placing a TamPoolEntry header */
/*  on each pool buffer.  The pool routines then manage the pool by           */
/*  manipulating the header fields and through a top-level data structure.    */
/*  Pool entries may be allocated, deallocated, locked (which causes the free */
/*  to silently fail, and unlocked.                                           */
/*                                                                            */
/*   The header data structure (TamPoolEntry) maintains longword alignment.   */
/* ************************************************************************** */
typedef struct TamPoolDef *TamPoolDefPtr;
typedef struct TamPoolEntry *TamPoolEntryPtr;
typedef struct TamPool *TamPoolPtr;

/* TMW Pool entry header - one per pool entry */
typedef struct TamPoolEntry { /* Header for pool entries */
    union {
        struct {
            TamPoolEntryPtr next; /* Next in avail list       */
            TamPoolEntryPtr next_thread; /* Next in overall list     */
            unsigned char in_use; /* Usage of this entry      */
            unsigned char lock_count; /* Number of locks requests */
            TamFifoIndex entry_num; /* Pool id number of entry  */
#if defined(TMW_DEBUG_POOL)
            TamPoolPtr pool; /* Owner of this entry      */
#endif
        } info;
        struct { /* Forces longword alignment, all compilers */
#if defined(TMW_DEBUG_POOL)
            long pad[5];
#else
            long pad[4];
#endif
        } padding;
    } u;
} TamPoolEntry;

/* Codes to be used for the 'usage' parameter in TMW_alloc_pool */
/*  This parameter is used in deadlock detection.  It should be */
/*  set to the purpose for which the buffer is being allocated. */
typedef unsigned char TamPoolUsage;
#define TMW_POOL_FREE       ( (TamPoolUsage) 0 )
#define TMW_POOL_RECEIVE    ( (TamPoolUsage) 1 )
#define TMW_POOL_SEND       ( (TamPoolUsage) 2 )
#define TMW_POOL_PENDING    ( (TamPoolUsage) 3 )
#define TMW_POOL_USE_MASK   ( (TamPoolUsage) 0x03 )
#define TMW_POOL_NUM_REASONS 4

typedef struct TamPoolDef {
    void (*TamInit)(TamPoolPtr pool, TamPoolDefPtr def);
    unsigned char *(*TamAllocate)(TamPoolPtr pool, TamPoolUsage reason);
    int (*TamRoom)(TamPoolPtr pool, TamPoolUsage reason);
    void (*TamFree)(TamPoolPtr pool, unsigned char *buffer);
    void (*TamLock)(unsigned char *buffer);
    void (*TamUnlock)(unsigned char *buffer);
    void (*TamDeletePool)(TamPoolPtr pool);
    void *user_ptr;
    int data_size;
    int data_offset;
    int media;
} TamPoolDef;
#define TMW_INIT(pool,defs)             \
        {                                       \
            (pool)->def = *(defs);              \
            if ((pool)->def.TamInit != NULL) (pool)->def.TamInit( (pool), (defs) ); \
        }
#define TMW_ROOM(pool,reason )          (((pool)->def.TamRoom != NULL) ? (pool)->def.TamRoom( (pool), (reason)) : 0)
#define TMW_ALLOC(pool,reason)          (((pool)->def.TamAllocate != NULL) ? (pool)->def.TamAllocate((pool), (reason)) : NULL)
#define TMW_FREE(pool,buffer)           if ((pool)->def.TamFree != NULL) (pool)->def.TamFree( (pool), (buffer) )
#define TMW_LOCK(pool,buffer)           if ((pool)->def.TamLock != NULL) (pool)->def.TamLock( (buffer) )
#define TMW_UNLOCK(pool,buffer)         if ((pool)->def.TamUnlock != NULL) (pool)->def.TamUnlock( (buffer) )
#define TMW_DELETEPOOL(pool)            if ((pool)->def.TamDeletePool != NULL) (pool)->def.TamDeletePool( (pool) )

/* Top level TMW Pool data structure - one per pool */
typedef struct TamPool {
    TamPoolEntry *avail; /* List of available entries      */
    int uses[TMW_POOL_NUM_REASONS]; /* Num entries with each use */
    TamFifoIndex num_entries; /* Number of entries in the pool  */
    TamPoolDef def; /* Pool definitions               */
#if defined(TMW_USE_THREADS)
    TmwTargSem_t semaphore;
#endif
#if defined(TMW_DEBUG_POOL)
    TamPoolEntry *thread_list; /* Overall list of entries        */
#endif
    TamPoolEntry *dynamic_chain;  /* chain entries for freeing */
    void *user_ptr;
    /*  used for context/config   */
} TamPool;

/* TMW Generic Pool Prototypes */
/*  Common Pool Routines */
void TMW_init_pool(TamPool *pool, TamPoolDef *defs);
void TMW_delete_pool(TamPool *pool);
void TMW_insert_pool(TamPoolEntry *entry, TamPool *pool);
int TMW_check_pool(TamPool *pool);
#if defined(TMW_DEBUG_POOL)
int TMW_check_pool_entry( TamPoolEntry *entry, TamPool *pool );
#endif
void TMW_dump_pool(TamPool *pool);

/*  Simple buffer allocation */
unsigned char *TMW_alloc_pool(TamPool *pool, TamPoolUsage reason);
int TMW_room_pool(TamPool *pool, TamPoolUsage reason);
void TMW_free_pool(TamPool *pool, unsigned char *buffer);
void TMW_lock_pool(unsigned char *buffer);
void TMW_unlock_pool(unsigned char *buffer);

/* The gnrc 'packet' structure is used whenever frames (in or out)
 must exist on queues in a partially parsed or formatted form.  All
 frames should be read into buffers, typically allocated from a pool.
 The TMW_Packet descriptor is used to point at one of these buffers,
 and the data_offset and length fields are adjusted as protocol
 information is parsed off the front.  This avoids nearly all buffer
 copying. */
typedef struct TMW_Packet {
    void *pool; /* Pool to return the buffer to */
    void *buffer_id; /* Pool identifier for this buffer */
    unsigned char *buffer; /* Start of the data in the buffer */
    int buffer_length; /* Overall length of data portion */
    int data_offset; /* Offset to unparsed data in buffer */
    int data_length; /* Length of unparsed data in buffer */
    TMW_Media media; /* Media from which data came from */
    char eot; /* TRUE-> this buffer completes a msg */
    TMW_Time in_time_value; /* Holds time stamp of msg arrival */
    TMW_Time *out_time_ptr; /* If !NULL, place for outgoing time */
    unsigned long serial;
    void *context; /* Generic context of this packet */
} TMW_Packet;

/* Packet allocation routines */
int TMW_alloc_packet(TMW_Packet *packet, TamPool *pool, TamPoolUsage reason);
int TMW_can_alloc_packet(TamPool *pool, int count, TamPoolUsage reason);
void TMW_free_packet(TMW_Packet *packet, TamPool *pool);
void TMW_lock_packet(TMW_Packet *packet);
void TMW_unlock_packet(TMW_Packet *packet);

/* ************************************************************************** */
/* TamQueue - TMW Generic Queue                                               */
/*                                                                            */
/*   The TamQueue maintains an ordered list of indexed entries.  The entries  */
/*   are allocated from a TamPool, and may be inserted in the list, using     */
/*   The list may be traversed from start to finish, or you can get the next  */
/*   entry (based on the policy).                                             */
/*                                                                            */
/*   When traversing a partially ordered list, it is up to the caller to      */
/*   check the sequence numbers to see if there are skips in the list.        */
/*                                                                            */
/*   When getting the next entry of a strictly ordered list, the next entry   */
/*   will fail if there are missing sequence numbers.                         */
/*   either strict or partial ordering (based on the configured policy).      */
/*                                                                            */
/*   The header data structure (TamPoolEntry) maintains longword alignment.   */
/* ************************************************************************** */
typedef struct TamQueueEntry *TamQueueEntryPtr;
typedef struct TamQueue *TamQueuePtr;
typedef int TamMark;

typedef struct TamQueueEntry {
    TamPoolEntry     pool_data; /* Pool management info */
    TamQueueEntryPtr next;      /* Pointer for chaining */
    TamFifoIndex     seq_num;   /* FifoIndex of this member */
    TamMark          mark;      /* Flag for storing mark */
} TamQueueEntry; 				/* Should preserve long align */

typedef enum {
    TMW_QueueStrict,
	TMW_QueuePartial,
	TMW_QueueTime
} TamQueuePolicy;

typedef struct TamQueue {
    TamFifo *fifo; /* Fifo for tracking list */
    TamPool *pool; /* Entry allocation pool  */
    TamQueuePolicy policy; /* Policy for this queue  */
    TamFifoIndex num_marked; /* Number of marked entries */
    TamQueueEntry *head; /* Head of queue list */
    TamQueueEntry *tail; /* Tail of queue list */
    TamFifoIndex num_entries; /* Number of nodes in list */
    TMW_ms_time timer; /* Used for retransmission */
#if defined(TMW_USE_THREADS)
    TmwTargSem_t semaphore; /* For protection */
#endif
} TamQueue;

void TMW_init_queue(TamQueue *queue, TamPool *pool, TamFifo *fifo, TamQueuePolicy policy);
void TMW_delete_queue(TamQueue *queue);
TamFifoIndex TMW_get_queue_room(TamQueue *queue);
TamFifoIndex TMW_insert_queue(TamQueue *queue, TamQueueEntry *entry);
TamQueueEntry *TMW_remove_queue(TamQueue *queue, TamFifoIndex *seq_num);
TamQueueEntry *TMW_clean_queue(TamQueue *queue, TamFifoIndex seq_num);
TamQueueEntry *TMW_first_queue(TamQueue *queue);
TamQueueEntry *TMW_next_queue(TamQueueEntry *current);
TamQueueEntry *TMW_next_unmarked_queue(TamQueueEntry *current);
void TMW_mark_queue_entry(TamQueue *queue, TamQueueEntry *entry);
void TMW_unmark_queue(TamQueue *queue);
TamFifoIndex TMW_num_marked_queue(TamQueue *queue);

/* Generic Link Interface Definition */
/* ---------------------------------------------------------------- *
 * These prototypes define the standardized entry points into       *
 * an arbitrary link layer.  These are used to hide the link        *
 * (Ethernet or Adlc) from the osi code.                            *
 * ---------------------------------------------------------------- */
#define TMW_BIND_DIRECTED   0x0000
#define TMW_BIND_MULTICAST  0x0001
#define TMW_BIND_PROMISCUOUS 0x000F

#define TMW_BIND_ANY_LSAP   0x0000
#define TMW_BIND_LSAP       0x0010
#define TMW_BIND_VL_GOOSE   0x0020
#define TMW_BIND_VL_GSE     0x0040
#define TMW_BIND_VL_SAMPV   0x0080

#define TMW_BIND_ANY_PROT   0x0000
#define TMW_BIND_802_3      0x0100
#define TMW_BIND_DIX        0x0200

#define TMW_BIND_NORMAL     0x0000
#define TMW_BIND_MGMT       0x1000
#define TMW_BIND_GOOSE      0x2000

#define TMW_BIND_MAC_MASK   0x000F
#define TMW_BIND_LSAP_MASK  0x00F0
#define TMW_BIND_PROT_MASK  0x0F00
#define TMW_BIND_IND_MASK   0xF000

typedef struct TMW_DL_Context *TMW_DL_ContextPtr;

typedef int TMW_DL_Bind_Type; /* MAC+LSAP+PROT+IND */

/* Basic Data Link Indications */
typedef void (*TMW_DL_Data_Ind)(TMW_DL_ContextPtr context, void *ind_param);
typedef int (*TMW_DL_Mgmt_Ind)(TMW_DL_ContextPtr context, TMW_Packet *packet, void *ind_param);
typedef int (*TMW_DL_Test_Ind)(TMW_DL_ContextPtr context, unsigned char *buffer, int length, void *ind_param);

/* The TMW_DL_Ind could be any of three forms:

 TMW_DL_Data_Ind       (TMW_BIND_NORMAL)
 - simply passes the data up - for osi
 TMW_DL_Mgmt_Ind       (TMW_BIND_MGMT)
 - return signals response to send
 TMW_DL_Test_Ind       (TMW_BIND_GOOSE)
 - return signals data accepted, else pass to other bindings
 */
typedef TMW_DL_Test_Ind TMW_DL_Ind; /* Used as param to Bind, below */
typedef union {
    TMW_DL_Data_Ind normal_func;
    TMW_DL_Mgmt_Ind mgmt_func;
    TMW_DL_Test_Ind goose_func;
    void *any_func;
} TMW_DL_Ind_Union;

/* Used internally to store binds */
typedef struct TMW_DL_Binding *TMW_DL_BindingPtr;
typedef struct TMW_DL_Binding {
    int bind_type;
    TMW_Mac_Address multi_addr;
    int lsap;
    TMW_DL_Ind_Union ind_function;
    void *ind_param;
    TMW_DL_BindingPtr next;
} TMW_DL_Binding;

/* Basic Data Link Services */
typedef TMW_DL_ContextPtr (*TMW_DL_Open)(char *adapter_name);
typedef int (*TMW_DL_Close)(TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Bind)(TMW_DL_ContextPtr context, TMW_DL_Bind_Type bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);
typedef int (*TMW_DL_UnBind)(TMW_DL_ContextPtr context, int bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);
typedef int (*TMW_DL_Service)(TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Receive)(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Can_Send)(int length, TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Send)(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Send_Mgmt)(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr cntxt, MMSd_time *ts_send);

/* Services and Indications for connection oriented links (ADLC) */
typedef int (*TMW_DL_Connect)(TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Disconnect)(TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
typedef int (*TMW_DL_Wait_For_Poll)(TMW_DL_ContextPtr cntxt, TMW_ms_time delay);
typedef void (*TMW_DL_Abort_Ind)(TMW_DL_ContextPtr context);
typedef void (*TMW_DL_Disconnect_Ind)(TMW_DL_ContextPtr context);
typedef void (*TMW_DL_Connect_Ind)(TMW_DL_ContextPtr context);

/* ---------------------------------------------------------------- */
typedef struct TMW_DL_Definition {
    TMW_DL_Open TamDlOpen;
    TMW_DL_Close TamDlClose;
    TMW_DL_Bind TamDlBind;
    TMW_DL_UnBind TamDlUnBind;
    TMW_DL_Service TamDlService;
    TMW_DL_Receive TamDlReceive;
    TMW_DL_Can_Send TamDlCanSend;
    TMW_DL_Send TamDlSend;
    TMW_DL_Send_Mgmt TamDlSendMgmt;
    /* Used for connection oriented links: */
    TMW_DL_Connect TamDlConnect;
    TMW_DL_Disconnect TamDlDisconnect;
    TMW_DL_Wait_For_Poll TamDlWaitForPoll;
    TMW_Mac_Address es_address;
    TMW_Mac_Address is_address;
} TMW_DL_Definition;

/* ---------------------------------------------------------------- */
typedef struct TMW_DL_Context {
#if defined(_WINDOWS) || defined(_WIN32)
	HANDLE fd;
#else
    unsigned long fd;
#endif
	char adapter_name[129];
    TMW_Mac_Address mac_address;
    TMW_DL_Data_Ind data_indication;
    TMW_DL_Mgmt_Ind mgmt_indication;
    TamPool *buffers;
    TamPool bindings;
    void *user_ptr;
    int index;
    unsigned long r_packet_serial; /* Next packet serial number */
    TMW_DL_ContextPtr next;
    TMW_DL_Definition *dl_def;
    TMW_DL_Binding *normal_binds;
    TMW_DL_Binding *goose_binds;
    TMW_DL_Binding *mgmt_binds;
    int nNumOpens;
#if defined(TMW_USE_THREADS)
    TmwTargSem_t sem;
    TmwTargSem_t userSem;
#endif
} TMW_DL_Context;

/* Basic Data Link Services */
int TMW_DL_close(TMW_DL_ContextPtr context);
int TMW_DL_bind(TMW_DL_ContextPtr context, int bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);
int TMW_DL_unbind(TMW_DL_ContextPtr context, int bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);
int TMW_DL_service(TMW_DL_ContextPtr context);
int TMW_DL_receive(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
int TMW_DL_can_send(int length, TMW_DL_ContextPtr context);
int TMW_DL_send(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
int TMW_DL_send_mgmt(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_ContextPtr cntxt, MMSd_time *ts_send);

/* Services for connection oriented links (ADLC) */
int TMW_DL_connect(TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
int TMW_DL_disconnect(TMW_Mac_Address *mac, TMW_DL_ContextPtr context);
int TMW_DL_wait_for_poll(TMW_DL_ContextPtr cntxt, TMW_ms_time delay);

/* Added for handling CRC16 in Goose/Sampled Values with 62351 security */
void TMW_61850_DL_crc16_compute(unsigned char *pBuffer, int nLength, unsigned int *pCrc);
int TMW_61850_DL_crc16_check(unsigned char *pBuffer, int nLength, unsigned int nCrc);

/* Used by routines in both Goose and Sampled Values for encoding/decoding security PDU extensions */
#define VLAN_Security_AuthenticationValue3  0x83
#define VLAN_Security_Private2              0x82
#define VLAN_Security_Reserved1             0x81
#define VLAN_Security_SecurityExtension0    0xA0
#define VLAN_SIGNATURE_SIZE                 128

/* Callback function used by both Goose and Sampled Values for digital signatures for secure frames */
typedef int (*TMW_61850_VLAN_MakeSignatureFcn)(unsigned char *pToBeSigned, int nToBeSignedLength,
        unsigned char *pSignature, int nSignatureLength, void *pCryptoContext, void *SignatureUserData);

typedef int (*TMW_61850_VLAN_CheckSignatureFcn)(unsigned char *pToBeSigned, int nToBeSignedLength,
        unsigned char *pSignature, int nSignatureLength, void *pCryptoContext, void *SignatureUserData);




//FIXME!!
#if defined(TMW_SAFE_MALLOC)
#include <malloc.h>

#define TMW_TARG_MALLOC(a) TMW_TMW_TARG_MALLOC(a)
#define TMW_TARG_FREE(a) TMW_TMW_TARG_FREE((char *) a)

#define TMW_TMW_TARG_MALLOC(a) TMW_safe_TMW_TARG_MALLOC(a)
#define TMW_TMW_TARG_FREE(a) TMW_safe_TMW_TARG_FREE(a)

/* malloc definition - only used in RTG routines */
#define TMW_MALLOC( c, size ) TMW_TMW_TARG_MALLOC( size )
#else
#define TMW_MALLOC( c, size ) TMW_TARG_MALLOC( size )
#endif

#ifdef __cplusplus
}
;
#endif

#endif /* _TMW_H */
