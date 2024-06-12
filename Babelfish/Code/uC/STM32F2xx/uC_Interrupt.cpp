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
#include "Includes.h"
#include "uC_Interrupt.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define START_STM32_INT_VEC_INDEX           ( 0x40U >> DIV_BY_4 )
#define NVIC_ARRAY_SIZE                     ( static_cast<uint_fast16_t>( NVIC_ENUM_SIZE ) + START_STM32_INT_VEC_INDEX )

#ifdef USE_FULL_RTOS
	#define AIRCR_VECTKEY_MASK          ( 0x05FA0000U )
	#define NVIC_PriorityGroup_4        ( 0x300U )	// 4 bits for pre-emption priority
	#define NVIC_PriorityGroup_0        ( 0x700U )	// 0 bits for pre-emption priority
#else
// This will disable all pre-emption.  Refer to the bottom of page 119 in the definitive
// guide to the m3.  Go team.
	#define NO_PRE_EMPTION_PRIORITY_GROUP               7U
#endif

#define NVIC_VectTab_RAM             ( 0x20000000U )

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
 */
/*lint -e{930} # MISRA Deviation */
static __no_init INT_CALLBACK_FUNC* m_ram_vectors[NVIC_ARRAY_SIZE] @"VTABLE";
/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 17-0-2, Re-use of C++ identifier pattern: __vector_table
 *@n Justification: __vector_table is extracted from the init assembly routine.
 */
/*lint -e{1960} # MISRA Deviation */
extern uint32_t __vector_table;	// __vector_table is extracted from the init assembly routine.
#define orig_vect_table             ( reinterpret_cast<INT_CALLBACK_FUNC**>( &__vector_table ) )

// However,  the  vector  table  can  be  relocated  to  other
// memory locations in the code or Random Access Memory (RAM) region where the RAM is so that
// we can change the handlers during run time. This is done by setting a register in the NVIC called the
// vector table offset register (address 0xE000ED08). The address offset should be aligned to the vector
// table size, extended to the next larger power of 2. For example, if there are 32 IRQ inputs, the total
// number of exceptions will be 32 + 16 (system exceptions) = 48. Extending it to the power of 2 makes
// it 64. Multiplying it by 4 (4 bytes per vector) makes it 256 bytes (0x100). Therefore, the vector table
// offset can be programmed as 0x0, 0x100, 0x200, and so on.

// We are going to use an alignment of 256 for the time being.  Since we are putting the
// VTABLE at 0 of ram we should not have an alignment problem but it is good to check.
#define NVIC_VECT_TBLE_ALIGN_FORCE_MASK         ( 0x1FFFFF00U )
#define NVIC_VECT_TBLE_ALIGN_CHECK_MASK         ( 0xFFU )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Interrupt::uC_Interrupt( void )
{
	GINT_TDEF temp_gint;
	INT_CALLBACK_FUNC** original_vect;

	Push_GINT( temp_gint );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 0-1-1, Constant value Boolean
	 *@n Justification: Although m_ram_vectors is set to be aligned and this should always be False,
	 *		this check is here in case m_ram_vectors becomes misaligned in runtime.
	 */
	/*lint -e{506} # MISRA Deviation */
	// Your RAM location needs to line up on word boundaries and it does not appear to.
	BF_ASSERT( ( reinterpret_cast<uint32_t>( m_ram_vectors ) & NVIC_VECT_TBLE_ALIGN_CHECK_MASK ) == 0U );

#ifdef USE_FULL_RTOS
	// Set the priority grouping.
	SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_4;
#else
	// Set the priority grouping.
	NVIC_SetPriorityGrouping( NO_PRE_EMPTION_PRIORITY_GROUP );
#endif

	// move the vector table to RAM
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-6, Unusual pointer cast
	 *@n Justification: __vector_table (#define orig_vect_table) is extracted from the init assembly routine.
	 */
	/*lint -e{740} # MISRA Deviation */
	original_vect = ( orig_vect_table );	// __vector_table is extracted from the init assembly routine.
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	for ( uint_fast16_t i = 0U; i < static_cast<uint_fast16_t>( NVIC_ARRAY_SIZE ); i++ )
	{
		m_ram_vectors[i] = *original_vect;
		original_vect++;
	}

	SCB->VTOR = NVIC_VectTab_RAM | ( reinterpret_cast<uint32_t>( m_ram_vectors ) & NVIC_VECT_TBLE_ALIGN_FORCE_MASK );

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Set_Vector( INT_CALLBACK_FUNC* vector_callback, IRQn_Type vector_number,
							   uint16_t priority )
{
	GINT_TDEF temp_gint;
	bool set_good = true;
	uint_fast16_t vector_index;

	// Has the remap happened?  If not then we missed the constructor.
	BF_ASSERT( ( SCB->VTOR & NVIC_VectTab_RAM ) != 0U );
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	vector_index = static_cast<uint_fast16_t>( vector_number ) +
		static_cast<uint_fast16_t>( START_STM32_INT_VEC_INDEX );

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-6, Unusual pointer cast
	 *@n Justification: __vector_table (#define orig_vect_table) is extracted from the init assembly routine.
	 */
	/*lint -e{740} # MISRA Deviation */
	BF_ASSERT( ( m_ram_vectors[vector_index] == orig_vect_table[vector_index] ) &&
			   ( priority < static_cast<uint16_t>( uC_INT_HW_MAX_PRIORITY ) ) );

	Push_GINT( temp_gint );

	m_ram_vectors[vector_index] = vector_callback;

	NVIC_SetPriority( vector_number, priority );

	Pop_GINT( temp_gint );

	set_good = true;

	return ( set_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::Clear_Vector( IRQn_Type vector_number )
{
	uint_fast16_t vector_index;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	vector_index = static_cast<uint_fast16_t>( vector_number ) + START_STM32_INT_VEC_INDEX;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-6, Unusual pointer cast
	 *@n Justification: __vector_table (#define orig_vect_table) is extracted from the init assembly routine.
	 */
	/*lint -e{740} # MISRA Deviation */
	m_ram_vectors[vector_index] = orig_vect_table[vector_index];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Is_Vector_Empty( IRQn_Type vector_number )
{
	uint16_t vector_index;
	bool vector_empty = false;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	vector_index = static_cast<uint16_t>( vector_number ) + START_STM32_INT_VEC_INDEX;

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-6, Unusual pointer cast
	 *@n Justification: __vector_table (#define orig_vect_table) is extracted from the init assembly routine.
	 */
	/*lint -e{740} # MISRA Deviation */
	if ( m_ram_vectors[vector_index] == orig_vect_table[vector_index] )
	{
		vector_empty = true;
	}

	return ( vector_empty );
}

/*lint +e1924 */
