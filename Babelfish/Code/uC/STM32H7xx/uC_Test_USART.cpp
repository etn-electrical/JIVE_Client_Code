/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Micro_Task.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
#include "uC_USART.h"
bool uC_TEST_USART_Test_Init( void );

void uC_TEST_USART_Int_Handler( void* param );

#include "uC_USART_DMA.h"
void uC_USART_DMA_Test( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_USART_Init( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#include "uC_USART_DMA.h"
uC_USART_DMA* dma_usart;
volatile uC_USART_DMA::cback_status_t past_status = ~0;
const uint16_t USART_DMA_BUFF_LEN = 4;
uint8_t usart_dma_tx_buff[USART_DMA_BUFF_LEN];
uint8_t usart_dma_rx_buff[USART_DMA_BUFF_LEN];
Output* usart_dma_txen;
Timers* usart_dma_timer;
uC_USART_DMA::cback_status_t usart_dma_req_bits;
uint32_t usart_rx_dma_timeout = 5000;	// This is in microseconds
uint32_t usart_tx_dma_timeout = 7500;	// This is in microseconds

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Flicker_Debug( uint8_t flickers )
{
	for ( uint_fast8_t i = 0; i < flickers; i++ )
	{
		uC_Delay( 1 );
		Debug_3_Toggle();
		uC_Delay( 1 );
		Debug_3_Toggle();
	}
}

void uC_USART_DMA_TX_Complete_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_TXRX_Buff_Full_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 7 );

	past_status |= status;
	if ( Test_Bit( status, uC_USART_DMA::TX_COMPLETE ) )
	{
		usart_dma_txen->Off();
	}

	if ( Test_Bit( past_status, uC_USART_DMA::TX_COMPLETE ) &&
		 Test_Bit( past_status, uC_USART_DMA::TX_HOLDOFF_COMPLETE ) &&
		 Test_Bit( past_status, uC_USART_DMA::RX_BYTE_TIMEOUT ) )
	{
		usart_dma_timer->Start( 1000, false );
	}

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_The_Works_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 6 );

	past_status |= status;

	if ( Test_Bit( past_status, uC_USART_DMA::RX_BYTE_TIMEOUT ) &&
		 Test_Bit( past_status, uC_USART_DMA::RX_BUFF_FULL ) &&
		 Test_Bit( past_status, uC_USART_DMA::TX_BUFF_EMPTY ) &&
		 Test_Bit( past_status, uC_USART_DMA::TX_COMPLETE ) &&
		 Test_Bit( past_status, uC_USART_DMA::TXRX_HOLDOFF_COMPLETE ) )
	{
		usart_dma_req_bits = 0;
		Set_Bit( usart_dma_req_bits, uC_USART_DMA::RX_BYTE_TIMEOUT );
		Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_HOLDOFF_COMPLETE );
		Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_COMPLETE );
		dma_usart->Config_Callback( uC_USART_DMA_TXRX_Buff_Full_Test_Cback, NULL, usart_dma_req_bits,
									usart_rx_dma_timeout, usart_tx_dma_timeout );
		dma_usart->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
		dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );
		past_status = 0;
	}

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_RX_Buff_Full_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 5 );

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::RX_BYTE_TIMEOUT );
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::RX_BUFF_FULL );
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_BUFF_EMPTY );
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_COMPLETE );
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TXRX_HOLDOFF_COMPLETE );
	dma_usart->Config_Callback( uC_USART_DMA_The_Works_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	dma_usart->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );
	past_status = 0;

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_RX_Timeout_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 4 );

	past_status = status;

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::RX_BUFF_FULL );
	dma_usart->Config_Callback( uC_USART_DMA_RX_Buff_Full_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	dma_usart->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_TX_Timeout_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 3 );

	past_status = status;

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::RX_BYTE_TIMEOUT );
	dma_usart->Config_Callback( uC_USART_DMA_RX_Timeout_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	dma_usart->Start_RX( usart_dma_rx_buff, USART_DMA_BUFF_LEN );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_TXRX_Timeout_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 2 );

	past_status = status;

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TXRX_HOLDOFF_COMPLETE );
	dma_usart->Config_Callback( uC_USART_DMA_TX_Timeout_Test_Cback, NULL, usart_dma_req_bits,
								0, 0 );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_TX_Buff_Empty_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();
	Flicker_Debug( 1 );

	past_status = status;

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_HOLDOFF_COMPLETE );
	dma_usart->Config_Callback( uC_USART_DMA_TXRX_Timeout_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_TX_Complete_Test_Cback( uC_USART_DMA::cback_param_t param, uC_USART_DMA::cback_status_t status )
{
	Debug_3_On();

	past_status = status;

	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_BUFF_EMPTY );
	dma_usart->Config_Callback( uC_USART_DMA_TX_Buff_Empty_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );

	Debug_3_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_Test_Timeout( void* param )
{
	usart_dma_req_bits = 0;
	Set_Bit( usart_dma_req_bits, uC_USART_DMA::TX_COMPLETE );

	dma_usart->Config_Callback( uC_USART_DMA_TX_Complete_Test_Cback, NULL, usart_dma_req_bits,
								usart_rx_dma_timeout, usart_tx_dma_timeout );
	usart_dma_txen->On();
	dma_usart->Start_TX( usart_dma_tx_buff, USART_DMA_BUFF_LEN );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA_Test( void )
{
	// uC_USART* usart;

	usart_dma_txen = new Output_Pos( &MODBUS_USART_TXEN_IO_CTRL );

	// usart = new uC_USART( MODBUS_USART_PORT, &MODBUS_USART_PIO, true );
	// usart->Configure( uC_USART_CHAR_SIZE_8BIT, uC_USART_PARITY_EVEN, uC_USART_1_STOP_BIT );
	// usart->Set_Baud_Rate( 19200 );

	dma_usart = new uC_USART_DMA( MODBUS_USART_PORT, uC_BASE_TIMER_CTRL_2,
								  &MODBUS_USART_PIO );
	dma_usart->Configure( 19200, uC_USART_DMA::CHAR_SIZE_8BIT,
						  uC_USART_DMA::PARITY_EVEN, uC_USART_DMA::STOP_BIT_1 );

	usart_dma_timer = new Timers( uC_USART_DMA_Test_Timeout, NULL );
	usart_dma_timer->Start( 1000, false );

	for ( uint_fast16_t i = 0; i < USART_DMA_BUFF_LEN; i++ )
	{
		usart_dma_tx_buff[i] = 0;
		usart_dma_rx_buff[i] = ~( i + 1 );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uC_BASE_USART_IO_PORT_STRUCT uC_BASE_USART_PORT_3_PIO = { &USART3_TX_PIO_PC10, &USART3_RX_PIO_PC11 };
uC_USART* usart_ctrl;
#define USART_TEST_STRING_LEN   8
uint8_t usart_test_string[USART_TEST_STRING_LEN];
uint8_t usart_test_index = 0;
void uC_TEST_USART_Int_Handler( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_USART_Test_Init( void )
{
	uC_TEST_Init_String( usart_test_string, USART_TEST_STRING_LEN );

#ifndef TEST_USART_DMA
	usart_ctrl = new uC_USART( uC_BASE_USART_PORT_3, &uC_BASE_USART_PORT_3_PIO, true );
	usart_ctrl->Configure( uC_USART_CHAR_SIZE_8BIT, uC_USART_PARITY_EVEN, uC_USART_1_STOP_BIT );
	usart_ctrl->Set_Baud_Rate( 19200, uC_USART_SAMPLE_COUNT_16 );

	usart_ctrl->Set_TX_DMA( usart_test_string, USART_TEST_STRING_LEN );
	usart_ctrl->Enable_TX();
	usart_ctrl->Enable_TX_DMA();
	while ( !usart_ctrl->TX_Data_Empty_Status() )
	{}

	usart_ctrl->Set_RX_DMA( usart_test_string, USART_TEST_STRING_LEN );
	usart_ctrl->Enable_RX_DMA();
	usart_ctrl->Enable_RX();

	while ( !usart_ctrl->RX_Ready_Status() )
	{}

#else
	usart_ctrl = new uC_USART( uC_BASE_USART_PORT_3, uC_BASE_USART_PORT_3_PIO, false );
	usart_ctrl->Configure( uC_USART_CHAR_SIZE_8BIT, uC_USART_PARITY_EVEN, uC_USART_1_STOP_BIT );

	usart_ctrl->Enable_TX();
	usart_ctrl->Set_Baud_Rate( 19200, uC_USART_SAMPLE_COUNT_16 );

	for ( uint_fast8_t i = 0; i < 100; i++ )
	{
		usart_ctrl->Set_TX_Data( i );
		while ( !usart_ctrl->TX_Data_Empty_Status() )
		{}
		usart_ctrl->Set_TX_Data( i );
		while ( !usart_ctrl->TX_Empty_Status() )
		{}
	}

	usart_test_index = 0;
	usart_ctrl->Set_Int_Vect( uC_TEST_USART_Int_Handler, NULL, uC_INT_HW_PRIORITY_4 );
	usart_ctrl->Clear_Int_Status();
	usart_ctrl->Enable_TX_Empty_Int();
	usart_ctrl->Enable_Int();

	usart_ctrl->Set_TX_Data( 0x55 );
	while ( usart_test_index == 0 )
	{}

	usart_ctrl->Set_TX_Data( 0xAA );
	while ( usart_test_index == 1 )
	{}

#endif

	uC_TEST_Init_String( usart_test_string, USART_TEST_STRING_LEN );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_USART_Int_Handler( void* param )
{
	uint32_t int_status;

	int_status = usart_ctrl->Get_Int_Status_Bits();
	usart_ctrl->Clear_Int_Status();

	while ( int_status )
	{
		if ( usart_ctrl->RX_Ready_Int_Status( int_status ) )
		{
			usart_test_string[usart_test_index] = usart_ctrl->Get_RX_Data();
			if ( usart_test_index >= USART_TEST_STRING_LEN )
			{
				usart_test_index = 0;
			}
			else
			{
				usart_test_index++;
			}
		}
		if ( usart_ctrl->TX_Empty_Int_Status( int_status ) )
		{
			if ( usart_test_index >= USART_TEST_STRING_LEN )
			{
				usart_test_index = 0;
			}
			else
			{
				usart_test_index++;
			}
		}
		int_status = usart_ctrl->Get_Int_Status_Bits();
		usart_ctrl->Clear_Int_Status();
	}
}
