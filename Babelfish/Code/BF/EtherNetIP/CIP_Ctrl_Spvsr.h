/*
 ********************************************************************************
 *
 * $Author: Mark A Verheyen$
 * $Date: 2/27/2008 5:08:46 PM$
 * Copyright © Eaton Corporation. All Rights Reserved.
 * Creator: Mark A Verheyen
 *
 * Description:
 *	CIP Control Supervisor Trip and Reset service functionality.
 *   This module will handle the service request made to the Control
 *   Supervisor object.
 ********************************************************************************
 */
#ifndef CIP_CTRL_SPVSR_H
	#define CIP_CTRL_SPVSR_H

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */
typedef void* CIP_CTRL_SUP_RESET_PARAM;
typedef void CIP_CTRL_SUP_RESET_CBACK( CIP_CTRL_SUP_RESET_PARAM, bool_t );

/*
 ********************************************************************************
 *  Class Definition
   --------------------------------------------------------------------------------
 */
void CIP_CTRL_SPVSR_Init( CIP_CTRL_SUP_RESET_CBACK* cback,
						  CIP_CTRL_SUP_RESET_PARAM param );


#endif
