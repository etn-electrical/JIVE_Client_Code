/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2013-2015 */
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


/*
 *
 *  This file contains
 *    Platform-independent interface into the target layer.
 *
 *   This file should not require modification.
 */


#ifndef _TMW_TARG_H
#define _TMW_TARG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vendor.h"

typedef enum TmwTargRc {
    TmwTargRcFailure = 0,
    TmwTargRcSuccess
} TmwTargRc_t;

#include "tmwtargtrace.h"
#include "targ_utils.h"  /* OS-specific */

#include "ACSI.h"

#if defined(TMW_USE_FILES)
#error ("Error reported- TMW_USE_FILES")
#include "files.h"
#endif

#include "timefunc.h" 
#include "tam.h" 
#include "user.h"
#include "tam_tp.h"
#include "timefunc.h"

typedef void *TmwTargServiceThreadHandle_t;

#define MMSd_time_stamp( t )            tmwTargTimeStamp( t )
#define MMSd_adjusted_time_stamp( t )   TMW_MMSd_adjusted_time_stamp( t )

#define tmwTargMsTimeStamp()            tmwTargGetMsCount()

/* Max number of ethernet address binds that are supported */
#ifndef TMW_ETH_MAX_BINDINGS
#define TMW_ETH_MAX_BINDINGS    100
#endif

TMW_CLIB_API void tmwTargTimeStamp(MMSd_time*);

TMW_CLIB_API TMW_ms_time tmwTargGetMsCount(void);

TMW_CLIB_API void tmwTargStartClock(void);

TMW_CLIB_API void tmwTargStopClock(void);


/*
 * Threading interface declarations
 ******************************************************************************
 */
#if  defined(TMW_USE_THREADS)
TmwTargThreadDecl_t tmwTargDlEventTaskFunc(TmwTargThreadArg_t pParams);

TmwTargThreadDecl_t tmwTargDlUserThreadFunc(TmwTargThreadArg_t pParams);

TmwTargThreadDecl_t tmwTargRfc1006EventTaskFunc(TmwTargThreadArg_t pParams);

TmwTargThreadDecl_t tmwTargEventTaskFunc(TmwTargThreadArg_t pParams);

TmwTargThreadDecl_t TMW_Thread(TmwTargThreadArg_t pParams);


#define TMW_TASK_FUNC TmwTargTaskFunction_t

TMW_CLIB_API TmwTargThreadHandle_t tmwTargThreadStart( TMW_TASK_FUNC threadFunction,
													   void *threadFunctionParams,
													   unsigned long stackSize,
													   unsigned long timeout);

TMW_CLIB_API void tmwTargThreadStop(TmwTargThreadHandle_t handle);

void tmwTargThreadQuit(TmwTargThreadHandle_t handle);

int tmwTargThisThreadChangeInterval(TmwTargThreadHandle_t handle,
                                    unsigned long interval);

int tmwTargThisThreadIntervalChangeLimited(TmwTargThreadHandle_t handle,
        unsigned long interval);

int tmwTargThreadWake(TmwTargThreadHandle_t handle);

int tmwTargThreadIntervalChange(TmwTargThreadHandle_t handle,
                                unsigned long interval);

int tmwTargThreadIntervalChangeLimited(TmwTargThreadHandle_t handle,
                                       unsigned long interval);

TMW_CLIB_API TmwTargServiceThreadHandle_t tmwTargThreadServiceStart(
    void *clientContext, unsigned long serviceTimeout);

TmwTargServiceThreadHandle_t tmwTargThreadTpServiceAdd(
											TmwTargServiceThreadHandle_t handle, TMW_TP_Context *tpContext,
											unsigned long tpTimeout);

TMW_CLIB_API TmwTargServiceThreadHandle_t tmwTargThreadDlServiceAdd(
											TmwTargServiceThreadHandle_t handle, TMW_DL_User_Func func,
											void *funcArg, TMW_DL_Context *dlContext, TmwTargSem_t sem,
											TMW_TARG_EVENT  eve,
											unsigned long dlTimeout);

TMW_CLIB_API void tmwTargThreadServiceStop(TmwTargServiceThreadHandle_t handle);

int tmwTargThreadServiceEnroll(TMW_TP_Connection *tpConnection,
                               TmwTargServiceThreadHandle_t handle);

int tmwTargThreadServiceRemove(TMW_TP_Connection *tpConnection,
                               TmwTargServiceThreadHandle_t handle);

int tmwTargThreadDlServiceStop(TmwTargServiceThreadHandle_t handle,
                               TMW_DL_User_Func func, void *arg,
                               TMW_DL_Context *dlContext);

int tmwTargTpServiceStop(TmwTargServiceThreadHandle_t handle,
                         TMW_TP_Context *tpContext);

TmwTargRc_t tmwTargThreadCreate( TmwThreadId_t *pId,
					 			 TmwTargThreadFunction_t pFunc,
								 TmwTargThreadArg_t pFuncParams,
								 unsigned long stackSize );




#define TMW_SEM_CREATE( a )     TMW_TARG_SEM_CREATE( a )
#define TMW_SEM_DELETE( a )     TMW_TARG_SEM_DELETE( a )
#define TMW_SEM_TAKE( a )       TMW_TARG_SEM_TAKE( a )
#define TMW_SEM_GIVE( a )       TMW_TARG_SEM_GIVE( a )
#define TMW_EVENT               TMW_TARG_EVENT
#define TMW_EVENT_CREATE( e )   TMW_TARG_EVENT_CREATE( e )
#define TMW_EVENT_DELETE( e )   TMW_TARG_EVENT_DELETE( e )
#define TMW_EVENT_SIGNAL( e )   TMW_TARG_EVENT_SIGNAL( e )
#define TMW_EVENT_RESET( e )    TMW_TARG_EVENT_RESET( e )
#define TMW_EVENT_WAIT( e, ms ) TMW_TARG_EVENT_WAIT( e, ms )


#endif /* defined(TMW_USE_THREADS) */

#if defined(MMSD_USE_FILE_SERVICES)

TmwTargRc_t tmwTargFileSize(FILE *file, unsigned long *size);
TmwTargRc_t tmwTargFileTime(FILE *file, struct tm **date);
MMSd_length tmwTargFileDirRequest(MMSd_descriptor *E,
                                  char *pathName,
                                  char *fileName,
                                  int  *haveContinueAfter,
                                  int  *done,
                                  int  *pduFull,
                                  MMSd_errorFlag *errorcode);


#endif /* defined(MMSD_USE_FILE_SERVICES) */



#ifdef __cplusplus
}
;
#endif

#endif /* _TMW_TARG_H */
