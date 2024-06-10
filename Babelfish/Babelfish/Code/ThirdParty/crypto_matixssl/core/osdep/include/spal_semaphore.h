/* spal_semaphore.h
 *
 * Description: Semaphore APIs
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

#ifndef INCLUDE_GUARD_SPAL_SEMAPHORE_H_
#define INCLUDE_GUARD_SPAL_SEMAPHORE_H_

#include "public_defs.h"
#include "spal_result.h"

#include "cfg_spal.h"

struct SPAL_Semaphore
{
    union
    {
#ifdef SPAL_CFG_SEMAPHORE_ALIGN_TYPE
        SPAL_CFG_SEMAPHORE_ALIGN_TYPE Alignment;
#endif
        uint8_t Size[SPAL_CFG_SEMAPHORE_SIZE];
    } Union;
};

typedef struct SPAL_Semaphore SPAL_Semaphore_t;


SPAL_Result_t
SPAL_Semaphore_Init(
    SPAL_Semaphore_t * const Semaphore_p,
    const unsigned int InitialCount);


void
SPAL_Semaphore_Wait(
    SPAL_Semaphore_t * const Semaphore_p);


SPAL_Result_t
SPAL_Semaphore_TryWait(
    SPAL_Semaphore_t * const Semaphore_p);


SPAL_Result_t
SPAL_Semaphore_TimedWait(
    SPAL_Semaphore_t * const Semaphore_p,
    const unsigned int TimeoutMilliSeconds);


void
SPAL_Semaphore_Post(
    SPAL_Semaphore_t * const Semaphore_p);


void
SPAL_Semaphore_Destroy(
    SPAL_Semaphore_t * const Semaphore_p);

#endif /* Include guard */

/* end of file spal_semaphore.h */
