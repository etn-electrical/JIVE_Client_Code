/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */


 #include "uC_PTP_Slave.h"

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_PTP_Slave::uC_PTP_Slave( uint8_t node_type, uint8_t update_method, uint8_t pkt_snooping )
{
	// __ETHMACPTP_CLK_ENABLE();

	/*select node type */
	// if ( node_type == ORDINARY_CLOCK )
	// {
	// ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFCFFFF ) );
	// }
	// else if ( node_type == BOUNDARY_CLOCK )
	// {
	// ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFEFFFF ) );
	// }
	// else if ( node_type == E2E_TC_CLOCK )
	// {
	// ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFDFFFF ) );
	// }
	// else if ( node_type == P2P_TC_CLOCK )
	// {
	// ETH->PTPTSCR |= ETH_PTPTSCR_TSCNT;
	// }

	/* packet snooping type*/
	if ( pkt_snooping == PTP_VER_1 )
	{
		ETH->MACTSCR &= ( ~( ETH_MACTSCR_TSVER2ENA ) );
	}
	else
	{
		ETH->MACTSCR |= ETH_MACTSCR_TSVER2ENA;
	}

	/* snap shot type selection */
	ETH->MACTSCR |= ( ETH_MACTSCR_SNAPTYPSEL & ( 0xFFFDFFFF ) );
	ETH->MACTSCR &= ( ~( ETH_MACTSCR_TSEVNTENA ) );

	/* time stamp for event message*/
	ETH->MACTSCR |= ETH_MACTSCR_TSEVNTENA;

	/* time stamp snapshot for master relevant message*/
	ETH->MACTSCR |= ETH_MACTSCR_TSMSTRENA;

	/* Time stamp subsecond rollover */
	ETH->MACTSCR |= ETH_MACTSCR_TSCTRLSSR;
	Start_Initialization( update_method );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Start_Initialization( uint8_t update_method )
{
	/* Mask the Time stamp trigger interrupt by setting bit 9 in the MACIMR register. */
	ETHMACIT_Config( ETH_MACIER_TSIE, ENABLE );

	/* Program Time stamp register bit 0 to enable time stamping. */
	Time_Stamp_Cmd( ENABLE );

	/* Program the Subsecond increment register based on the PTP clock frequency.
	   to achieve 20 ns accuracy, the value is ~ 43
	 */
	Set_Subsecond_Increment( ADJ_FREQ_BASE_INCREMENT );

	if ( update_method == ETH_PTP_FineUpdate )
	{
		/* If you are using the Fine correction method, program the Time stamp addend register
		 * and set Time stamp control register bit 5 (addend register update). */
		Set_Time_Stamp_Addend_Value( ADJ_FREQ_BASE_ADDEND );
		Enable_Time_Stamp_Addend();

		/* Poll the Time stamp control register until bit 5 is cleared. */
		while ( Get_TSCR_Flag_Status( ETH_MACTSCR_TSADDREG ) == SET )
		{}
	}

	/* To select the Fine correction method (if required),
	 * program Time stamp control register  bit 1. */
	Update_Method_Config( update_method );

	/* Program the Time stamp high update and Time stamp low update registers
	 * with the appropriate time value. */
	Set_Time_Stamp_Update( ETH_PTP_PositiveTime, 0, 0 );

	/* Set Time stamp control register bit 2 (Time stamp init). */
	TSCR_Initialize_Time_Stamp_Bit();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::ETHMACIT_Config( uint32_t eth_mac_int, FunctionalState state )
{
	if ( state != DISABLE )
	{
		/* Enable the selected ETHERNET MAC interrupts */
		ETH->MACIER &= ( ~( uint32_t )eth_mac_int );
	}
	else
	{
		/* Disable the selected ETHERNET MAC interrupts */
		ETH->MACIER |= eth_mac_int;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Time_Stamp_Cmd( FunctionalState state )
{
	if ( state != DISABLE )
	{
		/* Enable the PTP time stamp for transmit and receive frames */
		ETH->MACTSCR |= ETH_MACTSCR_TSENA | ETH_MACTSCR_TSIPV4ENA | ETH_MACTSCR_TSIPV6ENA |
			ETH_MACTSCR_TSIPENA;
	}
	else
	{
		/* Disable the PTP time stamp for transmit and receive frames */
		ETH->MACTSCR &= ( ~( uint32_t )ETH_MACTSCR_TSENA );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Set_Subsecond_Increment( uint32_t subsecondvalue )
{
	/* Set the PTP Sub-Second Increment Register */
	ETH->MACSSIR = subsecondvalue << 16U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Set_Time_Stamp_Addend_Value( uint32_t addend_val )
{
	/* Set the PTP Time Stamp Addend Register */
	ETH->MACTSAR = addend_val;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Enable_Time_Stamp_Addend( void )
{
	/* Enable the PTP block update with the Time Stamp Addend register value */
	ETH->MACTSCR |= ETH_MACTSCR_TSADDREG;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus uC_PTP_Slave::Get_TSCR_Flag_Status( uint32_t ptp_flag )
{
	FlagStatus bitstatus = RESET;

	if ( ( ETH->MACTSCR & ptp_flag ) != ( uint32_t )RESET )
	{
		bitstatus = SET;
	}
	else
	{
		bitstatus = RESET;
	}
	return ( bitstatus );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Update_Method_Config( uint32_t update_method )
{
	if ( update_method != ETH_PTP_CoarseUpdate )
	{
		/* Enable the PTP Fine Update method */
		ETH->MACTSCR |= ETH_MACTSCR_TSCFUPDT;
	}
	else
	{
		/* Disable the PTP Coarse Update method */
		ETH->MACTSCR &= ( ~( uint32_t )ETH_MACTSCR_TSCFUPDT );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Set_Time_Stamp_Update( uint32_t sign, uint32_t secondValue,
										  uint32_t subSecondValue )
{
	/* Set the PTP Time Update High Register */
	ETH->MACSTSUR = secondValue;

	/* Set the PTP Time Update Low Register with sign */
	ETH->MACSTNUR = sign | subSecondValue;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::TSCR_Initialize_Time_Stamp_Bit( void )
{
	/* Initialize the PTP Time Stamp */
	ETH->MACTSCR |= ETH_MACTSCR_TSINIT;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Get_System_Time( struct ptptime_t* timestamp )
{
	timestamp->tv_nsec = Subsecond_To_Nanosecond( Get_Register_Value( ETH_MACSTNR ) );
	timestamp->tv_sec = Get_Register_Value( ETH_MACSTSR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Set_Time( struct ptptime_t* timestamp )
{
	uint32_t sign = 0U;
	uint32_t second_value = 0U;
	uint32_t nanosecond_value = 0U;
	uint32_t subsecond_value = 0U;

	/* determine sign and correct Second and Nanosecond values */
	if ( ( timestamp->tv_sec < 0 ) || ( ( timestamp->tv_sec == 0 ) && ( timestamp->tv_nsec < 0 ) ) )
	{
		sign = ETH_PTP_NegativeTime;
		second_value = -timestamp->tv_sec;
		nanosecond_value = -timestamp->tv_nsec;
	}
	else
	{
		sign = ETH_PTP_PositiveTime;
		second_value = timestamp->tv_sec;
		nanosecond_value = timestamp->tv_nsec;
	}

	/* convert nanosecond to subseconds */
	subsecond_value = Nanosecond_To_Subsecond( nanosecond_value );

	/* Write the offset (positive or negative) in the Time stamp update high and low registers. */
	Set_Time_Stamp_Update( sign, second_value, subsecond_value );

	/* Set Time stamp control register bit 2 (Time stamp init). */
	TSCR_Initialize_Time_Stamp_Bit();

	/* The Time stamp counter starts operation as soon as it is initialized
	 * with the value written in the Time stamp update register. */
	while ( Get_TSCR_Flag_Status( ETH_MACTSCR_TSINIT ) == SET )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_PTP_Slave::Nanosecond_To_Subsecond( uint32_t subsecondvalue )
{
	uint64_t val = static_cast<uint64_t>( subsecondvalue ) * nanosecond_to_subsecond_multiplier;

	val /= nanosecond_to_subsecond_conv;
	return ( static_cast<uint32_t>( val ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_PTP_Slave::Subsecond_To_Nanosecond( uint32_t subsecondvalue )
{
	uint64_t val = static_cast<uint64_t>( subsecondvalue ) * nanosecond_to_subsecond_conv;

	val >>= shift_val;
	return ( val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_PTP_Slave::Get_Register_Value( uint32_t ptp_reg )
{
	/* Return the selected register value */
	uint32_t value = 0U;

	if ( ptp_reg == ETH_MACTSCR )
	{
		value = ETH->MACTSCR;
	}
	else if ( ptp_reg == ETH_MACSSIR )
	{
		value = ETH->MACSSIR;
	}
	else if ( ptp_reg == ETH_MACSTSR )
	{
		value = ETH->MACSTSR;
	}
	else if ( ptp_reg == ETH_MACSTNR )
	{
		value = ETH->MACSTNR;
	}
	else if ( ptp_reg == ETH_MACSTSUR )
	{
		value = ETH->MACSTSUR;
	}
	else if ( ptp_reg == ETH_MACSTNUR )
	{
		value = ETH->MACSTNUR;
	}
	else if ( ptp_reg == ETH_MACTSAR )
	{
		value = ETH->MACTSAR;
	}

	return ( value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Time_Stamp_Update_Offset( struct ptptime_t* timeoffset )
{
	uint32_t sign = 0U;
	uint32_t secondvalue = 0U;
	uint32_t nanosecondvalue = 0U;
	uint32_t subsecondvalue = 0U;
	uint32_t addend = 0U;

	/* determine sign and correct Second and Nanosecond values */
	if ( ( timeoffset->tv_sec < 0 ) ||
		 ( ( timeoffset->tv_sec == 0 ) && ( timeoffset->tv_nsec < 0 ) ) )
	{
		sign = ETH_PTP_NegativeTime;
		secondvalue = -timeoffset->tv_sec;
		nanosecondvalue = -timeoffset->tv_nsec;
	}
	else
	{
		sign = ETH_PTP_PositiveTime;
		secondvalue = timeoffset->tv_sec;
		nanosecondvalue = timeoffset->tv_nsec;
	}

	/* convert nanosecond to subseconds */
	subsecondvalue = Nanosecond_To_Subsecond( nanosecondvalue );

	/* read old addend register value*/
	addend = Get_Register_Value( ETH_MACTSAR );

	while ( Get_TSCR_Flag_Status( ETH_MACTSCR_TSUPDT ) == SET )
	{}
	while ( Get_TSCR_Flag_Status( ETH_MACTSCR_TSINIT ) == SET )
	{}

	/* Write the offset (positive or negative) in the Time stamp update high and low registers. */
	Set_Time_Stamp_Update( sign, secondvalue, subsecondvalue );

	/* Set bit 3 (TSSTU) in the Time stamp control register. */
	Enable_Time_Stamp_Update();

	/* The value in the Time stamp update registers is added to or subtracted from the system
	   time when the TSSTU bit is cleared.*/
	while ( Get_TSCR_Flag_Status( ETH_MACTSCR_TSUPDT ) == SET )
	{}

	/* Write back old addend register value. */
	Set_Time_Stamp_Addend_Value( addend );
	Enable_Time_Stamp_Addend();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Enable_Time_Stamp_Update()
{
	/* Enable the PTP system time update with the Time Stamp Update register value */
	ETH->MACTSCR |= ETH_MACTSCR_TSUPDT;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Time_Stamp_Adj_Frq( int32_t adj )
{
	uint32_t addend = 0U;

	/* calculate the rate by which you want to speed up or slow down the system time
	     increments */

	/* precise */
	/*
	   int64_t addend;
	   addend = Adj;
	   addend *= ADJ_FREQ_BASE_ADDEND;
	   addend /= 1000000000-Adj;
	   addend += ADJ_FREQ_BASE_ADDEND;
	 */

	/* 32bit estimation
	   ADJ_LIMIT = ((1l<<63)/275/ADJ_FREQ_BASE_ADDEND) = 11258181 = 11 258 ppm*/
	if ( adj > 5120000 )
	{
		adj = 5120000;
	}
	if ( adj < -5120000 )
	{
		adj = -5120000;
	}

	addend = ( ( ( ( 275LL * adj ) >> 8 ) * ( ADJ_FREQ_BASE_ADDEND >> 24 ) ) >> 6 ) +
		ADJ_FREQ_BASE_ADDEND;

	/* Reprogram the Time stamp addend register with new Rate value and set ETH_TPTSCR */
	Set_Time_Stamp_Addend_Value( reinterpret_cast<uint32_t>( addend ) );
	Enable_Time_Stamp_Addend();
}
