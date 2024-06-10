/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Voltage_Divider.h"
#include "Timers_Lib.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
namespace BF_Misc
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Voltage_Divider::Voltage_Divider( BF_Misc::AtoD* atod_ctrl, DCI_Owner* ctrl_V_owner,
								  const uint32_t r1_top,
								  const uint32_t r2_bottom, const uint16_t mV_diode_drop,
								  BF_Lib::Timers::TIMERS_TIME_TD refresh_rate ) :
	m_atod_ctrl( reinterpret_cast<BF_Misc::AtoD*>( nullptr ) ),
	m_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_V_calc_val( 0U ),
	m_mV_diode_drop( 0U )
{
	BF_Lib::Timers* timer;

	m_atod_ctrl = atod_ctrl;

	m_owner = ctrl_V_owner;

	m_mV_diode_drop = mV_diode_drop;
	m_V_calc_val = Calc_V_Constant( r1_top, r2_bottom );

	Refresh();

	if ( refresh_rate > 0U )
	{
		timer = new BF_Lib::Timers( &Refresh_Static,
									reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
									BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
									"Voltage Monitor" );

		timer->Start( refresh_rate, true );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Voltage_Divider::Get_Voltage( void )
{
	uint32_t calc_voltage;

	calc_voltage = m_atod_ctrl->Get_Val();
	calc_voltage = ( ( calc_voltage * m_V_calc_val ) / M_CALC_BOOST ) + m_mV_diode_drop;

	return ( static_cast<uint16_t>( calc_voltage ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Voltage_Divider::Refresh( void )
{
	uint16_t temp;

	temp = Get_Voltage();

	m_owner->Check_In( temp );

	return ( temp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Voltage_Divider::Calc_V_Constant( const uint32_t r1_top,
										   const uint32_t r2_bottom ) const
{
	uint64_t r1;
	uint64_t r2;
	uint64_t calc_3;
	uint64_t result;

	r1 = r1_top;
	r2 = r2_bottom;
	calc_3 = ( r2 + r1 ) *
		( static_cast<uint64_t>
		  ( M_CALC_BOOST ) ) * ( static_cast<uint64_t>( ATOD_VREF_VOLTAGE_mV ) );
	result = calc_3 / ( r2 * ( 1UL << ATOD_BITS_RESOLUTION ) );

	return ( static_cast<uint32_t>( result ) );
}

}
