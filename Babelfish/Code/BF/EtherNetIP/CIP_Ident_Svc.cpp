/*
 ********************************************************************************
 *       $Workfile: CIP_Ident_Svc.cpp$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 3/5/2009 8:49:08 AM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *		$Header: CIP_Ident_Svc.h, 22, 3/5/2009 8:49:08 AM, Mark A Verheyen$
 ********************************************************************************
 */
#include "Includes.h"
#include "CIP_Ident_Svc.h"
#include "EIP_Obj_Svc.h"
#include "Services.h"

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

/*
 ********************************************************************************
 *		Prototypes
 ********************************************************************************
 */
static bool_t CIP_IDENT_SVC_Execute( EIP_OBJ_SVC_CBPARAM param,
									 EtIPObjectRequest* request,
									 EtIPObjectResponse* response );

static EIP_OBJ_SVC_DEF cip_ident_svc_reset =
{
	0x05U,							// uint16_t service_code;
	0x01U,							// CIP_CLASS_TD cip_class;
	0x00U,							// CIP_INSTANCE_TD cip_instance;
	&CIP_IDENT_SVC_Execute,			// EIP_OBJ_SVC_CBACK* callback_func;
	reinterpret_cast<EIP_OBJ_SVC_CBPARAM>( NULL )	// EIP_OBJ_SVC_CBPARAM cback_param;
};

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
void CIP_IDENT_SVC_Init( void )
{
	EIP_OBJ_SVC_Attach( &cip_ident_svc_reset );
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
static bool_t CIP_IDENT_SVC_Execute( EIP_OBJ_SVC_CBPARAM param,
									 EtIPObjectRequest* request,
									 EtIPObjectResponse* response )
{
	bool_t respond = false;
	uint8_t reset_requested = 0U;

	param = param;

	if ( request->iDataSize != 0U )
	{
		reset_requested = request->requestData[0];
	}

	switch ( reset_requested )
	{
		case SERVICES_SOFT_RESET:
			BF_Lib::Services::Execute_Service( SERVICES_SOFT_RESET );
			break;

		case SERVICES_FACTORY_RESET:
			BF_Lib::Services::Execute_Service( SERVICES_FACTORY_RESET );
			break;

		default:
			response->bGeneralStatus =
				static_cast<uint8_t>( ROUTER_ERROR_INVALID_PARAMETER );
			respond = true;
			break;
	}

	return ( respond );
}
