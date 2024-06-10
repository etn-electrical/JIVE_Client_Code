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
 *  time functions.
 *
 *  This file should not require modification.
 */

#define TIMEFUNC_C
#ifdef TIMEFUNC_C

#include "sys.h" 

#include "tmwtarg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timefunc.h"

#define DAYS_PER_400_YEARS  146097L
#define DAYS_PER_4_YEARS    1461L
#define DAYS_PER_YEAR       365L
#if !defined(SECS_PER_DAY)
#define SECS_PER_DAY        (60L*60L*24L)
#endif
#define SECS_PER_HOUR       (60L*60L)
#define SECS_PER_MIN        60L
#define MSECS_PER_DAY       (60L*60L*24L*1000L)
#define MSECS_PER_HOUR      (60L*60L*1000L)
#define MSECS_PER_MIN       (60L*1000L)
#define MSECS_PER_SEC       1000L
#define CALENDAR_DAY_BASE   1721119L
#define BASE_OFFSET_DAYS    365L
#define BASE_OFFSET_YEARS   1
#define BASE_YEAR           1969
#define DAYS_OFFSET_1970    (12*365L+2*366L)

/* *********************************************************************
 * Time support routines
 ********************************************************************* */

static const unsigned int FirstDayOfEachMonth[12] = { 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
static const unsigned int LeapFirstDayOfEachMonth[12] = { 1, 32, 61, 92, 122, 153, 183, 214, 245, 275, 306, 336 };

/*
 * Converts UNIX time (seconds since 1/1/1970) to MMSd_DateTime structure
 *
 *******************************************************************************/
int MMSd_UnixToMdy( time_t unixTimeDate,
                   MMSd_DateTime *dateTime)
{
    time_t mon, day, year, hour, min, sec, msec, base;
    const unsigned int *monthDays;

    /* Normalize to day within 4 year epoch */
    base = unixTimeDate / SECS_PER_DAY;
    sec = unixTimeDate - (base * SECS_PER_DAY);
    msec = 0L;
    base = base + BASE_OFFSET_DAYS + 1;
    year = (((base << 2) - 1) / DAYS_PER_400_YEARS);
    base = (base << 2) - 1 - (DAYS_PER_400_YEARS * year);
    day = (base >> 2);
    base = ((day << 2) + 3) / DAYS_PER_4_YEARS;
    day = (day << 2) + 3 - (DAYS_PER_4_YEARS * base);
    day = (day + 4) >> 2;
    year = (100 * year + base) - BASE_OFFSET_YEARS + 1970;

    mon = 0;
    if (((year & 0x3) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))
        monthDays = LeapFirstDayOfEachMonth;
    else
        monthDays = FirstDayOfEachMonth;

    while ((mon < 11) && (day + 1 > monthDays[mon + 1]))
        mon++;
    {
        day -= monthDays[mon];
        mon++;
        day++;
    }
    hour = sec / SECS_PER_HOUR;
    sec = sec - (SECS_PER_HOUR * hour);
    min = sec / SECS_PER_MIN;
    sec = sec - (SECS_PER_MIN * min);

    dateTime->year = (unsigned int) year;
    dateTime->month = (unsigned int) mon;
    dateTime->day = (unsigned int) day;
    dateTime->hour = (unsigned int) hour;
    dateTime->minute = (unsigned int) min;
    dateTime->second = (unsigned int) sec;
    dateTime->msec = (unsigned int) msec;

    return (1);
}

/*
 * Converts MMSd_DateTime structure to UNIX time (seconds since 1/1/70)
 *
 *******************************************************************************/
int MMSd_MdyToUnix(MMSd_DateTime *dateTime,
                   unsigned long *unixTimeDate)
{
    unsigned long year;
    const unsigned int *monthDays;
    unsigned long result;

    if ((dateTime->year < 1970) || (dateTime->month > 12) || (dateTime->hour > 23) || (dateTime->minute > 59)
            || (dateTime->second > 59))
        return (0);
    year = dateTime->year - BASE_YEAR;
    result = (year >> 2) * DAYS_PER_4_YEARS * SECS_PER_DAY;
    year = (year & 0x03);
    result += year * DAYS_PER_YEAR * SECS_PER_DAY;
    result -= BASE_OFFSET_DAYS * SECS_PER_DAY;
    if (((dateTime->year & 0x3) == 0) && (((dateTime->year % 100) != 0) || ((dateTime->year % 400) == 0)))
        monthDays = LeapFirstDayOfEachMonth;
    else
        monthDays = FirstDayOfEachMonth;
    result += (monthDays[dateTime->month - 1] - 1) * SECS_PER_DAY;
    result += (dateTime->day - 1) * SECS_PER_DAY;
    result += dateTime->hour * SECS_PER_HOUR;
    result += dateTime->minute * SECS_PER_MIN;
    result += dateTime->second;
    *unixTimeDate = result;
    return (1);
}


/*
 * Converts MMSd_DateTime structure to MMS time (days and milliseconds since 1/1/84)
 *
 *******************************************************************************/
int MMSd_MdyToMms(MMSd_DateTime *dateTime,
                  MMSd_time *mms_time)
{
    unsigned long year;
    const unsigned int *monthDays;
    unsigned long result;

    if ((dateTime->year < 1984) || (dateTime->month > 12) || (dateTime->hour > 23) || (dateTime->minute > 59)
            || (dateTime->second > 59))
        return (0);
    year = dateTime->year - BASE_YEAR;
    result = (year >> 2) * DAYS_PER_4_YEARS;
    year = year & 0x03;
    result += year * DAYS_PER_YEAR;
    if (((dateTime->year & 0x3) == 0) && (((dateTime->year % 100) != 0) || ((dateTime->year % 400) == 0)))
        monthDays = LeapFirstDayOfEachMonth;
    else
        monthDays = FirstDayOfEachMonth;
    result += monthDays[dateTime->month - 1] - 1;
    result += dateTime->day - 2;
    mms_time->daysSinceJan1_1984 = (int) (result - DAYS_OFFSET_1970 - BASE_OFFSET_DAYS);
    mms_time->millisecondsSinceMidnight = dateTime->hour * MSECS_PER_HOUR;
    mms_time->millisecondsSinceMidnight += dateTime->minute * MSECS_PER_MIN;
    mms_time->millisecondsSinceMidnight += dateTime->second * MSECS_PER_SEC;
    mms_time->millisecondsSinceMidnight += dateTime->msec;
#if defined(MMSD_MICROSECONDS_TIMER)
    mms_time->microseconds = dateTime->microseconds;
#endif
    return (1);
}

/*
 * Converts MMS time(days and milliseconds since 1/1/84) to MMSd_DateTime structure
 *
 *******************************************************************************/
int MMSd_MmsToMdy(MMSd_time *mms_time,
                  MMSd_DateTime *dateTime)
{
    unsigned  long base, year, mon, day, hour, min, sec, msec;
    const unsigned int *monthDays;

    base = mms_time->daysSinceJan1_1984 + DAYS_OFFSET_1970 + BASE_OFFSET_DAYS + 1;
    base += 1;
    year = (((base << 2) - 1) / DAYS_PER_400_YEARS);
    base = (base << 2) - 1 - (DAYS_PER_400_YEARS * year);
    day = (base >> 2);
    base = ((day << 2) + 3) / DAYS_PER_4_YEARS;
    day = (day << 2) + 3 - (DAYS_PER_4_YEARS * base);
    day = (day + 4) >> 2;
    year = (100 * year + base) - BASE_OFFSET_YEARS + 1970;
    if (((year & 0x3) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))
        monthDays = LeapFirstDayOfEachMonth;
    else
        monthDays = FirstDayOfEachMonth;
    mon = 0;
    while ((day + 1 > monthDays[mon + 1]) && (mon < 11))
        mon++;
    day -= monthDays[mon];
    mon++;
    day++;
    msec = mms_time->millisecondsSinceMidnight;
    hour = msec / MSECS_PER_HOUR;
    msec -= (hour * MSECS_PER_HOUR);
    min = msec / MSECS_PER_MIN;
    msec -= (min * MSECS_PER_MIN);
    sec = msec / MSECS_PER_SEC;
    msec -= (sec * MSECS_PER_SEC);

    dateTime->hour = (unsigned int) hour;
    dateTime->minute = (unsigned int) min;
    dateTime->second = (unsigned int) sec;
    dateTime->month = (unsigned int) mon;
    dateTime->day = (unsigned int) day;
    dateTime->year = (unsigned int) year;
    dateTime->msec = (unsigned int) msec;
#if defined(MMSD_MICROSECONDS_TIMER)
    dateTime->microseconds = mms_time->microseconds;
#endif
    return (1);
}


/*
 * Converts US time(mm/dd/yyyy) string to MMSd_DateTime structure
 *
 *******************************************************************************/
int MMSd_AsciiToMdy(char *str,
                    MMSd_DateTime *dateTime)
{
    int fields;

    dateTime->hour = 0;
    dateTime->minute = 0;
    dateTime->second = 0;
    dateTime->msec = 0;
    fields = sscanf( str, "%2d/%2d/%4d_%2d:%2d:%2d.%3d", &dateTime->month, &dateTime->day, &dateTime->year,
                     &dateTime->hour, &dateTime->minute, &dateTime->second, &dateTime->msec );
    return (fields >= 3);
}

/*
 * Converts MMSd_DateTime structure to US time(mm/dd/yyyy) string
 *
 *******************************************************************************/
int MMSd_MdyToAscii(MMSd_DateTime *dateTime,
                    char *str)
{
    sprintf( str, "%02d/%02d/%04d_%02d:%02d:%02d.%03d",
             dateTime->month, dateTime->day, dateTime->year, dateTime->hour,
             dateTime->minute, dateTime->second, dateTime->msec );

    return (1);
}

/*
 * Converts time string(yyyy/mm/dd) to MMSd_DateTime structure
 *
 *******************************************************************************/
int MMSd_GeneralizedTimeToMdy(char *str,
                              MMSd_DateTime *dateTime)
{
    int fields;

    dateTime->hour = 0;
    dateTime->minute = 0;
    dateTime->second = 0;
    dateTime->msec = 0;
    fields = sscanf( str, "%4d/%2d/%2d%2d%2d%2d.%3d",
                     &dateTime->year, &dateTime->month, &dateTime->day,
                     &dateTime->hour, &dateTime->minute, &dateTime->second, &dateTime->msec );

    return (fields >= 3);
}

/*
 * Converts MMSd_DateTime structure to time string (yyyy/mm/dd)
 *
 *******************************************************************************/
int MMSd_MdyToGeneralizedTime(MMSd_DateTime *dateTime,
                              char *str)
{
    sprintf( str, "%04d%02d%02d%02d%02d%02d.%03dZ",
             dateTime->year, dateTime->month, dateTime->day, dateTime->hour,
             dateTime->minute, dateTime->second, dateTime->msec );

    return (1);
}


/*
 * Gets current adjusted timestamp and converts to time string (yyyy/mm/dd)
 *
 *******************************************************************************/
int MMSd_GeneralizedTimeStamp(char *pGeneralizedTime)
{
    unsigned long base, year, mon, day, hour, min, sec, msec;
    const unsigned int *monthDays;
    MMSd_time now;

    MMSd_adjusted_time_stamp( &now );

    base = now.daysSinceJan1_1984 + DAYS_OFFSET_1970 + BASE_OFFSET_DAYS + 1;
    base += 1;
    year = (((base << 2) - 1) / DAYS_PER_400_YEARS);
    base = (base << 2) - 1 - (DAYS_PER_400_YEARS * year);
    day = (base >> 2);
    base = ((day << 2) + 3) / DAYS_PER_4_YEARS;
    day = (day << 2) + 3 - (DAYS_PER_4_YEARS * base);
    day = (day + 4) >> 2;
    year = (100 * year + base) - BASE_OFFSET_YEARS + 1970;
    if (((year & 0x3) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))
        monthDays = LeapFirstDayOfEachMonth;
    else
        monthDays = FirstDayOfEachMonth;
    mon = 0;
    while ((day + 1 > monthDays[mon + 1]) && (mon < 11))
        mon++;
    day -= monthDays[mon];
    mon++;
    day++;
    msec = (unsigned long)now.millisecondsSinceMidnight;
    hour = msec / MSECS_PER_HOUR;
    msec -= (hour * MSECS_PER_HOUR);
    min = msec / MSECS_PER_MIN;
    msec -= (min * MSECS_PER_MIN);
    sec = msec / MSECS_PER_SEC;
    msec -= (sec * MSECS_PER_SEC);

    sprintf( pGeneralizedTime, "%04lu%02lu%02lu%02lu%02lu%02lu.%03luZ", year, mon, day, hour, min, sec, msec );
    return (1);
}


/*
 * Converts MMSd_DateTime structure to UtcBtime (seconds and fraction since beginning of century)
 *
 *******************************************************************************/
int MMSd_MdyToUtc(MMSd_DateTime *dateTime, MMSd_UtcBTime *utcTime)
{
    unsigned long usec = 0;        /* get fractions of second in Usec */
    unsigned long temp = 0;	
	
    if (!MMSd_MdyToUnix( dateTime, &utcTime->secondOfCentury ))
        return (0);

    if (dateTime->msec >= 1000)
        return (0);

    utcTime->quality = 0x00;
	usec = dateTime->msec * 1000;
#if defined(MMSD_MICROSECONDS_TIMER)
    usec += dateTime->microseconds;
#endif
    temp = ((usec * 1825) >> 5) + ((usec * 6) >> 12);
    utcTime->fractionOfSecond = (usec << 12) + (usec << 8) - temp;

    return (1);
}

/*
 * Converts UtcBtime (seconds and fraction since beginning of century) to MMSd_DateTime structure
 *
 *******************************************************************************/
int MMSd_UtcToMdy(MMSd_UtcBTime *utcTime, MMSd_DateTime *dateTime)
{
#if defined(MMSD_MICROSECONDS_TIMER)
	unsigned long fract_shifted12;
	unsigned long usec;
#else
	unsigned int tenMsecs;
#endif

	MMSd_UnixToMdy(utcTime->secondOfCentury, dateTime);	
#if defined(MMSD_MICROSECONDS_TIMER)
    fract_shifted12 = (utcTime->fractionOfSecond + 2048) >> 12;
    usec = ( (fract_shifted12 * 3906) + (fract_shifted12  >> 2) )  >> 12;

    dateTime->msec = usec / 1000;             /* get msec from this value */
    usec = usec - (dateTime->msec * 1000);    /* remove the msec already calc'd */
    dateTime->microseconds = usec;            /* get usec component  */

	if ( (utcTime->fractionOfSecond + 2048) & 0x00000800) {
		dateTime->microseconds++;
		if (dateTime->microseconds > 999) {
			dateTime->microseconds = 0;
			dateTime->msec++;
			if (dateTime->msec > 999 ) {
				dateTime->msec = 0;
				dateTime->second++;
			}
		}
	}
#else
	tenMsecs = (unsigned int) ((utcTime->fractionOfSecond >> 11) * 1250L) >> 18;
	if ((tenMsecs % 10) >= 5) {
		dateTime->msec = (tenMsecs / 10) + 1;
		/* If rounding up from 999 mS, we would cause a value that may be undefined in upper layers */
		if (dateTime->msec == 1000) {
			MMSd_UnixToMdy(utcTime->secondOfCentury + 1, dateTime); //#MODIFJS
			dateTime->msec = 0;
		}
	} else {
		dateTime->msec = tenMsecs / 10;
	}
#endif
	return (1);
}

/*
 * Converts MMS time(days and milliseconds since 1/1/84)
 *    to UtcBtime (seconds and fraction since beginning of century)
 *
 *******************************************************************************/
int MMSd_MmsToUtc(MMSd_time *mmstime, MMSd_UtcBTime *utcbtime)
{
    unsigned long  secsSinceMidnight;

    unsigned long usec, temp;

    utcbtime->secondOfCentury = ( mmstime->daysSinceJan1_1984 + DAYS_OFFSET_1970 + 1 ) * SECS_PER_DAY;
    secsSinceMidnight = (unsigned long)(mmstime->millisecondsSinceMidnight / 1000);
    utcbtime->secondOfCentury += secsSinceMidnight;
    utcbtime->quality = 0x00;
    usec = ( mmstime->millisecondsSinceMidnight - (secsSinceMidnight * 1000) ) * 1000 ;
#if defined(MMSD_MICROSECONDS_TIMER)
	usec = usec + mmstime->microseconds;
#endif
    temp = ( ( usec * 1825 ) >> 5 ) + ( ( usec * 6 ) >> 12 );
    utcbtime->fractionOfSecond = (unsigned long)(( usec << 12 ) + ( usec << 8 ) - temp);

    return (1);
}

/*
 * Converts UtcBtime (seconds and fraction since beginning of century)
 *    to MMS time(days and milliseconds since 1/1/84)
 *
 *******************************************************************************/
int MMSd_UtcToMms(MMSd_UtcBTime *utcbtime, MMSd_time *mmstime)
{
    unsigned long secsSinceMidnight;
    unsigned long daysSince1970;
    unsigned long millisecondsSinceMidnight;
    unsigned long milliseconds;
#if defined(MMSD_MICROSECONDS_TIMER) 
	unsigned long fract_shifted12;
	unsigned long usec;
    unsigned long fractMsec;
#endif
	daysSince1970 = utcbtime->secondOfCentury / SECS_PER_DAY;
    secsSinceMidnight = utcbtime->secondOfCentury - (daysSince1970 * SECS_PER_DAY);
    mmstime->daysSinceJan1_1984 = (int) (daysSince1970 - DAYS_OFFSET_1970 - 1);
    milliseconds = (((utcbtime->fractionOfSecond >> 8) * 125L) >> 21);
    mmstime->millisecondsSinceMidnight = (secsSinceMidnight * 1000) + milliseconds;
	millisecondsSinceMidnight = (secsSinceMidnight * 1000);
#if !defined(MMSD_MICROSECONDS_TIMER) 
	if ( milliseconds )
	    milliseconds++;
    mmstime->millisecondsSinceMidnight = millisecondsSinceMidnight + milliseconds;

#else

    /* get Microseconds from the Fraction. See UTC to MDY for info.  */
    fract_shifted12 = (utcbtime->fractionOfSecond + 2048) >> 12;
    usec = ( (fract_shifted12 * 3906) + (fract_shifted12  >> 2) )  >> 12;
    fractMsec = usec / 1000;                /* get msec from this value */
    mmstime->millisecondsSinceMidnight = millisecondsSinceMidnight + fractMsec;
    mmstime->microseconds = (int)( usec - (fractMsec * 1000));    /* remove the msec already calc'd */
	if ( (utcbtime->fractionOfSecond + 2048) & 0x00000800 ) {
		mmstime->microseconds++;
		if ( mmstime->microseconds > 999 ) {
			mmstime->microseconds=0;
			mmstime->millisecondsSinceMidnight++;
		}
	}	
  
#endif
    return (1);
}

/*
 *
 *
 *******************************************************************************/
void MMSd_adjusted_utc_time_stamp(MMSd_UtcBTime *pUtcBtime)
{
    MMSd_time mmsTime;
    MMSd_adjusted_time_stamp( &mmsTime );
    MMSd_MmsToUtc( &mmsTime, pUtcBtime );
}

/*
 *
 *
 *******************************************************************************/
int MMSd_ParseUtcString(char *str,
                        MMSd_UtcBTime *utcVal)
{
    unsigned char q;
    int i;
    double fTemp;

    if (*str == '(')
        str++;
    while (*str == ' ')
        str++;
    if ((*str == 'u') || (*str == 'U'))
        str++;

    if (strchr( str, '/' ) == NULL) {
        unsigned char quality, mask;
        MMSd_time mmsTime;

        while (*str == ' ')
            str++;

        if ((strncmp( str, "today", 5 ) == 0) || (strncmp( str, "TODAY", 5 ) == 0) || (strncmp( str, "now", 5 ) == 0)
                || (strncmp( str, "NOW", 5 ) == 0)) {
            MMSd_time_stamp( &mmsTime );
            MMSd_MmsToUtc( &mmsTime, utcVal );
            str += 5;
        } else {
            utcVal->secondOfCentury = atol( str );
            while ((*str != '.') && (*str != 0))
                str++;
            if (*str != '.')
                return (0);
            fTemp = atof( str );
            utcVal->fractionOfSecond = ((long) (fTemp * 0x1000000)) << 8;
            while ((*str != ',') && (*str != 0))
                str++;
        }
        quality = 0;
        if (*str == ',') {
            while ((*str != '[') && (*str != 0))
                str++;
            if (*str == '[') {
                str++;
                mask = 0x80;
                for (i = 0; i < 8; i++) {
                    switch (str[i]) {
                    case '0':
                        break;
                    case '1':
                        quality |= mask;
                        break;
                    default:
                        i = 8;
                        break;
                    }
                    mask = (unsigned char) (mask >> 1);
                }
            }
        }
        utcVal->quality = quality;
    } else {
        MMSd_DateTime dateTime;

        if (!MMSd_AsciiToMdy( str, &dateTime ))
            return (0);

        MMSd_MdyToUtc( &dateTime, utcVal );
        while ((*str != ',') && (*str != 0))
            str++;

        if (*str == ')')
            return (1);

        if (*str != ',')
            return (0);
        str++;
        while (*str == ' ')
            str++;
        if (*str != '[')
            return (0);
        str++;
        q = 0;
        i = 0;
        while ((*str != 0) && (*str != ']')) {
            switch (*str) {
            case '0':
                q = (unsigned char) (q << 1);
                break;
            case '1':
                q = (unsigned char) (q << 1);
                q = (unsigned char) (q | 1);
                break;
            default:
                return (0);
            }
            i++;
            str++;
        }
        if (i < 8)
            q = (unsigned char) (q << (8 - i));
        utcVal->quality = q;
    }
    return (1);
}

/*
 *
 *
 *******************************************************************************/
int MMSd_DisplayUtcTime(MMSd_UtcBTime *utcVal,
                        char *str,
                        int len)
{
    double utcTime;
    char buffer[100];
    char quality[9];
    unsigned char mask;
    int lenStr;
    int i;

    utcTime = ((double) (utcVal->fractionOfSecond >> 8)) / ((double) 0x1000000);
    utcTime = utcTime + (double) utcVal->secondOfCentury;
    quality[0] = 0;
    mask = 0x80;
    for (i = 0; i < 8; i++) {
        if (utcVal->quality & mask)
            quality[i] = '1';
        else
            quality[i] = '0';
        mask = (unsigned char) (mask >> 1);
    }
    quality[8] = 0;
    sprintf( buffer, "(u%f,[%s])", utcTime, quality );
    lenStr = (int) strlen( buffer );
    if (len <= lenStr)
        lenStr = len - 1;
    strncpy( str, buffer, lenStr );
    str[lenStr] = 0;
    return (lenStr);
}

/*
 *
 *
 *******************************************************************************/
int MMSd_DisplayUtcString(MMSd_UtcBTime *utcVal,
                          char *str,
                          int len)
{
    MMSd_DateTime dateTime;
    char buffer[100];
    char quality[9];
    unsigned char mask;
    int lenStr;
    int i;

    strcpy( buffer, "(u" );
    MMSd_UtcToMdy( utcVal, &dateTime );
    MMSd_MdyToAscii( &dateTime, &buffer[2] );
    strcat( buffer, ",[" );

    quality[0] = 0;
    mask = 0x80;
    for (i = 0; i < 8; i++) {
        if (utcVal->quality & mask)
            quality[i] = '1';
        else
            quality[i] = '0';
        mask = (unsigned char) (mask >> 1);
    }
    quality[8] = 0;
    strcat( buffer, quality );
    strcat( buffer, "])" );

    lenStr = (int) strlen( buffer );
    if (len <= lenStr)
        lenStr = len - 1;
    strncpy( str, buffer, lenStr );
    str[lenStr] = 0;
    return (lenStr);
}

#endif //#include TIMEFUNC_C