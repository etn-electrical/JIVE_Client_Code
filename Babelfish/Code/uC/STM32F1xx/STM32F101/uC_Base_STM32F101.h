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
#ifndef uC_Base_STM32F101_H
   #define uC_Base_STM32F101_H

#include "uC_Base.h"


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Base_STM32F101 : public uC_Base
{
	public:
		uC_Base_STM32F101( void );
		~uC_Base_STM32F101( void );

		uC_BASE_WDOG_CTRL_STRUCT const* Get_WDOG_Ctrl( uint8_t block );

		uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( uint32_t io_block );

		uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( uint32_t timer );

		uC_BASE_USART_IO_STRUCT const* Get_USART_Ctrl( uint32_t port );

		uC_BASE_SPI_IO_STRUCT const* Get_SPI_Ctrl( uint32_t spi_port );

		uC_BASE_EXT_INT_IO_STRUCT const* Get_Ext_Int_Ctrl( uC_USER_IO_STRUCT const* pio_ctrl );

		uC_BASE_TIMER_IO_STRUCT const* Get_PWM_Ctrl( uint32_t pwm_block );

		uC_BASE_ATOD_IO_STRUCT const* Get_AtoD_Ctrl( uint32_t atod_block );

		uC_BASE_DMA_IO_STRUCT const* Get_DMA_Ctrl( uint32_t dma_channel );

		uC_BASE_EMAC_IO_STRUCT const* Get_EMAC_Ctrl( uint8_t emac_block );

		uint32_t Get_PClock_Freq( uint8_t pclock_selection );

		// uC_BASE_TWI_IO_STRUCT const*		Get_TWI_Ctrl( UINT32 twi_block );
		// uC_BASE_CAN_IO_STRUCT const*		Get_CAN_Ctrl( UINT32 can_block );
		// uC_BASE_EXT_MEM_IO_STRUCT const*	Get_Ext_Mem_Ctrl( void )			{ return ( NULL ); }

		static void Get_Device_Sig( uint8_t* data, uint8_t length = 12 )
		{
			uint8_t* device_sig_ptr;

			device_sig_ptr = ( uint8_t* )0x1FFFF7E8;

			for ( uint_fast8_t i = 0; ( ( i < length ) && ( i < 12 ) ); i++ ){ data[i] = device_sig_ptr[i]; }
		}

	private:
		static const uC_BASE_PIO_CTRL_STRUCT m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];
		static const uC_BASE_TIMER_IO_STRUCT m_timer_io[uC_BASE_MAX_TIMER_CTRLS];
		static const uC_BASE_USART_IO_STRUCT m_usart_io[uC_BASE_MAX_USART_PORTS];
		static const uC_BASE_SPI_IO_STRUCT m_spi_io[uC_BASE_SPI_MAX_CTRLS];
		static const uC_BASE_EXT_INT_IO_STRUCT m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS];
		static const uC_BASE_ATOD_IO_STRUCT m_atod_io[uC_BASE_ATOD_MAX_CTRLS];
		static const uC_BASE_DMA_IO_STRUCT m_dma_io[uC_BASE_DMA_CHANNEL_MAX];

		static const uint32_t m_pclock_freqs[uC_BASE_STM32F101_MAX_PCLKS];
};


#endif
