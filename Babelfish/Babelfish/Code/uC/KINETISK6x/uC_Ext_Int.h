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
#define uC_EXT_INT_VECT_COUNT       32

static const uint8_t EXT_INT_DISABLED = 0U;
static const uint8_t EXT_INT_DMA_RISING_EDGE_TRIG = 1U;
static const uint8_t EXT_INT_DMA_FALLING_EDGE_TRIG = 2U;
static const uint8_t EXT_INT_DMA_CHANGE_TRIG = 3U;
static const uint8_t EXT_INT_LOGIC_ZERO = 8U;
static const uint8_t EXT_INT_LOGIC_ONE = 12U;
static const uint8_t EXT_INT_CHANGE_TRIG = 11U;
static const uint8_t EXT_INT_RISING_EDGE_TRIG = 9U;
static const uint8_t EXT_INT_FALLING_EDGE_TRIG = 10U;


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
						 uC_IO_INPUT_CFG_EN input_cfg, uint8_t priority );

		void Enable_Int( void ) const;

		void Disable_Int( void ) const;

		void Clear_Int( void ) const;

		static void External_Int_Vect_A( void );

		static void External_Int_Vect_B( void );

		static void External_Int_Vect_C( void );

		static void External_Int_Vect_D( void );

		static void External_Int_Vect_E( void );

	private:
		uC_BASE_EXT_INT_IO_STRUCT const* m_ext_int_ctrl;
		uC_USER_IO_STRUCT const* m_ext_pio_ctrl;
		uint8_t m_edge_select;

		static INT_CALLBACK_FUNC* m_ext_int_vect_a[uC_EXT_INT_VECT_COUNT];
		static INT_CALLBACK_FUNC* m_ext_int_vect_b[uC_EXT_INT_VECT_COUNT];
		static INT_CALLBACK_FUNC* m_ext_int_vect_c[uC_EXT_INT_VECT_COUNT];
		static INT_CALLBACK_FUNC* m_ext_int_vect_d[uC_EXT_INT_VECT_COUNT];
		static INT_CALLBACK_FUNC* m_ext_int_vect_e[uC_EXT_INT_VECT_COUNT];
		static BOOL m_attached[uC_BASE_EXT_INT_MAX_PORTS];
		static UINT32 m_activated_inputs[uC_BASE_EXT_INT_MAX_PORTS];
};


#endif



