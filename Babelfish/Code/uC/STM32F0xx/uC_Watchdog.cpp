/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Watchdog_HW.h"
#include "uC_Watchdog.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

uint8_t uC_Watchdog::m_number_bones_thrown = 0U;
uint_fast16_t uC_Watchdog::m_bones_thrown_mask = 0U;
uint_fast16_t uC_Watchdog::m_dog_bones_returned_mask = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Watchdog::uC_Watchdog( void ) :
	m_bone( 0U )
{
	m_bone = static_cast<uint8_t>( Get_Bone() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Init( uint16_t timeout_in_ms )
{
	uC_Watchdog_HW::Set_Timeout( timeout_in_ms );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
WATCHDOG_BONE_ID_TYPE uC_Watchdog::Get_Bone( void )
{
	uint8_t temp_bone_id = 0U;

	BF_ASSERT( m_number_bones_thrown < MAX_BONES );

	temp_bone_id = m_number_bones_thrown;
	m_bones_thrown_mask |= static_cast<uint_fast16_t>( 1U ) << temp_bone_id;
	m_number_bones_thrown++;

	return ( static_cast<WATCHDOG_BONE_ID_TYPE>( temp_bone_id ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Handler( void* param0 )
{
	if ( ( m_bones_thrown_mask ^ m_dog_bones_returned_mask ) == 0U )// if all bones returned
	{
		// Kick Watchdog Timer
		uC_Watchdog_HW::Kick_Dog();

		// reset and wait for bones to be thrown again
		m_dog_bones_returned_mask = 0U;
	}
	if ( param0 == param0 )
	{}						// let's use the parameter
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Enable_Bone( void ) const
{
	m_bones_thrown_mask |= static_cast<uint_fast16_t>( 1U ) << m_bone;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Disable_Bone( void ) const
{
	m_bones_thrown_mask &= ( ~( static_cast<uint_fast16_t>( 1U ) << m_bone ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Watchdog::Is_Bone_Enabled( void ) const
{
	return ( Test_Bit( m_bones_thrown_mask, m_bone ) );	// (bones_thrown_mask)&&(bone_ID));
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Soft_Reset( void )
{
#ifdef USE_VECTOR_TO_RESET
	CALLBACK_FUNC* vector_to_start;
#endif

	Disable_Interrupts();

	// want to make sure that no reads/writes are still pending
	// EEPROM_Wait_For_EEPROM_Done();

#ifdef USE_VECTOR_TO_RESET

	// a dirty little trick, point the PC to 0 to restart the program
	vector_to_start = 0U;
	( *vector_to_start )();

#else

	while ( true )
	{}

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Force_Feed_Dog( void )
{
	uC_Watchdog_HW::Kick_Dog();
}
