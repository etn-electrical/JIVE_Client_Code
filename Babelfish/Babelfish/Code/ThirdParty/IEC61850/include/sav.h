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
 *  Declarations for Sampled Values.
 *
 *  This file should not require modification.
 */

#ifndef _SAV_H
#define _SAV_H

#if defined(TMW_USE_90_5)
#include "spp.h"        /* 90-5 Extensiom */
#endif

/* There are three styles of buffer allocation supported:

 TMW_SAV_STATIC_ALLOCATION   - same as old style, can still be overridded
 TMW_SAV_DYNAMIC_ALLOCATION  - boot time malloc, so that multiple sampled values contexts can be opened
 TMW_SAV_UNBOUND_ALLOCATION  - run-time mallocs, with no limits (used for workstation type applications)

 */

/* No more than one can be defined in the package */
/* This should generate an error unless at most one is defined */
#if defined(TMW_SAV_STATIC_ALLOCATION)
#define TMW_SAV_ALLOCATION 1
#endif
#if defined(TMW_SAV_DYNAMIC_ALLOCATION)
#define TMW_SAV_ALLOCATION 2
#endif
#if defined(TMW_SAV_UNBOUND_ALLOCATION)
#define TMW_SAV_ALLOCATION 3
#endif

/* If none of them defined, assume old-style static or overrides */
#if !defined(TMW_SAV_ALLOCATION)
#define TMW_SAV_STATIC_ALLOCATION
#endif


/* Definitions for Error returns */
typedef enum {TMW_SAV_SUCCESS,
              TMW_SAV_ERR_LINK,
              TMW_SAV_ERR_SIZE,
              TMW_SAV_ERR_NO_BUFFERS,
              TMW_SAV_ERR_PARAMS,
              TMW_SAV_ERR_SIGNATURE
             } TMW_SAV_Error;

/* Switches for enabling/sensing optional elements */
#define TMW_SAV_refrTm_VALID        0x4000
#define TMW_SAV_smpSynch_VALID      0x2000
#define TMW_SAV_smpRate_VALID       0x1000
#define TMW_SAV_datSet_VALID        0x0800
#define TMW_SAV_security_VALID      0x0400
#define TMW_SAV_svID_VALID          0x0200
#define TMW_SAV_smpCnt_VALID        0x0100
#define TMW_SAV_confRev_VALID       0x0080
#define TMW_SAV_smpMod_VALID        0x0040

/* Mandatory elements (9-2LE) */
#define TMW_SAV_APDU_MINIMUM \
    ( TMW_SAV_svID_VALID       \
      | TMW_SAV_smpCnt_VALID   \
      | TMW_SAV_confRev_VALID  \
      | TMW_SAV_smpSynch_VALID )

typedef struct TMW_SAV_ASDU *TMW_SAV_ASDU_PTR;
typedef struct TMW_SAV_APDU *TMW_SAV_APDU_PTR;
typedef struct TMW_SAV_Context *TMW_SAV_CONTEXT_PTR;

typedef enum { TMW_SAV_Normal, 
               TMW_SAV_UDP } TMW_SAV_Protocol; /* 90-5 Extension */

/* **************************************** */
/*      Callback routine prototypes         */
/* **************************************** */

/* Callback routine prototypes
 ******************************************************************************
 */
/* User callback routine to supply sample data for output streams */
typedef MMSd_length (* TMW_SAV_EncodeAsduData)( TMW_SAV_APDU_PTR    pApdu,
        TMW_SAV_ASDU_PTR    pAsdu,
        int                 smpCnt,
        void                *encode_callback_data,
        MMSd_descriptor     *E );

/* User callback routine to receive indications from subscriptions */
typedef void (*TMW_SAV_Indication)(void *handle, TMW_SAV_APDU_PTR pApdu);

/* User callback routine called when a sampled values control block is enabled.
 This routine should be used to start the sampling process.  Note that the
 MMS context is passed as a void - this is to allow this code to be used
 in SAV-only configurations (i.e. no MMS server). In that case, the callback
 is not used, but the prototype still is needed. If the context is needed
 (for example, to check the dataset definition) cast it to MMSd_context *.

 Note that the sampling process then drives the publishing by calling
 TMW_SAV_Publish when the samples have been obtained.
 */
typedef int (*TMW_SAV_Start)(void *handle, void *userData, void *pMmsContext, TMW_SAV_APDU_PTR pApdu);

/* User callback routine called when a sampled values control block is enabled.
 This routine should be used to stop the sampling process.  Note that the
 MMS context is passed as a void - this is to allow this code to be used
 in SAV-only configurations (i.e. no MMS server). In that case, the callback
 is not used, but the prototype still is needed.
 */
typedef int (*TMW_SAV_Stop)(void *handle, void *userData, void *pMmsContext, TMW_SAV_APDU_PTR pApdu);

typedef struct TMW_SAV_ASDU {
    unsigned int        validMembers;
    int                 nAsduNum;
    char                svID[ MMSD_OBJECT_NAME_LEN ];
    char                datSet[ MMSD_OBJECT_NAME_LEN ];
    int                 smpCnt;     /* 16 bits */
    int                 confRev;    /* 32 bits */
    MMSd_UtcBTime       refrTm;     /* EntryTime, optional */
    unsigned char       smpSynch;   /* Enum */
    unsigned int        smpRate;    /* 16 bits, optional */
    int                 smpMod;     /* 16 bits, optional */
    TMW_SAV_ASDU_PTR    pNext;
    unsigned char       *dataPtr;
    MMSd_length         dataLen;
    TMW_SAV_EncodeAsduData  encode_callback;
    void                *encode_callback_data;
} TMW_SAV_ASDU;

typedef struct TMW_SAV_APDU {
    void                *handle;
    int                 useSecurity;
    int                 noASDUs;
    TMW_SAV_Protocol    protocol;                               /* 90-5 Extension */
    unsigned char       security[ SAV_MAX_SECURITY ];
    int                 nSecurityLen;
    unsigned char       *pPrivateSecurityData;
    int                 nPrivateSecurityDataLen;
    TMW_61850_VLAN_MakeSignatureFcn     genVlanSignature;       /* SAV outputs only */
    TMW_61850_VLAN_CheckSignatureFcn    checkVlanSignature;     /* SAV inputs only */
    void                                *pVlanSignatureData;    /* SAV inputs and outputs */
    void                                *pCryptoContext;        /* SAV inputs and outputs */
    unsigned char       simulateBit;
    int                 lengthOfExtension;
    int                 nDecodedPduLength;
    unsigned char       *pDecodedPduStart;
    unsigned int        smpRate;    /* 16 bits */
    int                 smpCnt;     /* 16 bits */
    TMW_Mac_Address     address; 
    unsigned int        VLAN_AppID;
    unsigned int        VLAN_VID;
    unsigned char       VLAN_Priority;
    TMW_SAV_ASDU        *pHead;
    TMW_SAV_ASDU        *pTail;
    TMW_SAV_Indication  indication_call_back;
    void                *indication_call_back_data;
    TMW_SAV_Start       start_call_back;
    void                *start_call_back_data;
    TMW_SAV_Stop        stop_call_back;
    void                *stop_call_back_data;
    TMW_Packet          packet;
#if defined(TMW_USE_90_5)
    TMW_SPP_Data        sppData;        /* 90-5 Extension */
#endif
    void                *pUserData;
    TMW_SAV_CONTEXT_PTR pContext;
    TMW_SAV_APDU_PTR    pNext;
} TMW_SAV_APDU;

typedef enum {
    tmwSavProtocol_Normal,
    tmwSavProtocol_UDP
} tmwSavProtocol_t; /* 90-5 Extension */

typedef struct TMW_SAV_Context {
    TMW_SAV_APDU        *avail_apdus;
    TMW_SAV_APDU        *active_in;
    TMW_SAV_APDU        *active_out;
    TMW_SAV_ASDU        *avail_asdus;
    char                adapter_name[129];
    TMW_DL_Context      *link_context;
    unsigned char       simulateBit;
    TmwTargSem_t    sem;

    TMW_TARG_EVENT  eve;

} TMW_SAV_Context;

#if defined(TMW_USE_90_5)
TMW_SAV_Error TMW_SAV_CreateUDP( TMW_SAV_Context     *cntxt,
                                 void                *handle,
                                 unsigned char       simulateBit,
                                 TMW_Mac_Address     *address,
                                 int                 port,
                                 unsigned int        VLAN_AppID );

TMW_SAV_Error TMW_SAV_SubscribeUDP( TMW_SAV_Context      *cntxt, 
                                    TMW_Mac_Address      *address, 
                                    int                  port,
                                    unsigned int         nAppID, 
                                    void                 *handle,
                                    int                  noASDUs,
                                    TMW_SAV_Indication   call_back, 
                                    void                 *call_back_data );
									
#endif

TMW_CLIB_API void TMW_SAV_byteorder(unsigned char *valstr, int length);

/* Called from user TMW_SAV_EncodeAsduData callback routine to insert data into output stream */
TMW_CLIB_API MMSd_length TMW_SAV_EmitSamples(unsigned char *s, MMSd_length len, MMSd_descriptor *E);

/* --------------------------------------------------------------
 ---   TMW_SAV_Initialize()
 ---       This function initializes the pool of TMW_SAV_APDU records,
 --- verifies the link adapter name and opens the link layer adapter
 --- (calls ETH_open_port()).
 ---
 --- NOTE: adapter_name must be initialized to valid setting before calling.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---
 ---   Returns:
 ---      TMW_SAV_SUCCESS     - Initialization succeeded
 ---      TMW_SAV_ERR_PARAMS  - Bad parameters passed
 ---      TMW_SAV_ERR_LINK    - Link layer failed to open
 -------------------------------------------------------------- */
TMW_CLIB_API TMW_SAV_Error TMW_SAV_Initialize(TMW_SAV_Context *cntxt);

/* --------------------------------------------------------------
 ---   TMW_SAV_Close()
 ---       This function shuts down all SAV activity for the given context.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---
 ---   Return:
 ---      none
 -------------------------------------------------------------- */
TMW_CLIB_API void TMW_SAV_Close(TMW_SAV_Context *cntxt);

/* --------------------------------------------------------------
 ---   TMW_SAV_Subscribe()
 ---       This function initializes an TMW_SAV_APDU
 --- entry from the available pool with information about a
 --- Sampled Values input stream to receive, and inserts it into the
 --- active_in pool and binds the entry to the data link.
 ---
 ---   Calling sequence:
 ---       cntxt           - TMW_SAV_Contect structure pointer
 ---       mac             - Pointer to the broadcast SAV address
 ---       AppID           - VLAN Application ID
 ---       handle          - Used defined handle pointer
 ---       call_back       - User defined indication callback function
 ---       call_back_data  - parameter to pass into callback function
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS         - Subscription succeeded
 ---      TMW_SAV_ERR_PARAMS      - Bad parameters
 ---      TMW_SAV_ERR_LINK        - Link layer failed
 ---      TMW_SAV_ERR_NO_BUFFERS  - Insufficient resources
 -------------------------------------------------------------- */
TMW_CLIB_API TMW_SAV_Error TMW_SAV_Subscribe(TMW_SAV_Context *cntxt, TMW_Mac_Address *mac, unsigned int AppID, void *handle,
                                int noASDUs, TMW_SAV_Indication call_back, void *call_back_data);

TMW_CLIB_API TMW_SAV_Error TMW_SAV_SubscribeSecure(TMW_SAV_Context *cntxt, TMW_Mac_Address *mac, unsigned int AppID, void *handle,
                                      int noASDUs, TMW_SAV_Indication call_back, void *call_back_data,
                                      TMW_61850_VLAN_CheckSignatureFcn checkVlanSignature, void *pCryptoContext, void *pVlanSignatureData);

/* --------------------------------------------------------------
 ---   TMW_SAV_Unsubscribe()
 ---       This function finds the TMW_SAV_APDU entry
 --- in the active_in pool that matches the handle, unbinds
 --- it and removes it from the active_in pool.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       handle  - Used defined handle pointer
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS: Unsubscribe succeeded
 ---      TMW_SAV_ERR_PARAMS: Unsubscribe failed (handle not found)
 -------------------------------------------------------------- */
TMW_CLIB_API TMW_SAV_Error TMW_SAV_Unsubscribe(TMW_SAV_Context *cntxt, void *handle);

/* --------------------------------------------------------------
 ---   TMW_SAV_Create()
 ---       This function sets up a Sampled Values output stream.
 --- Data can be sent on the output stream by passing the same
 --- handle to TMW_SAV_Publish. Note that the initialized stream
 --- contains no ASDUs (sets of sampled values).  These must be
 --- added to the stream by calling TMW_SAV_Add_ASDU for each
 --- set of samples to be contained in each message.  For example
 --- in 9-2LE the MSVCB01 contains one set of samples per message,
 --- so TMW_SAV_Add_ASDU must be called once to initialize the stream
 --- to a single set of samples per message.  For MSVCB02, call
 --- TMW_SAV_Add_ASDU eight times before the first call to
 --- TMW_SAV_Publish to establish eight sample sets per message.
 ---
 ---   Calling sequence:
 ---       cntxt          - TMW_SAV_Context structure pointer
 ---       handle         - Used defined handle pointer
 ---       security       - Unsigned char security data
 ---       nSecurityLen   - Length of security
 ---       address        - Destination address
 ---       VLAN_AppID     - VLAN AppID
 ---       VLAN_VID       - VLAN ID
 ---       VLAN_Priority  - VLAN Priority
 ---
 ---   Return:
 ---       TMW_SAV_ERR_PARAMS     - Bad parameters passed in
 ---       TMW_SAV_ERR_LINK       - Link layer failed to open
 ---       TMW_SAV_ERR_NO_BUFFERS - Allocation failed
 ---       TMW_SAV_SUCCESS        - Output stream created
 -------------------------------------------------------------- */
TMW_SAV_Error TMW_SAV_Create(TMW_SAV_Context *cntxt, void *handle, unsigned char *security, int nSecurityLen,
                             TMW_Mac_Address *address, unsigned int VLAN_AppID, unsigned int VLAN_VID, unsigned char VLAN_Priority);

TMW_SAV_Error TMW_SAV_CreateSecure(TMW_SAV_Context *cntxt, void *handle, unsigned char *security, int nSecurityLen,
                                   TMW_Mac_Address *address, unsigned int VLAN_AppID, unsigned int VLAN_VID, unsigned char VLAN_Priority,
                                   unsigned char *pPrivateSecurityData, int nPrivateSecurityDataLen,
                                   TMW_61850_VLAN_MakeSignatureFcn makeVlanSignature, void *pCryptoContext, void *pVlanSignatureData);

TMW_SAV_APDU *TMW_SAV_GetApdu(TMW_SAV_Context *cntxt, void *handle);

/* --------------------------------------------------------------
 ---   TMW_SAV_Add_ASDU()
 ---       This function adds a sample set to a Sampled Values
 ---  output stream previously created by TMW_SAV_Create. This must
 ---  be called one for each sample set to be contained in the
 ---  messages for the stream.  See TMW_SAV_Create for details.
 ---
 ---       pApdu                  - Pointer to output stream APDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       NULL       - Failed due to insufficient resources
 ---       otherwise  - Pointer to new ASDU
 -------------------------------------------------------------- */
TMW_SAV_ASDU *TMW_SAV_Add_ASDU(TMW_SAV_APDU *pApdu, TMW_SAV_EncodeAsduData encode_callback, void *encode_calback_data);

/* --------------------------------------------------------------
 ---   TMW_SAV_Set_ASDU_Encoding()
 ---       This function sets the encoding callback function and
 ---  parameter for an ASDU within an output stream previously
 ---  created by TMW_SAV_Create.
 ---
 ---       pAsdu                  - Pointer to ASDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       TMW_SAV_PARAMS         - Failed due bad parameters
 ---       TMW_SAV_SUCCESS        - Successfully set
 -------------------------------------------------------------- */
TMW_SAV_Error TMW_SAV_Set_ASDU_Encoding(TMW_SAV_ASDU *pAsdu, TMW_SAV_EncodeAsduData encode_callback,
                                        void *encode_callback_data);

/* --------------------------------------------------------------
 ---   TMW_SAV_Set_All_ASDU_Encoding()
 ---       This function sets the encoding callback function and
 ---  parameter for all ASDUs within the APDU for an output stream
 ---  previously created by TMW_SAV_Create.  See TMW_SAV_Create for
 ---  details.
 ---
 ---       pApdu                  - Pointer to output stream APDU
 ---       encode_call_back       - Callback function to encode data
 ---       encode_call_back_data  - User data for callback function
 ---
 ---   Return:
 ---       TMW_SAV_PARAMS       - Failed due to bad parameters or
 ---                              no ASDUs in APDU.
 ---       TMW_SAV_SUCCESS      - Succeeded
 -------------------------------------------------------------- */
TMW_SAV_Error TMW_SAV_Set_All_ASDU_Encoding(TMW_SAV_APDU *pApdu, TMW_SAV_EncodeAsduData encode_callback,
        void *encode_callback_data);

/* --------------------------------------------------------------
 ---   TMW_SAV_Delete()
 ---       This function deletes a Sampled Values output stream.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       handle  - Used defined handle pointer
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS     - Output stream deleted
 ---      TMW_SAV_ERR_PARAMS  - Handle not found
 -------------------------------------------------------------- */
TMW_SAV_Error TMW_SAV_Delete(TMW_SAV_Context *cntxt, void *handle);

/* --------------------------------------------------------------
 ---   TMW_SAV_Publish()
 ---       This function intiates the sending of data on an
 --- active output Sampled Values stream.
 ---
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---       handle  - Used defined handle pointer
 ---
 ---   Return:
 ---      TMW_SAV_SUCCESS     - Samples published
 ---      TMW_SAV_ERR_PARAMS  - Bad parameters
 ---      TMW_SAV_ERR_SIZE    - Encoding failed
 ---      TMW_SAV_ERR_LINK    - Link layer error
 -------------------------------------------------------------- */
TMW_CLIB_API TMW_SAV_Error TMW_SAV_Publish(TMW_SAV_Context *cntxt, void *handle);

/* --------------------------------------------------------------
 ---   TMW_SAV_Service()
 ---       This function simply services the data link.
 --- The Sampled Values protocol has no internal timers, since
 --- it is strictly driven by the sampling process.  There are
 --- no timeouts or retransmissions, so there is nothing else
 --- required for periodic service. This is only required for
 --- systems not using the full threaded (i.e. TMW_stack/waittask.c)
 --- part of the API - such as single threaded systems doing polled
 --- I/O - which likely won't be fast enough to do Sampled Values
 --- anyway.
 ---   Calling sequence:
 ---       cntxt   - TMW_SAV_Contect structure pointer
 ---   Return:
 ---       VOID
 -------------------------------------------------------------- */
void TMW_SAV_Service(TMW_SAV_Context *cntxt);

/* --------------------------------------------------------------
 --- Sampled Values encoding and decoding macros
 ---
 --- Unlike MMS and Goose, the data values for Sampled Values
 --- (9-2) are not ASN.1 encoded.  These macros work the same
 --- way as the ASN.1 equivalents, but the data is all untagged
 --- fixed-length encoded, so no error checking can be done.
 -------------------------------------------------------------- */

/* These sizes for the fixed-length encodings come from the 9-2 spec */
#define TMW_SAV_OCTETSTRING_SIZE        20
#define TMW_SAV_BITSTRING_SIZE           4
#define TMW_SAV_VISIBLESTRING_SIZE      35
#define TMW_SAV_MMSSTRING_SIZE          20
#define TMW_SAV_BINARYTIME_SIZE          6
#define TMW_SAV_UTCTIME_SIZE             8
#define TMW_SAV_FLOAT32_SIZE             4
#define TMW_SAV_FLOAT64_SIZE             8

#define TMW_SAV_Decoding_Error           1

#define TMW_SAV_fetchSAV_Errors( D, len )             \
    ( MMSd_noError( *(D) )                            \
        ? ( ( MMSd_DescIndex( *(D) ) + (len) > MMSd_DescLen( *(D) ) )  \
                ? ( MMSd_setDescErrorFlag( TMW_SAV_Decoding_Error, *(D) ) )\
                : 0 )                                 \
        : 1 )                                         \
 
#define TMW_SAV_fetchSAV_COMPLETE( D )                \
    ( MMSd_noError( *(D) )                            \
       ? ( MMSd_DescIndex( *(D) ) == MMSd_DescLen( *(D) ) ) \
       :  0 )


#define TMW_SAV_fetchSAV_Octets( D, buffer, buffLen ) \
    {                                                 \
        if ( !(TMW_SAV_fetchSAV_Errors( D, buffLen ) ) ) \
        {                                             \
            memcpy( buffer, MMSd_getDescPtr( *D ), buffLen );  \
            MMSd_DescIndex( *D ) = MMSd_DescIndex( *D ) + buffLen;  \
        }                                             \
    }

#define TMW_SAV_fetchSAV_BOOLEAN( D, pBVal ) \
    {                                                 \
        if ( !TMW_SAV_fetchSAV_Errors( (D), 1 ) )     \
        {                                             \
            *(pBVal) = MMSd_fetchDescOctet( *(D) );   \
        }                                             \
    }

#define TMW_SAV_fetchSAV_BITSTRING( D, pBitVal, len ) \
    {                                                 \
        unsigned char bits[ TMW_SAV_BITSTRING_SIZE ]; \
        int lclLen, numBytes;                         \
                                                      \
        if ( (len) < 0 )                              \
            lclLen = -(len);                          \
        else                                          \
            lclLen = (len);                           \
                                                      \
        numBytes = (lclLen + 7) / 8;                  \
                                                      \
        if ( numBytes > TMW_SAV_BITSTRING_SIZE )      \
            return( FALSE );                          \
                                                      \
        TMW_SAV_fetchSAV_Octets( D, bits, TMW_SAV_BITSTRING_SIZE );        \
        memcpy( pBitVal, bits, numBytes );            \
    }

#define TMW_SAV_fetchSAV_BINARYTIME( D, pBinaryTime ) \
    {                                                 \
        int i;                                        \
        unsigned long temp;                           \
        unsigned char buffer[ TMW_SAV_BINARYTIME_SIZE ];  \
                                                      \
        temp = 0L;                                    \
        buffer[4] = 0;                                \
        buffer[5] = 0;                                \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_BINARYTIME_SIZE );  \
        for (i = 0; i < 4; i++)                       \
        {                                             \
            temp = (temp << 8);                       \
            temp = temp | buffer[ i ];                \
        }                                             \
        (pBinaryTime)->millisecondsSinceMidnight = temp;  \
        (pBinaryTime)->daysSinceJan1_1984 = (buffer[ 4 ] << 8) | buffer[ 5 ]; \
    }

#define TMW_SAV_fetchSAV_INT8( D, pIntVal )           \
    {                                                 \
        if ( !TMW_SAV_fetchSAV_Errors( D, 1 ) )       \
            *(pIntVal) = MMSd_fetchDescOctet( *(D) ); \
    }

#define TMW_SAV_fetchSAV_INT16( D, pIntVal )          \
    {                                                 \
        int i;                                        \
        short v;                                      \
                                                      \
        if ( !TMW_SAV_fetchSAV_Errors( D, 2 ) )       \
        {                                             \
            if ( ( (unsigned char) MMSd_getDescOctet( *(D) ) ) & 0x80 )  \
                v = -1;                               \
            else                                      \
                v = 0;                                \
                                                      \
            for ( i = 0; i < 2; i++ )                 \
            {                                         \
                v = ( v << 8 ) | (unsigned char) MMSd_fetchDescOctet( *(D) );  \
            }                                         \
            *(pIntVal) = v;                           \
        }                                             \
    }

#define TMW_SAV_fetchSAV_INT32( D, pIntVal )          \
    {                                                 \
        int i;                                        \
        int v;                                        \
                                                      \
        if ( !TMW_SAV_fetchSAV_Errors( D, 2 ) )       \
        {                                             \
            if ( ( (unsigned char) MMSd_getDescOctet( *(D) ) ) & 0x80 )   \
                v = -1;                               \
            else                                      \
                v = 0;                                \
                                                      \
            for ( i = 0; i < 4; i++ )                 \
            {                                         \
                v = ( v << 8 ) | (unsigned char) MMSd_fetchDescOctet( *(D) );  \
            }                                         \
            *(pIntVal) = v;                           \
        }                                             \
    }

#define TMW_SAV_fetchSAV_UINT8( D, pUintVal ) \
        *(pUintVal) = MMSd_fetchDescOctet( *(D) )

#define TMW_SAV_fetchSAV_UINT16( D, pUintVal )        \
    {                                                 \
        int i;                                        \
        int v;                                        \
                                                      \
        v = 0;                                        \
                                                      \
        for ( i = 0; i < 2; i++ )                     \
        {                                             \
            v = ( v << 8 ) | (unsigned char) MMSd_fetchDescOctet( *(D) );  \
        }                                             \
        *(pUintVal) = v;                              \
    }

#define TMW_SAV_fetchSAV_UINT24( D, pUintVal )        \
    {                                                 \
        int i;                                        \
        int v;                                        \
                                                      \
        v = 0;                                        \
                                                      \
        for ( i = 0; i < 3; i++ )                     \
        {                                             \
            v = ( v << 8 ) | (unsigned char) MMSd_fetchDescOctet( *(D) );  \
        }                                             \
        *(pUintVal) = v;                              \
    }

#define TMW_SAV_fetchSAV_UINT32( D, pUintVal )        \
    {                                                 \
        int i;                                        \
        int v;                                        \
                                                      \
        v = 0;                                        \
                                                      \
        for ( i = 0; i < 4; i++ )                     \
        {                                             \
            v = ( v << 8 ) | (unsigned char) MMSd_fetchDescOctet( *(D) );  \
        }                                             \
        *(pUintVal) = v;                              \
    }

#define TMW_SAV_fetchSAV_OCTETSTRING( D, octetString, len ) \
    {                                                 \
        unsigned char lclOctets[ TMW_SAV_OCTETSTRING_SIZE ];  \
        int lclLen = (len);                           \
                                                      \
        if ( lclLen < 0 )                             \
            lclLen = -(len);                          \
                                                      \
        if ( lclLen > TMW_SAV_OCTETSTRING_SIZE )      \
            lclLen = TMW_SAV_OCTETSTRING_SIZE;        \
                                                      \
        TMW_SAV_fetchSAV_Octets( D, lclOctets, TMW_SAV_OCTETSTRING_SIZE );  \
        memcpy( octetString, lclOctets, lclLen );     \
    }

#define TMW_SAV_fetchSAV_VISIBLESTRING( D, visibleStringVal, len ) \
    {                                                 \
        unsigned char lclOctets[ TMW_SAV_VISIBLESTRING_SIZE ];  \
        int lclLen = (len);                           \
                                                      \
        if ( lclLen < 0 )                             \
            lclLen = -(len);                          \
                                                      \
        if ( lclLen > TMW_SAV_VISIBLESTRING_SIZE )    \
            lclLen = TMW_SAV_VISIBLESTRING_SIZE;      \
                                                      \
        TMW_SAV_fetchSAV_Octets( D, lclOctets, TMW_SAV_VISIBLESTRING_SIZE );  \
        memcpy( visibleStringVal, lclOctets, lclLen ); \
    }

#define TMW_SAV_fetchSAV_MMSSTRING( D, visibleString, len ) \
    {                                                 \
        unsigned char lclOctets[ TMW_SAV_MMSSTRING_SIZE ];  \
        int lclLen = (len);                           \
                                                      \
        if ( lclLen < 0 )                             \
            lclLen = -(len);                          \
                                                      \
        if ( lclLen > TMW_SAV_MMSSTRING_SIZE )        \
            lclLen = TMW_SAV_MMSSTRING_SIZE;          \
                                                      \
        TMW_SAV_fetchSAV_Octets( D, lclOctets, TMW_SAV_MMSSTRING_SIZE );  \
        memcpy( visibleString, lclOctets, lclLen );   \
    }

#define TMW_SAV_fetchSAV_UTCTIME( D, pUtcVal )        \
    {                                                 \
        int i;                                        \
        unsigned long temp;                           \
        unsigned char buffer[ TMW_SAV_UTCTIME_SIZE ]; \
                                                      \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_UTCTIME_SIZE );  \
        temp = 0L;                                    \
        for (i = 0; i < 4; i++)                       \
        {                                             \
            temp = (temp << 8);                       \
            temp = temp | buffer[ i ];                \
        }                                             \
        (pUtcVal)->secondOfCentury = temp;            \
                                                      \
        temp = 0L;                                    \
        for (i = 0; i < 3; i++)                       \
        {                                             \
            temp = (temp << 8);                       \
            temp = temp | buffer[ i + 4 ];            \
        }                                             \
        (pUtcVal)->fractionOfSecond = temp << 8;      \
        (pUtcVal)->quality = (MMSd_UtcQuality) buffer[ 7 ];  \
    }

#ifdef MMSD_FLOAT_INTEL_BYTE_ORDERING
#define TMW_SAV_fetchSAV_FLOAT32( D, pF32Val )        \
    {                                                 \
        unsigned char buffer[ TMW_SAV_FLOAT32_SIZE ]; \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_FLOAT32_SIZE );  \
        TMW_SAV_byteorder( buffer, TMW_SAV_FLOAT32_SIZE ); \
        memcpy( pF32Val, buffer, TMW_SAV_FLOAT32_SIZE ); \
    }

#define TMW_SAV_fetchSAV_FLOAT64( D, pF64Val )        \
    {                                                 \
        unsigned char buffer[ TMW_SAV_FLOAT64_SIZE ]; \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_FLOAT64_SIZE );  \
        TMW_SAV_byteorder( buffer, TMW_SAV_FLOAT64_SIZE ); \
        memcpy( pF64Val, buffer, TMW_SAV_FLOAT64_SIZE ); \
    }
#else
#define TMW_SAV_fetchSAV_FLOAT32( D, pF32Val )        \
    {                                                 \
        unsigned char buffer[ TMW_SAV_FLOAT32_SIZE ]; \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_FLOAT32_SIZE );  \
        memcpy( pF32Val, buffer, TMW_SAV_FLOAT32_SIZE ); \
    }

#define TMW_SAV_fetchSAV_FLOAT64( D, pF64Val )        \
    {                                                 \
        unsigned char buffer[ TMW_SAV_FLOAT64_SIZE ]; \
        TMW_SAV_fetchSAV_Octets( D, buffer, TMW_SAV_FLOAT64_SIZE );  \
        memcpy( pF64Val, buffer, TMW_SAV_FLOAT64_SIZE ); \
    }
#endif

#endif      /*  _SAV_H */
