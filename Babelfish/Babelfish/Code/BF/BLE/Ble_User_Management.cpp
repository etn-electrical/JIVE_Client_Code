/**
 *****************************************************************************************
 *   @file
 *   @details See header file for module overview.
 *   @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Ble_User_Management.h"
#include "BLE_DCI.h"
#include "BLE_Debug.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_User_Management::Ble_User_Management( ble_user_management_t const* user_entry_config,
										  ble_user_management_t const* role_based_config,
										  ble_user_management_t const* user_name_config,
										  ble_user_management_t const* user_pswd_config,
										  ble_user_management_t const* user_role_config ) :
	m_user_entry( user_entry_config ),
	m_role_based( role_based_config ),
	m_user_name( user_name_config ),
	m_user_password( user_pswd_config ),
	m_user_role( user_role_config ),
	m_ble_active_access_level_owner( new DCI_Owner( DCI_BLE_ACTIVE_ACCESS_LEVEL_DCID ) )
{
	Create_Usermanagement_Dcid_Owners_With_Callback( m_ble_roles_owner, m_role_based );
	Create_Usermanagement_Dcid_Owners( m_ble_user_entry_owner, m_user_entry );
	Create_Usermanagement_Dcid_Owners( m_ble_user_names_owner, m_user_name );
	Create_Usermanagement_Dcid_Owners_With_Callback( m_ble_user_password_owner, m_user_password );
	Create_Usermanagement_Dcid_Owners( m_ble_user_role_owner, m_user_role );

	m_ble_user_entry_owner[BLE_USER_NAME_ENTRY]->Attach_Callback(
		&Validate_UserName_Callback_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_GET_RAM_CMD_MSK );
	m_ble_user_entry_owner[BLE_USER_PSWD_ENTRY]->Attach_Callback(
		&Validate_UserPassword_Callback_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_GET_RAM_CMD_MSK | DCI_ACCESS_GET_INIT_CMD_MSK
		| DCI_ACCESS_GET_DEFAULT_CMD_MSK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_User_Management::~Ble_User_Management()
{
	m_user_entry = reinterpret_cast<ble_user_management_t const*>( nullptr );
	m_role_based = reinterpret_cast<ble_user_management_t const*>( nullptr );
	m_user_name = reinterpret_cast<ble_user_management_t const*>( nullptr );
	m_user_password = reinterpret_cast<ble_user_management_t const*>( nullptr );
	m_user_role = reinterpret_cast<ble_user_management_t const*>( nullptr );

	uint8_t loop_var;

	for ( loop_var = 0; loop_var < BLE_NO_OF_USER_ENTRY_PARAMS; loop_var++ )
	{
		delete m_ble_user_entry_owner[loop_var];
	}

	for ( loop_var = 0; loop_var < BLE_NO_OF_ROLES; loop_var++ )
	{
		delete m_ble_roles_owner[loop_var];
	}

	for ( loop_var = 0; loop_var < BLE_NO_OF_USERS; loop_var++ )
	{
		delete m_ble_user_names_owner[loop_var];
		delete m_ble_user_password_owner[loop_var];
		delete m_ble_user_role_owner[loop_var];
	}

	delete m_ble_active_access_level_owner;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Validate_UserName_Callback_Static( DCI_CBACK_PARAM_TD handle,
																	  DCI_ACCESS_ST_TD* access_struct )
{
	Ble_User_Management* object_ptr = static_cast<Ble_User_Management*>( handle );

	return ( object_ptr->Validate_UserName_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Validate_UserPassword_Callback_Static( DCI_CBACK_PARAM_TD handle,
																		  DCI_ACCESS_ST_TD* access_struct )
{
	Ble_User_Management* object_ptr = static_cast<Ble_User_Management*>( handle );

	return ( object_ptr->Validate_UserPassword_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Ble_Users_Hide_Password_Callback_Static( DCI_CBACK_PARAM_TD handle,
																			DCI_ACCESS_ST_TD* access_struct )
{
	Ble_User_Management* object_ptr = static_cast<Ble_User_Management*>( handle );

	return ( object_ptr->Ble_Users_Hide_Password_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Validate_UserName_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	memset( user_name_entry, 0, sizeof( user_name_entry ) );

	/* check if command is valid */
	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		memcpy( user_name_entry, access_struct->data_access.data, access_struct->data_access.length );
	}
	else if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
	{
		return_status = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Validate_UserPassword_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	static char user_entry_passwd[BLE_USER_CREDENTIALS_MAX_LENGTH];
	uint8_t loop_var;
	uint8_t user_name_validity = ACCESS_NOT_VALID;
	uint8_t active_access_level = ACCESS_NOT_VALID;
	uint8_t access_level_enum = ACCESS_NOT_VALID;
	static char valid_usernames[BLE_NO_OF_USERS][BLE_USER_CREDENTIALS_MAX_LENGTH];
	static char valid_password[BLE_USER_CREDENTIALS_MAX_LENGTH];
	static char valid_roles_pswd[BLE_NO_OF_ROLES][BLE_USER_CREDENTIALS_MAX_LENGTH];

	for ( loop_var = 0; loop_var < BLE_NO_OF_USERS; loop_var++ )
	{
		memset( valid_usernames[loop_var], 0, sizeof( valid_usernames[0] ) );
	}

	for ( loop_var = 0; loop_var < BLE_NO_OF_ROLES; loop_var++ )
	{
		memset( valid_roles_pswd[loop_var], 0, sizeof( valid_roles_pswd[0] ) );
	}

	memset( user_entry_passwd, 0, sizeof( user_entry_passwd ) );
	memset( valid_password, 0, sizeof( valid_password ) );

	for ( loop_var = 0; loop_var < BLE_NO_OF_USERS; loop_var++ )
	{
		m_ble_user_names_owner[loop_var]->Check_Out( &valid_usernames[loop_var] );
	}

	/* check if command is valid */
	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		memcpy( user_entry_passwd, access_struct->data_access.data,
				access_struct->data_access.length );

		for ( loop_var = 0; loop_var < BLE_NO_OF_USERS; loop_var++ )
		{
			if ( strcmp( valid_usernames[loop_var], user_name_entry ) == 0 )
			{
				m_ble_user_password_owner[loop_var]->Check_Out( &valid_password );
				user_name_validity = loop_var;
				break;
			}
		}

		if ( user_name_validity != ACCESS_NOT_VALID )
		{
			if ( strcmp( valid_password, user_entry_passwd ) == 0 )
			{
				m_ble_user_role_owner[user_name_validity]->Check_Out( &access_level_enum );
				if ( access_level_enum < BLE_NO_OF_ROLES )
				{
					active_access_level = m_role_access_levels_list[access_level_enum];
				}
				else
				{
					BLE_DEBUG_PRINT( DBG_MSG_INFO, "access_level invalid\n" );
				}
			}
		}

		if ( active_access_level == ACCESS_NOT_VALID )
		{

			for ( loop_var = 0; loop_var < BLE_NO_OF_ROLES; loop_var++ )
			{
				m_ble_roles_owner[loop_var]->Check_Out( &valid_roles_pswd[loop_var] );
				if ( strcmp( valid_roles_pswd[loop_var], user_entry_passwd ) == 0 )
				{
					active_access_level = m_role_access_levels_list[loop_var];
					break;
				}
			}

			if ( active_access_level == ACCESS_NOT_VALID )
			{
				return_status = DCI_CBack_Encode_Error( DCI_ERR_ACCESS_VIOLATION );
			}
		}

		if ( return_status == DCI_CBACK_RET_PROCESS_NORMALLY )
		{
			m_ble_active_access_level_owner->Check_In( active_access_level );
			BLE_DEBUG_PRINT( DBG_MSG_INFO, "Access Level : %d\n", active_access_level );
		}
	}
	else if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
	{
		uint8_t* data_buffer = reinterpret_cast<uint8_t*>( access_struct->data_access.data );

		memset( data_buffer, '*', access_struct->data_access.length );
		data_buffer[access_struct->data_access.length] = '\0';
		return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Ble_User_Management::Ble_Users_Hide_Password_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	if ( ( access_struct->command == DCI_ACCESS_GET_RAM_CMD ) ||
		 ( access_struct->command == DCI_ACCESS_GET_INIT_CMD ) ||
		 ( access_struct->command == DCI_ACCESS_GET_DEFAULT_CMD ) )
	{
		uint8_t* data_buffer = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
		memset( data_buffer, '*', access_struct->data_access.length );
		data_buffer[access_struct->data_access.length] = '\0';
		return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
	}
	else
	{
		// misra
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_User_Management::Create_Usermanagement_Dcid_Owners( DCI_Owner** m_ble_user_mgmt_owners,
															 ble_user_management_t const* user_mgmt_dcid_group )
{
	uint8_t loop_var;

	for ( loop_var = 0; loop_var < user_mgmt_dcid_group->m_total_user_mgmt_dcids; loop_var++ )
	{
		m_ble_user_mgmt_owners[loop_var] =
			new DCI_Owner( user_mgmt_dcid_group->m_user_mgmt_dcid[loop_var] );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ble_User_Management::Create_Usermanagement_Dcid_Owners_With_Callback( DCI_Owner** m_ble_user_mgmt_owners,
																		   ble_user_management_t const* user_mgmt_dcid_group )
{
	uint8_t loop_var;

	for ( loop_var = 0; loop_var < user_mgmt_dcid_group->m_total_user_mgmt_dcids; loop_var++ )
	{
		m_ble_user_mgmt_owners[loop_var] =
			new DCI_Owner( user_mgmt_dcid_group->m_user_mgmt_dcid[loop_var] );

		m_ble_user_mgmt_owners[loop_var]->Attach_Callback(
			&Ble_Users_Hide_Password_Callback_Static,
			reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
			DCI_ACCESS_GET_RAM_CMD_MSK | DCI_ACCESS_GET_INIT_CMD_MSK |
			DCI_ACCESS_GET_DEFAULT_CMD_MSK );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ble_User_Management::Get_Active_Access_Level( void )
{
	uint8_t access_level = 0U;

	m_ble_active_access_level_owner->Check_Out( access_level );
	return ( access_level );
}
