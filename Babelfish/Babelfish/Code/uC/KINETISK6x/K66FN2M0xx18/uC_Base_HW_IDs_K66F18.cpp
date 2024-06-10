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
#include "uC_Base_K66F18.h"
#include "uC_IO_Define_K66F18.h"

#define UNDEFINED_PID       32



/*
 ********************************
 *  Peripheral Clock Definitions
 */
// The first three of these freqs have to match the base clocks.
// The other clocks can be defined here.
const UINT32 uC_Base_K66F18::m_pclock_freqs[uC_BASE_K66F18_MAX_PCLKS] =
{
	AHB1_PERIPH_CLOCK_FREQ,	// TODO: k60 doesn't really have this concept, each peripheral is clock gated
};

/*
 ********************************
 *  PIO Ctrl Definition Structure
 */
const uC_BASE_PIO_CTRL_STRUCT uC_Base_K66F18::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS] =
{
	{
		PTA,
		PORTA,
		{
			SIM_SCGC5,
			SIM_SCGC5_PORTA_MASK
		},
		&SET_PIO_A_OUTPUT,
		&SET_PIO_A_OUTPUT_INIT,
		&SET_PIO_A_INPUT,
		&SET_PIO_A_INPUT_PULLUP,
		&SET_PIO_A_INPUT_PULLDOWN
	},
	{
		PTB,
		PORTB,
		{
			SIM_SCGC5,
			SIM_SCGC5_PORTB_MASK
		},
		&SET_PIO_B_OUTPUT,
		&SET_PIO_B_OUTPUT_INIT,
		&SET_PIO_B_INPUT,
		&SET_PIO_B_INPUT_PULLUP,
		&SET_PIO_B_INPUT_PULLDOWN
	},
	{
		PTC,
		PORTC,
		{
			SIM_SCGC5,
			SIM_SCGC5_PORTC_MASK
		},
		&SET_PIO_C_OUTPUT,
		&SET_PIO_C_OUTPUT_INIT,
		&SET_PIO_C_INPUT,
		&SET_PIO_C_INPUT_PULLUP,
		&SET_PIO_C_INPUT_PULLDOWN
	},
	{
		PTD,
		PORTD,
		{
			SIM_SCGC5,
			SIM_SCGC5_PORTD_MASK
		},
		&SET_PIO_D_OUTPUT,
		&SET_PIO_D_OUTPUT_INIT,
		&SET_PIO_D_INPUT,
		&SET_PIO_D_INPUT_PULLUP,
		&SET_PIO_D_INPUT_PULLDOWN
	},
	{
		PTE,
		PORTE,
		{
			SIM_SCGC5,
			SIM_SCGC5_PORTE_MASK
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
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART0_DEFAULT_PORT =
{ &USART0_TX_PIO_PD7,  &USART0_RX_PIO_PD6,  &USART0_RTS_PIO_PD4 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART1_DEFAULT_PORT =
{ &USART1_TX_PIO_PC4,  &USART1_RX_PIO_PC3,  &USART1_RTS_PIO_PC1 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART2_DEFAULT_PORT =
{ &USART2_TX_PIO_PD3,  &USART2_RX_PIO_PD2,  &USART2_RTS_PIO_PD0 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART3_DEFAULT_PORT =
{ &USART3_TX_PIO_PC17, &USART3_RX_PIO_PC16, &USART3_RTS_PIO_PC18 };
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART4_DEFAULT_PORT =
{ &USART4_TX_PIO_PC15, &USART4_RX_PIO_PC14, &USART4_RTS_PIO_PC12 };

const uC_BASE_USART_IO_STRUCT uC_Base_K66F18::m_usart_io[uC_BASE_MAX_USART_PORTS] =
{
	{
		UART0,
		{
			SIM_SCGC4,
			SIM_SCGC4_UART0_MASK
		},
		UART0_RX_TX_IRQn,
		uC_BASE_USART_PORT_0,
		DMA_USART0RX,
		DMA_USART0TX,
		&uC_BASE_USART0_DEFAULT_PORT
	},
	{
		UART1,
		{
			SIM_SCGC4,
			SIM_SCGC4_UART1_MASK
		},
		UART1_RX_TX_IRQn,
		uC_BASE_USART_PORT_1,
		DMA_USART1RX,
		DMA_USART1TX,
		&uC_BASE_USART1_DEFAULT_PORT
	},
	{
		UART2,
		{
			SIM_SCGC4,
			SIM_SCGC4_UART2_MASK
		},
		UART2_RX_TX_IRQn,
		uC_BASE_USART_PORT_2,
		DMA_USART2RX,
		DMA_USART2TX,
		&uC_BASE_USART2_DEFAULT_PORT
	},
	{
		UART3,
		{
			SIM_SCGC4,
			SIM_SCGC4_UART3_MASK
		},
		UART3_RX_TX_IRQn,
		uC_BASE_USART_PORT_3,
		DMA_USART3RX,
		DMA_USART3TX,
		&uC_BASE_USART3_DEFAULT_PORT
	},
	{
		UART4,
		{
			SIM_SCGC1,
			SIM_SCGC1_UART4_MASK
		},
		UART4_RX_TX_IRQn,
		uC_BASE_USART_PORT_4,
		DMA_USART4RX,
		DMA_USART4TX,
		&uC_BASE_USART4_DEFAULT_PORT
	},
	/* MB:NOT Present in MK66FN2M0VMF18
	   {
	    UART5,
	    {
	      SIM_SCGC1,
	      SIM_SCGC1_UART5_MASK
	    },
	    UART5_RX_TX_IRQn,
	    uC_BASE_USART_PORT_5,
	    uC_DMA_STREAM_DNE, // unused
	    uC_DMA_STREAM_DNE, // unused
	    &uC_BASE_USART5_DEFAULT_PORT
	   }*/
};

/*
 ********************************
 *  SPI Definition Structure
 */
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI0_DEFAULT_IO_PORTS =
{ &PERIPH_NULL_PIO,    &PERIPH_NULL_PIO,    &PERIPH_NULL_PIO,     &PERIPH_NULL_PIO };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI1_DEFAULT_IO_PORTS =
{ &PERIPH_NULL_PIO,    &PERIPH_NULL_PIO,    &PERIPH_NULL_PIO,     &PERIPH_NULL_PIO };
const uC_BASE_SPI_IO_PORT_STRUCT uC_BASE_SPI2_DEFAULT_IO_PORTS =
{ &SPI2_MOSI_PIO_PD13, &SPI2_MISO_PIO_PD14, &SPI2_CLOCK_PIO_PD12, &SPI2_PCS0_PIO_PD11 };

const uC_BASE_SPI_IO_STRUCT uC_Base_K66F18::m_spi_io[uC_BASE_SPI_MAX_CTRLS] =
{
	{
		SPI0,
		{
			SIM_SCGC6,
			SIM_SCGC6_SPI0_MASK
		},
		SPI0_IRQn,
		uC_DMA_STREAM_DNE,
		uC_DMA_STREAM_DNE,
		&uC_BASE_SPI0_DEFAULT_IO_PORTS
	},
	{
		SPI1,
		{
			SIM_SCGC6,
			SIM_SCGC6_SPI1_MASK
		},
		SPI1_IRQn,
		uC_DMA_STREAM_DNE,
		uC_DMA_STREAM_DNE,
		&uC_BASE_SPI1_DEFAULT_IO_PORTS
	},
	{
		SPI2,
		{
			SIM_SCGC3,
			SIM_SCGC3_SPI2_MASK
		},
		SPI2_IRQn,
		DMA_SPI2RX,
		DMA_SPI2TX,
		&uC_BASE_SPI2_DEFAULT_IO_PORTS,
	}
};


/*
 ********************************
 *  I2C Definition Structure
 */
// const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C1_DEFAULT_IO_PORTS =
// { &I2C1_SCL_PIO_PB8,&I2C1_SDA_PIO_PB9 };
// const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C2_DEFAULT_IO_PORTS =
// { &I2C2_SCL_PIO_PF1, &I2C2_SDA_PIO_PF0 };
// const uC_BASE_I2C_IO_PORT_STRUCT uC_BASE_I2C3_DEFAULT_IO_PORTS =
// { &I2C3_SCL_PIO_PA8,&I2C3_SDA_PIO_PC9 };
////Stores the function pointer of the objects static interrupt handler.
// uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_K66F18::m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
////Stores the function pointer of the objects static interrupt handler.
// uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_K66F18::m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
////Stores the object pointer associated with the interrupt.
// uC_BASE_I2C_OBJ_PARAM_TD uC_Base_K66F18::m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
////Stores the object pointer associated with the interrupt.
// uC_BASE_I2C_OBJ_PARAM_TD uC_Base_K66F18::m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
//
////Stores the function pointer of the objects static interrupt handler.
// uC_BASE_I2C_INT_OBJ_CBACK_FUNC* uC_Base_K66F18::m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
////Stores the object pointer associated with the interrupt.
// uC_BASE_I2C_OBJ_PARAM_TD uC_Base_K66F18::m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];
//
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT1_Tx( void )	{ (*m_obj_func_i2c_dma_list_Tx[0])( m_obj_i2c_dma_list_Tx[0] );
// }
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT2_Tx( void )	{ (*m_obj_func_i2c_dma_list_Tx[1])( m_obj_i2c_dma_list_Tx[1] );
// }
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT3_Tx( void )	{ (*m_obj_func_i2c_dma_list_Tx[2])( m_obj_i2c_dma_list_Tx[2] );
// }
//
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT1_Rx( void )	{ (*m_obj_func_i2c_dma_list_Rx[0])( m_obj_i2c_dma_list_Rx[0] );
// }
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT2_Rx( void )	{ (*m_obj_func_i2c_dma_list_Rx[1])( m_obj_i2c_dma_list_Rx[1] );
// }
// void uC_Base_K66F18::uC_BASE_I2C_DMA_INT3_Rx( void )	{ (*m_obj_func_i2c_dma_list_Rx[2])( m_obj_i2c_dma_list_Rx[2] );
// }
//
// void uC_Base_K66F18::uC_BASE_I2C_INT1( void )	{ (*m_obj_func_i2c_list[0])( m_obj_i2c_list[0] ); }
// void uC_Base_K66F18::uC_BASE_I2C_INT2( void )	{ (*m_obj_func_i2c_list[1])( m_obj_i2c_list[1] ); }
// void uC_Base_K66F18::uC_BASE_I2C_INT3( void )	{ (*m_obj_func_i2c_list[2])( m_obj_i2c_list[2] ); }
//
//
// const uC_BASE_I2C_IO_STRUCT         uC_Base_K66F18::m_i2c_io[uC_BASE_I2C_MAX_CTRLS] =
// {
// {
// I2C1,
// {
// RCC_PERIPH_CLCK_APB1,
// RCC_APB1ENR_I2C1EN
// },
// I2C1_EV_IRQn,
// DMA_I2C1RX_D1S5C1,
// DMA_I2C1TX_D1S6C1,
//
// &uC_BASE_I2C1_DEFAULT_IO_PORTS,
//
// &m_obj_func_i2c_dma_list_Tx[0],
// &m_obj_i2c_dma_list_Tx[0],
// uC_BASE_I2C_DMA_INT1_Tx,
// &m_obj_func_i2c_dma_list_Rx[0],
// &m_obj_i2c_dma_list_Rx[0],
// uC_BASE_I2C_DMA_INT1_Rx,
//
// &m_obj_func_i2c_list[0],
// &m_obj_i2c_list[0],
// uC_BASE_I2C_INT1
// },
// {
// I2C2,
// {
// RCC_PERIPH_CLCK_APB1,
// RCC_APB1ENR_I2C2EN
// },
// I2C2_EV_IRQn,
// DMA_I2C2RX_D1S3C7,
// DMA_I2C2TX_D1S7C7,
//
// &uC_BASE_I2C2_DEFAULT_IO_PORTS,
//
// &m_obj_func_i2c_dma_list_Tx[1],
// &m_obj_i2c_dma_list_Tx[1],
// uC_BASE_I2C_DMA_INT1_Tx,
// &m_obj_func_i2c_dma_list_Rx[1],
// &m_obj_i2c_dma_list_Rx[1],
// uC_BASE_I2C_DMA_INT1_Rx,
// &m_obj_func_i2c_list[1],
// &m_obj_i2c_list[1],
// uC_BASE_I2C_INT2
// },
// {
// I2C3,
// {
// RCC_PERIPH_CLCK_APB1,
// RCC_APB1ENR_I2C3EN
// },
// I2C3_EV_IRQn,
// DMA_I2C3RX_D1S2C3,
// DMA_I2C3TX_D1S4C3,
//
// &uC_BASE_I2C3_DEFAULT_IO_PORTS,
//
// &m_obj_func_i2c_dma_list_Tx[2],
// &m_obj_i2c_dma_list_Tx[2],
// uC_BASE_I2C_DMA_INT1_Tx,
// &m_obj_func_i2c_dma_list_Rx[2],
// &m_obj_i2c_dma_list_Rx[2],
// uC_BASE_I2C_DMA_INT1_Rx,
// &m_obj_func_i2c_list[2],
// &m_obj_i2c_list[2],
// uC_BASE_I2C_INT3
// }
// };


/*
 ********************************
 *  Timer Definition Structure
 */
const uint8_t TMR_CNT_SIZE_16BITS = 2;
// const uint8_t TMR_CNT_SIZE_32BITS = 4;
// const uint32_t TMR_FREQ_120MHZ = 120000000;
// const uint32_t TMR_FREQ_60MHZ = 60000000;
// current timer driver does not support pin output (hardware does but we are not implementing it yet)
// uC_PERIPH_IO_STRUCT const* const m_timer1_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER1_PIO1_PA8, &TIMER1_PIO2_PA9, &TIMER1_PIO3_PA10, &TIMER1_PIO4_PA11 };
// uC_PERIPH_IO_STRUCT const* const m_timer2_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER2_PIO1_PA0, &TIMER2_PIO2_PA1, &TIMER2_PIO3_PA2, &TIMER2_PIO4_PA3 };
// uC_PERIPH_IO_STRUCT const* const m_timer3_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER3_PIO1_PC6, &TIMER3_PIO2_PC7, &TIMER3_PIO3_PC8, &TIMER3_PIO4_PC9 };
// uC_PERIPH_IO_STRUCT const* const m_timer4_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER4_PIO1_PB6, &TIMER4_PIO2_PB7, &TIMER4_PIO3_PB8, &TIMER4_PIO4_PB9 };
// uC_PERIPH_IO_STRUCT const* const m_timer5_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER5_PIO1_PA0, &TIMER5_PIO2_PA1, &TIMER5_PIO3_PA2, &TIMER5_PIO4_PA3 };
// uC_PERIPH_IO_STRUCT const* const m_timer8_pio[uC_BASE_4_MAX_TIMER_CAP_COMP_CH] =
// { &TIMER8_PIO1_PC6, &TIMER8_PIO2_PC7, &TIMER8_PIO3_PC8, &TIMER8_PIO4_PC9 };
const uC_BASE_TIMER_IO_STRUCT uC_Base_K66F18::m_timer_io[uC_BASE_MAX_TIMER_CTRLS] =
{
	{	// Timer Control Block 0
		FTM0,
		{
			SIM_SCGC6,
			SIM_SCGC6_FTM0_MASK
		},
		0U,
		FTM0_IRQn,
		TMR_CNT_SIZE_16BITS,
		8U	// num compares
	},
	{	// Timer Control Block 1
		FTM1,
		{
			SIM_SCGC6,
			SIM_SCGC6_FTM1_MASK
		},
		0U,
		FTM1_IRQn,
		TMR_CNT_SIZE_16BITS,
		2U	// num compares
	},
	{	// Timer Control Block 2
		FTM2,
		{
			SIM_SCGC6,
			SIM_SCGC6_FTM2_MASK
		},
		0U,
		FTM2_IRQn,
		TMR_CNT_SIZE_16BITS,
		2U	// num compares
	},
	{	// Timer Control Block 3
		FTM3,
		{
			SIM_SCGC3,
			SIM_SCGC3_FTM3_MASK
		},
		0U,
		FTM3_IRQn,
		TMR_CNT_SIZE_16BITS,
		8U	// num compares
	}
};

/*
 ********************************
 *  Low Power Timer Definition Structure
 */
const uint32_t TMR_FREQ_1KHZ = 1000;
// const uint32_t TMR_FREQ_32768HZ  = 32768;
// const uint32_t TMR_FREQ_50MHZ    = 50000000;
enum
{
	LPTMR_CLOCK_SEL_MCGIRCLK = 0,
	LPTMR_CLOCK_SEL_LPO = 1,
	LPTMR_CLOCK_SEL_ERCLK32K = 2,
	LPTMR_CLOCK_SEL_OSC0ERCLK = 3
};

const uC_BASE_LP_TIMER_CTRL_STRUCT uC_Base_K66F18::m_lp_timer_ctrls[uC_BASE_MAX_LP_TIMER_CTRLS] =
{
	{	// Timer Control Block 0
		LPTMR0,
		{
			SIM_SCGC5,
			SIM_SCGC5_LPTMR_MASK
		},
		LPTMR_CLOCK_SEL_LPO,
		TMR_FREQ_1KHZ,
		LPTimer_IRQn,
		TMR_CNT_SIZE_16BITS
	}
};

/*
 ********************************
 *  External Interrupt Definition Structure
 */
const uC_BASE_EXT_INT_IO_STRUCT uC_Base_K66F18::m_ext_ints[uC_BASE_EXT_INT_MAX_PORTS] =
{
	{
		PORTA_IRQn
	},
	{
		PORTB_IRQn
	},
	{
		PORTC_IRQn
	},
	{
		PORTD_IRQn
	},
	{
		PORTE_IRQn
	}
};

/*
 ********************************
 *  AtoD Definition Structure
 */
// uC_PERIPH_IO_STRUCT const* const m_adc12_pio[uC_ATOD_CHANNEL_MAX] =
// {
// &ADC12_IN0_PIO, &ADC12_IN1_PIO, &ADC12_IN2_PIO, &ADC12_IN3_PIO,
// &ADC12_IN4_PIO, &ADC12_IN5_PIO, &ADC12_IN6_PIO, &ADC12_IN7_PIO,
// &ADC12_IN8_PIO, &ADC12_IN9_PIO, &ADC12_IN10_PIO, &ADC12_IN11_PIO,
// &ADC12_IN12_PIO, &ADC12_IN13_PIO, &ADC12_IN14_PIO, &ADC12_IN15_PIO,
// &ADC12_IN16_PIO, &ADC12_IN17_PIO, &ADC12_IN18_PIO
// };
// uC_PERIPH_IO_STRUCT const* const m_adc3_pio[uC_ATOD_CHANNEL_MAX] =
// {
// &ADC3_IN0_PIO, &ADC3_IN1_PIO, &ADC3_IN2_PIO, &ADC3_IN3_PIO,
// &ADC3_IN4_PIO, &ADC3_IN5_PIO, &ADC3_IN6_PIO, &ADC3_IN7_PIO,
// &ADC3_IN8_PIO, &ADC3_IN9_PIO, &ADC3_IN10_PIO, &ADC3_IN11_PIO,
// &ADC3_IN12_PIO, &ADC3_IN13_PIO, &ADC3_IN14_PIO, &ADC3_IN15_PIO,
// &ADC3_IN16_PIO, &ADC3_IN17_PIO, &ADC3_IN18_PIO
// };
//
// const uC_BASE_ATOD_IO_STRUCT	uC_Base_K66F18::m_atod_io[uC_BASE_ATOD_MAX_CTRLS] =
// {
// {
// ADC1,
// {
// RCC_PERIPH_CLCK_APB2,
// RCC_APB2ENR_ADC1EN
// },
// uC_BASE_K66F18_PCLK_INDEX_TIMER_ADC,
// ADC_IRQn,
// uC_ATOD_CHANNEL_MAX,
// m_adc12_pio,
// #ifdef ADC1_DMA_SELECT
// ADC1_DMA_SELECT,
// #else
// DMA_ADC1_D2S4C0,
// #endif
// },
////		{
////			ADC2,
////			{
////				RCC_PERIPH_CLCK_APB2,
////				RCC_APB2ENR_ADC2EN
////			},
////			ADC1_2_IRQn,
////			uC_ATOD_CHANNEL_MAX,
////			m_adc12_pio,
////			uC_BASE_ATOD_MAX_CHANNELS,
////			uC_DMA_CHANNEL_MAX		//This channel does not have a DMA.
////		},
// {
// ADC3,
// {
// RCC_PERIPH_CLCK_APB2,
// RCC_APB2ENR_ADC3EN
// },
// uC_BASE_K66F18_PCLK_INDEX_TIMER_ADC,
// ADC_IRQn,
// uC_ATOD_CHANNEL_MAX,
// m_adc3_pio,
// DMA_ADC3_D2S1C2
// }
// };

/*
 ********************************
 *  DMA Definition Structure
 */
const uC_BASE_DMA_IO_STRUCT uC_Base_K66F18::m_dma_io[uC_BASE_DMA_CHANNEL_MAX] =
{
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_0,
		uC_DMAMUX_STREAM_0,
		uC_DMAMUX0_SRC_UART1_RX,
		DMA0_DMA16_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_1,
		uC_DMAMUX_STREAM_1,
		uC_DMAMUX0_SRC_UART1_TX,
		DMA1_DMA17_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_2,
		uC_DMAMUX_STREAM_2,
		uC_DMAMUX0_SRC_UART0_RX,
		DMA2_DMA18_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_3,
		uC_DMAMUX_STREAM_3,
		uC_DMAMUX0_SRC_UART0_TX,
		DMA3_DMA19_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_4,
		uC_DMAMUX_STREAM_4,
		uC_DMAMUX0_SRC_UART2_RX,
		DMA4_DMA20_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_5,
		uC_DMAMUX_STREAM_5,
		uC_DMAMUX0_SRC_UART2_TX,
		DMA5_DMA21_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_6,
		uC_DMAMUX_STREAM_6,
		uC_DMAMUX0_SRC_UART4_RX,
		DMA6_DMA22_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_7,
		uC_DMAMUX_STREAM_7,
		uC_DMAMUX0_SRC_UART4_TX,
		DMA7_DMA23_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_8,
		uC_DMAMUX_STREAM_8,
		uC_DMAMUX0_SRC_UART3_RX,
		DMA8_DMA24_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_9,
		uC_DMAMUX_STREAM_9,
		uC_DMAMUX0_SRC_UART3_TX,
		DMA9_DMA25_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_10,
		uC_DMAMUX_STREAM_10,
		uC_DMAMUX0_SRC_SPI2_RX,
		DMA10_DMA26_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_11,
		uC_DMAMUX_STREAM_11,
		uC_DMAMUX0_SRC_SPI2_TX,
		DMA11_DMA27_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_12,
		uC_DMAMUX_STREAM_12,
		uC_DMAMUX0_SRC_DISABLED,
		DMA12_DMA28_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_13,
		uC_DMAMUX_STREAM_13,
		uC_DMAMUX0_SRC_DISABLED,
		DMA13_DMA29_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_14,
		uC_DMAMUX_STREAM_14,
		uC_DMAMUX0_SRC_DISABLED,
		DMA14_DMA30_IRQn
	},
	{
		DMA0,
		DMAMUX0,
		{
			{
				SIM_SCGC6,
				SIM_SCGC6_DMAMUX_MASK
			},
			{
				SIM_SCGC7,
				SIM_SCGC7_DMA_MASK
			},
		},
		uC_DMA_CHANNEL_15,
		uC_DMAMUX_STREAM_15,
		uC_DMAMUX0_SRC_DISABLED,
		DMA15_DMA31_IRQn
	},
	/* {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_16,
	     uC_DMAMUX_STREAM_0,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA0_DMA16_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_17,
	     uC_DMAMUX_STREAM_1,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA1_DMA17_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_18,
	     uC_DMAMUX_STREAM_2,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA2_DMA18_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_19,
	     uC_DMAMUX_STREAM_3,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA3_DMA19_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_20,
	     uC_DMAMUX_STREAM_4,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA4_DMA20_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_21,
	     uC_DMAMUX_STREAM_5,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA5_DMA21_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_22,
	     uC_DMAMUX_STREAM_6,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA6_DMA22_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_23,
	     uC_DMAMUX_STREAM_7,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA7_DMA23_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_24,
	     uC_DMAMUX_STREAM_8,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA8_DMA24_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_25,
	     uC_DMAMUX_STREAM_9,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA9_DMA25_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_26,
	     uC_DMAMUX_STREAM_10,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA10_DMA26_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_27,
	     uC_DMAMUX_STREAM_11,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA11_DMA27_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_28,
	     uC_DMAMUX_STREAM_12,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA12_DMA28_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_29,
	     uC_DMAMUX_STREAM_13,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA13_DMA29_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_30,
	     uC_DMAMUX_STREAM_14,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA14_DMA30_IRQn
	   },
	   {
	     DMA0,
	     DMAMUX1,
	     {
	         {
	            SIM_SCGC6,
	            SIM_SCGC6_DMAMUX1_MASK
	         },
	         {
	            SIM_SCGC7,
	            SIM_SCGC7_DMA_MASK
	         },
	     },
	     uC_DMA_CHANNEL_31,
	     uC_DMAMUX_STREAM_15,
	     uC_DMAMUX1_SRC_DISABLED,
	     DMA15_DMA31_IRQn
	   }*/
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

uC_PERIPH_IO_STRUCT const* const m_mii_pio_in[uC_BASE_MII_PIO_IN_COUNT] =
{
	&ETH_MII_COL_PIO, &ETH_MII_CRS_WKUP_PIO, &ETH_MII_TX_CLK_PIO,
	&ETH_MII_RX_ER_PIO, &ETH_MII_RX_CLK_PIO, &ETH_MII_RX_DV_PIO,
	&ETH_MII_RXD0_PIO, &ETH_MII_RXD1_PIO, &ETH_MII_RXD2_PIO,
	&ETH_MII_RXD3_PIO
};


uC_PERIPH_IO_STRUCT const* const m_rmii_pio_out[uC_BASE_RMII_PIO_OUT_COUNT] =
{
	&ETH_RMII_TX_EN_PIO, &ETH_RMII_TXD0_PIO, &ETH_RMII_TXD1_PIO,
	&ETH_RMII_MDIO_PIO, &ETH_RMII_MDC_PIO
};

uC_PERIPH_IO_STRUCT const* const m_rmii_pio_in[uC_BASE_RMII_PIO_IN_COUNT] =
{
	&ETH_RMII_CRS_DV_PIO, &ETH_RMII_RXD0_PIO, &ETH_RMII_RXD1_PIO,
	&ETH_RMII_RXER_PIO
};

const uC_BASE_EMAC_IO_PORT_STRUCT uC_BASE_EMAC_DEFAULT_PORT =
{
	m_mii_pio_out,
	m_mii_pio_in,
	m_rmii_pio_out,
	m_rmii_pio_in
};

const uC_BASE_EMAC_IO_STRUCT uC_Base_K66F18::m_emac_io[uC_BASE_EMAC_MAX_CTRLS] =
{
	{
		ENET,
		ENET_Transmit_IRQn,
		ENET_Receive_IRQn,
		ENET_Error_IRQn,
		ENET_1588_Timer_IRQn,
		ENET_RCR_RMII_MODE_MASK,
		// {
		// SIM_SCGC2,
		// SIM_SCGC2_ENET_MASK
		// },
		uC_BASE_MII_PIO_OUT_COUNT,
		uC_BASE_MII_PIO_IN_COUNT,
		uC_BASE_RMII_PIO_OUT_COUNT,
		uC_BASE_RMII_PIO_IN_COUNT,

		&uC_BASE_EMAC_DEFAULT_PORT,
	}
};