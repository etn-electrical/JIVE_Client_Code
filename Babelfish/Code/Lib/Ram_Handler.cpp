/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_Handler.h"
#include "Babelfish_Assert.h"

namespace BF_Lib
{
/*
 *************************************************************************************************
 *  Ram Class static member attributes
   -------------------------------------------------------------------------------------------------
 */
uint8_t Ram_Handler::ram_handler_class_heap[sizeof( Ram_Handler ) *
											static_cast<uint8_t>
											( MAX_NUMBER_OF_RAM_BLOCKS )];

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Handler::operator new( size_t size )
{
	void* temp_ptr = reinterpret_cast<void*>( nullptr );
	static uint8_t allocated_blocks = 0U;

	BF_ASSERT( allocated_blocks < static_cast<uint8_t>( MAX_NUMBER_OF_RAM_BLOCKS ) );
	temp_ptr = reinterpret_cast<void*>( &ram_handler_class_heap[size * allocated_blocks] );

	allocated_blocks++;

	return ( temp_ptr );
}

}	/* end namespace BF_Lib for this module */
