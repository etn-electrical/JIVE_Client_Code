/* debug_abort.c
 *
 * Description: Implementation of DEBUG_abort.
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

#include "implementation_defs.h"

#include "osdep_stdlib.h"
#include "osdep_stdio.h"      /* fflush, stderr */


/* This logic is to make it possible to get coverage reports on
   software runs that end-up (intentionally) to abort. */
#ifdef DEBUG_CF_ABORT_WRITE_PROFILE
void __gcov_flush();    /* Function to write profiles on disk. */
# define DEBUG_ABORT_WRITE_PROFILE __gcov_flush()
#else
# define DEBUG_ABORT_WRITE_PROFILE do { /* Not written. */ } while (0)
#endif


void DEBUG_abort(void)
{
#ifdef WIN32
    /* avoid the "report to microsoft?" dialog and the */
    /* "your program seems to have stopped abnormally" message */
    _set_abort_behavior(0, _WRITE_ABORT_MSG + _CALL_REPORTFAULT);
#endif

    /* flush stderr before calling Abort() to make sure
       out is not cut off due to buffering. */
    Fflush(stderr);

    DEBUG_ABORT_WRITE_PROFILE;

    Abort();
}

/* end of file debug_abort.c */
