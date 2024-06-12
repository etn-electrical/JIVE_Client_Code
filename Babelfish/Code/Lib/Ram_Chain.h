/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Provides a class which can allocate memory from a chain of handlers.
 *
 *	@details In some cases we need a single allocation routine for a group of memory
 *	interfaces.  This will allow the user to allocate memory from a pool of different
 *	memory handlers.  The attached RAM handlers can also be used independently of
 *	this RAM chain interface.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_CHAIN_H
	#define RAM_CHAIN_H

#include "Ram_Handler.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Ram_Hybrid class and it allows dynamic memory allocation but
 * it should be used with a caution.
 * @details In some cases we need a single allocation routine for a group of memory
 *	interfaces.  This will allow the user to allocate memory from a pool of different
 *	memory handlers.  The attached RAM handlers can also be used independently of
 *	this RAM chain interface.
 *	Will cycle through different ramhandlers in a priority order.  Starting with
 *  array element 0 and working its way up till it finds a block of memory with the
 *  available space.
 * @n @b Usage:
 * @n @b 1. Pass in an array of ram handlers and indicate how many are linked.
 * @n @b 2. Use allocate function as normal.
 ****************************************************************************************
 */
class Ram_Chain : public BF_Lib::Ram_Handler
{
	public:

		/**
		 * @brief Will cycle through different ramhandlers in a priority order.  Starting with
		 * array element 0 and working its way up till it finds a block of memory with the
		 * available space.  The constructor will calculate the total amount of RAM
		 * available during construction.  This means it would be good to not allocate
		 * any memory from the arrayed ram handlers before the constructor is called.
		 * @param ram_array Array of ram handlers which will be searched 0th element first then
		 * cycles through 1,2 etc if available.
		 * @param num_handlers The number of ram handlers passed in.
		 */
		Ram_Chain( BF_Lib::Ram_Handler** ram_array, uint8_t num_handlers );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Ram_Chain() {}


		/**
		 *  @brief Will allocate memory from the ram handlers array.  Will work
		 *  from array element 0 and up to find a ram handler which has available
		 *  space.
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
		 *  @brief Return the pointer to the ram malloc heap array created.
		 *  Put in public space to allow the creation of a Ram_Chain instance.
		 *
		 *  SPECIAL NOTE: If you use multiple instances of this class then you should use
		 *  the following new operator overload.  See new with additional parameters.
		 *
		 * @param[in] size: The total data size in bytes to be allocated with memory.
		 * @return void* to the Allocated block.
		 */
		void* operator new( size_t size );

		/**
		 * @brief This operator overload works better for multiple instances.
		 * @details This overload of the new operator will allow the user to create the
		 * class instance by passing in the class memory.  See example below.
		 * What is happening below is that we are passing in the memory to be used to store
		 * the class (ram_class_instance).  It is important that the array of ram_class_instance
		 * is static or persists and is not destroyed after construction.
		 *
		 * 	static uint8_t ram_class_instance[sizeof(BF_Lib::Ram_Chain)];
		 * 	static Ram_Handler* ram_handle;
		 * 	ram_handle = new ( ram_class_instance ) BF_Lib::Ram_Chain( ram_array, ARRAY_LEN );
		 *
		 * @param[in] size: The total data size in bytes to be allocated with memory.
		 * @param[in] space_for_instance: This is an array of space which will be used to
		 * store the instance of the class.  It must be sized appropriately for the
		 * @return void* to the Allocated block.
		 */
		void* operator new( size_t size, uint8_t* space_for_instance );

		/**
		 * @brief
		 * @details Gives the used memory value.
		 * @return The percentage of the memory used.
		 */
		uint8_t Used( void );

		/**
		 * @brief Will return total available space left for all ram handlers linked in.
		 * @return Total ram available yet for all ram handlers summed.
		 */
		size_t Available( void );

	private:
		BF_Lib::Ram_Handler** m_ram_handlers;
		size_t m_total_ram;		///< Total amount of RAM available in the chain.
		uint8_t m_num_handlers;
};
}

#endif
