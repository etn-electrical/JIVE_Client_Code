/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "ADC_Field_Input.h"
#include "Timers_Lib.h"
#include "Device_Identity.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define     FILTER_CONST        1
#define     AC_INPUT_ON_THRESHOLD       ( ( uint16_t )0xD000 )
#define     AC_INPUT_OFF_THRESHOLD      ( ( uint16_t )0xC000 )
#define     DC_INPUT_ON_THRESHOLD       ( ( uint16_t )0x3300 )
#define     DC_INPUT_OFF_THRESHOLD      ( ( uint16_t )0x0D00 )

enum
{
	OFF,
	ON
};



/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: ADC_Field_Input()----------------------------------------------------
 * ----Description: Constructor-----------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: AtoD* atod_ctrl - Pointer to the AtoD control to the analog channel-------
 * ---------------for a specific AC input-------------------------------------------------
 * ------arg#2: BF_Lib::Timers::TIMERS_TIME_TD refresh_rate - Time in ms between scans--------------------
 * ----Outputs: --------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * -------return: ------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
ADC_Field_Input::ADC_Field_Input( BF_Misc::AtoD* atod_ctrl, DEVICE_IDENT_IO_ENUM input_type,
								  BF_Lib::Timers::TIMERS_TIME_TD refresh_rate )
{
	Timers* timer;

	m_atod_ctrl = atod_ctrl;

	m_input_status = OFF;

	m_raw_input = 0U;

	if ( input_type == DEVICE_IDENT_IO_TYPE_120VAC )
	{
		m_on_threshold = AC_INPUT_ON_THRESHOLD;
		m_off_threshold = AC_INPUT_OFF_THRESHOLD;
	}
	else
	{
		m_on_threshold = DC_INPUT_ON_THRESHOLD;
		m_off_threshold = DC_INPUT_OFF_THRESHOLD;
	}

	if ( refresh_rate > 0 )
	{
		timer = new Timers( Refresh_Static, ( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD )this,
							TIMERS_DEFAULT_CR_PRIORITY, "ADC Field In" );
		timer->Start( refresh_rate, true );
	}

}

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: --------------------------------------------------------------------
 * ----Description: ----------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: --------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Outputs: --------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * -------return: ------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint16_t ADC_Field_Input::Get_Voltage( void )
{
	uint16_t calc_voltage = 0U;

	calc_voltage = m_atod_ctrl->Get_Val();

	return ( calc_voltage );
}

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: --------------------------------------------------------------------
 * ----Description: ----------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: --------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Outputs: --------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * -------return: ------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void ADC_Field_Input::Refresh( void )
{

	uint16_t filtered_input;

	filtered_input = Get_Voltage();
	// temp = Get_Voltage();

	// m_raw_input += temp - ( m_raw_input >> FILTER_CONST );
	// filtered_input = ( m_raw_input >> FILTER_CONST );

	if ( m_input_status == OFF )
	{
		if ( filtered_input >= m_on_threshold )
		{
			m_input_status = ON;
		}
		else
		{
			m_input_status = OFF;
		}
	}
	else
	{
		if ( filtered_input >= m_off_threshold )
		{
			m_input_status = ON;
		}
		else
		{
			m_input_status = OFF;
		}
	}
}
