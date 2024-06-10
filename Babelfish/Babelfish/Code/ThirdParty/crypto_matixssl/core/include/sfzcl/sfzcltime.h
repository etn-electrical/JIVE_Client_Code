/* sfzcltime.h

   Calendar time retrieval and manipulation.
 */

/*****************************************************************************
* Copyright (c) 2006-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef SFZCLTIME_H
#define SFZCLTIME_H

typedef int64_t SfzclTime;

typedef struct SfzclCalendarTimeRec
{
    uint8_t second;             /* 0-61 */
    uint8_t minute;             /* 0-59 */
    uint8_t hour;               /* 0-23 */
    uint8_t monthday;           /* 1-31 */
    uint8_t month;              /* 0-11 */
    int32_t year;               /* Absolute value of year.  1999=1999. */
    uint8_t weekday;            /* 0-6, 0=sunday */
    uint16_t yearday;           /* 0-365 */
    int32_t utc_offset;         /* Seconds from UTC (positive=east) */
    bool dst;                   /* FALSE=non-DST, TRUE=DST */
} *SfzclCalendarTime, SfzclCalendarTimeStruct;

typedef struct SfzclTimeValueRec
{
    int64_t seconds;
    int64_t microseconds;
} *SfzclTimeValue, SfzclTimeValueStruct;

/* Returns seconds from epoch "January 1 1970, 00:00:00 UTC".  */
SfzclTime sfzcl_time(void);

/* Fills the calendar structure according to ``current_time''. */
void sfzcl_calendar_time(SfzclTime current_time,
                         SfzclCalendarTime calendar_ret, bool local_time);

/* Return time string in RFC-2550 compatible format.  Returned string
   is allocated with SPAL_Memory_Alloc and has to be freed with SPAL_Memory_Free by
   the caller. */
char *sfzcl_time_string(SfzclTime input_time);

/* Format time string in RFC-2550 compatible format as snprintf renderer. The
   datum points to the SfzclTime. */
int sfzcl_time_render(unsigned char *buf, int buf_size, int precision,
                      void *datum);

/* Format time string in RFC-2550 compatible format as snprintf renderer. The
   datum points to the memory buffer having the 32-bit long time in seconds
   from the epoch in the network byte order. */
int sfzcl_time32buf_render(unsigned char *buf, int buf_size, int precision,
                           void *datum);

/* Return a time string that is formatted to be more or less human
   readable.  It is somewhat like the one returned by ctime(3) but
   contains no newline in the end.  Returned string is allocated with
   SPAL_Memory_Alloc and has to be freed with SPAL_Memory_Free by the caller. */
char *sfzcl_readable_time_string(SfzclTime input_time, bool local_time);

/* Convert SfzclCalendarTime to SfzclTime. If the dst is set to TRUE then
   daylight saving time is assumed to be set, if dst field is set to FALSE
   then it is assumed to be off. It if it is set to -1 then the function tries
   to find out if the dst was on or off at the time given.

   Weekday and yearday fields are ignored in the conversion, but filled with
   approriate values during the conversion. All other values are normalized to
   their normal range during the conversion.

   If the local_time is set to TRUE then dst and utc_offset values
   are ignored.

   If the time cannot be expressed as SfzclTime this function returns FALSE,
   otherwise returns TRUE. */
bool sfzcl_make_time(SfzclCalendarTime calendar_time,
                     SfzclTime *time_return, bool local_time);

#endif                          /* SFZCLTIME_H */

/* eof (sfzcltime.h) */
