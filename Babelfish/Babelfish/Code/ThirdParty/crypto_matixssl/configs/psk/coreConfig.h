/**
 *      @file    coreConfig.h
 *
 *
 *      Configuration settings for Matrix core module.
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

#ifndef _h_PS_CORECONFIG
# define _h_PS_CORECONFIG


/******************************************************************************/
/* Debug and tracing configuration */
/******************************************************************************/

/**
    Enable various levels of trace.
    When these option is turned off, messages are silently
    discarded and their text does not take space in the binary image.
 */
/* #define USE_CORE_TRACE */
#  ifndef NO_CORE_ERROR
#   define USE_CORE_ERROR
#  endif
#  ifndef NO_CORE_ASSERT
#   define USE_CORE_ASSERT
#  endif

/** Allow target file of psTrace output to be chosen with the
    PSCORE_DEBUG_FILE and PSCORE_DEBUG_FILE_APPEND environment variables.
    By default, stdout is used. Disable to minimize footprint. */
/* #define USE_TRACE_FILE */

/** Experimental, extensible logging facility. Only used by the SL/CL
    crypto libraries; not used by the TLS library. Disable to minimize
    footprint. */
/* #define PS_LOGF */

/******************************************************************************/
/* Other Configurable features */
/******************************************************************************/

/**
    If enabled, calls to the psError set of APIs will perform a platform
    abort on the exeutable to aid in debugging.
 */
#  ifdef DEBUG
/* #define HALT_ON_PS_ERROR  *//* NOT RECOMMENDED FOR PRODUCTION BUILDS */
#  endif

/** Enable to disable file IO related APIs, such as psGetFileBuf
    and psParseCertFile. This helps to minimize footprint when no file IO
    is needed. */
/* #define NO_FILE_SYSTEM */

/**
    Include support for Deterministic Memory Pools
 */
/* #define USE_MATRIX_MEMORY_MANAGEMENT */

/**
    Include the psCoreOsdepMutex family of APIs

    @note If intending to compile crypto-cl, then this flag should
    always be set.
*/
#  ifndef NO_MULTITHREADING
/* #define USE_MULTITHREADING */
#  endif /* NO_MULTITHREADING */

/**
    Include the psNetwork family of APIs.

    These APIs allow simple high-level socket api.
    The API derive from BSD Sockets, and therefore it can only be used
    on devices which have the prerequisitive APIs.
    MatrixSSL itself can be used also be used without PS networking, but
    many of example programs and MatrixSSLNet are based on PS networking.
 */
#  ifndef NO_PS_NETWORKING
#   define USE_PS_NETWORKING
#  endif /* NO_PS_NETWORKING */

/**
    Use the psStat statistics measurement for CL/SL.

    psStat is a generic statistics module. It contains features
    e.g. required for measuring performance.
    These capabilities can only be used on platforms with support for
    thread-local storage and pthreads, such as Linux. Currently the support
    will only be enabled for x86-64 Linux systems.
    If statistics feature is not in use, the performance effect is minimal,
    but for optimal performance in production environment, you may use
    NO_PS_STAT_CL.
 */
#  ifdef __x86_64__
#   ifndef NO_PS_STAT_CL
#    define USE_PS_STAT_CL
#   endif /* NO_PS_STAT_CL */
#  endif /* __x86_64__ */

/**
    Use the psStat statistics measurement for CL/SL by default.

    When psStat support has been compiled in (see above), USE_PS_STAT_CL,
    it is disabled by default. To enabled statistics framework, set
    environment variable PS_ENABLE_STATS to any value. Enabled setting below
    to get statistics measuring without any environment variable. When
    statistics are on by default, they can still be disabled with
    environment variable PS_SKIP_STATS. Disabling statistics is recommended
    to minimize footprint.
 */
/* #  define USE_PS_STAT_CL_BY_DEFAULT */

#endif   /* _h_PS_CORECONFIG */

/******************************************************************************/

