/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_Malloc.h"
#include "Babelfish_Assert.h"
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule 8-0-1: A +headerwarn option was previously issued for header 'Stdlib.h'
 * @n PCLint:
 * @n Justification: Stdlib.h is part of IAR includes
 */
/*lint -e829*/
#include "Stdlib.h"
/*lint +e829*/
namespace BF_Lib
{
/*
 *************************************************************************************************
 *  Ram Class static member attributes
   -------------------------------------------------------------------------------------------------
 */
#pragma segment="HEAP" __data
#define RAM_MALLOC_HEAP_BEGIN   __segment_begin( "HEAP" )
#define RAM_MALLOC_HEAP_END     __segment_end( "HEAP" )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ram_Malloc::Ram_Malloc( void ) :
	m_malloc_start( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_malloc_end( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_size( 0U ),
	m_percent_used( 0U ),
	Ram_Handler()
{
	m_percent_used = 0U;
	m_malloc_start = reinterpret_cast<uint8_t*>( RAM_MALLOC_HEAP_BEGIN );
	m_malloc_end = reinterpret_cast<uint8_t*>( RAM_MALLOC_HEAP_BEGIN );
	m_size = reinterpret_cast<uint8_t*>( RAM_MALLOC_HEAP_END ) -
		reinterpret_cast<uint8_t*>( RAM_MALLOC_HEAP_BEGIN );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Malloc::Allocate( size_t size, bool fill, uint8_t fill_val )
{
	void* temp_return;

	Push_TGINT();

	temp_return = malloc( size );

	Pop_TGINT();

	// The malloc heap size is stored in the xcl file (linker file).
	BF_ASSERT( temp_return != nullptr );
	if ( fill == true )
	{
		uint8_t* runner = reinterpret_cast<uint8_t*>( temp_return );
		for ( uint32_t i = 0U; i < size; i++ )
		{
			*runner = fill_val;
			runner++;
		}
	}

	if ( ( reinterpret_cast<uint8_t*>( temp_return ) + size ) > m_malloc_end )
	{
		m_malloc_end = reinterpret_cast<uint8_t*>( temp_return ) + size;
	}

	m_percent_used = Calculate_Percent_Used();

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Malloc::De_Allocate( void* ptr )
{
	BF_ASSERT( ( ptr > m_malloc_start ) &&
			   ( ptr < m_malloc_end ) );

	Push_TGINT();

	free( ptr );

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Malloc::Calculate_Percent_Used( void ) const
{
	uint32_t used_size;
	uint8_t used_percent;

	used_size = reinterpret_cast<uint32_t>( m_malloc_end ) -
		reinterpret_cast<uint32_t>( m_malloc_start );
	used_size = used_size * 100U;

	used_percent = static_cast<uint8_t>( used_size / m_size );

	return ( used_percent );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Malloc::Used( void )
{
	return ( m_percent_used );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
size_t Ram_Malloc::Available( void )
{
	size_t return_size = 0U;
	size_t bytes_used;

	bytes_used = reinterpret_cast<size_t>( m_malloc_end ) -
		reinterpret_cast<size_t>( m_malloc_start );

	if ( bytes_used <= m_size )
	{
		return_size = m_size - bytes_used;
	}

	return ( return_size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Malloc::operator new( size_t size )
{
	static void* temp_ptr = reinterpret_cast<void*>( nullptr );
	static uint8_t ram_malloc_class_heap[sizeof( Ram_Malloc )];

	BF_ASSERT( temp_ptr == nullptr );
	temp_ptr = reinterpret_cast<void*>( &ram_malloc_class_heap[0] );

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
