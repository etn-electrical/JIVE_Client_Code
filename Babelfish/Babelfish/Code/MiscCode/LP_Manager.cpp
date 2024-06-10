/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "LP_Manager.h"
#include "uC_PWR.h"

namespace BF_Misc
{

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
LP_Manager::cback_func_t LP_Manager::m_low_power_config_cb = nullptr;
LP_Manager::cback_param_t LP_Manager::m_lp_type_req_handle = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

LP_Manager::LP_Manager( cback_func_t low_power_config_cb,
						cback_param_t lp_type_req_handle )
{
	LP_Manager::m_low_power_config_cb = low_power_config_cb;
	LP_Manager::m_lp_type_req_handle = lp_type_req_handle;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LP_Manager::Enter_LP_Mode( lp_cmd_t lp_cmd )
{
	uC_PWR* pwr_down = new uC_PWR();

	if ( pwr_down != nullptr )
	{
		pwr_down->Configure_StopMode_Level( PWR_LP_MODE_STOP2 );	// for choice between PWR_LP_MODE_STOP0,

		LP_Manager::m_low_power_config_cb( m_lp_type_req_handle,
										   lp_cmd );

		pwr_down->Enter_PowerDown_StopMode( PWR_STOPENTRY_WFI );	// for choice between __WFI (1) or __WFE (2)

		LP_Manager::m_low_power_config_cb( m_lp_type_req_handle,
										   FULL_RUN );
		delete pwr_down;
	}
}

}