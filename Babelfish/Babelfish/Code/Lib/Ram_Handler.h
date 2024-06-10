/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module contains implementation for the memory allocation and deallocation.
 *
 *	@details It is from a customized new operator created that return a pointer to an array
 * referring
 *	to heap block created.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_HANDLER_H
#define RAM_HANDLER_H

#include "stdint.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief The Ram_Handler class is being inherited by the Ram_Static and Ram_Malloc classes.
 * All used for the memory allocation and deallocation.
 *
 * @details It is from a customized new operator created that return a pointer to an array
 * referring to heap block created.
 ****************************************************************************************
 */
class Ram_Handler
{
	public:

		/**
		 *  @brief Constructor
		 *  @details It does nothing here.
		 */
		Ram_Handler( void )
		{}

		/**
		 *  @brief Destructor
		 *  @details It does nothing here.
		 */
		virtual ~Ram_Handler()
		{}

		/**
		 *  @brief Return the pointer to the ram heap array created.
		 *  @param[in] size: It takes in the total data size in bytes to be allocated with memory.
		 *  @return void pointer to the Allocated block.
		 */
		void* operator new( size_t );

		/**
		 *  @brief To deallocate memory.
		 *  @param[in] ptr: The ptr to the memory to be de-allocated.
		 */
		void operator delete( void* ptr )
		{}

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Ram_Hybrid and Ram_Static.
		 *  @details Their purpose is to allocate, deallocate and calculate the used memory
		 */
		virtual void* Allocate( size_t size, bool fill = true, uint8_t fill_val = 0U ) = 0;

		virtual void* Allocate_Basic( size_t size, bool fill = true, uint8_t fill_val = 0U ) = 0;

		virtual void De_Allocate( void* ptr ) = 0;

		virtual void De_Allocate_Basic( void* ptr ) = 0;

		virtual uint8_t Used( void ) = 0;		// The percentage used.

		/**
		 * @brief Provides the amount of bytes left in this RAM handler.
		 *
		 * @return number of bytes remaining in this memory handler.
		 */
		virtual size_t Available( void ) = 0;

		virtual void* Reallocate( void* ptr, size_t size ) = 0;

		virtual void* Reallocate_Basic( void* ptr, size_t size ) = 0;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Ram_Handler( const Ram_Handler& rhs );
		Ram_Handler & operator =( const Ram_Handler& object );

		static uint8_t ram_handler_class_heap[];

};

}

#endif	/* RAM_HANDLER_H */
