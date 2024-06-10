/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 *all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 *signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 *re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_Rand_Num.h"
#include "Exception.h"

#if defined ( uC_KINETISK60DNx_USAGE ) || ( uC_KINETISK66x_USAGE )	// For MK60D10 and MK66F18 target

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */

#define Wait_For_Rand_Ready()       while ( ( RNG->SR & RNG_SR_OREG_LVL_MASK ) == 0 ){}

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
	volatile uint32_t dump_val;

	SIM->SCGC3 |= SIM_SCGC3_RNGA_MASK;	// open the clock gate for the RNG
	RNG->CR = ( UINT32 )( RNG_CR_GO_MASK | RNG_CR_INTM_MASK | RNG_CR_HA_MASK );	// start random number generation
	Wait_For_Rand_Ready();	// wait for a random number

	dump_val = RNG->OR;	// read the random number
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

			// Wait for Random num generator ready.
			Wait_For_Rand_Ready();
			temp_rand.u32 = static_cast<uint32_t>( RNG->OR );
		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
}

/* The register settings for Random Number Generator module are different from other k6x family controllers */
#else

/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */

#define Wait_For_Rand_Ready()       while ( ( RNG->SR & RNG_SR_FIFO_LVL_MASK ) == 0 ){}

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
	volatile uint32_t dump_val;

	SIM->SCGC3 |= SIM_SCGC3_RNGB_MASK;	// open the clock gate for the RNG
	RNG->CR = ( UINT32 )( RNG_CR_AR_MASK );	// start random number generation
	Wait_For_Rand_Ready();		// wait for a random number

	dump_val = RNG->OUT;// read the random number
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

			// Wait for Random num generator ready.
			Wait_For_Rand_Ready();
			temp_rand.u32 = static_cast<uint32_t>( RNG->OUT );

		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
}

#endif

/* lint +e1924
  lint +e1960*/
