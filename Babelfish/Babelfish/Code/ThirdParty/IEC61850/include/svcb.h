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
 *  Declarations for Sample Values Control Blocks.
 *
 *  This file should not require modification.
 */

#ifndef _SVBC_H
#define _SVBC_H

/*
 * Functions for application access to SVCBs
 ******************************************************************************
 */
char * TMW_SAV_SvControlGetSvCbName(void *handle);

void * TMW_SAV_SvControlGetHandle(MMSd_context *pMmsContext, char *SvCbName);

void * TMW_SAV_SvControlGetContext(void *handle);

int TMW_SAV_SvControlSetContext(void *handle, void *pContext);

TMW_CLIB_API int TMW_SAV_SvControlGetSvEna(void *handle, unsigned char *pEna);

TMW_CLIB_API int TMW_SAV_SvControlSetSvEna(void *handle, int newState);

TMW_CLIB_API int TMW_SAV_SvControlGetConfRev(void *handle, unsigned long *pConfRev);

TMW_CLIB_API int TMW_SAV_SvControlSetConfRev(void *handle, unsigned long confRev);

TMW_CLIB_API int TMW_SAV_SvControlGetDataSet(void *handle, char *DatSet);

TMW_CLIB_API int TMW_SAV_SvControlSetDataSet(void *handle, char *DatSet);

int TMW_SAV_SvControlGetLanPars(void *handle, TMW_Mac_Address *pAddress, unsigned int *VLAN_AppID,
                                unsigned int *VLAN_Vid, unsigned char *VLAN_Priority);

int TMW_SAV_SvControlSetLanPars(void *handle, TMW_Mac_Address *pAddress, unsigned int VLAN_AppID, unsigned int VLAN_Vid,
                                unsigned char VLAN_Priority);

TMW_CLIB_API int TMW_SAV_SvControlGetSmpRate(void *handle, int *pSmpRate);

TMW_CLIB_API int TMW_SAV_SvControlSetSmpRate(void *handle, int smpRate);

TMW_CLIB_API int TMW_SAV_SvControlGetSmpMod(void *handle, int *pSmpMod);

TMW_CLIB_API int TMW_SAV_SvControlSetSmpMod(void *handle, int smpMod);

int TMW_SAV_SvControlGetOptFlds(void *handle, unsigned char *pOptFlds);

int TMW_SAV_SvControlSetOptFlds(void *handle, unsigned char optFlds);

TMW_CLIB_API int TMW_SAV_SvControlGetNoASDUs(void *handle, unsigned int *pNoAsdus);

TMW_CLIB_API int TMW_SAV_SvControlSetNoASDUs(void *handle, unsigned int noASDU);

int TMW_SAV_SvControlSetStartStop(void *handle, TMW_SAV_Start start_call_back, void *start_call_back_data, TMW_SAV_Stop stop_call_back, void *stop_call_back_data);

int TMW_SAV_SvControlStart(MMSd_context *pMmsContext, void *handle);

int TMW_SAV_SvControlStop(MMSd_context *pMmsContext, void *handle);

void TMW_SAV_SvControlStartAll(MMSd_context *pMmsContext);

void TMW_SAV_SvControlStopAll(MMSd_context *pMmsContext);

void TMW_SAV_SvControlSetAllContext(MMSd_context *pMmsContext, void *pContext);

void TMW_SAV_SvControlSetAllStartStop(MMSd_context *pMmsContext, TMW_SAV_Start start_call_back_data, void *start_call_back, TMW_SAV_Stop stop_call_back, void *stop_call_back_data);

int TMW_SAV_SvControlCreate(TMW_SAV_Context *pSvContext, void *handle);

int TMW_SAV_SvControlCreateAll(MMSd_context *pMmsContext, TMW_SAV_Context *pSvContext);

int TMW_SAV_SvControlDelete(TMW_SAV_Context *pSvContext, void *handle);

int TMW_SAV_SvControlDeleteAll(MMSd_context *pMmsContext, TMW_SAV_Context *pSvContext);



#endif  /* _SVBC_H */
