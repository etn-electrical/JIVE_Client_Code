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
#include "uC_Base_STM32F205.h"
#include "uC_Watchdog_HW.h"
#include "string.h"
#include "Babelfish_Assert.h"

const FLASH_OBProgramInitTypeDef uC_Base_STM32F205::DEFAULT_DEBUG_OPTION_BYTES =
{
	( OPTIONBYTE_WRP | OPTIONBYTE_RDP | OPTIONBYTE_USER | OPTIONBYTE_BOR ),
	0U,
	0x00000FFFU,
	OB_RDP_LEVEL_0,
	OB_BOR_LEVEL3,
	( OB_IWDG_SW | OB_STOP_NO_RST | OB_STDBY_NO_RST )
};

const FLASH_OBProgramInitTypeDef uC_Base_STM32F205::DEFAULT_RELEASE_OPTION_BYTES =
{
	( OPTIONBYTE_WRP | OPTIONBYTE_RDP | OPTIONBYTE_USER | OPTIONBYTE_BOR ),
	0U,
	0x00000FFFU,
	OB_RDP_LEVEL_1,
	OB_BOR_LEVEL3,
	( OB_IWDG_SW | OB_STOP_NO_RST | OB_STDBY_NO_RST )
};

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
uC_Base_STM32F205::uC_Base_STM32F205( void ) : uC_Base()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_WDOG_CTRL_STRUCT const* uC_Base_STM32F205::Get_WDOG_Ctrl( uint8_t block )
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
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F205::Get_Timer_Ctrl( uint32_t timer )
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
uC_BASE_PIO_CTRL_STRUCT const* uC_Base_STM32F205::Get_PIO_Ctrl( uint32_t io_block )
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
uC_BASE_USB_IO_STRUCT const* uC_Base_STM32F205::Get_USB_Ctrl( uint8_t usb_channel )
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
uC_BASE_USART_IO_STRUCT const* uC_Base_STM32F205::Get_USART_Ctrl( uint32_t port )
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
uC_BASE_SPI_IO_STRUCT const* uC_Base_STM32F205::Get_SPI_Ctrl( uint32_t spi_block )
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
uC_BASE_I2C_IO_STRUCT const* uC_Base_STM32F205::Get_I2C_Ctrl( uint32_t i2c_block )
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
uC_BASE_EXT_INT_IO_STRUCT const* uC_Base_STM32F205::Get_Ext_Int_Ctrl(
	uC_USER_IO_STRUCT const* pio_ctrl )
{
	uC_BASE_EXT_INT_IO_STRUCT const* return_ctrl =
		reinterpret_cast<uC_BASE_EXT_INT_IO_STRUCT const*>( nullptr );
	uint_fast8_t i = 0U;

	while ( ( i < uC_BASE_EXT_INT_MAX_INPUTS ) &&
			( ( ( 1 << pio_ctrl->pio_num ) & m_ext_ints[i].irq_assignment ) == 0U ) )
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
uC_BASE_TIMER_IO_STRUCT const* uC_Base_STM32F205::Get_PWM_Ctrl( uint32_t pwm_block )
{
	return ( Get_Timer_Ctrl( pwm_block ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_ATOD_IO_STRUCT const* uC_Base_STM32F205::Get_AtoD_Ctrl( uint32_t atod_block )
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
uC_BASE_DMA_IO_STRUCT const* uC_Base_STM32F205::Get_DMA_Ctrl( uint32_t dma_channel )
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
uC_BASE_EMAC_IO_STRUCT const* uC_Base_STM32F205::Get_EMAC_Ctrl( uint8_t emac_block )
{
	return ( reinterpret_cast<uC_BASE_EMAC_IO_STRUCT const*>( nullptr ) );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'emac_block' not referenced
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
uint32_t uC_Base_STM32F205::Get_PClock_Freq( uint8_t pclock_selection )
{
	return ( m_pclock_freqs[pclock_selection] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

bool uC_Base_STM32F205::Set_Option_Bytes( FLASH_OBProgramInitTypeDef const* optionBytes )
{
	HAL_StatusTypeDef result;
	bool changed = false;
	FLASH_OBProgramInitTypeDef currentOptions;

	memset( &currentOptions, 0, sizeof( FLASH_OBProgramInitTypeDef ) );
	HAL_FLASHEx_OBGetConfig( &currentOptions );
	currentOptions.RDPLevel = ( currentOptions.RDPLevel == 0 ) ? OB_RDP_LEVEL_0 : ~OB_RDP_LEVEL_0;

	if ( memcmp( optionBytes, &currentOptions, sizeof( FLASH_OBProgramInitTypeDef ) ) != 0 )
	{
		uC_Watchdog_HW::Kick_Dog();	// Just in case this takes some time.
		Push_TGINT();

		result = HAL_FLASH_OB_Unlock();
		BF_ASSERT( result == HAL_OK );

		result =
			HAL_FLASHEx_OBProgram( const_cast<FLASH_OBProgramInitTypeDef*>( optionBytes ) );
		BF_ASSERT( result == HAL_OK );

		result = HAL_FLASH_OB_Launch();
		BF_ASSERT( result == HAL_OK );

		result = HAL_FLASH_OB_Lock();
		BF_ASSERT( result == HAL_OK );

		changed = true;

		Pop_TGINT();
	}

	return ( changed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_CAN_IO_STRUCT const* uC_Base_STM32F205::Get_CAN_Ctrl( uint32_t can_block )
{
	uC_BASE_CAN_IO_STRUCT const* return_ctrl = 0U;

	if ( can_block < uC_BASE_CAN_MAX_CTRLS )
	{
		return_ctrl = &m_can_io[can_block];
	}

	return ( return_ctrl );

}

#if 0





/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_TWI_IO_STRUCT const* uC_Base_STM32F205::Get_TWI_Ctrl( uint32_t twi_block )
{
	uC_BASE_TWI_IO_STRUCT const* return_ctrl = 0U;

	if ( twi_block < uC_BASE_TWI_MAX_CTRLS )
	{
		return_ctrl = &m_twi_io[twi_block];
	}

	return ( return_ctrl );

}

#endif


/* lint +e1924
   lint +e1960*/
