/**
 *****************************************************************************************
 *	@file
 *
 *	@brief A voltage divider is a linear circuit that produces an output voltage that is a fraction
 *	of its input voltage depending upon resistors connected and thus the voltage being divided.
 *
 *	@details Voltage calculations with the voltage divider circuit.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef VOLTAGE_DIVIDER_H
#define VOLTAGE_DIVIDER_H

#include "Includes.h"
#include "AtoD.h"
#include "DCI_Owner.h"

namespace BF_Misc
{
/*
 **************************************************************************************************
 *  @brief The Voltage_Divider class contains the implementation for the Voltage calculations using
 *  the voltage divider circuit that has the output voltage some fraction of the input voltage.
 *  @details The ADC channel provides the digital voltage value and timers are utilized to get the
 *  correct voltage value.
 *  @n @b Usage:
 *  @n @b 1. Calculates the control voltage in voltage divider circuit, during refresh also writes in
 *  the voltage value that can be further used.
 **************************************************************************************************
 */
class Voltage_Divider
{
	public:

		/**
		 * @brief Calculates the control voltage assuming we are using a voltage divider to
		 * find the V.
		 * @details It uses timers to get the refresh rate being calculated correctly.
		 * @param atod_ctrl: The source for the ADC data.
		 * @param ctrl_V_owner: Owner of ctrl voltage.
		 * @param r1_top: R1 of the voltage divider (or the top resistor).
		 * @param r2_bottom: R2 of the voltage divider.
		 * @param mV_diode_drop: The diode voltage drop if the circuit is after a protection diode.
		 * @param refresh_rate: Time in mS where the value is refreshed.  Could be handled elsewhere.
		 * Just (Refresh) needs to be called on an occasional basis.
		 */
		Voltage_Divider( BF_Misc::AtoD* atod_ctrl, DCI_Owner* ctrl_V_owner,
						 const uint32_t r1_top, const uint32_t r2_bottom,
						 const uint16_t mV_diode_drop = 0, BF_Lib::Timers::TIMERS_TIME_TD refresh_rate = 0 );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Voltage_Divider goes out of scope or deleted.
		 *  @return None
		 */
		~Voltage_Divider()
		{}

		/**
		 *  @brief This gives the digital value of the voltage from ADC channel.
		 *  @details Returned in milliVolts
		 *  @return The voltage value.
		 */
		uint16_t Get_Voltage( void );

		/**
		 *  @brief This gives the digital value of the voltage from ADC channel and also checks in this value in memory
		 *  that can be then used by other component to read.
		 *  @details Returned in milliVolts
		 *  @return The voltage value.
		 */
		uint16_t Refresh( void );

	private:
		Voltage_Divider( const Voltage_Divider& rhs );
		Voltage_Divider & operator =( const Voltage_Divider& object );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Refresh_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( Voltage_Divider* )handle )->Refresh();
		}

		uint32_t Calc_V_Constant( const uint32_t r1_top, const uint32_t r2_bottom ) const;

		static const uint8_t ATOD_CALC_BOOST = 16U;
		static const uint32_t M_CALC_BOOST = static_cast<uint32_t>( 1U ) << ATOD_CALC_BOOST;

		BF_Misc::AtoD* m_atod_ctrl;
		DCI_Owner* m_owner;
		uint32_t m_V_calc_val;
		uint16_t m_mV_diode_drop;
};

}

#endif
