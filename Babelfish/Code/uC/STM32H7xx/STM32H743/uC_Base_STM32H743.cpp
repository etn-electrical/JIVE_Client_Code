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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_Base_STM32H743.h"
#include "uC_Watchdog_HW.h"

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
uC_Base_STM32H743::uC_Base_STM32H743( void ) : uC_Base()
{
	/* Update SystemCoreClock variable according to Clock Register Values. */
	SystemCoreClockUpdate();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_WDOG_CTRL_STRUCT const* uC_Base_STM32H743::Get_WDOG_Ctrl( uint8_t block )
{
	return ( reinterpret_cast<uC_BASE_WDOG_CTRL_STRUCT const*>( nullptr ) );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'block' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32H743::Get_Timer_Ctrl( uint32_t timer )
{
	uC_BASE_TIMER_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr );

	if ( timer < uC_BASE_MAX_TIMER_CTRLS )
	{
		return_ctrl = &m_timer_io[timer];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_PIO_CTRL_STRUCT const* uC_Base_STM32H743::Get_PIO_Ctrl( uint32_t io_block )
{
	uC_BASE_PIO_CTRL_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_PIO_CTRL_STRUCT const*>( nullptr );

	if ( io_block < uC_BASE_MAX_PIO_CTRLS )
	{
		return_ctrl = &m_pio_ctrls[io_block];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_USB_IO_STRUCT const* uC_Base_STM32H743::Get_USB_Ctrl( uint8_t usb_channel )
{
	uC_BASE_USB_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_USB_IO_STRUCT const*>( nullptr );

	if ( usb_channel < uC_BASE_MAX_USB_PORTS )
	{
		return_ctrl = &m_usb_io[usb_channel];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_USART_IO_STRUCT const* uC_Base_STM32H743::Get_USART_Ctrl( uint32_t port )
{
	uC_BASE_USART_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr );

	if ( port < uC_BASE_MAX_USART_PORTS )
	{
		return_ctrl = &m_usart_io[port];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_SPI_IO_STRUCT const* uC_Base_STM32H743::Get_SPI_Ctrl( uint32_t spi_block )
{
	uC_BASE_SPI_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_SPI_IO_STRUCT const*>( nullptr );

	if ( spi_block < uC_BASE_SPI_MAX_CTRLS )
	{
		return_ctrl = &m_spi_io[spi_block];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_I2C_IO_STRUCT const* uC_Base_STM32H743::Get_I2C_Ctrl( uint32_t i2c_block )
{
	uC_BASE_I2C_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_I2C_IO_STRUCT const*>( nullptr );

	if ( i2c_block < uC_BASE_I2C_MAX_CTRLS )
	{
		return_ctrl = &m_i2c_io[i2c_block];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// uC_BASE_EXT_INT_IO_STRUCT const*    uC_Base_STM32H743::Get_Ext_Int_Ctrl(
// uC_USER_IO_STRUCT const* pio_ctrl )
// {
// uC_BASE_EXT_INT_IO_STRUCT const* return_ctrl =
// reinterpret_cast<uC_BASE_EXT_INT_IO_STRUCT const*>( nullptr );
// uint_fast8_t i = 0U;
//
// while ( ( i < uC_BASE_EXT_INT_MAX_INPUTS ) &&
// ( ( ( 1U << pio_ctrl->pio_num ) & m_ext_ints[i].irq_assignment ) == 0U ) )
// {
// i++;
// }
//
// if ( i < uC_BASE_EXT_INT_MAX_INPUTS )
// {
// return_ctrl = &m_ext_ints[i];
// }
//
// return ( return_ctrl );
// }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// uC_BASE_TIMER_IO_STRUCT const*  uC_Base_STM32H743::Get_PWM_Ctrl( uint32_t pwm_block )
// {
// return ( Get_Timer_Ctrl( pwm_block ) );
// }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_ATOD_IO_STRUCT const* uC_Base_STM32H743::Get_AtoD_Ctrl( uint32_t atod_block )
{
	uC_BASE_ATOD_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_ATOD_IO_STRUCT const*>( nullptr );

	if ( atod_block < uC_BASE_ATOD_MAX_CTRLS )
	{
		return_ctrl = &m_atod_io[atod_block];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_DMA_IO_STRUCT const* uC_Base_STM32H743::Get_DMA_Ctrl( uint32_t dma_channel )
{
	uC_BASE_DMA_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr );

	if ( dma_channel < uC_BASE_DMA_STREAM_MAX )
	{
		return_ctrl = &m_dma_io[dma_channel];
	}

	return ( return_ctrl );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_EMAC_IO_STRUCT const* uC_Base_STM32H743::Get_EMAC_Ctrl( uint8_t emac_block )
{
	uC_BASE_EMAC_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_EMAC_IO_STRUCT const*>( nullptr );

	if ( emac_block < uC_BASE_EMAC_MAX_CTRLS )
	{
		return_ctrl = &m_emac_io[emac_block];
	}

	return ( return_ctrl );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base_STM32H743::Get_PClock_Freq( uint8_t pclock_selection )
{
	return ( m_pclock_freqs[pclock_selection] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// #define uC_BASE_OPT_KEY_1       0x08192A3BU //keys to unlock the option bytes.
// #define uC_BASE_OPT_KEY_2       0x4C5D6E7FU //keys to unlock the option bytes.
// #define uC_BASE_OPT_LEVEL0      0xAA00U     //Value to store the level 0 full JTAG access.
// #define uC_BASE_OPT_FULL_ACCESS 0x00U       //0x0FFF0000	//This tells the device to provide read
////acces to all locations.
// #define OPTCR_BYTE0_ADDRESS         ( ( uint32_t )0x40023C14 )
// #define OPTCR_BYTE1_ADDRESS         ( ( uint32_t )0x40023C15 )
// #define OPTCR_BYTE2_ADDRESS         ( ( uint32_t )0x40023C16 )
// bool uC_Base_STM32H743::Set_Option_Bytes( BROWN_OUT_OPT_SELECT_EN brown_out_sel,
// WATCHDOG_OPT_SELECT_EN watchdog_sel )
// {
// bool changed = false;
// uint8_t target_opt;
// uint8_t current_opt;
// uint8_t volatile* flash_ctrl_ptr;
//
// flash_ctrl_ptr = ( uint8_t volatile* )OPTCR_BYTE0_ADDRESS;
// target_opt = ( brown_out_sel << 2U ) | ( watchdog_sel << 5U );
// current_opt = *flash_ctrl_ptr & ( FLASH_OPTCR_BOR_LEV | FLASH_OPTCR_WDG_SW );
// if ( current_opt != target_opt )
// {
// uC_Watchdog_HW::Kick_Dog(); //Just in case this takes some time.
// Push_TGINT();
// while ( ( FLASH->SR & FLASH_SR_BSY ) != 0U )
// {}
//
// FLASH->OPTKEYR = uC_BASE_OPT_KEY_1;
// FLASH->OPTKEYR = uC_BASE_OPT_KEY_2;
////while ( ( FLASH->SR & FLASH_SR_BSY ) != 0U ){};
//
// current_opt = *flash_ctrl_ptr & ~( FLASH_OPTCR_BOR_LEV | FLASH_OPTCR_WDG_SW );
//
// *flash_ctrl_ptr = target_opt | current_opt; //| current_opt;
////while ( ( FLASH->SR & FLASH_SR_BSY ) != 0U ){};
//
// *flash_ctrl_ptr |= FLASH_OPTCR_OPTSTRT;
// while ( ( FLASH->SR & FLASH_SR_BSY ) != 0U )
// {}
//
// *flash_ctrl_ptr |= FLASH_OPTCR_OPTLOCK;
////while ( ( FLASH->SR & FLASH_SR_BSY ) != 0U ){};
//
// changed = true;
//
// Pop_TGINT();
// }
//
// return ( changed );
// }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// uC_BASE_CAN_IO_STRUCT const*        uC_Base_STM32H743::Get_CAN_Ctrl( uint32_t can_block )
// {
// uC_BASE_CAN_IO_STRUCT const* return_ctrl = 0U;
//
// if ( can_block < uC_BASE_CAN_MAX_CTRLS )
// {
// return_ctrl = &m_can_io[can_block];
// }
//
// return ( return_ctrl );
//
// }

#if 0

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// uC_BASE_TWI_IO_STRUCT const*        uC_Base_STM32H743::Get_TWI_Ctrl( uint32_t twi_block )
// {
// uC_BASE_TWI_IO_STRUCT const* return_ctrl = 0U;
//
// if ( twi_block < uC_BASE_TWI_MAX_CTRLS )
// {
// return_ctrl = &m_twi_io[twi_block];
// }
//
// return ( return_ctrl );
//
// }

#endif

/* lint +e1924
   lint +e1960*/
