#ifndef _SPP_H
#define _SPP_H

#ifdef __cplusplus
extern "C" {
#endif

#define TMW_SPP_MAX_UDP_FRAME  2048

typedef enum {
    TMW_SPP_ERR_Success,
    TMW_SPP_ERR_Parameters,
    TMW_SPP_ERR_ParseFailed,
    TMW_SPP_ERR_SocketFailed,
    TMW_SPP_ERR_SignatureFailed,
    TMW_SPP_ERR_TransmitFailed
}
TMW_SPP_ERROR;

typedef unsigned char TMW_SPP_SPDU_type;

/* ASN.1 Tags used: */
/* TMW_SPP_SPDU_type */
#define TMW_SPP_SPDU_Tunnel         0xA0
#define TMW_SPP_SPDU_Goose          0xA1
#define TMW_SPP_SPDU_SAV            0xA2
#define TMW_SPP_SPDU_Mgmt           0xA3

/* Internal tags */
#define TMW_SPP_TPDU_UD             0x40
#define TMW_SPP_SPDU_Header         0x80
#define TMW_SPP_SPDU_Payload_Tunnel 0x80
#define TMW_SPP_SPDU_Payload_Goose  0x81
#define TMW_SPP_SPDU_Payload_SAV    0x82
#define TMW_SPP_SPDU_Payload_Mgmt   0x83
#define TMW_SPP_SPDU_Signature      0x85

#define TMW_SPP_SPDU_Version        0x0001

typedef enum {  TMW_SPP_Enc_None,
                TMW_SPP_Enc_AES_128_GCM,
                TMW_SPP_Enc_AES_256_GCM
             } TMW_SPP_Encr;

typedef enum {  TMW_SPP_Sig_Alg_MAC_None,
                TMW_SPP_Sig_Alg_HMAC_SHA256_80,
                TMW_SPP_Sig_Alg_HMAC_SHA256_128,
                TMW_SPP_Sig_Alg_HMAC_SHA256_256,
                TMW_SPP_Sig_Alg_AES_GMAC_64,
                TMW_SPP_Sig_Alg_AES_GMAC_128
             } TMW_SPP_Hmac_Alg;

typedef struct TMW_SPP_SecurityAlgorithms {
    TMW_SPP_Encr        encryption;
    TMW_SPP_Hmac_Alg    hmac_algorithm;
} TMW_SPP_SecurityAlgorithms;

/* NOTE: The spec currently says to leave 100 octets for signature.
         If this ever gets increased to more than 127, the ASN.1
         encoding wrapper may need to be increased to have a two
         octet length field.
*/
#define TMW_SPDU_MAX_SIGNATURE  100

typedef int (* TMW_SPP_SAV_EncodeFunc)( MMSd_descriptor *pDescriptor, void *pApdu );

typedef int (* TMW_SPP_Goose_EncodeFunc)(   MMSd_descriptor *pDescriptor,
											void            *pHeader,
											unsigned char   *pData,
											int             nDataLen );

typedef int (* TMW_SPP_SAV_DecodeFunc)( MMSd_descriptor *pDescriptor, void *pApdu );

typedef int (* TMW_SPP_Goose_DecodeFunc)( MMSd_descriptor *pDescriptor, void *pHeader );

typedef struct TMW_SPP_SAV_Data {
    TMW_SPP_SAV_EncodeFunc      func;
    void                        *pApdu;
    TMW_SPP_SAV_DecodeFunc      decode_func;
} TMW_SPP_SAV_Data;

typedef struct TMW_SPP_Goose_Data {
    TMW_SPP_Goose_EncodeFunc    func;
    void                        *pHeader;
    void                        *pStatus;
    unsigned char               *pData;
    int                         nDataLen;
    TMW_SPP_Goose_DecodeFunc    decode_func;
} TMW_SPP_Goose_Data;

typedef struct TMW_SPP_Data {
    TMW_SPP_SPDU_type           spdu_type;
    unsigned char               simulation;
    int                         appID;
    unsigned int                spdu_number;
    int                         spdu_version;
    unsigned int                timeOfCurrentKey;
    int                         timeToNextKey;
    TMW_SPP_SecurityAlgorithms  securityAlgorithms;
    unsigned char               keyID[4];
    TMW_Address                 destAddress;
    int                         destPort;
    char					    srcAddr[16];
	TmwSocket_t                 sendSocket;
    unsigned char               bThreadRunning;
    unsigned char               bThreadStopping;
    unsigned char               bThreadStopped;
    int                         wakeSocket;
    union   {
        TMW_SPP_SAV_Data        sav;
        TMW_SPP_Goose_Data      goose;
    } data;
} TMW_SPP_Data;

int TMW_SPP_UDP_Create( TMW_SPP_Data        *pSppData,
                        TMW_SPP_SPDU_type   spdu_type,
                        TMW_Mac_Address     *destAddress,
                        int                 destPort,
                        unsigned char       simulation,
                        int                 appID );

int TMW_SPP_UDP_Delete( TMW_SPP_Data *pSppData );

int TMW_SPP_UDP_Subscribe( TMW_SPP_Data        *pSppData,
                           TMW_SPP_SPDU_type   spdu_type,
                           TMW_Mac_Address     *srcAddress,
                           int                 srcPort,
                           int                 appID );

int TMW_SPP_UDP_Unsubscribe( TMW_SPP_Data *pSppData );

int TMW_SPP_Encode( unsigned char   *buffer,
                    int             buffLen,
                    int             *data_offset_returned,
                    int             *data_length_returned,
                    TMW_SPP_Data    *pSppData );

int TMW_SPP_UDP_Send( unsigned char *pMessage, int nMessageLen, TMW_SPP_Data *pSppData );

TMW_SPP_ERROR TMW_SPP_Decode(   unsigned char   *buffer,
                                int             buffLen,
                                TMW_SPP_Data    *pDecodedData );

#ifdef __cplusplus
};
#endif

#endif /* _SPP_H */
