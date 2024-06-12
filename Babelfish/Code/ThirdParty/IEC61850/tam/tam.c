/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2000-2014 */
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
 *  Pool, FIFO, and Queue Management Routines.
 *
 *  This file should not require modification.
 */

#define TAM_C
#ifdef TAM_C

#include "sys.h" 

#include "tmwtarg.h"

 #define USE_TMW_PARSING // for TMW_Parse_mac_address
/* #define USE_TMW_DISPLAY */

/* Only one of these at a time (none for production */
/* #define TMW_TEST */
/* #define TMW_QUEUE_TEST */
/* #define TMW_DEBUG_POOL do this one in tam.h */

#include <stdio.h>
#include <stdlib.h>


#if defined(SHOW_TMW_PARSE_ERRORS)
#define INCLUDE_STDIO
#endif

#if defined(DEBUG_TMW)
#define INCLUDE_STDIO
#endif

#if defined(TMW_TEST)
#define INCLUDE_STDIO
#endif

#if defined(TMW_QUEUE_TEST)
#define INCLUDE_STDIO
#endif

#if defined(INCLUDE_STDIO)
#include <stdio.h>
#else
#include <stdlib.h>
#endif
#include <string.h>

/* ******************************************************************** */
/* Thread-safe malloc                                                   */
/* These routines allow safe operation of run-time mallocs on platforms */
/* with non-thread-safe mallocs and frees. Note that the code does not  */
/* use malloc/free during run-time, unless you enable one of these:     */
/*     MMSD_DYNAMIC_NVLS   run-time dataset creation (vendor.h)         */
/*     MMSD_MAX_AA_NVLS    datasets specific to connections (vendor.h)  */
/*     MMSD_PSEUDO_AA_RCBS rcbs specific to connections (vendor.h)      */
/*      Note that the DYNAMIC_ALLOCATION uses maloc and free only at    */
/*          startup and shutdown.                                       */
/* ******************************************************************** */

#if defined(TMW_SAFE_MALLOC)
static int globalMallocSemaphoreInited = 0;
static TMW_SEMAPHORE globalMallocSemaphore;
#undef TMW_TARG_MALLOC(a)
#undef TMW_TARG_FREE(a)

void TMW_initialize_safe_TMW_TARG_MALLOC( void )
{
    globalMallocSemaphoreInited = 1;
    TMW_TARG_SEM_CREATE( globalMallocSemaphore );
}

void TMW_delete_safe_TMW_TARG_MALLOC( void )
{
    if ( globalMallocSemaphoreInited ) {
        TMW_TARG_SEM_DELETE( globalMallocSemaphore );
        globalMallocSemaphoreInited = 0;
    }
}

char *TMW_safe_TMW_TARG_MALLOC( size_t size )
{
    char *pData;

    if ( !globalMallocSemaphoreInited )
        TMW_initialize_safe_TMW_TARG_MALLOC();

    TMW_TARG_SEM_TAKE( globalMallocSemaphore );
    pData = (char *) TMW_TARG_MALLOC( size );
    TMW_TARG_SEM_GIVE( globalMallocSemaphore );
    return( pData );
}

void TMW_safe_TMW_TARG_FREE( char *pData )
{
    if ( !globalMallocSemaphoreInited )
        TMW_initialize_safe_TMW_TARG_MALLOC();

    TMW_TARG_SEM_TAKE( globalMallocSemaphore );
    TMW_TARG_FREE( pData );
    TMW_TARG_SEM_GIVE( globalMallocSemaphore );
}

#define TMW_TARG_MALLOC(a) TMW_TMW_TARG_MALLOC(a)
#define TMW_TARG_FREE(a) TMW_TMW_TARG_FREE(a)
#endif

/* ******************************************************************** */
/* Default Tamarack buffer pool definitions                             */
/* These are used if the user doesn't specify others in the call        */
/* to TMW_init_pool()                                                   */
/* ******************************************************************** */
const TamPoolDef TMW_PoolDefs = { NULL, /* init */
                                  TMW_alloc_pool, /* allocate */
                                  TMW_room_pool, /* room */
                                  TMW_free_pool, /* free */
                                  TMW_lock_pool, /* lock */
                                  TMW_unlock_pool, /* unlock */
                                  TMW_delete_pool, /* delete pool */
                                  NULL, /* user_ptr */
                                  1517, /* data_size */
                                  14, /* data_offset */
                                  TMW_Ethernet, /* data_media */
                                };

#if defined(DEBUG_TMW) || defined(TMW_DEBUG_POOL)
void TMW_dump_queue( TamQueue *queue );
char *use_names[] = {"free", "receive", "send", "pending"};
#endif
int hexOctet(unsigned char b,
             char *buffer);
int decOctet(unsigned char b,
             char *buffer);

/* ********************************************************************** *
 *  TMW Fifo Manipulation Routines
 *
 *  To insert into fifo:
 *      index = Tam_next_insert( q )
 *      if (TMW_QUEUE_OK( index ))
 *          copy entry into entries[index]
 *      Tam_insert_fifo_index( q, index )
 *
 * To remove from fifo
 *      index = Tam_next_remove_fifo( q )
 *      if (TMW_QUEUE_OK( index ))
 *          copy entry out of entries[index]
 *      Tam_remove_fifo_index( q, index )
 */

/* ------------------------------------------------------------------------
 ---   *TMW_init_fifo()                                                 ---
 ---       Initializes a Tamarack fifo structure                        ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                 ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_init_fifo(TamFifo *fifo,
                   TamFifoIndex fifo_len,
                   TamFifoIndex max_entries)
{
    fifo->ins_index = 0;
    fifo->rem_index = 0;
    fifo->fifo_len = fifo_len;
    fifo->max_entries = max_entries;
    fifo->num_entries = 0;
}

/* ------------------------------------------------------------------------
 ---   TMW_next_ack_fifo()                                               ---
 ---       Returns the next index to be inserted in the fifo             ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                   ---
 ---   Return:                                                           ---
 ---       TamFifoIndex    - Index to be inserted next                   ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_next_ack_fifo(TamFifo *fifo)
{
    return (fifo->ins_index);
}

/* ------------------------------------------------------------------------
 ---   TMW_next_insert_fifo()                                            ---
 ---       Returns the next index to be inserted in the fifo             ---
 ---       if there is room in the fifo                                  ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                   ---
 ---   Return:                                                           ---
 ---       TamFifoIndex    - Index to be inserted next                   ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_next_insert_fifo(TamFifo *fifo)
{
    TamFifoIndex temp_index = fifo->ins_index;

    temp_index++;
    if (temp_index >= fifo->fifo_len)
        temp_index = 0;
    if ((temp_index == fifo->rem_index) || (fifo->num_entries >= fifo->max_entries))
        return ( TMW_QUEUE_FULL);
    return (fifo->ins_index);
}

/* ------------------------------------------------------------------------
 ---   TMW_insert_fifo_index()                                          ---
 ---       Updates the fifo to contain one more member                  ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                 ---
 ---       index            - Index to be inserted in the fifo          ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_insert_fifo_index(TamFifo *fifo,
                           TamFifoIndex index)
{
#if defined(DEBUG_TMW)
    if (fifo->ins_index != index)
        TMW_PRINTF("TMW_insert_fifo_index(): Invalid index\n");
    if (fifo->num_entries >= fifo->max_entries)
        TMW_PRINTF("TMW_insert_fifo_index(): max entries exceeded\n");
    if (fifo->num_entries < 0)
        TMW_PRINTF("TMW_insert_fifo_index(): num entries < 0\n");
#endif
    fifo->num_entries++;
    fifo->ins_index = (index + 1) % fifo->fifo_len;
}

/* ------------------------------------------------------------------------
 ---   TMW_next_remove_fifo()                                           ---
 ---       Returns the next index to be removed from the fifo           ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                 ---
 ---   Return:                                                           ---
 ---       TamFifoIndex    - Index of next fifo member to remove       ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_next_remove_fifo(TamFifo *fifo)
{
    TamFifoIndex temp_index = fifo->rem_index;

#if defined(DEBUG_TMW)
    if (fifo->num_entries < 0)
        TMW_PRINTF("TMW_next_remove_fifo(): num entries < 0\n");
#endif
    if ((temp_index == fifo->ins_index) || (fifo->num_entries <= 0))
        return ( TMW_QUEUE_EMPTY);
    return (temp_index);
}

/* ------------------------------------------------------------------------
 ---   TMW_remove_fifo_index()                                          ---
 ---       Updates the fifo to remove the next index                    ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                 ---
 --        index            - Index of entry to remove                   ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_remove_fifo_index(TamFifo *fifo,
                           TamFifoIndex index)
{
#if defined(DEBUG_TMW)
    if (fifo->rem_index != index)
        TMW_PRINTF("TMW_remove_fifo_index(): Invalid index\n");
#endif
    index++;
    if (index >= fifo->fifo_len)
        index = 0;
    fifo->num_entries--;
#if defined(DEBUG_TMW)
    if (fifo->num_entries < 0)
        TMW_PRINTF("TMW_remove_fifo_index(): num entries < 0\n");
#endif
    fifo->rem_index = index;
}

#if defined(DEBUG_TMW)
/* ------------------------------------------------------------------------
 ---   TMW_display_fifo_status()                                        ---
 ---       Prints the fifo contents                                     ---
 ---   Calling sequence:                                                 ---
 ---       fifo            - Pointer to fifo structure                 ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_display_fifo_status( TamFifo *fifo )
{
    char deb_buffer[100];
    sprintf(deb_buffer, "Fifo status: insert = %d, remove = %d, length = %d\n",
            fifo->ins_index,
            fifo->rem_index,
            fifo->fifo_len);
    TMW_PRINTF(deb_buffer);
}
#endif

/* ------------------------------------------------------------------------
 ---   TMW_init_pool()                                                   ---
 ---       Initializes a Tamarack pool structure                         ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_init_pool(TamPool *pool,
                   TamPoolDef *defs)
{
    pool->avail = NULL;
#if defined(TMW_DEBUG_POOL)
    pool->thread_list = NULL;
#endif
    pool->uses[TMW_POOL_FREE] = 0;
    pool->uses[TMW_POOL_RECEIVE] = 0;
    pool->uses[TMW_POOL_SEND] = 0;
    pool->uses[TMW_POOL_PENDING] = 0;
    if (defs != NULL)
        pool->def = *defs;
    else
        pool->def = TMW_PoolDefs;
    pool->num_entries = 0;
    TMW_TARG_SEM_CREATE(pool->semaphore);
}

void TMW_delete_pool(TamPool *pool)
{
    TMW_TARG_SEM_DELETE(pool->semaphore);
}

#if defined(TMW_DEBUG_POOL)
/* ------------------------------------------------------------------------
 ---   *TMW_check_pool_entry()                                           ---
 ---       Checks an entry to see if its a pool member                   ---
 ---   Calling sequence:                                                 ---
 ---       entry           - Pointer to entry to check                   ---
 ---       pool            - Pointer to pool structure                   ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
int TMW_check_pool_entry( TamPoolEntry *entry, TamPool *pool )
{
    TamPoolEntry *list;
    list = pool->thread_list;
    while (list != NULL) {
        if (list == entry)
            return( 1 );
        list = list->u.info.next_thread;
    }
    return( 0 );
}
#endif
/* ------------------------------------------------------------------------
 ---   *TMW_insert_pool()                                                ---
 ---       Insert a buffer into the buffer pool.                         ---
 ---   Calling sequence:                                                 ---
 ---       entry           - Pointer to entry to insert                  ---
 ---       pool            - Pointer to pool structure                   ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_insert_pool(TamPoolEntry *entry,
                     TamPool *pool)
{
    entry->u.info.in_use = TMW_POOL_FREE;
    entry->u.info.lock_count = 0;
    entry->u.info.entry_num = pool->num_entries;
    entry->u.info.next = pool->avail;
#if defined(TMW_DEBUG_POOL)
    entry->u.info.pool = pool;
    entry->u.info.next_thread = pool->thread_list;
#endif
    TMW_TARG_SEM_TAKE(pool->semaphore);
    pool->num_entries++;
    pool->avail = entry;
#if defined(TMW_DEBUG_POOL)
    pool->thread_list = entry;
#endif
    pool->uses[TMW_POOL_FREE]++;
    TMW_TARG_SEM_GIVE(pool->semaphore);
}

/* ------------------------------------------------------------------------
 ---   *TMW_alloc_pool()                                                 ---
 ---       Allocate a buffer to be used from the buffer pool.            ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---       usage           - Usage code for how buffer will be used      ---
 ---   Return:                                                           ---
 ---       ptr             - pointer to allocated buffer (or NULL)       ---
 ------------------------------------------------------------------------ */
unsigned char *TMW_alloc_pool(TamPool *pool,
                              TamPoolUsage usage)
{
    TamPoolEntry *entry;

    TMW_TARG_SEM_TAKE(pool->semaphore);

#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool(pool)) {
        TMW_PRINTF("TMW_alloc_pool(): TMW_check_pool failed!\n");
        TMW_TARG_SEM_GIVE( pool->semaphore );
        return( NULL );
    }
#endif

    if (pool->avail == NULL) {
#if defined(TMW_DEBUG_POOL)
        TMW_PRINTF("TMW_alloc_pool(): Out of pool entries!\n");
#endif
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return (NULL);
    }
    usage = (TamPoolUsage) (usage & TMW_POOL_USE_MASK);

    entry = pool->avail;
#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool_entry( entry, pool )) {
        TMW_dump_pool( pool );
        printf("TMW_alloc_pool(): %p is not a member of pool %p\n",
               entry, pool );
        TMW_TARG_SEM_GIVE( pool->semaphore );
        return( NULL );
    }
#endif
    pool->avail = entry->u.info.next;
    pool->uses[TMW_POOL_FREE]--;
    pool->uses[usage]++;
    entry->u.info.next = NULL;
    entry->u.info.in_use = usage;
    entry->u.info.lock_count = 0;
    TMW_TARG_SEM_GIVE(pool->semaphore);

    return (((unsigned char *) entry) + sizeof(TamPoolEntry));
}

/* ------------------------------------------------------------------------
 ---   TMW_room_pool()                                                   ---
 ---       Checks buffer available of a particular type                  ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---       usage           - Usage code for how buffers will be used     ---
 ---   Return:                                                           ---
 ---       int             - number of available buffers                 ---
 ------------------------------------------------------------------------ */
int TMW_room_pool(TamPool *pool,
                  TamPoolUsage reason)
{
    int count;

    TMW_TARG_SEM_TAKE(pool->semaphore);
    count = pool->uses[TMW_POOL_FREE];
    if (count <= 0) {
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return (0);
    }

    /* Check for deadlock situation */
    switch (reason) {
    case TMW_POOL_SEND:
        /* Must have at least one receive buffer to get ACKs */
        if (pool->uses[TMW_POOL_PENDING] > 0) {
            TMW_TARG_SEM_GIVE(pool->semaphore);
            return (count);
        }
        if (pool->uses[TMW_POOL_RECEIVE] > 0) {
            TMW_TARG_SEM_GIVE(pool->semaphore);
            return (count);
        }
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return (count - 1);
    case TMW_POOL_RECEIVE:
        /* Must have at least one send buffer to send ACKs */
        if (pool->uses[TMW_POOL_SEND] > 0) {
            TMW_TARG_SEM_GIVE(pool->semaphore);
            return (count);
        }
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return (count - 1);
    case TMW_POOL_PENDING:
        /* Must have at least one send buffer to send ACKs */
        if (pool->uses[TMW_POOL_SEND] > 0) {
            TMW_TARG_SEM_GIVE(pool->semaphore);
            return (1);
        }
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return (count - 1);
    default:
        break;
    }
    TMW_TARG_SEM_GIVE(pool->semaphore);
    return (0);
}

/* ------------------------------------------------------------------------
 ---   TMW_free_pool()                                                   ---
 ---       Free a buffer in the buffer pool to reuse.                    ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---       buffer          - Pointer to the buffer to be freed.          ---
 ---   Return:                                                           ---
 ---       none                                                          ---
 ------------------------------------------------------------------------ */
void TMW_free_pool(TamPool *pool,
                   unsigned char *buffer)
{
    TamPoolEntry *ptr;

    TMW_TARG_SEM_TAKE(pool->semaphore);

#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool(pool)) {
        TMW_PRINTF("TMW_free_pool(): TMW_check_pool failed!\n");
    }
#endif
    ptr = (TamPoolEntry *) (buffer - sizeof(TamPoolEntry));
#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool_entry( ptr, pool )) {
        TMW_dump_pool( pool );
        printf("TMW_free_pool(): %p is not a member of pool %p\n",
               ptr, pool );
        return;
    }
#endif

    if (ptr->u.info.lock_count) {
        TMW_TARG_SEM_GIVE(pool->semaphore);
        return;
    }

    pool->uses[ptr->u.info.in_use]--;
    pool->uses[TMW_POOL_FREE]++;
    ptr->u.info.in_use = TMW_POOL_FREE;
    ptr->u.info.next = pool->avail;
    pool->avail = ptr;

    TMW_TARG_SEM_GIVE(pool->semaphore);
}

/* ------------------------------------------------------------------------
 ---   TMW_lock_pool()                                                   ---
 ---       Lock a buffer in the buffer pool to resend if needed.         ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---       buffer          - Pointer to the buffer to be freed.          ---
 ---   Return:                                                           ---
 ---       VOID                                                          ---
 ------------------------------------------------------------------------ */
void TMW_lock_pool(unsigned char *buffer)
{
    TamPoolEntry *ptr;

    ptr = (TamPoolEntry *) (buffer - sizeof(TamPoolEntry));
#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool_entry( ptr, ptr->u.info.pool)) {
        TMW_dump_pool( ptr->u.info.pool );
        printf("TMW_lock_pool(): %p is not a member of pool %p\n",
               ptr, ptr->u.info.pool );
        return;
    }
#endif
    ptr->u.info.lock_count++;
}

/* ------------------------------------------------------------------------
 ---   TMW_unlock_pool()                                                 ---
 ---       Unlock a buffer after successful transmit so it can be freed. ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---       buffer          - Pointer to the buffer to be unlocked.       ---
 ---   Return:                                                           ---
 ---       VOID                                                          ---
 ------------------------------------------------------------------------ */
void TMW_unlock_pool(unsigned char *buffer)
{
    TamPoolEntry *ptr;

    ptr = (TamPoolEntry *) (buffer - sizeof(TamPoolEntry));
#if defined(TMW_DEBUG_POOL)
    if (!TMW_check_pool_entry( ptr, ptr->u.info.pool )) {
        TMW_dump_pool( ptr->u.info.pool );
        printf("TMW_unlock_pool(): %p is not a member of pool %p\n",
               ptr, ptr->u.info.pool );
        return;
    }
#endif
    if (ptr->u.info.lock_count != 0)
        ptr->u.info.lock_count--;
}

int TMW_alloc_packet(TMW_Packet *packet,
                     TamPool *pool,
                     TamPoolUsage reason)
{
    unsigned char *ptr;

    ptr = TMW_ALLOC( pool, reason );
    if (ptr == NULL) {
#if defined(DEBUG_TMW)
        TMW_PRINTF("TMW_alloc_packet(): allocate failed!\n");
#endif
        return ( FALSE);
    }
    packet->pool = pool;
    packet->buffer_id = (void *) ptr;
    packet->buffer = ptr;
    packet->buffer_length = pool->def.data_size;
    packet->data_offset = pool->def.data_offset;
    packet->data_length = 0;
    packet->media = (TMW_Media) pool->def.media;
    packet->eot = FALSE;
    packet->out_time_ptr = NULL;
    return ( TRUE);
}

int TMW_can_alloc_packet(TamPool *pool,
                         int count,
                         TamPoolUsage reason)
{
    return ( TMW_ROOM( pool, reason ) >= count);
}

void TMW_free_packet(TMW_Packet *packet,
                     TamPool *pool)
{
    TMW_FREE( pool, (unsigned char *)packet->buffer_id ); // type cast (unsigned char *)
    packet->buffer_id = NULL;
    packet->buffer = NULL;
}

void TMW_lock_packet(TMW_Packet *packet)
{
    TMW_LOCK( (TamPool * ) packet->pool, (unsigned char *)packet->buffer_id );
}

void TMW_unlock_packet(TMW_Packet *packet)
{
    TMW_UNLOCK( (TamPool * ) packet->pool, (unsigned char *)packet->buffer_id ); 
}

/* ------------------------------------------------------------------------
 ---   TMW_check_pool()                                                  ---
 ---       Checks validity of the overall buffer pool                    ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---   Return:                                                           ---
 ---       int             - 0 -> error, 1 -> pool ok                    ---
 ------------------------------------------------------------------------ */
int TMW_check_pool(TamPool *pool)
{
#if defined(TMW_DEBUG_POOL)

    int i, uses[TMW_POOL_NUM_REASONS];
    TamPoolEntry *ptr;
    char deb_buffer[100];

    TMW_TARG_SEM_TAKE( pool->semaphore );

    for (i = 0; i < TMW_POOL_NUM_REASONS; i++)
        uses[i] = 0;

    ptr = pool->thread_list;
    while (ptr != NULL) {
        uses[ptr->u.info.in_use]++;
        ptr = ptr->u.info.next_thread;
    }
    if ( (uses[TMW_POOL_FREE] != pool->uses[TMW_POOL_FREE]) ||
            (uses[TMW_POOL_RECEIVE] != pool->uses[TMW_POOL_RECEIVE]) ||
            (uses[TMW_POOL_SEND] != pool->uses[TMW_POOL_SEND]) ||
            (uses[TMW_POOL_PENDING] != pool->uses[TMW_POOL_PENDING]) ||
            ( (uses[TMW_POOL_FREE] +
               uses[TMW_POOL_RECEIVE] +
               uses[TMW_POOL_SEND] +
               uses[TMW_POOL_PENDING])
              != pool->num_entries) ) {
        sprintf(deb_buffer,
                "TMW_check_pool(): Pool allocation mismatch! (%ld entries)\n",
                pool->num_entries);
        TMW_PRINTF(deb_buffer);
        for (i = 0; i < TMW_POOL_NUM_REASONS; i++) {
            sprintf(deb_buffer, "%s found %d, was %d\n",
                    use_names[i], uses[i], pool->uses[i]);
            TMW_PRINTF(deb_buffer);
        }
        TMW_TARG_SEM_GIVE( pool->semaphore );
        return( FALSE );
    }
    TMW_TARG_SEM_GIVE( pool->semaphore );
    return( TRUE );
#else
    if (pool)
        return ( TRUE);
    return ( FALSE);
#endif
}

/* ------------------------------------------------------------------------
 ---   TMW_init_queue()                                                  ---
 ---       Initializes a queue data structure                            ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---       pool             - Pointer to allocation pool structure       ---
 ---   Return:                                                           ---
 ---       void                                                          ---
 ------------------------------------------------------------------------ */
void TMW_init_queue(TamQueue *queue,
                    TamPool *pool,
                    TamFifo *fifo,
                    TamQueuePolicy policy)
{
    queue->pool = pool;
    queue->fifo = fifo;
    queue->policy = policy;
    queue->head = queue->tail = NULL;
    queue->num_marked = queue->num_entries = 0;
    TMW_TARG_SEM_CREATE(queue->semaphore);
}

void TMW_delete_queue(TamQueue *queue)
{
    TMW_TARG_SEM_DELETE(queue->semaphore);
}
/* ------------------------------------------------------------------------
 ---   TMW_get_queue_room()                                              ---
 ---       Gets the number of entries that may be allocated and inserted ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---   Return:                                                           ---
 ---       TamFifoIndex     - Number that can be inserted.               ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_get_queue_room(TamQueue *queue)
{
    int fifo_available, pool_available;

    /* First check FIFO availability */
    fifo_available = (int) (queue->fifo->max_entries - queue->fifo->num_entries);

    /* Next check pool allocation */
#if (GS_CHANGED_081099)
    pool_available = queue->pool->uses[TMW_POOL_FREE];
#else
    pool_available = TMW_ROOM( queue->pool, TMW_POOL_SEND );
#endif

    if (fifo_available > pool_available)
        return (pool_available);
    return (fifo_available);
}

/* Compares s1 and s2:
 -1 -> s1 < s2,
 0 -> s1 == s2,
 1 -> s1 > s2 ) */
int TMW_fifo_metric(TamFifoIndex fifo_len,
                    TamFifoIndex s1,
                    TamFifoIndex s2)
{
    long diff;
    /* Two cases: either between them or outside of them */
    diff = (s2 - s1) % fifo_len;
    if (diff == 0)
        return (0);
    if (diff < (long) (fifo_len >> 1)) {
        return (1);
    }
    return (-1);
}

/* ------------------------------------------------------------------------
 ---   TMW_insert_queue()                                                ---
 ---       Inserts a queue entry into the list. If the policy is strict, ---
 ---       the insertion will fail if teh sequence number is not next.   ---
 ---       The insertion may also fail if the queue is full.             ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---       entry            - Pointer to member to be inserted.          ---
 ---   Return:                                                           ---
 ---       TamFifoIndex     - Index of the inserted member.              ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_insert_queue(TamQueue *queue,
                              TamQueueEntry *entry)
{
    TamQueueEntry **scan, *anchor, dummy;
    int cmp;

    if (queue->fifo->num_entries >= queue->fifo->max_entries)
        return ( TMW_QUEUE_FULL);

    switch (queue->policy) {
    case TMW_QueuePartial:
        /* Insert based on sequence number */
        break;
    case TMW_QueueStrict:
        if (entry->seq_num != queue->fifo->ins_index)
            return ( TMW_QUEUE_ORDER);
        /* Insert based on sequence number */
    case TMW_QueueTime:
        TMW_TARG_SEM_TAKE(queue->semaphore);
        entry->seq_num = TMW_next_insert_fifo( queue->fifo );
        if (!TMW_QUEUE_OK( entry->seq_num )) {
            TMW_TARG_SEM_GIVE(queue->semaphore);
            return ( TMW_QUEUE_FULL);
        }
        TMW_insert_fifo_index( queue->fifo, entry->seq_num );
        /* Insert based on time of arrival */
        if (queue->head == NULL) {
            queue->tail = queue->head = entry;
            entry->next = NULL;
        } else {
            if (queue->tail == NULL) {
#if defined(DEBUG_TMW)
                TMW_PRINTF("Bad tail!\n");
                queue->tail = queue->head = entry;
#endif
            } else {
                queue->tail->next = entry;
                queue->tail = entry;
                entry->next = NULL;
            }
        }
        queue->num_entries++;
        entry->mark = FALSE;
#if defined(DEBUG_TMW)
        TMW_PRINTF("After insert 1\n");
        TMW_dump_queue( queue );
#endif
        TMW_TARG_SEM_GIVE(queue->semaphore);
        return (entry->seq_num);

    default:
        return ( TMW_QUEUE_FAILURE);
    }
    /* Falls to here for ordered insertion */
    TMW_TARG_SEM_TAKE(queue->semaphore);
    if (queue->head == NULL) {
        queue->tail = queue->head = entry;
        entry->next = NULL;
    } else {
        /* Scan for first entry in the list with greater seq_num  */
        scan = &queue->head;
        anchor = &dummy;
        dummy.next = queue->head;
        while (*scan != NULL) {
            cmp = TMW_fifo_metric( queue->fifo->fifo_len, (*scan)->seq_num, entry->seq_num );
            if (cmp == 0) {
                TMW_TARG_SEM_GIVE(queue->semaphore);
                return ( TMW_QUEUE_DUPLICATE);
            }
            if (cmp < 0)
                break;
            scan = &((*scan)->next);
            anchor = anchor->next;
        }
        entry->next = *scan;
        if (anchor == queue->tail)
            queue->tail = entry;
        *scan = entry;
    }
    entry->mark = FALSE;
    queue->num_entries++;
    TMW_insert_fifo_index( queue->fifo, entry->seq_num );
#if defined(DEBUG_TMW)
    TMW_PRINTF("After insert 2\n");
    TMW_dump_queue( queue );
#endif
    TMW_TARG_SEM_GIVE(queue->semaphore);
    return (entry->seq_num);
}

/* ------------------------------------------------------------------------
 ---   TMW_remove_queue()                                                ---
 ---       Pops the next member off the end of the queue.                ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---   Return:                                                           ---
 ---       seq_num          - Index of the popped member                 ---
 ---       TamQueueEntry    - Pointer to the popped member               ---
 ------------------------------------------------------------------------ */
TamQueueEntry *TMW_remove_queue(TamQueue *queue,
                                TamFifoIndex *seq_num)
{
    TamQueueEntry *entry;

    if (queue->head == NULL)
        return (NULL);
    TMW_TARG_SEM_TAKE(queue->semaphore);

    /* V7.23 - bug fix - added this test also AFTER the semaphore check */
    if (queue->head == NULL) {
        TMW_TARG_SEM_GIVE(queue->semaphore);
        return (NULL);
    }
    entry = queue->head;
    queue->head = entry->next;
    if (queue->tail == entry)
        queue->tail = entry->next;
    *seq_num = entry->seq_num;
    queue->num_entries--;
    TMW_remove_fifo_index( queue->fifo, entry->seq_num );
#if defined(DEBUG_TMW)
    TMW_PRINTF("After remove\n");
    TMW_dump_queue( queue );
#endif
    TMW_TARG_SEM_GIVE(queue->semaphore);
    return (entry);
}

/* ------------------------------------------------------------------------
 ---   TMW_clean_queue()                                                 ---
 ---       Purges members up to a sequence number                        ---
 ---       This routine continues to return the next member (up to a     ---
 ---       sequence number).  It is like TMW_next_queue, but stops eary  ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---       seq_num          - sequence number to purge                   ---
 ---   Return:                                                           ---
 ---       count            - Number of members purged                   ---
 ------------------------------------------------------------------------ */
TamQueueEntry *TMW_clean_queue(TamQueue *queue,
                               TamFifoIndex seq_num)
{
    TamFifoIndex temp_num;
    int cmp;

    if (queue->head == NULL)
        return (NULL);
    cmp = TMW_fifo_metric( queue->fifo->fifo_len, queue->head->seq_num, seq_num );
    if (cmp <= 0)
        return (TMW_remove_queue( queue, &temp_num ));
    return (NULL);
}

/* ------------------------------------------------------------------------
 ---   TMW_first_queue()                                                 ---
 ---       Retrieves the first member of a queue (FIFO ordering)         ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---   Return:                                                           ---
 ---       TamQueueEntry    - Pointer to the first member                ---
 ------------------------------------------------------------------------ */
TamQueueEntry *TMW_first_queue(TamQueue *queue)
{
    return (queue->head);
}

/* ------------------------------------------------------------------------
 ---   TMW_next_queue()                                                  ---
 ---       Retrieves the next member of a queue (FIFO ordering)          ---
 ---   Calling sequence:                                                 ---
 ---       TamQueueEntry    - Pointer to the current member              ---
 ---   Return:                                                           ---
 ---       TamQueueEntry    - Pointer to the next member                 ---
 ------------------------------------------------------------------------ */
TamQueueEntry *TMW_next_queue(TamQueueEntry *current)
{
    return (current->next);
}

/* ------------------------------------------------------------------------
 ---   TMW_mark_queue_entry()                                            ---
 ---       Marks a member of the queue.  This is intended to mark a      ---
 ---       member of a queue as having been (for instance) transmitted.  ---
 ---                                                                     ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---       TamQueueEntry    - Pointer to the member to mark              ---
 ---   Return:                                                           ---
 ---       void                                                          ---
 ------------------------------------------------------------------------ */
void TMW_mark_queue_entry(TamQueue *queue,
                          TamQueueEntry *entry)
{
    queue->num_marked++;
    entry->mark = TRUE;
}

/* ------------------------------------------------------------------------
 ---   TMW_unmark_queue()                                                ---
 ---       Makes all members of the queue unmarked.                      ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---   Return:                                                           ---
 ---       void                                                          ---
 ------------------------------------------------------------------------ */
void TMW_unmark_queue(TamQueue *queue)
{
    TamQueueEntry *entry;

    entry = queue->head;
    while (entry != NULL) {
        if (entry->mark) {
            if (queue->num_marked > 0)
                queue->num_marked--;
#if defined(DEBUG_TMW)
            else {
                TMW_PRINTF("TMW_unmark_queue(): bad mark counter!\n");
            }
#endif
            entry->mark = FALSE;
        }
        entry = entry->next;
    }
}

/* ------------------------------------------------------------------------
 ---   TMW_num_marked_queue()                                            ---
 ---       Retrieves the number of marked entries in the queue           ---
 ---   Calling sequence:                                                 ---
 ---       queue            - Pointer to queue structure                 ---
 ---   Return:                                                           ---
 ---       TamFifoIndex     - Number of unmarked members                 ---
 ------------------------------------------------------------------------ */
TamFifoIndex TMW_num_marked_queue(TamQueue *queue)
{
    return (queue->num_marked);
}

/* ------------------------------------------------------------------------
 ---   TMW_next_unmarked_queue()                                         ---
 ---       Returns next sequential non-marked entry                      ---
 ---   Calling sequence:                                                 ---
 ---       current          - Pointer to queue entry                     ---
 ---   Return:                                                           ---
 ---       TamQueueEntry *  - pointer to next unmarked entry             ---
 ------------------------------------------------------------------------ */
TamQueueEntry *TMW_next_unmarked_queue(TamQueueEntry *current)
{
    while (current != NULL) {
        current = current->next;
        if (current == NULL)
            return (NULL);
        if (current->mark)
            return (current);
    }
    return (NULL);
}

#if defined(USE_TMW_PARSING)
unsigned char *TMW_parse_octet( unsigned char *buff, unsigned char *octet )
{
    unsigned char digit1, digit2, c;

    if (buff == NULL)
        return( NULL );
    while (*buff == ' ')
        buff++;
    if ((*buff == 0) || (*buff == '\n'))
        return( NULL );

    c = *buff;
    if ( (c >= '0') && (c <= '9') )
        digit1 = (unsigned char) (c - '0');
    else if (( c >= 'a') && (c <= 'f'))
        digit1 = (unsigned char) (c - 'a' + 10);
    else if (( c >= 'A') && (c <= 'F'))
        digit1 = (unsigned char) (c - 'A' + 10);
    else {
#if defined(SHOW_TMW_PARSE_ERRORS)
        char deb_buffer[50];
        sprintf(deb_buffer, "Bad hex value %c in %s\n", c, buff);
        TMW_PRINTF(deb_buffer);
#endif
        return( NULL );
    }
    buff++;
    c = *buff;
    if ( (c >= '0') && (c <= '9'))
        digit2 = (unsigned char) (c - '0');
    else if (( c >= 'a') && (c <= 'f'))
        digit2 = (unsigned char) (c - 'a' + 10);
    else if (( c >= 'A') && (c <= 'F'))
        digit2 = (unsigned char) (c - 'A' + 10);
    else {
#if defined(SHOW_TMW_PARSE_ERRORS)
        char deb_buffer[50];
        sprintf(deb_buffer, "Bad hex value %c in %s\n", c, buff);
        TMW_PRINTF(deb_buffer);
#endif
        return( NULL );
    }
    *octet = (unsigned char) ((digit1 << 4) | digit2);
    buff++;
    return( buff );
}

void TMW_parse_mac_address( unsigned char *buff, TMW_Mac_Address *dest )
{
    unsigned char i;
    unsigned char octet, *ptr;

    ptr = (unsigned char *) buff;
    ptr = TMW_parse_octet( ptr, &octet );
    i = 0;
    while (ptr != NULL) {
        if (i >= TMW_MAX_MAC_ADDRESS) {
#if defined(SHOW_TMW_PARSE_ERRORS)
            char deb_buffer[100];
            sprintf(deb_buffer, "Address too long: %s, truncated\n", buff);
            TMW_PRINTF(deb_buffer);
#endif
            dest->len = i;
            return;
        }
        dest->addr[i++] = (unsigned char) octet;
        ptr = TMW_parse_octet( ptr, &octet );
    }
    dest->len = i;
    return;
}

void TMW_parse_address( unsigned char *buff, TMW_Address *dest )
{
    unsigned char i;
    unsigned char octet, *ptr;

    ptr = buff;
    ptr = TMW_parse_octet( ptr, &octet );
    i = 0;
    while (ptr != NULL) {
        if (i >= TMW_MAX_ADDRESS) {
#if defined(SHOW_TMW_PARSE_ERRORS)
            char deb_buffer[100];
            sprintf(deb_buffer, "Address too long: %s, truncated\n", buff);
            TMW_PRINTF(deb_buffer);
#endif
            dest->len = i;
            return;
        }
        dest->addr[i++] = octet;
        ptr = TMW_parse_octet( ptr, &octet );
    }
    dest->len = i;
    return;
}

void TMW_parse_dot_address( unsigned char *buff, TMW_Address *dest )
{
    unsigned char i;
    unsigned char *ptr;

    ptr = buff;
    dest->len = 4;
    for (i = 0; i < 4; i++) {
        if (ptr == NULL)
            dest->addr[i] = 0;
        else {
            dest->addr[i] = (unsigned char) atoi( (char *) ptr );
            ptr = (unsigned char *) strchr( (char *) ptr, '.');
            if (ptr == NULL)
                return;
            ptr++;
        }
    }
    dest->len = 4;
    return;
}

int hexOctet( unsigned char b, char *buffer )
{
    int nibble;

    nibble = (b >> 4) & 0x0F;
    if (nibble <= 9)
        *buffer = (unsigned char) (nibble + '0');
    else
        *buffer = (unsigned char) ((nibble - 10) + 'a');
    buffer++;
    nibble = b & 0x0F;
    if (nibble <= 9)
        *buffer = (unsigned char) (nibble + '0');
    else
        *buffer = (unsigned char) ((nibble - 10) + 'a');
    buffer++;
    *buffer = 0;
    return( 2 );
}

int decOctet( unsigned char b, char *buffer )
{
    int digs, divisor, val;

    digs = 0;
    divisor = 100;
    while (divisor != 0) {
        if (b >= divisor) {
            val = (b / divisor);
            buffer[digs] = (unsigned char) (val + '0');
            b = (unsigned char) (b - (val * divisor));
            digs++;
        } else {
            if (digs || (divisor == 1)) {
                buffer[digs] = '0';
                digs++;
            }
        }
        divisor = divisor / 10;
    }
    buffer[digs] = 0;
    return( digs );
}

TMW_Media TMW_parse_media( unsigned char *buff )
{
    if (strcmp((char *) buff, "ethernet") == 0)
        return( TMW_Ethernet );
    if (strcmp((char *) buff, "adlc") == 0)
        return( TMW_Adlc );
    return( TMW_Other );
}


int TMW_parseObjId( char *str, int *oid )
{
    int count = 0;
    int val;

    oid[ 0 ] = 0;
    while ( *str != 0 ) {
        /* Skip whitespace */
        while (( *str == ' ' ) || (*str == '\t'))
            str++;

        if (( *str >= '0' ) && (*str <= '9')) {
            val = 0;
            while (( *str >= '0' ) && (*str <= '9')) {
                val = val * 10 + (int) ( *str - '0' );
                str++;
            }
            switch ( count ) {
            case 0:
                oid[ 1 ] = val;
                if ( val >= 30 ) {
                    /* For backward compatibility (we used to make the user
                     do the folding of the first two integers).  We can sense
                     here if it was already folded or not. */
                    count++;
                    oid[0] = 1;
                }
                break;
            case 1:
                oid[ 1 ] = (oid[1] * 40) + val;
                oid[ 0 ] = 1;
                break;
            default:
                oid[ oid[0] + 1 ] = val;
                oid[ 0 ] = oid[ 0 ] + 1;
                break;
            }
            count++;
        } else
            return( 0 );

        /* Skip whitespace */
        while (( *str == ' ' ) || (*str == '\t'))
            str++;

        /* If there's more keep going, else quit or die */
        if ( *str == ',' )
            str++;
        else if ( *str != 0 )
            return( 0 );
    }
    return( 1 );
}
#endif

#if defined(USE_TMW_DISPLAY)
int TMW_addr2hex( TMW_Address *tam_addr, char *buffer, int len )
{
    int i, cursor;

    /* Needs 2 per byte, separators, plus null terminator */
    if (len < (tam_addr->len * 3))
        return( -1 );

    cursor = 0;
    for (i = 0; i < tam_addr->len; i++) {
        if (i)
            buffer[cursor++] = ' ';
        cursor += hexOctet( tam_addr->addr[i], &(buffer[cursor]));
    }
    return( cursor );
}

int TMW_addr2dot( TMW_Address *tam_addr, char *buffer, int len )
{
    int i, cursor;

    /* Needs 3 per byte, separators, plus null terminator */
    if (len < (tam_addr->len * 4))
        return( -1 );

    cursor = 0;
    for (i = 0; i < tam_addr->len; i++) {
        if (i)
            buffer[cursor++] = '.';
        cursor += decOctet( tam_addr->addr[i], &(buffer[cursor]));
    }
    return( cursor );
}

#define STORE_OBJ_INT( VAL, STR, BUFLEN ) \
{ \
    int len; \
    char temp[20]; \
    sprintf( temp, "%d", VAL ); \
    len = (int) strlen( temp ); \
    if ( len + 1 >= BUFLEN ) \
        return; \
    strcpy( STR, temp ); \
    STR += len; \
    BUFLEN -= len; \
}

#define STORE_OBJ_CHAR( CH, STR, BUFLEN ) \
{ \
    if ( BUFLEN < 2 ) \
        return; \
    *(STR) = CH; \
    STR += 1; \
    BUFLEN -= 1; \
}

void TMW_textObjId( int *oid, char *str, int bufLen )
{
    int index;
    int i1, i2;

    *str = 0;
    if ( oid[0] == 0 )
        return;

    i1 = oid[ 1 ] / 40;
    i2 = oid[ 1 ] - (i1 * 40);

    STORE_OBJ_INT( i1, str, bufLen );
    STORE_OBJ_CHAR( ',', str, bufLen );
    STORE_OBJ_INT( i2, str, bufLen );

    for ( index = 1; index < oid[ 0 ]; index++ ) {
        STORE_OBJ_CHAR( ',', str, bufLen );
        STORE_OBJ_INT( oid[index + 1], str, bufLen );
    }
}
#undef STORE_OBJ_INT
#undef STORE_OBJ_CHAR
#endif

#if defined(DEBUG_TMW) || defined(TMW_DEBUG_POOL)
void TMW_dump_queue( TamQueue *queue )
{
    char deb_buffer[100];
    TamQueueEntry *entry;

    sprintf(deb_buffer, "Dump of queue - policy: %d, marked: %ld\n",
            queue->policy, queue->num_marked);
    TMW_PRINTF(deb_buffer);
    entry = queue->head;
    while (entry != NULL) {
        sprintf(deb_buffer, "  %ld: %p mark %d\n",
                entry->seq_num, entry, entry->mark);
        TMW_PRINTF(deb_buffer);
        entry = entry->next;
    }
    if (queue->tail == NULL)
        TMW_PRINTF(" Tail is NULL\n");
    else {
        sprintf(deb_buffer, " Tail is %ld: %p, %d\n",
                queue->tail->seq_num, queue->tail, queue->tail->mark);
        TMW_PRINTF(deb_buffer);
    }
}

/* ------------------------------------------------------------------------
 ---   TMW_dump_pool()                                                   ---
 ---       Prints out the contents of the buffer pool                    ---
 ---   Calling sequence:                                                 ---
 ---       pool            - Pointer to pool structure                   ---
 ---   Return:                                                           ---
 ---       VOID                                                          ---
 ------------------------------------------------------------------------ */
void TMW_dump_pool(TamPool *pool)
{
    TamPoolEntry *ptr;
    char deb_buffer[100];

    sprintf(deb_buffer, "Pool: free:%d, receive:%d, send:%d, pend:%d, total: %ld\n",
            pool->uses[TMW_POOL_FREE],
            pool->uses[TMW_POOL_SEND],
            pool->uses[TMW_POOL_RECEIVE],
            pool->uses[TMW_POOL_PENDING],
            pool->num_entries);
    TMW_PRINTF(deb_buffer);
    ptr = pool->avail;
    TMW_PRINTF("  Avail list:\n");
    while (ptr != NULL) {
        sprintf(deb_buffer, " %8ld: addr: %p, in_use: %s, lock: %d\n",
                ptr->u.info.entry_num,
                ptr,
                use_names[ptr->u.info.in_use],
                (int) ptr->u.info.lock_count);
        TMW_PRINTF(deb_buffer);
        ptr = ptr->u.info.next;
    }
    TMW_PRINTF("  Threaded list:\n");
    ptr = pool->thread_list;
    while (ptr != NULL) {
        sprintf(deb_buffer, " %8ld: addr: %p, in_use: %s, lock: %d\n",
                ptr->u.info.entry_num,
                ptr,
                use_names[ptr->u.info.in_use],
                (int) ptr->u.info.lock_count);
        TMW_PRINTF(deb_buffer);
        ptr = ptr->u.info.next_thread;
    }
}
#endif


#if defined(TMW_TEST)
/* ------------------------------------------------------------------------
 ---   main()                                                            ---
 ---       Tests the buffer and fifo mechanisms                         ---
 ---   Calling sequence:                                                 ---
 ---       VOID                                                          ---
 ---   Return:                                                           ---
 ---       VOID                                                          ---
 ------------------------------------------------------------------------ */
#define TEST_DATA_SIZE   100
#define TEST_POOL_SIZE   100
void main(void)
{
    const TamPoolDef my_pool_defs = {
        TMW_alloc_pool, /* allocate */
        TMW_room_pool, /* room */
        TMW_free_pool, /* free */
        TMW_lock_pool, /* lock */
        TMW_unlock_pool, /* unlock */
        101, /* data_size */
        0, /* data_offset */
        TMW_Ethernet, /* data_media */
    };

    /* To make pool entries, just put the pool header in front of the data */
    typedef struct MyData {
        TamPoolEntry pool_data;
        unsigned char buffer[TEST_DATA_SIZE];
    } MyData;

    /* Pool allocation for test */
    MyData my_fifo_data[TEST_POOL_SIZE]; /* Allocation of pool entries */
    TamPool pool; /* Pool status */

    /* Fifo allocation for test */
    TamFifo fifo;
    unsigned char *my_data[TEST_POOL_SIZE];

    /* Local vars */
    TamFifoIndex index;
    int i, j;
    char *ptr;

    /* First initialize the entire pool */
    TMW_init_pool( &pool, (TamPoolDef *) &my_pool_defs );
    for (i = 0; i < TEST_POOL_SIZE; i++)
        TMW_insert_pool( (TamPoolEntry *) &my_fifo_data[i], &pool );

    /* Now check to see that the pool is OK */
    if (!TMW_check_pool( &pool )) {
#if defined(DEBUG_TMW)
        TMW_dump_pool( &pool );
#else
        printf("TMW_check_pool failed!\n");
#endif
        return;
    }

    /* Initialize fifo */
    TMW_init_fifo( &fifo, TEST_POOL_SIZE + 1, TEST_POOL_SIZE + 1 );

    /* Allocate all pool entries, put in fifo */
    for (i = 0; i < TEST_POOL_SIZE; i++) {
        index = TMW_next_insert_fifo( &fifo );
        if (!TMW_QUEUE_OK( index )) {
            printf("Fifo insertion failure: %d, at request %d\n", index, i);
            return;
        }
        my_data[index] = TMW_alloc_pool( &pool, TMW_POOL_RECEIVE );
        if (my_data[index] == NULL) {
            printf("Pool allocation failure, at request %d\n", i);
            return;
        }
        ptr = my_data[index];
        for (j = 0; j < TEST_DATA_SIZE; j++) {
            ptr[j] = (i & 0xFF);
        }

        /* Finally, update fifo with new entry */
        TMW_insert_fifo_index( &fifo, index );
    }
    /* Now check to see that the pool is OK */
    if (!TMW_check_pool( &pool )) {
#if defined(DEBUG_TMW)
        TMW_dump_pool( &pool );
#else
        printf("TMW_check_pool failed!\n");
#endif
        return;
    }
    /* Now pop them off the fifo and free them */
    for (i = 0; i < TEST_POOL_SIZE; i++) {
        index = TMW_next_remove_fifo( &fifo );
        if (index != i) {
            printf("Fifo remove error: was %d, should be %d\n", index, i);
            return;
        }
        ptr = my_data[index];
        for (j = 0; j < TEST_DATA_SIZE; j++) {
            if (ptr[j] != (i & 0xFF)) {
                printf("Data error, at entry %d\n", i);
                return;
            }
        }
        TMW_remove_fifo_index( &fifo, index );
        TMW_free_pool( &pool, my_data[index] );
    }
    /* Now check to see that the pool is OK */
    if (!TMW_check_pool( &pool )) {
#if defined(DEBUG_TMW)
        TMW_dump_pool( &pool );
#else
        printf("TMW_check_pool failed!\n");
#endif
        return;
    }
    TMW_PRINTF("All tests completed\n");
}
#endif

#if defined(TMW_QUEUE_TEST)
#define TEST_PACKET_SIZE        100
#define TEST_NUM_PACKETS        100
#define TEST_NUM_QUEUE_ENTRIES  51
#define TEST_NUM_CONNS          10
#define TEST_SEQ_RANGE          32768
#define TEST_MAX_QUEUE_ENTRIES  5
#define TEST_MAX_INPUTS         5
#define TEST_MAX_LOOPS          100

/* ------------------------------------------------------------------------
 ---   main()                                                            ---
 ---       Tests the queue mechanisms                                    ---
 ---   Calling sequence:                                                 ---
 ---       VOID                                                          ---
 ---   Return:                                                           ---
 ---       VOID                                                          ---
 ------------------------------------------------------------------------ */

/* To make pool entries, just put the pool header in front of the data */
typedef struct PacketPoolEntry {
    TamPoolEntry pool_data; /* Must appear first for pool entries */
    unsigned char buffer[TEST_PACKET_SIZE];
} PacketPoolEntry;

typedef struct MyQueueType {
    TamQueueEntry queue_data; /* Must appear first for queue entries */
    TMW_Packet packet;
} MyQueueType;

typedef struct QueuePoolEntry {
    TamPoolEntry queue_data; /* Must appear first for pool entries */
    MyQueueType queue_entry;
} QueuePoolEntry;

typedef struct MyQueue {
    TamFifo fifo;
    TamQueue queue;
} MyQueue;

typedef struct Connection {
    MyQueue input_q;
    MyQueue output_q;
    unsigned long test_seq_num_in;
    unsigned long test_seq_num_out;
    unsigned long test_next_seq_num_out;
} Connection;

int make_input_data( Connection *conn, TamPool *packet_pool )
{
    MyQueueType *q_entry;
    unsigned long temp_num;

    q_entry = (MyQueueType *) TMW_alloc_pool( conn->input_q.queue.pool,
              TMW_POOL_SEND );
    if (q_entry == NULL) {
        printf("make_input_data():  queue allocation failed\n");
        return( FALSE );
    }
    if (!TMW_alloc_packet( &q_entry->packet, packet_pool, TMW_POOL_SEND )) {
        printf("make_input_data():  packet allocation failed\n");
        return( FALSE );
    }
    q_entry->queue_data.seq_num = conn->test_seq_num_in;
    conn->test_seq_num_in = (conn->test_seq_num_in + 1) % TEST_SEQ_RANGE;
    temp_num = TMW_insert_queue( &conn->input_q.queue,
                                 (TamQueueEntry *) q_entry );
    if (temp_num != q_entry->queue_data.seq_num) {
        printf("make_input_data(): insertion failed: was %ld should be: %ld\n",
               temp_num, q_entry->queue_data.seq_num);
        return( FALSE );
    }
    return( TRUE );
}

int loopback_data( Connection *conn, TamPool *packet_pool )
{
    MyQueueType *q_entry;
    TamFifoIndex in_seq_num, out_seq_num;

    q_entry = (MyQueueType *) TMW_remove_queue( &conn->input_q.queue,
              &in_seq_num );
    if (in_seq_num != conn->test_seq_num_out) {
        printf("loopback_data(): bad in seq - was %ld, should be %ld\n",
               in_seq_num, conn->test_seq_num_out );
        return( FALSE );
    }
    out_seq_num = TMW_insert_queue( &conn->output_q.queue,
                                    (TamQueueEntry *) q_entry );
    if (in_seq_num != out_seq_num) {
        printf("loopback_data(): bad out seq - was %ld, should be %ld\n",
               in_seq_num, out_seq_num );
        return( FALSE );
    }
    conn->test_seq_num_out = (conn->test_seq_num_out + 1) % TEST_SEQ_RANGE;
    return( TRUE );
}

int send_output_data( Connection *conn, TamPool *packet_pool )
{
    MyQueueType *q_entry;
    TamFifoIndex seq_num, out_seq_num;

    q_entry = (MyQueueType *) TMW_remove_queue( &(conn->output_q.queue),
              &seq_num );
    if (q_entry == NULL) {
        printf("send_out_data(): remove_queue returned NULL\n");
        return( FALSE );
    }
    if (seq_num != conn->test_next_seq_num_out) {
        printf("send_out_data(): bad out seq - was %ld, should be %ld\n",
               seq_num, conn->test_next_seq_num_out );
        return( FALSE );
    }
    conn->test_next_seq_num_out =
        (conn->test_next_seq_num_out + 1) % TEST_SEQ_RANGE;
    TMW_free_pool( conn->output_q.queue.pool, (unsigned char *) q_entry );
    TMW_free_packet( (TMW_Packet *) &(q_entry->packet), packet_pool );
    return( TRUE );
}

void main( void )
{
    int i, conn, p;

    /* Defines default definitions for a pool of buffers */
    const TamPoolDef packet_pool_def = {
        TMW_alloc_pool, /* allocate */
        TMW_room_pool, /* room */
        TMW_free_pool, /* free */
        TMW_lock_pool, /* lock */
        TMW_unlock_pool, /* unlock */
        101, /* data_size */
        0, /* data_offset */
        TMW_Ethernet, /* data_media */
    };

    const TamPoolDef queue_pool_def = {
        TMW_alloc_pool, /* allocate */
        TMW_room_pool, /* room */
        TMW_free_pool, /* free */
        TMW_lock_pool, /* lock */
        TMW_unlock_pool, /* unlock */
        101, /* data_size */
        0, /* data_offset */
        TMW_Ethernet, /* data_media */
    };

    PacketPoolEntry packets[TEST_NUM_PACKETS];
    QueuePoolEntry queue_entries[TEST_NUM_QUEUE_ENTRIES];

    TamPool packet_pool;
    TamPool queue_pool;

    Connection connections[TEST_NUM_CONNS];

    /* Initialize all pools */
    TMW_init_pool( &packet_pool, (TamPoolDef *) &packet_pool_def );
    for (i = 0; i < TEST_NUM_PACKETS; i++)
        TMW_insert_pool( (TamPoolEntry *) &packets[i], &packet_pool );

    TMW_init_pool( &queue_pool, (TamPoolDef *) &queue_pool_def );
    for (i = 0; i < TEST_NUM_QUEUE_ENTRIES; i++)
        TMW_insert_pool( (TamPoolEntry *) &queue_entries[i], &queue_pool );

    /* Now initialize the queues */
    for (conn = 0; conn < TEST_NUM_CONNS; conn++) {
        /* Each connection has its own i/o queues, drawing from a
         common pool */

        /* First initialze the FIFO for each queue */

        /* This implies that sequence numbers cycle mod TEST_SEQ_RANGE,
         and that each queue can contain at most TEST_MAX_QUEUE_ENTRIES
         at any one time. */
        TMW_init_fifo( &connections[conn].input_q.fifo, TEST_SEQ_RANGE,
                       TEST_MAX_QUEUE_ENTRIES );

        TMW_init_fifo( &connections[conn].output_q.fifo, TEST_SEQ_RANGE,
                       TEST_MAX_QUEUE_ENTRIES );

        /* Now initialize the actual queues */
        TMW_init_queue( &connections[conn].input_q.queue,
                        &queue_pool,
                        &connections[conn].input_q.fifo,
                        TMW_QueueStrict ); /* Entries must be in seq */

        TMW_init_queue( &connections[conn].output_q.queue,
                        &queue_pool,
                        &connections[conn].output_q.fifo,
                        TMW_QueueStrict ); /* Entries must be in seq */

        /* Strictly for checking the test results */
        connections[conn].test_seq_num_in = 0L;
        connections[conn].test_seq_num_out = 0L;
        connections[conn].test_next_seq_num_out = 0L;
    }

    /* Simulate communications */
    for (i = 0; i < TEST_MAX_LOOPS; i++) {
        /* Simulate input traffic */
        for (conn = 0; conn < TEST_NUM_CONNS; conn++) {
            /* Add as many inputs as possible */
            for (p = 0; p < TEST_MAX_INPUTS; p++) {
                if (!make_input_data( &connections[conn], &packet_pool )) {
                    printf("make_input failed at loop:%d, conn:%d, p:%d\n",
                           i, conn, p );
                    return;
                }
            }
        }

        /* Now transfer them to the output queues */
        for (conn = 0; conn < TEST_NUM_CONNS; conn++) {
            /* Add as many inputs as possible */
            for (p = 0; p < TEST_MAX_INPUTS; p++) {
                if (!loopback_data( &connections[conn], &packet_pool )) {
                    printf("loopback failed at loop:%d, conn:%d, p:%d\n",
                           i, conn, p );
                    return;
                }
            }
        }

        /* Now pop them all off and free them up */
        for (conn = 0; conn < TEST_NUM_CONNS; conn++) {
            /* Add as many inputs as possible */
            for (p = 0; p < TEST_MAX_INPUTS; p++) {
                if (!send_output_data( &connections[conn], &packet_pool )) {
                    printf("send_output failed at loop:%d, conn:%d, p:%d\n",
                           i, conn, p );
                    return;
                }
            }
        }
    }
    printf("Queue test completed successfully\n");
}
#endif

#define INITFCS         0xFFFF  /* Initial FCS value */
#define GOODFCS         0xF0B8  /* Final good FCS value */

/* -----------------------------------------------------------------------
 --- Long FCS lookup table                                            -----
 ----------------------------------------------------------------------- */
static unsigned int TMW_61850_DL_crc16_fcstab[256] = { 0x0000,
                                                       0x1189,
                                                       0x2312,
                                                       0x329b,
                                                       0x4624,
                                                       0x57ad,
                                                       0x6536,
                                                       0x74bf,
                                                       0x8c48,
                                                       0x9dc1,
                                                       0xaf5a,
                                                       0xbed3,
                                                       0xca6c,
                                                       0xdbe5,
                                                       0xe97e,
                                                       0xf8f7,
                                                       0x1081,
                                                       0x0108,
                                                       0x3393,
                                                       0x221a,
                                                       0x56a5,
                                                       0x472c,
                                                       0x75b7,
                                                       0x643e,
                                                       0x9cc9,
                                                       0x8d40,
                                                       0xbfdb,
                                                       0xae52,
                                                       0xdaed,
                                                       0xcb64,
                                                       0xf9ff,
                                                       0xe876,
                                                       0x2102,
                                                       0x308b,
                                                       0x0210,
                                                       0x1399,
                                                       0x6726,
                                                       0x76af,
                                                       0x4434,
                                                       0x55bd,
                                                       0xad4a,
                                                       0xbcc3,
                                                       0x8e58,
                                                       0x9fd1,
                                                       0xeb6e,
                                                       0xfae7,
                                                       0xc87c,
                                                       0xd9f5,
                                                       0x3183,
                                                       0x200a,
                                                       0x1291,
                                                       0x0318,
                                                       0x77a7,
                                                       0x662e,
                                                       0x54b5,
                                                       0x453c,
                                                       0xbdcb,
                                                       0xac42,
                                                       0x9ed9,
                                                       0x8f50,
                                                       0xfbef,
                                                       0xea66,
                                                       0xd8fd,
                                                       0xc974,
                                                       0x4204,
                                                       0x538d,
                                                       0x6116,
                                                       0x709f,
                                                       0x0420,
                                                       0x15a9,
                                                       0x2732,
                                                       0x36bb,
                                                       0xce4c,
                                                       0xdfc5,
                                                       0xed5e,
                                                       0xfcd7,
                                                       0x8868,
                                                       0x99e1,
                                                       0xab7a,
                                                       0xbaf3,
                                                       0x5285,
                                                       0x430c,
                                                       0x7197,
                                                       0x601e,
                                                       0x14a1,
                                                       0x0528,
                                                       0x37b3,
                                                       0x263a,
                                                       0xdecd,
                                                       0xcf44,
                                                       0xfddf,
                                                       0xec56,
                                                       0x98e9,
                                                       0x8960,
                                                       0xbbfb,
                                                       0xaa72,
                                                       0x6306,
                                                       0x728f,
                                                       0x4014,
                                                       0x519d,
                                                       0x2522,
                                                       0x34ab,
                                                       0x0630,
                                                       0x17b9,
                                                       0xef4e,
                                                       0xfec7,
                                                       0xcc5c,
                                                       0xddd5,
                                                       0xa96a,
                                                       0xb8e3,
                                                       0x8a78,
                                                       0x9bf1,
                                                       0x7387,
                                                       0x620e,
                                                       0x5095,
                                                       0x411c,
                                                       0x35a3,
                                                       0x242a,
                                                       0x16b1,
                                                       0x0738,
                                                       0xffcf,
                                                       0xee46,
                                                       0xdcdd,
                                                       0xcd54,
                                                       0xb9eb,
                                                       0xa862,
                                                       0x9af9,
                                                       0x8b70,
                                                       0x8408,
                                                       0x9581,
                                                       0xa71a,
                                                       0xb693,
                                                       0xc22c,
                                                       0xd3a5,
                                                       0xe13e,
                                                       0xf0b7,
                                                       0x0840,
                                                       0x19c9,
                                                       0x2b52,
                                                       0x3adb,
                                                       0x4e64,
                                                       0x5fed,
                                                       0x6d76,
                                                       0x7cff,
                                                       0x9489,
                                                       0x8500,
                                                       0xb79b,
                                                       0xa612,
                                                       0xd2ad,
                                                       0xc324,
                                                       0xf1bf,
                                                       0xe036,
                                                       0x18c1,
                                                       0x0948,
                                                       0x3bd3,
                                                       0x2a5a,
                                                       0x5ee5,
                                                       0x4f6c,
                                                       0x7df7,
                                                       0x6c7e,
                                                       0xa50a,
                                                       0xb483,
                                                       0x8618,
                                                       0x9791,
                                                       0xe32e,
                                                       0xf2a7,
                                                       0xc03c,
                                                       0xd1b5,
                                                       0x2942,
                                                       0x38cb,
                                                       0x0a50,
                                                       0x1bd9,
                                                       0x6f66,
                                                       0x7eef,
                                                       0x4c74,
                                                       0x5dfd,
                                                       0xb58b,
                                                       0xa402,
                                                       0x9699,
                                                       0x8710,
                                                       0xf3af,
                                                       0xe226,
                                                       0xd0bd,
                                                       0xc134,
                                                       0x39c3,
                                                       0x284a,
                                                       0x1ad1,
                                                       0x0b58,
                                                       0x7fe7,
                                                       0x6e6e,
                                                       0x5cf5,
                                                       0x4d7c,
                                                       0xc60c,
                                                       0xd785,
                                                       0xe51e,
                                                       0xf497,
                                                       0x8028,
                                                       0x91a1,
                                                       0xa33a,
                                                       0xb2b3,
                                                       0x4a44,
                                                       0x5bcd,
                                                       0x6956,
                                                       0x78df,
                                                       0x0c60,
                                                       0x1de9,
                                                       0x2f72,
                                                       0x3efb,
                                                       0xd68d,
                                                       0xc704,
                                                       0xf59f,
                                                       0xe416,
                                                       0x90a9,
                                                       0x8120,
                                                       0xb3bb,
                                                       0xa232,
                                                       0x5ac5,
                                                       0x4b4c,
                                                       0x79d7,
                                                       0x685e,
                                                       0x1ce1,
                                                       0x0d68,
                                                       0x3ff3,
                                                       0x2e7a,
                                                       0xe70e,
                                                       0xf687,
                                                       0xc41c,
                                                       0xd595,
                                                       0xa12a,
                                                       0xb0a3,
                                                       0x8238,
                                                       0x93b1,
                                                       0x6b46,
                                                       0x7acf,
                                                       0x4854,
                                                       0x59dd,
                                                       0x2d62,
                                                       0x3ceb,
                                                       0x0e70,
                                                       0x1ff9,
                                                       0xf78f,
                                                       0xe606,
                                                       0xd49d,
                                                       0xc514,
                                                       0xb1ab,
                                                       0xa022,
                                                       0x92b9,
                                                       0x8330,
                                                       0x7bc7,
                                                       0x6a4e,
                                                       0x58d5,
                                                       0x495c,
                                                       0x3de3,
                                                       0x2c6a,
                                                       0x1ef1,
                                                       0x0f78
                                                     };

/* ------------------------------------------------------------------------
 ---   TMW_61850_DL_crc16_fcs()                                           -----
 ---      Computes CRC16 FCS on byte by byte basis.                    -----
 ---   Calling sequence:                                               -----
 ---      fcs     - FCS value up to this point                         -----
 ---      cp      - Pointer to next byte in frame                      -----
 ---      len     - Number of bytes to add to FCS                      -----
 ---   Returns                                                         -----
 ---      New FCS value                                                -----
 ------------------------------------------------------------------------ */
unsigned int TMW_61850_DL_crc16_fcs(unsigned int fcs,
                                    unsigned char *cp,
                                    int len)
{
    /* assert(sizeof (unsigned int) == 2); */
    /* assert(((unsigned int) -1) > 0); */
    while (len--)
        fcs = (fcs >> 8) ^ TMW_61850_DL_crc16_fcstab[(fcs ^ *cp++) & 0xff];

    return (fcs);
}

void TMW_61850_DL_crc16_compute(unsigned char *pBuffer,
                                int nLength,
                                unsigned int *pCrc)
{
    *pCrc = TMW_61850_DL_crc16_fcs( INITFCS, pBuffer, nLength );
    *pCrc = ~(*pCrc) & 0xFFFF;
}

int TMW_61850_DL_crc16_check(unsigned char *pBuffer,
                             int nLength,
                             unsigned int nCrc)
{
    unsigned int lclCrc, resultCrc;

    lclCrc = TMW_61850_DL_crc16_fcs( INITFCS, pBuffer, nLength );
    resultCrc = ~lclCrc & 0xFFFF;
    if (resultCrc != nCrc)
        return (0);
    return (1);
}

/* Basic Data Link Services */
int TMW_DL_close(TMW_DL_ContextPtr context)
{
    if (context->dl_def->TamDlClose != NULL)
        return (context->dl_def->TamDlClose( context ));
    else
        return ( FALSE);
}

int TMW_DL_bind(TMW_DL_ContextPtr context,
                TMW_DL_Bind_Type bind_type,
                TMW_Mac_Address *multi_addr,
                int lsap,
                TMW_DL_Ind ind_function,
                void *ind_param)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def != NULL && context->dl_def->TamDlBind != NULL)
        return (context->dl_def->TamDlBind( context, bind_type, multi_addr, lsap, ind_function, ind_param ));
    else
        return ( FALSE);
}

int TMW_DL_unbind(TMW_DL_ContextPtr context,
                  TMW_DL_Bind_Type bind_type,
                  TMW_Mac_Address *multi_addr,
                  int lsap,
                  TMW_DL_Ind ind_function,
                  void *ind_param)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlUnBind != NULL)
        return (context->dl_def->TamDlUnBind( context, bind_type, multi_addr, lsap, ind_function, ind_param ));
    else
        return ( FALSE);
}

int TMW_DL_service(TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlService != NULL)
        return (context->dl_def->TamDlService( context ));
    else
        return ( FALSE);
}

int TMW_DL_receive(TMW_Packet *packet,
                   TMW_Mac_Address *mac,
                   TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlReceive != NULL)
        return (context->dl_def->TamDlReceive( packet, mac, context ));
    else
        return ( FALSE);
}

int TMW_DL_can_send(int length,
                    TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlCanSend != NULL)
        return (context->dl_def->TamDlCanSend( length, context ));
    else
        return ( FALSE);
}

int TMW_DL_send(TMW_Packet *packet,
                TMW_Mac_Address *mac,
                TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlSend != NULL)
        return (context->dl_def->TamDlSend( packet, mac, context ));
    else
        return ( FALSE);
}

int TMW_DL_send_mgmt(TMW_Packet *packet,
                     TMW_Mac_Address *mac,
                     TMW_DL_ContextPtr context,
                     MMSd_time *ts_send)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlSendMgmt != NULL)
        return (context->dl_def->TamDlSendMgmt( packet, mac, context, ts_send ));
    else
        return ( FALSE);
}

/* Services for connection oriented links (ADLC) */
int TMW_DL_connect(TMW_Mac_Address *mac,
                   TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlConnect != NULL)
        return (context->dl_def->TamDlConnect( mac, context ));
    else
        return ( TRUE); /* Act like its connected */
}

int TMW_DL_disconnect(TMW_Mac_Address *mac,
                      TMW_DL_ContextPtr context)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlDisconnect != NULL)
        return (context->dl_def->TamDlDisconnect( mac, context ));
    else
        return ( TRUE); /* Act like its disconnected */
}

int TMW_DL_wait_for_poll(TMW_DL_ContextPtr context,
                         TMW_ms_time delay)
{
    if (context == NULL)
        return ( FALSE);

    if (context->dl_def->TamDlWaitForPoll != NULL)
        return (context->dl_def->TamDlWaitForPoll( context, delay ));
    else
        return ( TRUE); /* Act like its polling */
}

#if defined(DEBUG_SEMAPHORES)

/* ***************************************************************************** */
/* This code is used internally to verify the operation of semaphores and events */
/* It should NEVER be turned on in user builds.                                  */
/* ***************************************************************************** */

typedef struct TamWindowsSemJournalEntry {
    TamWindowsSem semData;
    MMSd_time timeStamp;
    int action;
} TamWindowsSemJournalEntry;

#define TMW_SEM_JOURNAL_SIZE 10000

HANDLE TamWindowsSemJournalSem;
TamWindowsSemJournalEntry TamWindowsSemJournal[ TMW_SEM_JOURNAL_SIZE ];
long TamWindowsSemJournalIndex = -1;
#include <time.h>

int heldCounts = 0;

void TMW_StoreSemAccess( TamWindowsSem *pSem, int action )
{
    if ( TamWindowsSemJournalIndex == -1 ) {
        TamWindowsSemJournalSem = CreateMutex( NULL, 0, NULL );
        memset( TamWindowsSemJournal, 0, sizeof( TamWindowsSemJournal ) );
        TamWindowsSemJournalIndex = 0;
    }

    WaitForSingleObject( TamWindowsSemJournalSem, INFINITE );
    if ( pSem->held > 1 )
        heldCounts++;
    if ( TamWindowsSemJournalIndex >= TMW_SEM_JOURNAL_SIZE )
        TamWindowsSemJournalIndex = 0;
    TamWindowsSemJournal[ TamWindowsSemJournalIndex ].semData = *pSem;
    TamWindowsSemJournal[ TamWindowsSemJournalIndex ].action = action;
    /* This causes problems when time_stamp uses semaphores */
    /* MMSd_time_stamp( &TamWindowsSemJournal[ TamWindowsSemJournalIndex++ ].timeStamp ); */
    ReleaseMutex( TamWindowsSemJournalSem );
}

void TMW_DumpSemAccess( char *fileName )
{
    FILE *fd;
    long i, j;

    fd = fopen( fileName, "wt" );
    if ( fd == NULL )
        return;

    WaitForSingleObject( TamWindowsSemJournalSem, INFINITE );
    if ( TamWindowsSemJournalIndex >= TMW_SEM_JOURNAL_SIZE )
        TamWindowsSemJournalIndex = 0;

    if ( ( TamWindowsSemJournal[ TamWindowsSemJournalIndex ].timeStamp.millisecondsSinceMidnight == 0 )
            && ( TamWindowsSemJournal[ TamWindowsSemJournalIndex ].timeStamp.daysSinceJan1_1984 == 0 ) )
        j = 0;
    else
        j = TamWindowsSemJournalIndex;

    for ( i = 0; i < TMW_SEM_JOURNAL_SIZE; i++, j++ ) {
        if ( j >= TMW_SEM_JOURNAL_SIZE )
            j = 0;

        if ( ( TamWindowsSemJournal[ j ].timeStamp.millisecondsSinceMidnight == 0 )
                && ( TamWindowsSemJournal[ j ].timeStamp.daysSinceJan1_1984 == 0 ) )
            break;
        fprintf( fd, "%lu",
                 TamWindowsSemJournal[ j ].timeStamp.millisecondsSinceMidnight );
        fprintf( fd, " %lu",
                 TamWindowsSemJournal[ j ].semData.threadId );
        if ( TamWindowsSemJournal[ j ].action == 1 )
            fprintf( fd, " t" );
        else if ( TamWindowsSemJournal[ j ].action == 0 )
            fprintf( fd, " r" );
        else
            fprintf( fd, " ?" );
        fprintf( fd, " %d",
                 TamWindowsSemJournal[ j ].semData.held );
        fprintf( fd, " %d",
                 TamWindowsSemJournal[ j ].semData.high );
        fprintf( fd, " %s",
                 TamWindowsSemJournal[ j ].semData.filename );
        fprintf( fd, ":%d",
                 TamWindowsSemJournal[ j ].semData.lineno );
        fprintf( fd, " %s",
                 TamWindowsSemJournal[ j ].semData.semname );
        fprintf( fd, " %s",
                 TamWindowsSemJournal[ j ].semData.filenamehigh );
        fprintf( fd, ":%d\n",
                 TamWindowsSemJournal[ j ].semData.linenohigh );
    }
    ReleaseMutex( TamWindowsSemJournalSem );

    fclose( fd );
}
#endif

#if defined(DEBUG_EVENTS)

/* ***************************************************************************** */
/* This code is used internally to verify the operation of semaphores and events */
/* It should NEVER be turned on in user builds.                                  */
/* ***************************************************************************** */

typedef enum {EVE_NONE, EVE_CREATE, EVE_DELETE, EVE_WAIT, EVE_WAKE, EVE_SIGNAL} TamWindowsEventAction;
char *EventActionNames[ ] = {
    "None",
    "Create",
    "Delete",
    "Wait",
    "Wake",
    "Signal"
};

typedef struct {
    HANDLE handle;
    TamWindowsEventAction action;
    DWORD threadId;
    unsigned long msWait;
    MMSd_time timeStamp;
    int lineno;
    char funcname[ 100 ];
} TamWindowsEveJournalEntry;

void FillEvent( TamWindowsEveJournalEntry *pEve, TMW_TARG_EVENT *eve, unsigned long msWait, char *pFuncName, int lineno )
{
    pEve->handle = *eve;
    strcpy( pEve->funcname, pFuncName );
    pEve->lineno = lineno;
    pEve->msWait = msWait;
    pEve->threadId = GetCurrentThreadId();
    MMSd_time_stamp( &pEve->timeStamp );
}

#define TMW_EVE_JOURNAL_SIZE 100000

HANDLE TamWindowsEveJournalSem;
TamWindowsEveJournalEntry TamWindowsEveJournal[ TMW_EVE_JOURNAL_SIZE ];
long TamWindowsEveJournalIndex = -1;

void StoreEvent( TamWindowsEveJournalEntry *pEve )
{
    if ( TamWindowsEveJournalIndex == -1 ) {
        TamWindowsEveJournalSem = CreateMutex( NULL, 0, NULL );
        memset( TamWindowsEveJournal, 0, sizeof( TamWindowsEveJournal ) );
        TamWindowsEveJournalIndex = 0;
    }

    WaitForSingleObject( TamWindowsEveJournalSem, INFINITE );
    if ( TamWindowsEveJournalIndex >= TMW_EVE_JOURNAL_SIZE )
        TamWindowsEveJournalIndex = 0;
    TamWindowsEveJournal[ TamWindowsEveJournalIndex++ ] = *pEve;
    ReleaseMutex( TamWindowsEveJournalSem );
}

FILE *eveFd = NULL;
int DEBUG_TMW_TARG_EVENT_CREATE( TMW_TARG_EVENT *eve, char *pFuncName, int lineno )
{
    TamWindowsEveJournalEntry tempEve;

    *eve = CreateEvent(NULL,FALSE,FALSE,NULL);
    FillEvent( &tempEve, eve, 0L, pFuncName, lineno );
    tempEve.action = EVE_CREATE;
    StoreEvent( &tempEve );
    return( *eve != NULL );
}

void DEBUG_TMW_TARG_EVENT_DELETE( TMW_TARG_EVENT *eve, char *pFuncName, int lineno )
{
    TamWindowsEveJournalEntry tempEve;

    FillEvent( &tempEve, eve, 0L, pFuncName, lineno );
    tempEve.action = EVE_DELETE;
    StoreEvent( &tempEve );
    CloseHandle( *eve );
    *eve = NULL;
}

int DEBUG_TMW_TARG_EVENT_WAIT( TMW_TARG_EVENT *eve, unsigned long ms, char *pFuncName, int lineno )
{
    int result;
    TamWindowsEveJournalEntry tempEve;

    FillEvent( &tempEve, eve, ms, pFuncName, lineno );
    tempEve.action = EVE_WAIT;
    StoreEvent( &tempEve );

    result = (WaitForSingleObject( *eve, ms ) != WAIT_TIMEOUT);

    tempEve.action = EVE_WAKE;
    tempEve.msWait = (unsigned long) result;
    StoreEvent( &tempEve );
    return( result );
}

void DEBUG_TMW_TARG_EVENT_SIGNAL( TMW_TARG_EVENT *eve, char *pFuncName, int lineno )
{
    TamWindowsEveJournalEntry tempEve;

    FillEvent( &tempEve, eve, 0L, pFuncName, lineno );
    tempEve.action = EVE_SIGNAL;
    StoreEvent( &tempEve );
    SetEvent( *eve );
}

void TMW_DumpEveAccess( char *fileName )
{
    FILE *fd;
    long i, j;

    fd = fopen( fileName, "wt" );
    if ( fd == NULL )
        return;

    WaitForSingleObject( TamWindowsEveJournalSem, INFINITE );
    if ( TamWindowsEveJournalIndex >= TMW_EVE_JOURNAL_SIZE )
        TamWindowsEveJournalIndex = 0;

    if ( TamWindowsEveJournal[ TamWindowsEveJournalIndex ].action == EVE_NONE )
        j = 0;
    else
        j = TamWindowsEveJournalIndex;

    for ( i = 0; i < TMW_EVE_JOURNAL_SIZE; i++, j++ ) {
        if ( j >= TMW_EVE_JOURNAL_SIZE )
            j = 0;

        if ( TamWindowsEveJournal[ j ].action == EVE_NONE )
            break;
        fprintf( fd, "%lu %lu %lu %d %lu %s:%d\n",
                 TamWindowsEveJournal[ j ].timeStamp.millisecondsSinceMidnight,
                 TamWindowsEveJournal[ j ].handle,
                 TamWindowsEveJournal[ j ].threadId,
                 TamWindowsEveJournal[ j ].action,
                 TamWindowsEveJournal[ j ].msWait,
                 TamWindowsEveJournal[ j ].funcname,
                 TamWindowsEveJournal[ j ].lineno );
    }
    ReleaseMutex( TamWindowsEveJournalSem );

    fclose( fd );
}

#endif

#endif //#include TAM_C
