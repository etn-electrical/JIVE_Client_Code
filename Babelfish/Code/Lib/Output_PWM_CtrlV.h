/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *		Code Inspection Date:
 *
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_PWM_CTRLV_H
   #define OUTPUT_PWM_CTRLV_H

#include "Output.h"
#include "uC_IO_Config.h"
#include "uC_PWM.h"
#include "Ctrl_Voltage.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MIN_PULLIN_VOLTAGE      17000			// in millivolts (17VDC)

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Output_PWM_CtrlV : public Output
{
	public:
		Output_PWM_CtrlV( uC_PWM* pwm_ctrl, UINT16 pullin_v, UINT16 hold_v, Ctrl_Voltage* ctrl_voltage_ctrl,
						  BOOL initial_state = OUTPUT_STATE_OFF );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Output_PWM_CtrlV() {}

		inline void On( void );

		inline void Off( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_To( BOOL state )
		{
			if ( state != 0 )
			{
				On();				// Parrish Enable();
			}
			else
			{
				Off();					// Parrish Disable();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline BOOL State( void )
		{ return ( m_pwm_ctrl->Is_On() );}				// Parrish Is_Enabled()); }

	private:
		Timers* m_timer;
		UINT8 m_polarity;
		UINT16 m_min_pullin_v;
		UINT16 m_min_hold_v;
		uC_PWM* m_pwm_ctrl;
		Ctrl_Voltage* m_ctrl_voltage_ctrl;
		BOOL m_use_pullin_v;
		UINT16 Duty_Cycle_Calc( void );

		void Update_Duty_Cycle( void );


		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Update_Duty_Cycle_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{ ( ( Output_PWM_CtrlV* )handle )->Update_Duty_Cycle(); }


};



#endif
