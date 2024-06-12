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
 *  Declarations for MMS Object List routines.
 *
 *  This file should not require modification.
 */

#ifndef _MMS_QUEU_H
#define _MMS_QUEU_H

#ifdef __cplusplus
extern "C" {
#endif

#define MMSd_initializeObjectNameList()                 \
{                                   \
    _CNTX->objectCount = 0;                    \
    _CNTX->dataCount = 0;                      \
}

#define MMSd_queueDataFailure()                     \
{                                   \
    MMSd_setDescErrorFlag( MMSd_queueOverrun, _CNTX->Emit );   \
    YYACCEPT;                           \
}

#define MMSd_queueSearchFailure()                   \
{                                   \
    if ( _CNTX->objectCount >= MMSD_MAX_OBJECT_LIST_COUNT )    \
        MMSd_queueDataFailure()                 \
    else                                \
    {                               \
        MMSd_setDescErrorFlag(                  \
            MMSd_data_object_non_existent,         \
            _CNTX->objectList[ _CNTX->objectCount ].object);\
        _CNTX->objectCount++;                  \
    }                               \
}

#define MMSd_queueUnimplementedFailure()                \
{                                   \
    if ( _CNTX->objectCount >= MMSD_MAX_OBJECT_LIST_COUNT )    \
        MMSd_queueDataFailure()                 \
    else                                \
    {                               \
        MMSd_setDescErrorFlag(                  \
            MMSd_data_object_access_unsupported,        \
            _CNTX->objectList[ _CNTX->objectCount ].object);\
        _CNTX->objectCount++;                  \
    }                               \
}

#define  MMSd_queueObjectName( _TKN_, _domain_, _index_ )       \
{                                   \
    if ( _CNTX->objectCount >= MMSD_MAX_OBJECT_LIST_COUNT )    \
        MMSd_queueDataFailure()                 \
    else                                \
    {                               \
        MMSd_ObjectListEntry *ptr \
                    = &_CNTX->objectList[ _CNTX->objectCount ]; \
        ptr->object = _TKN_; \
        ptr->domainIndex = _domain_;\
        ptr->dictionaryIndex = _index_;\
        MMSd_setDescErrorFlag( MMSd_null_descriptor, ptr->altAccess ); \
        MMSd_setDescErrorFlag( MMSd_noHandlerErrors, ptr->object );\
        _CNTX->objectCount++;                  \
    }                               \
}

#define  MMSd_queueData( _TKN_ )                    \
{                                   \
    if ( _CNTX->dataCount >= MMSD_MAX_OBJECT_LIST_COUNT )    \
        MMSd_queueDataFailure()                 \
    else                                \
    {                               \
        _CNTX->objectList[ _CNTX->dataCount ].data = ( _TKN_ );\
        _CNTX->dataCount++;                    \
    }                               \
}

#define  MMSd_queueAltAccess( _TKN_ )                    \
{                                   \
    if ( _CNTX->objectCount <= 0 )    \
        MMSd_queueDataFailure()                 \
    else \
    { \
        _CNTX->objectList[ _CNTX->objectCount - 1 ].altAccess = ( _TKN_ );\
        MMSd_setDescErrorFlag(                  \
            MMSd_noHandlerErrors,               \
            _CNTX->objectList[ _CNTX->objectCount - 1].altAccess);  \
    } \
}

#ifdef __cplusplus
}
;
#endif

#endif /* _MMS_QUEU_H */
