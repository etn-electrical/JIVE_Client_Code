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

/*
 ***********************************
 *	Variables and Constants
 ***********************************
 */
#define uC_EXT_INT_0to1_VECT_COUNT      2U
#define uC_EXT_INT_2to3_VECT_COUNT      2U
#define uC_EXT_INT_4to15_VECT_COUNT     12U

static const uint8_t EXT_INT_CHANGE_TRIG = 0U;
static const uint8_t EXT_INT_RISING_EDGE_TRIG = 1U;
static const uint8_t EXT_INT_FALLING_EDGE_TRIG = 2U;


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

		static void External_Int_Vect_0to1( void );

		static void External_Int_Vect_2to3( void );

		static void External_Int_Vect_4to15( void );

		void Set_Vector_0to1( INT_CALLBACK_FUNC* ext_int_callback ) const;		// SSN added

		void Set_Vector_2to3( INT_CALLBACK_FUNC* ext_int_callback ) const;		// SSN added

		void Set_Vector_4to15( INT_CALLBACK_FUNC* ext_int_callback ) const;					// SSN added

	private:
		uC_BASE_EXT_INT_IO_STRUCT const* m_ext_int_ctrl;
		uC_USER_IO_STRUCT const* m_ext_pio_ctrl;

		static INT_CALLBACK_FUNC** m_ext_int_vect_0to1;
		static INT_CALLBACK_FUNC** m_ext_int_vect_2to3;
		static INT_CALLBACK_FUNC** m_ext_int_vect_4to15;
		static uint16_t m_activated_inputs;
};


#endif



