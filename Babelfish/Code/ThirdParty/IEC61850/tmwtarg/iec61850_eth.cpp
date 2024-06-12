/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1998-2014 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                       www.TriangleMicroWorks.com                          */
/*                          (919) 870-6615                                   */
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
 *  Interface to iec61850 ethernet.
 *  
 */

#define ETH_LNXP_C
#ifdef ETH_LNXP_C

#define IEC61850_ETH_CHANGE_FREERTOS // iec61850 ethenet port - code changes for FreeRTOS

#define DISABLE_VALUE   0U

/* #define ETH_SHOW */
/* #define DEBUG_ETH */
/* #define XDEBUG_ETH */
/* #define DEBUG_ETH_ERRORS */
/* #define DEBUG_ETH_DUMP */

#include <stdio.h>
#include <string.h>

#include "lwip/sockets.h" 
#include "Stm32_Eth_Driver.h"   
#include "sys.h" 

#include "tmwtarg.h"
#include "eth1.h"
#include "iec61850_eth.h"   

#include "DCI_Patron.h"

#define RAWLEN_RECALCULATION       /* Raw length calculation for read packet length */

#ifndef IEC61850_ETH_CHANGE_FREERTOS
#define PF_PACKET                       17        /* Packet family.  */ /
#define AF_PACKET                       PF_PACKET 
#define ETH_P_ALL                       3   
#define PACKET_MR_MULTICAST             0 //  https://docs.huihoo.com/doxygen/linux/kernel/3.7/if__packet_8h_source.html
#define PACKET_ADD_MEMBERSHIP           1 

/* Per TUCOWS Packet.7 Man page */
#ifndef SOL_PACKET
#define SOL_PACKET 263
#endif
#endif

#define ETHER_ADDR_LEN       6
#define IEEE802_3_OVERHEAD  14
typedef struct IEEE802_3_frame {
    unsigned char dest_addr[6];
    unsigned char src_addr[6];
    unsigned short length;
    unsigned char data[1500];
} IEEE802_3_frame;

TMW_DL_Definition ETH_DL_Definition = { ETH_open_port, /* TamDlOpen       */
                                        ETH_close_port, /* TamDlClose      */
                                        ETH_bind, /* TamDlBind       */
                                        ETH_unbind, /* TamDlUnBind     */
                                        ETH_service, /* TamDlService    */
                                        ETH_receive, /* TamDlReceive    */
                                        ETH_can_send, /* TamDlCanSend    */
                                        ETH_send, /* TamDlSend       */
                                        ETH_send_mgmt, /* TamDlSendMgmt   */
                                        NULL, /* TamDlConnect    */
                                        NULL, /* TamDlDisconnect */
                                        NULL, /* TamDlWaitForPoll*/
{
    6,
    {
        0x09,
        0x00,
        0x2b,
        0x00,
        0x00,
        0x04
    },
	NULL
}, /* es_address      */
{
    6,
    {
        0x09,
        0x00,
        0x2b,
        0x00,
        0x00,
        0x05
    },
	NULL
} /* is_address      */
                                      };

#define MAX_ETH_CONTEXTS 4
static LNXETH_Context eth_context_list[MAX_ETH_CONTEXTS];
static unsigned char eth_context_allocated[MAX_ETH_CONTEXTS] = { 0 };
static TMW_DL_Context *eth_context_active = NULL;
#if defined(TMW_USE_THREADS)
int ethSemInited = 0;
TmwTargSem_t eth_semaphore;
#endif




/* ------------------------------------------- */
/* Pool definition for pool of binding records */
typedef struct ETH_BindingPoolEntry {
    TamPoolEntry pool_data;
    LNXETH_Binding binding_data;
} ETH_BindingPoolEntry;

TamPoolDef ETH_BindPoolDefs = { NULL, /* TamInit */
                                TMW_alloc_pool, /* TamAllocate */
                                TMW_room_pool, /* TamRoom */
                                TMW_free_pool, /* TamFree */
                                NULL, /* TamLock */
                                NULL, /* TamUnlock */
                                NULL, /* TamDeletePool */
                                NULL, /* user_ptr */
                                sizeof(LNXETH_Binding), /* data_size */
                                0, /* data_offset */
                                TMW_Ethernet, /* data_media */
                              };

/* ------------------------------------------- */
/* Pool definition for pool frame buffers      */
/* NOTE: The data buffer below is extended past 1517 to allow for the
 WinDIS Ethernet driver overhead.  Shouldn't be needed on most
 platforms. */
typedef struct ETH_BufferPoolEntry {
    TamPoolEntry pool_data; /* Used by Tam gnrc pool fcns */
    unsigned char buffer_data[1600]; /* ETH_size + longword alignment */
} ETH_BufferPoolEntry;

#define ETH_MAX_BUFFERS (NUM_PACKET_PACKS * 2)

TamPoolDef ETH_BuffPoolDefs = { NULL, /* TamInit */
                                TMW_alloc_pool, /* TamAllocate */
                                TMW_room_pool, /* TamRoom */
                                TMW_free_pool, /* TamFree */
                                TMW_lock_pool, /* TamLock */
                                TMW_unlock_pool, /* TamUnlock */
                                NULL, /* TamDeletePool */
                                NULL, /* user_ptr */
                                ETH_size, /* data_size */
                                IEEE802_3_OVERHEAD, /* data_offset */
                                TMW_Ethernet, /* data_media */
                              };

/* Local prototypes */
TMW_DL_Context *ETH_find_context(char *adapter_name);
TMW_DL_Context *ETH_new_context(char *adapter_name);
void ETH_free_context(TMW_DL_Context *freeCtx);
void ETH_init_queue(ETH_QUEUE *queue);
int ETH_insert_queue(ETH_Read_Packet *read_packet,
                     ETH_QUEUE *queue);
ETH_Read_Packet *ETH_remove_queue(ETH_QUEUE *queue);
int ETH_fill_pool(TamPool *pool,
                  int count,
                  long recSize);
/* Added V7.99x - cleanup closed ethernet adapters */
int ETH_drain_pool(TamPool *pool,
                   int count,
                   long recSize);
int ETH_check_bind(unsigned char *packetBuffer,
                   TMW_DL_Binding *binding);
int ETH_data_indication(LNXETH_ContextPtr lnxeth_context);

#ifdef IEC61850_ETH_CHANGE_FREERTOS
/**
 * @brief: Get Ethernet/EMAC address
 *  
 * @param[in] mac_addres: pointer to get Mac address
 * @param[out] void
 */
void GetMacAddress(uint8_t* mac_address); 

/**
 * @brief: Revesre the string value
 *  
 * @param[in] string: pointer to reverse the string
 * @param[in] length: lenth of the pointer
 */
void Reverse(uint8_t* string, uint8_t length);
#endif

void ETH_lockContextSet(void)
{
#if defined(TMW_USE_THREADS)
    if ( ethSemInited == 0 ) {
        TMW_TARG_SEM_CREATE( eth_semaphore );
        ethSemInited = 1;
    }
    TMW_TARG_SEM_TAKE( eth_semaphore );
#endif
}

void ETH_unlockContextSet(void)
{
#if defined(TMW_USE_THREADS)
    TMW_TARG_SEM_GIVE( eth_semaphore );
#endif
}

//FIXME!! void?
int ETH_adapter_dialog(const char *prompt,
                       char *adapter)
{
    int len;

    /* V7.24 - updated this for LINUX only */
#if defined(DEBUG_ETH)
    if (prompt != NULL)
        printf( "\n%s - Ethernet adapter not specified\n", prompt );
    else
        printf( "Ethernet adapter not specified\n" );
    printf( "Enter adapter name (e.g. eth0): " );
#endif
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    fgets( adapter, 129, stdin );
#endif
    len = strlen( adapter );
    while (len > 0) {
        if ((adapter[len - 1] == '\n') || (adapter[len - 1] == '\r')) {
            adapter[len - 1] = 0;
            len--;
        } else
            break;
    }
    if (len == 0)
        strcpy( adapter, "en0" ); // adapter name changed
    return 0;
}

/* ************************************************** */
TMW_DL_Context *ETH_find_context(char *adapter_name)
{
    TMW_DL_Context *ctx;
    ctx = eth_context_active;
    while (ctx != NULL) {
        if (strcmp( ctx->adapter_name, adapter_name ) == 0)
            return (ctx);
        ctx = ctx->next;
    }
    return (NULL);
}

TMW_DL_Context *ETH_new_context(char *adapter_name)
{
    int i;
    TMW_DL_Context *ctx;

    for (i = 0; i < MAX_ETH_CONTEXTS; i++) { 
        if (eth_context_allocated[i])
            continue;
//FIXME?
        /* NOT THREAD SAFE */
        eth_context_allocated[i] = TRUE;
        ctx = (TMW_DL_Context *) &eth_context_list[i];
        memset( ctx, 0, sizeof(TMW_DL_Context) );
        ctx->index = i;
        ctx->next = eth_context_active;
        eth_context_active = (TMW_DL_Context *) &eth_context_list[i];
        /* END NOT THREAD SAFE */

        ((LNXETH_Context *) ctx)->socket = 0; 
        ((LNXETH_Context *) ctx)->if_index = 0;

        strcpy( ctx->adapter_name, adapter_name );
        TMW_TARG_SEM_CREATE( ctx->sem );
        TMW_TARG_SEM_CREATE( ctx->userSem );
        return (ctx);
    }
    return (NULL);
}

void ETH_free_context(TMW_DL_Context *freeCtx)
{
    TMW_DL_Context **anchor, *scan;
    anchor = &eth_context_active;
    scan = eth_context_active;
    while (scan != NULL) {
        if (scan == freeCtx) {
            *anchor = freeCtx->next;
            eth_context_allocated[freeCtx->index] = FALSE;
            TMW_TARG_SEM_DELETE( freeCtx->sem );
            TMW_TARG_SEM_DELETE( freeCtx->userSem );
            return;
        }
        anchor = &scan->next;
        scan = scan->next;
    }
    /* Not found in active list */
#if defined(DEBUG_ETH_ERRORS)
    if (freeCtx->index >= MAX_ETH_CONTEXTS) {
        printf("Bad free of ethernet context\n");
        return;
    }
    if (!eth_context_allocated[ freeCtx->index ]) {
        printf("Free of free ethernet context\n");
        return;
    } else {
        printf("Allocated ETH context not in list\n");
        return;
    }
#endif
    if (freeCtx->buffers != NULL)
        TMW_TARG_FREE( freeCtx->buffers );
}

void ETH_init_queue(ETH_QUEUE *queue)
{
    queue->ins_pkt_index = 0;
    queue->rem_pkt_index = 0;
    queue->pkt_queue_len = 0;
}

int ETH_insert_queue(ETH_Read_Packet *read_packet,
                     ETH_QUEUE *queue)
{
    int temp_index = queue->ins_pkt_index;

    temp_index++;
    if (temp_index >= NUM_PACKET_PACKS)
        temp_index = 0;
    if (temp_index == queue->rem_pkt_index)
        return ( FALSE);
    queue->read_packets[queue->ins_pkt_index] = *read_packet;
    queue->ins_pkt_index = temp_index;
    return ( TRUE);
}

ETH_Read_Packet *ETH_remove_queue(ETH_QUEUE *queue)
{
    int temp_index = queue->rem_pkt_index;
    ETH_Read_Packet *read_packet;

    if (temp_index == queue->ins_pkt_index)
        return (NULL);

    read_packet = &(queue->read_packets[temp_index]);
    temp_index++;
    if (temp_index >= NUM_PACKET_PACKS)
        temp_index = 0;
    queue->rem_pkt_index = temp_index;
    return (read_packet);
}

#ifdef XDEBUG_ETH
void ETH_display_queue_status( ETH_QUEUE *queue )
{
    printf(, "# ins = %d, rem = %d, qlen = %d\n", ins_pkt_index,
           rem_pkt_index, pkt_queue_len);
}
#endif

int ETH_fill_pool(TamPool *pool,
                  int count,
                  long recSize)
{
    int i;
    TamPoolEntry *ptr;

    for (i = 0; i < count; i++) {
        ptr = (TamPoolEntry *) TMW_TARG_MALLOC( recSize );
        if (ptr == NULL)
            return ( FALSE);
        TMW_insert_pool( ptr, pool );
    }
    return (1);
}

int ETH_drain_pool(TamPool *pool,
                   int count,
                   long recSize)
{
    int i;
    TamPoolEntry *ptr, *pnext;

    if ( recSize || count )
    {}; /* suppress warning */

    ptr = pool->avail;
    for (i = 0; i < pool->num_entries; i++) {
        if (ptr == NULL)
            return ( FALSE);

        pnext = ptr->u.info.next;
        if (ptr->u.info.in_use == TMW_POOL_FREE)
            TMW_TARG_FREE( ptr );

        ptr = pnext;
    }
    pool->avail = NULL;
    pool->num_entries = 0;
    return (1);
}

TMW_DL_Context *ETH_open_port(char *adapter_name)
{
    int i;
    TMW_DL_Context *eth_context;
    static LNXETH_Context *lnxeth_context;
    static struct sockaddr_in sockaddr; // replace sockaddr_ll struct to sockaddr_in for socket cration using lwip
    struct ifreq my_ifreq;
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    unsigned char *ptr;
#endif                          
    /* Verify adapter name */
    if (strlen( adapter_name ) == 0) {
#ifdef DEBUG_ETH_ERRORS
        printf("# Invalid Adapter Name! - exiting\n");
#endif
        return (NULL);
    }

    ETH_lockContextSet();    
  
    /* See if this adapter already open */
    eth_context = ETH_find_context( adapter_name );
    if (eth_context != NULL) {
#if defined(DEBUG_ETH_ERRORS)
        printf("ETH_open_port(): port already opened for %s\n", adapter_name );
#endif
        TMW_TARG_SEM_TAKE( eth_context->sem );
        /* Added V7.99x - cleanup closed ethernet adapters */
        eth_context->nNumOpens++;
        TMW_TARG_SEM_GIVE( eth_context->sem );

        /* This is not an error! */
        ETH_unlockContextSet();
        return (eth_context);
    }

    /* Allocate new context */
    eth_context = ETH_new_context( adapter_name );
    if (eth_context == NULL) {
#if defined(DEBUG_ETH)
        printf("ETH_open_port(): out of ETH contexts for %s\n", adapter_name );
#endif
        ETH_unlockContextSet();
        return (NULL);
    }

    TMW_TARG_SEM_TAKE( eth_context->sem );
    eth_context->nNumOpens = 1;

    lnxeth_context = (LNXETH_Context *) eth_context;

    /* Initialize pool of binding records for this context */
    TMW_init_pool( &eth_context->bindings, &ETH_BindPoolDefs );
    ETH_fill_pool( &eth_context->bindings,
                   TMW_ETH_MAX_BINDINGS,
                   sizeof(ETH_BindingPoolEntry) );

    /* Initialize pool of frame buffers for this context */
    eth_context->buffers = (TamPool *) TMW_TARG_MALLOC( sizeof(TamPool) );
    if (eth_context->buffers == NULL) {
#if defined(DEBUG_ETH)
        printf("Malloc buffers failed!\n");
#endif
        ETH_drain_pool( &eth_context->bindings,
                        TMW_ETH_MAX_BINDINGS,
                        sizeof(ETH_BindingPoolEntry) );
        TMW_DELETEPOOL( &eth_context->bindings );
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_free_context( eth_context );
        ETH_unlockContextSet();
        return (NULL);
    }

    TMW_init_pool( eth_context->buffers, &ETH_BuffPoolDefs );
    ETH_fill_pool( eth_context->buffers,
                   ETH_MAX_BUFFERS,
                   sizeof(ETH_BufferPoolEntry) );

    eth_context->dl_def = &ETH_DL_Definition;
    ETH_init_queue( &lnxeth_context->eth_queue );

    /* Init packet serial number */
    eth_context->r_packet_serial = 0L;

    /* Get socket for all I/O */
    lnxeth_context->socket = socket( AF_INET, SOCK_RAW, htons(IPPROTO_RAW)); // Socket cration for RAW packet    
    
    if (lnxeth_context->socket < 0)  // return value from 0, if socket creation is succesful
    {
        ETH_drain_pool( eth_context->buffers,
                     ETH_MAX_BUFFERS,
                     sizeof(ETH_BufferPoolEntry) );
        ETH_drain_pool( &eth_context->bindings,
                     TMW_ETH_MAX_BINDINGS,
                     sizeof(ETH_BindingPoolEntry) );
        TMW_DELETEPOOL( eth_context->buffers );
        TMW_DELETEPOOL( &eth_context->bindings );
        TMW_TARG_FREE( eth_context->buffers );
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_free_context( eth_context );
        ETH_unlockContextSet();
        #if defined(DEBUG_ETH)
        printf("Socket call failed!\n");
        #endif
        return (NULL);
    }

    /* Get interface index of ethernet */
    memset( &my_ifreq, 0, sizeof(my_ifreq) );

    strncpy( my_ifreq.ifr_name, adapter_name, IFNAMSIZ );
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    if (ioctl( lnxeth_context->socket, SIOCGIFINDEX, &my_ifreq ) < 0) {
#else
    if (0)    {
#endif
#if defined(DEBUG_ETH)
        perror("IOCTL(ifindex) FAILED:");
#endif
        ETH_drain_pool( eth_context->buffers,
                        ETH_MAX_BUFFERS,
                        sizeof(ETH_BufferPoolEntry) );
        ETH_drain_pool( &eth_context->bindings,
                        TMW_ETH_MAX_BINDINGS,
                        sizeof(ETH_BindingPoolEntry) );
        TMW_DELETEPOOL( eth_context->buffers );
        TMW_DELETEPOOL( &eth_context->bindings );
        TMW_TARG_FREE( eth_context->buffers );
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_free_context( eth_context );
        ETH_unlockContextSet();
        return (NULL);
    }

    /* Bind to any address */
    memset( &sockaddr, 0, sizeof(sockaddr) );
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    sockaddr.sll_ifindex = my_ifreq.ifr_ifindex;
    sockaddr.sll_family = AF_PACKET;
    sockaddr.sll_protocol = htons( ETH_P_ALL );
#else    
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port =  0; // unused port randomly
    sockaddr.sin_addr.s_addr = INADDR_ANY; // use my IP addrees
#endif
    
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    if (setsockopt(lnxeth_context->socket, SOL_SOCKET, SO_BINDTODEVICE, (void *) &mreq, sizeof(mreq) ))  // socket option that apply to socket layer (SOL_PACKET
    { 
#ifdef DEBUG_ETH
            printf("setsockopt failed for adding multicast\n");
#endif
    }
#endif
        
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    lnxeth_context->if_index = my_ifreq.ifr_ifindex;
#endif
    
    if (bind( lnxeth_context->socket, (struct sockaddr *) &sockaddr, sizeof(sockaddr) ) < 0) {
#if defined(DEBUG_ETH)
        perror("Bind:");
#endif
        ETH_drain_pool( eth_context->buffers,
                        ETH_MAX_BUFFERS,
                        sizeof(ETH_BufferPoolEntry) );
        ETH_drain_pool( &eth_context->bindings,
                        TMW_ETH_MAX_BINDINGS,
                        sizeof(ETH_BindingPoolEntry) );
        TMW_DELETEPOOL( eth_context->buffers );
        TMW_DELETEPOOL( &eth_context->bindings );
        TMW_TARG_FREE( eth_context->buffers );
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_free_context( eth_context );
        ETH_unlockContextSet();
        return (NULL);
    }

#ifndef IEC61850_ETH_CHANGE_FREERTOS
    /* Get MAC Address */
    if (ioctl( lnxeth_context->socket, SIOCGIFHWADDR, &my_ifreq ) >= 0) 
    { 
        ptr = (unsigned char *) &my_ifreq.ifr_hwaddr;
        for (i = 0; i < 6; i++) {
            eth_context->mac_address.addr[i] = *(ptr + 2 + i);
        }
        eth_context->mac_address.len = 6;
    }
#endif

#ifdef IEC61850_ETH_CHANGE_FREERTOS
    /* Get the MAC Address */     
    {             
      uint8_t mac_address[TMW_MAX_MAC_ADDRESS];
      
      GetMacAddress(mac_address);     
      Reverse(mac_address, TMW_MAX_MAC_ADDRESS);
      
      memcpy(eth_context->mac_address.addr, mac_address,TMW_MAX_MAC_ADDRESS);
      eth_context->mac_address.len = TMW_MAX_MAC_ADDRESS;   
    }    
#endif

    TMW_TARG_SEM_GIVE( eth_context->sem );
    ETH_unlockContextSet();
    return (eth_context);
}

int ETH_close_port(TMW_DL_Context *eth_context)
{
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) eth_context;

#ifdef ETH_SHOW
    printf("# Closing Adapter!\n" );
#endif
    if (eth_context == NULL)
        return ( FALSE);

    ETH_lockContextSet();
    TMW_TARG_SEM_TAKE( eth_context->sem );
    if (eth_context->nNumOpens <= 0) {
#if defined(DEBUG_ETH)
        printf("Eth context was previously closed\n");
#endif
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_unlockContextSet();
        return ( FALSE); /* Error - more closes than opens */
    }

    eth_context->nNumOpens--;
    if (eth_context->nNumOpens > 0) {
        TMW_TARG_SEM_GIVE( eth_context->sem );
        ETH_unlockContextSet();
        return ( TRUE); /* Not an error, must wait for all closes */
    }

    close( lnxeth_context->socket );
    lnxeth_context->socket = 0;

    ETH_drain_pool( eth_context->buffers,
                    ETH_MAX_BUFFERS,
                    sizeof(ETH_BufferPoolEntry) );
    ETH_drain_pool( &eth_context->bindings,
                    TMW_ETH_MAX_BINDINGS,
                    sizeof(ETH_BindingPoolEntry) );
    TMW_TARG_FREE( eth_context->buffers );

    TMW_TARG_SEM_GIVE( eth_context->sem );
    ETH_free_context( eth_context );
    ETH_unlockContextSet();
    return (TRUE);
}

int ETH_bind(TMW_DL_Context *eth_context,
             TMW_DL_Bind_Type bind_type,
             TMW_Mac_Address *multi_addr,
             int lsap,
             TMW_DL_Ind ind_function,
             void *ind_param)
{
    int i;
    int bind_ind_only = (bind_type & TMW_BIND_IND_MASK);
    TMW_DL_Binding **anchor, *newBind;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) eth_context;
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    struct packet_mreq mreq;
#endif

    /* NOTE: We probably should check for duplicates, but instead
     we will propogate each incoming frame to all bindings
     that match it. */

    if (eth_context == NULL)
        return ( FALSE);

    TMW_TARG_SEM_TAKE( eth_context->sem );
    switch (bind_ind_only) {
    case TMW_BIND_NORMAL:
        anchor = &eth_context->normal_binds;
        break;
    case TMW_BIND_MGMT:
        anchor = &eth_context->mgmt_binds;
        break;
    case TMW_BIND_GOOSE:
        switch (bind_type & TMW_BIND_LSAP_MASK) {
        case TMW_BIND_VL_GOOSE:
            if ((lsap >= 0x0000) && (lsap <= 0x3FFF)) /* Type 1 Goose AppID */
                break;
            if ((lsap >= 0x8000) && (lsap <= 0xBFFF)) /* Type 1A (trip) Goose AppID */
                break;
            return ( FALSE);
        case TMW_BIND_VL_GSE:
            if (lsap & 0xFF00)
                return ( FALSE);
            break;
        case TMW_BIND_VL_SAMPV:
            if ((lsap >= 0x4000) && (lsap <= 0x7FFF)) /* Sampled Values AppID */
                break;
            return ( FALSE);
        default:
            break;
        }

        anchor = &eth_context->goose_binds;
        break;
    default:
        TMW_TARG_SEM_GIVE( eth_context->sem );
        return ( FALSE);
    }

    /* Make sure we can allocate one before doing anything */
    newBind = (TMW_DL_Binding *) TMW_ALLOC( &eth_context->bindings, TMW_POOL_RECEIVE );
    if (newBind == NULL) {
#ifdef DEBUG_ETH
        printf("# Too many binds\n" );
#endif
        TMW_TARG_SEM_GIVE( eth_context->sem );
        return ( FALSE);
    }

    switch (bind_type & TMW_BIND_MAC_MASK) {
    case TMW_BIND_MULTICAST:
#ifndef IEC61850_ETH_CHANGE_FREERTOS
      /* Use multi_addr->addr */
        mreq.mr_ifindex = lnxeth_context->if_index;
        mreq.mr_type = PACKET_MR_MULTICAST;
        mreq.mr_alen = 6;
        for (i = 0; i < 6; i++)
            mreq.mr_address[i] = multi_addr->addr[i];
        if (setsockopt( lnxeth_context->socket,
                        SOL_PACKET,
                        PACKET_ADD_MEMBERSHIP, (void *) &mreq, sizeof(mreq) )) {
#ifdef DEBUG_ETH
            printf("setsockopt failed for adding multicast\n");
#endif
        }
#endif        
        break;
    case TMW_BIND_DIRECTED:
        break;
    case TMW_BIND_PROMISCUOUS:
        /* This is not required for normal operation.  It is used
         strictly for the Tamarack test client to do browsing
         of GOOSE messages.  This shouldn't be needed for any
         production systems. */
        break;
    default:
        TMW_FREE( &eth_context->bindings, (unsigned char * ) newBind );
        TMW_TARG_SEM_GIVE( eth_context->sem );
        return ( FALSE);
    }

    if (*anchor != NULL) {
        if (((*anchor)->bind_type & TMW_BIND_MAC_MASK) == TMW_BIND_PROMISCUOUS)
            anchor = &(*anchor)->next;
    }
    newBind->bind_type = bind_type;
    if (multi_addr != NULL)
        newBind->multi_addr = *multi_addr;
    else
        newBind->multi_addr.len = 0;
    newBind->lsap = lsap;
    newBind->ind_function.any_func = ind_function;
    newBind->ind_param = ind_param;
    newBind->next = *anchor;
    *anchor = newBind;
    TMW_TARG_SEM_GIVE( eth_context->sem );
    return ( TRUE);
}

int ETH_unbind(TMW_DL_Context *eth_context,
               TMW_DL_Bind_Type bind_type,
               TMW_Mac_Address *multi_addr,
               int lsap,
               TMW_DL_Ind ind_function,
               void *ind_param)
{
    TMW_DL_Binding **anchor, *scan;
    int bind_ind_only = (bind_type & TMW_BIND_IND_MASK);
    int i;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) eth_context;
#ifndef IEC61850_ETH_CHANGE_FREERTOS
    struct packet_mreq mreq;
#endif
    
    if (eth_context == NULL)
        return ( FALSE);

    TMW_TARG_SEM_TAKE( eth_context->sem );
    switch (bind_ind_only) {
    case TMW_BIND_NORMAL:
        anchor = &eth_context->normal_binds;
        break;
    case TMW_BIND_MGMT:
        anchor = &eth_context->mgmt_binds;
        break;
    case TMW_BIND_GOOSE:
        anchor = &eth_context->goose_binds;
        break;
    default:
        return ( FALSE);
    }
    scan = *anchor;
    while (scan != NULL) {
        if (scan->bind_type == bind_type) {
            if (!(bind_type & TMW_BIND_LSAP) || (scan->lsap == lsap)) {
#ifndef IEC61850_ETH_CHANGE_FREERTOS
                  if ((scan->ind_function.any_func == ind_function) && (scan->ind_param == ind_param)) {
#else
                    if(1) {
#endif
                      switch (bind_type & TMW_BIND_MAC_MASK) {
                    case TMW_BIND_MULTICAST:
                        /* Destroy binding and socket */
                        if (scan->multi_addr.len == multi_addr->len) {
                            if (multi_addr->len <= 0) {
                                *anchor = scan->next;
                                TMW_FREE( &eth_context->bindings, (unsigned char * ) scan );
                                TMW_TARG_SEM_GIVE( eth_context->sem );
                                return ( TRUE);
                            } else {
                                if (memcmp( scan->multi_addr.addr, multi_addr->addr, multi_addr->len ) == 0) {
#ifndef IEC61850_ETH_CHANGE_FREERTOS
                                  mreq.mr_ifindex = lnxeth_context->if_index;
                                    mreq.mr_type = PACKET_MR_MULTICAST;
                                    mreq.mr_alen = 6;
                                    for (i = 0; i < 6; i++)
                                        mreq.mr_address[i] = multi_addr->addr[i];
                                    if (setsockopt( lnxeth_context->socket,
                                                    SOL_PACKET,
                                                    PACKET_DROP_MEMBERSHIP, (void *) &mreq, sizeof(mreq) )) {
#ifdef DEBUG_ETH
                                        printf("setsockopt failed for drop multicast\n");
#endif
                                    }

                                    *anchor = scan->next;
                                    TMW_FREE( &eth_context->bindings, (unsigned char * ) scan );
                                    TMW_TARG_SEM_GIVE( eth_context->sem );
                                    return ( TRUE);
#endif                                    
                                }
                            }
                        }
                        break;
                    case TMW_BIND_DIRECTED:
                        *anchor = scan->next;
                        TMW_FREE( &eth_context->bindings, (unsigned char * ) scan );
                        TMW_TARG_SEM_GIVE( eth_context->sem );
                        return ( TRUE);
                    case TMW_BIND_PROMISCUOUS:
                        /* This is not required for normal operation.  It is
                         used strictly for the Tamarack test client to do
                         browsing of GOOSE messages.  This shouldn't be needed
                         for any production systems. */
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        anchor = &scan->next;
        scan = *anchor;
    }
    TMW_TARG_SEM_GIVE( eth_context->sem );
    return ( FALSE);
}

int ETH_check_bind(unsigned char *packetBuffer,
                   TMW_DL_Binding *binding)
{
    if ((binding->bind_type & TMW_BIND_LSAP_MASK) == TMW_BIND_LSAP) {
        if (binding->lsap != packetBuffer[(2 * ETHER_ADDR_LEN) + 2])
            return ( FALSE);
    } else {
        /* Check for sub-ethertype and APPID */
        int appId;
        unsigned char et;
        /* Set offset based on whether tag bytes have been stripped */
        int et_offset = (2 * ETHER_ADDR_LEN) + 1;

        /* Set offset based on whether tag bytes have been stripped */
        if (packetBuffer[et_offset - 1] != 0x88)
            et_offset += 4;

        /* Get sub-ethertype from frame (only need lower due to filter) */
        /* Use offset */
        et = packetBuffer[et_offset];
        appId = packetBuffer[et_offset + 1] << 8;
        appId |= packetBuffer[et_offset + 2];

        switch (binding->bind_type & TMW_BIND_LSAP_MASK) {
        case TMW_BIND_VL_GOOSE:
            /* Removed appId mask check (was wrong at 0xC0 anyway).
             Just check for equality with lsap, because appId must have
             most significant bits = 00 */
            if ((et != 0xB8) || (binding->lsap != appId))
                return ( FALSE);
            break;
        case TMW_BIND_VL_GSE:
            /* Removed appId mask check (was wrong at 0xC0 anyway).
             Just check for equality with lsap, because appId must have
             most significant bits = 00 */
            if ((et != 0xB9) || (binding->lsap != appId))
                return ( FALSE);
            break;
        case TMW_BIND_VL_SAMPV:
            /* For sampled values, the inbound appId must have its
             most significant bits = 01.
             Fixed appId check mask to be 0x4000 (was 0x40)
             Fixed lsap check mask to be 0xBFFF (was 0x3FF) */
            if ((et != 0xBA) || (binding->lsap != (appId & 0xFFFF))) /* V10.0 (was 0xBFFF) */
                return ( FALSE);
            break;
        default:
            break; /* 0 = TMW_BIND_ANY_LSAP */
        }
    }

    switch (binding->bind_type & TMW_BIND_MAC_MASK) {
    case TMW_BIND_DIRECTED:
        if (*packetBuffer & 0x01)
            return ( FALSE);
        return ( TRUE);
    case TMW_BIND_MULTICAST:
        if (memcmp( packetBuffer, binding->multi_addr.addr, binding->multi_addr.len ) == 0)
            return ( TRUE);
        else
            return ( FALSE);
    case TMW_BIND_PROMISCUOUS:
        return ( TRUE);
    default:
        break;
    }
    return ( FALSE);
}

#ifndef IEC61850_ETH_CHANGE_FREERTOS
int ETH_receive(TMW_Packet *packet,
                TMW_Mac_Address *mac,
                TMW_DL_Context *ctx)
{
#ifdef DEBUG_ETH_DUMP
    int i;
#endif
    int len;
    ETH_Read_Packet *read_packet;
    IEEE802_3_frame *frame;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;
#ifdef DEBUG_ETH_DUMP
    unsigned char *buffer;
#endif

#ifdef DEBUG_ETH
    printf("# ETH_receive called\n" );
#endif

    if (ctx == NULL)
        return (0);

    read_packet = ETH_remove_queue( &lnxeth_context->eth_queue );
    if (read_packet == NULL)
        return (0);

    frame = (IEEE802_3_frame *) read_packet->buffer;

    packet->buffer_id = read_packet->bufferId;
    packet->buffer = read_packet->buffer;
    packet->buffer_length = ETH_size;
    packet->media = TMW_Ethernet;
    packet->eot = TRUE;
    packet->data_offset = IEEE802_3_OVERHEAD;
    packet->serial = ++(ctx->r_packet_serial);
    packet->pool = ctx->buffers;
    packet->out_time_ptr = NULL;

    /* Get size field, don't swap bytes */
    packet->data_length = len = (read_packet->buffer[12] << 8) | read_packet->buffer[13];

    /* Get incoming MAC address */
    mac->len = 6;
    memcpy( mac->addr, frame->src_addr, sizeof(frame->src_addr) );

    packet->in_time_value = read_packet->time_in;

#ifdef DEBUG_ETH_DUMP
    buffer = (unsigned char *) packet->buffer + packet->data_offset;
//    printf("# Received 802.3 Frame, len = %d, data len = %d\n",
//           (len + MHdrSize), len);

    printf("   Src MAC addr : ");
    for (i = 0; i < ETHER_ADDR_LEN; i++)
        printf("%2.2x ", mac->addr[i]);

    printf("\n  ");
    for (i = 0; i < 3; i++) {
        printf("%2.2x ",buffer[i]);
    }
    printf("\n");
//    HexDump(0, (buffer + 3), (len - 3));
#endif

    return (len);
}
#endif // IEC61850_ETH_CHANGE_FREERTOS

int ETH_send(TMW_Packet *packet,
             TMW_Mac_Address *mac,
             TMW_DL_Context *ctx)
{
    int i;
    int retval;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;
    IEEE802_3_frame *header;
    int nHeaderSize;

#ifdef ETH_SHOW
    printf("# Sending frame\n" );
#endif
    if (ctx == NULL)
        return ( FALSE);

    /* Handle Null VLAN headers for GSE */
    nHeaderSize = IEEE802_3_OVERHEAD;
    if (packet->media == TMW_VLAN)
        nHeaderSize -= 2;

    /* Handle Null VLAN headers for GSE */
    if (packet->data_offset < nHeaderSize) {
#ifdef ETH_SHOW
        printf("Packet was not allocated with header space!\n");
#endif
        return ( FALSE);
    }

    {
        unsigned char *pOctets;
        pOctets = (unsigned char *) (packet->buffer + packet->data_offset - nHeaderSize);
        for (i = 0; i < 6; i++) {
            pOctets[i] = mac->addr[i];
            pOctets[i + 6] = ctx->mac_address.addr[i];
        }
        if (packet->media != TMW_VLAN) {
            pOctets[12] = (unsigned char) ((packet->data_length & 0xFF00) >> 8);
            pOctets[13] = (unsigned char) (packet->data_length & 0xFF);
        }
        header = (IEEE802_3_frame *) pOctets;
    }

    retval = write( lnxeth_context->socket, header, packet->data_length + nHeaderSize );
    if (retval != (packet->data_length + nHeaderSize)) {
#ifdef DEBUG_ETH
        if (retval < 0)
            perror("Write failed");
        else
            printf("Write failed: attempted %d, actual %d\n",
                   packet->data_length + nHeaderSize, retval );
#endif
        TMW_FREE( ctx->buffers, (unsigned char * ) packet->buffer_id ); 
        return (FALSE);
    }
    if (packet->out_time_ptr != NULL)
        MMSd_time_stamp( packet->out_time_ptr );

    /* Attempt to free the buffer (could be locked!) */
    TMW_FREE( ctx->buffers, (unsigned char * ) packet->buffer_id ); 
    return (TRUE);
}

int ETH_send_mgmt(TMW_Packet *packet,
                  TMW_Mac_Address *mac,
                  TMW_DL_Context *ctx,
                  MMSd_time *outTime)
{
    packet->out_time_ptr = outTime;
    return (ETH_send( packet, mac, ctx ));
}

#ifndef IEC61850_ETH_CHANGE_FREERTOS
int ETH_service(TMW_DL_Context *ctx)
{
    int ret;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;
    fd_set fds;
    struct timeval timeOut;

    if (ctx == NULL)
        return ( FALSE);

    FD_ZERO( &fds );
    FD_SET( lnxeth_context->socket, &fds );
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 0;
    ret = select( lnxeth_context->socket + 1, &fds, NULL, NULL, &timeOut );
    if (ret == 0) {
        return ( TRUE);
    }

    /* Now READ the data and handle it */
    ETH_data_indication( lnxeth_context );
    return (ret);
}
#endif // IEC61850_ETH_CHANGE_FREERTOS


int ETH_can_send(int length,
                 TMW_DL_Context *ctx)
{
    fd_set fds;
    struct timeval timeOut;
    int ret;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;

    if ( length )
    {}; /* suppress warning */

    FD_ZERO( &fds );
    FD_SET( lnxeth_context->socket, &fds );
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 0;
    ret = select( lnxeth_context->socket + 1, NULL, &fds, NULL, &timeOut );
    if (ret <= 0)
        return ( FALSE);

    return (TRUE);
}

#ifndef IEC61850_ETH_CHANGE_FREERTOS
int ETH_data_indication(LNXETH_ContextPtr lnxeth_context)
{
    unsigned char *buffer;
    unsigned int decoded_length, raw_len;
    TMW_DL_Context *eth_context = (TMW_DL_Context *) lnxeth_context;
    TMW_DL_Binding *binding;

    /* Non mmap version - uses read - slower, but supported on all kernels */
#if defined(DEBUG_ETH)
    printf("packet received\n");
#endif

    buffer = (unsigned char *) TMW_ALLOC( eth_context->buffers, TMW_POOL_RECEIVE );
    if (buffer == NULL) {
#ifdef DEBUG_ETH
        printf("out of buffers\n");
#endif
        return ( FALSE);
    }
    raw_len = read( lnxeth_context->socket, buffer, ETH_size );
#ifdef RAWLEN_RECALCULATION
            /* Check for sub-ethertype and APPID */
        int appId;
        unsigned char et;

        /* Set offset based on whether tag bytes have been stripped */
        int et_offset = (2 * ETHER_ADDR_LEN) + 1;

        /* Set offset based on whether tag bytes have been stripped */
        if (buffer[et_offset - 1] != 0x88)
            et_offset += 4;

        decoded_length = (buffer[et_offset+3] << 8) | (buffer[et_offset+4]);
        decoded_length = decoded_length + et_offset + 1; 
        decoded_length = decoded_length -  14; /* skip header */
#endif  

#ifndef RAWLEN_RECALCULATION
    if (raw_len == 0) {
        TMW_FREE( lnxeth_context->gnrc.buffers, buffer );
        return ( FALSE);
    }

    decoded_length = (buffer[12] << 8) | (buffer[13]);
    if (decoded_length > 1514) {
        decoded_length = raw_len - 14; /* Skip header */
    }

    if (decoded_length <= 0) { 
        TMW_FREE( lnxeth_context->gnrc.buffers, buffer );
        return ( FALSE);
    }
#endif 
    /* Check if it matches any bindings */
    TMW_TARG_SEM_TAKE( lnxeth_context->gnrc.sem );
    binding = lnxeth_context->gnrc.goose_binds;
    while (binding != NULL) {
        if (ETH_check_bind( buffer, binding )) {
        binding->ind_function.goose_func( eth_context, buffer + IEEE802_3_OVERHEAD, decoded_length,
                                              binding->ind_param );
        }
        binding = binding->next;
    }
    TMW_TARG_SEM_GIVE( lnxeth_context->gnrc.sem );

    TMW_FREE( lnxeth_context->gnrc.buffers, buffer );

    return ( FALSE);
}
#endif //  IEC61850_ETH_CHANGE_FREERTOS

#ifdef IEC61850_ETH_CHANGE_FREERTOS
static void GetMacAddress(uint8_t* mac_address)
{
    uint8_t* mac_addr;
    DCI_Patron* patron;
    DCI_ACCESS_ST_TD access_struct;
    DCI_SOURCE_ID_TD m_source_id;
    
    patron = new DCI_Patron(false);
    m_source_id = DCI_SOURCE_IDS_Get();
    mac_addr = mac_address;
    
    access_struct.command = DCI_ACCESS_GET_RAM_CMD;
    access_struct.data_access.offset = DISABLE_VALUE;
    access_struct.source_id = m_source_id;
    access_struct.data_access.length = DISABLE_VALUE;
    access_struct.data_id = DCI_ETHERNET_MAC_ADDRESS_DCID;
    access_struct.data_access.data = mac_addr;
    patron->Data_Access( &access_struct );   
    
}

void Reverse(unsigned char *s, uint8_t length)
{
    uint8_t index1, index2;
    unsigned char t;

    if (length == DISABLE_VALUE)
    {
        return;
    }
    index1 = DISABLE_VALUE;
    index2 = length - 1;
    while (index1 < index2)
    {
        t     = s[index1];
        s[index1] = s[index2];
        s[index2] = t;
        ++index1;
        --index2;
    }
}
#endif

#ifdef IEC61850_ETH_CHANGE_FREERTOS
int ETH_service(TMW_DL_Context *ctx)
{
    int ret;
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;
    fd_set fds;
    struct timeval timeOut;
    unsigned char *buffer_write;
    ETH_Read_Packet write_packet;
    unsigned int raw_len;
    TMW_DL_Context *eth_context;

    if (ctx == NULL)
        return ( FALSE);

    FD_ZERO( &fds );
    FD_SET( lnxeth_context->socket, &fds );
    timeOut.tv_sec = SELECT_TIME_IEC61850 / 1000;
    timeOut.tv_usec = ( SELECT_TIME_IEC61850 % 1000 ) * 1000;
    ret = select( lnxeth_context->socket + 1, &fds, NULL, NULL, &timeOut );
    if (ret == 0) {
        return ( FALSE);
    }
    
    eth_context = (TMW_DL_Context *) lnxeth_context;
    buffer_write = (unsigned char *) TMW_ALLOC( eth_context->buffers, TMW_POOL_RECEIVE );
    
    if (buffer_write == nullptr) {
      unsigned char *buffer;
      
      buffer = reinterpret_cast<unsigned char*>( TMW_TARG_MALLOC( 1518 ) );
      raw_len = read( lnxeth_context->socket, buffer, ETH_size );  // dummy read to aoid overflow 
      free (buffer);
#ifdef DEBUG_ETH
      printf("out of buffers\n");
#endif
      return ( FALSE);
    }
    
    raw_len = read( lnxeth_context->socket, buffer_write, ETH_size );        
    write_packet.buffer = buffer_write;
    write_packet.raw_len = raw_len;
    
   TMW_TARG_SEM_TAKE( eth_context->sem );
   ETH_insert_queue(&write_packet, &lnxeth_context->eth_queue);
   TMW_TARG_SEM_GIVE( eth_context->sem );
    
    /* Now READ the data and handle it */
    //ETH_data_indication( lnxeth_context );
    return (ret);
}

int ETH_receive(TMW_Packet *packet,
                TMW_Mac_Address *mac,
                TMW_DL_Context *ctx)
{
    LNXETH_Context *lnxeth_context = (LNXETH_Context *) ctx;

    if (ctx == NULL)
        return (0);
    
    ETH_data_indication( lnxeth_context );
    return(1);
}

int ETH_data_indication(LNXETH_ContextPtr lnxeth_context)
{
    unsigned char *buffer;
    unsigned int decoded_length, raw_len;
    TMW_DL_Context *eth_context = (TMW_DL_Context *) lnxeth_context;
    TMW_DL_Binding *binding;
    
    /*ETH_Read_Packet write_packet;*/
    ETH_Read_Packet *read_packet;
    /*unsigned char *buffer_write;*/

    while(1)
    {
      TMW_TARG_SEM_TAKE( eth_context->sem );
      read_packet =  ETH_remove_queue(&lnxeth_context->eth_queue);
      TMW_TARG_SEM_GIVE( eth_context->sem );
      
      if (read_packet != nullptr)
      {
        buffer = read_packet->buffer;
            
#ifdef RAWLEN_RECALCULATION
        /* Check for sub-ethertype and APPID */
        /*int appId;
        unsigned char et;*/

        /* Set offset based on whether tag bytes have been stripped */
        int et_offset = (2 * ETHER_ADDR_LEN) + 1;

        /* Set offset based on whether tag bytes have been stripped */
        if (buffer[et_offset - 1] != 0x88)
            et_offset += 4;

        decoded_length = (buffer[et_offset+3] << 8) | (buffer[et_offset+4]);
        decoded_length = decoded_length + et_offset + 1; 
        decoded_length = decoded_length -  14; /* skip header */
#endif  

#ifndef RAWLEN_RECALCULATION
    
        if (raw_len == 0) {
          TMW_FREE( lnxeth_context->gnrc.buffers, buffer );
          return ( FALSE);
        }
        
        decoded_length = (buffer[12] << 8) | (buffer[13]);
        
        if (decoded_length > 1514) {
          decoded_length = raw_len - 14; /* Skip header */
        }

        if (decoded_length <= 0) {
          TMW_FREE( lnxeth_context->gnrc.buffers, buffer );
          return ( FALSE);
        }
#endif 
        /* Check if it matches any bindings */
        TMW_TARG_SEM_TAKE( lnxeth_context->gnrc.sem );
        binding = lnxeth_context->gnrc.goose_binds;
        while (binding != NULL) {
          if (ETH_check_bind( buffer, binding )) {
            binding->ind_function.goose_func( eth_context, buffer + IEEE802_3_OVERHEAD, decoded_length,
                                              binding->ind_param );
          }
          binding = binding->next;
        }
                        
        TMW_TARG_SEM_GIVE( lnxeth_context->gnrc.sem );

        TMW_FREE( lnxeth_context->gnrc.buffers, buffer );
      }
      else {
        break;
      }
    }
    return ( FALSE);
}
#endif // IEC61850_ETH_CHANGE_FREERTOS

#endif // IEC61850_ETH_C
