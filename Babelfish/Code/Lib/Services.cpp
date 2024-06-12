/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Services.h"
#include "System_Reset.h"

namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
/// NOTE: The standard services as options are commented out for now.  They can be added in if we
// decide
/// to make the standard services configurable on startup.  Right now they are hard coded.
Services::SERVICES_HANDLER_CB* Services::m_app_spec_cb[SERVICES_MAX_APP_SPECIFIC_ARRAY];
Services::SERVICES_HANDLE_CB Services::m_app_spec_handle[SERVICES_MAX_APP_SPECIFIC_ARRAY];
bool Services::m_mfg_access_unlocked = SERVICES_DEFAULT_MFG_ACCESS;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Services::Services( void )
{
	// For future standard services.
	// for ( uint8_t i = 0U; i < SERVICES_MAX_STANDARD; i++ )
	// {
	// m_standard_cb[i] = Null_Func;
	// m_standard_handle[i] = NULL;
	// }

	for ( uint8_t i = 0U; i < SERVICES_MAX_APP_SPECIFIC_ARRAY; i++ )
	{
		m_app_spec_cb[i] = &Null_Func;
		m_app_spec_handle[i] = reinterpret_cast<SERVICES_HANDLE_CB>( nullptr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Services::Add_Service_Handler( uint8_t service_id, SERVICES_HANDLER_CB* cb_handler,
									SERVICES_HANDLE_CB cb_handle )
{
	uint16_t service_index;

	if ( service_id > SERVICES_APP_SPECIFIC_START )
	{
		service_index = static_cast<uint16_t>( service_id ) - SERVICES_APP_SPECIFIC_START;
		if ( service_id < SERVICES_APP_SPECIFIC_END )
		{
			m_app_spec_cb[service_index] = cb_handler;
			m_app_spec_handle[service_index] =
				reinterpret_cast<SERVICES_HANDLE_CB>( cb_handle );
		}
	}
	/// The following is commented out because we are not adding standard service addition
	/// flexibility.  This is for future.
	// else if ( service_id < SERVICES_MAX_STANDARD )
	// {
	// m_standard_cb[service_id] = cb_handler;
	// m_standard_handle[service_id] = cb_handler;
	// }
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'cb_handle' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Services::Execute_Service( uint8_t service_id,
								SERVICES_HANDLE_DATA_ST* service_data )
{
	bool service_success = false;
	uint16_t service_index;

	if ( service_id > SERVICES_APP_SPECIFIC_START )
	{
		service_index = static_cast<uint16_t>( service_id ) - SERVICES_APP_SPECIFIC_START;
		if ( ( service_id < SERVICES_APP_SPECIFIC_END ) &&
			 ( m_app_spec_cb[service_index] != nullptr ) )
		{
			service_success =
				( *m_app_spec_cb[service_index] )( m_app_spec_handle[service_index],
												   service_id, service_data );
		}
	}
	else if ( service_id < SERVICES_MAX_STANDARD )
	{
		service_success = true;

		switch ( service_id )
		{
			case SERVICES_SOFT_RESET:			// This reset will simply do a hardware reset.
				BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );
				break;

			case SERVICES_FACTORY_RESET:		// This reset brings the device to out of box state.
				BF::System_Reset::Reset_Device( BF::System_Reset::FACTORY_RESET );
				break;

			case SERVICES_APP_PARAM_RESET:		// This reset service will bring all application values
												// to defaults.  The application parameters are defined
												// in the app column of the spreadshet.
				BF::System_Reset::Reset_Device( BF::System_Reset::APP_PARAM_RESET );
				break;

			case SERVICES_WATCHDOG_TEST_RESET:		// This reset will sit in a loop until the watchdog
													// catches it.
				BF::System_Reset::Reset_Device( BF::System_Reset::WATCHDOG_TEST_RESET );
				break;

			case SERVICES_MFG_ACCESS_UNLOCK:
				if ( service_data != nullptr )
				{
					if ( service_data->rx_len == SERVICES_MFG_ACCESS_UNLOCK_DATA_LEN )
					{
						if ( *service_data->rx_data != 0U )
						{
							m_mfg_access_unlocked = true;
						}
						else
						{
							m_mfg_access_unlocked = false;
						}
					}
					else if ( ( service_data->rx_len == 0U ) &&
							  ( service_data->tx_data != nullptr ) )
					{
						*service_data->tx_data = m_mfg_access_unlocked;
						service_data->tx_len = SERVICES_MFG_ACCESS_UNLOCK_DATA_LEN;
					}
					else
					{	/* do nothing */
					}
				}
				break;

			default:
				service_success = true;
				break;
		}
	}
	else
	{	/* do nothing */
	}
	return ( service_success );
}

}	/*  end namespace BF_Lib for this module */
