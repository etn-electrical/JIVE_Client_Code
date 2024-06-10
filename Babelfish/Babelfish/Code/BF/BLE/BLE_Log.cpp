/**
 *****************************************************************************************
 *  @file BLE_Log.cpp
 *  @details See header file for module overview.
 *  @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "BLE_Log.h"
#include "DCI_Data.h"
#include "Log_Config.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BLE_Log::BLE_Log( log_id_t log_id, DCI_ID_TD log_dcid, DCI_ID_TD index_dcid, uint16_t max_client_data_buf,
				  BF_Misc::Log_Mem* log_mem ) :
	m_log_id( log_id ),
	m_log_owner( new DCI_Owner( log_dcid ) ),
	m_index_owner( new DCI_Owner( index_dcid ) ),
	m_last_read_index( 0U ),
	m_tx_buff_size( max_client_data_buf - LOG_HEADER_SIZE ),
	m_log_mem( log_mem )
{
	DCI_LENGTH_TD index_dcid_length = 0U;

	m_index_owner->Check_Out_Length( &index_dcid_length );

	BF_ASSERT( index_dcid_length == sizeof ( BF_Misc::Log_Mem::entry_index_t ) );

	m_index_owner->Check_Out( m_last_read_index );

	m_log_owner->Attach_Callback( &Ble_Log_Process_Callback_Static,
								  reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
								  ( DCI_ACCESS_GET_RAM_CMD_MSK ) );

	m_index_owner->Attach_Callback( &Index_Update_Callback_Static,
									reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
									( DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_GET_RAM_CMD_MSK ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD BLE_Log::Ble_Log_Process_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD ret_val = DCI_CBACK_RET_PROCESS_NORMALLY;
	BF_Misc::Log_Mem::error_t get_entry_status = BF_Misc::Log_Mem::NO_ERROR;
	uint8_t* log_data_ptr = ( uint8_t* ) ( access_struct->data_access.data );
	BF_Misc::Log_Mem::entry_index_t start_index = 0U;
	BF_Misc::Log_Mem::entry_index_t entry_count = BF_Misc::Log_Mem::MAX_VALUE;
	BF_Misc::Log_Mem::entry_index_t next_index = 0U;
	BF_Misc::Log_Mem::entry_index_t remaining_entries = 0U;
	BF_Misc::Log_Mem::entry_size_t entry_size = 0U;

	if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
	{
		start_index = m_last_read_index;
		get_entry_status = m_log_mem->Get_Entry( start_index, &entry_count,
												 reinterpret_cast<uint8_t*>( log_data_ptr +
																			 LOG_DATA_LOC ),
												 &next_index, &remaining_entries, m_tx_buff_size );
		if ( get_entry_status == BF_Misc::Log_Mem::NO_ERROR )
		{
			entry_size = m_log_mem->Get_Entry_Size();
			BF_Lib::Copy_String( &log_data_ptr[NEXT_INDEX_LOC], reinterpret_cast<uint8_t*>( &next_index ),
								 sizeof( next_index ) );
			BF_Lib::Copy_String( &log_data_ptr[RETRIEVED_INDEX_LOC], reinterpret_cast<uint8_t*>( &entry_count ),
								 sizeof( entry_count ) );
			BF_Lib::Copy_String( &log_data_ptr[REMAINING_INDEX_LOC], reinterpret_cast<uint8_t*>( &remaining_entries ),
								 sizeof( remaining_entries ) );
			access_struct->data_access.length = LOG_HEADER_SIZE + ( entry_count * entry_size );
			m_last_read_index = next_index;
			ret_val = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
		}
		else
		{
			switch ( get_entry_status )
			{
				case BF_Misc::Log_Mem::INVALID_INDEX:
					ret_val = DCI_CBack_Encode_Error( DCI_ERR_INVALID_DATA_ID );
					break;

				case BF_Misc::Log_Mem::BUSY:
					ret_val = DCI_CBack_Encode_Error( DCI_ERR_NV_ACCESS_FAILURE );
					break;

				default:
					ret_val = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
			}
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD BLE_Log::Index_Update_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	/* check if command is valid */
	switch ( access_struct->command )
	{
		case DCI_ACCESS_SET_RAM_CMD:
		{
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &m_last_read_index ),
								 reinterpret_cast<uint8_t*>( access_struct->data_access.data ),
								 static_cast<uint32_t>( access_struct->data_access.length ) );
			return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
			break;
		}

		case DCI_ACCESS_GET_RAM_CMD:
		{
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( access_struct->data_access.data ),
								 reinterpret_cast<uint8_t*>( &m_last_read_index ),
								 static_cast<uint32_t>( access_struct->data_access.length ) );
			return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
			break;
		}

		default:
			return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;

	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_Log::Update_Tx_Buff_Size( BF_Misc::Log_Mem::entry_size_t buff_size )
{
	m_tx_buff_size = buff_size;
}
