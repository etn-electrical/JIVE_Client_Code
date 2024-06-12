/** osdep_signal.h
 *
 * Wrapper for system header osdep_signal.h
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

/* This file just includes system header signal.h.
   In case your system does not include all functions
   signal/sigprocmask via that file or
   does not have implementation of signal.h, please
   customize this place holder header. 
*/

#ifndef OSDEP_SIGNAL_H_DEFINED
#define OSDEP_SIGNAL_H_DEFINED 1


#include <signal.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Signal, which is macro wrapper for signal. */
#ifndef Signal
#define Signal signal
#endif /* Signal */

/* Macro that provides Sigprocmask, which is macro wrapper for sigprocmask. */
#ifndef Sigprocmask
#define Sigprocmask sigprocmask
#endif /* Sigprocmask */



#endif /* OSDEP_SIGNAL_H_DEFINED */
