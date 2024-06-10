/**
 *****************************************************************************************
 *	@file		LP_Manager.h
 *
 *	@brief 		This monitors the Low Power(LP) entry and exit of device.
 *
 *	@details 	Here only STOP2 mode is implementated.
 *				Others will keep getting added to this class
 *
 *	@copyright 	2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

 #ifndef LP_MANAGER_H
 #define LP_MANAGER_H

namespace BF_Misc
{

class LP_Manager
{
	public:

		enum lp_cmd_t
		{
			SLEEP,
			LP_RUN,
			LP_SLEEP,
			STOP_0,
			STOP_1,
			STOP_2,
			STAND_BY,
			SHUT_DOWN,
			FULL_RUN
		};

		/**
		 * Callback function for handling user specific behaviour with Low Power mode .
		 */
		typedef void* cback_param_t;
		typedef bool (* cback_func_t)( cback_param_t handle, lp_cmd_t lp_cmd );

		/**
		 * @brief 		Constructor
		 * @param[in]
		 */
		LP_Manager( cback_func_t low_power_config_cb,
					cback_param_t low_power_config_ready_handle );


		/**
		 * @brief 		Destructor
		 * @param[in]
		 */
		~LP_Manager( void );


		/**
		 * @brief 		Enter_LP_Mode
		 * @param[in]
		 */
		static void Enter_LP_Mode( BF_Misc::LP_Manager::lp_cmd_t lp_cmd );

	private:

		static cback_func_t m_low_power_config_cb;
		static cback_param_t m_lp_type_req_handle;

	protected:
};

}

 #endif