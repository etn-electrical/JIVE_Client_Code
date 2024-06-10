/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2000-2015 */
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
 *  UCA time functions.
 *
 *  This file should not require modification.
 */

#define TARG_TIME_C
#ifdef TARG_TIME_C

#define TARG_TIME_CHANGE_FREERTOS  /* Targ_time.c file code changes for timer function */

#include <stdio.h>

#ifdef TARG_TIME_CHANGE_FREERTOS
#include "sys.h" 
#endif

#include "tmwtarg.h"
#include "uca_time.h"


#ifndef TARG_TIME_CHANGE_FREERTOS
#include <sys/timeb.h>
#endif
#include "timefunc.h"

#ifdef TARG_TIME_CHANGE_FREERTOS
#include "Rtc_If.h" 
#include "Timers_Lib.h" 
#include "../../PTPd/ptpd.h"
#endif

extern MMSd_time adjust;

#ifdef TARG_TIME_CHANGE_FREERTOS
void RtcToMms(Time_Internal *RtcDateTime, MMSd_DateTime *MmsDateTime); 
#endif
/* ------------------------------------------------------------------------
 ---   tmwTargStartClock()                                                 --
 ---       x                                                              --
 ---   Calling sequence:                                                  --
 ---       void                                                           --
 ---   Return:                                                            --
 ---       void                                                           --
 ------------------------------------------------------------------------ */
void tmwTargStartClock(void)
{
    adjust.millisecondsSinceMidnight = 0L;
    adjust.daysSinceJan1_1984 = 0;
    return;
}

void tmwTargStopClock(void)
{
    return;
}

/* ------------------------------------------------------------------------
 ---   tmwTargTimeStamp()                                              --
 ---       Stores the time in ticks (1.024 / millisecond) since midnight, --
 ---       and days since Jan1_1984 of the clock.                         --
 ---   Calling sequence:                                                  --
 ---       &mms_time       - structure filled in with time and day.       --
 ---   Return:                                                            --
 ---       void                                                           --
 ------------------------------------------------------------------------ */
void tmwTargTimeStamp(MMSd_time *mms_time)
{
#ifndef TARG_TIME_CHANGE_FREERTOS
    struct timeb time_b;
    MMSd_DateTime datTime;

    ftime( &time_b );
    MMSd_UnixToMdy( time_b.time, &datTime );
    MMSd_MdyToMms( &datTime, mms_time );
    mms_time->millisecondsSinceMidnight += time_b.millitm;
#if defined(MMSD_MICROSECONDS_TIMER)
    mms_time->microseconds = 0;
#endif
#endif

#ifdef TARG_TIME_CHANGE_FREERTOS  
/************************************************************************************
************************************************************************************* 
############ Eaton Change: To get PTP timestamp for Goose message ############ 
************************************************************************************* 
************************************************************************************* 
LTK-7586 Create interface so that PTP time can be used by IEC 61850 stack
*/
    MMSd_DateTime MmsDateTime;    
    Time_Internal RtcDateTime ={ 0 };
    TimeInternal time;
    getTime( &time );
    const uint32_t DAYS_OFFSET_EPOCH = (12*365L+2*366L);
    const uint32_t SECONDS_PER_DAY = (60L*60L*24L);
    if ( time.seconds > ( DAYS_OFFSET_EPOCH*SECONDS_PER_DAY ))
    {
      MMSd_UtcToMdy((MMSd_UtcBTime*)&time,&MmsDateTime);
    }
    else
    {
      Rtc_If::Get_Date_Time(RtcDateTime);
      RtcToMms(&RtcDateTime, &MmsDateTime);
    }
    MMSd_MdyToMms( &MmsDateTime, mms_time );
#endif
}

#ifdef TARG_TIME_CHANGE_FREERTOS
void RtcToMms(Time_Internal *RtcDateTime, MMSd_DateTime *MmsDateTime)
{
    MmsDateTime->year = RtcDateTime->year+2000;
    MmsDateTime->month = RtcDateTime->month;
    MmsDateTime->day = RtcDateTime->day;
    MmsDateTime->hour = RtcDateTime->hour;
    MmsDateTime->minute = RtcDateTime->minute;
    MmsDateTime->second = RtcDateTime->second;
    MmsDateTime->msec = RtcDateTime->microsecond/1000;
 #if defined(MMSD_MICROSECONDS_TIMER)
    MmsDateTime->microseconds = RtcDateTime->microsecond % 1000;
#endif   
}
#endif

/* ------------------------------------------------------------------------
 ---   tmwTargGetMsCount()                                               --
 ---       Returns the millisecond count since the system start subject   --
 ---       to rollover every 49.7 days. The count is based on the         --
 ---       monotonic clock to ensure changes to the wall clock do not    --
 ---       effect the timeout values.                                     --
 ---   Return:                                                            --
 ---       TMW_ms_time                                                    --
 ------------------------------------------------------------------------ */
TMW_ms_time tmwTargGetMsCount(void)
{
#ifndef TARG_TIME_CHANGE_FREERTOS
    struct timespec now;

   if (clock_gettime(CLOCK_MONOTONIC_RAW, &now) != 0) {
      TMW_TARG_TRACE_MSG(TmwTargTraceMaskTime, TmwTargTraceLevelCritical,
                         "clock_gettime returned failure\n");
   }
   return (now.tv_sec * 1000 + now.tv_nsec/1000000);
#endif
   
#ifdef TARG_TIME_CHANGE_FREERTOS
    return(BF_Lib::Timers::Get_Time());
#endif
}

#endif //#ifdef TARG_TIME_C
