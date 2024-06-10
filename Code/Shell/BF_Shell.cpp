/**
 *****************************************************************************************
 * @file
 *
 * @brief	Class implementation for Shell::BF_Shell::.
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
 * See header file for function definition.
 *****************************************************************************************
 */
BF_Shell::BF_Shell( uC_USART_Buff* usart_ptr, shell_config_t const* shell_config_ptr, Command_Line_Editor* cle,
					Command_Line_Interpreter* cli ) :
	Shell_Base( shell_config_ptr, cle, cli ),
	m_queue_ptr( nullptr ),
	m_usart_ptr( usart_ptr ),
	m_task_ptr( nullptr ),
	m_cmd_buffer_ptr( nullptr ),
	m_tx_head_ptr( nullptr ),
	m_tx_tail_ptr( nullptr ),
	m_tx_next_tail_ptr( nullptr ),
	m_tx_end_ptr( nullptr ),
	m_tx_busy( false ),
	m_function_tested( nullptr ),
	m_test_criteria( nullptr )
{
	// initialize instance of CR_Tasker:: and register our callback
	m_task_ptr = new CR_Tasker(
		&BF_Shell::Coroutine_Callback_Static,
		reinterpret_cast<CR_TASKER_PARAM>( this ),
		CR_TASKER_PRIORITY_0,
		"Shell task."
		);

	// initialize a CR_Queue for events from USART callback to coroutine
	m_queue_ptr = new CR_Queue( BF_Shell::QUEUE_LENGTH, static_cast<uint8_t>( sizeof( event_t ) ) );

	// register callback BF_Shell_USART_Callback()
	uint8_t status_bits = 0U;

	// lint -e(1960)
	Set_Bit( status_bits, uC_USART_Buff::RX_BYTE_TIMEOUT );
	// lint -e(1960)
	Set_Bit( status_bits, uC_USART_Buff::RX_BUFF_FULL );
	// lint -e(1960)
	Set_Bit( status_bits, uC_USART_Buff::TX_COMPLETE );

	m_usart_ptr->Config_Callback(
		&BF_Shell::UART_Callback_Static,
		reinterpret_cast<uC_USART_Buff::cback_param_t>( this ),
		status_bits,
		RX_TIMEOUT,
		TX_TIMEOUT
		);

	m_cmd_buffer_ptr = new shell_char_t[m_shell_config_ptr->m_command_buffer_size];
	m_function_tested = new shell_char_t[80];
	m_test_criteria = new shell_char_t[80];

	Transmit_Buffer_Initialize();
	Clear_Current_Test();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BF_Shell::~BF_Shell()
{
	delete m_task_ptr;
	delete m_queue_ptr;
	m_usart_ptr = nullptr;
	m_tx_head_ptr = nullptr;
	m_tx_tail_ptr = nullptr;
	m_tx_next_tail_ptr = nullptr;
	m_tx_end_ptr = nullptr;
	Clear_Current_Test();
	delete[] m_cmd_buffer_ptr;
	delete[] m_function_tested;
	delete[] m_test_criteria;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Put_Char( shell_char_t out_char )
{
	switch ( static_cast<uint32_t>( out_char ) )
	{
		case Shell_Base::ASCII_CR:
			break;

		case Shell_Base::ASCII_LF:
			Transmit_Buffer_Put_Char( Shell_Base::ASCII_CR );
			Transmit_Buffer_Put_Char( Shell_Base::ASCII_LF );
			break;

		default:
			Transmit_Buffer_Put_Char( out_char );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Put_String( shell_char_t const* out_string )
{
	/*
	 * NOTE: Don't fall for the trap of calling Start_TX( out_string, strlen(out_string) ) !!!
	 * It seems more efficient, but it will skip the handling of newline expansion !!!
	 */
	shell_char_t out_char;
	bool_t done = false;

	while ( !done )
	{
		out_char = *out_string;
		if ( out_char == '\0' )
		{
			done = true;
		}
		else
		{
			out_string++;
			Put_Char( out_char );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Put_String( shell_char_t const* out_string, uint32_t bytes_to_send )
{
	shell_char_t out_char;

	while ( 0U != bytes_to_send )
	{
		out_char = *out_string;
		Put_Char( out_char );
		out_string++;
		bytes_to_send--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Enter_Critical( void )
{
	// TODO implement me!
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Exit_Critical( void )
{
	// TODO implement me!
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
shell_char_t* BF_Shell::Get_New_Command_Buffer( void )
{
	memset( m_cmd_buffer_ptr, 0, m_shell_config_ptr->m_command_buffer_size );
	return ( m_cmd_buffer_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Release_Command_Buffer( shell_char_t* buffer )
{
	// buffer = buffer;
}

void BF_Shell::Set_Tested_Function_Name( const shell_char_t* name )
{
	memset( m_function_tested, 0, 80 );
	sprintf( m_function_tested, "%s.", name );
}

void BF_Shell::Set_Test_Criteria( const shell_char_t* name )
{
	memset( m_test_criteria, 0, 80 );
	sprintf( m_test_criteria, "%s.", name );
}

shell_char_t* BF_Shell::Get_Tested_Function_Name()
{
	return ( m_function_tested );
}

shell_char_t* BF_Shell::Get_Test_Criteria()
{
	return ( m_test_criteria );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Event_Dispatcher( event_t* event_ptr )
{
	switch ( event_ptr->m_type )
	{
		case BF_Shell::RECIEVE_DATA:
			Receive_Handler( event_ptr );
			break;

		case BF_Shell::TRANSMIT_COMPLETE:
			Transmit_Handler( event_ptr );
			break;

		case BF_Shell::ERROR:
		default:
			Error_Handler( event_ptr );
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Receive_Handler( event_t* event_ptr )
{
	shell_char_t in_char;
	uint16_t index;
	Shell_Base::command_status_t result;

	// for each character in receive buffer
	for ( index = 0U; index < event_ptr->m_size; index++ )
	{
		// fetch character
		in_char = event_ptr->m_data[index];

		// edit character into a command line
		if ( On_Arrival( in_char ) )
		{
			// parse command line into tokens
			result = Parse_Command_Line();
			if ( result == Shell_Base::SUCCESS )
			{
				// dispatch the command
				result = Dispatch();
				switch ( result )
				{
					case Shell_Base::NOT_FOUND:
						Put_String( "Unknown command.\n" );
						// complete the command, prepare for next
						Get_Cle_Ptr()->On_Completion( result );
						break;

					case Shell_Base::FAILURE:
						Put_String( "Argument Mismatch.\n" );
						Get_Cle_Ptr()->On_Completion( result );
						break;

					case Shell_Base::SUCCESS:
					case Shell_Base::BUSY:
					case Shell_Base::ABORTED:
					default:
						break;
				}
			}
			else
			{
				Get_Cle_Ptr()->On_Completion( result );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Transmit_Handler( event_t* event_ptr )
{
	// event_ptr = event_ptr;
	m_tx_tail_ptr = m_tx_next_tail_ptr;
	Transmit_Buffer_Flush();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Error_Handler( event_t* event_ptr ) const
{
	// event_ptr = event_ptr;
	// TODO: consider what errors need to be handled, and how?
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

#define COROUTINE_T	/*lint -e{646, 725, 744, 904, 1924, 1960} */ void

COROUTINE_T BF_Shell::Coroutine_Callback( void )
{
	BF_Shell::event_t event;

	event.m_type = ERROR;
	event.m_size = 0U;

	CR_QUEUE_STATUS result;
	test_status_t test_status;

	CR_Tasker_Begin( m_task_ptr );
	Initialize();
	for (;;)
	{
		CR_Queue_Receive( m_task_ptr, m_queue_ptr, &event, CR_QUEUE_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			Event_Dispatcher( &event );
			if ( Get_Current_Test() != nullptr )
			{
				test_status = Get_Test_Status();
				while ( test_status == TEST_ENUMS_IN_PROGRESS )
				{
					CR_Tasker_Yield( m_task_ptr );
					test_status = Get_Test_Status();
				}
				Clear_Current_Test();
			}
		}
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Test_Base* BF_Shell::Get_Current_Test( void )
{
	return ( m_current_test );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Set_Current_Test( Test_Base* test )
{
	m_current_test = test;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Clear_Current_Test( void )
{
	m_current_test = nullptr;
	memset( m_function_tested, 0, 80 );
	memset( m_test_criteria, 0, 80 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
test_status_t BF_Shell::Get_Test_Status( void )
{
	return ( m_current_test->Get_Test_Status() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// lint -e{1960}
void BF_Shell::UART_Callback( uC_USART_Buff::cback_status_t status_bits )
{
	uint16_t rx_len = 0U;
	BF_Shell::event_t event;

	memset( &event, 0, sizeof( event ) );

	// ascertain reason for callback


	if ( Test_Bit( status_bits,
				   uC_USART_Buff::RX_BYTE_TIMEOUT ) || Test_Bit( status_bits, uC_USART_Buff::RX_BUFF_FULL ) )
	{
		rx_len = m_usart_ptr->Get_RX_Length();

		// build message
		event.m_type = RECIEVE_DATA;
		memcpy( event.m_data, m_rx_buffer, rx_len );
		event.m_size = rx_len;

		m_usart_ptr->Start_RX( m_rx_buffer, RX_CHAR_MAX );

		// send to queue
		m_queue_ptr->Send_From_ISR( &event, false );

	}

	if ( Test_Bit( status_bits, uC_USART_Buff::TX_COMPLETE ) )
	{
		// build message
		event.m_type = TRANSMIT_COMPLETE;

		// send to queue
		m_queue_ptr->Send_From_ISR( &event, false );

		Transmit_Buffer_Flush();
	}


}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Transmit_Buffer_Initialize( void )
{
	m_tx_head_ptr = &m_tx_buffer[0];
	m_tx_tail_ptr = &m_tx_buffer[0];
	m_tx_next_tail_ptr = &m_tx_buffer[0];
	m_tx_end_ptr = &m_tx_buffer[( TX_CHAR_MAX - 1U )];
	m_tx_busy = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Transmit_Buffer_Put_Char( uint8_t out_char )
{
	*m_tx_head_ptr = out_char;
	m_tx_head_ptr++;
	if ( m_tx_head_ptr > m_tx_end_ptr )
	{
		m_tx_head_ptr = &m_tx_buffer[0];
	}

	if ( !m_tx_busy )
	{
		Transmit_Buffer_Flush();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Transmit_Buffer_Flush( void )
{
	int32_t delta = m_tx_head_ptr - m_tx_tail_ptr;

	// are there characters in buffer (non-wrapped case)?

	if ( delta > 0 )
	{
		m_tx_next_tail_ptr = m_tx_head_ptr;
		m_usart_ptr->Start_TX( m_tx_tail_ptr, static_cast<uint16_t>( delta ) );
		m_tx_busy = true;
	}
	// are there characters in buffer (wrapped case)?
	else if ( delta < 0 )
	{
		m_tx_next_tail_ptr = &m_tx_buffer[0];
		delta = ( m_tx_end_ptr + 1 ) - m_tx_tail_ptr;
		m_usart_ptr->Start_TX( m_tx_tail_ptr, static_cast<uint16_t>( delta ) );
		m_tx_busy = true;
	}
	// otherwise we're empty
	else
	{
		m_tx_busy = false;
	}
	// m_tx_busy = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Internal_Initialize( void )
{
	Put_String( "\n\n\n" );
	m_usart_ptr->Start_RX( m_rx_buffer, static_cast<uint16_t>( RX_CHAR_MAX ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Shell::Put_String_Now( shell_char_t const* out_string, uint32_t bytes_to_send )
{
	// Put data in circular buffer
	Put_String( ( char* )out_string, bytes_to_send );

	// Transmit data on serial interface without waiting for shell CR tasker
	Transmit_Buffer_Flush();
}

}	// namespace Shell
