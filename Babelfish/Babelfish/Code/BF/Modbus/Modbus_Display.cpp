/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_Display.h"

namespace BF_Mbus
{


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Display::Modbus_Display() :
	m_led_control( nullptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Display::Modbus_Display( BF_Misc::LED* led_ctrl ) :
	m_led_control( led_ctrl )
{
	m_led_control = led_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Display::~Modbus_Display( void )
{
	m_led_control = reinterpret_cast<BF_Misc::LED*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Display::Bus_Active( void )
{
	m_led_control->Flash( LED_ON_TIME, BF_Misc::LED::SINGLE_FLASH_OFF_TIME, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Display::Bus_Idle( void )
{
	m_led_control->Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Display::Device_Faulted( void ) const
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Display::Device_Ok( void ) const
{}

}	/* end namespace BF_Mbus for this module */
