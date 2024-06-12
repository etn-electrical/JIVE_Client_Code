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
 *  This file contains Definitions for use with the connection
 *  management function of MMSd.  Included are definitions of the
 *  basic data structures, negotiated parameters, and connection state.
 *
 *  This file should not require modification.
 */

#ifndef _CONNECT_H
#define _CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tmwtarg.h"

#include "dictiona.h"



#define MMSd_NUM_SERVICE_FLAGS 85
#define SERVICE_FLAGS_INIT        \
    {   0xee,       /* byte  0 */ \
        0x08,       /* byte  1 */ \
        0x00,       /* byte  2 */ \
        0x3f,       /* byte  3 */ \
        0x00,       /* byte  4 */ \
        0x00,       /* byte  5 */ \
        0x00,       /* byte  6 */ \
        0x7e,       /* byte  7 */ \
        0x01,       /* byte  8 */ \
        0x03,       /* byte  9 */ \
        0x98        /* byte 10 */ \
    }

#define MMSd_NUM_CBB_FLAGS  11
#define CBB_FLAGS_INIT {0xe1,0x00}

/* --- Definitions for accessing the parameters of Initiate
   ------------------------------------------------------------
*/

#define MMSd_NUM_SERVICE_BYTES  11 /* Use one for num bits unused */
typedef unsigned char           MMSd_initiateServices[MMSd_NUM_SERVICE_BYTES];

#define MMSd_NUM_CBB_BYTES      2
typedef unsigned char           MMSd_initiateCBBs[MMSd_NUM_CBB_BYTES];

typedef struct {
    unsigned int LocalDetail;           /* Maximum size of an MMS PDU for the association */
    int ServicesCalling;                /* Maximum outstanding Client->Server transactions */
    int ServicesCalled;                 /* Maximum outstanding Server->Client transactions */
    int NestLevel;                      /* Depth of data structure nesting */
    int VersNumber;                     /* Should be 1 for current ISO 9506 version */
    MMSd_initiateCBBs ParameterCBBs;    /* Bitstring defining the MMS Parameter CBBs */
    MMSd_initiateServices ServicesSup;  /* Bitstring defining the MMS Services supported */
} MMSd_InitiatePars;

#define MMSd_ConnStateConcluding 3
#define MMSd_ConnStateConcluded 2
#define MMSd_ConnStateAssociated 1
#define MMSd_ConnStateNotAssociated 0

#ifdef MMSD_FULL_STACK
typedef struct Pres_ctx {
    int ctx_id;
    int abstract;
    int transfer;
    int reason;
    int result;
} Pres_ctx;

typedef enum {
    MMS_protocol,
    ACSE_protocol
} Pres_Protocols;

typedef enum {
    BER_encoding,
    PER_encoding
} Pres_Encodings;

typedef enum {
    MMSd_short_stack,         /* Used only for 3 layer (UCA Reduced Stack) operation */
    MMSd_full_stack,          /* Normal UCA Upper Layer stack */
    MMSd_fastbyte_stack,      /* Uses ISO FastByte encoding of upper layers */
    MMSd_no_stack             /* No stack */
} MMSd_Conn_Type;

#define MAX_PRES_CTX 5

typedef struct Pres_data {
    Pres_ctx    pres_ctx[MAX_PRES_CTX];
    short       pres_num_ctx;
    Pres_ctx    pres_default_context;
    short       pres_mms_ctx;
    short       pres_acse_ctx;
    short       pres_stase_mms_ctx;
    short       pres_def_ctx_present;
    short       pres_refuse_reason;
    MMSd_Conn_Type  conn_type;
} Pres_data;

MMSd_length Presentation_abort(MMSd_context *mms_cntxt,  MMSd_descriptor *E);

#endif

#define TMW_MAX_STACK_ADDRESS       50
#define TMW_MAX_STACK_OBJ_ID_SIZE   10

#define MMSD_T_SEL_CALLING      0x0001U
#define MMSD_S_SEL_CALLING      0x0002U
#define MMSD_P_SEL_CALLING      0x0004U
#define MMSD_AP_TTL_CALLING     0x0008U
#define MMSD_AE_QUA_CALLING     0x0010U
#define MMSD_AP_INV_CALLING     0x0020U
#define MMSD_AE_INV_CALLING     0x0040U
#define MMSD_T_SEL_CALLED       0x0080U
#define MMSD_S_SEL_CALLED       0x0100U
#define MMSD_P_SEL_CALLED       0x0200U
#define MMSD_AP_TTL_CALLED      0x0400U
#define MMSD_AE_QUA_CALLED      0x0800U
#define MMSD_AP_INV_CALLED      0x1000U
#define MMSD_AE_INV_CALLED      0x2000U
#define MMSD_PASSWORD           0x4000U
#define MMSD_MECHANISM          0x8000U

typedef struct TMW_Stack_Address {
    unsigned char len;
    unsigned char addr[TMW_MAX_STACK_ADDRESS];
} TMW_Stack_Address;

typedef struct MMSd_Stack_Addresses {
    unsigned int            inclusion;
    TMW_Stack_Address       t_sel_called;
    TMW_Stack_Address       s_sel_called;       /* Used in response */
    TMW_Stack_Address       p_sel_called;       /* Used in response */
    int                     ap_title_called[TMW_MAX_STACK_OBJ_ID_SIZE];
    int                     ap_inv_id_called;
    int                     ae_qual_called;
    int                     ae_inv_id_called;

    TMW_Stack_Address       t_sel_calling;
    TMW_Stack_Address       s_sel_calling;
    TMW_Stack_Address       p_sel_calling;
    int                     ap_title_calling[TMW_MAX_STACK_OBJ_ID_SIZE];
    int                     ap_inv_id_calling;
    int                     ae_qual_calling;
    int                     ae_inv_id_calling;

    int                     port;

    TMW_Stack_Address       password;
    TMW_Stack_Address       mechanism;
} MMSd_Stack_Addresses;

#define P_ADDR( x ) ((MMSd_Connection *) (_CNTX->connection))->Stack_Addresses.x
#define A_ADDR( x ) ((MMSd_Connection *) (_CNTX->connection))->Stack_Addresses.x
#define CLEAR_STACK_ADDRESSES( ctx ) { memset( &((MMSd_Connection *) (ctx)->connection)->Stack_Addresses, \
                                        0, \
                                        sizeof( ((MMSd_Connection *) (ctx)->connection)->Stack_Addresses ) ); }

typedef struct MMSd_Connection *MMSd_ConnectionPtr;

typedef struct MMSd_OpenFileEntry {
 // ltts   FILE                *fd;
    MMSd_ConnectionPtr  pMmsConnection;
} MMSd_OpenFileEntry;

#define MMSD_MAX_OBTAIN_FILE_NAME               1024
#define MMSD_MAX_OBTAIN_FILE_TRANSFERS            10
#define MMSD_OBTAIN_FILE_OPEN_BUFFER_SIZE       1024
#define MMSD_OBTAIN_FILE_READ_BUFFER_SIZE       1024
#define MMSD_OBTAIN_FILE_CLOSE_BUFFER_SIZE      1024

typedef enum { MMSd_ObtainFile_Idle,
               MMSd_ObtainFile_OpenRequest,
               MMSd_ObtainFile_OpenRequestPending,
               MMSd_ObtainFile_ReadRequest,
               MMSd_ObtainFile_ReadRequestPending,
               MMSd_ObtainFile_CloseRequest,
               MMSd_ObtainFile_CloseRequestPending,
               MMSd_ObtainFile_CloseComplete,
               MMSd_ObtainFile_SendError,
               MMSd_ObtainFile_SendConfirmedError
             } MMSd_ObtainFileState;

typedef enum { MMSd_obtainFileNoError,
               MMSd_obtainFileSourceError,
               MMSd_obtainFileDestError,
               MMSd_obtainFileConnectionError,
               MMSd_obtainFileStatusError,
               MMSd_obtainFileEncodeError,
               MMSd_obtainFileDecodeError,
               MMSd_obtainFileFlowControlled,
               MMSd_obtainFileSendError
             } MMSd_ObtainFileError;

typedef struct MMSd_ObtainFileStatus {
    char                    sourceFileName[ MMSD_MAX_OBTAIN_FILE_NAME + 1 ];
    char                    destFileName[ MMSD_MAX_OBTAIN_FILE_NAME + 1 ];
 //! \todo    FILE                    *pDestFile;
    MMSd_ObtainFileState    nState;
    unsigned long           originalInvokeId;
    unsigned long           invokeId;
    long                    remoteFd;
    MMSd_time               timeStamp;
    int                     obtainFileError;
    int                     obtainFileAdditionalInfo;
} MMSd_ObtainFileStatus;

#define MMSD_MAX_SIGNATURE_LEN          1000
#define MMSD_MAX_CERTIFICATE_LEN        8192
#define MMSD_MAX_GENERALIZED_TIME_LEN    100

/* This callback is invoked when ACSE security requires a security check.  The X509 signed certificate
   must be validated (using its contents and its signature, which was presumably done by a Certificate
   Authority).  The public key and mechanism within the certificate must then be used to validate the
   ACSE signature, which was generated using the ASCII text Timestamp which is also passed.

   It is called on the server side (via the ServerInterfaceTable) when the server receives an AARQ
   (incoming Associate Request) to validate the requesting client.  It is also called on the client
   side (via the TMW_UL_Server data structure) it validate the server's AARE (response). */

typedef int (* TMW_CheckSignature)( unsigned char   *pX509_encoded_signed_certificate,  /* Input */
                                    int             nCertificateLength,                 /* Input */
                                    char            *pTimeStamp,                        /* Input */
                                    unsigned char   *pSignature,                        /* Input */
                                    int             nSignatureLength,                   /* Input */
                                    void            *pCryptoUserContext,                /* User field */
                                    void            *pCryptoUserData );                 /* User field */

/* This callback is invoked when the ACSE security mechanism requires the generation of security
   parameters.  The signed DER encoded X509 certificate must be copied into the buffer (maximum length
   8192), and the signature must be generated using the ASCII text Timestamp.  The pCryptoUSerData can
   be any pointer that the user may need to generate the signature.

   It is called on the server side (via the ServerInterfaceTable) when the server is encoding an AARE
   (outgoing Associate Resoponse) to be validated by the requesting client.  It is also called on the
   client side (via the TMW_UL_Server data structure) generate the security parameters when sending an
   AARQ (outgoing Associate Request). */

typedef int (* TMW_MakeSignature)( unsigned char   *pX509_encoded_signed_certificate,  /* Output buffer */
                                   int             *nCertificateLength,                /* In: cert len, Out: cert len */
                                   char            *pTimeStamp,                        /* Input */
                                   unsigned char   *pSignature,                        /* Output buffer */
                                   int             *nSignatureLength,                  /* In: sig len, Out: sig len */
                                   void            *pCryptoUserContext,                /* User field */
                                   void            *pCryptoUserData );                 /* User field */

/* --- MMSd_Connection record
   --- This record type holds all info regarding the state of
   --- the association
   ------------------------------------------------------------
*/

typedef struct MMSd_Connection {
    int             state;
    int             outstandingRequests;
    int             loading;
    MMSd_InitiatePars neg;
    void           *clientData;
#ifdef MMSD_FULL_STACK
    Pres_data       presentation;
#endif
    int             auth_level;
    int             authenticationType;
    TMW_MakeSignature  makeSignatureFcn;
    TMW_CheckSignature checkSignatureFcn;
    void               *pCryptoUserData;
    void               *pCryptoUserContext;
    void           *dls_list;
#if defined(MMSD_USE_TSDU_SIZE)
    unsigned int    max_tsdu_size;
#endif
    void           *tpConnection;
    MMSd_context   *pMmsContext;
#define USE_MMSD_TP_CONN 1
#if ( defined(MMSD_PSEUDO_AA_RCBS) || defined(MMSD_MAX_AA_NVLS) )
    directoryType   aaDomain;               /* aa-specific domain image */
#endif
#if defined(MMSD_MAX_AA_NVLS)
    NV_type         aaNVLs[MMSD_MAX_AA_NVLS];
    dictionaryIndex aaIndices[MMSD_MAX_AA_NVLS];
    MMSd_NvlEval    aaNvlEvals[MMSD_MAX_AA_NVLS];
    MMSd_ConnectionPtr pNextAA;
#endif
#if defined(MMSD_ADDR_INTERFACE)
    MMSd_Stack_Addresses Stack_Addresses;
#endif
#if defined(MMSD_MAX_OPEN_FILES)
    MMSd_OpenFileEntry openFiles[ MMSD_MAX_OPEN_FILES ];
#endif
    MMSd_ObtainFileStatus   obtainFileStatus[ MMSD_MAX_OBTAIN_FILE_TRANSFERS ];
    unsigned long           invokeId;
} MMSd_Connection;

/* From service.c : */
void MMSd_initializeConnection( MMSd_context *ctx, MMSd_Connection *conn );
void MMSd_cleanupConnection( MMSd_context *ctx, MMSd_Connection *conn );


TMW_CLIB_API int TMW_Stack_Address2Hex(TMW_Stack_Address *TMW_addr, char *buffer, int len);


/* --- Access macro for the MMSd_Connection record in (* _CNTX)
   ------------------------------------------------------------
*/
#define CONNECTION   (*((MMSd_Connection *) _CNTX->connection))
#ifdef MMSD_FULL_STACK
#define PRESENTATION   ((*((MMSd_Connection *)_CNTX->connection)).presentation)
#endif


#ifdef __cplusplus
};
#endif

#endif /* _CONNECT_H */

