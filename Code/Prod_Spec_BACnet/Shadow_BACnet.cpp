/**
 **********************************************************************************************
 * @file            Shadow_BACnet.cpp  product specific initialization
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
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "includes.h"
#include "Shadow_BACnet.h"
#include "Babelfish_Assert.h"

/**
 **********************************************************************************************
 * @brief                     Function for Shadow_BACnet
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */

Shadow_BACnet::Shadow_BACnet( void )
{
	m_bacnet_patron = new DCI_Patron( FALSE );
	m_bacnet_source_id = DCI_SOURCE_IDS_Get();

	m_bacnet_shadow_ctrl = new Shadow( m_bacnet_patron, m_bacnet_source_id );
	m_bacnet_change_tracker = new Change_Tracker( &Bacnet_Change_Track_Handler_Static,
												  reinterpret_cast<Change_Tracker::cback_param_t>( this ), true );

	m_bacnet_patron->Attach_Tracker( m_bacnet_change_tracker );
	/*  create owner -- Attach Callback -- Track Change    */
	// m_bacnet_change_tracker->Track_ID( DCI_ubRS485CommSet_DCID );
}

/**
 **********************************************************************************************
 * @brief                     Function for Bacnet_Change_Track_Handler
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Shadow_BACnet::Bacnet_Change_Track_Handler( DCI_ID_TD dci_id, UINT8 attribute )
{
	switch ( dci_id )
	{
		// case DCI_ubRS485CommSet_DCID:
		break;

		default:
			// This indicates that a value change for a parameter we are supposedly watching doesn't exist.
			BF_ASSERT( false );
			break;
	}
}
