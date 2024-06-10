/*
 ********************************************************************************
 *		$Workfile: Shadow_Product.h$
 *
 *		$Author: Pranav$
 *		$Date: 17/09/2013 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 *		This file is under development. Code development & it's testing
 *       is not complete as of now.
 ********************************************************************************
 */

#ifndef SHADOW_PRODUCT_H
#define SHADOW_PRODUCT_H

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Change_Tracker.h"
#include "Shadow.h"
#include "Com_Dog.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------
 */
class Shadow_Product
{
	public:
		virtual ~Shadow_Product( void ) {}

		static DCI_Patron* m_patron;
		static Shadow* m_shadow_ctrl;
		static Change_Tracker* m_change_tracker;
		static bool_t ConnectionTimedOutEventSet;
};

#endif
