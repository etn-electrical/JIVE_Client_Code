/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Modbus_Logging.h"
#include "Exception.h"
#include "Modbus_Log_Defines.h"
#include "Modbus_Defines.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Logging::Modbus_Logging( Modbus_Net* modbus_net, BF_Misc::Log_Mem** log_mem_handle_array,
								uint8_t total_log_id ) :
	m_log_mem_handle_array( log_mem_handle_array ),
	m_tx_buff_size( modbus_net->Get_Msg_Buffer_Size() ),
	m_total_log_id( total_log_id )
{
	modbus_net->Attach_User_Function( MB_LOGGING_FUNC_CODE, this,
									  &Process_Logging_Message_Static );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Logging::~Modbus_Logging( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Logging::Process_Logging_Message( MB_FRAME_STRUCT* rx_struct,
												 MB_FRAME_STRUCT* tx_struct )
{
	uint8_t log_cmd = 0xFFU;
	uint8_t return_status;

	log_cmd = rx_struct->data[MB_MREG_LOG_CMD];
	tx_struct->data[MB_MREG_LOG_CMD] = log_cmd;

	switch ( log_cmd )
	{
		case RETRIEVE_LOG_COMMAND:
			return_status = Read_Log_Data( rx_struct, tx_struct );
			break;

		case CLEAR_LOG_COMMAND:
			return_status = Clear_Log_Mem( rx_struct, tx_struct );
			break;

		default:
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::INVALID_CMD;
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE;
			return_status = MB_NO_ERROR_CODE;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Logging::Read_Log_Data( MB_FRAME_STRUCT* rx_struct,
									   MB_FRAME_STRUCT* tx_struct )
{
	/*Extract query and read logged data then Prepare Rsp Frame*/
	uint8_t modbus_return_status = MB_NO_ERROR_CODE;
	BF_Misc::Log_Mem::error_t log_return_status = BF_Misc::Log_Mem::NO_ERROR;
	uint8_t log_id;
	uint16_t total_byte_count;

	MBLOG_REG_GET_STRUCT reg_get;
	MBLOG_REG_GET_RESP_STRUCT reg_get_resp;

	log_id = rx_struct->data[MB_MREG_LOG_ID];

	if ( ( log_id >= m_total_log_id ) || ( m_log_mem_handle_array[log_id] == nullptr ) )
	{
		modbus_return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}
	else
	{
		tx_struct->data[MB_MREG_LOG_ID] = log_id;

		BF_Lib::SPLIT_UINT32 next_index;
		BF_Lib::SPLIT_UINT32 remaining_entries;

		reg_get.frame = rx_struct;
		reg_get_resp.frame = tx_struct;

		reg_get.start_index.u8[0U] = rx_struct->data[MB_MREG_REG_STARTINDEX0];
		reg_get.start_index.u8[1U] = rx_struct->data[MB_MREG_REG_STARTINDEX1];
		reg_get.start_index.u8[2U] = rx_struct->data[MB_MREG_REG_STARTINDEX2];
		reg_get.start_index.u8[3U] = rx_struct->data[MB_MREG_REG_STARTINDEX3];

		reg_get.num_regs.u8[0U] = rx_struct->data[MB_MREG_REG_DATALENTH0];
		reg_get.num_regs.u8[1U] = rx_struct->data[MB_MREG_REG_DATALENTH1];
		reg_get.num_regs.u8[2U] = rx_struct->data[MB_MREG_REG_DATALENTH2];
		reg_get.num_regs.u8[3U] = rx_struct->data[MB_MREG_REG_DATALENTH3];

		reg_get_resp.data = &tx_struct->data[MB_MREG_LOGRESP_DATA];

		log_return_status = m_log_mem_handle_array[log_id]->Get_Entry( reg_get.start_index.u32,
																	   &reg_get.num_regs.u32,
																	   reg_get_resp.data,
																	   &next_index.u32,
																	   &remaining_entries.u32,
																	   ( m_tx_buff_size -
																		 MODBUS_LOG_HEADER_SIZE ) );

		if ( log_return_status == BF_Misc::Log_Mem::NO_ERROR )
		{
			tx_struct->data[MB_MREG_LOGRESP_NXT_IND0] = next_index.u8[0U];
			tx_struct->data[MB_MREG_LOGRESP_NXT_IND1] = next_index.u8[1U];
			tx_struct->data[MB_MREG_LOGRESP_NXT_IND2] = next_index.u8[2U];
			tx_struct->data[MB_MREG_LOGRESP_NXT_IND3] = next_index.u8[3U];

			tx_struct->data[MB_MREG_REG_DATALENTH0] = reg_get.num_regs.u8[0U];
			tx_struct->data[MB_MREG_REG_DATALENTH1] = reg_get.num_regs.u8[1U];
			tx_struct->data[MB_MREG_REG_DATALENTH2] = reg_get.num_regs.u8[2U];
			tx_struct->data[MB_MREG_REG_DATALENTH3] = reg_get.num_regs.u8[3U];

			tx_struct->data[MB_MREG_LOGRESP_REMAINING_IND0] = remaining_entries.u8[0U];
			tx_struct->data[MB_MREG_LOGRESP_REMAINING_IND1] = remaining_entries.u8[1U];
			tx_struct->data[MB_MREG_LOGRESP_REMAINING_IND2] = remaining_entries.u8[2U];
			tx_struct->data[MB_MREG_LOGRESP_REMAINING_IND3] = remaining_entries.u8[3U];

			total_byte_count = m_log_mem_handle_array[log_id]->Get_Entry_Size();
			reg_get.num_regs.u32 *= static_cast<uint32_t>( total_byte_count );

			reg_get_resp.byte_count.u32 = reg_get.num_regs.u32 + MB_MREG_LOGRESP_NXT_INDSIZE +
				MB_MREG_LOGRESP_DATA_INDSIZE + MB_MREG_LOGRESP_REMAINING_INDSIZE
				+ MB_MREG_LOGRESP_CMD_IDSIZE;

			tx_struct->data_length = reg_get_resp.byte_count.u32 + MB_MREG_RESP_HEADER_SIZE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::BUSY_TIME )
		{
			/* Added code for future implementation */
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::BUSY_TIME;
			/* Total time the Flash module will take to clear the memory */
			tx_struct->data[MB_MREG_ERROR_DATA_TIME1] = next_index.u8[0U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME2] = next_index.u8[1U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME3] = next_index.u8[2U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME4] = next_index.u8[3U];
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE + MB_MREG_ERROR_DATA_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::BUSY )
		{
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::BUSY;
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::INVALID_INDEX )
		{
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::INVALID_INDEX;
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else
		{
			/* Do Nothing */
		}
	}
	return ( modbus_return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Logging::Clear_Log_Mem( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
{
	uint8_t modbus_return_status = MB_NO_ERROR_CODE;
	BF_Misc::Log_Mem::error_t log_return_status = BF_Misc::Log_Mem::NO_ERROR;
	uint8_t log_id;
	MBLOG_CLEAR_GET_STRUCT reg_get;
	MBLOG_CLEAR_RESP_STRUCT reg_get_resp;

	log_id = rx_struct->data[MB_MREG_LOG_ID];

	if ( ( log_id >= m_total_log_id ) || ( m_log_mem_handle_array[log_id] == nullptr ) )
	{
		modbus_return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}
	else
	{
		reg_get.frame = rx_struct;
		reg_get_resp.frame = tx_struct;

		reg_get.entry_count.u8[0U] = rx_struct->data[MB_MREG_REG_STARTINDEX0];
		reg_get.entry_count.u8[1U] = rx_struct->data[MB_MREG_REG_STARTINDEX1];
		reg_get.entry_count.u8[2U] = rx_struct->data[MB_MREG_REG_STARTINDEX2];
		reg_get.entry_count.u8[3U] = rx_struct->data[MB_MREG_REG_STARTINDEX3];

		log_return_status = m_log_mem_handle_array[log_id]->Clear( reg_get.entry_count.u32,
																   &reg_get_resp.num_deleted.u32 );
		if ( log_return_status == BF_Misc::Log_Mem::NO_ERROR )
		{
			tx_struct->data[MB_MREG_LOG_ID] = log_id;
			tx_struct->data[MB_MREG_LOGRESP_DELETED_IND0] = reg_get_resp.num_deleted.u8[0U];
			tx_struct->data[MB_MREG_LOGRESP_DELETED_IND1] = reg_get_resp.num_deleted.u8[1U];
			tx_struct->data[MB_MREG_LOGRESP_DELETED_IND2] = reg_get_resp.num_deleted.u8[2U];
			tx_struct->data[MB_MREG_LOGRESP_DELETED_IND3] = reg_get_resp.num_deleted.u8[3U];

			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_LOGRESP_CMD_IDSIZE +
				MB_MREG_DELETE_RESP_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::BUSY_TIME )
		{
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::BUSY_TIME;
			/* Total time the Flash module will take to clear the memory */
			tx_struct->data[MB_MREG_ERROR_DATA_TIME1] = reg_get_resp.num_deleted.u8[0U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME2] = reg_get_resp.num_deleted.u8[1U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME3] = reg_get_resp.num_deleted.u8[2U];
			tx_struct->data[MB_MREG_ERROR_DATA_TIME4] = reg_get_resp.num_deleted.u8[3U];
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE + MB_MREG_ERROR_DATA_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::BUSY )
		{
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::BUSY;
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else if ( log_return_status == BF_Misc::Log_Mem::INVALID_INDEX )
		{
			tx_struct->data[MB_MREG_LOG_CMD] = ( 0x80 | tx_struct->data[MB_MREG_LOG_CMD] );
			tx_struct->data[MB_MREG_LOG_ERRORCODE] = BF_Misc::Log_Mem::INVALID_INDEX;
			tx_struct->data_length = MB_MREG_RESP_HEADER_SIZE + MB_MREG_ERROR_LOG_CMD +
				MB_MREG_ERROR_CODE_SIZE;
			modbus_return_status = MB_NO_ERROR_CODE;
		}
		else
		{
			/* Do Nothing */
		}

		/* For non 0 delete command
		   if(reg_get_resp.byte_count.u32 !=reg_get.num_regs.u32)//read data validation
		   {
		    return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
		    return return_status;
		   }
		   if ( return_status == MB_NO_ERROR_CODE )
		   {
		    reg_get_resp.byte_count.u32 = MB_MREG_CLEAR_LOGRESP_DATASIZE;
		    tx_struct->data_length = reg_get_resp.byte_count.u32 + MB_MREG_RESP_HEADER_SIZE;
		   }
		   else
		   {
		    tx_struct->data[MB_MREG_REG_STARTINDEX3] = return_status;
		    reg_get_resp.byte_count.u32 = MB_MREG_CLEAR_LOGRESP_ERR_DATASIZE;
		    tx_struct->data_length = reg_get_resp.byte_count.u32 + MB_MREG_RESP_HEADER_SIZE;
		   }*/
	}
	return ( modbus_return_status );
}

}/* End namespace BF_Mbus for this module*/
