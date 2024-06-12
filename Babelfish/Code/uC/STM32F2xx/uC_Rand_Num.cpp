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
		static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB2 ),
		RCC_AHB2ENR_RNGEN
	};

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
			while ( ( RNG->SR & RNG_SR_DRDY ) == 0U )
			{}

			temp_rand.u32 = static_cast<uint32_t>( RNG->DR );
		}
		rand_data[i] = temp_rand.u8[rand_index];
		rand_index++;
	}
}

/* lint +e1924
  lint +e1960*/
