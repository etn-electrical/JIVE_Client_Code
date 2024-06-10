/**
 *****************************************************************************************
 *	@file
 *
 *	@brief A file which contains a User Session Management functionality.
 *  This module use digest authentication as a authorization method.
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef USER_SESSION_H
#define USER_SESSION_H
#include "Includes.h"
#include "DCI_Owner.h"
#include "User_Account.h"
#include "httpd_digest_authentication.h"
#include "Crypt.h"

enum USER_SESSION_ERR_TD
{
	USER_SESS_NO_ERR,
	USER_SESS_AUTH_REQUIRED_ERR,
	USER_SESS_USER_LOCKED_ERR,
	USER_SESS_NO_SESSION_AVAILABLE_ERR,
	USER_SESS_INTERNAL_ERR,
	USER_SESS_UNKNOWN_ERR,
	USER_SESS_INVALID_CRED_ERR,
	USER_SESS_USER_NOT_AVAILABLE,
	USER_SESS_ACCEPTED_ERR
};

typedef void (* Session_Audit_Function_Decl_1 ) ( uint8_t event_code );
typedef void (* Session_Audit_Function_Decl_2 ) ( uint8_t* log_param );

/**
 * @brief Session_Audit_Log_Assign_Callback.
 * @details Assign callback necessary for capturing logs related to user session
 * @return this.
 */
void Session_Audit_Log_Assign_Callback( Session_Audit_Function_Decl_1 session_audit_log_cb,
										Session_Audit_Function_Decl_2 session_audit_param_cb );

/**
 **************************************************************************************************
 * @brief This class is responsible for handling user sessions.
 * @details It provides method and callbacks to manage session.
 * @li It provides public methods to
 * @li 1. Initialize session.
 * @li 2. Create Nonce.
 * @li 3. Check if Nonce is active or not.
 * @li 4. Delete a session.
 * @li 5. Log suspicious activity.
 * @li 6. Auntheticate a user.
 **************************************************************************************************
 */
class User_Session
{
	public:
		static const uint8_t FRESH_NONCE_TIMEOUT = 30U;
		static const uint16_t FR_TIMER_PERIOD_1000_MS = 1000U;
		static const uint8_t MAGIC_NUMBER_SIZE = 5U;
		static const uint8_t LOGIN_URI[];
		static const uint8_t LOGOUT_URI[];

		typedef uint8_t session_id_t;

		static const session_id_t MAX_NUM_SESSIONS = static_cast<session_id_t>( 0xFF );

		struct session_object_t
		{
			uint32_t epoch_time;
			uint32_t last_activity_time;	///< Time at which last request came
			uint32_t session_create_time;	///< Time at which this session is created
			char_t nonce[NONCE_LENGTH];
			uint8_t user_id;
			bool is_active;
			bool timeout_expiry;
		};

		struct logout_session_list_t
		{
			uint16_t auth_counter;
			uint16_t fr_expiry_value;
		};

		struct nonce_element_t
		{
			uint32_t fr_counter;
			uint32_t ip_address;
			uint16_t auth_counter;
			uint8_t port;
			uint8_t magic_number[MAGIC_NUMBER_SIZE];
		};

		struct user_lock_info_t
		{
			bool is_lock;
			bool login_attempt;
			uint16_t failed_attempt_counter;
			uint32_t unlock_time;
		};

		struct user_session_config_t
		{
			DCI_Owner* enable_session_takeover;
			DCI_Owner* max_concurrent_sessions;
			DCI_Owner* inactivity_timeout_sec;
			DCI_Owner* absolute_timeout_sec;
			DCI_Owner* user_lock_time_sec;
			DCI_Owner* max_num_failed_login_attempts;
			DCI_Owner* password_waiver_level;
			uint8_t max_num_session;
			uint8_t max_num_stored_logout_sessions;
		};

		User_Account* m_user_account;

		/**
		 * @brief Constructor.
		 * @details Initialize owners for each session parameters, allocate memory and call
		 * necessary function.
		 * @param[in] user_acc: User_Account class handle.
		 * @param[in] user_session_config: Structure of user session related DCIDs and owner.
		 * @param[in] crypt_obj_sess: Crypt_Aes class handle.
		 * @return this.
		 */
		User_Session( User_Account* user_acc, const user_session_config_t* user_session_config,
					  Crypt* crypt_obj_sess );

		/**
		 * @brief Function to create a nonce.
		 * @details This function generates a unique nonce and then divides it into nonce and opaque
		 * for client to use.
		 * @param[out] nonce: Nonce.
		 * @param[out] nonce_len: Length of the nonce.
		 * @param[out] opaque: Opaque.
		 * @param[out] opaque_len: length of opaque.
		 * @param[in] ip_address: IP address of the client.
		 * @param[in] port: Port of the server.
		 * @return 1 if success, error_code if failed.
		 */
		bool Create_Nonce( char_t* nonce, char_t* opaque, uint32_t ip_address, uint8_t port );

		/**
		 * @brief Check if Nonce is active or not.
		 * @details This function checks if the nonce provided is in the active session list or not.
		 * @param[in] nonce: A unique nonce.
		 * @return index of session if found, m_max_sessions if not found.
		 */
		uint8_t Is_Nonce_Active( char_t* nonce );

		/**
		 * @brief Function to create a session.
		 * @details This function assigns the values to the assgned session's variables.
		 * @param[in] session_index: index of the session which needs to be assigned.
		 * @param[in] user_id: user to whom this session is assigned.
		 * @param[in] nonce: Nonce
		 * @param[in] ip_address: IP address to log.
		 * @param[in] port: Port to log.
		 * @return true if session creation successful, false otherwise.
		 */
		bool Create_Session( uint8_t session_index, uint8_t user_id, char_t* nonce,
							 uint32_t ip_address, uint8_t port );

		/**
		 * @brief Function to manually logout.
		 * @details This function manually logout the session and store nonce if it is fresh.
		 * @param[in] p: Pointer to Digest_data structure.
		 * @param[in] user_id: ID of the user.
		 * @return true if logout successful, false if unsuccessful.
		 */
		bool Logout_Session( Digest_data* p, uint8_t user_id );

		/**
		 * @brief Function to delete a session.
		 * @details This function deletes the session.
		 * @param[in] session_index: index of the session which needs to be deleted.
		 * @return none.
		 */
		void Delete_Session( uint8_t session_index );

		/**
		 * @brief Function to authenticate a user.
		 * @details This function validates whether the user and other authentication information is
		 * valid.
		 * @param[in] p: Pointer to Digest_data structure.
		 * @param[out] user_id: ID of the user.
		 * @param[out] auth_level: Role of the user.
		 * @param[in] auth_type: Authorization type, custom or default. Used to identify the
		 * source of the request.
		 * @return USER_SESS_NO_ERR if success, error_code if failed.
		 */
		USER_SESSION_ERR_TD Authenticate_User( Digest_data* p, uint8_t* user_id,
											   uint8_t* auth_level, uint16_t auth_type );

		/**
		 * @brief Function to check if user is locked.
		 * @param[in] user_index: index in an array respective to the user.
		 * @return True if locked, False if not locked.
		 */
		inline bool_t Is_User_Locked( uint8_t user_index )
		{
			return ( m_user_lock_info[user_index].is_lock );
		}

		/**
		 * @brief Function will read failed attempt counter.
		 * @param[in] user_index: index in an array respective to that user.
		 * @return uint8_t Failed Attempt Count for this user.
		 */
		inline uint8_t Get_Failed_Attempt_Count( uint8_t user_index )
		{
			return ( m_user_lock_info[user_index].failed_attempt_counter );
		}

		/**
		 * @brief Function will return password exemption level
		 * @param[in] auth_level: User role.
		 * @return exempt level.
		 */
		uint8_t Get_Max_Auth_Role_Exemption_Level( uint8_t auth_level );

		/**
		 * @brief Function will delete all the session of a user.
		 * @details This function will delete the all the available session of a user. it is useful
		 * when delete a user.
		 * @param[in] user_id: index in an array respective to that user.
		 * @return true if successful, false if unsuccessful.
		 */
		bool Clear_User_Session( uint8_t user_id );

		/**
		 * @brief Function will block the session from expiry.
		 * @details This function can used to extend the sessoin untill its unblocked again
		 * @param[in] nonce: nonce is required to identify the session to be blocked from deletion.
		 * @param[in] session_id: session index is updated in pointer
		 * @return true if successful, false if unsuccessful.
		 */
		bool Block_Session_Expiration( uint8_t* nonce, session_id_t* session_id );

		/**
		 * @brief Function will unblock the blocked session
		 * @details This function can used to unblock the blocked session,
		 * A session can be deleted on expiry after unblocking
		 * @param[in] session_id: session identifier to be unblocked (previously retured from block function).
		 * @return none
		 */
		void Unblock_Session_Expiration( session_id_t session_id );

	private:
		Crypt* m_crypt_obj;
		DCI_Owner* m_enable_sess_takeover_owner;
		DCI_Owner* m_max_concurr_sess_owner;
		DCI_Owner* m_inactivity_timeout_owner;
		DCI_Owner* m_absolute_timeout_owner;
		DCI_Owner* m_user_lock_time_owner;
		DCI_Owner* m_max_failed_login_owner;
		DCI_Owner* m_web_access_password_waiver_level_owner;
		static uint32_t m_fr_counter;
		static uint16_t m_auth_req_counter;
		static uint8_t m_magic_num[MAGIC_NUMBER_SIZE];
		uint8_t m_max_sessions;
		uint8_t m_max_logout_sessions;
		uint8_t m_curr_logout_sessions;
		session_object_t* m_sessions;
		user_lock_info_t* m_user_lock_info;
		logout_session_list_t* m_logout_sessions;

		static const uint16_t MAX_FAIL_ATTEMPT_COUNTER = 65535U;

		static const bool SESSION_EXPIRY_BLOCK = true;
		static const bool SESSION_EXPIRY_UNBLOCK = false;

		/**
		 * @brief Function to initialize global session constants.
		 * @details This function initializes the critical parameters responsible of most of the
		 * @basic functionality like FR timer.
		 * @param[in] void.
		 * @return void.
		 */
		void Initialize_Global_Session_Constants( void );

		/**
		 * @brief Function to check if the nonce is still fresh.
		 * @details This function checks whether the fresh_nonce timer is elapsed for the passed
		 * nonce. The default value of fresh_nonce timer is 30 Second.
		 * @param[in] nonce_element: Nonce element structure.
		 * @return true if valid, false if invalid.
		 */
		bool Is_Fresh_Nonce_Valid( nonce_element_t* nonce_element );

		/**
		 * @brief Function to check if the session is expired.
		 * @details This function checks whether the session time is greater than
		 * DCI_ABSOLUTE_TIMEOUT_SEC_DCID or not.
		 * @param[in] session_index: index of the session.
		 * @param[in] inactivity_timeout: Inactivity timeout value.
		 * @param[in] absolute_timeout: Absolute timeout value.
		 * @return true if timeout, false if not timeout.
		 */
		bool Is_Session_Timed_Out( uint8_t session_index, const uint16_t inactivity_timeout,
								   const uint32_t absolute_timeout );

		/**
		 * @brief Function to encrypt the nonce.
		 * @details This function encrypts the nonce_element and generates the nonce.
		 * @param[in] nonce_element: Structure which contains basic elements using which a nonce is
		 * created.
		 * @param[in] nonce_element_size: Size of the nonce_element structure.
		 * @param[out] nonce: Nonce.
		 * @param[out] nonce_len: Length of the nonce.
		 * @return true if success, false if failed.
		 */
		bool Encrypt_Nonce( nonce_element_t* nonce_element, uint8_t nonce_element_size,
							char_t* nonce );

		/**
		 * @brief Function to decrypt the nonce.
		 * @details This function decrypts the nonce and provide necessary information
		 * @param[in] nonce: Nonce.
		 * @param[out] nonce_element: Structure which contains basic elements using which a nonce is
		 * created.
		 * @param[in] nonce_len: Length of the nonce.
		 * @return true if success, false if failed.
		 */
		bool Decrypt_Nonce( uint8_t* nonce, nonce_element_t* nonce_element );

		/**
		 * @brief Static function.
		 * @details This is static function to call instance member function Checker_Task()
		 * @param[in] cr_task.
		 * @return void.
		 */
		static void Checker_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * @brief A co-routine task.
		 * @details This task is responsible for incrementing FR(free running) timer,
		 * @expiring the session etc.
		 * @param[in] cr_task.
		 * @return void.
		 */
		void Checker_Task( CR_Tasker* cr_task );

		/**
		 * @brief Function to delete a session if all session is occupied or max concurrent session
		 * per user is full.
		 * @details This function find and delete a session based on the role of the new user and
		 * the inactivity time of the sessions.
		 * @param[in] user_index: user_index for whom new session is to be created.
		 * @param[in] concur_session: It define from which bucket session should be hijack.
		 * Pass true for concurrent session bucket, false for all the session.
		 * @return true if success, false if unsuccessful.
		 */
		uint8_t Find_Hijackable_Session( uint8_t user_index, bool_t is_concur_session );

		/**
		 * @brief Function to find a slot for new session.
		 * @details This function find a slot for new session and if there is no empty slot then
		 * make
		 * delete a session to make room for new session.
		 * @param[in] user_index: user_index for whom new session is to be created.
		 * @return index of the session if possible otherwise too many request or server busy error.
		 */
		uint8_t Find_New_Session_Index( uint8_t user_index );

		/**
		 * @brief Function will increment failed attempt counter and
		 * fr_counter count as timestamp.
		 * @param[in] user_index: index in an array respective to that user.
		 * @param[in] ip_address: IP address to log.
		 * @param[in] port: Port to log.
		 * @param[in] auth_type: Authorization type, custom or default. Used to identify the
		 * source of the request.
		 * @param[in] uri: URI of the request.
		 * @return void.
		 */
		USER_SESSION_ERR_TD Capture_Failed_Attempt( uint8_t user_index, uint32_t ip_address,
													uint8_t port, uint16_t auth_type, uint8_t* uri );

};

#endif	// USER_SESSION_H
