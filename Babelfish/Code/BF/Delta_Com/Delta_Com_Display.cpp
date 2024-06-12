/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Delta_Com_Display.h"
#include "Output_Null.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Display::Delta_Com_Display( BF_Misc::Bi_Color_LED* bi_led ) :
	m_current_status( INITIAL ),
	m_bi_led( bi_led )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Display::Delta_Com_Display( void ) :
	m_current_status( INITIAL ),
	m_bi_led( new BF_Misc::Bi_Color_LED( new BF_Misc::LED( new BF_Lib::Output_Null() ),
										 new BF_Misc::LED( new BF_Lib::Output_Null() ) ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Display::~Delta_Com_Display( void )
{
	delete m_bi_led;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Display::Idle( void )
{
	if ( m_current_status != IDLE )
	{
		m_current_status = IDLE;
		m_bi_led->Flash( GREEN_LED, OFFLINE_ON_FLASH_TIME, OFFLINE_OFF_FLASH_TIME, true );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Display::Discovery()
{
	if ( m_current_status != DISCOVERY )
	{
		m_current_status = DISCOVERY;
		m_bi_led->Flash( GREEN_LED, DISCOVERY_FLASH_TIME, false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Display::Active()
{
	if ( m_current_status != ACTIVE )
	{
		m_current_status = ACTIVE;
		m_bi_led->Flash( GREEN_LED, ONLINE_ON_FLASH_TIME, ONLINE_OFF_FLASH_TIME, true );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Display::Faulted()
{
	if ( m_current_status != FAULTED )
	{
		m_current_status = FAULTED;
		m_bi_led->Flash( RED_LED, MAJOR_FAULT_FLASH_TIME, true );
	}
}
