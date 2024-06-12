/**
 *****************************************************************************************
 * @file
 * @brief This file contains class that handles cadence rate of alarm publishing
 * @details IOT_Alarms_Cadence contains all the necessary methods required to initialize and
 * configure timer for alarm publishing cadence rate
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IOT_ALARMS_CADENCE_H
#define IOT_ALARMS_CADENCE_H

#include "Includes.h"	///< Need to include when Timers_Lib.h is used
#include "Timers_Lib.h"
#include "Babelfish_Assert.h"
#include "azure_c_shared_utility/vector.h"

#ifdef IOT_ALARM_SUPPORT

using namespace BF_Event;

/**
 ****************************************************************************************
 * @brief This is an IOT_Alarms_Cadence class
 * @details It provides methods to initialize and configure timer for alarm publishing cadence rate
 * @ingroup alarms_over_iot
 *
 * @li It provides public methods to
 * @li 1. Initialize Cadence rate for publishing alarms
 * @li 2. Re-initialize Cadence rate for publishing alarms(Dynamic cadence)
 ****************************************************************************************
 */
class IOT_Alarms_Cadence
{
	public:
		/**
		 * @brief Constructor.
		 * @details Initialize an IOT_Alarms_Cadence
		 * @param pub_cadence_owner: Owner for cadence rate DCID
		 * @param sev_level_idx: Severity level index
		 * @param cadence_owners_vect: Handle to the array (Azure vector) of DCI owners for the cadence rates
		 */
		IOT_Alarms_Cadence( DCI_Owner* pub_cadence_owner, uint16_t sev_level_idx, VECTOR_HANDLE cadence_owners_vect );

		~IOT_Alarms_Cadence( void );

		/**
		 * @brief Function that checks whether alarms are ready to be published
		 * @details Cadence_Timer_Task will set m_alarms_ready flag when the
		 * cadence interval is reached and alarm updates are available
		 * @param[in] None
		 * @param[out] None
		 * @return True if alarms are ready for publishing, else false
		 */
		static bool Alarms_Ready( void );

		/**
		 * @brief Function that clears m_alarms_ready
		 * @details This function will be called once alarms are published successfully
		 * @param[in] None
		 * @param[out] None
		 * @return None
		 */
		static void Clear_Alarms_Ready( void );

		/**
		 * @brief Function that marks severity level as per its index
		 * @details It marks the bit in m_severity_lvl_marker which will then be checked in
		 * Cadence_Timer_Task() before marking them ready for publishing
		 * @param[in] severity_level_idx: Severity level index
		 * @param[out] None
		 * @return None
		 */
		static void Mark_Severity_Level( uint16_t severity_level_idx );

		/**
		 * @brief Function that clears severity level marker
		 * @details It clears all bits in m_severity_lvl_marker
		 * @param[in] None
		 * @param[out] None
		 * @return None
		 */
		static void Clear_Severity_Level_Marker( void );

		static uint16_t m_total_severity_levels;

	private:

		/**
		 * @brief Marks the bit that severity level corresponds to.
		 *	For eg, m_severity_lvl_marker = 1 indicates that bit 0 is marked i.e
		 *	it corresponds to severity level 0. As alarms are generated, we mark their severity level
		 *	which will then be checked in Cadence_Timer_Task() before marking them
		 *	ready for publishing */
		static uint16_t m_severity_lvl_marker;

		/**
		 * @brief Static function to redirect the call to run our alarm's cadence timer task.
		 * @param[in] param: An opaque argument that resolves to a pointer to "this".
		 * @param[out] None
		 * @return None
		 */
		static void Cadence_Timer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		/**
		 * @brief Steady timer task that checks to see if alarms need to be published.
		 * @details This timer task is invoked periodically on the cadence
		 * It will check, if there are any alarm updates and thus need to be published.
		 * If so, it will set m_alarms_ready flag
		 * We keep this task intentionally small and quick, since it runs under the task for all
		 * FreeRTOS timers.
		 * @param[in] None
		 * @param[out] None
		 * @return None
		 */
		void Cadence_Timer_Task( void );

		/**
		 * @brief Callback function updates the cadence rate of alarm publishing at run time.
		 * @details After updating the cadence rate, the publishing of alarms should happen as per new cadence rate
		 * [Note: There's no need to re-initialize/re-connect IOT]
		 * @param cback_param: Parameters for Callback function.
		 * @param access_struct: The access struct is the order slip.
		 * @return DCI callback return type.
		 */
		static DCI_CB_RET_TD Reinitialize_Cadence_Rate_Static( DCI_CBACK_PARAM_TD handle,
															   DCI_ACCESS_ST_TD* access_struct );

		DCI_CB_RET_TD Reinitialize_Cadence( DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief Holds the cadence rate in milliseconds. It is configured by DCI
		 * in milliseconds */
		uint32_t m_cadence_ms;

		/**
		 * @brief Holds the timer instance that will manage the cadence update rate
		 * for this group. */
		BF_Lib::Timers* m_cadence_timer;

		/**
		 * @brief Flag that indicates whether alarms are ready to be published or not.
		 * The Timer task for alarm cadence will set this flag when it is ready. We make it static
		 * because when the deadline is reached then this flag will tell IOT_Net
		 * to publish all the avilable alarm updates(Shortest cadence wins). Suppose cadence rate of
		 * Alarm1 is 10s then its update will be published in less than or equal to 10s */
		static bool m_alarms_ready;

		///< Owner for cadence rate DCID
		DCI_Owner* m_pub_cadence_owner;

		uint16_t m_severity_level_index;

		VECTOR_HANDLE m_cadence_owners_vect;

};

#endif	// IOT_ALARM_SUPPORT
#endif	// IOT_ALARMS_CADENCE_H
