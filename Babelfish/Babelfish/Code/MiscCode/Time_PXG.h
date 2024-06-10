/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_H
   #define TIME_H


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Time
{
	public:
		Time( void );

		void Get_Time_Stamp( uint8_t* dest );

		bool Set_Time_Using_Stamp( uint8_t* source );

		bool Set_Time( uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t days,
					   uint8_t months, uint16_t year );

		bool Valid( void );

		void Inc_Seconds( void );

	private:
		uint8_t m_second;
		uint8_t m_minute;
		uint8_t m_hour;
		uint8_t m_day;
		uint8_t m_month;
		uint16_t m_year;

		static bool Is_Leap_Year( int16_t year );

		static const uint8_t leap_year_months[2][13];

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Time() {}

};



#endif
