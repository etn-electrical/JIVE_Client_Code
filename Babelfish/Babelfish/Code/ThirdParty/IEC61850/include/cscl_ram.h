/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2005-2015 */
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
 *  Declarations for Sample SCL Mapping routines, map to RAM.
 *
 *  This file should not require modification.
 */

#ifndef _CSCL_RAM_H
#define _CSCL_RAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmwtargtrace.h"

TMW_CLIB_API int RAM_MapLnNode(char *pIedName, SCL_GenericNode *pRoot, char *pDomainName,
                  char *pObjectName, SCL_GenericNode *pLN0_or_LN,
                  SCL_LNodeType *pLNodeType, void *pModelHandle, void **ppValue,
                  void *pUserData, TmwTargTraceMessageFunc pErrorMessageFcn,
                  void *pErrorMessageParameter);

TMW_CLIB_API int RAM_MapLnNodeEnd(char *pIedName,
                     SCL_GenericNode *pRoot,
                     char *pDomainName,
                     char *pObjectName,
                     SCL_GenericNode *pLN0_or_LN,
                     SCL_LNodeType *pLNodeType,
                     void **ppValue,
                     void *pUserData,
                     TmwTargTraceMessageFunc pErrorMessageFcn,
                     void *pErrorMessageParameter);

TMW_CLIB_API int RAM_MapNonLeafNode(char *pIedName,
                       SCL_GenericNode *pRoot,
                       char *pDomainName,
                       char *pObjectName,
                       unsigned long *pOffset,
                       void **pValue,
                       void **pMemory,
                       SCL_GenericNode *pTypeNode,
                       SCL_GenericNode *pInstanceNode,
                       void *pUserData,
                       MMSd_indexHandler *pIndexHandler,
                       MMSd_accessHandler *pAccessHandler,
                       void **pIndexHandlerParameter,
                       int *nElementSize,
                       TmwTargTraceMessageFunc pErrorMessageFcn,
                       void *pErrorMessageParameter);

TMW_CLIB_API int RAM_MapNonLeafNodeEnd(char *pIedName,
                          SCL_GenericNode *pRoot,
                          char *pDomainName,
                          char *pObjectName,
                          unsigned long *pOffset,
                          void **pValue,
                          void **pMemory,
                          SCL_GenericNode *pTypeNode,
                          void *pUserData,
                          TmwTargTraceMessageFunc pErrorMessageFcn,
                          void *pErrorMessageParameter);

TMW_CLIB_API int RAM_MapLeafNode(char *pIedName,
                    SCL_GenericNode *pRoot,
                    char *pDomainName,
                    char *pObjectName,
                    SCL_GenericNode *pDA_or_BDA,
                    SCL_RTG_ObjectInfo *pObjectInfo,
                    unsigned long *pOffset,
                    void **pMemory,
                    void *pUserData,
                    TmwTargTraceMessageFunc pErrorMessageFcn,
                    void *pErrorMessageParameter);

TMW_CLIB_API void RAM_InitialValue(char *pIedName,
                      SCL_GenericNode *pRoot,
                      char *pDomainName,
                      char *pObjectName,
                      SCL_GenericNode *pDAI_DA_or_BDA,
                      char *p_bType,
                      char *p_type,
                      char *p_valKind,
                      char *pTextValue,
                      int nDomainIndex,
                      void *pHandle,
                      int nFound,
                      void *pUserData,
                      TmwTargTraceMessageFunc pErrorMessageFcn,
                      void *pErrorMessageParameter);

TMW_CLIB_API int RAM_MapLog(char *pIedName,
               SCL_GenericNode *pRoot,
               char *pDomainName,
               char *pObjectName,
               unsigned char **ppLogBuffer,
               unsigned long *pLogBufferLen,
               void *pUserData,
               TmwTargTraceMessageFunc pErrorMessageFcn,
               void *pErrorMessageParameter);

TMW_CLIB_API void RAM_MapFinished(char *pIedName,
                     SCL_GenericNode *pRoot,
                     MMSd_context *pMmsContext,
                     int nSuccess,
                     void *pUserData,
                     TmwTargTraceMessageFunc pErrorMessageFcn,
                     void *pErrorMessageParameter);

void RAM_ErrorMessage(char *pTextMessage,
                      void *pErrorMessageParameter);

#ifdef __cplusplus
}
;
#endif

#endif /* _CSCL_RAM_H */
