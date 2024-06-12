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
#ifndef uC_Base_STM32F767_H
   #define uC_Base_STM32F767_H

#include "uC_Base.h"

/*
 **************************************************************************************************
 *  Constants
 **************************************************************************************************
 */
typedef enum BROWN_OUT_OPT_SELECT_EN
{
	BROWN_OUT_OPT_SEL_2p7,
	BROWN_OUT_OPT_SEL_2p4,
	BROWN_OUT_OPT_SEL_2p1,
	BROWN_OUT_OPT_SEL_1p8
} BROWN_OUT_SELECT_EN;
typedef enum WATCHDOG_OPT_SELECT_EN
{
	WATCHDOG_OPT_SEL_HW,
	WATCHDOG_OPT_SEL_SW
} WATCHDOG_OPT_SELECT_EN;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Base_STM32F767 : public uC_Base
{
	public:
		uC_Base_STM32F767( void );
		~uC_Base_STM32F767( void );

		uC_BASE_WDOG_CTRL_STRUCT const* Get_WDOG_Ctrl( uint8_t block );

		uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( uint32_t io_block );

		uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( uint32_t timer );

		uC_BASE_USART_IO_STRUCT const* Get_USART_Ctrl( uint32_t port );

		uC_BASE_SPI_IO_STRUCT const* Get_SPI_Ctrl( uint32_t spi_port );

		uC_BASE_FMC_SDRAM_IO_STRUCT const* Get_FMC_SDRAM_Ctrl( uint32_t fmc_sdram_block );	/*Only tested on F750 board
																							   to support External RAM*/

		uC_BASE_I2C_IO_STRUCT const* Get_I2C_Ctrl( uint32_t i2c_port );

		uC_BASE_EXT_INT_IO_STRUCT const* Get_Ext_Int_Ctrl( uC_USER_IO_STRUCT const* pio_ctrl );

		uC_BASE_TIMER_IO_STRUCT const* Get_PWM_Ctrl( uint32_t pwm_block );

		uC_BASE_ATOD_IO_STRUCT const* Get_AtoD_Ctrl( uint32_t atod_block );

		uC_BASE_DMA_IO_STRUCT const* Get_DMA_Ctrl( uint32_t dma_channel );

		uC_BASE_EMAC_IO_STRUCT const* Get_EMAC_Ctrl( uint8_t emac_block );

		uC_BASE_USB_IO_STRUCT const* Get_USB_Ctrl( uint8_t usb_channel );

		uint32_t Get_PClock_Freq( uint8_t pclock_selection );

		uC_BASE_CAN_IO_STRUCT const* Get_CAN_Ctrl( UINT32 can_block );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Get_Device_Sig( uint8_t* data, uint8_t length = 12 )
		{
			uint8_t* device_sig_ptr;

			device_sig_ptr = ( uint8_t* )0x1FFF7A10;

			for ( uint_fast8_t i = 0; ( ( i < length ) && ( i < 12 ) ); i++ )
			{
				data[i] = device_sig_ptr[i];
			}
		}

		static bool Set_Option_Bytes( BROWN_OUT_OPT_SELECT_EN brown_out_sel = BROWN_OUT_OPT_SEL_2p7,
									  WATCHDOG_OPT_SELECT_EN watchdog_sel = WATCHDOG_OPT_SEL_SW );

	private:
		static const uC_BASE_PIO_CTRL_STRUCT m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];
		static const uC_BASE_TIMER_IO_STRUCT m_timer_io[uC_BASE_MAX_TIMER_CTRLS];
		static const uC_BASE_USART_IO_STRUCT m_usart_io[uC_BASE_MAX_USART_PORTS];
		static const uC_BASE_USB_IO_STRUCT m_usb_io[uC_BASE_MAX_USB_PORTS];
		static const uC_BASE_SPI_IO_STRUCT m_spi_io[uC_BASE_SPI_MAX_CTRLS];
		static const uC_BASE_FMC_SDRAM_IO_STRUCT m_fmc_sdram_io[uC_BASE_FMC_SDRAM_MAX_CTRLS];
		static const uC_BASE_I2C_IO_STRUCT m_i2c_io[uC_BASE_I2C_MAX_CTRLS];
		static const uC_BASE_EXT_INT_IO_STRUCT m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS];
		static const uC_BASE_ATOD_IO_STRUCT m_atod_io[uC_BASE_ATOD_MAX_CTRLS];
		static const uC_BASE_DMA_IO_STRUCT m_dma_io[uC_BASE_DMA_STREAM_MAX];
		static const uC_BASE_EMAC_IO_STRUCT m_emac_io[uC_BASE_EMAC_MAX_CTRLS];

		static const uint32_t m_pclock_freqs[uC_BASE_STM32F767_MAX_PCLKS];


		/// I2C Interfaces.
		// Stores the function pointer of the objects static interrupt handler.
		static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];

		// Stores the function pointer of the objects static interrupt handler.
		static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

		// Stores the function pointer of the objects static interrupt handler.
		static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];
		// static const uC_BASE_CAN_IO_STRUCT m_can_io[uC_BASE_CAN_MAX_CTRLS];


		static void uC_BASE_I2C_DMA_INT1_Tx( void );

		static void uC_BASE_I2C_DMA_INT2_Tx( void );

		static void uC_BASE_I2C_DMA_INT3_Tx( void );

		static void uC_BASE_I2C_DMA_INT4_Tx( void );

		static void uC_BASE_I2C_DMA_INT1_Rx( void );

		static void uC_BASE_I2C_DMA_INT2_Rx( void );

		static void uC_BASE_I2C_DMA_INT3_Rx( void );

		static void uC_BASE_I2C_DMA_INT4_Rx( void );

		static void uC_BASE_I2C_INT1( void );

		static void uC_BASE_I2C_INT2( void );

		static void uC_BASE_I2C_INT3( void );

		static void uC_BASE_I2C_INT4( void );

};


#endif
