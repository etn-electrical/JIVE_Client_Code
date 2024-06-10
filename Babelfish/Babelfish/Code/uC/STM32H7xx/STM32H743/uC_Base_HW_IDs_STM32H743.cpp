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
#include "uC_Base_STM32H743.h"
#include "uC_IO_Define_STM32H743.h"

#define UNDEFINED_PID       32


/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32H743::m_pclock_freqs[uC_BASE_STM32H743_MAX_PCLKS] =
{
	AHB1_PERIPH_CLOCK_FREQ,
	AHB2_PERIPH_CLOCK_FREQ,
	AHB3_PERIPH_CLOCK_FREQ,
	AHB4_PERIPH_CLOCK_FREQ,
	APB1L_PERIPH_CLOCK_FREQ,
	APB1H_PERIPH_CLOCK_FREQ,
	APB2_PERIPH_CLOCK_FREQ,
	APB3_PERIPH_CLOCK_FREQ,
	APB4_PERIPH_CLOCK_FREQ,
	TIMERS2_TO_7_12_TO_14_PERIPH_CLOCK_FREQ,
	TIMERS1_8_15_TO_17_PERIPH_CLOCK_FREQ,
	ADC_PERIPH_CLOCK_FREQ
};

/*
 ********************************
 *  PIO Ctrl Definition Structure
 */
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32H743::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOAEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOBEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOCEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIODEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOEEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOFEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOGEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOHEN
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
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOIEN
		},
		&SET_PIO_I_OUTPUT,
		&SET_PIO_I_OUTPUT_INIT,
		&SET_PIO_I_INPUT,
		&SET_PIO_I_INPUT_PULLUP,
		&SET_PIO_I_INPUT_PULLDOWN
	},
	{
		GPIOJ,
		{
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOJEN
		},
		&SET_PIO_J_OUTPUT,
		&SET_PIO_J_OUTPUT_INIT,
		&SET_PIO_J_INPUT,
		&SET_PIO_J_INPUT_PULLUP,
		&SET_PIO_J_INPUT_PULLDOWN
	},
	{
		GPIOK,
		{
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_GPIOKEN
		},
		&SET_PIO_K_OUTPUT,
		&SET_PIO_K_OUTPUT_INIT,
		&SET_PIO_K_INPUT,
		&SET_PIO_K_INPUT_PULLUP,
		&SET_PIO_K_INPUT_PULLDOWN
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

const uC_BASE_USB_IO_STRUCT uC_Base_STM32H743::m_usb_io[uC_BASE_MAX_USB_PORTS] =
{
	{
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB1 ),
			RCC_AHB1ENR_USB2OTGFSEN
		},
		OTG_FS_IRQn,
		&uC_Base_USB_FS_IO_PORT
	},
	{
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB1 ),
			RCC_AHB1ENR_USB1OTGHSEN
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
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO_PD5, &USART2_RX_PIO_PD6 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO_PB10, &USART3_RX_PIO_PB11 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_UART4_DEFAULT_PORT = { &UART4_TX_PIO_PC10, &UART4_RX_PIO_PC11 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_UART5_DEFAULT_PORT = { &UART5_TX_PIO_PC12, &UART5_RX_PIO_PD2 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART6_DEFAULT_PORT = { &USART6_TX_PIO_PG14, &USART6_RX_PIO_PG9 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_UART7_DEFAULT_PORT = { &UART7_TX_PIO_PF7, &UART7_RX_PIO_PF6 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_UART8_DEFAULT_PORT = { &UART8_TX_PIO_PE1, &UART8_RX_PIO_PE0 };

const uC_BASE_USART_IO_STRUCT uC_Base_STM32H743::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		USART1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART1EN
		},
		USART1_IRQn,
		uC_BASE_USART_PORT_1,
		DMA_USART1RX_D2S0,
		DMA_USART1TX_D1S0,

		&uC_BASE_USART1_DEFAULT_PORT
	},
	{
		USART2,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_USART2EN
		},
		USART2_IRQn,
		uC_BASE_USART_PORT_2,
		DMA_USART2RX_D2S1,
		DMA_USART2TX_D1S1,

		&uC_BASE_USART2_DEFAULT_PORT
	},
	{
		USART3,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_USART3EN
		},
		USART3_IRQn,
		uC_BASE_USART_PORT_3,
		DMA_USART3RX_D2S2,
		DMA_USART3TX_D1S2,

		&uC_BASE_USART3_DEFAULT_PORT
	},
	{
		UART4,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_UART4EN
		},
		UART4_IRQn,
		uC_BASE_UART_PORT_4,
		DMA_UART4RX_D2S3,
		DMA_UART4TX_D1S3,

		&uC_BASE_UART4_DEFAULT_PORT
	},
	{
		UART5,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_UART5EN
		},
		UART5_IRQn,
		uC_BASE_UART_PORT_5,
		DMA_UART5RX_D2S4,
		DMA_UART5TX_D1S4,

		&uC_BASE_UART5_DEFAULT_PORT
	},
	{
		USART6,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART6EN
		},
		USART6_IRQn,
		uC_BASE_USART_PORT_6,
		DMA_USART6RX_D2S5,
		DMA_USART6TX_D1S5,

		&uC_BASE_USART6_DEFAULT_PORT
	},
	{
		UART7,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_UART7EN
		},
		UART7_IRQn,
		uC_BASE_UART_PORT_7,
		DMA_UART7RX_D2S6,
		DMA_UART7TX_D1S6,
		&uC_BASE_UART7_DEFAULT_PORT
	},
	{
		UART8,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_UART8EN
		},
		UART8_IRQn,
		uC_BASE_UART_PORT_8,
		DMA_UART8RX_D2S7,
		DMA_UART8TX_D1S7,
		&uC_BASE_UART8_DEFAULT_PORT
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
const uC_BASE_SPI_IO_STRUCT uC_Base_STM32H743::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
{
	{
		SPI1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI1EN
		},
		SPI1_IRQn,
		DMA_SPI1RX_D2S5,
		DMA_SPI1TX_D1S5,

		&uC_BASE_SPI1_DEFAULT_IO_PORTS
	},
	{
		SPI2,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_SPI2EN
		},
		SPI2_IRQn,
		DMA_SPI2RX_D2S6,
		DMA_SPI2TX_D1S6,

		&uC_BASE_SPI2_DEFAULT_IO_PORTS
	},
	{
		SPI3,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_SPI3EN
		},
		SPI3_IRQn,
		DMA_SPI3RX_D2S7,
		DMA_SPI3TX_D1S7,

		&uC_BASE_SPI3_DEFAULT_IO_PORTS
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
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32H743::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32H743::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32H743::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32H743::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32H743::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32H743::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT2_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT3_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[2] )( m_obj_i2c_dma_list_Tx[2] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT2_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32H743::uC_BASE_I2C_DMA_INT3_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[2] )( m_obj_i2c_dma_list_Rx[2] );
}

void uC_Base_STM32H743::uC_BASE_I2C_INT1( void )    { ( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] ); }

void uC_Base_STM32H743::uC_BASE_I2C_INT2( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }

void uC_Base_STM32H743::uC_BASE_I2C_INT3( void )    { ( *m_obj_func_i2c_list[2] )( m_obj_i2c_list[2] ); }


const uC_BASE_I2C_IO_STRUCT uC_Base_STM32H743::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_I2C1EN
		},
		I2C1_EV_IRQn,
		DMA_I2C1RX_D2S2,
		DMA_I2C1TX_D1S2,

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
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_I2C2EN
		},
		I2C2_EV_IRQn,
		DMA_I2C2RX_D2S3,// TODO
		DMA_I2C2TX_D1S3,

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
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_I2C3EN
		},
		I2C3_EV_IRQn,
		DMA_I2C3RX_D2S4,// TODO
		DMA_I2C3TX_D1S4,

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
const uint8_t TMR_CNT_SIZE_16BITS = 2U;
const uint8_t TMR_CNT_SIZE_32BITS = 4U;
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

uC_PERIPH_IO_STRUCT const* const m_timer12_pio[uC_BASE_2_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER12_PIO1_PH6, &TIMER12_PIO2_PH9 };

uC_PERIPH_IO_STRUCT const* const m_timer13_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER13_PIO1_PF8 };

uC_PERIPH_IO_STRUCT const* const m_timer14_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER14_PIO1_PF9 };

uC_PERIPH_IO_STRUCT const* const m_timer15_pio[uC_BASE_2_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER15_PIO1_PE5, &TIMER15_PIO2_PE6 };

uC_PERIPH_IO_STRUCT const* const m_timer16_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER16_PIO1_PF6 };

uC_PERIPH_IO_STRUCT const* const m_timer17_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER17_PIO1_PF7 };

const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32H743::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{	// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMER1_8_15_TO_17,
		TIM1_UP_IRQn,
		TIM1_CC_IRQn,
		m_timer1_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM2,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM2EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM2_IRQn,
		TIM2_IRQn,
		m_timer2_pio,
		TMR_CNT_SIZE_32BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM3,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM3EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM3_IRQn,
		TIM3_IRQn,
		m_timer3_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM4,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM4EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM4_IRQn,
		TIM4_IRQn,
		m_timer4_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM5,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM5EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM5_IRQn,
		TIM5_IRQn,
		m_timer5_pio,
		TMR_CNT_SIZE_32BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM6,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM6EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM6_DAC_IRQn,
		TIM6_DAC_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM7,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM7EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
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
		uC_BASE_STM32H743_PCLK_INDEX_TIMER1_8_15_TO_17,
		TIM8_UP_TIM13_IRQn,
		TIM8_CC_IRQn,
		m_timer8_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM12,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM12EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM8_BRK_TIM12_IRQn,
		TIM8_BRK_TIM12_IRQn,
		m_timer12_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_2_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM13,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM13EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM8_UP_TIM13_IRQn,
		TIM8_UP_TIM13_IRQn,
		m_timer13_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM14,
		{
			RCC_PERIPH_CLCK_APB1L,
			RCC_APB1LENR_TIM14EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMERS2_TO_7_12_TO_14,
		TIM8_TRG_COM_TIM14_IRQn,
		TIM8_TRG_COM_TIM14_IRQn,
		m_timer14_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM15,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM15EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMER1_8_15_TO_17,
		TIM15_IRQn,
		TIM15_IRQn,
		m_timer15_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_2_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM16,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM16EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMER1_8_15_TO_17,
		TIM16_IRQn,
		TIM16_IRQn,
		m_timer16_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM17,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM17EN
		},
		uC_BASE_STM32H743_PCLK_INDEX_TIMER1_8_15_TO_17,
		TIM17_IRQn,
		TIM17_IRQn,
		m_timer17_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	}
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
// const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32H743::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
// {
// {
// EXTI0_IRQn,
// 0x0001
// },
// {
// EXTI1_IRQn,
// 0x0002
// },
// {
// EXTI2_IRQn,
// 0x0004
// },
// {
// EXTI3_IRQn,
// 0x0008
// },
// {
// EXTI4_IRQn,
// 0x0010
// },
// {
// EXTI9_5_IRQn,
// 0x03E0
// },
// {
// EXTI15_10_IRQn,
// 0xFC00
// }
// };

/*
 ********************************
 *  AtoD Definition Structure
 */
uC_PERIPH_IO_STRUCT const* const m_adc1_pio[uC_ATOD_1_CHANNEL_MAX] =
{
	/* Channel 0/1 not available */ &ADC1_IN2_PIO, &ADC12_IN3_PIO,
	&ADC12_IN4_PIO, &ADC12_IN5_PIO, &ADC1_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC123_IN10_PIO, &ADC123_IN11_PIO,
	/*Channel 12/13 not available*/ &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	&ADC1_IN16_PIO, &ADC1_IN17_PIO, &ADC12_IN18_PIO, &ADC12_IN19_PIO
};

uC_PERIPH_IO_STRUCT const* const m_adc2_pio[uC_ATOD_2_CHANNEL_MAX] =
{
	/* Channel 0/1 not available */ &ADC2_IN2_PIO, &ADC12_IN3_PIO,
	&ADC12_IN4_PIO, &ADC12_IN5_PIO, &ADC2_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC123_IN10_PIO, &ADC123_IN11_PIO,
	/*Channel 12/13 not available*/ &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	/*Channel 16/17 not available*/ &ADC12_IN18_PIO, &ADC12_IN19_PIO
};

uC_PERIPH_IO_STRUCT const* const m_adc3_pio[uC_ATOD_3_CHANNEL_MAX] =
{
	/* Channel 0/1 not available */ &ADC3_IN2_PIO, &ADC3_IN3_PIO,
	&ADC3_IN4_PIO, &ADC3_IN5_PIO, &ADC3_IN6_PIO, &ADC3_IN7_PIO,
	&ADC3_IN8_PIO, &ADC3_IN9_PIO, &ADC123_IN10_PIO, &ADC123_IN11_PIO,
	&ADC123_IN12_PIO, &ADC3_IN13_PIO, &ADC3_IN14_PIO, &ADC3_IN15_PIO,
	&ADC3_IN16_PIO
};

const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32H743::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		ADC12_COMMON,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_ADC12EN
		},
		ADC_IRQn,
		uC_ATOD_1_CHANNEL_MAX,
		m_adc1_pio,
		DMA_ADC1_D1S6
	},
	{
		ADC2,
		ADC12_COMMON,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_ADC12EN
		},
		ADC_IRQn,
		uC_ATOD_2_CHANNEL_MAX,
		m_adc2_pio,
		DMA_ADC2_D1S7
	},
	{
		ADC3,
		ADC3_COMMON,
		{
			RCC_PERIPH_CLCK_AHB4,
			RCC_AHB4ENR_ADC3EN
		},
		ADC_IRQn,
		uC_ATOD_3_CHANNEL_MAX,
		m_adc3_pio,
		DMA_ADC3_D2S6
	},
};

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32H743::m_dma_io[uC_BASE_DMA_STREAM_MAX] =
{
	{
		DMA1,
		DMA1_Stream0,
		DMAMUX1_Channel0,
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
		DMAMUX1_Channel1,
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
		DMAMUX1_Channel2,
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
		DMAMUX1_Channel3,
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
		DMAMUX1_Channel4,
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
		DMAMUX1_Channel5,
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
		DMAMUX1_Channel6,
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
		DMAMUX1_Channel7,
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
		DMAMUX1_Channel8,
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
		DMAMUX1_Channel9,
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
		DMAMUX1_Channel10,
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
		DMAMUX1_Channel11,
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
		DMAMUX1_Channel12,
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
		DMAMUX1_Channel13,
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
		DMAMUX1_Channel14,
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
		DMAMUX1_Channel15,
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

const uC_BASE_EMAC_IO_STRUCT uC_Base_STM32H743::m_emac_io[uC_BASE_EMAC_MAX_CTRLS] =
{
	{
		ETH,
		{
			RCC_PERIPH_CLCK_AHB1,
			( RCC_AHB1ENR_ETH1MACEN | RCC_AHB1ENR_ETH1TXEN | RCC_AHB1ENR_ETH1RXEN )
		},
		ETH_IRQn,
		SYSCFG_PMCR_EPIS_SEL_2,
		// &ETH_MCO_CLOCK_OUT_PIO,

		uC_BASE_MII_PIO_OUT_COUNT,
		uC_BASE_MII_PIO_IN_COUNT,
		uC_BASE_RMII_PIO_OUT_COUNT,
		uC_BASE_RMII_PIO_IN_COUNT,

		&uC_BASE_EMAC_DEFAULT_PORT,
	}
};



/*
 ********************************
 *  CAN Definition Structure
 */

// const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN1_TX_PIO_PD1, &CAN1_RX_PIO_PD0};
////const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN1_DEFAULT_IO_PORTS = { &CAN1_RX_PIO_PB8,
////&CAN1_TX_PIO_PB9};
////const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN_RX_PIO_PD0,
////&CAN_TX_PIO_PD1};
//
//
// const uC_BASE_CAN_IO_STRUCT uC_Base_STM32H743::m_can_io[uC_BASE_CAN_MAX_CTRLS] =
// {
// {
// CAN1,
// {
// RCC_PERIPH_CLCK_APB1,
// RCC_APB1ENR_CAN1EN
// },
// CAN1_RX0_IRQn,        /*!< CAN RX0 Interrupt */
// CAN1_TX_IRQn,         /*!< CAN TX Interrupt  */
// &uC_BASE_CAN_DEFAULT_IO_PORTS
// },
// {
// CAN2,
// {
// RCC_PERIPH_CLCK_APB1,
// RCC_APB1ENR_CAN2EN
// },
// CAN1_RX0_IRQn,        /*!< CAN RX0 Interrupt */
// CAN1_TX_IRQn,         /*!< CAN TX Interrupt  */
// &uC_BASE_CAN_DEFAULT_IO_PORTS
// }
// };


/* lint +e1924
   lint +e1960*/
