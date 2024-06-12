/**
 *	@file    osdep.c
 *	@version a90e925 (tag: 3-8-3-open)
 *
 *	POSIX layer.
 *		Mac OSX 10.5
 *		Linux
 */
/*
 *	Copyright (c) 2013-2016 INSIDE Secure Corporation
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from INSIDE at
 *	http://www.insidesecure.com/
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include "Includes.h"
#include "Timers_Lib.h"
#include "uC_Rand_Num.h"
#include "Device_Cert_Config.h"

#include "../coreApi.h"

#ifdef FREERTOS
/******************************************************************************/
/*
    Universal system POSIX headers and then PScore/coreApi.h
    OS-specific header includes should be added to PScore/osdep.h
 */

/******************************************************************************/
/*
    TIME FUNCTIONS
 */
#ifndef USE_HIGHRES_TIME
/******************************************************************************/
/*
    Module open and close
 */
int osdepTimeOpen( void )
{
	/*
	    Just a check that gettimeofday is functioning
	 */
	if ( BF_Lib::Timers::Get_Time() <= 0 )
	{
		return ( PS_FAILURE );
	}
	return ( PS_SUCCESS );
}

void osdepTimeClose( void )
{}

/*
    PScore Public API implementations

    This function always returns seconds/ticks AND OPTIONALLY populates
    whatever psTime_t happens to be on the given platform.
 */
int32 psGetTime( psTime_t*t, void*userPtr )
{
	int32 return_val = BF_Lib::Timers::Get_Time();

	if ( t != NULL )
	{
		*t = return_val;
	}
	
	return ( return_val );
}

//int32 psGetEpochTime( void )
//{
//	return ( CERT_GEN_EPOCH_TIME );
//}


int32 psDiffMsecs( psTime_t then, psTime_t now, void*userPtr )
{
	int32 return_val = 0;

	/*
	   if now is greater than then, return the difference(> 0) else return error in difference(<= 0)
	 */
	if ( now > then )
	{
		return_val = ( now - then );
	}
	else
	{
	 //MISRA compatible else
	}
	return ( return_val );
}

int32 psCompareTime( psTime_t a, psTime_t b, void*userPtr )
{
	int32 return_val = 0;

	/*
	    Time comparison.  1 if 'a' is less than or equal.  0 if 'a' is greater
	 */
	if ( a <= b )
	{
		return_val = 1;
	}
	else
	{
	//MISRA compatible else
	}

	return ( return_val );
}

#define YEAR0           1900                    /* the first year */
#define EPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY        ( 24L * 60L * 60L )
#define LEAPYEAR( year )  ( !( ( year ) % 4 ) && ( ( ( year ) % 100 ) || !( ( year ) % 400 ) ) )
#define YEARSIZE( year )  ( LEAPYEAR( year ) ? 366 : 365 )

const int _ytab[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

struct tm* gmtime_r( const time_t*timer, struct tm*tmbuf )
{
	time_t time = *timer;
	unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = ( unsigned long ) time % SECS_DAY;
	dayno = ( unsigned long ) time / SECS_DAY;

	tmbuf->tm_sec = dayclock % 60;
	tmbuf->tm_min = ( dayclock % 3600 ) / 60;
	tmbuf->tm_hour = dayclock / 3600;
	tmbuf->tm_wday = ( dayno + 4 ) % 7; //Day 0 was a thursday
	while ( dayno >= ( unsigned long ) YEARSIZE( year ) )
	{
		dayno -= YEARSIZE( year );
		year++;
	}
	tmbuf->tm_year = year - YEAR0;
	tmbuf->tm_yday = dayno;
	tmbuf->tm_mon = 0;
	while ( dayno >= ( unsigned long ) _ytab[LEAPYEAR( year )][tmbuf->tm_mon] )
	{
		dayno -= _ytab[LEAPYEAR( year )][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday = dayno + 1;
	tmbuf->tm_isdst = 0;
	return ( tmbuf );
}

#endif /* USE_HIGHRES_TIME */

/******************************************************************************/

#ifdef USE_MULTITHREADING
/******************************************************************************/
/*
    MUTEX FUNCTIONS
 */
/******************************************************************************/
static pthread_mutexattr_t attr;
/*
    Module open and close
 */
int osdepMutexOpen( void )
{
	int32 rc;

	if ( ( rc = pthread_mutexattr_init( &attr ) ) < 0 )
	{
		psErrorInt( "pthread_mutexattr_init failed %d\n", rc );
		return ( PS_PLATFORM_FAIL );
	}
	return ( PS_SUCCESS );
}

int osdepMutexClose( void )
{
	if ( pthread_mutexattr_destroy( &attr ) != 0 )
	{
		psTraceCore( "pthread_mutex_destroy failed\n" );
	}
	return ( PS_SUCCESS );
}

/*
    PScore Public API implementations
 */
int32 psCreateMutex( psMutex_t*mutex )
{
	int32 err;

	if ( ( err = pthread_mutex_init( mutex, &attr ) ) != 0 )
	{
		psTraceIntCore( "pthread_mutex_init failed %d\n", err );
		return ( PS_PLATFORM_FAIL );
	}
	return ( PS_SUCCESS );
}

int32 psLockMutex( psMutex_t*mutex )
{
	if ( pthread_mutex_lock( mutex ) != 0 )
	{
		psTraceCore( "pthread_mutex_lock failed\n" );
		return ( PS_PLATFORM_FAIL );
	}
	return ( PS_SUCCESS );
}

int32 psUnlockMutex( psMutex_t*mutex )
{
	if ( pthread_mutex_unlock( mutex ) != 0 )
	{
		psTraceCore( "pthread_mutex_unlock failed\n" );
		return ( PS_PLATFORM_FAIL );
	}
	return ( PS_SUCCESS );
}

void psDestroyMutex( psMutex_t*mutex )
{
	pthread_mutex_destroy( mutex );
}

#endif /* USE_MULTITHREADING */
/******************************************************************************/


/******************************************************************************/
/*
    ENTROPY FUNCTIONS
 */
/******************************************************************************/
#define MAX_RAND_READS      1024

/*
    Module open and close
 */
int osdepEntropyOpen( void )
{
	return ( PS_SUCCESS );
}

void osdepEntropyClose( void )
{}

/*
    PScore Public API implementations
 */
int32 psGetEntropy( unsigned char*bytes, uint32 size, void*userPtr )
{
	uC_Rand_Num::Get( ( uint8_t* ) bytes, size );
	return ( ( int32 )size );
}

/******************************************************************************/


/******************************************************************************/
/*
    RAW TRACE FUNCTIONS
 */
/******************************************************************************/
int osdepTraceOpen( void )
{
	return ( PS_SUCCESS );
}

void osdepTraceClose( void )
{}

void _psTrace( const char*msg )
{
	Term_Print( "%s", msg );
}

/* message should contain one %s, unless value is NULL */
void _psTraceStr( const char*message, const char*value )
{
	if ( value )
	{
		Term_Print( message, value );
	}
	else
	{
		Term_Print( "%s", message );
	}
}

/* message should contain one %d */
void _psTraceInt( const char*message, int32 value )
{
	Term_Print( message, value );
}

/* message should contain one %p */
void _psTracePtr( const char*message, const void*value )
{
	Term_Print( message, value );
}

/******************************************************************************/

#ifdef HALT_ON_PS_ERROR
/******************************************************************************/
/*
    POSIX - abort() on psError when HALT_ON_PS_ERROR enabled
 */
void osdepBreak( void )
{
	abort();
}

#endif /* HALT_ON_PS_ERROR */

#endif /* FREERTOS */
/******************************************************************************/
