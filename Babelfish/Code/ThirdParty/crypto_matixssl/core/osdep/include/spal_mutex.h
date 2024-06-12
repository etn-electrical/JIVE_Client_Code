/* spal_mutex.h
 *
 * Description: Mutex APIs
 */

/*****************************************************************************
* Copyright (c) 2007-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef INCLUDE_GUARD_SPAL_MUTEX_H_
#define INCLUDE_GUARD_SPAL_MUTEX_H_

#include "public_defs.h"
#include "spal_result.h"

#include "cfg_spal.h"

struct SPAL_Mutex
{
    union
    {
#ifdef SPAL_CFG_MUTEX_ALIGN_TYPE
        SPAL_CFG_MUTEX_ALIGN_TYPE Alignment;
#endif
        uint8_t Size[SPAL_CFG_MUTEX_SIZE];
    } Union;
};

typedef struct SPAL_Mutex SPAL_Mutex_t;

SPAL_Result_t
SPAL_Mutex_Init(
    SPAL_Mutex_t * const Mutex_p);

void
SPAL_Mutex_Lock(
    SPAL_Mutex_t * const Mutex_p);

void
SPAL_Mutex_UnLock(
    SPAL_Mutex_t * const Mutex_p);

void
SPAL_Mutex_Destroy(
    SPAL_Mutex_t * const Mutex_p);

bool
SPAL_Mutex_IsLocked(
    SPAL_Mutex_t * const Mutex_p);

SPAL_Result_t
SPAL_Mutex_TryLock(
    SPAL_Mutex_t * const Mutex_p);

#endif /* Include guard */

/* end of file spal_mutex.h */
