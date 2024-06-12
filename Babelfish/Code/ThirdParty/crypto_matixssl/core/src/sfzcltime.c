/* sfzcltime.c
 *
 * Time retrieval (time as a counter).
 */

/*****************************************************************************
* Copyright (c) 2006-2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#include "sfzclincludes.h"
#include "sfzclgetput.h"
#include "osdep_time.h"

#define SFZCL_DEBUG_MODULE "SfzclTime"

#ifndef SFZCLDIST_FREESTANDING

/* Returns seconds from epoch "January 1 1970, 00:00:00 UTC".  This
   implementation is Y2K compatible as far as system provided time_t
   is such.  However, since systems seldomly provide with more than 31
   meaningful bits in time_t integer, there is a strong possibility
   that this function needs to be rewritten before year 2038.  No
   interface changes are needed in reimplementation. */
SfzclTime
sfzcl_time(void)
{
    return (SfzclTime) (Time(NULL));
}
#else
/* C99 Freestanding implementation. There is no time() function available.
   The customer needs to implement sfzcl_time() that determines current time
   in platform dependent way. */

/*
   Example:
   SfzclTime
   sfzcl_time (void)
   {
   return (SfzclTime) platform_dependent_get_time();
   }
 */

SfzclTime
sfzcl_time(void)
{
    /* Return static value. (2.4.2011) */
    return 1304432553;
}

#endif                          /* SFZCLDIST_FREESTANDING */

/* end of file sfzcltime.c */
