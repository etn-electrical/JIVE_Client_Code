/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Memory allocation and de-allocation from the section considered for dynamic
 *  memory.
 *
 *	@details This module provides an API to enable the dynamic memory allocation.
 *	But the feature is one way, once enabled it cannot be altered.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_HYBRID_H
#define RAM_HYBRID_H

#include "Ram_Handler.h"


namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Ram_Hybrid class and it allows dynamic memory allocation but
 * it should be used with a caution.
 * @details It provides an interface to allocate and de-allocate memory from the Heap
 * section created.
 * @n @b Usage:
 * @n @b 1. It provides public methods to: allocate and de-allocate memory,
 * get the status if the used memory is dynamically allocated.
 * @n @b 2. It has a function to enable the dynamic memory allocation.
 ****************************************************************************************
 */
class Ram_Hybrid : public BF_Lib::Ram_Handler
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Initializes the data members with appropriate values for memory start,
		 *  alignment boundary size, memory size, start of the dynamic memory.
		 *  Since this is using the one malloc available there is no way
		 *  to create multiple instances.
		 */
		Ram_Hybrid( uint8_t alignment_boundry_size =
					static_cast<uint8_t>( PROCESSOR_REGISTER_BASE_SIZE ) );

		/**
		 *  @brief Destructor for the Ram_Hybrid Module.
		 *  @details This will get called when object of Ram_Hybrid goes out of scope.
		 */
		~Ram_Hybrid() {}

		/**
		 *  @brief Return the pointer to the ram malloc heap array created.
		 *  @param[in] size: The total data size in bytes to be allocated with memory.
		 *  @return void* to the Allocated block.
		 */
		void* operator new( size_t );

		/**
		 *  @brief
		 *  @details returns the pointer to the allocated block, updates the malloc end and the
		 *  percentage memory used.
		 *  @param[in] size: The total data size to be allocated the memory.
		 *  @param[in] fill: The status bit whether to fill memory or not.
		 *  @param[in] fill_val: Values to be filled in memory, initially filled zero.
		 *  @return Void pointer to the Allocated memory.
		 */
		void* Allocate( size_t size, bool fill = true, uint8_t fill_val = 0U );

		/**
		 *  @brief
		 *  @details returns the pointer to the allocated block, updates the malloc end and the
		 *  percentage memory used.
		 *  @param[in] size: The total data size to be allocated the memory.
		 *  @param[in] fill: The status bit whether to fill memory or not.
		 *  @param[in] fill_val: Values to be filled in memory, initially filled zero.
		 *  @return Void pointer to the Allocated memory or a null pointer
		 */
		void* Allocate_Basic( size_t size, bool fill = true, uint8_t fill_val = 0U );

		/**
		 *  @brief
		 *  @details To De-Allocate the memory.
		 *  @param[in] ptr: Void Pointer to the allocated memory used to free that memory.
		 *  @return None
		 */
		void De_Allocate( void* ptr );

		/**
		 *  @brief
		 *  @details To De-Allocate the memory.
		 *  @param[in] ptr: Void Pointer to the allocated memory used to free that memory.
		 *  @return None
		 */
		void De_Allocate_Basic( void* ptr );

		/**
		 *  @brief
		 *  @details Gives the used memory value.
		 *  @return The percentage of the memory used.
		 */
		uint8_t Used( void );

		/**
		 * @brief Provides the amount of bytes left in this RAM handler.
		 * @details This can be used to determine ahead of time whether your allocation
		 * request will be allowed.  You can compare your size_t with the desired size.
		 * @return number of bytes remaining in this memory handler.
		 */
		size_t Available( void );

		/**
		 *  @brief To set the Heap dynamic.
		 *  @details This is a one way setting. You can only enable it once. From that point on
		 *  the heap is all dynamic. The static functionality is closed off.
		 *  @return None
		 */
		void Enable_Dynamic_Mem( void );

		/**
		 *  @brief
		 *  @details Indicates the state of the dynamic memory configuration, true
		 *  indicates dynamic memory is active which means that we are using standard
		 *  malloc/dealloc from the c++ library.
		 *  @return Status whether we are using dynamic memory or not.
		 *  @retval true indicates dynamic memory is active.
		 *  @retval false indicates dynamic memory is not in use currently.
		 */
		bool Dynamic_Enabled( void )
		{
			return ( m_use_malloc == true );
		}

		/**
		 *  @brief
		 *  @details this API is used to reallocate the dynamically
		 *  allocated memory to a new size and assign it to the same
		 *  pointer
		 *  @param[in] ptr, the dynamic pointer that needs to be reallocated.
		 *  @param[in] size indicates new dynamic memory to be used.
		 *  @return pointer to the newly dynamically allocated memory
		 */
		void* Reallocate( void* ptr, size_t size );

		/**
		 *  @brief
		 *  @details this API is used to reallocate the dynamically
		 *  allocated memory to a new size and assign it to the same
		 *  pointer
		 *  @param[in] ptr, the dynamic pointer that needs to be reallocated.
		 *  @param[in] size indicates new dynamic memory to be used.
		 *  @return pointer to the newly dynamically allocated memory or null pointer
		 */
		void* Reallocate_Basic( void* ptr, size_t size );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Ram_Hybrid( const Ram_Hybrid& rhs );
		Ram_Hybrid & operator =( const Ram_Hybrid& object );

		uint8_t Calculate_Percent_Used( void ) const;

		static const uint8_t RAM_HYBRID_MALLOC_OVERHEAD = 64U;
		size_t m_malloc_overhead;	// The amount of space used just for a single byte of data.
		uint8_t* m_mem_start;
		uint8_t* m_dyn_mem_start;
		uint32_t m_index;
		uint32_t m_size;
		uint8_t m_percent_used;
		bool m_use_malloc;
		uint8_t m_alignment_boundry_size;
};

}

#endif	/*RAM_HYBRID_H */
