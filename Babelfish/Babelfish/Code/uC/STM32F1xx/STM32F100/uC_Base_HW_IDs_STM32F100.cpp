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
#include "Includes.h"
#include "uC_Base_STM32F100.h"
#include "uC_IO_Define_STM32F100.h"
// #include "stm32f10x.h"

#define UNDEFINED_PID       32



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32F100::m_pclock_freqs[uC_BASE_STM32F100_MAX_PCLKS] =
{
	AHB_PERIPH_CLOCK_FREQ,
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
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32F100::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_IOPAEN
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
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_IOPBEN
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
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_IOPCEN
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
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_IOPDEN
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
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_IOPEEN
		},
		&SET_PIO_E_OUTPUT,
		&SET_PIO_E_OUTPUT_INIT,
		&SET_PIO_E_INPUT,
		&SET_PIO_E_INPUT_PULLUP,
		&SET_PIO_E_INPUT_PULLDOWN
	}
};


/*
 ********************************
 *  USART Definition Structure
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT = { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO_PA2, &USART2_RX_PIO_PA3 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO_PB10, &USART3_RX_PIO_PB11 };
const uC_BASE_USART_IO_STRUCT uC_Base_STM32F100::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		USART1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART1EN
		},
		USART1_IRQn,
		uC_BASE_USART_PORT_1,
		uC_DMA1_CHANNEL_5,
		uC_DMA1_CHANNEL_4,

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
		uC_DMA1_CHANNEL_6,
		uC_DMA1_CHANNEL_7,

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
		uC_DMA1_CHANNEL_3,
		uC_DMA1_CHANNEL_2,

		&uC_BASE_USART3_DEFAULT_PORT
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
const uC_BASE_SPI_IO_STRUCT uC_Base_STM32F100::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
{
	{
		SPI1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_SPI1EN
		},
		SPI1_IRQn,
		uC_DMA1_CHANNEL_2,
		uC_DMA1_CHANNEL_3,

		&uC_BASE_SPI1_DEFAULT_IO_PORTS
	},
	{
		SPI2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_SPI2EN
		},
		SPI2_IRQn,
		uC_DMA1_CHANNEL_4,
		uC_DMA1_CHANNEL_5,

		&uC_BASE_SPI2_DEFAULT_IO_PORTS
	},
};

/*
 ********************************
 *  I2C Definition Structure
 */

const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C1_DEFAULT_IO_PORTS =
{ &I2C1_SCL_PIO_PB6, &I2C1_SDA_PIO_PB7 };
const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C2_DEFAULT_IO_PORTS =
{ &I2C2_SCL_PIO_PB10, &I2C2_SDA_PIO_PB11 };

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F100::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F100::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F100::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F100::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F100::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F100::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];

void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT2_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}

// void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT3_Tx( void )	{ (*m_obj_func_i2c_dma_list_Tx[2])( m_obj_i2c_dma_list_Tx[2]
// ); }

void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT2_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}

// void uC_Base_STM32F100::uC_BASE_I2C_DMA_INT3_Rx( void )	{ (*m_obj_func_i2c_dma_list_Rx[2])( m_obj_i2c_dma_list_Rx[2]
// ); }

void uC_Base_STM32F100::uC_BASE_I2C_INT1( void )    { ( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] ); }

void uC_Base_STM32F100::uC_BASE_I2C_INT2( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }

// void uC_Base_STM32F100::uC_BASE_I2C_INT3( void )	{ (*m_obj_func_i2c_list[2])( m_obj_i2c_list[2] ); }


const uC_BASE_I2C_IO_STRUCT uC_Base_STM32F100::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C1EN
		},
		I2C1_EV_IRQn,
		uC_DMA1_CHANNEL_7,
		uC_DMA1_CHANNEL_6,

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
		uC_DMA1_CHANNEL_5,
		uC_DMA1_CHANNEL_4,

		&uC_BASE_I2C2_DEFAULT_IO_PORTS,

		&m_obj_func_i2c_dma_list_Tx[1],
		&m_obj_i2c_dma_list_Tx[1],
		uC_BASE_I2C_DMA_INT1_Tx,
		&m_obj_func_i2c_dma_list_Rx[1],
		&m_obj_i2c_dma_list_Rx[1],
		uC_BASE_I2C_DMA_INT1_Rx,
		&m_obj_func_i2c_list[1],
		&m_obj_i2c_list[1],
		uC_BASE_I2C_INT2
	},


};


/*
 ********************************
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2;
uC_PERIPH_IO_STRUCT const* const m_timer1_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER1_PIO1_PA8, &TIMER1_PIO2_PA9, &TIMER1_PIO3_PA10, &TIMER1_PIO4_PA11 };
uC_PERIPH_IO_STRUCT const* const m_timer2_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER2_PIO1_PA0, &TIMER2_PIO2_PA1, &TIMER2_PIO3_PA2, &TIMER2_PIO4_PA3 };
uC_PERIPH_IO_STRUCT const* const m_timer3_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER3_PIO1_PA6, &TIMER3_PIO2_PA7, &TIMER3_PIO3_PB0, &TIMER3_PIO4_PB1 };
uC_PERIPH_IO_STRUCT const* const m_timer4_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER4_PIO1_PB6, &TIMER4_PIO2_PB7, &TIMER4_PIO3_PB8, &TIMER4_PIO4_PB9 };
// uC_PERIPH_IO_STRUCT const* m_timer5_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER5_PIO1_PB14, &TIMER5_PIO2, &TIMER5_PIO3, &TIMER5_PIO4 };
const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32F100::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{			// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32F100_PCLK_INDEX_TIMER1,
		TIM1_UP_TIM16_IRQn,
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
		uC_BASE_STM32F100_PCLK_INDEX_TIMERS2to7,
		TIM2_IRQn,
		TIM2_IRQn,
		m_timer2_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM3EN
		},
		uC_BASE_STM32F100_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F100_PCLK_INDEX_TIMERS2to7,
		TIM4_IRQn,
		TIM4_IRQn,
		m_timer4_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM6,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM6EN
		},
		uC_BASE_STM32F100_PCLK_INDEX_TIMERS2to7,
		TIM6_DAC_IRQn,
		TIM6_DAC_IRQn,
		NULL,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM7,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM7EN
		},
		uC_BASE_STM32F100_PCLK_INDEX_TIMERS2to7,
		TIM7_IRQn,
		TIM7_IRQn,
		NULL,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	}
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32F100::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
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
#define uC_BASE_ATOD_MAX_CHANNELS           18
uC_PERIPH_IO_STRUCT const* m_adc12_pio[uC_BASE_ATOD_MAX_CHANNELS] =
{
	&ADC12_IN0_PIO_PA0, &ADC12_IN1_PIO_PA1, &ADC12_IN2_PIO_PA2, &ADC12_IN3_PIO_PA3,
	&ADC12_IN4_PIO_PA4, &ADC12_IN5_PIO_PA5, &ADC12_IN6_PIO_PA6, &ADC12_IN7_PIO_PA7,
	&ADC12_IN8_PIO_PB0, &ADC12_IN9_PIO_PB1, &ADC12_IN10_PIO_PC0, &ADC12_IN11_PIO_PC1,
	&ADC12_IN12_PIO_PC2, &ADC12_IN13_PIO_PC3, &ADC12_IN14_PIO_PC4, &ADC12_IN15_PIO_PC5,
	&ADC12_IN16_PIO, &ADC12_IN17_PIO
};
const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32F100::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_ADC1EN
		},
		uC_BASE_STM32F100_PCLK_INDEX_TIMER_ADC,
		ADC1_IRQn,		// Parrish ADC1_2_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc12_pio,
		uC_BASE_ATOD_MAX_CHANNELS,
		uC_DMA1_CHANNEL_1
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
	// }
};

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32F100::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
{
	{
		DMA1,
		DMA1_Channel1,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel1_IRQn,
		0
	},
	{
		DMA1,
		DMA1_Channel2,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel2_IRQn,
		1
	},
	{
		DMA1,
		DMA1_Channel3,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel3_IRQn,
		2
	},
	{
		DMA1,
		DMA1_Channel4,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel4_IRQn,
		3
	},
	{
		DMA1,
		DMA1_Channel5,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel5_IRQn,
		4
	},
	{
		DMA1,
		DMA1_Channel6,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel6_IRQn,
		5
	},
	{
		DMA1,
		DMA1_Channel7,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel7_IRQn,
		6
	}
};

