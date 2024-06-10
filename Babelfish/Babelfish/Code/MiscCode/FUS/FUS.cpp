/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "FUS.h"

namespace BF_FUS
{


FUS* FUS::m_fus_handle = nullptr;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FUS::FUS( Component_Ctrl** component_list, uint8_t component_cnt,
		  prod_config_t* prod_config, event_cb_t FUS_Event_Cb )
	: m_comp_ctrls( component_list ),
	m_num_comp_ctrls( component_cnt ),
	m_prod_config( prod_config ),
	m_fus_event_cb( FUS_Event_Cb ),
	m_session_id( 0U ),
	m_session_logout_time( 0U ),
	m_wait_time( 0U ),
	m_time_out( 0U ),
	m_fus_state( FUS_INIT_READ ),
	m_init_complete( false ),
	m_comp_sel( 0U ),
	m_image_sel( 0U ),
	m_interface_sel( fus_if_t::NO_FUS ),
	m_write_first_chunk( true )
{
	// coverity[leaked_storage]
	new CR_Tasker( &Session_Handler_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "FUS Session Task" );
	m_fus_handle = this;


}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Product_Info( prod_info_t* prod_info )
{
	op_status_t return_status = SUCCESS;

	prod_info->component_count = m_num_comp_ctrls;
	prod_info->fus_version = FUS_VERSION;
	prod_info->boot_version = m_prod_config->boot_version;
	prod_info->boot_spec_version = m_prod_config->boot_spec_version;
	prod_info->guid = PRODUCT_GUID;
	prod_info->code = PRODUCT_CODE;
	prod_info->serial_num = m_prod_config->serial_num;
	prod_info->name = PRODUCT_NAME;
	prod_info->name_len = PRODUCT_NAME_LEN;
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Component( comp_info_req_t* comp_req,
							comp_info_resp_t* comp_resp )
{
	op_status_t return_status = REQUEST_INVALID;

	uint8_t comp_sel = comp_req->comp_id;

	if ( comp_sel < m_num_comp_ctrls )
	{
		if ( FUS_NO_ERROR == m_comp_ctrls[comp_sel]->Get_Comp_Desc( comp_resp ) )
		{
			return_status = SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", comp_sel );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Image( image_info_req_t* image_req, image_info_resp_t* image_resp )
{
	op_status_t return_status = REQUEST_INVALID;
	uint8_t comp_sel = image_req->proc_id;

	if ( comp_sel < m_num_comp_ctrls )
	{
		if ( FUS_NO_ERROR == m_comp_ctrls[comp_sel]->Get_Image_Specs( image_req, image_resp ) )
		{
			return_status = SUCCESS;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", comp_sel );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Goto_FUM( session_info_req_t* session_info, session_info_resp_t* session_resp )
{
	op_status_t return_status = REQUEST_INVALID;
	event_t fus_event;

	m_time_out = session_info->timeout_sec;
	if ( ( Is_Session_Alive() == false ) && ( Is_Init_Complete() == true ) )
	{
		fus_event.event = GOTO_FUM;
		if ( SUCCESS == m_fus_event_cb( &fus_event ) )
		{
			if ( ( 0U != session_info->timeout_sec ) && ( session_info->timeout_sec <= MAX_SESSION_TIMEOUT_SEC ) )
			{
				uint32_t comp_wait_time = SESSION_ZERO_WAIT_TIME_MS;
				session_resp->wait_time = comp_wait_time;
				for ( uint8_t comp_id = 0; comp_id < m_num_comp_ctrls; comp_id++ )
				{
					comp_wait_time = m_comp_ctrls[comp_id]->Goto_FUM();
					if ( comp_wait_time > session_resp->wait_time )
					{
						session_resp->wait_time = comp_wait_time;
					}
				}
				if ( session_resp->wait_time == SESSION_ZERO_WAIT_TIME_MS )
				{
					m_session_logout_time = BF_Lib::Timers::Get_Time() + ( m_time_out * SEC_TO_MS_MULTIPLIER );
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session Create Time : %d ", BF_Lib::Timers::Get_Time() );
					Generate_Session_Id();
					FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session ID : %d ", m_session_id );
					m_wait_time = session_resp->wait_time;
					Set_Interface( session_info->interface );	/* Setting interface after successful session creation
																 */
					fus_event.event = SESSION_CREATED;
					if ( m_fus_state == FUS_IDLE )
					{
						m_fus_state = FUS_SESSION;
					}
					if ( SUCCESS == m_fus_event_cb( &fus_event ) )
					{
						return_status = SUCCESS;
					}
				}
				else
				{
					return_status = SUCCESS;
				}
			}
			else
			{
				FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session timeout is out of range: %d", session_info->timeout_sec );
			}
		}
		else
		{
			return_status = REJECT;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Product Event Callback denied Permission for firmware upgrade" );
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session already exists or FUS init in progress !!!" );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Generate_Session_Id( void )
{
	uint32_t temp32 = 0U;

	while ( 0U == ( temp32 & 0x0fffff ) )
	{
		temp32 = BF_Lib::Timers::Get_Time();
		if ( ( temp32 & 0x0fffff ) > MAX_NUMBER_5_DIGIT )
		{
			temp32 = temp32 & MAX_NUMBER_5_DIGIT;
		}
	}

	m_session_id =
		( temp32 & 0x0fffff ) > MIN_NUMBER_4_DIGIT ? ( temp32 & 0x0fffff ) :
		( temp32 & 0x0fffff ) | ( MIN_NUMBER_4_DIGIT + 1U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Write( image_write_req_t* write_req, image_write_resp_t* write_resp )
{
	op_status_t return_status = REQUEST_INVALID;
	event_t fus_event;

	fus_event.comp_id = write_req->proc_index;
	fus_event.image_id = write_req->image_index;

	if ( write_req->session_id == m_session_id )
	{
		uint8_t comp_sel = write_req->proc_index;
		m_session_logout_time = BF_Lib::Timers::Get_Time() + ( m_time_out * SEC_TO_MS_MULTIPLIER );
		if ( comp_sel < m_num_comp_ctrls )
		{
			uint8_t write_status = m_comp_ctrls[comp_sel]->Write( write_req, write_resp );
			if ( ( FUS_NO_ERROR == write_status ) ||
				 ( FUS_PENDING_CALL_BACK == write_status ) )
			{
				return_status = SUCCESS;
				if ( m_write_first_chunk )
				{
					m_write_first_chunk = false;
					fus_event.event = WRITE_IN_PROGRESS;
					m_fus_event_cb( &fus_event );
				}
			}
			else if ( FUS_ACCESS_TIME_PENDING == write_status )
			{
				return_status = BUSY;
			}
			else if ( FUS_INVALID_MEM_SELECTED_ERROR == write_status )
			{
				return_status = REQUEST_INVALID;
			}
			else
			{
				return_status = FAILURE;
			}
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", comp_sel );
		}
	}
	else
	{
		return_status = REJECT;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Validate( image_validate_req_t* validate_req,
						   image_validate_resp_t* validate_resp )
{
	op_status_t return_status = REQUEST_INVALID;
	event_t fus_event;

	fus_event.comp_id = validate_req->comp_index;
	fus_event.image_id = validate_req->image_index;

	m_session_logout_time = BF_Lib::Timers::Get_Time() + ( m_time_out * 1000 );

	if ( fus_event.comp_id < m_num_comp_ctrls )
	{
		fus_op_status_t validate_status = m_comp_ctrls[fus_event.comp_id]->Validate( validate_req, validate_resp );
		if ( validate_status == FUS_VALID_IMAGE )
		{
			fus_event.event = VALIDATE_COMPLETE;
			if ( SUCCESS == m_fus_event_cb( &fus_event ) )
			{
				return_status = SUCCESS;
			}
		}
		else if ( FUS_PENDING_CALL_BACK == validate_status )
		{
			return_status = SUCCESS;
		}
		else if ( ( validate_status == FUS_INVALID_IMAGE_CHECKSUM_FAILED ) || \
				  ( validate_status == FUS_INVALID_IMAGE_SIGNATURE_FAILED ) || \
				  ( validate_status == FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED ) || \
				  ( validate_status == FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED ) )
		{
			fus_event.event = FUS_OP_ERROR;
			if ( SUCCESS == m_fus_event_cb( &fus_event ) )
			{
				return_status = SUCCESS;
			}
		}
		else if ( FUS_INVALID_MEM_SELECTED_ERROR == validate_status )
		{
			return_status = REQUEST_INVALID;
		}
		else
		{
			return_status = FAILURE;
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", validate_req->comp_index );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Commit( image_commit_req_t* commit_req,
						 image_commit_resp_t* commit_resp )
{
	op_status_t return_status = REQUEST_INVALID;

	if ( commit_req->session_id == m_session_id )
	{
		event_t fus_event;
		fus_event.comp_id = commit_req->comp_id;
		fus_event.image_id = commit_req->image_id;

		m_session_logout_time = BF_Lib::Timers::Get_Time() + ( m_time_out * SEC_TO_MS_MULTIPLIER );
		if ( fus_event.comp_id < m_num_comp_ctrls )
		{
			fus_op_status_t commit_status = m_comp_ctrls[fus_event.comp_id]->Commit( commit_req, commit_resp );
			if ( ( FUS_NO_ERROR == commit_status ) || ( FUS_PENDING_CALL_BACK == commit_status ) )
			{
				return_status = SUCCESS;
			}
			else
			{
				return_status = FAILURE;
			}
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", commit_req->comp_id );
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session ID mismatch in Commit !!!" );
		return_status = REJECT;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Status( image_status_req_t* status_req, image_status_resp_t* status_resp )
{
	op_status_t return_status = REQUEST_INVALID;
	event_t fus_event;

	fus_event.comp_id = status_req->proc_index;
	fus_event.image_id = status_req->image_index;

	if ( fus_event.comp_id < m_num_comp_ctrls )
	{
		m_session_logout_time = ( BF_Lib::Timers::Get_Time() ) + ( m_time_out * SEC_TO_MS_MULTIPLIER );
		fus_op_status_t status = m_comp_ctrls[fus_event.comp_id]->Status( status_req, status_resp );
		if ( FUS_NO_ERROR == status )
		{
			if ( status_resp->status == FUS_COMMIT_DONE )
			{
				fus_event.event = COMMIT_COMPLETE;
				if ( SUCCESS == m_fus_event_cb( &fus_event ) )
				{
					return_status = SUCCESS;
				}
				else
				{
					return_status = SUCCESS;
					status_resp->status = FUS_PREVIOUS_CMD_ERROR;
				}
			}
		}
		else if ( FUS_PENDING_CALL_BACK == status )
		{
			return_status = SUCCESS;
		}
		else if ( FUS_INVALID_MEM_SELECTED_ERROR == status )
		{
			return_status = REQUEST_INVALID;
		}
		else
		{
			fus_event.event = FUS_OP_ERROR;
			if ( SUCCESS == m_fus_event_cb( &fus_event ) )
			{
				return_status = SUCCESS;
			}
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Selected Component is not available : %d", status_req->proc_index );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Exit_FUM( session_info_req_t* session_info, session_info_resp_t* session_resp )
{
	op_status_t return_status = REQUEST_INVALID;

	if ( m_session_id == session_info->session_id )
	{
		Delete_Session();
		session_resp->wait_time = SESSION_ZERO_WAIT_TIME_MS;
		return_status = SUCCESS;
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session ID mismatch !!!" );
	}
	return ( return_status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
op_status_t FUS::Read( image_read_req_t* read_mem, image_read_resp_t* read_mem_resp )
{
	op_status_t return_status = REQUEST_INVALID;

	// uint8_t comp_sel = read_mem->proc_id;
	//
	// if ( comp_sel < m_num_comp_ctrls )
	// {
	// return_status = m_proc_ctrls[comp_sel]->Read_Mem( read_mem, read_mem_resp );
	// }
	//
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FUS* FUS::Get_FUS_Handle_Static()
{
	return ( FUS::m_fus_handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Delete_Session( void )
{
	event_t fus_event;
	uint8_t comp_sel;

	for ( comp_sel = 0U; comp_sel < m_num_comp_ctrls; comp_sel++ )
	{
		m_comp_ctrls[comp_sel]->Exit_FUM();
	}
	m_fus_state = FUS_IDLE;
	fus_event.event = SESSION_DELETED;
	if ( SUCCESS == m_fus_event_cb( &fus_event ) )
	{
		/* Clear the selected interface for FUS */
		Clear_Interface();
		m_write_first_chunk = true;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

bool FUS::Is_Session_Alive( void )
{
	bool return_status = false;

	if ( m_fus_state == FUS_SESSION )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint32_t FUS::Get_Session_Id( void )
{
	return ( m_session_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t FUS::Get_Component_Count( void )
{
	return ( m_num_comp_ctrls );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FUS::Is_Init_Complete( void )
{
	bool status = true;

	for ( uint8_t comp_sel = 0; comp_sel < m_num_comp_ctrls; comp_sel++ )
	{
		uint8_t scanned_images = BF_Lib::Bit::Total_Highs( m_image_data[m_comp_sel].checkbox );
		if ( scanned_images < m_image_data[comp_sel].count )
		{
			/* FUS init is still in-process */
			status = false;
			break;
		}
		else
		{
			comp_sel++;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Init_Read_Handler( void )
{
	uint8_t count = 0;
	fus_op_status_t status = m_comp_ctrls[m_comp_sel]->Get_Image_Count( &count );

	if ( FUS_NO_ERROR == status )
	{
		m_image_data[m_comp_sel].count = count;
		m_image_data[m_comp_sel].data = 0;
		m_image_data[m_comp_sel].checkbox = 0;

		m_comp_sel++;
		if ( m_comp_sel >= m_num_comp_ctrls )
		{
			m_comp_sel = 0;
			m_fus_state = FUS_INIT;
		}
	}
	else if ( FUS_PENDING_CALL_BACK == status )
	{
		// Waiting for component to reply
	}
	else
	{
		// ToDO: Error condition
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Init_Handler( void )
{
	image_validate_req_t validate_req;
	image_validate_resp_t validate_resp;

	event_t fus_event;

	validate_req.comp_index = m_comp_sel;
	validate_req.image_index = m_image_sel;
	validate_resp.status = FUS_NO_ERROR;

	fus_event.comp_id = m_comp_sel;
	fus_event.image_id = m_image_sel;

	fus_op_status_t status =
		m_comp_ctrls[m_comp_sel]->Validate( &validate_req, &validate_resp );

	if ( FUS_VALID_IMAGE == status )
	{
		BF_Lib::Bit::Set( m_image_data[m_comp_sel].data, m_image_sel );
		BF_Lib::Bit::Set( m_image_data[m_comp_sel].checkbox, m_image_sel );
		fus_event.event = INIT_VALID;
		if ( SUCCESS == m_fus_event_cb( &fus_event ) )
		{}
		m_image_sel++;
	}
	else if ( ( FUS_INVALID_IMAGE_CHECKSUM_FAILED == validate_resp.status ) || \
			  ( FUS_INVALID_IMAGE_SIGNATURE_FAILED == validate_resp.status ) || \
			  ( FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED == validate_resp.status ) || \
			  ( FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED == validate_resp.status ) )
	{
		BF_Lib::Bit::Clr( m_image_data[m_comp_sel].data, m_image_sel );
		BF_Lib::Bit::Set( m_image_data[m_comp_sel].checkbox, m_image_sel );
		fus_event.event = INIT_INVALID;
		if ( SUCCESS == m_fus_event_cb( &fus_event ) )
		{}
		m_image_sel++;
	}
	else if ( FUS_PENDING_CALL_BACK == status )
	{
		// Image validation is in-process
	}
	else
	{
		// Internal operation error condition
		fus_event.event = FUS_OP_ERROR;
		if ( SUCCESS == m_fus_event_cb( &fus_event ) )
		{
			m_comp_sel = 0;
			m_image_sel = 0;
		}
	}

	if ( m_image_sel >= m_image_data[m_comp_sel].count )
	{
		m_image_sel = 0;
		m_comp_sel++;
		if ( m_comp_sel >= m_num_comp_ctrls )
		{
			m_comp_sel = 0;
			m_fus_state = FUS_IDLE;
			FUS_DEBUG_PRINT( DBG_MSG_INFO, " Init validation of all image completed " );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Session_Handler( void )
{
	if ( BF_Lib::Timers::Get_Time() > m_session_logout_time )
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Session Timeout::: Session Delete Time : %d ",
						 BF_Lib::Timers::Get_Time() );
		m_session_logout_time = 0U;
		Delete_Session();
		m_fus_state = FUS_IDLE;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Fus_Operation_Handler( void )
{
	switch ( m_fus_state )
	{
		case FUS_IDLE:
			break;

		case FUS_INIT_READ:
			Init_Read_Handler();
			break;

		case FUS_INIT:
			Init_Handler();
			break;

		case FUS_SESSION:
			Session_Handler();
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
void FUS::Session_Handler_Task( CR_Tasker* cr_task )
{
	CR_Tasker_Begin( cr_task );
	for (;;)
	{
		Fus_Operation_Handler();
		// Sleep task
		CR_Tasker_Delay( cr_task, FUS_INIT_OP_POLLING_TIME_MS );
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Set_Interface( fus_if_t selected_interface )
{
	m_interface_sel = selected_interface;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FUS::Clear_Interface( void )
{
	m_interface_sel = fus_if_t::NO_FUS;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_if_t FUS::Get_Interface( void )
{
	return ( m_interface_sel );
}

}
