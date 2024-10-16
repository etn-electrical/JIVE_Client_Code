/**
 *      @file    matrixCmsApi.h
 *
 *
 *      Prototypes for the Matrix CMS public APIs.
 */
/*
 *      Copyright (c) 2013-2017 INSIDE Secure Corporation
 *      All Rights Reserved
 *
 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF INSIDE.
 *
 *      Please do not edit this file without first consulting INSIDE support.
 *      Unauthorized changes to this file are not supported by INSIDE.
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 *
 *      This Module contains Proprietary Information of INSIDE and should be
 *      treated as Confidential.
 *
 *      The information in this file is provided for the exclusive use of the
 *      licensees of INSIDE. Such users have the right to use, modify,
 *      and incorporate this code into products for purposes authorized by the
 *      license agreement provided they include this notice and the associated
 *      copyright notice with any such product.
 *
 *      The information in this file is provided "AS IS" without warranty.
 */
/******************************************************************************/

#ifndef _h_PS_CMSAPI
# define _h_PS_CMSAPI

# if defined __cplusplus && !defined MATRIXCMS_API_NO_EXTERN_C
extern "C" {
# endif

# include "matrixCms.h"

# ifdef USE_CMS

/******************************************************************************/
/*      Public APIs exposed based on settings in matrixCmsConfig.h that gives
    flexibility to which CMS data types are supported and whether the
    creation and/or parsing halves of those types are needed

    No edits should be made to this file
 */


/*********************** Authenticated Enveloped Data *************************/
/*****
 *      AED CREATION
 */

/* Atomic creation */
#  ifdef USE_MCMS_ATOMIC_AED_CREATE
PSPUBLIC int32 matrixCmsCreateAuthEnvData(psPool_t *pool,
                                          const psX509Cert_t *myCert,
                                          const psPubKey_t *privKey,
                                          const psX509Cert_t *recipientCert,
                                          const int32 keyMethod,
                                          const int32 encryptMethod,
                                          const int32 wrapMethod,
                                          const int32 keyAgreeScheme,
                                          unsigned char *content,
                                          const int32 contentLen,
                                          const int32 contentType,
                                          unsigned char **outputBuf,
                                          int32 *outputLen,
                                          const int32 flags);
PSPUBLIC int32 matrixCmsCreateAuthEnvDataExt(
	psPool_t *pool,
        const psX509Cert_t *myCert,
        const psPubKey_t *privKey,
        const uint32 recipients,
        const psX509Cert_t **recipientCerts,
        const int32 keyMethod,
        const int32 encryptMethod,
        const int32 wrapMethod,
        const int32 keyAgreeScheme,
        unsigned char *content,
        const int32 contentLen,
        const int32 contentType,
        unsigned char **outputBuf,
        int32 *outputLen,
        matrixCmsCreateAuthEnvDataExtInfo_t *ei);
#  endif /* Atomic */

/* Streaming creation */
#  ifdef USE_MCMS_STREAMING_AED_CREATE
PSPUBLIC int32 matrixCmsInitCreateAuthEnvData(psPool_t *pool,
                                              const psX509Cert_t *myCert,
                                              const psPubKey_t *privKey,
                                              const psX509Cert_t *recipientCert,
                                              const int32 keyMethod,
                                              const int32 encryptMethod,
                                              const int32 wrapMethod,
                                              const int32 keyAgreeScheme,
                                              const int32 contentType,
                                              unsigned char **out,
                                              int32 *outLen,
                                              const int32 flags,
                                              cmsAuthEnvelopedData_t **aedOut);
PSPUBLIC int32 matrixCmsUpdateCreateAuthEnvData(psPool_t *pool,
                                                cmsAuthEnvelopedData_t *aedCtx,
                                                unsigned char *dataIn,
                                                const int32 dataInLen,
                                                unsigned char **out,
                                                int32 *outLen);
PSPUBLIC int32 matrixCmsFinalCreateAuthEnvData(psPool_t *pool,
                                               cmsAuthEnvelopedData_t *aedCtx,
                                               unsigned char **out, int32 *outLen);
PSPUBLIC void matrixCmsFreeStreamCreatedAuthEnvData(cmsAuthEnvelopedData_t *aed);
#  endif /* Streaming */



/*****
 *      AED PARSING
 */

/* Atomic parse */
#  ifdef USE_MCMS_ATOMIC_AED_PARSE
PSPUBLIC int32 matrixCmsParseAuthEnvData(psPool_t *pool,
                                         unsigned char *buf,
                                         const uint32 bufLen,
                                         const psX509Cert_t *originatorCert,
                                         const psPubKey_t *privKey,
                                         const int32 flags,
                                         unsigned char **data,
                                         int32 *dataLen,
                                         cmsEncryptedEnvelope_t **authData);

#   ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 matrixCmsParseAuthEnvDataFile(psPool_t *pool, char *file,
                                             cmsEncryptedEnvelope_t **authData);
#   endif /* MATRIX_USE_FILE_SYSTEM */
#  endif  /* Atomic */

/* Stream parse */
#  ifdef USE_MCMS_STREAMING_AED_PARSE
PSPUBLIC int32 matrixCmsInitParseAuthEnvData(psPool_t *pool,
                                             unsigned char *buf,
                                             const uint32 bufLen,
                                             cmsEncryptedEnvelope_t **eeCtx,
                                             unsigned char **remainder,
                                             int32 *remainderLen, int32 flags);
PSPUBLIC int32 matrixCmsPostInitParseAuthEnvData(psPool_t *pool,
                                                 cmsEncryptedEnvelope_t *ee,
                                                 const psX509Cert_t *originatorCert,
                                                 const psPubKey_t *privKey);
PSPUBLIC int32 matrixCmsUpdateParseAuthEnvData(psPool_t *pool,
                                               unsigned char *buf, uint32 bufLen, cmsEncryptedEnvelope_t *eeCtx,
                                               unsigned char **data, int32 *dataLen, uint32 dataSize,
                                               unsigned char **remainder, int32 *remainderLen);

PSPUBLIC int32 matrixCmsFinalParseAuthEnvData(psPool_t *pool,
                                              unsigned char *buf, uint32 bufLen, cmsEncryptedEnvelope_t *eeCtx);
#  endif /* Streaming */


PSPUBLIC void matrixCmsFreeParsedAuthEnvData(cmsEncryptedEnvelope_t *eeCtx);


/******************************** Signed Data *********************************/
/*****
 * SignedData PARSING
 */

/* Atomic parse */
#  ifdef USE_MCMS_ATOMIC_SD_PARSE
PSPUBLIC int32 matrixCmsParseSignedData(psPool_t *pool, unsigned char *sdBuf,
                                        uint32 sdBufLen, cmsSignedData_t **signedData, int32 flags);
PSPUBLIC int32 matrixCmsConfirmSignature(psPool_t *pool,
                                         cmsSignedData_t *signedData, unsigned char *data,
                                         int32 dataLen, psX509Cert_t *validationCert);

#   ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 matrixCmsParseSignedDataFile(psPool_t *pool, char *file,
                                            cmsSignedData_t **signedData);
#   endif
#  endif /* USE_MCMS_ATOMIC_SD_PARSE */

/* Streaming parse */
#  ifdef USE_MCMS_STREAMING_SD_PARSE
PSPUBLIC int32 matrixCmsInitParseSignedData(psPool_t *pool,
                                            unsigned char *sdBuf, uint32 sdBufLen,
                                            cmsSignedData_t **signedData, unsigned char **remainder,
                                            uint32 *remainderLen, int32 flags);
PSPUBLIC int32 matrixCmsUpdateParseSignedData(psPool_t *pool,
                                              cmsSignedData_t *signedData, unsigned char *sdBuf,
                                              uint32 sdBufLen, unsigned char **data, uint32 *dataLen,
                                              unsigned char **remainder, uint32 *remainderLen);
PSPUBLIC int32 matrixCmsFinalParseSignedData(psPool_t *pool,
                                             cmsSignedData_t *signedData, const unsigned char *hash,
                                             uint32 hashLen, psX509Cert_t *validationCert);
#  endif /* USE_MCMS_STREAMING_SD_PARSE */

/* Cleanup */
PSPUBLIC void matrixCmsFreeParsedSignedData(cmsSignedData_t *signedData);



/*****
 * SignedData CREATION
 */
/* Atomic creation */
#  ifdef USE_MCMS_ATOMIC_SD_CREATE
PSPUBLIC int32 matrixCmsCreateSignedData(psPool_t *pool, unsigned char *content,
                                         int32 contentLen, int32 contentType, psX509Cert_t *cert,
                                         psPubKey_t *key, int32 hashId, unsigned char **outputBuf,
                                         int32 *outputLen, int32 flags);
PSPUBLIC int32 matrixCmsCreateSignedDataExt(
        psPool_t *pool,
        const unsigned char *content,
        int32 contentLen,
        int32 contentType,
        int32 count,
        psX509Cert_t * const *certs,
        psPubKey_t * const *keys,
        const int32 * const hashIds,
        unsigned char **outputBuf,
        int32 *outputLen,
        struct matrixCmsCreateSignedDataExtInfo *ei_p);
#   ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 matrixCmsCreateSdFromContentFile(psPool_t *pool,
                                                char *contentFile, char *certFile, char *keyFile, char *password,
                                                int32 hashId, unsigned char **outputBuf, int32 *outputLen,
                                                int32 flags);
#   endif
#  endif


/* Streaming creation */
#  ifdef USE_MCMS_STREAMING_SD_CREATE
PSPUBLIC int32 matrixCmsInitCreateSignedData(psPool_t *pool, psX509Cert_t *cert,
                                             psPubKey_t *key, int32 hashId, int32 contentType,
                                             unsigned char **outputBuf, int32 *outputLen, int32 flags,
                                             cmsSdStream_t **sdCtx);
PSPUBLIC int32 matrixCmsUpdateCreateSignedData(psPool_t *pool,
                                               cmsSdStream_t *sdCtx, unsigned char *content, int32 contentLen,
                                               unsigned char **outputBuf, int32 *outputLen);
PSPUBLIC int32 matrixCmsFinalCreateSignedData(psPool_t *pool,
                                              cmsSdStream_t *sdCtx, unsigned char **outputBuf,
                                              int32 *outputLen);
PSPUBLIC void matrixCmsFreeStreamCreatedSignedData(cmsSdStream_t *sdCtx);
#  endif


/******************************* Compressed Data ******************************/
/*****
 * CompressedData PARSING
 */

/* Atomic parse */
#  ifdef USE_MCMS_ATOMIC_CD_PARSE
PSPUBLIC int32 matrixCmsParseCompressedData(psPool_t *pool,
                                            unsigned char *cdBuf, uint32 cdBufLen,
                                            cmsCompressedData_t **retCompressedData, int32 flags);
#   ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 matrixCmsParseCompressedDataFile(psPool_t *pool, char *file,
                                                cmsCompressedData_t **compressedData);
#   endif
#  endif

/* Streaming parse */
#  ifdef USE_MCMS_STREAMING_CD_PARSE
/* NEW ONES */
PSPUBLIC int32 matrixCmsInitParseCompressedData(psPool_t *pool,
                                                unsigned char *cdBuf, uint32 cdBufLen,
                                                cmsCompressedData_t **retCompressedData, unsigned char **compressedBuf,
                                                int32 *compressedBufLen, int32 flags);
PSPUBLIC int32 matrixCmsUpdateParseCompressedData(cmsCompressedData_t *ctx,
                                                  unsigned char *cdBuf, uint32 cdBufLen, unsigned char **compressedBuf,
                                                  int32 *compressedBufLen, unsigned char **remainder,
                                                  int32 *remainderLen);
#  endif

PSPUBLIC void matrixCmsFreeCompressedData(cmsCompressedData_t *compressedData);

/*****
 * CompressedData CREATION
 */

/* Atomic creation */
#  ifdef USE_MCMS_ATOMIC_CD_CREATE
PSPUBLIC int32 matrixCmsCreateCompressedData(psPool_t *pool,
                                             unsigned char *compressedData, int32 compressedDataLen,
                                             unsigned char **compressedDataType, int32 *compressedDataTypeLen,
                                             int32 flags);
#  endif

/* Streaming creation */
#  ifdef USE_MCMS_STREAMING_CD_CREATE
PSPUBLIC int32 matrixCmsInitCreateCompressedData(psPool_t *pool,
                                                 unsigned char **dataOut, int32 *dataOutLen, int32 flags);
PSPUBLIC int32 matrixCmsUpdateCreateCompressedData(psPool_t *pool,
                                                   unsigned char *compressedIn, int32 compressedInLen,
                                                   unsigned char **dataOut, int32 *dataOutLen);
PSPUBLIC int32 matrixCmsFinalCreateCompressedData(psPool_t *pool,
                                                  unsigned char **dataOut, int32 *dataOutLen, int32 flags);
#  endif

/****************************** Enveloped Data ********************************/
#  ifdef USE_MCMS_ENVELOPED_DATA_PARSE
#   ifdef MATRIX_USE_FILE_SYSTEM
PSPUBLIC int32 matrixCmsParseEnvelopedDataFile(psPool_t *pool, char *file,
                                               cmsEncryptedEnvelope_t **signedData);
#   endif
PSPUBLIC int32 matrixCmsParseEnvelopedDataBuf(psPool_t *pool,
                                              unsigned char *sdBuf, uint32 sdBufLen,
                                              cmsEncryptedEnvelope_t **retee);
#  endif /* USE_MCMS_ENVELOPED_DATA_PARSE */

# endif  /* USE_CMS */

# if defined __cplusplus && !defined MATRIXCMS_API_NO_EXTERN_C
}
# endif

#endif   /* _h_PS_CMSAPI */
