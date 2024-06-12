/**
 *****************************************************************************************
 *	@file		uC_USART_DMA.cpp
 *	@details	See header file for module overview.
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_USART_DMA.h"
#include "uC_Base.h"
#include "uC_UHCI.h"
#include "uC_USART_Buff.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_DMA::uC_USART_DMA( uint8_t hw_usart_num, uint8_t hw_timer_num,
							const uC_BASE_USART_IO_PORT_STRUCT* io_port ) :
	uC_USART_HW( hw_usart_num, io_port, true ),
	uC_USART_Buff()
{
	m_udma_ctrl = new uC_UHCI( UHCI_NUM_0, hw_usart_num );

	m_udma_ctrl->Enable( UHCI_LINK_IN );
	m_udma_ctrl->Enable_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_DMA::~uC_USART_DMA( void )
{
	delete m_udma_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Config_Callback( uC_USART_Buff::cback_func_t cback_func,
									uC_USART_Buff::cback_param_t cback_param,
									uC_USART_Buff::cback_status_t cback_req_mask,
									uint32_t rx_byte_time, uint32_t tx_holdoff_time )
{
	Stop_TX();
	Stop_RX();

	m_cback_func = cback_func;
	m_cback_param = cback_param;
	m_cback_request = cback_req_mask;

	m_udma_ctrl->Config_Callback( m_cback_func, m_cback_param, m_cback_request );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Start_RX( uint8_t* data, uint16_t length )
{
	m_rx_state = RX_RECEIVING;
	m_udma_ctrl->Set_Buff( data, UHCI_LINK_IN, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_Buff::rx_state_t uC_USART_DMA::RX_State( void )
{
	return ( m_rx_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_USART_DMA::Get_RX_Length( void )
{
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Stop_RX( void )
{
	m_rx_state = RX_IDLE;
	m_udma_ctrl->Disable( UHCI_LINK_IN );
	m_udma_ctrl->Disable_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Start_TX( uint8_t* data, uint16_t length )
{
	m_tx_state = TX_RUNNING;
	m_tx_holdoff = true;
	m_udma_ctrl->Set_Buff( data, UHCI_LINK_OUT, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USART_Buff::tx_state_t uC_USART_DMA::TX_State( void )
{
	return ( m_tx_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART_DMA::TX_Holdoff( void )
{
	return ( m_tx_holdoff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_DMA::Stop_TX( void )
{
	m_tx_state = TX_IDLE;
	m_tx_holdoff = false;
	m_udma_ctrl->Disable( UHCI_LINK_OUT );
	m_udma_ctrl->Disable_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART_DMA::Config_Port( uint32_t baud_rate, uC_USART_Buff::char_size_t char_size,
								uC_USART_Buff::parity_t parity,
								uC_USART_Buff::stop_bit_t stop_bit_count )
{
	uC_USART_HW::char_size_t hw_char_size;
	uC_USART_HW::parity_t hw_parity;
	uC_USART_HW::stop_bit_t hw_stop_bit_count;
	bool success = true;

	switch ( char_size )
	{
		case uC_USART_Buff::CHAR_SIZE_5BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_5BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_6BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_6BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_7BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_7BIT;
			break;

		case uC_USART_Buff::CHAR_SIZE_8BIT:
			hw_char_size = uC_USART_HW::CHAR_SIZE_8BIT;
			break;

		default:
			hw_char_size = uC_USART_HW::CHAR_SIZE_8BIT;
			success = false;
			break;
	}

	switch ( parity )
	{
		case uC_USART_Buff::PARITY_EVEN:
			hw_parity = uC_USART_HW::PARITY_EVEN;
			break;

		case uC_USART_Buff::PARITY_ODD:
			hw_parity = uC_USART_HW::PARITY_ODD;
			break;

		case uC_USART_Buff::PARITY_NONE:
			hw_parity = uC_USART_HW::PARITY_NONE;
			break;

		default:
			hw_parity = uC_USART_HW::PARITY_NONE;
			success = false;
			break;
	}

	switch ( stop_bit_count )
	{
		case uC_USART_Buff::STOP_BIT_1:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1;
			break;

		case uC_USART_Buff::STOP_BIT_1p5:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1P5;
			break;

		case uC_USART_Buff::STOP_BIT_2:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_2;
			break;

		default:
			hw_stop_bit_count = uC_USART_HW::STOP_BIT_1;
			success = false;
			break;
	}

	if ( success == true )
	{
		Configure( baud_rate, hw_char_size, hw_parity, hw_stop_bit_count );
	}

	return ( success );
}
