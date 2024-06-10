/**
 *****************************************************************************************
 *	@file		uC_UART_HW.cpp
 *	@details	See header file for module overview.
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_USART_HW.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint8_t UART_TX_IDLE_NUM_DEFAULT = 0U;
static const int32_t MHZ = 1000000U;
static const uint8_t UART_FLOW_DISABLE = 0U;
static const uint8_t UART_CLK_REF_OFF = 0U;
static const uint8_t UART_CLK_DEF_FREQ = 80U;
static const uint8_t UART_CLK_DIV_FOUR = 4U;
static const uint8_t UART_CLK_DIV_SIXTEEN = 16U;
static const uint8_t UART_STOP_BIT_ENABLE = 1U;
static const uint8_t UART_STOP_BIT_DISABLE = 0U;
static const uint8_t UART_FIFO_COUNT_ZERO = 0U;
static const uint8_t UART_CLK_MASK = 0xf;

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */

/**
 * @brief uart_dev_t UART used for configuring UART interface
 * @n DRAM_ATTR is required to avoid UART array placed in flash, due to accessed from ISR
 */
static DRAM_ATTR uart_dev_t* const UART[UART_NUM_MAX] = { &UART0, &UART1, &UART2 };

/**
 * @brief uart_spinlock  used for UART configure operation
 * */
static portMUX_TYPE uart_spinlock[UART_NUM_MAX] =
{ portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED };

/**
 * @brief uC_BASE_USART_IO_STRUCT contain uart port informations
 */
static uC_BASE_USART_IO_STRUCT m_io_ctrls[UART_NUM_MAX];

/**
 *  @brief defines for critical session management
 */
#define UART_ENTER_CRITICAL( mux )      portENTER_CRITICAL( mux )
#define UART_EXIT_CRITICAL( mux )       portEXIT_CRITICAL( mux )
#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )

/**
 * @brief g_ticks defined in ROMs for PRO CPU.
 */
extern uint32_t g_ticks_per_us_pro;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_HW::uC_USART_HW( uint8_t port_requested,
						  uC_BASE_USART_IO_PORT_STRUCT const* io_port, bool enable_dma )
{
	m_io_ctrl = &( m_io_ctrls[port_requested] );

	m_io_ctrl->usart_num = port_requested;


	if ( m_io_ctrl->usart_num == UART_NUM_0 )
	{
		uC_Base::Periph_Module_Enable( PERIPH_UART0_MODULE );
	}
	else if ( m_io_ctrl->usart_num == UART_NUM_1 )
	{
		uC_Base::Periph_Module_Enable( PERIPH_UART1_MODULE );
	}
	else if ( m_io_ctrl->usart_num == UART_NUM_2 )
	{
		uC_Base::Periph_Module_Enable( PERIPH_UART2_MODULE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Configure( uint32_t baud_rate, char_size_t char_size,
							 parity_t parity, stop_bit_t stop_bit_count )
{

	UART[m_io_ctrl->usart_num]->conf1.rx_flow_en = UART_FLOW_DISABLE;	///< Disabling RX  flow control
	UART[m_io_ctrl->usart_num]->conf0.tx_flow_en = UART_FLOW_DISABLE;	///< Disabling TX  flow control

	UART[m_io_ctrl->usart_num]->conf0.val =
		( parity << UART_PARITY_S )
		| ( char_size << UART_BIT_NUM_S )
		| ( UART_FLOW_DISABLE )
		| ( UART_TICK_REF_ALWAYS_ON_M );

	Set_Baud_Rate( baud_rate );

	UART_ENTER_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );
	UART[m_io_ctrl->usart_num]->idle_conf.tx_idle_num = UART_TX_IDLE_NUM_DEFAULT;
	UART_EXIT_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );

	Set_Stop_Bit( stop_bit_count );
	Reset_Rx_Fifo();	///< A hardware reset does not reset the fifo,so we need to reset the fifo manually.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Set_Baud_Rate( uint32_t baud_rate )
{

	int32_t uart_clk_freq;
	uint32_t clk_div;

	UART_ENTER_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );

	/* Setting the clock frequency by checking tick_ref_always_on flag */
	if ( UART[m_io_ctrl->usart_num]->conf0.tick_ref_always_on == UART_CLK_REF_OFF )
	{
		uart_clk_freq = REF_CLK_FREQ;
	}
	else
	{
		uart_clk_freq = MIN( g_ticks_per_us_pro, UART_CLK_DEF_FREQ ) * MHZ;
	}

	clk_div = ( ( ( uart_clk_freq ) << UART_CLK_DIV_FOUR ) / baud_rate );

	if ( clk_div >= UART_CLK_DIV_SIXTEEN )
	{
		UART[m_io_ctrl->usart_num]->clk_div.div_int = ( clk_div >> UART_CLK_DIV_FOUR );
		UART[m_io_ctrl->usart_num]->clk_div.div_frag = ( clk_div & UART_CLK_MASK );
	}
	UART_EXIT_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Set_Stop_Bit( stop_bit_t stop_bit )
{

	UART_ENTER_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );
	/* workaround for hardware bug, when uart stop bit set as 2-bit mode.*/
	if ( stop_bit == STOP_BIT_2 )
	{
		stop_bit = STOP_BIT_1;
		UART[m_io_ctrl->usart_num]->rs485_conf.dl1_en = UART_STOP_BIT_ENABLE;
	}
	else
	{
		UART[m_io_ctrl->usart_num]->rs485_conf.dl1_en = UART_STOP_BIT_DISABLE;
	}
	/* Setting stop bit */
	UART[m_io_ctrl->usart_num]->conf0.stop_bit_num = stop_bit;
	UART_EXIT_CRITICAL( &uart_spinlock[m_io_ctrl->usart_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_HW::Reset_Rx_Fifo( void )
{
	/* Reset RX fifo  */
	while ( UART[m_io_ctrl->usart_num]->status.rxfifo_cnt != UART_FIFO_COUNT_ZERO ||
			( UART[m_io_ctrl->usart_num]->mem_rx_status.wr_addr != UART[m_io_ctrl->usart_num]->mem_rx_status.rd_addr ) )
	{
		READ_PERI_REG( UART_FIFO_REG( m_io_ctrl->usart_num ) );
	}
}
