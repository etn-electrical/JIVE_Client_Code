/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 *all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 *signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 *re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_IO_Config.h"


/*
 *****************************************************************************************
 *		Macros
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_IO_Config::uC_IO_Config( void )
{
	uC_BASE_PIO_CTRL_STRUCT const* temp_pio;
	uC_USER_IO_STRUCT temp_user_io;
	uC_IO_INPUT_CFG_EN input_cfg;

	for ( uint8_t i = 0U; i < static_cast<uint8_t>( uC_BASE_MAX_PIO_CTRLS ); i++ )
	{
		temp_pio = ( uC_Base::Self() )->Get_PIO_Ctrl( i );

		if ( temp_pio != nullptr )
		{
			temp_user_io.reg_ctrl = temp_pio->reg_ctrl;
			temp_user_io.port_index = i;

			uC_Base::Reset_Periph( &temp_pio->periph_def );
			uC_Base::Enable_Periph_Clock( &temp_pio->periph_def );

			for ( uint8_t j = 0U; j < 16U; j++ )
			{
				if ( Test_Bit( *temp_pio->set_input, j ) )
				{
					temp_user_io.pio_num = j;
					if ( Test_Bit( *temp_pio->input_pullup, j ) )
					{
						input_cfg = uC_IO_IN_CFG_PULLUP;
					}
					else if ( Test_Bit( *temp_pio->input_pulldown, j ) )
					{
						input_cfg = uC_IO_IN_CFG_PULLDOWN;
					}
					else
					{
						input_cfg = uC_IO_IN_CFG_HIZ;
					}
					Set_Pin_To_Input( &temp_user_io, input_cfg );
				}
				else if ( Test_Bit( *temp_pio->set_output, j ) )
				{
					temp_user_io.pio_num = j;
					Set_Pin_To_Output( &temp_user_io, Test_Bit( *temp_pio->output_init, j ) );
				}
				else
				{
					// do nothing
				}
			}
		}
	}

	DEBUG_OUTPUT_Config();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		cfg_reg_shift = pio_ctrl->pio_num * 2U;

		pio_ctrl->reg_ctrl->MODER &= ~( GPIO_MODER_MODER0 << cfg_reg_shift );

		if ( input_cfg == uC_IO_IN_CFG_PULLUP )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
					  ( GPIO_PUPDR_PUPDR0_0 << cfg_reg_shift ) );
		}
		else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
					  ( GPIO_PUPDR_PUPDR0_1 << cfg_reg_shift ) );
		}
		else// if ( input_cfg == uC_IO_IN_CFG_HIZ )	//No pull up/down.
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( static_cast<uint32_t>( GPIO_PUPDR_PUPDR0 ) << cfg_reg_shift ), 0U );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->reg_ctrl->BSRRL = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->reg_ctrl->BSRRH = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}

		cfg_reg_shift = pio_ctrl->pio_num * 2U;

		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
		 *sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
		 *throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( pio_ctrl->reg_ctrl->MODER,
				  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
				  ( GPIO_MODER_MODER0_0 << cfg_reg_shift ) );

		pio_ctrl->reg_ctrl->OTYPER &= ~( static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num );

		// Set to 25MHz.
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
		 *sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
		 *throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( pio_ctrl->reg_ctrl->OSPEEDR,
				  ( GPIO_OSPEEDER_OSPEEDR0 << cfg_reg_shift ),
				  ( GPIO_OSPEEDER_OSPEEDR0_0 << cfg_reg_shift ) );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_High_Z( uC_USER_IO_STRUCT const* pio_ctrl )
{
	Set_Pin_To_Input( pio_ctrl, uC_IO_IN_CFG_HIZ );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Output_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->reg_ctrl->BSRRL = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->reg_ctrl->BSRRH = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}

		pio_ctrl->reg_ctrl->OTYPER &= ~( static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num );

		Set_Pin_Remap( pio_ctrl );

		cfg_reg_shift = pio_ctrl->pio_num * 2U;

		// Set to highest speed possible.
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
		 *sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
		 *throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( pio_ctrl->reg_ctrl->OSPEEDR,
				  ( GPIO_OSPEEDER_OSPEEDR0 << cfg_reg_shift ),
				  ( GPIO_OSPEEDER_OSPEEDR0 << cfg_reg_shift ) );

		if ( pio_ctrl->alt_map <= static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_15 ) )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->MODER,
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
					  ( GPIO_MODER_MODER0_1 << cfg_reg_shift ) );
		}
		else if ( pio_ctrl->alt_map == static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->MODER,
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ) );
		}
		else
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->MODER,
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
					  ( GPIO_MODER_MODER0_0 << cfg_reg_shift ) );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		cfg_reg_shift = pio_ctrl->pio_num * 2U;

		if ( pio_ctrl->alt_map <= static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_15 ) )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->MODER,
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
					  ( GPIO_MODER_MODER0_1 << cfg_reg_shift ) );
		}
		else if ( pio_ctrl->alt_map == static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_ANALOG ) )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->MODER,
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ),
					  ( GPIO_MODER_MODER0 << cfg_reg_shift ) );
		}
		else
		{
			// do nothing
		}
		if ( input_cfg == uC_IO_IN_CFG_PULLUP )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
					  ( GPIO_PUPDR_PUPDR0_0 << cfg_reg_shift ) );
		}
		else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
					  ( GPIO_PUPDR_PUPDR0_1 << cfg_reg_shift ) );
		}
		else// if ( input_cfg == uC_IO_IN_CFG_HIZ )	//No pull up/down.
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
			 *sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
			 *throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( pio_ctrl->reg_ctrl->PUPDR,
					  ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ), 0U );
		}

		Set_Pin_Remap( pio_ctrl );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_Remap( uC_PERIPH_IO_STRUCT const* pio_ctrl )
{
	uint8_t afr_index;
	uint8_t alt_map_shift;
	uint32_t shift_temp;

	afr_index = pio_ctrl->pio_num >> DIV_BY_8;
	alt_map_shift = ( pio_ctrl->pio_num & 0x7U ) * 4U;

	if ( pio_ctrl->alt_map <= static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_15 ) )
	{
		shift_temp = pio_ctrl->alt_map;
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
		 *sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
		 *throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( pio_ctrl->reg_ctrl->AFR[afr_index],
				  ( static_cast<uint32_t>( uC_IO_ALTERNATE_MAP_MASK ) << static_cast<uint32_t>( alt_map_shift ) ),
				  ( shift_temp << alt_map_shift ) );
	}
	else
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type for
		 *sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
		 *throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( pio_ctrl->reg_ctrl->AFR[afr_index],
				  ( static_cast<uint32_t>( uC_IO_ALTERNATE_MAP_MASK ) << static_cast<uint32_t>( alt_map_shift ) ), 0U );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	uC_USER_IO_STRUCT user_io;

	user_io.reg_ctrl = pio_ctrl->reg_ctrl;
	user_io.pio_num = pio_ctrl->pio_num;

	Set_Pin_To_Output( &user_io, initial_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Disable_Periph_Pin_Set_To_Input( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													uC_IO_INPUT_CFG_EN input_cfg )
{
	uC_USER_IO_STRUCT user_io;

	user_io.reg_ctrl = pio_ctrl->reg_ctrl;
	user_io.pio_num = pio_ctrl->pio_num;

	Set_Pin_To_Input( &user_io, input_cfg );
}

/* lint +e1924
  lint +e1960*/
