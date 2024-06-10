/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Prod_Spec_NV_Refresh.h"
#include "NV_Refresh_List.h"
#include "DCI_Constants.h"
#include "DCI_Owner.h"
#include "Prod_Spec_Debug.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_NV_REFRESH_Init( void )
{
	uint16_t previous_dci_nv_map_ver = 0U;
	bool result = false;
	DCI_Owner* nv_map_version_num = nullptr;

	/* Initialize the NV MAP Version Number Owner */
	// coverity[leaked_storage]
	nv_map_version_num = new DCI_Owner( DCI_NV_MAP_VER_NUM_DCID );

	/* Read previous firmware version number */
	result = nv_map_version_num->Check_Out_Init( reinterpret_cast<uint8_t*>( &previous_dci_nv_map_ver ) );

	if ( result )
	{
		/* Compare previous firmware version number with DCI NV MAP version number  */
		if ( previous_dci_nv_map_ver != CONST_DCI_NV_MAP_VERSION_NUM )
		{
			if ( ver_update_package.rev_update_list != nullptr )
			{
				/* Get the status if change in version number */
				result = NV_Refresh::Update( previous_dci_nv_map_ver, &ver_update_package );
				if ( result )
				{
					/* Save current firmware version number */
					previous_dci_nv_map_ver = CONST_DCI_NV_MAP_VERSION_NUM;
					result =
						nv_map_version_num->Check_In_Init( reinterpret_cast<uint8_t*>( &previous_dci_nv_map_ver ) );
					if ( !result )
					{
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to update dci map version into nv" );
					}
					else
					{
						/* Soft Reset to update all new parameters values */
						BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );
					}
				}
			}
			else
			{
				/* Compare previous firmware version number with DCI NV MAP version number  */
				if ( previous_dci_nv_map_ver != CONST_DCI_NV_MAP_VERSION_NUM )
				{
					/* Save current firmware version number */
					previous_dci_nv_map_ver = CONST_DCI_NV_MAP_VERSION_NUM;
					result =
						nv_map_version_num->Check_In_Init( reinterpret_cast<uint8_t*>( &previous_dci_nv_map_ver ) );
					if ( !result )
					{
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to update dci map version into nv" );
					}
				}
			}
		}
	}
	// coverity[leaked_storage]
}