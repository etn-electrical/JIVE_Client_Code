/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Commit_Copy.h"

#include "includes.h"

namespace BF_FUS
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Commit_Copy::Commit_Copy( void )
	: m_commit( nullptr ),
	m_state( IDLE )
{
	// TODO Auto-generated constructor stub
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Commit_Copy::Image_Handler( fw_state_op_t* commit )
{
	switch ( m_state )
	{
		case IDLE:
			m_commit = commit;
			if ( ( m_commit->untrusted_nv_ctrl != nullptr ) && ( m_commit->trusted_nv_ctrl != nullptr ) )
			{
				m_state = COMMIT;
			}
			else
			{
				m_commit->status = FUS_PREVIOUS_CMD_ERROR;
			}
			break;

		case COMMIT:
			Commit_State_Handler();
			break;

		case RETRY:
			Retry_Image_Handler();
			break;

		case WAIT:
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Reset( void )
{
	if ( m_commit != nullptr )
	{
		m_commit->status = FUS_NO_ERROR;
		m_commit->state_prev = IDLE;
		m_commit->sub_state = COMMON_IDLE;
	}
	m_state = IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Commit_State_Handler( void )
{
	switch ( m_commit->sub_state )
	{
		case COMMON_IDLE:
			if ( m_commit->data != nullptr )
			{
				m_commit->sub_state = COMMIT_ERASE_REAL_NV;
				m_commit->status = FUS_PENDING_CALL_BACK;
				m_commit->state_prev = IDLE;
			}
			else
			{
				m_commit->status = FUS_PREVIOUS_CMD_ERROR;
			}
			break;

		case COMMIT_ERASE_REAL_NV:
			Erase_Real_Nv_Image_Handler();
			break;

		case COMMIT_READ_HEADER:
			Read_Header_Image_Handler();
			break;

		case COMMIT_EXTRACT_HEADER:
			Extract_Header_Image_Handler();
			break;

		case COMMIT_READ_DATA:
			Read_Data_Image_Handler();
			break;

		case COMMIT_WRITE_DATA:
			Write_Data_Image_Handler();
			break;

		default:
			break;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Erase_Real_Nv_Image_Handler( void )
{
	NV_Ctrl::mem_range_info_t trusted_mem_range_info = {0};
	NV_Ctrl::nv_status_t nv_status = m_commit->trusted_nv_ctrl->Erase_All( Commit_Copy::Commit_Status_Cb_Static, this );

	m_commit->trusted_nv_ctrl->Get_Memory_Info( &trusted_mem_range_info );

	if ( NV_Ctrl::PENDING_CALL_BACK == nv_status )
	{
		m_commit->wait_time =
			m_commit->trusted_nv_ctrl->Erase_Time( trusted_mem_range_info.start_address,
												   ( trusted_mem_range_info.end_address -
													 trusted_mem_range_info.start_address ) );
		m_commit->wait_time += Add_Buffer_Time( m_commit->wait_time );
		m_commit->state_prev = COMMIT;
		m_state = WAIT;	/* Image will be in WAIT state for infinite time ToDo */
		m_commit->sub_state = COMMIT_ERASE_REAL_NV;
		m_commit->status = FUS_PENDING_CALL_BACK;
		m_commit->start_time = BF_Lib::Timers::Get_Time();

	}
	else if ( NV_Ctrl::BUSY_ERROR == nv_status )
	{
		m_commit->wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_commit->retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_commit->state_prev = COMMIT;
		m_state = RETRY;// Alert: Update m_retry_timeout before going into RETRY state
		m_commit->sub_state = COMMIT_ERASE_REAL_NV;
		m_commit->retry_count = 0U;	// Clear retry count
	}
	else
	{
		// ToDo
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Read_Header_Image_Handler( void )
{
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = {0};

	m_commit->untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );

	NV_Ctrl::nv_status_t status = m_commit->untrusted_nv_ctrl->Read( ( const_cast<uint8_t*>( m_commit->data ) ),
																	 untrusted_mem_range_info.start_address,
																	 sizeof( real_fw_header_s ),
																	 Commit_Copy::Commit_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_commit->wait_time = m_commit->untrusted_nv_ctrl->Read_Time( m_commit->data, 0, sizeof( real_fw_header_s ) );
		m_commit->wait_time += Add_Buffer_Time( m_commit->wait_time );
		m_commit->state_prev = COMMIT;
		m_state = WAIT;	/* We will be in WAIT state for infinite time ToDo */
		m_commit->sub_state = COMMIT_READ_HEADER;
		m_commit->status = FUS_PENDING_CALL_BACK;
		m_commit->start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_commit->wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_commit->retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_commit->state_prev = COMMIT;
		m_state = RETRY;// Alert: Update m_retry_timeout before going into RETRY state
		m_commit->sub_state = COMMIT_READ_HEADER;
		m_commit->retry_count++;
	}
	else
	{
		// ToDo
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Extract_Header_Image_Handler( void )
{
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = {0};

	m_commit->untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );
	m_commit->address = untrusted_mem_range_info.start_address;

	/* Extract firmware length from header */
	real_fw_header_s* header = reinterpret_cast<real_fw_header_s*>( m_commit->data );

	m_commit->fw_length = header->f.data_length;

	m_commit->state_prev = COMMIT;
	m_state = COMMIT;
	m_commit->sub_state = COMMIT_READ_DATA;
	FUS_DEBUG_PRINT( DBG_MSG_INFO, "Copied Firmware length : %d Bytes", m_commit->fw_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Read_Data_Image_Handler( void )
{
	if ( m_commit->fw_length > MAX_CHUNK_SIZE )
	{
		m_commit->length = MAX_CHUNK_SIZE;
	}
	else
	{
		m_commit->length = m_commit->fw_length;
	}
	NV_Ctrl::nv_status_t status = m_commit->untrusted_nv_ctrl->Read( ( const_cast<uint8_t*>( m_commit->data ) ),
																	 m_commit->address, m_commit->length,
																	 Commit_Copy::Commit_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_commit->wait_time = m_commit->untrusted_nv_ctrl->Read_Time( m_commit->data, 0, m_commit->length );
		m_commit->wait_time += Add_Buffer_Time( m_commit->wait_time );
		m_commit->state_prev = COMMIT;
		m_state = WAIT;
		m_commit->sub_state = COMMIT_READ_DATA;
		m_commit->status = FUS_PENDING_CALL_BACK;
		m_commit->start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_commit->wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_commit->retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_commit->state_prev = COMMIT;
		m_state = RETRY;
		m_commit->sub_state = COMMIT_READ_DATA;
		m_commit->retry_count++;
	}
	else
	{
		// ToDo
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Write_Data_Image_Handler( void )
{
	NV_Ctrl::mem_range_info_t trusted_mem_range_info = {0};
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = {0};

	m_commit->untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );
	m_commit->trusted_nv_ctrl->Get_Memory_Info( &trusted_mem_range_info );

	uint32_t offset = m_commit->address - untrusted_mem_range_info.start_address;
	uint32_t real_address = trusted_mem_range_info.start_address + offset;

	/*
	   LTK-8543 -
	   In STM32F767 controller, due to cache memory implementation WebUI code pack image on external flash failes to
	      download. Operation failes during the validation stage.
	   To resovlve this issue we need to clean and invalidate the cache memory before write operation, so that
	      controller will copy data from actual memory rather than cache memory during write operation.
	 */
#if defined( DATA_CACHE_ENABLE )
	SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( g_fus_op_buf ), MAX_CHUNK_SIZE );
#endif	// DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
	SCB_InvalidateICache();
#endif	// INSTRUCTION_CACHE_ENABLE

	NV_Ctrl::nv_status_t status = m_commit->trusted_nv_ctrl->Write( ( const_cast<uint8_t*>( m_commit->data ) ),
																	real_address,
																	m_commit->length,
																	Commit_Copy::Commit_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_commit->wait_time = m_commit->trusted_nv_ctrl->Write_Time( m_commit->data, 0, m_commit->length, false );
		m_commit->wait_time += Add_Buffer_Time( m_commit->wait_time );
		m_commit->state_prev = COMMIT;
		m_state = WAIT;
		m_commit->sub_state = COMMIT_WRITE_DATA;
		m_commit->status = FUS_PENDING_CALL_BACK;
		m_commit->start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_commit->wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_commit->retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_commit->state_prev = COMMIT;
		m_state = RETRY;
		m_commit->sub_state = COMMIT_WRITE_DATA;
		m_commit->retry_count++;
	}
	else
	{
		// ToDo
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Retry_Image_Handler( void )
{
	uint32_t cur_time = BF_Lib::Timers::Get_Time();

	if ( cur_time > m_commit->retry_timeout )
	{
		m_commit->retry_count++;
		if ( m_commit->retry_count > MAX_RETRY_COUNT )
		{
			m_commit->retry_count = 0U;
			/* Enough retry, Now stop */
			m_state = ERROR;
		}
		else
		{
			m_state = m_commit->state_prev;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_Copy::Commit_Status_Cb( NV_Ctrl::nv_status_t status )
{
	switch ( status )
	{
		case NV_Ctrl::SUCCESS:
			// m_commit->status = FUS_NO_ERROR;
			if ( ( m_commit->state_prev == COMMIT ) && ( m_commit->sub_state == COMMIT_ERASE_REAL_NV ) )
			{
				m_commit->state_prev = COMMIT;
				m_state = COMMIT;
				m_commit->sub_state = COMMIT_READ_HEADER;
			}
			else if ( ( m_commit->state_prev == COMMIT ) && ( m_commit->sub_state == COMMIT_READ_HEADER ) )
			{
				m_commit->state_prev = COMMIT;
				m_state = COMMIT;
				m_commit->sub_state = COMMIT_EXTRACT_HEADER;

			}
			else if ( ( m_commit->state_prev == COMMIT ) && ( m_commit->sub_state == COMMIT_READ_DATA ) )
			{
				m_commit->state_prev = COMMIT;
				m_state = COMMIT;
				m_commit->sub_state = COMMIT_WRITE_DATA;
			}
			else if ( ( m_commit->state_prev == COMMIT ) && ( m_commit->sub_state == COMMIT_WRITE_DATA ) )
			{
				m_commit->address += m_commit->length;
				m_commit->fw_length -= m_commit->length;
				if ( m_commit->fw_length == 0U )
				{
					/* Complete fw copied from src to dest */
					m_commit->state_prev = IDLE;
					m_state = IDLE;
					m_commit->sub_state = COMMON_IDLE;
					m_commit->status = FUS_COMMIT_DONE;

				}
				else
				{
					m_commit->state_prev = COMMIT;
					m_state = COMMIT;
					m_commit->sub_state = COMMIT_READ_DATA;
				}
			}
			break;

		case NV_Ctrl::DATA_ERROR:
			m_commit->status = FUS_PREVIOUS_CMD_ERROR;
			m_commit->state_prev = ERROR;
			FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Error in Commit operation " );
			break;

		// case NV_Ctrl::BUSY_ERROR:
		// case NV_Ctrl::WRITE_PROTECTED:
		// case NV_Ctrl::INVALID_ADDRESS:
		// case NV_Ctrl::OPERATION_NOT_SUPPORTED:
		// case NV_Ctrl::PENDING_CALL_BACK:
		// break;
		default:
			break;
	}
}

}


