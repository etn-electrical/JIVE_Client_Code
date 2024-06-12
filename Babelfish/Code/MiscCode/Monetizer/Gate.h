/**
 *****************************************************************************************
 *	@file Gate.h
 *
 *	@brief A file which contains APIs for Gating functionality in the Monetization of API.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef GATE_H
	#define GATE_H

#include "Enabler.h"
#include "Monetize_Feature_ID.h"

/**
 **************************************************************************************************
 *  @brief Gate class is responsible for handling the Gating functionality for disabling and enableing the
 *  specific DCI set or an application.
 **************************************************************************************************
 */
class Gate
{
	public:

		enum fid_status_t
		{
			ENABLED,		///< This command is used to indicate that the feature is enabled.
			DISABLED,		///< This command is used to indicate that the feature is disabled.
			INVALID_FID		///< This command is used to indicate that the feature ID is invalid.
		};

		enum command_status_t
		{
			SUCCESS,		///< This command is used to indicate that the enable or diable command is successful.
			FAILURE,		///< This command is used to indicate that the enable or diable command failed.
			INVALID_ID		///< This command is used to indicate that the Feature ID is invalid.

		};

		/**
		 * @brief Function Parameter argument for callback function
		 */
		typedef void* cback_param_t;

		/**
		 *  @brief Call back function type
		 *  @param[in] cback_param_t: Callback parameter
		 *  @param[in] fid_t: Feature id
		 *  @param[in] fid_status_t: Feature operation status : Enabled Disabled or Invalid ID
		 *  @return void
		 */
		typedef void (* cback_func_t) ( cback_param_t, Enabler::fid_t, fid_status_t );

		/**
		 *  @brief Constructor for the Gate Class.
		 *  @details This class is used to provide Gating (on/off) for different DCID set or features
		 *  @param[in] fid_dcid_owner: Owner for Feature ID bitfield DCID.
		 *  @param[in] enabler_handle_array: Array handle of Enabler objects.
		 */
		Gate( DCI_Owner* fid_dcid_owner, Enabler** enabler_handle_array );

		/**
		 *  @brief Destructor for the Gate Class.
		 */
		~Gate( void );

		/**
		 *  @brief This function checks if the specific enabler module is enabled or not.
		 *  @details This function is invoked using the enabler handle for particualar FID.
		 *  @param[in] fid: Feature ID
		 *  @return fid_status_t: Result of enabled query Enabled , Disabled or Invalid Fid
		 */
		fid_status_t Is_Enabled( Enabler::fid_t fid );

		/**
		 *  @brief This function enables the specific enabler module.
		 *  @details This function is invoked using the enabler handle for particualar FID.
		 *  @param[in] fid: Feature ID
		 *  @return command_status_t: Result of enable status query is SUCCESS,FAILED or INVALID FID.
		 */
		command_status_t Enable( Enabler::fid_t fid );

		/**
		 *  @brief This function disables the specific enabler module.
		 *  @details This function is invoked using the enabler handle for particualar FID.
		 *  @param[in] fid: Feature ID
		 *  @return command_status_t: Result of enable status query is SUCCESS,FAILED or INVALID FID.
		 */
		command_status_t Disable( Enabler::fid_t fid );

		/**
		 * @brief Used to attach application specific callback.
		 * @param[in] func_callback: callback function
		 * @param[in] func_param: parameter
		 * @return None
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

	private:
		Gate( const Gate& object );		///< Private copy constructor
		Gate & operator =( const Gate& object );		///< Private copy assignment operator

		/**
		 * @brief Callback attached to fid_dcid change.
		 */
		DCI_CB_RET_TD Fid_Change_Callback( DCI_ACCESS_ST_TD* access_struct );

		DCI_Owner* m_fid_dcid_owner;
		Enabler** m_enabler_handle_array;
		cback_func_t m_func_callback;
		cback_param_t m_cback_param;





		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Fid_Change_Callback_static( DCI_CBACK_PARAM_TD param, DCI_ACCESS_ST_TD* access_struct )
		{
			return ( reinterpret_cast<Gate*>( param )->Fid_Change_Callback( access_struct ) );
		}

};

#endif

