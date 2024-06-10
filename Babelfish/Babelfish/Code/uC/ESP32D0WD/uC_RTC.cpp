/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "TimeCommon.h"
#include "POSIXTimeUtil.h"
#include "uC_RTC.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RTC::uC_RTC( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RTC::uC_RTC( RTC_clock_source_t clk_src, RTC_format_t format )
{
	/* In ESP32, RTC Clock source selection is configured through sdkconfig file
	   and also it doesn't required BCD format conversion. */
	uC_RTC_Init( clk_src, format );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::uC_RTC_Init( RTC_clock_source_t clk_src, RTC_format_t format )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Time( RTC_time_t* time )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Date( RTC_date_t* date )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Date_Time( RTC_date_t* date, RTC_time_t* time )
{
	struct timeval nowset;
	int rc = 0;
	uint32_t epoch_time;
	Time_Internal utc_date_time = { 0, 0, 0, 0, 1, 1, 0 };

	utc_date_time.second = time->seconds;
	utc_date_time.minute = time->minutes;
	utc_date_time.hour = time->hours;
	utc_date_time.day = date->date;
	utc_date_time.month = date->month;
	utc_date_time.year = date->year;


	Time_Conversion::POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( &utc_date_time, &epoch_time );

	nowset.tv_sec = epoch_time;
	rc = settimeofday( &nowset, NULL );
	if ( rc == 0 )
	{
		printf( "settimeofday() successful\n" );
	}
	else
	{
		printf( "settimeofday() failed\n" );
	}

	setenv( "TZ", "IST-05:30", 1 );	///< Set timezone to Indian Standard Time.
	tzset();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Synch_Time( uint32_t count )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Get_Time( RTC_time_t* time )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Get_Date( RTC_date_t* date )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_RTC::Get_Date_Time( RTC_date_t* d, RTC_time_t* t )
{
	time_t now = 0;
	struct tm timeinfo;

	time( &now );		///< fetching epoch time second.
	localtime_r( &now, &timeinfo );


	d->date = timeinfo.tm_mday;
	d->month = timeinfo.tm_mon + 1;		// tm_mon will have value from 0 -11
	d->year = timeinfo.tm_year % 100;	// tm_year will have value like 2021
	t->hours = timeinfo.tm_hour;
	t->minutes = timeinfo.tm_min;
	t->seconds = timeinfo.tm_sec;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_RTC::Is_Time_Recovered( void )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_RTC::RTC_Wait_For_Synchro( void )
{

	bool status = false;

	return ( status );
}
