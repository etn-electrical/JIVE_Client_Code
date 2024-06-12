/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_RTC.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_rtc.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline void uC_RTC::RTC_Write_Protection_Disable( void )
{
	m_RTC->WPR = 0xCA;
	m_RTC->WPR = 0x53;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline void uC_RTC::RTC_Write_Protection_Enable( void )
{
	m_RTC->WPR = 0xFF;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RTC::uC_RTC( RTC_clock_source_t clk_src, RTC_format_t format )
{
	m_RTC = RTC;
	m_time_recovered = true;

	/* Enable PWR peripheral clock */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/* disable backup domain write protection */
	PWR->CR1 |= PWR_CR1_DBP;

	uC_Delay( 2U );

	if ( clk_src == CLOCK_SOURCE_LSE )
	{
		/* Enable the LSE OSC */
		// __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
		__HAL_RCC_LSE_CONFIG( RCC_LSE_ON );
		/* Wait till LSE is ready */
		while ( ( RCC->BDCR & RCC_BDCR_LSERDY ) != RCC_BDCR_LSERDY )
		{}
		/* Select the RTC Clock Source */
		RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	}
	else if ( clk_src == CLOCK_SOURCE_LSI )
	{
		/* Enable the LSI Internal Clk */
		__HAL_RCC_RTC_CONFIG( RCC_RTCCLKSOURCE_LSI );
		/* Wait till LSI is ready */
		while ( ( RCC->CSR & RCC_CSR_LSIRDY ) != RCC_CSR_LSIRDY )
		{}

		/* Select the RTC Clock Source */
		RCC->BDCR |= RCC_BDCR_RTCSEL_1;
	}
	else if ( clk_src == CLOCK_SOURCE_HSE )
	{
		const uint32_t mega_div = ( CLOCK_FREQ / 1000000U );
		/* currently supported for 1 to 31 MHz ext crystal most likely used */
		if ( ( mega_div < 32U ) && ( mega_div > 0U ) )
		{
			/* divide apprpriately to get HSE clock out of 1 MHz */
			const uint32_t RCC_RTCCLKSource_HSE_Div = HSE_CLK_DIV_MASK | ( mega_div << 16U );
			/* Enable the HSE OSC */
			__HAL_RCC_RTC_CONFIG( RCC_RTCCLKSource_HSE_Div );
		}

		/* Wait till HSE is ready */
		while ( ( RCC->CR & RCC_CR_HSERDY ) != RCC_CR_HSERDY )
		{}
		/* Select the RTC Clock Source */
		RCC->BDCR |= RCC_BDCR_RTCSEL;
	}

	/* Enable the RTC Clock */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	uC_RTC_Init( clk_src, format );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::uC_RTC_Init( RTC_clock_source_t clk_src, RTC_format_t format )
{
	m_datetime_format = format;
	/* Disable the write protection for RTC registers */
	RTC_Write_Protection_Disable();

	Enter_Init();

	/* Clear RTC_CR FMT, OSEL and POL Bits */
	m_RTC->CR &=
		static_cast<uint32_t>( ~( RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL ) );

	/* Set RTC_CR register */
	// defaults are fine
	// m_RTC->CR |= (uint32_t)(HourFormat | hrtc->OutPut | hrtc->Init.Polarity);
	if ( clk_src == CLOCK_SOURCE_LSE )
	{
		// typically 32.768K ext crystal
		/* Configure the RTC PRER */
		m_RTC->PRER = static_cast<uint32_t>( LSE_LSI_PRER_REG_PREDIV_S );
		m_RTC->PRER |= static_cast<uint32_t>( LSE_LSI_PRER_REG_PREDIV_A << 16U );
	}
	else if ( clk_src == CLOCK_SOURCE_LSI )
	{
		// typically 32K internal osc
		/* Configure the RTC PRER */
		m_RTC->PRER = static_cast<uint32_t>( LSE_LSI_PRER_REG_PREDIV_S );
		m_RTC->PRER |= static_cast<uint32_t>( LSE_LSI_PRER_REG_PREDIV_A << 16U );
	}
	else if ( clk_src == CLOCK_SOURCE_HSE )
	{
		/* Configure the RTC PRER */
		m_RTC->PRER = static_cast<uint32_t>( HSE_PRER_REG_PREDIV_S );
		m_RTC->PRER |= static_cast<uint32_t>( HSE_PRER_REG_PREDIV_A << 16U );
	}

	/* Wait for RTC registers synchronization */
	RTC_Wait_For_Synchro();

	// check if backup register retain last initialised value
	if ( m_RTC->BKP0R != BKP_REG_RST_CHK )
	{
		m_time_recovered = false;
		m_RTC->BKP0R = BKP_REG_RST_CHK;
	}

	Exit_Init();

	/* Enable the write protection for RTC registers */
	RTC_Write_Protection_Enable();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Time( RTC_time_t* time )
{
	uint32_t time_reg = 0U;

	/* If Interface sets time in Binary format, then only following conversion will take place properly */
	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Time_To_BCDFormat( time );
	}
	time_reg = ( ( static_cast<uint32_t>( time->hours ) << 16U )
				 | ( static_cast<uint32_t>( time->minutes ) << 8U )
				 | ( static_cast<uint32_t>( time->seconds ) )
				 | ( static_cast<uint32_t>( time->timeformat ) << 16U ) );

	/* Disable the write protection for RTC registers */
	RTC_Write_Protection_Disable();

	Enter_Init();

	/* Set the RTC_TR register */
	m_RTC->TR = static_cast<uint32_t>( time_reg & RTC_TR_RESERVED_MASK );

	/* Clear the bits to be configured */
	m_RTC->CR &= static_cast<uint32_t>( ~RTC_CR_BCK );

	/* Configure the RTC_CR register */
	m_RTC->CR |= static_cast<uint32_t>( time->daylightsaving | time->storeoperation );

	Exit_Init();

	/* Enable the write protection for RTC registers */
	RTC_Write_Protection_Enable();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Date( RTC_date_t* date )
{
	uint32_t date_reg = 0U;

	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Date_To_BCDFormat( date );
	}
	date_reg = ( ( static_cast<uint32_t>( date->year ) << 16U ) |
				 ( static_cast<uint32_t>( date->month ) << 8U ) |
				 ( static_cast<uint32_t>( date->date ) ) |
				 ( static_cast<uint32_t>( date->weekday ) << 13U ) );

	/* Disable the write protection for RTC registers */
	RTC_Write_Protection_Disable();

	Enter_Init();

	/* Set the RTC_DR register */
	m_RTC->DR = static_cast<uint32_t>( date_reg & RTC_DR_RESERVED_MASK );

	Exit_Init();

	/* Enable the write protection for RTC registers */
	RTC_Write_Protection_Enable();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Set_Date_Time( RTC_date_t* date, RTC_time_t* time )
{
	uint32_t date_reg = 0U;
	uint32_t time_reg = 0U;
	uint32_t sub_sec_reg = 0U;

	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Date_To_BCDFormat( date );
		Convert_Time_To_BCDFormat( time );
	}
	date_reg = ( ( static_cast<uint32_t>( date->year ) << 16U ) |
				 ( static_cast<uint32_t>( date->month ) << 8U ) |
				 ( static_cast<uint32_t>( date->date ) ) |
				 ( static_cast<uint32_t>( date->weekday ) << 13U ) );

	time_reg = ( ( static_cast<uint32_t>( time->hours ) << 16U ) |
				 ( static_cast<uint32_t>( time->minutes ) << 8U ) |
				 ( static_cast<uint32_t>( time->seconds ) ) |
				 ( static_cast<uint32_t>( time->timeformat ) << 16U ) );

	// subseconds range from 0 to 999999. Convert subseconds into rtc register count
	// Second fraction = ( PREDIV_S - rtc->SSR ) / ( PREDIV_S + 1 )
	sub_sec_reg = Round_Div( time->subseconds, RTC_COUNT_TO_SUBSECOND_SCALE );
	sub_sec_reg = LSE_LSI_PRER_REG_PREDIV_S - sub_sec_reg;

	/* Disable the write protection for RTC registers */
	RTC_Write_Protection_Disable();

	Enter_Init();

	/* Set the RTC_DR register */
	m_RTC->DR = static_cast<uint32_t>( date_reg & RTC_DR_RESERVED_MASK );

	/* Set the RTC_TR register */
	m_RTC->TR = static_cast<uint32_t>( time_reg & RTC_TR_RESERVED_MASK );

	/* Clear the bits to be configured */
	m_RTC->CR &= static_cast<uint32_t>( ~RTC_CR_BCK );

	/* Configure the RTC_CR register */
	m_RTC->CR |= static_cast<uint32_t>( time->daylightsaving | time->storeoperation );

	Exit_Init();

	/* Enable the write protection for RTC registers */
	RTC_Write_Protection_Enable();

	/* Synchronize rtc with subsecond count */
	Synch_Time( sub_sec_reg );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Synch_Time( uint32_t count )
{
	/* Disable the write protection for RTC registers */
	RTC_Write_Protection_Disable();

	while ( ( m_RTC->ISR & RTC_ISR_SHPF ) != 0U )
	{}
	m_RTC->SHIFTR = ( ( RTC_SHIFTR_ADD1S ) | count );

	/* Enable the write protection for RTC registers */
	RTC_Write_Protection_Enable();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Get_Time( RTC_time_t* time )
{
	volatile uint32_t temp = 0U;
	uint32_t time_reg = 0U;

	/* Wait for shadow register synchronization complete */
	while ( ( m_RTC->ISR & RTC_ISR_RSF ) == 0U )
	{}

	/* Reading both registers to get correct data from shadow register */
	time_reg = m_RTC->TR & RTC_TR_RESERVED_MASK;
	temp = m_RTC->DR;

	time->timeformat = ( time_reg & RTC_TR_PM ) >> 16U;
	time->hours = ( time_reg & ( RTC_TR_HT | RTC_TR_HU ) ) >> 16U;
	time->minutes = ( time_reg & ( RTC_TR_MNT | RTC_TR_MNU ) ) >> 8U;
	time->seconds = ( time_reg & ( RTC_TR_ST | RTC_TR_SU ) );

	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Time_To_BinaryFormat( time );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Get_Date( RTC_date_t* date )
{
	volatile uint32_t temp = 0U;
	uint32_t date_reg = 0U;

	/* Wait for shadow register synchronization complete */
	while ( ( m_RTC->ISR & RTC_ISR_RSF ) == 0U )
	{}

	/* Reading both registers to get correct data from shadow register */
	temp = m_RTC->TR;
	date_reg = m_RTC->DR & RTC_DR_RESERVED_MASK;

	date->year = ( date_reg & ( RTC_DR_YT | RTC_DR_YU ) ) >> 16U;
	date->weekday = ( date_reg & ( RTC_DR_WDU ) ) >> 13U;
	date->month = ( date_reg & ( RTC_DR_MT | RTC_DR_MU ) ) >> 8U;
	date->date = ( date_reg & ( RTC_DR_DT | RTC_DR_DU ) );

	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Date_To_BinaryFormat( date );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_RTC::Get_Date_Time( RTC_date_t* date, RTC_time_t* time )
{
	uint32_t time_reg = 0U;
	uint32_t date_reg = 0U;
	uint32_t sub_second_reg = 0U;

	/* Wait for shadow register synchronization complete */
	while ( ( m_RTC->ISR & RTC_ISR_RSF ) == 0U )
	{}

	/* Reading both registers to get correct data from shadow register */
	sub_second_reg = m_RTC->SSR & RTC_SSR_RESERVED_MASK;
	time_reg = m_RTC->TR & RTC_TR_RESERVED_MASK;
	date_reg = m_RTC->DR & RTC_DR_RESERVED_MASK;

	// sub_second_reg range from 0 to PREDIV_S-1
	// time->subseconds range from 0 to 999999
	if ( sub_second_reg > LSE_LSI_PRER_REG_PREDIV_S )
	{
		sub_second_reg = sub_second_reg - LSE_LSI_PRER_REG_PREDIV_S;
		time->subseconds = ( sub_second_reg * RTC_COUNT_TO_SUBSECOND_SCALE ) + 1000000U;
	}
	else
	{
		sub_second_reg = LSE_LSI_PRER_REG_PREDIV_S - sub_second_reg;
		time->subseconds = sub_second_reg * RTC_COUNT_TO_SUBSECOND_SCALE;
	}

	time->timeformat = ( time_reg & RTC_TR_PM ) >> 16U;
	time->hours = ( time_reg & ( RTC_TR_HT | RTC_TR_HU ) ) >> 16U;
	time->minutes = ( time_reg & ( RTC_TR_MNT | RTC_TR_MNU ) ) >> 8U;
	time->seconds = ( time_reg & ( RTC_TR_ST | RTC_TR_SU ) );

	date->year = ( date_reg & ( RTC_DR_YT | RTC_DR_YU ) ) >> 16U;
	date->weekday = ( date_reg & ( RTC_DR_WDU ) ) >> 13U;
	date->month = ( date_reg & ( RTC_DR_MT | RTC_DR_MU ) ) >> 8U;
	date->date = ( date_reg & ( RTC_DR_DT | RTC_DR_DU ) );

	if ( m_datetime_format == RTC_BCD_FORMAT )
	{
		Convert_Time_To_BinaryFormat( time );
		Convert_Date_To_BinaryFormat( date );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Enter_Init( void )
{
	/* Set the Initialization mode */
	m_RTC->ISR |= RTC_ISR_INIT;

	// wait for Initialization complete
	while ( ( m_RTC->ISR & RTC_ISR_INITF ) == 0U )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Exit_Init( void )
{
	/* Exit Initialization mode */
	m_RTC->ISR &= static_cast<uint32_t>( ~RTC_ISR_INIT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_RTC::Is_Time_Recovered( void )
{
	return ( m_time_recovered );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Convert_Date_To_BCDFormat( RTC_date_t* date )
{
	date->date = Byte_To_Bcd( date->date );
	date->month = Byte_To_Bcd( date->month );
	date->year = Byte_To_Bcd( date->year );
	// weekday conversion not needed, since value will be less than 10
	// date->weekday = RTC_ByteToBcd(date->year);
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Convert_Time_To_BCDFormat( RTC_time_t* time )
{
	time->hours = Byte_To_Bcd( time->hours );
	time->minutes = Byte_To_Bcd( time->minutes );
	time->seconds = Byte_To_Bcd( time->seconds );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Convert_Date_To_BinaryFormat( RTC_date_t* date )
{
	date->date = Bcd_To_Byte( date->date );
	date->month = Bcd_To_Byte( date->month );
	date->year = Bcd_To_Byte( date->year );
	// weekday conversion not needed, since value will be less than 10
	// date->weekday = Bcd_To_Byte(date->year);
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RTC::Convert_Time_To_BinaryFormat( RTC_time_t* time )
{
	time->hours = Bcd_To_Byte( time->hours );
	time->minutes = Bcd_To_Byte( time->minutes );
	time->seconds = Bcd_To_Byte( time->seconds );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_RTC::Byte_To_Bcd( uint8_t value )
{
	uint8_t bcdhigh = 0U;

	while ( value >= 10U )
	{
		bcdhigh++;
		value -= 10U;
	}

	return ( static_cast<uint8_t>( bcdhigh << 4U ) | value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_RTC::Bcd_To_Byte( uint8_t value )
{
	uint8_t tmp = 0U;

	tmp = static_cast<uint8_t>( ( ( value & static_cast<uint8_t>( 0xF0 ) )
								  >> static_cast<uint8_t>( 0x4 ) ) * 10U );
	return ( tmp + ( value & static_cast<uint8_t>( 0x0F ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_RTC::RTC_Wait_For_Synchro( void )
{
	volatile uint32_t synchrocounter = 0U;
	bool status = false;
	uint32_t synchrostatus = 0x00;

	/* Clear RSF flag */
	m_RTC->ISR &= static_cast<uint32_t>( RTC_RSF_MASK );

	/* Wait the registers to be synchronised */
	do
	{
		synchrostatus = m_RTC->ISR & RTC_ISR_RSF;
		synchrocounter++;
	} while ( ( synchrocounter < SYNCHRO_TIMEOUT ) && ( synchrostatus == 0x00 ) );

	if ( ( m_RTC->ISR & RTC_ISR_RSF ) != RESET )
	{
		status = true;
	}
	else
	{
		status = false;
	}
	return ( status );
}
