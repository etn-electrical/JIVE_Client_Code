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
 *  Declarations for Time Conversion.
 *
 *  This file should not require modification.
 */

#ifndef _TIMEFUNC_H
#define _TIMEFUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "uca_time.h"

typedef struct MMSd_DateTime {
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
    unsigned int msec;
#if defined(MMSD_MICROSECONDS_TIMER)
    unsigned int microseconds;
#endif

} MMSd_DateTime;

TMW_CLIB_API int MMSd_UnixToMdy( time_t unixTimeDate, MMSd_DateTime *dateTime );

TMW_CLIB_API int MMSd_MdyToUnix( MMSd_DateTime *dateTime, unsigned long *unixTimeDate );

TMW_CLIB_API int MMSd_MdyToMms(MMSd_DateTime *dateTime, MMSd_time *mms_time);

TMW_CLIB_API int MMSd_MmsToMdy(MMSd_time *mms_time, MMSd_DateTime *dateTime);

TMW_CLIB_API int MMSd_AsciiToMdy(char *str, MMSd_DateTime *dateTime);

TMW_CLIB_API int MMSd_MdyToAscii(MMSd_DateTime *dateTime, char *str);

/* Utc Time conversion routines */
TMW_CLIB_API int MMSd_MdyToUtc(MMSd_DateTime *dateTime, MMSd_UtcBTime *utcTime);

TMW_CLIB_API int MMSd_UtcToMdy(MMSd_UtcBTime *utcTime, MMSd_DateTime *dateTime);

TMW_CLIB_API int MMSd_MmsToUtc(MMSd_time *mmstime, MMSd_UtcBTime *utcbtime);

TMW_CLIB_API int MMSd_UtcToMms(MMSd_UtcBTime *utcbtime, MMSd_time *mmstime);

TMW_CLIB_API void MMSd_adjusted_utc_time_stamp(MMSd_UtcBTime *pUtcBtime);

/* Utc Time parse/display routines */
TMW_CLIB_API int MMSd_ParseUtcString(char *str, MMSd_UtcBTime *utcVal);

TMW_CLIB_API int MMSd_DisplayUtcTime(MMSd_UtcBTime *utcVal, char *str, int len);

TMW_CLIB_API int MMSd_DisplayUtcString(MMSd_UtcBTime *utcVal, char *str, int len);

TMW_CLIB_API int MMSd_GeneralizedTimeToMdy(char *str, MMSd_DateTime *dateTime);

TMW_CLIB_API int MMSd_MdyToGeneralizedTime(MMSd_DateTime *dateTime, char *str);

TMW_CLIB_API int MMSd_GeneralizedTimeStamp(char *pGeneralizedTime);

#ifdef __cplusplus
};
#endif

#endif /* _TIMEFUNC_H */
