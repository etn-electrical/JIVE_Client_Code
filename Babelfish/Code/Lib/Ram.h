/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the Ram class with functionalities mainly for Memory allocation
 *	and de-allocation.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_H
#define RAM_H

#include "Ram_Handler.h"

/**
 * Debug Define:
 * #define RAM_DIAGNOSTICS
 *
 * This will turn on RAM Diagnostics and collection of data i.e. distribution data and
 * data size.  This should be defined in your device Config header file if you wish it to be
 * included.
 */

/**
 * Debug Define:
 * #define RAM_TERM_PRINT
 *
 * This will turn on the ability to print-out data allocation at run time to the terminal.
 * This is assuming you have defined Term_Print.
 */

/** Setter for the flag to enable or disable RAM_Term_Print output.
 * @param enable Set to True to enable or False to disable RAM_Term_Print output.
 */
void Enable_Ram_Term_Print( bool enable );

/**
 * This will turn on RAM Diagnostics and collection of data i.e. distribution data and
 * data size.  This should be defined in your device Config header file if you wish it to be
 * included.
 */
// #define RAM_DIAGNOSTICS


/**
 * @brief The global overload of the new operator.
 *
 * @details In order to control the allocation method and placement
 * of RAM data, the C++ memory allocation methods must be overridden.  The following provide
 * the prototypes for these overrides.  It forces every new op to use the
 * Ram class to allocate memory.  This new operator overload is special in that
 * it shall allow the designation of a target memory block.
 * @param[in] size: The total data size in bytes to be allocated with memory.
 * @param[in] ram_block_num: The particular Ram block to use.
 * @return void*: Pointer to the Allocated block.
 */
void* operator new( size_t size, uint8_t ram_block_num );

/**
 *  @brief The global overload of the new operator.
 *
 *  @param[in] size: The total data size in bytes to be allocated with memory.
 *  @return void*: Pointer to the Allocated block.
 */
void* operator new( size_t size );

/**
 *  @brief Global overload of delete operation. It forces everything to use the
 *  this Ram class to deallocate memory.
 *
 *  @param[in] ptr: The ptr to the memory to be de-allocated.
 */
void operator delete( void* ptr );


/**
 ****************************************************************************************
 * @brief The Ram class shall control the allocation of memory.
 *
 * @details For purposes of simplicity this class is purely static.
 * It provides support for the global new and delete override.  The Ram class has the ability
 * to redirect memory allocation during run time to support situations where RAM is scattered
 * across different address spaces and different functional needs or as one block is filled
 * all future memory can be allocated in a different block.  This avoids issues with scatter
 * linker files and/or writing memory allocation functions in classes to have knowledge of
 * where the memory is being allocated from.
 *
 * This class requires the definition of how many blocks will be assigned.  The definition of the
 * following is typically done in a platform definition file (typically Device_Config.h).
 * @code
 *      static const uint8_t MAIN_RAM_BLOCK = 0U;	///<  Block zero is the default block on init.
 *      static const uint8_t DCI_RAM_BLOCK = 1U;	///< Commonly DCI but could be your own.
 *		static const uint8_t MAX_NUMBER_OF_RAM_BLOCKS = 2U;	///< Must identify the total number of
 * memory handlers you plan to assign.
 * @endcode
 * @n @b Usage:
 * @n @b 1. Memory allocation and de-allocation mainly.
 * @n @b 2. It provides public methods to: assign block, allocate and de-allocate memory,
 * get used memory value and set values of default new operator allocation and default
 * allocation blocks.
 ****************************************************************************************
 */
class Ram
{
	public:
		/**
		 *  @brief Initializes the array of memory controls (Ram_Handlers).
		 *  @details The RAM class will maintain all the possible interfaces to memory allocation.
		 *  This requires an array of the Ram_Handlers which actually provide memory back.  The
		 *  init function initializes the Ram Handler array and the default allocation blocks.
		 */
		static void Init( void );

		/**
		 *  @brief Assigns a Ram_Handler to the passed in block number.
		 *  @details The Ram class must have Ram_Handlers to actually allocate memory for
		 *  it since the RAM class is simply a switch station of Ram_Handlers.
		 *  @param[in] ram_handle: The Ram_Handler control to assigne to the ram block num.
		 *  @n @b Usage:  This Ram_Handler shall be constructed and passed in to the assign func.
		 *  @param[in] ram_block_num: The number which was defined in the device defines.
		 *  @n @b Usage: This shall assign the Ram_Handler passed in to the array of possible
		 *  blocks.  Trying to allocate memory from a block which has not been assigned ends in
		 *  an exception.
		 */
		static void Assign_Block( BF_Lib::Ram_Handler* ram_handle, uint8_t ram_block_num );

		/**
		 *  @brief Shall allocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the allocate fails an internal exception will occur.
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @param[in] fill: Tell the allocator to initialize the memory or not.
		 *  @n @b Usage: To put the newly allocated memory in a known state assign fill to
		 *  true.
		 *  @param[in] fill_val: The uint8 value which should be used to initialize/fill the
		 *  newly allocated memory.
		 *  @n @b Usage: This value is only uint8 to provide more flexibility.  If a more
		 *  complicated fill value is desired then it shall be done manually.
		 *  @param[in] ram_block_num: The desired block number to allocate from.  This will
		 *  actually call the appropriate Ram_Handler to allocate the memory.
		 *  @return Pointer to the newly allocated memory.
		 */
		static void* Allocate( size_t size, bool fill = true, uint8_t fill_val = 0U,
							   uint8_t ram_block_num = m_default_alloc_block );

		/**
		 *  @brief Shall allocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the allocate fails a null pointer is returned.
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @param[in] fill: Tell the allocator to initialize the memory or not.
		 *  @n @b Usage: To put the newly allocated memory in a known state assign fill to
		 *  true.
		 *  @param[in] fill_val: The uint8 value which should be used to initialize/fill the
		 *  newly allocated memory.
		 *  @n @b Usage: This value is only uint8 to provide more flexibility.  If a more
		 *  complicated fill value is desired then it shall be done manually.
		 *  @param[in] ram_block_num: The desired block number to allocate from.  This will
		 *  actually call the appropriate Ram_Handler to allocate the memory.
		 *  @return Pointer to the newly allocated memory.
		 */
		static void* Allocate_Basic( size_t size, bool fill = true, uint8_t fill_val = 0U,
									 uint8_t ram_block_num = m_default_alloc_block );

		/**
		 *  @brief Shall reallocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the allocate fails an internal exception will occur.
		 *  @param[in] ptr: The pointer to the newly reallocated memory
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @param[in] ram_block_num: The desired block number to allocate from.  This will
		 *  actually call the appropriate Ram_Handler to allocate the memory.
		 *  @return Pointer to the newly reallocated memory.
		 */
		static void* Reallocate( void* ptr, size_t size,
								 uint8_t ram_block_num = m_default_alloc_block );

		/**
		 *  @brief Shall reallocate the specified amount of RAM and return the pointer to the new
		 *  memory.
		 *  @details When the allocate fails a null pointer is returned.
		 *  @param[in] ptr: The pointer to the newly reallocated memory
		 *  @param[in] size: The size in bytes of memory to be allocated.
		 *  @param[in] ram_block_num: The desired block number to allocate from.  This will
		 *  actually call the appropriate Ram_Handler to allocate the memory.
		 *  @return Pointer to the newly reallocated memory.
		 */
		static void* Reallocate_Basic( void* ptr, size_t size,
									   uint8_t ram_block_num = m_default_alloc_block );

		/**
		 *  @brief Shall de-allocate the specified pointer from the passed in block.
		 *  @details The de-allocate will sometimes not be supported by Ram_Handler classes
		 *  in the embedded space, ie when only doing static allocation.
		 *  If the de-alloc is not supported the Ram_Handler has the
		 *  option to do an exception stop here.
		 *  @param[in] ptr: The pointer to de-allocate.
		 */
		static void De_Allocate( void* ptr )
		{
			De_Allocate( ptr, m_default_dealloc_block );
		}

		/**
		 *  @brief Shall de-allocate the specified pointer from the passed in block.
		 *  @details The de-allocate will sometimes not be supported by Ram_Handler classes
		 *  in the embedded space, ie when only doing static allocation.
		 *  If the de-alloc is not supported the Ram_Handler does nothing .
		 *  @param[in] ptr: The pointer to de-allocate.
		 */
		static void De_Allocate_Basic( void* ptr )
		{
			De_Allocate_Basic( ptr, m_default_dealloc_block );
		}

		/**
		 *  @brief Shall de-allocate the specified pointer from the passed in block.
		 *  @details The de-allocate will typically not be supported by Ram_Handler classes
		 *  in the embedded space.  If the de-alloc is not supported the Ram_Handler has the
		 *  option to do an exception stop here.
		 *  @param[in] ptr: The pointer to de-allocate.
		 *  @param[in] ram_block_num: The Ram block number to use.
		 */
		static void De_Allocate( void* ptr, uint8_t ram_block_num );

		/**
		 *  @brief Shall de-allocate the specified pointer from the passed in block.
		 *  @details The de-allocate will typically not be supported by Ram_Handler classes
		 *  in the embedded space.  If the de-alloc is not supported the Ram_Handler does
		 *  nothing.
		 *  @param[in] ptr: The pointer to de-allocate.
		 *  @param[in] ram_block_num: The Ram block number to use.
		 */
		static void De_Allocate_Basic( void* ptr, uint8_t ram_block_num );

		/**
		 *  @brief Provide a percent usage of a specific block of Ram memory.
		 *  @details Every Ram_Handler shall provide a percentage used value.
		 *  @param[in] ram_block_num: The block number which we want to measure.
		 *  @return The integer percentage of memory used.  Anything near 99% is
		 *  considered full.
		 */
		static uint8_t Used( uint8_t ram_block_num = DEFAULT_MEMORY_ALLOC_BLOCK );

		/**
		 * @brief Provides the amount of bytes left in this RAM handler.
		 * @details This can be used to determine ahead of time whether your allocation
		 * request will be allowed.  You can compare your size_t with the desired size.
		 * @return number of bytes remaining in this memory handler.
		 */
		static size_t Available( uint8_t ram_block_num = DEFAULT_MEMORY_ALLOC_BLOCK );

		/**
		 *  @brief Modifies the default memory block for the overridden new operator.
		 *  @details The "new" construct cannot be controlled in every place and because
		 *  of this it is beneficial to modify where the new operator actually
		 *  pulls the memory from.  An example usage would be if you have two different
		 *  address spaces for RAM.  When one fills up it would be possible to switch over to
		 *  a new memory block.
		 *  @param[in] new_operator_block: This is the RAM block which shall become the default
		 *  allocation block for the overridden new operator.
		 */
		static void Set_Default_New_Op_Alloc_Block( uint8_t new_operator_block );

		/**
		 *  @brief Behaves similar to/mirrors Set_Default_New_Op_Alloc_Block()
		 */
		static void Set_Default_Delete_Op_Alloc_Block( uint8_t delete_operator_block );

		/**
		 *  @brief Behaves similar to/mirrors Set_Default_New_Op_Alloc_Block()
		 */
		static void Set_Default_Alloc_Block( uint8_t alloc_block );

		/**
		 *  @brief Behaves similar to/mirrors Set_Default_New_Op_Alloc_Block()
		 */
		static void Set_Default_Dealloc_Block( uint8_t dealloc_block );

		/**
		 *  @details If requirements dictate that dynamic memory allocation
		 *  not occur during run-time, call this function just before run-time
		 *  mode is entered.
		 *  @param[in] param: True if dynamic memory allocation is to be prohibited;
		 *  false if it is to be permitted.
		 */
		static void Prohibit_Dynamic_Memory_Allocation( bool param = true );

		/**
		 * Helper function that determines whether a pointer points to a valid RAM area.
		 * Useful when just checking for a null pointer isn't enough.
		 * Reverse of the InvalidRamPtr() function in the lwip code.
		 * @param mem_ptr The pointer to be checked.
		 * @return True if the pointer falls within the __ICFEDIT_region_RAM_start__ to _end__ region.
		 */
		static bool Valid_Ram_Ptr( void* mem_ptr );

	private:

		/**
		 * @brief Copy constructor. It is private and inaccessible.
		 * @param[in] rhs: The object.
		 */
		Ram( const Ram& rhs );

		/**
		 * @brief Copy assignment operator. It is private and inaccessible.
		 * @param[in] object: The object.
		 */
		Ram & operator =( const Ram& object );

		Ram( void );
		~Ram( void );

		static const uint8_t DEFAULT_MEMORY_ALLOC_BLOCK = 0U;
		/// Ram pointers for the Ram blocks.
		static BF_Lib::Ram_Handler* m_ram_ptrs[MAX_NUMBER_OF_RAM_BLOCKS];
		/// To set a default allocation block.
		static uint8_t m_default_alloc_block;
		static uint8_t m_default_dealloc_block;
		/// True if dynamic memory allocation is prohibited; false otherwise.
		static bool_t m_dynamic_mem_alloc_prohibited;

#ifdef RAM_DIAGNOSTICS
		static const uint16_t MAX_DATA_DISTRIB_COUNT = 0xFFFFU;
		static const uint32_t MAX_DATA_SIZE_SUM = 0xFFFFFFFFU;

		/// The number of distribution items with Ram data.
		static const uint8_t RAM_DATA_DISTRIBUTION_ITEMS = 17U;

		/// Array for the block-wise distribution of data distribution items.
		static uint16_t m_data_distribution[MAX_NUMBER_OF_RAM_BLOCKS][RAM_DATA_DISTRIBUTION_ITEMS];
		/// To have the allocated data size for size greater than allocated data distribution items.
		static uint32_t m_data_size_large[MAX_NUMBER_OF_RAM_BLOCKS];
		/// Array to have the total memory data size being allocated.
		static uint32_t m_data_size_total[MAX_NUMBER_OF_RAM_BLOCKS];
		/// Array to have the percentage of the memory used. TBD
		static uint8_t m_percent_used[MAX_NUMBER_OF_RAM_BLOCKS];
#endif
};

#endif
