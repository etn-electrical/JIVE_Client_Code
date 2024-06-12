/**
 *****************************************************************************************
 *	@file App_Enabler.h
 *
 *  @brief File contains App_Enabler class for handling locking and unlocking Specific Application
 *  @li in the firmware and the DCID set used for it.
 *  @li In case of DCID set, the set of dcid can be locked or unlocked for reading/writing/both.
 *  @li These set of DCIDs can be one which will be unblocked with the Licenser Module enables
 *  @li the set at the event of customer makes the payment through Revenera or some other payment
    @li gateway. Same is applicable for App enablement.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef APP_ENABLER_H
	#define APP_ENABLER_H
#include "Enabler.h"

/**
 **************************************************************************************************
 *  @brief App_Enabler class is responsible for handling locking and unlocking the Application
 **************************************************************************************************
 */

class App_Enabler : public Enabler
{
	public:
		enum command_t
		{
			ENABLE,		///< This command is used to enable the application
			DISABLE		///< This command is used to disable the application
		};

		/**
		 * @brief Function Parameter argument for callback function
		 */
		typedef void* cback_param_t;

		/**
		 *  @brief Call back function type
		 *  @param[in] command_t: Indicates to operation to be performed is for Enabling or Disabling the application
		 *  @param[in,out] ret_status_t *: enabled state TRUE - Enabled. FALSE - Disabled
		 *  @return void
		 */
		typedef void (* cback_func_t)( cback_param_t func_param, command_t command,
									   Enabler::ret_status_t* ret_status );

		/**
		 *  @brief Constructor for the App_Enabler Class.
		 *  @details This class is used to enable or disable the specific application
		 *  @param[in] func_callback: pointer to Callback function which will have the operations to be performed to
		 *  enable or disable any application.
		 *  @param[in] func_param: Callback function parameter.
		 */
		App_Enabler( cback_func_t func_callback, cback_param_t func_param );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~App_Enabler( void );

		/**
		 *  @brief Locks the DCI set of App or complete App for a particular fid.
		 *  @param[in] : none
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		ret_status_t Lock( void );

		/**
		 *  @brief Unlocks the DCI set of App or complete App for a particular fid.
		 *  @param[in] : none
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		ret_status_t Unlock( void );

	private:
		App_Enabler( const App_Enabler& object );		///< Private copy constructor
		App_Enabler & operator =( const App_Enabler& object );		///< Private copy assignment operator

		cback_func_t m_func_callback;
		cback_param_t m_func_param;


};
#endif

