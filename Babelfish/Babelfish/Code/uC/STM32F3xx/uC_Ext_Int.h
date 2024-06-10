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
#ifndef uC_EXT_INT_H
   #define uC_EXT_INT_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"

/*
 ***********************************
 *	Variables and Constants
 ***********************************
 */
#define uC_EXT_INT_5to9_VECT_COUNT      5U
#define uC_EXT_INT_10to15_VECT_COUNT    6U

enum
{
	EXT_INT_CHANGE_TRIG,
	EXT_INT_RISING_EDGE_TRIG,
	EXT_INT_FALLING_EDGE_TRIG
};

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Ext_Int
{
	public:
		uC_Ext_Int( uC_USER_IO_STRUCT const* pio_ctrl );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Ext_Int() {}


		void Attach_IRQ( INT_CALLBACK_FUNC* ext_int_callback, uint8_t edge_select,
						 uC_IO_INPUT_CFG_EN input_cfg, uint8_t priority ) const;

		void Enable_Int( void ) const;

		void Disable_Int( void ) const;

		void Clear_Int( void ) const;

		static void External_Int_Vect_5to9( void );

		static void External_Int_Vect_10to15( void );

	private:
		uC_BASE_EXT_INT_IO_STRUCT const* m_ext_int_ctrl;
		uC_USER_IO_STRUCT const* m_ext_pio_ctrl;

		static INT_CALLBACK_FUNC** m_ext_int_vect_5to9;
		static INT_CALLBACK_FUNC** m_ext_int_vect_10to15;
		static uint16_t m_activated_inputs;
};


#endif



