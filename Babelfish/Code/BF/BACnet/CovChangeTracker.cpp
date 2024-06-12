/*
 *****************************************************************************************
 *
 *		Copyright 2016, Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "CovChangeTracker.h"
#include "BACnetStackInterface.h"

/** @fn CovChangeTracker()
 *   @brief :Constructor to create instance of CovChangeTracker
 *   @details :Constructor to create instance of CovChangeTrackerT
 *   @param[in]:
 *   @return:
 */

CovChangeTracker::CovChangeTracker()
{
	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static,
										   reinterpret_cast<Change_Tracker::cback_param_t>( this ) );
}

/** @fn Change_Tracker_CB()
 *   @brief : To transmit COV when object instance value changes.
 *   @details : Function is used to transmit COV using DoSolicitedCOV() when
 *              registered instance value changes.
 *   @param[in]: dci_id, attribute
 *   @return:
 */

void CovChangeTracker::Change_Track_Handler( DCI_ID_TD dci_id,
											 Change_Tracker::attrib_mask_t attribute_mask )
{
	DoSolicitedCOV( dci_id );
}

/** @fn Track_ID(DCI_ID_TD)
 *   @brief : To transmit COV when object instance value changes.
 *   @details : Function is used to track DCID's registered by Subscribe COV service.
 *   @param[in]: dci_id
 *   @return:
 */

void CovChangeTracker::Track_ID( DCI_ID_TD dci_id )
{
	m_change_tracker->Track_ID( dci_id );
}

/** @fn UnTrack_ID(DCI_ID_TD)
 *   @brief : To transmit COV when object instance value changes.
 *   @details : Function is used to untrack ID's when de-registered from COV service.
 *   @param[in]: dci_id
 *   @return:
 */

void CovChangeTracker::UnTrack_ID( DCI_ID_TD dci_id )
{
	m_change_tracker->UnTrack_ID( dci_id );
}
