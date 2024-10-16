/**
 *      @file    osdep.c
 *
 *
 *      Any ANSI-C compatible system including POSIX.
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

#include "coreApi.h"
#include "osdep.h"
#include "osdep_stdlib.h"

#ifdef HALT_ON_PS_ERROR
/******************************************************************************/
/*
    POSIX - abort() on psError when HALT_ON_PS_ERROR enabled
 */
void osdepBreak(void)
{
    Abort();
}
#else

extern void osDepBreak_not_included(void);

#endif /* HALT_ON_PS_ERROR */

/******************************************************************************/
