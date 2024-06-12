/**
 *****************************************************************************************
 * @file

 * @details See header file for module overview.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "Local_Component.h"

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

Local_Component::Local_Component( Image** image, uint8_t num_image_ctrls,
								  comp_config_t* comp_config )
	: m_mem_ctrls( image ),
	m_comp_config( comp_config ),
	m_num_mem_ctrls( num_image_ctrls )
{
	// Copy all Nv_ctrls since passed through local variable space
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Get_Image_Count( uint8_t* image_count )
{
	*image_count = m_num_mem_ctrls;
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Local_Component::Goto_FUM( void )
{
	uint32_t wait_time = SESSION_ZERO_WAIT_TIME_MS;

	return ( wait_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Get_Comp_Desc( comp_info_resp_t* info_resp )
{
	info_resp->name = LOCAL_COMP_NAME;
	info_resp->name_len = LOCAL_COMP_NAME_LEN;
	info_resp->firmware_version = m_comp_config->firm_rev;
	info_resp->serial_num = m_comp_config->serial_num;
	info_resp->hardware_version = LOCAL_COMP_HW_VERSION;
	info_resp->guid = LOCAL_COMP_GUID;
	info_resp->image_count = m_num_mem_ctrls;
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

fus_op_status_t Local_Component::Get_Image_Specs( image_info_req_t* mem_specs,
												  image_info_resp_t* mem_specs_resp )
{
	fus_op_status_t return_status = FUS_INVALID_MEM_SELECTED_ERROR;
	uint8_t mem_device = mem_specs->image_id;

	if ( mem_device < m_num_mem_ctrls )
	{
		return_status = m_mem_ctrls[mem_device]->Get_Image_Specs( mem_specs_resp );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Status( image_status_req_t* status_req, image_status_resp_t* status_resp )
{
	fus_op_status_t return_status = FUS_INVALID_MEM_SELECTED_ERROR;
	uint8_t mem_device = status_req->image_index;

	if ( mem_device < m_num_mem_ctrls )
	{
		return_status = m_mem_ctrls[mem_device]->Status( status_resp );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Write( image_write_req_t* write_mem,
										image_write_resp_t* write_mem_resp )
{
	fus_op_status_t return_status = FUS_INVALID_MEM_SELECTED_ERROR;
	uint8_t mem_device = write_mem->image_index;

	if ( mem_device < m_num_mem_ctrls )
	{
		return_status = m_mem_ctrls[mem_device]->Write( write_mem, write_mem_resp );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Validate( image_validate_req_t* validate_req,
										   image_validate_resp_t* validate_resp )
{
	fus_op_status_t return_status = FUS_INVALID_MEM_SELECTED_ERROR;
	uint8_t mem_device = validate_req->image_index;

	if ( mem_device < m_num_mem_ctrls )
	{
		return_status = m_mem_ctrls[mem_device]->Validate( validate_req, validate_resp );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Commit( image_commit_req_t* commit_req,
										 image_commit_resp_t* commit_resp )
{
	fus_op_status_t return_status = FUS_INVALID_MEM_SELECTED_ERROR;
	uint8_t mem_device = commit_req->image_id;

	if ( mem_device < m_num_mem_ctrls )
	{
		return_status = m_mem_ctrls[mem_device]->Commit( commit_resp );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Local_Component::Exit_FUM()
{
	uint8_t mem_device;

	for ( mem_device = 0U; mem_device < m_num_mem_ctrls; mem_device++ )
	{
		m_mem_ctrls[mem_device]->Exit_FUM();
	}
	return ( FUS_NO_ERROR );
}

}


