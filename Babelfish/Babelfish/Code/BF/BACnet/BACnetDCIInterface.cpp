/**
 *****************************************************************************************
 *	@file BACnetDCIInterfacet.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "BACnetDCIInterface.h"
#include "OS_Tasker.h"

static const BACNET_TARGET_INFO_ST_TD* m_bacnet_target_info;
void Init_BACnetDCIInterface( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Init_BACnetDCIInterface( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info )
{
	m_bacnet_target_info = bacnet_target_info;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD getDCIDForDevObject( BACNET_ID devicePropertyNum )
{
	DCI_ID_TD result;

	if ( devicePropertyNum >= TOTAL_DEV )
	{
		result = DCI_TOTAL_DCIDS;	/*Assign invalid DCID value */
	}
	else
	{
		result = BACnet_DEV_to_DCID_map[devicePropertyNum].dcid;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD getDCIDForAVObject( BACNET_ID avInstanceNum )
{
	DCI_ID_TD result;

	if ( avInstanceNum >= TOTAL_AV )
	{
		result = DCI_TOTAL_DCIDS;	/*Assign invalid DCID value */
	}
	else
	{
		result = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[avInstanceNum].dcid;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD getDCIDForAIObject( BACNET_ID aiInstanceNum )
{
	DCI_ID_TD result;

	if ( aiInstanceNum >= TOTAL_AI )
	{
		result = DCI_TOTAL_DCIDS;	/*Assign invalid DCID value */
	}
	else
	{
		result = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[aiInstanceNum].dcid;
	}
	return ( result );
}

// Nishchal - MSV Implementation
DCI_ID_TD getDCIDForMSVObject( BACNET_ID msvInstanceNum )
{
	DCI_ID_TD result;

	if ( msvInstanceNum >= TOTAL_MSV )
	{
		result = DCI_TOTAL_DCIDS;	/*Assign invalid DCID value */
	}
	else
	{
		result = m_bacnet_target_info->class_list->BACnet_MSV_to_DCID_map[msvInstanceNum].dcid;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD getDCIDForBVObject( BACNET_ID bvInstanceNum, uint8_t& bitNum )
{
	DCI_ID_TD result;

	if ( bvInstanceNum >= TOTAL_BV )
	{
		result = DCI_TOTAL_DCIDS;	/*Assign invalid DCID value */
	}
	else
	{
		bitNum = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[bvInstanceNum].bitNumber;
		result = m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[bvInstanceNum].dcid;
	}
	return ( result );
}
