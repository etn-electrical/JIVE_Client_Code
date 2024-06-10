/**
 *****************************************************************************************
 * @file
 *
 * @brief   Declaration of Voltage_Monitor_Plus class.
 *
 * @copyright 2015 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef Voltage_Monitor_Plus_H_
#define Voltage_Monitor_Plus_H_

#include "Includes.h"
#include "Voltage_Divider.h"

namespace BF_Misc
{
/**
 * @brief Monitor power supply voltage.
 */
class Voltage_Monitor_Plus
{
	public:
		/**
		 * @brief Default value for m_hysteresis [mV].
		 */
		static const uint16_t DEFAULT_HYSTERESIS_MV = 151U;

		/**
		 * @brief Default value for m_sample_interval [mSec].
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD
			DEFAULT_SAMPLE_INTERVAL_MSEC = 11U;

		/**
		 * @brief Default value for m_startup_interval [mSec].
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD
			DEFAULT_STARTUP_INTERVAL_MSEC = 499U;

		/**
		 * @brief List of possible states.
		 */
		enum state_t
		{
			STARTUP,
			RUN,
			LOW_POWER
		};

		/**
		 * @brief Callback prototype for any voltage State change notification.
		 * @param[in] state: Preset Voltage State
		 * @param[in] voltage: Present Voltage [mV]
		 */
		typedef void (* callback_t)( BF_Misc::Voltage_Monitor_Plus::state_t state,
									 uint16_t voltage );

		/**
		 * @brief Constructor.
		 * @param[in] voltage_divider: Point to voltage divider
		 * @param[in] shutdown_threshold: Low voltage threshold in mV
		 * @param[in] startup_threshold: Running/Operational voltage in mV
		 * @param[in] callback_function: Callback to receive any notifications when voltage
		 * state changes
		 * @param[in] hysteresis in mV
		 * @param[in] sample_interval: Rate at which the voltage is refreshed in milliseconds
		 * @param[in] startup_interval: Interval to wait for power to stabilize in milliseconds
		 */
		Voltage_Monitor_Plus(
			Voltage_Divider* voltage_divider,
			uint16_t shutdown_threshold,
			uint16_t startup_threshold,
			callback_t callback_function = reinterpret_cast<callback_t>( nullptr ),
			uint16_t hysteresis = DEFAULT_HYSTERESIS_MV,
			BF_Lib::Timers::TIMERS_TIME_TD
			sample_interval = DEFAULT_SAMPLE_INTERVAL_MSEC,
			BF_Lib::Timers::TIMERS_TIME_TD
			startup_interval = DEFAULT_STARTUP_INTERVAL_MSEC );

		/**
		 * @brief Destructor.
		 */
		~Voltage_Monitor_Plus();

		/**
		 * @brief Waits for power to stabilize.
		 *
		 * Power is deemed to have stabilized when it remains above m_startup_threshold for
		 * m_startup_interval.
		 * Any dip below m_shutdown_threshold forces soft reset.
		 * Any dip below m_startup_threshold minus m_hysteresis restarts the timer.
		 */
		void Wait_For_Power_To_Stablize( void );

		/**
		 * @brief Commences monitoring for power-down situation.
		 */
		void Commence_Monitoring( void );

		/**
		 * @brief Gets measured voltage [mV].
		 */
		uint16_t Get_Voltage( void ) const;

		/**
		 * @brief Gets present state.
		 */
		Voltage_Monitor_Plus::state_t Get_State( void ) const;

	private:
		/**
		 * @brief Voltage_Divider object which measures voltage.
		 */
		Voltage_Divider* m_voltage_divider;

		/**
		 * @brief Threshold voltage for shutdown [mV].
		 */
		uint16_t m_shutdown_threshold;

		/**
		 * @brief Threshold voltage for startup [mV].
		 */
		uint16_t m_startup_threshold;

		/**
		 * @brief Callback function to notify application of change of power status.
		 */
		callback_t m_callback_function;

		/**
		 * @brief Hysteresis [mV].
		 */
		uint16_t m_hysteresis;

		/**
		 * @brief Sample interval [mSec].
		 */
		BF_Lib::Timers::TIMERS_TIME_TD
			m_sample_interval;

		/**
		 * @brief Startup interval [mSec].
		 */
		BF_Lib::Timers::TIMERS_TIME_TD
			m_startup_interval;

		/**
		 * @brief Current operating state.
		 */
		state_t m_state;

		/**
		 * @brief Measured voltage [mV].
		 */
		uint16_t m_voltage;

		/**
		 * @brief Startup counter.
		 */
		uint16_t m_startup_counter;

		/**
		 * @brief A free-running periodic timer.
		 */
		BF_Lib::Timers* m_timer;

		/**
		 * @brief Initializes the startup counter.
		 */
		void Init_Startup_Counter( void );

		/**
		 * @brief Tests whether startup counter has expired.
		 */
		bool Is_Startup_Counter_Expired( void ) const;

		/**
		 * @brief Bumps the startup counter towards expiration.
		 */
		void Bump_Startup_Counter( void );

		/**
		 * @brief Refreshes the current voltage and voltage state
		 */
		void Timer_Callback( void );

		/**
		 * @brief Static method for timer callback
		 * @param[in] handle - this pointer
		 */
		static void Timer_Callback_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		Voltage_Monitor_Plus( const Voltage_Monitor_Plus& object );

		/**
		 * @brief Privatized Copy Assignment Operator.
		 */
		Voltage_Monitor_Plus & operator =( const Voltage_Monitor_Plus& object );

};

}	// end namespace

#endif	// end include guard
