/**
 **************************************************************************************************
 *  @file
 *  @brief DSI_Buffer.h This file provides APIs to access Data Stream APIs
 *
 *	@details This merely abstracts away the memory management.  It is optional for the publisher.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef DSI_BUFFER_H
#define DSI_BUFFER_H

#include "Includes.h"
#include "DSI_Core.h"

class DSI_Buffer
{
	public:
		/**
		 * @brief Allocate the specified amount of RAM block
		 * @param[in] size: The size in bytes of memory to be allocated.
		 * @return Returns valid base address of space allocated
		 */
		static void* Allocate( size_t size );

		/**
		 * @brief Deallocate the specified amount of RAM block
		 * @param[in] ptr: The pointer to de-allocate.
		 * @return void
		 */
		static void Deallocate( void* ptr );

	private:
};


#endif	// DSI_BUFFER_H
