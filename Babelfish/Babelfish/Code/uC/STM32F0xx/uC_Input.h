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
#ifndef uC_INPUT_H
   #define uC_INPUT_H

#include "uC_IO_Config.h"
#include "CR_Tasker.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define UC_INPUT_DEBOUNCE_ARRAY_COUNT       4
// #define UC_INPUT_XOR_DEBOUNCE

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Input
{
	public:
		uC_Input( void );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Input() {}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static bool Get_Input( uC_USER_IO_STRUCT const* pio_ctrl )
		{ return ( Test_Bit( m_good_input[pio_ctrl->port_index], pio_ctrl->pio_num ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Config_Input_Pin( uC_USER_IO_STRUCT const* pio_ctrl,
									  uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ )
		{ uC_IO_Config::Set_Pin_To_Input( pio_ctrl, input_cfg ); }

		static void Debounce_Quick_Update( uC_USER_IO_STRUCT const* pio_ctrl );

		static void Debounce_Quick_Update( void );

	private:
		typedef uint16_t INPUT_WRD_TD;

		static void Debounce_Pins( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		static uC_BASE_PIO_CTRL_STRUCT const* m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];

		static INPUT_WRD_TD m_deb_array[uC_BASE_MAX_PIO_CTRLS][UC_INPUT_DEBOUNCE_ARRAY_COUNT];
		static INPUT_WRD_TD m_good_input[uC_BASE_MAX_PIO_CTRLS];
};


#endif
