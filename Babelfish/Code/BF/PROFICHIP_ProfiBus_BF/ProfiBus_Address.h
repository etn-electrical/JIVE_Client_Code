/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROFIBUS_ADDRESS_H
   #define PROFIBUS_ADDRESS_H

#include "Dip_Switch.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class ProfiBus_Address
{
	public:
		ProfiBus_Address( Dip_Switch* dipswitch_ctrl, DCI_Owner* profi_address_owner );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~ProfiBus_Address() {}

	private:
		void Update_Address( void );		// Updates ProfiBus Address from dipswitches


		DCI_Owner* m_profi_address_owner;
		Dip_Switch* m_dipswitch_ctrl;
};



#endif
