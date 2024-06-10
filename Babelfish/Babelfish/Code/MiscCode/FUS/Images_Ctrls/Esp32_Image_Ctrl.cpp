/**
 *****************************************************************************************
 * @file Esp32_Image_Ctrl.cpp
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "Esp32_Image_Ctrl.h"
#include "Babelfish_Assert.h"

namespace BF_FUS
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Esp32_Image_Ctrl::Esp32_Image_Ctrl( NV_Ctrl* scratch_nv_ctrl, NV_Ctrl* real_nv_ctrl,
									img_config_t* image_config, Commit_Ctrl* commit_ctrl )
	: Image_Header(),
	m_scratch_nv_ctrl( scratch_nv_ctrl ),
	m_image_config( image_config ),
	m_commit_ctrl( commit_ctrl ),
	m_new_image_ver( 0U ),
	m_init_crc( 0U ),
	m_new_prod_code( 0U ),
	m_is_scratch_erased( false ),
	m_esp_header_size( 0U )
{
	memset( &m_image, 0, sizeof( m_image ) );
	m_image.untrusted_nv_ctrl = real_nv_ctrl;
	m_image.trusted_nv_ctrl = real_nv_ctrl;
	#ifdef OVERRIDE_FUS_TIME
	m_set_validate_time = VALIDATE_WAIT_TIME_MS;
	m_set_commit_time = 0U;
	#endif	// OVERRIDE_FUS_TIME

	if ( LOCAL_COMP_APP_IMG_GUID == m_image_config->guid )
	{
		/*
		 * Offset for PXGreen header is = 24+ 8 + 256 = 288.
		 * sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)
		 * This offset should be used only for APP IMAGE, For WEB and REST Image it should be 0U.
		 */
		m_esp_header_size = sizeof( esp_image_header_t ) + sizeof( esp_image_segment_header_t ) +
			sizeof( esp_app_desc_t );
	}

	new CR_Tasker( &Image_Upgrade_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "Image Upgrade Task" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Get_Image_Specs( image_info_resp_t* image_resp )
{
	image_resp->name = m_image_config->name;
	image_resp->name_len = m_image_config->name_len;
	image_resp->guid = m_image_config->guid;
	image_resp->version = m_image_config->version;
	Image_Header::Get_Compatibility_Num( &image_resp->compatibility_num );
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Status( image_status_resp_t* status_resp )
{
	fus_op_status_t return_status = FUS_NO_ERROR;

	if ( m_image.status == FUS_COMMIT_DONE )
	{
		return_status = FUS_NO_ERROR;
		status_resp->remaining_op_time = DEFAULT_FUS_WAIT_TIME_MS;
	}
	else if ( m_image.status == FUS_PENDING_CALL_BACK )
	{
		return_status = FUS_PENDING_CALL_BACK;
		uint32_t time = BF_Lib::Timers::Time_Passed( m_image.start_time );
		if ( time < m_image.wait_time )
		{
			status_resp->remaining_op_time = ( m_image.wait_time - time );
		}
		else
		{
			status_resp->remaining_op_time = DEFAULT_FUS_WAIT_TIME_MS;
		}
	}
	else if ( m_image.status == FUS_PREVIOUS_CMD_ERROR )
	{
		return_status = FUS_PREVIOUS_CMD_ERROR;
		status_resp->remaining_op_time = DEFAULT_IDLE_TIME_MS;
	}
	else
	{
		status_resp->remaining_op_time = DEFAULT_IDLE_TIME_MS;
	}
	status_resp->status = m_image.status;
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

fus_op_status_t Esp32_Image_Ctrl::Write( image_write_req_t* write_req,
										 image_write_resp_t* write_resp )
{
	m_image.status = FUS_NO_ERROR;
	if ( m_is_scratch_erased == false )
	{
		if ( m_scratch_nv_ctrl != nullptr )
		{
			m_image.untrusted_nv_ctrl = m_scratch_nv_ctrl;
		}
		else
		{
			m_image.status = FUS_MEMORY_ACCESS_VIOLATION_ERROR;
		}
	}

	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = { };
	NV_Ctrl::mem_range_info_t trusted_mem_range_info = { };

	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );
	m_image.trusted_nv_ctrl->Get_Memory_Info( &trusted_mem_range_info );
	m_image.status = Check_Range( write_req->address, write_req->data_len, &trusted_mem_range_info );

	if ( m_image.status == FUS_NO_ERROR )
	{
		if ( m_image.state == IDLE )
		{
			uint32_t offset = write_req->address - trusted_mem_range_info.start_address;
			m_image.address = untrusted_mem_range_info.start_address + offset;
			m_image.length = write_req->data_len;

			if ( true == New_Request( m_image.address, m_image.length ) )
			{
				m_image.data = g_fus_op_buf;	/* m_length is validated against MAX_CHUNK_LENGTH defined in user
												   space(ToDo) */
				if ( m_image.data != nullptr )
				{
					BF_Lib::Copy_Data( m_image.data, m_image.length, write_req->data, write_req->data_len );
					/*
					   LTK-8543 -
					   In STM32F767 controller, due to cache memory implementation WebUI code pack image on external
					      flash failes to download. Operation failes during the validation stage.
					   To resovlve this issue we need to clean and invalidate the cache memory before write operation,
					      so that controller will copy data from actual memory rather than cache memory during write
					      operation.
					 */
#if defined( DATA_CACHE_ENABLE )
					SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( g_fus_op_buf ), MAX_CHUNK_SIZE );
#endif	// DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
					SCB_InvalidateICache();
#endif	// INSTRUCTION_CACHE_ENABLE
					if ( m_is_scratch_erased == false )
					{
						NV_Ctrl::nv_status_t nv_status = m_image.untrusted_nv_ctrl->Erase_All(
							Esp32_Image_Ctrl::Image_Status_Cb_Static, this );
						if ( NV_Ctrl::PENDING_CALL_BACK == nv_status )
						{
							m_image.wait_time =
								m_image.untrusted_nv_ctrl->Erase_Time( untrusted_mem_range_info.start_address,
																	   ( untrusted_mem_range_info.end_address -
																		 untrusted_mem_range_info.start_address ) );
							m_image.wait_time += m_image.untrusted_nv_ctrl->Write_Time( m_image.data,
																						untrusted_mem_range_info.start_address, m_image.length,
																						false );
							m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
							m_image.state_prev = ERASE;
							m_image.state = WAIT;	/* Image will be in WAIT state for infinite time ToDo */
							m_image.status = FUS_PENDING_CALL_BACK;
						}
						else if ( NV_Ctrl::BUSY_ERROR == nv_status )
						{
							m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
							m_image.retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
							m_image.state_prev = ERASE;
							m_image.state = RETRY;	// Alert: Update m_retry_timeout before going into RETRY state
							m_image.retry_count = 0U;	// Clear retry count
						}
						else
						{
							// ToDo
						}
					}
					else
					{
						NV_Ctrl::nv_status_t status =
							m_image.untrusted_nv_ctrl->Write( ( const_cast<uint8_t*>( m_image.data ) ),
															  m_image.address, m_image.length,
															  Esp32_Image_Ctrl::Image_Status_Cb_Static,
															  this );
						if ( NV_Ctrl::PENDING_CALL_BACK == status )
						{
							m_image.wait_time = m_image.untrusted_nv_ctrl->Write_Time( m_image.data, m_image.address,
																					   m_image.length,
																					   false );
							m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
							m_image.state_prev = WRITE;
							m_image.state = WAIT;	/* Image will be in WAIT state for infinite time ToDo */
							m_image.status = FUS_PENDING_CALL_BACK;
							m_image.start_time = BF_Lib::Timers::Get_Time();
						}
						else if ( NV_Ctrl::BUSY_ERROR == status )
						{
							m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
							m_image.retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
							m_image.state_prev = WRITE;
							m_image.state = RETRY;	// Alert: Update m_retry_timeout before going into RETRY state
							m_image.retry_count = 0U;	// Clear retry count
						}
						else
						{
							// ToDo
						}
					}
				}
				else
				{
					FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Heap memory not available " );
					// ToDo: Heap not available
					BF_ASSERT( false );
				}
			}
			else
			{
				/* Invalid request */
				m_image.wait_time = DEFAULT_IDLE_TIME_MS;
				m_image.status = FUS_BAD_ADDRESS_ERROR;
				FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Selected address invalid" );
			}
		}
		else
		{
			if ( m_image.state == ERROR )
			{
				m_image.wait_time = DEFAULT_IDLE_TIME_MS;
				m_image.status = FUS_PROCESSOR_NOT_READY_ERROR;
				FUS_DEBUG_PRINT( DBG_MSG_ERROR, "Processor not ready " );
			}
			else
			{
				m_image.wait_time = m_image.untrusted_nv_ctrl->Get_Wait_Time();
				m_image.status = FUS_ACCESS_TIME_PENDING;
			}

		}

	}
	else
	{
		// REQUEST_INVALID due to FUS_BAD_ADDRESS_ERROR or FUS_BAD_DATA_LENGTH_ERROR
		m_image.wait_time = DEFAULT_IDLE_TIME_MS;
	}
	write_resp->op_time = m_image.wait_time;
	write_resp->response_code = m_image.status;
	return ( m_image.status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Validate( image_validate_req_t* validate_req, image_validate_resp_t* validate_resp )
{
	if ( m_image.status == FUS_VALID_IMAGE )
	{
		validate_resp->op_time = 0U;
		validate_resp->status = m_image.status;
		m_image.state_prev = IDLE;
		m_image.state = RESET;
		m_image.sub_state = COMMON_IDLE;
	}
	else if ( ( m_image.status == FUS_INVALID_IMAGE_CHECKSUM_FAILED ) || \
			  ( m_image.status == FUS_INVALID_IMAGE_SIGNATURE_FAILED ) || \
			  ( m_image.status == FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED ) || \
			  ( m_image.status == FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED ) )
	{
		validate_resp->op_time = 0U;
		m_image.state_prev = IDLE;
		m_image.state = RESET;
		m_image.sub_state = COMMON_IDLE;
		validate_resp->status = m_image.status;
	}
	else if ( m_image.status == FUS_PENDING_CALL_BACK )
	{
	#ifdef OVERRIDE_FUS_TIME
		m_image.wait_time = m_commit_ctrl->Add_Buffer_Time( m_set_validate_time );
	#else
		m_image.wait_time = m_commit_ctrl->Add_Buffer_Time( VALIDATE_WAIT_TIME_MS );
	#endif	// OVERRIDE_FUS_TIME
		validate_resp->op_time = m_image.wait_time;
		validate_resp->status = FUS_VALIDATION_IN_PROGRESS;
	}
	else
	{
		m_image.data = g_fus_op_buf;
		if ( m_image.data != nullptr )
		{
			if ( m_image.state != VALIDATE )
			{
				m_image.state_prev = IDLE;
				m_image.state = VALIDATE;
				m_image.sub_state = VALIDATE_READ_HEADER;
			}
		#ifdef OVERRIDE_FUS_TIME
			m_image.wait_time = m_set_validate_time;
		#else
			m_image.wait_time = VALIDATE_WAIT_TIME_MS;
		#endif	// OVERRIDE_FUS_TIME
			m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
			validate_resp->op_time = m_image.wait_time;
			m_image.status = FUS_PENDING_CALL_BACK;
			validate_resp->status = FUS_VALIDATION_IN_PROGRESS;
			m_image.start_time = BF_Lib::Timers::Get_Time();
		}
	}
	return ( m_image.status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Commit( image_commit_resp_t* commit_resp )
{
	m_image.status = FUS_PENDING_CALL_BACK;
	NV_Ctrl::mem_range_info_t trusted_mem_range_info = { };
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = { };

	m_image.data = g_fus_op_buf;
	m_image.trusted_nv_ctrl->Get_Memory_Info( &trusted_mem_range_info );
	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );

	if ( m_image.data != nullptr )
	{
	#ifdef OVERRIDE_FUS_TIME
		if ( m_set_commit_time != 0U )
		{
			m_image.wait_time = m_set_commit_time;
		}
		else
		{
			m_image.wait_time =
				m_image.untrusted_nv_ctrl->Read_Time( m_image.data, 0,
													  ( untrusted_mem_range_info.end_address -
														untrusted_mem_range_info.start_address ) );
			m_image.wait_time +=
				m_image.trusted_nv_ctrl->Write_Time( NULL, trusted_mem_range_info.start_address,
													 ( trusted_mem_range_info.end_address -
													   trusted_mem_range_info.start_address ), false );
			m_image.wait_time +=
				m_image.trusted_nv_ctrl->Erase_Time( trusted_mem_range_info.start_address,
													 ( trusted_mem_range_info.end_address -
													   trusted_mem_range_info.start_address ) );
		}
	#else
		m_image.wait_time =
			m_image.untrusted_nv_ctrl->Read_Time( m_image.data, 0,
												  ( untrusted_mem_range_info.end_address -
													untrusted_mem_range_info.start_address ) );
		m_image.wait_time +=
			m_image.trusted_nv_ctrl->Write_Time( NULL, trusted_mem_range_info.start_address,
												 ( trusted_mem_range_info.end_address -
												   trusted_mem_range_info.start_address ), false );
		m_image.wait_time +=
			m_image.trusted_nv_ctrl->Erase_Time( trusted_mem_range_info.start_address,
												 ( trusted_mem_range_info.end_address -
												   trusted_mem_range_info.start_address ) );
	#endif	// OVERRIDE_FUS_TIME
		m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
		commit_resp->op_time = m_image.wait_time;
		m_image.state_prev = IDLE;
		m_image.state = COMMIT;
		m_image.sub_state = COMMIT_ERASE_REAL_NV;
		m_image.start_time = BF_Lib::Timers::Get_Time();
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, " In Commit URI - Heap memory not available " );
		// ToDo: Heap not available
		BF_ASSERT( false );
	}
	return ( m_image.status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Exit_FUM( void )
{
	m_image.status = FUS_NO_ERROR;
	m_image.untrusted_nv_ctrl = m_image.trusted_nv_ctrl;
	m_image.state = RESET;
	m_commit_ctrl->Reset();
	return ( m_image.status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Commit_Handler( void )
{
	if ( m_image.status != FUS_COMMIT_DONE )
	{
		m_commit_ctrl->Image_Handler( &m_image );
		if ( m_image.status == FUS_PREVIOUS_CMD_ERROR )
		{
			m_image.state_prev = ERROR;
		}
	}
	else
	{
		Update_Version();
		m_image.state = RESET;
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "New Firmware Updated " );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Validate_Handler( void )
{
	switch ( m_image.sub_state )
	{
		case COMMON_IDLE:
			break;

		case VALIDATE_READ_HEADER:
			Validate_Read_Header_Handler();
			break;

		case VALIDATE_EXTRACT_HEADER:
			Validate_Extract_Header_Handler();
			break;

		case VALIDATE_READ_DATA:
			Validate_Read_Data_Handler();
			break;

		case CALCULATE_CRC_DATA:
			Validate_CRC_Calculation_Handler();
			break;

		case VALIDATE_CRC_DATA:
			Validate_CRC_Data_Handler();
			break;

		case VALIDATE_CODE_SIGNING:
			Validate_Code_Signing_Handler();
			break;

		case VALIDATE_PROD_INFO:
			Validate_Product_Info_Handler();
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
void Esp32_Image_Ctrl::Validate_Read_Header_Handler( void )
{
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = { };

	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );

	NV_Ctrl::nv_status_t status = m_image.untrusted_nv_ctrl->Read( ( const_cast<uint8_t*>( m_image.data ) ),
																   untrusted_mem_range_info.start_address + m_esp_header_size,
																   sizeof( real_fw_header_s ),
																   Esp32_Image_Ctrl::Image_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_image.wait_time = m_image.untrusted_nv_ctrl->Read_Time( m_image.data, 0, sizeof( real_fw_header_s ) );
		m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
		m_image.state_prev = VALIDATE;
		m_image.state = WAIT;		/* We will be in WAIT state for infinite time ToDo */
		m_image.sub_state = VALIDATE_READ_HEADER;
		m_image.status = FUS_PENDING_CALL_BACK;
		m_image.start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_image.retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_image.state_prev = VALIDATE;
		m_image.state = RETRY;	// Alert: Update m_retry_timeout before going into RETRY state
		m_image.sub_state = VALIDATE_READ_HEADER;
		m_image.retry_count++;
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

void Esp32_Image_Ctrl::Validate_Extract_Header_Handler( void )
{

	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = { };

	m_new_prod_code = 0U;
	uint32_t version = 0U;
	uint32_t firm_rev = 0U;

	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );
	m_image.address = untrusted_mem_range_info.start_address + m_esp_header_size;
	const img_fw_version_t* new_firm_rev;

	/* Extract firmware length from header */
	m_init_crc = 0U;
	real_fw_header* header = reinterpret_cast<real_fw_header*>( m_image.data );

	uint32_t untrusted_mem_range =
		( ( untrusted_mem_range_info.end_address - untrusted_mem_range_info.start_address ) + 1 );

	if ( ( header == reinterpret_cast<const real_fw_header*>( NULL ) ) ||
		 ( header == reinterpret_cast<const real_fw_header*>( 0xffffffff ) ) ||
		 ( header->f.magic_endian != MAGIC_ENDIAN ) ||
		 ( header->f.data_length > untrusted_mem_range ) ||
		 ( header->f.image_guid != m_image_config->guid ) )
	{
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Firmware Length/ Firmware ID/ Endiancing mismatch " );
		m_image.status = FUS_INVALID_IMAGE_CHECKSUM_FAILED;
		m_image.state_prev = VALIDATE;
		m_image.state = RESET;
		m_image.sub_state = COMMON_IDLE;
	}
	else
	{
		m_image.fw_length = header->f.data_length - m_esp_header_size - ( sizeof ( header->fw_crc ) );
		m_image.address += sizeof ( header->fw_crc );
		m_new_prod_code = *( ( uint16_t* )header->f.product_code );
		new_firm_rev = reinterpret_cast<const img_fw_version_t*>( header->f.version_info );
		Image_Header::Update_Compatibility_Num( header->f.compatibility_num );
		m_image.fw_crc = header->fw_crc.crc;
		version = ( new_firm_rev->build << 16 ) & 0xFFFF0000;
		firm_rev = ( new_firm_rev->minor << 8 ) & 0x0000FF00;
		version = version | ( firm_rev );
		version = version | ( new_firm_rev->major & 0x00FF );
		m_new_image_ver = version;
		m_image.state_prev = VALIDATE;
		m_image.state = VALIDATE;
		/*
		   In ESP32, We are not using the CRC value for validation, That's why instead of moving to VALIDATE_READ_DATA.
		   We are directly moving to code singing validation ( VALIDATE_CODE_SIGNING ).
		   For Image Verification ESP32 is using checksum and HASH value which will already taken care in ESP32
		      bootlaoder.
		   Issue we are facing for CRC validation:- After appending the CRC value in"*.bin" file, ESP32 bootloader
		   checksum and HASH verificaiton becoming invalid. Due to this ESP is not able to find proper bootable
		      partition.
		 */
		if ( LOCAL_COMP_APP_IMG_GUID == m_image_config->guid )
		{
			m_image.sub_state = VALIDATE_CODE_SIGNING;
		}
		else
		{
			m_image.sub_state = VALIDATE_READ_DATA;
		}
        //Ahmed
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Firmware_version :  %x.%x.%x ", new_firm_rev->major, new_firm_rev->minor,
						 new_firm_rev->build );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Validate_Read_Data_Handler( void )
{
	uint32_t read_time = 0U;

	if ( m_image.fw_length > MAX_CHUNK_SIZE )
	{
		m_image.length = MAX_CHUNK_SIZE;
	}
	else
	{
		m_image.length = m_image.fw_length;
	}

	NV_Ctrl::nv_status_t status = m_image.untrusted_nv_ctrl->Read( ( const_cast<uint8_t*>( m_image.data ) ),
																   m_image.address, m_image.length,
																   Esp32_Image_Ctrl::Image_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		read_time = m_image.untrusted_nv_ctrl->Read_Time( m_image.data, 0, m_image.length );
		m_image.wait_time = ( ( m_image.fw_length / MAX_CHUNK_SIZE ) * read_time );
		m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
		m_image.state_prev = VALIDATE;
		m_image.state = WAIT;
		m_image.sub_state = VALIDATE_READ_DATA;
		m_image.status = FUS_PENDING_CALL_BACK;
		m_image.start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_image.retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_image.state_prev = VALIDATE;
		m_image.state = RETRY;
		m_image.sub_state = VALIDATE_READ_DATA;
		m_image.retry_count++;
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
void Esp32_Image_Ctrl::Validate_CRC_Calculation_Handler()
{
	uint16_t calc_crc = 0U;

	if ( m_image.fw_length == 0U )
	{
		m_image.state_prev = VALIDATE;
		m_image.state = VALIDATE;
		m_image.sub_state = VALIDATE_CRC_DATA;
		m_image.status = FUS_PENDING_CALL_BACK;
	}
	else
	{
		calc_crc = BF_Lib::CRC16::Calc_On_String( ( const_cast<uint8_t*>( m_image.data ) ), m_image.length,
												  m_init_crc );
		m_init_crc = calc_crc;
		m_image.fw_length -= m_image.length;
		if ( m_image.fw_length != 0U )
		{
			m_image.address += m_image.length;
			m_image.state_prev = VALIDATE;
			m_image.state = VALIDATE;
			m_image.sub_state = VALIDATE_READ_DATA;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void Esp32_Image_Ctrl::Validate_CRC_Data_Handler( void )
{
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = { };

	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );

	if ( m_image.fw_crc == m_init_crc )
	{
		if ( m_image.untrusted_nv_ctrl == m_image.trusted_nv_ctrl )
		{
			m_image.state_prev = VALIDATE;
			m_image.state = RESET;
			m_image.sub_state = COMMON_IDLE;
			m_image.status = FUS_VALID_IMAGE;
		}
		else
		{
			m_image.state_prev = IDLE;
			m_image.state = VALIDATE;
			m_image.sub_state = VALIDATE_CODE_SIGNING;
			m_image.status = FUS_PENDING_CALL_BACK;
		}
	}
	else
	{
		m_image.state_prev = VALIDATE;
		m_image.state = RESET;
		m_image.sub_state = COMMON_IDLE;
		m_image.status = FUS_INVALID_IMAGE_CHECKSUM_FAILED;
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Firmware checksum failed " );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Validate_Code_Signing_Handler( void )
{
	bool sign_status = false;
	PKI_Common::pki_status_t code_sign_ret;

	code_sign_ret = Prod_Spec_Code_Sign( m_image.untrusted_nv_ctrl, &sign_status );
	if ( code_sign_ret == PKI_Common::SUCCESS )
	{
		m_image.state_prev = IDLE;
		m_image.state = VALIDATE;
		m_image.sub_state = VALIDATE_PROD_INFO;
		m_image.status = FUS_PENDING_CALL_BACK;
	}
#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
	else if ( code_sign_ret == PKI_Common::PENDING_CALL_BACK )
	{
		// We are in pki_state = PKI_CERT_INIT
		// wait till operation completes.
	}
#endif
	else if ( code_sign_ret != PKI_Common::BUSY_ERROR )
	{
		m_image.state_prev = VALIDATE;
		m_image.state = RESET;
		m_image.sub_state = COMMON_IDLE;
		m_image.status = FUS_INVALID_IMAGE_SIGNATURE_FAILED;
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Firmware signature failed " );
	}
	else
	{
		// Verification is in-progress
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Validate_Product_Info_Handler( void )
{
	if ( PRODUCT_CODE == m_new_prod_code )
	{
		m_image.status = FUS_VALID_IMAGE;
		if ( false == Check_Rollback_Enabled() )
		{
			if ( false == Check_Version() )
			{
				m_image.status = FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED;
				FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Firmware version roll back failed " );
			}
		}
	}
	else
	{
		m_image.status = FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED;
		FUS_DEBUG_PRINT( DBG_MSG_ERROR, " Firmware product code failed " );
	}

	m_image.state_prev = VALIDATE;
	m_image.state = RESET;
	m_image.sub_state = COMMON_IDLE;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Image_Ctrl::Check_Rollback_Enabled( void )
{
	uint8_t image_upgrade_mode = m_image_config->upgrade_mode;
	bool status = true;		/// < Rollback allowed by default

	switch ( image_upgrade_mode )
	{

		case RESTRICTED_FW_UPGRADE:
		case NO_FW_UPGRADE:
			// Firmware rollback not allowed
			status = false;
			break;

		case UNRESTRICTED_FW_UPGRADE:
		default:
			break;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Esp32_Image_Ctrl::Check_Version( void )
{
	bool status = false;
	img_fw_version_t current_image_version;
	img_fw_version_t new_image_version;

	current_image_version.major = ( m_image_config->version ) & 0x000000FF;
	current_image_version.minor = ( ( m_image_config->version ) & 0x0000FF00 ) >> 8;
	current_image_version.build = ( ( m_image_config->version ) & 0xFFFF0000 ) >> 16;

	new_image_version.major = ( m_new_image_ver ) & 0x000000FF;
	new_image_version.minor = ( ( m_new_image_ver ) & 0x0000FF00 ) >> 8;
	new_image_version.build = ( ( m_new_image_ver ) & 0xFFFF0000 ) >> 16;

	if ( current_image_version.major == new_image_version.major )
	{
		if ( current_image_version.minor == new_image_version.minor )
		{
			if ( current_image_version.build <= new_image_version.build )
			{
				status = true;
			}
			else
			{
				// misra
			}
		}
		else if ( current_image_version.minor < new_image_version.minor )
		{
			status = true;
		}
		else
		{
			// misra
		}
	}
	else if ( current_image_version.major < new_image_version.major )
	{
		status = true;
	}
	else
	{
		// misra
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Reset_Handler( void )
{
	m_image.state = IDLE;
	m_image.address = 0U;
	m_image.length = 0U;
	if ( m_image.state_prev != WRITE )
	{
		m_is_scratch_erased = false;
	}
	m_image.retry_timeout = 0U;
	m_image.retry_count = 0U;
	m_image.fw_length = 0U;

	m_image.state_prev = IDLE;
	m_image.sub_state = COMMON_IDLE;
	m_init_crc = 0U;
	m_image.wait_time = DEFAULT_IDLE_TIME_MS;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Write_Handler( void )
{
	NV_Ctrl::nv_status_t status = m_image.untrusted_nv_ctrl->Write( ( const_cast<uint8_t*>( m_image.data ) ),
																	m_image.address, m_image.length,
																	Esp32_Image_Ctrl::Image_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_image.wait_time =
			m_image.untrusted_nv_ctrl->Write_Time( m_image.data, m_image.address, m_image.length, false );
		m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
		m_image.state_prev = WRITE;
		m_image.state = WAIT;	/* Image will be in WAIT state for infinite time ToDo */
		m_image.status = FUS_PENDING_CALL_BACK;
		m_image.start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		/* Scratch NV is not available, Somebody might be acquired it. Let's wait  */
		m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_image.retry_timeout = RETRY_TIME_MS + BF_Lib::Timers::Get_Time();
		m_image.state_prev = WRITE;
		m_image.state = RETRY;	// Alert: Update m_retry_timeout before going into RETRY state
		m_image.retry_count++;
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
void Esp32_Image_Ctrl::Erase_Handler( void )
{
	NV_Ctrl::mem_range_info_t untrusted_mem_range_info = {0};

	m_image.untrusted_nv_ctrl->Get_Memory_Info( &untrusted_mem_range_info );

	NV_Ctrl::nv_status_t status =
		m_image.untrusted_nv_ctrl->Erase_All( Esp32_Image_Ctrl::Image_Status_Cb_Static, this );

	if ( NV_Ctrl::PENDING_CALL_BACK == status )
	{
		m_image.wait_time =
			m_image.untrusted_nv_ctrl->Erase_Time( untrusted_mem_range_info.start_address,
												   ( untrusted_mem_range_info.end_address -
													 untrusted_mem_range_info.start_address ) );
		m_image.wait_time += m_commit_ctrl->Add_Buffer_Time( m_image.wait_time );
		m_image.state_prev = ERASE;
		m_image.state = WAIT;	/* Image will be in WAIT state for infinite time ToDo */
		m_image.status = FUS_PENDING_CALL_BACK;
		m_image.start_time = BF_Lib::Timers::Get_Time();
	}
	else if ( NV_Ctrl::BUSY_ERROR == status )
	{
		m_image.wait_time = BUSY_ERROR_WAIT_TIME_MS;
		m_image.state_prev = ERASE;
		m_image.state = RETRY;
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
void Esp32_Image_Ctrl::Retry_Handler( void )
{
	uint32_t cur_time = BF_Lib::Timers::Get_Time();

	if ( cur_time > m_image.retry_timeout )
	{
		m_image.retry_count++;
		if ( m_image.retry_count > MAX_RETRY_COUNT )
		{
			m_image.state = ERROR;
			m_image.retry_count = 0U;
			/* Enough retry, Now stop */
		}
		else
		{
			m_image.state = m_image.state_prev;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Error_Handler( void )
{
	m_image.status = FUS_PREVIOUS_CMD_ERROR;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Image_Status_Cb( NV_Ctrl::nv_status_t status )
{
	switch ( status )
	{
		case NV_Ctrl::SUCCESS:
			m_image.status = FUS_NO_ERROR;
			if ( m_image.state_prev == ERASE )
			{
				m_is_scratch_erased = true;
				m_image.state = WRITE;
				m_image.status = FUS_NO_ERROR;
			}
			else if ( m_image.state_prev == WRITE )
			{
				m_image.state = RESET;
			}
			else if ( ( m_image.state_prev == VALIDATE ) && ( m_image.sub_state == VALIDATE_READ_HEADER ) )
			{
				m_image.state_prev = VALIDATE;
				m_image.state = VALIDATE;
				m_image.sub_state = VALIDATE_EXTRACT_HEADER;
			}
			else if ( ( m_image.state_prev == VALIDATE ) && ( m_image.sub_state == VALIDATE_READ_DATA ) )
			{
				m_image.state_prev = VALIDATE;
				m_image.state = VALIDATE;
				m_image.sub_state = CALCULATE_CRC_DATA;
			}
			break;

		case NV_Ctrl::DATA_ERROR:
			m_image.status = FUS_PREVIOUS_CMD_ERROR;
			m_image.state_prev = ERROR;
			m_image.state = RESET;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Esp32_Image_Ctrl::Check_Range( uint32_t address, uint32_t length,
											   const NV_Ctrl::mem_range_info_t* mem_range_ref )
{
	/* Length should not exceed MAX_CHUNK_LENGTH
	   Range should not exceed reference memory*/
	fus_op_status_t status = FUS_NO_ERROR;

	if ( ( address < mem_range_ref->start_address ) ||
		 ( address > ( mem_range_ref->end_address + CODE_SIGN_INFO_MAX_SIZE ) ) )
	{
		status = FUS_BAD_ADDRESS_ERROR;
	}
	else if ( ( ( address + length ) > ( mem_range_ref->end_address + 1U + CODE_SIGN_INFO_MAX_SIZE ) ) ||
			  ( length > MAX_CHUNK_SIZE ) )
	{
		status = FUS_BAD_DATA_LENGTH_ERROR;
	}
	else
	{
		// misra
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Update_Version( void )
{
	if ( m_new_image_ver != m_image_config->version )
	{
		m_image_config->version = m_new_image_ver;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Image_Upgrade_Task( CR_Tasker* cr_task )
{
	switch ( m_image.state )
	{
		case IDLE:
			break;

		case ERASE:
			Erase_Handler();
			break;

		case WRITE:
			Write_Handler();
			break;

		case VALIDATE:
			Validate_Handler();
			break;

		case COMMIT:
			Commit_Handler();
			break;

		case ERROR:
			Error_Handler();
			break;

		case RETRY:
			Retry_Handler();
			break;

		case WAIT:
			Wait_Handler();
			break;

		case RESET:
			Reset_Handler();
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
bool Esp32_Image_Ctrl::New_Request( uint32_t address, uint8_t length )
{
	// return( address > m_image.address ) ;
	// Todo: Implementation pending. This functiomn will check that
	// we should receive fresh and increamental address request
	return ( 1 );
}

#ifdef OVERRIDE_FUS_TIME
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Set_Validate_Time( uint32_t time_msec )
{
	if ( time_msec != 0U )
	{
		m_set_validate_time = time_msec;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Esp32_Image_Ctrl::Set_Commit_Time( uint32_t time_msec )
{
	if ( time_msec != 0U )
	{
		m_set_commit_time = time_msec;
	}
}

#endif	// OVERRIDE_FUS_TIME

}
