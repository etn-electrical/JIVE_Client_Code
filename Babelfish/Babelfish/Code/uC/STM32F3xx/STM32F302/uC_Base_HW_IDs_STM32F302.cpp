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
#include "uC_Base_STM32F302.h"
#include "uC_IO_Define_STM32F302.h"
#define UNDEFINED_PID       32

/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32F302::m_pclock_freqs[uC_BASE_STM32F302_MAX_PCLKS] =
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
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32F302::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOAEN
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
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOBEN
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
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOCEN
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
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIODEN
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
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOEEN
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
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOFEN
		},
		&SET_PIO_F_OUTPUT,
		&SET_PIO_F_OUTPUT_INIT,
		&SET_PIO_F_INPUT,
		&SET_PIO_F_INPUT_PULLUP
	}
};
/*
 ********************************
 * USB Definition Structure
 */
const uC_BASE_USB_IO_PORT_STRUCT uC_Base_USB_FS_IO_PORT =
{&USB_FS_DM, &USB_FS_DP, &USB_FS_ID, &USB_FS_VBUS};


const uC_BASE_USB_IO_STRUCT uC_Base_STM32F302::m_usb_io[uC_BASE_MAX_USB_PORTS] =
{
	{
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_AHB ),
			RCC_APB1ENR_USBEN
		},
		USB_HP_IRQn,
		&uC_Base_USB_FS_IO_PORT
	},
};


/*
 ********************************
 *  USART Definition Structure
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT = { &USART1_TX_PIO_PA9, &USART1_RX_PIO_PA10 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO_PA2, &USART2_RX_PIO_PA3 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO_PB10, &USART3_RX_PIO_PC11 };
const uC_BASE_USART_IO_STRUCT uC_Base_STM32F302::m_usart_io[uC_BASE_MAX_USART_PORTS] =
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
{ &SPI2_MOSI_PIO_PA10, &SPI2_MISO_PIO_PA9, &SPI2_CLOCK_PIO_PA8, &SPI2_SS_PIO_PA11 };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI3_DEFAULT_IO_PORTS =
{ &SPI3_MOSI_PIO_PB5, &SPI3_MISO_PIO_PB4, &SPI3_CLOCK_PIO_PB3, &SPI3_SS_PIO_PA15 };
const uC_BASE_SPI_IO_STRUCT uC_Base_STM32F302::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
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
	{
		SPI3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_SPI3EN
		},
		SPI3_IRQn,
		uC_DMA2_CHANNEL_1,
		uC_DMA2_CHANNEL_2,

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
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F302::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F302::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F302::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F302::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];


// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F302::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F302::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];

void uC_Base_STM32F302::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

void uC_Base_STM32F302::uC_BASE_I2C_DMA_INT2_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}


void uC_Base_STM32F302::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

void uC_Base_STM32F302::uC_BASE_I2C_DMA_INT2_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}


void uC_Base_STM32F302::uC_BASE_I2C_INT1( void )    { ( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] ); }

void uC_Base_STM32F302::uC_BASE_I2C_INT2( void )    { ( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] ); }



const uC_BASE_I2C_IO_STRUCT uC_Base_STM32F302::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C1EN
		},
		I2C1_EV_IRQn,
		uC_DMA2_CHANNEL_1,
		uC_DMA2_CHANNEL_2,

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
		uC_DMA2_CHANNEL_3,
		uC_DMA2_CHANNEL_4,

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
	}
};
/*
 ********************************
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2;
const uint8_t TMR_CNT_SIZE_32BITS = 4;
uC_PERIPH_IO_STRUCT const* const m_timer1_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER1_PIO1_PA8, &TIMER1_PIO2_PA9, &TIMER1_PIO3_PA10, &TIMER1_PIO4_PA11 };
uC_PERIPH_IO_STRUCT const* const m_timer2_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER2_PIO1_PA0, &TIMER2_PIO2_PA1, &TIMER2_PIO3_PA2, &TIMER2_PIO4_PA3 };
uC_PERIPH_IO_STRUCT const* const m_timer3_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER3_PIO1_PC6, &TIMER3_PIO2_PC7, &TIMER3_PIO3_PC8, &TIMER3_PIO4_PC9 };
uC_PERIPH_IO_STRUCT const* const m_timer4_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER4_PIO1_PB6, &TIMER4_PIO2_PB7, &TIMER4_PIO3_PB8, &TIMER4_PIO4_PB9 };
// uC_PERIPH_IO_STRUCT const* const m_timer6_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER6_PIO1_PA0, &TIMER6_PIO2_PA1, &TIMER6_PIO3_PA2, &TIMER6_PIO4_PA3 };
const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32F302::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{			// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMER1,
		TIM1_UP_TIM16_IRQn,								/*This is Just the update interrupt of TIM1.
														   Timer 1 has other interrupts like CC*/

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
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
		TIM6_DAC1_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM15,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM15EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
		TIM15_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM16,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM16EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
		TIM16_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM17,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM17EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMERS2to7,
		TIM17_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32F302::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
{
	{
		EXTI0_IRQn,
		0x0001
	},
	{
		EXTI1_IRQn,
		0x0002
	},
	// {
	// EXTI2_TS_IRQn,
	// 0x0004
	// },
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

static uC_PERIPH_IO_STRUCT const* m_adc1_pio[uC_ATOD_CHANNEL_MAX] =
{
	&ADC12_IN0_PIO, &ADC1_IN1_PIO, &ADC1_IN2_PIO, &ADC1_IN3_PIO,
	&ADC1_IN4_PIO, &ADC1_IN5_PIO, &ADC12_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC12_IN10_PIO, &ADC1_IN11_PIO,
	&ADC1_IN12_PIO, &ADC12_IN13_PIO, &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	&ADC12_IN16_PIO, &ADC12_IN17_PIO
};

static uC_PERIPH_IO_STRUCT const* m_adc2_pio[uC_ATOD_CHANNEL_MAX] =
{
	&ADC12_IN0_PIO, &ADC2_IN1_PIO, &ADC2_IN2_PIO, &ADC2_IN3_PIO,
	&ADC2_IN4_PIO, &ADC2_IN5_PIO, &ADC12_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC12_IN10_PIO, &ADC2_IN11_PIO,
	&ADC2_IN12_PIO, &ADC12_IN13_PIO, &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	&ADC12_IN16_PIO, &ADC12_IN17_PIO
};

const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32F302::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_ADC12EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMER_ADC,
		ADC1_IRQn,		// Parrish ADC1_2_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc1_pio,
		uC_DMA1_CHANNEL_1
	},
	{
		ADC2,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_ADC12EN
		},
		uC_BASE_STM32F302_PCLK_INDEX_TIMER_ADC,
		ADC1_IRQn,		// Parrish ADC1_2_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc2_pio,
		uC_DMA2_CHANNEL_1
	}
};

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32F302::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
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
	},
	{
		DMA2,
		DMA2_Channel1,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel1_IRQn,
		0
	},
	{
		DMA2,
		DMA2_Channel2,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel2_IRQn,
		1
	},
	{
		DMA2,
		DMA2_Channel3,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel3_IRQn,
		2
	},
	{
		DMA2,
		DMA2_Channel4,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel4_IRQn,
		3
	},
	{
		DMA2,
		DMA2_Channel5,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel5_IRQn,
		4
	}
};

/*
 ********************************
 *  CAN Definition Structure
 */

const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN_TX_PIO_PA12, &CAN_RX_PIO_PA11};
// const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN1_DEFAULT_IO_PORTS = { &CAN1_RX_PIO_PB8,
// &CAN1_TX_PIO_PB9};
// const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN_DEFAULT_IO_PORTS = { &CAN_RX_PIO_PD0,
// &CAN_TX_PIO_PD1};


const uC_BASE_CAN_IO_STRUCT uC_Base_STM32F302::m_can_io[uC_BASE_CAN_MAX_CTRLS] =
{
	CAN,
	{
		RCC_PERIPH_CLCK_APB1,
		RCC_APB1ENR_CANEN
	},
	CAN_RX0_IRQn,			/*!< CAN RX0 Interrupt */
	CAN_TX_IRQn,			/*!< CAN TX Interrupt  */
	&uC_BASE_CAN_DEFAULT_IO_PORTS
};


/* lint +e1924
  lint +e1960*/
