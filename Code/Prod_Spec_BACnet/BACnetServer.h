/**
 **********************************************************************************************
 * @file            BACnetServer.h  product specific initialization
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
#include "OS_TASK.h"
#include "OS_Tasker.h"
#include "Timers_Lib.h"
#include "DCI_Defines.h"
#include "DCI_Patron.h"
#include "Base_DCI_BACnet_Data.h"

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */

#define FR_MAIN_UPDATE_INTERVAL 10
#define FR_WORK_UPDATE_INTERVAL 5

class BACnetServer
{
	public:

		BACnetServer( const BACNET_TARGET_INFO_ST_TD* bacnet_dev_profile, bool_t ip_enable );
		~BACnetServer( void )
		{}

		void BACnetfrMain();

		static void BACnetfrMainStatic( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( BACnetServer* )handle )->BACnetfrMain();
		}

		void BACnetFrWork();

		static void BACnetfrWorkStatic( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( BACnetServer* )handle )->BACnetFrWork();
		}

	private:

		DCI_SOURCE_ID_TD m_BACnet_Server_source_id;
		DCI_Patron* m_BACnet_Server_Patron;
		static Change_Tracker* m_bacnetip_tracker;
		static BACNET_TARGET_INFO_ST_TD const* m_eip_target_info;
		static bool_t m_ip_enable;
		void BACNet_IP_Callback( DCI_ID_TD dci_id );

		static void BACNet_IP_Callback_Static(
			Change_Tracker::cback_param_t handle,
			DCI_ID_TD dci_id,
			Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<BACnetServer*>( handle )->BACNet_IP_Callback( dci_id );
		}

};
