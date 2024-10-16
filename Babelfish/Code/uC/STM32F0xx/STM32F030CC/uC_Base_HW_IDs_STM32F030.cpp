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
#include "uC_Base_STM32F030.h"
#include "uC_IO_Define_STM32F030.h"
#include "stm32f0xx_hal_tim.h"

#define UNDEFINED_PID       32
#define DMA_CHANNEL( CHANNEL_NUMBER )   ( CHANNEL_NUMBER << 4 )

/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const uint32_t uC_Base_STM32F030::m_pclock_freqs[uC_BASE_STM32F030_MAX_PCLKS] =
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
const uC_BASE_PIO_CTRL_STRUCT uC_Base_STM32F030::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
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
		GPIOF,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_GPIOFEN
		},
		&SET_PIO_F_OUTPUT,
		&SET_PIO_F_OUTPUT_INIT,
		&SET_PIO_F_INPUT,
		&SET_PIO_F_INPUT_PULLUP,
		&SET_PIO_F_INPUT_PULLDOWN
	}
};

/*
 ********************************
 *  USART Definition Structure
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT =
{ &USART1_TX_PIO_PA9,
  &USART1_RX_PIO_PA10 };

const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT =
{ &USART2_TX_PIO_PA2,
  &USART2_RX_PIO_PA3 };

const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT =
{ &USART3_TX_PIO_PB10,
  &USART3_RX_PIO_PB11 };

const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART4_DEFAULT_PORT =
{ &USART4_TX_PIO_PA0,
  &USART4_RX_PIO_PA1 };

const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART5_DEFAULT_PORT =
{ &USART5_TX_PIO_PB3,
  &USART5_RX_PIO_PB4 };

const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART6_DEFAULT_PORT =
{ &USART6_TX_PIO_PC0,
  &USART6_RX_PIO_PC1 };

const uC_BASE_USART_IO_STRUCT uC_Base_STM32F030::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		USART1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART1EN
		},
		USART1_IRQn,
		uC_BASE_USART_PORT_1,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_3 ) + uC_DMA1_CHN_SELVAL_0 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_0 ),
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
		( DMA_CHANNEL( uC_DMA1_CHANNEL_5 ) + uC_DMA1_CHN_SELVAL_0 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_4 ) + uC_DMA1_CHN_SELVAL_0 ),
		&uC_BASE_USART2_DEFAULT_PORT
	},
	{
		USART3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_USART3EN
		},
		USART3_6_IRQn,
		uC_BASE_USART_PORT_3,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_1 ) + uC_DMA1_CHN_SELVAL_10 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_10 ),
		&uC_BASE_USART3_DEFAULT_PORT
	},
	{
		USART4,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_USART4EN
		},
		USART3_6_IRQn,
		uC_BASE_USART_PORT_4,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_1 ) + uC_DMA1_CHN_SELVAL_11 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_11 ),
		&uC_BASE_USART4_DEFAULT_PORT
	},
	{
		USART5,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_USART5EN
		},
		USART3_6_IRQn,
		uC_BASE_USART_PORT_5,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_1 ) + uC_DMA1_CHN_SELVAL_12 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_12 ),
		&uC_BASE_USART5_DEFAULT_PORT
	},
	{
		USART6,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_USART6EN
		},
		USART3_6_IRQn,
		uC_BASE_USART_PORT_6,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_1 ) + uC_DMA1_CHN_SELVAL_13 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_13 ),
		&uC_BASE_USART6_DEFAULT_PORT
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
//
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F030::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F030::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F030::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F030::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

// Stores the function pointer of the objects static interrupt handler.
uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_STM32F030::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
// Stores the object pointer associated with the interrupt.
uC_BASE_I2C_OBJ_PARAM_TD uC_Base_STM32F030::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];
//
void uC_Base_STM32F030::uC_BASE_I2C_DMA_INT1_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[0] )( m_obj_i2c_dma_list_Tx[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32F030::uC_BASE_I2C_DMA_INT2_Tx( void )
{
	( *m_obj_func_i2c_dma_list_Tx[1] )( m_obj_i2c_dma_list_Tx[1] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32F030::uC_BASE_I2C_DMA_INT1_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[0] )( m_obj_i2c_dma_list_Rx[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32F030::uC_BASE_I2C_DMA_INT2_Rx( void )
{
	( *m_obj_func_i2c_dma_list_Rx[1] )( m_obj_i2c_dma_list_Rx[1] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32F030::uC_BASE_I2C_INT1( void )
{
	( *m_obj_func_i2c_list[0] )( m_obj_i2c_list[0] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Base_STM32F030::uC_BASE_I2C_INT2( void )
{
	( *m_obj_func_i2c_list[1] )( m_obj_i2c_list[1] );
}

const uC_BASE_I2C_IO_STRUCT uC_Base_STM32F030::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
{
	{
		I2C1,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_I2C1EN
		},
		// I2C1_EV_IRQn,
		I2C1_IRQn,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_3 ) + uC_DMA1_CHN_SELVAL_0 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_2 ) + uC_DMA1_CHN_SELVAL_0 ),

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
		// I2C1_EV_IRQn,
		I2C2_IRQn,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_5 ) + uC_DMA1_CHN_SELVAL_0 ),
		( DMA_CHANNEL( uC_DMA1_CHANNEL_4 ) + uC_DMA1_CHN_SELVAL_0 ),

		&uC_BASE_I2C2_DEFAULT_IO_PORTS,

		&m_obj_func_i2c_dma_list_Tx[0],
		&m_obj_i2c_dma_list_Tx[0],
		uC_BASE_I2C_DMA_INT2_Tx,
		&m_obj_func_i2c_dma_list_Rx[0],
		&m_obj_i2c_dma_list_Rx[0],
		uC_BASE_I2C_DMA_INT1_Rx,

		&m_obj_func_i2c_list[0],
		&m_obj_i2c_list[0],
		uC_BASE_I2C_INT2
	},
};

/*
 ********************************
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2;

uC_PERIPH_IO_STRUCT const* const m_timer1_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER1_PIO1_PA8, &TIMER1_PIO4_PA11 };

uC_PERIPH_IO_STRUCT const* const m_timer3_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER3_PIO1_PB4, &TIMER3_PIO2_PA7, &TIMER3_PIO3_PB0, &TIMER3_PIO4_PB1 };

uC_PERIPH_IO_STRUCT const* const m_timer14_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER14_PIO1_PA7};

uC_PERIPH_IO_STRUCT const* const m_timer15_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER15_PIO1_PA2, &TIMER15_PIO2_PB15};

uC_PERIPH_IO_STRUCT const* const m_timer16_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER16_PIO1_PA6};

uC_PERIPH_IO_STRUCT const* const m_timer17_pio[uC_BASE_MAX_TIMER_CAP_COMP_CH] =
{ &TIMER17_PIO1_PB9};

const uC_BASE_TIMER_IO_STRUCT uC_Base_STM32F030::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{				// Timer Control Block 0
		TIM1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM1EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMER1,
		TIM1_BRK_UP_TRG_COM_IRQn,
		TIM1_CC_IRQn,
		m_timer1_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			},
			{
				uC_BASE_TIMER_CAP_COMP_CH2,
				TIM_ICSELECTION_INDIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0

			},
			{
				uC_BASE_TIMER_CAP_COMP_CH3,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0

			},
			{
				uC_BASE_TIMER_CAP_COMP_CH4,
				TIM_ICSELECTION_INDIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			}
		},
	},
	{
		TIM3,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM3EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM3_IRQn,
		TIM3_IRQn,
		m_timer3_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_4_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			},
			{
				uC_BASE_TIMER_CAP_COMP_CH2,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0

			},
			{
				uC_BASE_TIMER_CAP_COMP_CH3,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0

			},
			{
				uC_BASE_TIMER_CAP_COMP_CH4,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			}
		},
	},
	{
		TIM6,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM6EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM6_IRQn,
		TIM6_IRQn,
		NULL,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM7,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM7EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM7_IRQn,
		TIM7_IRQn,
		NULL,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_0_MAX_TIMER_CAP_COMP_CH
	},
	{
		TIM14,
		{
			RCC_PERIPH_CLCK_APB1,
			RCC_APB1ENR_TIM14EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM14_IRQn,
		TIM14_IRQn,
		m_timer14_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			}
		},
	},
	{
		TIM15,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM15EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM15_IRQn,
		TIM15_IRQn,
		m_timer15_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_2_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			},
			{
				uC_BASE_TIMER_CAP_COMP_CH2,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0

			}
		},
	},
	{
		TIM16,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM16EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM16_IRQn,
		TIM16_IRQn,
		m_timer16_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			}
		},
	},
	{
		TIM17,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_TIM17EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMERS2to7,
		TIM17_IRQn,
		TIM17_IRQn,
		m_timer17_pio,
		TMR_CNT_SIZE_16BITS,
		uC_BASE_1_MAX_TIMER_CAP_COMP_CH,
		{
			{
				uC_BASE_TIMER_CAP_COMP_CH1,
				TIM_ICSELECTION_DIRECTTI,
				TIM_ICPSC_DIV1,
				uC_INPUT_CAPT_FILTER_0
			}
		},
	},
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_STM32F030::m_ext_ints[uC_BASE_EXT_INT_MAX_INPUTS] =
{
	{
		EXTI0_1_IRQn,
		0x0003
	},
	{
		EXTI2_3_IRQn,
		0x000C
	},
	{
		EXTI4_15_IRQn,
		0xFFF0
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
const uC_BASE_ATOD_IO_STRUCT uC_Base_STM32F030::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
{
	{
		ADC1,
		{
			RCC_PERIPH_CLCK_APB2,
			RCC_APB2ENR_ADC1EN
		},
		uC_BASE_STM32F030_PCLK_INDEX_TIMER_ADC,
		ADC1_IRQn,				// Parrish ADC1_2_IRQn,
		uC_ATOD_CHANNEL_MAX,
		m_adc12_pio,
		uC_BASE_ATOD_MAX_CHANNELS,
		( DMA_CHANNEL( uC_DMA1_CHANNEL_1 ) + uC_DMA1_CHN_SELVAL_0 )
	}
};

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_STM32F030::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
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
		DMA1_Channel2_3_IRQn,
		1
	},
	{
		DMA1,
		DMA1_Channel3,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel2_3_IRQn,
		2
	},
	{
		DMA1,
		DMA1_Channel4,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel4_5_IRQn,
		3
	},
	{
		DMA1,
		DMA1_Channel5,
		{
			RCC_PERIPH_CLCK_AHB,
			RCC_AHBENR_DMA1EN
		},
		DMA1_Channel4_5_IRQn,
		4
	},

};

