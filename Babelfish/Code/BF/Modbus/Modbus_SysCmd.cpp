/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_SysCmd.h"
#include "StdLib_MV.h"
// #include "uC_Reset.h"
#include "Services.h"
#include "Com_Dog.h"

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
Modbus_SysCmd::Modbus_SysCmd( DCI_SOURCE_ID_TD source_id ) :
	m_exceptions( 0U ),
	m_source_id( source_id )
{}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t Modbus_SysCmd::Read_Diagnostics( MB_DIAG_STRUCT* diag_req,
										 MB_DIAG_RESP_STRUCT* diag_resp ) const
{
	uint8_t return_status = MB_NO_ERROR_CODE;

	switch ( diag_req->diag_code.u16 )
	{
		case MB_DIAG_CODE_RETURN_QUERY_DATA:
			BF_Lib::Copy_String( diag_resp->data, diag_req->data, diag_req->data_len );
			diag_resp->data_len = diag_req->data_len;
			break;

		default:
			return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t Modbus_SysCmd::Read_Exceptions( MB_EXCEPT_STRUCT* except_req,
										MB_EXCEPT_RESP_STRUCT* except_resp ) const
{
	except_resp->exception = m_exceptions;

	return ( MB_NO_ERROR_CODE );
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'except_req' not referenced
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
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t Modbus_SysCmd::Sys_Cmd_Request( MB_SYS_CMD_REQ_STRUCT* sys_cmd_req,
										MB_SYS_CMD_RESP_STRUCT* sys_cmd_resp )
{
	uint8_t return_status = MB_NO_ERROR_CODE;

	BF_Lib::Services::SERVICES_HANDLE_DATA_ST temp_service_data;

	temp_service_data.rx_data = sys_cmd_req->data;
	temp_service_data.rx_len = sys_cmd_req->data_len;
	temp_service_data.tx_data = sys_cmd_resp->data;
	temp_service_data.tx_len = sys_cmd_resp->data_len;

	switch ( sys_cmd_req->command )
	{
		// This command is used for doing factory reset via Modbus TCP
		// Note: Define DISABLE_MODBUS_TCP_FACTORY_RESET macro to disable factory reset
		// functionality from Modbus TCP
#ifndef DISABLE_MODBUS_TCP_FACTORY_RESET
		case MB_SYS_CMD_FACTORY_RESET:
			BF_Lib::Services::Execute_Service( SERVICES_FACTORY_RESET,
											   reinterpret_cast<BF_Lib::Services::
																SERVICES_HANDLE_DATA_ST*>( nullptr ) );
			// BF::System_Reset::Reset_Device( BF::System_Reset::FACTORY_RESET );
			break;
#endif	// #ifndef DISABLE_MODBUS_TCP_FACTORY_RESET

		case MB_SYS_CMD_SOFT_RESET:
			BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET,
											   reinterpret_cast<BF_Lib::Services::
																SERVICES_HANDLE_DATA_ST*>( nullptr ) );
			// BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );
			break;

		case MB_SYS_CMD_APP_PARAM_RESET:
			BF_Lib::Services::Execute_Service( SERVICES_APP_PARAM_RESET,
											   reinterpret_cast<BF_Lib::Services::
																SERVICES_HANDLE_DATA_ST*>( nullptr ) );
			// BF::System_Reset::Reset_Device( BF::System_Reset::APP_PARAM_RESET );
			break;

		case MB_SYS_CMD_WATCHDOG_TEST:
			BF_Lib::Services::Execute_Service( SERVICES_WATCHDOG_TEST_RESET,
											   reinterpret_cast<BF_Lib::Services::
																SERVICES_HANDLE_DATA_ST*>( nullptr ) );
			// BF::System_Reset::Reset_Device( BF::System_Reset::WATCHDOG_TEST_RESET );
			break;

		case MB_SYS_CMD_TRIG_COMM_LOSS_ACTION:
			BF_Misc::Com_Dog::Trigger_Comm_Loss( static_cast<DCI_SOURCE_ID_TD>( m_source_id ) );
			break;

		case MB_SYS_CMD_MFG_UNLOCK:
			BF_Lib::Services::Execute_Service( static_cast<uint8_t>( MB_SYS_CMD_MFG_UNLOCK ),
											   &temp_service_data );
			break;

		default:
			if ( sys_cmd_req->command > static_cast<uint8_t>( SERVICES_APP_SPECIFIC_START ) )
			{
				if ( BF_Lib::Services::Execute_Service( sys_cmd_req->command,
														&temp_service_data ) == false )
				{
					return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
				}
				sys_cmd_resp->data_len = temp_service_data.tx_len;
			}
			else
			{
				return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
			}
			break;
	}

	return ( return_status );
}
