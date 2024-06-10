/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
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
	__ETHMACPTP_CLK_ENABLE();

	/*select node type */
	if ( node_type == ORDINARY_CLOCK )
	{
		ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFCFFFF ) );
	}
	else if ( node_type == BOUNDARY_CLOCK )
	{
		ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFEFFFF ) );
	}
	else if ( node_type == E2E_TC_CLOCK )
	{
		ETH->PTPTSCR |= ( ETH_PTPTSCR_TSCNT & ( 0xFFFDFFFF ) );
	}
	else if ( node_type == P2P_TC_CLOCK )
	{
		ETH->PTPTSCR |= ETH_PTPTSCR_TSCNT;
	}

	/* packet snooping type*/
	if ( pkt_snooping == PTP_VER_1 )
	{
		ETH->PTPTSCR &= ( ~( ETH_PTPTSSR_TSPTPPSV2E ) );
	}
	else
	{
		ETH->PTPTSCR |= ETH_PTPTSSR_TSPTPPSV2E;
	}

	/* time stamp for event message*/
	ETH->PTPTSCR |= ETH_PTPTSSR_TSSEME;

	/* time stamp snapshot for master relevant message*/
	ETH->PTPTSCR |= ETH_PTPTSSR_TSSMRME;

	/* Time stamp subsecond rollover */
	ETH->PTPTSCR |= ETH_PTPTSSR_TSSSR;
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
	ETHMACIT_Config( ETH_MAC_IT_TST, DISABLE );

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
		while ( Get_TSCR_Flag_Status( ETH_PTP_FLAG_TSARU ) == SET )
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

#ifdef USE_ENHANCED_DMA_DESCRIPTORS
	/* The enhanced descriptor format is enabled and the descriptor size is
	 * increased to 32 bytes (8 DWORDS). This is required when time stamping
	 * is activated above. */
	Enhanced_Descriptor_Cmd( ENABLE );
#endif

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
		ETH->MACIMR &= ( ~( uint32_t )eth_mac_int );
	}
	else
	{
		/* Disable the selected ETHERNET MAC interrupts */
		ETH->MACIMR |= eth_mac_int;
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
		ETH->PTPTSCR |= ETH_PTPTSCR_TSE | ETH_PTPTSSR_TSSIPV4FE | ETH_PTPTSSR_TSSIPV6FE |
			ETH_PTPTSSR_TSSARFE;
	}
	else
	{
		/* Disable the PTP time stamp for transmit and receive frames */
		ETH->PTPTSCR &= ( ~( uint32_t )ETH_PTPTSCR_TSE );
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
	ETH->PTPSSIR = subsecondvalue;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Set_Time_Stamp_Addend_Value( uint32_t addend_val )
{
	/* Set the PTP Time Stamp Addend Register */
	ETH->PTPTSAR = addend_val;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Enable_Time_Stamp_Addend( void )
{
	/* Enable the PTP block update with the Time Stamp Addend register value */
	ETH->PTPTSCR |= ETH_PTPTSCR_TSARU;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus uC_PTP_Slave::Get_TSCR_Flag_Status( uint32_t ptp_flag )
{
	FlagStatus bitstatus = RESET;

	if ( ( ETH->PTPTSCR & ptp_flag ) != ( uint32_t )RESET )
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
		ETH->PTPTSCR |= ETH_PTPTSCR_TSFCU;
	}
	else
	{
		/* Disable the PTP Coarse Update method */
		ETH->PTPTSCR &= ( ~( uint32_t )ETH_PTPTSCR_TSFCU );
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
	ETH->PTPTSHUR = secondValue;

	/* Set the PTP Time Update Low Register with sign */
	ETH->PTPTSLUR = sign | subSecondValue;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::TSCR_Initialize_Time_Stamp_Bit( void )
{
	/* Initialize the PTP Time Stamp */
	ETH->PTPTSCR |= ETH_PTPTSCR_TSSTI;
}

#ifdef USE_ENHANCED_DMA_DESCRIPTORS
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Enhanced_Descriptor_Cmd( FunctionalState state )
{
	if ( state != DISABLE )
	{
		/* Enable enhanced descriptor structure */
		ETH->DMABMR |= ETH_DMABMR_EDE;
	}
	else
	{
		/* Disable enhanced descriptor structure */
		ETH->DMABMR &= ~ETH_DMABMR_EDE;
	}
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PTP_Slave::Get_System_Time( struct ptptime_t* timestamp )
{
	timestamp->tv_nsec = Subsecond_To_Nanosecond( Get_Register_Value( ETH_PTPTSLR ) );
	timestamp->tv_sec = Get_Register_Value( ETH_PTPTSHR );
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
	while ( Get_TSCR_Flag_Status( ETH_PTP_FLAG_TSSTI ) == SET )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_PTP_Slave::Nanosecond_To_Subsecond( uint32_t subsecondvalue )
{
	uint64_t val = subsecondvalue * nanosecond_to_subsecond_multiplier;

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

	if ( ptp_reg == ETH_PTPTSCR )
	{
		value = ETH->PTPTSCR;
	}
	else if ( ptp_reg == ETH_PTPSSIR )
	{
		value = ETH->PTPSSIR;
	}
	else if ( ptp_reg == ETH_PTPTSHR )
	{
		value = ETH->PTPTSHR;
	}
	else if ( ptp_reg == ETH_PTPTSLR )
	{
		value = ETH->PTPTSLR;
	}
	else if ( ptp_reg == ETH_PTPTSHUR )
	{
		value = ETH->PTPTSHUR;
	}
	else if ( ptp_reg == ETH_PTPTSLUR )
	{
		value = ETH->PTPTSLUR;
	}
	else if ( ptp_reg == ETH_PTPTSAR )
	{
		value = ETH->PTPTSAR;
	}
	else if ( ptp_reg == ETH_PTPTTHR )
	{
		value = ETH->PTPTTHR;
	}
	else if ( ptp_reg == ETH_PTPTTLR )
	{
		value = ETH->PTPTTLR;
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
	addend = Get_Register_Value( ETH_PTPTSAR );

	while ( Get_TSCR_Flag_Status( ETH_PTP_FLAG_TSSTU ) == SET )
	{}
	while ( Get_TSCR_Flag_Status( ETH_PTP_FLAG_TSSTI ) == SET )
	{}

	/* Write the offset (positive or negative) in the Time stamp update high and low registers. */
	Set_Time_Stamp_Update( sign, secondvalue, subsecondvalue );

	/* Set bit 3 (TSSTU) in the Time stamp control register. */
	Enable_Time_Stamp_Update();

	/* The value in the Time stamp update registers is added to or subtracted from the system
	   time when the TSSTU bit is cleared.*/
	while ( Get_TSCR_Flag_Status( ETH_PTP_FLAG_TSSTU ) == SET )
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
	ETH->PTPTSCR |= ETH_PTPTSCR_TSSTU;
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
