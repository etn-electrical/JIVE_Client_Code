/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1998-2014 */
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

/*
 *
 *  This file contains
 *  Interface to iec61850 freeRTOS ethernet.
 *
 */

#ifndef _IEC61850_ETH_H
#define _IEC61850_ETH_H

#ifdef __cplusplus
extern "C" {
#endif

/* If there is only Goose (IEC or UCA-GSSE) protocols used, the packets are never queued. The
 buffering is only used for OSI or the old UCA time synchronization, which are not part of
 61850.  This means that we don't need very many packet packs, since there is no overlapped IO
 as in Windows. */

#define NUM_PACKET_PACKS              4

/* Input packet as it appears on input queues */
typedef struct ETH_Read_Packet {
    unsigned char *buffer;
    void *bufferId;
    int raw_len;
    TMW_Time time_in;
} ETH_Read_Packet;

/* Our Linux version of address binding */
typedef struct LNXETH_Binding {
    TMW_DL_Binding gnrc;
} LNXETH_Binding;

/* Basic packet queue entry */
typedef struct ETH_QUEUE {
    ETH_Read_Packet read_packets[ NUM_PACKET_PACKS];
    int ins_pkt_index;
    int rem_pkt_index;
    int pkt_queue_len;
} ETH_QUEUE;

typedef struct LNXETH_Context *LNXETH_ContextPtr;

typedef struct LNXETH_Context {
    TMW_DL_Context gnrc;
    void *ind_data;
    ETH_QUEUE eth_queue;
    int socket;
    int if_index;

} LNXETH_Context;

#ifdef __cplusplus
}
;
#endif

#endif /* _IEC61850_ETH_H */
