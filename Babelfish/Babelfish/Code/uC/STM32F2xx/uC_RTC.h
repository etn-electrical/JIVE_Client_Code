/**
 **********************************************************************************************
 * @file            uC_RTC.h Header File for RTC Driver implementation.
 *
 * @brief           The file contains uC_RTC Class required for accessing internal RTC time and
 * date.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef UC_RTC_H
#define UC_RTC_H

#include "Includes.h"

/**
 ****************************************************************************************
 * @brief Class to access RTC date time in BCD as well as binary format.
 *
 * @details RTC initialation expects RTC clock source and RTC_format_t as input
 *          and there after sets and gets date-time in same format. It also checks whether
 *          last time is recovered, otherwise resets the date time registers.
 *          User must not create multiple instances of uC_RTC.
 ****************************************************************************************
 */

class uC_RTC
{
	public:
		/**
		 * @brief RTC_format_t RTC Input Parameter Format Definitions.
		 */
		enum RTC_format_t
		{
			RTC_BIN_FORMAT,	///< Time and Date in Binary format for eg - 1 Nov 2016 to dd-01, mm-11,
							///< yy-16
			RTC_BCD_FORMAT	///< Time and Date in BCD format for eg - 1 Nov 2016 to dd-0x01,
							///< mm-0x11, yy-0x16
		};

		/**
		 * @brief RTC_clock_source_t RTC Input clock source Definitions.
		 */
		enum RTC_clock_source_t
		{
			CLOCK_SOURCE_LSE,	///< low speed external crystal (Typically 32.7K Hz)
			CLOCK_SOURCE_LSI,	///< low speed internal osc (Typically 32.7K Hz)
			CLOCK_SOURCE_HSE///< High speed External crystal (supports 1 MHz to 31 MHz External
			// crystal)
		};

		/**
		 * @brief Default Constructor a RTC interface.
		 * @param
		 * @param
		 */
		uC_RTC( void );

		/**
		 * @brief Parameterised Constructor a RTC interface.
		 * @param  clk_src: Clock source required to be used by the RTC
		 * @param  format: Format of Time and Date to be used
		 */
		uC_RTC( RTC_clock_source_t clk_src,
				RTC_format_t format = RTC_BCD_FORMAT );

		/**
		 * @brief The destructor.
		 */
		~uC_RTC( void );

		/**
		 * @brief  RTC Time structure definition
		 */
		struct RTC_time_t
		{
			uint8_t hours;		/*!< Specifies the RTC Time Hour.
								   This parameter must be a number between Min_Data = 0 and Max_Data
								      = 12 if the RTC_HourFormat_12 is selected.
								   This parameter must be a number between Min_Data = 0 and Max_Data
								      = 23 if the RTC_HourFormat_24 is selected  */

			uint8_t minutes;	/*!< Specifies the RTC Time Minutes.
								   This parameter must be a number between Min_Data = 0 and
								      Max_Data = 59 */

			uint8_t seconds;	/*!< Specifies the RTC Time Seconds.
								   This parameter must be a number between Min_Data = 0 and
								      Max_Data = 59 */

			uint32_t subseconds;	/*!< Specifies the RTC Time SubSeconds in micro seconds.
									   This parameter must be a number between Min_Data = 0 and
									      Max_Data = 999999 */

			uint8_t timeformat;		/*!< Specifies the RTC AM/PM Time.
									   This parameter can be a value of @ref RTC_AM_PM_Definitions
									 */

			uint32_t daylightsaving;	/*!< Specifies DayLight Save Operation.
										   This parameter can be a value of @ref
										      RTC_DayLightSaving_Definitions */

			uint32_t storeoperation;	/*!< Specifies RTC_StoreOperation value to be written in
										   the BCK bit
										   in CR register to store the operation.
										   This parameter can be a value of @ref
										      RTC_StoreOperation_Definitions */
		};

		/**
		 * @brief RTC Date structure definition
		 */
		struct RTC_date_t
		{
			uint8_t weekday;	/*!< Specifies the RTC Date WeekDay.
								   This parameter can be a value of @ref RTC_WeekDay_Definitions */

			uint8_t month;		/*!< Specifies the RTC Date Month.
								   This parameter can be a value of @ref RTC_Month_Date_Definitions
								 */

			uint8_t date;		/*!< Specifies the RTC Date.
								   This parameter must be a number between Min_Data = 1 and Max_Data =
								      31 */

			uint8_t year;		/*!< Specifies the RTC Date Year.
								   This parameter must be a number between Min_Data = 0 and Max_Data =
								      99 */
		};

		/**
		 * @brief  Sets RTC current time.
		 * @param  Time: Pointer to Time structure
		 * @retval none
		 */
		void Set_Time( RTC_time_t* Time );

		/**
		 * @brief  Sets RTC current date.
		 * @param  Date: Pointer to date structure
		 * @retval none
		 */
		void Set_Date( RTC_date_t* Date );

		/**
		 * @brief   Sets RTC current date and time.
		 * @param  date: Pointer to date structure
		 * @param  time: Pointer to time structure
		 * @retval none
		 */
		void Set_Date_Time( RTC_date_t* date, RTC_time_t* time );

		/**
		 * @brief  Gets RTC current time.
		 * @param  Time: Pointer to Time structure
		 * @retval none
		 */
		void Get_Time( RTC_time_t* Time );

		/**
		 * @brief  Gets RTC current date.
		 * @param  Date: Pointer to date structure
		 * @retval none
		 */
		void Get_Date( RTC_date_t* Date );

		/**
		 * @brief  Gets RTC current date and time.
		 * @param  Date: Pointer to date structure
		 * @param  Date: Pointer to time structure
		 * @retval none
		 */
		void Get_Date_Time( RTC_date_t* date, RTC_time_t* time );

		/**
		 * @brief  returns status if backup registers not reset after powercycle
		 * @retval true if recovered else false
		 */
		bool Is_Time_Recovered( void );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		uC_RTC( const uC_RTC& rhs );
		uC_RTC & operator =( const uC_RTC& object );

		void uC_RTC_Init( RTC_clock_source_t clk_src, RTC_format_t format );

		bool RTC_Wait_For_Synchro( void );

		void Convert_Date_To_BCDFormat( RTC_date_t* date );

		void Convert_Time_To_BCDFormat( RTC_time_t* time );

		void Convert_Date_To_BinaryFormat( RTC_date_t* date );

		void Convert_Time_To_BinaryFormat( RTC_time_t* time );

		static uint8_t Byte_To_Bcd( uint8_t value );

		static uint8_t Bcd_To_Byte( uint8_t value );

		void Enter_Init( void );

		void Exit_Init( void );

		/**
		 * @brief  Enable the write protection for RTC registers.
		 * @retval None
		 */
		inline void RTC_Write_Protection_Enable( void );

		/**
		 * @brief  Enable the write protection for RTC registers.
		 * @retval None
		 */
		inline void RTC_Write_Protection_Disable( void );

		/**
		 * @brief RTC registers Masks Definition
		 */
		static const uint32_t RTC_TR_RESERVED_MASK = static_cast<uint32_t>( 0x007F7F7F );
		static const uint32_t RTC_DR_RESERVED_MASK = static_cast<uint32_t>( 0x00FFFF3F );
		static const uint32_t RTC_SSR_RESERVED_MASK = static_cast<uint32_t>( 0x0000FFFF );
		static const uint32_t RTC_RSF_MASK = static_cast<uint32_t>( 0xFFFFFF5F );

		/**
		 * @brief RTC registers synchronous timeoput
		 */
		static const uint32_t SYNCHRO_TIMEOUT = static_cast<uint32_t>( 0x00020000 );

		/**
		 * @brief To check magic number to identify if RTC backedup after powercycle
		 */
		static const uint32_t BKP_REG_RST_CHK = static_cast<uint32_t>( 0x4472 );

		/**
		 * @brief Mask to select appropriate HSE clock divider
		 */
		static const uint32_t HSE_CLK_DIV_MASK = static_cast<uint32_t>( 0x00000300 );

		/**
		 * @brief RTC_PRER register's PRE_DIV Synchronous and Asynchronous values.
		 *        Considers 1MHz HSE input(after divided by RCC_RTCCLKSource_HSE_Div) or 32.7 KHz
		 * LSE/LSI
		 */
		static const uint32_t HSE_PRER_REG_PREDIV_A = static_cast<uint32_t>( 100U - 1 );
		static const uint32_t HSE_PRER_REG_PREDIV_S = static_cast<uint32_t>( 10000U - 1 );
		static const uint32_t LSE_LSI_PRER_REG_PREDIV_A = static_cast<uint32_t>( 16U - 1 );
		static const uint32_t LSE_LSI_PRER_REG_PREDIV_S = static_cast<uint32_t>( 2048U - 1 );

		/**
		 * @brief Scaling constants required for conversion betweeb subsecond and rtc count.
		 */
		static const uint32_t RTC_COUNT_TO_SUBSECOND_SCALE =
			static_cast<uint32_t>( 1000000U / ( LSE_LSI_PRER_REG_PREDIV_S + 1 ) );

		RTC_TypeDef* m_RTC;

		bool m_time_recovered;

		RTC_format_t m_datetime_format;
};

#endif
