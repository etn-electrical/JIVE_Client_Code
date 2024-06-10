/**
 *****************************************************************************************
 *	@file
 *
 *	@brief It works with a static area using arrays creation for memory allocation.
 *
 *	@details RAM Static does not allow de-allocate. It uses the option to do an
 *	exception stop here i.e. it handles the exception internally.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_STATIC_H
#define RAM_STATIC_H

#include "Ram_Handler.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Ram_Static class used for the memory allocation and
 *  RAM Static does not allow to de-allocate memory.
 * @details It provides an interface to allocate memory from the created
 * Heap block. It creates the count of the number of blocks (arrays) created.
 * @n @b Usage:
 * @n @b 1. Memory allocation from the created Heap area.
 * @n @b 2. It provides public methods to: allocate, get used memory value.
 ****************************************************************************************
 */
class Ram_Static : public BF_Lib::Ram_Handler
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Initializes the data members with appropriate values of the heap, the memory
		 *  size and the alignment boundary size.
		 *  @param[in] heap: The address of the static ram array created.
		 *  @param[in] size: The static ram block size.
		 *  @param[in] alignment_boundry_size: The particular Ram block either the Main or DCI for
		 * instance.
		 */
		Ram_Static( void* heap, uint32_t size, uint8_t alignment_boundry_size );

		/**
		 *  @brief Destructor for the Ram_Static Module.
		 *  @details This will get called when object of Ram_Static goes out of scope.
		 */
		~Ram_Static()
		{}

		/**
		 *  @brief Return the pointer to the ram static heap array created.
		 *  @param[in] size: The total data size in bytes to be allocated with memory.
		 *  @return void* to the Allocated block.
		 */
		void* operator new( size_t );

		/**
		 * @brief This operator overload works better for multiple instances.
		 * @details This overload of the new operator will allow the user to create the
		 * class instance by passing in the class memory.  See example below.
		 * What is happening below is that we are passing in the memory to be used to store
		 * the class (ram_class_instance).  It is important that the array of ram_class_instance
		 * is static or persists and is not destroyed after construction.
		 *
		 *  static uint8_t ram_class_instance[sizeof(BF_Lib::Ram_Static)];
		 *  static Ram_Handler* ram_handle;
		 *  ram_handle = new ( ram_class_instance ) BF_Lib::Ram_Static( STATIC_RAM_SIZE );
		 *
		 * @param[in] size: The total data size in bytes to be allocated with memory.
		 * @param[in] space_for_instance: This is an array of space which will be used to
		 * store the instance of the class.  It must be sized appropriately for the
		 * @return void* to the Allocated block.
		 */
		void* operator new( size_t size, uint8_t* space_for_instance );

		/**
		 *  @brief
		 *  @details It shall allocate the specified amount of memory.
		 *  @param[in] size: The total data size to be allocated the memory.
		 *  @param[in] fill: The status bit whether to fill memory or not.
		 *  @param[in] fill_val: Value to be filled in memory, initially filled zero.
		 *  @return Void pointer to the newly allocated memory.
		 */
		void* Allocate( size_t size, bool fill = true, uint8_t fill_val = 0U );

		/**
		 *  @brief
		 *  @details It shall allocate the specified amount of memory.
		 *  @param[in] size: The total data size to be allocated the memory.
		 *  @param[in] fill: The status bit whether to fill memory or not.
		 *  @param[in] fill_val: Value to be filled in memory, initially filled zero.
		 *  @return Void pointer to the newly allocated memory.
		 */
		void* Allocate_Basic( size_t size, bool fill = true, uint8_t fill_val = 0U );

		/**
		 *  @brief RAM Static does not allow de-allocate.
		 *  @details Ram_Static uses the option to do an exception stop here.
		 *  @param[in] ptr: The pointer to de-allocate.
		 */
		void De_Allocate( void* ptr );

		/**
		 *  @brief RAM Static does not allow de-allocate.
		 *  @details Ram_Static uses the option to do an exception stop here.
		 *  @param[in] ptr: The pointer to de-allocate.
		 */
		void De_Allocate_Basic( void* ptr );

		/**
		 *  @brief Provide a percent usage of a specific block of Ram memory.
		 *  @details It shall provide a percentage used value.
		 *  @return The percentage of memory used in integer value.
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
		 *  @brief Shall reallocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the allocate fails an internal exception will occur.
		 *  @param[in] ptr: The pointer to the newly reallocated memory
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @return Pointer to the newly reallocated memory.
		 */
		void* Reallocate( void* ptr, size_t size );

		/**
		 *  @brief Shall reallocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the reallocate fails an internal exception will occur.
		 *  @param[in] ptr: The pointer to the newly reallocated memory
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @return Pointer to the newly reallocated memory.
		 */
		void* Reallocate_Basic( void* ptr, size_t size );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Ram_Static( const Ram_Static& rhs );
		Ram_Static & operator =( const Ram_Static& object );

		uint8_t Calculate_Percent_Used( void ) const;

		uint8_t* m_heap;
		uint32_t m_size;
		uint32_t m_index;
		uint8_t m_alignment_boundry_size;
		uint8_t m_percent_used;
};

}

#endif	/*RAM_STATIC_H*/

