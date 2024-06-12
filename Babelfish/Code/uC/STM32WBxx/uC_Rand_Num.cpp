/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Rand_Num.h"
#include "Exception.h"
#include "stm32wbxx_hal_hsem.h"
#include "app_conf.h"

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
std::mt19937 uC_Rand_Num::prng;
bool uC_Rand_Num::TRNG_active = false;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Rand_Num::uC_Rand_Num( void )
{
	volatile uint32_t dump_val;

	const PERIPH_DEF_ST peripheral_def =
	{
		static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB3 ),
		RCC_AHB3ENR_RNGEN
	};

	// Enable this semaphore if we want to use full RNG
	// RNG registers get reset by M0, taking this semaphore in M4 will prevent register from reset by M0
	// HAL_HSEM_Take( RNG_HSEM , HSEM_CR_COREID_CURRENT );
	/*select LSE Clock for RNG*/
	RCC->CCIPR |= RCC_CCIPR_RNGSEL_1;

	uC_Base::Enable_Periph_Clock( &peripheral_def );

	RNG->CR = RNG_CR_RNGEN;

	// Wait for Random num generator ready.
	while ( ( RNG->SR & RNG_SR_DRDY ) == 0U )
	{}

	dump_val = RNG->DR;
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-4: Symbol 'dump_val' not accessed
	 * @n PCLint:
	 * @n Justification: The data assignment is to dump the data, data is not used.
	 * This is per design.
	 */
	/*lint -e{550}*/

	TRNG_active = true;

	uint32_t seed;

	// Wait for Random num generator ready.
	while ( ( RNG->SR & RNG_SR_DRDY ) == 0U )
	{}
	seed = static_cast<uint32_t>( RNG->DR );
	prng.seed( seed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Rand_Num::Get( uint8_t* rand_data, uint16_t length )
{
	BF_Lib::SPLIT_UINT32 temp_rand;
	uint_fast8_t rand_index = sizeof( temp_rand );

	for ( uint_fast16_t i = 0U; i < length; i++ )
	{
		if ( rand_index >= sizeof( temp_rand ) )
		{
			rand_index = 0U;

			if ( TRNG_active )
			{
				do
				{
					// Verify that no seed errors occurred
					if ( ( RNG->SR & RNG_SR_SEIS ) == 0U )
					{
						TRNG_Error_Recovery();
					}

					// Wait for random number generator ready
					while ( ( RNG->SR & RNG_SR_DRDY ) == 0U )
					{}

					temp_rand.u32 = static_cast<uint32_t>( RNG->DR );

				} while ( temp_rand.u32 != 0 );	// Verify that RNG_DR is different from zero.
												// In rare cases DRDY could be 1 but RNG->DR
												// still not have the data (see programming
												// manual, sec. 21.3.5).
			}
			else
			{
				temp_rand.u32 = prng();
			}
		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Rand_Num::De_Init( void )
{
	const PERIPH_DEF_ST peripheral_def =
	{
		static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB3 ),
		RCC_AHB3ENR_RNGEN
	};

	RNG->CR = 0;
	uC_Base::Disable_Periph_Clock( &peripheral_def );

	// M0 core will not go into deep sleep mode when
	// RNGSEL bit of CCIPR register is configured before M0 boot.
	// Thus M4 core is clearing after its use.
	// HSEM semaphore needs to be used for exclusive usage of RNG peripheral
	RCC->CCIPR &= ~RCC_CCIPR_RNGSEL_1;

	TRNG_active = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Rand_Num::Is_Active( void )
{
	return ( TRNG_active );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Rand_Num::TRNG_Error_Recovery( void )
{
	while ( RNG->SR & RNG_SR_SEIS != 0U )	// Repeat until SEIS is cleared
	{
		RNG->SR &= ~RNG_SR_SEIS;// Clear the SEIS bit

		// Read out 12 words from the RNG_DR register and discard each of them
		// in order to clean the pipeline.
		for ( uint8_t i = 0; i < 12; i++ )
		{
			volatile uint32_t tmp = RNG->DR;
		}
	}
}
