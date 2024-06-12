/**
 *****************************************************************************************
 *	@file DCI_Licenser.h
 *
 *	@brief A module capable of doing license management based on token received.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_LICENSER_H
	#define DCI_LICENSER_H

#include "Includes.h"
#include "DCI.h"
#include "Enabler.h"
#include "License_Parser.h"
#include "Timers_Lib.h"
#include "Gate.h"


namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief The DCI_Licenser class is single-instance and manages licenses for device features.
 *  @details This class will receive license tokens from DCI module. Thus adopter can use any
 *  interface to update these tokens into DCI. Then DCI will provide license tokens to DCI_Licenser
 *  module using DCI callback mechanism.
 **************************************************************************************************
 */
class DCI_Licenser
{
	public:
		/**
		 * @brief This structure represents mapping between dcid and feature id
		 * @li feature_id Feature ID which is defined by monetization api
		 * @li license_dcid DCID number of specific license
		 */
		struct feature_info_t
		{
			const Enabler::fid_t feature_id;		///< Feature ID
			const DCI_ID_TD dcid;			///< DCID
		};

		/**
		 *  @brief Constructs an instance of a DCI licenser
		 *  @details
		 *  @param[in] feature_info_list: List of FID's with correlated data
		 *  @param[in] total_features: Number of features for a product
		 *  @param[in] parser: Parser object will transform token into license
		 */
		DCI_Licenser( const feature_info_t* feature_info_list, const Enabler::fid_t total_features,
					  License_Parser* parser, Gate* gate_handle );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~DCI_Licenser( void );

	private:
		DCI_Licenser( const DCI_Licenser& );				///< Private copy constructor
		DCI_Licenser & operator =( const DCI_Licenser & );		///< Private copy assignment operator

		/**
		 * @brief Gate handle to call the enable or disable api's
		 */
		Gate* m_gate_handle;
		/**
		 * @brief List of features and its correlated information
		 */
		const feature_info_t* m_feature_info_list;

		/**
		 * @brief Total number of features inside a device
		 */
		const Enabler::fid_t m_total_features;

		/**
		 * @brief Parser object will transform token into license
		 */
		License_Parser* m_parser;

		/**
		 * @brief List of all licenses
		 */
		License_Parser::license_data_t* m_license_list;

		/**
		 * @brief Soft timer used for license tracking
		 */
		BF_Lib::Timers* m_tracker_timer;

		/**
		 * @brief Scanning interval of license tracker thread
		 */
		static const uint16_t LICENSE_TRACKING_TIMEOUT_MS = 1000U;	///< Track at 1 sec ???

		/**
		 * @brief Get base address of license from list based on dcid
		 */
		License_Parser::license_data_t* Get_License( DCI_ID_TD dcid );

		/**
		 * @brief Features without license model will be tracked by this handler
		 */
		void Model_None_Handler( feature_info_t* feature_info, License_Parser::license_data_t* license );

		/**
		 * @brief Features with FREE license model will be tracked by this handler
		 */
		void Model_Free_Handler( feature_info_t* feature_info, License_Parser::license_data_t* license );

		/**
		 * @brief Features with EXPIRY license model will be tracked by this handler
		 */
		void Model_Expiry_Handler( feature_info_t* feature_info, License_Parser::license_data_t* license );

		/**
		 * @brief Features with METERED license model will be tracked by this handler
		 */
		void Model_Metered_Handler( feature_info_t* feature_info, License_Parser::license_data_t* license );

		/**
		 * @brief DCI callback will get called when any token received
		 */
		DCI_CB_RET_TD License_Update_Callback( DCI_ACCESS_ST_TD* access_struct );

		static DCI_CB_RET_TD License_Update_Callback_Static( DCI_CBACK_PARAM_TD handle,
															 DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( DCI_Licenser* )handle )->License_Update_Callback( access_struct ) );
		}

		/**
		 * @brief This thread will read events from license database and trigger monetization api's
		 * for Allow/Disallow features over runtime
		 */
		void License_Tracker_Callback( void );

		static void License_Tracker_Callback_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( DCI_Licenser* )handle )->License_Tracker_Callback();
		}

};

}	// end of namespace::BF_Misc
#endif	// End of DCI_LICENSER_H
