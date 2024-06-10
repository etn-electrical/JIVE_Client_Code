/**
 *****************************************************************************************
 * @file	   	RTC_Example.cpp

 * @details    	This file contains test application for internal RTC.
 * @copyright  	2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "RTC_Example.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_RTC_App_Main( void )
{
	time_t now = 0;
	char strftime_buf[STRFTIME_BUF_SIZE];
	struct tm timeinfo;
	struct timeval nowset;
	int rc = 0;

	memset( &timeinfo, 0, sizeof( timeinfo ) );
	memset( strftime_buf, 0, STRFTIME_BUF_SIZE );
	nowset.tv_sec = EPOCH_TIME_SEC;		/* SNTP Sync option is not yet ported in the ESP32 code base,
										   setting to current time manually for testing purpose.*/
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

	while ( 1 )
	{
		time( &now );		///< fetching epoch time second.
		localtime_r( &now, &timeinfo );
		strftime( strftime_buf, sizeof( strftime_buf ), "%c", &timeinfo );
		printf( "Formatted date & time : %s\n", strftime_buf );
		vTaskDelay( 1000 / portTICK_RATE_MS );
	}
}

#ifdef __cplusplus
}
#endif

