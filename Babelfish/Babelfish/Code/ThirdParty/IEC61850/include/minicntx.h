//*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
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

/*
 *
 *  This file contains
 *  Declarations for Small context for MMS Routines.
 *
 *  This file should not require modification.
 */

#ifndef _MINICNTX_H
#define _MINICNTX_H

#ifdef __cplusplus
extern "C" {
#endif


/* ******** NOTE WELL: *************************************************** */
/* **** This structure is the 'top' of an MMSd_context structure as     ** */
/* **** defined in context.h.  It allows for a short (stack-resident)   ** */
/* **** context for those modules which need to use the parsers but     ** */
/* **** don't actually need the full MMS context - especially the       ** */
/* **** object lists, which take up significant RAM and would likely    ** */
/* **** cause stack overflows on some machines.                         ** */
/* **** DO NOT CALL THE FULL MMS PARSER WITH THIS  STRUCTURE.           ** */
/* *********************************************************************** */
typedef struct {

    MMSd_descriptor Y;                      /* Fetch descriptor for request */
    MMSd_descriptor Emit;                   /* Emit descriptor for response */

    short           objectCount;            /* Index, count for Obj List */
    short           dataCount;              /* Index, count for Data List */
    MMSd_svcCheck   token_check;            /* Enables service neg. checking */
    LENGTH_TYPE     L;                      /* Used for checking CLOSES */
    void            *connection;            /* Info about association */
#if defined(MMSD_MAX_AA_NVLS)
    void            *aa_connections;        /* aa-specific NVLs */
#endif
    void            *userData;              /* implementation specific */
    MMSd_descriptor Alt;                    /* Descriptor for alternate acc. */
    void            *directory;             /* Base of system directory */
    int             directorySize;          /* Size of system directory */
    boolean         isNVL;                  /* I/O for vars or NVL? */
    void            *NVL;                   /* Pointer to NVL */
    int             iCurDomain;
    int             iCurObject;

    TMW_TrackingContext trackingContext;

#if defined(TMW_REPORT_ALL_CLIENT_WRITES)
    void                *pWriteUpdateCache;
#endif

} MMSd_mini_context;

#ifdef __cplusplus
};
#endif

#endif /* _MINICNTX_H */
