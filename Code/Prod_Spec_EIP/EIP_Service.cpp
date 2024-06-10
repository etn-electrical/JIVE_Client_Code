/**
 *****************************************************************************************
 * @file       EIP_Services.cpp
 * @details    EIP services specific declarations and definitions.
 *             This file shall include the initialization routines specific to EIP component.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "EIP_Service.h"
#include "Scratch_Ram.h"
#include "EIP_Obj_Svc.h"
#include "eipid.h"
#include "Stdlib_MV.h"
#include "Services.h"
#include "OS_Tasker.h"
#include "EIP_if.h"
#include "platform_app.h"
#include "Shadow_Product.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
bool_t EIP_Service::m_restart_eip_stack = false;

EtIPObjectRequest EIP_Service::m_request;
EtIPObjectResponse EIP_Service::m_response;

EtherNetIP_DCI* EIP_Service::m_eip_dci;
EIP_TARGET_INFO_ST_TD const* EIP_Service::m_eip_target_info;
EIP_Display* EIP_Service::m_eip_display;
OS_Binary_Semaphore* EIP_Service::m_BinSem_Lwip_Started =
	reinterpret_cast<OS_Binary_Semaphore*>( NULL );

CIP_ASM_STRUCT_TD const* EIP_Service::m_end_asm_struct =
	reinterpret_cast<CIP_ASM_STRUCT_TD*>( NULL );
CIP_Asm* EIP_Service::m_cip_asm;

EtIPIdentityInfo* const eip_stack_ident = &gIDInfo[0];
OS_Mutex* m_EipApiMutex =
	reinterpret_cast<OS_Mutex*>( NULL );

// DCI_Owner*					EIP_Service::meip_restart_test_code =
// reinterpret_cast<DCI_Owner*>(NULL);

Shadow_Product* EIP_Service::m_shadow_product =
	reinterpret_cast<Shadow_Product*>( NULL );

/*
 ********************************************************************************
 *		Prototypes
 ********************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
EIP_Service::EIP_Service( EIP_TARGET_INFO_ST_TD const* eip_target_info,
						  EIP_Display* eip_display, Shadow_Product* product_shadow )
	: mEFT_test_owner( nullptr )
{
	EIP_PLATFORM_Init();
	Platform_App::EIP_CALLBACK_Init();
	m_shadow_product = product_shadow;
	m_eip_target_info = eip_target_info;
	m_eip_dci = new EtherNetIP_DCI( m_eip_target_info, DCI_SOURCE_IDS_Get() );

	m_eip_display = eip_display;
	m_eip_display->MS_Operational();

	m_EipApiMutex = new OS_Mutex();
	m_BinSem_Lwip_Started = new OS_Binary_Semaphore( false );

	OS_Tasker::Create_Task( &EIP_Stack_Task,
							EIP_OS_STACK_SIZE,
							OS_TASK_PRIORITY_4,
							reinterpret_cast<uint8_t const*>( "EIP Task" ),
							nullptr );

	m_cip_asm = new CIP_Asm( eip_target_info->asm_list,
							 eip_target_info->total_assemblies );

	BF_Misc::Faults::Attach_State_Change_Cback( &Handle_Com_Fault_State_Change,
												reinterpret_cast<BF_Misc::STATE_CHANGE_CBACK_PARAM>( NULL ) );

	// meip_restart_test_code = new DCI_Owner( DCI_EIP_RESTART_TEST_CODE_DCID );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Start_EIP_Stack( void )
{
	m_BinSem_Lwip_Started->Post();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::EIP_Stack_Task( OS_TASK_PARAM handle )
{

	while ( !m_BinSem_Lwip_Started->Pend( OS_BINARY_SEMAPHORE_INDEFINITE_TIMEOUT ) )
	{  }

	while ( clientStart() == false )
	{  }

	Initialize_Stack_Hooks();

	// Tell the stack to verify any new connections coming in.
	clientSetAppVerifyConnection( true );

	clientAutomaticRunIdleEnforcement( true );

	// coverity[no_escape]
	while ( true )
	{
		OS_Tasker::Delay( 1U );

		if ( m_restart_eip_stack == true )
		{
			clientUnclaimAllHostIPAddress();

			platformSocketInitTargetData();

			clientClaimAllHostIPAddress();

			m_restart_eip_stack = false;
		}
		// Call the client task of EIP stack
		clientMainTask();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Event_Callback( int32_t event, int32_t param1, int32_t param2 )
{
	static volatile uint32_t counter = 0U;

	switch ( event )
	{
		case NM_ID_RESET:
			// Identity Reset was received.
			// Second parameter is Instance being reset.
			// Third parameter is reset type
			Ident_Reset_Svc( param2 );
			break;

		case NM_CLIENT_OBJECT_REQUEST_RECEIVED:
			Process_Object_Request( param1 );
			break;

		case NM_ASSEMBLY_NEW_INSTANCE_DATA:
			m_cip_asm->Process_RXed_Assembly( static_cast<uint32_t>( param1 ) );
			break;

		case NM_NETWORK_LED_CHANGE:
			Net_Status_LED_State_Change( param1 );
			break;

		case NM_CONNECTION_ESTABLISHED:
			connectionGetFromInstance( param1 );
			counter++;
			break;

		case NM_CONNECTION_NEW_INPUT_SCANNER_DATA:
			// ETH_System::C445_Com_Dog::Inc_Active_Message_Count();//Show fieldbus activity
			m_active_msg_count_callback();
			break;

		case NM_CONNECTION_TIMED_OUT:
			// Active connection timed out.
			// iWatchdogTimeoutAction member of the connection
			// configuration will determine the associated action
			BF_Misc::Com_Dog::Trigger_Comm_State( BF_Misc::COM_DOG_COMM_LOSS );
			// m_shadow_product->ConnectionTimedOutEventSet = true;
			break;

		case NM_CONNECTION_CLOSED:
			BF_Misc::Com_Dog::Trigger_Comm_State( BF_Misc::COM_DOG_GOTO_SAFE );
			break;

		case NM_CONNECTION_VERIFICATION:
			Verify_Connection( param1 );
			break;

		case NM_INCOMING_CONNECTION_RUN_IDLE_FLAG_CHANGED:
			// Run Idle flag changed for the incoming Class1 connection.
			// Param1 contains connection instance.
			// Param2 contains the new flag (1 - Run, 0 - Idle).
			if ( param2 == 0 )
			{
				BF_Misc::Com_Dog::Trigger_Comm_State( BF_Misc::COM_DOG_COMM_IDLE );
			}
			else
			{
				// Running
				connectionGetFromInstance( param1 );
			}
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
void EIP_Service::Net_Status_LED_State_Change( int32_t new_status )
{
	switch ( new_status )
	{
		case NetworkStatusOff:
			m_eip_display->NS_Offline();
			break;

		case NetworkStatusFlashingGreen:
			m_eip_display->NS_Online_Disconnected();
			break;

		case NetworkStatusGreen:
			m_eip_display->NS_Online_Connected();
			break;

		case NetworkStatusFlashingRed:
			m_eip_display->NS_Connection_Timeout();
			break;

		case NetworkStatusRed:
			m_eip_display->NS_Critical_Link_Failure();
			break;

		case NetworkStatusFlashingGreenRed:
			m_eip_display->NS_Comm_Faulted_With_Identify();
			break;

		default:
			m_eip_display->NS_Comm_Faulted_With_Identify();
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Process_Object_Request( int32_t request_id )
{
	bool_t respond = false;

	// Get the request from the Ethernet/IP library that was scheduled for
	// this application processing.
	// Request with this Id not found
	if ( !static_cast<bool_t>( clientGetClientRequest( request_id, &m_request ) ) )
	{
		m_response.bGeneralStatus = static_cast<uint8_t>( ROUTER_ERROR_SUCCESS );
		m_response.iExtendedStatus = 0U;
		m_response.iDataSize = 0U;

		switch ( m_request.bService )
		{
			case SVC_GET_ATTR_SINGLE:
				respond = Get_Class_Attr_Single( &m_request, &m_response );
				break;

			case SVC_SET_ATTR_SINGLE:
				respond = Set_Class_Attr_Single( &m_request, &m_response );
				break;

			default:
				respond = EIP_OBJ_SVC_Execute( &m_request, &m_response );
				// m_response.bGeneralStatus = ROUTER_ERROR_BAD_SERVICE;
				// respond = true;
				break;
		}

		if ( respond == true )
		{
			clientSendClientResponse( request_id, &m_response );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t EIP_Service::Get_Class_Attr_Single( EtIPObjectRequest* request,
										   EtIPObjectResponse* response )
{
	CIP_GET_STRUCT dci_req;

	dci_req.cia.cip_class = static_cast<uint16_t>( request->iClass );
	dci_req.cia.cip_instance = static_cast<uint16_t>( request->iInstance );
	dci_req.cia.cip_attrib = static_cast<uint16_t>( request->iAttribute );
	dci_req.dest_data = response->responseData;
	dci_req.length = 0U;

	// Get Value from DCI DataBase
	response->bGeneralStatus = m_eip_dci->Get( &dci_req );
	response->iDataSize = dci_req.length;

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t EIP_Service::Set_Class_Attr_Single( EtIPObjectRequest* request,
										   EtIPObjectResponse* response )
{
	CIP_SET_STRUCT dci_req;

	dci_req.cia.cip_class = static_cast<uint16_t>( request->iClass );
	dci_req.cia.cip_instance = static_cast<uint16_t>( request->iInstance );
	dci_req.cia.cip_attrib = static_cast<uint16_t>( request->iAttribute );

	dci_req.length = request->iDataSize;
	dci_req.src_data = request->requestData;

	response->bGeneralStatus = m_eip_dci->Set( &dci_req );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Initialize_Stack_Hooks( void )
{
	Load_EIP_Class_Stack_Hook();
	Load_EIP_Assembly_Stack_Hook();
	Load_EIP_Identity();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Load_EIP_Class_Stack_Hook( void )
{
	uint32_t* class_list_dest;
	uint16_t const* class_list_src;
	BF_Lib::MUINT16 num_registered_classes;
	BF_Lib::MUINT16 class_count = m_eip_dci->Class_Count();
	BF_Lib::Scratch_Ram class_list( class_count * sizeof( uint32_t ) );

	/*We are allowing the stack to handle the Identity object
	   & the Assembly object.
	   This requires that we skip passing in those objects as
	   something for us to handle.
	 */
	// class_count = m_eip_dci->Class_Count();
	class_list_src = m_eip_dci->Class_List();

	// class_list.Allocate( class_count * sizeof(uint32_t) );
	class_list_dest = reinterpret_cast<uint32_t*>( reinterpret_cast<void*>(
													   class_list.m_data ) );

	num_registered_classes = 0U;
	for ( BF_Lib::MUINT16 class_index = 0U; class_index < class_count;
		  class_index++ )
	{
		if ( ( class_list_src[class_index] != EIP_SERVICE_ASSEMBLY_CLASS_ID )
			 &&
			 ( class_list_src[class_index] != EIP_SERVICE_IDENTITY_CLASS_ID )
			  )
		{
			class_list_dest[num_registered_classes] = class_list_src[class_index];
			num_registered_classes++;
		}
	}

	clientRegisterEventCallBack( &Event_Callback );
	clientRegObjectsForClientProc( class_list_dest,
								   num_registered_classes,
								   nullptr,
								   0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Load_EIP_Assembly_Stack_Hook( void )
{
	m_cip_asm->Initialize();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Load_EIP_Identity( void )
{
	CIP_GET_STRUCT dci_req;
	BF_Lib::SPLIT_UINT16 firmware_rev;
	uint8_t temp_string[static_cast<uint8_t>( MAX_PRODUCT_NAME_SIZE ) + 1U];

	dci_req.cia.cip_class = CIP_IDENTITY_CLASS;
	dci_req.cia.cip_instance = CIP_IDENTITY_INSTANCE;

	dci_req.cia.cip_attrib = 1U;
	dci_req.dest_data = reinterpret_cast<uint8_t*>( &eip_stack_ident->iVendor );
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );

	dci_req.cia.cip_attrib = 2U;
	dci_req.dest_data = reinterpret_cast<uint8_t*>(
		&eip_stack_ident->iProductType );
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );

	dci_req.cia.cip_attrib = 3U;
	dci_req.dest_data = reinterpret_cast<uint8_t*>(
		&eip_stack_ident->iProductCode );
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );

	dci_req.cia.cip_attrib = 4U;
	// #k:Assign address of Union-firmware_rev to *.dest_data
	dci_req.dest_data = firmware_rev.u8;
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );
	eip_stack_ident->bMajorRevision = firmware_rev.u8[1];
	eip_stack_ident->bMinorRevision = firmware_rev.u8[0];

	dci_req.cia.cip_attrib = 5U;
	dci_req.dest_data = reinterpret_cast<uint8_t*>(
		&eip_stack_ident->iStatus );
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );

	dci_req.cia.cip_attrib = 6U;
	dci_req.dest_data = reinterpret_cast<uint8_t*>(
		&eip_stack_ident->lSerialNumber );
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );

	dci_req.cia.cip_attrib = 7U;
	dci_req.dest_data = temp_string;
	dci_req.length = 0U;
	m_eip_dci->Get( &dci_req );
	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( eip_stack_ident->productName ),
						 &temp_string[1],
						 static_cast<uint32_t>( MAX_PRODUCT_NAME_SIZE ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Load_EIP_Identity_Attribute( CIP_ATTRIB_TD attribute )
{
	CIP_GET_STRUCT dci_req;
	BF_Lib::SPLIT_UINT16 firmware_rev;
	uint8_t temp_string[static_cast<uint8_t>( MAX_PRODUCT_NAME_SIZE ) + 1U];
	bool_t return_immediately = false;

	dci_req.cia.cip_class = CIP_IDENTITY_CLASS;
	dci_req.cia.cip_instance = CIP_IDENTITY_INSTANCE;
	dci_req.length = 0U;
	dci_req.cia.cip_attrib = attribute;

	switch ( attribute )
	{
		case ID_ATTR_VENDOR_ID:
			dci_req.dest_data = reinterpret_cast<uint8_t*>(
				&eip_stack_ident->iVendor );
			break;

		case ID_ATTR_DEVICE_TYPE:
			dci_req.dest_data = reinterpret_cast<uint8_t*>(
				&eip_stack_ident->iProductType );
			break;

		case ID_ATTR_PRODUCT_CODE:
			dci_req.dest_data = reinterpret_cast<uint8_t*>(
				&eip_stack_ident->iProductCode );
			break;

		case ID_ATTR_REVISION:
			// #k: assign address of Union-firmware_rev to *.dest_data
			dci_req.dest_data = firmware_rev.u8;
			dci_req.length = 0U;
			m_eip_dci->Get( &dci_req );
			eip_stack_ident->bMajorRevision = firmware_rev.u8[1];
			eip_stack_ident->bMinorRevision = firmware_rev.u8[0];
			return_immediately = true;
			// return;
			break;

		case ID_ATTR_STATUS:
			dci_req.dest_data = reinterpret_cast<uint8_t*>( &eip_stack_ident->iStatus );
			break;

		case ID_ATTR_SERIAL_NBR:
			dci_req.dest_data = reinterpret_cast<uint8_t*>(
				&eip_stack_ident->lSerialNumber );
			break;

		case ID_ATTR_PRODUCT_NAME:
			dci_req.dest_data = temp_string;
			dci_req.length = 0U;
			m_eip_dci->Get( &dci_req );
			BF_Lib::Copy_String( reinterpret_cast<uint8_t*>(
									 eip_stack_ident->productName ),
								 &temp_string[1],
								 static_cast<uint32_t>( MAX_PRODUCT_NAME_SIZE ) );
			// return;
			return_immediately = true;
			break;

		default:
			return_immediately = true;
	}

	if ( !return_immediately )
	{
		m_eip_dci->Get( &dci_req );
	}
	return;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Ident_Reset_Svc( int32_t reset_requested )
{
	switch ( reset_requested )
	{
		case 0:
			BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET );
			break;

		case 1:
			BF_Lib::Services::Execute_Service( SERVICES_FACTORY_RESET );
			break;

		default:
			// We should never get here since it is filtered on the stack end.
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Handle_Com_Fault_State_Change(
	BF_Misc::STATE_CHANGE_CBACK_PARAM param )
{
	if ( BF_Misc::Faults::Faulted() )
	{
		m_eip_display->MS_Unrecoverable_Fault();
	}
	else if ( BF_Misc::Faults::Warned() )
	{
		m_eip_display->MS_Minor_Fault();
	}
	else
	{
		m_eip_display->MS_Operational();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Service::Verify_Connection( int32_t iConnectionInstance )
{
	EtIPConnectionConfig Config;
	EtIPErrorInfo errorInfo;
	uint8_t* temp_data;

	/* Get the connection information */
	clientGetConnectionConfig( iConnectionInstance, &Config );

	memset( &errorInfo, 0, sizeof( EtIPErrorInfo ) );

	Term_Print( "Cons Size: %d     Prod Size: %d \n",
				Config.iCurrentConsumingSize,
				Config.iCurrentProducingSize
				);

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
	 * unsigned short
	 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
	 * casting pointers, not enums.
	 */
	/*lint -e{930} # MISRA Deviation */
	if ( Config.bTransportClass == static_cast<uint8_t>( Class1 ) )
	{
		/* InputOnly and ListenOnly connections don't have a Run/Idle header */
		if ( ( ( ( Config.iTargetConsumingConnPoint ==
				   static_cast<uint32_t>( HEARTBEAT_CONN_POINT ) ) ||
				 ( Config.iTargetConsumingConnPoint ==
				   static_cast<uint32_t>( LISTEN_ONLY_CONN_POINT ) ) ) &&
			   ( Config.bInputRunProgramHeader != false ) ) ||
			/* Consuming assembly must have Run/Idle header */
			 ( ( ( Config.iTargetConsumingConnPoint !=
				   static_cast<uint32_t>( HEARTBEAT_CONN_POINT ) ) &&
				 ( Config.iTargetConsumingConnPoint !=
				   static_cast<uint32_t>( LISTEN_ONLY_CONN_POINT ) )
				  ) &&
			   ( Config.bInputRunProgramHeader != true )
			    )
			 ||	/* Producing assembly must not have Run/Idle header */
			 ( Config.bOutputRunProgramHeader != false )
			  )
		{
			errorInfo.bGeneralStatus = static_cast<uint8_t>( ROUTER_ERROR_FAILURE );
			// ROUTER_EXT_ERR_INVALID_CONN_SIZE; //EIP Stcak V4.1.0
			// errorInfo.iExtendedStatus = MAV_ROUTER_EXT_ERR_INVALID_TO_SIZE;
			// #k: as per New EIP Stcak V4.3.0
			errorInfo.iExtendedStatus = static_cast<uint16_t>(
				ROUTER_EXT_ERR_INVALID_TO_SIZE );
			errorInfo.extendedStatusData[0] = 3U;
			errorInfo.iExtendedStatusDataSize = 1U;
		}
		else
		{
			/* Only allow class1 connections with an RPI of EIP_CLASS_1_MIN_RPI ms
			   or slower */
			if ( ( Config.lConsumingDataRate < EIP_CLASS_1_MIN_RPI ) ||
				 ( Config.lProducingDataRate < EIP_CLASS_1_MIN_RPI )
				  )
			{
				errorInfo.bGeneralStatus = static_cast<uint8_t>( ROUTER_ERROR_FAILURE );
				errorInfo.iExtendedStatus =
					static_cast<uint16_t>( ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE );

				errorInfo.iExtendedStatusDataSize = 5U;	/* size in words */
				/* treat it as a byte array */
				temp_data = reinterpret_cast<uint8_t*>( errorInfo.extendedStatusData );

				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-0-4, Implicit conversion changes
				 * signedness (1960)
				 *@n Justification: Definitions are defined in third party software.
				 * To fix it, we'd have to update the third party code.
				 */
				/*lint -e{1960} # MISRA Deviation */
				*temp_data = (
					( Config.lConsumingDataRate < EIP_CLASS_1_MIN_RPI )
						? CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE
						: CONNMGR_EXT_ERR_RPI_ACCEPTABLE
					);
				temp_data++;
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-0-4, Implicit conversion changes
				 * signedness (1960)
				 *@n Justification: Definitions are defined in third party software.
				 * To fix it, we'd have to update the third party code.
				 */
				/*lint -e{1960} # MISRA Deviation */
				*temp_data = (
					( Config.lProducingDataRate < EIP_CLASS_1_MIN_RPI )
						? CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE
						: CONNMGR_EXT_ERR_RPI_ACCEPTABLE
					);
				temp_data++;

				/* RPI is in microseconds */
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-4, C-style cast (1924)
				 *@n MISRA-C++[2008] Required Rule 3-9-2, Use of modifier or type
				 * 'unsigned' outside of a typedef (970)
				 *@n MISRA-C++[2008] Required Rule 5-0-21, Bitwise operator applied to
				 * signed underlying type: &
				 *@n Justification: UINT32_SET is defined in third party software.
				 *  To fix it, we'd have to update the third party code.
				 */
				/*lint -e{1924, 970, 1960} # MISRA Deviation */
				UINT32_SET( temp_data,
							( ( Config.lConsumingDataRate < EIP_CLASS_1_MIN_RPI )
						? ( EIP_CLASS_1_MIN_RPI * 1000U )
						: ( Config.lConsumingDataRate * 1000U )
							   )
							)
				temp_data += sizeof( uint32_t );
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-4, C-style cast (1924)
				 *@n MISRA-C++[2008] Required Rule 3-9-2, Use of modifier or type
				 * 'unsigned' outside of a typedef (970)
				 *@n MISRA-C++[2008] Required Rule 5-0-21, Bitwise operator applied to
				 * signed underlying type: &
				 *@n Justification: UINT32_SET is defined in third party software.
				 * To fix it, we'd have to update the third party code.
				 */
				/*lint -e{1924, 970, 1960} # MISRA Deviation */
				UINT32_SET( temp_data,
							( ( Config.lProducingDataRate < EIP_CLASS_1_MIN_RPI )
								? ( EIP_CLASS_1_MIN_RPI * 1000U )
								: ( Config.lProducingDataRate * 1000U )
							   )
							)
				temp_data += sizeof( uint32_t );
			}
		}
	}
	else
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
		 * unsigned short
		 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
		 * casting pointers, not enums.
		 */
		/*lint -e{930} # MISRA Deviation */
		if ( ( Config.bTransportClass == static_cast<uint8_t>( Class3 ) )
			/* Only allow class3
			          connections with an RPI of EIP_CLASS_3_MIN_RPI ms or slower*/
			 && ( ( Config.lConsumingDataRate < EIP_CLASS_3_MIN_RPI ) ||
				  ( Config.lProducingDataRate < EIP_CLASS_3_MIN_RPI )
				   )
			  )
		{
			errorInfo.bGeneralStatus = static_cast<uint8_t>( ROUTER_ERROR_FAILURE );
			errorInfo.iExtendedStatus =
				static_cast<uint16_t>( ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE );

			errorInfo.iExtendedStatusDataSize = 5U;	/* size in words
													   treat it as a byte array*/
			temp_data = reinterpret_cast<uint8_t*>( errorInfo.extendedStatusData );

			*temp_data = ( ( Config.lConsumingDataRate < EIP_CLASS_3_MIN_RPI )
					  ? static_cast<uint8_t>( CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE )
					  : static_cast<uint8_t>( CONNMGR_EXT_ERR_RPI_ACCEPTABLE )
						    );
			temp_data++;
			*temp_data = ( ( Config.lProducingDataRate < EIP_CLASS_3_MIN_RPI )
					  ? static_cast<uint8_t>( CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE )
					  : static_cast<uint8_t>( CONNMGR_EXT_ERR_RPI_ACCEPTABLE )
						    );
			temp_data++;

			/* RPI is in microseconds */
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-2-4, C-style cast (1924)
			 *@n MISRA-C++[2008] Required Rule 3-9-2, Use of modifier or type
			 * 'unsigned' outside of a typedef (970)
			 *@n MISRA-C++[2008] Required Rule 5-0-21, Bitwise operator applied to
			 * signed underlying type: &
			 *@n Justification: UINT32_SET is defined in third party software.
			 * To fix it, we'd have to update the third party code.
			 */
			/*lint -e{1924, 970, 1960} # MISRA Deviation */
			UINT32_SET( temp_data,
						( ( Config.lConsumingDataRate < EIP_CLASS_3_MIN_RPI )
								? ( EIP_CLASS_3_MIN_RPI * 1000U )
					  : ( Config.lConsumingDataRate * 1000U )
						   )
						)
			temp_data += sizeof( uint32_t );

			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-2-4, C-style cast (1924)
			 *@n MISRA-C++[2008] Required Rule 3-9-2, Use of modifier or type
			 * 'unsigned' outside of a typedef (970)
			 *@n MISRA-C++[2008] Required Rule 5-0-21, Bitwise operator applied to
			 * signed underlying type: &
			 *@n Justification: UINT32_SET is defined in third party software.
			 * To fix it, we'd have to update the third party code.
			 */
			/*lint -e{1924, 970, 1960} # MISRA Deviation */
			UINT32_SET( temp_data,
						( ( Config.lProducingDataRate < EIP_CLASS_3_MIN_RPI )
					  ?( EIP_CLASS_3_MIN_RPI * 1000U )
					  :( Config.lProducingDataRate * 1000U )
						   )
						)
			temp_data += sizeof( uint32_t );
		}
	}

	// coverity[var_deref_model]
	clientSendConnectionResponse( iConnectionInstance,
								  &errorInfo,
								  0U,
								  reinterpret_cast<uint8_t*>( NULL ) );
}
