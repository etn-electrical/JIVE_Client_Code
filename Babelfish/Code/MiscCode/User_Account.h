/**
 *****************************************************************************************
 *	@file
 *
 *	@brief A file which contains a User Session Management functionality.
 *  This module use digest authenticaation as a aurization method.
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef USER_ACCOUNT_H
#define USER_ACCOUNT_H
#include "Includes.h"
#include "DCI_Defines.h"
#include "DCI_Owner.h"
#include "string.h"


// Bit position for password change bit in password complexity variable
#define PWD_CHANGE_BIT ( ( sizeof( uint8_t ) * 8 ) - 1U )
#define PWD_WARNING_BIT ( ( sizeof( uint8_t ) * 8 ) - 2U )


#define SET_PWD_CHANGE( Var )   ( Var |= ( 1U << PWD_CHANGE_BIT ) )
#define RESET_PWD_CHANGE( Var )  ( Var &= ~( 1U << PWD_CHANGE_BIT ) )
#define IS_PWD_CHANGE_BIT_SET( Var ) ( Var & ( 1 << PWD_CHANGE_BIT ) )

// Bit position for trigger password change warning
#define SET_PWD_WARNING( Var ) ( Var |= ( 1U << PWD_WARNING_BIT ) )
#define RESET_PWD_WARNING( Var )  ( Var &= ~( 1U << PWD_WARNING_BIT ) )
#define IS_PWD_WARNING_BIT_SET( Var ) ( Var & ( 1U << PWD_WARNING_BIT ) )

/**
 **************************************************************************************************
 * @brief This class is responsible for handling user account.
 * @details It provides method and callbacks to manage user accounts.
 * @li It provides public methods to
 * @li 1. Initialize user account related parameters.
 * @li 2. Check if user name is valid. If valid return its user index.
 * @li 3. Check if password is valid.
 * @li 4. Get user role.
 * @li 5. Set user role.
 * @li 6. Get user name.
 * @li 7. Set user name.
 * @li 8. Get password for a user.
 * @li 9. Set password for a user.
 * @li 10. Get maximum length of the user
 * @li 11. Get max length of the full name.
 * @li 12. Get max length of the password.
 * @li 13. Get total number of the user.
 * @li 14. Get password modified epoch time.
 * @li 15. Set password modified epoch time.
 * @li 18. Get last login epoch time.
 * @li 19. Set last login epoch time.
 * @li 20. Get password complexity.
 * @li 21. Set password complexity.
 * @li 22. Get password aging.
 * @li 23. Set password aging.
 * @li 24. Get user's full name.
 * @li 25. Set user's full name.
 * @li 26. Get and Set account's valid bit list.
 * @li 27. Clear user account's valid bit list.
 **************************************************************************************************
 */
class User_Account
{
	public:

		static const uint8_t USER_NAME_LIMIT = 20U;
		enum user_account_err
		{
			UNKNOWN_ERROR = 0xFF
		};

		// Enum to identify different password change behaviours as per
		// California and Oregon law
		enum trigger_pwd_behaviour
		{
			NO_PWD_CHANGE = 0U,
			PWD_CHANGE_WARNING,
			PWD_CHANGE_MANDATORY
		};

		struct dci_user_mgmt_t
		{
			DCI_ID_TD full_name;
			DCI_ID_TD user_name;
			DCI_ID_TD password;
			DCI_ID_TD role;
			DCI_ID_TD pwd_set_epoch_time;
			DCI_ID_TD last_login_epoch_time;
			DCI_ID_TD pwd_complexity;
			DCI_ID_TD pwd_aging;
		};

		struct auth_struct_t
		{
			const char_t* role_name;
			uint8_t role_level;
			const char_t* role_desc;
		};

		struct user_account_owner_t
		{
			DCI_Owner* full_name;
			DCI_Owner* user_name;
			DCI_Owner* password;
			DCI_Owner* role;
			DCI_Owner* pwd_set_epoch_time;
			DCI_Owner* last_login_epoch_time;
			DCI_Owner* pwd_complexity;
			DCI_Owner* pwd_aging;
		};

		struct user_account_owner_t* m_user_account_owner_list;
		/**
		 * @brief Constructor to create an instance of User_Account.
		 * @details this constructor initializes user account related parameters.
		 * @param[in] user_accounts_dci: Pointer to User Accounts structure DCI parameter.
		 * @param[in] num_user_accounts: Total number of user account.
		 * @param[in] auth_role_struct: Pointer to Auth_role structure DCI parameter.
		 * @param[in] number_of_auth_roles: Total number of authorization roles.
		 * @param[in] valid_bit_list: Account valid bit list
		 * @param[in] device_password: DCI Owner for device password DCID
		 * @return this.
		 */
		User_Account( const dci_user_mgmt_t* user_accounts_dci, const uint8_t num_user_accounts,
					  const auth_struct_t* auth_role_struct, const uint8_t number_of_auth_roles,
					  const DCI_Owner* valid_bit_list, const DCI_Owner* device_password );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of User_Account goes out of scope or deleted.
		 *  @return None
		 */
		~User_Account();

		/**
		 * @brief Function to check if user name is valid.
		 * @param[in] user_name: user name relative to the user.
		 * @param[in] length: Length of the user name.
		 * @return index of the user
		 */
		uint8_t Verify_User_Name( const uint8_t* user_name, uint8_t length );

		/**
		 * @brief Function to check if password is valid.
		 * @param[in] user_index: index in an array respective to that user.
		 * @return True if valid, False if not valid.
		 */
		bool Is_Password_Valid( uint8_t user_index, const uint8_t* pwd, uint8_t length );

		/**
		 * @brief Function to get the role of the user.
		 * @param[in] user_index: index in an array respective to that user.
		 * @return.
		 */
		uint8_t Get_User_Role( uint8_t user_index );

		/**
		 * @brief Function to get the  default role of the user.
		 * @param[in] user_index: index in an array respective to that user.
		 * @return.
		 */
		uint8_t Get_Default_User_Role( uint8_t user_index );

		/**
		 * @brief Function to set role.
		 * @param[in] user_index: index in array respective to that user.
		 * @param[in] role: role of user.
		 * @param[in] length: Length of the role.
		 * @return True if successful, False if un successful.
		 */
		bool Set_User_Role( uint8_t user_index, uint8_t* role, uint8_t length );

		/**
		 * @brief Function to get the user name.
		 * @param[in] user_index: index in an array respective to that user.
		 * * @param[in] length: Length of the user name.
		 * @return True if successful, False if un successful.
		 */
		bool Get_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length );

		/**
		 * @brief Function to get the default user name.
		 * @param[in] user_index: index in an array respective to that user.
		 * @param[out] user_name: Default user name.
		 * @param[in] length: Length of the user name.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Default_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length );

		/**
		 * @brief Function to set user name.
		 * @param[in] user_index: index in an array respective to that user.
		 * @param[in] user_name: user name relative to the user.
		 * @param[in] length: Length of the user name.
		 * @return True if successful, False if un successful.
		 */
		bool Set_User_Name( uint8_t user_index, uint8_t* user_name, uint8_t length );

		/**
		 * @brief Function to get the password.
		 * @param[in] user_index: index in an array respective to that user.
		 * * @param[in] length: Length of the password.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Password( uint8_t user_index, uint8_t* pwd, uint8_t length );

		/**
		 * @brief Function to get the default password from DCID
		 * @param[in] user_index: index in an array respective to that user.
		 * @param[in] length: Length of the password.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Default_Password( uint8_t user_index, uint8_t* pwd, uint8_t length );

		/**
		 * @brief Function to set password.
		 * @param[in] user_index: index in array respective to that user.
		 * @param[in] pwd: password for user.
		 * @param[in] length: Length of the password.
		 * @return True if successful, False if un successful.
		 */
		bool Set_Password( uint8_t user_index, const uint8_t* pwd, uint8_t length );

		/**
		 * @brief Function to get the max length of the username.
		 * @param[in] void: none
		 * @return length of the user name.
		 */
		inline uint8_t Get_Max_User_Name_Length( void )
		{
			return ( m_user_name_len );
		}

		/**
		 * @brief Function to get the max length of the user fullname.
		 * @param[in] void: none
		 * @return length of the fullname.
		 */
		inline uint8_t Get_Max_User_Fullname_Length( void )
		{
			return ( m_user_full_name_len );
		}

		/**
		 * @brief Function to get the max length of the password.
		 * @param[in] void: none
		 * @return length of the password.
		 */
		inline uint8_t Get_Max_Password_Length( void )
		{
			return ( m_pwd_len );
		}

		/**
		 * @brief Function will provide count of total users.
		 * @param[in] void
		 * @return Number of total users
		 */
		inline uint8_t Get_Total_User_Count( void )
		{
			return ( m_num_user_accounts );
		}

		/**
		 * @brief Function to get the password set date.
		 * @param[in] user_index:
		 * @param[in] pwd_set_epoch_time: pointer to the pwd set epoch time
		 * @param[in] length: Length of the pwd set epoch time.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Password_Set_Epoch_Time( uint8_t user_index, uint8_t* pwd_set_epoch_time,
										  uint8_t length );

		/**
		 * @brief Function to set the password epoch time.
		 * @param[in] user_index:
		 * @param[in] pwd_set_date: pointer to the pwd set epoch time
		 * @param[in] length: Length of the pwd set epoch time.
		 * @return True if successful, False if un successful.
		 */
		bool Set_Password_Epoch_Time( uint8_t user_index, uint8_t* pwd_set_epoch_time,
									  uint8_t length );

		/**
		 * @brief Function to get the last login time.
		 * @param[in] user_index:
		 * @param[in] last_login_epoch_time: pointer to the login set time
		 * @param[in] length: Length of the login set time.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Last_Login_Epoch_Time( uint8_t user_index, uint8_t* last_login_epoch_time,
										uint8_t length );

		/**
		 * @brief Function to set the last login time.
		 * @param[in] user_index:
		 * @param[in] last_login_epoch_time: pointer to the login set time
		 * @param[in] length: Length of the login set time.
		 * @return True if successful, False if un successful.
		 */
		bool Set_Last_Login_Epoch_Time( uint8_t user_index, uint8_t* last_login_epoch_time,
										uint8_t length );

		/**
		 * @brief Function to get the password complexity.
		 * @param[in] user_index:
		 * @param[in] pwd_complexity: pointer to the password complexity
		 * @param[in] length: Length of the password complexity.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity, uint8_t length );

		/**
		 * @brief Function to get the default password complexity.
		 * @param[in] user_index:
		 * @param[in] pwd_complexity: pointer to the password complexity
		 * @param[in] length: Length of the password complexity.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Default_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity, uint8_t length );

		/**
		 * @brief Function to set the password complexity.
		 * @param[in] user_index:
		 * @param[in] pwd_complexity: pointer to the password complexity
		 * @param[in] length: Length of the password complexity.
		 * @return True if successful, False if un successful.
		 */
		bool Set_Password_Complexity( uint8_t user_index, uint8_t* pwd_complexity, uint8_t length );

		/**
		 * @brief Function to get the password aging.
		 * @param[in] user_index:
		 * @param[in] pwd_aging: pointer to the password aging
		 * @param[in] length: Length of the password aging
		 * @return True if successful, False if un successful.
		 */
		bool Get_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length );

		/**
		 * @brief Function to get the default password aging.
		 * @param[in] user_index:
		 * @param[in] pwd_aging: pointer to the password aging
		 * @param[in] length: Length of the password aging
		 * @return True if successful, False if un successful.
		 */
		bool Get_Default_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length );

		/**
		 * @brief Function to set the password aging.
		 * @param[in] user_index:
		 * @param[in] pwd_aging: pointer to the password aging
		 * @param[in] length: Length of the password aging
		 * @return True if successful, False if un successful.
		 */
		bool Set_Password_Aging( uint8_t user_index, uint8_t* pwd_aging, uint8_t length );

		/**
		 * @brief Function to get the user fullname.
		 * @param[in] user_index:
		 * @param[in] full_name: pointer to the fullname
		 * @param[in] length: Length of the fullname
		 * @return True if successful, False if un successful.
		 */
		bool Get_User_Fullname( uint8_t user_index, uint8_t* full_name, uint8_t length );

		/**
		 * @brief Function to get the default user fullname.
		 * @param[in] user_index: index in an array respective to that user.
		 * @param[out] user_name: Default user fullname.
		 * @param[in] length: Length of the user fullname.
		 * @return True if successful, False if un successful.
		 */
		bool Get_Default_User_Fullname( uint8_t user_index, uint8_t* user_name, uint8_t length );

		/**
		 * @brief Function to set the user fullname.
		 * @param[in] user_index:
		 * @param[in] full_name: pointer to the fullname
		 * @param[in] length: Length of the fullname
		 * @return True if successful, False if un successful.
		 */
		bool Set_User_Fullname( uint8_t user_index, uint8_t* full_name, uint8_t length );

		/**
		 * @brief Function will return valid user account valid bit list
		 * @param[in] none void
		 * @return valid user account list
		 */
		uint16_t Get_User_Account_Valid_Bit_List( void );

		/**
		 * @brief Function will set valid user account valid bit list
		 * @param[in] bitmap uint16_t
		 * @param[in] user_index:
		 * @return True if successful, False if un successful.
		 */
		bool Set_User_Account_Valid_Bit_List( uint16_t bitmap, uint8_t user_index );

		/**
		 * @brief Function will clear valid user account valid bit list
		 * @param[in] bitmap uint16_t
		 * @param[in] user_index:
		 * @return True if successful, False if un successful.
		 */
		bool Clear_User_Account_Valid_Bit_List( uint16_t bitmap, uint8_t user_index );

		/**
		 * @brief Function will clear all user account valid bit list
		 * @return True if successful, False if unsuccessful.
		 */
		bool Clear_All_User_Account_Valid_Bit_List( void );

		/*
		 * @brief Function to get password change state
		 * @param[in] user_id: User id
		 * @return Password change state
		 */
		uint8_t Get_Pwd_Change_State( uint8_t user_id );

		/*
		 * @brief Function to get device password scheme
		 * @return Password scheme
		 */
		uint8_t Get_Device_Password_Scheme( void );

		/*
		 * @brief Function to Get default user password. This function will work only if
		 *        DCI owner for Unique password DCID is created in the constructor of this class
		 * @param[out] default password
		 * @return Function status
		 */
		bool Get_User_Default_Password( uint8_t* def_pwd );

		/*
		 * @brief Function to set device unique password. This function will work only if
		 *        DCI owner for Unique password DCID is created in the constructor of this class
		 * @param[in] unique password
		 * @return DCI Error
		 */
		DCI_CB_RET_TD Set_Device_Unique_Password( uint8_t const* unique_pwd, uint8_t const pwd_length );

	private:
		uint8_t m_num_user_accounts;
		uint8_t m_num_auth_roles;
		uint8_t m_pwd_len;
		uint8_t m_user_name_len;
		uint8_t m_user_full_name_len;
		uint8_t m_device_pwd_scheme;
		const DCI_Owner* m_default_password;
		const DCI_Owner* m_valid_bit_list_owner;
		const struct dci_user_mgmt_t* m_user_acc_dci_struct;
		const struct auth_struct_t* m_auth_role_dci_struct;
		static const uint8_t PWD_SIZE_LIMIT = 28U;
		#ifndef MIN_USERNAME_LENGTH
		static const uint8_t MIN_USERNAME_LENGTH = 5U;
		#endif

		/**
		 * @brief Function to read the user account information.
		 * @param[in] user_owner_info: pointer to the user account owner
		 * @param[in] data: data to be written to the user account
		 * @param[in] length: length of the data
		 * @return True if successful, False if un successful.
		 */
		bool Read_User_Account_Info( const DCI_Owner* user_owner_info, uint8_t* data,
									 uint8_t length );

		/**
		 * @brief Function to read the user account information.
		 * @param[in] user_owner_info: pointer to the user account owner
		 * @param[in] data: data to be written to the user account
		 * @param[in] length: length of the data
		 * @return True if successful, False if un successful.
		 */
		bool Read_Default_User_Account_Info( const DCI_Owner* user_owner_info, uint8_t* data,
											 uint8_t length );

		/**
		 * @brief Function to write the user account information.
		 * @param[in] user_owner_info: pointer to the user account owner
		 * @param[in] data: data to be written to the user account
		 * @param[in] length: length of the data
		 * @return True if successful, False if un successful.
		 */
		bool Write_User_Account_Info( const DCI_Owner* user_owner_info, const uint8_t* data,
									  uint8_t length );

		/**
		 * @brief	This DCI callback will protect from reading critical data.
		 * @param[in] cback_param: Pass this pointer as parameter
		 * @param[in] access_struct: Acess structure
		 * @return Callback status.
		 */
		static DCI_CB_RET_TD User_Cred_Data_Cb_Static(
			DCI_CBACK_PARAM_TD cback_param, DCI_ACCESS_ST_TD* access_struct )
		{
			return ( reinterpret_cast<User_Account*>( cback_param )->User_Cred_Data_Cb(
						 access_struct ) );
		}

		/**
		 * @brief The instance based DCI callback handler.
		 * @param access_struct
		 * @return Callback status
		 */
		DCI_CB_RET_TD User_Cred_Data_Cb( DCI_ACCESS_ST_TD* access_struct );


		/*
		 * @brief Function will process password setting as per inputs from DCI
		 * 		  If Device Common or Unique password change DCID has been set in DCI Sheet
		 * 		  This function will set the MSB of password complexity variable which will be
		 * 		  used to prompt change password window on Seed UI
		 *
		 */
		void Process_Pwd_Setting( uint_fast8_t user_id, uint8_t pwd_Scheme );

		/*
		 * @brief Function will set password scheme bit
		 *
		 */
		bool Set_Pwd_Scheme_Bit( uint_fast8_t user_id,  uint8_t pwd_Scheme );

};

#endif	// USER_ACCOUNT_H

