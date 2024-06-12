/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Single_LED.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Single_LED::Single_LED( DCI_Owner* target_led_owner, BF_Misc::LED* led_ctrl, BF_Misc::LED* ble_find_target_led ) :
	Ble_Display(),
	m_led_control( led_ctrl ),
	m_ble_find_target_led( ble_find_target_led ),
	m_target_led_owner( target_led_owner )
{
	m_target_led_owner->Attach_Callback(
		&Find_Target_Device_Callback_Static,
		reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Single_LED::~Single_LED( void )
{
	m_led_control = reinterpret_cast<BF_Misc::LED*>( nullptr );
	m_ble_find_target_led = reinterpret_cast<BF_Misc::LED*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Attention( void )
{
	m_led_control->Flash( LED_ON_TIME_ATTENTION, LED_OFF_TIME_ATTENTION, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Device_Faulted( void )
{
	m_led_control->Flash( LED_ON_TIME_FAULT, LED_OFF_TIME_FAULT, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Device_Connected( void )
{
	m_led_control->Flash( LED_ON_TIME_CONNECTED, LED_OFF_TIME_CONNECTED, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Pairing_Mode( void )
{
	m_led_control->Flash( LED_ON_TIME_PAIRING, LED_ON_TIME_PAIRING, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Data_Operations( void )
{
	m_led_control->Flash( LED_ON_TIME_POLLING, LED_OFF_TIME_POLLING, true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Single_LED::Device_Disconnected( void )
{
	m_led_control->Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Single_LED::Find_Target_Device_Callback_Static( DCI_CBACK_PARAM_TD handle,
															  DCI_ACCESS_ST_TD* access_struct )
{
	Single_LED* object_ptr = static_cast<Single_LED*>( handle );

	return ( object_ptr->Find_Target_Device_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Single_LED::Find_Target_Device_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	static bool toggle = false;

	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		if ( toggle == false )
		{
			m_ble_find_target_led->Flash( LED_ON_TIME_FIND_TARGET, LED_OFF_TIME_FIND_TARGET, true );
			return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
			toggle = true;
		}
		else
		{
			m_ble_find_target_led->Off();
			toggle = false;
		}
	}
	else
	{
		// misra
	}
	return ( return_status );
}
