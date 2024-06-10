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
 *  Declarations for
 *
 *  This file should not require modification.
 */

#ifndef _RTG_H
#define _RTG_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MMSD_MAX_MODELS_PER_LD)
#define MMSD_MAX_MODELS_PER_LD 100
#endif

#if !defined(MMSD_MAX_NVLS)
#define MMSD_MAX_NVLS           120
#endif

#if !defined(MMSD_MAX_NVL_MEMBERS)
#define MMSD_MAX_NVL_MEMBERS    120
#endif

#if !defined(MMSD_MAX_RCBS)
#define MMSD_MAX_RCBS           100
#endif

#if !defined(MMSD_MAX_JOURNALS)
#define MMSD_MAX_JOURNALS       10
#endif

/* #define DEBUG_RTG_REPORTING */

#define RTG_DEFAULT_TYPE           0
#define STRUCTURE                 -2

typedef signed int MMSd_RTG_Form;
typedef unsigned char MMSd_RTG_Index;
typedef int MMSd_RTG_Dimension;
typedef unsigned char MMSd_RTG_TabDimension;
typedef int MMSd_RTG_Size;
typedef unsigned char MMSd_RTG_NVL_Size;
typedef unsigned char MMSd_RTG_Options;

#define MMSd_OPT_read       0x80
#define MMSd_OPT_write      0x40
#define MMSd_OPT_optional   0x20
#define MMSd_OPT_custom     0x10
#define MMSd_OPT_ignored    0x08
#define MMSd_OPT_bit_size   0x04
#define MMSd_OPT_array      0x02
#define MMSd_OPT_fc_wrapper 0x01

typedef unsigned char MMSd_RTG_CB_type;

#define MMSd_RTG_NOCB_type      0
#define MMSd_RTG_RCB_type       1
#define MMSd_RTG_URCB_type      2
#define MMSd_RTG_BRCB_type      3
#define MMSd_RTG_LCB_type       4
#define MMSd_RTG_GOCB_type      5
#define MMSd_RTG_GSCB_type      6
#define MMSd_RTG_MSMVCB_type    7
#define MMSd_RTG_USMVCB_type    8
#define MMSd_RTG_SGCB_type      9

/* Deepest structure nesting */
#define MMSd_MAX_RTG_DEPTH 7

typedef MMSd_RTG_Index MMSd_RTG_Canonical[MMSd_MAX_RTG_DEPTH + 1];

typedef struct MMSd_RTG_NVL_Defs {
    const char *name;
    MMSd_RTG_NVL_Size index; /* into MMSd_RTG_NVL_members */
    MMSd_RTG_NVL_Size count; /* number of member entries */
} MMSd_RTG_NVL_Defs;

typedef struct MMSd_RTG_RCB_Def {
    MMSd_RTG_Canonical rcbName;
    const char *name;
    MMSd_RTG_Index nvl;
    char *efunc;
    char tropts;
    MMSd_RTG_CB_type cbType; /* Added V7.22 */
} MMSd_RTG_RCB_Def;

typedef struct MMSd_Handler_Attr {
    int sizeKind;
    void *triggerMask;
    void *triggerSubstitute;
    void *userData;
} MMSd_Handler_Attr;

/* For sizeKind, above */
#define MMSD_BYTE_SIZE 0
#define MMSD_BIT_SIZE  1

typedef struct GOMSFE_Entry {
    const char *name;
    MMSd_RTG_Form classForm; /* Table index of type, or MMS primitive */
    MMSd_RTG_Index classIndex; /* Index in structure, or # of members */
    MMSd_RTG_TabDimension dimension; /* Array dimension (zero for scalar types) */
    MMSd_RTG_Size size; /* Base type size */
    MMSd_RTG_Options options; /* Options (bits defined above) */
} GOMSFE_Entry;

#define MAX_DEPTH 9
typedef struct MMSd_RTG_entry {
    char *fullNamePtr;
    char *namePtr;
    void *valuePtr;
    int flag;
    int gomsfe_index;
    int alt_index;
    int mmsd_index;
    int mmsd_type_index;
    int typeCode;
    int size;
    int dimension;
    int parent_stack_index;
    int entry_state;
    void *userData;
    unsigned char canon[MAX_DEPTH];
} MMSd_RTG_entry;

typedef struct {
    void *reportList;
    int reportListSize;
} MMSd_reportListBase;

typedef struct {
    char *name;
    int model;
} MMSd_modelInstance;

typedef struct {
    MMSd_NvlEval *nvlEval;
    int num_evalLists;
} MMSd_DomainEvalDirectory; /* One for each domain */

typedef struct RTG_TypeTranslate {
    const char *sclName;
    int rtgHandlerIndex;
    int typeSize;
    unsigned char sizeFlag;
    int cSize;
} RTG_TypeTranslate;

typedef void (*MMSd_RTG_RptDiscardFcn)(char *errorName,
                                       int errcode,
                                       void *pParameter);

typedef struct MMSd_RTG_State {
    int gomsfe_major_version;
    int gomsfe_minor_version;
    unsigned char *alloc_space;
    unsigned long alloc_space_size;
    int maxDepth;
    int num_spareNVLs; /* GS 05/01/02 */
    int current;
    GOMSFE_Entry *gomsfe;
    int num_classes;
    int num_models;
    MMSd_context *mms_cntxt;
    directoryType *directory;
    int max_directory_entries;
    int num_directory_entries;
    MMSd_ReportControl *reportControlList;
    MMSd_ReportControl *logControlList;
    MMSd_ReportControl *gooseControlList;
    MMSd_ReportControl *gseControlList;
    MMSd_ReportControl *msmvControlList;
    MMSd_ReportControl *usmvControlList;
    MMSd_ReportControl *sgControlList;
    TMW_TrackingObject *trackingObjectList;
    TMW_TrackingVariant trackingVariant;
    MMSd_entry *dictionary;
    int maxDict;
    MMSd_list_entry *references;
    int maxRefs;

    /* New to support multiple GOMSFE versions - GS 02/04/99 */
    const MMSd_RTG_RCB_Def *gomsfe_rcbs;
    int num_gomsfe_rcbs;
    const MMSd_RTG_NVL_Defs *gomsfe_nvls;
    int num_gomsfe_nvls;
    const MMSd_RTG_Canonical *gomsfe_nvl_members;
    int num_gomsfe_nvl_members;

    MMSd_modelInstance *saveModels;
    int max_saveModels;
    int num_saveModels;
    int scl_defer_rw_only;
    int domain;

    const MMSd_ExtendedHandlerSet *mms_handlers;
    int num_handlers;
    int nDataObjectHandler;
    int (*get_entry)(struct MMSd_RTG_State *state,
                     MMSd_RTG_entry *entry);
    char use_custom;
    char use_scl_order;
    char use_tase2;
    void *user_ptr;

    /* Storage allocated by user */
    int max_dictionary_entries;
    int max_reference_entries;
    char *name_storage;
    int max_name_storage_bytes;

    directoryType *remain_directory;
    MMSd_entry *remain_dictionary;
    MMSd_list_entry *remain_references;
    char *remain_name_storage;
    int remain_directory_entries;
    int remain_dictionary_entries;
    int remain_reference_entries;
    int remain_name_storage_bytes;

    MMSd_RTG_RptDiscardFcn pReportDiscardFcn;
    void *pReportDiscardParameter;
    int omitStdNvls;
    int omitStdCbs;
    int num_cbs;
    int numHidden;

    MMSd_CdcClass *pCdcClass;
    int nCdcClass;
    MMSd_CdcClassMember *pCdcClassMember;
    int nCdcClassMember;
    TMW_CdcTrackingDirectory *pCdcTrackingDirectory;

    char **pFuncCompNames;
    int nFuncCompNames;

    RTG_TypeTranslate *typeTranslationTable;
    int nTypeTranslationTableSize;

    MMSd_entry *currentNVL;
    int domains_reordered;

    char *pModelVersion;
    char *pModelRevision;

} MMSd_RTG_State;

#define MMSd_RTG_SUCCESS                 0
#define MMSd_RTG_POSSIBLE_CUSTOM        -1
#define MMSd_RTG_PRIMITIVE_CONSTRUCTOR  -2
#define MMSd_RTG_STRUCTURE_REFERENCE    -3
#define MMSd_RTG_DICTIONARY_OVERFLOW    -4
#define MMSd_RTG_STACK_OVERFLOW         -5
#define MMSd_RTG_NOT_FOUND              -6
#define MMSD_CUSTOM_MODEL               -7
#define MMSd_RTG_BAD_CANNONICAL_FORM    -8
#define MMSd_RTG_END_OF_DATA            -9
#define MMSd_RTG_OUT_OF_MEMORY         -10
#define MMSd_RTG_CUSTOM_NOT_CONFIGURED -11
#define MMSd_RTG_TYPE_NOT_SPECIFIED    -12
#define MMSd_RTG_BAD_GOMSFE_VERSION    -13
#define MMSd_RTG_DIR_ENTRIES_EXCEEDED  -14
#define MMSd_RTG_ARRAY_OF_ARRAY        -15
#define MMSd_RTG_ARRAY_INSTANCE        -16
#define MMSd_RTG_MEMORY_ALLOCATION     -17
#define MMSd_RTG_ILLEGAL_NAME          -18
#define MMSd_RTG_REF_ENTRIES_EXCEEDED  -19
#define MMSd_RTG_DUPLICATE_NAME        -20
#define MMSd_RTG_NESTING_LEVEL_EXCEEDED -21
#define MMSd_RTG_INTERNAL_ERROR        -22
#define MMSd_RTG_DIRECTORY_OPEN_ERROR  -23
#define MMSd_RTG_BAD_OBJECT_CLASS      -24
#define MMSd_RTG_NVL_SEQUENCE_ERR      -25

#define MMSd_IB_IsInclusion     0x80
#define MMSd_IB_SBO             0x40
#define MMSd_IB_TAG             0x20
#define MMSd_IB_SECURE          0x10
#define MMSd_IB_CASMVersion     0x08
#define MMSd_IB_reserved1       0x04
#define MMSd_IB_reserved2       0x02
#define MMSd_IB_reserved3       0x01

typedef int RTG_Error;

typedef struct MMSd_StarComponentDef {
    char *name;
    int nameLen;
    int typeCode;
} MMSd_StarComponentDef;

typedef struct MMSd_StarDirect {
    MMSd_RTG_Index index;
    int offset;
    int numComponents;
} MMSd_StarDirect;

#if defined(TMW_STATIC_MODEL_TOOL)
typedef void (* TMW_GenericHandlerFcnPtr)(void);

typedef struct TMW_FunctionResolution {
    TMW_GenericHandlerFcnPtr pAddress;
    char *pName;
} TMW_FunctionResolution;
#endif

typedef struct {
    int gomsfe_major_version;
    int gomsfe_minor_version;
    const GOMSFE_Entry *gomsfe;
    const MMSd_RTG_Canonical *gomsfe_nvl_members; /* unused as of V10.0 - need to remove code */  //FIXME
    const MMSd_RTG_NVL_Defs *gomsfe_nvls; /* unused as of V10.0 - need to remove code */
    const MMSd_RTG_RCB_Def *gomsfe_rcbs; /* unused as of V10.0 - need to remove code */
    int num_classes;
    int num_models; /* unused as of V10.0 - need to remove code */
    int num_control_blocks;
    int num_gomsfe_nvls; /* unused as of V10.0 - need to remove code */
    int num_gomsfe_nvl_members; /* unused as of V10.0 - need to remove code */
    int num_gomsfe_rcbs;
    MMSd_ExtendedHandlerSet *handlers;
    int num_handlers;
    MMSd_CdcClass *pCdcClass;
    int nCdcClass;
    MMSd_CdcClassMember *pCdcClassMember;
    int nCdcClassMember;
    TMW_CdcTrackingDirectory *pCdcTrackingDirectory;
    char **pFuncCompNames;
    int nFuncCompNames;
    RTG_TypeTranslate *typeTranslationTable;
    int nTypeTranslationTableSize;
    char *pModelVersion;
    char *pModelRevision;
#if defined(TMW_STATIC_MODEL_TOOL)
    const TMW_FunctionResolution *pFunctionResolution;
    int nFunctionResolution;
#endif
} MMSd_RTG_Gomsfe_Release;

/* Builds full MMS Dictionary */
int MMSd_rtg_resolve(MMSd_RTG_State *state);
#if defined(TMW_USE_JOURNALS)
int MMSd_RTG_AddJournal( MMSd_RTG_State *state, char *journalName,
                         unsigned char *buffer, unsigned long buffSize );

MMSd_Journal * MMSd_RTG_FindJournal( MMSd_RTG_State *state, char *journalName );
#endif
void MMSd_fixup_LogControls(MMSd_RTG_State *state);

/* Looks up a GOMSFE entry based on name */
int MMSd_GomsfeLookup(char *namePtr, unsigned char *canon, GOMSFE_Entry *gomsfe, int *altIndex, int first_rec, int num_recs, MMSd_RTG_State *state);

int MMSd_GomsfeSearch(char *namePtr, GOMSFE_Entry *gomsfe, int first_rec, int num_recs);

int MMSd_GomsfeModelSearch(char *namePtr, GOMSFE_Entry *gomsfe, int first_rec, int num_recs);

void sort_stack(MMSd_RTG_State *state, int limit);

/* Looks up a GOMSFE entry based on cannonical numbering scheme.
 If nameBuff is not NULL, returns MMS name of entry */
int MMSd_GomsfeFindIndex(char *nameBuff, char delimiter, GOMSFE_Entry *gomsfe, int num_classes, int num_models, unsigned char *canon);

int MMSd_GomsfeFindName(char *nameBuff, char delimiter, GOMSFE_Entry *gomsfe, int num_classes, int num_models, unsigned char *canon, char *modelName);

int MMSd_GomsfeFindDimension(MMSd_RTG_State *state, unsigned char *canon);

int MMSd_GomsfeFind(MMSd_RTG_State *state, unsigned char *canon);

int rtg_compare(MMSd_RTG_entry *top, MMSd_RTG_entry *prev);

int MMSd_RTG_compare_RP(char *name1, char *name2);

int MMSd_RTG_get_dimension(MMSd_RTG_State *state, int gomsfe_index);

/* Find index of start of last component name */
int MMSd_last_rtg_component(char *name);

/* Used for domain parsing of input vnames */

char *MMSd_RTG_skip_domain(char *fullNamePtr);

char *MMSd_RTG_get_new_domain(MMSd_RTG_State *state, char *newScan, char *fullNamePtr);

int MMSd_OpenDirectory(MMSd_RTG_State *state, char *domainName);

int MMSd_CloseDirectory(MMSd_RTG_State *state);

int MMSd_GomsfePrimitive(GOMSFE_Entry *gomsfe, int index);

int MMSd_next_rtg_component(char *name);

void MMSd_RTG_report_discard(char *name, int errCode);

int MMSd_RTG_init_gomsfe_version(MMSd_RTG_State *state);

int MMSd_RTG_check_new_model(MMSd_RTG_entry *entry, MMSd_RTG_State *state);

TMW_CdcTrackingDirectory *TMW_RTG_getTrackingDirectory(TMW_TRK_ClassOption nTrackingCdcOptions, MMSd_RTG_State *pRtgState);

int TMW_RTG_getTrackingIndex(char *pName, TMW_CdcTrackingRange *pTrackingRange);

TMW_CdcTrackingRange *TMW_RTG_getTrackingRange(char *pName, TMW_CdcTrackingRange *pTrackingRange);

int MMSd_RTG_compare_domains(char *domainName, int domainLen, char *newVarName, int *newDomainLen);

int MMSd_rtg_init(MMSd_RTG_State *state);

int MMSd_rtg_init_by_name(MMSd_RTG_State *state, char *pModelVersion, char *pModelRevision);

TMW_CLIB_API int MMSd_RTG_open_domain(MMSd_RTG_State *pState, char *domainName);

int MMSd_RTG_add_variable(MMSd_RTG_State *pState, int domainIndex, char *name, int size, int dimension, int options, int typeCode, void *pVoidValue);

int MMSd_RTG_add_variable_ordered(MMSd_RTG_State *pState, int domainIndex, char *name, int nOrder, int size, int dimension, int options, int typeCode, MMSd_indexHandler indexHandler,
                                  MMSd_accessHandler accessHandler, void *pIndexParameter, int nElementSize, void *pVoidValue);

/* V7.91 - added to specify class of custom objects */
int MMSd_RTG_add_variable_class(MMSd_RTG_State *pState, int domainIndex, char *name, char *className, int size, int dimension, int options, void *pVoidValue);

int MMSd_RTG_add_variable_class_ordered(MMSd_RTG_State *pState, int domainIndex, char *name, int nOrder, char *className, int size, int dimension, int options, MMSd_indexHandler indexHandler,
                                        MMSd_accessHandler accessHandler, void *pIndexParameter, int nElementSize, void *pVoidValue);

int MMSd_RTG_close_domain_vars(MMSd_RTG_State *pState, int domainIndex);

int MMSd_RTG_close_domain(MMSd_RTG_State *pState, int domainIndex);

int MMSd_RTG_add_named_variable_list(MMSd_RTG_State *pState, int domainIndex, char *name);

int MMSd_RTG_open_named_variable_list(MMSd_RTG_State *pState, char *nvlDomName, char *nvlName, MMSd_entry **pNvlDictEntry);

char *MMSd_RTG_alloc_string(MMSd_RTG_State *pState, char *str, int len);

int MMSd_RTG_add_named_variable_list_member(MMSd_RTG_State *pState, char *domName, char *memberName, MMSd_entry *pNvlEntry);

int MMSd_RTG_close_named_variable_list(MMSd_RTG_State *pState, MMSd_entry *pNvlEntry);

int MMSd_RTG_output_structure_header(MMSd_RTG_State *pState, int domainIndex, char *pNameBuffer);

int MMSd_RTG_output_structure_header_ordered(MMSd_RTG_State *pState, int domainIndex, char *pNameBuffer, int nOrder);

int MMSd_output_rcb(MMSd_RTG_State *pState, int domainIndex, char *cbName, MMSd_ObjectName *DatSet, char *pRptID, unsigned long intgPd, unsigned long ConfRev, unsigned char *OptFlds,
                    unsigned long BufTim, unsigned char TrgOps, char bIsBuffered, char *className, unsigned long nReadEnableSwitches, unsigned long nWriteEnableSwitches, MMSd_ReportControl *reportControl);

int MMSd_output_lcb(MMSd_RTG_State *pState, int domainIndex, char *cbName, unsigned char LogEna, char *LogRef, MMSd_ObjectName *DatSet, unsigned long intgPd, unsigned char trgOps,
                    unsigned char *OptFlds, char *className, unsigned long nReadEnableSwitches, unsigned long nWriteEnableSwitches, MMSd_ReportControl *reportControl);

int MMSd_output_gcb(MMSd_RTG_State *pState, int domainIndex, char *cbName, unsigned char GoEna, char *GoID, MMSd_ObjectName *DatSet, unsigned long ConfRev, unsigned char NdsCom,
                    unsigned char *DstAddr, unsigned char Priority, unsigned short VID, unsigned short APPID, char *className, unsigned long nReadEnableSwitches, unsigned long nWriteEnableSwitches,
                    MMSd_ReportControl *reportControl);

int MMSd_output_gsecb(MMSd_RTG_State *pState, int domainIndex, char *cbName, unsigned char GsEna, char *GsID, char **pDnaLabels, int nDnaLabels, char **pUserStLabels, int nUserStLabels,
                      char *className, MMSd_ReportControl *reportControl);

int MMSd_output_smvcb(MMSd_RTG_State *pState, int domainIndex, char *cbName, unsigned char multicast, unsigned char SvEna, char *MsvID, MMSd_ObjectName *DatSet, unsigned long ConfRev,
                      unsigned int SmpRate, unsigned int nofASDU, unsigned char *DstAddr, unsigned char Priority, unsigned int VID, unsigned int APPID, char *className, unsigned long nReadEnableSwitches,
                      unsigned long nWriteEnableSwitches, MMSd_ReportControl *reportControl);

int MMSd_output_sgcb(MMSd_RTG_State *pState, int domainIndex, char *cbName, char *className, MMSd_ReportControl *pSgcb);

int MMSd_RTG_locateDomain(char *name, MMSd_RTG_State *pState);
int MMSd_RTG_locateEntry(char *name, MMSd_entry *entryList, int listLen);

int MMSd_rtg_resolve2(char *domainName, MMSd_RTG_State *state);

int MMSd_rtg_finish(MMSd_RTG_State *pState);

int MMSd_RTG_get_c_type_size(MMSd_RTG_State *pState, int index);

int MMSd_RTG_get_c_element_size(MMSd_RTG_State *pState, int domainIndex, int index);

/* Gets CDC class index based on standardized names */
int MMSd_RTG_object_lookup(MMSd_RTG_State *pState, char *pDoName);

int MMSd_RTG_lookup_handler_name(MMSd_RTG_State *pState, char *name);

/* Retrieves type information based on CDC class index */
/* Note that CDC class index could be from MMSd_RTG_object_lookup or
 from 61850_1.H (I_CDC_xxx) */
int MMSd_RTG_object_definition(MMSd_RTG_State *pState, int nCdcIndex, char **pCdcClassName, int *options, int *mmsSize, int *cSize, MMSd_FuncCompBits *pFcBits, int *typeCode);

int MMSd_RTG_add_object(MMSd_RTG_State *pState, int domainIndex, char *name, int size, int dimension, int options, int nCdcIndex, unsigned char *inclusionMask, void *pVoidValue);

int MMSd_RTG_add_structure(MMSd_RTG_State *pState, int domainIndex, char *name, int size, int dimension, int options, int typeIndex, void *pVoidValue);

/* Gets handler index based on standardized names */
int MMSd_RTG_variable_lookup(MMSd_RTG_State *pState, char *pVarName, int *mmsSize, int *dimension, int *options);

/* Retrieves type information based on handler index */
/* Note that handler index could be from MMSd_RTG_handler_lookup or
 from 61850_1.H (Z_xxx) */
int MMSd_RTG_handler_definition(MMSd_RTG_State *pState, int nHandlerIndex, char **pHandlerName, int *mmsSize, int *cSize, int *dimension);

int MMSd_RTG_FindCdcClass(char *pName, MMSd_RTG_State *pState);

MMSd_CdcClassMember *MMSd_RTG_FindCdcClassMember(char *pName, char *pFcName, int nCdcClassIndex, MMSd_RTG_State *pState);

int MMSd_RTG_GetClassInfo(MMSd_RTG_State *pState, MMSd_CdcClassMember *pCdcClassMember, int *mmsSize, int *dimension, int *options);

int MMSd_RTG_GetCdcClassMemberClass(MMSd_CdcClassMember *pCdcClassMember, MMSd_RTG_State *pState);

int MMSd_RTG_ControlBlockClassId(MMSd_RTG_State *pState, char *className);

GOMSFE_Entry *MMSd_ControlBlockModel(MMSd_RTG_State *pState, char *className);

void MMSd_RTG_HideStructureMembers(MMSd_RTG_State *pState);

void MMSd_RTG_reset_scl_read_only_handlers(MMSd_RTG_State *pState, int domainIndex);

#ifdef __cplusplus
}
;
#endif

#endif /* _RTG_H */
