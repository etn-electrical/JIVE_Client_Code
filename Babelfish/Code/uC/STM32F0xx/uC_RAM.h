/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the uC_RAM class with functionality for testing system
 *  SRAM.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_RAM_H
#define UC_RAM_H

#include "Includes.h"
#include "Timers_Lib.h"
#include "CR_Tasker.h"
#include "uC_DMA.h"

namespace RAM_Diag
{

/**
 ****************************************************************************************
 * @brief uC_RAM.
 *
 * @details uC_RAM is a class that provides product-specific RAM diagnostics in
 * the form of a start-up test and and run-time test.  The start-up test calls
 * the run-time test until all of RAM has been tested.  After that, the run-time test is
 * called periodically and continually.  The run-time test tests six words of RAM
 * at a time using a March X algorithm.  Interrupts are disabled during this time.
 * When the run-time test reaches the end of RAM, it performs the March X test on
 * the buffer used for temporary storage.  If the start-up test or the run-time test
 * fails, an Exception::Stop_Here() occurs.
 *
 * @n @b Usage:
 * @n @b 1. Identify volatile regions of RAM by defining their start and end address
 * in a volatile_item_t struct in the object in which the region is used.  Typical
 * volatile regions are those used by a DMA controller.
 * @n @b 2. Whenever the region is being used and cannot be tested (volatile),
 * indicate the fact by calling Mark_As_Volatile() and passing the address of the
 * volatile_item_t.
 * @n @b 3. Whenever the region is not being used and can be tested (not volatile),
 * indicate the fact by calling Unmark_As_Volatile() and passing the address of the
 * volatile_item_t.
 * @n @b Note that all of the usage items above are done automatically if a uC_DMA
 * object is used to configure the DMA Controller.
 ****************************************************************************************
 */

typedef enum
{
	TEST_RUNNING,	///< The RAM test is in progress.
	TEST_OK,		///< All selected RAM was tested and passed.
	TEST_FAILURE	///< Part of the RAM failed the test.
} TestStatus;

class uC_RAM
{
	public:

		/**
		 * @details Number of RAM words tested at once.
		 */
		static const uint32_t RT_RAM_BLOCKSIZE = 6U;

		/**
		 * @brief alignment size for ram diagnostics test
		 * It is made 4U as Cortex M0 based F0xxx series microcontrollers
		 * does not allow unaligned access to memory. So only 4-byte aligned access
		 * is permitted and any unaligned access will cause a hard fault to occur.
		 */
		static const uint32_t PROCESSOR_WORD_SIZE = 4U;

		/**
		 * @brief constructor
		 */
		uC_RAM( void );

		/**
		 * @brief destructor
		 */
		~uC_RAM( void );

		/**
		 *  @brief .
		 *  @details .
		 */
		static void Startup_Test( void );

		/**
		 *  @brief Identify a block of RAM as volatile (cannot be tested).
		 *  @details Is called to indicate that a block of RAM is in use and
		 *  shouldn't be tested.  It adds the block to the list of volatile
		 *  blocks in order of start address.  No harm is done if a block that
		 *  has been marked as volatile is marked as volatile again.
		 *  @param[in] item: The address of the volatile_item_t struct that
		 *  describes the block of RAM.
		 */
		static void Mark_As_Volatile( volatile_item_t* new_item );

		/**
		 *  @brief Identify a block of RAM as not volatile (can be tested).
		 *  @details Is called to indicate that a block or RAM is not in use
		 *  and can be tested.  It removes the block from the list of volatile
		 *  blocks.  The next and prev pointers are nulled.
		 *  @param[in]  item: The address of the volatile_item_t struct that
		 *  describes the block of RAM.
		 */
		static void Unmark_As_Volatile( volatile_item_t* item );

	private:

		/**
		 * @brief Copy constructor shall be private and inaccessible.
		 * @param object - The object
		 */
		uC_RAM( const uC_RAM& object );

		/**
		 * @brief Copy assignment operator shall be private and inaccessible.
		 * @param object - The object
		 */
		uC_RAM & operator =( const uC_RAM& object );

		/**
		 * @brief Update
		 */
		void Update( CR_Tasker* cr_task ) const;

		/**
		 * @brief static member function for getting the byte aligned ram address
		 *
		 * @param uint32_t mem_size is used to pass the memory size in bytes
		 */
		static inline uint32_t Mem_Aligned_Size_Word( uint32_t mem_size )
		{
			return ( ( static_cast<UINT32>( mem_size ) + ( PROCESSOR_WORD_SIZE - 1U ) )
					 & ( ~static_cast<UINT32>( PROCESSOR_WORD_SIZE - 1U ) ) );
		}

		/**
		 * @brief static member function for update task
		 *
		 * @param tasker pointer to CR tasker, not used
		 * @param param CR tasker parameter, pass "this"
		 */
		static void Update_Static( CR_Tasker* tasker, CR_TASKER_PARAM param )
		{
			( ( uC_RAM* )param )->Update( tasker );
		}

		// Update task related
		CR_Tasker* m_cr_update_task;///< pointer to the diagnostic cleanup CR task

		/**
		 *  @brief Initialize the list of volatile RAM.
		 *  @details Is called to initialize the list of RAM is in use and
		 *  shouldn't be tested.  It adds the block to the list of volatile
		 *  blocks.
		 *  start and end addresses can be  and
		 *  @param[in] first_item: The address of the volatile_item_t struct that
		 *  describes the first volatile block in RAM.
		 *  @param[in] last_item: The address of the volatile_itme_t struct that
		 *  describes the last volatile block in RAM.
		 */
		static void Init_Volatile_List( volatile_item_t* first_item );

		/**
		 *  @brief Find out if any part of a block of RAM is volatile.
		 *  @details Is called by a uC_RAM object to see if the memory it
		 *  wants to test can be tested.  If the region cannot be tested,
		 *  an alternate region will be recommended.
		 *  @param[in] start: This is the address of the beginning of the
		 *  block of RAM to be tested.  It will be re-aligned forward as necessary.
		 *  @param[in] length: This is the number of bytes in the block
		 *  of RAM to be tested.  Is assumed to be evenly divisible by
		 *  PROCESSOR_REGISTER_BASE_SIZE.
		 *  @retval start is word-aligned and returned if the block is not volatile,
		 *  otherwise  the return value will be the word-aligned start address
		 *  of the next available region that fits the passed-in length requirement.
		 */
		static uint8_t* Check_Volatile( uint8_t* start, uint32_t length );

		/**
		 *  @brief Find out if the RAM test has reached the end of RAM.
		 *  @details Use this to trigger a special operation when the RAM test
		 *  has reached the end of RAM.  It will return true only once.
		 *  @retval true is returned if the RAM test has reached the end of RAM,
		 *  false otherwise.
		 */
		static bool_t Test_Reached_End_Of_RAM( void );

		/**
		 * @brief This function verifies that 6 words of RAM are functional,
		 * overlapping) using the March X algorithm.
		 * @retval: TestStatus
		 */
		static TestStatus Runtime_Test( void );

		/**
		 *  @details This function tests the RAM used by the buffer that's used
		 *  by the RAM test.
		 */
		static TestStatus Buffer_Self_Check( void );

		/**
		 * @details This is the pointer that's used by the RAM test to point to
		 * the RAM that's to be tested.
		 */
		static uint32_t* p_RunTimeRamChk;

		/**
		 * @details This is the FIRST region in the list of volatile regions of
		 * RAM that should not be tested by the RAM test.  It's marked as
		 * volatile at startup and NEVER unmarked.  It isn't really a volatile
		 * region in RAM, but its presence speeds up the handling of the truly
		 * volatile regions.  It's typically initialized as follows:
		 *
		 * @code
		   m_volatile_item_first.start = reinterpret_cast< uint8_t* >( 0x00000000U );	// start of
		 * MCU address space
		   m_volatile_item_first.end   = &__ICFEDIT_region_RAM_start__ - 1U;	// start of system
		 * SRAM - 1
		   m_volatile_item_first.next  = &m_volatile_item_last;	/// next (point it at the last item
		 * in the list)
		   m_volatile_item_first.prev  = &m_volatile_item_last;	/// prev (point it at the last item
		 * in the list)
		 * @endcode
		 */
		static volatile_item_t m_volatile_item_first;

		/**
		 * @details This represents a pointer that's used by the RAM test to
		 * point to the RAM being tested.  It's marked as volatile at startup
		 * and NEVER unmarked.
		 */
		static volatile_item_t m_volatile_item_pointer;

		/**
		 * @details This is a buffer that's used by the RAM test to temporarily
		 * store the data contained in the RAM that's to be tested.
		 */
		static uint32_t RunTimeRamBuf[RT_RAM_BLOCKSIZE];

		/**
		 * @details This represents a buffer that's used by the RAM test as a
		 * place to store the data in the area of RAM that it's testing.  It gets
		 * tested by the RAM test itself.  It's marked as volatile at startup
		 * and NEVER unmarked.
		 */
		static volatile_item_t m_volatile_item_buffer;

		/**
		 * @details This is the LAST region in the list of volatile regions of
		 * RAM that should not be tested by the RAM test.  It is marked as
		 * volatile at startup and NEVER unmarked.  It isn't really a volatile
		 * region in RAM, but its presence speeds up the handling of the truly
		 * volatile regions.  It's typically initialized as follows:
		 *
		 * @code
		   m_volatile_item_last.start = &__ICFEDIT_region_RAM_end__ + 1U;	// end of system SRAM +
		 * 1
		   m_volatile_item_last.end   = reinterpret_cast< uint8_t* >( 0xFFFFFFFFU );	// end of
		 * MCU address space
		   m_volatile_item_last.next  = &m_volatile_item_first;	/// next (point it at the first item
		 * in the list)
		   m_volatile_item_last.prev  = &m_volatile_item_first;	/// prev (point it at the first item
		 * in the list)
		 * @endcode
		 */
		static volatile_item_t m_volatile_item_last;

		/**
		 * @details This is a pointer to the beginning of the circular list of
		 * volatile regions in RAM.
		 */
		static volatile_item_t* m_volatile_head;

		/**
		 * @details This is a pointer to the next volatile region in RAM that the
		 * run-time RAM test will encounter.  It's used to skip over the
		 * irrelevant regions in the list of volatile regions.
		 */
		static volatile_item_t* m_volatile_marker;

		/**
		 * @details This pointer contains the address of the most recent byte of RAM
		 * that was tested.  It's used to help decide when to adjust the marker.
		 */
		static uint8_t* m_last_address_tested;

		/**
		 * @details This Boolean is true when the RAM test has reached the end
		 * of RAM and false otherwise.
		 */
		static bool_t m_End_Of_RAM_Reached;

#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
		static volatile uint8_t counter;
#endif
#endif
};

}

#endif	// #ifndef UC_RAM_H
