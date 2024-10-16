/**
 *      @file    pscompilerdep.h
 *
 *
 *      Compiler Pragmas/Diagnostics Capabilities Abstraction.
 */
/*
 *      Copyright (c) 2018 INSIDE Secure Corporation
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

#ifndef _h_PS_COMPILERDEP
# define _h_PS_COMPILERDEP

/* Compiler detection. */

/* Detect GCC 4.x series. */
# ifdef __GNUC__
#  if __GNUC__ == 4
#   define __GNUC4__ /* 4.x series GNU C. These default to C89 standard,
                        which has to be considered in some of the code. */
#  endif
/* For all GCC versions, provide version as single number for easier range
   matching. */
#  define __GCC_VERSION ((__GNUC__ * 1000000) |                         \
                         (__GNUC_MINOR__ * 1000) |                      \
                         (__GNUC_PATCHLEVEL__))
# else /* __GNUC__ */
# define __GCC_VERSION 0 /* No GCC. */
# endif

/* Detect ARM DS-5 compiler. */
# ifdef __ARMCC_VERSION
#  if __ARMCC_VERSION < 6000000
#   define __ARMCC5 /* armcc prior 6.0 (not clang-based) */
#   undef inline
#   define inline __inline /* Use compiler specific inline keyword. */
#  endif
# endif /* __ARMCC_VERSION */

/* Mark API function as deprecated. */
#define PSDEPRECATED /* the following function is deprecated. */

/* Control for diagnostics on deprecated functions.
   PSDEPRECATED_START ... PSDEPRECATED_END allows section where
   deprected functions are allowed. */
#ifdef __GNUC__
#if ( __GNUC__ == 4 && __GNUC_MINOR__ >= 8 ) || __GNUC__ >= 5
#define PSDEPRECATED_START /* Omit deprecated warnings */ \
    _Pragma( "GCC diagnostic push" )                      \
    _Pragma( "GCC diagnostic ignored \"-Wdeprecated-declarations\"" )
#define PSDEPRECATED_END /* end section with omitted warnings */ \
    _Pragma( "GCC diagnostic pop" )
#endif
#endif

/* Mark API function as deprecated, and warn when it is used if compiler
   provides the capability. This only happens on GCC 4.8 or later. */
#ifdef PSDEPRECATED_START
#define PSDEPRECATED_WARN __attribute__((__deprecated__))
#else
#define PSDEPRECATED_WARN /* silently ignored on compilers
                             not providing PSDEPRECATED_START.
                             Note: Header will still advice function is
                             deprecated. */
#endif

/* If compiling source files that intentionally use deprecated functions,
   omit warnings. (For instance implementation may use deprecated functions
   internally as components of implementation.) */
#ifdef CONTAINS_DEPRECATED_FUNCTION_CALLS
#undef PSDEPRECATED_WARN
#define PSDEPRECATED_WARN PSDEPRECATED
#endif /* CONTAINS_DEPRECATED_FUNCTION_CALLS */

#ifndef PSDEPRECATED_START
#define PSDEPRECATED_START /* this code may call deprecated functions. */
#define PSDEPRECATED_END /* end of section. */
#endif

#ifdef __GNUC__
/* Mark intentionally unused functions. */
#define PSFUNC_UNUSED __attribute__((__unused__))
#else
/* Mark intentionally unused functions (can used compiler specific
   directive) here. */
#define PSFUNC_UNUSED /* __attribute__((__unused__)) */
#endif


#endif  /* _h_PS_COMPILERDEP */

