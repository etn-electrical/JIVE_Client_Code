/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "NV_Refresh_List.h"
#include "DCI.h"
#include "NV_Address.h"

#define DCI_ARRAY_SIZE( array )             ( sizeof( array ) / sizeof( DCI_ID_TD ) )
/**
 *****************************************************************************************
 * Below is an example structure definition.
 * 1. User needs to add the DCI_ID_TD revision array with the newly added NV parameter corrosponding to that
      particular release, e.g. "dci_nv_init_ver_1" release has added the NV parameter - Log Spec Version DCIs.
   2. User needs to append rev_update_list array with corrosponding revision number,  size of new DCI item &
      DCI change list.
      This feature reads the current firmware version & compares it with the list of firmware revisions newly added.If
 * found greater versions, then, it selectively apply the factory default values which are listed with the new
 * firmware DCI list.
      Thus,user need not to perform a factory default operation as it's internally applied by NV refresh functionality.
 */

const DCI_ID_TD dci_nv_init_ver_1[] =
{
	DCI_DATA_LOG_SPEC_VERSION_DCID,
	DCI_EVENT_LOG_SPEC_VERSION_DCID,
	DCI_AUDIT_POWER_LOG_SPEC_VERSION_DCID,
	DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_DCID,
	DCI_AUDIT_USER_LOG_SPEC_VERSION_DCID,
	DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_DCID
};

const NV_Refresh::data_update_t ver_update_list[] =
{
	{
		1,						// Version number that this change goes with.
		DCI_ARRAY_SIZE( dci_nv_init_ver_1 ),		// Size of the new DCI items list.
		dci_nv_init_ver_1				// The DCI change list.
	}
};

const NV_Refresh::data_update_package_t ver_update_package =
{
	ver_update_list,
	static_cast<uint16_t>( sizeof( ver_update_list ) / sizeof( NV_Refresh::data_update_t ) )
};