/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
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
	PERIPH_DEF_ST temp_periph_def;
	uC_IO_INPUT_CFG_EN input_cfg;

	temp_periph_def.pclock_reg_index = RCC_PERIPH_CLCK_APB2;
	temp_periph_def.pid = RCC_APB2ENR_AFIOEN;

	uC_Base::Reset_Periph( &temp_periph_def );
	uC_Base::Enable_Periph_Clock( &temp_periph_def );

	for ( uint_fast8_t i = 0U; i < uC_BASE_MAX_PIO_CTRLS; i++ )
	{
		temp_pio = ( uC_Base::Self() )->Get_PIO_Ctrl( i );

		if ( temp_pio != nullptr )
		{
			temp_user_io.reg_ctrl = temp_pio->reg_ctrl;
			temp_user_io.port_index = static_cast<uint8_t>( i );

			uC_Base::Reset_Periph( &temp_pio->periph_def );
			uC_Base::Enable_Periph_Clock( &temp_pio->periph_def );

			for ( uint_fast8_t j = 0U; j < 16U; j++ )
			{
				if ( Test_Bit( *temp_pio->set_input, j ) )
				{
					temp_user_io.pio_num = static_cast<uint8_t>( j );
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
					temp_user_io.pio_num = static_cast<uint8_t>( j );
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
	__IO UINT32* config_reg;
	uint_fast8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( pio_ctrl->pio_num < 8U )
		{
			config_reg = &pio_ctrl->reg_ctrl->CRL;
		}
		else
		{
			config_reg = &pio_ctrl->reg_ctrl->CRH;
		}
		cfg_reg_shift = ( static_cast<uint32_t>( pio_ctrl->pio_num ) & 0x7U ) << MULT_BY_4;

		*config_reg &=
			~( ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) |
			   ( static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift ) );

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
			Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
			static_cast<uint32_t>( GPIO_CRL_CNF0_1 ) << cfg_reg_shift );
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
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
			Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
			static_cast<uint32_t>( GPIO_CRL_CNF0_1 ) << cfg_reg_shift );
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
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
			Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
			static_cast<uint32_t>( GPIO_CRL_CNF0_0 ) << cfg_reg_shift );
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
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
	__IO UINT32* config_reg;
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( pio_ctrl->pio_num < 8U )
		{
			config_reg = &pio_ctrl->reg_ctrl->CRL;
		}
		else
		{
			config_reg = &pio_ctrl->reg_ctrl->CRH;
		}
		cfg_reg_shift = ( pio_ctrl->pio_num & 0x7U ) << MULT_BY_4;

		*config_reg &=
			~( ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) |
			   ( static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift ) );
		*config_reg |= static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift;		// GPIO_CRL_MODE0 = 50MHz,
																						// GPIO_CRL_MODE0_0 = 10MHz

		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
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
	__IO UINT32* config_reg;
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}

		Set_Pin_Remap( pio_ctrl );

		if ( pio_ctrl->pio_num < 8U )
		{
			config_reg = &pio_ctrl->reg_ctrl->CRL;
		}
		else
		{
			config_reg = &pio_ctrl->reg_ctrl->CRH;
		}
		cfg_reg_shift = ( pio_ctrl->pio_num & 0x7U ) << MULT_BY_4;

		*config_reg &=
			~( ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) |
			   ( static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift ) );
		*config_reg |=
			( ( static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift ) |
			  ( static_cast<uint32_t>( GPIO_CRL_CNF0_1 ) << cfg_reg_shift ) );
		// GPIO_CRL_MODE0 == High speed 50MHz
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	__IO UINT32* config_reg;
	uint8_t cfg_reg_shift;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		Set_Pin_Remap( pio_ctrl );

		if ( pio_ctrl->pio_num < 8U )
		{
			config_reg = &pio_ctrl->reg_ctrl->CRL;
		}
		else
		{
			config_reg = &pio_ctrl->reg_ctrl->CRH;
		}
		cfg_reg_shift = ( pio_ctrl->pio_num & 0x7U ) << MULT_BY_4;
		*config_reg &=
			~( ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) |
			   ( static_cast<uint32_t>( GPIO_CRL_MODE0 ) << cfg_reg_shift ) );

		// If the port is an analog port then we do something different.
		if ( ( pio_ctrl->remap_num & uC_IO_ALTERNATE_MAP_ANALOG ) == 0U )
		{
			if ( input_cfg == uC_IO_IN_CFG_PULLUP )
			{
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type
				 *for sub-integers
				 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
				 *throughout.
				 */
				/*lint -e{1960} # MISRA Deviation */
				Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
				static_cast<uint32_t>( GPIO_CRL_CNF0_1 ) << cfg_reg_shift );
				pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
			}
			else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
			{
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type
				 *for sub-integers
				 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
				 *throughout.
				 */
				/*lint -e{1960} # MISRA Deviation */
				Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
				static_cast<uint32_t>( GPIO_CRL_CNF0_1 ) << cfg_reg_shift );
				pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
			}
			else
			{
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to underlying type
				 *for sub-integers
				 *@n Justification: Generic #define is used here, recasting in that #define would have adverse effects
				 *throughout.
				 */
				/*lint -e{1960} # MISRA Deviation */
				Set_Mask( *config_reg, static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift,
				static_cast<uint32_t>( GPIO_CRL_CNF0_0 ) << cfg_reg_shift );
				pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_Remap( uC_PERIPH_IO_STRUCT const* pio_ctrl )
{
	uint32_t temp_afio;
	uint32_t temp_remap_mask;

	temp_afio = static_cast<uint32_t>( pio_ctrl->remap_num ) >> uC_IO_REMAP_MASK_PACKED_SHIFT;
	temp_afio = AFIO->MAPR & ~( temp_afio << ( pio_ctrl->remap_bit_shift ) );
	temp_remap_mask = ( static_cast<uint32_t>( pio_ctrl->remap_num ) & uC_IO_ALTERNATE_MAP_MASK );
	temp_remap_mask = temp_remap_mask << ( pio_ctrl->remap_bit_shift );
	AFIO->MAPR = ( temp_remap_mask | temp_afio );
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
