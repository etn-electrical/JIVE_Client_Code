/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
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
#include "uC_Base_STM32F439.h"
#include "uC_IO_Define_STM32F439.h"

#define UNDEFINED_PID       32



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32F439::m_pclock_freqs[uC_BASE_STM32F439_MAX_PCLKS] =
{
	AHB1_PERIPH_CLOCK_FREQ,
	AHB2_PERIPH_CLOCK_FREQ,
	AHB3_PERIPH_CLOCK_FREQ,
	APB1_PERIPH_CLOCK_FREQ,
	APB2_PERIPH_CLOCK_FREQ,
	TIMERS2to7_PERIPH_CLOCK_FREQ,
	TIMER1_PERIPH_CLOCK_FREQ,
	ADC_PERIPH_CLOCK_FREQ
};

/*
 ********************************
 *  PIO Ctrl Definition Structure
 */
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32F439::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOAEN
		},
		&SET_PIO_A_OUTPUT,
		&SET_PIO_A_OUTPUT_INIT,
		&SET_PIO_A_INPUT,
		&SET_PIO_A_INPUT_PULLUP,
		&SET_PIO_A_INPUT_PULLDOWN
	},
	{
		GPIOB,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOBEN
		},
		&SET_PIO_B_OUTPUT,
		&SET_PIO_B_OUTPUT_INIT,
		&SET_PIO_B_INPUT,
		&SET_PIO_B_INPUT_PULLUP,
		&SET_PIO_B_INPUT_PULLDOWN
	},
	{
		GPIOC,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOCEN
		},
		&SET_PIO_C_OUTPUT,
		&SET_PIO_C_OUTPUT_INIT,
		&SET_PIO_C_INPUT,
		&SET_PIO_C_INPUT_PULLUP,
		&SET_PIO_C_INPUT_PULLDOWN
	},
	{
		GPIOD,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIODEN
		},
		&SET_PIO_D_OUTPUT,
		&SET_PIO_D_OUTPUT_INIT,
		&SET_PIO_D_INPUT,
		&SET_PIO_D_INPUT_PULLUP,
		&SET_PIO_D_INPUT_PULLDOWN
	},
	{
		GPIOE,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOEEN
		},
		&SET_PIO_E_OUTPUT,
		&SET_PIO_E_OUTPUT_INIT,
		&SET_PIO_E_INPUT,
		&SET_PIO_E_INPUT_PULLUP,
		&SET_PIO_E_INPUT_PULLDOWN
	},
	{
		GPIOF,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOFEN
		},
		&SET_PIO_F_OUTPUT,
		&SET_PIO_F_OUTPUT_INIT,
		&SET_PIO_F_INPUT,
		&SET_PIO_F_INPUT_PULLUP,
		&SET_PIO_F_INPUT_PULLDOWN
	},
	{
		GPIOG,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOGEN
		},
		&SET_PIO_G_OUTPUT,
		&SET_PIO_G_OUTPUT_INIT,
		&SET_PIO_G_INPUT,
		&SET_PIO_G_INPUT_PULLUP,
		&SET_PIO_G_INPUT_PULLDOWN
	},
	{
		GPIOH,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOHEN
		},
		&SET_PIO_H_OUTPUT,
		&SET_PIO_H_OUTPUT_INIT,
		&SET_PIO_H_INPUT,
		&SET_PIO_H_INPUT_PULLUP,
		&SET_PIO_H_INPUT_PULLDOWN
	},
	{
		GPIOI,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_GPIOIEN
		},
		&SET_PIO_I_OUTPUT,
		&SET_PIO_I_OUTPUT_INIT,
		&SET_PIO_I_INPUT,
		&SET_PIO_I_INPUT_PULLUP,
		&SET_PIO_I_INPUT_PULLDOWN
	}
};


/*
 ********************************
 * USB Definition Structure
 */
const uC_BASE_USB_IO_PORT_STRUCT uC_Base_USB_FS_IO_PORT =
{&USB_FS_DM, &USB_FS_DP, &USB_FS_ID, &USB_FS_VBUS};

const uC_BASE_USB_IO_PORT_STRUCT uC_Base_USB_HS_IO_PORT =
{&USB_HS_FS_DM, &USB_HS_FS_DP, &USB_HS_FS_ID, &USB_HS_FS_VBUS};

const uC_BASE_USB_IO_STRUCT uC_Base_STM32F439::m_usb_io[uC_BASE_MAX_USB_PORTS] =
{
	{
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB2 ),
			RCC_AHB2ENR_OTGFSEN
		},
		OTG_FS_IRQn,
		&uC_Base_USB_FS_IO_PORT
	},
	{
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB1 ),
			RCC_AHB1ENR_OTGHSEN
		},
		OTG_HS_IRQn,
		&uC_Base_USB_HS_IO_PORT
	}
};


/*
 ********************************
 *  USART Definition Structure
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT = { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO_PA2, &USART2_RX_PIO_PA3 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO_PB10, &USART3_RX_PIO_PB11 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART4_DEFAULT_PORT = { &USART4_TX_PIO_PC10, &USART4_RX_PIO_PC11 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART5_DEFAULT_PORT = { &USART5_TX_PIO_PC12, &USART5_RX_PIO_PD2 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART6_DEFAULT_PORT = { &USART6_TX_PIO_PC6, &USART6_RX_PIO_PC7 };

const uC_BASE_USART_IO_STRUCT uC_Base_STM32F439::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		USART1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART1EN
		},
		USART1_IRQn,
		uC_BASE_USART_PORT_1,
		DMA_USART1RX_D2S5C4,
		DMA_USART1TX_D2S7C4,

		&uC_BASE_USART1_DEFAULT_PORT
	},
	{
		USART2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_USART2EN
		},
		USART2_IRQn,
		uC_BASE_USART_PORT_2,
		DMA_UART2RX_D1S5C4,
		DMA_UART2TX_D1S6C4,

		&uC_BASE_USART2_DEFAULT_PORT
	},
	{
		USART3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_USART3EN
		},
		USART3_IRQn,
		uC_BASE_USART_PORT_3,
		DMA_UART3RX_D1S1C4,
		DMA_UART3TX_D1S3C4,

		&uC_BASE_USART3_DEFAULT_PORT
	},
	{
		UART4,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_UART4EN
		},
		UART4_IRQn,
		uC_BASE_USART_PORT_4,
		DMA_UART4RX_D1S2C4,
		DMA_UART4TX_D1S4C4,

		&uC_BASE_USART4_DEFAULT_PORT
	},
	{
		UART5,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_UART5EN
		},
		UART5_IRQn,
		uC_BASE_USART_PORT_5,
		DMA_UART5RX_D1S0C4,
		DMA_UART5TX_D1S7C4,

		&uC_BASE_USART5_DEFAULT_PORT
	},
	{
		USART6,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART6EN
		},
		USART6_IRQn,
		uC_BASE_USART_PORT_6,
		DMA_USART6RX_D2S1C5,
		DMA_USART6TX_D2S6C5,

		&uC_BASE_USART6_DEFAULT_PORT
	}
};

/*
 ********************************
 *  SPI Definition Structure
 */
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI1_DEFAULT_IO_PORTS =
{ &SPI1_MOSI_PIO_PA7, &SPI1_MISO_PIO_PA6, &SPI1_CLOCK_PIO_PA5, &SPI1_SS_PIO_PA4 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI2_DEFAULT_IO_PORTS =
{ &SPI2_MOSI_PIO_PB15, &SPI2_MISO_PIO_PB14, &SPI2_CLOCK_PIO_PB13, &SPI2_SS_PIO_PB12 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI3_DEFAULT_IO_PORTS =
{ &SPI3_MOSI_PIO_PB5, &SPI3_MISO_PIO_PB4, &SPI3_CLOCK_PIO_PB3, &SPI3_SS_PIO_PA15 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI4_DEFAULT_IO_PORTS =
{ &SPI4_MOSI_PIO_PE14, &SPI4_MISO_PIO_PE13, &SPI4_CLOCK_PIO_PE12, &SPI4_SS_PIO_PE11 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI5_DEFAULT_IO_PORTS =
{ &SPI5_MOSI_PIO_PF9, &SPI5_MISO_PIO_PF8, &SPI5_CLOCK_PIO_PF7, &SPI5_SS_PIO_PF6 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI6_DEFAULT_IO_PORTS =
{ &SPI6_MOSI_PIO_PG14, &SPI6_MISO_PIO_PG12, &SPI6_CLOCK_PIO_PG13, &SPI6_SS_PIO_PG8 };
const uC_BASE_SPI_IO_STRUCT uC_Base_STM32F439::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
{
	{
		SPI1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI1EN
		},
		SPI1_IRQn,
		DMA_SPI1RX_D2S0C3,
		DMA_SPI1TX_D2S3C3,

		&uC_BASE_SPI1_DEFAULT_IO_PORTS
	},
	{
		SPI2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_SPI2EN
		},
		SPI2_IRQn,
		DMA_SPI2RX_D1S3C0,
		DMA_SPI2TX_D1S4C0,

		&uC_BASE_SPI2_DEFAULT_IO_PORTS
	},
	{
		SPI3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_SPI3EN
		},
		SPI3_IRQn,
		DMA_SPI3RX_D1S2C0,
		DMA_SPI3TX_D1S5C0,

		&uC_BASE_SPI3_DEFAULT_IO_PORTS
	},
	{
		SPI4,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI4EN
		},
		SPI4_IRQn,
		DMA_SPI4RX_D2S0C4,
		DMA_SPI4TX_D2S1C4,

		&uC_BASE_SPI4_DEFAULT_IO_PORTS
	},
	{
		SPI5,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI5EN
		},
		SPI5_IRQn,
		DMA_SPI5RX_D2S3C2,
		DMA_SPI5TX_D2S4C2,

		&uC_BASE_SPI5_DEFAULT_IO_PORTS
	},
	{
		SPI6,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI6EN
		},
		SPI6_IRQn,
		DMA_SPI6RX_D2S6C1,
		DMA_SPI6TX_D2S5C1,

		&uC_BASE_SPI6_DEFAULT_IO_PORTS
	}


};

/*
 ********************************
 *  I2C Definition Structure
 */
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C1_DEFAULT_IO_PORTS =
{ &I2C1_SCL_PIO_PB6, &I2C1_SDA_PIO_PB7 };
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C2_DEFAULT_IO_PORTS =
{ &I2C2_SCL_PIO_PF1, &I2C2_SDA_PIO_PF0 };
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C3_DEFAULT_IO_PORTS =
{ &I2C3_SCL_PIO_PA8, &I2C3_SDA_PIO_PC9 };
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F439::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F439::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F439::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F439::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F439::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F439::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT2_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT3_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[2] )( m_obj_i2c_dma_list_Tx[2] );
}

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT2_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}

void uC_Base_STM32F439::uC_BASE_I2C_DMA_INT3_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[2] )( m_obj_i2c_dma_list_Rx[2] );
}

void uC_Base_STM32F439::uC_BASE_I2C_INT1( void )    { ( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] ); }

void uC_Base_STM32F439::uC_BASE_I2C_INT2( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }

void uC_Base_STM32F439::uC_BASE_I2C_INT3( void )    { ( *m_obj_func_i2c_list[2] )( m_obj_i2c_list[2] ); }


const uC_BASE_I2C_IO_STRUCT uC_Base_STM32F439::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C1EN
		},
		I2C1_EV_IRQn,
		DMA_I2C1RX_D1S5C1,
		DMA_I2C1TX_D1S6C1,

		&uC_BASE_I2C1_DEFAULT_IO_PORTS,

		&m_obj_func_i2c_dma_list_Tx[0],
		&m_obj_i2c_dma_list_Tx[0],
		uC_BASE_I2C_DMA_INT1_Tx,
		&m_obj_func_i2c_dma_list_Rx[0],
		&m_obj_i2c_dma_list_Rx[0],
		uC_BASE_I2C_DMA_INT1_Rx,

		&m_obj_func_i2c_list[0],
		&m_obj_i2c_list[0],
		uC_BASE_I2C_INT1

	},
	{
		I2C2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C2EN
		},
		I2C2_EV_IRQn,
		DMA_I2C2RX_D1S3C7,
		DMA_I2C2TX_D1S7C7,

		&uC_BASE_I2C2_DEFAULT_IO_PORTS,

		&m_obj_func_i2c_dma_list_Tx[1],
		&m_obj_i2c_dma_list_Tx[1],
		uC_BASE_I2C_DMA_INT2_Tx,
		&m_obj_func_i2c_dma_list_Rx[1],
		&m_obj_i2c_dma_list_Rx[1],
		uC_BASE_I2C_DMA_INT2_Rx,
		&m_obj_func_i2c_list[1],
		&m_obj_i2c_list[1],
		uC_BASE_I2C_INT2
	},
	{
		I2C3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C3EN
		},
		I2C3_EV_IRQn,
		DMA_I2C3RX_D1S2C3,
		DMA_I2C3TX_D1S4C3,

		&uC_BASE_I2C3_DEFAULT_IO_PORTS,

		&m_obj_func_i2c_dma_list_Tx[2],
		&m_obj_i2c_dma_list_Tx[2],
		uC_BASE_I2C_DMA_INT3_Tx,
		&m_obj_func_i2c_dma_list_Rx[2],
		&m_obj_i2c_dma_list_Rx[2],
		uC_BASE_I2C_DMA_INT3_Rx,
		&m_obj_func_i2c_list[2],
		&m_obj_i2c_list[2],
		uC_BASE_I2C_INT3
	}
};

/*
 ********************************
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2;
const uint8_t TMR_CNT_SIZE_32BITS = 4;
uC_PERIPH_IO_STRUCT const* const m_timer1_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER1_PIO1_PA8, &TIMER1_PIO2_PA9, &TIMER1_PIO3_PA10, &TIMER1_PIO4_PA11 };
uC_PERIPH_IO_STRUCT const* const m_timer2_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER2_PIO1_PA0, &TIMER2_PIO2_PA1, &TIMER2_PIO3_PA2, &TIMER2_PIO4_PA3 };
uC_PERIPH_IO_STRUCT const* const m_timer3_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER3_PIO1_PC6, &TIMER3_PIO2_PC7, &TIMER3_PIO3_PC8, &TIMER3_PIO4_PC9 };
uC_PERIPH_IO_STRUCT const* const m_timer4_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER4_PIO1_PB6, &TIMER4_PIO2_PB7, &TIMER4_PIO3_PB8, &TIMER4_PIO4_PB9 };
uC_PERIPH_IO_STRUCT const* const m_timer5_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER5_PIO1_PA0, &TIMER5_PIO2_PA1, &TIMER5_PIO3_PA2, &TIMER5_PIO4_PA3 };
uC_PERIPH_IO_STRUCT const* const m_timer8_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER8_PIO1_PC6, &TIMER8_PIO2_PC7, &TIMER8_PIO3_PC8, &TIMER8_PIO4_PC9 };
const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32F439::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{			// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMER1and8,
		TIM1_UP_TIM10_IRQn,
		TIM1_CC_IRQn,
		m_timer1_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM2EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM2_IRQn,
		TIM2_IRQn,
		m_timer2_pio,
		TMR_CNT_SIZE_32BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM3EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM3_IRQn,
		TIM3_IRQn,
		m_timer3_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM4,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM4EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM4_IRQn,
		TIM4_IRQn,
		m_timer4_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM5,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM5EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM5_IRQn,
		TIM5_IRQn,
		m_timer5_pio,
		TMR_CNT_SIZE_32BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM6,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM6EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM6_DAC_IRQn,
		TIM6_DAC_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM7,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM7EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMERS2to7,
		TIM7_IRQn,
		TIM7_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM8,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM8EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMER1and8,
		TIM8_UP_TIM13_IRQn,
		TIM8_CC_IRQn,
		m_timer8_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	}
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32F439::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
{
	{
		EXTI0_IRQn,
		0x0001
	},
	{
		EXTI1_IRQn,
		0x0002
	},
	{
		EXTI2_IRQn,
		0x0004
	},
	{
		EXTI3_IRQn,
		0x0008
	},
	{
		EXTI4_IRQn,
		0x0010
	},
	{
		EXTI9_5_IRQn,
		0x03E0
	},
	{
		EXTI15_10_IRQn,
		0xFC00
	}
};

/*
 ********************************
 *  AtoD Definition Structure
 */
uC_PERIPH_IO_STRUCT const* const m_adc12_pio[uC_ATOD_CHANNEL_MAX] =
{
	&ADC12_IN0_PIO, &ADC12_IN1_PIO, &ADC12_IN2_PIO, &ADC12_IN3_PIO,
	&ADC12_IN4_PIO, &ADC12_IN5_PIO, &ADC12_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC12_IN10_PIO, &ADC12_IN11_PIO,
	&ADC12_IN12_PIO, &ADC12_IN13_PIO, &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	&ADC12_IN16_PIO, &ADC12_IN17_PIO, &ADC12_IN18_PIO
};
uC_PERIPH_IO_STRUCT const* const m_adc3_pio[uC_ATOD_CHANNEL_MAX] =
{
	&ADC3_IN0_PIO, &ADC3_IN1_PIO, &ADC3_IN2_PIO, &ADC3_IN3_PIO,
	&ADC3_IN4_PIO, &ADC3_IN5_PIO, &ADC3_IN6_PIO, &ADC3_IN7_PIO,
	&ADC3_IN8_PIO, &ADC3_IN9_PIO, &ADC3_IN10_PIO, &ADC3_IN11_PIO,
	&ADC3_IN12_PIO, &ADC3_IN13_PIO, &ADC3_IN14_PIO, &ADC3_IN15_PIO,
	&ADC3_IN16_PIO, &ADC3_IN17_PIO, &ADC3_IN18_PIO
};

const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32F439::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_ADC1EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMER_ADC,
		ADC_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc12_pio,
#ifdef ADC1_DMA_SELECT
		ADC1_DMA_SELECT
#else
		DMA_ADC1_D2S4C0
#endif
	},
	// {
	// ADC2,
	// {
	// RCC_PERIPH_CLCK_APB2,
	// RCC_APB2ENR_ADC2EN
	// },
	// ADC1_2_IRQn,
	// uC_ATOD_CHANNEL_MAX,
	// m_adc12_pio,
	// uC_BASE_ATOD_MAX_CHANNELS,
	// uC_DMA_CHANNEL_MAX		//This channel does not have a DMA.
	// },
	{
		ADC3,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_ADC3EN
		},
		uC_BASE_STM32F439_PCLK_INDEX_TIMER_ADC,
		ADC_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc3_pio,
		DMA_ADC3_D2S1C2
	}
};

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32F439::m_dma_io[uC_BASE_DMA_STREAM_MAX] =
{
	{
		DMA1,
		DMA1_Stream0,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream0_IRQn,
		0
	},
	{
		DMA1,
		DMA1_Stream1,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream1_IRQn,
		1
	},
	{
		DMA1,
		DMA1_Stream2,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream2_IRQn,
		2
	},
	{
		DMA1,
		DMA1_Stream3,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream3_IRQn,
		3
	},
	{
		DMA1,
		DMA1_Stream4,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream4_IRQn,
		4
	},
	{
		DMA1,
		DMA1_Stream5,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream5_IRQn,
		5
	},
	{
		DMA1,
		DMA1_Stream6,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream6_IRQn,
		6
	},
	{
		DMA1,
		DMA1_Stream7,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN
		},
		DMA1_Stream7_IRQn,
		7
	},
	{
		DMA2,
		DMA2_Stream0,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream0_IRQn,
		0
	},
	{
		DMA2,
		DMA2_Stream1,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream1_IRQn,
		1
	},
	{
		DMA2,
		DMA2_Stream2,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream2_IRQn,
		2
	},
	{
		DMA2,
		DMA2_Stream3,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream3_IRQn,
		3
	},
	{
		DMA2,
		DMA2_Stream4,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream4_IRQn,
		4
	},
	{
		DMA2,
		DMA2_Stream5,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream5_IRQn,
		5
	},
	{
		DMA2,
		DMA2_Stream6,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream6_IRQn,
		6
	},
	{
		DMA2,
		DMA2_Stream7,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN
		},
		DMA2_Stream7_IRQn,
		7
	}
};


/*
 ********************************
 *  EMAC Controller Definition Structure
 */

uC_PERIPH_IO_STRUCT const* const m_mii_pio_out[uC_BASE_MII_PIO_OUT_COUNT] =
{
	&ETH_MII_TX_EN_PIO, &ETH_MII_TXD0_PIO, &ETH_MII_TXD1_PIO,
	&ETH_MII_TXD2_PIO, &ETH_MII_TXD3_PIO, &ETH_MII_MDC_PIO,
	&ETH_MII_MDIO_PIO
};
/*
   Signal RX_ER is not used specifically for STM32F439NI Discovery Eval Board.
   Note from User Manual(STM32439I-EVAL evaluation board for the STM32F439 line) -
   RX_ER is not connected to PI10 of STM32F439NIH6 because PI10 is shared with data signal of SDRAM.
   If RX_ER signal is needed, remove R244 and solder R43.
   For other boards, ETH_MII_RX_ER_PIO can be enabled and uC_BASE_MII_PIO_IN_COUNT can be set to 10
 */
#define uC_BASE_MII_PIO_IN_COUNT            9
uC_PERIPH_IO_STRUCT const* const m_mii_pio_in[uC_BASE_MII_PIO_IN_COUNT] =
{
	&ETH_MII_COL_PIO, &ETH_MII_CRS_WKUP_PIO, &ETH_MII_TX_CLK_PIO,
	/*&ETH_MII_RX_ER_PIO,*/ &ETH_MII_RX_CLK_PIO, &ETH_MII_RX_DV_PIO,
	&ETH_MII_RXD0_PIO, &ETH_MII_RXD1_PIO, &ETH_MII_RXD2_PIO,
	&ETH_MII_RXD3_PIO
};
#define uC_BASE_RMII_PIO_OUT_COUNT          5
uC_PERIPH_IO_STRUCT const* const m_rmii_pio_out[uC_BASE_RMII_PIO_OUT_COUNT] =
{
	&ETH_RMII_TX_EN_PIO, &ETH_RMII_TXD0_PIO, &ETH_RMII_TXD1_PIO,
	&ETH_RMII_MDIO_PIO, &ETH_RMII_MDC_PIO
};
#define uC_BASE_RMII_PIO_IN_COUNT           4
uC_PERIPH_IO_STRUCT const* const m_rmii_pio_in[uC_BASE_RMII_PIO_IN_COUNT] =
{
	&ETH_RMII_CRS_DV_PIO, &ETH_RMII_RXD0_PIO, &ETH_RMII_RXD1_PIO,
	&ETH_RMII_REF_CLK_PIO
};
const uC_BASE_EMAC_IO_STRUCT uC_Base_STM32F439::m_emac_io[uC_BASE_EMAC_MAX_CTRLS] =
{
	{
		ETH,
		{
			RCC_PERIPH_CLCK_AHB1,
			( RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN )
		},
		ETH_IRQn,
		SYSCFG_PMC_MII_RMII,
		// &ETH_MCO_CLOCK_OUT_PIO,

		uC_BASE_MII_PIO_OUT_COUNT,
		uC_BASE_MII_PIO_IN_COUNT,
		m_mii_pio_out,
		m_mii_pio_in,

		uC_BASE_RMII_PIO_OUT_COUNT,
		uC_BASE_RMII_PIO_IN_COUNT,
		m_rmii_pio_out,
		m_rmii_pio_in,
	}
};



/*
 ********************************
 *  CAN Definition Structure
 */

const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN1_TX_PIO_PA12, &CAN1_RX_PIO_PA11};
// const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN1_DEFAULT_IO_PORTS = { &CAN1_RX_PIO_PB8,
// &CAN1_TX_PIO_PB9};
// const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN_RX_PIO_PD0,
// &CAN_TX_PIO_PD1};


const uC_BASE_CAN_IO_STRUCT uC_Base_STM32F439::m_can_io[uC_BASE_CAN_MAX_CTRLS] =
{
	{
		CAN1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_CAN1EN
		},
		CAN1_RX0_IRQn,			/*!< CAN RX0 Interrupt */
		CAN1_TX_IRQn,			/*!< CAN TX Interrupt  */
		&uC_BASE_CAN_DEFAULT_IO_PORTS
	},
	{
		CAN2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_CAN2EN
		},
		CAN1_RX0_IRQn,			/*!< CAN RX0 Interrupt */
		CAN1_TX_IRQn,			/*!< CAN TX Interrupt  */
		&uC_BASE_CAN_DEFAULT_IO_PORTS
	}
};


/* lint +e1924
  lint +e1960*/
