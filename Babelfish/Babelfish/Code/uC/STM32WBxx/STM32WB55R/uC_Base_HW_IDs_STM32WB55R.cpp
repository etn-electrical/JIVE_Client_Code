/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
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
#include "uC_Base_STM32WB55R.h"
#include "uC_IO_Define_STM32WB55R.h"

#define UNDEFINED_PID       32



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32WB55R::m_pclock_freqs[uC_BASE_STM32WB55R_MAX_PCLKS] =
{
	AHB1_PERIPH_CLOCK_FREQ,
	AHB2_PERIPH_CLOCK_FREQ,
	AHB3_PERIPH_CLOCK_FREQ,
	AHB4_PERIPH_CLOCK_FREQ,
	AHB5_PERIPH_CLOCK_FREQ,

	APB1_REG1_PERIPH_CLOCK_FREQ,
	APB1_REG2_PERIPH_CLOCK_FREQ,
	APB2_PERIPH_CLOCK_FREQ,
	APB3_PERIPH_CLOCK_FREQ,

	TIMERS2to7_PERIPH_CLOCK_FREQ,
	TIMER1_PERIPH_CLOCK_FREQ,
	ADC_PERIPH_CLOCK_FREQ
};

/*
 ********************************
 *  PIO Ctrl Definition Structure
 */
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32WB55R::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIOAEN
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
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIOBEN
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
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIOCEN
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
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIODEN
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
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIOEEN
		},
		&SET_PIO_E_OUTPUT,
		&SET_PIO_E_OUTPUT_INIT,
		&SET_PIO_E_INPUT,
		&SET_PIO_E_INPUT_PULLUP,
		&SET_PIO_E_INPUT_PULLDOWN
	},
	{
		GPIOH,
		{
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_GPIOHEN
		},
		&SET_PIO_H_OUTPUT,
		&SET_PIO_H_OUTPUT_INIT,
		&SET_PIO_H_INPUT,
		&SET_PIO_H_INPUT_PULLUP,
		&SET_PIO_H_INPUT_PULLDOWN
	}
};




/*
 ********************************
 *  USART Definition Structure
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT = { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
// const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO_PA2, &USART2_RX_PIO_PA3 };
// const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO_PC4, &USART3_RX_PIO_PC5 };
// const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART4_DEFAULT_PORT = { &USART4_TX_PIO_PA0, &USART4_RX_PIO_PA1 };
const uC_BASE_USART_IO_STRUCT uC_Base_STM32WB55R::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		USART1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART1EN
		},
		USART1_IRQn,
		uC_BASE_USART_PORT_1,
		DMAMUX_REQ_USART1_RX,
		DMAMUX_REQ_USART1_TX,

		&uC_BASE_USART1_DEFAULT_PORT
	},

	{
		LPUART1,
		{
			RCC_PERIPH_CLCK_APB1_REG2,
			RCC_APB1ENR2_LPUART1EN
		},
		LPUART1_IRQn,
		uC_BASE_LPUART_PORT_1,
		DMAMUX_REQ_LPUART1_RX,		// TODO Verify if needed when DMA is implemented
		DMAMUX_REQ_LPUART1_TX,		// TODO Verify if needed when DMA is implemented

		&uC_BASE_USART1_DEFAULT_PORT
	}

};

/*
 ********************************
 *  SPI Definition Structure
 */
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI1_DEFAULT_IO_PORTS =
{ &SPI1_MOSI_PIO_PA7, &SPI1_MISO_PIO_PA6, &SPI1_CLOCK_PIO_PA5, &PERIPH_NULL_PIO };

const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI2_DEFAULT_IO_PORTS =
{ &SPI2_MOSI_PIO_PB15, &SPI2_MISO_PIO_PB14, &SPI2_CLOCK_PIO_PB13, &PERIPH_NULL_PIO };

// const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI3_DEFAULT_IO_PORTS =
// { &SPI3_MOSI_PIO_PC12, &SPI3_MISO_PIO_PC11, &SPI3_CLOCK_PIO_PB12, &SPI3_SS_PIO_PA4 };

const uC_BASE_SPI_IO_STRUCT uC_Base_STM32WB55R::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
{
	{
		SPI1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI1EN
		},
		SPI1_IRQn,
		DMAMUX_REQ_SPI1_RX,
		DMAMUX_REQ_SPI1_TX,

		&uC_BASE_SPI1_DEFAULT_IO_PORTS
	},

	{
		SPI2,
		{
			RCC_PERIPH_CLCK_APB1_REG1,
			RCC_APB1ENR1_SPI2EN
		},
		SPI2_IRQn,
		DMAMUX_REQ_SPI2_RX,
		DMAMUX_REQ_SPI2_TX,

		&uC_BASE_SPI2_DEFAULT_IO_PORTS
	}
};

/*
 ********************************
 *  I2C Definition Structure
 */
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C1_DEFAULT_IO_PORTS =
{ &I2C1_SCL_PIO_PB8, &I2C1_SDA_PIO_PB9 };
// const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C2_DEFAULT_IO_PORTS =
// { &I2C2_SCL_PIO_PF1, &I2C2_SDA_PIO_PF0 };
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C3_DEFAULT_IO_PORTS =
{ &I2C3_SCL_PIO_PA8, &I2C3_SDA_PIO_PC9 };
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32WB55R::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32WB55R::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32WB55R::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32WB55R::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32WB55R::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32WB55R::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

// void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT2_Tx( void ) { ( *m_obj_func_i2c_dma_list_Tx[1] )(
// m_obj_i2c_dma_list_Tx[1] ); }

void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT3_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

// void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT2_Rx( void ) { ( *m_obj_func_i2c_dma_list_Rx[1] )(
// m_obj_i2c_dma_list_Rx[1] ); }

void uC_Base_STM32WB55R::uC_BASE_I2C_DMA_INT3_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}

void uC_Base_STM32WB55R::uC_BASE_I2C_INT1( void )    { ( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] ); }

// void uC_Base_STM32WB55R::uC_BASE_I2C_INT2( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }

void uC_Base_STM32WB55R::uC_BASE_I2C_INT3( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }


const uC_BASE_I2C_IO_STRUCT uC_Base_STM32WB55R::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1_REG1,
			RCC_APB1ENR1_I2C1EN
		},
		I2C1_EV_IRQn,
		nullptr,
		nullptr,

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
		I2C3,
		{
			RCC_PERIPH_CLCK_APB1_REG1,
			RCC_APB1ENR1_I2C3EN
		},
		I2C3_EV_IRQn,
		nullptr,
		nullptr,

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

// TODO: use PE0 for TIM16 and PE1 for TIM17 as CH1, Need to update PIN mapping for TIM16 and TIM17
uC_PERIPH_IO_STRUCT const* const m_timer16_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER5_PIO1_PA0, &TIMER5_PIO2_PA1, &TIMER5_PIO3_PA2, &TIMER5_PIO4_PA3 };
// uC_PERIPH_IO_STRUCT const* const m_timer17_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] = { &TIMER5_PIO1_PA0,
// &TIMER5_PIO2_PA1, &TIMER5_PIO3_PA2, &TIMER5_PIO4_PA3 };

uC_PERIPH_IO_STRUCT const* const m_timer17_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] = { &TIMER17_PIO1_PA7 };

uC_PERIPH_IO_STRUCT const* const m_lptimer1_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] = { &LPTIMER1_PIO1_PC1 };
uC_PERIPH_IO_STRUCT const* const m_lptimer2_pio[uC_BASE_1_MAX_TIMER_CAP_COMP_CH] = { &LPTIMER2_PIO1_PA4 };

const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32WB55R::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{			// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_TIMER1and8,
		TIM1_UP_TIM16_IRQn,
		TIM1_CC_IRQn,
		m_timer1_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM2,
		{
			RCC_PERIPH_CLCK_APB1_REG1,
			RCC_APB1ENR1_TIM2EN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_TIMERS2to7,
		TIM2_IRQn,
		TIM2_IRQn,
		m_timer2_pio,
		TMR_CNT_SIZE_32BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},

	{
		TIM16,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM16EN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_TIMER1and8,
		TIM1_UP_TIM16_IRQn,
		TIM1_UP_TIM16_IRQn,
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
		uC_BASE_STM32WB55R_PCLK_INDEX_APB2,
		TIM1_TRG_COM_TIM17_IRQn,
		TIM1_TRG_COM_TIM17_IRQn,
		m_timer17_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	}

	// TODO check for TIM17 post board bring up
	// {
	// TIM17,
	// {
	// RCC_PERIPH_CLCK_APB2,
	// RCC_APB2ENR_TIM16EN
	// },
	// uC_BASE_STM32WB55R_PCLK_INDEX_TIMER1and8,
	// TIM1_UP_TIM16_IRQn,
	// TIM1_UP_TIM16_IRQn,
	// m_timer16_pio,
	// TMR_CNT_SIZE_16BITS,
	// uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	// }
};

const uC_BASE_LPTIMER_IO_STRUCT uC_Base_STM32WB55R::m_lptimer_io[uC_BASE_MAX_LPTIMER_CTRLS] =
{
	{
		LPTIM1,
		{
			RCC_PERIPH_CLCK_APB1_REG1,
			RCC_APB1ENR1_LPTIM1EN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_APB1_REG1,
		LPTIM1_IRQn,
		LPTIM1_IRQn,
		m_lptimer1_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH
	},

	{
		LPTIM2,
		{
			RCC_PERIPH_CLCK_APB1_REG2,
			RCC_APB1ENR2_LPTIM2EN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_APB1_REG2,
		LPTIM2_IRQn,
		LPTIM2_IRQn,
		m_lptimer2_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH

	}

};
/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32WB55R::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
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
uC_PERIPH_IO_STRUCT const* const m_adc1_pio[uC_ATOD_CHANNEL_MAX] =
{
	&ADC1_IN0_PIO, &ADC1_IN1_PIO, &ADC1_IN2_PIO, &ADC1_IN3_PIO,
	&ADC1_IN4_PIO, &ADC1_IN5_PIO, &ADC1_IN6_PIO, &ADC1_IN7_PIO,
	&ADC1_IN8_PIO, &ADC1_IN9_PIO, &ADC1_IN10_PIO, &ADC1_IN11_PIO,
	&ADC1_IN12_PIO, &ADC1_IN13_PIO, &ADC1_IN14_PIO, &ADC1_IN15_PIO,
	&ADC1_IN16_PIO, &ADC1_IN17_PIO, &ADC1_IN18_PIO
};

const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32WB55R::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	/* There is only one ADC in STM32WB55R */
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_AHB2,
			RCC_AHB2ENR_ADCEN
		},
		uC_BASE_STM32WB55R_PCLK_INDEX_TIMER_ADC,
		ADC1_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc1_pio,
#ifdef ADC1_DMA_SELECT
		ADC1_DMA_SELECT
#else
		DMAMUX_REQ_ADC1
#endif
	},

};

/*
 ********************************
 *  DMA Definition Structure
 */


const uC_BASE_DMA_IO_STRUCT uC_Base_STM32WB55R::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
{
	{
		DMA1,
		DMA1_Channel1,
		DMAMUX1_Channel0,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel1_IRQn,
		1U
	},
	{
		DMA1,
		DMA1_Channel2,
		DMAMUX1_Channel1,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel2_IRQn,
		2U
	},
	{
		DMA1,
		DMA1_Channel3,
		DMAMUX1_Channel2,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel3_IRQn,
		3U
	},
	{
		DMA1,
		DMA1_Channel4,
		DMAMUX1_Channel3,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel4_IRQn,
		4U
	},
	{
		DMA1,
		DMA1_Channel5,
		DMAMUX1_Channel4,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel5_IRQn,
		5U
	},
	{
		DMA1,
		DMA1_Channel6,
		DMAMUX1_Channel5,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel6_IRQn,
		6U
	},
	{
		DMA1,
		DMA1_Channel7,
		DMAMUX1_Channel6,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA1_Channel7_IRQn,
		7U
	},
	{
		DMA2,
		DMA2_Channel1,
		DMAMUX1_Channel7,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel1_IRQn,
		1U
	},
	{
		DMA2,
		DMA2_Channel2,
		DMAMUX1_Channel8,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel2_IRQn,
		2U
	},
	{
		DMA2,
		DMA2_Channel3,
		DMAMUX1_Channel9,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel3_IRQn,
		3U
	},
	{
		DMA2,
		DMA2_Channel4,
		DMAMUX1_Channel10,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel4_IRQn,
		4U
	},
	{
		DMA2,
		DMA2_Channel5,
		DMAMUX1_Channel11,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel5_IRQn,
		5U
	},
	{
		DMA2,
		DMA2_Channel6,
		DMAMUX1_Channel12,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel6_IRQn,
		6U
	},
	{
		DMA2,
		DMA2_Channel7,
		DMAMUX1_Channel13,
		{
			RCC_PERIPH_CLCK_AHB1,
			RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_DMAMUX1EN
		},
		DMA2_Channel7_IRQn,
		7U
	}
};