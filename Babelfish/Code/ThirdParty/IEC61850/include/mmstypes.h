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
 *  definitions for Basic MMS Data Types.
 *
 *  This file will not require modification.
 */

#ifndef _MMSTYPES_H
#define _MMSTYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#define MMSD_CHECK_READ_ARGS \
    /* To avoid compiler warnings */ \
    if ( N ) \
    {}; \
    if ( size )\
    {}; \
    if ( parameter )\
    {};

#define MMSD_CHECK_WRITE_ARGS \
    /* To avoid compiler warnings */ \
    if ( N ) \
    {}; \
    if ( size )\
    {}; \
    if ( parameter )\
    {}; \
    if ( E )\
    {};

#define MMSD_CHECK_ATTR_ARGS \
    /* To avoid compiler warnings */ \
    if ( size )\
    {}; \
    if ( parameter )\
    {};

#define MMSd_CHECK_DESC_ERRORS( _a_, _b_ ) \
                ( (((MMSd_descriptor *) _a_ )->errorFlag) \
                || (((MMSd_descriptor *) _b_ )->errorFlag) )

#define MMSd_NV(nv_name, nv_type, nv_size, nv_data, nv_nhandler) \
    { nv_name, \
        { nv_size,  \
            { (void *) nv_data, MMSd_read##nv_type }, \
            { (void *) nv_data, MMSd_write##nv_type }, \
            { (void *) 0, nv_nhandler },\
            {             MMSd_attr##nv_type } \
        } \
    }

typedef  char                    MMSd_Boolean;
typedef  unsigned char           MMSd_BitString;
typedef  int                     MMSd_Integer;
typedef  long                    MMSd_LongInteger;
typedef  unsigned int            MMSd_Unsigned;
typedef  unsigned long           MMSd_LongUnsigned;
#ifdef MMSD_FLOAT_DATA_TYPE
typedef  MMSD_FLOAT_DATA_TYPE    MMSd_Float;
#else
typedef  int                     MMSd_Float;
#endif
#ifdef MMSD_FLOAT64_DATA_TYPE
typedef  MMSD_FLOAT64_DATA_TYPE  MMSd_Float64;
#else
typedef  int                     MMSd_Float64;
#endif
typedef  char                    MMSd_OctetString;
typedef  char                    MMSd_String;
typedef  char                    MMSd_GeneralTime;
typedef  MMSd_time               MMSd_Btime;
typedef  int                     MMSd_BCD;
typedef  int                     MMSd_ObjectId;
#if defined(MMSD_UNICODE_TYPE)
typedef  MMSD_UNICODE_TYPE       MMSd_MmsString;
#else
typedef  char                    MMSd_MmsString;
#endif

/**
 * Definitions for MMS standardized types
 */

#define MMSD_OBJECT_DOMAIN_LEN  64
#define MMSD_OBJECT_VAR_LEN     64
#define MMSD_OBJECT_NAME_LEN ( MMSD_OBJECT_DOMAIN_LEN + 1 + MMSD_OBJECT_VAR_LEN + 1 )
#define MMSD_MAX_STRING_LEN     129   /* VISIBLE STRING 129 */


/**
 * MMS scope can be vmd (IED), domain(LD), or aa(connection)
 */
typedef enum
{
    MMSdScope_notSet = -1,
    MMSdScope_vmdSpecific,
    MMSdScope_domainSpecific,
    MMSdScope_aaSpecific
} MMSdScope_t;

/**
 *
 */
typedef enum {
    MMSdObjectClass_namedVariable,
    MMSdObjectClass_scatteredAccess,
    MMSdObjectClass_namedVariableList,
    MMSdObjectClass_namedType,
    MMSdObjectClass_semaphore,
    MMSdObjectClass_eventCondition,
    MMSdObjectClass_eventAction,
    MMSdObjectClass_eventEnrollment,
    MMSdObjectClass_journal,
    MMSdObjectClass_domain,
    MMSdObjectClass_programInvocation,
    MMSdObjectClass_operatorStation
} MMSdObjectClass_t;


/**
 *
 */
typedef struct MMSd_ObjectName {
    MMSdScope_t     scope;
    char            domain[MMSD_OBJECT_DOMAIN_LEN + 1];
    char            name[MMSD_OBJECT_VAR_LEN + 1];
} MMSd_ObjectName;

typedef struct MMSd_Enumerated {
    char value;
    int min;
    int max;
} MMSd_Enumerated;


typedef struct MMSd_Counter {
    int value;
    int changedFlag;
} MMSd_Counter;


typedef struct MMSd_LongCounter {
    long value;
    int changedFlag;
} MMSd_LongCounter;


typedef struct MMSd_CounterUtcBTime {
    MMSd_UtcBTime value;
    int changedFlag;
} MMSd_CounterUtcBTime;


typedef enum {
    MMSd_EC_disabled,
    MMSd_EC_idle,
    MMSd_EC_active,
    MMSd_EC_error
} MMSd_EventConditionType;


#ifdef __cplusplus
};
#endif

#endif          /* _MMSTYPES_H */
