/* spal_thread.h
 *
 * Description: Thread APIs
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

#ifndef INCLUDE_GUARD_SPAL_THREAD_H
#define INCLUDE_GUARD_SPAL_THREAD_H

#include "public_defs.h"
#include "spal_result.h"

#include "cfg_spal.h"


#ifdef SPAL_CFG_THREAD_TYPE
typedef SPAL_CFG_THREAD_TYPE SPAL_Thread_t;
#else
typedef uint32_t SPAL_Thread_t;
#endif

SPAL_Thread_t
SPAL_Thread_Self(
    void);


SPAL_Result_t
SPAL_Thread_Create(
    SPAL_Thread_t * const Thread_p,
    const void * const Reserved_p,
    void * (*StartFunction_p)(void * const Param_p),
    void * const ThreadParam_p);


SPAL_Result_t
SPAL_Thread_Detach(
    const SPAL_Thread_t Thread);


SPAL_Result_t
SPAL_Thread_Join(
    const SPAL_Thread_t Thread,
    void ** const Status_p);


void
SPAL_Thread_Exit(
    void * const Status);

#endif /* Include guard */

/* end of file spal_thread.h */
