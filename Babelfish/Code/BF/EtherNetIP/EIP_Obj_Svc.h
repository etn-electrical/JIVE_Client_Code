/*
 ********************************************************************************
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 2/27/2008 5:08:46 PM$
 *		Copyright © Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 ********************************************************************************
 */
#ifndef EIP_OBJ_SERVICE_H
	#define EIP_OBJ_SERVICE_H

#include "EtherNetIP_DCI_Defines.h"
#include "eipinc.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *  Functions
   --------------------------------------------------------------------------------
 */
typedef void* EIP_OBJ_SVC_CBPARAM;
typedef bool_t EIP_OBJ_SVC_CBACK( EIP_OBJ_SVC_CBPARAM param,
								  EtIPObjectRequest* request,
								  EtIPObjectResponse* response );

typedef struct EIP_OBJ_SVC_DEF
{
	uint16_t service_code;
	CIP_CLASS_TD cip_class;
	CIP_INSTANCE_TD cip_instance;
	EIP_OBJ_SVC_CBACK* callback_func;
	EIP_OBJ_SVC_CBPARAM cback_param;
} EIP_OBJ_SVC_DEF;


void EIP_OBJ_SVC_Attach( EIP_OBJ_SVC_DEF* svc_def );

bool_t EIP_OBJ_SVC_Execute( EtIPObjectRequest* request,
							EtIPObjectResponse* response );

#endif
