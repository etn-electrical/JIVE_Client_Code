/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_Msg.h"
#include "Modbus_RTUFrame_Defines.h"
#include "Modbus_Defines.h"
#include "StdLib_MV.h"
#include "Ram.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// The rounded shift div provides a binary shift that will round.  The following results will
// occur.
// 7>>DIV_BY_8 = 1
// 8>>DIV_BY_8 = 2
// 4>>DIV_BY_8 = 1
// 0>>DIV_BY_8 = 1
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Msg::Modbus_Msg( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
						DCI_Owner* comm_dog_owner, uint16_t buffer_size,
						bool correct_out_of_range_sets ) :
	m_dci( reinterpret_cast<Modbus_DCI*>( nullptr ) ),
	m_syscmd( reinterpret_cast<Modbus_SysCmd*>( nullptr ) ),
	m_buffer_size( buffer_size ),
	m_com_watchdog( reinterpret_cast<BF_Misc::Com_Dog*>( nullptr ) ),
	m_user_defined_commands( reinterpret_cast<callback_list_item_t*>( nullptr ) )
{
	DCI_SOURCE_ID_TD temp_source_id;

	temp_source_id = DCI_SOURCE_IDS_Get();

	m_dci = new Modbus_DCI( modbus_dev_profile, temp_source_id, correct_out_of_range_sets );

	m_syscmd = new Modbus_SysCmd( temp_source_id );

	if ( comm_dog_owner != nullptr )
	{
		m_com_watchdog = new BF_Misc::Com_Dog( comm_dog_owner, temp_source_id );

	}
	else
	{
		m_com_watchdog = reinterpret_cast<BF_Misc::Com_Dog*>( nullptr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Msg::Modbus_Msg( Modbus_DCI* modbus_dci_ctrl, BF_Misc::Com_Dog* comm_dog,
						Modbus_SysCmd* mbus_syscmd, uint16_t buffer_size ) :
	m_dci( modbus_dci_ctrl ),
	m_syscmd( mbus_syscmd ),
	m_buffer_size( buffer_size ),
	m_com_watchdog( comm_dog ),
	m_user_defined_commands( reinterpret_cast<callback_list_item_t*>( nullptr ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Msg::~Modbus_Msg( void )
{
	delete m_dci;
	delete m_syscmd;
	delete m_com_watchdog;

	m_user_defined_commands = reinterpret_cast<callback_list_item_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Process_Msg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t func_code;
	uint8_t return_status;

	tx_struct->data_length = 0U;

	func_code = rx_struct->data[MB_RTU_FRAME_FUNC_CODE_IND];
	tx_struct->data[MB_RTU_FRAME_FUNC_CODE_IND] = func_code;

	return_status = Msg_Length_Good( rx_struct, func_code );

	if ( m_com_watchdog != nullptr )
	{
		m_com_watchdog->Feed_Dog();
	}

	if ( return_status == MB_NO_ERROR_CODE )
	{
		switch ( func_code )
		{
			case MB_READ_COILS_FUNC_CODE:
				return_status = Read_Coils( rx_struct, tx_struct );
				break;

			case MB_READ_DISCRETE_INPUTS_FUNC_CODE:
				return_status = Read_Discrete_Inputs( rx_struct, tx_struct );
				break;

			case MB_READ_HOLDING_REG_FUNC_CODE:
				rx_struct->data[MB_MREG_GET_NUM_REGS_HIGH_IND] = 0U;
				return_status = Read_Holding_Reg( rx_struct, tx_struct );
				break;

			case MB_READ_INPUT_REG:
				return_status = Read_Input_Reg( rx_struct, tx_struct );
				break;

			case MB_WRITE_SINGLE_COIL_FUNC_CODE:
				return_status = Write_Single_Coil( rx_struct, tx_struct );
				break;

			case MB_WRITE_SINGLE_REGISTER_FUNC_CODE:
				return_status = Write_Single_Reg( rx_struct, tx_struct );
				break;

			case MB_READ_EXCEPTION_STATUS_FUNC_CODE:
				return_status = Read_Exception_Status( rx_struct, tx_struct );
				break;

			case MB_DIAGNOSTICS_FUNC_CODE:
				return_status = Read_Diagnostics( rx_struct, tx_struct );
				break;

			case MB_WRITE_MULTIPLE_COILS_FUNC_CODE:
				return_status = Write_Multiple_Coils( rx_struct, tx_struct );
				break;

			case MB_WRITE_MULTIPLE_REG_FUNC_CODE:
				rx_struct->data[MB_MREG_SET_NUM_REGS_HIGH_IND] = 0U;
				return_status = Write_Multiple_Reg( rx_struct, tx_struct );
				break;

			case MB_WRITE_MASK_WRITE_REG_FUNC_CODE:
				// Write_Mask_Reg();
				return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
				break;

			case MB_READ_WRITE_MULTIPLE_REG_FUNC_CODE:
				rx_struct->data[MB_MREG_GETSET_NUM_REGS_GET_HIGH_IND] = 0U;
				rx_struct->data[MB_MREG_GETSET_NUM_REGS_SET_HIGH_IND] = 0U;
				return_status = Read_Write_Multiple_Reg( rx_struct, tx_struct );
				break;

			case MB_SYS_CMD_USER_FUNC_CODE:
				return_status = Sys_Cmd_User_Func( rx_struct, tx_struct );
				break;

			case MB_GET_DEVICE_IDENTITY_FUNC_CODE:
				return_status = Get_Device_Identity( rx_struct, tx_struct );
				break;

			case MB_READ_ATTRIB_ACCESS_FUNC_CODE:
				return_status = Read_Holding_Reg( rx_struct, tx_struct );
				break;

			case MB_WRITE_ATTRIB_ACCESS_FUNC_CODE:
				return_status = Write_Multiple_Reg( rx_struct, tx_struct );
				break;

			case MB_READ_WRITE_ATTRIB_ACCESS_FUNC_CODE:
				return_status = Read_Write_Multiple_Reg( rx_struct, tx_struct );
				break;

			case MB_READ_ATTRIB_BIT_ACCESS_FUNC_CODE:
				return_status = Read_Coils( rx_struct, tx_struct );		// Added for attribute retrieval.
				break;

			case MB_WRITE_ATTRIB_BIT_ACCESS_FUNC_CODE:
				return_status = Write_Multiple_Coils( rx_struct, tx_struct );	// Added for attribute retrieval.
				break;

			default:
				return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
				callback_list_item_t* temp_item = m_user_defined_commands;

				while ( temp_item != nullptr )
				{
					if ( func_code == temp_item->m_function_code )
					{
						return_status = temp_item->m_callback( temp_item->m_param, rx_struct,
															   tx_struct );
						temp_item = reinterpret_cast<callback_list_item_t*>( nullptr );
					}
					else
					{
						temp_item = temp_item->m_next_item;
					}
				}
				break;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Msg::Attach_Callback( uint16_t function_code, modbus_message_callback_t callback,
								  modbus_message_callback_param_t param )
{
	callback_list_item_t* new_callback_item = reinterpret_cast<callback_list_item_t*>(
		Ram::Allocate( sizeof( callback_list_item_t ) ) );

	new_callback_item->m_function_code = function_code;
	new_callback_item->m_callback = callback;
	new_callback_item->m_param = param;
	new_callback_item->m_next_item = reinterpret_cast<callback_list_item_t*>( nullptr );

	if ( m_user_defined_commands == nullptr )
	{
		m_user_defined_commands = new_callback_item;
	}
	else
	{
		callback_list_item_t* iterator = m_user_defined_commands;
		while ( iterator->m_next_item != nullptr )
		{
			iterator = iterator->m_next_item;
		}
		iterator->m_next_item = new_callback_item;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Coils( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_BIT_GET_STRUCT bit_get;
	MB_BIT_GET_RESP_STRUCT bit_get_resp;
	uint16_t potential_resp_len;

	bit_get.frame = rx_struct;
	bit_get_resp.frame = tx_struct;

	bit_get.start_bit.u8[0] = rx_struct->data[MB_MBIT_GET_BIT_NUM_LOW_IND];
	bit_get.start_bit.u8[1] = rx_struct->data[MB_MBIT_GET_BIT_NUM_HIGH_IND];
	bit_get.num_bits.u8[0] = rx_struct->data[MB_MBIT_GET_NUM_BITS_LOW_IND];
	bit_get.num_bits.u8[1] = 0U;	// rx_struct->data[MB_MBIT_GET_NUM_BITS_HIGH_IND];
	bit_get.attribute = rx_struct->data[MB_MBIT_GET_NUM_BITS_HIGH_IND];
	potential_resp_len = ( ( bit_get.num_bits.u16 + 7U ) >> DIV_BY_8 )
		+ MB_MBIT_GET_RESP_HEADER_SIZE;

	if ( ( bit_get.num_bits.u16 >= 1U ) &&
		 ( bit_get.num_bits.u16 <= MB_READ_COILS_MAX_BITS ) &&
		 ( potential_resp_len <= m_buffer_size ) )
	{
		bit_get_resp.data = &tx_struct->data[MB_MBIT_GET_RESP_DATA];
		bit_get_resp.byte_count = 0U;

		return_status = m_dci->Read_Bits( &bit_get, &bit_get_resp );

		tx_struct->data[MB_MBIT_GET_RESP_BYTE_NUM_IND] = bit_get_resp.byte_count;
		tx_struct->data_length = static_cast<uint16_t>( bit_get_resp.byte_count ) +
			MB_MBIT_GET_RESP_HEADER_SIZE;
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Discrete_Inputs( MB_FRAME_STRUCT* rx_struct,
										  MB_FRAME_STRUCT* tx_struct )
{
	return ( Read_Coils( rx_struct, tx_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Holding_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_REG_GET_STRUCT reg_get;
	MB_REG_GET_RESP_STRUCT reg_get_resp;
	uint16_t potential_resp_len;

	reg_get.frame = rx_struct;
	reg_get_resp.frame = tx_struct;

	reg_get.start_reg.u8[0] = rx_struct->data[MB_MREG_GET_REG_NUM_LOW_IND];
	reg_get.start_reg.u8[1] = rx_struct->data[MB_MREG_GET_REG_NUM_HIGH_IND];
	reg_get.num_regs.u8[0] = rx_struct->data[MB_MREG_GET_NUM_REGS_LOW_IND];
	reg_get.num_regs.u8[1] = 0U;	// rx_struct->data[MB_MREG_GET_NUM_REGS_HIGH_IND];
	reg_get.attribute = rx_struct->data[MB_MREG_GET_NUM_REGS_HIGH_IND];
	potential_resp_len = static_cast<uint16_t>( reg_get.num_regs.u16 << MULT_BY_2 )
		+ MB_MREG_GET_RESP_HEADER_SIZE;

	if ( ( reg_get.num_regs.u16 >= 1U ) &&
		 ( reg_get.num_regs.u16 <= MB_READ_HOLDING_REG_MAX_REGS ) &&
		 ( potential_resp_len <= m_buffer_size ) )
	{
		reg_get_resp.data = &tx_struct->data[MB_MREG_GET_RESP_DATA];
		reg_get_resp.byte_count = 0U;

		return_status = m_dci->Read_Registers( &reg_get, &reg_get_resp );

		tx_struct->data[MB_MREG_GET_RESP_BYTE_NUM_IND] = static_cast<uint8_t>( reg_get_resp
																			   .byte_count );
		tx_struct->data_length = reg_get_resp.byte_count + MB_MREG_GET_RESP_HEADER_SIZE;
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Input_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	return ( Read_Holding_Reg( rx_struct, tx_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Write_Single_Coil( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_BIT_SET_STRUCT bit_set;
	MB_BIT_SET_RESP_STRUCT bit_set_resp;

	BF_Lib::SPLIT_UINT16 temp_val;

	bit_set.frame = rx_struct;
	bit_set_resp.frame = tx_struct;

	bit_set.start_bit.u8[0] = rx_struct->data[MB_SBIT_SET_BIT_NUM_LOW_IND];
	bit_set.start_bit.u8[1] = rx_struct->data[MB_SBIT_SET_BIT_NUM_HIGH_IND];

	temp_val.u8[0] = rx_struct->data[MB_SBIT_SET_BIT_VAL_LOW_IND];
	temp_val.u8[1] = rx_struct->data[MB_SBIT_SET_BIT_VAL_HIGH_IND];

	tx_struct->data[MB_SBIT_SET_RESP_BIT_NUM_HIGH_IND] =
		rx_struct->data[MB_SBIT_SET_BIT_NUM_HIGH_IND];
	tx_struct->data[MB_SBIT_SET_RESP_BIT_NUM_LOW_IND] =
		rx_struct->data[MB_SBIT_SET_BIT_NUM_LOW_IND];
	tx_struct->data[MB_SBIT_SET_RESP_BIT_VAL_HIGH_IND] =
		rx_struct->data[MB_SBIT_SET_BIT_VAL_HIGH_IND];
	tx_struct->data[MB_SBIT_SET_RESP_BIT_VAL_LOW_IND] =
		rx_struct->data[MB_SBIT_SET_BIT_VAL_LOW_IND];

	if ( ( temp_val.u16 == SET_SINGLE_COIL_TRUE_VAL ) ||
		 ( temp_val.u16 == SET_SINGLE_COIL_FALSE_VAL ) )
	{
		bit_set.num_bits.u16 = 1U;
		bit_set.num_bytes = 1U;
		bit_set.attribute = MB_ATTRIB_NORMAL_ACCESS;

		if ( temp_val.u16 == SET_SINGLE_COIL_TRUE_VAL )
		{
			temp_val.u8[0] = 1U;
			temp_val.u8[1] = 0U;
		}
		else if ( temp_val.u16 == 0U )
		{
			temp_val.u16 = 0U;
		}
		else
		{
			return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
		}

		if ( return_status == MB_NO_ERROR_CODE )
		{
			bit_set.data = &temp_val.u8[0];

			return_status = m_dci->Write_Bits( &bit_set, &bit_set_resp );
		}
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	tx_struct->data_length = MB_SBIT_SET_RESP_HEADER_SIZE;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Write_Single_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status;
	MB_REG_SET_STRUCT reg_set;
	MB_REG_SET_RESP_STRUCT reg_set_resp;

	reg_set.frame = rx_struct;
	reg_set_resp.frame = tx_struct;

	reg_set.start_reg.u8[0] = rx_struct->data[MB_SREG_SET_REG_NUM_LOW_IND];
	reg_set.start_reg.u8[1] = rx_struct->data[MB_SREG_SET_REG_NUM_HIGH_IND];
	reg_set.data = &rx_struct->data[MB_SREG_SET_REG_VAL_HIGH_IND];

	reg_set.num_regs.u16 = 1U;
	reg_set.num_bytes = 2U;
	reg_set.attribute = MB_ATTRIB_NORMAL_ACCESS;

	tx_struct->data[MB_SREG_SET_RESP_REG_NUM_HIGH_IND] =
		rx_struct->data[MB_SREG_SET_REG_NUM_HIGH_IND];
	tx_struct->data[MB_SREG_SET_RESP_REG_NUM_LOW_IND] =
		rx_struct->data[MB_SREG_SET_REG_NUM_LOW_IND];
	tx_struct->data[MB_SREG_SET_RESP_REG_VAL_HIGH_IND] =
		rx_struct->data[MB_SREG_SET_REG_VAL_HIGH_IND];
	tx_struct->data[MB_SREG_SET_RESP_REG_VAL_LOW_IND] =
		rx_struct->data[MB_SREG_SET_REG_VAL_LOW_IND];
	tx_struct->data_length = MB_SREG_SET_RESP_HEADER_SIZE;

	return_status = m_dci->Write_Registers( &reg_set, &reg_set_resp );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Write_Multiple_Coils( MB_FRAME_STRUCT* rx_struct,
										  MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_BIT_SET_STRUCT bit_set;
	MB_BIT_SET_RESP_STRUCT bit_set_resp;
	bool temp_bt_inx_shft_dv;

	bit_set.frame = rx_struct;
	bit_set_resp.frame = tx_struct;

	bit_set.start_bit.u8[0] = rx_struct->data[MB_MBIT_SET_BIT_NUM_LOW_IND];
	bit_set.start_bit.u8[1] = rx_struct->data[MB_MBIT_SET_BIT_NUM_HIGH_IND];
	bit_set.num_bits.u8[0] = rx_struct->data[MB_MBIT_SET_NUM_BITS_LOW_IND];
	bit_set.num_bits.u8[1] = 0U;		// rx_struct->data[MB_MBIT_SET_NUM_BITS_HIGH_IND];
	bit_set.attribute = rx_struct->data[MB_MREG_SET_NUM_REGS_HIGH_IND];
	bit_set.num_bytes = rx_struct->data[MB_MBIT_SET_NUM_BYTES_IND];
	bit_set.data = &rx_struct->data[MB_MBIT_SET_BIT_VALS_IND];

	tx_struct->data[MB_MBIT_SET_RESP_BIT_NUM_HIGH_IND] =
		rx_struct->data[MB_MBIT_SET_BIT_NUM_HIGH_IND];
	tx_struct->data[MB_MBIT_SET_RESP_BIT_NUM_LOW_IND] =
		rx_struct->data[MB_MBIT_SET_BIT_NUM_LOW_IND];
	tx_struct->data[MB_MBIT_SET_RESP_NUM_BITS_HIGH_IND] =
		rx_struct->data[MB_MBIT_SET_NUM_BITS_HIGH_IND];
	tx_struct->data[MB_MBIT_SET_RESP_NUM_BITS_LOW_IND] =
		rx_struct->data[MB_MBIT_SET_NUM_BITS_LOW_IND];
	tx_struct->data_length = MB_MBIT_SET_RESP_HEADER_SIZE;

	if ( ( bit_set.num_bytes == Bit_Index_Shift_Div( bit_set.num_bits.u16, 3U ) ) )
	{
		temp_bt_inx_shft_dv = true;
	}
	else
	{
		temp_bt_inx_shft_dv = false;
	}

	if ( ( bit_set.num_bits.u16 >= 1U ) &&
		 ( bit_set.num_bits.u16 <= MB_WRITE_MULTIPLE_COILS_MAX_BITS ) &&
		 temp_bt_inx_shft_dv )
	{
		return_status = m_dci->Write_Bits( &bit_set, &bit_set_resp );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Write_Multiple_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_REG_SET_STRUCT reg_set;
	MB_REG_SET_RESP_STRUCT reg_set_resp;

	reg_set.frame = rx_struct;
	reg_set_resp.frame = tx_struct;

	reg_set.start_reg.u8[0] = rx_struct->data[MB_MREG_SET_REG_NUM_LOW_IND];
	reg_set.start_reg.u8[1] = rx_struct->data[MB_MREG_SET_REG_NUM_HIGH_IND];
	reg_set.num_regs.u8[0] = rx_struct->data[MB_MREG_SET_NUM_REGS_LOW_IND];
	reg_set.num_regs.u8[1] = 0U;		// rx_struct->data[MB_MREG_SET_NUM_REGS_HIGH_IND];
	reg_set.attribute = rx_struct->data[MB_MREG_SET_NUM_REGS_HIGH_IND];
	reg_set.num_bytes = rx_struct->data[MB_MREG_SET_NUM_BYTES_IND];
	reg_set.data = &rx_struct->data[MB_MREG_SET_REG_VALS_IND];

	tx_struct->data[MB_MREG_SET_RESP_REG_NUM_HIGH_IND] =
		rx_struct->data[MB_MREG_SET_REG_NUM_HIGH_IND];
	tx_struct->data[MB_MREG_SET_RESP_REG_NUM_LOW_IND] =
		rx_struct->data[MB_MREG_SET_REG_NUM_LOW_IND];
	tx_struct->data[MB_MREG_SET_RESP_NUM_REGS_HIGH_IND] =
		rx_struct->data[MB_MREG_SET_NUM_REGS_HIGH_IND];
	tx_struct->data[MB_MREG_SET_RESP_NUM_REGS_LOW_IND] =
		rx_struct->data[MB_MREG_SET_NUM_REGS_LOW_IND];
	tx_struct->data_length = MB_MREG_SET_RESP_HEADER_SIZE;

	if ( ( reg_set.num_regs.u16 >= 1U ) &&
		 ( reg_set.num_regs.u16 <= MB_WRITE_MULTIPLE_REG_MAX_REGS ) &&
		 ( static_cast<uint16_t>( reg_set.num_regs.u16 << MULT_BY_2 ) ==
		   reg_set.num_bytes ) )
	{
		return_status = m_dci->Write_Registers( &reg_set, &reg_set_resp );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Write_Multiple_Reg( MB_FRAME_STRUCT* rx_struct,
											 MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	MB_REG_GET_STRUCT reg_get;
	MB_REG_GET_RESP_STRUCT reg_get_resp;
	MB_REG_SET_STRUCT reg_set;
	MB_REG_SET_RESP_STRUCT reg_set_resp;
	uint16_t potential_resp_len;

	// *** This is the read.
	reg_get.frame = rx_struct;
	reg_get_resp.frame = tx_struct;

	reg_get.start_reg.u8[0] = rx_struct->data[MB_MREG_GETSET_GET_REG_NUM_LOW_IND];
	reg_get.start_reg.u8[1] = rx_struct->data[MB_MREG_GETSET_GET_REG_NUM_HIGH_IND];
	reg_get.num_regs.u8[0] = rx_struct->data[MB_MREG_GETSET_NUM_REGS_GET_LOW_IND];
	reg_get.num_regs.u8[1] = 0U;		// rx_struct->data[MB_MREG_GETSET_NUM_REGS_GET_HIGH_IND];
	reg_get.attribute = rx_struct->data[MB_MREG_GETSET_NUM_REGS_GET_HIGH_IND];
	potential_resp_len = static_cast<uint16_t>( reg_get.num_regs.u16 << MULT_BY_2 ) +
		MB_MREG_GETSET_RESP_HEADER_SIZE;

	// *** This is the write.
	reg_set.frame = rx_struct;
	reg_set_resp.frame = tx_struct;

	reg_set.start_reg.u8[0] = rx_struct->data[MB_MREG_GETSET_SET_REG_NUM_LOW_IND];
	reg_set.start_reg.u8[1] = rx_struct->data[MB_MREG_GETSET_SET_REG_NUM_HIGH_IND];
	reg_set.num_regs.u8[0] = rx_struct->data[MB_MREG_GETSET_NUM_REGS_SET_LOW_IND];
	reg_set.num_regs.u8[1] = 0U;		// rx_struct->data[MB_MREG_GETSET_NUM_REGS_SET_HIGH_IND];
	reg_set.attribute = rx_struct->data[MB_MREG_GETSET_NUM_REGS_SET_HIGH_IND];
	reg_set.num_bytes = rx_struct->data[MB_MREG_GETSET_NUM_BYTES_SET_LOW_IND];
	reg_set.data = &rx_struct->data[MB_MREG_GETSET_DATA_IND];

	if ( /*( return_status == MB_NO_ERROR_CODE ) && */ ( reg_get.num_regs.u16 >= 1U ) &&
		 ( reg_get.num_regs.u16 <=
		   MB_READ_WRITE_MULTIPLE_READ_REG_MAX_REGS )
		 &&
		 ( reg_set.num_regs.u16 >= 1U ) &&
		 ( reg_set.num_regs.u16 <=
		   MB_READ_WRITE_MULTIPLE_WRITE_REG_MAX_REGS )
		 &&
		 ( static_cast<uint16_t>
		   ( reg_set.num_regs.u16 <<
			 MULT_BY_2 ) ==
		   static_cast<uint16_t>
		   ( reg_set.num_bytes ) ) &&
		 ( potential_resp_len <= m_buffer_size ) )
	{
		// Gotta check the read range before we do a write.  The write will check for us.
		return_status =
			m_dci->Check_Register_Range( reg_get.start_reg.u16, reg_get.num_regs.u16 );

		if ( return_status == MB_NO_ERROR_CODE )
		{
			return_status = m_dci->Write_Registers( &reg_set, &reg_set_resp );

			if ( return_status == MB_NO_ERROR_CODE )
			{
				reg_get_resp.data = &tx_struct->data[MB_MREG_GETSET_RESP_DATA];
				reg_get_resp.byte_count = 0U;

				return_status = m_dci->Read_Registers( &reg_get, &reg_get_resp );

				tx_struct->data[MB_MREG_GETSET_RESP_BYTE_NUM_IND] =
					static_cast<uint8_t>( reg_get_resp.byte_count );
				tx_struct->data_length = reg_get_resp.byte_count +
					MB_MREG_GETSET_RESP_HEADER_SIZE;
			}
		}
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Diagnostics( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status;

	Modbus_SysCmd::MB_DIAG_STRUCT diag_req;
	Modbus_SysCmd::MB_DIAG_RESP_STRUCT diag_resp;

	diag_req.frame = rx_struct;
	diag_resp.frame = tx_struct;
	diag_resp.data_len = 0U;

	diag_req.diag_code.u8[0] = rx_struct->data[MB_DIAG_GET_SUBCMD_HIGH_IND];
	diag_req.diag_code.u8[1] = rx_struct->data[MB_DIAG_GET_SUBCMD_LOW_IND];
	diag_req.data_len = rx_struct->data_length - MB_DIAG_GET_HEADER_SIZE;
	diag_req.data = &rx_struct->data[MB_DIAG_GET_DATA_IND];

	tx_struct->data[MB_DIAG_GET_RESP_SUBCMD_HIGH_IND] =
		rx_struct->data[MB_DIAG_GET_SUBCMD_HIGH_IND];
	tx_struct->data[MB_DIAG_GET_RESP_SUBCMD_LOW_IND] =
		rx_struct->data[MB_DIAG_GET_SUBCMD_LOW_IND];
	diag_resp.data = &tx_struct->data[MB_DIAG_GET_RESP_DATA_IND];

	return_status = m_syscmd->Read_Diagnostics( &diag_req, &diag_resp );

	tx_struct->data_length = diag_resp.data_len + MB_DIAG_GET_RESP_HEADER_SIZE;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Read_Exception_Status( MB_FRAME_STRUCT* rx_struct,
										   MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status;

	Modbus_SysCmd::MB_EXCEPT_STRUCT except_req;
	Modbus_SysCmd::MB_EXCEPT_RESP_STRUCT except_resp;

	except_resp.exception = 0U;

	except_req.frame = rx_struct;
	except_resp.frame = tx_struct;

	return_status = m_syscmd->Read_Exceptions( &except_req, &except_resp );

	tx_struct->data[MB_EXCEPTION_GET_RESP_DATA_IND] = except_resp.exception;
	tx_struct->data_length = MB_EXCEPTION_GET_RESP_HEADER_SIZE;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Sys_Cmd_User_Func( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status;

	Modbus_SysCmd::MB_SYS_CMD_REQ_STRUCT sys_cmd_req;
	Modbus_SysCmd::MB_SYS_CMD_RESP_STRUCT sys_cmd_resp;

	sys_cmd_req.frame = rx_struct;
	sys_cmd_req.command = rx_struct->data[MB_SYS_CMD_SUB_FUNC_CODE_IND];
	sys_cmd_req.data_len = rx_struct->data_length - MB_SYS_CMD_HEADER_SIZE;
	sys_cmd_req.data = &rx_struct->data[MB_SYS_CMD_DATA_IND];

	sys_cmd_resp.frame = tx_struct;
	sys_cmd_resp.data_len = 0U;
	tx_struct->data[MB_SYS_CMD_RESP_SUB_FUNC_CODE_IND] =
		rx_struct->data[MB_SYS_CMD_SUB_FUNC_CODE_IND];
	sys_cmd_resp.data = &tx_struct->data[MB_SYS_CMD_RESP_DATA_IND];
	BF_Lib::Copy_String( sys_cmd_resp.data, sys_cmd_req.data, sys_cmd_req.data_len );

	return_status = m_syscmd->Sys_Cmd_Request( &sys_cmd_req, &sys_cmd_resp );

	tx_struct->data_length = MB_SYS_CMD_RESP_HEADER_SIZE + sys_cmd_resp.data_len;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Get_Device_Identity( MB_FRAME_STRUCT* rx_struct,
										 MB_FRAME_STRUCT* tx_struct )
{
	uint8_t return_status;
	MB_DEV_ID_REQ_STRUCT dev_id_req = {0U};
	MB_DEV_ID_RESP_STRUCT dev_id_resp = {0U};

	dev_id_req.frame = rx_struct;
	dev_id_req.mei_req = rx_struct->data[MB_DEV_ID_MEI_TYPE_IND];
	dev_id_req.read_dev_id_code = rx_struct->data[MB_DEV_ID_READ_DEV_ID_IND];
	dev_id_req.obj_id = rx_struct->data[MB_DEV_ID_OBJ_ID_IND];
	dev_id_req.max_obj_data_length = m_buffer_size - MB_DEV_ID_RESP_HEADER_SIZE;
	dev_id_resp.obj_list_dest = &tx_struct->data[MB_DEV_ID_RESP_OBJ_LIST_IND];

	dev_id_resp.frame = tx_struct;
	return_status = m_dci->Get_Device_Identity( &dev_id_req, &dev_id_resp );

	tx_struct->data[MB_DEV_ID_RESP_MEI_TYPE_IND] = rx_struct->data[MB_DEV_ID_MEI_TYPE_IND];
	tx_struct->data[MB_DEV_ID_RESP_READ_DEV_ID_IND] =
		rx_struct->data[MB_DEV_ID_READ_DEV_ID_IND];
	tx_struct->data[MB_DEV_ID_RESP_CONFORMITY_LEVEL_IND] = dev_id_resp.conformity_level;
	tx_struct->data[MB_DEV_ID_RESP_MORE_FOLLOWS_IND] = dev_id_resp.more_follows;
	tx_struct->data[MB_DEV_ID_RESP_NEXT_OBJ_ID_IND] = dev_id_resp.next_obj_id;
	tx_struct->data[MB_DEV_ID_RESP_NUM_OBJ_IND] = dev_id_resp.num_of_objects;
	tx_struct->data_length = dev_id_resp.obj_list_len + MB_DEV_ID_RESP_OBJ_LIST_IND;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Msg::Msg_Length_Good( MB_FRAME_STRUCT const* x_struct, uint8_t func_code )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t check_length = 0U;

	switch ( func_code )
	{
		case MB_READ_ATTRIB_BIT_ACCESS_FUNC_CODE:		// Added for bit attribute retrieval.
		case MB_READ_COILS_FUNC_CODE:
		case MB_READ_DISCRETE_INPUTS_FUNC_CODE:
			check_length = MB_MBIT_GET_HEADER_SIZE;
			break;

		case MB_READ_ATTRIB_ACCESS_FUNC_CODE:
		case MB_READ_HOLDING_REG_FUNC_CODE:
		case MB_READ_INPUT_REG:
			check_length = MB_MREG_GET_HEADER_SIZE;
			break;

		case MB_WRITE_SINGLE_COIL_FUNC_CODE:
			check_length = MB_SBIT_SET_HEADER_SIZE;
			break;

		case MB_WRITE_SINGLE_REGISTER_FUNC_CODE:
			check_length = MB_SREG_SET_HEADER_SIZE;
			break;

		case MB_READ_EXCEPTION_STATUS_FUNC_CODE:
			check_length = MB_EXCEPTION_GET_HEADER_SIZE;
			break;

		case MB_DIAGNOSTICS_FUNC_CODE:
			if ( x_struct->data_length >= MB_DIAG_GET_HEADER_SIZE )
			{	// The diagnostic message has no information regarding length so we just pass it
				// along.
				check_length = static_cast<uint8_t>( x_struct->data_length );
			}
			else
			{	// If the length is not even the size of the diag header size then we force a
				// failure.
				check_length = static_cast<uint8_t>( x_struct->data_length + 1U );
			}
			break;

		case MB_WRITE_ATTRIB_BIT_ACCESS_FUNC_CODE:		// Added for bit attribute retrieval.
		case MB_WRITE_MULTIPLE_COILS_FUNC_CODE:
			check_length = MB_MBIT_SET_HEADER_SIZE + x_struct->data[MB_MBIT_SET_NUM_BYTES_IND];
			break;

		case MB_WRITE_ATTRIB_ACCESS_FUNC_CODE:
		case MB_WRITE_MULTIPLE_REG_FUNC_CODE:
			check_length = MB_MREG_SET_HEADER_SIZE + x_struct->data[MB_MREG_SET_NUM_BYTES_IND];
			break;

		case MB_WRITE_MASK_WRITE_REG_FUNC_CODE:
			check_length = MB_MSKREG_SET_HEADER_SIZE;
			break;

		case MB_READ_WRITE_ATTRIB_ACCESS_FUNC_CODE:
		case MB_READ_WRITE_MULTIPLE_REG_FUNC_CODE:
			check_length = MB_MREG_GETSET_HEADER_SIZE
				+ x_struct->data[MB_MREG_GETSET_NUM_BYTES_SET_LOW_IND];
			break;

		case MB_SYS_CMD_USER_FUNC_CODE:
			if ( x_struct->data_length >= MB_SYS_CMD_HEADER_SIZE )
			{		// The sys cmd message has no information regarding length so we just pass it
					// along.
				check_length = static_cast<uint8_t>( x_struct->data_length );
			}
			else
			{
				check_length = static_cast<uint8_t>( x_struct->data_length + 1U );
			}
			break;

		case MB_GET_DEVICE_IDENTITY_FUNC_CODE:
			check_length = MB_DEV_ID_HEADER_SIZE;
			break;

		default:
			return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;

			callback_list_item_t* temp_item = m_user_defined_commands;

			while ( temp_item != nullptr )
			{
				if ( func_code == temp_item->m_function_code )
				{
					return_status = MB_NO_ERROR_CODE;
					temp_item = reinterpret_cast<callback_list_item_t*>( nullptr );
					check_length = static_cast<uint8_t>( x_struct->data_length );
				}
				else
				{
					temp_item = temp_item->m_next_item;
				}
			}

			break;
	}

	if ( ( return_status == MB_NO_ERROR_CODE ) &&
		 ( x_struct->data_length != check_length ) )
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

}	/* end namespace BF_Mbus for this module */
