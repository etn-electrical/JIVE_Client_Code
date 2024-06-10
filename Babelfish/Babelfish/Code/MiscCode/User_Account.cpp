/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "User_Account.h"
#include "StdLib_MV.h"
#include "DCI_Enums.h"
#include "Babelfish_Assert.h"
#include "User_Account_Debug.h"
/*
 *****************************************************************************************
 *		Static Variable
 *****************************************************************************************
 */
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
User_Account::User_Account( const dci_user_mgmt_t* user_accounts_dci,
							const uint8_t num_user_accounts,
							const auth_struct_t* auth_role_struct,
							const uint8_t number_of_auth_roles,
							const DCI_Owner* valid_bit_list,
							const DCI_Owner* device_password ) :
	m_user_acc_dci_struct( user_accounts_dci ),
	m_num_user_accounts( num_user_accounts ),
	m_auth_role_dci_struct( auth_role_struct ),
	m_num_auth_roles( number_of_auth_roles ),
	m_valid_bit_list_owner( valid_bit_list ),
	m_user_full_name_len( 0U ),
	m_user_name_len( 0U ),
	m_pwd_len( 0U ),
	m_device_pwd_scheme( 0U ),
	m_default_password( device_password )
{
	uint_fast8_t i;

	m_user_account_owner_list = new user_account_owner_t[m_num_user_accounts];

	// Create a owner to read default Device password setting from DCID
	DCI_Owner* pwd_setting = new DCI_Owner( DCI_DEVICE_PWD_SETTING_DCID );

	pwd_setting->Check_Out( m_device_pwd_scheme );

	// Halt if m_default_password is null.
	BF_ASSERT( m_default_password );

	/* create user account list based on number of user accounts */
	for ( i = 0; i < m_num_user_accounts; i++ )
	{
		m_user_account_owner_list[i].full_name =
			new DCI_Owner( m_user_acc_dci_struct[i].full_name );

		m_user_account_owner_list[i].user_name =
			new DCI_Owner( m_user_acc_dci_struct[i].user_name );

		m_user_account_owner_list[i].password = new DCI_Owner( m_user_acc_dci_struct[i].password );

		m_user_account_owner_list[i].password->Attach_Callback( User_Cred_Data_Cb_Static,
																this,
																DCI_ACCESS_GET_RAM_CMD_MSK |
																DCI_ACCESS_GET_INIT_CMD_MSK |
																DCI_ACCESS_GET_DEFAULT_CMD_MSK );
		m_user_account_owner_list[i].role = new DCI_Owner( m_user_acc_dci_struct[i].role );

		m_user_account_owner_list[i].pwd_set_epoch_time = new DCI_Owner(
			m_user_acc_dci_struct[i].pwd_set_epoch_time );

		m_user_account_owner_list[i].last_login_epoch_time = new DCI_Owner(
			m_user_acc_dci_struct[i].last_login_epoch_time );

		m_user_account_owner_list[i].pwd_complexity = new DCI_Owner(
			m_user_acc_dci_struct[i].pwd_complexity );

		m_user_account_owner_list[i].pwd_aging =
			new DCI_Owner( m_user_acc_dci_struct[i].pwd_aging );

		// Process password change setting on this user
		Process_Pwd_Setting( i, m_device_pwd_scheme );
	}

	m_user_full_name_len = m_user_account_owner_list[0].full_name->Get_Length();
	m_user_name_len = m_user_account_owner_list[0].user_name->Get_Length();
	m_pwd_len = m_user_account_owner_list[0].password->Get_Length();
	if ( m_pwd_len > PWD_SIZE_LIMIT )
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Account::Verify_User_Name( const uint8_t* user_name, uint8_t length )
{
	uint_fast8_t i;
	uint8_t return_index = UNKNOWN_ERROR;
	uint16_t bitmap = 0U;
	uint8_t* temp_data = new uint8_t[m_user_name_len];

	memset( temp_data, 0, m_user_name_len );

	if ( temp_data != nullptr )
	{
		if ( length <= m_user_name_len )
		{
			bitmap = Get_User_Account_Valid_Bit_List();

			if ( bitmap != UNKNOWN_ERROR )
			{
				for ( i = 0; i < m_num_user_accounts; i++ )
				{
					if ( ( bitmap & static_cast<uint16_t>( 1U << i ) ) != 0U )
					{
						if ( m_user_account_owner_list[i].user_name->Check_Out( temp_data ) )
						{
							if ( ( length ==
								   BF_Lib::Get_String_Length( temp_data, m_user_name_len ) ) &&
								 ( strncmp( reinterpret_cast<const char*>( temp_data ),
											reinterpret_cast<const char*>( user_name ),
											length ) == 0U ) )
							{
								return_index = i;
								break;
							}
							else
							{
								return_index = UNKNOWN_ERROR;
							}
						}
					}
				}
			}
		}
		else
		{
			return_index = UNKNOWN_ERROR;
		}

		delete[] temp_data;
	}
	return ( return_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Is_Password_Valid( uint8_t user_index, const uint8_t* pwd, uint8_t length )
{
	bool return_status = false;
	uint8_t buffer_len = 0;

	if ( ( length > m_pwd_len ) && ( user_index > m_num_user_accounts ) )
	{
		return_status = false;
	}
	else
	{
		buffer_len = m_user_account_owner_list[user_index].password->Get_Length();
		uint8_t* temp_data = new uint8_t[buffer_len];
		memset( temp_data, 0, buffer_len );
		if ( temp_data != nullptr )
		{
			if ( Read_User_Account_Info( m_user_account_owner_list[user_index].password, temp_data,
										 length ) )
			{
				if ( strncmp( reinterpret_cast<const char*>( temp_data ),
							  reinterpret_cast<const char*>( pwd ), length ) == 0 )
				{
					return_status = true;
				}
				else
				{
					return_status = false;
				}

			}

			delete[] temp_data;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Account::Get_User_Role( uint8_t user_index )
{
	uint8_t role = 0U;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].role, &role,
									 sizeof( role ) ) != true )
		{
			role = UNKNOWN_ERROR;	// error value
		}
	}
	else
	{
		role = UNKNOWN_ERROR;	// error value
	}
	return ( role );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Account::Get_Default_User_Role( uint8_t user_index )
{
	uint8_t role = 0U;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].role, &role,
											 sizeof( role ) ) != true )
		{
			role = UNKNOWN_ERROR;	// error value
		}
	}
	else
	{
		role = UNKNOWN_ERROR;	// error value
	}
	return ( role );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_User_Role( uint8_t user_index, uint8_t* role, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length >= sizeof( uint8_t ) ) )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].role, role, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_name_len ) )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].user_name, user_name,
									 length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Default_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_name_len ) )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].user_name, user_name, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_name_len ) && ( length >= MIN_USERNAME_LENGTH ) )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].user_name, user_name,
									  length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Password( uint8_t user_index, uint8_t* pwd, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_pwd_len ) )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].password, pwd, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Default_Password( uint8_t user_index, uint8_t* pwd, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_pwd_len ) )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].password, pwd, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_Password( uint8_t user_index, const uint8_t* pwd, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_pwd_len ) )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].password, pwd,
									  length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Password_Set_Epoch_Time( uint8_t user_index, uint8_t* pwd_set_epoch_time,
												uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].pwd_set_epoch_time,
									 pwd_set_epoch_time, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_Password_Epoch_Time( uint8_t user_index, uint8_t* pwd_set_epoch_time,
											uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].pwd_set_epoch_time,
									  pwd_set_epoch_time, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Last_Login_Epoch_Time( uint8_t user_index, uint8_t* last_login_epoch_time,
											  uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].last_login_epoch_time,
									 last_login_epoch_time, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_Last_Login_Epoch_Time( uint8_t user_index, uint8_t* last_login_epoch_time,
											  uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].last_login_epoch_time,
									  last_login_epoch_time, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity,
											uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].pwd_complexity,
									 pwd_complexity, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Default_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity,
													uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].pwd_complexity,
											 pwd_complexity, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity,
											uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].pwd_complexity,
									  pwd_complexity, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].pwd_aging, pwd_aging,
									 length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Default_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].pwd_aging, pwd_aging,
											 length ) )
		{
			return_status = true;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length )
{
	bool return_status = false;

	if ( user_index < m_num_user_accounts )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].pwd_aging, pwd_aging,
									  length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_User_Fullname( uint8_t user_index, uint8_t* full_name, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_full_name_len ) )
	{
		if ( Read_User_Account_Info( m_user_account_owner_list[user_index].full_name, full_name,
									 length ) )
		{
			return_status = true;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_Default_User_Fullname( uint8_t user_index, uint8_t* user_fullname, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_full_name_len ) )
	{
		if ( Read_Default_User_Account_Info( m_user_account_owner_list[user_index].full_name, user_fullname, length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_User_Fullname( uint8_t user_index, uint8_t* full_name, uint8_t length )
{
	bool return_status = false;

	if ( ( user_index < m_num_user_accounts ) && ( length <= m_user_full_name_len ) )
	{
		if ( Write_User_Account_Info( m_user_account_owner_list[user_index].full_name, full_name,
									  length ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t User_Account::Get_User_Account_Valid_Bit_List( void )
{
	uint16_t bitmap = 0U;

	if ( Read_User_Account_Info( m_valid_bit_list_owner, reinterpret_cast<uint8_t*>( &bitmap ),
								 sizeof( uint16_t ) ) == false )
	{
		bitmap = UNKNOWN_ERROR;
	}
	return ( bitmap );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Set_User_Account_Valid_Bit_List( uint16_t bitmap, uint8_t user_index )
{
	bool return_status = false;

	bitmap |= ( 1U << user_index );
	if ( Write_User_Account_Info( m_valid_bit_list_owner, reinterpret_cast<uint8_t*>( &bitmap ),
								  sizeof( uint16_t ) ) )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Clear_User_Account_Valid_Bit_List( uint16_t bitmap, uint8_t user_index )
{
	bool return_status = false;

	bitmap &= ~( 1U << user_index );
	if ( Write_User_Account_Info( m_valid_bit_list_owner, reinterpret_cast<uint8_t*>( &bitmap ),
								  sizeof( uint16_t ) ) )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Clear_All_User_Account_Valid_Bit_List( void )
{
	bool return_status = false;

	uint16_t bitmap = 0U;

	if ( Write_User_Account_Info( m_valid_bit_list_owner, reinterpret_cast<uint8_t*>( &bitmap ),
								  sizeof( uint16_t ) ) )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Read_User_Account_Info( const DCI_Owner* user_owner_info, uint8_t* data,
										   uint8_t length )
{
	bool return_status = false;

	if ( user_owner_info->Check_Out( data ) )
	{
		return_status = true;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Read_Default_User_Account_Info( const DCI_Owner* user_owner_info,
												   uint8_t* data,
												   uint8_t length )
{
	bool return_status = false;

	if ( user_owner_info->Check_Out_Default( data ) )
	{
		return_status = true;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Write_User_Account_Info( const DCI_Owner* user_owner_info, const uint8_t* data,
											uint8_t length )
{
	bool return_status = false;

	if ( user_owner_info->Check_In( data ) )
	{
		if ( user_owner_info->Check_In_Init( data ) )
		{
			return_status = true;
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD User_Account::User_Cred_Data_Cb( DCI_ACCESS_ST_TD* access_struct )
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
bool User_Account::Set_Pwd_Scheme_Bit( uint_fast8_t user_id,  uint8_t pwd_Scheme )
{
	bool ret_status = false;
	uint8_t pwd_complexity_val = 0U;

	ret_status = Get_Password_Complexity( user_id, &pwd_complexity_val, 0 );
	if ( true == ret_status )
	{
		// Check for the password scheme
		if ( ( pwd_Scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_COMMON_PWD_CHANGE ) ||
			 ( pwd_Scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_UNIQUE_PWD_CHANGE ) )
		{
			SET_PWD_CHANGE( pwd_complexity_val );
		}
		else
		{
			SET_PWD_WARNING( pwd_complexity_val );
		}
	}

	ret_status = Set_Password_Complexity( user_id, &pwd_complexity_val, 0 );
	if ( false == ret_status )
	{
		USER_ACC_PRINT( DBG_MSG_INFO, "Unable to set pwd complexity User:%d", user_id );
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void User_Account::Process_Pwd_Setting( uint_fast8_t user_id, uint8_t pwd_Scheme )
{
	uint8_t cur_pwd[PWD_SIZE_LIMIT + 1U] = {'\0'};
	uint8_t default_pwd[PWD_SIZE_LIMIT + 1U] = {'\0'};
	uint8_t user_name[USER_NAME_LIMIT + 1U] = {"\0"};

	// Get default user name to check if this user is default user or not
	if ( true == Get_Default_User_Name( user_id, user_name, 0U ) )
	{
		// Check if user name is empty, this is to identify if this user is a default user
		if ( false == BF_Lib::String_Compare( user_name, reinterpret_cast<const uint8_t*>( "" ), 1U ) )
		{
			// Get default password for this user
			if ( true == Get_Default_Password( user_id, default_pwd, 0U ) )
			{
				// Check for default password value. This value should be empty as per design of California Oregon Lawe
				// implementation
				// If not empty, hard fault will be triggered
				if ( true == BF_Lib::String_Compare( default_pwd, reinterpret_cast<const uint8_t*>( "" ), 1U ) )
				{
					// Check for current value of user password
					if ( true == Get_Password( user_id, cur_pwd, 0U ) )
					{
						if ( true == BF_Lib::String_Compare( cur_pwd, reinterpret_cast<const uint8_t*>( "" ), 1U ) )
						{
							// Check if password scheme is common password change, use unique password DCID for default
							// password
							if ( pwd_Scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_COMMON_PWD_CHANGE )
							{
								// Read password from unique DCID
								if ( true == m_default_password->Check_Out_Default( default_pwd ) )
								{
									if ( true == m_user_account_owner_list[user_id].password->Check_In( default_pwd ) )
									{
										if ( true ==
											 m_user_account_owner_list[user_id].password->Check_In_Init( default_pwd ) )
										{
											Set_Pwd_Scheme_Bit( user_id, pwd_Scheme );
											USER_ACC_PRINT( DBG_MSG_INFO, "Updated current password for user %d",
															user_id );
										}
									}
								}
							}
							else if ( ( pwd_Scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_UNIQUE_PWD_CHANGE ) ||
									  ( pwd_Scheme == DCI_DEVICE_PWD_SETTING_ENUM::DCI_UNIQUE_PWD_NO_CHANGE ) )
							{
								if ( true == m_default_password->Check_Out_Default( default_pwd ) )
								{
									( void ) m_default_password->Check_Out( cur_pwd );
									if ( false ==
										 BF_Lib::String_Compare( cur_pwd, default_pwd,
																 static_cast<uint32_t>( PWD_SIZE_LIMIT ) ) )
									{
										( void ) m_user_account_owner_list[user_id].password->Check_In( cur_pwd );
										( void ) m_user_account_owner_list[user_id].password->Check_In_Init( cur_pwd );
										Set_Pwd_Scheme_Bit( user_id, pwd_Scheme );
									}

								}
							}
						}
						else
						{
							// Do Nothing as user already has a current password
						}
					}
				}
				else
				{
					// Hard fault here as we are not expecting any pwd here
					// By default Default user password DCID should remain blank in DCI sheet
					BF_ASSERT( false );
				}
			}
			else
			{
				USER_ACC_PRINT( DBG_MSG_INFO, "Unable to read default password for user %d", user_id );
			}
		}
		else
		{
			USER_ACC_PRINT( DBG_MSG_INFO, "Current User %d is not the default user", user_id );
		}
	}
	else
	{
		USER_ACC_PRINT( DBG_MSG_INFO, "Unable to read default password for user %d", user_id );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Account::Get_Pwd_Change_State( uint8_t user_id )
{
	uint8_t pwd_change_State = 0U;
	uint8_t pwd_complexity_val = 0U;

	if ( true == Get_Password_Complexity( user_id, &pwd_complexity_val, 0 ) )
	{
		if ( IS_PWD_CHANGE_BIT_SET( pwd_complexity_val ) )
		{
			pwd_change_State = ( uint8_t )PWD_CHANGE_MANDATORY;
			USER_ACC_PRINT( DBG_MSG_INFO, "Password change for user:%d is mandatory", user_id );
		}
		else if ( IS_PWD_WARNING_BIT_SET( pwd_complexity_val ) )
		{
			pwd_change_State = ( uint8_t )PWD_CHANGE_WARNING;
			USER_ACC_PRINT( DBG_MSG_INFO, "Password change for user:%d is not mandatory, displaying warning", user_id );
		}
		else
		{
			pwd_change_State = ( uint8_t )NO_PWD_CHANGE;
			USER_ACC_PRINT( DBG_MSG_INFO, "Password change for user:%d is not required", user_id );
		}
	}
	return ( pwd_change_State );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t User_Account::Get_Device_Password_Scheme( void )
{
	return ( m_device_pwd_scheme );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool User_Account::Get_User_Default_Password( uint8_t* def_pwd )
{
	bool ret_Status = m_default_password->Check_Out( def_pwd );

	return ( ret_Status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
User_Account::~User_Account()
{
	BF_ASSERT( false );	// this class is not supposed to destructed
	delete m_user_account_owner_list;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD User_Account::Set_Device_Unique_Password( uint8_t const* unique_pwd, uint8_t const pwd_length )
{
	DCI_CB_RET_TD status = DCI_CBACK_RET_PROCESS_NORMALLY;

	// Check for unique password received pointer and password length
	if ( ( nullptr != unique_pwd ) && ( pwd_length < PWD_SIZE_LIMIT ) )
	{
		// Check if password scheme is not COMMON DEVICE PASSWORD
		if ( m_device_pwd_scheme != DCI_DEVICE_PWD_SETTING_ENUM::DCI_COMMON_PWD_CHANGE )
		{
			// Set Unique password
			( void ) m_default_password->Check_In( unique_pwd );
			if ( true == m_default_password->Check_In_Init( unique_pwd ) )
			{
				// Process Password setting again to use new unique password
				for ( uint8_t user_id = 0; user_id < m_num_user_accounts; user_id++ )
				{
					Process_Pwd_Setting( user_id, m_device_pwd_scheme );
				}

				USER_ACC_PRINT( DBG_MSG_INFO, "Device Unique password applied" );
			}
			else
			{
				USER_ACC_PRINT( DBG_MSG_ERROR, "Unable to set Device unique password" );
				status = DCI_ERR_ACCESS_VIOLATION;
			}
		}
		else
		{
			USER_ACC_PRINT( DBG_MSG_ERROR, "Unique pwd setting failure due to pwd scheme is currently set to: %d",
							m_device_pwd_scheme );
			status = DCI_ERR_INVALID_COMMAND;
		}
	}
	else
	{
		USER_ACC_PRINT( DBG_MSG_ERROR, "Received either a null pointer for pwd or pwd length is > max length" );
		status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( status );
}
