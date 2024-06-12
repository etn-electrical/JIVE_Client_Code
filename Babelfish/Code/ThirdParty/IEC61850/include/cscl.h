/* ------------------------------------------------------
   -- File: CSCL.H
   --
   --       SCL Lexical Scanner
   --          (c) 2004-2015
   --     TriangleMicroWorks, Inc.
   --
   --     all rights reserved.
   --  THIS WORK MAY NOT BE COPIED FOR ANY REASON
   --  WITHOUT EXPRESSED WRITTEN PERMISSION OF AN
   --  OFFICER OF TriangleMicroWorks.
   --
   ------------------------------------------------------

*/

#ifndef _CSCL_H
#define _CSCL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SCHEMA3_1

/* Version of IEC 61850 as parsed from SCL */
typedef enum { SCL_Version_Error = -1, 
			   SCL_Version_Unknown = 0, 
			   SCL_Version_2003_A, 
			   SCL_Version_2007_A, 
			   SCL_Version_2007_B, SCL_Version_Other 
} SCL_SclVersionType;

/* SCL Services - Get Service Settings from SCL file for IED and AccessPoints */

/* These bit-flags can be used to detect which services, etc. 
    have been specified in the SCL Services elements */
#define SCL_SVC_DynAssociation          0x00000001
#define SCL_SVC_SettingGroups           0x00000002
#define SCL_SVC_GetDirectory            0x00000004
#define SCL_SVC_GetDataObjectDefinition 0x00000008
#define SCL_SVC_DataObjectDirectory     0x00000010
#define SCL_SVC_GetDataSetValue         0x00000020
#define SCL_SVC_SetDataSetValue         0x00000040
#define SCL_SVC_DataSetDirectory        0x00000080
#define SCL_SVC_ConfDataSet             0x00000100
#define SCL_SVC_DynDataSet              0x00000200
#define SCL_SVC_ReadWrite               0x00000400
#define SCL_SVC_TimerActivatedControl   0x00000800
#define SCL_SVC_ConfReportControl       0x00001000
#define SCL_SVC_GetCBValues             0x00002000
#define SCL_SVC_ConfLogControl          0x00004000
#define SCL_SVC_ReportSettings          0x00008000
#define SCL_SVC_LogSettings             0x00010000
#define SCL_SVC_GSESettings             0x00020000
#define SCL_SVC_SMVSettings             0x00040000
#define SCL_SVC_GSEDir                  0x00080000
#define SCL_SVC_GOOSE                   0x00100000
#define SCL_SVC_GSSE                    0x00200000
#define SCL_SVC_SMVsc                   0x00400000
#define SCL_SVC_FileHandling            0x00800000
#define SCL_SVC_ConfLNs                 0x01000000
#define SCL_SVC_ClientServices          0x02000000
#define SCL_SVC_ConfLdName              0x04000000
#define SCL_SVC_SupSubscription         0x08000000
#define SCL_SVC_ConfSigRef              0x10000000
#if defined(SCHEMA3_1)
#define SCL_SVC_ValueHandling           0x20000000
#define SCL_SVC_RedProt                 0x40000000
#define SCL_SVC_TimeSyncProt            0x80000000
#define SCL_SVC_CommProt                0x0100000000
#endif

/* Data type long enough to hold all bit flags */
typedef unsigned long   SCL_SVC_ServiceSettingsFlags;

/* Data types to hold all configured values and settings from SCL */

typedef struct SCL_SVC_tServiceWithMax {
    unsigned int    max;
} SCL_SVC_tServiceWithMax;

typedef SCL_SVC_tServiceWithMax SCL_SVC_tServiceWithOptionalMax;

typedef struct SCL_SVC_tServiceWithMaxAndMaxAttributes {
    unsigned int    max;
    unsigned int    maxAttributes;
} SCL_SVC_tServiceWithMaxAndMaxAttributes;

typedef struct SCL_SVC_tServiceWithMaxAndModify {
    unsigned int    max;
    unsigned char   modify;
} SCL_SVC_tServiceWithMaxAndModify;

typedef struct SCL_SVC_tServiceWithMaxAndClient {
    unsigned int    max;
    unsigned char   client;
} SCL_SVC_tServiceWithMaxAndClient;

typedef struct SCL_SVC_tServiceForConfDataSet {
    unsigned int    max;
    unsigned int    maxAttributes;
    unsigned char   modify;
} SCL_SVC_tServiceForConfDataSet; 

typedef enum { SCL_SVC_BUF_none, 
			   SCL_SVC_BUF_unbuffered, 
			   SCL_SVC_BUF_buffered, 
			   SCL_SVC_BUF_both 
} SCL_SVC_bufMode; 

typedef struct SCL_SVC_tServiceConfReportControl {
    unsigned int    max;
    SCL_SVC_bufMode bufMode;    
    unsigned char   bufConf;
} SCL_SVC_tServiceConfReportControl; 

typedef enum { SCL_SVC_Fix, 
			   SCL_SVC_Conf, 
			   SCL_SVC_Dyn 
} SCL_SVC_tServiceSettingsEnum;

typedef struct SCL_SVC_tReportSettings {
    SCL_SVC_tServiceSettingsEnum    cbName;
    SCL_SVC_tServiceSettingsEnum    datSet;
	SCL_SVC_tServiceSettingsEnum    rptID;
	SCL_SVC_tServiceSettingsEnum    optFields;
	SCL_SVC_tServiceSettingsEnum    bufTime;
	SCL_SVC_tServiceSettingsEnum    trgOps;
	SCL_SVC_tServiceSettingsEnum    intgPd;
    unsigned char                   resvTms;
    unsigned char                   Owner;
} SCL_SVC_tReportSettings; 

typedef struct SCL_SVC_tLogSettings  {
    SCL_SVC_tServiceSettingsEnum    cbName;
    SCL_SVC_tServiceSettingsEnum    datSet;
    SCL_SVC_tServiceSettingsEnum    logEna;
    SCL_SVC_tServiceSettingsEnum    trgOps;
    SCL_SVC_tServiceSettingsEnum    intgPd;
} SCL_SVC_tLogSettings; 

typedef struct tGSESettings  {
    SCL_SVC_tServiceSettingsEnum    cbName;
    SCL_SVC_tServiceSettingsEnum    datSet;
    SCL_SVC_tServiceSettingsEnum    appID;
    SCL_SVC_tServiceSettingsEnum    dataLabel;
} SCL_SVC_tGSESettings; 

typedef struct SCL_SVC_tSMVSettings  {
    SCL_SVC_tServiceSettingsEnum    cbName;
    SCL_SVC_tServiceSettingsEnum    datSet;
    unsigned int                    SmpRate;
    unsigned int                    SamplesPerSec;
    unsigned int                    SecPerSamples;
    SCL_SVC_tServiceSettingsEnum    svID;
    SCL_SVC_tServiceSettingsEnum    optFields;
    SCL_SVC_tServiceSettingsEnum    smpRate;
    SCL_SVC_tServiceSettingsEnum    samplesPerSec;
#if defined(SCHEMA3_1)
    unsigned char                   pdcTimeStamp;   /* NEW 3.1 Schema */
#endif
} SCL_SVC_tSMVSettings; 

typedef struct SCL_SVC_tConfLNs  {
    unsigned char           fixPrefix;
    unsigned char           fixLnInst;
} SCL_SVC_tConfLNs; 

typedef struct SCL_SVC_tSettingGroups  {
    unsigned char           SGEdit;
#if defined(SCHEMA3_1)
    unsigned char           SGEdit_resvTms;         /* NEW 3.1 Schema */
#endif
    unsigned char           ConfSG;
#if defined(SCHEMA3_1)
    unsigned char           ConfSG_resvTms;         /* NEW 3.1 Schema */
#endif
} SCL_SVC_tSettingGroups; 

typedef struct SCL_SVC_tClientServices {
    unsigned char           goose;
    unsigned char           gsse;
    unsigned char           bufReport;
    unsigned char           unbufReport;
    unsigned char           readLog;
    unsigned char           sv;
#if defined(SCHEMA3_1)
    unsigned char           supportsLdName;         /* NEW 3.1 Schema */
    unsigned int            maxAttributes;          /* NEW 3.1 Schema */
    unsigned int            maxReports;             /* NEW 3.1 Schema */
    unsigned int            maxGOOSE;               /* NEW 3.1 Schema */
    unsigned int            maxSMV;                 /* NEW 3.1 Schema */
#endif
} SCL_SVC_tClientServices; 

#if defined(SCHEMA3_1)
typedef struct SCL_SVC_tValueHandling {             /* NEW 3.1 Schema */
    unsigned char                   setToRO;
} SCL_SVC_tValueHandling;

typedef struct SCL_SVC_tFileHandling {              /* NEW 3.1 Schema */
    unsigned char                   mms;
    unsigned char                   ftp;
    unsigned char                   ftps;
} SCL_SVC_tFileHandling;

typedef struct SCL_SVC_tGOOSEcapabilities {         /* NEW 3.1 Schema */
    unsigned int                    max;
    unsigned char                   fixedOffs;
    unsigned char                   client; /* 1.4 compatibility */
} SCL_SVC_tGOOSEcapabilities;

typedef struct SCL_SVC_tRedProt {                   /* NEW 3.1 Schema */
    unsigned char                   hsr;
    unsigned char                   prp;
    unsigned char                   rstp;
} SCL_SVC_tRedProt;

typedef struct SCL_SVC_tTimeSyncProt {              /* NEW 3.1 Schema */
    unsigned char                   sntp;
    unsigned char                   c37_238;
    unsigned char                   other;
} SCL_SVC_tTimeSyncProt;

typedef enum { SCL_SVC_unicast, SCL_SVC_multicast, SCL_SVC_both } SCL_SVC_tSMVDeliveryEnum;

typedef struct SCL_SVC_tSMVsc {                     /* NEW 3.1 Schema */
    unsigned int                    max;
    SCL_SVC_tSMVDeliveryEnum        delivery; /*  default multicast */
    unsigned char                   deliveryConf;
} SCL_SVC_tSMVsc;

typedef struct SCL_SVC_tSupSubscription {           /* NEW 3.1 Schema */
    unsigned int                    maxGo;
    unsigned int                    maxSv;
    unsigned int                    max;
} SCL_SVC_tSupSubscription;

typedef struct SCL_SVC_tCommProt {                  /* NEW 3.1 Schema */
    unsigned char                   ipv6;
} SCL_SVC_tCommProt;

typedef struct SCL_SVC_tKDC {                       /* NEW 3.1 Schema */
    char                            *iedName;       /* Could be array of char (copy) */
    char                            *apName;        /* Could be array of char (copy) */
} SCL_SVC_tKDC;
#endif

/* This is the full set of data retrieved from the Services SCL element */
typedef struct SCL_SVC_ServiceSettingsType {
    SCL_SVC_ServiceSettingsFlags            settings;
    SCL_SVC_tServiceWithOptionalMax         DynAssociation;
    SCL_SVC_tSettingGroups                  SettingGroups;
    SCL_SVC_tServiceForConfDataSet          ConfDataSet;
    SCL_SVC_tServiceWithMaxAndMaxAttributes DynDataSet;
    SCL_SVC_tServiceConfReportControl       ConfReportControl;
    SCL_SVC_tServiceWithMax                 ConfLogControl;
    SCL_SVC_tReportSettings                 ReportSettings;
    SCL_SVC_tLogSettings                    LogSettings;
    SCL_SVC_tGSESettings                    GSESettings;
    SCL_SVC_tSMVSettings                    SMVSettings;
#if defined(SCHEMA3_1)
    SCL_SVC_tGOOSEcapabilities              GOOSE;              /* CHANGED 3.1 Schema */
#else
    SCL_SVC_tServiceWithMaxAndClient        GOOSE;
#endif
    SCL_SVC_tServiceWithMaxAndClient        GSSE;
#if defined(SCHEMA3_1)
    SCL_SVC_tSMVsc                          SMVsc;
#else
    SCL_SVC_tServiceWithMax                 SMVsc;
#endif
    SCL_SVC_tConfLNs                        ConfLNs;
    SCL_SVC_tClientServices                 ClientServices;
#if defined(SCHEMA3_1)
    SCL_SVC_tSupSubscription                SupSubscription;    /* CHANGED 3.1 Schema */
#else
    SCL_SVC_tServiceWithMax                 SupSubscription;
#endif
    SCL_SVC_tServiceWithMax                 ConfSigRef;
#if defined(SCHEMA3_1)
    SCL_SVC_tValueHandling                  ValueHandling;      /* NEW 3.1 Schema */
    SCL_SVC_tRedProt                        RedProt;            /* NEW 3.1 Schema */
    SCL_SVC_tTimeSyncProt                   TimeSyncProt;       /* NEW 3.1 Schema */
    SCL_SVC_tCommProt                       CommProt;           /* NEW 3.1 Schema */
#endif
    unsigned int                            nameLength;
    unsigned char                           valid;
} SCL_SVC_ServiceSettingsType;



typedef struct SCL_GenericNode *SCL_GenericNodePtr;

typedef struct SCL_GenericNode {
    char                      *pName;              
    SCL_GenericNodePtr        pNext;               
    SCL_GenericNodePtr        pParent;             
    int                       nLineNo;             
    int                       nElementType;        
    int                       nParseState;         
} SCL_GenericNode;


typedef struct SCL_GenericNodeList {
    SCL_GenericNode           *pHead;              
    SCL_GenericNode           *pTail;              
} SCL_GenericNodeList;


typedef struct SCL_GenericAttribute *SCL_GenericAttributePtr;

typedef struct SCL_GenericAttribute {
    char                      *pAttributeName;     
    char                      *pAttributeValue;    
    SCL_GenericAttributePtr   pNext;               
} SCL_GenericAttribute;

typedef struct SCL_GenericAttributeList {
    SCL_GenericAttribute      *pHead;              
    SCL_GenericAttribute      *pTail;              
} SCL_GenericAttributeList;

typedef struct SCL_ANY_ELEMENT {
    SCL_GenericNode           gen;                 
    SCL_GenericNodeList       pANY_ELEMENT;        
    SCL_GenericAttributeList  pANY_ATTRIBUTE;      
    char                      *pAnyContent;        
    char                      *p_Name;             
} SCL_ANY_ELEMENT;


void SCL_PrintGenericNode( FILE *fd, SCL_GenericNode *pNode, int indent );
void SCL_PrintGenericNodeList( FILE *fd, SCL_GenericNodeList *pList, int indent );

void SCL_PrintGenericAttributeList( FILE *fd, SCL_GenericAttributeList *pList );

typedef struct SCL_Text {     /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_source;           /* SCL_BaseTypes.xsd:50 */
} SCL_Text;

typedef struct SCL_Private {     /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_type;             /* SCL_BaseTypes.xsd:57 */
    char                      *p_source;           /* SCL_BaseTypes.xsd:64 */
} SCL_Private;

typedef struct SCL_LNode {     /* SCL_Substation.xsd:14 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedName;          /* SCL_Substation.xsd:432 */
    char                      *p_ldInst;           /* SCL_Substation.xsd:433 */
    char                      *p_prefix;           /* SCL_Substation.xsd:434 */
    char                      *p_lnClass;          /* SCL_Substation.xsd:435 */
    char                      *p_lnInst;           /* SCL_Substation.xsd:436 */
    char                      *p_lnType;           /* SCL_Substation.xsd:437 */
} SCL_LNode;

typedef struct SCL_Terminal {     /* SCL_Substation.xsd:71 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_Substation.xsd:604 */
    char                      *p_connectivityNode; /* SCL_Substation.xsd:605 */
    char                      *p_processName;      /* SCL_Substation.xsd:606 */
    char                      *p_substationName;   /* SCL_Substation.xsd:607 */
    char                      *p_voltageLevelName; /* SCL_Substation.xsd:608 */
    char                      *p_bayName;          /* SCL_Substation.xsd:609 */
    char                      *p_cNodeName;        /* SCL_Substation.xsd:610 */
} SCL_Terminal;

typedef struct SCL_EqFunction {     /* SCL_Substation.xsd:273 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction;

typedef struct SCL_GeneralEquipment {     /* SCL_Substation.xsd:551 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:273 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:288 */
} SCL_GeneralEquipment;

typedef struct SCL_EqSubFunction {     /* SCL_Substation.xsd:565 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqSubFunction;

typedef struct SCL_EqFunction2 {     /* SCL_Substation.xsd:117 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction2;

typedef struct SCL_SubEquipment {     /* SCL_Substation.xsd:72 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:117 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_phase;            /* SCL_Substation.xsd:132 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
} SCL_SubEquipment;

typedef struct SCL_SubEquipment2 {     /* SCL_Substation.xsd:235 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:117 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_phase;            /* SCL_Substation.xsd:132 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
} SCL_SubEquipment2;

typedef struct SCL_EqFunction3 {     /* SCL_Substation.xsd:249 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction3;

typedef struct SCL_TapChanger {     /* SCL_Substation.xsd:192 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:235 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:249 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:264 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
} SCL_TapChanger;

typedef struct SCL_NeutralPoint {     /* SCL_Substation.xsd:211 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_Substation.xsd:604 */
    char                      *p_connectivityNode; /* SCL_Substation.xsd:605 */
    char                      *p_processName;      /* SCL_Substation.xsd:606 */
    char                      *p_substationName;   /* SCL_Substation.xsd:607 */
    char                      *p_voltageLevelName; /* SCL_Substation.xsd:608 */
    char                      *p_bayName;          /* SCL_Substation.xsd:609 */
    char                      *p_cNodeName;        /* SCL_Substation.xsd:610 */
} SCL_NeutralPoint;

typedef struct SCL_EqFunction4 {     /* SCL_Substation.xsd:212 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction4;

typedef struct SCL_TransformerWinding {     /* SCL_Substation.xsd:141 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pTapChanger;         /* SCL_Substation.xsd:192 */
    SCL_GenericNodeList       pNeutralPoint;       /* SCL_Substation.xsd:211 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:212 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:227 */
} SCL_TransformerWinding;

typedef struct SCL_SubEquipment3 {     /* SCL_Substation.xsd:155 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:117 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_phase;            /* SCL_Substation.xsd:132 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
} SCL_SubEquipment3;

typedef struct SCL_EqFunction5 {     /* SCL_Substation.xsd:169 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction5;

typedef struct SCL_PowerTransformer {     /* SCL_Substation.xsd:28 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTransformerWinding; /* SCL_Substation.xsd:141 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:155 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:169 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:184 */
} SCL_PowerTransformer;

typedef struct SCL_GeneralEquipment2 {     /* SCL_Substation.xsd:42 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:273 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:288 */
} SCL_GeneralEquipment2;

typedef struct SCL_Voltage {     /* SCL_Substation.xsd:332 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_unit;             /* SCL_BaseTypes.xsd:121 */
    char                      *p_multiplier;       /* SCL_BaseTypes.xsd:122 */
} SCL_Voltage;

typedef struct SCL_EqFunction6 {     /* SCL_Substation.xsd:94 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:551 */
    SCL_GenericNodeList       pEqSubFunction;      /* SCL_Substation.xsd:565 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:580 */
} SCL_EqFunction6;

typedef struct SCL_ConductingEquipment {     /* SCL_Substation.xsd:383 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:94 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:109 */
} SCL_ConductingEquipment;

typedef struct SCL_ConnectivityNode {     /* SCL_Substation.xsd:401 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_pathName;         /* SCL_Substation.xsd:597 */
} SCL_ConnectivityNode;

typedef struct SCL_GeneralEquipment3 {     /* SCL_Substation.xsd:500 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:273 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:288 */
} SCL_GeneralEquipment3;

typedef struct SCL_ConductingEquipment2 {     /* SCL_Substation.xsd:514 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:94 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:109 */
} SCL_ConductingEquipment2;

typedef struct SCL_SubFunction {     /* SCL_Substation.xsd:528 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:528 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:543 */
} SCL_SubFunction;

typedef struct SCL_SubFunction2 {     /* SCL_Substation.xsd:445 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:500 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:514 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:528 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:543 */
} SCL_SubFunction2;

typedef struct SCL_GeneralEquipment4 {     /* SCL_Substation.xsd:459 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:273 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:288 */
} SCL_GeneralEquipment4;

typedef struct SCL_ConductingEquipment3 {     /* SCL_Substation.xsd:473 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:94 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:109 */
} SCL_ConductingEquipment3;

typedef struct SCL_Function {     /* SCL_Substation.xsd:411 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:445 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:459 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:473 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:492 */
} SCL_Function;

typedef struct SCL_Bay {     /* SCL_Substation.xsd:333 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pPowerTransformer;   /* SCL_Substation.xsd:28 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:42 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:383 */
    SCL_GenericNodeList       pConnectivityNode;   /* SCL_Substation.xsd:401 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:411 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_Bay;

typedef struct SCL_Function2 {     /* SCL_Substation.xsd:347 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:445 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:459 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:473 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:492 */
} SCL_Function2;

typedef struct SCL_VoltageLevel {     /* SCL_Substation.xsd:296 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pPowerTransformer;   /* SCL_Substation.xsd:28 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:42 */
    SCL_GenericNodeList       pVoltage;            /* SCL_Substation.xsd:332 */
    SCL_GenericNodeList       pBay;                /* SCL_Substation.xsd:333 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:347 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_nomFreq;          /* SCL_Substation.xsd:362 */
    char                      *p_numPhases;        /* SCL_Substation.xsd:369 */
} SCL_VoltageLevel;

typedef struct SCL_Function3 {     /* SCL_Substation.xsd:310 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:445 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:459 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:473 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:492 */
} SCL_Function3;

typedef struct SCL_Substation {     /* SCL_Substation.xsd:700 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pPowerTransformer;   /* SCL_Substation.xsd:28 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:42 */
    SCL_GenericNodeList       pVoltageLevel;       /* SCL_Substation.xsd:296 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:310 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_Substation;

typedef struct SCL_GeneralEquipment5 {     /* SCL_Substation.xsd:618 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:273 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:288 */
} SCL_GeneralEquipment5;

typedef struct SCL_Function4 {     /* SCL_Substation.xsd:632 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pSubFunction;        /* SCL_Substation.xsd:445 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:459 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:473 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:492 */
} SCL_Function4;

typedef struct SCL_ConductingEquipment4 {     /* SCL_Substation.xsd:665 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:94 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:109 */
} SCL_ConductingEquipment4;

typedef struct SCL_Substation2 {     /* SCL_Substation.xsd:666 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pPowerTransformer;   /* SCL_Substation.xsd:28 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:42 */
    SCL_GenericNodeList       pVoltageLevel;       /* SCL_Substation.xsd:296 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:310 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_Substation2;

typedef struct SCL_ConductingEquipment5 {     /* SCL_Substation.xsd:654 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pTerminal;           /* SCL_Substation.xsd:71 */
    SCL_GenericNodeList       pSubEquipment;       /* SCL_Substation.xsd:72 */
    SCL_GenericNodeList       pEqFunction;         /* SCL_Substation.xsd:94 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_virtual;          /* SCL_Substation.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:109 */
} SCL_ConductingEquipment5;

typedef struct SCL_ConnectivityNode2 {     /* SCL_Substation.xsd:655 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_pathName;         /* SCL_Substation.xsd:597 */
} SCL_ConnectivityNode2;

typedef struct SCL_Line {     /* SCL_Substation.xsd:680 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:618 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:632 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:654 */
    SCL_GenericNodeList       pConnectivityNode;   /* SCL_Substation.xsd:655 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:657 */
} SCL_Line;

typedef struct SCL_Process {     /* SCL_Substation.xsd:681 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:618 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:632 */
    SCL_GenericNodeList       pProcess;            /* SCL_Substation.xsd:681 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:696 */
} SCL_Process;

typedef struct SCL_Process2 {     /* SCL_Substation.xsd:714 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:618 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:632 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:665 */
    SCL_GenericNodeList       pSubstation;         /* SCL_Substation.xsd:666 */
    SCL_GenericNodeList       pLine;               /* SCL_Substation.xsd:680 */
    SCL_GenericNodeList       pProcess;            /* SCL_Substation.xsd:681 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:696 */
} SCL_Process2;

typedef struct SCL_Line2 {     /* SCL_Substation.xsd:728 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLNode;              /* SCL_Substation.xsd:14 */
    SCL_GenericNodeList       pGeneralEquipment;   /* SCL_Substation.xsd:618 */
    SCL_GenericNodeList       pFunction;           /* SCL_Substation.xsd:632 */
    SCL_GenericNodeList       pConductingEquipment; /* SCL_Substation.xsd:654 */
    SCL_GenericNodeList       pConnectivityNode;   /* SCL_Substation.xsd:655 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Substation.xsd:657 */
} SCL_Line2;

typedef struct SCL_FCDA {     /* SCL_IED.xsd:375 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_ldInst;           /* SCL_IED.xsd:382 */
    char                      *p_prefix;           /* SCL_IED.xsd:383 */
    char                      *p_lnClass;          /* SCL_IED.xsd:384 */
    char                      *p_lnInst;           /* SCL_IED.xsd:385 */
    char                      *p_doName;           /* SCL_IED.xsd:386 */
    char                      *p_daName;           /* SCL_IED.xsd:387 */
    char                      *p_fc;               /* SCL_IED.xsd:388 */
    char                      *p_ix;               /* SCL_IED.xsd:389 */
} SCL_FCDA;

typedef struct SCL_DataSet {     /* SCL_IED.xsd:319 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_FCDA;            /* SCL_IED.xsd:374 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:377 */
} SCL_DataSet;

typedef struct SCL_TrgOps {     /* SCL_IED.xsd:403 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_dchg;             /* SCL_IED.xsd:410 */
    char                      *p_qchg;             /* SCL_IED.xsd:411 */
    char                      *p_dupd;             /* SCL_IED.xsd:412 */
    char                      *p_period;           /* SCL_IED.xsd:413 */
    char                      *p_gi;               /* SCL_IED.xsd:414 */
} SCL_TrgOps;

typedef struct SCL_OptFields {     /* SCL_IED.xsd:420 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_seqNum;           /* SCL_IED.xsd:24 */
    char                      *p_timeStamp;        /* SCL_IED.xsd:25 */
    char                      *p_dataSet;          /* SCL_IED.xsd:26 */
    char                      *p_reasonCode;       /* SCL_IED.xsd:27 */
    char                      *p_dataRef;          /* SCL_IED.xsd:28 */
    char                      *p_entryID;          /* SCL_IED.xsd:29 */
    char                      *p_configRef;        /* SCL_IED.xsd:30 */
    char                      *p_bufOvfl;          /* SCL_IED.xsd:31 */
    char                      *p_segmentation;     /* SCL_IED.xsd:32 */
} SCL_OptFields;

typedef struct SCL_ClientLN {     /* SCL_IED.xsd:439 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedName;          /* SCL_IED.xsd:36 */
    char                      *p_ldInst;           /* SCL_IED.xsd:37 */
    char                      *p_prefix;           /* SCL_IED.xsd:41 */
    char                      *p_lnClass;          /* SCL_IED.xsd:42 */
    char                      *p_lnInst;           /* SCL_IED.xsd:43 */
    char                      *p_apRef;            /* SCL_IED.xsd:447 */
} SCL_ClientLN;

typedef struct SCL_RptEnabled {     /* SCL_IED.xsd:425 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pClientLN;           /* SCL_IED.xsd:439 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_max;              /* SCL_IED.xsd:441 */
} SCL_RptEnabled;

typedef struct SCL_ReportControl {     /* SCL_IED.xsd:320 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pTrgOps;             /* SCL_IED.xsd:403 */
    SCL_GenericNodeList       pOptFields;          /* SCL_IED.xsd:420 */
    SCL_GenericNodeList       pRptEnabled;         /* SCL_IED.xsd:425 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:394 */
    char                      *p_datSet;           /* SCL_IED.xsd:395 */
    char                      *p_intgPd;           /* SCL_IED.xsd:405 */
    char                      *p_rptID;            /* SCL_IED.xsd:427 */
    char                      *p_confRev;          /* SCL_IED.xsd:428 */
    char                      *p_buffered;         /* SCL_IED.xsd:429 */
    char                      *p_bufTime;          /* SCL_IED.xsd:430 */
    char                      *p_indexed;          /* SCL_IED.xsd:431 */
} SCL_ReportControl;

typedef struct SCL_LogControl {     /* SCL_IED.xsd:321 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pTrgOps;             /* SCL_IED.xsd:403 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:394 */
    char                      *p_datSet;           /* SCL_IED.xsd:395 */
    char                      *p_intgPd;           /* SCL_IED.xsd:405 */
    char                      *p_ldInst;           /* SCL_IED.xsd:452 */
    char                      *p_prefix;           /* SCL_IED.xsd:453 */
    char                      *p_lnClass;          /* SCL_IED.xsd:454 */
    char                      *p_lnInst;           /* SCL_IED.xsd:455 */
    char                      *p_logName;          /* SCL_IED.xsd:456 */
    char                      *p_logEna;           /* SCL_IED.xsd:457 */
    char                      *p_reasonCode;       /* SCL_IED.xsd:458 */
    char                      *p_bufTime;          /* SCL_IED.xsd:459 */
} SCL_LogControl;

typedef struct SCL_SDI {     /* SCL_IED.xsd:607 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_SDI_DAI;         /* SCL_IED.xsd:606 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:610 */
    char                      *p_ix;               /* SCL_IED.xsd:611 */
    char                      *p_sAddr;            /* SCL_IED.xsd:612 */
} SCL_SDI;

typedef struct SCL_Val {     /* SCL_IED.xsd:620 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_sGroup;           /* SCL_BaseTypes.xsd:106 */
} SCL_Val;

typedef struct SCL_DAI {     /* SCL_IED.xsd:608 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pVal;                /* SCL_IED.xsd:620 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:622 */
    char                      *p_sAddr;            /* SCL_IED.xsd:623 */
    char                      *p_valKind;          /* SCL_IED.xsd:624 */
    char                      *p_ix;               /* SCL_IED.xsd:625 */
    char                      *p_valImport;        /* SCL_IED.xsd:626 */
} SCL_DAI;

typedef struct SCL_SDI2 {     /* SCL_IED.xsd:588 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_SDI_DAI;         /* SCL_IED.xsd:606 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:610 */
    char                      *p_ix;               /* SCL_IED.xsd:611 */
    char                      *p_sAddr;            /* SCL_IED.xsd:612 */
} SCL_SDI2;

typedef struct SCL_DAI2 {     /* SCL_IED.xsd:595 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pVal;                /* SCL_IED.xsd:620 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:622 */
    char                      *p_sAddr;            /* SCL_IED.xsd:623 */
    char                      *p_valKind;          /* SCL_IED.xsd:624 */
    char                      *p_ix;               /* SCL_IED.xsd:625 */
    char                      *p_valImport;        /* SCL_IED.xsd:626 */
} SCL_DAI2;

typedef struct SCL_DOI {     /* SCL_IED.xsd:322 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_SDI_DAI;         /* SCL_IED.xsd:587 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:597 */
    char                      *p_ix;               /* SCL_IED.xsd:598 */
    char                      *p_accessControl;    /* SCL_IED.xsd:599 */
} SCL_DOI;

typedef struct SCL_ExtRef {     /* SCL_IED.xsd:467 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedName;          /* SCL_IED.xsd:474 */
    char                      *p_ldInst;           /* SCL_IED.xsd:475 */
    char                      *p_prefix;           /* SCL_IED.xsd:476 */
    char                      *p_lnClass;          /* SCL_IED.xsd:477 */
    char                      *p_lnInst;           /* SCL_IED.xsd:478 */
    char                      *p_doName;           /* SCL_IED.xsd:479 */
    char                      *p_daName;           /* SCL_IED.xsd:480 */
    char                      *p_intAddr;          /* SCL_IED.xsd:481 */
    char                      *p_serviceType;      /* SCL_IED.xsd:482 */
    char                      *p_srcLDInst;        /* SCL_IED.xsd:483 */
    char                      *p_srcPrefix;        /* SCL_IED.xsd:484 */
    char                      *p_srcLNClass;       /* SCL_IED.xsd:485 */
    char                      *p_srcLNInst;        /* SCL_IED.xsd:486 */
    char                      *p_srcCBName;        /* SCL_IED.xsd:487 */
} SCL_ExtRef;

typedef struct SCL_Inputs {     /* SCL_IED.xsd:329 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pExtRef;             /* SCL_IED.xsd:467 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_Inputs;

typedef struct SCL_Log {     /* SCL_IED.xsd:342 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:492 */
} SCL_Log;

typedef struct SCL_IEDName {     /* SCL_IED.xsd:500 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_apRef;            /* SCL_IED.xsd:504 */
    char                      *p_ldInst;           /* SCL_IED.xsd:505 */
    char                      *p_prefix;           /* SCL_IED.xsd:506 */
    char                      *p_lnClass;          /* SCL_IED.xsd:507 */
    char                      *p_lnInst;           /* SCL_IED.xsd:508 */
} SCL_IEDName;

typedef struct SCL_Protocol {     /* SCL_IED.xsd:529 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_mustUnderstand;   /* SCL_IED.xsd:521 */
} SCL_Protocol;

typedef struct SCL_GSEControl {     /* SCL_IED.xsd:361 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pIEDName;            /* SCL_IED.xsd:500 */
    SCL_GenericNodeList       pProtocol;           /* SCL_IED.xsd:529 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:394 */
    char                      *p_datSet;           /* SCL_IED.xsd:395 */
    char                      *p_confRev;          /* SCL_IED.xsd:514 */
    char                      *p_type;             /* SCL_IED.xsd:531 */
    char                      *p_appID;            /* SCL_IED.xsd:532 */
    char                      *p_fixedOffs;        /* SCL_IED.xsd:533 */
    char                      *p_securityEnable;   /* SCL_IED.xsd:534 */
} SCL_GSEControl;

typedef struct SCL_SmvOpts {     /* SCL_IED.xsd:542 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_refreshTime;      /* SCL_IED.xsd:15 */
    char                      *p_sampleSynchronized; /* SCL_IED.xsd:16 */
    char                      *p_sampleRate;       /* SCL_IED.xsd:17 */
    char                      *p_dataSet;          /* SCL_IED.xsd:18 */
    char                      *p_security;         /* SCL_IED.xsd:19 */
    char                      *p_timestamp;        /* SCL_IED.xsd:20 */
    char                      *p_dataRef;          /* SCL_IED.xsd:21 */
} SCL_SmvOpts;

typedef struct SCL_Protocol2 {     /* SCL_IED.xsd:547 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_mustUnderstand;   /* SCL_IED.xsd:521 */
} SCL_Protocol2;

typedef struct SCL_SampledValueControl {     /* SCL_IED.xsd:362 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pIEDName;            /* SCL_IED.xsd:500 */
    SCL_GenericNodeList       pSmvOpts;            /* SCL_IED.xsd:542 */
    SCL_GenericNodeList       pProtocol;           /* SCL_IED.xsd:547 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:394 */
    char                      *p_datSet;           /* SCL_IED.xsd:395 */
    char                      *p_confRev;          /* SCL_IED.xsd:514 */
    char                      *p_smvID;            /* SCL_IED.xsd:549 */
    char                      *p_multicast;        /* SCL_IED.xsd:550 */
    char                      *p_smpRate;          /* SCL_IED.xsd:551 */
    char                      *p_nofASDU;          /* SCL_IED.xsd:552 */
    char                      *p_smpMod;           /* SCL_IED.xsd:553 */
    char                      *p_securityEnable;   /* SCL_IED.xsd:554 */
} SCL_SampledValueControl;

typedef struct SCL_SettingControl {     /* SCL_IED.xsd:363 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_numOfSGs;         /* SCL_IED.xsd:561 */
    char                      *p_actSG;            /* SCL_IED.xsd:568 */
    char                      *p_resvTms;          /* SCL_IED.xsd:575 */
} SCL_SettingControl;

typedef struct SCL_SCLControl {     /* SCL_IED.xsd:364 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_SCLControl;

typedef struct SCL_LN0 {     /* SCL_IED.xsd:234 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pDataSet;            /* SCL_IED.xsd:319 */
    SCL_GenericNodeList       pReportControl;      /* SCL_IED.xsd:320 */
    SCL_GenericNodeList       pLogControl;         /* SCL_IED.xsd:321 */
    SCL_GenericNodeList       pDOI;                /* SCL_IED.xsd:322 */
    SCL_GenericNodeList       pInputs;             /* SCL_IED.xsd:329 */
    SCL_GenericNodeList       pLog;                /* SCL_IED.xsd:342 */
    SCL_GenericNodeList       pGSEControl;         /* SCL_IED.xsd:361 */
    SCL_GenericNodeList       pSampledValueControl; /* SCL_IED.xsd:362 */
    SCL_GenericNodeList       pSettingControl;     /* SCL_IED.xsd:363 */
    SCL_GenericNodeList       pSCLControl;         /* SCL_IED.xsd:364 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_lnType;           /* SCL_IED.xsd:344 */
    char                      *p_lnClass;          /* SCL_IED.xsd:366 */
    char                      *p_inst;             /* SCL_IED.xsd:367 */
} SCL_LN0;

typedef struct SCL_LN {     /* SCL_IED.xsd:285 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pDataSet;            /* SCL_IED.xsd:319 */
    SCL_GenericNodeList       pReportControl;      /* SCL_IED.xsd:320 */
    SCL_GenericNodeList       pLogControl;         /* SCL_IED.xsd:321 */
    SCL_GenericNodeList       pDOI;                /* SCL_IED.xsd:322 */
    SCL_GenericNodeList       pInputs;             /* SCL_IED.xsd:329 */
    SCL_GenericNodeList       pLog;                /* SCL_IED.xsd:342 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_lnType;           /* SCL_IED.xsd:344 */
    char                      *p_prefix;           /* SCL_IED.xsd:351 */
    char                      *p_lnClass;          /* SCL_IED.xsd:352 */
    char                      *p_inst;             /* SCL_IED.xsd:353 */
} SCL_LN;

typedef struct SCL_DynAssociation {     /* SCL_IED.xsd:73 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:632 */
} SCL_DynAssociation;

typedef struct SCL_SGEdit {     /* SCL_IED.xsd:860 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_resvTms;          /* SCL_IED.xsd:864 */
} SCL_SGEdit;

typedef struct SCL_ConfSG {     /* SCL_IED.xsd:869 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_resvTms;          /* SCL_IED.xsd:873 */
} SCL_ConfSG;

typedef struct SCL_SettingGroups {     /* SCL_IED.xsd:74 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pSGEdit;             /* SCL_IED.xsd:860 */
    SCL_GenericNodeList       pConfSG;             /* SCL_IED.xsd:869 */
    char                      *pAnyContent;       
} SCL_SettingGroups;

typedef struct SCL_GetDirectory {     /* SCL_IED.xsd:75 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_GetDirectory;

typedef struct SCL_GetDataObjectDefinition {     /* SCL_IED.xsd:76 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_GetDataObjectDefinition;

typedef struct SCL_DataObjectDirectory {     /* SCL_IED.xsd:77 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_DataObjectDirectory;

typedef struct SCL_GetDataSetValue {     /* SCL_IED.xsd:78 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_GetDataSetValue;

typedef struct SCL_SetDataSetValue {     /* SCL_IED.xsd:79 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_SetDataSetValue;

typedef struct SCL_DataSetDirectory {     /* SCL_IED.xsd:80 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_DataSetDirectory;

typedef struct SCL_ConfDataSet {     /* SCL_IED.xsd:81 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_maxAttributes;    /* SCL_IED.xsd:672 */
    char                      *p_modify;           /* SCL_IED.xsd:692 */
} SCL_ConfDataSet;

typedef struct SCL_DynDataSet {     /* SCL_IED.xsd:82 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_maxAttributes;    /* SCL_IED.xsd:672 */
} SCL_DynDataSet;

typedef struct SCL_ReadWrite {     /* SCL_IED.xsd:83 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_ReadWrite;

typedef struct SCL_TimerActivatedControl {     /* SCL_IED.xsd:84 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_TimerActivatedControl;

typedef struct SCL_ConfReportControl {     /* SCL_IED.xsd:85 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_bufMode;          /* SCL_IED.xsd:656 */
    char                      *p_bufConf;          /* SCL_IED.xsd:665 */
} SCL_ConfReportControl;

typedef struct SCL_GetCBValues {     /* SCL_IED.xsd:86 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_GetCBValues;

typedef struct SCL_ConfLogControl {     /* SCL_IED.xsd:87 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:638 */
} SCL_ConfLogControl;

typedef struct SCL_ReportSettings {     /* SCL_IED.xsd:88 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_cbName;           /* SCL_IED.xsd:729 */
    char                      *p_datSet;           /* SCL_IED.xsd:730 */
    char                      *p_rptID;            /* SCL_IED.xsd:735 */
    char                      *p_optFields;        /* SCL_IED.xsd:736 */
    char                      *p_bufTime;          /* SCL_IED.xsd:737 */
    char                      *p_trgOps;           /* SCL_IED.xsd:738 */
    char                      *p_intgPd;           /* SCL_IED.xsd:739 */
    char                      *p_resvTms;          /* SCL_IED.xsd:740 */
    char                      *p_owner;            /* SCL_IED.xsd:741 */
} SCL_ReportSettings;

typedef struct SCL_LogSettings {     /* SCL_IED.xsd:89 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_cbName;           /* SCL_IED.xsd:729 */
    char                      *p_datSet;           /* SCL_IED.xsd:730 */
    char                      *p_logEna;           /* SCL_IED.xsd:748 */
    char                      *p_trgOps;           /* SCL_IED.xsd:749 */
    char                      *p_intgPd;           /* SCL_IED.xsd:750 */
} SCL_LogSettings;

typedef struct SCL_GSESettings {     /* SCL_IED.xsd:90 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_cbName;           /* SCL_IED.xsd:729 */
    char                      *p_datSet;           /* SCL_IED.xsd:730 */
    char                      *p_appID;            /* SCL_IED.xsd:757 */
    char                      *p_dataLabel;        /* SCL_IED.xsd:758 */
} SCL_GSESettings;

typedef struct SCL_SmpRate {     /* SCL_IED.xsd:766 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    int                       nDummy;             
} SCL_SmpRate;

typedef struct SCL_SamplesPerSec {     /* SCL_IED.xsd:773 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    int                       nDummy;             
} SCL_SamplesPerSec;

typedef struct SCL_SecPerSamples {     /* SCL_IED.xsd:780 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    int                       nDummy;             
} SCL_SecPerSamples;

typedef struct SCL_SMVSettings {     /* SCL_IED.xsd:91 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pCh_SmpRate_SamplesPerSec_SecPerSamples; /* SCL_IED.xsd:765 */
    char                      *pAnyContent;       
    char                      *p_cbName;           /* SCL_IED.xsd:729 */
    char                      *p_datSet;           /* SCL_IED.xsd:730 */
    char                      *p_svID;             /* SCL_IED.xsd:788 */
    char                      *p_optFields;        /* SCL_IED.xsd:789 */
    char                      *p_smpRate;          /* SCL_IED.xsd:790 */
    char                      *p_samplesPerSec;    /* SCL_IED.xsd:791 */
    char                      *p_pdcTimeStamp;     /* SCL_IED.xsd:792 */
} SCL_SMVSettings;

typedef struct SCL_GSEDir {     /* SCL_IED.xsd:92 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_GSEDir;

typedef struct SCL_GOOSE {     /* SCL_IED.xsd:93 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_fixedOffs;        /* SCL_IED.xsd:815 */
    char                      *p_client;           /* SCL_IED.xsd:816 */
} SCL_GOOSE;

typedef struct SCL_GSSE {     /* SCL_IED.xsd:94 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_client;           /* SCL_IED.xsd:649 */
} SCL_GSSE;

typedef struct SCL_SMVsc {     /* SCL_IED.xsd:95 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:635 */
    char                      *p_delivery;         /* SCL_IED.xsd:837 */
    char                      *p_deliveryConf;     /* SCL_IED.xsd:838 */
} SCL_SMVsc;

typedef struct SCL_FileHandling {     /* SCL_IED.xsd:96 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_mms;              /* SCL_IED.xsd:806 */
    char                      *p_ftp;              /* SCL_IED.xsd:807 */
    char                      *p_ftps;             /* SCL_IED.xsd:808 */
} SCL_FileHandling;

typedef struct SCL_ConfLNs {     /* SCL_IED.xsd:97 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_fixPrefix;        /* SCL_IED.xsd:797 */
    char                      *p_fixLnInst;        /* SCL_IED.xsd:798 */
} SCL_ConfLNs;

typedef struct SCL_TimeSyncProt {     /* SCL_IED.xsd:698 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_sntp;             /* SCL_IED.xsd:828 */
    char                      *p_c37_238;          /* SCL_IED.xsd:829 */
    char                      *p_other;            /* SCL_IED.xsd:830 */
} SCL_TimeSyncProt;

typedef struct SCL_ClientServices {     /* SCL_IED.xsd:98 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pTimeSyncProt;       /* SCL_IED.xsd:698 */
    char                      *pAnyContent;       
    char                      *p_goose;            /* SCL_IED.xsd:700 */
    char                      *p_gsse;             /* SCL_IED.xsd:701 */
    char                      *p_bufReport;        /* SCL_IED.xsd:702 */
    char                      *p_unbufReport;      /* SCL_IED.xsd:703 */
    char                      *p_readLog;          /* SCL_IED.xsd:704 */
    char                      *p_sv;               /* SCL_IED.xsd:705 */
    char                      *p_supportsLdName;   /* SCL_IED.xsd:706 */
    char                      *p_maxAttributes;    /* SCL_IED.xsd:707 */
    char                      *p_maxReports;       /* SCL_IED.xsd:712 */
    char                      *p_maxGOOSE;         /* SCL_IED.xsd:717 */
    char                      *p_maxSMV;           /* SCL_IED.xsd:722 */
} SCL_ClientServices;

typedef struct SCL_ConfLdName {     /* SCL_IED.xsd:99 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
} SCL_ConfLdName;

typedef struct SCL_SupSubscription {     /* SCL_IED.xsd:100 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_maxGo;            /* SCL_IED.xsd:843 */
    char                      *p_maxSv;            /* SCL_IED.xsd:844 */
    char                      *p_max;              /* SCL_IED.xsd:845 */
} SCL_SupSubscription;

typedef struct SCL_ConfSigRef {     /* SCL_IED.xsd:101 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_max;              /* SCL_IED.xsd:638 */
} SCL_ConfSigRef;

typedef struct SCL_ValueHandling {     /* SCL_IED.xsd:102 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_setToRO;          /* SCL_IED.xsd:801 */
} SCL_ValueHandling;

typedef struct SCL_RedProt {     /* SCL_IED.xsd:103 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_hsr;              /* SCL_IED.xsd:821 */
    char                      *p_prp;              /* SCL_IED.xsd:822 */
    char                      *p_rstp;             /* SCL_IED.xsd:823 */
} SCL_RedProt;

typedef struct SCL_TimeSyncProt2 {     /* SCL_IED.xsd:104 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_sntp;             /* SCL_IED.xsd:828 */
    char                      *p_c37_238;          /* SCL_IED.xsd:829 */
    char                      *p_other;            /* SCL_IED.xsd:830 */
} SCL_TimeSyncProt2;

typedef struct SCL_CommProt {     /* SCL_IED.xsd:105 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_ipv6;             /* SCL_IED.xsd:850 */
} SCL_CommProt;

typedef struct SCL_Services {     /* SCL_IED.xsd:49 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pDynAssociation;     /* SCL_IED.xsd:73 */
    SCL_GenericNodeList       pSettingGroups;      /* SCL_IED.xsd:74 */
    SCL_GenericNodeList       pGetDirectory;       /* SCL_IED.xsd:75 */
    SCL_GenericNodeList       pGetDataObjectDefinition; /* SCL_IED.xsd:76 */
    SCL_GenericNodeList       pDataObjectDirectory; /* SCL_IED.xsd:77 */
    SCL_GenericNodeList       pGetDataSetValue;    /* SCL_IED.xsd:78 */
    SCL_GenericNodeList       pSetDataSetValue;    /* SCL_IED.xsd:79 */
    SCL_GenericNodeList       pDataSetDirectory;   /* SCL_IED.xsd:80 */
    SCL_GenericNodeList       pConfDataSet;        /* SCL_IED.xsd:81 */
    SCL_GenericNodeList       pDynDataSet;         /* SCL_IED.xsd:82 */
    SCL_GenericNodeList       pReadWrite;          /* SCL_IED.xsd:83 */
    SCL_GenericNodeList       pTimerActivatedControl; /* SCL_IED.xsd:84 */
    SCL_GenericNodeList       pConfReportControl;  /* SCL_IED.xsd:85 */
    SCL_GenericNodeList       pGetCBValues;        /* SCL_IED.xsd:86 */
    SCL_GenericNodeList       pConfLogControl;     /* SCL_IED.xsd:87 */
    SCL_GenericNodeList       pReportSettings;     /* SCL_IED.xsd:88 */
    SCL_GenericNodeList       pLogSettings;        /* SCL_IED.xsd:89 */
    SCL_GenericNodeList       pGSESettings;        /* SCL_IED.xsd:90 */
    SCL_GenericNodeList       pSMVSettings;        /* SCL_IED.xsd:91 */
    SCL_GenericNodeList       pGSEDir;             /* SCL_IED.xsd:92 */
    SCL_GenericNodeList       pGOOSE;              /* SCL_IED.xsd:93 */
    SCL_GenericNodeList       pGSSE;               /* SCL_IED.xsd:94 */
    SCL_GenericNodeList       pSMVsc;              /* SCL_IED.xsd:95 */
    SCL_GenericNodeList       pFileHandling;       /* SCL_IED.xsd:96 */
    SCL_GenericNodeList       pConfLNs;            /* SCL_IED.xsd:97 */
    SCL_GenericNodeList       pClientServices;     /* SCL_IED.xsd:98 */
    SCL_GenericNodeList       pConfLdName;         /* SCL_IED.xsd:99 */
    SCL_GenericNodeList       pSupSubscription;    /* SCL_IED.xsd:100 */
    SCL_GenericNodeList       pConfSigRef;         /* SCL_IED.xsd:101 */
    SCL_GenericNodeList       pValueHandling;      /* SCL_IED.xsd:102 */
    SCL_GenericNodeList       pRedProt;            /* SCL_IED.xsd:103 */
    SCL_GenericNodeList       pTimeSyncProt;       /* SCL_IED.xsd:104 */
    SCL_GenericNodeList       pCommProt;           /* SCL_IED.xsd:105 */
    char                      *pAnyContent;       
    char                      *p_nameLength;       /* SCL_IED.xsd:107 */
} SCL_Services;

typedef struct SCL_Authentication {     /* SCL_IED.xsd:192 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_none;             /* SCL_IED.xsd:8 */
    char                      *p_password;         /* SCL_IED.xsd:9 */
    char                      *p_weak;             /* SCL_IED.xsd:10 */
    char                      *p_strong;           /* SCL_IED.xsd:11 */
    char                      *p_certificate;      /* SCL_IED.xsd:12 */
} SCL_Authentication;

typedef struct SCL_AccessControl {     /* SCL_IED.xsd:217 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
} SCL_AccessControl;

typedef struct SCL_LDevice {     /* SCL_IED.xsd:197 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pLN0;                /* SCL_IED.xsd:234 */
    SCL_GenericNodeList       pLN;                 /* SCL_IED.xsd:285 */
    SCL_GenericNodeList       pAccessControl;      /* SCL_IED.xsd:217 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_inst;             /* SCL_IED.xsd:219 */
    char                      *p_ldName;           /* SCL_IED.xsd:220 */
} SCL_LDevice;

typedef struct SCL_Association {     /* SCL_IED.xsd:205 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedName;          /* SCL_IED.xsd:36 */
    char                      *p_ldInst;           /* SCL_IED.xsd:37 */
    char                      *p_prefix;           /* SCL_IED.xsd:41 */
    char                      *p_lnClass;          /* SCL_IED.xsd:42 */
    char                      *p_lnInst;           /* SCL_IED.xsd:43 */
    char                      *p_kind;             /* SCL_IED.xsd:231 */
    char                      *p_associationID;    /* SCL_IED.xsd:232 */
} SCL_Association;

typedef struct SCL_Server {     /* SCL_IED.xsd:124 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pAuthentication;     /* SCL_IED.xsd:192 */
    SCL_GenericNodeList       pLDevice;            /* SCL_IED.xsd:197 */
    SCL_GenericNodeList       pAssociation;        /* SCL_IED.xsd:205 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_timeout;          /* SCL_IED.xsd:207 */
} SCL_Server;

typedef struct SCL_ServerAt {     /* SCL_IED.xsd:131 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_apName;           /* SCL_IED.xsd:184 */
} SCL_ServerAt;

typedef struct SCL_Services2 {     /* SCL_IED.xsd:133 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pDynAssociation;     /* SCL_IED.xsd:73 */
    SCL_GenericNodeList       pSettingGroups;      /* SCL_IED.xsd:74 */
    SCL_GenericNodeList       pGetDirectory;       /* SCL_IED.xsd:75 */
    SCL_GenericNodeList       pGetDataObjectDefinition; /* SCL_IED.xsd:76 */
    SCL_GenericNodeList       pDataObjectDirectory; /* SCL_IED.xsd:77 */
    SCL_GenericNodeList       pGetDataSetValue;    /* SCL_IED.xsd:78 */
    SCL_GenericNodeList       pSetDataSetValue;    /* SCL_IED.xsd:79 */
    SCL_GenericNodeList       pDataSetDirectory;   /* SCL_IED.xsd:80 */
    SCL_GenericNodeList       pConfDataSet;        /* SCL_IED.xsd:81 */
    SCL_GenericNodeList       pDynDataSet;         /* SCL_IED.xsd:82 */
    SCL_GenericNodeList       pReadWrite;          /* SCL_IED.xsd:83 */
    SCL_GenericNodeList       pTimerActivatedControl; /* SCL_IED.xsd:84 */
    SCL_GenericNodeList       pConfReportControl;  /* SCL_IED.xsd:85 */
    SCL_GenericNodeList       pGetCBValues;        /* SCL_IED.xsd:86 */
    SCL_GenericNodeList       pConfLogControl;     /* SCL_IED.xsd:87 */
    SCL_GenericNodeList       pReportSettings;     /* SCL_IED.xsd:88 */
    SCL_GenericNodeList       pLogSettings;        /* SCL_IED.xsd:89 */
    SCL_GenericNodeList       pGSESettings;        /* SCL_IED.xsd:90 */
    SCL_GenericNodeList       pSMVSettings;        /* SCL_IED.xsd:91 */
    SCL_GenericNodeList       pGSEDir;             /* SCL_IED.xsd:92 */
    SCL_GenericNodeList       pGOOSE;              /* SCL_IED.xsd:93 */
    SCL_GenericNodeList       pGSSE;               /* SCL_IED.xsd:94 */
    SCL_GenericNodeList       pSMVsc;              /* SCL_IED.xsd:95 */
    SCL_GenericNodeList       pFileHandling;       /* SCL_IED.xsd:96 */
    SCL_GenericNodeList       pConfLNs;            /* SCL_IED.xsd:97 */
    SCL_GenericNodeList       pClientServices;     /* SCL_IED.xsd:98 */
    SCL_GenericNodeList       pConfLdName;         /* SCL_IED.xsd:99 */
    SCL_GenericNodeList       pSupSubscription;    /* SCL_IED.xsd:100 */
    SCL_GenericNodeList       pConfSigRef;         /* SCL_IED.xsd:101 */
    SCL_GenericNodeList       pValueHandling;      /* SCL_IED.xsd:102 */
    SCL_GenericNodeList       pRedProt;            /* SCL_IED.xsd:103 */
    SCL_GenericNodeList       pTimeSyncProt;       /* SCL_IED.xsd:104 */
    SCL_GenericNodeList       pCommProt;           /* SCL_IED.xsd:105 */
    char                      *pAnyContent;       
    char                      *p_nameLength;       /* SCL_IED.xsd:107 */
} SCL_Services2;

typedef struct SCL_Subject {     /* SCL_IED.xsd:148 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_commonName;       /* SCL_IED.xsd:164 */
    char                      *p_idHierarchy;      /* SCL_IED.xsd:173 */
} SCL_Subject;

typedef struct SCL_IssuerName {     /* SCL_IED.xsd:149 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_commonName;       /* SCL_IED.xsd:164 */
    char                      *p_idHierarchy;      /* SCL_IED.xsd:173 */
} SCL_IssuerName;

typedef struct SCL_GOOSESecurity {     /* SCL_IED.xsd:134 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pSubject;            /* SCL_IED.xsd:148 */
    SCL_GenericNodeList       pIssuerName;         /* SCL_IED.xsd:149 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_xferNumber;       /* SCL_IED.xsd:151 */
    char                      *p_serialNumber;     /* SCL_IED.xsd:152 */
} SCL_GOOSESecurity;

typedef struct SCL_SMVSecurity {     /* SCL_IED.xsd:135 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pSubject;            /* SCL_IED.xsd:148 */
    SCL_GenericNodeList       pIssuerName;         /* SCL_IED.xsd:149 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_xferNumber;       /* SCL_IED.xsd:151 */
    char                      *p_serialNumber;     /* SCL_IED.xsd:152 */
} SCL_SMVSecurity;

typedef struct SCL_AccessPoint {     /* SCL_IED.xsd:50 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_Server_LN_ServerAt; /* SCL_IED.xsd:123 */
    SCL_GenericNodeList       pServices;           /* SCL_IED.xsd:133 */
    SCL_GenericNodeList       pGOOSESecurity;      /* SCL_IED.xsd:134 */
    SCL_GenericNodeList       pSMVSecurity;        /* SCL_IED.xsd:135 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:137 */
    char                      *p_router;           /* SCL_IED.xsd:138 */
    char                      *p_clock;            /* SCL_IED.xsd:139 */
    char                      *p_kdc;              /* SCL_IED.xsd:140 */
} SCL_AccessPoint;

typedef struct SCL_KDC {     /* SCL_IED.xsd:58 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_iedName;          /* SCL_IED.xsd:855 */
    char                      *p_apName;           /* SCL_IED.xsd:856 */
} SCL_KDC;

typedef struct SCL_IED {     /* SCL_IED.xsd:880 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pServices;           /* SCL_IED.xsd:49 */
    SCL_GenericNodeList       pAccessPoint;        /* SCL_IED.xsd:50 */
    SCL_GenericNodeList       pKDC;                /* SCL_IED.xsd:58 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_IED.xsd:60 */
    char                      *p_type;             /* SCL_IED.xsd:61 */
    char                      *p_manufacturer;     /* SCL_IED.xsd:62 */
    char                      *p_configVersion;    /* SCL_IED.xsd:63 */
    char                      *p_originalSclVersion; /* SCL_IED.xsd:64 */
    char                      *p_originalSclRevision; /* SCL_IED.xsd:65 */
    char                      *p_engRight;         /* SCL_IED.xsd:66 */
    char                      *p_owner;            /* SCL_IED.xsd:67 */
    SCL_SclVersionType        nSclVersion;         /* user-defined */
} SCL_IED;

typedef struct SCL_BitRate {     /* SCL_Communication.xsd:37 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_unit;             /* SCL_BaseTypes.xsd:145 */
    char                      *p_multiplier;       /* SCL_BaseTypes.xsd:146 */
} SCL_BitRate;

typedef struct SCL_P {     /* SCL_Communication.xsd:77 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_type;             /* SCL_Communication.xsd:115 */
} SCL_P;

typedef struct SCL_Address {     /* SCL_Communication.xsd:59 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pP;                  /* SCL_Communication.xsd:77 */
    char                      *pAnyContent;       
} SCL_Address;

typedef struct SCL_Address2 {     /* SCL_Communication.xsd:11 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pP;                  /* SCL_Communication.xsd:77 */
    char                      *pAnyContent;       
} SCL_Address2;

typedef struct SCL_MinTime {     /* SCL_Communication.xsd:84 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_unit;             /* SCL_BaseTypes.xsd:137 */
    char                      *p_multiplier;       /* SCL_BaseTypes.xsd:138 */
} SCL_MinTime;

typedef struct SCL_MaxTime {     /* SCL_Communication.xsd:85 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_unit;             /* SCL_BaseTypes.xsd:137 */
    char                      *p_multiplier;       /* SCL_BaseTypes.xsd:138 */
} SCL_MaxTime;

typedef struct SCL_GSE {     /* SCL_Communication.xsd:60 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pAddress;            /* SCL_Communication.xsd:11 */
    SCL_GenericNodeList       pMinTime;            /* SCL_Communication.xsd:84 */
    SCL_GenericNodeList       pMaxTime;            /* SCL_Communication.xsd:85 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_ldInst;           /* SCL_Communication.xsd:13 */
    char                      *p_cbName;           /* SCL_Communication.xsd:14 */
} SCL_GSE;

typedef struct SCL_SMV {     /* SCL_Communication.xsd:61 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pAddress;            /* SCL_Communication.xsd:11 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_ldInst;           /* SCL_Communication.xsd:13 */
    char                      *p_cbName;           /* SCL_Communication.xsd:14 */
} SCL_SMV;

typedef struct SCL_P2 {     /* SCL_Communication.xsd:99 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_type;             /* SCL_Communication.xsd:108 */
} SCL_P2;

typedef struct SCL_PhysConn {     /* SCL_Communication.xsd:62 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pP;                  /* SCL_Communication.xsd:99 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Communication.xsd:101 */
} SCL_PhysConn;

typedef struct SCL_ConnectedAP {     /* SCL_Communication.xsd:38 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pAddress;            /* SCL_Communication.xsd:59 */
    SCL_GenericNodeList       pGSE;                /* SCL_Communication.xsd:60 */
    SCL_GenericNodeList       pSMV;                /* SCL_Communication.xsd:61 */
    SCL_GenericNodeList       pPhysConn;           /* SCL_Communication.xsd:62 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedName;          /* SCL_Communication.xsd:69 */
    char                      *p_apName;           /* SCL_Communication.xsd:70 */
    char                      *p_redProt;          /* SCL_Communication.xsd:71 */
} SCL_ConnectedAP;

typedef struct SCL_SubNetwork {     /* SCL_Communication.xsd:22 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pBitRate;            /* SCL_Communication.xsd:37 */
    SCL_GenericNodeList       pConnectedAP;        /* SCL_Communication.xsd:38 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_name;             /* SCL_BaseTypes.xsd:28 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_type;             /* SCL_Communication.xsd:51 */
} SCL_SubNetwork;

typedef struct SCL_Communication {     /* SCL_Communication.xsd:325 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pSubNetwork;         /* SCL_Communication.xsd:22 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_Communication;

typedef struct SCL_DO {     /* SCL_DataTypeTemplates.xsd:32 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_DataTypeTemplates.xsd:42 */
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:43 */
    char                      *p_accessControl;    /* SCL_DataTypeTemplates.xsd:44 */
    char                      *p_transient;        /* SCL_DataTypeTemplates.xsd:45 */
    unsigned long             nOffset;             /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_DO;

typedef struct SCL_LNodeType {     /* SCL_DataTypeTemplates.xsd:135 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pDO;                 /* SCL_DataTypeTemplates.xsd:32 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_id;               /* SCL_BaseTypes.xsd:36 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedType;          /* SCL_DataTypeTemplates.xsd:34 */
    char                      *p_lnClass;          /* SCL_DataTypeTemplates.xsd:35 */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_LNodeType;

typedef struct SCL_SDO {     /* SCL_DataTypeTemplates.xsd:53 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_DataTypeTemplates.xsd:70 */
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:71 */
    char                      *p_count;            /* SCL_DataTypeTemplates.xsd:72 */
    unsigned long             nOffset;             /* user-defined */
    int                       nRtgCdcMemberIndex;  /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_SDO;

typedef struct SCL_Val2 {     /* SCL_DataTypeTemplates.xsd:16 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_sGroup;           /* SCL_BaseTypes.xsd:106 */
} SCL_Val2;

typedef struct SCL_ProtNs {     /* SCL_DataTypeTemplates.xsd:80 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:115 */
} SCL_ProtNs;

typedef struct SCL_DA {     /* SCL_DataTypeTemplates.xsd:54 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pVal;                /* SCL_DataTypeTemplates.xsd:16 */
    SCL_GenericNodeList       pProtNs;             /* SCL_DataTypeTemplates.xsd:80 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_DataTypeTemplates.xsd:18 */
    char                      *p_sAddr;            /* SCL_DataTypeTemplates.xsd:19 */
    char                      *p_bType;            /* SCL_DataTypeTemplates.xsd:20 */
    char                      *p_valKind;          /* SCL_DataTypeTemplates.xsd:21 */
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:22 */
    char                      *p_count;            /* SCL_DataTypeTemplates.xsd:23 */
    char                      *p_valImport;        /* SCL_DataTypeTemplates.xsd:24 */
    char                      *p_dchg;             /* SCL_DataTypeTemplates.xsd:8 */
    char                      *p_qchg;             /* SCL_DataTypeTemplates.xsd:9 */
    char                      *p_dupd;             /* SCL_DataTypeTemplates.xsd:10 */
    char                      *p_fc;               /* SCL_DataTypeTemplates.xsd:83 */
    unsigned long             nOffset;             /* user-defined */
    int                       nRtgCdcMemberIndex;  /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_DA;

typedef struct SCL_DOType {     /* SCL_DataTypeTemplates.xsd:141 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pCh_SDO_DA;          /* SCL_DataTypeTemplates.xsd:52 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_id;               /* SCL_BaseTypes.xsd:36 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedType;          /* SCL_DataTypeTemplates.xsd:62 */
    char                      *p_cdc;              /* SCL_DataTypeTemplates.xsd:63 */
    int                       nRtgCdcClassIndex;   /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_DOType;

typedef struct SCL_BDA {     /* SCL_DataTypeTemplates.xsd:91 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pVal;                /* SCL_DataTypeTemplates.xsd:16 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_name;             /* SCL_DataTypeTemplates.xsd:18 */
    char                      *p_sAddr;            /* SCL_DataTypeTemplates.xsd:19 */
    char                      *p_bType;            /* SCL_DataTypeTemplates.xsd:20 */
    char                      *p_valKind;          /* SCL_DataTypeTemplates.xsd:21 */
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:22 */
    char                      *p_count;            /* SCL_DataTypeTemplates.xsd:23 */
    char                      *p_valImport;        /* SCL_DataTypeTemplates.xsd:24 */
    unsigned long             nOffset;             /* user-defined */
    int                       nRtgCdcMemberIndex;  /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_BDA;

typedef struct SCL_ProtNs2 {     /* SCL_DataTypeTemplates.xsd:92 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_type;             /* SCL_DataTypeTemplates.xsd:115 */
} SCL_ProtNs2;

typedef struct SCL_DAType {     /* SCL_DataTypeTemplates.xsd:147 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pBDA;                /* SCL_DataTypeTemplates.xsd:91 */
    SCL_GenericNodeList       pProtNs;             /* SCL_DataTypeTemplates.xsd:92 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_id;               /* SCL_BaseTypes.xsd:36 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    char                      *p_iedType;          /* SCL_DataTypeTemplates.xsd:94 */
    int                       nRtgCdcClassIndex;   /* user-defined */
    int                       nTouched;            /* user-defined */
    int                       nTypeSize;           /* user-defined */
} SCL_DAType;

typedef struct SCL_EnumVal {     /* SCL_DataTypeTemplates.xsd:107 */
    SCL_GenericNode           gen;                
    char                      *pAnyContent;       
    char                      *p_ord;              /* SCL_DataTypeTemplates.xsd:128 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
} SCL_EnumVal;

typedef struct SCL_EnumType {     /* SCL_DataTypeTemplates.xsd:158 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pEnumVal;            /* SCL_DataTypeTemplates.xsd:107 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_id;               /* SCL_BaseTypes.xsd:36 */
    char                      *p_desc;             /* SCL_BaseTypes.xsd:8 */
    int                       nEnumId;             /* user-defined */
    int                       nEnumVals;           /* user-defined */
} SCL_EnumType;

typedef struct SCL_DataTypeTemplates {     /* SCL_DataTypeTemplates.xsd:170 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pLNodeType;          /* SCL_DataTypeTemplates.xsd:135 */
    SCL_GenericNodeList       pDOType;             /* SCL_DataTypeTemplates.xsd:141 */
    SCL_GenericNodeList       pDAType;             /* SCL_DataTypeTemplates.xsd:147 */
    SCL_GenericNodeList       pEnumType;           /* SCL_DataTypeTemplates.xsd:158 */
    char                      *pAnyContent;       
} SCL_DataTypeTemplates;

typedef struct SCL_Text2 {     /* SCL_BaseTypes.xsd:70 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_source;           /* SCL_BaseTypes.xsd:50 */
} SCL_Text2;

typedef struct SCL_Hitem {     /* SCL_BaseTypes.xsd:74 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_version;          /* SCL_BaseTypes.xsd:94 */
    char                      *p_revision;         /* SCL_BaseTypes.xsd:95 */
    char                      *p_when;             /* SCL_BaseTypes.xsd:96 */
    char                      *p_who;              /* SCL_BaseTypes.xsd:97 */
    char                      *p_what;             /* SCL_BaseTypes.xsd:98 */
    char                      *p_why;              /* SCL_BaseTypes.xsd:99 */
} SCL_Hitem;

typedef struct SCL_History {     /* SCL_BaseTypes.xsd:71 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pHitem;              /* SCL_BaseTypes.xsd:74 */
    char                      *pAnyContent;       
} SCL_History;

typedef struct SCL_Header {     /* SCL.xsd:18 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:70 */
    SCL_GenericNodeList       pHistory;            /* SCL_BaseTypes.xsd:71 */
    char                      *pAnyContent;       
    char                      *p_id;               /* SCL_BaseTypes.xsd:79 */
    char                      *p_version;          /* SCL_BaseTypes.xsd:80 */
    char                      *p_revision;         /* SCL_BaseTypes.xsd:81 */
    char                      *p_toolID;           /* SCL_BaseTypes.xsd:82 */
    char                      *p_nameStructure;    /* SCL_BaseTypes.xsd:83 */
} SCL_Header;

typedef struct SCL_SCL {     /* SCL.xsd:13 */
    SCL_GenericNode           gen;                
    SCL_GenericNodeList       pANY_ELEMENT;       
    SCL_GenericNodeList       pText;               /* SCL_BaseTypes.xsd:13 */
    SCL_GenericNodeList       pPrivate;            /* SCL_BaseTypes.xsd:14 */
    SCL_GenericNodeList       pHeader;             /* SCL.xsd:18 */
    SCL_GenericNodeList       pSubstation;         /* SCL_Substation.xsd:700 */
    SCL_GenericNodeList       pCommunication;      /* SCL_Communication.xsd:325 */
    SCL_GenericNodeList       pIED;                /* SCL_IED.xsd:880 */
    SCL_GenericNodeList       pDataTypeTemplates;  /* SCL_DataTypeTemplates.xsd:170 */
    SCL_GenericNodeList       pLine;               /* SCL_Substation.xsd:728 */
    SCL_GenericNodeList       pProcess;            /* SCL_Substation.xsd:714 */
    char                      *pAnyContent;       
    SCL_GenericAttributeList  pANY_ATTRIBUTE;     
    char                      *p_version;          /* SCL.xsd:33 */
    char                      *p_revision;         /* SCL.xsd:34 */
    SCL_SclVersionType        nSclVersion;         /* user-defined */
    void *                    pUserData;           /* user-defined */
} SCL_SCL;

typedef enum {
    ANY_ATTRIBUTE_Attr,
    XMLNS_ATTRIBUTE_Attr,
    accessControl_Attr,
    actSG_Attr,
    apName_Attr,
    apRef_Attr,
    appID_Attr,
    associationID_Attr,
    bType_Attr,
    bayName_Attr,
    bufConf_Attr,
    bufMode_Attr,
    bufOvfl_Attr,
    bufReport_Attr,
    bufTime_Attr,
    buffered_Attr,
    c37_238_Attr,
    cNodeName_Attr,
    cbName_Attr,
    cdc_Attr,
    certificate_Attr,
    client_Attr,
    clock_Attr,
    commonName_Attr,
    confRev_Attr,
    configRef_Attr,
    configVersion_Attr,
    connectivityNode_Attr,
    count_Attr,
    daName_Attr,
    datSet_Attr,
    dataLabel_Attr,
    dataRef_Attr,
    dataSet_Attr,
    dchg_Attr,
    delivery_Attr,
    deliveryConf_Attr,
    desc_Attr,
    doName_Attr,
    dupd_Attr,
    engRight_Attr,
    entryID_Attr,
    fc_Attr,
    fixLnInst_Attr,
    fixPrefix_Attr,
    fixedOffs_Attr,
    ftp_Attr,
    ftps_Attr,
    gi_Attr,
    goose_Attr,
    gsse_Attr,
    hsr_Attr,
    id_Attr,
    idHierarchy_Attr,
    iedName_Attr,
    iedType_Attr,
    indexed_Attr,
    inst_Attr,
    intAddr_Attr,
    intgPd_Attr,
    ipv6_Attr,
    ix_Attr,
    kdc_Attr,
    kind_Attr,
    ldInst_Attr,
    ldName_Attr,
    lnClass_Attr,
    lnInst_Attr,
    lnType_Attr,
    logEna_Attr,
    logName_Attr,
    manufacturer_Attr,
    max_Attr,
    maxAttributes_Attr,
    maxGOOSE_Attr,
    maxGo_Attr,
    maxReports_Attr,
    maxSMV_Attr,
    maxSv_Attr,
    mms_Attr,
    modify_Attr,
    multicast_Attr,
    multiplier_Attr,
    mustUnderstand_Attr,
    name_Attr,
    nameLength_Attr,
    nameStructure_Attr,
    nofASDU_Attr,
    nomFreq_Attr,
    none_Attr,
    numOfSGs_Attr,
    numPhases_Attr,
    optFields_Attr,
    ord_Attr,
    originalSclRevision_Attr,
    originalSclVersion_Attr,
    other_Attr,
    owner_Attr,
    password_Attr,
    pathName_Attr,
    pdcTimeStamp_Attr,
    period_Attr,
    phase_Attr,
    prefix_Attr,
    processName_Attr,
    prp_Attr,
    qchg_Attr,
    readLog_Attr,
    reasonCode_Attr,
    redProt_Attr,
    refreshTime_Attr,
    resvTms_Attr,
    revision_Attr,
    router_Attr,
    rptID_Attr,
    rstp_Attr,
    sAddr_Attr,
    sGroup_Attr,
    sampleRate_Attr,
    sampleSynchronized_Attr,
    samplesPerSec_Attr,
    security_Attr,
    securityEnable_Attr,
    segmentation_Attr,
    seqNum_Attr,
    serialNumber_Attr,
    serviceType_Attr,
    setToRO_Attr,
    smpMod_Attr,
    smpRate_Attr,
    smvID_Attr,
    sntp_Attr,
    source_Attr,
    srcCBName_Attr,
    srcLDInst_Attr,
    srcLNClass_Attr,
    srcLNInst_Attr,
    srcPrefix_Attr,
    strong_Attr,
    substationName_Attr,
    supportsLdName_Attr,
    sv_Attr,
    svID_Attr,
    timeStamp_Attr,
    timeout_Attr,
    timestamp_Attr,
    toolID_Attr,
    transient_Attr,
    trgOps_Attr,
    type_Attr,
    unbufReport_Attr,
    unit_Attr,
    valImport_Attr,
    valKind_Attr,
    version_Attr,
    virtual_Attr,
    voltageLevelName_Attr,
    weak_Attr,
    what_Attr,
    when_Attr,
    who_Attr,
    why_Attr,
    xferNumber_Attr,
} SCL_AttributeType;

typedef enum {
    AccessControl_Elem,
    AccessPoint_Elem,
    Address_Elem,
    Association_Elem,
    Authentication_Elem,
    BDA_Elem,
    Bay_Elem,
    BitRate_Elem,
    ClientLN_Elem,
    ClientServices_Elem,
    CommProt_Elem,
    Communication_Elem,
    ConductingEquipment_Elem,
    ConfDataSet_Elem,
    ConfLNs_Elem,
    ConfLdName_Elem,
    ConfLogControl_Elem,
    ConfReportControl_Elem,
    ConfSG_Elem,
    ConfSigRef_Elem,
    ConnectedAP_Elem,
    ConnectivityNode_Elem,
    DA_Elem,
    DAI_Elem,
    DAType_Elem,
    DO_Elem,
    DOI_Elem,
    DOType_Elem,
    DataObjectDirectory_Elem,
    DataSet_Elem,
    DataSetDirectory_Elem,
    DataTypeTemplates_Elem,
    DynAssociation_Elem,
    DynDataSet_Elem,
    EnumType_Elem,
    EnumVal_Elem,
    EqFunction_Elem,
    EqSubFunction_Elem,
    ExtRef_Elem,
    FCDA_Elem,
    FileHandling_Elem,
    Function_Elem,
    GOOSE_Elem,
    GOOSESecurity_Elem,
    GSE_Elem,
    GSEControl_Elem,
    GSEDir_Elem,
    GSESettings_Elem,
    GSSE_Elem,
    GeneralEquipment_Elem,
    GetCBValues_Elem,
    GetDataObjectDefinition_Elem,
    GetDataSetValue_Elem,
    GetDirectory_Elem,
    Header_Elem,
    History_Elem,
    Hitem_Elem,
    IED_Elem,
    IEDName_Elem,
    Inputs_Elem,
    IssuerName_Elem,
    KDC_Elem,
    LDevice_Elem,
    LN_Elem,
    LN0_Elem,
    LNode_Elem,
    LNodeType_Elem,
    Line_Elem,
    Log_Elem,
    LogControl_Elem,
    LogSettings_Elem,
    MaxTime_Elem,
    MinTime_Elem,
    NeutralPoint_Elem,
    OptFields_Elem,
    P_Elem,
    PhysConn_Elem,
    PowerTransformer_Elem,
    Private_Elem,
    Process_Elem,
    ProtNs_Elem,
    Protocol_Elem,
    ReadWrite_Elem,
    RedProt_Elem,
    ReportControl_Elem,
    ReportSettings_Elem,
    RptEnabled_Elem,
    SCL_Elem,
    SCLControl_Elem,
    SDI_Elem,
    SDO_Elem,
    SGEdit_Elem,
    SMV_Elem,
    SMVSecurity_Elem,
    SMVSettings_Elem,
    SMVsc_Elem,
    SampledValueControl_Elem,
    SamplesPerSec_Elem,
    SecPerSamples_Elem,
    Server_Elem,
    ServerAt_Elem,
    Services_Elem,
    SetDataSetValue_Elem,
    SettingControl_Elem,
    SettingGroups_Elem,
    SmpRate_Elem,
    SmvOpts_Elem,
    SubEquipment_Elem,
    SubFunction_Elem,
    SubNetwork_Elem,
    Subject_Elem,
    Substation_Elem,
    SupSubscription_Elem,
    TapChanger_Elem,
    Terminal_Elem,
    Text_Elem,
    TimeSyncProt_Elem,
    TimerActivatedControl_Elem,
    TransformerWinding_Elem,
    TrgOps_Elem,
    Val_Elem,
    ValueHandling_Elem,
    Voltage_Elem,
    VoltageLevel_Elem,
    ANY_ELEMENT_Elem
} SCL_ElementType;

typedef enum {
    SCL_State_AccessControl,
    SCL_State_AccessPoint,
    SCL_State_Address,
    SCL_State_Association,
    SCL_State_Authentication,
    SCL_State_BDA,
    SCL_State_Bay,
    SCL_State_BitRate,
    SCL_State_ClientLN,
    SCL_State_ClientServices,
    SCL_State_CommProt,
    SCL_State_Communication,
    SCL_State_ConductingEquipment,
    SCL_State_ConfDataSet,
    SCL_State_ConfLNs,
    SCL_State_ConfLdName,
    SCL_State_ConfLogControl,
    SCL_State_ConfReportControl,
    SCL_State_ConfSG,
    SCL_State_ConfSigRef,
    SCL_State_ConnectedAP,
    SCL_State_ConnectivityNode,
    SCL_State_DA,
    SCL_State_DAI,
    SCL_State_DAType,
    SCL_State_DO,
    SCL_State_DOI,
    SCL_State_DOType,
    SCL_State_DataObjectDirectory,
    SCL_State_DataSet,
    SCL_State_DataSetDirectory,
    SCL_State_DataTypeTemplates,
    SCL_State_DynAssociation,
    SCL_State_DynDataSet,
    SCL_State_EnumType,
    SCL_State_EnumVal,
    SCL_State_EqFunction,
    SCL_State_EqSubFunction,
    SCL_State_ExtRef,
    SCL_State_FCDA,
    SCL_State_FileHandling,
    SCL_State_Function,
    SCL_State_GOOSE,
    SCL_State_GOOSESecurity,
    SCL_State_GSE,
    SCL_State_GSEControl,
    SCL_State_GSEDir,
    SCL_State_GSESettings,
    SCL_State_GSSE,
    SCL_State_GeneralEquipment,
    SCL_State_GetCBValues,
    SCL_State_GetDataObjectDefinition,
    SCL_State_GetDataSetValue,
    SCL_State_GetDirectory,
    SCL_State_Header,
    SCL_State_History,
    SCL_State_Hitem,
    SCL_State_IED,
    SCL_State_IEDName,
    SCL_State_Inputs,
    SCL_State_IssuerName,
    SCL_State_KDC,
    SCL_State_LDevice,
    SCL_State_LN,
    SCL_State_LN0,
    SCL_State_LNode,
    SCL_State_LNodeType,
    SCL_State_Line,
    SCL_State_Log,
    SCL_State_LogControl,
    SCL_State_LogSettings,
    SCL_State_MaxTime,
    SCL_State_MinTime,
    SCL_State_NeutralPoint,
    SCL_State_OptFields,
    SCL_State_P,
    SCL_State_PhysConn,
    SCL_State_PowerTransformer,
    SCL_State_Private,
    SCL_State_Process,
    SCL_State_ProtNs,
    SCL_State_Protocol,
    SCL_State_ReadWrite,
    SCL_State_RedProt,
    SCL_State_ReportControl,
    SCL_State_ReportSettings,
    SCL_State_RptEnabled,
    SCL_State_SCL,
    SCL_State_SCLControl,
    SCL_State_SDI,
    SCL_State_SDO,
    SCL_State_SGEdit,
    SCL_State_SMV,
    SCL_State_SMVSecurity,
    SCL_State_SMVSettings,
    SCL_State_SMVsc,
    SCL_State_SampledValueControl,
    SCL_State_SamplesPerSec,
    SCL_State_SecPerSamples,
    SCL_State_Server,
    SCL_State_ServerAt,
    SCL_State_Services,
    SCL_State_SetDataSetValue,
    SCL_State_SettingControl,
    SCL_State_SettingGroups,
    SCL_State_SmpRate,
    SCL_State_SmvOpts,
    SCL_State_SubEquipment,
    SCL_State_SubFunction,
    SCL_State_SubNetwork,
    SCL_State_Subject,
    SCL_State_Substation,
    SCL_State_SupSubscription,
    SCL_State_TapChanger,
    SCL_State_Terminal,
    SCL_State_Text,
    SCL_State_TimeSyncProt,
    SCL_State_TimerActivatedControl,
    SCL_State_TransformerWinding,
    SCL_State_TrgOps,
    SCL_State_Val,
    SCL_State_ValueHandling,
    SCL_State_Voltage,
    SCL_State_VoltageLevel,
    SCL_State_Idle,
    SCL_State_Skipping,
} SCL_ParseState;


#include "cscl_lex.h"

typedef void (* Attribute_Indication)   ( void                  *param, 
                                          SCL_AttributeType    attr, 
                                          char                  *value );

typedef void *(* Element_OpenIndication)( void                  *param, 
                                          SCL_ElementType      elem );

typedef void (* Element_CloseIndication)( void                  *param, 
                                          void                  *openParam,
                                          SCL_ElementType      elem );

int  SCL_Parse( SCL_parser_context_type *pLexContext,
                void                      *param,
                Attribute_Indication      attr_ind,
                Element_OpenIndication    elem_open_ind,
                Element_CloseIndication   elem_close_ind,
                TmwTargTraceMessageFunc   err_message_ind,
                void                      *err_message_param,
                SCL_GenericNode       **pRetRoot);
void SCL_init_lex( SCL_parser_context_type *pParserContext, FILE *fd );
void SCL_reset_lex( SCL_parser_context_type *pParserContext );
void SCL_Discard( SCL_parser_context_type *pParserContext );
void SCL_NoDiscard( SCL_parser_context_type *pParserContext );
void SCL_freeGenericNode( SCL_GenericNode *pNode );
void SCL_freeGenericNodeList( SCL_GenericNode *pNode );

/* Tests for various forms of true - returns false if NULL */
int SCL_TestTrueFalse( char *str );

/* Converts test to integer, NULL is returned as zero */
TMW_CLIB_API int SCL_AtoI( char *str );

/* Converts test to integer with error checks */
int SCL_Val_AtoI( char *str, int *pResult );

/* Retrieve IEC 61850 Version from overall SCL file */
SCL_SclVersionType SCL_RTG_getSclVersionInfo( SCL_SCL *pScl );

/* Retrieve IEC 61850 Version from IED */
SCL_SclVersionType SCL_RTG_getIedVersionInfo( SCL_IED *pIED );

/* Retrieve Service element data from SCL */
int SCL_SVC_RetrieveServiceSettings( SCL_Services *pServices, SCL_SVC_ServiceSettingsType *pSettingsType );


#ifdef __cplusplus
};
#endif

#endif /*  _CSCL_H */
