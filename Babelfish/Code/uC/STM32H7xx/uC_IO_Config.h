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
#ifndef uC_IO_CONFIG_H
   #define uC_IO_CONFIG_H

#include "uC_Base.h"

typedef enum uC_IO_INPUT_CFG_EN
{
	uC_IO_IN_CFG_HIZ,
	uC_IO_IN_CFG_PULLUP,
	uC_IO_IN_CFG_PULLDOWN
} uC_IO_INPUT_CFG_EN;

static const bool uC_IO_OUT_INIT_LOW = false;
static const bool uC_IO_OUT_INIT_HIGH = true;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_IO_Config
{
	public:
		uC_IO_Config( void );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_IO_Config() {}

		// Takes just the pin number.  You want input PIO 10 then ask for 10.  Not the mask.
		static inline bool Get_In( uC_USER_IO_STRUCT const* pio_ctrl )
		{ return ( Test_Bit( pio_ctrl->reg_ctrl->IDR, pio_ctrl->pio_num ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Get_In( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ return ( Test_Bit( pio_ctrl->reg_ctrl->IDR, pio_ctrl->pio_num ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{ pio_ctrl->reg_ctrl->BSRRL = static_cast<uint32_t>( 1 << pio_ctrl->pio_num ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set_Out( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ pio_ctrl->reg_ctrl->BSRRL = static_cast<uint32_t>( 1 << pio_ctrl->pio_num ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{ pio_ctrl->reg_ctrl->BSRRH = static_cast<uint32_t>( ( 1U ) << pio_ctrl->pio_num ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr_Out( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ pio_ctrl->reg_ctrl->BSRRH = static_cast<uint32_t>( ( 1U ) << pio_ctrl->pio_num ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Get_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{ return ( Test_Bit( pio_ctrl->reg_ctrl->ODR, pio_ctrl->pio_num ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Get_Out( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ return ( Test_Bit( pio_ctrl->reg_ctrl->ODR, pio_ctrl->pio_num ) ); }

		static void Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl,
									  uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		static void Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state = uC_IO_OUT_INIT_LOW );

		static void Set_Pin_To_High_Z( uC_USER_IO_STRUCT const* pio_ctrl );

		// Peripheral IO Configuration - requires pin descriptors.
		static void Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											 uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		static void Enable_Periph_Output_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											  bool initial_state = uC_IO_OUT_INIT_LOW );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Disable_Periph_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ Disable_Periph_Pin_Set_To_Input( pio_ctrl ); }

		static void Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													  bool initial_state = uC_IO_OUT_INIT_LOW );

		static void Disable_Periph_Pin_Set_To_Input( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													 uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static uint32_t Get_Input_Bits( GPIO_TypeDef* pio_reg )
		{ return ( pio_reg->IDR ); }

	private:
		static void Set_Pin_Remap( uC_PERIPH_IO_STRUCT const* pio_ctrl );

};



#endif
