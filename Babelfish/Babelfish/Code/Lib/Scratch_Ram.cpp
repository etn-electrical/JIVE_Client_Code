/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Scratch_Ram.h"
#include "Ram.h"
#include "Babelfish_Assert.h"
namespace BF_Lib
{

/*
 *************************************************************************************************
 *  Statics
   -------------------------------------------------------------------------------------------------
 */
uint8_t* Scratch_Ram::m_heap_start;
uint8_t* Scratch_Ram::m_high_water_mark;
uint8_t* Scratch_Ram::m_heap_head;
size_t Scratch_Ram::m_heap_size;

uint8_t Scratch_Ram::m_alignment_boundry_size;
uint8_t Scratch_Ram::m_percent_used;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Scratch_Ram::Scratch_Ram( size_t size, bool fill, uint8_t fill_val ) :
	m_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_size( 0U )
{
	if ( size != 0U )
	{
		Allocate( size, fill, fill_val );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Scratch_Ram::~Scratch_Ram( void )
{
	m_data = reinterpret_cast<uint8_t*>( nullptr );
	m_heap_head -= m_size;
	m_size = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Scratch_Ram::Init( size_t size, uint8_t alignment_boundry_size )
{
	size_t aligned_alloc_size;
	uintptr_t ptr_mask;
	uintptr_t ptr_calc;

	m_alignment_boundry_size = alignment_boundry_size;

	m_heap_size = size;
	aligned_alloc_size = static_cast<size_t>( m_alignment_boundry_size ) - 1U;
	aligned_alloc_size = static_cast<size_t>( size ) + aligned_alloc_size;

	ptr_calc = reinterpret_cast<uintptr_t>( Ram::Allocate( aligned_alloc_size ) );

	ptr_calc = ptr_calc + ( static_cast<uintptr_t>( m_alignment_boundry_size ) - 1U );

	ptr_mask = static_cast<uintptr_t>( m_alignment_boundry_size ) - 1U;
	ptr_mask = static_cast<uintptr_t>( ~ptr_mask );
	ptr_calc = ptr_calc & ptr_mask;

	m_heap_start = reinterpret_cast<uint8_t*>( ptr_calc );
	m_heap_head = m_heap_start;
	m_high_water_mark = m_heap_start;

	m_percent_used = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Scratch_Ram::Allocate( size_t size, bool fill, uint8_t fill_val )
{
	BF_ASSERT( m_size == 0U );
	m_size = size;
	if ( m_size == 0U )
	{
		m_size = m_alignment_boundry_size;
	}

	m_size = ( ( ( m_size - 1U ) / m_alignment_boundry_size ) + 1U ) * m_alignment_boundry_size;

	Push_TGINT();

	BF_ASSERT( ( m_heap_head + m_size ) < ( m_heap_start + m_heap_size ) );

	m_data = m_heap_head;
	m_heap_head += m_size;

	Pop_TGINT();

	if ( fill == true )
	{
		uint8_t* runner = reinterpret_cast<uint8_t*>( m_data );
		for ( uint32_t i = 0U; i < m_size; i++ )
		{
			*runner = fill_val;
			runner++;
		}
	}

	if ( m_heap_head > m_high_water_mark )
	{
		m_high_water_mark = m_heap_head;
		m_percent_used = Calculate_Percent_Used();
	}

	return ( m_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Scratch_Ram::Calculate_Percent_Used( void )
{
	size_t used_temp;
	uint8_t return_percent;

	used_temp = m_high_water_mark - m_heap_start;
	used_temp = used_temp * 100U;

	used_temp = used_temp / m_heap_size;

	return_percent = static_cast<uint8_t>( used_temp & 0xFFU );

	return ( return_percent );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Scratch_Ram::Used( void )
{
	return ( m_percent_used );
}

}	/* end namespace BF_Lib for this module */
