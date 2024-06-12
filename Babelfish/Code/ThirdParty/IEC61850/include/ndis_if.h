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
 *  Declarations for NDisHook (WinDis) drivers .
 *
 *  This file will not require modification.
 */

#ifndef NDIS_IF_H
#define NDIS_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Note
 -------
 * Increasing NUM_PACKET_PACKS beyond 64 is not possible. The wait function
 * WaitForMultipleObjects() can wait on a maximum of MAXIMUM_WAIT_OBJECTS,
 * defined to be 64 in WINNT.H. An alternate design must be developed to
 * pass additional resources to the driver.
 */
#define NUM_PACKET_PACKS              62
#define NUM_MULTI_ADDRS               75
#define MAX_ETH_CONTEXTS              4

#define TMW_ETHER_ADDR_LENGTH   6
#define TMW_ETHER_TYPE_LENGTH   2
#define TMW_MAX_802_3_LENGTH    1500
#define TMW_MAX_ETHER_SIZE      1514
#define TMW_MIN_ETHER_SIZE      60
#define TMW_MDstAddr    0
#define TMW_MSrcAddr    TMW_ETHER_ADDR_LENGTH
#define TMW_MLength (TMW_MSrcAddr + TMW_ETHER_ADDR_LENGTH)
#define TMW_MHdrSize (TMW_MLength + TMW_ETHER_TYPE_LENGTH )

typedef struct ETH_Read_Packet {
    unsigned char *buffer;
    void *bufferId;
    int raw_len;
    LARGE_INTEGER raw_time;
} ETH_Read_Packet;

typedef struct ETH_QUEUE {
    ETH_Read_Packet read_packets[NUM_PACKET_PACKS];
    int ins_pkt_index;
    int rem_pkt_index;
    int pkt_queue_len;
} ETH_QUEUE;

typedef int (*NDIS_Indication)(TMW_DL_Context *eth_context, void *packetBufferId, unsigned char *packetBuffer, unsigned int packetLength, LARGE_INTEGER packetTime, LARGE_INTEGER packetSequence,
                               void *ind_data);

#define     DISABLE_LOOPBACK    1

typedef unsigned char NDIS_BOOL;

void NDIS_InitializeContext(TMW_DL_Context *dl_context);

int NDIS_OpenAdapter(TMW_DL_Context *eth_context, NDIS_Indication ind_function, void *ind_data);

void NDIS_CloseAdapter(TMW_DL_Context *eth_context);

NDIS_BOOL NDIS_GetAdapterInformation(TMW_DL_Context *eth_context);

NDIS_BOOL NDIS_SetPacketFilter(TMW_DL_Context *dl_context, ULONG nPacketFilter);

#if DISABLE_LOOPBACK
TMW_CLIB_API NDIS_BOOL NDIS_DisableLoopback(TMW_DL_Context *dl_context);

TMW_CLIB_API NDIS_BOOL NDIS_EnableLoopback(TMW_DL_Context *dl_context);
#endif

NDIS_BOOL NDIS_PostReadRequests(TMW_DL_Context *dl_context);

NDIS_BOOL NDIS_StartReception(TMW_DL_Context *dl_context);

NDIS_BOOL NDIS_StopReception(TMW_DL_Context *dl_context);

int NDIS_service(TMW_DL_Context *dl_context);

int NDIS_send(unsigned char *buffer, int offset, int length, TMW_Mac_Address *dstMac, TMW_DL_Context *eth_context, TMW_Media media);

#define ETH_USE_SEND_APC
#if defined(ETH_USE_SEND_APC)
typedef void (*NDIS_SendCompleteFcn)(TMW_DL_Context *pContext, void *pIoCompletionData, int nBytesReturned);

int NDIS_sendEx(unsigned char *buffer, int offset, int length, TMW_Mac_Address *dstMac, TMW_DL_Context *eth_context, TMW_Media media, NDIS_SendCompleteFcn ioCompletion,
                         void *pIoCompletionData);
#endif

NDIS_BOOL NDIS_AddMulticastAddress(TMW_DL_Context *dl_context, unsigned char *pMacAddress);

NDIS_BOOL NDIS_RemoveMulticastAddress(TMW_DL_Context *dl_context, unsigned char *pMacAddress);

TMW_DL_Context *NDIS_find_context(char *adapter_name);

TMW_DL_Context *NDIS_new_context(char *adapter_name);

void NDIS_free_context(TMW_DL_Context *freeCtx);

HANDLE *NDIS_MakeHandles(TMW_DL_Context *dl_context);

int NDIS_WaitForNetworkEvents(HANDLE *PackageHandles, unsigned long timeout);

void NDIS_init_queue(TMW_DL_Context *dl_context);

int NDIS_insert_queue(ETH_Read_Packet *read_packet, TMW_DL_Context *dl_context);

ETH_Read_Packet *NDIS_remove_queue(TMW_DL_Context *dl_context);

/* This is not required for normal operation.  It is used  strictly for the TMW test client to do browsing
 of GOOSE messages.  This shouldn't be needed for any  production systems. */
int NDIS_SetPromiscuous(TMW_DL_Context *dl_context);

int NDIS_ClearPromiscuous(TMW_DL_Context *dl_context);

int NDIS_adapter_dialog(char *prompt, char *adapter);

int NDIS_adapter_select(char *selectString, char *adapter);

TMW_CLIB_API HANDLE NDIS_find_first_adapter(char *name, char *desc);

TMW_CLIB_API int NDIS_find_next_adapter(HANDLE enum_handler, char *name, char *desc);

void NDIS_FreeHandles(HANDLE *NDIShandle);

void NDIS_WakeSendThread(TMW_DL_Context *cntxt);

void NDIS_lockContextSet(void);

void NDIS_unlockContextSet(void);

#ifdef __cplusplus
};
#endif

#endif /* NDIS_IF_H */
