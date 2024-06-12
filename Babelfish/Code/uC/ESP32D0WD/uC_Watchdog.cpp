/**
 *****************************************************************************************
 * @file					uC_Watchdog.cpp
 * @details					See header file for module overview.
 * @copyright				2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Watchdog.h"
// #include "../esp_system_internal.h"
#include "uC_Watchdog_HW.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t MAX_BONES = 16U;				///< Maximum number of bones.
static const uint32_t DISABLE_VALUE = 0U;			///< Disable value.

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint8_t uC_Watchdog::m_number_bones_thrown = 0U;			///< Number of bones thrown.
uint_fast16_t uC_Watchdog::m_bones_thrown_mask = 0U;		///< Bones thrown mask.
uint_fast16_t uC_Watchdog::m_dog_bones_returned_mask = 0U;	///< Dog bones returned mask.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Watchdog::uC_Watchdog( void ) :
	m_bone( DISABLE_VALUE )
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
	uint8_t temp_bone_id = DISABLE_VALUE;

	if ( m_number_bones_thrown < MAX_BONES )
	{
		temp_bone_id = m_number_bones_thrown;
		m_bones_thrown_mask |= static_cast<uint_fast16_t>( ENABLE_VALUE ) << temp_bone_id;
		m_number_bones_thrown++;
	}
	else
	{
		for (;;)
		{}
	}

	return ( static_cast<WATCHDOG_BONE_ID_TYPE>( temp_bone_id ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Handler( void* param0 )
{
#ifndef ESP32_SETUP	// Watchdog not yet supported on ESP32
	if ( ( m_bones_thrown_mask ^ m_dog_bones_returned_mask ) == DISABLE_VALUE )
	{
		// Kick Watchdog Timer
		uC_Watchdog_HW::Kick_Dog();
		// reset and wait for bones to be thrown again
		m_dog_bones_returned_mask = DISABLE_VALUE;
	}
	if ( param0 == ( nullptr ) )
	{}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Enable_Bone( void ) const
{
	m_bones_thrown_mask |= static_cast<uint_fast16_t>( ENABLE_VALUE ) << m_bone;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Disable_Bone( void ) const
{
	m_bones_thrown_mask &= ( ~( static_cast<uint_fast16_t>( ENABLE_VALUE ) << m_bone ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Watchdog::Is_Bone_Enabled( void ) const
{
	bool status;

	if ( ( static_cast<uint32_t>( m_bones_thrown_mask ) & ( static_cast<uint32_t>( ENABLE_VALUE )
															<< ( m_bone ) ) ) )
	{
		status = true;
	}
	else
	{
		status = false;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Watchdog::Soft_Reset( void )
{
	esp_restart();
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
