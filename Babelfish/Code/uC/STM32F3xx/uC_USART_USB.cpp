/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_USART_USB.h"
#include "Exception.h"
#include "Babelfish_Assert.h"

extern "C"
{
#include "usbd_desc.h"
}

static uC_USART_USB* g_usart_usb_ptr = reinterpret_cast<uC_USART_USB*>( nullptr );

const uint16_t USB_MAX_PACKET = static_cast<uint16_t>( CDC_DATA_FS_MAX_PACKET_SIZE );
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
uC_USART_USB::uC_USART_USB( uint8_t hw_timer_num, uint8_t id ) :
	uC_USART_Buff(),
	m_is_connected( false ),
	m_rx_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_rx_buff_len( 0U ),
	m_rx_buff_index( 0U ),
	m_tx_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_tx_buff_len( 0U ),
	m_tx_buff_index( 0U ),
	m_cback_request( 0U ),
	m_cback_func( reinterpret_cast<uC_USART_Buff::cback_func_t>( nullptr ) ),
	m_cback_param( reinterpret_cast<uC_USART_Buff::cback_param_t>( nullptr ) ),
	m_tx_holdoff( false ),
	m_timer( reinterpret_cast<uC_Multi_Timers*>( nullptr ) ),
	m_rx_state( RX_IDLE ),
	m_tx_state( TX_IDLE ),
	m_premature_rx_timeout( false ),
	m_premature_tx_timeout( false ),
	m_bitrate( 19200U ),
	/* baud rate */
	m_format( 0U ),
	/* stop bits = 1*/
	m_paritytype( 0U ),
	/* parity = none*/
	m_datatype( 8U )	/* nb. of bits = 8*/
{
	BF_ASSERT( g_usart_usb_ptr == nullptr );

	g_usart_usb_ptr = this;

	Push_TGINT();

	USBD_Init( &m_USBD_CDC_Device, &CDC_Desc, id );

	USBD_RegisterClass( &m_USBD_CDC_Device, &USBD_CDC );

	USBD_CDC_RegisterInterface( &m_USBD_CDC_Device, &USBD_CDC_fops );

	USBD_Start( &m_USBD_CDC_Device );
	// Because of the possibility to miss a timer interrupt we should set the interrupt priority
	// of the timer to be less than the rx interrupt.
	m_timer = new uC_Multi_Timers( hw_timer_num, TOTAL_TIMERS, uC_INT_HW_PRIORITY_13 );

	m_timer->Set_Callback( static_cast<uint8_t>( RX_TIMER ), &RX_Timer_Int_Static, this );
	m_timer->Set_Callback( static_cast<uint8_t>( TX_TIMER ), &TX_Timer_Int_Static, this );

	Stop_TX();
	Stop_RX();

	m_tx_holdoff = false;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
uC_USART_USB::~uC_USART_USB()
{
	m_rx_buff = reinterpret_cast<uint8_t*>( nullptr );
	m_tx_buff = reinterpret_cast<uint8_t*>( nullptr );
	m_cback_func = reinterpret_cast<uC_USART_Buff::cback_func_t>( nullptr );
	m_cback_param = reinterpret_cast<uC_USART_Buff::cback_param_t>( nullptr );
	delete m_timer;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
void uC_USART_USB::Config_Callback( uC_USART_Buff::cback_func_t cback_func,
									uC_USART_Buff::cback_param_t cback_param,
									uC_USART_Buff::cback_status_t cback_req_mask,
									uint32_t rx_byte_time,
									uint32_t tx_holdoff_time )
{
	Stop_TX();
	Stop_RX();

	m_cback_func = cback_func;
	m_cback_param = cback_param;
	m_cback_request = cback_req_mask;

	Config_Timers( rx_byte_time, tx_holdoff_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_USB::Config_Timers( uint32_t rx_byte_time, uint32_t tx_holdoff_time )
{
	uint32_t rx_freq_calc;
	uint32_t tx_freq_calc;

	rx_freq_calc = Round_Div( M_TIME_TO_FREQ_CALC, rx_byte_time );
	tx_freq_calc = Round_Div( M_TIME_TO_FREQ_CALC, tx_holdoff_time );

	if ( tx_freq_calc < rx_freq_calc )
	{
		m_timer->Set_Min_Frequency( tx_freq_calc );
	}
	else
	{
		m_timer->Set_Min_Frequency( rx_freq_calc );
	}
	if ( ( tx_holdoff_time != 0U ) &&
		 ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) ||
		   BF_Lib::Bit::Test( m_cback_request,
							  TX_HOLDOFF_COMPLETE ) ) )
	{
		m_timer->Set_Timeout( static_cast<uint8_t>( TX_TIMER ), tx_freq_calc, false );
		m_tx_holdoff = false;
	}

	if ( ( rx_byte_time != 0U ) && BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
	{
		m_timer->Set_Timeout( static_cast<uint8_t>( RX_TIMER ), rx_freq_calc, false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
void uC_USART_USB::Start_RX( uint8_t* data, uint16_t length )
{
	Push_TGINT();
	Stop_RX();

	m_rx_state = RX_WAITING;
	m_premature_rx_timeout = false;

	m_rx_buff = data;
	m_rx_buff_index = 0U;
	m_rx_buff_len = length;

	USBD_CDC_SetRxBuffer( &m_USBD_CDC_Device, m_rx_buff );
	USBD_CDC_ReceivePacket( &m_USBD_CDC_Device );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
uC_USART_USB::rx_state_t uC_USART_USB::RX_State( void )
{
	Push_TGINT();
	uC_USART_USB::rx_state_t retval = m_rx_state;

	Pop_TGINT();
	return ( retval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
uint16_t uC_USART_USB::Get_RX_Length( void )
{
	Push_TGINT();
	uint16_t retval = m_rx_buff_index;

	Pop_TGINT();
	return ( retval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
void uC_USART_USB::Stop_RX( void )
{
	Push_TGINT();
	m_rx_state = RX_IDLE;
	m_timer->Stop_Timeout( static_cast<uint8_t>( RX_TIMER ) );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
void uC_USART_USB::Start_TX( uint8_t* data, uint16_t length )
{
	// Debug_0_Off();	// Indicate end of 3.5-character delay.
	Push_TGINT();
	Stop_TX();

	m_tx_state = TX_RUNNING;

	m_tx_holdoff = true;
	m_premature_tx_timeout = false;

	m_tx_buff = data;
	m_tx_buff_index = 0U;
	m_tx_buff_len = length;


	USBD_CDC_SetTxBuffer( &m_USBD_CDC_Device, m_tx_buff, m_tx_buff_len );
	USBD_CDC_TransmitPacket( &m_USBD_CDC_Device );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
uC_USART_USB::tx_state_t uC_USART_USB::TX_State( void )
{
	Push_TGINT();
	uC_USART_USB::tx_state_t retval = m_tx_state;

	Pop_TGINT();
	return ( retval );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
bool uC_USART_USB::TX_Holdoff( void )
{
	return ( m_tx_holdoff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// called from mainline code
void uC_USART_USB::Stop_TX( void )
{
	Push_TGINT();
	m_tx_state = TX_IDLE;
	m_tx_holdoff = false;

	m_timer->Stop_Timeout( static_cast<uint8_t>( TX_TIMER ) );
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USART_USB::Config_Port( uint32_t baud_rate, uC_USART_Buff::char_size_t char_size,
								uC_USART_Buff::parity_t parity,
								uC_USART_Buff::stop_bit_t stop_bit_count )
{
	/*
	   // NOTE: USB CDC Virtual Com Port doesn't *REALLY* need to worry about baud, parity, etc.
	   // TODO double-check that uC_USART_Buff:: definitions match USBD_CDC !!! Translate as needed
	      !!!
	    m_bitrate = baud_rate;
	    m_format = stop_bit_count;
	    m_paritytype = parity;
	    m_datatype = char_size;
	 */
	baud_rate = baud_rate;
	char_size = char_size;
	parity = parity;
	stop_bit_count = stop_bit_count;
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback from ISR
int8_t uC_USART_USB::Usb_Cdc_Init( void )
{
	m_is_connected = true;
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback from ISR
int8_t uC_USART_USB::Usb_Cdc_DeInit( void )
{
	m_is_connected = false;
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback from ISR
int8_t uC_USART_USB::Usb_Cdc_Control( uint8_t cmd, uint8_t* pbuf, uint16_t length )
{
	uint32_t temp;
	uint32_t rx_byte_time;
	uint32_t tx_holdoff_time;

	switch ( cmd )
	{
		case CDC_SET_LINE_CODING:
			m_bitrate = pbuf[0];
			temp = pbuf[1];
			temp <<= 8;
			m_bitrate |= temp;
			temp = pbuf[2];
			temp <<= 16;
			m_bitrate |= temp;
			temp = pbuf[3];
			temp <<= 24;
			m_bitrate |= temp;
			m_format = pbuf[4];
			m_paritytype = pbuf[5];
			m_datatype = pbuf[6];

			// The following is architecturally unattractive because the baud rate
			// comes from an upper layer, namely Modbus_UART, but in this case it's
			// coming from a lower layer, namely, the USB CDC, so the following
			// if-else clause is a duplication of what already exists in Modbus_UART.
			if ( m_bitrate > 19200U )
			{
				rx_byte_time = 750U;		// in microseconds;
				tx_holdoff_time = 1750U;	// in microseconds;
			}
			else
			{
				rx_byte_time = static_cast<uint32_t>( 1000000.0 * 11.0 * 1.5 ) / m_bitrate;		// in
																								// microseconds
				tx_holdoff_time = static_cast<uint32_t>( 1000000.0 * 11.0 * 3.5 ) / m_bitrate;		// in
																									// microseconds
			}
			Config_Timers( rx_byte_time, tx_holdoff_time );
			break;

		case CDC_GET_LINE_CODING:
			pbuf[0] = static_cast<uint8_t>( m_bitrate );
			pbuf[1] = static_cast<uint8_t>( m_bitrate >> 8 );
			pbuf[2] = static_cast<uint8_t>( m_bitrate >> 16 );
			pbuf[3] = static_cast<uint8_t>( m_bitrate >> 24 );
			pbuf[4] = m_format;
			pbuf[5] = m_paritytype;
			pbuf[6] = m_datatype;
			break;

		default:
			break;
	}
	length = length;
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback from ISR
int8_t uC_USART_USB::Usb_Cdc_Receive( uint8_t* Buf, uint32_t* Len )
{
	cback_status_t cback_status_bits = 0U;

	if ( RX_IDLE != m_rx_state )
	{
		m_rx_state = RX_RECEIVING;

		uint16_t length = static_cast<uint16_t>( *Len );

		// add the number of bytes received to the index
		m_rx_buff_index += length;

		if ( length < USB_MAX_PACKET )
		{
			// Debug_0_On();	// Indicate beginning of 3.5-character delay.
			if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
			{
				if ( m_timer->Timeout_Running( static_cast<uint8_t>( RX_TIMER ) ) )
				{
					m_timer->Restart_Timeout( static_cast<uint8_t>( RX_TIMER ) );
				}
				else
				{
					m_timer->Start_Timeout( static_cast<uint8_t>( RX_TIMER ) );
				}
				// If we get here and a timeout is pending then we likely were
				// delayed in processing the timer interrupt.  So we make sure to follow
				// on and reset it later.
				if ( m_timer->Timeout_Pending( static_cast<uint8_t>( RX_TIMER ) ) )
				{
					m_premature_rx_timeout = true;
				}
			}

			if ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
			{
				if ( m_timer->Timeout_Running( static_cast<uint8_t>( TX_TIMER ) ) )
				{
					m_timer->Restart_Timeout( static_cast<uint8_t>( TX_TIMER ) );
				}
				else
				{
					m_timer->Start_Timeout( static_cast<uint8_t>( TX_TIMER ) );
					m_tx_holdoff = true;
				}
				// If we get here and a timeout is pending then we likely were
				// delayed in processing the timer interrupt.  So we make sure to follow
				// on and reset it later.
				if ( m_timer->Timeout_Pending( static_cast<uint8_t>( TX_TIMER ) ) )
				{
					m_premature_tx_timeout = true;
				}
			}
		}
		else
		{
			// if we have room for another full endpoint buffer's worth
			if ( ( m_rx_buff_index + USB_MAX_PACKET ) < m_rx_buff_len )
			{
				// get set up for another one
				USBD_CDC_SetRxBuffer( &m_USBD_CDC_Device, &m_rx_buff[m_rx_buff_index] );
				USBD_CDC_ReceivePacket( &m_USBD_CDC_Device );
			}
			else
			{
				m_rx_state = RX_IDLE;
				if ( BF_Lib::Bit::Test( m_cback_request,
										( static_cast<cback_status_t>( RX_BUFF_FULL ) ) ) )
				{
					BF_Lib::Bit::Set( cback_status_bits,
									  static_cast<cback_status_t>( RX_BUFF_FULL ) );
					( *m_cback_func )( m_cback_param, cback_status_bits );
				}
			}
		}
	}
	Buf = Buf;
	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback from ISR
int8_t uC_USART_USB::Usb_Cdc_Transmit( uint8_t* Buf, uint32_t* Len )
{
	cback_status_t cback_status_bits = 0U;

	BF_Lib::Unused<uint8_t*>::Okay( Buf );
	BF_Lib::Unused<uint32_t*>::Okay( Len );

	if ( TX_RUNNING == m_tx_state )
	{
		m_tx_state = TX_IDLE;

		if ( BF_Lib::Bit::Test( m_cback_request,
								( static_cast<cback_status_t>( TX_COMPLETE ) ) ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, static_cast<cback_status_t>( TX_COMPLETE ) );
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}

		if ( BF_Lib::Bit::Test( m_cback_request, TX_HOLDOFF_COMPLETE ) ||
			 BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
		{
			if ( m_timer->Timeout_Running( static_cast<uint8_t>( TX_TIMER ) ) )
			{
				m_timer->Restart_Timeout( static_cast<uint8_t>( TX_TIMER ) );
			}
			else
			{
				m_timer->Start_Timeout( static_cast<uint8_t>( TX_TIMER ) );
				m_tx_holdoff = true;
			}
			// If we get here and a timeout is pending then we likely were
			// delayed in processing the timer interrupt.  So we make sure to follow
			// on and reset it later.
			if ( m_timer->Timeout_Pending( static_cast<uint8_t>( TX_TIMER ) ) )
			{
				m_premature_tx_timeout = true;
			}
		}
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_USB::RX_Timer_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	if ( m_premature_rx_timeout == false )
	{
		m_rx_state = RX_IDLE;

		m_timer->Stop_Timeout( static_cast<uint8_t>( RX_TIMER ) );

		if ( BF_Lib::Bit::Test( m_cback_request, RX_BYTE_TIMEOUT ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, RX_BYTE_TIMEOUT );
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}
	}
	else
	{
		// We restart the timeout because we had a receive happen before we were really ready.
		// likely because we were delayed in processing.
		m_timer->Start_Timeout( static_cast<uint8_t>( RX_TIMER ) );
		m_premature_rx_timeout = false;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART_USB::TX_Timer_Int( void )
{
	cback_status_t cback_status_bits = 0U;

	if ( m_premature_tx_timeout == false )
	{
		m_tx_state = TX_IDLE;
		m_tx_holdoff = false;

		m_timer->Stop_Timeout( static_cast<uint8_t>( TX_TIMER ) );

		if ( BF_Lib::Bit::Test( m_cback_request, TX_HOLDOFF_COMPLETE ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, TX_HOLDOFF_COMPLETE );
		}
		if ( BF_Lib::Bit::Test( m_cback_request, TXRX_HOLDOFF_COMPLETE ) )
		{
			BF_Lib::Bit::Set( cback_status_bits, TXRX_HOLDOFF_COMPLETE );
		}

		if ( cback_status_bits != 0U )
		{
			( *m_cback_func )( m_cback_param, cback_status_bits );
		}
	}
	else
	{
		// We restart the timeout because we had a receive happen before we were really ready.
		// likely because we were delayed in processing.
		m_timer->Start_Timeout( static_cast<uint8_t>( TX_TIMER ) );
		m_premature_tx_timeout = false;
	}
}

/*
 *****************************************************************************************
 * The following implements usbd_cdc_interface.c/.h but in a C++ friendly manner.
 *****************************************************************************************
 */

extern "C"
{

/**
 * Internal callback upon USB CDC connection establishment.
 */
// callback from ISR
static int8_t CDC_Itf_Init( void )
{
	return ( g_usart_usb_ptr->Usb_Cdc_Init() );
}

/**
 * Internal callback upon USB CDC disconnection.
 */
// callback from ISR
static int8_t CDC_Itf_DeInit( void )
{
	return ( g_usart_usb_ptr->Usb_Cdc_DeInit() );
}

/**
 * Internal callback upon USB CDC control message.
 */
// callback from ISR
static int8_t CDC_Itf_Control( uint8_t cmd, uint8_t* pbuf, uint16_t length )
{
	return ( g_usart_usb_ptr->Usb_Cdc_Control( cmd, pbuf, length ) );
}

/**
 * Internal callback upon USB CDC receipt of data IN message.
 */
// callback from ISR
static int8_t CDC_Itf_Receive( uint8_t* Buf, uint32_t* Len )
{
	return ( g_usart_usb_ptr->Usb_Cdc_Receive( Buf, Len ) );
}

/**
 * Internal callback upon USB CDC completion of data OUT message.
 */
// callback from ISR
static int8_t CDC_Itf_Transmit( uint8_t* Buf, uint32_t* Len, uint8_t epnum )
{
	UNUSED( epnum );
	return ( g_usart_usb_ptr->Usb_Cdc_Transmit( Buf, Len ) );
}

/**
 * Table of callback functions for USB CDC class.
 */
USBD_CDC_ItfTypeDef USBD_CDC_fops =
{
	&CDC_Itf_Init,
	&CDC_Itf_DeInit,
	&CDC_Itf_Control,
	&CDC_Itf_Receive,
	&CDC_Itf_Transmit,
};

}

