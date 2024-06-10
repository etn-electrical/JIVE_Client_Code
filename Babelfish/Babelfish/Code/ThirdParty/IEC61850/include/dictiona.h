/*****************************************************************************/
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
 *  Declarations for MMSd Dictionary.
 *
 *  This file should not require modification.
 */


#ifndef _DICTIONA_H
#define _DICTIONA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmwtarg.h"

#include "context.h"
#include "ber.h"



#include "journal.h"

/* MMSd Dictionary
   ---
   --- The following definitions are used to construct and access
   --- the MMSd dictionaries. To conserve code size, all dictionaries
   --- share a common record format and utilities.  These records
   --- include the dictionary entry name and the associated handlers
   --- and parameters.
   ---
   --- These dictionaries are initialized outside of the MMSd Server,
   --- and are passed in via the context records for the association.
   -----------------------------------------------------------
*/

/* -- Definitions for getNameList support */

typedef enum {
    MMSd_noMoreFollows = 0,
    MMSd_moreFollows = 1
}
emitState;

/* -- NV_Type */

typedef MMSd_dictionaryIndex dictionaryIndex;

/* Function pointer definitions for handler types */
typedef int  (* MMSd_ReadHandler)( void * N, int size, void * parameter, void * E );
typedef void (* MMSd_WriteHandler)( void * N, void *D, int size, void * parameter, void * E );
typedef int  (* MMSd_NameHandler)( emitState *PS, void *N, int size, void * parameter, void * E );

#if defined(MMSD_ATTR_HANDLER_NAME)
typedef int (* MMSd_AttributeHandler)( void *N, int size, void * parameter, void * E );
#else
typedef int (* MMSd_AttributeHandler)( int size, void * parameter, void * E );
#endif

typedef struct {
    int     size;
    struct {
        void                    *parameter;
        MMSd_ReadHandler        rhandler;
    } readHandler;
    struct {
        void                    *parameter;
        MMSd_WriteHandler       whandler;
    } writeHandler;
    struct {
        void                    *parameter;
        MMSd_NameHandler        nhandler;
    } nameHandler;
    struct {
        void                    *parameter;
        MMSd_AttributeHandler   ahandler;
    } attributeHandler;
} handler_TYPE;

/* This is always used, MMSD_DYNAMIC_NVLS only controls enabling the services */
#define MMSD_DYNAMIC_NVLS_LOGIC 1

typedef struct NV_type {
    char            *name;
    handler_TYPE    H;
#if defined(MMSD_DYNAMIC_NVLS_LOGIC)
    int             handler_index;
#endif
    int             trackingCdcOptions;          /* CdcClassIndex and options for matching */
    int             trackingLengthOrEnumId;      /*     entries with dictionary entries */
    int             trackingIndex;               /* index into tracking object tracking descriptors */
    MMSd_descriptor **pNvlMemberAltAccess;       /* array of descriptor pointers */
} NV_type;


/* -- Named variable lists
   -- NVL_explicit_type: explicit named variable list
   -- NVL_set_type: defines a collection of NVLs for a domain
*/

typedef struct {
    MMSd_ushort             domainIndex;      /* The directory index for the domain */
    MMSd_dictionaryIndex    dictionaryIndex;  /* Dictionary position */
} NVL_explicit_type;

typedef struct {
    char * name;                /* Name for the named variable list */
    enum { isList, isHandler } kind;
    MMSd_ushort NVL_length;     /* Number of elements on the explicit list */
    union {
        void * _x_placeholder;    /* Simplifies initialization */
        NVL_explicit_type    *NVL;
        handler_TYPE *handlers;
    } u;
} NVL_set_type;

typedef int (* MMSd_evalFunction)( void *val, int siz, void *save, void *db, void *scale, unsigned char *reason );
typedef void * (* MMSd_indexHandler)( int index, void *parameter, int size ); /* V9.1 */

typedef int MMSd_HandlerParamDefs;

#define MMSd_DEF_READP    0x0000
#define MMSd_NUL_READP    0x0001
#define MMSd_DIC_READP    0x0002
#define MMSd_DOM_READP    0x0004
#define MMSd_DEF_WRITEP   0x0000
#define MMSd_NUL_WRITEP   0x0010
#define MMSd_DIC_WRITEP   0x0020
#define MMSd_DOM_WRITEP   0x0040
#define MMSd_DEF_ATTRP    0x0000
#define MMSd_NUL_ATTRP    0x0100
#define MMSd_DIC_ATTRP    0x0200
#define MMSd_DOM_ATTRP    0x0400
#define MMSd_DEF_NAMEP    0x0000
#define MMSd_NUL_NAMEP    0x1000
#define MMSd_DIC_NAMEP    0x2000
#define MMSd_DOM_NAMEP    0x4000

#define MMSD_READP_MASK   0x000F
#define MMSD_WRITEP_MASK  0x00F0
#define MMSD_ATTRP_MASK   0x0F00
#define MMSD_NAMEP_MASK   0xF000

typedef enum SizeKind {
    sizeNormal,     /* Size is in bytes, no special processing */
    sizeBytes,      /* Size is in bytes, make typedef char[x] */
    sizeChar,       /* Size is in char, make typedef char[x+1] */
    sizeBits        /* Size is in bits, make typedef char[x mod 8 + 1] */
} SizeKind;

typedef struct {
    const char             *name;
    handler_TYPE            H;
    struct {
        void                    *parameter;
        MMSd_evalFunction       ehandler;
    } evalHandler;
    SizeKind                sizeKind;
    MMSd_HandlerParamDefs   paramdef;
    MMSd_indexHandler       indexHandler;
    int                     dimension;
    int                     cTypeSize;
} MMSd_ExtendedHandlerSet;

typedef struct MMSd_NvlComponentEval {
    MMSd_evalFunction    func;
    void                 *value;
    int                  valueSize;
    void                 *save;
    void                 *dbValue;
    void                 *scale;
    int                  bitNum;
} MMSd_NvlComponentEval;

typedef struct MMSd_NvlEval {
    int                     nvlsize;
    NVL_explicit_type      *members;
    MMSd_descriptor         **pMemberAltAccess;
    MMSd_NvlComponentEval   *componentEval;
    int                     numcomps;
    unsigned char           *inclusion;
    unsigned char           *reason;
    unsigned char           *trgs;
#if defined(TMW_USE_JOURNALS)
    MMSd_Journal            journal;
#endif
} MMSd_NvlEval;

/* -- The system directory structure
   --
   -- The system directory is represented as an ARRAY of elements
   -- of directory Type.  The first (i.e. 0th) element is the
   -- dictionary for the VMD specific elements.
   --
   -- All subsequent entries are for the DOMAINS of the VMD.  Thus
   -- DICT[ 1 ] contains the dictionary (with Named Variable Lists,
   -- etc.) for domain(1); DICT[ 2 ] for domain(2), etc.
*/

typedef struct {
    char * name;                /* The domain name.  NULL for VMD */
    MMSd_dictionaryIndex NV_size;        /* Number of entries for dictionary */
    MMSd_ushort NVL_size;       /* Number of NVLs defined for entry */
    NV_type    * dictionary;    /* Dictionary pointer for domain */
    NV_type * named_variable_list;    /* Pointer to NVL data structure */
#if defined(MMSD_DYNAMIC_NVLS_LOGIC)
    dictionaryIndex *nvl_indices;          /* Index of NVLs sorted by name */
    dictionaryIndex nvl_static;            /* Index of NVLs sorted by name */
    void *primitive_handlers;              /* Defined primitive types */
    int   num_primitive_handlers;
#endif
    void *listOfJournals;                   /* List of journals in VMD only */
    int  JRN_size;
//FIXME??
    void *named_variable_list_eval;        /* MMSd_NvlEval from CASM.H */
    void *report_control_blocks;           /* MMSd_ReportControl from CASM.H */
    void *log_control_blocks;              /* MMSd_ReportControl from CASM.H */
    void *goose_control_blocks;            /* MMSd_ReportControl from CASM.H */
    void *gse_control_blocks;              /* MMSd_ReportControl from CASM.H */
    void *msmv_control_blocks;             /* MMSd_ReportControl from CASM.H */
    void *usmv_control_blocks;             /* MMSd_ReportControl from CASM.H */
    void *sg_control_blocks;               /* SGCB from CASM.H               */
    void *tracking_objects;                /* TMW_TrackingObject from CASM.H */
    void *pCdcClassList;                   /* List of Common Data Classes   */
    int  nCdcClassList;                    /* Number of Common Data Classes */
    void *pCdcClassMemberList;             /* List of CDC Members           */
    int  nCdcClassMemberList;              /* Number of CDC Members         */
    void *pMmsHandlers;                    /* List of MMS Primitive Types   */
    int  nMmsHandlers;                     /* Number of MMS Primitive Types */
#if defined(TMW_USE_THREADS)
    TmwTargSem_t sem;
#endif
    int moreChanges;

    unsigned char   modeControl;        /* Current value of mod for this LD */
    unsigned char   behStatus;          /* Current value of beh for this LD */
    unsigned char   simControl;         /* Current value of sim for this LD */

    int             modStVal;           /* Index of LLN0.Mod.stVal in this LD */
    int             behStVal;           /* Index of LLN0.Beh.stVal in this LD */
    int             simStVal;           /* Index of LPHD.Sim.stVal in this LD */

    /* These should be MMSd_ushort, but we use -1 for when they are NULL */
    int             nParentLD;          /* Parent LD -per GrRef */
    int             nChildLD;           /* First child LD -per GrRefs (follow peers for list) */
    int             nPeerLD;            /* List of peer LDs -per GrRefs */
} directoryType;

#define MMSd_NORMAL_ARRAY_FLAG 0x1000
#define MMSd_CONSTR_ARRAY_FLAG 0x4000
#define MMSd_MEMBER_ARRAY_FLAG 0x2000
#define MMSd_ARRAY_FLAG 0x7000

#define MMSd_STRUCT_SIZE(s)   ( ((s) < 0) ? (s) \
                                          : (s & ~MMSd_ARRAY_FLAG) )

#define MMSd_ARRAY_SIZE(s)    (s & ~MMSd_ARRAY_FLAG)
#define MMSd_NormArraySize(s) (s | MMSd_NORMAL_ARRAY_FLAG)
#define MMSd_ConsArraySize(s) (s | MMSd_CONSTR_ARRAY_FLAG)
#define MMSd_MembArraySize(s) (s | MMSd_MEMBER_ARRAY_FLAG)

#define MMSd_IsArray(s)       ( ((s) < 0) ? FALSE \
                                          : ((s) & MMSd_ARRAY_FLAG) )
#define MMSd_IsNormArray(s)   ( ((s) < 0) ? FALSE \
                                          : ((s) & MMSd_NORMAL_ARRAY_FLAG) )
#define MMSd_IsConsArray(s)   ( ((s) < 0) ? FALSE \
                                          : ((s) & MMSd_CONSTR_ARRAY_FLAG) )
#define MMSd_IsMembArray(s)   ( ((s) < 0) ? FALSE \
                                          : ((s) & MMSd_MEMBER_ARRAY_FLAG) )

/* These are the types that the users should use. */
typedef NV_type             MMSd_entry;         /* For NVs and NVLs */
typedef NVL_explicit_type   MMSd_list_entry;    /* For structures and lists */
typedef directoryType       MMSd_domain;        /* For vmds and domains */

typedef enum {
    altREAD,
    altWRITE,
    altCHECK
} MMSd_altService;

/* This handler is used for accessing attributes of array element structures */
typedef int (* MMSd_accessHandler)( char *memberName,           /* Name of attribute */
                                    int  memberNum,             /* Index of attribute in structure */
                                    MMSd_altService service,    /* altREAD or altWRITE */
                                    MMSd_entry *pMemberEntry,   /* Dictionary entry of attribute */
                                    void *parentParam,          /* Param generated for parent structure */
                                    void **resultParam );       /* new param to pass to read/write handler */


typedef struct MMSd_array_entry {
    MMSd_ushort          domainIndex;
    MMSd_dictionaryIndex dictionaryIndex;
    MMSd_indexHandler    indexHandler;
    void                *pIndexParameter;
    MMSd_accessHandler   accessHandler;
    long                 elementSize;
    long                 numElements;
} MMSd_array_entry;

typedef struct MMSd_AlternateAccessState {
    MMSd_array_entry    *pArrayEntry;
    void                *pParameter;
} MMSd_AlternateAccessState;

int MMSd_alternateAccess( MMSd_context *_CNTX,
                          MMSd_altService service,
                          MMSd_AlternateAccessState *pAccessState,
                          MMSd_descriptor *N,
                          MMSd_descriptor *A,
                          MMSd_descriptor *D,
                          MMSd_descriptor *E,
                          MMSd_entry *entry );

int MMSd_executeAccessHandler( MMSd_altService              nService,
                               MMSd_AlternateAccessState    *pAccessState,
                               int                          nAttributeNum,
                               MMSd_entry                   *pDictEntry,
                               void                         **pNewParamValue );


#define NULL_HANDLER ((void *)0L)        /* Useful when no handlers specified */

/* Domain Attributes
   --
   -- Used for MMS getDomainAttributes service.
   --------------------------------------------------------------
*/

typedef enum {
    MMSd_Domain_Non_Existent,  /* Shall not be reported */
    MMSd_Domain_Loading,
    MMSd_Domain_Ready,
    MMSd_Domain_In_Use,
    MMSd_Domain_Complete,
    MMSd_Domain_Incomplete
} MMSd_DomainState;

typedef struct {
    char               *capabilities;
    MMSd_DomainState    state;
    int                 sharable;
    int                 uploads;
    unsigned char       deletable;
    void               *programs;
} MMSd_DomainAttributes;


int MMSd_encodeDomainAttr( MMSd_DomainAttributes *attributes,
                           MMSd_descriptor *E );

/* Dictionary Searching
   --
   -- All dictionary lookups use this routine.
   --------------------------------------------------------------
*/

boolean MMSd_find_Domain( directoryType * directory,
                          int sizeOfDirectory,
                          MMSd_descriptor *name,  /* Descr of name field in req */
                          int * INDEX );

typedef enum {
    MMSd_searchFound,
    MMSd_searchAfter,
    MMSd_searchNone
} MMSd_SearchType;

MMSd_SearchType MMSd_matchSearch(NV_type    *dictionary,
                                 dictionaryIndex sizeOfDict,
                                 MMSd_descriptor *name,
                                 dictionaryIndex *INDEX );

MMSd_SearchType MMSd_indexedMatchNameSearch( NV_type            *dictionary,
        dictionaryIndex    *indices,
        dictionaryIndex    sizeOfDict,
        char               *name,
        int                nameLen,
        dictionaryIndex    * INDEX );

MMSd_SearchType MMSd_indexedMatchSearch( NV_type            *dictionary,
        dictionaryIndex    *indices,
        dictionaryIndex    sizeOfDict,
        MMSd_descriptor    *name,
        dictionaryIndex    * INDEX );

#define MMSD_LOOKUP_NV_ENTRY(cntxt, domain, index) \
    &( ((directoryType *) (cntxt)->directory)[ domain ].dictionary[ index ] )

#define MMSD_ENTRY( dictionary, indices, x ) &dictionary[ x ]

#define MMSD_EVAL( evalList, indices, x ) &( (MMSd_NvlEval *) (evalList))[ x ]

#if defined(MMSD_DYNAMIC_NVLS_LOGIC)

#define MMSD_INDEXED( dictionary, indices, x ) \
    ( indices == NULL  ? x \
                                  : indices[ x ] )

#define MMSD_INDEXED_ENTRY( dictionary, indices, x ) \
    ( indices == NULL  ? &dictionary[ x ] \
                                  : &dictionary[ indices[ x ] ] )

#define MMSD_INDEXED_EVAL( evalList, indices, x ) \
    ( indices == NULL  ? &( (MMSd_NvlEval *) (evalList))[ x ] \
                                  : &( (MMSd_NvlEval *) (evalList))[ indices[ x ] ] )

#define MMSD_LOOKUP_DOMAIN_NVL_ENTRY(pDomain, index) \
    MMSD_ENTRY(  \
        (pDomain)->named_variable_list, \
        (pDomain)->nvl_indices, \
        index )

#define MMSD_LOOKUP_DOMAIN_NVL_EVAL( pDomain, nvlIndex) \
    MMSD_EVAL( (pDomain)->named_variable_list_eval, \
                       (pDomain)->nvl_indices, \
                       nvlIndex )

#if defined(MMSD_MAX_AA_NVLS)
#define MMSD_LOOKUP_NVL_ENTRY(cntxt, domain, index) \
    ( ( domain >= 0 ) ? \
    MMSD_ENTRY(  \
        ((directoryType *) (cntxt)->directory)[ domain ].named_variable_list, \
        ((directoryType *) (cntxt)->directory)[ domain ].nvl_indices, \
        index ) \
      : \
    MMSD_ENTRY(  \
        ((MMSd_Connection *) (cntxt)->connection)->aaDomain.named_variable_list, \
        ((MMSd_Connection *) (cntxt)->connection)->aaDomain.nvl_indices, \
        index ) )

#define MMSD_LOOKUP_NVL_EVAL(cntxt, domain, nvlIndex) \
    ( (domain >= 0 ) \
        ? ( MMSD_LOOKUP_DOMAIN_NVL_EVAL(  \
                            &( (directoryType *) (cntxt)->directory)[domain], \
                            nvlIndex ) ) \
        : ( ( domain < -1 ) \
                ? FALSE \
                : ( MMSD_LOOKUP_DOMAIN_NVL_EVAL(  \
                                &((MMSd_Connection *) \
                                    (cntxt)->connection)->aaDomain, \
                                nvlIndex ) ) ) )


#define MMSD_CAN_DELETE_NVL( cntxt, dindex, nvlIndex ) \
    ( ( dindex >= 0 ) \
       ?  ( dindex >= (cntxt)->directorySize )  \
            ? FALSE \
            : ( ( (cntxt)->directory == NULL ) \
                ? FALSE \
                : ( nvlIndex >= \
                    ((directoryType *) (cntxt)->directory)[dindex].nvl_static ) \
                    ? TRUE \
                    : FALSE ) \
       : ( ( dindex < -1 ) \
           ? FALSE \
           : ( ( nvlIndex >= \
                ( (MMSd_Connection *)  \
                        (cntxt)->connection)->aaDomain.nvl_static ) ) ) )
#else
#define MMSD_LOOKUP_NVL_ENTRY(cntxt, domain, index) \
    ( (domain >= 0 ) ? \
    MMSD_ENTRY(  \
        ((directoryType *) (cntxt)->directory)[ domain ].named_variable_list, \
        ((directoryType *) (cntxt)->directory)[ domain ].nvl_indices, \
        index ) \
      : \
    NULL )

#define MMSD_LOOKUP_NVL_EVAL(cntxt, domain, nvlIndex) \
    ( (domain >= 0 ) \
        ? ( MMSD_LOOKUP_DOMAIN_NVL_EVAL(  \
                            &( (directoryType *) (cntxt)->directory)[domain], \
                            nvlIndex ) ) \
        : NULL )

#define MMSD_CAN_DELETE_NVL( cntxt, dindex, nvlIndex ) \
    ( ( dindex >= 0 ) \
       ?  ( dindex >= (cntxt)->directorySize )  \
            ? FALSE \
            : ( ( cntxt->directory == NULL ) \
                ? FALSE \
                : ( nvlIndex >= \
                    ((directoryType *) (cntxt)->directory)[dindex].nvl_static ) \
                    ? TRUE \
                    : FALSE ) \
       : FALSE )

#endif

#else   /* MMSD_DYNAMIC_NVLS_LOGIC */

#define MMSD_INDEXED( dictionary, indices, x ) (x)

#define MMSD_INDEXED_ENTRY( dictionary, indices, x ) (&dictionary[(x)])

#define MMSD_LOOKUP_NVL_ENTRY(cntxt, domain, index) \
    &( ((directoryType *) (cntxt)->directory)[ domain ].named_variable_list[ index ] )

#define MMSD_CAN_DELETE_NVL( cntxt, dindex, nvlIndex ) FALSE

#endif  /* MMSD_DYNAMIC_NVLS_LOGIC */

MMSd_SearchType MMSd_DOMAIN_matchSearch(
    directoryType *directory,
    dictionaryIndex sizeOfDict,
    MMSd_descriptor *name,  /* Descr of name field in req */
    dictionaryIndex * INDEX ); /* Dictionary index to return */


/* ------------------------------------Prototypes for initializer function.
   --
   -- The initializer function must be supplied by the user.
   -- using the following calling sequence.  This routine
   -- is used to initialize the user's directory and dictionary
   -- structure from a STATIC configuration.
*/

void MMSd_initializeDirectory( directoryType **directoryPtr, int *directoryLen);

/* ------------------------------------Prototype MMSd_READ_Object
   --
   -- This function allows direct access to evaluation of dictionary
   -- entries, primarily for generation of infoReports.  The actual
   -- body of the function is in IOROUTIN.C, it is called there and also
   -- by CASM.C, where the user-visible form of the function
   -- (readDictionaryObject) is found.
*/

MMSd_length MMSd_READ_Object( MMSd_context *pMmsContext,
                              NV_type *entry,
                              boolean isNVL,
                              MMSd_descriptor *N,
                              MMSd_descriptor *alt,
                              MMSd_descriptor *E );

void MMSd_WRITE_Object( MMSd_context *pMmsContext,
                        NV_type *entry,
                        boolean isNVL,
                        MMSd_descriptor *N,
                        MMSd_descriptor *D,
                        MMSd_descriptor *alt,
                        MMSd_descriptor *E );

int MMSd_getAltList( MMSd_descriptor    *alist,
                     MMSd_descriptor    *dlist,
                     MMSd_descriptor    *A,
                     MMSd_descriptor    *D,
                     int                max_entries,
                     MMSd_altService    service );

#ifdef __cplusplus
};
#endif

#endif /* _DICTIONA_H */
