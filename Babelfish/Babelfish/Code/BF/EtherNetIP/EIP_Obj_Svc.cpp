/*
 ********************************************************************************
 *		$Workfile: EIP_Obj_Svc.cpp$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 3/5/2009 8:49:08 AM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *		$Header: EIP_Obj_Svc.h, 22, 3/5/2009 8:49:08 AM, Mark A Verheyen$
 ********************************************************************************
 */
#include "Includes.h"
#include "EIP_Obj_Svc.h"
#include "Ram.h"
#include "Linked_List.h"

/*
 ********************************************************************************
 *		Constants and typedefs
 ********************************************************************************
 */

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
static BF_Lib::Linked_List* m_linked_list =
	reinterpret_cast<BF_Lib::Linked_List*>( NULL );

/*
 ********************************************************************************
 *		Prototypes
 ********************************************************************************
 */

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
void EIP_OBJ_SVC_Attach( EIP_OBJ_SVC_DEF* svc_def )
{
	if ( m_linked_list == NULL )
	{
		m_linked_list = new BF_Lib::Linked_List();
	}

	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 5-2-6: Unusual pointer cast
	 * @n PCLint: 740
	 * @n Justification: While it is true that this is an unusual pointer cast,
	 *  it is necessary in order to stuff the object in the linked list payload.
	 */
	/*lint -e{740}*/
	m_linked_list->Add( reinterpret_cast<BF_Lib::Linked_List::LL_PAYLOAD*>(
							svc_def ) );
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
bool_t EIP_OBJ_SVC_Execute( EtIPObjectRequest* request,
							EtIPObjectResponse* response )
{
	EIP_OBJ_SVC_DEF const* svc_def =
		reinterpret_cast<EIP_OBJ_SVC_DEF const*>( NULL );
	bool_t respond = false;

	m_linked_list->Restart();
	do
	{
		svc_def = reinterpret_cast<EIP_OBJ_SVC_DEF const*>(
			m_linked_list->Get_And_Inc() );
	}while ( ( svc_def != NULL ) &&
			 ( !(
				   ( svc_def->cip_class == request->iClass ) &&
				   (
					   ( svc_def->cip_instance == request->iInstance ) ||
					   ( svc_def->cip_instance == 0U )
				   ) &&
				   ( svc_def->service_code == request->bService )
				   ) ) );

	if ( svc_def != reinterpret_cast<EIP_OBJ_SVC_DEF const*>( NULL ) )
	{
		respond = ( *svc_def->callback_func )( svc_def->cback_param,
											   request, response );
	}
	else
	{
		response->bGeneralStatus = static_cast<uint8_t>( ROUTER_ERROR_BAD_SERVICE );
		respond = true;
	}
	return ( respond );
}
