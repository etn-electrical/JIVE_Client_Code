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
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
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
			temp_user_io.gio_ctrl = temp_pio->reg_ctrl;
			temp_user_io.pin_ctrl = temp_pio->pin_ctrl;
			temp_user_io.port_index = i;

			switch ( i )
			{
				case uC_BASE_PIO_A:
					SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
					break;

				case uC_BASE_PIO_B:
					SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
					break;

				case uC_BASE_PIO_C:
					SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
					break;

				case uC_BASE_PIO_D:
					SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
					break;

				case uC_BASE_PIO_E:
					SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
					break;

				default:
					// BF_ASSERT( false );
					break;
			}

			// uC_Base::Reset_Periph( &temp_pio->periph_def );
			// uC_Base::Enable_Periph_Clock( &temp_pio->periph_def );

			for ( uint8_t j = 0U; j < 16U; j++ )
			{
				if ( Test_Bit( *temp_pio->set_input, j ) )
				{
					temp_user_io.pio_num = j;
					if ( Test_Bit( *temp_pio->input_pullup, j ) )
					{
						input_cfg = uC_IO_IN_CFG_PULLUP;
					}
					if ( Test_Bit( *temp_pio->input_pulldown, j ) )
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

	// DEBUG_OUTPUT_Config();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	if ( pio_ctrl->pio_num < IO_PIN_UNDEFINED )
	{
		pio_ctrl->gio_ctrl->PDDR &= ~( 1 << ( pio_ctrl->pio_num ) );

		if ( input_cfg == uC_IO_IN_CFG_PULLUP )
		{
			Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK,				// pull select and pull enable
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK );			// pull select and pull enable
		}
		else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
		{
			Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK,				// pull select and pull enable
					  PORT_PCR_PE_MASK );								// pull enable
		}
		else// if ( input_cfg == uC_IO_IN_CFG_HIZ )	//No pull up/down.
		{
			Clr_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PE_MASK );							// pull enable
		}
		// assume if using this setup, the pin will be used as GPIO
		Set_Pin_To_GPIO_MUX( pio_ctrl );
	}
	else
	{
		// Do nothing
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	if ( pio_ctrl->pio_num < IO_PIN_UNDEFINED )
	{
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->gio_ctrl->PSOR = 1 << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->gio_ctrl->PCOR = 1 << pio_ctrl->pio_num;
		}

		pio_ctrl->gio_ctrl->PDDR |= ( 1 << ( pio_ctrl->pio_num ) );

		// assume if using this setup, the pin will be used as GPIO
		Set_Pin_To_GPIO_MUX( pio_ctrl );

	}
	else
	{
		// Do nothing
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
	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			pio_ctrl->gio_ctrl->PSOR = 1 << pio_ctrl->pio_num;
		}
		else
		{
			pio_ctrl->gio_ctrl->PCOR = 1 << pio_ctrl->pio_num;
		}

		Set_Pin_Remap( pio_ctrl );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		if ( input_cfg == uC_IO_IN_CFG_PULLUP )
		{
			Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK,				// pull select and pull enable
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK );			// pull select and pull enable
		}
		else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
		{
			Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PS_MASK | PORT_PCR_PE_MASK,				// pull select and pull enable
					  PORT_PCR_PE_MASK );								// pull enable
		}
		else// if ( input_cfg == uC_IO_IN_CFG_HIZ )	//No pull up/down.
		{
			Clr_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
					  PORT_PCR_PE_MASK );							// pull enable
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
	Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
			  PORT_PCR_MUX_MASK,
			  ( pio_ctrl->alt_map ) << PORT_PCR_MUX_SHIFT );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_GPIO_MUX( uC_USER_IO_STRUCT const* pio_ctrl )
{
	Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
			  PORT_PCR_MUX_MASK,
			  ( uC_IO_ALTERNATE_MAP_1 ) << PORT_PCR_MUX_SHIFT );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_Interrupt_State( uC_USER_IO_STRUCT const* pio_ctrl, UINT32 int_state )
{
	Set_Mask( pio_ctrl->pin_ctrl->PCR[pio_ctrl->pio_num],
			  PORT_PCR_IRQC_MASK,
			  ( int_state ) << PORT_PCR_IRQC_SHIFT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	uC_USER_IO_STRUCT user_io;

	user_io.gio_ctrl = pio_ctrl->gio_ctrl;
	user_io.pin_ctrl = pio_ctrl->pin_ctrl;
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

	user_io.gio_ctrl = pio_ctrl->gio_ctrl;
	user_io.pin_ctrl = pio_ctrl->pin_ctrl;
	user_io.pio_num = pio_ctrl->pio_num;

	Set_Pin_To_Input( &user_io, input_cfg );
}

/* lint +e1924
   lint +e1960*/
