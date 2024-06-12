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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
 */
/*lint -e1960  */


#include "Includes.h"
#include "uC_Base_STM32F103.h"
#include "uC_IO_Define_STM32F103.h"

#define UNDEFINED_PID       32



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32F103::m_pclock_freqs[uC_BASE_STM32F103_MAX_PCLKS] =
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
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32F103::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		GPIOA,
		{
			static_cast<uint8_t>( RCC_PERIPH_CLCK_APB2 ),
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
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT = { &USART1_TX_PIO, &USART1_RX_PIO };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT = { &USART2_TX_PIO, &USART2_RX_PIO };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT = { &USART3_TX_PIO, &USART3_RX_PIO };


const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART4_DEFAULT_PORT = { &USART4_TX_PIO, &USART4_RX_PIO };// #k:
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART5_DEFAULT_PORT = { &USART5_TX_PIO, &USART5_RX_PIO };// #k:

const uC_BASE_USART_IO_STRUCT uC_Base_STM32F103::m_usart_io[uC_BASE_MAX_USART_PORTS] =
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
	},
	{											// #k: New
		UART4,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_UART4EN
		},
		UART4_IRQn,
		uC_BASE_USART_PORT_4,
		uC_DMA1_CHANNEL_3,
		// uC_DMA1_CHANNEL_2,
		uC_DMA2_CHANNEL_5,					// Taken from 105

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
		uC_DMA_CHANNEL_DNE,
		uC_DMA_CHANNEL_DNE,

		&uC_BASE_USART5_DEFAULT_PORT
	}
};

/*
 ********************************
 *  SPI Definition Structure
 */
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI1_DEFAULT_IO_PORTS =
{ &SPI1_MOSI_PIO, &SPI1_MISO_PIO, &SPI1_CLOCK_PIO, &SPI1_SS_PIO };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI2_DEFAULT_IO_PORTS =
{ &SPI2_MOSI_PIO, &SPI2_MISO_PIO, &SPI2_CLOCK_PIO, &SPI2_SS_PIO };
// const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI3_DEFAULT_IO_PORTS =
// { &SPI3_MOSI_PIO, &SPI3_MISO_PIO, &SPI3_CLOCK_PIO, &SPI3_SS_PIO };
const uC_BASE_SPI_IO_STRUCT uC_Base_STM32F103::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
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
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2U;
const uint8_t TMR_CNT_SIZE_32BITS = 4U;
static uC_PERIPH_IO_STRUCT const* m_timer1_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER1_PIO1, &TIMER1_PIO2, &TIMER1_PIO3, &TIMER1_PIO4 };
static uC_PERIPH_IO_STRUCT const* m_timer2_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER2_PIO1, &TIMER2_PIO2, &TIMER2_PIO3, &TIMER2_PIO4 };
static uC_PERIPH_IO_STRUCT const* m_timer3_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER3_PIO1, &TIMER3_PIO2, &TIMER3_PIO3, &TIMER3_PIO4 };
static uC_PERIPH_IO_STRUCT const* m_timer4_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER4_PIO1, &TIMER4_PIO2, &TIMER4_PIO3, &TIMER4_PIO4 };
uC_PERIPH_IO_STRUCT const* m_timer5_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER5_PIO1, &TIMER5_PIO2, &TIMER5_PIO3, &TIMER5_PIO4 };
const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32F103::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{			// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32F103_PCLK_INDEX_TIMER1,
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
		uC_BASE_STM32F103_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F103_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F103_PCLK_INDEX_TIMERS2to7,
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
		uC_BASE_STM32F103_PCLK_INDEX_TIMERS2to7,
		TIM6_DAC_IRQn,
		TIM6_DAC_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM7,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM7EN
		},
		uC_BASE_STM32F103_PCLK_INDEX_TIMERS2to7,
		TIM7_IRQn,
		TIM7_IRQn,
		reinterpret_cast<uC_PERIPH_IO_STRUCT const**>( nullptr ),
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH
	}
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32F103::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
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
static uC_PERIPH_IO_STRUCT const* m_adc12_pio[uC_BASE_ATOD_MAX_CHANNELS] =
{
	&ADC12_IN0_PIO, &ADC12_IN1_PIO, &ADC12_IN2_PIO, &ADC12_IN3_PIO,
	&ADC12_IN4_PIO, &ADC12_IN5_PIO, &ADC12_IN6_PIO, &ADC12_IN7_PIO,
	&ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC12_IN10_PIO, &ADC12_IN11_PIO,
	&ADC12_IN12_PIO, &ADC12_IN13_PIO, &ADC12_IN14_PIO, &ADC12_IN15_PIO,
	&ADC12_IN16_PIO, &ADC12_IN17_PIO
};
const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32F103::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_ADC1EN
		},
		uC_BASE_STM32F103_PCLK_INDEX_TIMER_ADC,
		ADC1_2_IRQn,				// Sandip: For STM32F10X_HD, ADC1_IRQn is not valid
		// ADC1_2_IRQn, //Sandip: For STM32F10X_HD, ADC1_IRQn is not valid
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
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32F103::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
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
	// #k: New-------7Nov13-----------------
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
		DMA2_Channel4_5_IRQn,
		3
	},

	{
		DMA2,
		DMA2_Channel5,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA2EN
		},
		DMA2_Channel4_5_IRQn,
		4
	}

};
/*
 ********************************
 *  CAN Definition Structure
 */
const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN1_DEFAULT_IO_PORTS =
{ &CAN1_RX_PIO, &CAN1_TX_PIO};
#ifdef STM32F10X_CL
const uC_BASE_CAN_IO_PORT_STRUCT uC_BASE_CAN2_DEFAULT_IO_PORTS =
{ &CAN2_RX_PIO, &CAN2_TX_PIO};
#endif
const uC_BASE_CAN_IO_STRUCT uC_Base_STM32F103::m_can_io[uC_BASE_CAN_MAX_CTRLS] =
{
	{
		CAN1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_CAN1EN
		},
		USB_LP_CAN1_RX0_IRQn,
		USB_HP_CAN1_TX_IRQn,

		&uC_BASE_CAN1_DEFAULT_IO_PORTS,
	},
#ifdef STM32F10X_CL
	{
		CAN2,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_CAN2EN
		},
		CAN2_RX0_IRQn,
		CAN2_TX_IRQn,

		&uC_BASE_CAN2_DEFAULT_IO_PORTS,
	},
#endif
};


/* lint +e1924
   lint +e1960*/
