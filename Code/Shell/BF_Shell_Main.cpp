/**
 *****************************************************************************************
 * @file
 *
 * @brief	Main file for running the Shell.
 *
 * @details
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Shell_Includes.h"

namespace Shell
{


/*
 *****************************************************************************************
 * Instantiate our shell (global?)
 *****************************************************************************************
 */
BF_Shell* g_shell0_ptr;
Shell_Base::shell_config_t g_shell0_config;
uC_USART_Buff* g_shell0_usart_ptr;
Shell::Test_Base* g_shell_cmd_table_instance[1] = {NULL};

#define UART0_RX_PIN     ( 3 )
#define UART0_TX_PIN     ( 1 )
const uC_BASE_USART_IO_PORT_STRUCT USART_DMA_PIO = {NULL, NULL};
#define USART_DMA_TIMER_CTRL    1

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell_Initialize0( void )
{
	// Setting baud rate to 460.8kb
	uint32_t bf_shell0_baud_rate = 115200U;

	// Create the UART and configure baud rate
	g_shell0_usart_ptr = new uC_USART_DMA( UART_NUM_0, USART_DMA_TIMER_CTRL, &USART_DMA_PIO );

	/* Configure RX & Tx pins */
	uart_set_pin( UART_NUM_0, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );

	g_shell0_usart_ptr->Config_Port( bf_shell0_baud_rate, uC_USART_Buff::CHAR_SIZE_8BIT, uC_USART_Buff::PARITY_NONE,
									 uC_USART_Buff::STOP_BIT_1 );

	// build configuration structure
	memcpy( &g_shell0_config, &Shell_Base::DEFAULT_SHELL_CONFIG, sizeof( Shell_Base::shell_config_t ) );
	g_shell0_config.m_command_table = ( Command_Base** )g_shell_cmd_table_instance;
	g_shell0_config.m_command_cnt = G_SHELL_COMMAND_TABLE_COUNT;

	// create the shell
	g_shell0_ptr = new BF_Shell( g_shell0_usart_ptr, &g_shell0_config );
}

/*
 * Test/Debug stubs
 */
bool FOO_123_Test( void )
{
	return ( true );
}

bool BAR_246_Test( void )
{
	return ( true );
}

bool BAZ_789_Test( void )
{
	return ( true );
}

bool SNAFU_1234_Test( void )
{
	return ( true );
}

bool SNAFU_123_Test( void )
{
	return ( true );
}

bool WAFWOT_246_Test( void )
{
	return ( true );
}

bool WAFWOT_246b_Test( void )
{
	return ( true );
}

}	// namespace Shell
