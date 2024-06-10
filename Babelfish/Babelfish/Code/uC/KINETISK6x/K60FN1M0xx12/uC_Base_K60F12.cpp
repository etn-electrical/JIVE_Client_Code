/*
 *****************************************************************************************
 *
 *		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "uC_Base_K60F12.h"
#include "uC_Watchdog_HW.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_Base_K60F12::uC_Base_K60F12( void ) : uC_Base()
{}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_WDOG_CTRL_STRUCT const* uC_Base_K60F12::Get_WDOG_Ctrl( uint8_t block )
{
	return ( NULL );
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
uC_BASE_TIMER_IO_STRUCT const* uC_Base_K60F12::Get_Timer_Ctrl( uint32_t timer )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_LP_TIMER_CTRL_STRUCT const* uC_Base_K60F12::Get_LP_Timer_Ctrl( uint32_t timer )
{
	uC_BASE_LP_TIMER_CTRL_STRUCT const* return_ctrl = NULL;

	if ( timer < uC_BASE_MAX_LP_TIMER_CTRLS )
	{
		return_ctrl = &m_lp_timer_ctrls[timer];
	}

	return ( return_ctrl );
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
uC_BASE_PIO_CTRL_STRUCT const* uC_Base_K60F12::Get_PIO_Ctrl( uint32_t io_block )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_USART_IO_STRUCT const* uC_Base_K60F12::Get_USART_Ctrl( uint32_t port )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_SPI_IO_STRUCT const* uC_Base_K60F12::Get_SPI_Ctrl( uint32_t spi_block )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
// uC_BASE_I2C_IO_STRUCT const*	uC_Base_K60F12::Get_I2C_Ctrl( uint32_t i2c_block )
// {
// uC_BASE_I2C_IO_STRUCT const* return_ctrl = NULL;
//
// if ( i2c_block < uC_BASE_I2C_MAX_CTRLS )
// {
// return_ctrl = &m_i2c_io[i2c_block];
// }
//
// return ( return_ctrl );
// }

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_EXT_INT_IO_STRUCT const* uC_Base_K60F12::Get_Ext_Int_Ctrl( uC_USER_IO_STRUCT const* pio_ctrl )
{
	uC_BASE_EXT_INT_IO_STRUCT const* return_ctrl = NULL;

	if ( pio_ctrl->port_index < uC_BASE_EXT_INT_MAX_PORTS )
	{
		return_ctrl = &m_ext_ints[pio_ctrl->port_index];
	}

	return ( return_ctrl );
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
// uC_BASE_TIMER_IO_STRUCT const*	uC_Base_K60F12::Get_PWM_Ctrl( uint32_t pwm_block )
// {
// return ( Get_Timer_Ctrl( pwm_block ) );
// }


/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
// uC_BASE_ATOD_IO_STRUCT const*	uC_Base_K60F12::Get_AtoD_Ctrl( uint32_t atod_block )
// {
// uC_BASE_ATOD_IO_STRUCT const* return_ctrl = NULL;
//
// if ( atod_block < uC_BASE_ATOD_MAX_CTRLS )
// {
// return_ctrl = &m_atod_io[atod_block];
// }
//
// return ( return_ctrl );
// }

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_DMA_IO_STRUCT const* uC_Base_K60F12::Get_DMA_Ctrl( uint32_t dma_channel )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_EMAC_IO_STRUCT const* uC_Base_K60F12::Get_EMAC_Ctrl( uint8_t emac_block )
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint32_t uC_Base_K60F12::Get_PClock_Freq( uint8_t pclock_selection )
{
	return ( m_pclock_freqs[pclock_selection] );
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
// #define uC_BASE_OPT_KEY_1		0x08192A3B	//keys to unlock the option bytes.
// #define uC_BASE_OPT_KEY_2		0x4C5D6E7F	//keys to unlock the option bytes.
// #define uC_BASE_OPT_LEVEL0		0xAA00		//Value to store the level 0 full JTAG access.
// #define uC_BASE_OPT_FULL_ACCESS 0x00		//0x0FFF0000	//This tells the device to provide read acces to all
// locations.
// #define OPTCR_BYTE0_ADDRESS         ((uint32_t_t)0x40023C14)
// #define OPTCR_BYTE1_ADDRESS         ((uint32_t_t)0x40023C15)
// #define OPTCR_BYTE2_ADDRESS         ((uint32_t_t)0x40023C16)
// BOOL uC_Base_K60F12::Set_Option_Bytes( BROWN_OUT_OPT_SELECT_EN brown_out_sel,
// WATCHDOG_OPT_SELECT_EN watchdog_sel )
// {
// BOOL changed = FALSE;
// uint8_t target_opt;
// uint8_t current_opt;
// uint8_t volatile* flash_ctrl_ptr;
//
// flash_ctrl_ptr = (uint8_t volatile*)OPTCR_BYTE0_ADDRESS;
// target_opt = ( brown_out_sel<<2 ) | ( watchdog_sel<<5 );
// current_opt = *flash_ctrl_ptr & ( FLASH_OPTCR_BOR_LEV | FLASH_OPTCR_WDG_SW );
// if ( current_opt != target_opt )
// {
// uC_Watchdog_HW::Kick_Dog();	//Just in case this takes some time.
// Push_TGINT();
// while ( ( FLASH->SR & FLASH_SR_BSY ) != 0 ){};
//
// FLASH->OPTKEYR = uC_BASE_OPT_KEY_1;
// FLASH->OPTKEYR = uC_BASE_OPT_KEY_2;
////		while ( ( FLASH->SR & FLASH_SR_BSY ) != 0 ){};
//
// current_opt = *flash_ctrl_ptr & ~( FLASH_OPTCR_BOR_LEV | FLASH_OPTCR_WDG_SW );
//
// *flash_ctrl_ptr = target_opt | current_opt;	// | current_opt;
////		while ( ( FLASH->SR & FLASH_SR_BSY ) != 0 ){};
//
// *flash_ctrl_ptr |= FLASH_OPTCR_OPTSTRT;
// while ( ( FLASH->SR & FLASH_SR_BSY ) != 0 ){};
//
// *flash_ctrl_ptr |= FLASH_OPTCR_OPTLOCK;
////		while ( ( FLASH->SR & FLASH_SR_BSY ) != 0 ){};
//
// changed = TRUE;
//
// Pop_TGINT();
// }
//
// return ( changed );
// }

#if 0





/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uC_BASE_TWI_IO_STRUCT const* uC_Base_K60F12::Get_TWI_Ctrl( uint32_t twi_block )
{
	uC_BASE_TWI_IO_STRUCT const* return_ctrl = 0;

	if ( twi_block < uC_BASE_TWI_MAX_CTRLS )
	{
		return_ctrl = &m_twi_io[twi_block];
	}

	return ( return_ctrl );

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
uC_BASE_CAN_IO_STRUCT const* uC_Base_K60F12::Get_CAN_Ctrl( uint32_t can_block )
{
	uC_BASE_CAN_IO_STRUCT const* return_ctrl = 0;

	if ( can_block < uC_BASE_CAN_MAX_CTRLS )
	{
		return_ctrl = &m_can_io[can_block];
	}

	return ( return_ctrl );

}

#endif
