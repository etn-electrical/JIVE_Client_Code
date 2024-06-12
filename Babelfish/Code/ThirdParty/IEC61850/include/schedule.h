/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c)      2016 */
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
 *  This file contains declarations to support scheduling.
 *
 *  This file should not require modification.
 */

#ifndef _SHEDULE_H
#define _SHEDULE_H


#ifdef __cplusplus
extern "C" {
#endif



typedef void (*TMW_ReportWaken)(MMSd_context *pMmsContext);

typedef void *TMW_ScheduleHandle;

typedef unsigned long (*TMW_ScheduledFcn)(TMW_ScheduleHandle handle,
                                          MMSd_time *pScheduledTime,
                                          TMW_ReportWaken reportWakenFcn,
                                          void *pConnection, void *pVoidParam);

typedef unsigned long (*TMW_AbortFcn)(TMW_ScheduleHandle handle,
                                      MMSd_time *pScheduledTime,
                                      TMW_ReportWaken reportWakenFcn,
                                      void *pConnection, void *pVoidParam);
 
TMW_CLIB_API void TMW_ScheduleTerminate(void);

TMW_CLIB_API void TMW_ScheduleDestroy(void);

TMW_CLIB_API void TMW_ThreadServiceSchedule(void *handle);

TMW_CLIB_API void *TMW_ScheduleCreate(TMW_ReportWaken reportWakenFcn);

TMW_CLIB_API unsigned long TMW_TempIecGooseService( void *param );

#define TMW_MAX_SCHEDULED_CONTROLS  100

#ifdef __cplusplus
}
#endif


#endif /* _SHEDULE_H */
