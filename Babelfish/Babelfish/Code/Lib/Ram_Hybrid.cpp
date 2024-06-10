/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_Hybrid.h"
#include "Babelfish_Assert.h"

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule 8-0-1: A +headerwarn option was previously issued for header 'Stdlib.h'
 * @n PCLint:
 * @n Justification: Stdlib.h is part of IAR includes
 */
/*lint -e829*/
#include "Stdlib.h"

namespace BF_Lib
{
/*
 *************************************************************************************************
 *  Ram Class static member attributes
   -------------------------------------------------------------------------------------------------
 */
#pragma segment="HEAP" __data
const uint32_t RAM_HYBRID_HEAP_BEGIN = reinterpret_cast<uint32_t>( __segment_begin( "HEAP" ) );
const uint32_t RAM_HYBRID_HEAP_END = reinterpret_cast<uint32_t>( __segment_end( "HEAP" ) );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ram_Hybrid::Ram_Hybrid( uint8_t alignment_boundry_size ) :
	m_malloc_overhead( 0U ),
	m_mem_start( nullptr ),
	m_dyn_mem_start( nullptr ),
	m_index( 0U ),
	m_size( 0U ),
	m_percent_used( 0U ),
	m_use_malloc( false ),
	m_alignment_boundry_size( alignment_boundry_size ),
	Ram_Handler()
{
	const uint8_t ALLOC_SIZE_DEC = 8;		// Used to decrement alloc size for the malloc test.
	void* test_mem1;
	void* test_mem2;

	Push_TGINT();

	// The following allocates some memory then measures the distance between.
	test_mem1 = malloc( 1U );
	test_mem2 = malloc( 1U );

	m_malloc_overhead = reinterpret_cast<size_t>( test_mem2 ) -
		reinterpret_cast<size_t>( test_mem1 );

	free( test_mem2 );
	free( test_mem1 );

	m_size = RAM_HYBRID_HEAP_END - RAM_HYBRID_HEAP_BEGIN;

	// What we are doing here is allocating as much space as we can from the malloc
	// routine.  The malloc routine overhead is not measurable so essentially
	// we allocate until we get a valid pointer.  Then we consider that the
	// malloc overhead and our initial available space.
	do
	{
		m_size = m_size - ALLOC_SIZE_DEC;
		m_mem_start = reinterpret_cast<uint8_t*>( malloc( m_size ) );
	} while ( m_mem_start == nullptr );

	// Here we set the dynamic mem start to the end because are currently going to be using
	// the static allocation functionality and not the true malloc.
	m_dyn_mem_start = m_mem_start + m_size;
	m_index = 0U;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Hybrid::Enable_Dynamic_Mem( void )
{
	uint8_t* realloc_ptr;

	Push_TGINT();

	if ( !m_use_malloc )
	{
		realloc_ptr = reinterpret_cast<uint8_t*>( realloc( m_mem_start, m_index ) );
		BF_ASSERT( realloc_ptr == m_mem_start );
		m_dyn_mem_start = m_mem_start + m_index;
		m_use_malloc = true;
	}

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Hybrid::Allocate( size_t size, bool fill, uint8_t fill_val )
{
	void* temp_return = reinterpret_cast<void*>( nullptr );

	Push_TGINT();

	if ( size == 0U )
	{
		size = 1U;
	}

	if ( m_use_malloc == true )
	{
		temp_return = malloc( size );
		BF_ASSERT( temp_return != nullptr );

		if ( ( reinterpret_cast<uint32_t>( temp_return ) + size +
			   m_malloc_overhead ) > ( m_index + RAM_HYBRID_HEAP_BEGIN ) )
		{
			m_index = reinterpret_cast<uint32_t>( temp_return ) + size
				+ m_malloc_overhead;
			m_index -= RAM_HYBRID_HEAP_BEGIN;
		}
	}
	else
	{
		BF_ASSERT( ( m_index + size ) < m_size );
		temp_return = &m_mem_start[m_index];
		m_index += ( ( ( size - 1U ) / m_alignment_boundry_size )
					 + 1U ) * m_alignment_boundry_size;
	}

	Pop_TGINT();

	if ( fill == true )
	{
		uint8_t* runner = reinterpret_cast<uint8_t*>( temp_return );
		for ( uint32_t i = 0U; i < size; i++ )
		{
			*runner = fill_val;
			runner++;
		}
	}

	m_percent_used = Calculate_Percent_Used();

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Hybrid::Allocate_Basic( size_t size, bool fill, uint8_t fill_val )
{
	void* temp_return = reinterpret_cast<void*>( nullptr );

	bool mem_allocation_failure = false;

	Push_TGINT();

	if ( size == 0U )
	{
		size = 1U;
	}

	if ( m_use_malloc == true )
	{
		temp_return = malloc( size );
		if ( temp_return == nullptr )
		{
			mem_allocation_failure = true;		// You need more XCL heap space.
			// The malloc heap size is stored in the xcl file (linker file).
		}

		if ( ( mem_allocation_failure == false ) && ( ( reinterpret_cast<uint32_t>( temp_return ) + size +
														m_malloc_overhead ) > ( m_index + RAM_HYBRID_HEAP_BEGIN ) ) )
		{
			m_index = reinterpret_cast<uint32_t>( temp_return ) + size
				+ m_malloc_overhead;
			m_index -= RAM_HYBRID_HEAP_BEGIN;
		}
	}
	else
	{
		if ( ( m_index + size ) < m_size )
		{
			temp_return = &m_mem_start[m_index];
			m_index += ( ( ( size - 1U ) / m_alignment_boundry_size )
						 + 1U ) * m_alignment_boundry_size;
		}
		else
		{
			mem_allocation_failure = true;		// You need more XCL heap space.
			// The malloc heap size is stored in the xcl file (linker file).
		}
	}

	Pop_TGINT();

	if ( ( fill == true ) && ( mem_allocation_failure == false ) )
	{
		uint8_t* runner = reinterpret_cast<uint8_t*>( temp_return );
		for ( uint32_t i = 0U; i < size; i++ )
		{
			*runner = fill_val;
			runner++;
		}

		m_percent_used = Calculate_Percent_Used();
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Hybrid::Reallocate( void* ptr, size_t size )
{
	if ( size == 0U )
	{
		size = 1U;
	}

	BF_ASSERT( m_use_malloc == true );
	Push_TGINT();
	ptr = realloc( ptr, size );
	BF_ASSERT( ptr != nullptr );
	if ( ( reinterpret_cast<uint32_t>( ptr ) + size +
		   m_malloc_overhead ) > ( m_index + RAM_HYBRID_HEAP_BEGIN ) )
	{
		m_index = reinterpret_cast<uint32_t>( ptr ) + size
			+ m_malloc_overhead;
		m_index -= RAM_HYBRID_HEAP_BEGIN;
	}

	Pop_TGINT();

	// if ( fill == true )
	// {
	// uint8_t* runner = reinterpret_cast<uint8_t*>( ptr );
	// for ( uint32_t i = 0U; i < size; i++ )
	// {
	// *runner = fill_val;
	// runner++;
	// }
	// }

	m_percent_used = Calculate_Percent_Used();

	return ( ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Hybrid::Reallocate_Basic( void* ptr, size_t size )
{
	bool mem_reallocation_failure = false;

	if ( size == 0U )
	{
		size = 1U;
	}

	if ( m_use_malloc == true )
	{
		Push_TGINT();
		ptr = realloc( ptr, size );
		if ( ptr == nullptr )
		{
			mem_reallocation_failure = true;	// You need more XCL heap space.
			// The malloc heap size is stored in the xcl file (linker file).
		}
		if ( ( mem_reallocation_failure == false ) && ( ( reinterpret_cast<uint32_t>( ptr ) + size +
														  m_malloc_overhead ) > ( m_index + RAM_HYBRID_HEAP_BEGIN ) ) )
		{
			m_index = reinterpret_cast<uint32_t>( ptr ) + size
				+ m_malloc_overhead;
			m_index -= RAM_HYBRID_HEAP_BEGIN;
		}
		Pop_TGINT();
	}
	if ( mem_reallocation_failure == false )
	{
		m_percent_used = Calculate_Percent_Used();
	}
	return ( ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Hybrid::De_Allocate( void* ptr )
{
	BF_ASSERT( ( m_use_malloc == true ) && ( ptr >= m_dyn_mem_start ) );
	BF_ASSERT( reinterpret_cast<uint32_t>( ptr ) <=
			   ( ( reinterpret_cast<uint32_t>( m_mem_start ) + m_index ) - m_malloc_overhead ) );

	Push_TGINT();
	free( ptr );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Hybrid::De_Allocate_Basic( void* ptr )
{
	if ( ( m_use_malloc == true ) && ( ptr >= m_dyn_mem_start ) &&
		 ( reinterpret_cast<uint32_t>( ptr ) <=
		   ( ( reinterpret_cast<uint32_t>( m_mem_start ) + m_index ) -
			 m_malloc_overhead ) ) )
	{
		Push_TGINT();

		free( ptr );

		Pop_TGINT();
	}
	else
	{
		/* need to decide what to do*/
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Hybrid::Calculate_Percent_Used( void ) const
{
	static const uint32_t PERCENTAGE_SCALING = 100U;
	uint32_t used_temp;

	used_temp = m_index * PERCENTAGE_SCALING;

	used_temp = used_temp / m_size;

	return ( static_cast<uint8_t>( used_temp ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Hybrid::Used( void )
{
	return ( m_percent_used );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
size_t Ram_Hybrid::Available( void )
{
	size_t return_size = 0U;

	if ( m_index <= m_size )
	{
		return_size = m_size - m_index;
	}

	return ( return_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Hybrid::operator new( size_t size )
{
	static bool allocated = false;
	static uint8_t ram_static_class_heap[sizeof( Ram_Hybrid )];
	void* temp_ptr = nullptr;

	// We do this to allow multiple instances of this RAM Handler to be created.
	if ( allocated == false )
	{
		temp_ptr = reinterpret_cast<void*>( ram_static_class_heap );
		allocated = true;
	}
	else
	{
		BF_ASSERT( allocated );
	}

	return ( temp_ptr );

	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'size' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

}	/* end namespace BF_Lib for this module */
