/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_Static.h"
#include "Babelfish_Assert.h"

namespace BF_Lib
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ram_Static::Ram_Static( void* heap, uint32_t size, uint8_t alignment_boundry_size ) :
	Ram_Handler(),
	m_heap( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_size( size ),
	m_index( 0U ),
	m_alignment_boundry_size( alignment_boundry_size ),
	m_percent_used( 0U )
{
	// Check to make sure we have a size alignment with the micro.
	BF_ASSERT( ( size & ( static_cast<uint32_t>( PROCESSOR_REGISTER_BASE_SIZE ) - 1U ) ) == 0U );
	m_heap = reinterpret_cast<uint8_t*>( heap );
	m_size = size;
	m_alignment_boundry_size = alignment_boundry_size;
	m_index = 0U;
	m_percent_used = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Static::Allocate( size_t size, bool fill, uint8_t fill_val )
{
	void* ram_block = reinterpret_cast<void*>( nullptr );

	// If you got here you probably ran out of heap space.
	// Either increase your heap space or figure out who is requesting
	// too much RAM.
	// You likely need to increase the heap size for the static ram.
	// Search for "new Ram_Static(" then modify the heap size.
	BF_ASSERT( ( m_index + size ) <= m_size );

	if ( size > 0U )
	{
		Push_TGINT();

		ram_block = &m_heap[m_index];

		m_index += ( ( ( size - 1U ) / m_alignment_boundry_size ) + 1U )
			* m_alignment_boundry_size;
		Pop_TGINT();

		if ( fill == true )
		{
			uint8_t* new_data = reinterpret_cast<uint8_t*>( ram_block );

			while ( size > 0U )
			{
				size--;
				new_data[size] = fill_val;
			}
		}
		m_percent_used = Calculate_Percent_Used();
	}

	return ( ram_block );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Static::Allocate_Basic( size_t size, bool fill, uint8_t fill_val )
{
	void* ram_block = reinterpret_cast<void*>( nullptr );

	if ( ( m_index + size ) <= m_size )
	{
		if ( size > 0U )
		{
			Push_TGINT();

			ram_block = &m_heap[m_index];

			m_index += ( ( ( size - 1U ) / m_alignment_boundry_size ) + 1U )
				* m_alignment_boundry_size;
			Pop_TGINT();

			if ( fill == true )
			{
				uint8_t* new_data = reinterpret_cast<uint8_t*>( ram_block );

				while ( size > 0U )
				{
					size--;
					new_data[size] = fill_val;
				}
			}
			m_percent_used = Calculate_Percent_Used();
		}
	}

	return ( ram_block );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Static::Reallocate( void* ptr, size_t size )
{
	BF_ASSERT( false );
	return ( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Static::Reallocate_Basic( void* ptr, size_t size )
{
	BF_ASSERT( false );
	return ( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Static::De_Allocate( void* ptr )
{
	BF_ASSERT( false );		// RAM Static does not allow de-allocate.
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'ptr' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_Static::De_Allocate_Basic( void* ptr )
{
	BF_ASSERT( false );		// RAM Static does not allow de-allocate.
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'ptr' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Static::Calculate_Percent_Used( void ) const
{
	BF_Lib::SPLIT_UINT32 used_temp;

	used_temp.u32 = m_index;
	used_temp.u32 = used_temp.u32 * 100U;

	used_temp.u8[0] = static_cast<uint8_t>( used_temp.u32 / m_size );

	return ( used_temp.u8[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram_Static::Used( void )
{
	return ( m_percent_used );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
size_t Ram_Static::Available( void )
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
void* Ram_Static::operator new( size_t size )
{
	static uint8_t allocated_blocks = 0x00U;
	static uint8_t ram_static_class_heap[sizeof( Ram_Static ) *
										 static_cast<uint8_t>( MAX_NUMBER_OF_RAM_BLOCKS )];
	void* temp_ptr = reinterpret_cast<void*>( nullptr );

	BF_ASSERT( allocated_blocks < static_cast<uint8_t>( MAX_NUMBER_OF_RAM_BLOCKS ) );
	temp_ptr = reinterpret_cast<void*>( &ram_static_class_heap[size * allocated_blocks] );

	allocated_blocks++;

	return ( temp_ptr );

#if 0
	// The following code should replace the code above at a point when we are ready to
	// save some ram for this class.  We should deprecate the above code and note the change.
	// The code below will cause an exception if you try to allocate more than one instance.
	// This is by design.  Allocating static memory for potential RAM instances which
	// will likely never be created wastes ram.
	// The waste is because we crudely allocate enough ram for all possible
	// instances of static ram handlers.
	// We can save that space by having an adopter pass in the static memory
	// they want to use for the instance.
	// See the alternate new operator overload below as well as the definition.
	static bool allocated = false;
	static uint8_t ram_static_class_heap[sizeof( Ram_Static )];
	void* temp_ptr = reinterpret_cast<void*>( nullptr );

	// If you have arrived here it is because you tried to allocate more than
	// one ram static without using the other operator overload.  See below operator
	// new overload.  This requires the passing in of a string to store the value
	BF_ASSERT( allocated == false );

	temp_ptr = reinterpret_cast<void*>( ram_static_class_heap );
	allocated = true;

	return ( temp_ptr );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Static::operator new( size_t size, uint8_t* space_for_instance )
{
	void* temp_ptr = nullptr;

	// You passed in a null ptr for ram.  That is just strange.
	BF_ASSERT( space_for_instance != nullptr );

	temp_ptr = reinterpret_cast<void*>( space_for_instance );

	return ( temp_ptr );
}

}	/* end namespace BF_Lib for this module */
