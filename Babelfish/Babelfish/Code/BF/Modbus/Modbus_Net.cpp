/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_Net.h"
#include "Modbus_CRC.h"
#include "Modbus_Defines.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"
#include "Device_Defines.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */// Slave implementation.
Modbus_Net::Modbus_Net( uint8_t address, Modbus_Serial* modbus_serial_ctrl,
						const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
						DCI_Owner* comm_dog_owner,
						uint16_t buffer_size, bool correct_out_of_range_sets ) :
	m_mb_serial( modbus_serial_ctrl ),
	m_mb_msg( new BF_Mbus::Modbus_Msg( modbus_dev_profile, comm_dog_owner,
									   ( buffer_size - MB_RTU_CRC_SIZE ),
									   correct_out_of_range_sets ) ),
	m_tx_mode( MODBUS_RTU_TX_MODE ),
	m_address( address ),
	m_resp_cback( reinterpret_cast<mb_net_resp_cback>( nullptr ) ),
	m_resp_cback_handle( reinterpret_cast<mb_net_resp_cback_handle>( nullptr ) ),
	m_serial_rx_proc_cback( &Process_Req_Static ),
	m_tx_buff_size( buffer_size ),
	m_count_of_crc_failures( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_num_crc_errors( 0U )
{
	/**
	 * The argument "buffer_size" should be equal to MODBUS_DEFAULT_BUFFER_SIZE.
	 * Otherwise, define MODBUS_NET_BUFFER_SIZE to buffer_size value.
	 * Example: For buffer size 513
	 * Add "-D MODBUS_NET_BUFFER_SIZE=513" in common_define_config.xcl
	 */
	// coverity[leaked_storage]
	BF_ASSERT( buffer_size == MODBUS_DEFAULT_BUFFER_SIZE );
	Disable_Port();
	// coverity[leaked_storage]
	new BF::System_Reset( &Ready_For_Reset_Static,
						  static_cast<BF::System_Reset::cback_param_t>( this ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */// External Fieldbus Slave implementation.
Modbus_Net::Modbus_Net( uint8_t address, Modbus_Serial* modbus_serial_ctrl,
						BF_Mbus::Modbus_Msg* mbus_msg ) :
	m_mb_serial( modbus_serial_ctrl ),
	m_mb_msg( mbus_msg ),
	m_tx_mode( MODBUS_RTU_TX_MODE ),
	m_address( address ),
	m_resp_cback( reinterpret_cast<mb_net_resp_cback>( nullptr ) ),
	m_resp_cback_handle( reinterpret_cast<mb_net_resp_cback_handle>( nullptr ) ),
	// coverity[leaked_storage]
	m_serial_rx_proc_cback( &Process_Req_Static ),
	m_tx_buff_size( 0U ),
	m_count_of_crc_failures( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_num_crc_errors( 0U )
{
	m_tx_buff_size = m_mb_msg->Get_Msg_Buffer_Size() + MB_RTU_CRC_SIZE;

	Disable_Port();
	// coverity[leaked_storage]
	new BF::System_Reset( &Ready_For_Reset_Static,
						  static_cast<BF::System_Reset::cback_param_t>( this ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Net::~Modbus_Net( void )
{
	delete m_mb_serial;
	delete m_mb_msg;

	m_resp_cback = reinterpret_cast<mb_net_resp_cback>( nullptr );
	m_resp_cback_handle = reinterpret_cast<mb_net_resp_cback_handle>( nullptr );
	m_serial_rx_proc_cback = reinterpret_cast<Modbus_Serial::MB_RX_PROC_CBACK>( nullptr );
	m_count_of_crc_failures = reinterpret_cast<DCI_Owner*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */// Master implementation.
Modbus_Net::Modbus_Net( Modbus_Serial* modbus_serial_ctrl,
						mb_net_resp_cback resp_cback, mb_net_resp_cback_handle resp_cback_handle ) :
	m_mb_serial( modbus_serial_ctrl ),
	m_mb_msg( reinterpret_cast<BF_Mbus::Modbus_Msg*>( nullptr ) ),
	m_tx_mode( MODBUS_RTU_TX_MODE ),
	m_address( 0U ),
	m_resp_cback( reinterpret_cast<mb_net_resp_cback>( nullptr ) ),
	m_resp_cback_handle( reinterpret_cast<mb_net_resp_cback_handle>( nullptr ) ),
	m_serial_rx_proc_cback( reinterpret_cast<Modbus_Serial::MB_RX_PROC_CBACK>( nullptr ) ),
	m_tx_buff_size( 0U ),
	m_count_of_crc_failures( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_num_crc_errors( 0U )
{
	// Get Modbus serial handler
	m_mb_serial = modbus_serial_ctrl;// m_mb_serial = new Modbus_Serial( Process_Resp_Static,
									// (void*)this, usart_ctrl,
									// modbus_display_ctrl, rx_buffer_size );

	m_resp_cback = resp_cback;
	m_resp_cback_handle = resp_cback_handle;

	m_tx_buff_size = 0U;

	m_mb_msg = reinterpret_cast<BF_Mbus::Modbus_Msg*>( nullptr );

	m_address = 0U;

	m_serial_rx_proc_cback = &Process_Resp_Static;

	Disable_Port();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Enable_Port( tx_mode_t modbus_tx_mode, parity_t parity,
							  uint32_t baud_rate, bool use_long_holdoff, stop_bits_t stop_bits )
{
	m_tx_mode = modbus_tx_mode;
	m_mb_serial->Enable_Port( m_serial_rx_proc_cback, reinterpret_cast<void*>( this ),
							  modbus_tx_mode, parity, baud_rate, use_long_holdoff, stop_bits );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Disable_Port( void )
{
	m_mb_serial->Disable_Port();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Net::Port_Enabled( void )
{
	return ( m_mb_serial->Port_Enabled() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Change_Address( uint8_t address )
{
	m_address = address;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Process_Req( MB_FRAME_STRUCT* rx_struct )
{
	uint8_t fault_code = MB_NO_ERROR_CODE;
	MB_FRAME_STRUCT tx_msg_struct;

	if ( Check_CRC_And_Convert( rx_struct ) )
	{
		// Must check the address after the conversion.
		// Otherwise we have to know whether it is ASCII or RTU
		if ( Msg_For_Me( rx_struct->data[MB_NET_RTU_ADDRESS_IND] ) )
		{
			tx_msg_struct.data = m_tx_buff;
			tx_msg_struct.data_length = 0U;

			fault_code = m_mb_msg->Process_Msg( rx_struct, &tx_msg_struct );
			// Process Msg is expecting a frame sans the CRC length.
			// It will also return a length that is sans the CRC length.

			if ( rx_struct->data[MB_NET_RTU_ADDRESS_IND] != MODBUS_BROADCAST_ADDRESS )
			{
				m_tx_buff[MB_NET_RTU_ADDRESS_IND] = rx_struct->data[MB_NET_RTU_ADDRESS_IND];

				if ( fault_code != MB_NO_ERROR_CODE )
				{
					m_tx_buff[MB_EXCEPTION_FUNC_CODE_IND] |= MB_EXCEPTION_RESP_FUNC_CODE_MASK;
					m_tx_buff[MB_EXCEPTION_CODE_IND] = fault_code;
					tx_msg_struct.data_length = MB_EXCEPTION_RESP_HEADER_SIZE;
				}

				Send_Frame( &tx_msg_struct );		// m_tx_buff, tx_length );
			}
		}
	}
	else
	{
		m_num_crc_errors++;
		if ( m_count_of_crc_failures != nullptr )
		{
			m_count_of_crc_failures->Check_In( m_num_crc_errors );
			m_count_of_crc_failures->Load_Current_To_Init();
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Net::Process_Msg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	return ( m_mb_msg->Process_Msg( rx_struct, tx_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Process_Resp( MB_FRAME_STRUCT* rx_struct )
{
	if ( Check_CRC_And_Convert( rx_struct ) )
	{
		// Process Response is expecting a frame with all data intact minus the CRC.
		( *m_resp_cback )( m_resp_cback_handle, rx_struct );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Net::Check_CRC_And_Convert( MB_FRAME_STRUCT* rx_struct ) const
{
	bool msg_good = false;
	bool temp_data_ln_chk;
	bool temp_mbus_crc_chk;

	switch ( m_tx_mode )
	{
		case MODBUS_RTU_TX_MODE:
			if ( rx_struct->data_length <= MODBUS_MAX_RTU_MSG_LENGTH )
			{
				temp_data_ln_chk = true;
			}
			else
			{
				temp_data_ln_chk = false;
			}

			temp_mbus_crc_chk = BF_Mbus::MODBUS_CRC_Check_String_For_Good_CRC( rx_struct->data,
																			   rx_struct->data_length );

			if ( ( rx_struct->data_length >= MODBUS_MIN_RTU_MSG_LENGTH ) &&
				 temp_data_ln_chk && temp_mbus_crc_chk )
			{
				rx_struct->data_length -= MB_RTU_CRC_SIZE;
				msg_good = true;
			}
			else
			{
				msg_good = false;
			}
			break;

		case MODBUS_ASCII_TX_MODE:
			if ( ( rx_struct->data_length >= MODBUS_MIN_ASCII_MSG_LENGTH ) &&
				 ( rx_struct->data_length <= MODBUS_MAX_ASCII_MSG_LENGTH ) )
			{
				rx_struct->data_length -= MODBUS_ASCII_FRAMING_SIZE;	// Get rid of the ':', CR, LF
				msg_good = Convert_String_ASCII2HEX( rx_struct->data + 1, rx_struct->data_length,
													 rx_struct->data );
				if ( msg_good == true )
				{
					if ( BF_Mbus::MODBUS_LRC_Check_String_For_Good_LRC( rx_struct->data,
																		rx_struct->data_length ) )
					{
						rx_struct->data_length -= MB_ASCII_BYTE_CRC_SIZE;
						msg_good = true;
					}
					else
					{
						msg_good = false;
					}
				}
			}
			break;

		case MODBUS_TCP_TX_MODE:
			msg_good = true;
			break;

		default:
			// BF_ASSERT(false);
			break;
	}

	return ( msg_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Send_Frame( MB_FRAME_STRUCT* tx_struct )
{
	switch ( m_tx_mode )
	{
		case MODBUS_RTU_TX_MODE:
			Create_RTU_Frame( tx_struct->data, tx_struct->data_length );
			break;

		case MODBUS_ASCII_TX_MODE:
			Create_ASCII_Frame( tx_struct->data, tx_struct->data_length );
			break;

		case MODBUS_TCP_TX_MODE:
			break;

		default:
			BF_ASSERT( false );
			break;
	}
	m_mb_serial->Send_Frame( tx_struct );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Attach_User_Function( uint16_t function_code,
									   modbus_message_callback_param_t param,
									   modbus_message_callback_t callback )
{
	m_mb_msg->Attach_Callback( function_code, callback, param );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Monitor_Failed_CRC_Count( DCI_Owner* count_of_failures )
{
	m_count_of_crc_failures = count_of_failures;
	m_count_of_crc_failures->Check_Out( &m_num_crc_errors );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Create_RTU_Frame( uint8_t* tx_data, uint16_t& tx_length ) const
{
	BF_Lib::SPLIT_UINT16 temp_crc;

	temp_crc.u16 = BF_Mbus::MODBUS_CRC_Calc_On_String( tx_data, tx_length );

	tx_data[tx_length] = temp_crc.u8[0];
	tx_data[tx_length + 1U] = temp_crc.u8[1];
	tx_length += MB_RTU_CRC_SIZE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Create_ASCII_Frame( uint8_t* tx_data, uint16_t& tx_length ) const
{
	tx_data[tx_length] = BF_Mbus::MODBUS_LRC_Calc_On_String( tx_data, tx_length );
	tx_length++;	// Add the size of the LRC.

	Convert_String_HEX2ASCII( tx_data, tx_length, &tx_data[1] );

	tx_data[MODBUS_ASCII_BOF_LOC] = MODBUS_ASCII_BOF;
	tx_length++;
	tx_data[tx_length] = MODBUS_ASCII_CR;
	tx_data[tx_length + 1U] = MODBUS_ASCII_LF;
	tx_length += MB_ASCII_CHAR_EOF_SIZE;		// This adds the CR and LF lengths to the buffer.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Net::Convert_String_ASCII2HEX( uint8_t* src_data, uint16_t& length,
										   uint8_t* dest_data, bool high_nibble_first ) const
{
	bool conv_good = true;
	uint8_t high_byte;
	uint8_t low_byte;
	uint_fast16_t temp_length = length;

	if ( Is_Number_Odd( temp_length ) )
	{
		conv_good = false;
	}

	while ( ( temp_length > 0U ) && ( conv_good == true ) )
	{
		if ( high_nibble_first )
		{
			high_byte = Convert_ASCII2HEX( src_data[0] );
			low_byte = Convert_ASCII2HEX( src_data[1] );
		}
		else
		{
			high_byte = Convert_ASCII2HEX( src_data[1] );
			low_byte = Convert_ASCII2HEX( src_data[0] );
		}
		if ( ( high_byte == 0xFFU ) || ( low_byte == 0xFFU ) )
		{
			conv_good = false;
		}
		else
		{
			*dest_data = low_byte | static_cast<uint8_t>( ( high_byte << 4U ) );
		}
		temp_length -= 2U;
		src_data += 2U;
		dest_data++;
	}

	if ( conv_good == true )
	{
		length = length >> 1U;
	}

	return ( conv_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Net::Convert_ASCII2HEX( uint8_t ascii_data ) const
{
	uint8_t hex_val;	// For the 8bit machines.
	uint8_t ascii_uppcase_a = 'A';
	uint8_t ascii_lowercase_a = 'a';
	uint8_t ascii_zero = '0';

	hex_val = ascii_data - ascii_zero;

	if ( hex_val > 0x09U )
	{
		/**
		 * @remark Coding Standard Deviation:
		 * @n MISRA-C++[2008] Rule:
		 * @n PCLint: Info 734: Loss of precision (9bit to 8bit).
		 * @n Justification: These values are checked below for sanity (actual hex value).
		 */
		/*lint -e{734} */
		hex_val = ( ( hex_val + ascii_zero ) - ascii_uppcase_a ) + 10U;
		if ( hex_val > 0x0FU )
		{
			hex_val = ( hex_val + ascii_uppcase_a ) - ascii_lowercase_a;
		}
		if ( hex_val < 0x0AU )
		{
			hex_val = 0xFFU;
		}
	}

	if ( hex_val > 0x0FU )
	{
		hex_val = 0xFFU;
	}

	return ( static_cast<uint8_t>( hex_val ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Convert_String_HEX2ASCII( uint8_t* src_data, uint16_t& length,
										   uint8_t* dest_data, bool high_byte_first ) const
{	// Work from the backend then you don't have to worry about running into another buffer.  Sweet!
	uint8_t* dest_ptr;
	uint8_t* src_ptr;
	uint_fast16_t temp_length = length;

	dest_ptr = dest_data + ( ( length - 1U ) * 2U );
	src_ptr = src_data + ( length - 1U );
	while ( temp_length > 0U )
	{
		temp_length--;
		Convert_HEX2ASCII( *src_ptr, dest_ptr, high_byte_first );
		dest_ptr -= 2U;
		src_ptr--;
	}

	length = static_cast<uint16_t>( length * 2U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Convert_HEX2ASCII( uint8_t ascii_data, uint8_t* dest_string,
									bool high_byte_first ) const
{
	uint8_t ascii_zero = '0';
	uint8_t ascii_one = '1';
	uint8_t ascii_two = '2';
	uint8_t ascii_three = '3';
	uint8_t ascii_four = '4';
	uint8_t ascii_five = '5';
	uint8_t ascii_six = '6';
	uint8_t ascii_seven = '7';
	uint8_t ascii_eight = '8';
	uint8_t ascii_nine = '9';
	uint8_t ascii_uppcase_a = 'A';
	uint8_t ascii_uppcase_b = 'B';
	uint8_t ascii_uppcase_c = 'C';
	uint8_t ascii_uppcase_d = 'D';
	uint8_t ascii_uppcase_e = 'E';
	uint8_t ascii_uppcase_f = 'F';


	static flash uint8_t MODBUS_ASCII_CONV[16] =
	{ ascii_zero, ascii_one, ascii_two, ascii_three, ascii_four, ascii_five,
	  ascii_six, ascii_seven, ascii_eight, ascii_nine, ascii_uppcase_a,
	  ascii_uppcase_b, ascii_uppcase_c, ascii_uppcase_d, ascii_uppcase_e, ascii_uppcase_f };

	if ( high_byte_first == true )
	{
		dest_string[0] = MODBUS_ASCII_CONV[ascii_data >> 4U];
		dest_string[1] = MODBUS_ASCII_CONV[ascii_data & 0x0FU];
	}
	else
	{
		dest_string[0] = MODBUS_ASCII_CONV[ascii_data & 0x0FU];
		dest_string[1] = MODBUS_ASCII_CONV[ascii_data >> 4U];
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Net::Msg_For_Me( uint8_t address ) const
{
	bool address_mine = false;

	if ( ( address == m_address ) ||
		 ( address == MODBUS_BROADCAST_ADDRESS ) ||
		 ( m_tx_mode == MODBUS_TCP_TX_MODE ) )
	{
		address_mine = true;
	}

	return ( address_mine );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Net::Ready_For_Reset( BF::System_Reset::reset_select_t reset_req )
{
	bool device_idle;

	device_idle = m_mb_serial->Idle();

	return ( device_idle );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'reset_req' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Net::Debug_Dump( uint8_t* data, uint8_t length )
{
	static uint8_t tx_counter = 0U;
	MB_FRAME_STRUCT tx_msg_struct;

	tx_msg_struct.data_length = 0U;

	// If we are in master mode and have no buffer allocated.
	if ( m_tx_buff_size == 0U )
	{
		data[DEBUG_FRAME_COUNT_IND] = tx_counter;
		tx_msg_struct.data = data;
		tx_msg_struct.data_length = length;
	}
	else if ( ( length + DEBUG_FRAME_DATA_IND ) <= m_tx_buff_size )
	{
		m_tx_buff[DEBUG_FRAME_COUNT_IND] = tx_counter;
		BF_Lib::Copy_String( &m_tx_buff[DEBUG_FRAME_DATA_IND], data, length );

		tx_msg_struct.data = m_tx_buff;
		tx_msg_struct.data_length = length + DEBUG_FRAME_DATA_IND;
	}
	else
	{	/* do nothing */
	}

	if ( tx_msg_struct.data_length != 0U )
	{
		m_mb_serial->Send_Frame( &tx_msg_struct );

		tx_counter++;
	}
	else
	{	/* do nothing */
	}
}
