//*****************************************************************************/
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

/** @eth.h
 *
 *  This file contains
 *  Declarations for Ethernet services.
 *
 *  This file should not require modification.
 */

#ifndef ETH_H
#define ETH_H



#ifdef __cplusplus
extern "C" {
#endif
#include "tam.h"  
  
#define ETH_size 1517
#define OSI_LSAP 0xFE
#define UCA_LSAP 0xFB
#define ETHER_TICKS_PER_SEC  1000
#define ETH_Context TMW_DL_Context

ETH_Context *ETH_open_port(char *adapter_name);

int ETH_close_port(ETH_Context *context);

int ETH_bind(ETH_Context *eth_context, TMW_DL_Bind_Type bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);

int ETH_unbind(ETH_Context *eth_context, TMW_DL_Bind_Type bind_type, TMW_Mac_Address *multi_addr, int lsap, TMW_DL_Ind ind_function, void *ind_param);

int ETH_service(ETH_Context *context);

int ETH_can_send(int length, ETH_Context *context);

int ETH_send(TMW_Packet *packet, TMW_Mac_Address *mac, ETH_Context *ctx);

int ETH_receive(TMW_Packet *packet, TMW_Mac_Address *mac, ETH_Context *ctx);

int ETH_send_mgmt(TMW_Packet *packet, TMW_Mac_Address *mac, TMW_DL_Context *ctx, MMSd_time *outTime);

int ETH_find_next_adapter(unsigned char *name, unsigned char *desc);

TMW_CLIB_API int ETH_adapter_dialog(const char *prompt, char *adapter);  //FIXME: move to targ

#ifdef __cplusplus
}
;
#endif

#endif /* ETH_H */
