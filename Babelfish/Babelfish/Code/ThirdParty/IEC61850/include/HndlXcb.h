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
#ifndef HNDLXCB_H_
#define HNDLXCB_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Handlers for Setting Group Control Blocks
 * ******************************************************************** */
TMW_CLIB_API int  MMSd_readNumOfSG(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeNumOfSG(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int  MMSd_readActSG(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeActSG(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int  MMSd_readEditSG(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeEditSG(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int  MMSd_readCnfEdit(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeCnfEdit(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int  MMSd_readLActTm(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeLActTm(void * N, void *D, int size, void * parameter, void * E);


#if defined(TMW_USE_SAV)
/*
 * Data handlers for client access to SVCBs
 ******************************************************************************
 */
TMW_CLIB_API int TMW_SAV_readSvEna(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvEna(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvResv(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvResv(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvID(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvID(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvDatSet(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvDatSet(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvConfRev(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvConfRev(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvOptFlds(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvOptFlds(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvSmpRate(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvSmpRate(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvSmpMod(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvSmpMod(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvNoASDU(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvNoASDU(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvDstAddr(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvDstAddr(void * N, void *D, int size, void * parameter, void * E);
TMW_CLIB_API void TMW_SAV_writeSvDstAddrR(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvPriority(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvPriority(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvVid(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvVid(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int TMW_SAV_readSvAppid(void * N, int size, void * parameter, void * E);

TMW_CLIB_API void TMW_SAV_writeSvAppid(void * N, void *D, int size, void * parameter, void * E);

#endif

#if defined(TMW_USE_IEC_GOOSE)
/**
 * Handlers for GOOSE Control Blocks
 * ******************************************************************** */
TMW_CLIB_API int MMSd_readGoEna(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoEna(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoID(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoDatSet(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoDatSet(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoConfRev(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoConfRev(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoNdsCom(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoNdsCom(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoDstAddr(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoDstAddr(void * N, void *D, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoDstAddrR(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoPriority(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoPriority(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoVid(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoVid(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoAppid(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeGoAppid(void * N, void *D, int size, void * parameter, void * E);

/* New for Ed2 */
TMW_CLIB_API int MMSd_readGoMaxTime(void * N, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoMinTime(void * N, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readGoFixedOffs(void * N, int size, void * parameter, void * E);
#endif

/**
 * Handlers for Control Blocks
 * ******************************************************************** */
TMW_CLIB_API int  MMSd_readOrIdent(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void MMSd_writeOrIdent(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int MMSd_readResvTms(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeResvTms(void *N, void *D, int size, void *parameter, void *E);
TMW_CLIB_API int MMSd_readResvTmsSG(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeResvTmsSG(void *N, void *D, int size, void *parameter, void *E);

TMW_CLIB_API int MMSd_readOwner(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeOwner(void *N, void *D, int size, void *parameter, void *E);

TMW_CLIB_API int     MMSd_readTrgOps(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeTrgOps(void * N, void *D, int size, void * parameter, void * E);


TMW_CLIB_API int     MMSd_readBufTim(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeBufTim(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readDatSetNa(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeDatSetNa(void * N, void *D, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeDatSetNaU(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readIntgPd(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeIntgPd(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readOptFlds(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeOptFlds(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readRptID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeRptID(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readRptEna(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeRptEna(void * N, void *D, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeRptEnaU(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readSeqNum(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeSeqNum(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readSubSeqNum(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeSubSeqNum(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readGenIntr(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeGenIntr(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readConfRev(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeConfRev(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readPurgeBuf(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writePurgeBuf(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readEntryID(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeEntryID(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readResv(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeResv(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readTimeOfEntry(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeTimeOfEntry(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readLogEna(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeLogEna(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readLogRef(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeLogRef(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readNewEnt(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeNewEnt(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readOldEnt(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeOldEnt(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readNewEntrTim(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeNewEntrTim(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readOldEntrTim(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeOldEntrTim(void * N, void *D, int size, void * parameter, void * E);

TMW_CLIB_API int     MMSd_readNeedsCommissioning(void * N, int size, void * parameter, void * E);
TMW_CLIB_API void    MMSd_writeNeedsCommissioning(void * N, void *D, int size, void * parameter, void * E);

#ifdef __cplusplus
};
#endif


#endif /* HNDLXCB_H_ */
