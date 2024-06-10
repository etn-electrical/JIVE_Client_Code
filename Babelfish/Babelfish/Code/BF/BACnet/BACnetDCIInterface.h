/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BACNETDCIINTERFACE_H
#define BACNETDCIINTERFACE_H

#include "includes.h"
#include "DCI_Defines.h"
// #include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "DCI_Patron.h"
#include "Base_DCI_BACnet_Data.h"
// class BACnetDCI{
//
// public:
//
// BACnetDCI( DCI_Owner* owner);
//
// static DCI_CB_RET_TD  DCI_BACnetEnabled_CBack_Static( DCI_CBACK_PARAM_TD handle, DCI_ACCESS_ST_TD* access_struct )
// { return ( ((BACnetDCI*)handle)->DCI_BACnet_Enabled_CBack( access_struct ) ); }
//
// DCI_Owner* m_enable_Bacnet_owner;
// DCI_Patron*  m_enable_Bacnet_patron;
// DCI_CB_RET_TD DCI_BACnet_Enabled_CBack( DCI_ACCESS_ST_TD* access_struct );
//
// };

DCI_ID_TD getDCIDForAVObject( BACNET_ID avInstanceNum );

DCI_ID_TD getDCIDForAIObject( BACNET_ID aiInstanceNum );

DCI_ID_TD getDCIDForBVObject( BACNET_ID bvInstanceNum, uint8_t& bitNum );

DCI_ID_TD getDCIDForDevObject( BACNET_ID devicePropertyNum );

DCI_ID_TD getDCIDForMSVObject( BACNET_ID msvInstanceNum );	// Nishchal -MSV Implementation

#endif
