/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"

/*
 *****************************************************************************************
 *		Declarations
 *****************************************************************************************
 */


#if PROCESSOR_REGISTER_BASE_SIZE == 1U
	#define uC_CSTACK_FILL_KEY          0xA1U
#elif PROCESSOR_REGISTER_BASE_SIZE == 2U
	#define uC_CSTACK_FILL_KEY          0xA2A1U
#elif PROCESSOR_REGISTER_BASE_SIZE == 4U
	#define uC_CSTACK_FILL_KEY          0xA4A3A2A1U
#elif PROCESSOR_REGISTER_BASE_SIZE == 8U
	#define uC_CSTACK_FILL_KEY          0xA8A7A6A5A4A3A2A1U
#endif
#define uC_CSTACK_FILL_HEAD_START   0x10U	// This is to avoid any issues with the stack data.

#pragma section="CSTACK"

static BF_Lib::MBASE* cstack_begin;
static BF_Lib::MBASE* cstack_end;
static BF_Lib::MBASE* present_top;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CSTACK_Init( void )
{
	BF_Lib::MBASE* target_ptr;

	Push_TGINT();

	target_ptr =
		reinterpret_cast<BF_Lib::MBASE*>( ( ( ( static_cast<UINT32>( __get_SP() ) - uC_CSTACK_FILL_HEAD_START ) ) -
											( sizeof( BF_Lib::MBASE ) - 1U ) ) & ~( sizeof( BF_Lib::MBASE ) - 1U ) );
	present_top = target_ptr;

	cstack_begin =
		reinterpret_cast<BF_Lib::MBASE*>( ( reinterpret_cast<UINT32>( __section_begin( "CSTACK" ) ) +
											( sizeof( BF_Lib::MBASE ) - 1U ) ) & ~( sizeof( BF_Lib::MBASE ) - 1U ) );
	cstack_end =
		reinterpret_cast<BF_Lib::MBASE*>( ( reinterpret_cast<UINT32>( __section_end( "CSTACK" ) ) -
											( sizeof( BF_Lib::MBASE ) - 1U ) ) & ~( sizeof( BF_Lib::MBASE ) - 1U ) );

	while ( target_ptr > cstack_begin )
	{
		*target_ptr = uC_CSTACK_FILL_KEY;
		target_ptr--;
	}

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_CSTACK_Used( void )
{
	static uint8_t present_use = 0U;
	uint32_t percent_used;
	BF_Lib::MBASE* old_top;

	Push_TGINT();

	old_top = present_top;
	while ( ( ( present_top - 2U ) > cstack_begin ) &&
			( !( ( *present_top == uC_CSTACK_FILL_KEY ) &&
				 ( *( present_top - 1U ) == uC_CSTACK_FILL_KEY ) &&
				 ( *( present_top - 2U ) == uC_CSTACK_FILL_KEY ) ) ) )
	{
		present_top--;
	}

	Pop_TGINT();

	if ( present_top != old_top )
	{
		percent_used = ( cstack_end - present_top ) * 100U;
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-8-1, out of bounds value for right hand side of shift operator
		 *@n Justification: PC-lint false alarm.  Round_Div shifts the divisor right 2. PC-lint is seeing
		 * BF_Lib::MBASE as being less than 2 bits wide when it is 32-bits long.
		 */
		/*lint -e{1960} # MISRA Deviation */

		percent_used = Round_Div( percent_used, ( cstack_end - cstack_begin ) );

		present_use = static_cast<uint8_t>( percent_used );
	}

	return ( present_use );
}
