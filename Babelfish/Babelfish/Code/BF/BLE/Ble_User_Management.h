/**
 *****************************************************************************************
 *   @file
 *
 *   @brief
 *
 *   @details
 *
 *   @version
 *   C++ Style Guide Version 1.0
 *
 *   @copyright 2018 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */

#ifndef BLE_USER_MANAGEMENT_H_
#define BLE_USER_MANAGEMENT_H_

#include "Includes.h"
#include "DCI_Defines.h"
#include "DCI_Owner.h"
#include <string.h>

/**
 * @brief Encapsulates handling of BLE User Management, So that behave in common fashion
 * across all LTK devices.
 */
class Ble_User_Management
{
	public:
		/**
		 * @brief User management configuration structure.
		 */

		struct ble_user_management_t
		{
			uint8_t m_total_user_mgmt_dcids;
			const DCI_ID_TD* m_user_mgmt_dcid;
		};

		/**
		 * @brief Constructs an instance of the class.
		 */
		Ble_User_Management( const ble_user_management_t* user_entry_config,
							 const ble_user_management_t* role_based_config,
							 const ble_user_management_t* user_name_config,
							 const ble_user_management_t* user_pswd_config,
							 const ble_user_management_t* user_role_config );

		/**
		 * @brief Destructor (for MISRA purposes only).
		 */
		~Ble_User_Management();

		/**
		 * @brief                   Function for accessing active level from DCI sheet
		 * @param[in]         		void
		 * @return 					access level
		 */
		uint8_t Get_Active_Access_Level( void );

		/**
		 * @brief We kept this ble_active_access_level_owner in public space since it is accessed in app.c
		 * to retrieve the user access level and compare with the each parameters predefined access level.
		 */
		DCI_Owner* m_ble_active_access_level_owner;

	private:
		/**
		 * @brief                   Function handler for static Call back function of User Name
		 * @param[in] handle        User Name callback handle
		 * @[in] access_struct      acces struct passed in for the DCI Access
		 * @return None
		 */
		static DCI_CB_RET_TD Validate_UserName_Callback_Static( DCI_CBACK_PARAM_TD handle,
																DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief                   Function handler for static Call back function of User Password
		 * @param[in] handle        User Password callback handle
		 * @[in] access_struct      acces struct passed in for the DCI Access
		 * @return None
		 */
		static DCI_CB_RET_TD Validate_UserPassword_Callback_Static( DCI_CBACK_PARAM_TD handle,
																	DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief                   Function handler for static Call back function of Role based and User name
		 *                          based login and hide the password with asterik '*'
		 * @param[in] handle        Password callback handle
		 * @[in] access_struct      acces struct passed in for the DCI Access
		 * @return None
		 */
		static DCI_CB_RET_TD Ble_Users_Hide_Password_Callback_Static( DCI_CBACK_PARAM_TD handle,
																	  DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief Constants used for the user management
		 */
		static const uint8_t BLE_NO_OF_ROLES = 4U;
		static const uint8_t BLE_NO_OF_USERS = 4U;
		static const uint8_t BLE_USER_NAME_ENTRY = 0U;
		static const uint8_t BLE_USER_PSWD_ENTRY = 1U;
		static const uint8_t BLE_NO_OF_USER_ENTRY_PARAMS = 2U;
		static const uint8_t BLE_USER_CREDENTIALS_MAX_LENGTH = 32U;
		static const uint8_t ACCESS_NOT_VALID = 0xFF;
		uint8_t m_role_access_levels_list[BLE_NO_OF_ROLES] = {7, 15, 31, 99};

		/**
		 * @brief DCI owners for the user management
		 */
		const ble_user_management_t* m_user_entry;
		const ble_user_management_t* m_role_based;
		const ble_user_management_t* m_user_name;
		const ble_user_management_t* m_user_password;
		const ble_user_management_t* m_user_role;



		DCI_Owner* m_ble_user_entry_owner[BLE_NO_OF_USER_ENTRY_PARAMS] = { nullptr };
		DCI_Owner* m_ble_roles_owner[BLE_NO_OF_ROLES] = { nullptr };
		DCI_Owner* m_ble_user_names_owner[BLE_NO_OF_USERS] = { nullptr };
		DCI_Owner* m_ble_user_password_owner[BLE_NO_OF_USERS] = { nullptr };
		DCI_Owner* m_ble_user_role_owner[BLE_NO_OF_USERS] = { nullptr };

		/**
		 * @brief variable to hold the user entered name
		 */
		char user_name_entry[BLE_USER_CREDENTIALS_MAX_LENGTH] = { 0U };

		/**
		 * @brief                        Function handler for Call back function of User Name
		 * @param[in] access_struct      acces struct passed in for the DCI Access
		 * @return DCI_CB_RET_TD         Return the status of the call back function
		 */
		DCI_CB_RET_TD Validate_UserName_Callback( DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief                        Function handler for Call back function of User Password
		 * @param[in] access_struct      acces struct passed in for the DCI Access
		 * @return DCI_CB_RET_TD         Return the status of the call back function
		 */
		DCI_CB_RET_TD Validate_UserPassword_Callback( DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief                        Function handler for Call back function of Role based and User name
		 * based login and hide the password with asterik '*'
		 * @param[in] access_struct      acces struct passed in for the DCI Access
		 * @return DCI_CB_RET_TD         Return the status of the call back function
		 */
		DCI_CB_RET_TD Ble_Users_Hide_Password_Callback( DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		Ble_User_Management( const Ble_User_Management& object );

		/**
		 * @brief Privatized Copy Assignment Operator.
		 */
		Ble_User_Management & operator =( const Ble_User_Management& object );

		/**
		 * @brief                                   Create the DCID owners for the user management
		 * @param[in] m_ble_usermanagement_owners   Pointer to pass the usermanagement dcid owners
		 * @param[in] usermgmt_dcid_group		    Pointer to pass the usermanagement parameter
		 * group
		 * @return None
		 */
		void Create_Usermanagement_Dcid_Owners( DCI_Owner** m_ble_user_mgmt_owners,
												const ble_user_management_t* user_mgmt_dcid_group );

		/**
		 * @brief                                   Create the DCID owners for the user management
		 *                                          with callback to Display cred/password type
		 *                                          parameter in asterisk format
		 * @param[in] m_ble_usermanagement_owners   Pointer to pass the usermanagement dcid owners
		 * @param[in] usermgmt_dcid_group		    Pointer to pass the usermanagement parameter
		 * group
		 * @return None
		 */
		void Create_Usermanagement_Dcid_Owners_With_Callback( DCI_Owner** m_ble_user_mgmt_owners,
															  const ble_user_management_t* user_mgmt_dcid_group );

};

#endif
