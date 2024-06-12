/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "openamp_interface.h"
#include "openamp.h"
#include "stm32mp1xx_hal.h"
#include "openamp_log.h"

VIRT_UART_HandleTypeDef huart0;
VIRT_UART_HandleTypeDef huart1;
IPCC_HandleTypeDef hipcc;
FlagStatus VirtUart1RxMsg = RESET;
FlagStatus VirtUart0RxMsg = RESET;

uint8_t VirtUart0ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart0ChannelRxSize = 0;

uint8_t VirtUart1ChannelBuffRx[MAX_BUFFER_SIZE];
uint16_t VirtUart1ChannelRxSize = 0;

static void MX_IPCC_Init( void );

void VIRT_UART0_RxCpltCallback( VIRT_UART_HandleTypeDef* huart );

void VIRT_UART1_RxCpltCallback( VIRT_UART_HandleTypeDef* huart );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OpenAmp_Init( void )
{
	/* IPCC initialisation */
	MX_IPCC_Init();

	/* OpenAmp initialisation ---------------------------------*/
	MX_OPENAMP_Init( RPMSG_REMOTE, NULL );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Virtual_UART_Init0( void )
{

	if ( VIRT_UART_Init( &huart0 ) != VIRT_UART_OK )
	{
		log_err( "VIRT_UART_Init UART0 failed.\r\n" );
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Virtual_UART_Init1( void )
{
	if ( VIRT_UART_Init( &huart1 ) != VIRT_UART_OK )
	{
		log_err( "VIRT_UART_Init UART1 failed.\r\n" );
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Virtual_UART_RegisterCallback0( void )
{
	if ( VIRT_UART_RegisterCallback( &huart0, VIRT_UART_RXCPLT_CB_ID, VIRT_UART0_RxCpltCallback ) != VIRT_UART_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Virtual_UART_RegisterCallback1( void )
{
	if ( VIRT_UART_RegisterCallback( &huart1, VIRT_UART_RXCPLT_CB_ID, VIRT_UART1_RxCpltCallback ) != VIRT_UART_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void VIRT_UART0_RxCpltCallback( VIRT_UART_HandleTypeDef* huart )
{

	log_info( "Msg received on VIRTUAL UART0 channel:  %s \n\r", ( char* ) huart->pRxBuffPtr );

	/* copy received msg in a variable to sent it back to master processor in main infinite loop*/
	VirtUart0ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE - 1;
	memcpy( VirtUart0ChannelBuffRx, huart->pRxBuffPtr, VirtUart0ChannelRxSize );
	VirtUart0RxMsg = SET;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void VIRT_UART1_RxCpltCallback( VIRT_UART_HandleTypeDef* huart )
{

	log_info( "Msg received on VIRTUAL UART1 channel:  %s \n\r", ( char* ) huart->pRxBuffPtr );

	/* copy received msg in a variable to sent it back to master processor in main infinite loop*/
	VirtUart1ChannelRxSize = huart->RxXferSize < MAX_BUFFER_SIZE? huart->RxXferSize : MAX_BUFFER_SIZE - 1;
	memcpy( VirtUart1ChannelBuffRx, huart->pRxBuffPtr, VirtUart1ChannelRxSize );
	VirtUart1RxMsg = SET;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Check_Message( void )
{
	while ( 1 )
	{

		OPENAMP_check_for_message();

		if ( VirtUart0RxMsg )
		{
			VirtUart0RxMsg = RESET;
			VIRT_UART_Transmit( &huart0, VirtUart0ChannelBuffRx, VirtUart0ChannelRxSize );
		}

		if ( VirtUart1RxMsg )
		{
			VirtUart1RxMsg = RESET;
			VIRT_UART_Transmit( &huart1, VirtUart1ChannelBuffRx, VirtUart1ChannelRxSize );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void MX_IPCC_Init( void )
{

	hipcc.Instance = IPCC;
	if ( HAL_IPCC_Init( &hipcc ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Error_Handler( void )
{
	log_err( "Error_Handler" );
	while ( 1 )
	{}
}