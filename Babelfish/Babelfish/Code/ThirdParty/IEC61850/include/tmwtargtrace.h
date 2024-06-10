/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c)      2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                       www.TriangleMicroWorks.com                          */
/*                          (919) 870-6615                                   */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/** @tmwtargtrace.h
 *
 */

#ifndef _TMW_TARG_TRACE_H
#define _TMW_TARG_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

#define TMW_TARG_TRACE_MAX_BUFFER           512

/**
 * Select the severity level to display in the trace mechanism.
 */
typedef enum {
    TmwTargTraceLevelCritical   = 0x0001,
    TmwTargTraceLevelError      = 0x0002,
    TmwTargTraceLevelWarn       = 0x0004,
    TmwTargTraceLevelInfo       = 0x0008,
    TmwTargTraceLevelDebug      = 0x0010,
} TmwTargTraceLevel_t;

/**
 * Masks to filter messages from parts of the code.
 */
typedef enum {
    TmwTargTraceMaskNone        = 0x0000,
    TmwTargTraceMaskScl         = 0x0001,
    TmwTargTraceMaskStack       = 0x0002,
    TmwTargTraceMaskAcsi        = 0x0004,
    TmwTargTraceMaskTest        = 0x0008,
    TmwTargTraceMaskApp         = 0x0010,
    TmwTargTraceMaskExtref      = 0x0020,
    TmwTargTraceMaskFile        = 0x0040,
    TmwTargTraceMaskTransport   = 0x0080,
    TmwTargTraceMaskTransLow    = 0x0100,
    TmwTargTraceMaskClient      = 0x0200,
    TmwTargTraceMaskClientRpt   = 0x0400,
    TmwTargTraceMaskClientState = 0x0800,
    TmwTargTraceMaskTime        = 0x1000,
    TmwTargTraceMaskTarget      = 0x2000,
    TmwTargTraceMaskDynNvls     = 0x4000,
    TmwTargTraceMaskAll         = 0xFFFF,
} TmwTargTraceMask_t;

typedef void (* TmwTargTraceMessageFunc)( TmwTargTraceMask_t mask, TmwTargTraceLevel_t level, ...);
typedef void (* TmwTargTraceOutputFunc)( TmwTargTraceMask_t mask, TmwTargTraceLevel_t level, char * message );

#if defined(TMW_USE_TARG_TRACE)


/**
 * purpose: override the output function at runtime
 *
 * arguments: function pointer
 *
 * returns: none.
 */
TMW_CLIB_API void tmwTargSetTraceOutputFunc(TmwTargTraceOutputFunc t);

/**
 * purpose: change the trace mask at runtime
 *
 */
TMW_CLIB_API void tmwTargSetMask( TmwTargTraceMask_t m );


/**
 * purpose: change the trace level at runtime
 *
 */
TMW_CLIB_API void tmwTargSetLevel(  TmwTargTraceLevel_t l);

/**
 * purpose: Print a text string to the debug interface
 *
 * arguments: printBuffer - pointer to a buffer containing the debug text
 *
 * NOTES: The default debug interface is target dependent.
 *
 * returns: none.
 */
void tmwTargTraceOutput(char *printBuffer);


/**
 * purpose: Format and print traces messages based on the current debug mask/
 *          and level
 *
 * arguments: traceMask   - Mask indicating the trace type of the message
 *             traceLevel  - Mask indicating the trace level of the message
 *             numParams   - Number of parameters passed with the message
 *             format      - pointer to a variable length string containing
 *                           the debug text
 *
 * NOTES: This tracing function uses the standard C variable argument list.
 *        This provides a general purpose example that handles parameter
 *        types that can be type cast to a void* type. All the tracing calls
 *        provided in the library adhere to this. However if additional
 *        tracing is added with parameter types than cannot be cast to a
 *        void* type, then this function may require modification.
 *
 * returns: none.
 */
TMW_CLIB_API void tmwTargTraceMsg(TmwTargTraceMask_t traceMask,
                     TmwTargTraceLevel_t traceLevel, ...);

#endif /* defined(TMW_TARG_TRACE_MSG) */

#ifdef __cplusplus
};
#endif

#endif /* _TMW_TARG_TRACE_H */

