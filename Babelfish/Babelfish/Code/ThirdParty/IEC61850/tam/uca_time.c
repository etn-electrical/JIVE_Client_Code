/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2000-2014 */
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

#define UCA_TIME_C
#ifdef UCA_TIME_C

#include "sys.h" 

#include "tmwtarg.h"
#include "uca_time.h"

MMSd_time adjust = { 0L,
					 0
#if defined(MMSD_MICROSECONDS_TIMER)
					 ,0
#endif
};

/* #define TESTING_MIDNIGHT */

#if defined(TESTING_MIDNIGHT)
#define HOURS_UNTIL_MIDNIGHT    0
#define MINUTES_UNTIL_MIDNIGHT  0
#define SECONDS_UNTIL_MIDNIGHT  20
#define MSEC_UNTIL_MIDNIGHT     0

#endif

/* #define TESTING_ROLLOVER */

#if defined(TESTING_ROLLOVER)
#define MSEC_UNTIL_ROLLOVER    20000

unsigned __int64 rollover_offset;
int rollover_inited = 0;

int __QueryPerformanceCounter( LARGE_INTEGER *perftime )
{
    LARGE_INTEGER realCounts;

    if ( !QueryPerformanceCounter( &realCounts ) )
        return( 0 );

    if ( !rollover_inited ) {
        unsigned __int64 temp;

        rollover_offset = _UI64_MAX - ((unsigned __int64) realCounts.QuadPart);
        rollover_offset = rollover_offset - (clockrate * MSEC_UNTIL_ROLLOVER); /* 15 seconds of counts */
        rollover_inited = 1;
    }
    ((unsigned __int64) realCounts.QuadPart) += rollover_offset;
    *perftime = realCounts;
    return( 1 );
}

#define QueryPerformanceCounter( a ) __QueryPerformanceCounter( a )
#endif


/*
 *
 *
 *******************************************************************************/
void MMSd_null_time(MMSd_time *timer)
{
    if (timer == NULL)
        return;
    timer->millisecondsSinceMidnight = 0;
    timer->daysSinceJan1_1984 = 0;
}


/*
 *
 *
 *******************************************************************************/
int MMSd_is_null_time(MMSd_time *timer)
{
    if (timer == NULL)
        return (0);

    if ((timer->millisecondsSinceMidnight == 0) && (timer->daysSinceJan1_1984 == 0))
        return (1);
    return (0);
}


/*
 *
 *
 *******************************************************************************/
int MMSd_timed_out(MMSd_time *timer,
                   MMSd_time *now,
                   unsigned long mmsDelay)
{
    long long diff_milliseconds;
    int diff_days;
#define MMSD_MAX_LONG 2147483647L
#define MMSD_SECS_PER_DAY 86400L
#define MMSD_MAX_DAYS ((MMSD_MAX_LONG / MMSD_SECS_PER_DAY) / 1000)

    if ((timer == NULL) || (now == NULL))
        return (0);

    diff_milliseconds = now->millisecondsSinceMidnight - timer->millisecondsSinceMidnight;
    diff_days = now->daysSinceJan1_1984 - timer->daysSinceJan1_1984;

    if (diff_days > MMSD_MAX_DAYS)
        return (1); /* Must be time! */

    diff_milliseconds = diff_milliseconds + (diff_days * (MMSD_SECS_PER_DAY * 1000));
    if (diff_milliseconds <= 0)
        return (0);

    if ((unsigned long) diff_milliseconds >= mmsDelay)
        return (1);

    return (0);
#undef MMSD_MAX_LONG
#undef MMSD_MAX_DAYS
}


/*
 *   Allow an offset adjustment for certain timestamps.
 *
 *******************************************************************************/
void MMSd_set_adjust(MMSd_time *mms_time)
{
    adjust = *mms_time;
    return;
}

/*
 *
 *
 *******************************************************************************/
void MMSd_get_adjust(MMSd_time *mms_time)
{
    *mms_time = adjust;
    return;
}


/*
 *
 *
 *******************************************************************************/
void TMW_MMSd_adjusted_time_stamp(MMSd_time *mms_time)
{
    tmwTargTimeStamp( mms_time );

#if defined(MMSD_MICROSECONDS_TIMER)
	
    mms_time->microseconds = mms_time->microseconds + adjust.microseconds;
    if (mms_time->microseconds < 0) {
        mms_time->microseconds = mms_time->microseconds + 1000;
        mms_time->millisecondsSinceMidnight =
            mms_time->millisecondsSinceMidnight - 1;
    }
    if (mms_time->microseconds > 999) {
        mms_time->microseconds = 0;
        mms_time->millisecondsSinceMidnight =
            mms_time->millisecondsSinceMidnight + 1;
    }
#endif
    mms_time->millisecondsSinceMidnight = mms_time->millisecondsSinceMidnight + adjust.millisecondsSinceMidnight;
    if (mms_time->millisecondsSinceMidnight < 0) {
        mms_time->millisecondsSinceMidnight = mms_time->millisecondsSinceMidnight + 86400000L;
        mms_time->daysSinceJan1_1984 = mms_time->daysSinceJan1_1984 - 1;
    }
    if (mms_time->millisecondsSinceMidnight > 86399999L) {
        mms_time->millisecondsSinceMidnight = mms_time->millisecondsSinceMidnight - 86400000L;
        mms_time->daysSinceJan1_1984 = mms_time->daysSinceJan1_1984 + 1;
    }

    mms_time->daysSinceJan1_1984 = mms_time->daysSinceJan1_1984 + adjust.daysSinceJan1_1984;

    return;
}

/*
 *  Check for expired time
 *
 *******************************************************************************/
int TMW_ms_timed_out( TMW_ms_time then, TMW_ms_time interval )
{
    TMW_ms_time diff;

    /* Unsigned arithemetic handles roll over */
    diff = tmwTargMsTimeStamp() - then;
#if defined(TMW_TARG_TRACE_MSG)
    if ( diff >= interval ) {
        TMW_TARG_TRACE_MSG(TmwTargTraceMaskTime, TmwTargTraceLevelDebug,
                           "%s: returning TIMEOUT, diff = %d, interval = %d\n",
                           __FUNCTION__, diff, interval);
    }
#endif
    return ( diff >= interval );
}

#endif //#ifdef UCA_TIME_C
