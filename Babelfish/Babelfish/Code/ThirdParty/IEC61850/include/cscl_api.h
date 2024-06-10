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
 *  Declarations for CSCL API.
 *
 *  This file should not require modification.
 */

#ifndef _CSCL_API_H
#define _CSCL_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmwtargtrace.h"

typedef struct SCL_RTG_InstanceData {
    int             ix;
    SCL_GenericNode *pInstanceNode; /* DOI, SDI, or DAI */
} SCL_RTG_InstanceData;

typedef struct SCL_RTG_ObjectInfo {
    int             typeCode;       /* Input to RTG */
    int             dimension;      /* Input to RTG */
    int             options;        /* Input to RTG */
    int             mmsSize;        /* Input to RTG */
    void            *pValue;        /* Input to RTG */
    int             sclTypeSize;    /* Derived from SCL */
    int             sclTypeCode;    /* Derived from SCL */
    unsigned char   sclTypeFlags;   /* Derived from SCL */
    int             cTypeSize;      /* C Size based on SCL definition */
    int             nDimension;     /* Input to RTG */
    char            *bType;         /* From raw SCL */
    char            *type;          /* From raw SCL */
    char            *count;         /* From raw SCL */
    char            *valKind;       /* From raw SCL */
    char            *sAddr;         /* From raw SCL */
    char            *sAddrInstance; /* From raw SCL */
    SCL_GenericNode *pInstanceNode; /* DOI or DAI   */
    int             nLineNo;        /* From raw SCL */
    SCL_RTG_InstanceData *pArrayInstanceData;   /* From SCL */
    int                  nArrayInstanceDataSize;/* From SCL */
} SCL_RTG_ObjectInfo;


#define SCL_RTG_OK          0
#define SCL_RTG_ERR_FILE    1
#define SCL_RTG_ERR_MEMORY  2
#define SCL_RTG_ERR_PARSE_FAILED  3
#define SCL_RTG_ERR_DOMAIN_OVERFLOW  4
#define SCL_RTG_ERR_REPORT_CONTROL  5
#define SCL_RTG_ERR_DATA_SET  6
#define SCL_RTG_ERR_DATA_SET_MEMBER 7
#define SCL_RTG_ERR_VAL_TYPE 8
#define SCL_RTG_ERR_NOT_FOUND 9
#define SCL_RTG_ERR_ENUM_TYPE 10
#define SCL_RTG_ERR_ENUM_VAL 11
#define SCL_RTG_ERR_TYPES 12
#define SCL_RTG_ERR_VALUE 13
#define SCL_RTG_ERR_MODELS 14
#define SCL_RTG_ERR_INDEXING 15
#define SCL_RTG_ERR_MAPPING_FCN_FAIL 16
#define SCL_RTG_ERR_INVALID_SETTINGS 17

typedef int (* SCL_RTG_MapLeafNode)(    char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        SCL_GenericNode             *pDA_or_BDA,
                                        SCL_RTG_ObjectInfo          *pObjectInfo,
                                        unsigned long               *pOffset,
                                        void                        **pMemory,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef int (* SCL_RTG_MapLnNode)(      char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        SCL_GenericNode             *pLN0_or_LN,
                                        SCL_LNodeType               *pLNodeType,
                                        void                        *pModelHandle,
                                        void                        **ppValue,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef int (* SCL_RTG_MapLnNodeEnd)(   char               *        pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        SCL_GenericNode             *pLN0_or_LN,
                                        SCL_LNodeType               *pLNodeType,
                                        void                        **ppValue,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef void (* SCL_RTG_InitialValue)(  char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        SCL_GenericNode             *pDA_or_BDA,
                                        char                        *p_bType,
                                        char                        *p_type,
                                        char                        *p_valKind,
                                        char                        *pTextValue,
                                        int                         nDomainIndex,
                                        void                        *pHandle,
                                        int                         nFound,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef int (* SCL_RTG_MapNonLeafNode)( char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        unsigned long               *pOffset,
                                        void                        **pValue,
                                        void                        **pMemory,
                                        SCL_GenericNode             *pTypeNode,
                                        SCL_GenericNode             *pInstanceNode,
                                        void                        *pUserData,
                                        MMSd_indexHandler           *pIndexHandler,
                                        MMSd_accessHandler          *pAccessHandler,
                                        void                        **pIndexHandlerParameter,
                                        int                         *nElementSize,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef int (* SCL_RTG_MapNonLeafNodeEnd)(  char                        *pIedName,
											SCL_GenericNode             *pRoot,
											char                        *pDomainName,
											char                        *pObjectName,
											unsigned long               *pOffset,
											void                        **pValue,
											void                        **pMemory,
											SCL_GenericNode             *pTypeNode,
											void                        *pUserData,
											TmwTargTraceMessageFunc     pErrorMessageFcn,
											void                        *pErrorMessageParameter );

typedef int (* SCL_RTG_MapLog)(         char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        char                        *pDomainName,
                                        char                        *pObjectName,
                                        unsigned char               **ppLogBuffer,
                                        unsigned long               *pLogBufferLen,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

typedef void (* SCL_RTG_MapFinished)(   char                        *pIedName,
                                        SCL_GenericNode             *pRoot,
                                        MMSd_context                *pMmsContext,
                                        int                         nSuccess,
                                        void                        *pUserData,
                                        TmwTargTraceMessageFunc     pErrorMessageFcn,
                                        void                        *pErrorMessageParameter );

/* The callback function should receive the control block name, subject and issuer names, and serial number.
   It should return the signature generation function and signature data, along with (optionally) the private
   data to send with the signature.  This is used for both Goose and Sampled Values. */
typedef int (* SCL_RTG_MapVlanSecurity)( char                               *pCbName,
        char                               *pSubjectCommonName,
        char                               *pSubjectIdHierarchy,
        char                               *pIssuerNameCommonName,
        char                               *pIssuerNameIdHierarchy,
        char                               *pSerialNumber,
        void                               *pCryptoContext,
        TMW_61850_VLAN_MakeSignatureFcn    *genVlanSignature,
        void                               **ppVlanSignatureData,
        unsigned char                      **ppPrivateSecurity,
        int                                *pPrivateSecurityLen );

typedef void (* SCL_RTG_MapGooseControlBlockFcn)(      TMW_CB_CommonData *pCbCommon, TMW_GCB_Data   *pGCB,   void *pCbUserData );

typedef void (* SCL_RTG_MapReportControlBlockFcn)(     TMW_CB_CommonData *pCbCommon, TMW_XRCB_Data  *pXRCB,  void *pCbUserData );

typedef void (* SCL_RTG_MapLogControlBlockFcn)(        TMW_CB_CommonData *pCbCommon, TMW_LCB_Data   *pLCB,   void *pCbUserData );

typedef void (* SCL_RTG_MapSgControlBlockFcn)(         TMW_CB_CommonData *pCbCommon, TMW_SGCB_Data  *pSGCB,  void *pCbUserData );

typedef void (* SCL_RTG_MapSavControlBlockFcn)(        TMW_CB_CommonData *pCbCommon, TMW_XSVCB_Data *pXSVCB, void *pCbUserData );

typedef void (* SCL_RTG_MapUcaGooseControlBlockFcn)(   TMW_CB_CommonData *pCbCommon, TMW_GSECB_Data *pGSECB, void *pCbUserData );

typedef struct SCL_RTG_MappingTable {
    /* General parameters */
    unsigned char                       bIgnoreSclIedGlobalSettings;
    unsigned char                       bIgnoreSclIedControlBlockSettings;
    unsigned char                       bForceEd1IndexedRCBs;

    /* Callback functions related to SCL processing events */
    void                                *pUserData; /* Argument passed to each of the following */
    SCL_RTG_MapLnNode                   pMapLnNodeFcn;
    SCL_RTG_MapNonLeafNode              pMapNonLeafNodeFcn;
    SCL_RTG_MapLeafNode                 pMapLeafNodeFcn;
    SCL_RTG_InitialValue                pInitialValueFcn;
    SCL_RTG_MapLog                      pMapLogFcn;
    SCL_RTG_MapLnNodeEnd                pMapLnNodeEndFcn;
    SCL_RTG_MapNonLeafNodeEnd           pMapNonLeafNodeEndFcn;
    SCL_RTG_MapFinished                 pMapFinishedFcn;

    /* Callback function for Security Mapping */
    SCL_RTG_MapVlanSecurity             pMapVlanSecurityFcn;
    void                                *pCryptoContext;

    /* Callback functions called before starting up each control block class */
    void                                *pControlBlockUserData; /* Argument passed to each of the following */
    SCL_RTG_MapGooseControlBlockFcn     pMapGooseControlBlockFcn;
    SCL_RTG_MapReportControlBlockFcn    pMapReportControlBlockFcn;
    SCL_RTG_MapLogControlBlockFcn       pMapLogControlBlockFcn;
    SCL_RTG_MapSgControlBlockFcn        pMapSgControlBlockFcn;
    SCL_RTG_MapSavControlBlockFcn       pMapSavControlBlockFcn;
    SCL_RTG_MapUcaGooseControlBlockFcn  pMapUcaGooseControlBlockFcn;
} SCL_RTG_MappingTable;


typedef struct SCL_RTG_Context {
    SCL_ParseState nState;
    SCL_ElementType skipElement;
    char iedNameConfigured;
    char *iedName;
    char *iedDesc;
    char *iedType;
    char *iedManufacturer;
    char *iedConfigVersion;
    SCL_SclVersionType nIedVersion;
    SCL_GenericNode *pRoot;
    SCL_Server *pCurrentServer;
    SCL_AccessPoint *pCurrentAccessPoint;
    SCL_GenericNodeList pLNodeTypes;
    SCL_GenericNodeList pDOTypes;
    SCL_GenericNodeList pDATypes;
    SCL_GenericNodeList pEnumTypes;
    SCL_GenericNodeList pSubNetworks;
    SCL_SVC_ServiceSettingsType iedServices;
    SCL_SVC_ServiceSettingsType apServices;
    int domainIndex;
    int nLDs;
    int nLNs;
    void *pRtgContext;
    SCL_RTG_MappingTable mappingTable;
    TMW_TrackingObject *pTrackingObject;
#if defined( TMW_MAX_IEDS )
    char iedNames[TMW_MAX_IEDS][128];
    int numIeds;
#endif
} SCL_RTG_Context;

typedef SCL_RTG_Context* SCLhandle;

/*
 *
 */
TMW_CLIB_API int SCL_RTG_isEnumType( char *pTypeName );

TMW_CLIB_API int SCL_RTG_isCodedEnumType(char *pTypeName);

TMW_CLIB_API int SCL_RTG_GetEnumCount( SCL_EnumType *pEnumType);

TMW_CLIB_API int SCL_RTG_GetEnumRange( char              *pBtype,
									   char              *pType,
									   SCL_GenericNode   *pRoot,
									   int               *pMinVal,
									   int               *pMaxVal );

TMW_CLIB_API SCL_GenericNode *SCL_RTG_findTypeDefinition( char               *pName,
														  SCL_GenericNode    *pRoot,
														  SCL_ElementType    nElementType );

TMW_CLIB_API int SCL_RTG_invokeWriteHandler( void            *pHandle,
											 char            *pName,
											 int             domainIndex,
											 char            *bType,
											 char            *da_type,
											 char            *valKind,
											 char            *pTextValue );

TMW_CLIB_API int SCL_RTG_config( char *fileName, 
								 char *iedName, 
								 MMSd_context *pMmsContext, 
								 SCL_RTG_MappingTable *pMappingTable);

TMW_CLIB_API int SCL_RTG_config_all( char *fileName, 
									 char *iedName, 
									 MMSd_Server **ppMmsServerList, 
									 MMSd_ServerInterfaceTable *pServerInterface, 
									 SCL_RTG_MappingTable *pMappingTable);

TMW_CLIB_API SCLhandle SCL_RTG_readfile( char * fileName, SCL_RTG_MappingTable *pMappingTable );

TMW_CLIB_API int SCL_RTG_start_server( SCL_RTG_Context *pSclContext,
									   char * iedName,
									   MMSd_ServerInterfaceTable *pServerInterface,
									   MMSd_Server **ppMmsServerList );

TMW_CLIB_API int SCL_RTG_make_server_list( SCL_RTG_Context *pSclContext );

TMW_CLIB_API int SCL_RTG_list_servers( SCL_RTG_Context *pSclContext );
TMW_CLIB_API int SCL_RTG_free_context( SCL_RTG_Context *pSclContext );



TMW_CLIB_API int SCL_RTG_align( unsigned long nTotalSize, 
								unsigned long size );

TMW_CLIB_API int SCL_RTG_advanceSize( unsigned long *pOffset, 
									  unsigned long size );

TMW_CLIB_API int SCL_HasSingleValElement( SCL_GenericNode *pNode );

TMW_CLIB_API char *SCL_GetName( SCL_GenericNode *pNode );

TMW_CLIB_API SCL_Val *SCL_Get_initialValElementList( SCL_GenericNode *pNode );

TMW_CLIB_API SCL_Val *SCL_Get_nextValElement( SCL_Val *pNode );

TMW_CLIB_API char *SCL_Get_initialValueSG( SCL_GenericNode *pNode );

TMW_CLIB_API char *SCL_Get_sAddr( SCL_GenericNode *pNode );

TMW_CLIB_API char *SCL_Get_count( SCL_GenericNode *pNode );

TMW_CLIB_API SCL_Private *SCL_Get_privateElementList( SCL_GenericNode *pNode );

TMW_CLIB_API SCL_Private *SCL_Get_nextPrivateElement( SCL_Private *pNode );

/* Call from InitialValue mapping callback function to locate a controlling SGCB, if any */
TMW_CLIB_API void *SCL_RTG_getSettingGroupControl( void *pHandle, 
												   int domainIndex );

TMW_CLIB_API void TMW_SCL_GenericNodeMessage( SCL_GenericNode *pNode, 
								 char *pMessage,
                                 TmwTargTraceLevel_t level );

TMW_CLIB_API SCL_GenericNode *SCL_RTG_findGenericNodeList(char *pName, SCL_GenericNodeList *pList);

TMW_CLIB_API SCL_GenericNode *SCL_RTG_findInstance(SCL_GenericNode *pGeneric, char *pName);

TMW_CLIB_API int SCL_RetrieveValues(SCL_GenericNodeList *pValueList, char *pBuffer, int nBuffLen);


#ifdef __cplusplus
};
#endif

#endif /* _CSCL_API_H */
