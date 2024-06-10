/*
 *****************************************************************************************
 * @file
 * @details BLE-FUS interface file help to get the execute BLE operation.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "BLE_FUS.h"
#include "StdLib_MV.h"
#include "flash_if.h"
#include "FUS.h"
#include "FUS_Config.h"
#include "Babelfish_Assert.h"
#include "FUS_Debug.h"
#include "FUS_Defines.h"

/*
 *****************************************************************************************
 *		Functions
 *****************************************************************************************
 */
// ------------------------ Getting the product related details ---------------------------
static ble_status_t Product_Detail_Cb( prod_info_req_t* prod_req, prod_resp_t* prod_resp );

// ----------------------- Getting the Processor related details --------------------------
static ble_status_t Processor_Detail_Cb( proc_info_req_t* proc_req, proc_resp_t* firm_resp );

// ----------------------- Getting the Firmware related details ---------------------------
static ble_status_t Firmware_Detail_Cb( firm_info_req_t* firm_req, firm_resp_t* firm_resp );

////////////Firmware upgrade session functions //////////////////////
// --------------------- Session creation for the firmware upgrade ------------------------
static ble_status_t Create_Session_Cb( session_info_req_t* session_req, session_resp_t* session_resp );

// ---------------------- Session Deletion for the firmware upgrade -----------------------
static ble_status_t Delete_Session_Cb( session_exit_req_t* session_req, session_exit_t* session_resp );

// ---------------- Data write functionality for the firmware upgrade ---------------------
static ble_status_t Data_Write_Cb( write_req_t* write_req, write_resp_t* write_resp );

// ---------------- Image Validation function for the firmware upgrade --------------------
static ble_status_t Data_Validate_Cb( image_validate_req_t* ble_validate_req, image_validate_t* ble_validate_resp );

// ----------------------------- Image copying function -----------------------------------
static ble_status_t Data_Commit_Cb( commit_req_t* commit_req, image_commit_t* commit_resp );

// ------------------- Status function to check the commit operation ----------------------
static ble_status_t Data_Status_Cb( image_status_check_req_t* ble_status_check_req,
									image_status_check_t* ble_status_check_resp );

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */

static BLE_Stack_Interface* ble_stack_interface_handler = nullptr;
static const DCI_Owner* ble_resp_frame = nullptr;

// Variable used for tracking duplicate write chunks
static uint32_t prev_recv_add = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Product_Detail_Cb( prod_info_req_t* prod_req, prod_resp_t* prod_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	prod_resp->prod_info.resp_byte.cmd_enum = prod_req->funct_cmd;
	prod_resp->prod_info.resp_byte.resp_bit = SET;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	if ( fus_handle->Is_Session_Alive() == true )
	{
		status = BLE_IN_PROGRESS;
		prod_resp->err_frame.resp_byte.err_bit = SET;
		prod_resp->err_frame.resp_byte.wait_bit = RESET;
		prod_resp->err_frame.err_code = SESSION_IN_PROGRESS;
	}
	else
	{
		BF_FUS::prod_info_t prod_info;
		uint8_t prod_name[PROD_NAME_MAX_LENGTH] = "";
		prod_info.name = prod_name;

		if ( BF_FUS::SUCCESS == fus_handle->Product_Info( &prod_info ) )
		{
			status = BLE_SUCCESS;
			prod_resp->prod_info.resp_byte.err_bit = RESET;
			prod_resp->prod_info.fus_protcol_ver = CONST_BLE_FUS_PROTOCOL_VER;
			prod_resp->prod_info.firm_ver =
				reinterpret_cast<BF_FUS::img_fw_version_t*>( static_cast<uint32_t>( prod_info.fus_version ) );
			prod_resp->prod_info.comp_cnt = prod_info.component_count;
			prod_resp->prod_info.unique_identifier_type = FOUR_BYTE;
			prod_resp->prod_info.unique_identifier = static_cast<uint32_t>( prod_info.guid );
			BF_Lib::Copy_Null_String( prod_resp->prod_info.name, prod_info.name, prod_info.name_len );
		}
		else
		{
			status = BLE_ERROR;
			prod_resp->err_frame.resp_byte.err_bit = SET;
			prod_resp->err_frame.resp_byte.wait_bit = RESET;
			prod_resp->err_frame.err_code = REQUEST_INVALID;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Processor_Detail_Cb( proc_info_req_t* proc_req, proc_resp_t* proc_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	proc_resp->proc_info.resp_byte.cmd_enum = proc_req->funct_cmd;
	proc_resp->proc_info.resp_byte.resp_bit = SET;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	if ( fus_handle->Is_Session_Alive() == true )
	{
		status = BLE_IN_PROGRESS;
		proc_resp->err_frame.resp_byte.err_bit = SET;
		proc_resp->err_frame.resp_byte.wait_bit = RESET;
		proc_resp->err_frame.err_code = SESSION_IN_PROGRESS;
	}
	else
	{
		BF_FUS::comp_info_req_t comp_req;
		comp_req.comp_id = proc_req->proc_id;
		BF_FUS::comp_info_resp_t comp_resp = { 0U };
		uint8_t proc_name[COMP_NAME_MAX_LENGTH] = "";
		comp_resp.name = proc_name;

		if ( BF_FUS::SUCCESS == fus_handle->Component( &comp_req, &comp_resp ) )
		{
			status = BLE_SUCCESS;
			proc_resp->proc_info.resp_byte.err_bit = RESET;
			proc_resp->proc_info.proc_id = comp_req.comp_id;
			proc_resp->proc_info.image_cnt = comp_resp.image_count;
			proc_resp->proc_info.firm_ver = reinterpret_cast<BF_FUS::img_fw_version_t*>( comp_resp.firmware_version );
			proc_resp->proc_info.hardware_ver =
				reinterpret_cast<BF_FUS::img_fw_version_t*>( comp_resp.hardware_version );
			proc_resp->proc_info.unique_identifier_type = FOUR_BYTE;
			proc_resp->proc_info.unique_identifier = static_cast<uint32_t>( comp_resp.guid );
			BF_Lib::Copy_Null_String( proc_resp->proc_info.name, comp_resp.name, comp_resp.name_len );
		}
		else
		{
			status = BLE_ERROR;
			proc_resp->err_frame.resp_byte.err_bit = SET;
			proc_resp->err_frame.resp_byte.wait_bit = RESET;
			proc_resp->err_frame.err_code = REQUEST_INVALID;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Firmware_Detail_Cb( firm_info_req_t* firm_req, firm_resp_t* firm_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	firm_resp->firm_info.resp_byte.cmd_enum = firm_req->funct_cmd;
	firm_resp->firm_info.resp_byte.resp_bit = SET;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	if ( fus_handle->Is_Session_Alive() == true )
	{
		status = BLE_IN_PROGRESS;
		firm_resp->err_frame.resp_byte.err_bit = SET;
		firm_resp->err_frame.resp_byte.wait_bit = RESET;
		firm_resp->err_frame.err_code = SESSION_IN_PROGRESS;
	}
	else
	{
		BF_FUS::image_info_req_t image_req;
		image_req.proc_id = firm_req->comp_id;
		image_req.image_id = firm_req->image_id;
		BF_FUS::image_info_resp_t image_resp;
		uint8_t img_name[IMAGE_NAME_MAX_LENGTH] = "";
		image_resp.name = img_name;
		if ( BF_FUS::SUCCESS == fus_handle->FUS::Image( &image_req, &image_resp ) )
		{
			status = BLE_SUCCESS;
			firm_resp->firm_info.resp_byte.err_bit = RESET;
			firm_resp->firm_info.comp_id = image_req.proc_id;
			firm_resp->firm_info.image_id = image_req.image_id;
			firm_resp->firm_info.firm_ver = reinterpret_cast<BF_FUS::img_fw_version_t*>( image_resp.version );
			firm_resp->firm_info.compatibility_num = image_resp.compatibility_num;
			firm_resp->firm_info.buffer_size = MAX_BLE_FUS_DATA_SIZE;
			firm_resp->firm_info.unique_identifier_type = FOUR_BYTE;
			firm_resp->firm_info.unique_identifier = static_cast<uint32_t>( image_resp.guid );
			BF_Lib::Copy_Null_String( firm_resp->firm_info.name, image_resp.name, image_resp.name_len );
		}
		else
		{
			status = BLE_ERROR;
			firm_resp->err_frame.resp_byte.err_bit = SET;
			firm_resp->err_frame.resp_byte.wait_bit = RESET;
			firm_resp->err_frame.err_code = REQUEST_INVALID;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Create_Session_Cb( session_info_req_t* session_req, session_resp_t* session_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::session_info_resp_t fus_session_resp = { 0U };
	BF_FUS::session_info_req_t fus_session_req = { 0U };
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	fus_session_req.timeout_sec = session_req->timeout_time;
	session_resp->session_info_resp.resp_byte.cmd_enum = session_req->funct_cmd;
	session_resp->session_info_resp.resp_byte.resp_bit = SET;
	session_resp->err_frame.resp_byte.err_bit = SET;
	session_resp->err_frame.resp_byte.wait_bit = RESET;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( fus_handle->Is_Session_Alive() == true )
	{
		session_resp->err_frame.err_code = SESSION_ALREADY_EXISTS;
	}
	else
	{
		if ( session_req->encoding_type == NO_ENCODE )
		{
			uint32_t ble_session_id = 0U;
			return_status = fus_handle->Goto_FUM( &fus_session_req, &fus_session_resp );
			if ( BF_FUS::SUCCESS == return_status )
			{
				// Reset duplicate address checker
				prev_recv_add = 0U;

				status = BLE_SUCCESS;
				session_resp->session_info_resp.resp_byte.err_bit = RESET;
				if ( fus_session_resp.wait_time > 0U )
				{
					session_resp->session_info_resp.resp_byte.wait_bit = SET;
					session_resp->session_info_resp.wait_time = fus_session_resp.wait_time;
				}
				else
				{
					session_resp->session_info_resp.resp_byte.wait_bit = RESET;
				}
				ble_session_id = fus_handle->Get_Session_Id();
				// BLE Protocol interface using uint8_t format, so using typecast 4 byte session id converted into 1
				// byte
				session_resp->session_info_resp.session_id = static_cast<uint8_t>( ble_session_id );
			}
			else if ( BF_FUS::REJECT == return_status )
			{
				status = BLE_REJECT;
				session_resp->err_frame.err_code = ACCESS_DENIED;
			}
			else if ( BF_FUS::REQUEST_INVALID == return_status )
			{
				status = BLE_INVALID;
				session_resp->err_frame.err_code = INVALID_TIMEOUT;
			}
			else
			{
				status = BLE_ERROR;
				session_resp->err_frame.err_code = SESSION_IN_PROGRESS;
			}
		}
		else
		{
			status = BLE_ERROR;
			session_resp->err_frame.err_code = INVALID_ENCODING_TYPE;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Delete_Session_Cb( session_exit_req_t* session_req, session_exit_t* session_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	session_resp->session_exit.resp_byte.cmd_enum = session_req->funct_cmd;
	session_resp->session_exit.resp_byte.resp_bit = SET;
	session_resp->err_frame.resp_byte.err_bit = SET;
	session_resp->err_frame.resp_byte.wait_bit = RESET;
	session_resp->err_frame.err_code = SESSION_ID_MISMATCH;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	BF_FUS::session_info_resp_t fus_session_resp = { 0U };
	BF_FUS::session_info_req_t fus_session_req = { 0U };
	BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;

	uint32_t ble_session_id = fus_handle->Get_Session_Id();

	if ( session_req->session_id == static_cast<uint8_t>( ( ble_session_id ) & 0xFF ) )
	{
		fus_session_req.session_id = ble_session_id;


		return_status = fus_handle->Exit_FUM( &fus_session_req, &fus_session_resp );
		if ( BF_FUS::SUCCESS == return_status )
		{
			status = BLE_SUCCESS;
			session_resp->session_exit.resp_byte.err_bit = RESET;
			if ( fus_session_resp.wait_time > 0 )
			{
				session_resp->session_exit.resp_byte.wait_bit = SET;
				session_resp->session_exit.wait_time = fus_session_resp.wait_time;
			}
			else
			{
				session_resp->session_exit.resp_byte.wait_bit = RESET;
			}
		}
		else if ( BF_FUS::REQUEST_INVALID == return_status )
		{
			status = BLE_INVALID;
			session_resp->err_frame.err_code = SESSION_ID_MISMATCH;
		}
		else
		{
			status = BLE_ERROR;
			session_resp->err_frame.err_code = BAD_COMMAND;
		}
	}
	else
	{
		status = BLE_INVALID;
		session_resp->err_frame.err_code = SESSION_ID_MISMATCH;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

ble_status_t Data_Write_Cb( write_req_t* write_req, write_resp_t* write_resp )
{
	ble_status_t status = BLE_SUCCESS;
	BF_FUS::FUS* fus_handle = nullptr;

	write_resp->ble_write_resp.resp_byte.cmd_enum = write_req->funct_cmd;
	write_resp->ble_write_resp.resp_byte.resp_bit = SET;
	write_resp->err_frame.resp_byte.err_bit = SET;
	write_resp->err_frame.resp_byte.wait_bit = RESET;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( fus_handle->Is_Session_Alive() )
	{
		uint32_t ble_session_id = fus_handle->Get_Session_Id();

		if ( write_req->session_id == static_cast<uint8_t>( ( ble_session_id ) & 0xFF ) )
		{

			BF_FUS::image_write_req_t write_image_in_mem = { 0U };
			BF_FUS::image_write_resp_t write_image_resp = { 0U };
			write_image_in_mem.proc_index = write_req->comp_id;
			write_image_in_mem.image_index = write_req->image_id;
			write_image_in_mem.session_id = ble_session_id;
			write_image_in_mem.address = write_req->address;
			write_image_in_mem.data_len = write_req->data_len;
			write_image_in_mem.data = write_req->data;

			if ( write_image_in_mem.data_len <= MAX_BLE_FUS_DATA_SIZE )
			{
				if ( write_image_in_mem.address > prev_recv_add )
				{
					BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;
					return_status = fus_handle->Write( &write_image_in_mem, &write_image_resp );
					if ( BF_FUS::SUCCESS == return_status )
					{
						status = BLE_SUCCESS;

						// FUS should not receive duplicate write requests
						prev_recv_add = write_image_in_mem.address + write_image_in_mem.data_len - 1U;

						write_resp->ble_write_resp.resp_byte.err_bit = RESET;
						write_resp->ble_write_resp.session_id = write_req->session_id;
						write_resp->ble_write_resp.comp_id = write_image_in_mem.proc_index;
						write_resp->ble_write_resp.image_id = write_image_in_mem.image_index;
						write_resp->ble_write_resp.address = write_image_in_mem.address;
						write_resp->ble_write_resp.data_len = write_image_in_mem.data_len;
						if ( write_image_resp.op_time > 0 )
						{
							write_resp->ble_write_resp.resp_byte.wait_bit = SET;
							write_resp->ble_write_resp.wait_time = write_image_resp.op_time;
						}
						else
						{
							write_resp->ble_write_resp.resp_byte.wait_bit = RESET;
						}
					}
					else if ( BF_FUS::REJECT == return_status )
					{
						status = BLE_REJECT;
						write_resp->err_frame.err_code = SESSION_ID_MISMATCH;
					}
					else if ( BF_FUS::REQUEST_INVALID == return_status )
					{
						status = BLE_INVALID;
						write_resp->err_frame.err_code = REQUEST_INVALID;
					}
					else if ( BF_FUS::BUSY == return_status )
					{
						status = BLE_BUSY;
						write_resp->err_frame.err_code = DEVICE_BUSY;
					}
					else
					{
						status = BLE_ERROR;
						write_resp->err_frame.err_code = DATA_WRITE_FAILS;
					}
				}
				else
				{
					status = BLE_ERROR;
					prev_recv_add = 0U;
					write_resp->err_frame.err_code = DUPLICATE_WRITE_CHUNK;
				}
			}
			else
			{
				status = BLE_ERROR;
				write_resp->err_frame.err_code = INVALID_DATA_LENGTH;
			}
		}
		else
		{
			status = BLE_INVALID;
			write_resp->err_frame.err_code = SESSION_ID_MISMATCH;
		}
	}
	else
	{
		status = BLE_ERROR;
		write_resp->err_frame.err_code = SESSION_DOES_NOT_EXISTS;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

ble_status_t Data_Validate_Cb( image_validate_req_t* ble_validate_req, image_validate_t* ble_validate_resp )
{
	ble_validate_resp->image_validate.resp_byte.cmd_enum = ble_validate_req->funct_cmd;
	ble_validate_resp->image_validate.resp_byte.resp_bit = SET;
	ble_validate_resp->err_frame.resp_byte.err_bit = SET;
	ble_validate_resp->err_frame.resp_byte.wait_bit = RESET;
	ble_status_t status = BLE_SUCCESS;

	BF_FUS::FUS* fus_handle = nullptr;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	uint32_t ble_session_id = fus_handle->Get_Session_Id();

	if ( ble_validate_req->session_id == static_cast<uint8_t>( ( ble_session_id ) & 0xFF ) )
	{
		BF_FUS::op_status_t return_status = BF_FUS::SUCCESS;
		BF_FUS::image_validate_req_t validate_req = { 0U };
		BF_FUS::image_validate_resp_t validate_resp = { 0U };
		validate_req.comp_index = ble_validate_req->comp_index;
		validate_req.image_index = ble_validate_req->image_index;
		return_status = fus_handle->Validate( &validate_req, &validate_resp );

		if ( BF_FUS::SUCCESS == return_status )
		{
			status = BLE_SUCCESS;
			ble_validate_resp->image_validate.resp_byte.err_bit = RESET;
			if ( validate_resp.op_time > 0U )
			{
				ble_validate_resp->image_validate.resp_byte.wait_bit = SET;
				ble_validate_resp->image_validate.op_time = validate_resp.op_time;
			}
			else
			{
				ble_validate_resp->image_validate.resp_byte.wait_bit = RESET;
			}
			ble_validate_resp->image_validate.comp_index = ble_validate_req->comp_index;
			ble_validate_resp->image_validate.image_index = ble_validate_req->image_index;

			if ( validate_resp.status == BF_FUS::FUS_VALIDATION_IN_PROGRESS )
			{
				ble_validate_resp->image_validate.status = IMAGE_VALIDATION_IN_PROCESS;
			}
			else if ( validate_resp.status == BF_FUS::FUS_VALID_IMAGE )
			{
				ble_validate_resp->image_validate.status = IMAGE_VALID;
			}
			else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_CHECKSUM_FAILED )
			{
				ble_validate_resp->image_validate.status = IMAGE_INVALID;
			}
			else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_SIGNATURE_FAILED )
			{
				ble_validate_resp->image_validate.status = IMAGE_INVALID;
			}
			else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED )
			{
				ble_validate_resp->image_validate.status = IMAGE_INVALID;
			}
			else if ( validate_resp.status == BF_FUS::FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED )
			{
				ble_validate_resp->image_validate.status = IMAGE_INVALID;
			}
		}
		else if ( BF_FUS::REQUEST_INVALID == return_status )
		{
			status = BLE_INVALID;
			ble_validate_resp->err_frame.err_code = REQUEST_INVALID;
		}
		else
		{
			status = BLE_ERROR;
			ble_validate_resp->err_frame.err_code = BAD_COMMAND;
		}
	}
	else
	{
		status = BLE_ERROR;
		ble_validate_resp->err_frame.err_code = SESSION_ID_MISMATCH;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

ble_status_t Data_Commit_Cb( commit_req_t* ble_commit_req, image_commit_t* ble_commit_resp )
{
	ble_status_t status = BLE_ERROR;
	BF_FUS::op_status_t fus_return_status;
	BF_FUS::image_commit_resp_t fus_commit_resp = { 0U };
	BF_FUS::image_commit_req_t fus_commit_req = { 0U };
	BF_FUS::FUS* fus_handle = nullptr;

	// Get the FUS handle
	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	// fill up the response packet
	ble_commit_resp->image_commit.resp_byte.cmd_enum = ble_commit_req->funct_cmd;
	ble_commit_resp->image_commit.resp_byte.resp_bit = SET;
	ble_commit_resp->err_frame.resp_byte.err_bit = SET;
	ble_commit_resp->err_frame.resp_byte.wait_bit = RESET;

	uint32_t ble_session_id = fus_handle->Get_Session_Id();

	if ( ble_commit_req->session_id == static_cast<uint8_t>( ( ble_session_id ) & 0xFF ) )
	{
		fus_commit_req.session_id = ble_session_id;
		fus_commit_req.comp_id = ble_commit_req->comp_index;
		fus_commit_req.image_id = ble_commit_req->image_index;

		// Execute FUS command
		fus_return_status = fus_handle->Commit( &fus_commit_req, &fus_commit_resp );

		if ( BF_FUS::SUCCESS == fus_return_status )
		{
			status = BLE_SUCCESS;
			ble_commit_resp->image_commit.comp_index = ble_commit_req->comp_index;
			ble_commit_resp->image_commit.image_index = ble_commit_req->image_index;
			ble_commit_resp->image_commit.resp_byte.err_bit = RESET;
			if ( fus_commit_resp.op_time > 0U )
			{
				ble_commit_resp->image_commit.resp_byte.wait_bit = SET;
				ble_commit_resp->image_commit.status = COMMIT_IN_PROCESS;
				if ( fus_commit_req.image_id == 0U )
				{
					ble_commit_resp->image_commit.op_time = APP_IMAGE_COPY_TIME_MS;
				}
				else
				{
					ble_commit_resp->image_commit.op_time = fus_commit_resp.op_time;
				}
			}
			else
			{
				ble_commit_resp->image_commit.resp_byte.wait_bit = RESET;
				ble_commit_resp->image_commit.status = COMMIT_SUCCESSFUL;
			}
		}
		else if ( BF_FUS::REJECT == fus_return_status )
		{
			status = BLE_IN_PROGRESS;
			ble_commit_resp->err_frame.err_code = SESSION_ID_MISMATCH;
		}
		else if ( ( BF_FUS::REQUEST_INVALID == fus_return_status ) || ( BF_FUS::FAILURE == fus_return_status ) )
		{
			status = BLE_INVALID;
			ble_commit_resp->err_frame.err_code = REQUEST_INVALID;
		}
		else
		{
			status = BLE_ERROR;
			ble_commit_resp->err_frame.err_code = BAD_COMMAND;
		}
	}
	else
	{
		status = BLE_ERROR;
		ble_commit_resp->err_frame.err_code = SESSION_ID_MISMATCH;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ble_status_t Data_Status_Cb( image_status_check_req_t* ble_status_check_req,
							 image_status_check_t* ble_status_check_resp )
{
	ble_status_t status = BLE_ERROR;
	BF_FUS::op_status_t fus_return_status;
	BF_FUS::image_status_resp_t fus_status_resp = { 0U };
	BF_FUS::image_status_req_t fus_status_req = { 0U };
	BF_FUS::FUS* fus_handle = nullptr;

	// Fill up the response packet
	ble_status_check_resp->image_status_check.resp_byte.cmd_enum = ble_status_check_req->funct_cmd;
	ble_status_check_resp->image_status_check.resp_byte.resp_bit = SET;
	ble_status_check_resp->err_frame.resp_byte.err_bit = SET;
	ble_status_check_resp->err_frame.resp_byte.wait_bit = RESET;

	// Get the FUS handle
	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();

	if ( fus_handle->Is_Session_Alive() )
	{
		uint32_t ble_session_id = fus_handle->Get_Session_Id();
		if ( ble_status_check_req->session_id == static_cast<uint8_t>( ( ble_session_id ) & 0xFF ) )
		{
			fus_status_req.proc_index = ble_status_check_req->comp_index;
			fus_status_req.image_index = ble_status_check_req->image_index;

			// Execute FUS command
			fus_return_status = fus_handle->Status( &fus_status_req, &fus_status_resp );

			if ( BF_FUS::SUCCESS == fus_return_status )
			{
				ble_status_check_resp->image_status_check.status = fus_status_resp.status;
				status = BLE_SUCCESS;
				if ( fus_status_resp.status == BF_FUS::FUS_COMMIT_DONE )
				{
					ble_status_check_resp->image_status_check.comp_index = ble_status_check_req->comp_index;
					ble_status_check_resp->image_status_check.image_index = ble_status_check_req->image_index;
					ble_status_check_resp->image_status_check.resp_byte.err_bit = RESET;
					if ( fus_status_resp.remaining_op_time > 0U )
					{
						ble_status_check_resp->image_status_check.resp_byte.wait_bit = SET;
						ble_status_check_resp->image_status_check.op_time = fus_status_resp.remaining_op_time;
					}
					else
					{
						ble_status_check_resp->image_status_check.resp_byte.wait_bit = RESET;
					}
				}
				else if ( fus_status_resp.status == BF_FUS::FUS_PENDING_CALL_BACK )
				{
					ble_status_check_resp->image_status_check.resp_byte.err_bit = RESET;
					ble_status_check_resp->image_status_check.resp_byte.wait_bit = RESET;
					ble_status_check_resp->image_status_check.comp_index = ble_status_check_req->comp_index;
					ble_status_check_resp->image_status_check.image_index = ble_status_check_req->image_index;
				}
				else
				{
					ble_status_check_resp->err_frame.err_code = COMMIT_FAIL;
				}
			}
			else if ( BF_FUS::REQUEST_INVALID == fus_return_status )
			{
				status = BLE_INVALID;
				ble_status_check_resp->err_frame.err_code = REQUEST_INVALID;
			}
			else
			{
				status = BLE_ERROR;
				ble_status_check_resp->err_frame.err_code = BAD_COMMAND;
			}
		}
		else
		{
			ble_status_check_resp->err_frame.err_code = SESSION_ID_MISMATCH;
		}
	}
	else
	{
		status = BLE_ERROR;
		ble_status_check_resp->err_frame.err_code = SESSION_DOES_NOT_EXISTS;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Clear_Bootup_validation_Data( void )
{
	BF_FUS::FUS* fus_handle = nullptr;

	fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	fus_handle->Delete_Session();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_BLE_Stack_Interface_Handle( BLE_Stack_Interface* handle, const DCI_Owner* ble_fus_resp_frame )
{
	ble_resp_frame = ble_fus_resp_frame;
	ble_stack_interface_handler = handle;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD BLE_FUS_Event( DCI_CBACK_PARAM_TD handle, DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	switch ( access_struct->command )
	{
		case DCI_ACCESS_SET_RAM_CMD:
			BLE_FUS_Interface_Request_Handler( ( uint8_t* )access_struct->data_access.data,
											   access_struct->data_access.length );
			break;

		default:
			return_status = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_FUS_Interface_Request_Handler( uint8_t* req_data, uint8_t req_data_len )
{
	uint8_t data_resp[MAX_BLE_RESPONSE_LENGTH] = { 0U };
	uint8_t cmd_index = 0U;
	uint8_t resp_length = 0U;
	prod_info_req_t* prod_req = nullptr;
	prod_resp_t* prod_resp = nullptr;
	proc_info_req_t* proc_req = nullptr;
	proc_resp_t* proc_resp = nullptr;
	firm_info_req_t* firm_req = nullptr;
	firm_resp_t* firm_resp = nullptr;
	session_info_req_t* session_req = nullptr;
	session_resp_t* session_resp = nullptr;
	session_exit_req_t* session_exit_req = nullptr;
	session_exit_t* session_exit_resp = nullptr;
	write_req_t* write_req = nullptr;
	write_resp_t* write_resp = nullptr;
	image_validate_req_t* validate_req = nullptr;
	image_validate_t* validate_resp = nullptr;
	commit_req_t* commit_req = nullptr;
	image_commit_t* commit_resp = nullptr;
	image_status_check_req_t* status_check_req = nullptr;
	image_status_check_t* status_check_resp = nullptr;

	ble_status_t status_msg = BLE_SUCCESS;

	/* Example code for response back to client */
	if ( ( req_data[cmd_index] == NO_OPTION ) && ( req_data[cmd_index] >= MAX_CAMMAND ) )
	{
		// ToDo - send error response
		// status_msg = BLE_INVALID;
		// err_code.response_code = BAD_COMMAND;
	}
	else
	{
		switch ( req_data[cmd_index] )
		{
			case PRODUCT_INFO:
				prod_req = reinterpret_cast<prod_info_req_t*>( req_data );
				prod_resp = reinterpret_cast<prod_resp_t*>( data_resp );

				status_msg = Product_Detail_Cb( prod_req, prod_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( prod_resp ), resp_length );
				break;

			case COMPONENT_INFO:

				proc_req = reinterpret_cast<proc_info_req_t*>( req_data );
				proc_resp = reinterpret_cast<proc_resp_t*>( data_resp );
				status_msg = Processor_Detail_Cb( proc_req, proc_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( proc_resp ), resp_length );

				break;

			case IMAGE_INFO:
				firm_req = reinterpret_cast<firm_info_req_t*>( req_data );
				firm_resp = reinterpret_cast<firm_resp_t*>( data_resp );
				status_msg = Firmware_Detail_Cb( firm_req, firm_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( firm_resp ), resp_length );

				break;

			case CREATE_SESSION:

				session_req = reinterpret_cast<session_info_req_t*>( req_data );
				session_resp = reinterpret_cast<session_resp_t*>( data_resp );
				status_msg = Create_Session_Cb( session_req, session_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( session_resp ), resp_length );

				break;

			case DELETE_SESSION:

				session_exit_req = reinterpret_cast<session_exit_req_t*>( req_data );
				session_exit_resp = reinterpret_cast<session_exit_t*>( data_resp );
				status_msg = Delete_Session_Cb( session_exit_req, session_exit_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( session_exit_resp ), resp_length );

				break;

			case DATA_WRITE:

				write_req = reinterpret_cast<write_req_t*>( req_data );
				write_resp = reinterpret_cast<write_resp_t*>( data_resp );
				status_msg = Data_Write_Cb( write_req, write_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( write_resp ), resp_length );

				break;

			case DATA_VALIDATE:

				validate_req = reinterpret_cast<image_validate_req_t*>( req_data );
				validate_resp = reinterpret_cast<image_validate_t*>( data_resp );

				status_msg = Data_Validate_Cb( validate_req, validate_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( validate_resp ), resp_length );
				break;

			case DATA_COMMIT:

				commit_req = reinterpret_cast<commit_req_t*>( req_data );
				commit_resp = reinterpret_cast<image_commit_t*>( data_resp );

				status_msg = Data_Commit_Cb( commit_req, commit_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( commit_resp ), resp_length );

				break;

			case FUS_STATUS:

				status_check_req = reinterpret_cast<image_status_check_req_t*>( req_data );
				status_check_resp = reinterpret_cast<image_status_check_t*>( data_resp );

				status_msg = Data_Status_Cb( status_check_req, status_check_resp );
				if ( status_msg != BLE_SUCCESS )
				{
					resp_length = MAX_BLE_ERROR_LENGTH;
				}
				else
				{
					resp_length = MAX_BLE_RESPONSE_LENGTH;
				}

				BLE_FUS_Interface_Send_Response( reinterpret_cast<uint8_t*>( status_check_resp ), resp_length );
				break;

			default:
				break;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BLE_FUS_Interface_Send_Response( uint8_t* rsp_data, uint8_t rsp_data_len )
{
	if ( ble_stack_interface_handler != nullptr )
	{
		DCI_LENGTH_TD dci_length = 0U;
		dci_length = ble_resp_frame->Get_Length();
		if ( rsp_data_len <= dci_length )
		{
			ble_resp_frame->Check_In( rsp_data );
		}
	}
}
