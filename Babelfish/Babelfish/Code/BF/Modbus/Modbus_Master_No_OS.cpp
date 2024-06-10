/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Modbus_Master_No_OS.h"
#include "Modbus_RTUFrame_Defines.h"
#include "Ram.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Master_No_OS::Modbus_Master_No_OS( Modbus_Serial* modbus_serial_ctrl,
										  response_callback_t* response_callback,
										  response_callback_param_t response_callback_param,
										  BF_Lib::Timers::TIMERS_TIME_TD response_timeout,
										  uint16_t tx_buffer_size, bool coil_byte_swapping_enable ) :
	m_response_timer( reinterpret_cast<BF_Lib::Timers*>( nullptr ) ),
	m_response_timeout( response_timeout ),
	m_mb_net( reinterpret_cast<Modbus_Net*>( nullptr ) ),
	m_resp_check_len( 0U ),
	m_response_callback_param( response_callback_param ),
	m_response_callback( response_callback ),
	m_sporadic_response_callback_param( reinterpret_cast<response_callback_param_t>( nullptr ) ),
	m_sporadic_response_callback( reinterpret_cast<response_callback_t*>( nullptr ) ),
	m_tx_buff_size( tx_buffer_size ),
	m_passed_rx_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_passed_rx_buff_len( 0U ),
	m_message_sent( NO_MSG_PENDING ),
	m_resp_check{ 0U },
	m_resp_check_mask{ 0U },
	m_coil_byte_swapping_enable( true )
{
	/**
	 * The argument "tx_buffer_size" should be equal to MODBUS_DEFAULT_BUFFER_SIZE.
	 * Otherwise, define MODBUS_NET_BUFFER_SIZE to tx_buffer_size value.
	 * Example: For buffer size 513
	 * Add "-D MODBUS_NET_BUFFER_SIZE=513" in common_define_config.xcl
	 */
	BF_ASSERT( tx_buffer_size == MODBUS_DEFAULT_BUFFER_SIZE );
	m_mb_net = new Modbus_Net( modbus_serial_ctrl,
							   &Response_Handler_Static, this );

	m_response_callback_param = response_callback_param;
	m_response_callback = response_callback;

	m_response_timer = new BF_Lib::Timers( &Response_Timeout_Static, this,
										   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
										   "Modbus Master No-OS" );
	m_tx_buff_size = tx_buffer_size;

	m_response_timeout = response_timeout;
	m_message_sent = NO_MSG_PENDING;

	m_coil_byte_swapping_enable = coil_byte_swapping_enable;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Master_No_OS::~Modbus_Master_No_OS( void )
{
	delete m_mb_net;
	delete m_response_timer;
	m_response_callback_param = nullptr;			// m_response_callback_param is a void * so cannot be deleted using
													// delete also no memory is assigned to it so making to point to
													// nullptr to avoid dangling pointer
	// delete m_response_callback_param;
	delete m_passed_rx_buff;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::Read_Coil( uint8_t address, uint16_t coil, uint16_t coil_count,
									 uint8_t* get_data, uint8_t attribute )
{
	BF_Lib::SPLIT_UINT16 bit_split;
	BF_Lib::SPLIT_UINT16 bit_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	bool success = false;
	uint16_t byte_count = 0U;

	if ( coil_count > 0U )
	{
		m_tx_buff[MB_MBIT_GET_ADDRESS_IND] = address;

		if ( attribute == MB_ATTRIB_NORMAL_ACCESS )
		{
			m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND] = MB_READ_COILS_FUNC_CODE;
		}
		else
		{
			m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND] = MB_READ_ATTRIB_BIT_ACCESS_FUNC_CODE;
		}
		/* Actual coil count to be read is sent in transmit frame */
		bit_split.u16 = coil;
		m_tx_buff[MB_MBIT_GET_BIT_NUM_HIGH_IND] = bit_split.u8[1];
		m_tx_buff[MB_MBIT_GET_BIT_NUM_LOW_IND] = bit_split.u8[0];

		bit_cnt_split.u16 = coil_count;
		m_tx_buff[MB_MBIT_GET_NUM_BITS_HIGH_IND] = attribute;		// temp_split.u8[1];
		m_tx_buff[MB_MBIT_GET_NUM_BITS_LOW_IND] = bit_cnt_split.u8[0];

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MBIT_GET_HEADER_SIZE;

		m_resp_check[MB_MBIT_GET_RESP_ADDRESS_IND] = address;
		m_resp_check_mask[MB_MBIT_GET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MBIT_GET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MBIT_GET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK;
		/* Actual coil count is converted to bytes for reception */
		if ( ( coil_count % 8 ) > 0 )
		{
			bit_cnt_split.u16 = ( coil_count ) >> DIV_BY_8;
			bit_cnt_split.u16++;
		}
		else
		{
			bit_cnt_split.u16 = coil_count >> DIV_BY_8;
		}
		m_resp_check[MB_MBIT_GET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>( bit_cnt_split.u8[0] );
		m_resp_check_mask[MB_MBIT_GET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_len = MB_MBIT_GET_RESP_HEADER_SIZE;

		byte_count = bit_cnt_split.u8[0];

		m_passed_rx_buff = get_data;
		m_passed_rx_buff_len = byte_count;

		m_message_sent = READ_COIL;
		m_mb_net->Send_Frame( &tx_struct );

		if ( m_response_timeout != RESPONSE_TIMEOUT_DISABLE_VAL )
		{
			m_response_timer->Start( m_response_timeout, false );
		}

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::Read_Discrete_Input( uint8_t address, uint16_t discrete_input, uint16_t discrete_input_count,
											   uint8_t* get_data, uint8_t attribute )
{
	BF_Lib::SPLIT_UINT16 bit_split;
	BF_Lib::SPLIT_UINT16 bit_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	bool success = false;
	uint16_t byte_count = 0U;

	if ( discrete_input_count > 0U )
	{
		m_tx_buff[MB_MBIT_GET_ADDRESS_IND] = address;

		if ( attribute == MB_ATTRIB_NORMAL_ACCESS )
		{
			m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND] = MB_READ_DISCRETE_INPUTS_FUNC_CODE;
		}
		else
		{
			m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND] = MB_READ_ATTRIB_BIT_ACCESS_FUNC_CODE;
		}

		bit_split.u16 = discrete_input;
		m_tx_buff[MB_MBIT_GET_BIT_NUM_HIGH_IND] = bit_split.u8[1];
		m_tx_buff[MB_MBIT_GET_BIT_NUM_LOW_IND] = bit_split.u8[0];
		/* Actual discrete input count to be read is sent in transmit frame */
		bit_cnt_split.u16 = discrete_input_count;
		m_tx_buff[MB_MBIT_GET_NUM_BITS_HIGH_IND] = attribute;		// temp_split.u8[1];
		m_tx_buff[MB_MBIT_GET_NUM_BITS_LOW_IND] = bit_cnt_split.u8[0];

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MBIT_GET_HEADER_SIZE;

		m_resp_check[MB_MBIT_GET_RESP_ADDRESS_IND] = address;
		m_resp_check_mask[MB_MBIT_GET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MBIT_GET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MBIT_GET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MBIT_GET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK;
		/* Actual discrete input count is converted to bytes for reception */
		if ( ( discrete_input_count % 8 ) > 0 )
		{
			bit_cnt_split.u16 = ( discrete_input_count ) >> DIV_BY_8;
			bit_cnt_split.u16++;
		}
		else
		{
			bit_cnt_split.u16 = discrete_input_count >> DIV_BY_8;
		}
		m_resp_check[MB_MBIT_GET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>( bit_cnt_split.u8[0] );
		m_resp_check_mask[MB_MBIT_GET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_len = MB_MBIT_GET_RESP_HEADER_SIZE;

		byte_count = bit_cnt_split.u8[0];

		m_passed_rx_buff = get_data;
		m_passed_rx_buff_len = byte_count;

		m_message_sent = READ_DISCRETE_INPUT;
		m_mb_net->Send_Frame( &tx_struct );

		if ( m_response_timeout != RESPONSE_TIMEOUT_DISABLE_VAL )
		{
			m_response_timer->Start( m_response_timeout, false );
		}

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::Get_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
								   uint8_t* get_data, uint8_t attribute )
{
	BF_Lib::SPLIT_UINT16 reg_split;
	BF_Lib::SPLIT_UINT16 reg_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	bool success = false;

	if ( byte_count > 0U )
	{
		m_tx_buff[MB_MREG_GET_ADDRESS_IND] = address;

		if ( attribute == MB_ATTRIB_NORMAL_ACCESS )
		{
			m_tx_buff[MB_MREG_GET_FUNC_CODE_IND] = MB_READ_HOLDING_REG_FUNC_CODE;
		}
		else
		{
			m_tx_buff[MB_MREG_GET_FUNC_CODE_IND] = MB_READ_ATTRIB_ACCESS_FUNC_CODE;
		}

		reg_split.u16 = reg;
		m_tx_buff[MB_MREG_GET_REG_NUM_HIGH_IND] = reg_split.u8[1];
		m_tx_buff[MB_MREG_GET_REG_NUM_LOW_IND] = reg_split.u8[0];

		reg_cnt_split.u16 = ( byte_count + 1U ) >> DIV_BY_2;
		m_tx_buff[MB_MREG_GET_NUM_REGS_HIGH_IND] = attribute;		// temp_split.u8[1];
		m_tx_buff[MB_MREG_GET_NUM_REGS_LOW_IND] = reg_cnt_split.u8[0];

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MREG_GET_HEADER_SIZE;

		m_resp_check[MB_MREG_GET_RESP_ADDRESS_IND] = m_tx_buff[MB_MREG_GET_ADDRESS_IND];
		m_resp_check_mask[MB_MREG_GET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_GET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MREG_GET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_GET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_GET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>
			( reg_cnt_split.u8[0]
			  << MULT_BY_2 );
		m_resp_check_mask[MB_MREG_GET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;

		m_resp_check_len = MB_MREG_GET_RESP_HEADER_SIZE;

		m_passed_rx_buff = get_data;
		m_passed_rx_buff_len = byte_count;

		m_message_sent = GET_REG;
		m_mb_net->Send_Frame( &tx_struct );

		if ( m_response_timeout != RESPONSE_TIMEOUT_DISABLE_VAL )
		{
			m_response_timer->Start( m_response_timeout, false );
		}

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::Set_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
								   uint8_t* set_data, uint8_t attribute )
{
	BF_Lib::SPLIT_UINT16 reg_split;
	MB_FRAME_STRUCT tx_struct;
	BF_Lib::SPLIT_UINT16 reg_count;
	bool success = false;

	if ( byte_count > 0U )
	{
		m_tx_buff[MB_MREG_SET_ADDRESS_IND] = address;

		if ( attribute == MB_ATTRIB_NORMAL_ACCESS )
		{
			m_tx_buff[MB_MREG_SET_FUNC_CODE_IND] = MB_WRITE_MULTIPLE_REG_FUNC_CODE;
		}
		else
		{
			m_tx_buff[MB_MREG_SET_FUNC_CODE_IND] = MB_WRITE_ATTRIB_ACCESS_FUNC_CODE;
		}

		reg_split.u16 = reg;
		m_tx_buff[MB_MREG_SET_REG_NUM_HIGH_IND] = reg_split.u8[1];
		m_tx_buff[MB_MREG_SET_REG_NUM_LOW_IND] = reg_split.u8[0];

		reg_count.u16 = ( byte_count + 1U ) >> DIV_BY_2;
		m_tx_buff[MB_MREG_SET_NUM_REGS_HIGH_IND] = attribute;		// reg_count.u8[1];
		m_tx_buff[MB_MREG_SET_NUM_REGS_LOW_IND] = reg_count.u8[0];
		m_tx_buff[MB_MREG_SET_NUM_BYTES_IND] = static_cast<uint8_t>( reg_count.u16 * 2U );

		Create_MB_Data( set_data, &m_tx_buff[MB_MREG_SET_REG_VALS_IND], byte_count );

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MREG_SET_HEADER_SIZE +
			static_cast<uint16_t>( reg_count.u16 * 2U );

		m_resp_check[MB_MREG_SET_RESP_ADDRESS_IND] = m_tx_buff[MB_MREG_SET_ADDRESS_IND];
		m_resp_check_mask[MB_MREG_SET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_SET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MREG_SET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_SET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK >> 1U;

		m_resp_check[MB_MREG_SET_RESP_REG_NUM_HIGH_IND] = reg_split.u8[1];
		m_resp_check_mask[MB_MREG_SET_RESP_REG_NUM_HIGH_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_SET_RESP_REG_NUM_LOW_IND] = reg_split.u8[0];
		m_resp_check_mask[MB_MREG_SET_RESP_REG_NUM_LOW_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_SET_RESP_NUM_REGS_HIGH_IND] = reg_count.u8[1];
		m_resp_check_mask[MB_MREG_SET_RESP_NUM_REGS_HIGH_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_SET_RESP_NUM_REGS_LOW_IND] = reg_count.u8[0];
		m_resp_check_mask[MB_MREG_SET_RESP_NUM_REGS_LOW_IND] = MB_RESP_MASK_CHECK;

		m_resp_check_len = MB_MREG_SET_RESP_HEADER_SIZE;

		m_passed_rx_buff = reinterpret_cast<uint8_t*>( nullptr );
		m_passed_rx_buff_len = 0U;

		m_message_sent = SET_REG;
		m_mb_net->Send_Frame( &tx_struct );

		m_response_timer->Start( m_response_timeout, false );

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::GetSet_Reg( uint8_t address,
									  uint16_t get_reg, uint16_t get_byte_count,
									  uint8_t* get_data,
									  uint16_t set_reg, uint16_t set_byte_count,
									  uint8_t* set_data,
									  uint8_t get_attribute, uint8_t set_attribute )
{
	BF_Lib::SPLIT_UINT16 temp_split;
	MB_FRAME_STRUCT tx_struct;
	BF_Lib::SPLIT_UINT16 reg_get_count;
	BF_Lib::SPLIT_UINT16 reg_set_count;
	bool success = false;

	if ( ( get_byte_count > 0U ) && ( set_byte_count > 0U ) )
	{
		m_tx_buff[MB_MREG_GETSET_ADDRESS_IND] = address;

		if ( ( get_attribute == MB_ATTRIB_NORMAL_ACCESS ) &&
			 ( set_attribute == MB_ATTRIB_NORMAL_ACCESS ) )
		{
			m_tx_buff[MB_MREG_GETSET_FUNC_CODE_IND] = MB_READ_WRITE_MULTIPLE_REG_FUNC_CODE;
		}
		else
		{
			m_tx_buff[MB_MREG_GETSET_FUNC_CODE_IND] = MB_READ_WRITE_ATTRIB_ACCESS_FUNC_CODE;
		}

		temp_split.u16 = get_reg;
		m_tx_buff[MB_MREG_GETSET_GET_REG_NUM_HIGH_IND] = temp_split.u8[1];
		m_tx_buff[MB_MREG_GETSET_GET_REG_NUM_LOW_IND] = temp_split.u8[0];

		reg_get_count.u16 = ( get_byte_count + 1U ) >> DIV_BY_2;
		m_tx_buff[MB_MREG_GETSET_NUM_REGS_GET_HIGH_IND] = get_attribute;		// reg_count.u8[1];
		m_tx_buff[MB_MREG_GETSET_NUM_REGS_GET_LOW_IND] = reg_get_count.u8[0];

		temp_split.u16 = set_reg;
		m_tx_buff[MB_MREG_GETSET_SET_REG_NUM_HIGH_IND] = temp_split.u8[1];
		m_tx_buff[MB_MREG_GETSET_SET_REG_NUM_LOW_IND] = temp_split.u8[0];

		reg_set_count.u16 = ( set_byte_count + 1U ) >> DIV_BY_2;
		m_tx_buff[MB_MREG_GETSET_NUM_REGS_SET_HIGH_IND] = set_attribute;		// reg_count.u8[1];
		m_tx_buff[MB_MREG_GETSET_NUM_REGS_SET_LOW_IND] = reg_set_count.u8[0];

		m_tx_buff[MB_MREG_GETSET_NUM_BYTES_SET_LOW_IND] =
			static_cast<uint8_t>( reg_set_count.u8[0] << MULT_BY_2 );

		Create_MB_Data( set_data, &m_tx_buff[MB_MREG_GETSET_DATA_IND], set_byte_count );

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MREG_GETSET_HEADER_SIZE
			+ static_cast<uint16_t>( reg_set_count.u16 * 2U );

		m_resp_check[MB_MREG_GETSET_RESP_ADDRESS_IND] = m_tx_buff[MB_MREG_GETSET_ADDRESS_IND];
		m_resp_check_mask[MB_MREG_GETSET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_GETSET_RESP_FUNC_CODE_IND] =
			m_tx_buff[MB_MREG_GETSET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_GETSET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK >> 1;

		m_resp_check[MB_MREG_GETSET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>
			( reg_get_count.u8[0]
			  << MULT_BY_2 );
		m_resp_check_mask[MB_MREG_GETSET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;

		m_resp_check_len = MB_MREG_GETSET_RESP_HEADER_SIZE;

		m_passed_rx_buff = get_data;
		m_passed_rx_buff_len = get_byte_count;

		m_message_sent = GET_SET_REG;
		m_mb_net->Send_Frame( &tx_struct );

		m_response_timer->Start( m_response_timeout, false );

		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::User_Func_Code( uint8_t address, uint8_t user_func,
										  uint8_t* tx_data, uint16_t tx_data_len,
										  uint8_t* rx_data, uint16_t rx_data_len )
{
	MB_FRAME_STRUCT tx_struct;
	bool success = true;

	m_tx_buff[MB_RTU_FRAME_ADDRESS_IND] = address;
	m_tx_buff[MB_RTU_FRAME_FUNC_CODE_IND] = user_func;

	if ( ( tx_data != nullptr ) && ( tx_data_len > 0U ) )
	{
		BF_Lib::Copy_String( &m_tx_buff[MB_RTU_FRAME_DATA_IND], tx_data, tx_data_len );
	}

	tx_struct.data = m_tx_buff;
	tx_struct.data_length = MB_RTU_FRAME_DATA_IND + tx_data_len;

	m_resp_check[MB_RTU_FRAME_ADDRESS_IND] = m_tx_buff[MB_RTU_FRAME_ADDRESS_IND];
	m_resp_check_mask[MB_RTU_FRAME_ADDRESS_IND] = MB_RESP_MASK_CHECK;

	m_resp_check[MB_RTU_FRAME_FUNC_CODE_IND] = m_tx_buff[MB_RTU_FRAME_FUNC_CODE_IND];
	m_resp_check_mask[MB_RTU_FRAME_FUNC_CODE_IND] = MB_RESP_MASK_CHECK >> 1;
	m_resp_check_len = MB_RTU_FRAME_FUNC_CODE_HEADER_SIZE;

	m_passed_rx_buff = rx_data;
	m_passed_rx_buff_len = rx_data_len;

	m_message_sent = USER_FUNC;
	m_mb_net->Send_Frame( &tx_struct );

	m_response_timer->Start( m_response_timeout, false );

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Response_Timeout( void )
{
	( *m_response_callback )( m_response_callback_param,
							  reinterpret_cast<uint8_t*>( nullptr ), 0U,
							  MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Response_Handler( MB_FRAME_STRUCT* rx_struct )
{
	uint8_t return_status = MB_GENERAL_RECEIVE_ERROR_CODE;
	uint16_t copy_length = 0U;
	bool response_valid_test;

	BF_ASSERT( rx_struct != nullptr );
	response_valid_test = Response_Check( rx_struct );

	if ( ( m_message_sent != NO_MSG_PENDING ) && response_valid_test )
	{
		m_response_timer->Stop();

		switch ( m_message_sent )
		{
			case GET_REG:
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_GET_RESP_FUNC_CODE_IND] ) )
				{
					copy_length = rx_struct->data_length - MB_MREG_GET_RESP_HEADER_SIZE;
					if ( ( copy_length >> DIV_BY_2 ) ==
						 ( ( m_passed_rx_buff_len + 1U ) >> DIV_BY_2 ) )
					{
						Extract_MB_Data( &rx_struct->data[MB_MREG_GET_RESP_DATA],
										 m_passed_rx_buff, m_passed_rx_buff_len );
						return_status = MB_NO_ERROR_CODE;
					}
					else
					{
						return_status = MB_DEST_STRING_SIZE_WRONG_ERROR_CODE;
					}
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
				break;

			case SET_REG:
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_SET_RESP_FUNC_CODE_IND] ) )
				{
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
				break;

			case GET_SET_REG:
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_GETSET_RESP_FUNC_CODE_IND] ) )
				{
					copy_length = rx_struct->data_length - MB_MREG_GETSET_RESP_HEADER_SIZE;
					if ( ( copy_length >> DIV_BY_2 ) ==
						 ( ( m_passed_rx_buff_len + 1U ) >> DIV_BY_2 ) )
					{
						Extract_MB_Data( &rx_struct->data[MB_MREG_GETSET_RESP_DATA],
										 m_passed_rx_buff, m_passed_rx_buff_len );
						return_status = MB_NO_ERROR_CODE;
					}
					else
					{
						return_status = MB_DEST_STRING_SIZE_WRONG_ERROR_CODE;
					}
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
				break;

			case USER_FUNC:
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_RTU_FRAME_FUNC_CODE_IND] ) )
				{
					copy_length = rx_struct->data_length - MB_RTU_FRAME_DATA_IND;
					if ( ( m_passed_rx_buff_len == 0U ) ||
						 ( m_passed_rx_buff == nullptr ) )
					{
						m_passed_rx_buff_len = 0U;
						return_status = MB_NO_ERROR_CODE;
					}
					else if ( copy_length <= m_passed_rx_buff_len )
					{
						m_passed_rx_buff_len = copy_length;

						BF_Lib::Copy_String( m_passed_rx_buff,
											 &rx_struct->data[MB_RTU_FRAME_DATA_IND],
											 m_passed_rx_buff_len );

						return_status = MB_NO_ERROR_CODE;
					}
					else
					{
						return_status = MB_DEST_STRING_SIZE_WRONG_ERROR_CODE;
					}
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
				break;

			/* Following case were added for Function code 1 & 2 */
			case READ_COIL:
			case READ_DISCRETE_INPUT:
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MBIT_GET_RESP_FUNC_CODE_IND] ) )
				{
					copy_length = rx_struct->data_length - MB_MBIT_GET_RESP_HEADER_SIZE;
					if ( ( copy_length >> DIV_BY_8 ) ==
						 ( ( m_passed_rx_buff_len ) >> DIV_BY_8 ) )
					{
						if ( m_coil_byte_swapping_enable == true )
						{
							Extract_MB_Data( &rx_struct->data[MB_MBIT_GET_RESP_DATA], m_passed_rx_buff,
											 m_passed_rx_buff_len );
						}
						else
						{
							BF_Lib::Copy_String( m_passed_rx_buff, &rx_struct->data[MB_MBIT_GET_RESP_DATA],
												 m_passed_rx_buff_len );
						}
						return_status = MB_NO_ERROR_CODE;
					}
					else
					{
						return_status = MB_DEST_STRING_SIZE_WRONG_ERROR_CODE;
					}
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
				break;

			case NO_MSG_PENDING:
			default:
				break;
		}

		( *m_response_callback )( m_response_callback_param,
								  m_passed_rx_buff, m_passed_rx_buff_len, return_status );
	}
	else
	{
		if ( response_valid_test == false )
		{
			if ( m_sporadic_response_callback !=
				 reinterpret_cast<response_callback_t*>( nullptr ) )
			{
				return_status = MB_NO_ERROR_CODE;
				( *m_sporadic_response_callback )( m_sporadic_response_callback_param,
												   rx_struct->data, rx_struct->data_length,
												   return_status );
			}
		}
		// BF_ASSERT(false);
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master_No_OS::Response_Check( MB_FRAME_STRUCT* rx_struct )
{
	bool resp_valid = false;
	uint8_t* rx_data = rx_struct->data;
	uint8_t* rx_check_data = m_resp_check;
	uint8_t* rx_check_mask = m_resp_check_mask;
	uint_fast16_t rx_cntr = m_resp_check_len;

	if ( rx_struct->data_length >= MB_RTU_FRAME_DATA_IND )
	{
		if ( !BF_Lib::Bit::Test( rx_data[MB_RTU_FRAME_FUNC_CODE_IND], MB_FUNC_ERROR_RESP_BIT ) )
		{
			if ( rx_struct->data_length >= m_resp_check_len )
			{
				resp_valid = true;
				while ( ( resp_valid == true ) && ( rx_cntr > 0U ) )
				{
					if ( *rx_check_data != ( *rx_data & *rx_check_mask ) )
					{
						resp_valid = false;
					}
					rx_check_data++;
					rx_data++;
					rx_check_mask++;
					rx_cntr--;
				}
			}
		}
		else if ( rx_struct->data_length >= MB_RTU_FRAME_EXCEPTION_HEADER_SIZE )
		{
			resp_valid = true;
		}
		else
		{	/* do nothing */
		}
	}
	else
	{	/* do nothing */
	}

	return ( resp_valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Create_MB_Data( uint8_t* src_data, uint8_t* dest_data,
										  uint16_t length ) const
{
	uint_fast16_t counter = 0U;
	uint8_t temp_swap = 0U;

	counter = length;
	if ( counter > 0U )
	{
		if ( Is_Number_Odd( counter ) )
		{
			dest_data[counter] = src_data[counter - 1U];
			dest_data[counter - 1U] = 0U;
			counter--;
			length++;
		}

		while ( counter > 0U )
		{
			temp_swap = src_data[counter - 1U];
			dest_data[counter - 1U] = src_data[counter - 2U];
			dest_data[counter - 2U] = temp_swap;
			counter -= 2U;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Extract_MB_Data( uint8_t* src_data, uint8_t* dest_data,
										   uint16_t length ) const
{
	uint_fast16_t counter = 0U;
	uint8_t temp_swap = 0U;

	counter = length;
	if ( counter > 0U )
	{
		if ( Is_Number_Odd( counter ) )
		{
			/* In case if length of data is odd number then last byte is copied at last index of destination before
			   swapping other elements in Little Endian format as Modbus response is in big endian format*/
			dest_data[counter - 1U] = src_data[counter - 1U];
			counter--;
		}

		while ( counter > 0U )
		{
			/* Here data is arranged as per little endian format */
			temp_swap = src_data[counter - 1U];
			dest_data[counter - 1U] = src_data[counter - 2U];
			dest_data[counter - 2U] = temp_swap;
			counter -= 2U;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Attach_Callback( response_callback_t response_callback,
										   response_callback_param_t response_callback_param )
{
	m_response_callback_param = response_callback_param;
	m_response_callback = response_callback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Attach_Sporadic_Response_Callback(
	response_callback_t response_callback,
	response_callback_param_t response_callback_param )
{
	m_sporadic_response_callback_param = response_callback_param;
	m_sporadic_response_callback = response_callback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master_No_OS::Set_Timeout( BF_Lib::Timers::TIMERS_TIME_TD response_timeout )
{
	m_response_timeout = response_timeout;
}

}	/* end namespace BF_Mbus for this module */
