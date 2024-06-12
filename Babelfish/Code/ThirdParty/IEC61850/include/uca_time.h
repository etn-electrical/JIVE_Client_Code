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
 *  Declarations for UCA time functions.
 *
 *  This file should not require modification.
 */

#ifndef _UCA_TIME_H
#define _UCA_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#define JAN_1_1984      441763200L      /* Unix date for 1/1/84 00:00:00.0 */
#define SECS_PER_DAY    86400L
#define MS_PER_HOUR     (60. * 60L * 1000L)
#define MS_PER_MIN      (60. * 1000L)
#define MS_PER_SEC      1000.
#define MS_PER_HUND     10.

#if !defined(_UCA_TIME_DEF)
typedef struct {
    long millisecondsSinceMidnight;
    int daysSinceJan1_1984;
#if defined(MMSD_MICROSECONDS_TIMER)
     int microseconds;
#endif
} MMSd_time;
#define _UCA_TIME_DEF
#endif

typedef unsigned long TMW_ms_time;

#if !defined(_UTC_TIME_DEF)
typedef unsigned char MMSd_UtcQuality;

#define MMSd_Utc_LeapSecondKnown        0x80
#define MMSd_Utc_FaultClockFailure      0x40
#define MMSd_Utc_FaultNotSynchronzed    0x20
#define MMSd_Utc_AccuracyMask           0x1F
#define MMSd_UtcAccuracy( q )   ( (q) & MMSd_Utc_AccuracyMask )

typedef struct MMSd_UtcBTime {
    unsigned long secondOfCentury;
    unsigned long fractionOfSecond;
    MMSd_UtcQuality quality;
} MMSd_UtcBTime;
#define _UTC_TIME_DEF
#endif

/* Prototypes */

void MMSd_set_adjust(MMSd_time *adjust);

void MMSd_get_adjust(MMSd_time *mms_time);

TMW_CLIB_API void TMW_MMSd_time_stamp(MMSd_time *mms_time);

TMW_CLIB_API void MMSd_null_time(MMSd_time *timer);

TMW_CLIB_API int MMSd_is_null_time(MMSd_time *timer);

TMW_CLIB_API int MMSd_timed_out(MMSd_time *timer, MMSd_time *now, unsigned long mmsDelay);

TMW_CLIB_API void TMW_MMSd_adjusted_time_stamp(MMSd_time *mms_time);

TMW_CLIB_API int TMW_ms_timed_out(TMW_ms_time then, TMW_ms_time interval);

TMW_CLIB_API void TMW_ms_time_stamp(TMW_ms_time *now);

TMW_CLIB_API long TMW_ms_time_diff(TMW_ms_time then, TMW_ms_time now);

TMW_CLIB_API int TMW_reset_clock(TMW_ms_time before_midnight);

#ifdef __cplusplus
}
;
#endif

#endif          /* _UCA_TIME_H */
