/* sfzclmalloc.h

   Versions of malloc and friends that check their results, and never return
   failure (they call fatal if they encounter an error).

   These functions MUST be multi thread safe, if the system is using threads.
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

#ifndef SFZCLMALLOC_H
#define SFZCLMALLOC_H

#include "public_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

void *sfzcl_strdup(const void *str);

void *sfzcl_memdup(const void *data, size_t len);

void *sfzcl_realloc(void *ptr, size_t old_size, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif                          /* SFZCLMALLOC_H */
