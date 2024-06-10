/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_OPEN_DRAIN_NEG_H_
#define OUTPUT_OPEN_DRAIN_NEG_H_

#include "Includes.h"
#include "Output.h"

class Output_Open_Drain_Neg : public BF_Lib::Output
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		Output_Open_Drain_Neg( uC_USER_IO_STRUCT const* output_ctrl, bool initial_state =
							   OUTPUT_STATE_OFF )
		{
			m_output_ctrl = output_ctrl;
			m_state = -1;
			uC_IO_Config::Set_Pin_To_Output( output_ctrl, !initial_state );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual ~Output_Open_Drain_Neg() {}

		// On = High Impedence
		void Off( void )
		{
			m_state = 1;
			uC_IO_Config::Set_Pin_To_Input( m_output_ctrl, uC_IO_IN_CFG_HIZ );
		}

		// OFF = Pin pulled to low.
		void On( void )
		{
			m_state = 0U;
			uC_IO_Config::Set_Pin_To_Output( m_output_ctrl, uC_IO_OUT_INIT_LOW );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_To( bool state )
		{
			if ( state != 0 )
			{
				m_state = 1;
				On();
			}
			else
			{
				m_state = 0U;
				Off();
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool State( void )
		{
			return ( m_state );
		}

	private:
		uC_USER_IO_STRUCT const* m_output_ctrl;
		int8_t m_state;
};

#endif	/* OUTPUT_OPEN_DRAIN_NEG_H_ */
