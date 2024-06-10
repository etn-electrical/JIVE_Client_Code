/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Time.h"
#include "Exception.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
const uint8_t Time::leap_year_months[2][13] =
{
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Time::Time( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Time::Get_Time_Stamp( uint8_t* dest )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Time::Set_Time_Using_Stamp( uint8_t* source )
{
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Time::Is_Leap_Year( int16_t year )
{
	return ( ( ( year / 4 ) == 0 ) &&
			 ( ( year / 100 ) != 0 ) ||
			 ( 0 < year ) &&
			 ( ( year / 400 ) == 100 ) ||
			 ( year < 0 ) &&
			 ( ( -year / 400 ) == 300 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Time::Valid( void )
{
	bool time_valid = true;
	uint8_t leap_year_index;

	leap_year_index = Is_Leap_Year( m_year );
	if ( ( m_second >= 60 ) ||
		 ( m_minute >= 60 ) ||
		 ( m_hour >= 24 ) ||
		 ( m_day > 7 ) ||
		 ( m_month > 12 ) ||
		 ( m_day > leap_year_months[leap_year_index][m_month] ) )
	{
		time_valid = false;
	}
	return ( time_valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Time::Inc_Seconds( void )
{
	uint8_t leap_year_index;

	m_second++;
	if ( m_second == 60 )
	{
		m_second = 0;
		m_minute++;
		if ( m_minute == 60 )
		{
			m_minute = 0;
			m_hour++;
			if ( m_hour == 24 )
			{
				m_hour = 0;

				leap_year_index = Is_Leap_Year( m_year );
				m_day++;
				if ( m_day == leap_year_months[leap_year_index][m_month] )
				{
					m_day = 1;
					m_month++;
					if ( m_month == 13 )
					{
						m_month = 1;
						m_year++;
					}
				}
			}
		}
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Time::Set_Time( uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days,
					 uint8_t months, uint16_t year )
{
	m_second = seconds;
	m_minute = minutes;
	m_hour = hours;
	m_day = days;
	m_month = months;
	m_year = year;

	return ( Valid() );
}
