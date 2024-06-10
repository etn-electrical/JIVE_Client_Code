/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2015 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "EIP_Device_Identity.h"
#include "NV_Mem.h"
#include "Services.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
EIP_Device_Identity::EIP_Device_Identity( device_config_t const* config ) :
	m_config( config ),
	m_product_code_owner( new DCI_Owner( m_config->m_product_code_dcid ) ),
	m_product_subcode_owner( new DCI_Owner( m_config->m_product_subcode_dcid ) ),
	m_firmware_revision_owner( new DCI_Owner( m_config->m_firmware_revision_dcid ) ),
	m_hardware_revision_owner( new DCI_Owner( m_config->m_hardware_revision_dcid ) ),
	m_product_serial_number_owner( new DCI_Owner( m_config->m_serial_number_dcid ) )
{
	m_product_subcode_owner->Attach_Callback(
		&Write_Protect_Callback_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK );
	m_hardware_revision_owner->Attach_Callback(
		&Write_Protect_Callback_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK );
	m_product_serial_number_owner->Attach_Callback(
		&Write_Protect_Callback_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
EIP_Device_Identity::~EIP_Device_Identity()
{
	m_config = reinterpret_cast<device_config_t const*>( nullptr );
	delete m_product_code_owner;
	delete m_product_subcode_owner;
	delete m_firmware_revision_owner;
	delete m_hardware_revision_owner;
	delete m_product_serial_number_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_Device_Identity::Wipe_NV_Preserve_Identity( void )
{
	/*this service is used to erase NV memory completely preserving the DCIs which are listed in
	   PROD_SPEC_SERVICES_Wipe_NV_SVC of Prod_Spec_Services.cpp */
	BF_Lib::Services::Execute_Service( SERVICES_APP_WIPE_NV_MEM );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint16_t EIP_Device_Identity::Get_Product_Code( void ) const
{
	uint16_t result;

	m_product_code_owner->Check_Out( result );
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint16_t EIP_Device_Identity::Get_Product_Subcode( void ) const
{
	uint16_t result;

	m_product_subcode_owner->Check_Out( result );
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint32_t EIP_Device_Identity::Get_Firmware_Revision( void ) const
{
	BF_Lib::SPLIT_UINT32 result;

	m_firmware_revision_owner->Check_Out( result.u16 );
	return ( result.u32 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint16_t EIP_Device_Identity::Get_Hardware_Revision( void ) const
{
	uint16_t result;

	m_hardware_revision_owner->Check_Out( result );
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

uint32_t EIP_Device_Identity::Get_Serial_Number( void ) const
{
	uint32_t result;

	m_product_serial_number_owner->Check_Out( result );
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD EIP_Device_Identity::Write_Protect_Callback_Static( DCI_CBACK_PARAM_TD handle,
																  DCI_ACCESS_ST_TD* access_struct )
{
	EIP_Device_Identity* object_ptr = static_cast<EIP_Device_Identity*>( handle );

	return ( object_ptr->Write_Protect_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD EIP_Device_Identity::Write_Protect_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status;
	DCI_Owner* dci_owner = reinterpret_cast<DCI_Owner*>( nullptr );

	if ( BF_Lib::Services::MFG_Access_Unlocked() )
	{
		if ( access_struct->data_id == m_config->m_product_subcode_dcid )
		{
			dci_owner = m_product_subcode_owner;
		}
		else if ( access_struct->data_id == m_config->m_hardware_revision_dcid )
		{
			dci_owner = m_hardware_revision_owner;
		}
		else if ( access_struct->data_id == m_config->m_serial_number_dcid )
		{
			dci_owner = m_product_serial_number_owner;
		}
		else
		{
			// punt it up to our super-class for resolution (or nullptr)
			dci_owner = Get_DCI_Owner( access_struct->data_id );
		}

		if ( dci_owner != reinterpret_cast<DCI_Owner*>( nullptr ) )
		{
			dci_owner->Check_In_Offset( access_struct->data_access.data,
										access_struct->data_access.offset,
										access_struct->data_access.length );
			dci_owner->Load_Current_To_Init();
		}
		return_status = DCI_CBACK_RET_NO_FURTHER_PROC_VAL_CHANGE;
	}
	else
	{
		return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Owner* EIP_Device_Identity::Get_DCI_Owner( DCI_ID_TD dcid )
{
	BF_Lib::Unused<DCI_ID_TD>::Okay( dcid );

	return ( reinterpret_cast<DCI_Owner*>( nullptr ) );
}
