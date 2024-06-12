/**
 *****************************************************************************************
 *	@file		uC_UHCI.cpp
 *	@details	See header file for module overview.
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_UHCI.h"
#include "uC_Base.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "soc/dport_reg.h"
#include "CR_Tasker.h"
#include "uC_USART_DMA.h"
#include "uC_USART_Buff.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint8_t UDMA_QUEUE_SIZE = 10U;
static const uint8_t UDMA_RX_DATA_LEN = 0U;
static const uint16_t UDMA_LINK_SIZE = 256U;
static const uint8_t UDMA_LINK_LEN = 0U;
static const uint8_t UDMA_LINK_EOF = 1U;
static const uint8_t UDMA_LINK_OWNER = 1U;
static const uint8_t UART_NUMBER_1 = 1;
static const uint8_t UART_NUMBER_2 = 2;
static const char* UHCI_TAG = "uhci module";

uC_UHCI::cback_status_t uC_UHCI::m_cback_request = 0;
uC_UHCI::cback_func_t uC_UHCI::m_cback_func = nullptr;
uC_UHCI::cback_param_t uC_UHCI::m_cback_param = nullptr;

static const char* UHCI_NUM_ERR_STR = "uhci_num error!!! valid param: { UHCI_NUM_0, UHCI_NUM_0 }";
static const char* UHCI_LINK_TYPE_ERR_STR = "uhci_link_type error!!! valid param: { UHCI_LINK_IN, UHCI_LINK_OUT }";

/*
 *****************************************************************************************
 *		Static Variables uhci_spinlock and  periph_spinlock used for UART DMA operation
 *****************************************************************************************
 */
static DRAM_ATTR uhci_dev_t* const UHCI[UHCI_NUM_MAX] = { &UHCI0, &UHCI1 };
static portMUX_TYPE uhci_spinlock[UHCI_NUM_MAX] = { portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED };
static uC_BASE_UHCI_STRUCT m_udma_ctrls[UHCI_NUM_MAX];

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_UHCI::uC_UHCI( uhci_num_t uhci_num, uint32_t uart_num )
{
	int32_t intr_alloc_flags = 0U;

	m_udma = &( m_udma_ctrls[uhci_num] );
	m_udma->uhci_num = uhci_num;
	if ( m_udma->uhci_num == UHCI_NUM_0 )
	{
		uC_Base::Periph_Module_Enable( PERIPH_UHCI0_MODULE );
	}
	else if ( m_udma->uhci_num == UHCI_NUM_1 )
	{
		uC_Base::Periph_Module_Enable( PERIPH_UHCI1_MODULE );
	}
	UHCI[m_udma->uhci_num]->conf0.val = UDMA_RX_DATA_LEN;
	UHCI[m_udma->uhci_num]->conf1.val = UDMA_RX_DATA_LEN;
	UHCI[m_udma->uhci_num]->escape_conf.val = UDMA_RX_DATA_LEN;
	UHCI[m_udma->uhci_num]->hung_conf.val = UDMA_RX_DATA_LEN;
	UHCI[m_udma->uhci_num]->conf0.uart_idle_eof_en = UDMA_LINK_EOF;

	Uhci_Attach_Uart( m_udma->uhci_num, uart_num );

	m_udma->queue_size = UDMA_QUEUE_SIZE;
	m_udma->rx_data_len = UDMA_RX_DATA_LEN;
	vSemaphoreCreateBinary( m_udma->tx_sem );

	m_udma->xQueueUhci = xQueueCreate( m_udma->queue_size, sizeof( uhci_event_t ) );
	m_udma->rx_ring_buf = xRingbufferCreate( RINGBUF_SIZE, RINGBUF_TYPE_NOSPLIT );

	m_udma->rx_link.des.size = UDMA_LINK_SIZE;
	m_udma->rx_link.des.length = UDMA_LINK_LEN;
	m_udma->rx_link.des.eof = UDMA_LINK_EOF;
	m_udma->rx_link.des.owner = UDMA_LINK_OWNER;
	m_udma->rx_link.buf = m_udma->rx_buf;
	m_udma->rx_link.pnext = nullptr;
	m_udma->tx_link.des.size = UDMA_LINK_SIZE;
	m_udma->tx_link.des.length = UDMA_LINK_LEN;
	m_udma->tx_link.des.eof = UDMA_LINK_EOF;
	m_udma->tx_link.des.owner = UDMA_LINK_OWNER;
	m_udma->tx_link.buf = nullptr;
	m_udma->tx_link.pnext = nullptr;

	Uhci_Set_Link_Addr( uhci_num, UHCI_LINK_IN, ( uint32_t )&( m_udma->rx_link ) );
	Uhci_Set_Link_Addr( m_udma->uhci_num, UHCI_LINK_OUT, ( uint32_t )&( m_udma->tx_link ) );

	Uhci_Isr_Register( m_udma->uhci_num, Uhci_Intr_Handle, ( void* )m_udma, intr_alloc_flags,
					   &( m_udma->uhci_isr_handle ) );



}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Set_Buff( uint8_t* mem_ptr, uhci_link_type_t link_type, uint16_t item_count )
{
	uint8_t* rec = NULL;
	size_t size = item_count;

	if ( link_type == UHCI_LINK_IN )
	{
		rec = ( uint8_t* )xRingbufferReceive( m_udma->rx_ring_buf, &size, ( TickType_t )0 );
		if ( rec == NULL )
		{
			size = 0;
		}
		else
		{
			memcpy( ( void* )mem_ptr, ( void* )rec, size );
			vRingbufferReturnItem( m_udma->rx_ring_buf, rec );
			m_udma->rx_data_len -= size;
			ESP_LOGE( UHCI_TAG, "Data :%s ", rec );
		}
	}
	else if ( link_type == UHCI_LINK_OUT )
	{
		m_udma->tx_link.des.length = item_count;
		m_udma->tx_link.buf = mem_ptr;
		xSemaphoreTake( m_udma->tx_sem, ( TickType_t ) 0 );
		Enable( UHCI_LINK_OUT );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Enable( uhci_link_type_t link_type )
{
	if ( link_type == UHCI_LINK_OUT )
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_out_link.start = UDMA_LINK_OWNER;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	else
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_in_link.start = UDMA_LINK_OWNER;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Disable( uhci_link_type_t link_type )
{
	if ( link_type == UHCI_LINK_OUT )
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_out_link.stop = UDMA_LINK_OWNER;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	else
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_in_link.stop = UDMA_LINK_OWNER;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Enable_Int( void ) const
{
	uint32_t ena_mask;

	ena_mask = UHCI_OUT_EOF | UHCI_OUT_DONE | UHCI_IN_DONE | UHCI_RX_HUNG | UHCI_TX_HUNG;
	portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	SET_PERI_REG_MASK( UHCI_INT_ENA_REG( m_udma->uhci_num ), ena_mask );
	portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Disable_Int( void ) const
{
	portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	CLEAR_PERI_REG_MASK( UHCI_INT_ENA_REG( m_udma->uhci_num ), UHCI_INTR_DIS );
	portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Clear_Int( uhci_num_t uhci_num )
{
	portENTER_CRITICAL( &uhci_spinlock[uhci_num] );
	uint32_t clr_mask = UHCI[uhci_num]->int_st.val;

	SET_PERI_REG_MASK( UHCI_INT_CLR_REG( uhci_num ), clr_mask );
	portEXIT_CRITICAL( &uhci_spinlock[uhci_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_UHCI::Uhci_Set_Link_Addr( uhci_num_t uhci_num, uhci_link_type_t link_type, uint32_t link_addr )
{
	Check_UHCI( m_udma->uhci_num < UHCI_NUM_MAX, UHCI_NUM_ERR_STR, ESP_ERR_INVALID_ARG );
	Check_UHCI( link_type < UHCI_LINK_MAX, UHCI_LINK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG );

	if ( link_type == UHCI_LINK_OUT )
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_out_link.addr = ( link_addr & UHCI_OUTLINK_ADDR_M );
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	else
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->dma_in_link.addr = ( link_addr & UHCI_OUTLINK_ADDR_M );
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_UHCI::Uhci_Isr_Register( uhci_num_t uhci_num, void ( * fun )(
										  void* ), void* arg, int intr_alloc_flags, uhci_isr_handle_t* handle )
{
	esp_err_t ret;

	Check_UHCI( m_udma->uhci_num < UHCI_NUM_MAX, UHCI_NUM_ERR_STR, ESP_ERR_INVALID_ARG );

	if ( m_udma->uhci_num == UHCI_NUM_1 )
	{
		ret = esp_intr_alloc( ETS_UHCI1_INTR_SOURCE, intr_alloc_flags, fun, arg, handle );
	}
	else
	{
		ret = esp_intr_alloc( ETS_UHCI0_INTR_SOURCE, intr_alloc_flags, fun, arg, handle );
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_UHCI::Uhci_Attach_Uart( uhci_num_t uhci_num, uint32_t uart_num )
{
	Check_UHCI( m_udma->uhci_num < UHCI_NUM_MAX, UHCI_NUM_ERR_STR, ESP_ERR_INVALID_ARG );

	if ( uart_num == UART_NUMBER_1 )
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->conf0.uart1_ce = UDMA_LINK_EOF;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	else if ( uart_num == UART_NUMBER_2 )
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->conf0.uart2_ce = UDMA_LINK_EOF;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	else
	{
		portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
		UHCI[m_udma->uhci_num]->conf0.uart0_ce = UDMA_LINK_EOF;
		portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_UHCI::Uhci_Intr_Handle( void* parm )
{
	uint32_t status;
	uint32_t addr_tmp;
	size_t length;

	uC_BASE_UHCI_STRUCT* m_udma = ( uC_BASE_UHCI_STRUCT* )parm;
	BaseType_t HP_awoken = 0U;
	uhci_event_t uhci_event;

	portENTER_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	status = UHCI[m_udma->uhci_num]->int_st.val;
	portEXIT_CRITICAL( &uhci_spinlock[m_udma->uhci_num] );
	Clear_Int( m_udma->uhci_num );

	if ( ( status & UHCI_RX_START ) != false )
	{
		uhci_event.event_type = RX_START;
	}
	else if ( ( status & UHCI_TX_START ) != false )
	{
		uhci_event.event_type = TX_START;
	}
	else if ( ( status & UHCI_RX_HUNG ) != false )
	{
		uhci_event.event_type = RX_HUNG;
	}
	else if ( ( status & UHCI_TX_HUNG ) != false )
	{
		uhci_event.event_type = TX_HUNG;
	}
	else if ( ( status & UHCI_OUT_DONE ) != false )
	{
		uhci_event.event_type = OUT_DONE;
	}
	else if ( ( status & UHCI_OUT_EOF ) != false )
	{
		uhci_event.event_type = OUT_DONE;
		xSemaphoreGive( m_udma->tx_sem );
	}
	else if ( ( status & UHCI_IN_DONE ) != false )
	{
		length = m_udma->rx_link.des.length;
		addr_tmp = ( uint32_t )m_udma->rx_link.buf;
		m_udma->rx_link.buf = ( addr_tmp ==
								( uint32_t )m_udma->rx_buf + ( RINGBUF_SIZE - UDMA_LINK_SIZE ) ) ? m_udma->rx_buf :
			( uint8_t* )( addr_tmp + UDMA_LINK_SIZE );
		Uhci_Link_Restart( m_udma->uhci_num, UHCI_LINK_IN );
		uhci_event.send_sta = UDMA_LINK_LEN;
		if ( xRingbufferSendFromISR( m_udma->rx_ring_buf, ( void* )addr_tmp, length, &HP_awoken ) )
		{
			m_udma->rx_data_len += length;
			uhci_event.send_sta = UDMA_LINK_EOF;
		}
		uhci_event.data_len = length;
		uhci_event.event_type = IN_DONE;
	}
	else
	{
		uhci_event.event_type = UNDEFINED;
	}

	if ( m_udma->xQueueUhci != NULL )
	{
		xQueueSendFromISR( m_udma->xQueueUhci, ( void* )&uhci_event,  ( BaseType_t* ) 0 );
	}

	( *m_cback_func )( m_cback_param, m_cback_request );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_UHCI::Uhci_Link_Restart( uhci_num_t uhci_num, uhci_link_type_t link_type )
{
	Check_UHCI( uhci_num < UHCI_NUM_MAX, UHCI_NUM_ERR_STR, ESP_ERR_INVALID_ARG );
	Check_UHCI( link_type < UHCI_LINK_MAX, UHCI_LINK_TYPE_ERR_STR, ESP_ERR_INVALID_ARG );

	if ( link_type == UHCI_LINK_OUT )
	{
		portENTER_CRITICAL( &uhci_spinlock[uhci_num] );
		UHCI[uhci_num]->dma_out_link.restart = UDMA_LINK_EOF;
		portEXIT_CRITICAL( &uhci_spinlock[uhci_num] );
	}
	else
	{
		portENTER_CRITICAL( &uhci_spinlock[uhci_num] );
		UHCI[uhci_num]->dma_in_link.restart = UDMA_LINK_EOF;
		portEXIT_CRITICAL( &uhci_spinlock[uhci_num] );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_UHCI::Check_UHCI( bool validation_result, const char* str, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		ESP_LOGE( UHCI_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str );
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_UHCI::Config_Callback( cback_func_t cback_func,
							   cback_param_t cback_param, cback_status_t cback_request )
{
	m_cback_func = cback_func;
	m_cback_param = cback_param;
	m_cback_request = cback_request;
}
