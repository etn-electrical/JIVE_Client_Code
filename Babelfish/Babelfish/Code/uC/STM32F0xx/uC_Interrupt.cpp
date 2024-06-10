/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

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
#define NVIC_ARRAY_SIZE                     ( static_cast<uint_fast16_t>( NVIC_ENUM_SIZE ) + \
											  START_STM32_INT_VEC_INDEX )

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

static __no_init INT_CALLBACK_FUNC* m_ram_vectors[NVIC_ARRAY_SIZE] @"VTABLE";
// static __no_init INT_CALLBACK_FUNC* m_ram_vectors[NVIC_ARRAY_SIZE];

extern uint32_t __vector_table;	// __vector_table is extracted from the init assembly routine.
#define orig_vect_table             ( reinterpret_cast<INT_CALLBACK_FUNC**>( &__vector_table ) )

// However,  the  vector  table  can  be  relocated  to  other
// memory locations in the code or Random Access Memory (RAM) region where the RAM is so that
// we can change the handlers during run time. This is done by setting a register in the NVIC called
// the
// vector table offset register (address 0xE000ED08). The address offset should be aligned to the
// vector
// table size, extended to the next larger power of 2. For example, if there are 32 IRQ inputs, the
// total
// number of exceptions will be 32 + 16 (system exceptions) = 48. Extending it to the power of 2
// makes
// it 64. Multiplying it by 4 (4 bytes per vector) makes it 256 bytes (0x100). Therefore, the vector
// table
// offset can be programmed as 0x0, 0x100, 0x200, and so on.

// We are going to use an alignment of 256 for the time being.  Since we are putting the
// VTABLE at 0 of ram we should not have an alignment problem but it is good to check.
#define NVIC_VECT_TBLE_ALIGN_FORCE_MASK         ( 0x1FFFFF00U )
#define NVIC_VECT_TBLE_ALIGN_CHECK_MASK         ( 0xFFU )



void Default_Interrupt_Handler( void );

void Int_Vect_N14( void );

void Int_Vect_N13( void );

void Int_Vect_N5( void );

void Int_Vect_N2( void );

void Int_Vect_N1( void );

void Int_Vect_0( void );

void Int_Vect_2( void );

void Int_Vect_3( void );

void Int_Vect_4( void );

void Int_Vect_5( void );

void Int_Vect_6( void );

void Int_Vect_7( void );

void Int_Vect_9( void );

void Int_Vect_10( void );

void Int_Vect_11( void );

void Int_Vect_12( void );

void Int_Vect_13( void );

void Int_Vect_14( void );

void Int_Vect_16( void );

void Int_Vect_17( void );

#ifdef uC_STM32F030CC_USAGE
void Int_Vect_18( void );

#endif

void Int_Vect_19( void );

void Int_Vect_20( void );

void Int_Vect_21( void );

void Int_Vect_22( void );

void Int_Vect_23( void );

void Int_Vect_24( void );

void Int_Vect_25( void );

void Int_Vect_26( void );

void Int_Vect_27( void );

void Int_Vect_28( void );

#ifdef uC_STM32F030CC_USAGE
void Int_Vect_29( void );

#endif

#if defined ( uC_STM32F030CC_USAGE )
void ( * m_ram_vector[NVIC_ARRAY_SIZE] )() = {
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler
};
#elif defined ( uC_STM32F030R8_USAGE )
void ( * m_ram_vector[NVIC_ARRAY_SIZE] )() = {
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler, &Default_Interrupt_Handler,
	&Default_Interrupt_Handler, &Default_Interrupt_Handler
};
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Interrupt::uC_Interrupt( void )
{
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );

	// if ( ( reinterpret_cast<uint32_t>(m_ram_vectors) & NVIC_VECT_TBLE_ALIGN_CHECK_MASK ) != 0U )
	// {
	// Your RAM location needs to line up on word boundaries and it does not appear to.
	// BF_ASSERT(false);
	// }

#ifdef USE_FULL_RTOS
	// Set the priority grouping.
	// SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_4;
#else
	// NVIC_SetPriorityGrouping(NO_PRE_EMPTION_PRIORITY_GROUP);
	// NVIC_SystemReset();
#endif

	// move the vector table to RAM
	// for ( uint_fast16_t i = 0U; i < static_cast<uint_fast16_t>(NVIC_ARRAY_SIZE); i++ )
	// {
	// m_ram_vectors[i] = *original_vect;
	// original_vect++;
	// }

	// SCB->VTOR = NVIC_VectTab_RAM | ( reinterpret_cast<uint32_t>(m_ram_vectors) &
	// NVIC_VECT_TBLE_ALIGN_FORCE_MASK );

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
	uint_fast16_t vector_index;

	vector_index = static_cast<uint_fast16_t>( vector_number )
		+ static_cast<uint_fast16_t>( START_STM32_INT_VEC_INDEX );

	Push_GINT( temp_gint );

	// Settable_Vect::Attach_Vect( vector_callback, index_number );
	m_ram_vector[vector_index] = vector_callback;

	/* Set the priority of the interrupt */
	NVIC_SetPriority( vector_number, priority );
	/* Enable peripheral interrrupt*/
	NVIC_EnableIRQ( vector_number );

	Pop_GINT( temp_gint );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::Clear_Vector( IRQn_Type vector_number )
{
	uint_fast16_t vector_index;

	vector_index = static_cast<uint_fast16_t>( vector_number ) + START_STM32_INT_VEC_INDEX;

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

	vector_index = static_cast<uint16_t>( vector_number ) + START_STM32_INT_VEC_INDEX;

	if ( m_ram_vectors[vector_index] == reinterpret_cast<INT_CALLBACK_FUNC*>( nullptr ) )
	{
		vector_empty = true;
	}

	return ( vector_empty );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Default_Interrupt_Handler( void )
{
	BF_ASSERT( false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_N14( void )
{
	( *m_ram_vector[INT_VECT_N14] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_N13( void )
{
	( *m_ram_vector[INT_VECT_N13] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_N5( void )
{
	( *m_ram_vector[INT_VECT_N5] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_N2( void )
{
	( *m_ram_vector[INT_VECT_N2] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_N1( void )
{
	( *m_ram_vector[INT_VECT_N1] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_0( void )
{
	( *m_ram_vector[INT_VECT_0] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_2( void )
{
	( *m_ram_vector[INT_VECT_2] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_3( void )
{
	( *m_ram_vector[INT_VECT_3] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_4( void )
{
	( *m_ram_vector[INT_VECT_4] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_5( void )
{
	( *m_ram_vector[INT_VECT_5] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_6( void )
{
	( *m_ram_vector[INT_VECT_6] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_7( void )
{
	( *m_ram_vector[INT_VECT_7] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_9( void )
{
	( *m_ram_vector[INT_VECT_9] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_10( void )
{
	( *m_ram_vector[INT_VECT_10] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_11( void )
{
	( *m_ram_vector[INT_VECT_11] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_12( void )
{
	( *m_ram_vector[INT_VECT_12] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_13( void )
{
	( *m_ram_vector[INT_VECT_13] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_14( void )
{
	( *m_ram_vector[INT_VECT_14] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_16( void )
{
	( *m_ram_vector[INT_VECT_16] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_17( void )
{
	( *m_ram_vector[INT_VECT_17] )();
}

#ifdef uC_STM32F030CC_USAGE
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_18( void )
{
	( *m_ram_vector[INT_VECT_18] )();
}

#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_19( void )
{
	( *m_ram_vector[INT_VECT_19] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_20( void )
{
	( *m_ram_vector[INT_VECT_20] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_21( void )
{
	( *m_ram_vector[INT_VECT_21] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_22( void )
{
	( *m_ram_vector[INT_VECT_22] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_23( void )
{
	( *m_ram_vector[INT_VECT_23] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_24( void )
{
	( *m_ram_vector[INT_VECT_24] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_25( void )
{
	( *m_ram_vector[INT_VECT_25] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_26( void )
{
	( *m_ram_vector[INT_VECT_26] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_27( void )
{
	( *m_ram_vector[INT_VECT_27] )();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_28( void )
{
	( *m_ram_vector[INT_VECT_28] )();
}

#ifdef uC_STM32F030CC_USAGE
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Int_Vect_29( void )
{
	( *m_ram_vector[INT_VECT_29] )();
}

#endif
