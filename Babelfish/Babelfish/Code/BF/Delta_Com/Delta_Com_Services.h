/**
 *****************************************************************************************
 * @file
 *
 * @brief Declaration of Delta_Com::Delta_Com_Services class.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef Delta_Com_Services_H_
#define Delta_Com_Services_H_

#include "Includes.h"
#include "Services.h"
#include "DCI_Owner.h"
#include "Timers_Lib.h"
#include "System_Reset.h"

namespace BF
{
/**
 * @brief Represents a client and server for invocation of services on remote modules via delta com.
 */
class Delta_Com_Services
{
	public:
		/** Service identifier type. */
		typedef uint8_t service_id_t;

		/** Acknowledgement ID. */
		static const service_id_t DCOMSVC_ID_ACK_ID = 0;

		/** Generic first ID (for iteration). */
		static const service_id_t DCOMSVC_ID_FIRST_ID = 1;

		/** Service identifier for soft reset. */
		static const service_id_t DCOMSVC_ID_SOFT_RESET_ID = DCOMSVC_ID_FIRST_ID;

		/** Service identifier for factory reset. */
		static const service_id_t DCOMSVC_ID_FACTORY_RESET_ID = 2;

		/** Service identifier for application parameter reset. */
		static const service_id_t DCOMSVC_ID_APP_PARAM_RESET_ID = 3;

		/** Service identifier for watchdog test. */
		static const service_id_t DCOMSVC_ID_WATCHDOG_TEST_RESET_ID = 4;

		/** Service identifier for triggering comm loss. */
		static const service_id_t DCOMSVC_ID_APP_TRIGGER_COMM_LOSS_ID = 5;

		/** Service identifier for unlocking manufacturing access. */
		static const service_id_t DCOMSVC_ID_APP_MFG_ACCESS_FORCE_UNLOCK_ID = 6;

		/** Service identifier for locking manufacturing access. */
		static const service_id_t DCOMSVC_ID_APP_MFG_ACCESS_FORCE_LOCK_ID = 7;

		/** Service identifier for running bootloader. */
		static const service_id_t DCOMSVC_ID_APP_RUN_BOOTLOADER_ID = 8;

		/** Service identifier for wiping non-volatile memory. */
		static const service_id_t DCOMSVC_ID_APP_WIPE_NV_MEM_ID = 9;

		/** Service identifier for performing LED test. */
		static const service_id_t DCOMSVC_ID_APP_LED_TEST_ID = 10;

		/** Service identifier for dump coverage memory. */
		static const service_id_t DCOMSVC_ID_APP_COV_DMP_ID = 11;

		/** Service identifier for triggering comm idle. */
		static const service_id_t DCOMSVC_ID_APP_TRIGGER_COMM_IDLE_ID = 12;

		/** Maximum type (for iteration). */
		static const service_id_t DCOMSVC_ID_MAX_STD_USED_ID = 13;

		/** Maximum type (for allocation). */
		static const service_id_t MAX_ALLOC_ID = 32;

		/**
		 * @brief An invalid BF_Lib:Services identifier.
		 */
		static const uint8_t DCOMSVC_ID_INVALID_SERVICE_ID = 0xFF;

		/**
		 * @brief Configuration structure.
		 */
		struct config_t
		{
			/**
			 * @brief DCI parameter id for sending service invocations.
			 */
			DCI_ID_TD m_pitch_dci_id;

			/**
			 * @brief DCI parameter id for receiving service invocations.
			 */
			DCI_ID_TD m_catch_dci_id;


			/**
			 * @brief Reset holdoff timeout.
			 */
			BF_Lib::Timers::TIMERS_TIME_TD m_reset_timeout;

			/**
			 * @brief Number of elements in service table.
			 */
			service_id_t m_max_service_id;

			/**
			 * @brief Lookup table from global to local service id.
			 */
			uint8_t* m_service_table;
		};

		/**
		 * @brief Constructor.
		 * @param[in] config: Pointer to configuration structure.
		 */
		Delta_Com_Services( config_t const* config );

		/**
		 * @brief Destructor.
		 */
		virtual ~Delta_Com_Services();

		/**
		 * @brief Invoke service on remote system.
		 * @param[in] service_id Service identifier.
		 */
		void Invoke( service_id_t service_id );

	private:
		/**
		 * @brief Pointer to our configuration structure.
		 */
		config_t const* m_config;

		/**
		 * @brief DCI owner for sending service invocations.
		 */
		DCI_Owner* m_pitch_dci;

		/**
		 * @brief DCI owner for receiving service invocations.
		 */
		DCI_Owner* m_catch_dci;

		/**
		 * @brief Service state bits.
		 */
		uint32_t m_state_bits;

		/**
		 * @brief System Reset object for holding off reset until our outgoing delta com messages can be sent.
		 */
		BF::System_Reset* m_reset;

		/**
		 * @brief Reset hold-off timer.
		 */
		BF_Lib::Timers* m_reset_timer;

		/**
		 * @brief Flag indicates whether ready to reset.
		 */
		bool m_reset_ready_flag;

		/**
		 * @brief Reverse lookup to find global service ID given local service ID.
		 */
		service_id_t Reverse_Lookup( BF::System_Reset::reset_select_t reset_req ) const;

		/**
		 * @brief Static callback function for deltacom notification.
		 * @param[in] cback_param: The callback parameter (our 'this' pointer).
		 * @param[in] access_struct: DCI access structure.
		 * @return DCI callback return status, successful or abort.
		 */
		static DCI_CB_RET_TD Catch_Callback_Static(
			const DCI_CBACK_PARAM_TD cback_param,
			DCI_ACCESS_ST_TD* const access_struct );

		/**
		 * @brief Callback function for deltacom notification.
		 */
		DCI_CB_RET_TD Catch_Callback( DCI_ACCESS_ST_TD* const access_struct );

		/**
		 * @brief Callback function for System_Reset object.
		 * @param[in] reset_req Reset request type (not used).
		 */
		bool Reset_Callback( BF::System_Reset::reset_select_t reset_req );

		/**
		 * @brief Static callback function for System_Reset object.
		 * @param[in] handle: The callback parameter (our 'this' pointer).
		 * @param[in] reset_req Reset request type (not used).
		 */
		static bool Reset_CallBack_Static(
			BF::System_Reset::cback_param_t handle,
			BF::System_Reset::reset_select_t reset_req );

		/**
		 * @brief Callback function for Timer object.
		 */
		void Timer_Callback( void );

		/**
		 * @brief Static callback function for Timer object.
		 * @param[in] param: The callback parameter (our 'this' pointer).
		 */
		static void Timer_Callback_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		Delta_Com_Services( const Delta_Com_Services& object );

		/**
		 * @brief Privatized Copy Assignment Operator.
		 */
		Delta_Com_Services & operator =( const Delta_Com_Services& object );

};

}	// end namespace


#endif	// end include guard
