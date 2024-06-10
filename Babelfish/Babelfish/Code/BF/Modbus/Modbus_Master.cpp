/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_RTUFrame_Defines.h"
#include "Modbus_Master.h"
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
Modbus_Master::Modbus_Master( Modbus_Serial* modbus_serial_ctrl,
							  OS_TICK_TD response_timeout, uint16_t tx_buffer_size, bool coil_byte_swapping_enable ) :
	m_mb_net( new Modbus_Net( modbus_serial_ctrl,
							  &Response_Handler_Static, this ) ),
	m_rx_queue( new OS_Queue( static_cast<BF_Lib::MBASE>( 1 ) ) ),
	m_busy_sph( new OS_Semaphore( true ) ),
	m_resp_pending( false ),
	m_resp_check_len( 0U ),
	m_tx_buff_size( tx_buffer_size ),
	m_response_timeout( response_timeout ),
	m_resp_check{ 0U },
	m_resp_check_mask{ 0U },
	m_resp_check_buff{ 0U },
	m_resp_mask_buff{ 0U },
	m_coil_byte_swapping_enable( true )
{
	/**
	 * The argument "tx_buffer_size" should be equal to
	 * Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE. Otherwise,
	 * define MODBUS_NET_BUFFER_SIZE to tx_buffer_size value.
	 * Example: For buffer size 513
	 * Add "-D MODBUS_NET_BUFFER_SIZE=513" in common_define_config.xcl
	 */
	BF_ASSERT( tx_buffer_size == Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE );

	m_coil_byte_swapping_enable = coil_byte_swapping_enable;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Master::~Modbus_Master( void )
{
	delete m_mb_net;
	delete m_rx_queue;
	delete m_busy_sph;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Master::Read_Coil( uint8_t address, uint16_t coil, uint16_t coil_count,
								  uint8_t* data, uint8_t attribute )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;

	BF_Lib::SPLIT_UINT16 bit_split;
	BF_Lib::SPLIT_UINT16 bit_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;
	uint16_t byte_count = 0U;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
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
		m_resp_check_len = 3U;

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0 )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MBIT_GET_RESP_FUNC_CODE_IND] ) )
				{
					/* Data bytes receveied from Slave is extracted from response */
					byte_count = rx_struct->data[MB_MBIT_GET_RESP_BYTE_NUM_IND];
					if ( m_coil_byte_swapping_enable == true )
					{
						Extract_MB_Data( &rx_struct->data[MB_MBIT_GET_RESP_DATA], data, byte_count );
					}
					else
					{
						BF_Lib::Copy_String( data, &rx_struct->data[MB_MBIT_GET_RESP_DATA], byte_count );
					}
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;
		m_busy_sph->Post();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Master::Read_Discrete_Input( uint8_t address, uint16_t discrete_input, uint16_t discrete_input_count,
											uint8_t* data, uint8_t attribute )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;

	BF_Lib::SPLIT_UINT16 bit_split;
	BF_Lib::SPLIT_UINT16 bit_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;
	uint16_t byte_count = 0U;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
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
		m_resp_check_len = 3U;

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0 )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MBIT_GET_RESP_FUNC_CODE_IND] ) )
				{
					/* Data bytes receveied from Slave is extracted from response */
					byte_count = rx_struct->data[MB_MBIT_GET_RESP_BYTE_NUM_IND];
					if ( m_coil_byte_swapping_enable == true )
					{
						Extract_MB_Data( &rx_struct->data[MB_MBIT_GET_RESP_DATA], data, byte_count );
					}
					else
					{
						BF_Lib::Copy_String( data, &rx_struct->data[MB_MBIT_GET_RESP_DATA], byte_count );
					}
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;

		m_busy_sph->Post();
	}

	return ( return_status );
}

uint8_t Modbus_Master::Get_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
								uint8_t* data, uint8_t attribute )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;

	BF_Lib::SPLIT_UINT16 reg_split;
	BF_Lib::SPLIT_UINT16 reg_cnt_split;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
	{
		m_tx_buff[MB_MREG_GET_RESP_ADDRESS_IND] = address;

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

		m_resp_check[MB_MREG_GET_RESP_ADDRESS_IND] = address;
		m_resp_check_mask[MB_MREG_GET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MREG_GET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MREG_GET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_GET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MREG_GET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>( reg_cnt_split.u8[0]
																			<< MULT_BY_2 );
		m_resp_check_mask[MB_MREG_GET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_len = 3U;

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0 )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_GET_RESP_FUNC_CODE_IND] ) )
				{
					Extract_MB_Data( &rx_struct->data[MB_MREG_GET_RESP_DATA], data, byte_count );
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;

		m_busy_sph->Post();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Master::Set_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
								uint8_t* data, uint8_t attribute )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;

	BF_Lib::SPLIT_UINT16 reg_split;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;
	BF_Lib::SPLIT_UINT16 reg_count;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
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
		m_tx_buff[MB_MREG_SET_NUM_REGS_HIGH_IND] = attribute;	// reg_count.u8[1];
		m_tx_buff[MB_MREG_SET_NUM_REGS_LOW_IND] = reg_count.u8[0];
		m_tx_buff[MB_MREG_SET_NUM_BYTES_IND] = static_cast<uint8_t>( reg_count.u16 * 2U );

		Create_MB_Data( data, &m_tx_buff[MB_MREG_SET_REG_VALS_IND], byte_count );

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_MREG_SET_HEADER_SIZE
			+ static_cast<uint16_t>( reg_count.u16 * 2U );

		m_resp_check[MB_MREG_SET_RESP_ADDRESS_IND] = address;
		m_resp_check_mask[MB_MREG_SET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MREG_SET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MREG_SET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_SET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK >> 1;
		m_resp_check[MB_MREG_SET_RESP_REG_NUM_HIGH_IND] = reg_split.u8[1];
		m_resp_check[MB_MREG_SET_RESP_REG_NUM_LOW_IND] = reg_split.u8[0];
		m_resp_check_mask[MB_MREG_SET_RESP_REG_NUM_HIGH_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_mask[MB_MREG_SET_RESP_REG_NUM_LOW_IND] = MB_RESP_MASK_CHECK;
		m_resp_check[MB_MREG_SET_RESP_NUM_REGS_HIGH_IND] = reg_count.u8[1];
		m_resp_check[MB_MREG_SET_RESP_NUM_REGS_LOW_IND] = reg_count.u8[0];
		m_resp_check_mask[MB_MREG_SET_RESP_NUM_REGS_HIGH_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_mask[MB_MREG_SET_RESP_NUM_REGS_LOW_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_len = 4U;

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0U )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_SET_RESP_FUNC_CODE_IND] ) )
				{
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;

		m_busy_sph->Post();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Master::GetSet_Reg( uint8_t address,
								   uint16_t get_reg, uint16_t get_byte_count, uint8_t* get_data,
								   uint16_t set_reg, uint16_t set_byte_count, uint8_t* set_data,
								   uint8_t get_attribute, uint8_t set_attribute )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;

	BF_Lib::SPLIT_UINT16 temp_split;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;
	BF_Lib::SPLIT_UINT16 reg_get_count;
	BF_Lib::SPLIT_UINT16 reg_set_count;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
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

		m_resp_check[MB_MREG_GETSET_RESP_ADDRESS_IND] = address;
		m_resp_check_mask[MB_MREG_GETSET_RESP_ADDRESS_IND] = MB_RESP_MASK_CHECK;

		m_resp_check[MB_MREG_GETSET_RESP_FUNC_CODE_IND] = m_tx_buff[MB_MREG_GETSET_FUNC_CODE_IND];
		m_resp_check_mask[MB_MREG_GETSET_RESP_FUNC_CODE_IND] = MB_RESP_MASK_CHECK >> 1;

		m_resp_check[MB_MREG_GETSET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>( reg_get_count.u8[0]
																			   << MULT_BY_2 );
		m_resp_check_mask[MB_MREG_GETSET_RESP_BYTE_NUM_IND] = MB_RESP_MASK_CHECK;
		m_resp_check_len = 3U;

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0 )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_MREG_GETSET_RESP_FUNC_CODE_IND] ) )
				{
					Extract_MB_Data( &rx_struct->data[MB_MREG_GETSET_RESP_DATA], get_data,
									 get_byte_count );
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;

		m_busy_sph->Post();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Master::User_Func_Code( uint8_t address, uint8_t user_func,
									   uint8_t* tx_data, uint16_t tx_data_len,
									   uint8_t* rx_data, uint16_t* rx_data_len )
{
	uint8_t return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
	MB_FRAME_STRUCT tx_struct;
	MB_FRAME_STRUCT* rx_struct = nullptr;

	if ( m_busy_sph->Pend( MB_MASTER_CTRL_REQ_TIMEOUT ) )
	{
		m_tx_buff[MB_RTU_FRAME_ADDRESS_IND] = address;
		m_tx_buff[MB_RTU_FRAME_FUNC_CODE_IND] = user_func;

		if ( ( tx_data != nullptr ) && ( tx_data_len > 0U ) )
		{
			BF_Lib::Copy_String( &m_tx_buff[MB_RTU_FRAME_DATA_IND], tx_data, tx_data_len );
		}

		tx_struct.data = m_tx_buff;
		tx_struct.data_length = MB_RTU_FRAME_DATA_IND + tx_data_len;

		m_resp_check_len = 0U;	// Don't check the response.

		m_mb_net->Send_Frame( &tx_struct );
		m_resp_pending = true;

		if ( m_rx_queue->Receive( reinterpret_cast<OS_TASK_PARAM*>( &rx_struct ),
								  m_response_timeout ) )
		{
			if ( rx_struct != 0 )
			{
				if ( !Test_MB_Error_Resp( rx_struct->data[MB_RTU_FRAME_FUNC_CODE_IND] ) )
				{
					if ( ( rx_data_len != nullptr ) && ( rx_data != nullptr ) )
					{
						*rx_data_len = rx_struct->data_length - MB_RTU_FRAME_DATA_IND;
						BF_Lib::Copy_String( rx_data, &rx_struct->data[MB_RTU_FRAME_DATA_IND],
											 *rx_data_len );
					}
					return_status = MB_NO_ERROR_CODE;
				}
				else
				{
					return_status = rx_struct->data[MB_RTU_FRAME_EXCEPTION_IND];
				}
			}
			else
			{
				return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
			}
		}
		else
		{
			return_status = MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE;
		}

		m_resp_pending = false;

		m_busy_sph->Post();
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master::Response_Handler( MB_FRAME_STRUCT* rx_struct )
{
	if ( rx_struct != 0U )
	{
		bool temp_resp_rx_struct = Response_Check( rx_struct );

		if ( ( m_resp_pending == true ) && temp_resp_rx_struct )
		{
			m_rx_queue->Send( &rx_struct, 100U );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Modbus_Master::Response_Check( MB_FRAME_STRUCT* rx_struct )
{
	bool resp_valid = true;
	uint8_t* rx_data = rx_struct->data;
	uint8_t* rx_check_data = m_resp_check;
	uint8_t* rx_check_mask = m_resp_check_mask;
	uint_fast16_t rx_cntr = m_resp_check_len;

	if ( !BF_Lib::Bit::Test( rx_data[MB_RTU_FRAME_FUNC_CODE_IND], MB_FUNC_ERROR_RESP_BIT ) )
	{
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

	return ( resp_valid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Master::Create_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const
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
void Modbus_Master::Extract_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const
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

}	/* end namespace BF_Mbus for this module */
