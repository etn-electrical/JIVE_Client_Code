/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Rand_Num.h"
#include "Exception.h"

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */
#define Wait_For_Rand_Ready()       while ( ( RNG->SR & RNG_SR_DRDY ) == 0 ){}

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Rand_Num::uC_Rand_Num( void )
{
#if !defined ( uC_STM32F107_USAGE ) && !defined ( uC_STM32F302_USAGE ) && \
	!defined ( uC_STM32F100_USAGE )

	volatile uint32_t dump_val;

	const PERIPH_DEF_ST peripheral_def =
	{
		RCC_PERIPH_CLCK_AHB2,
		RCC_AHB2ENR_RNGEN
	};

	uC_Base::Enable_Periph_Clock( &peripheral_def );

	RNG->CR = RNG_CR_RNGEN;
	Wait_For_Rand_Ready();

	dump_val = RNG->DR;
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Rand_Num::Get( uint8_t* rand_data, uint16_t length )
{
#if !defined ( uC_STM32F107_USAGE ) && !defined ( uC_STM32F302_USAGE ) && \
	!defined ( uC_STM32F100_USAGE )

	BF_Lib::SPLIT_UINT32 temp_rand;
	uint_fast8_t rand_index = sizeof( temp_rand );

	for ( uint_fast16_t i = 0; i < length; i++ )
	{
		if ( rand_index >= sizeof( temp_rand ) )
		{
			rand_index = 0;

			Wait_For_Rand_Ready();
			temp_rand.u32 = RNG->DR;
		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
#endif
}
