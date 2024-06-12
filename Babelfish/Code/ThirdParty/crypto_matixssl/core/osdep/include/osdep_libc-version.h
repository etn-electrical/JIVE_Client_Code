/** osdep_libc-version.h
 *
 * Wrapper for system header osdep_libc-version.h
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/* This file just includes system header gnu/libc-version.h.
   In case your system does not include all functions
    via that file or
   does not have implementation of gnu/libc-version.h, please
   customize this place holder header.
*/

#ifndef OSDEP_LIBC_VERSION_H_DEFINED
#define OSDEP_LIBC_VERSION_H_DEFINED 1

/* Get (build time) libc version on Linux from GLIBC headers.
   This header is no-op on other systems.
   Android systems are autodetected, they do not use GLIBC.
   There is -DPS_NO_GLIBC for other linux kernel based systems not using
   GLIBC. */

#ifdef __unix__
#ifdef __linux__
#ifndef __ANDROID__
#ifndef PS_NO_GLIBC
#include <gnu/libc-version.h>
#endif /* PS_NO_GLIBC */
#endif /* __ANDROID__ */
#endif /* __linux__ */
#endif /* __unix__ */

#endif /* OSDEP_LIBC_VERSION_H_DEFINED */
