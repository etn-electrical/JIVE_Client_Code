/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Base_STM32F107.h"

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
uC_Base_STM32F107::uC_Base_STM32F107( void ) : uC_Base()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_WDOG_CTRL_STRUCT const* uC_Base_STM32F107::Get_WDOG_Ctrl( uint8_t block )
{
	return ( NULL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F107::Get_Timer_Ctrl( uint32_t timer )
{
	uC_BASE_TIMER_IO_STRUCT const* return_ctrl = NULL;

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
uC_BASE_PIO_CTRL_STRUCT const* uC_Base_STM32F107::Get_PIO_Ctrl( uint32_t io_block )
{
	uC_BASE_PIO_CTRL_STRUCT const* return_ctrl = NULL;

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
uC_BASE_USB_IO_STRUCT const* uC_Base_STM32F107::Get_USB_Ctrl( uint8_t usb_channel )
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
uC_BASE_USART_IO_STRUCT const* uC_Base_STM32F107::Get_USART_Ctrl( uint32_t port )
{
	uC_BASE_USART_IO_STRUCT const* return_ctrl = NULL;

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
uC_BASE_SPI_IO_STRUCT const* uC_Base_STM32F107::Get_SPI_Ctrl( uint32_t spi_block )
{
	uC_BASE_SPI_IO_STRUCT const* return_ctrl = NULL;

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
uC_BASE_I2C_IO_STRUCT const* uC_Base_STM32F107::Get_I2C_Ctrl( uint32_t i2c_block )
{
	uC_BASE_I2C_IO_STRUCT const* return_ctrl = NULL;

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
uC_BASE_EXT_INT_IO_STRUCT const* uC_Base_STM32F107::Get_Ext_Int_Ctrl(
	uC_USER_IO_STRUCT const* pio_ctrl )
{
	uC_BASE_EXT_INT_IO_STRUCT const* return_ctrl = NULL;
	uint_fast8_t i = 0;

	while ( ( i < uC_BASE_EXT_INT_MAX_INPUTS ) &&
			( ( ( 1 << pio_ctrl->pio_num ) & m_ext_ints[i].irq_assignment ) == 0 ) )
	{
		i++;
	}

	if ( i < uC_BASE_EXT_INT_MAX_INPUTS )
	{
		return_ctrl = &m_ext_ints[i];
	}

	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F107::Get_PWM_Ctrl( uint32_t pwm_block )
{
	return ( Get_Timer_Ctrl( pwm_block ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_ATOD_IO_STRUCT const* uC_Base_STM32F107::Get_AtoD_Ctrl( uint32_t atod_block )
{
	uC_BASE_ATOD_IO_STRUCT const* return_ctrl = NULL;

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
uC_BASE_DMA_IO_STRUCT const* uC_Base_STM32F107::Get_DMA_Ctrl( uint32_t dma_channel )
{
	uC_BASE_DMA_IO_STRUCT const* return_ctrl = NULL;

	if ( dma_channel < uC_BASE_DMA_CHANNEL_MAX )
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
uC_BASE_EMAC_IO_STRUCT const* uC_Base_STM32F107::Get_EMAC_Ctrl( uint8_t emac_block )
{
	uC_BASE_EMAC_IO_STRUCT const* return_ctrl = 0;

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
uint32_t uC_Base_STM32F107::Get_PClock_Freq( uint8_t pclock_selection )
{
	return ( m_pclock_freqs[pclock_selection] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_CAN_IO_STRUCT const* uC_Base_STM32F107::Get_CAN_Ctrl( uint32_t can_block )
{
	uC_BASE_CAN_IO_STRUCT const* return_ctrl = 0;

	if ( can_block < uC_BASE_CAN_MAX_CTRLS )
	{
		return_ctrl = &m_can_io[can_block];
	}

	return ( return_ctrl );

}
