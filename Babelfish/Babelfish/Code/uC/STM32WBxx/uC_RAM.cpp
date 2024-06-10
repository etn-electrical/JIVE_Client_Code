/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_DMA.h"
// #define	DEBUG_RAM_TEST	// Define this to provide debug information to the host via printf.
#include "uC_RAM.h"
#include "Babelfish_Assert.h"


namespace RAM_Diag
{

uint32_t* uC_RAM::p_RunTimeRamChk = reinterpret_cast<uint32_t*>( nullptr );
volatile_item_t uC_RAM::m_volatile_item_first;
volatile_item_t uC_RAM::m_volatile_item_post_ccm;
volatile_item_t uC_RAM::m_volatile_item_pointer;
uint32_t uC_RAM::RunTimeRamBuf[RT_RAM_BLOCKSIZE];
volatile_item_t uC_RAM::m_volatile_item_buffer;
volatile_item_t uC_RAM::m_volatile_item_last;
volatile_item_t* uC_RAM::m_volatile_head = reinterpret_cast<volatile_item_t*>( nullptr );
volatile_item_t* uC_RAM::m_volatile_marker = reinterpret_cast<volatile_item_t*>( nullptr );
uint8_t* uC_RAM::m_last_address_tested = reinterpret_cast<uint8_t*>( nullptr );
bool_t uC_RAM::m_End_Of_RAM_Reached = false;

#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
volatile uint8_t uC_RAM::counter = 0U;
#endif
#endif

/**
 *@remark Coding Standard Deviation:
 *@n MISRA-C++[2008] Required Rule 17-0-2, Re-use of C++ identifier pattern: __vector_table
 *@n Justification: The pattern of __ICFEDIT_region_RAM_start__, __ICFEDIT_region_RAM_end__, etc.
 *@n are determined by the linker and cannot be changed.
 */
/*lint -e{1960} # MISRA Deviation */
extern "C" {
extern uint8_t __ICFEDIT_region_RAM_start__;
extern uint8_t __ICFEDIT_region_RAM_end__;
}

/* These are the direct and inverted data (patterns) used during the RAM
   test, performed using March X Algorithm */
static const uint32_t BCKGRND = 0x00000000U;
static const uint32_t INV_BCKGRND = 0xFFFFFFFFU;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RAM::uC_RAM( void )
	: m_cr_update_task( reinterpret_cast<CR_Tasker*>( NULL ) )
{
	// Start the service task
	m_cr_update_task = new CR_Tasker( &Update_Static,
									  reinterpret_cast<CR_TASKER_PARAM>( this ), CR_TASKER_IDLE_PRIORITY,
									  "uC_RAM update" );

	// This volatile region represents the address space starting at the very
	// beginning and going up to but not including the beginning of Core Connected
	// Memory.  It should NEVER be unmarked as volatile.
	m_volatile_item_first.start = reinterpret_cast<uint8_t*>( 0x00000000U );	// start of MCU address space
	m_volatile_item_first.end = &__ICFEDIT_region_RAM_start__;
	m_volatile_item_first.end -= 1U;	// Separate statements to avoid a PC-lint warning (428).
	m_volatile_item_first.next = &m_volatile_item_last;
	m_volatile_item_first.prev = &m_volatile_item_last;
	// This volatile region represents the pointer used by the diagnostic RAM test.
	// It should NEVER be unmarked as volatile.
	m_volatile_item_pointer.start = reinterpret_cast<uint8_t*>( &p_RunTimeRamChk );
	m_volatile_item_pointer.end = ( m_volatile_item_pointer.start + sizeof( p_RunTimeRamChk ) ) - 1U;
	m_volatile_item_pointer.next = reinterpret_cast<volatile_item_t*>( nullptr );
	m_volatile_item_pointer.prev = reinterpret_cast<volatile_item_t*>( nullptr );

	// This volatile region represents the buffer used by the diagnostic RAM test.
	// It should NEVER be unmarked as volatile.  The diagnostic RAM test will test
	// this buffer each time the rest of RAM has been completely tested.
	m_volatile_item_buffer.start = reinterpret_cast<uint8_t*>( &RunTimeRamBuf[0] );
	m_volatile_item_buffer.end = ( m_volatile_item_buffer.start + sizeof( RunTimeRamBuf ) ) - 1U;
	m_volatile_item_buffer.next = reinterpret_cast<volatile_item_t*>( nullptr );
	m_volatile_item_buffer.prev = reinterpret_cast<volatile_item_t*>( nullptr );

	// This volatile region represents the address space just beyond
	// __ICFEDIT_region_RAM_end__ and extending all the way to the end of the
	// address space.  It should NEVER be unmarked as volatile.
	m_volatile_item_last.start = &__ICFEDIT_region_RAM_end__ + 1U;	// end of system SRAM + 1
	m_volatile_item_last.end = reinterpret_cast<uint8_t*>( 0xFFFFFFFFU );		// end of MCU address space
	m_volatile_item_last.next = &m_volatile_item_first;
	m_volatile_item_last.prev = &m_volatile_item_first;

	Init_Volatile_List( &m_volatile_item_first );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RAM::~uC_RAM( void )
{
	delete m_cr_update_task;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RAM::Update( CR_Tasker* cr_task ) const
{
	TestStatus ram_test_result;
	const OS_TICK_TD UPDATE_PERIOD_IN_MS = 1000U;
	GINT_TDEF stack_int_bit;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		Push_Int( stack_int_bit );
		ram_test_result = Runtime_Test();
		Pop_Int( stack_int_bit );

		switch ( ram_test_result )
		{
			case TEST_RUNNING:	// Fall-through intentional.
			case TEST_OK:
				break;

			case TEST_FAILURE:	// Fall-through intentional.
			default:
				// The RAM test failed.  Go to HEL.
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
				printf( "Stop_Here\n" );
#endif
#endif
				BF_ASSERT(false);
				break;
		}

		if ( Test_Reached_End_Of_RAM() == true )
		{
			Push_Int( stack_int_bit );
			ram_test_result = Buffer_Self_Check();
			Pop_Int( stack_int_bit );

			switch ( ram_test_result )
			{
				case TEST_RUNNING:	// Fall-through intentional.
				case TEST_OK:
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
					printf( "%03d\n", counter );
					counter++;	// Wrap is OK.
#endif
#endif
					break;

				case TEST_FAILURE:	// Fall-through intentional.
				default:
					// The RAM test failed.  Go to HEL.
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
					printf( "B: Stop_Here\n" );
#endif
#endif
					BF_ASSERT(false);
					break;
			}
		}
		CR_Tasker_Delay( cr_task, UPDATE_PERIOD_IN_MS );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RAM::Init_Volatile_List( volatile_item_t* initial_item )
{
	m_volatile_head = initial_item;
	m_volatile_marker = initial_item;
	Mark_As_Volatile( &m_volatile_item_pointer );
	Mark_As_Volatile( &m_volatile_item_buffer );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RAM::Mark_As_Volatile( volatile_item_t* new_item )
{
	if( m_volatile_head != nullptr )
	{
		Push_TGINT();

		// Start at the head of the circular list and insert the item in such a
		// way as to maintain the order of the list.
		volatile_item_t* volatile_current = m_volatile_head;

		if ( new_item->start < volatile_current->start )
		{
			// The new item is less than the current item, so insert it before the
			// current item and update the head.
			new_item->prev = volatile_current->prev;
			new_item->next = volatile_current;
			volatile_current->prev->next = new_item;
			volatile_current->prev = new_item;
			m_volatile_head = new_item;

			if ( ( m_last_address_tested <= new_item->end ) &&
				 ( new_item->start < m_volatile_marker->start ) )
			{
				// The address of the new item is such that it will be tested before
				// the item pointed to by the marker, so point the marker at the new
				// item.
				m_volatile_marker = new_item;
			}
		}
		else
		{
			bool_t item_inserted = false;

			while ( item_inserted == false )
			{
				if ( new_item->start == volatile_current->start )
				{
					// The new item is already in the list (has been marked), so
					// terminate the loop.
					item_inserted = true;
				}
				else if ( ( new_item->start > volatile_current->start ) &&
						  ( new_item->start < volatile_current->next->start ) )
				{
					// The new item is greater than the current item and less than
					// the next item, so insert it after the current item and terminate
					// the loop.
					new_item->prev = volatile_current;
					new_item->next = volatile_current->next;
					volatile_current->next->prev = new_item;
					volatile_current->next = new_item;

					if ( ( m_last_address_tested <= new_item->end ) &&
						 ( new_item->start < m_volatile_marker->start ) )
					{
						// The address of the new item is such that it will be tested before
						// the item pointed to by the marker, so point the marker at the new
						// item.
						m_volatile_marker = new_item;
					}
					item_inserted = true;
				}
				else
				{
					// The new item is greater than the current item AND the next item,
					// so advance to the next item and try again.
					volatile_current = volatile_current->next;
				}
			}
		}

		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RAM::Unmark_As_Volatile( volatile_item_t* item )
{
	if( m_volatile_head != nullptr )
	{
		Push_TGINT();

		if ( ( item->next != reinterpret_cast<volatile_item_t*>( nullptr ) ) &&
			 ( item->prev != reinterpret_cast<volatile_item_t*>( nullptr ) ) )
		{
			// Item has been marked as volatile, so it's safe to unmark it.
			if ( m_volatile_marker == item )
			{
				// The marker is pointing at the volatile item to be unmarked, so point
				// the marker at the next volatile region.
				m_volatile_marker = m_volatile_marker->next;
			}

			item->prev->next = item->next;
			item->next->prev = item->prev;

			item->next = reinterpret_cast<volatile_item_t*>( nullptr );
			item->prev = reinterpret_cast<volatile_item_t*>( nullptr );
		}
		// Else the item has already been unmarked, so don't unmark it again.

		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_RAM::Check_Volatile( uint8_t* start, uint32_t length )
{
	// length has been size-aligned by the caller.
	// Assume proper alignment and no overlap with a volatile region.
	uint8_t* return_value = start;
	bool_t testable_region_found = false;

	// Re-align the start address forward as necessary.
	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
	 *sub-integers
	 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects throughout.
	 */
	/*lint -e{1960} # MISRA Deviation */
	return_value = reinterpret_cast<uint8_t*>( Mem_Aligned_Size_Byte(
												   reinterpret_cast<uint32_t>( return_value ) ) );

	while ( testable_region_found == false )
	{
		if ( ( return_value >= m_volatile_marker->start ) &&
			 ( return_value <= m_volatile_marker->end ) )
		{
			// The start of the desired test region is in a volatile region.
			// See if the region just beyond the volatile region is testable.
			return_value = m_volatile_marker->end + 1U;
			m_volatile_marker = m_volatile_marker->next;
			// Re-align the start address forward as necessary.
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			return_value = reinterpret_cast<uint8_t*>( Mem_Aligned_Size_Byte(
														   reinterpret_cast<uint32_t>( return_value ) ) );
		}
		else if ( ( ( return_value + length ) >= m_volatile_marker->start ) &&
				  ( ( return_value + length ) <= m_volatile_marker->end ) )
		{
			// The end of the desired test region is in a volatile region.  See
			// if the region just beyond the volatile region is testable.
			return_value = m_volatile_marker->end + 1U;
			m_volatile_marker = m_volatile_marker->next;
			// Re-align the start address forward as necessary.
			return_value = reinterpret_cast<uint8_t*>( Mem_Aligned_Size_Byte(
														   reinterpret_cast<uint32_t>( return_value ) ) );
		}
		else if ( ( return_value < m_volatile_marker->start ) &&
				  ( ( return_value + length ) > m_volatile_marker->end ) )
		{
			// The desired test region straddles a volatile region.  See if the
			// region just beyond the volatile region is testable.
			return_value = m_volatile_marker->end + 1U;
			m_volatile_marker = m_volatile_marker->next;
			// Re-align the start address forward as necessary.
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			return_value = reinterpret_cast<uint8_t*>( Mem_Aligned_Size_Byte(
														   reinterpret_cast<uint32_t>( return_value ) ) );
		}
		else
		{
			// The region doesn't overlap with any volatile regions, so terminate
			// the loop.
			testable_region_found = true;
		}
	}
	m_last_address_tested = ( return_value + length ) - 1U;

	if ( return_value < start )
	{
		// The test has reached the end of RAM, so record the fact.
		m_End_Of_RAM_Reached = true;
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_RAM::Test_Reached_End_Of_RAM( void )
{
	bool_t return_value = m_End_Of_RAM_Reached;

	if ( m_End_Of_RAM_Reached == true )
	{
		// Reset when read.
		m_End_Of_RAM_Reached = false;
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_RAM::Startup_Test( void )
{
	TestStatus ram_test_result;

	while ( Test_Reached_End_Of_RAM() == false )
	{
		ram_test_result = Runtime_Test();

		switch ( ram_test_result )
		{
			case TEST_RUNNING:	// Fall-through intentional.
			case TEST_OK:
				break;

			case TEST_FAILURE:	// Fall-through intentional.
			default:
				// The RAM test failed.  Go to HEL.
				BF_ASSERT(false);
				break;
		}
	}

	ram_test_result = Buffer_Self_Check();

	switch ( ram_test_result )
	{
		case TEST_RUNNING:	// Fall-through intentional.
		case TEST_OK:
			break;

		case TEST_FAILURE:	// Fall-through intentional.
		default:
			// The RAM test failed.  Go to HEL.
			BF_ASSERT(false);
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TestStatus uC_RAM::Runtime_Test( void )
{
	TestStatus Result = TEST_RUNNING;

	static const uint32_t BLOCKSIZE = ( RT_RAM_BLOCKSIZE * sizeof( uint32_t ) );

	uint32_t i;				/* Index for RAM physical addressing */
	const int8_t* pBlock;	/* Index for address scrambling/descrambling */

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, cast from pointer to pointer
	 *@n Justification: Both pointers point to Flash memory.
	 */
	/*lint -e{927,826} # MISRA Deviation */
	p_RunTimeRamChk =
		reinterpret_cast<uint32_t*>( Check_Volatile( reinterpret_cast<uint8_t*>( p_RunTimeRamChk ), BLOCKSIZE ) );

	/* This array contains scrambling/descrambling pattern for RT_RAM_BLOCKSIZE words */
	static const int8_t RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE] = {0, 1, 2, 3, 4, 5};

	/* ------------------ Regular memory Self-check -----------------------
	  ---------------------------- STEP 1 ---------------------------------*/
	/* Save the content of the RT_RAM_BLOCKSIZE words to be tested and start March X.
	   Write background with addresses increasing */
	pBlock = &RT_RAM_SCRMBL[0];	/* Takes into account RAM scrambling */
	i = 0U;
	do
	{
		RunTimeRamBuf[i] = *( p_RunTimeRamChk + *pBlock );
		i++;
		*( p_RunTimeRamChk + *pBlock ) = BCKGRND;
		pBlock++;
	}while( pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE] );

	/* ---------------------------- STEP 2 ---------------------------------
	   Verify background and write inverted background addresses increasing*/
	pBlock = &RT_RAM_SCRMBL[0];
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "2: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		*( p_RunTimeRamChk + *pBlock ) = INV_BCKGRND;
		pBlock++;
	}while( pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE] );

	/* ---------------------------- STEP 3 ---------------------------------
	   Verify inverted background and write background addresses decreasing*/
	pBlock = &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE - 1U];
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != INV_BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "3: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		*( p_RunTimeRamChk + *pBlock ) = BCKGRND;
		pBlock--;
	}while( pBlock >= &RT_RAM_SCRMBL[0] );

	/* ---------------------------- STEP 4 ---------------------------------
	   Verify background with addresses increasing
	   and restore the content of the 6 tested words*/
	pBlock = &RT_RAM_SCRMBL[0];
	i = 0U;
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "4: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		*( p_RunTimeRamChk + *pBlock ) = RunTimeRamBuf[i];
		i++;
		pBlock++;
	}while( pBlock < &RT_RAM_SCRMBL[RT_RAM_BLOCKSIZE] );

	/* Prepare next Row Tranparent RAM test */
	p_RunTimeRamChk += RT_RAM_BLOCKSIZE;

	if ( Result != TEST_RUNNING )
	{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
		printf( "5: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
		Result = TEST_FAILURE;	/* Word line under test was not functional */
	}
	// else Do nothing: keep Result as TEST_RUNNING;
	/* --------------- End of Regular memory Self-check --------------------- */

	return ( Result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
TestStatus uC_RAM::Buffer_Self_Check( void )
{
	TestStatus Result = TEST_RUNNING;
	uint32_t* RunTimeRamChk = p_RunTimeRamChk;	// Save the pointer value.

	/* This array contains scrambling/descrambling pattern for RT_RAM_BLOCKSIZE words */
	const int8_t RT_RAMBUF_SCRMBL[RT_RAM_BLOCKSIZE] = {0, 1, 2, 3, 4, 5};

	const int8_t* pBlock;	/* Index for address scrambling/descrambling */

	/*------------- Apply March X test to the RAM Buffer itself --------------- */
	p_RunTimeRamChk = &RunTimeRamBuf[0];

	/* ---------------------------- STEP 1 ---------------------------------
	   Write background with addresses increasing*/
	pBlock = &RT_RAMBUF_SCRMBL[0];
	do
	{
		*( p_RunTimeRamChk + *pBlock ) = BCKGRND;
		pBlock++;
	}while( pBlock < &RT_RAMBUF_SCRMBL[RT_RAM_BLOCKSIZE] );

	/* ---------------------------- STEP 2 ---------------------------------
	   Verify background and write inverted background addresses increasing*/
	pBlock = &RT_RAMBUF_SCRMBL[0];
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "2B: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		*( p_RunTimeRamChk + *pBlock ) = INV_BCKGRND;
		pBlock++;
	}while( pBlock < &RT_RAMBUF_SCRMBL[RT_RAM_BLOCKSIZE] );

	/* ---------------------------- STEP 3 ---------------------------------
	   Verify inverted background and write background addresses decreasing*/
	pBlock = &RT_RAMBUF_SCRMBL[RT_RAM_BLOCKSIZE - 1U];
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != INV_BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "3B: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		*( p_RunTimeRamChk + *pBlock ) = BCKGRND;
		pBlock--;
	}while( pBlock >= &RT_RAMBUF_SCRMBL[0] );

	/* ---------------------------- STEP 4 ---------------------------------
	   Verify background with addresses increasing*/
	pBlock = &RT_RAMBUF_SCRMBL[0];
	do
	{
		if ( *( p_RunTimeRamChk + *pBlock ) != BCKGRND )
		{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
			printf( "4B: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
			Result = TEST_FAILURE;
		}
		pBlock++;
	}while( pBlock < &RT_RAMBUF_SCRMBL[RT_RAM_BLOCKSIZE] );

	p_RunTimeRamChk = RunTimeRamChk;	// Restore the pointer value.

	if ( Result == TEST_RUNNING )
	{
		Result = TEST_OK;	/* Means all selected variable memory was scanned */
	}
	else	/* Buffer is not functional */
	{
#ifdef DEBUG
#ifdef DEBUG_RAM_TEST
		printf( "5B: 0x%08X\n", p_RunTimeRamChk );
#endif
#endif
		Result = TEST_FAILURE;
	}
	/* ------------------ End of Buffer Self-check ------------------------ */

	return ( Result );
}

}
