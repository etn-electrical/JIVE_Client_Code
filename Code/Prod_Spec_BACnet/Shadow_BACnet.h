/**
 **********************************************************************************************
 * @file            Shadow_BACnet.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef Shadow_BACnet_H
#define Shadow_BACnet_H
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Change_Tracker.h"
#include "Shadow.h"

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Shadow_BACnet
{
	public:
		Shadow_BACnet( void );
		~Shadow_BACnet()
		{}

	private:
		void Bacnet_Change_Track_Handler( DCI_ID_TD dci_id, UINT8 attribute );

		static void Bacnet_Change_Track_Handler_Static(
			Change_Tracker::cback_param_t handle,
			DCI_ID_TD dci_id,
			Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<Shadow_BACnet*>( handle )->Bacnet_Change_Track_Handler( dci_id,
																					 attribute_mask );
		}

		uint16_t getFieldbusCntrWord();

		DCI_SOURCE_ID_TD m_bacnet_source_id;
		DCI_Patron* m_bacnet_patron;

		Shadow* m_bacnet_shadow_ctrl;
		Change_Tracker* m_bacnet_change_tracker;

};

#endif

