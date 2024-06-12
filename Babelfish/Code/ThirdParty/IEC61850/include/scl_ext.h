/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1997-2015 */
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

/* File: scl_ext.h
 * Description: SCL processing for Goose and Sampled Values subscriptions
 *
 *  This file contains prototypes and type definitions for retrieving Goose
 *  and Sampled Values subscriptions from an SCL file and setting up the
 *  appropriate data structures for mapping the data to the application and
 *  for handling all decoding and delivery of the data from received messages.
 *
 */

#ifndef _SCL_EXT_H
#define _SCL_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TMW_SCL_MAX_MESSAGE_SIZE 512

typedef enum {
    TmwSclService_None,
    TmwSclService_Poll,
    TmwSclService_Report,
    TmwSclService_GOOSE,
    TmwSclService_SMV
} TmwSclService_t;

typedef struct TMW_SCL_SubscribeControlBlockInfo {
    SCL_IED *pSrcIed;
    SCL_Server *pSrcServer;
    SCL_AccessPoint *pSrcAccessPoint;
    SCL_LDevice *pSrcLDevice;
    SCL_GenericNode *pSrcGenericLN;
    SCL_GenericNode *pGenericControlBlock;
    SCL_DataSet *pDataSet;
    SCL_Address *pAddress;
    char *pEncryptionInUse;
    SCL_IED *pDstIed;
    SCL_Server *pDstServer;
    SCL_AccessPoint *pDstAccessPoint;
    SCL_LDevice *pDstLDevice;
    SCL_GenericNode *pDstGenericLN;
    SCL_SCL *pRoot;
    SCL_ExtRef *pExtRef;
} TMW_SCL_SubscribeControlBlockInfo;

typedef struct TMW_SCL_ExtRefMapContext *TMW_SCL_ExtRefMapContextPtr;

typedef void (*TMW_SCL_GooseSubscribeFcn)(
        TMW_SCL_SubscribeControlBlockInfo *pExtRefInfo,
        void *pSubscribeGooseFcnData,
        TMW_SCL_ExtRefMapContextPtr pExtRefMapContext);

typedef void (*TMW_SCL_SampledValueSubscribeFcn)(
        TMW_SCL_SubscribeControlBlockInfo *pExtRefInfo,
        void *pSubscribeSampledValueFcnData,
        TMW_SCL_ExtRefMapContextPtr pExtRefMapContext);

typedef void (*TMW_SCL_ReportSubscribeFcn)(
        TMW_SCL_SubscribeControlBlockInfo *pExtRefInfo,
        void *pSubscribeReportFcnData,
        TMW_SCL_ExtRefMapContextPtr pExtRefMapContext);

typedef struct TMW_SCL_ExtRefMapContext {
    TMW_SCL_GooseSubscribeFcn pHandleGooseSubscribeFcn;
    void *pGooseSubscribeFcnData;

    TMW_SCL_SampledValueSubscribeFcn pHandleSampledValueSubscribeFcn;
    void *pSampledValueSubscribeFcnData;

    TMW_SCL_ReportSubscribeFcn pHandleReportSubscribeFcn;
    void *pReportSubscribeFcnData;

    void *pUserPtr;
} TMW_SCL_ExtRefMapContext;

SCL_IED *TMW_LocateIED(SCL_SCL *pRoot, char *pIedName);
SCL_AccessPoint *TMW_LocateAccessPoint(SCL_IED *pIed, char *pApName);
SCL_LDevice *TMW_LocateLDevice(SCL_Server *pServer, char *pLDInst);
SCL_GenericNode *TMW_LocateLN(SCL_LDevice *pLDevice, char *pPrefix,
                              char *pLnClass, char *pLnInst);
SCL_DataSet *TMW_SCL_LocateDataSet(SCL_GenericNode *pGenericLN,
                                   char *pDataSetName);
int TMW_SCL_LocateDAValue(SCL_SCL *pRoot, char *pLDName, char *pLnPrefix, char *pLnClass, char *pLnInst,
        char *pDoName, char *pSDoName, char *pDaName, char *pSDaName, char *pDaVal);
int TMW_SCL_EqualStrings(char *pStr1, char *pStr2);
SCL_FCDA *TMW_SCL_LocateDataSetMember(SCL_DataSet *pDataSet, char *pIedName,
                                      char *pLdInst, char *pLnPrefix,
                                      char *pLnClass, char *pLnInst,
                                      char *pDoName, char *pDaName);
SCL_GenericNode *TMW_LocateControlBlock(SCL_GenericNode *pGenericLN,
                                        TmwSclService_t nServiceType,
                                        char *pCbName);

/*
 * Extract all valid subscriptions based on ExtRefs for named IED
 */
int TMW_SCL_ConfigureSclSubscriptions(
        char *pFileName, char *pIedName,
        TMW_SCL_ExtRefMapContext *pExtRefMapContext);
/*
 * Find ethernet addresses from SCL
 */
SCL_Address *TMW_SCL_find_GSE_Address(char *pIedName, char *pApName,
                                      char *pLdInst, char *pCbName,
                                      char **pEncryptionInUse,
                                      SCL_GenericNode *pRoot);
SCL_Address *TMW_SCL_find_SMV_Address(char *pIedName, char *pApName,
                                      char *pLdInst, char *pCbName,
                                      char **pEncryptionInUse,
                                      SCL_GenericNode *pRoot);

#ifdef __cplusplus
};
#endif

#endif /* _SCL_EXT_H */
