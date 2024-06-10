/* sfzclfastalloc.h
 */

/*****************************************************************************
* Copyright (c) 2006-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef SFZCLFASTALLOC_H_INCLUDED
#define SFZCLFASTALLOC_H_INCLUDED

typedef struct SfzclFastMemoryAllocatorRec *SfzclFastMemoryAllocator;

/** Initialize a new memory allocator for fixed-sized blobs.
   `blob_size' is the size of the blobs. `blob_quant' is the number of
   blobs for which room will be reserved atomically. Both `blob_size'
   and `blob_quant' must be larger than zero. */
SfzclFastMemoryAllocator sfzcl_fastalloc_initialize(int blob_size,
                                                    int blob_quant);

/** Uninitialize a memory allocator. All allocated blobs must have been
   freed, otherwise sfzcl_fatal() may be triggered. */
void sfzcl_fastalloc_uninitialize(SfzclFastMemoryAllocator allocator);

/** Allocate a new blob of the size `blob_size'. The returned data is
   correctly aligned for all kinds of purposes. The data is not
   necessarily initialized.

   This can return NULL if lower-level memory allocation can. */
void *sfzcl_fastalloc_alloc(SfzclFastMemoryAllocator allocator);

/** Free an individual blob. */
void sfzcl_fastalloc_free(SfzclFastMemoryAllocator allocator, void *data);

/** You do not need to access these structures directly but the
   declarations must be public so that the macros above can work. */
typedef struct
{
    void *free_chain;
} SfzclFastallocProtoBlob;

typedef struct sfzcl_fastalloc_blobs
{
    void *blobs;
    struct sfzcl_fastalloc_blobs *next;
} SfzclFastallocBlobs;

struct SfzclFastMemoryAllocatorRec
{
    int total_size;
    int allocated;
    int blob_size;
    int blob_quant;
    SfzclFastallocBlobs *blobs;
    SfzclFastallocProtoBlob *free_chain;
};

#endif /** SFZCLFASTALLOC_H_INCLUDED */
