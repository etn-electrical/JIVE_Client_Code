/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Gate.h"
#include "Babelfish_Assert.h"



/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Gate::Gate( DCI_Owner* fid_dcid_owner, Enabler** enabler_handle_array ) :
	m_fid_dcid_owner( fid_dcid_owner ),
	m_enabler_handle_array( enabler_handle_array ),
	m_func_callback( nullptr ),
	m_cback_param( nullptr )
{

	DCI_OWNER_ATTRIB_TD owner_attrib;

	if ( m_fid_dcid_owner != nullptr )
	{
		owner_attrib = m_fid_dcid_owner->Get_Owner_Attrib();
		BF_ASSERT( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_CALLBACK ) );

		if ( BF_Lib::Bit::Test( owner_attrib, DCI_OWNER_ATTRIB_INFO_CALLBACK ) )
		{
			m_fid_dcid_owner->Attach_Callback( &Fid_Change_Callback_static,
											   reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
											   DCI_ACCESS_SET_RAM_CMD_MSK );
		}
	}
	else
	{
		BF_ASSERT( m_fid_dcid_owner );
	}


}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Gate::~Gate()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Gate::fid_status_t Gate::Is_Enabled( Enabler::fid_t fid )
{
	Enabler::fid_t fid_value = 0;
	Enabler::ret_status_t ret_status = false;
	fid_status_t return_status = INVALID_FID;

	if ( fid < TOTAL_FID_COUNT )
	{
		m_fid_dcid_owner->Check_Out( fid_value );
		ret_status = Test_Bit( fid_value, fid );

		if ( ret_status == true )
		{
			return_status = ENABLED;
		}
		else
		{
			return_status = DISABLED;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Gate::command_status_t Gate::Enable( Enabler::fid_t fid )
{
	Enabler::fid_t fid_value = 0;
	Enabler::ret_status_t ret_status = false;
	command_status_t return_status = INVALID_ID;

	if ( fid < TOTAL_FID_COUNT )
	{
		if ( m_enabler_handle_array[fid] != nullptr )
		{

			ret_status = m_enabler_handle_array[fid]->Unlock();
			if ( ret_status == true )
			{
				m_fid_dcid_owner->Check_Out( fid_value );
				Set_Bit( fid_value, fid );
				m_fid_dcid_owner->Check_In( fid_value );
				return_status = SUCCESS;
			}
			else
			{
				return_status = FAILURE;
			}

		}
	}

	if ( m_func_callback != nullptr )
	{
		m_func_callback( m_cback_param, fid, ENABLED );
	}
	return ( return_status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Gate::command_status_t Gate::Disable( Enabler::fid_t fid )
{
	Enabler::fid_t fid_value = 0;
	Enabler::ret_status_t ret_status = false;
	command_status_t return_status = INVALID_ID;

	if ( fid < TOTAL_FID_COUNT )
	{
		if ( m_enabler_handle_array[fid] != nullptr )
		{
			ret_status = m_enabler_handle_array[fid]->Lock();
			if ( ret_status == true )
			{
				m_fid_dcid_owner->Check_Out( fid_value );
				Clr_Bit( fid_value, fid );
				m_fid_dcid_owner->Check_In( fid_value );
				return_status = SUCCESS;
			}
			else
			{
				return_status = FAILURE;
			}

		}

	}
	if ( m_func_callback != nullptr )
	{
		m_func_callback( m_cback_param, fid, DISABLED );
	}
	return ( return_status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Gate::Fid_Change_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	BF_Lib::Unused<DCI_ACCESS_ST_TD*>::Okay( access_struct );

	// TO DO : This call back will be needed if the feature id value is updated by patron (other then owner)
	// to Disable or enable any feature in this call back we might need to add 2 more dcid's
	// 1. for which feature id is being updated
	// 2. for what action (disable or enable) needs to be taken

	return ( DCI_CBACK_RET_PROCESS_NORMALLY );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Gate::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_cback_param = func_param;
	m_func_callback = func_callback;
}