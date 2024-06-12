/** osdep_netdb.h
 *
 * Wrapper for system header osdep_netdb.h
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

/* This file just includes system header netdb.h.
   In case your system does not include all functions
    via that file or
   does not have implementation of netdb.h, please
   customize this place holder header.
*/

#ifndef OSDEP_NETDB_H_DEFINED
#define OSDEP_NETDB_H_DEFINED 1


#include <netdb.h>


/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from POSIX.1-2001.
   The getaddrinfo() function is documented in RFC 2553.
   The defines may be overrided from command line. */

/* Macro that provides Getaddrinfo, which is macro wrapper for getaddrinfo. */
#ifndef Getaddrinfo
#define Getaddrinfo getaddrinfo
#endif /* Getaddrinfo */

/* Macro that provides Freeaddrinfo, which is macro wrapper for freeaddrinfo. */
#ifndef Freeaddrinfo
#define Freeaddrinfo freeaddrinfo
#endif /* Freeaddrinfo */

#endif /* OSDEP_NETDB_H_DEFINED */
