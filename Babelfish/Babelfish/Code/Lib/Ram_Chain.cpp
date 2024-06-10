/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_Chain.h"
#include "Babelfish_Assert.h"
/*
 *************************************************************************************************
 *  Ram Class static member attributes
   -------------------------------------------------------------------------------------------------
 */
namespace BF_Lib
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ram_Chain::Ram_Chain( BF_Lib::Ram_Handler** ram_array, uint8_t num_handlers ) :
	m_ram_handlers( ram_array ),
	m_num_handlers( num_handlers ),
	m_total_ram( 0U )
{
	m_ram_handlers = ram_array;
	m_num_handlers = num_handlers;

	// This makes the assumption that none of the passed in memory has not been allocated at all.
	// We calculate the total amount of ram available.
	for ( uint_fast8_t i = 0U; i < m_num_handlers; i++ )
	{
		m_total_ram += ( m_ram_handlers[i] )->Available();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Chain::Allocate( size_t size, BOOL fill, UINT8 fill_val )
{
	uint_fast8_t iterator = 0U;
	void* new_ram = nullptr;

	Push_TGINT();

	// Find the first memory allocation block which has the space available.
	while ( ( iterator < m_num_handlers ) &&
			( ( m_ram_handlers[iterator] )->Available() < size ) )
	{
		iterator++;
	}

	BF_ASSERT( iterator < m_num_handlers );
	new_ram = ( m_ram_handlers[iterator] )->Allocate( size, fill, fill_val );

	Pop_TGINT();

	return ( new_ram );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Chain::De_Allocate( void* ptr )
{
	BF_ASSERT( false );			// Not handled at this time.  Consider for future.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Chain::Used( void )
{
	static const uint32_t PERCENTAGE_SCALING = 100U;
	size_t currently_available = 0U;
	uint32_t used_temp;

	// Here we calculate the amount of total ram is available.
	for ( uint_fast8_t i = 0U; i < m_num_handlers; i++ )
	{
		currently_available += ( m_ram_handlers[i] )->Available();
	}

	used_temp = ( m_total_ram - currently_available ) * PERCENTAGE_SCALING;

	used_temp = used_temp / m_total_ram;

	return ( static_cast<uint8_t>( used_temp ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Chain::operator new( size_t size )
{
	static bool allocated = false;
	static uint8_t ram_static_class_heap[sizeof( Ram_Chain )];
	void* temp_ptr = nullptr;

	// We do this to allow multiple instances of this RAM Handler to be created.
	BF_ASSERT( allocated == false );
	temp_ptr = reinterpret_cast<void*>( ram_static_class_heap );
	allocated = true;

	return ( temp_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Chain::operator new( size_t size, uint8_t* space_for_instance )
{
	void* temp_ptr = nullptr;

	// You passed in a null ptr for ram.  That is just strange.
	BF_ASSERT( space_for_instance != nullptr );
	temp_ptr = reinterpret_cast<void*>( space_for_instance );

	return ( temp_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
size_t Ram_Chain::Available( void )
{
	size_t return_size = 0U;

	for ( uint_fast8_t i = 0U; i < m_num_handlers; i++ )
	{
		return_size += ( m_ram_handlers[i] )->Available();
	}

	return ( return_size );
}

}
