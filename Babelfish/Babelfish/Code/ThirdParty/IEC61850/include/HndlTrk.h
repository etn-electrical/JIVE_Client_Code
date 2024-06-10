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


/** @HndlXcb.h
 *
 * This file contains declarations for Handler Functions
 *
 * for 61850 Control Blocks
 *
 */
#ifndef HNDLTRK_H_
#define HNDLTRK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Handlers for Tracking
 * ******************************************************************** */
TMW_CLIB_API int   TMW_TRK_readRptEna(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readRptEna(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readConfRev(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readSqNum(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readGI(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readPurgeBuf(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readResv(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readResvTms(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readNdsCom(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readSmpRate(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readSmpMod(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readServiceParameter(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readRespAddCause(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readEntryID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readEntryTime(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readOptFlds(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readIntgPd(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readBufTm(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readTrgOps(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readRptID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readDatSet(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readLogRef(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readPhyComAddrAddr(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readPhyComAddrPRIORITY(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readPhyComAddrVID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readPhyComAddrAPPID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readTrackingObjRef(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readServiceType(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readErrorCode(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readOriginatorID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API int   TMW_TRK_readTimestamp( void * N, int size, void * parameter, void * E );
#ifdef __cplusplus
};
#endif


#endif /* HNDLTRK_H_ */
