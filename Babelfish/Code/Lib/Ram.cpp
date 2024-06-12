/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram.h"
#include "Babelfish_Assert.h"
#include "Ram_Debug.h"

// #define IAR_HEAP_MONITOR

#ifdef IAR_HEAP_MONITOR
#include "mallocstats.h"
#endif

// Include this header if we use OS_Tasker::Delay()
#include "OS_Tasker.h"

/*
 *************************************************************************************************
 *  Ram Class static member attributes
   -------------------------------------------------------------------------------------------------
 */
BF_Lib::Ram_Handler* Ram::m_ram_ptrs[MAX_NUMBER_OF_RAM_BLOCKS];
uint8_t Ram::m_default_alloc_block;
uint8_t Ram::m_default_dealloc_block;
bool_t Ram::m_dynamic_mem_alloc_prohibited = false;

// Not a static member because it is accessed by the new operator overload.
static uint8_t m_default_new_operator_block;
static uint8_t m_default_delete_operator_block;

#ifdef RAM_DIAGNOSTICS
uint16_t Ram::m_data_distribution[MAX_NUMBER_OF_RAM_BLOCKS][RAM_DATA_DISTRIBUTION_ITEMS];
uint32_t Ram::m_data_size_large[MAX_NUMBER_OF_RAM_BLOCKS];
uint32_t Ram::m_data_size_total[MAX_NUMBER_OF_RAM_BLOCKS];
uint8_t Ram::m_percent_used[MAX_NUMBER_OF_RAM_BLOCKS];
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Init( void )
{
	for ( uint8_t i = 0U; i < MAX_NUMBER_OF_RAM_BLOCKS; i++ )
	{
		m_ram_ptrs[i] = reinterpret_cast<BF_Lib::Ram_Handler*>( nullptr );
#ifdef RAM_DIAGNOSTICS
		m_data_size_large[i] = 0U;
		for ( uint8_t j = 0U; j < RAM_DATA_DISTRIBUTION_ITEMS; j++ )
		{
			m_data_distribution[i][j] = 0U;
		}
#endif
	}

	m_default_new_operator_block = DEFAULT_MEMORY_ALLOC_BLOCK;
	m_default_alloc_block = DEFAULT_MEMORY_ALLOC_BLOCK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Assign_Block( BF_Lib::Ram_Handler* ram_handle, uint8_t ram_block_num )
{
	if ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS )
	{
		m_ram_ptrs[ram_block_num] = ram_handle;
	}
	else
	{
		RAM_DEBUG_PRINT( BF_DBG_MSG_ERROR, "Error: Assign_Block failed." );
		BF_ASSERT( ram_block_num >= MAX_NUMBER_OF_RAM_BLOCKS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef RAM_TERM_PRINT
	#pragma optimize=none
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram::Allocate( size_t size, bool fill, uint8_t fill_val, uint8_t ram_block_num )
{
	void* temp_return = nullptr;

	// Dynamic memory allocation may not be permitted once the system has entered run-time mode,
	// ie, if m_dynamic_mem_alloc_prohibited is set True.
	// If you need to allocate memory in those configurations, do it while the system
	// is still in start-up mode.
	BF_ASSERT( !m_dynamic_mem_alloc_prohibited );

	if ( m_dynamic_mem_alloc_prohibited == false )
	{
		if ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS )
		{
			if ( m_ram_ptrs[ram_block_num] != nullptr )
			{
				temp_return = m_ram_ptrs[ram_block_num]->Allocate( size, fill, fill_val );

#ifdef RAM_DIAGNOSTICS
				Push_TGINT();

				m_percent_used[ram_block_num] = m_ram_ptrs[ram_block_num]->Used();


				if ( size < RAM_DATA_DISTRIBUTION_ITEMS )
				{
					if ( m_data_distribution[ram_block_num][size] < MAX_DATA_DISTRIB_COUNT )
					{
						m_data_distribution[ram_block_num][size]++;
					}
				}
				else
				{
					if ( m_data_distribution[ram_block_num][0] < MAX_DATA_DISTRIB_COUNT )
					{
						m_data_distribution[ram_block_num][0]++;
					}
					if ( m_data_size_large[ram_block_num] < ( MAX_DATA_SIZE_SUM - size ) )
					{
						m_data_size_large[ram_block_num] += size;
					}
				}

				m_data_size_total[ram_block_num] += size;

				Pop_TGINT();

				RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Size Alloc: %d, Percent Used: %d",
								 ram_block_num, temp_return, size, m_percent_used[ram_block_num] );
#else
				RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Size Alloc: %d", ram_block_num,
								 temp_return, size );
#endif
			}
		}

		BF_ASSERT( temp_return != nullptr );
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram::Allocate_Basic( size_t size, bool fill, uint8_t fill_val, uint8_t ram_block_num )
{
	void* temp_return = reinterpret_cast<void*>( nullptr );

	if ( m_dynamic_mem_alloc_prohibited == false )
	{
		if ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS )
		{
			if ( m_ram_ptrs[ram_block_num] != reinterpret_cast<BF_Lib::Ram_Handler*>( nullptr ) )
			{
				temp_return = m_ram_ptrs[ram_block_num]->Allocate_Basic( size, fill, fill_val );
				if ( temp_return != nullptr )
				{
#ifdef RAM_DIAGNOSTICS
					Push_TGINT();

					m_percent_used[ram_block_num] = m_ram_ptrs[ram_block_num]->Used();


					if ( size < RAM_DATA_DISTRIBUTION_ITEMS )
					{
						if ( m_data_distribution[ram_block_num][size] < MAX_DATA_DISTRIB_COUNT )
						{
							m_data_distribution[ram_block_num][size]++;
						}
					}
					else
					{
						if ( m_data_distribution[ram_block_num][0] < MAX_DATA_DISTRIB_COUNT )
						{
							m_data_distribution[ram_block_num][0]++;
						}
						if ( m_data_size_large[ram_block_num] < ( MAX_DATA_SIZE_SUM - size ) )
						{
							m_data_size_large[ram_block_num] += size;
						}
					}

					m_data_size_total[ram_block_num] += size;

					Pop_TGINT();

					RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Basic Alloc: %d, Percent Used: %d",
									 ram_block_num, temp_return, size, m_percent_used[ram_block_num] );
#else
					RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Basic Alloc: %d", ram_block_num,
									 temp_return, size );
#endif
#ifdef RAM_TERM_PRINT
					/* Catch the cases that look like our leaks, normally by size:
					 * (This example shows a test for a size of 384) */
					if ( ram_term_print_enable && ( size == 384 ) )
					{
						RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Set a breakpoint here!" );
						// Be sure that #pragma optimize=none is set above for this breakpoint to hit correctly, just on
						// this case.
						// OS_Tasker::Delay( 2 );		// msec
					}
#endif
				}
			}
		}
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram::Reallocate( void* ptr, size_t size, uint8_t ram_block_num )
{
	void* temp_return = nullptr;

	// Dynamic memory re-allocation may not be permitted once the system has entered run-time mode,
	// ie, if m_dynamic_mem_alloc_prohibited is set True.
	// If you need to allocate memory in those configurations, do it while the system
	// is still in start-up mode.
	BF_ASSERT( !m_dynamic_mem_alloc_prohibited );

	if ( m_dynamic_mem_alloc_prohibited == false )
	{
		if ( ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS ) && Ram::Valid_Ram_Ptr( ptr ) )
		{
			if ( m_ram_ptrs[ram_block_num] != nullptr )
			{
				temp_return = m_ram_ptrs[ram_block_num]->Reallocate( ptr, size );

#ifdef RAM_DIAGNOSTICS
				Push_TGINT();

				m_percent_used[ram_block_num] = m_ram_ptrs[ram_block_num]->Used();


				if ( size < RAM_DATA_DISTRIBUTION_ITEMS )
				{
					if ( m_data_distribution[ram_block_num][size] < MAX_DATA_DISTRIB_COUNT )
					{
						m_data_distribution[ram_block_num][size]++;
					}
				}
				else
				{
					if ( m_data_distribution[ram_block_num][0] < MAX_DATA_DISTRIB_COUNT )
					{
						m_data_distribution[ram_block_num][0]++;
					}
					if ( m_data_size_large[ram_block_num] < ( MAX_DATA_SIZE_SUM - size ) )
					{
						m_data_size_large[ram_block_num] += size;
					}
				}

				m_data_size_total[ram_block_num] += size;

				Pop_TGINT();

				RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Size Realloc: %d, Percent Used: %d\n",
								 ram_block_num, temp_return, size, m_percent_used[ram_block_num] );
#else
				RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Size Realloc: %d\n", ram_block_num,
								 temp_return, size );
#endif
#ifdef RAM_TERM_PRINT
				// Provide some breathing room, since we're printing an awful lot of these messages
				// OS_Tasker::Delay( 2 );		// msec
#endif
			}
		}

		BF_ASSERT( temp_return != nullptr );
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram::Reallocate_Basic( void* ptr, size_t size, uint8_t ram_block_num )
{
	void* temp_return = nullptr;

	if ( m_dynamic_mem_alloc_prohibited == false )
	{
		if ( ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS ) && Ram::Valid_Ram_Ptr( ptr ) )
		{
			if ( m_ram_ptrs[ram_block_num] != nullptr )
			{
				temp_return = m_ram_ptrs[ram_block_num]->Reallocate_Basic( ptr, size );
				if ( temp_return != nullptr )
				{
#ifdef RAM_DIAGNOSTICS
					Push_TGINT();

					m_percent_used[ram_block_num] = m_ram_ptrs[ram_block_num]->Used();


					if ( size < RAM_DATA_DISTRIBUTION_ITEMS )
					{
						if ( m_data_distribution[ram_block_num][size] < MAX_DATA_DISTRIB_COUNT )
						{
							m_data_distribution[ram_block_num][size]++;
						}
					}
					else
					{
						if ( m_data_distribution[ram_block_num][0] < MAX_DATA_DISTRIB_COUNT )
						{
							m_data_distribution[ram_block_num][0]++;
						}
						if ( m_data_size_large[ram_block_num] < ( MAX_DATA_SIZE_SUM - size ) )
						{
							m_data_size_large[ram_block_num] += size;
						}
					}

					m_data_size_total[ram_block_num] += size;

					Pop_TGINT();

					RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Basic Realloc: %d, Percent Used: %d",
									 ram_block_num, temp_return, size, m_percent_used[ram_block_num] );
#else
					RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p, Basic Realloc: %d", ram_block_num,
									 temp_return, size );
#endif
				}
			}
		}
	}
	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ram::Used( uint8_t ram_block_num )
{
	uint8_t used_percent = 0U;

	if ( ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS ) &&
		 ( m_ram_ptrs[ram_block_num] != reinterpret_cast<BF_Lib::Ram_Handler*>( nullptr ) ) )
	{
		used_percent = m_ram_ptrs[ram_block_num]->Used();
	}
#ifdef IAR_HEAP_MONITOR
	SHOW_HEAP_STASTICS( 1 );
#endif
	return ( used_percent );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
size_t Ram::Available( uint8_t ram_block_num )
{
	size_t free_space = 0U;

	if ( ( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS ) &&
		 ( m_ram_ptrs[ram_block_num] != reinterpret_cast<BF_Lib::Ram_Handler*>( nullptr ) ) )
	{
		free_space = m_ram_ptrs[ram_block_num]->Available();
	}

	return ( free_space );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::De_Allocate( void* ptr, uint8_t ram_block_num )
{
	// Dynamic memory de-allocation may not be permitted once the system has entered run-time mode,
	// ie, if m_dynamic_mem_alloc_prohibited is set True.
	// If you need to de-allocate memory in those configurations, do it while the system
	// is still in start-up mode.
	BF_ASSERT( !m_dynamic_mem_alloc_prohibited );
	BF_ASSERT( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS );
	BF_ASSERT( m_ram_ptrs[ram_block_num] != nullptr );

	if ( ( m_dynamic_mem_alloc_prohibited == false ) && Ram::Valid_Ram_Ptr( ptr ) )
	{
		m_ram_ptrs[ram_block_num]->De_Allocate( ptr );
		RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p", ram_block_num, ptr );
#ifdef RAM_TERM_PRINT
		if ( ram_term_print_enable )
		{
			// Provide some breathing room, since we're printing an awful lot of these messages
			// OS_Tasker::Delay( 2 );		// msec
		}
#endif
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::De_Allocate_Basic( void* ptr, uint8_t ram_block_num )
{
	BF_ASSERT( !m_dynamic_mem_alloc_prohibited );
	BF_ASSERT( ram_block_num < MAX_NUMBER_OF_RAM_BLOCKS );
	BF_ASSERT( m_ram_ptrs[ram_block_num] != nullptr );

	if ( ( m_dynamic_mem_alloc_prohibited == false ) && Ram::Valid_Ram_Ptr( ptr ) )
	{
		m_ram_ptrs[ram_block_num]->De_Allocate_Basic( ptr );
		RAM_DEBUG_PRINT( DBG_MSG_DEBUG, "Ram #:%d, Ptr: %p", ram_block_num, ptr );

#ifdef RAM_TERM_PRINT
		if ( ram_term_print_enable )
		{
			// Provide some breathing room, since we're printing an awful lot of these messages
			// OS_Tasker::Delay( 2 );		// msec
		}
#endif
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Set_Default_New_Op_Alloc_Block( uint8_t new_operator_block )
{
	BF_ASSERT( new_operator_block < MAX_NUMBER_OF_RAM_BLOCKS );
	m_default_new_operator_block = new_operator_block;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Set_Default_Delete_Op_Alloc_Block( uint8_t delete_operator_block )
{
	BF_ASSERT( delete_operator_block < MAX_NUMBER_OF_RAM_BLOCKS );
	m_default_delete_operator_block = delete_operator_block;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Set_Default_Alloc_Block( uint8_t alloc_block )
{
	BF_ASSERT( alloc_block < MAX_NUMBER_OF_RAM_BLOCKS );
	m_default_alloc_block = alloc_block;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Set_Default_Dealloc_Block( uint8_t dealloc_block )
{
	BF_ASSERT( dealloc_block < MAX_NUMBER_OF_RAM_BLOCKS );
	m_default_dealloc_block = dealloc_block;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram::Prohibit_Dynamic_Memory_Allocation( bool param )
{
	m_dynamic_mem_alloc_prohibited = param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* operator new( size_t size, uint8_t ram_block_num )
{
	// This is the global overload of the new operator.  It forces everything to use the
	// Ram class to allocate memory
	return ( Ram::Allocate( size, true, 0U, ram_block_num ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void* operator new( size_t size )
{
	// This is the global overload of the new operator.  It forces everything to use the
	// Ram class to allocate memory
	return ( Ram::Allocate( size, true, 0U, m_default_new_operator_block ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void operator delete( void* ptr )
{
	// This is the global overload of the delete operator.  It forces everything to use the
	// Ram class to deallocate memory
	if ( ptr != nullptr )
	{
		Ram::De_Allocate( ptr, m_default_delete_operator_block );
	}
}

extern "C"
{
extern uint8_t __ICFEDIT_region_RAM_start__;
extern uint8_t __ICFEDIT_region_RAM_end__;
}

// static const uint32_t ram_start = (uint32_t) ( &__ICFEDIT_region_RAM_start__ );
// static const uint32_t ram_end = (uint32_t) ( &__ICFEDIT_region_RAM_end__ );
bool Ram::Valid_Ram_Ptr( void* mem_ptr )
{
	// First, exclude the easy case: nullptr
	if ( mem_ptr == nullptr )
	{
		return ( false );
	}
#ifndef ESP32_SETUP
	// Then the more interesting case: is it outside the valid range for Ram-based data (ie, the heap) ?
	else if ( ( ( uint32_t ) ( mem_ptr ) < ( uint32_t ) ( &__ICFEDIT_region_RAM_start__ ) ) ||
			  ( ( uint32_t ) ( mem_ptr ) > ( uint32_t ) ( &__ICFEDIT_region_RAM_end__ ) ) )
	{
		RAM_DEBUG_PRINT( DBG_MSG_ERROR, "Warning: Bad RAM pointer: %p", mem_ptr );
		#ifndef NDEBUG		// If not release code
		BF_ASSERT( false );
		#endif
		return ( false );
	}
	else
	{
		return ( true );
	}
#else
	return ( true );

#endif

}
