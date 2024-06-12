/******************************************************************************
 * @file    uc_IO_Config.cpp
 *
 * @brief   GPIO HAL module driver.
 *          This file provides firmware functions to manage the following
 *          functionalities of the General Purpose Input/Output (GPIO) peripheral:
 *          + Initialization and de-initialization functions
 *          + IO operation functionsn
 *
 * @date    05 May 2017
 * @author  PXGREEN Team
 * @details Inital Draft
 *
 ******************************************************************************
 */
/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************

   ==============================================================================
 ##### GPIO Peripheral features #####
   ==============================================================================
   [..]
   Each port bit of the general-purpose I/O (GPIO) ports can be individually
   configured by software in several modes:
   (+) Input mode
   (+) Analog mode
   (+) Output mode
   (+) Alternate function mode

   [..]
   During and just after reset, the alternate functions and external interrupt
   lines are not active and the I/O ports are configured in input floating mode.

   [..]
   All GPIO pins have weak internal pull-up and pull-down resistors, which can be
   activated or not.

   [..]
   In Output or Alternate mode, each IO can be configured on open-drain or push-pull
   type and the IO speed can be selected depending on the VDD value.

   [..]
   The microcontroller IO pins are connected to onboard peripherals/modules through a
   multiplexer that allows only one peripheral s alternate function (AF) connected
   to an IO pin at a time. In this way, there can be no conflict between peripherals
   sharing the same IO pin.

   [..]
   All ports have external interrupt/event capability. To use external interrupt
   lines, the port must be configured in input mode. All available GPIO pins are
   connected to the 16 external interrupt/event lines from EXTI0 to EXTI15.

 */

/*

   ==============================================================================
 ##### GPIO Registers  #####
   ==============================================================================
 */
/*
 *****************************************************************************************
 *		         GPIO port mode register
 *****************************************************************************************
 */
// GPIOx_MODER (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__|- MODER0[1:0] |
// |||| |||| |||| ||||  |||| |||| |||| ||___               |
// |||| |||| |||| ||||  |||| |||| |||| |____|- MODER1[1:0] |
// |||| |||| |||| ||||  |||| |||| ||||______               |
// |||| |||| |||| ||||  |||| |||| |||_______|- MODER2[1:0] |
// |||| |||| |||| ||||  |||| |||| ||________               |
// |||| |||| |||| ||||  |||| |||| |_________|- MODER3[1:0] |
// |||| |||| |||| ||||  |||| ||||___________               |
// |||| |||| |||| ||||  |||| |||____________|- MODER4[1:0] |
// |||| |||| |||| ||||  |||| ||_____________               |
// |||| |||| |||| ||||  |||| |______________|- MODER5[1:0] |
// |||| |||| |||| ||||  ||||________________               |
// |||| |||| |||| ||||  |||_________________|- MODER6[1:0] |
// |||| |||| |||| ||||  ||__________________               |
// |||| |||| |||| ||||  |___________________|- MODER7[1:0] |
// |||| |||| |||| ||||______________________               |- MODERy[1:0]: 00: Input mode (reset
// state)
// |||| |||| |||| |||_______________________|- MODER8[1:0] |               01: General purpose output
// mode
// |||| |||| |||| ||________________________               |               10: Alternate function
// mode
// |||| |||| |||| |_________________________|- MODER9[1:0] |               11: Analog mode
// |||| |||| ||||___________________________               |
// |||| |||| |||____________________________|- MODER10[1:0]|
// |||| |||| ||_____________________________               |
// |||| |||| |______________________________|- MODER11[1:0]|
// |||| ||||________________________________               |
// |||| |||_________________________________|- MODER12[1:0]|
// |||| ||__________________________________               |
// |||| |___________________________________|- MODER13[1:0]|
// ||||_____________________________________               |
// |||______________________________________|- MODER14[1:0]|
// ||_______________________________________               |
// |________________________________________|- MODER15[1:0]|

/*
 *****************************************************************************************
 *		         GPIO port pull-up/pull-down register
 *****************************************************************************************
 */

// GPIOx_PUPDR (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__|- PUPDR0[1:0] |
// |||| |||| |||| ||||  |||| |||| |||| ||___               |
// |||| |||| |||| ||||  |||| |||| |||| |____|- PUPDR1[1:0] |
// |||| |||| |||| ||||  |||| |||| ||||______               |
// |||| |||| |||| ||||  |||| |||| |||_______|- PUPDR2[1:0] |
// |||| |||| |||| ||||  |||| |||| ||________               |
// |||| |||| |||| ||||  |||| |||| |_________|- PUPDR3[1:0] |
// |||| |||| |||| ||||  |||| ||||___________               |
// |||| |||| |||| ||||  |||| |||____________|- PUPDR4[1:0] |
// |||| |||| |||| ||||  |||| ||_____________               |
// |||| |||| |||| ||||  |||| |______________|- PUPDR5[1:0] |
// |||| |||| |||| ||||  ||||________________               |
// |||| |||| |||| ||||  |||_________________|- PUPDR6[1:0] |
// |||| |||| |||| ||||  ||__________________               |
// |||| |||| |||| ||||  |___________________|- PUPDR7[1:0] |
// |||| |||| |||| ||||______________________               |- PUPDRy[1:0]: 00: No pull-up, pull-down
// |||| |||| |||| |||_______________________|- PUPDR8[1:0] |               01: Pull-up
// |||| |||| |||| ||________________________               |               10: Pull-down
// |||| |||| |||| |_________________________|- PUPDR9[1:0] |               11: Reserved
// |||| |||| ||||___________________________               |
// |||| |||| |||____________________________|- PUPDR10[1:0]|
// |||| |||| ||_____________________________               |
// |||| |||| |______________________________|- PUPDR11[1:0]|
// |||| ||||________________________________               |
// |||| |||_________________________________|- PUPDR12[1:0]|
// |||| ||__________________________________               |
// |||| |___________________________________|- PUPDR13[1:0]|
// ||||_____________________________________               |
// |||______________________________________|- PUPDR14[1:0]|
// ||_______________________________________               |
// |________________________________________|- PUPDR15[1:0]|


/*
 *****************************************************************************************
 *		         GPIO port output type register
 *****************************************************************************************
 */

// GPIOx_OTYPER (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__OT0  |
// |||| |||| |||| ||||  |||| |||| |||| ||___OT1  |
// |||| |||| |||| ||||  |||| |||| |||| |____OT2  |
// |||| |||| |||| ||||  |||| |||| ||||______OT3  |
// |||| |||| |||| ||||  |||| |||| |||_______OT4  |
// |||| |||| |||| ||||  |||| |||| ||________OT5  |
// |||| |||| |||| ||||  |||| |||| |_________OT6  |
// |||| |||| |||| ||||  |||| ||||___________OT7  |
// |||| |||| |||| ||||  |||| |||____________OT8  |-Bits 15:0 OTy: Port x configuration bits (y =
// 0..15)
// |||| |||| |||| ||||  |||| ||_____________OT9  |     0: Output push-pull (reset state)
// |||| |||| |||| ||||  |||| |______________OT10 |     1: Output open-drain
// |||| |||| |||| ||||  ||||________________OT11 |
// |||| |||| |||| ||||  |||_________________OT12 |
// |||| |||| |||| ||||  ||__________________OT13 |
// |||| |||| |||| ||||  |___________________OT14 |
// |||| |||| |||| ||||______________________OT15 |
// |||| |||| |||| |||_______________________|
// |||| |||| |||| ||________________________|
// |||| |||| |||| |_________________________|
// |||| |||| ||||___________________________|
// |||| |||| |||____________________________|
// |||| |||| ||_____________________________|
// |||| |||| |______________________________|- Reserved
// |||| ||||________________________________|
// |||| |||_________________________________|
// |||| ||__________________________________|
// |||| |___________________________________|
// ||||_____________________________________|
// |||______________________________________|
// ||_______________________________________|
// |________________________________________|

/*
 *****************************************************************************************
 *		         GPIO port output speed register
 *****************************************************************************************
 */

// GPIOx_OSPEEDR (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__|- OSPEEDR0[1:0] |
// |||| |||| |||| ||||  |||| |||| |||| ||___                 |
// |||| |||| |||| ||||  |||| |||| |||| |____|- OSPEEDR1[1:0] |
// |||| |||| |||| ||||  |||| |||| ||||______                 |
// |||| |||| |||| ||||  |||| |||| |||_______|- OSPEEDR2[1:0] |
// |||| |||| |||| ||||  |||| |||| ||________                 |
// |||| |||| |||| ||||  |||| |||| |_________|- OSPEEDR3[1:0] |
// |||| |||| |||| ||||  |||| ||||___________                 |
// |||| |||| |||| ||||  |||| |||____________|- OSPEEDR4[1:0] |
// |||| |||| |||| ||||  |||| ||_____________                 |
// |||| |||| |||| ||||  |||| |______________|- OSPEEDR5[1:0] |
// |||| |||| |||| ||||  ||||________________                 |
// |||| |||| |||| ||||  |||_________________|- OSPEEDR6[1:0] |
// |||| |||| |||| ||||  ||__________________                 |
// |||| |||| |||| ||||  |___________________|- OSPEEDR7[1:0] |
// |||| |||| |||| ||||______________________                 |- OSPEEDRy[1:0]: Port x configuration
// bits (y = 0..15)
// |||| |||| |||| |||_______________________|- OSPEEDR8[1:0] |  x0: Low speed
// |||| |||| |||| ||________________________                 |  01: Medium speed
// |||| |||| |||| |_________________________|- OSPEEDR9[1:0] |  11: High speed
// |||| |||| ||||___________________________                 |
// |||| |||| |||____________________________|- OSPEEDR10[1:0]|
// |||| |||| ||_____________________________                 |
// |||| |||| |______________________________|- OSPEEDR11[1:0]|
// |||| ||||________________________________                 |
// |||| |||_________________________________|- OSPEEDR12[1:0]|
// |||| ||__________________________________                 |
// |||| |___________________________________|- OSPEEDR13[1:0]|
// ||||_____________________________________                 |
// |||______________________________________|- OSPEEDR14[1:0]|
// ||_______________________________________                 |
// |________________________________________|- OSPEEDR15[1:0]|


/*
 *****************************************************************************************
 *		         GPIO port bit set/reset register
 *****************************************************************************************
 */

// GPIOx_BSRR (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_BS0   |
// |||| |||| |||| ||||  |||| |||| |||| |||__BS1   |
// |||| |||| |||| ||||  |||| |||| |||| ||___BS2   |
// |||| |||| |||| ||||  |||| |||| |||| |____BS3   |
// |||| |||| |||| ||||  |||| |||| ||||______BS4   |
// |||| |||| |||| ||||  |||| |||| |||_______BS5   |
// |||| |||| |||| ||||  |||| |||| ||________BS6   |
// |||| |||| |||| ||||  |||| |||| |_________BS7   |- Bits 15:0 BSy: Port x set bit y (y= 0..15)
// |||| |||| |||| ||||  |||| ||||___________BS8   |  These bits are write-only. A read to these bits
// returns the value 0x0000.
// |||| |||| |||| ||||  |||| |||____________BS9   |  0: No action on the corresponding ODRx bit
// |||| |||| |||| ||||  |||| ||_____________BS10  |  1: Sets the corresponding ODRx bit
// |||| |||| |||| ||||  |||| |______________BS11  |
// |||| |||| |||| ||||  ||||________________BS12  |
// |||| |||| |||| ||||  |||_________________BS13  |
// |||| |||| |||| ||||  ||__________________BS14  |
// |||| |||| |||| ||||  |___________________BS15  |
// |||| |||| |||| ||||______________________BR0  |
// |||| |||| |||| |||_______________________BR1  |
// |||| |||| |||| ||________________________BR2  |
// |||| |||| |||| |_________________________BR3  |
// |||| |||| ||||___________________________BR4  |
// |||| |||| |||____________________________BR5  |
// |||| |||| ||_____________________________BR6  |
// |||| |||| |______________________________BR7  |
// |||| ||||________________________________BR8  |- Bits 31:16 BRy: Port x reset bit y (y = 0..15)
// |||| |||_________________________________BR9  |  These bits are write-only. A read to these bits
// returns the value 0x0000.
// |||| ||__________________________________BR10 |  0: No action on the corresponding ODRx bit
// |||| |___________________________________BR11 |  1: Resets the corresponding ODRx bit
// ||||_____________________________________BR12 |  Note:If both BSx and BRx are set, BSx has
// priority.
// |||______________________________________BR13 |
// ||_______________________________________BR14 |
// |________________________________________BR15 |


/*
 *****************************************************************************************
 *		         GPIO port bit reset register
 *****************************************************************************************
 */

// GPIOx_BRR (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__BR0  |
// |||| |||| |||| ||||  |||| |||| |||| ||___BR1  |
// |||| |||| |||| ||||  |||| |||| |||| |____BR2  |
// |||| |||| |||| ||||  |||| |||| ||||______BR3  |
// |||| |||| |||| ||||  |||| |||| |||_______BR4  |
// |||| |||| |||| ||||  |||| |||| ||________BR5  |
// |||| |||| |||| ||||  |||| |||| |_________BR6  |
// |||| |||| |||| ||||  |||| ||||___________BR7  |
// |||| |||| |||| ||||  |||| |||____________BR8  |- Bits 15:0 BRy: Port x reset bit y (y = 0..15)
// |||| |||| |||| ||||  |||| ||_____________BR9  |  These bits are write-only. A read to these bits
// returns the value 0x0000.
// |||| |||| |||| ||||  |||| |______________BR10 |  0: No action on the corresponding ODRx bit
// |||| |||| |||| ||||  ||||________________BR11 |  1: Resets the corresponding ODRx bit
// |||| |||| |||| ||||  |||_________________BR12 |  Note:If both BSx and BRx are set, BSx has
// priority.
// |||| |||| |||| ||||  ||__________________BR13 |
// |||| |||| |||| ||||  |___________________BR14 |
// |||| |||| |||| ||||______________________BR15 |
// |||| |||| |||| |||_______________________|
// |||| |||| |||| ||________________________|
// |||| |||| |||| |_________________________|
// |||| |||| ||||___________________________|
// |||| |||| |||____________________________|
// |||| |||| ||_____________________________|
// |||| |||| |______________________________|- Reserved
// |||| ||||________________________________|
// |||| |||_________________________________|
// |||| ||__________________________________|
// |||| |___________________________________|
// ||||_____________________________________|
// |||______________________________________|
// ||_______________________________________|
// |________________________________________|


/*
 *****************************************************************************************
 *		         GPIO alternate function high register
 *****************************************************************************************
 */
// GPIOx_AFRH (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0010
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__|
// |||| |||| |||| ||||  |||| |||| |||| ||___|- AFR0[3:0](Pin 8)
// |||| |||| |||| ||||  |||| |||| |||| |____|
// |||| |||| |||| ||||  |||| |||| ||||______
// |||| |||| |||| ||||  |||| |||| |||_______|
// |||| |||| |||| ||||  |||| |||| ||________|- AFR1[3:0] (Pin 9)
// |||| |||| |||| ||||  |||| |||| |_________|
// |||| |||| |||| ||||  |||| ||||___________
// |||| |||| |||| ||||  |||| |||____________|
// |||| |||| |||| ||||  |||| ||_____________|- AFR2[3:0] (Pin 10)
// |||| |||| |||| ||||  |||| |______________|
// |||| |||| |||| ||||  ||||________________
// |||| |||| |||| ||||  |||_________________|
// |||| |||| |||| ||||  ||__________________|- AFR3[3:0] (Pin 11)
// |||| |||| |||| ||||  |___________________|
// |||| |||| |||| ||||______________________
// |||| |||| |||| |||_______________________|
// |||| |||| |||| ||________________________|- AFR4[3:0] (Pin 12)
// |||| |||| |||| |_________________________|
// |||| |||| ||||___________________________
// |||| |||| |||____________________________|
// |||| |||| ||_____________________________|- AFR5[3:0] (Pin 13)
// |||| |||| |______________________________|
// |||| ||||________________________________
// |||| |||_________________________________|
// |||| ||__________________________________|- AFR6[3:0] (Pin 14)
// |||| |___________________________________|
// ||||_____________________________________
// |||______________________________________|
// ||_______________________________________|- AFR7[3:0] (Pin 15)
// |________________________________________|


/*
 *****************************************************************************************
 *		         GPIO alternate function low register
 *****************************************************************************************
 */
// GPIOx_AFRL (x =A..D, F) =

// 0b0000 0000 0000 0000  0000 0000 0000 0000
// |||| |||| |||| ||||  |||| |||| |||| ||||_
// |||| |||| |||| ||||  |||| |||| |||| |||__|
// |||| |||| |||| ||||  |||| |||| |||| ||___|- AFR0[3:0](Pin 0)
// |||| |||| |||| ||||  |||| |||| |||| |____|
// |||| |||| |||| ||||  |||| |||| ||||______
// |||| |||| |||| ||||  |||| |||| |||_______|
// |||| |||| |||| ||||  |||| |||| ||________|- AFR1[3:0] (Pin 1)
// |||| |||| |||| ||||  |||| |||| |_________|
// |||| |||| |||| ||||  |||| ||||___________
// |||| |||| |||| ||||  |||| |||____________|
// |||| |||| |||| ||||  |||| ||_____________|- AFR2[3:0] (Pin 2)
// |||| |||| |||| ||||  |||| |______________|
// |||| |||| |||| ||||  ||||________________
// |||| |||| |||| ||||  |||_________________|
// |||| |||| |||| ||||  ||__________________|- AFR3[3:0] (Pin 3)
// |||| |||| |||| ||||  |___________________|
// |||| |||| |||| ||||______________________
// |||| |||| |||| |||_______________________|
// |||| |||| |||| ||________________________|- AFR4[3:0] (Pin 4)
// |||| |||| |||| |_________________________|
// |||| |||| ||||___________________________
// |||| |||| |||____________________________|
// |||| |||| ||_____________________________|- AFR5[3:0] (Pin 5)
// |||| |||| |______________________________|
// |||| ||||________________________________
// |||| |||_________________________________|
// |||| ||__________________________________|- AFR6[3:0] (Pin 6)
// |||| |___________________________________|
// ||||_____________________________________
// |||______________________________________|
// ||_______________________________________|- AFR7[3:0] (Pin 7)
// |________________________________________|

/*
 *****************************************************************************************
 *		Include
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

/*******************************************************************************
 *  @fn         uC_IO_Config
 *  @brief      Configures all the ports to input,output,pullup or pulldown and
 *              also enables their clock.Configuration is based on the configuration
 *              done in the m_pio_ctrls structure array.
 *
 *  @param[in]  None
 *  @param[out] None
 *  @return     None
 ******************************************************************************/

uC_IO_Config::uC_IO_Config( void )
{
	uC_BASE_PIO_CTRL_STRUCT const* temp_pio;
	uC_USER_IO_STRUCT temp_user_io;
	uC_IO_INPUT_CFG_EN input_cfg;

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

/*******************************************************************************
 *  @fn         Set_Pin_To_Input
 *  @brief      Configure the port pin to GPIO input mode and set it to pull up
 *              or pull down
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              input_cfg => Provides information whether the pin is required to
 *                           be configured pull up or pulldown
 *  @param[out] None
 *  @return     None
 ******************************************************************************/

void uC_IO_Config::Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl,
									 uC_IO_INPUT_CFG_EN input_cfg )
{
	__IO UINT32* config_reg;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		/* Get the access to mode register */
		config_reg = &pio_ctrl->reg_ctrl->MODER;

		/* Clear/Mask the MODER register */
		/* Port number is multiplied by 2 and the value 3 is shifted that number of times
		   the value is inverted and ANDed with the PUPDR register*/
		/* Example:- pio_ctrl->pio_num = 2
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << (2 * 2)))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << 4))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000030))
		   GPIOx_MODER =  GPIOx_MODER & FFFFFFCF */
		/* The value to set the input is 0x00 the value is already set in the process*/
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Get the access to Pullup /pull down register */
		config_reg = &pio_ctrl->reg_ctrl->PUPDR;

		/* Clear/Mask the PUPDR register */
		/* Port number is multiplied by 2 and the value 3 is shifted that number of times
		   the value is inverted and ANDed with the PUPDR register*/
		/* Example:- pio_ctrl->pio_num = 2
		   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000003 << (2 * 2)))
		   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000003 << 4))
		   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000030))
		   GPIOx_PUPDR =  GPIOx_PUPDR & FFFFFFCF */
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		if ( input_cfg == uC_IO_IN_CFG_PULLUP )
		{
			/* Set the pin to pull up */
			*config_reg |= ( 0x000000001 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			/* Set the pin to HIGH */
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
		{
			/* Set the pin to pull down  */
			*config_reg |= ( 0x000000002 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			/* Set the pin to LOW */
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;

		}
		else
		{
			/* Do Nothing */
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Set_Pin_To_Output
 *  @brief      Configure the port pin to GPIO output mode and set it to push-pull
 *              and set speed to High speed
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              initial_state => State in which the output pin requires to be configured
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_IO_Config::Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	__IO UINT32* config_reg;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		/* Get the access to mode register */
		config_reg = &pio_ctrl->reg_ctrl->MODER;

		/* Clear/Mask the MODER register */
		/* Port number is multiplied by 2 and the value 3 is shifted that number of times
		   the value is inverted and ANDed with the MODER register*/
		/* Example:- pio_ctrl->pio_num = 2
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << (2 * 2)))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << 4))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000030))
		   GPIOx_MODER = GPIOx_MODER & FFFFFFCF */
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Set the port pin to output mode */
		*config_reg |= ( 0x00000001 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Get the access to output type register */
		config_reg = &pio_ctrl->reg_ctrl->OTYPER;

		/* Set the pin to push-pull */
		*config_reg &= ~( 0x00000001 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Get the access to output type register */
		config_reg = &pio_ctrl->reg_ctrl->OSPEEDR;

		/* Clear the pin speed configuration*/
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Set the port pin to High speed mode */
		*config_reg |= ( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			/* Set the port pin to high state */
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			/* Set the port pin to low state */
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Set_Pin_To_High_Z
 *  @brief      Configure the port pin to tristate
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *  @param[out] None
 *  @return     None
 ******************************************************************************/

void uC_IO_Config::Set_Pin_To_High_Z( uC_USER_IO_STRUCT const* pio_ctrl )
{
	Set_Pin_To_Input( pio_ctrl, uC_IO_IN_CFG_HIZ );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Enable_Periph_Output_Pin
 *  @brief      Configure the port pin to GPIO Alternate function mode and set it to push-pull
 *              and set speed to High speed and to set the type of Alternate function
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              initial_state => State in which the output pin requires to be configured
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_IO_Config::Enable_Periph_Output_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											 bool initial_state )
{
	__IO UINT32* config_reg;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		/* Function to set the type of Alternate function eg:- Timer or UART*/
		Set_Pin_Remap( pio_ctrl );

		/* Get the access to mode register */
		config_reg = &pio_ctrl->reg_ctrl->MODER;

		/* Clear/Mask the MODER register */
		/* Port number is multiplied by 2 and the value 3 is shifted that number of times
		   the value is inverted and ANDed with the MODER register*/
		/* Example:- pio_ctrl->pio_num = 2
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << (2 * 2)))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << 4))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000030))
		   GPIOx_MODER = GPIOx_MODER & FFFFFFCF */
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Set the port pin to Alternate Function mode */
		*config_reg |= ( 0x00000002 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Get the access to output type register */
		config_reg = &pio_ctrl->reg_ctrl->OTYPER;

		/* Set the pin to push-pull */
		*config_reg &= ~( 0x00000001 << ( pio_ctrl->pio_num ) );

		// *config_reg |=  (0x00000001 << (pio_ctrl->pio_num));

		/* Get the access to speed mode register */
		config_reg = &pio_ctrl->reg_ctrl->OSPEEDR;

		/* Clear the pin speed configuration*/
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		/* Set the port pin to High speed mode */
		// *config_reg |= (0x00000003 << (pio_ctrl->pio_num * MULT_BY_4));
		if ( initial_state == uC_IO_OUT_INIT_HIGH )
		{
			/* Set the port pin to high state */
			pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
		else
		{
			/* Set the port pin to low state */
			pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Enable_Periph_Input_Pin
 *  @brief      Configure the port pin to GPIO Analog mode and set it to pull up
 *              or pull down and to set the type of Alternate function
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              input_cfg => Provides information whether the pin is required to
 *                           be configured pull up or pulldown
 *  @param[out] None
 *  @return     None
 ******************************************************************************/

void uC_IO_Config::Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											uC_IO_INPUT_CFG_EN input_cfg )
{
	__IO UINT32* config_reg;

	if ( pio_ctrl->pio_num != IO_PIN_UNDEFINED )
	{
		/* Function to set the type of Alternate function eg:- Timer or UART*/
		Set_Pin_Remap( pio_ctrl );

		/* Get the access to mode register */
		config_reg = &pio_ctrl->reg_ctrl->MODER;

		/* Clear/Mask the MODER register */
		/* Port number is multiplied by 2 and the value 3 is shifted that number of times
		   the value is inverted and ANDed with the PUPDR register*/
		/* Example:- pio_ctrl->pio_num = 2
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << (2 * 2)))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000003 << 4))
		   GPIOx_MODER = GPIOx_MODER & (~(0x00000030))
		   GPIOx_MODER = GPIOx_MODER & FFFFFFCF */
		*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

		// If the port is an analog port then we do something different.
		if ( ( pio_ctrl->alternate_func & uC_IO_ALTERNATE_MAP_ANALOG ) == 0x08U )
		{

			/* Set the port pin to Analog Function mode */
			*config_reg |= ( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			/* Get the access to Pullup /pull down register */
			config_reg = &pio_ctrl->reg_ctrl->PUPDR;

			/* Clear/Mask the PUPDR register */
			/* Port number is multiplied by 2 and the value 3 is shifted that number of times
			   the value is inverted and ANDed with the PUPDR register*/
			/* Example:- pio_ctrl->pio_num = 2
			   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000003 << (2 * 2)))
			   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000003 << 4))
			   GPIOx_PUPDR = GPIOx_PUPDR & (~(0x00000030))
			   GPIOx_PUPDR =  GPIOx_PUPDR & FFFFFFCF
			 */
			*config_reg &= ~( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			if ( input_cfg == uC_IO_IN_CFG_PULLUP )
			{
				/* Set the pin to pull up */
				*config_reg |= ( 0x000000001 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

				/* Set the pin to HIGH */
				pio_ctrl->reg_ctrl->BSRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;

			}
			else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
			{
				/* Set the pin to pull down  */
				*config_reg |= ( 0x000000002 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

				/* Set the pin to LOW */
				pio_ctrl->reg_ctrl->BRR = static_cast<uint32_t>( 1U ) << pio_ctrl->pio_num;

			}
			else
			{
				/* Do nothing */
			}
		}
		else
		{
			/* Set the port pin to Alternate Function mode */
			*config_reg |= ( 0x00000002 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			/* Get the access to output type register */
			config_reg = &pio_ctrl->reg_ctrl->OSPEEDR;

			/* Clear the pin speed configuration*/
			*config_reg |= ( 0x00000003 << ( pio_ctrl->pio_num * MULT_BY_4 ) );

			/* Get the access to output type register */
			config_reg = &pio_ctrl->reg_ctrl->OTYPER;

			/* Set the pin to push-pull */
			*config_reg &= ~( 0x00000001 << ( pio_ctrl->pio_num ) );

			*config_reg |= ( 0x00000001 << ( pio_ctrl->pio_num ) );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Set_Pin_Remap
 *  @brief      Configure the port pin to Alterante mode
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_IO_Config::Set_Pin_Remap( uC_PERIPH_IO_STRUCT const* pio_ctrl )
{

	/* For eg :-
	   1) Alternate func Timer 1 channel 1 is required to be selected

	 |Pin name | AF0 | AF1       | AF2      | AF3      |
	 |---------|-----|-----------|----------|----------|
	 |PA8      | MCO | USART1_CK | TIM1_CH1 | EVENTOUT |

	   In uC_IO_Define_STM32F030.cpp configure for Timer peripheral
	   1) reg_ctrl  = GPIOA
	   2) pio_num   = IO_PIN_8
	   3) alternate_func = uC_IO_ALTERNATE_FUNC_2

	   GPIOx_AFRH = 0x0002


	   2) Alternate func Timer 3 channel 1 is required to be selected

	 |Pin name | AF0      | AF1       | AF2      | AF3      |
	 |---------|----------|-----------|----------|----------|
	 |PC6      | TIM3_CH1 | -         | -        | -        |

	   In uC_IO_Define_STM32F030.cpp configure for Timer peripheral
	   1) reg_ctrl  = GPIOC
	   2) pio_num   = IO_PIN_6
	   3) alternate_func = uC_IO_ALTERNATE_FUNC_0

	   GPIOx_AFRL = 0x0000

	 */
	/* Check port pin number */
	if ( pio_ctrl->pio_num < 8 )
	{
		/* Example 1 :-  Alternate Func Reg Low &= ~(0xF << ( 7 * 4))
		   AFR[0] = AFRL
		   Port is selected as per port configured and accordingly ports register is selected*/
		pio_ctrl->reg_ctrl->AFR[0] &=
			( ~( uC_IO_ALTERNATE_MAP_MASK << ( pio_ctrl->pio_num * uC_IO_ALT_SHIFT ) ) );

		/* Example 1 :- Alternate Func Reg Low = (0x00 << ( 7 * 4)) */
		pio_ctrl->reg_ctrl->AFR[0] |=
			( pio_ctrl->alternate_func << ( pio_ctrl->pio_num * uC_IO_ALT_SHIFT ) );

	}
	else
	{
		/* Example 2 :- Alternate Func Reg High &= ~(0xF << ( 8 * 4)
		   AFR[1] = AFRH*/
		/* Port is selected as per port(eg : PortA) configured and accordingly ports register is
		   selected */
		pio_ctrl->reg_ctrl->AFR[1] &=
			( ~( uC_IO_ALTERNATE_MAP_MASK << ( ( pio_ctrl->pio_num % 8 ) * uC_IO_ALT_SHIFT ) ) );

		/* Example 2 :-  Alternate Func Reg High = (0x02 << (0 * 4)) */
		pio_ctrl->reg_ctrl->AFR[1] |=
			( pio_ctrl->alternate_func << ( ( pio_ctrl->pio_num % 8 ) * uC_IO_ALT_SHIFT ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Disable_Periph_Pin_Set_To_Output
 *  @brief      Configure the port pin to GPIO output mode and set it to push-pull
 *              and set speed to High speed
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              initial_state => State in which the output pin requires to be configured
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_IO_Config::Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													 bool initial_state )
{
	uC_USER_IO_STRUCT user_io;

	/* Get the port i.e A or B or C*/
	user_io.reg_ctrl = pio_ctrl->reg_ctrl;

	/* Get the port pin number */
	user_io.pio_num = pio_ctrl->pio_num;

	/* Function to set the port pin to output mode */
	Set_Pin_To_Output( &user_io, initial_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Disable_Periph_Pin_Set_To_Input
 *  @brief      Configure the port pin to GPIO Input mode and set it to pull up
 *              or pull down
 *  @param[in]  pio_ctrl => Provides info to funtion port,pin and mode to be configured
 *              input_cfg => Provides information whether the pin is required to
 *                           be configured pull up or pulldown
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_IO_Config::Disable_Periph_Pin_Set_To_Input( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													uC_IO_INPUT_CFG_EN input_cfg )
{
	uC_USER_IO_STRUCT user_io;

	/* Get the port i.e A or B or C*/
	user_io.reg_ctrl = pio_ctrl->reg_ctrl;

	/* Get the port pin number */
	user_io.pio_num = pio_ctrl->pio_num;

	/* Function to set the port pin to input mode */
	Set_Pin_To_Input( &user_io, input_cfg );
}
