/**
 *      @file    corelib_main.c
 *
 *
 *      Open and Close APIs.
 */
/*
 *      Copyright (c) 2013-2018 INSIDE Secure Corporation
 *      All Rights Reserved
 *
 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF INSIDE.
 *
 *      Please do not edit this file without first consulting INSIDE support.
 *      Unauthorized changes to this file are not supported by INSIDE.
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 *
 *      This Module contains Proprietary Information of INSIDE and should be
 *      treated as Confidential.
 *
 *      The information in this file is provided for the exclusive use of the
 *      licensees of INSIDE. Such users have the right to use, modify,
 *      and incorporate this code into products for purposes authorized by the
 *      license agreement provided they include this notice and the associated
 *      copyright notice with any such product.
 *
 *      The information in this file is provided "AS IS" without warranty.
 */
/******************************************************************************/

#include "osdep_stdio.h"
#include "coreApi.h"
#include "osdep.h"
#include "psUtil.h"
#include "osdep_strict.h"

#ifdef USE_MULTITHREADING
/* A mutex for concurrency control of functions implemented in this file.
   Obvious exception are psCoreOpen() and psCoreClose(). */
static psMutex_t corelibMutex;
#endif /* USE_MULTITHREADING */

/******************************************************************************/
/*
    Open (initialize) the Core module
    The config param should always be passed as:
        PSCORE_CONFIG
 */
static char g_config[32] = "N";

/******************************************************************************/
int32 psCoreOpen(const char *config)
{
    if (*g_config == 'Y')
    {
        return PS_CORE_IS_OPEN;
    }
    Strncpy(g_config, PSCORE_CONFIG, sizeof(g_config) - 1);
    if (Strncmp(g_config, config, Strlen(PSCORE_CONFIG)) != 0)
    {
        psErrorStr( "Core config mismatch.\n" \
            "Library: " PSCORE_CONFIG \
            "\nCurrent: %s\n", config);
        return -1;
    }

    if (halOpen() < 0)
    {
        return PS_FAILURE;
    }

    if (osdepTimeOpen() < 0)
    {
        psTraceCore("osdepTimeOpen failed\n");
        return PS_FAILURE;
    }
    if (osdepEntropyOpen() < 0)
    {
        psTraceCore("osdepEntropyOpen failed\n");
        osdepTimeClose();
        return PS_FAILURE;
    }

#ifdef USE_MULTITHREADING
    if (osdepMutexOpen() < 0)
    {
        psTraceCore("osdepMutexOpen failed\n");
        osdepEntropyClose();
        osdepTimeClose();
        return PS_FAILURE;
    }
    if (psCreateMutex(&corelibMutex, 0) < 0)
    {
        psTraceCore("psCreateMutex failed\n");
        osdepMutexClose();
        osdepEntropyClose();
        osdepTimeClose();
        return PS_FAILURE;
    }
#endif /* USE_MULTITHREADING */

    if (psOpenMalloc() < 0)
    {
        psTraceCore("psOpenMalloc failed\n");
# ifdef USE_MULTITHREADING
        psDestroyMutex(&corelibMutex);
        osdepMutexClose();
# endif /* USE_MULTITHREADING */
        osdepEntropyClose();
        osdepTimeClose();
        return PS_FAILURE;
    }

    return PS_SUCCESS;
}

/******************************************************************************/
void psCoreClose(void)
{
    if (*g_config == 'Y')
    {
        *g_config = 'N';

        psCloseMalloc();

#ifdef USE_MULTITHREADING
        psDestroyMutex(&corelibMutex);
        osdepMutexClose();
#endif  /* USE_MULTITHREADING */

        osdepEntropyClose();

        osdepTimeClose();

        halClose();
    }
}

#ifdef USE_MULTITHREADING

/* These functions are intended for internal use of corelib. */

/* Acquire lock for performing an operation that does not allow
   multithreading.
   Returns >= 0 for success and negative value otherwise. */
int32 psCoreLibInternalLock(void)
{
    psLockMutex(&corelibMutex);
    return 0;
}

/* Free lock for performing an operation that does not allow
   multithreading. */
void psCoreLibInternalUnlock(int32 lockid)
{
    if (lockid == 0)
    {
        psUnlockMutex(&corelibMutex);
    }
}

#endif /* USE_MULTITHREADING */

/******************************************************************************/
/*
    Clear the stack deeper than the caller to erase any potential secrets
    or keys.
 */
void psBurnStack(uint32 len)
{
    unsigned char buf[32];

    memset_s(buf, sizeof(buf), 0x0, sizeof(buf));
    if (len > (uint32) sizeof(buf))
    {
        psBurnStack(len - sizeof(buf));
    }
}
