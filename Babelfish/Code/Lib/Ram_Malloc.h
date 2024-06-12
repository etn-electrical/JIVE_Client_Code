/**
 *****************************************************************************************
 *	@file

 *	@brief Contains the Ram_Malloc class with functionalities mainly for Memory allocation
 *	and de-allocation from the Heap block created.
 *
 *	@details The heap size is stored in the linker file.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_MALLOC_H
#define RAM_MALLOC_H

#include "Ram_Handler.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Ram_Malloc class and it provides an interface to allocate
 * and de-allocate memory from the created Heap block.
 * @n @b Usage:
 * @n @b 1. Memory allocation and de-allocation from Heap block mainly.
 * @n @b 2. It provides public methods to: allocate and de-allocate memory,
 * get used memory value.
 ****************************************************************************************
 */
class Ram_Malloc : public BF_Lib::Ram_Handler
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Initializes the malloc start and end data members with segment 'heap' begin
		 *  values, and the size shall be the difference between the start and the end values.
		 */
		Ram_Malloc( void );

		/**
		 *  @brief Destructor for the Ram_Malloc Module.
		 *  @details This will get called when object of Ram_Malloc goes out of scope.
		 */
		~Ram_Malloc()
		{}

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
		 *  @details To De-Allocate the memory.
		 *  @param[in] ptr: Void Pointer to the allocated memory used to free that memory.
		 *  @return None
		 */
		void De_Allocate( void* ptr );

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

		/*
		 *  @brief
		 *  @details this API is used to reallocate the dynamically
		 *  allocated memory to a new size and assign it to the same
		 *  pointer
		 *  @param[in] ptr, the dynamic pointer that needs to be reallocated.
		 *  @param[in] size indicates new dynamic memory to be used.
		 *  @return pointer to the newly dynamically allocated memory
		 */
		void* Reallocate( void* ptr, size_t size );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Ram_Malloc( const Ram_Malloc& rhs );
		Ram_Malloc & operator =( const Ram_Malloc& object );

		uint8_t* m_malloc_start;
		uint8_t* m_malloc_end;
		uint32_t m_size;
		uint8_t m_percent_used;
		uint8_t Calculate_Percent_Used( void ) const;

};

}

#endif	/*RAM_MALLOC_H */

