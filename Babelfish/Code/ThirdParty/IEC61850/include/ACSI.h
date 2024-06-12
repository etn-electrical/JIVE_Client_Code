/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1997-2015 */
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
 *  Declarations for 61850-7-2:
 *  Abstract Communication Service Interface.
 *
 *  This file should not require modification.
 */


#ifndef _CASM_H
#define _CASM_H

#ifdef __cplusplus
extern "C" {
#endif

#define MMSd_MAX_UNSIGNED_LONG 0xFFFFFFFF

#include "cbuff.h"
#include "uca_time.h"
#include "mmstypes.h"

/* default handlers */
#include "HndlData.h"


#include "dictiona.h"
#include "connect.h"
#include "tam.h"

#if defined(TMW_USE_SIMULATED_CONTROLS)
#include "../examples/common/simulate.h"
#endif

/* NOTE WELL:
 *
 * The inclusion bit strings contained in each of the control block
 * structures can be used to turn the various on or off.  If the
 * corresponding bit is clear, the feature is not visible to MMS
 * getVarAccessAttributes, read, or write.  The fields will be left
 * undisturbed (and hence maintain their original default values.
 * If you call MMSd_initXCB (where X is E, R, or L), the inclusion
 * fields will all be turned on.  You should therefore either not call
 * them or initialize afer the call.
 *
 * Note also that the use of the bitstrings here is possibly dangerous.
 * The bitstrings are defined to be of integer type (since they never
 * exceed 16 bits), which is convenient for processing.  Be careful of
 * using integer type bit switches, however, for anything that is mapped
 * to an MMS variable field, as the ASN.1 will not do integer byte swapping.
 */

#define MMSD_61850_OPTFLDS_LEN  10
#define MMSD_ENTRY_ID_LEN       8
#define MMSD_REASON_BIT_LEN     6

#define MMSD_ENC_SEQUENCE_NUMBER        0x4000
#define MMSD_ENC_REPORT_TIME_STAMP      0x2000
#define MMSD_ENC_REASON_FOR_INCLUSION   0x1000
#define MMSD_ENC_OUTPUT_DATA_SET_NAME   0x0800
#define MMSD_ENC_USE_MEMBER_NAMES       0x0400
#define MMSD_ENC_BUFFER_OVERFLOW        0x0200
#define MMSD_ENC_ENTRY_ID               0x0100
#define MMSD_ENC_CONF_REV               0x0080
#define MMSD_ENC_SEGMENTATION           0x0040

/* Switches for SvOptFlds in Sampled Values Control Blocks */
#define TMW_SAV_ENC_refresh_time        0x4000
#define TMW_SAV_ENC_sample_synchronized 0x2000
#define TMW_SAV_ENC_sample_rate         0x1000
#define TMW_SAV_ENC_data_set            0x0800
#define TMW_SAV_ENC_security            0x0400

#define MMSD_BIT_LogEnr                 0x4000
#define MMSD_BIT_LogNam                 0x2000
#define MMSD_BIT_LogWrp                 0x1000
#define MMSD_BIT_WrnLvl                 0x0800
#define MMSD_BIT_LogSiz                 0x0400
#define MMSD_BIT_LogEna                 0x0200
#define MMSD_BIT_OvrST                  0x0100
#define MMSD_BIT_WrnST                  0x0080
#define MMSD_BIT_UseST                  0x0040
#define MMSD_BIT_OldTim                 0x0020
#define MMSD_BIT_NewTim                 0x0010
#define MMSD_BIT_Count                  0x0008

#if defined(TMW_USE_JOURNALS)
typedef struct MMSd_EventReportStatus {
    CircBuffIndex   tail;
    CircBuffPass    tailPass;
    unsigned char   used;
    unsigned char   reason;
    int             length;
} MMSd_EventReportStatus;

#define RPT_JOURNAL_typeCode  0
#define RPT_JOURNAL_reason    1
#define RPT_JOURNAL_memberNum 2
#define RPT_JOURNAL_stamp     3
#define RPT_JOURNAL_handle    4
#define RPT_JOURNAL_data      5

#else
typedef unsigned char MMSd_EventReportStatus;
#endif

/* Macros for handling 61850 Quality flags */
/*
    The 61850 Quality attributes are 13 bits long, which is represented as
    two unsigned char octets.  This means that the bits are somewhat complicated
    to access individually.  These macros simplify the access.  The first set of
    flags can be used to construct an integer representation (or-ing) which can
    then be passed to TMW_storeQuality to initialize the entire quality field.

    The rest of the definitions can be used to reference the bits in-place.
    The library itself never deals with Quality - it is an application issue -
    so these macros are never used internally. An example of their use can be seen
    in the examples\scl_demo\demo.c.
*/

/* OR these flags together for use with TMW_storeQuality to initialize Quality */
#define TMW_Validity_Good           0x0000
#define TMW_Validity_Invalid        0x4000
#define TMW_Validity_Reserved       0x8000
#define TMW_Validity_Questionable   0xC000
#define TMW_Overflow                0x2000
#define TMW_OutofRange              0x1000
#define TMW_BadReference            0x0800
#define TMW_Oscillatory             0x0400
#define TMW_Failure                 0x0200
#define TMW_OldData                 0x0100
#define TMW_Inconsistent            0x0080
#define TMW_Inaccurate              0x0040
#define TMW_Source_Process          0x0000
#define TMW_Source_Substituted      0x0020
#define TMW_Test                    0x0010
#define TMW_OperatorBlocked         0x0008

/* Retrieves the quality fields as an integer */
#define TMW_getQuality( q ) \
    ( (int) ( ( q[ 0 ] << 8 ) | q[ 1 ] ) )

/* Stores the quality fields, passed as an integer */
#define TMW_storeQuality( q, v ) \
{   \
    q[ 0 ] = (unsigned char) ( ( (v) >> 8 ) & 0xFF); \
    q[ 1 ] = (unsigned char) ( (v) & 0xFF); \
}

#define TMW_testQuality_Validity_Good( q )           ( ( q[ 0 ] & (~0xC0) ) == 0x00 )
#define TMW_testQuality_Validity_Invalid( q )        ( ( q[ 0 ] & (~0xC0) ) == 0x40 )
#define TMW_testQuality_Validity_Reserved( q )       ( ( q[ 0 ] & (~0xC0) ) == 0x80 )
#define TMW_testQuality_Validity_Questionable( q )   ( ( q[ 0 ] & (~0xC0) ) == 0xC0 )
#define TMW_testQuality_Overflow( q )                ( q[ 0 ] & 0x20 )
#define TMW_testQuality_OutofRange( q )              ( q[ 0 ] & 0x10 )
#define TMW_testQuality_BadReference( q )            ( q[ 0 ] & 0x08 )
#define TMW_testQuality_Oscillatory( q )             ( q[ 0 ] & 0x04 )
#define TMW_testQuality_Failure( q )                 ( q[ 0 ] & 0x02 )
#define TMW_testQuality_OldData( q )                 ( q[ 0 ] & 0x01 )
#define TMW_testQuality_Inconsistent( q )            ( q[ 1 ] & 0x80 )
#define TMW_testQuality_Inaccurate( q )              ( q[ 1 ] & 0x40 )
#define TMW_testQuality_Source_Process( q )          ( ( q[ 1 ] & 0x00 ) == 0 )
#define TMW_testQuality_Source_Substituted( q )      ( q[ 1 ] & 0x20 )
#define TMW_testQuality_Test( q )                    ( q[ 1 ] & 0x10 )
#define TMW_testQuality_OperatorBlocked( q )         ( q[ 1 ] & 0x08 )

#define TMW_setQuality_Validity_Good( q )           q[ 0 ] = ( q[ 0 ] & (~0xC0) )
#define TMW_setQuality_Validity_Invalid( q )        q[ 0 ] = ( ( q[ 0 ] & (~0xC0) ) | 0x40 )
#define TMW_setQuality_Validity_Reserved( q )       q[ 0 ] = ( ( q[ 0 ] & (~0xC0) ) | 0x80 )
#define TMW_setQuality_Validity_Questionable( q )   q[ 0 ] = ( ( q[ 0 ] & (~0xC0) ) | 0xC0 )
#define TMW_setQuality_Overflow( q )                q[ 0 ] |= (unsigned char) 0x20
#define TMW_setQuality_OutofRange( q )              q[ 0 ] |= (unsigned char) 0x10
#define TMW_setQuality_BadReference( q )            q[ 0 ] |= (unsigned char) 0x08
#define TMW_setQuality_Oscillatory( q )             q[ 0 ] |= (unsigned char) 0x04
#define TMW_setQuality_Failure( q )                 q[ 0 ] |= (unsigned char) 0x02
#define TMW_setQuality_OldData( q )                 q[ 0 ] |= (unsigned char) 0x01
#define TMW_setQuality_Inconsistent( q )            q[ 1 ] |= (unsigned char) 0x80
#define TMW_setQuality_Inaccurate( q )              q[ 1 ] |= (unsigned char) 0x40
#define TMW_setQuality_Source_Process( q )          q[ 1 ] |= (unsigned char) 0x00
#define TMW_setQuality_Source_Substituted( q )      q[ 1 ] |= (unsigned char) 0x20
#define TMW_setQuality_Test( q )                    q[ 1 ] |= (unsigned char) 0x10
#define TMW_setQuality_OperatorBlocked( q )         q[ 1 ] |= (unsigned char) 0x08

#define TMW_clearQuality_Overflow( q )              q[ 0 ] &= (unsigned char) ~0x20
#define TMW_clearQuality_OutofRange( q )            q[ 0 ] &= (unsigned char) ~0x10
#define TMW_clearQuality_BadReference( q )          q[ 0 ] &= (unsigned char) ~0x08
#define TMW_clearQuality_Oscillatory( q )           q[ 0 ] &= (unsigned char) ~0x04
#define TMW_clearQuality_Failure( q )               q[ 0 ] &= (unsigned char) ~0x02
#define TMW_clearQuality_OldData( q )               q[ 0 ] &= (unsigned char) ~0x01
#define TMW_clearQuality_Inconsistent( q )          q[ 1 ] &= (unsigned char) ~0x80
#define TMW_clearQuality_Inaccurate( q )            q[ 1 ] &= (unsigned char) ~0x40
#define TMW_clearQuality_Source_Process( q )        q[ 1 ] &= (unsigned char) ~0x00
#define TMW_clearQuality_Source_Substituted( q )    q[ 1 ] &= (unsigned char) ~0x20
#define TMW_clearQuality_Test( q )                  q[ 1 ] &= (unsigned char) ~0x10
#define TMW_clearQuality_OperatorBlocked( q )       q[ 1 ] &= (unsigned char) ~0x08

/*
 * 61850 Tracking definitions
*/
typedef enum {
    TMW_TRK_SVC_Unknown,
    TMW_TRK_SVC_Associate,
    TMW_TRK_SVC_Abort,
    TMW_TRK_SVC_Release,
    TMW_TRK_SVC_GetServerDirectory,
    TMW_TRK_SVC_GetLogicalDeviceDirectory,
    TMW_TRK_SVC_GetAllDataVaues,
    TMW_TRK_SVC_GetDataValues,
    TMW_TRK_SVC_SetDataValues,
    TMW_TRK_SVC_GetDataDirectory,
    TMW_TRK_SVC_GetDataDefinition,
    TMW_TRK_SVC_GetDataSetValues,
    TMW_TRK_SVC_SetDataSetValues,
    TMW_TRK_SVC_CreateDataSet,
    TMW_TRK_SVC_DeleteDataSet,
    TMW_TRK_SVC_GetDataSetDirectory,
    TMW_TRK_SVC_SelectActiveSG,
    TMW_TRK_SVC_SelectEditSG,
    TMW_TRK_SVC_SetEditSGValue,
    TMW_TRK_SVC_ConfirmEditSGValues,
    TMW_TRK_SVC_GetEditSGValue,
    TMW_TRK_SVC_GetSGCBValues,
    TMW_TRK_SVC_Report,
    TMW_TRK_SVC_GetBRCBValues,
    TMW_TRK_SVC_SetBRCBValues,
    TMW_TRK_SVC_GetURCBValues,
    TMW_TRK_SVC_SetURCBValues,
    TMW_TRK_SVC_GetLCBValues,
    TMW_TRK_SVC_SetLCBValues,
    TMW_TRK_SVC_QueryLogByTime,
    TMW_TRK_SVC_QueryLogAfter,
    TMW_TRK_SVC_GetLogStatusValues,
    TMW_TRK_SVC_SendGOOSEMessage,
    TMW_TRK_SVC_GetGCBValues,
    TMW_TRK_SVC_SetGCBValues,
    TMW_TRK_SVC_GetGoReference,
    TMW_TRK_SVC_GetGOOSEElementNumber,
    TMW_TRK_SVC_SendMSVMessage,
    TMW_TRK_SVC_GetMSVCBValues,
    TMW_TRK_SVC_SetMSVCBValues,
    TMW_TRK_SVC_SendUSVMessage,
    TMW_TRK_SVC_GetUSVCBValues,
    TMW_TRK_SVC_SetUSVCBValues,
    TMW_TRK_SVC_Select,
    TMW_TRK_SVC_SelectWithValue,
    TMW_TRK_SVC_Cancel,
    TMW_TRK_SVC_Operate,
    TMW_TRK_SVC_CommandTermination,
    TMW_TRK_SVC_TimeActivatedOperate,
    TMW_TRK_SVC_GetFile,
    TMW_TRK_SVC_SetFile,
    TMW_TRK_SVC_DeleteFile,
    TMW_TRK_SVC_GetFileAttributeValues,
    TMW_TRK_SVC_TimeSynchronization,
    TMW_TRK_SVC_InternalChange
} TMW_TRK_SVC_Enum;

/* This is just to make all CBs symmetric - the service doesn't exist,
   it get specialized by SetActiveSG, etc., but the code handles
   the special cases when the value is TMW_TRK_SVC_Unknown */
#define TMW_TRK_SVC_SetSGCBValues TMW_TRK_SVC_Unknown


typedef enum {
    TMW_TRK_ERR_no_error,
    TMW_TRK_ERR_instance_not_available,
    TMW_TRK_ERR_instance_in_use,
    TMW_TRK_ERR_access_violation,
    TMW_TRK_ERR_access_not_allowed_in_current_state,
    TMW_TRK_ERR_parameter_value_inappropriate,
    TMW_TRK_ERR_parameter_value_inconsistent,
    TMW_TRK_ERR_class_not_supported,
    TMW_TRK_ERR_instance_locked_by_other_client,
    TMW_TRK_ERR_control_must_be_selected,
    TMW_TRK_ERR_type_conflict,
    TMW_TRK_ERR_failed_due_to_communications_constraint,
    TMW_TRK_ERR_failed_due_to_server_constraint,
} TMW_TRK_ERR_Enum;

typedef int TMW_TRK_ClassOption;

#define TMW_TRK_AnalogI     0x01
#define TMW_TRK_AnalogF     0x02
#define TMW_TRK_resvTms     0x04
#define TMW_TRK_operTm      0x08
#define TMW_TRK_ctlVal      0x10
#define TMW_TRK_enum        0x20
#define TMW_TRK_cb          0x40
#define TMW_TRK_tracking    0x80
#define TMW_TRK_TAG_MASK    0xff00
#define TMW_TRK_CTLV_MASK   ( TMW_TRK_ctlVal | TMW_TRK_enum | TMW_TRK_TAG_MASK )
#define TMW_TRK_CB_MASK     ( TMW_TRK_operTm | TMW_TRK_resvTms)
#define TMW_TRK_CTL_MASK    ( TMW_TRK_operTm | TMW_TRK_resvTms | TMW_TRK_CTLV_MASK )

#define TMW_TRK_SUBSET_MASK ( TMW_TRK_ctlVal | TMW_TRK_enum | TMW_TRK_operTm | TMW_TRK_resvTms | TMW_TRK_AnalogI | TMW_TRK_AnalogF )

#define TMW_TRK_SUBSET_OF( a, b )   ( ( ( a & TMW_TRK_SUBSET_MASK ) | ( b & TMW_TRK_SUBSET_MASK ) ) \
                                            == ( b & TMW_TRK_SUBSET_MASK ) )

#define TMW_TRK_HAVE_ALL_OPTS( o ) ( ( o & (TMW_TRK_ctlVal | TMW_TRK_operTm | TMW_TRK_resvTms) ) \
                                                == (TMW_TRK_ctlVal | TMW_TRK_operTm | TMW_TRK_resvTms) )

/* Used to pack/unpack the cdcOptions, below */
#define TMW_TRK_OPT_ASN1_TAG( a ) (( a >> 8 ) & 0xFF)
#define TMW_TRK_OPT_OPTS( a ) (( a ) & 0xFF)
#define TMW_TRK_OPT_CLASS( a ) ( a >> 16 )
#define TMW_TRK_OPT_TRACKING( a ) ( a & TMW_TRK_tracking )
#define TMW_TRK_OPT_CB( a ) ( a & TMW_TRK_cb )
#define TMW_TRK_OPT_CTL( a ) ( a & TMW_TRK_ctlVal )
#define TMW_TRK_MAKE_OPTS( cla, ctlvaltag, flags ) ( ( cla << 16 ) | ( ctlvaltag << 8 ) | ( flags & 0xFF ) )
#define TMW_TRK_GET_OPTS( opt, cla, ctlvaltag, flags ) { cla = (opt >> 16); ctlvaltag = (opt >> 8 ) & 0xFF; flags = opt & 0xff; }
#define TMW_TRK_CB_INSTANCE( cl_opt, inst_opt ) \
                ( ( cl_opt & ~TMW_TRK_CB_MASK ) | ( ( cl_opt & TMW_TRK_CB_MASK ) & ( inst_opt & TMW_TRK_CB_MASK ) ) )
#define TMW_TRK_CTL_INSTANCE( cl_opt, inst_opt ) \
                ( ( cl_opt & ~TMW_TRK_CTL_MASK ) | ( ( cl_opt & TMW_TRK_CTL_MASK ) & ( inst_opt & TMW_TRK_CTL_MASK ) ) )


typedef const struct TMW_CdcTrackingOrder *TMW_CdcTrackingOrderPtr;

typedef struct TMW_CdcTrackingRange {
    TMW_CdcTrackingOrderPtr         pTrackingOrder;
    int                             nNumTrackingEntries;
} TMW_CdcTrackingRange;

typedef struct TMW_CdcTrackingOrder {
    const char                      *pAttributeName;
    int                             nTrackingIndex;
    TMW_CdcTrackingRange            range;
} TMW_CdcTrackingOrder;

typedef struct TMW_CdcTrackingDirectory {
    TMW_CdcTrackingRange            range;
    int                             nNumTrackingMembers;
    TMW_TRK_SVC_Enum                nReadService;
    TMW_TRK_SVC_Enum                nWriteService;
} TMW_CdcTrackingDirectory;


/* TMW_TrackingVariant works like a signature on a DOType - it captures the options like:

        ctlVal presence, type, length and possibly Enum type id
        operTm presence
        resvTms presense
        tracking class

   It is generated by SCL_RTG.C and passed to RTG to initialize the dictionary information used to
   match up tracking nodes with trackable class instances.
*/

typedef struct TMW_TrackingVariant {
    TMW_TRK_ClassOption nCdcOptions;                /* See definitions above */
    int                 nTrackingIndex;             /* Set to the tracking index of each attribute */
    int                 nTrackingLengthOrEnumId;    /* Meaning depends in TMW_TRK_enum bit */
} TMW_TrackingVariant;

typedef int (* MMSd_EvaluationFunction) ( MMSd_NvlEval *nvl, void *context );

typedef struct MMSd_ReportStatus *MMSd_ReportStatusPtr;

typedef struct MMSd_ReportStatus {
    MMSd_context                *context;
    MMSd_time                   lastReported;
    MMSd_time                   recentChange;
    MMSd_time                   lastIntegrity;
    MMSd_time                   currentTime;
    MMSd_time                   scheduledReportTime;
    MMSd_NvlEval                *nvlEval;
    int                         numIncluded;
    MMSd_EvaluationFunction     evalFunction;
    unsigned char               periodic_report;
    unsigned char               integrity_report;
#if defined(TMW_USE_JOURNALS)
    MMSd_JournalReader          reader;
    int                         nextMember;
    int                         newNextMember;
    CircBuffIndex               sentTail;
    CircBuffPass                sentTailPass;
    int                         sentValid;
#endif
    int                         nextSubSeqNum;
} MMSd_ReportStatus;

typedef enum { TMW_CB_NONE,
               TMW_CB_BRCB,
               TMW_CB_URCB,
               TMW_CB_LCB,
               TMW_CB_GOOSE,
               TMW_CB_GSE,
               TMW_CB_MSVCB,
               TMW_CB_USVCB,
               TMW_CB_SGCB,
               TMW_CB_TASE2_DSTS,
               TMW_CB_TASE2_TSTS,
               TMW_CB_TASE2_TATS
             } TMW_CB_TYPE;

typedef struct TMW_CB_CommonData *TMW_CB_CommonDataPtr;

typedef struct TMW_CB_CommonData {
    char                    *pName;
    TMW_CB_TYPE             nCbType;
    unsigned long           ConfRev;
    MMSd_short              ResvTms;
    char                    Resv;
    TMW_Stack_Address       Owner;
    TMW_ms_time             resvTimeStamp;
    MMSd_Connection         *rpt_connection;
    TMW_TrackingVariant     trackingVariant;
    char                    ignoreOwner;

    /* If this is non-zero, make pseudo-aa-specific copy in connection */
    /* The handlers will all look into the connection for the copy */
    void                    *aaInstanceOf;
    TMW_CB_CommonDataPtr    pNext;
} TMW_CB_CommonData;

typedef enum {
    TMW_BRCB_STATE_DISABLED,
    TMW_BRCB_STATE_RESYNC,
    TMW_BRCB_STATE_ENABLED
} TMW_BRCB_STATE;


typedef struct TMW_XRCB_Data {
    char                RptID[ MMSD_OBJECT_NAME_LEN ];
    char                RptEna;
    MMSd_ObjectName     DatSet;
    unsigned char       OptFlds[ 2 ];
    unsigned long       BufTm;
    unsigned int        SqNum;
    unsigned short      SubSqNum;       /* Reported, but not in RCB */
    unsigned char       TrgOps;
    unsigned long       IntgPd;
    char                GI;
    char                PurgeBuf;       /* Always FALSE on reading */
    unsigned char       EntryID[ MMSD_ENTRY_ID_LEN ];
    MMSd_time           TimeofEntry;
    unsigned char       testOptFlds[ 2 ];    // Overload OptFlds for conformance / negative testing only
    boolean             useTestOptFlds;
    TMW_BRCB_STATE      state;
    char                BufOvfl;
    CircBuffIndex       startLastGIentry;
    CircBuffPass        startLastGIpass;
    CircBuffIndex       endLastGIentry;
    CircBuffPass        endLastGIpass;
} TMW_XRCB_Data;

typedef struct TMW_LCB_Data {
    char                LogEna;
    MMSd_ObjectName     LogRef;
    MMSd_ObjectName     DatSet;
    unsigned char       OptFlds[ 2 ];       /* Not visible in LCB, but preset */
    MMSd_time           OldEntrTm;
    MMSd_time           NewEntrTm;
    unsigned char       OldEnt[ MMSD_ENTRY_ID_LEN ];
    unsigned char       NewEnt[ MMSD_ENTRY_ID_LEN ];
    unsigned char       TrgOps;
    unsigned long       IntgPd;
    void                *journal;
} TMW_LCB_Data;

typedef enum {
	TMW_PROT_Normal,
	TMW_PROT_UDP
} TMW_GooseSavProtocol;

typedef struct TMW_DstAddress {
    TMW_Mac_Address     GoAddr;
    unsigned char       GoPriority;
    unsigned short      GoVid;
    unsigned short      GoAppid;
} TMW_DstAddress;

typedef struct TMW_GCB_Data {
    char                GoEna;
    char                GoID[ MMSD_OBJECT_NAME_LEN ];
    MMSd_ObjectName     DatSet;
    char                NdsCom;
    TMW_DstAddress      DstAddress;
    unsigned long       MinTime;
    unsigned long       MaxTime;
    char                FixedOffs;    /* Ed2 8-1 Table 73 */
    unsigned char       test;
    unsigned char       simulate;
    TMW_GooseSavProtocol protocol;    /* 90-5 extension */
    int                 udp_port;     /* 90-5 extension */
    void                *pContext;    /* IEC_Goose_Context */
    void                *pStrategy;   /* IEC_Goose_Strategy */
    TMW_61850_VLAN_MakeSignatureFcn     genVlanSignature;
    void                                *pVlanSignatureData;
    void                                *pCryptoContext;
    unsigned char                       *pPrivateSecurityData;
    int                                 nPrivateSecurityDataLen;
} TMW_GCB_Data;

typedef struct TMW_GSECB_Data {
    char                GsEna;
    char                GsID[ MMSD_OBJECT_NAME_LEN ];
    char                **DNALabels;
    int                 nDNALabels;
    char                **UserSTLabels;
    int                 nUserSTLabels;
    /* NOTE: We don't need LSentData, since the GSE is not TRACKED, and it is retrieved on
             reading from the GSE internal data structures */
    void                *pContext;    /* UCA_Goose_Context */
} TMW_GSECB_Data;

typedef struct TMW_SGCB_Data MMSd_SGCB;

typedef int     (* TMW_SetActSG_Indication)(   MMSd_SGCB *pSGCB );

typedef int     (* TMW_SetEditSG_Indication)(  MMSd_SGCB *pSGCB );

typedef int     (* TMW_CnfEditSG_Indication)(  MMSd_SGCB *pSGCB );

typedef void  * (* TMW_SGCB_UserHandle_Indication)(  MMSd_SGCB *pSGCB );

typedef struct TMW_SGCB_Data {
    unsigned char               NumOfSG;
    unsigned char               ActSG;
    unsigned char               EditSG;
    char                        CnfEdit;
    MMSd_UtcBTime               LActTm;
    TMW_SetActSG_Indication     pSetActSgFcn;
    TMW_SetEditSG_Indication    pSetEditSgFcn;
    TMW_CnfEditSG_Indication    pCnfEditSgFcn;
    void                        *pUserHandle;
    char                        *pDomainName;
    MMSd_short                  ResvTms;
} TMW_SGCB_Data;

typedef struct TMW_XSVCB_Data {
    char                SvEna;
    char                XsvID[ MMSD_OBJECT_NAME_LEN ];
    MMSd_ObjectName     DatSet;
    unsigned int        SmpRate;
    unsigned char       OptFlds[ 2 ];
    unsigned char       SmpMod;
    unsigned char       SmpSynch;       /* used?? */
    TMW_DstAddress      DstAddress;
    int                 noASDU;
    unsigned char       test;
    unsigned char       simulate;
    TMW_GooseSavProtocol protocol;    /* 90-5 extension */
    int                 udp_port;     /* 90-5 extension */
    void                *pContext;    /* TMW_SAV_Context */
    TMW_61850_VLAN_MakeSignatureFcn     genVlanSignature;
    void                                *pVlanSignatureData;
    void                                *pCryptoContext;
    unsigned char                       *pPrivateSecurityData;
    int                                 nPrivateSecurityDataLen;
} TMW_XSVCB_Data;

typedef struct TMW_TASE2_DSTS {
    MMSd_ObjectName     DataSetName;
    unsigned long       StartTime;
    unsigned long       Interval;
    unsigned int        TLE;
    unsigned long       BufferTime;
    unsigned long       IntegrityCheck;
    unsigned char       DSConditionsRequested;
    char                BlockData;      /* Not supported */
    char                Critical;
    char                RBE;
    char                AllChangesReported;
    char                Status;
    int                 EventCodeRequested;
    char                Started;
} TMW_TASE2_DSTS;

typedef struct TMW_TASE2_TSTS {
    MMSd_ObjectName     DataValueName;
    unsigned char       TSConditionsRequested;
    char                BlockData;
    long                BeginTime;
    long                EndTime;
    long                SamplingInterval;
    long                ReportingInterval;
    char                Status;
} TMW_TASE2_TSTS;

typedef struct TMW_TASE2_TATS {
    unsigned char       TAConditionsRequested;
    char                BlockData;
    char                Status;
} TMW_TASE2_TATS;

typedef struct MMSd_ReportControl *MMSd_ReportControlPtr;

typedef struct MMSd_ReportControl {
    TMW_CB_CommonData       common;

    union {
        TMW_XRCB_Data       XRCB;   /* URCB and BRCB */
        TMW_LCB_Data        LCB;
        TMW_GCB_Data        GCB;
        TMW_GSECB_Data      GSECB;
        TMW_SGCB_Data       SGCB;
        TMW_XSVCB_Data      XSVCB;
        TMW_TASE2_DSTS      TASE2_DSTS;
        TMW_TASE2_TSTS      TASE2_TSTS;
        TMW_TASE2_TATS      TASE2_TATS;
    } data;
    MMSd_ReportStatus       status;
    void                    *aaInstanceOf;
    unsigned long           nMsUntilNextEvent;
    int                     nMsUntilNextEventValid;
    MMSd_ReportControlPtr   next;
} MMSd_ReportControl;


#define NUM_DNA_LABELS      32
#define NUM_USERST_LABELS   128

typedef MMSd_ReportControl MMSd_report_info;

typedef struct {
    char                  *name;
    MMSd_ReportControl    *reportControl;   /* Was info */
    MMSd_NvlEval          *initial;
} MMSd_reportList;

/* Call this routine at startup to initialize callbacks for all SGCBs in the model */
TMW_CLIB_API void TMW_EstablishSettingsGroups(MMSd_context                      *pMmsContext,
                                  TMW_SetActSG_Indication           pSetActSgFcn,
                                  TMW_SetEditSG_Indication          pSetEditSgFcn,
                                  TMW_CnfEditSG_Indication          pSetCnfEditSgFcn,
                                  TMW_SGCB_UserHandle_Indication    pSgcbUserHandleFcn );

/* Get Setting Group control for a given LDevice in the MMS context */
MMSd_SGCB *MMSd_getContextSGCB( MMSd_context *pMmsContext,
                                char         *pDomainName );

/* Get Setting Group control from within a handler for its LDevice */
MMSd_SGCB *MMSd_getHandlerSGCB( void *E );

/* Call from within a handler to verify that the connection owns the reservation */
TMW_CLIB_API int MMSd_canWriteSettingGroupValue( void *E );

#define MMSD_DATA_CHANGE    0x40
#define MMSD_QUALITY_CHANGE 0x20
#define MMSD_DATA_FREEZE    0x10
#define MMSD_INTEGRITY      0x08
#define MMSD_GENINTER       0x04
#define MMSD_PERIODIC       0x08
#define MMSD_BUFFERED       0x02


typedef unsigned long  MMSd_FuncCompBits;
typedef unsigned char  MMSd_FuncCompType;
typedef struct MMSd_CdcClassMember {
    const char               *cdcmName;
    long                     cdcmDimension;
    int                      cdcmBase;
    int                      cdcmTypeSize;
    short                    cdcmNumElements;
    MMSd_FuncCompType        cdcmFc;
    unsigned char            cdcmFlags;
} MMSd_CdcClassMember;


#define CDC_OPT_NoOffsets 0x01


typedef struct MMSd_CdcClass {
    const char               *cdcName;
    int                      cdcBase;
    int                      cdcNumElements;
    long                     cdcTypeSize;       /* Used only by rtg_cfg.c */
    MMSd_FuncCompBits        cdcFcBits;         /* Used only by rtg_cfg.c */
    int                      cdcTrackingIndex;  /* Used only by scl_rtg.c */
    int                      cdcNumFcVariants;
    TMW_TRK_ClassOption      cdcOptions;
} MMSd_CdcClass;

void MMSd_initObjectName( MMSd_ObjectName *obj );

int MMSd_locateEntry( char *name, MMSd_entry *entryList, int listLen );

int MMSd_locateIndexedEntry( char *name, MMSd_entry *entryList, dictionaryIndex *indices, int listLen );

MMSd_entry *MMSd_locateNVL( MMSd_context *context, MMSd_ObjectName *nvl_obname, int *dindex, int *lindex );

MMSd_entry *MMSd_locateSclNV( MMSd_context *context, MMSd_ObjectName *pObjName, int *pDomIndex, int *pVarIndex );

MMSd_entry *MMSd_locateNvMember( MMSd_context   *context, MMSd_entry     *pNvEntry, char           *pMemberName );

TMW_CLIB_API MMSd_entry *MMSd_locateNV( MMSd_context *context, MMSd_ObjectName *nvl_obname, int *dindex, int *lindex );

int MMSd_readDictionaryObject(MMSd_context *context, MMSd_entry *entry, MMSd_descriptor *A, /* alt */ MMSd_descriptor *E);

void MMSd_writeDictionaryObject(MMSd_context *context, MMSd_entry *entry, boolean isNVL, MMSd_descriptor *N,
                                MMSd_descriptor *D, MMSd_descriptor *E);

int MMSd_makeAttribute_read_only(MMSd_context *pMmsContext, char *pDomainName, char *pVarName);

MMSd_tag TMW_GetMmsAsn1Tag( MMSd_AttributeHandler pAttrHand, int size );
MMSd_tag TMW_GetPrimitiveMmsAsn1TagAndLength( int nPrimitive, MMSd_length *pLength, MMSd_context *pMmsContext );

int MMSd_ObjectNameToString( MMSd_ObjectName *pObj, char *outStr );
int MMSd_parseObjectName( char *buffer, MMSd_ObjectName *obj );


TMW_CLIB_API MMSd_context *MMSd_get_handler_mms_context(void *E);

MMSd_Connection *MMSd_check_connection_context(void *E,
                                               MMSd_Connection *rpt_conn);
void MMSd_get_handler_parent_name(void *E, MMSd_entry *pMmsDictEntry,
                                  char **pDomainName, char *pName,
                                  int *pNameLen);

void MMSd_set_NVL_Reporting(MMSd_NvlEval *nvlEval, unsigned char reason,
                            unsigned char inclusion);

TMW_CLIB_API int MMSd_setReportInfo( MMSd_context *mms_context,
								     MMSd_ReportControl *reportControl,
									 MMSd_ObjectName *dataSetObject);

int MMSd_SetConfiguredOwnerRcb(MMSd_ReportControl *pReportControl,
                               TMW_Stack_Address *pAddress);

TMW_CLIB_API MMSd_length MMSd_emitNvlConfirmedError(MMSd_ushort errorClass, MMSd_ushort errorCode, MMSd_ushort deleting,
                                       unsigned long numDeleted, unsigned long invokeID, MMSd_descriptor *E);

int 	MMSd_canDeleteNvl(MMSd_context *cntxt, MMSd_ushort domainIndex, MMSd_ushort nvlIndex);
int 	MMSd_inUseNvl(MMSd_context *cntxt, MMSd_short domainIndex, MMSd_ushort nvlIndex);
int 	MMSd_nvlSort(dictionaryIndex *indices, MMSd_entry *entries, dictionaryIndex numEntries);

unsigned long MMSd_NvlInitEval(MMSd_context *cntxt, directoryType *directory, MMSd_ExtendedHandlerSet *mms_handlers,
                               int numPrimitives, MMSd_ushort directorySize, MMSd_entry *nvlEntry, MMSd_NvlEval *nvlEval);

MMSd_length MMSd_readNvlMember(MMSd_context * mmsContext, MMSd_list_entry *member, MMSd_descriptor *pAltAccess,
                               MMSd_descriptor *E);

void MMSd_ResetNvlReport(MMSd_NvlEval *list);
int MMSd_ResetRcbReport( MMSd_ReportStatus *rptStatus );


TMW_CLIB_API int MMSd_acse_abort_req(void *pvMmsConnection, int abortSource, int abortDiag);

/**
 * Support for IEC 61850 Controls
 *********************************************************************
 */
typedef int MMSd_AnalogValueValid;
#define MMSd_ANALOG_FOUND_i  0x0001
#define MMSd_ANALOG_FOUND_f  0x0002

typedef enum {
    TMW_ModBeh_on         = 1,
    TMW_ModBeh_on_block   = 2,
    TMW_ModBeh_test       = 3,
    TMW_ModBeh_test_block = 4,
    TMW_ModBeh_off        = 5
} TMW_ModBeh_t;

typedef enum {
    MMSd_ANALOG_ORDER_ERROR,
    MMSd_ANALOG_ORDER_i_FIRST,
    MMSd_ANALOG_ORDER_f_FIRST
} MMSd_AnalogValueOrder;

typedef struct MMSd_AnalogControlValue {
    long                    i;
#if defined(MMSD_FLOAT_DATA_TYPE)
    MMSD_FLOAT_DATA_TYPE    f;
#endif
    MMSd_AnalogValueValid   validity;
    MMSd_AnalogValueOrder   order;
} MMSd_AnalogControlValue;

typedef enum {
    MMSd_ValType_none          = 0,
    MMSd_ValType_Boolean       = boolean3,
    MMSd_ValType_BitString     = bitString4,
    MMSd_ValType_Integer       = integer5,
    MMSd_ValType_Unsigned      = unsigned6,
    MMSd_ValType_AnalogueValue = structure2
} MMSd_ControlValueType;

typedef struct MMSd_ControlServiceValue {
    MMSd_ControlValueType        nTag;                  /* MMS ASN.1 tag, used as data type */
    int                          nSize;                 /* MMS size from dictionary entry */
    union  {
        unsigned char              boolVal;
        unsigned char              bitVal;
        unsigned long              unsignedVal;
        long                       integerVal;
        MMSd_AnalogControlValue    analogValue;
    } val;
} MMSd_ControlServiceValue;





typedef struct MMSd_ControlConfigEntries {
    MMSd_entry                   *pSboTimeout;
    MMSd_entry                   *pOperTimeout;
    MMSd_entry                   *pSboClass;
    MMSd_entry                   *pCtlModel;
    directoryType                *pMmsDirectory;
    int                          nMmsDirectorySize;
    int                          nDomain;
    void                         *pMmsUserData;
} MMSd_ControlConfigEntries;

typedef struct MMSd_ControlStatusEntries {
    MMSd_entry                   *pStValEntry;
    unsigned char                nStValEntryTag;
    MMSd_entry                   *pMxValEntry;
    unsigned char                nMxValEntryTag;
    unsigned char                bValueWithTrans;
    MMSd_entry                   *pOrCatEntry;
    MMSd_entry                   *pOrIdentEntry;
    int                          nOrIdentSize;
    MMSd_entry                   *pTEntry;
    MMSd_entry                   *pCtlNumEntry;
    MMSd_entry                   *pStSeldEntry;
    directoryType                *pMmsDirectory;
    int                          nMmsDirectorySize;
    int                          nDomain;
    void                         *pMmsUserData;
} MMSd_ControlStatusEntries;

typedef struct MMSd_ControllingEntries {
    MMSd_entry                   *pLdMod;
    MMSd_entry                   *pLdLoc;
    MMSd_entry                   *pLnMod;
    MMSd_entry                   *pLnLoc;
    MMSd_entry                   *pCiloOpen;
    MMSd_entry                   *pCiloClose;
    directoryType                *pMmsDirectory;
    int                          nMmsDirectorySize;
    int                          nDomain;
    void                         *pMmsUserData;
} MMSd_ControllingEntries;

typedef struct MMSd_SboControlSelectValues {
    /* Status information for SBOw checking */
    MMSd_context                 *pMmsContext;
    void                         *pMmsConnection;
    TMW_ms_time                  sboTimeStamp;
    boolean                      selected;
    void                         *threadHandle;
    void                         *pUserData;
} MMSd_SboControlSelectValues;

typedef enum {
    MMSd_ControlParsFound_ctlVal  = 0x0001,
    MMSd_ControlParsFound_operTm  = 0x0002,
    MMSd_ControlParsFound_orCat   = 0x0004,
    MMSd_ControlParsFound_orIdent = 0x0008,
    MMSd_ControlParsFound_ctlNum  = 0x0010,
    MMSd_ControlParsFound_T       = 0x0020,
    MMSd_ControlParsFound_Test    = 0x0040,
    MMSd_ControlParsFound_Check   = 0x0080
} MMSd_ControlParsFound_t;

#define CNTRLS_SBOW_OPER_CHECKS  \
     MMSd_ControlParsFound_ctlVal  |  \
     MMSd_ControlParsFound_operTm  |  \
     MMSd_ControlParsFound_orCat   |  \
     MMSd_ControlParsFound_orIdent |  \
     MMSd_ControlParsFound_ctlNum  |  \
     MMSd_ControlParsFound_Test    |  \
     MMSd_ControlParsFound_Check

#define CNTRLS_CANCEL_CHECKS  \
     MMSd_ControlParsFound_ctlVal  |  \
     MMSd_ControlParsFound_operTm  |  \
     MMSd_ControlParsFound_orCat   |  \
     MMSd_ControlParsFound_orIdent |  \
     MMSd_ControlParsFound_ctlNum  |  \
     MMSd_ControlParsFound_Test

typedef struct MMSd_ControlServiceParEntries {
    MMSd_entry                   *pCtlValEntry;
    MMSd_entry                   *pOperTmEntry;
    MMSd_entry                   *pOrCatEntry;
    MMSd_entry                   *pOrIdentEntry;
    MMSd_entry                   *pCtlNumEntry;
    MMSd_entry                   *pTEntry;
    MMSd_entry                   *pTestEntry;
    MMSd_entry                   *pCheckEntry;
    directoryType                *pMmsDirectory;
    int                          nMmsDirectorySize;
    int                          nDomain;
    void                         *pMmsUserData;
} MMSd_ControlServiceParEntries;

typedef void (* TMW_OperateFcn)( int                         nDomain,
                                 MMSd_entry                  *pVarEntry,
                                 MMSd_context                *pMmsContext,
                                 MMSd_ControlServiceValue    *pCtlVal,
                                 MMSd_Connection             *pMmsConnection,
                                 void                        *pMmsUserData,
                                 int                         *nAddCauseReturned,
                                 int                         *nErrorCodeReturned );

typedef struct MMSd_ControlServiceParams {
    unsigned char                nCheck;
    MMSd_ControlServiceValue     ctlVal;
    MMSd_UtcBTime                nOperTm;
    unsigned char                nOrCat;
    unsigned char                nOrIdent[65]; /* Should be MMSd_OrIdent */
    int                          nOrIdentSize;
    unsigned char                nCtlNum;
    MMSd_UtcBTime                nT;
    unsigned char                nTest;
    unsigned char                nCtlVal_Tag;
    MMSd_ControlParsFound_t       nParametersFound;
} MMSd_ControlServiceParams;

typedef struct MMSd_SboControlCommon {
    MMSd_SboControlSelectValues  selectValues;
    MMSd_ControlStatusEntries    statusEntries;
    MMSd_ControlConfigEntries    configEntries;
} MMSd_SboControlCommon;

typedef struct MMSd_SboControlNormal {
    MMSd_SboControlCommon        sboCommon;
} MMSd_SboControlNormal;

typedef struct MMSd_SboControlCheckParams {
    unsigned char                nCheck;
    MMSd_SboControlCommon        sboCommon;
    MMSd_ControllingEntries      controllingEntries;
    MMSd_ControlServiceParams    serviceParams;
} MMSd_SboControlCheckParams;


typedef struct MMSd_OperControlCheckParams {
    unsigned char                nCheck;
    MMSd_ControlServiceParams    serviceParams;
    MMSd_SboControlCheckParams   *pSboEnhancedParams;
    MMSd_SboControlNormal        *pSboNormalParams;
    MMSd_entry                   *pOperDict;
    int                          nDomain;
    int                          nAddCause;
    int                          nErrorCode;
    MMSd_context                 *pMmsContext;
    MMSd_Connection              *pMmsConnection;
    directoryType                *pMmsDirectory;
    int                          nMmsDirectorySize;
    void                         *pMmsUserData;
    int                          nThreadState;
    void                         *threadHandle;
    void                         *pUserData;
    MMSd_ControlStatusEntries    statusEntries;
    MMSd_ControlConfigEntries    configEntries;
    MMSd_ControllingEntries      controllingEntries;
#if defined(TMW_USE_SIMULATED_CONTROLS)
    MMSd_ControlFaultSimulation_t  nFault;
#endif
    MMSd_time                    operateStateTimeStamp;
    unsigned long                nStatusChangeDelay;        /* msec from start operation */
    unsigned long                nOperateCompleteDelay;     /* msec from status change */
    TMW_OperateFcn               operateFcn;                /* If !NULL, callback to perform function */
} MMSd_OperControlCheckParams;


typedef struct MMSd_CancelControlTestParams {
    MMSd_Boolean                 nTest;
    MMSd_ControlServiceParams    serviceParams;
    MMSd_OperControlCheckParams  *pOperParams;
    MMSd_SboControlCheckParams   *pSboEnhancedParams;
    MMSd_SboControlNormal        *pSboNormalParams;
    MMSd_ControlStatusEntries    statusEntries;
    MMSd_ControlConfigEntries    configEntries;
    void                         *pUserData;
} MMSd_CancelControlTestParams;




#if defined(USE_MMSD_TP_CONN)

typedef int LastApplErrorCode;
#define LastApplErr_NoError             0
#define LastApplErr_Unknown             1
#define LastApplErr_TimeoutTestFailed   2
#define LastApplErr_OperatorTestFailed  3

TMW_CLIB_API int MMSd_LastApplError(void *pvMmsConnection,
                        char *pCntrlName,
                        char *pDomainName,
                        LastApplErrorCode errorCode,
                        int orCatEnum,
                        unsigned char *orIdent,
                        int orIdentSize,
                        unsigned char cltNum,
                        int addCause );



MMSd_length MMSd_emitCommonOper( MMSd_UtcBTime *T,     /* MMSd_UtcBTime */
                                 unsigned char Test,   /* Boolean */
                                 unsigned char *Check, /* BitString[2] */
                                 MMSd_descriptor *E );

/* Works with generic control value */
TMW_CLIB_API int MMSd_CommandTerm(void                      *pvMmsConnection,
								  char                      *pObjectName,
								  char                      *pDomainName,
								  MMSd_ControlServiceValue  *pCtlVal,
								  MMSd_UtcBTime             *T,
								  unsigned char             Test,
								  unsigned char             *Check,
								  MMSd_UtcBTime             *operTim,   /* MMSd_UtcBTime */
								  int                       orCatEnum,  /* Integer8 */
								  unsigned char             *orIdent,   /* OctetString[-64] */
								  int                       orIdentSize,
								  unsigned char             ctlNum,     /* Unsigned8 */
								  LastApplErrorCode         errorCode,
								  int                       addCause );

TMW_CLIB_API int MMSd_CommandTermInt(void                 *pvMmsContext,
									 char                 *pObjectName,
									 char                 *pDomainName,
									 long                 ctlVal,
									 MMSd_UtcBTime        *T,         /* MMSd_UtcBtime */
									 unsigned char        Test,       /* Boolean */
									 unsigned char        *Check,     /* BitString[2] */
									 MMSd_UtcBTime        *operTim,   /* MMSd_UtcBTime */
									 int                  orCatEnum,  /* Integer8 */
									 unsigned char        *orIdent,   /* OctetString[-64] */
									 int                  orIdentSize,
									 unsigned char        cltNum,     /* unsigned8 */
									 LastApplErrorCode    errorCode,
									 int                  addCause );

TMW_CLIB_API int MMSd_CommandTermUInt(void                 *pvMmsContext,
									  char                 *pObjectName,
									  char                 *pDomainName,
									  unsigned long        ctlVal,
									  MMSd_UtcBTime        *T,         /* MMSd_UtcBTime */
									  unsigned char        Test,       /* Boolean */
									  unsigned char        *Check,     /* BitString[2] */
									  MMSd_UtcBTime        *operTim,   /* MMSd_UtcBTime */
									  int                  orCatEnum,  /* Integer8 */
									  unsigned char        *orIdent,   /* OctetString[-64] */
									  int                  orIdentSize,
									  unsigned char        cltNum,     /* Unsigned8 */
									  LastApplErrorCode    errorCode,
									  int                  addCause );

TMW_CLIB_API int MMSd_CommandTermBool(void                 *pvMmsContext,
									  char                 *varName,
									  char                 *domainName,
									  unsigned char        ctlVal,      /* Boolean */
									  MMSd_UtcBTime        *T,          /* MMSd_UtcBTime */
									  unsigned char        Test,        /* Boolean */
									  unsigned char        *Check,      /* BitString[2] */
									  MMSd_UtcBTime        *operTim,    /* MMSd_UtcBTime */
									  int                  orCatEnum,   /* Integer8 */
									  unsigned char        *orIdent,    /* OctetString[-64] */
									  int                  orIdentSize,
									  unsigned char        cltNum,      /* Unsigned8 */
									  LastApplErrorCode    errorCode,
									  int                  addCause );

#if defined(MMSD_FLOAT_DATA_TYPE)
TMW_CLIB_API int MMSd_CommandTermAnaF(void                 *pvMmsContext,
									  char                 *pObjectName,
									  char                 *pDomainName,
									  MMSD_FLOAT_DATA_TYPE ctlVal,      /* Integer8 */
									  MMSd_UtcBTime        *T,          /* MMSd_UtcBTime */
									  unsigned char        Test,        /* Boolean */
									  unsigned char        *Check,      /* BitString[2] */
									  MMSd_UtcBTime        *operTim,    /* MMSd_UtcBTime */
									  int                  orCatEnum,   /* Integer8 */
									  unsigned char        *orIdent,   /* OctetString[-64] */
									  int                  orIdentSize,
									  LastApplErrorCode    errorCode,
									  int                  addCause );
#endif

TMW_CLIB_API int MMSd_CommandTermAnaI(void                 *pvMmsContext,
									  char                 *pObjectName,
									  char                 *pDomainName,
									  long                 ctlVal,      /* Integer8 */
									  MMSd_UtcBTime        *T,          /* MMSd_UtcBTime */
									  unsigned char        Test,        /* Boolean */
									  unsigned char        *Check,      /* BitString[2] */
									  MMSd_UtcBTime        *operTim,    /* MMSd_UtcBTime */
									  int                  orCatEnum,   /* Integer8 */
									  unsigned char        *orIdent,  /* OctetString[-64] */
									  int                  orIdentSize,
									  LastApplErrorCode    errorCode,
									  int                  addCause );

TMW_CLIB_API int MMSd_CommandTermBitstring(void                *pvMmsConnection,
										   char                 *varName,
										   char                 *domainName,
										   unsigned char        *ctlVal,    /* Bitstring */
										   int                  nBits,      /* Bitstring length */
										   MMSd_UtcBTime        *T,         /* MMSd_time */
										   unsigned char        Test,       /* Boolean */
										   unsigned char        *Check,     /* BitString[2] */
										   MMSd_UtcBTime        *operTim,   /* MMSd_UtcBTime */
										   int                  orCatEnum,  /* Integer8 */
										   unsigned char        *orIdent,   /* OctetString[-64] */
										   int                  orIdentSize,
										   unsigned char        ctlNum,     /* unsigned8 */
										   LastApplErrorCode    errorCode,
										   int                  addCause );

#endif /* defined(USE_MMSD_TP_CONN) */

int MMSd_NotifyAttributeChangeEvent( void            *pReadParam,
                                     unsigned char   reason,
                                     unsigned char   numChanges,
                                     unsigned char   *outBuff,
                                     MMSd_length     outBuffLen,
                                     MMSd_context    *pMmsContext );

int MMSd_NotifyObjectChangeEvent( MMSd_ushort           domainIndex,
                                  MMSd_dictionaryIndex  dictionaryIndex,
                                  unsigned char         reason,
                                  unsigned char         numChanges,
                                  unsigned char         *outBuff,
                                  MMSd_length           outBuffLen,
                                  MMSd_context          *pMmsContext );

TMW_CLIB_API int TMW_TriggerPoint(MMSd_context      *cntxt,
								  void              *readParam,
								  unsigned char     *outBuff,
								  MMSd_length       outBuffLen );

void MMSd_SortStaticNvlEvals( MMSd_context *cntxt );

void MMSd_createReportingAA( MMSd_context *pContext, MMSd_Connection *conn );

#if defined(MMSD_MAX_AA_NVLS)
void MMSd_Enroll_AA_NvlConnection( MMSd_context       *cntxt,
                                   MMSd_Connection    *pConnection );
#endif

/*
 * Support for Object-level dictionary Entries
 ********************************************************************
 */

typedef unsigned char  MMSd_DataObjectClass;

void MMSd_writeDataObjectClassMembers(void *N, void *D, int size, void *parameter, void *E);

int MMSd_readDataObjectClassMembers(void *N, int size, void *parameter, void *E);

#if defined(MMSD_ATTR_HANDLER_NAME)
int MMSd_attrDataObjectClassMembers( void *N, int len,
                                     void *parameter, void *E );
#else
int MMSd_attrDataObjectClassMembers(int len, void *parameter, void *E);
#endif

int MMSd_nameDataObjectClassMembers(emitState *PS, void *N, int size, void * parameter, void * E);

void *MMSd_indexDataObjectClassMembers(int index, void *parameter, int size);

#if defined(TMW_USE_JOURNALS)
int MMSd_StoreChangeEvent( void *handle,
                           MMSd_Journal *journal,
                           int memberNum,
                           unsigned char reason,
                           MMSd_time *stamp,
                           unsigned char *pData,
                           int length );

TMW_CLIB_API int MMSd_JournalGetChangeTimeStamp(MMSd_Journal *journal,
                                    CircBuffIndex index,
                                    CircBuffPass pass,
                                    MMSd_time *stamp );

TMW_CLIB_API void MMSd_EncodeBadEntryId(unsigned char *pOctets);

TMW_CLIB_API void MMSd_EncodeEntryId(CircBuffPass pass, CircBuffIndex index,
                         unsigned char *pOctets );

int MMSd_DecodeEntryId( unsigned char *pOctets,
                        CircBuffPass *pass, CircBuffIndex *index );


#endif


void MMSd_initializeReportingAA( MMSd_context *pContext, MMSd_Connection *conn );
void MMSd_cleanupReportingAA( MMSd_context *pContext, MMSd_Connection *conn );
void MMSd_InitializeReportingLock(MMSd_context *cntxt );
void MMSd_RemoveReportingLock( MMSd_context *cntxt );
TMW_CLIB_API int  MMSd_initChanges( MMSd_context *cntxt);
TMW_CLIB_API void MMSd_LockReporting( MMSd_context *cntxt);
TMW_CLIB_API void MMSd_UnLockReporting( MMSd_context *cntxt);
TMW_CLIB_API void MMSd_WakeReporting( MMSd_context *cntxt);
int  MMSd_scanForDataChanges(MMSd_context *mms_context, unsigned char *outBuff, MMSd_length outBuffLen);
int  TMW_scanChanges( MMSd_context *pMmsContext );




typedef enum {
    RCB_other,
    RCB_RptEna,
    RCB_GI,
    RCB_Resv,
    RCB_ResvTms,
    SGCB_values,
    SGCB_EditSG,
    SGCB_CnfEditSG,
    SGCB_ActSG
} MMSd_RCB_Action;

TMW_CLIB_API int MMSd_CanWriteControlBlock(MMSd_ReportControl *pReportControl, MMSd_RCB_Action nAction, void * E);


/*
 * Access functions for Log Control Blocks
 */
TMW_CLIB_API int MMSd_LogControlGetLogRef(void *handle, char *pLogRef);
TMW_CLIB_API int MMSd_LogControlSetLogRef(MMSd_context *pCntxt, void *handle, char *pLogRef);
TMW_CLIB_API int MMSd_LogControlGetDataSet(void *handle, char *DatSet);
TMW_CLIB_API int MMSd_LogControlSetDataSet(void *handle, char *DatSet);
TMW_CLIB_API int MMSd_LogControlGetTrgOps(void *handle, unsigned char *pTrgOps);
TMW_CLIB_API int MMSd_LogControlSetTrgOps(void *handle, unsigned char nTrgOps);
int MMSd_LogControlGetIntgPd(  void *handle, unsigned int *pIntgPd );
int MMSd_LogControlSetIntgPd(  void *handle, unsigned int nIntgPd );
void * MMSd_LogControlGetHandle(  MMSd_context    *pMmsContext, char *LogName );

/*
 * Access functions for Report Control Blocks
 */
TMW_CLIB_API int MMSd_ReportControlSetRptID(void *handle, char *pRptID);
TMW_CLIB_API int MMSd_ReportControlSetConfRev(void *handle, unsigned long confRev);
TMW_CLIB_API int MMSd_ReportControlGetDataSet(void *handle, char *DatSet);
TMW_CLIB_API int MMSd_ReportControlSetDataSet(void *handle, char *DatSet);
TMW_CLIB_API int MMSd_ReportControlSetBufTm(void *handle, int nBufTm);
TMW_CLIB_API int MMSd_ReportControlGetTrgOps(void *handle, unsigned char *pTrgOps);
TMW_CLIB_API int MMSd_ReportControlSetTrgOps(void *handle, unsigned char nTrgOps);
int MMSd_ReportControlGetIntgPd(  void *handle, unsigned int *pIntgPd );
int MMSd_ReportControlSetIntgPd(  void *handle, unsigned int nIntgPd );
int MMSd_ReportControlGetOptFlds(  void *handle, unsigned char *pOptFlds );
int MMSd_ReportControlSetOptFlds(  void *handle, unsigned char *pOptFlds );
int MMSd_ReportControlGetBufTm(  void *handle, int *pBufTm );
int MMSd_ReportControlGetConfRev( void *handle, unsigned int *pConfRev );
int MMSd_ReportControlGetRptID( void *handle, char *pRptID );
void * MMSd_ReportControlGetHandle(  MMSd_context    *pMmsContext, char *RcbName );

/*
 * Access functions for Setting Group Control Blocks
 */
void * MMSd_SettingGroupControlGetHandle(  MMSd_context *pMmsContext, char * SgcbName );
int MMSd_SettingGroupControlGetNumOfSG( void *handle, unsigned char *pNumOfSG );
int MMSd_SettingGroupControlSetNumOfSG( void *handle, unsigned char nNumOfSG );
int MMSd_SettingGroupControlGetActSG( void *handle, unsigned char *pActSG );
int MMSd_SettingGroupControlSetActSG( void *handle, unsigned char nActSG );
int MMSd_SettingGroupControlGetEditSG( void *handle, unsigned char *pEditSG );
int MMSd_SettingGroupControlSetEditSG( void *handle, unsigned char nEditSG );
int MMSd_SettingGroupControlGetLActTm( void *handle, MMSd_UtcBTime *pLActTm );

typedef struct MMSd_UpdateCacheMember {
    MMSd_NvlComponentEval   *pNvlComponentEval;
    MMSd_NvlEval            *pNvlEval;
    int                     nMemberNum;
    unsigned char           nReason;
    MMSd_ushort             nNumChanges;
} MMSd_UpdateCacheMember;

typedef struct MMSd_UpdateCache {
    int                     nNumMembers;
    int                     nMaxMembers;
    MMSd_UpdateCacheMember  *pCacheMembers;
    unsigned char           *outBuff;
    int                     outBuffLen;
    MMSd_context            *pMmsContext;
} MMSd_UpdateCache;

TMW_CLIB_API void MMSd_OpenUpdating( MMSd_UpdateCache          *pUpdateCache,
									int                       nMaxMembers,
									MMSd_UpdateCacheMember    *pUpdateCacheMembers,
									unsigned char             *outBuff,
									int                       outBuffLen,
									MMSd_context              *pMmsContext );

TMW_CLIB_API void MMSd_CloseUpdating( MMSd_UpdateCache  *pUpdateCache);

TMW_CLIB_API int MMSd_UpdatePoint( void              *readParam,
                      	  	  	   MMSd_UpdateCache  *pUpdateCache );

/* Forces update, omits call to evaluation handler */
int MMSd_ForceUpdatePoint( void *readParam, MMSd_UpdateCache *pUpdateCache );

/* Cleanup routines used by scl_rtg and service */
void MMSd_freeXCB_list( MMSd_ReportControl *pReportControl );
void MMSd_freeSGCB_list( MMSd_ReportControl *pSGCB );

/* Edition 2 Tracking Data */

#define TMW_TRK_ORIGINATOR_ID_SIZE 65
typedef struct TMW_TRK_CTS {
    MMSd_ObjectName         objRef;
    TMW_TRK_SVC_Enum        serviceType;
    TMW_TRK_ERR_Enum        errorCode;
    unsigned char           originatorID[ TMW_TRK_ORIGINATOR_ID_SIZE ];
    MMSd_UtcBTime           t;
    int                     respAddCause;
} TMW_TRK_CTS;

typedef struct TMW_TrackingDescriptor {
    MMSd_descriptor         D;
    MMSd_tag                nAsn1Tag;
    int                     nMmsLength;
} TMW_TrackingDescriptor;


typedef struct TMW_TrackingObject *TMW_TrackingObjectPtr;
typedef struct TMW_TrackingObject {
    TMW_TRK_CTS             cts;
    void                    *pData; /* xrcb or dictionaryEntry which has changed */
    int                     nMemberCount;
    unsigned char           bInitialized;
    TMW_TRK_ClassOption     bWritten;
    TMW_TrackingVariant     trackingVariant;
    TMW_TRK_SVC_Enum        nReadService;
    TMW_TRK_SVC_Enum        nWriteService;
    TMW_TrackingDescriptor  *pTrackingDescriptors;
    int                     nTrackingDescriptors;
    TMW_TrackingObjectPtr   pNext;
} TMW_TrackingObject;

#if defined(TMW_REPORT_ALL_CLIENT_WRITES)

void TMW_OpenWriteReporting( MMSd_context              *pMmsContext,
                             MMSd_UpdateCache          *pUpdateCache,
                             MMSd_UpdateCacheMember    *pUpdateCacheMembers,
                             int                       nNumCacheMembers,
                             unsigned char             *pBuffer,
                             int                       nBuffLen );

void TMW_TriggerWriteReport( MMSd_context       *pMmsContext,
                             MMSd_entry         *pDictionaryEntry,
                             void               *parameter,
                             MMSd_descriptor    *pFinalD );

void TMW_CloseWriteReporting( MMSd_context *pMmsContext );

#endif

TMW_TrackingObject *TMW_FindTrackingObject( MMSd_context *pMmsContext, MMSd_entry *pDictionaryEntry );
TMW_TrackingObject *TMW_FindTrackingObjectByClassName( MMSd_context     *pMmsContext,
													   MMSd_ObjectName  *pObjectName,
													   char             *pClassName );

void TMW_OpenTrackingContext( MMSd_context              *pMmsContext,
                              MMSd_UpdateCache          *pUpdateCache,
                              MMSd_UpdateCacheMember    *pUpdateCacheMembers,
                              int                       nNumCacheMembers,
                              unsigned char             *pBuffer,
                              int                       nBuffLen );

TMW_TRK_SVC_Enum TMW_WriteTrackingService( TMW_TrackingObject *pTrackingObject, MMSd_entry *pDictionaryEntry );

TMW_TRK_ERR_Enum TMW_TrackingServiceError( MMSd_context *pMmsContext, unsigned long errorFlag );

void TMW_TrackCbService(MMSd_context *pMmsContext, TMW_TRK_SVC_Enum nService,
                        MMSd_ReportControl *pReportControl);

void TMW_TrackWrite(MMSd_context *pMmsContext, MMSd_entry *pDictionaryEntry,
                    MMSd_descriptor *pStartingD, MMSd_descriptor *pFinalD);

TMW_CLIB_API void TMW_SetOriginatorID(MMSd_context *pMmsContext, unsigned char *pOrIdent,
                         int nOrIdentSize);

void TMW_CloseTrackingContext( MMSd_context *pMmsContext );

void TMW_SaveTrackingContext( MMSd_context        *pMmsContext,
                              TMW_TrackingContext *pSaveTrackingContext );

void TMW_SetTrackingObject( MMSd_context *pMmsContext, NV_type *pDictionaryEntry );

void TMW_RestoreTrackingContext( MMSd_context          *pMmsContext,
                                 TMW_TrackingContext   *pTrackingContext );

TMW_CLIB_API int TMW_TRK_evalCbAttribute(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);
TMW_CLIB_API int TMW_TRK_evalCtsAttribute(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);
TMW_CLIB_API int TMW_TRK_evalServiceParameter(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

long MMSd_computeAltAccessMemberSpace( MMSd_descriptor *pAltAccess );
MMSd_descriptor *MMSd_makeNvlAltAccess( MMSd_descriptor *pAltAccess, unsigned char *pBuffer, long *nSpaceUsed );


//ACSI_report
TMW_CLIB_API unsigned long MMSd_generateEventInformationReport( void *tpConn, void *tpContext, MMSd_context *mms_context,
													MMSd_EventReportStatus *pEventReportStatus, int nEventReportStatus,
													unsigned char *outBuff, MMSd_length outBuffLen);

int MMSd_EventReportStatusSize( MMSd_context *contextPtr);

void MMSd_disconnect_occurred( MMSd_context *contextPtr,
                               MMSd_Connection *connectPtr);

//ASCI_modBeh
int TMW_setLogicalDeviceMode( MMSd_ushort nDomain, MMSd_context *pMmsContext, unsigned char nNewMode );
int TMW_setLogicalDeviceModeByName( char *pDomainName, MMSd_context *pMmsContext, unsigned char nNewMode );
unsigned char TMW_getLogicalDeviceMode( MMSd_ushort nDomain, MMSd_context *pMmsContext );
unsigned char TMW_getLogicalDeviceModeByName( char *pDomainName, MMSd_context *pMmsContext );
unsigned char TMW_getLogicalDeviceBeh( MMSd_ushort nDomain, MMSd_context *pMmsContext );
unsigned char TMW_getLogicalDeviceBehByName( char *pDomainName, MMSd_context *pMmsContext );
int TMW_setLogicalDeviceSim( MMSd_ushort nDomain, MMSd_context *pMmsContext, unsigned char nNewSim );
int TMW_setLogicalDeviceSimByName( char *pDomainName, MMSd_context *pMmsContext, unsigned char nNewSim );
unsigned char TMW_getLogicalDeviceSim( MMSd_ushort nDomain, MMSd_context *pMmsContext );
unsigned char TMW_getLogicalDeviceSimByName( char *pDomainName, MMSd_context *pMmsContext );
int TMW_handleGrRef( char *pGrRefString, MMSd_ushort nThisDomain, MMSd_context *pMmsContext );
int TMW_handleModBehLinking( MMSd_context *pMmsContext );
char *TMW_getDomainName( MMSd_context *pMmsContext, int nDomain );
int TMW_getDomainNumber( MMSd_context *pMmsContext, char *pDomainName );
unsigned char TMW_getHandlerBeh( void *E );
int MMSd_getLnLocalMode(MMSd_ControllingEntries *pCntEntries);
int MMSd_getLnLocalRemote(MMSd_ControllingEntries *pCntEntries);



//ACSI_dataChanges
void MMSd_PurgeUpdates(MMSd_UpdateCache *pUpdateCache);



//ACSI_object
int MMSd_locateDomain(MMSd_context *context, MMSd_ObjectName *nvl_obname);
MMSd_entry *MMSd_getLnDoAttribute(MMSd_entry *pLnDictEntry, char *pFcName, char *pDoName, char *pAttrName);
MMSd_ControlValueType MMSd_getMmsAttributeTag(MMSd_entry *pDictEntry);
MMSd_entry *MMSd_childOfEntry(MMSd_entry *pDictEntry, MMSd_list_entry *pBaseRefEntry, char *pName);
MMSd_entry *MMSd_parentOfEntry(MMSd_entry *pDictEntry);
MMSd_list_entry *MMSd_baseListEntry(MMSd_entry *pDictEntry);
MMSd_entry *MMSd_indexChildOfEntry(MMSd_entry *pDictEntry, MMSd_list_entry *pBaseRefEntry, unsigned long nIndex);
void MMSd_InitializeContextLock( MMSd_context *cntxt );
void MMSd_RemoveContextLock( MMSd_context *cntxt );
TMW_CLIB_API void MMSd_LockContext(MMSd_context *cntxt);
TMW_CLIB_API void MMSd_UnLockContext(MMSd_context *cntxt);
TMW_CLIB_API MMSd_entry *TMW_gethandlerEntry( void *E );
TMW_CLIB_API MMSd_context *MMSd_get_handler_mms_context(void *E);
TMW_CLIB_API void MMSd_to_61850_Name( MMSd_ObjectName *pMmsName, MMSd_ObjectName *pIec61850Name, char *pFc );



//ACSI_xCB
int MMSd_SetOwnerRcb(MMSd_ReportControl *pReportControl, MMSd_Connection *pMmsConnection);
int MMSd_HandleDisconnectRcb(MMSd_context *pMmsContext, MMSd_ReportControl *pReportControl, MMSd_Connection *pMmsConnection);
MMSd_ObjectName *TMW_accessDatSet(MMSd_ReportControl *pReportControl);


//ACSI_controls
unsigned long MMSd_getConfigOperTimeout( MMSd_ControlConfigEntries *pConfigEntries );

void TMW_SetTrackingAddCause( MMSd_context *pMmsContext, int nAddCause );
unsigned long MMSd_getConfigSboTimeout(MMSd_ControlConfigEntries *pConfigEntries);
unsigned char MMSd_getConfigSboClass(MMSd_ControlConfigEntries *pConfigEntries);
unsigned long MMSd_getConfigOperTimeout(MMSd_ControlConfigEntries *pConfigEntries);

TMW_CLIB_API unsigned char MMSd_getConfigCtlModel( MMSd_ControlConfigEntries *pConfigEntries );

void MMSd_DeSelectControl(MMSd_SboControlCommon *pSelectCommon, MMSd_context *pMmsContext);
void MMSd_updateStSeld(MMSd_ControlStatusEntries *pStatusEntries,
                       MMSd_context *pMmsContext, unsigned char bNewValue);

TMW_CLIB_API void MMSd_retrieveOrIdent(	unsigned char *pOrIdent, 
										unsigned char **ppOrIdentValue, 
										int *pOrIdentSize);

TMW_CLIB_API void MMSd_storeOrIdent( unsigned char *pOrIdentSource, int nOrIdentSize,
									 unsigned char *pOrIdentDest);

int MMSd_retrieveConfigEntries( MMSd_entry *pDoEntry,
                                MMSd_ControlConfigEntries *pConfigEntries);

int MMSd_use_enhanced_security(unsigned char nCtlModel);

int MMSd_IsSelectedAnyConnection( MMSd_SboControlSelectValues *pSelectValues,
                                  MMSd_ControlConfigEntries *pConfigEntries);

int MMSd_IsSelectedThisConnection( MMSd_SboControlSelectValues *pSelectValues,
                                   MMSd_ControlConfigEntries *pConfigEntries,
                                   MMSd_context *pMmsContext);

int MMSd_retrieveStatusEntries( MMSd_entry *pDoEntry,
                                MMSd_ControlStatusEntries *pStatusEntries);

int MMSd_retrieveControlServiceParEntries( MMSd_entry *pSboOrOperEntry,
										   MMSd_ControlServiceParEntries *pServiceEntries);

int MMSd_retrieveOperSboParams(MMSd_entry *pOperEntry,
                               MMSd_OperControlCheckParams *pOperPars);
int MMSd_retrieveControllingEntries(MMSd_entry *pDictEntry,
                                    MMSd_context *pMmsContext, int nDomain,
                                    MMSd_ControllingEntries *pCntEntries);
int MMSd_validateValue(MMSd_ControlServiceValue *pCtlVal,
                       MMSd_ControlStatusEntries *pStatusEntries);
int MMSd_retrieveOperSboParams(MMSd_entry *pOperEntry,
                               MMSd_OperControlCheckParams *pOperPars);
int MMSd_validateServiceParams(MMSd_ControlServiceParams *pServicePars1,
                               MMSd_ControlServiceParams *pServicePars2,
                               MMSd_ControlParsFound_t nMask);
int MMSd_retrieveCancelOperSboParams(MMSd_entry *pCancelEntry,
                                     MMSd_CancelControlTestParams *pCancelPars);
int MMSd_retrieveControlServiceParams(
        MMSd_ControlServiceParEntries *pServiceEntries,
        MMSd_ControlServiceParams *pServiceParams);


//ACSI_datasets
int TMW_binsearch(unsigned long key, unsigned char *array, unsigned int rec_size, unsigned int key_offset, long num_recs, long *index);
int TMW_sort(unsigned char *array, unsigned int rec_size, unsigned int key_offset, long num_recs);
int TMW_GetAltAccessSize(MMSd_descriptor *pAlt, int *pUsedLen);
int TMW_GetAltAccessString(MMSd_descriptor *pAlt, char *msgBuff, int msgBuffLen);



//ASCI_user
TMW_CLIB_API int MMSd_retrieveGenericValue(MMSd_entry *pDictEntry, MMSd_ControlServiceValue *pServiceValue);
TMW_CLIB_API int MMSd_storeGenericValue(MMSd_ControlServiceValue *pServiceValue, MMSd_entry *pDictEntry);




#define SORT_NVL_EVAL( components, num_recs ) \
            TMW_sort( (unsigned char *) components, \
                      sizeof( MMSd_NvlComponentEval ), \
                      sizeof( void *), \
                      (long) num_recs )

#define FIND_NVL_EVAL( key, components, num_recs, index ) \
            TMW_binsearch( (unsigned long) key, \
                           (unsigned char *) components, \
                           sizeof( MMSd_NvlComponentEval ), \
                           sizeof( void *), \
                           (long) num_recs,  \
                           index )
#define TMW_MAX_SORT_RECORD_SIZE 128

#define KEY_FIELD( buffer, offset ) \
        *( (unsigned long *) ((buffer) + (offset)) )

#ifdef __cplusplus
};
#endif

#endif
