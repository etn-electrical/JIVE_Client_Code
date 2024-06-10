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
#include "uC_Base_STM32F373.h"
//
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
uC_Base_STM32F373::uC_Base_STM32F373( void ) : uC_Base()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_WDOG_CTRL_STRUCT const* uC_Base_STM32F373::Get_WDOG_Ctrl( uint8_t block )
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
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F373::Get_Timer_Ctrl( uint32_t timer )
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
uC_BASE_PIO_CTRL_STRUCT const* uC_Base_STM32F373::Get_PIO_Ctrl( uint32_t io_block )
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
uC_BASE_USART_IO_STRUCT const* uC_Base_STM32F373::Get_USART_Ctrl( uint32_t port )
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
uC_BASE_SPI_IO_STRUCT const* uC_Base_STM32F373::Get_SPI_Ctrl( uint32_t spi_block )
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
uC_BASE_EXT_INT_IO_STRUCT const* uC_Base_STM32F373::Get_Ext_Int_Ctrl(
	uC_USER_IO_STRUCT const* pio_ctrl )
{
	uC_BASE_EXT_INT_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_EXT_INT_IO_STRUCT const*>( nullptr );
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
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F373::Get_PWM_Ctrl( uint32_t pwm_block )
{
	return ( Get_Timer_Ctrl( pwm_block ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_ATOD_IO_STRUCT const* uC_Base_STM32F373::Get_AtoD_Ctrl( uint32_t atod_block )
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
uC_BASE_DMA_IO_STRUCT const* uC_Base_STM32F373::Get_DMA_Ctrl( uint32_t dma_channel )
{
	uC_BASE_DMA_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr );

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
// uC_BASE_EMAC_IO_STRUCT const*		uC_Base_STM32F373::Get_EMAC_Ctrl( uint8_t emac_block )
// {
////	uC_BASE_EMAC_IO_STRUCT const* return_ctrl = 0;
////
////	if ( emac_block < uC_BASE_EMAC_MAX_CTRLS )
////	{
////		return_ctrl = &m_emac_io[emac_block];
////	}
//
// return ( NULL );
//
// }


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base_STM32F373::Get_PClock_Freq( uint8_t pclock_selection )
{
	return ( m_pclock_freqs[pclock_selection] );
}

/* lint +e1924
  lint +e1960*/
