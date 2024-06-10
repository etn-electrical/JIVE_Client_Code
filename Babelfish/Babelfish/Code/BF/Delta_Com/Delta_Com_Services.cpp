/**
 *****************************************************************************************
 * @file
 *
 * @brief Implementation of Delta_Com::Delta_Com_Services class.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Delta_Com_Services.h"
#include "Babelfish_Assert.h"

namespace BF
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Services::Delta_Com_Services( config_t const* config ) :
	m_config( config ),
	m_pitch_dci( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_catch_dci( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_state_bits( 0U ),
	m_reset( reinterpret_cast<BF::System_Reset*>( nullptr ) ),
	m_reset_timer( reinterpret_cast<BF_Lib::Timers*>( nullptr ) ),
	m_reset_ready_flag( false )
{
	// establish DCI parameters
	m_pitch_dci = new DCI_Owner( m_config->m_pitch_dci_id );

	m_pitch_dci->Check_In( DCOMSVC_ID_ACK_ID );

	m_catch_dci = new DCI_Owner( m_config->m_catch_dci_id );

	m_catch_dci->Check_In( DCOMSVC_ID_ACK_ID );
	m_catch_dci->Attach_Callback
	(
		&Catch_Callback_Static,
		reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
		DCI_ACCESS_SET_RAM_CMD_MSK
	);

	m_reset = new System_Reset(
		&Reset_CallBack_Static,
		reinterpret_cast<BF::System_Reset::cback_param_t>( this ) );

	m_reset_timer = new BF_Lib::Timers( &Timer_Callback_Static,
										reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
										BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
										"DCOM Services" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Services::~Delta_Com_Services()
{
	m_config = reinterpret_cast<config_t const*>( nullptr );
	delete m_pitch_dci;
	delete m_catch_dci;
	delete m_reset;
	delete m_reset_timer;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Services::Invoke( service_id_t service_id )
{
	service_id_t temp_service_id;

	if ( service_id < m_config->m_max_service_id )
	{
		if ( BF_Lib::Bit::Test( m_state_bits, service_id ) == false )
		{
			m_pitch_dci->Check_Out( &temp_service_id );
			if ( temp_service_id == DCOMSVC_ID_ACK_ID )
			{
				m_pitch_dci->Check_In( service_id );
				m_pitch_dci->Check_In();
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Delta_Com_Services::Catch_Callback( DCI_ACCESS_ST_TD* const access_struct )
{
	/// could validate that access_struct->command equals DCI_ACCESS_SET_RAM_CMD
	/// could validate that access_struct->data_access.length equals expected size of this DCI
	// (sizeof(uint32_t))

	service_id_t* data_ptr = reinterpret_cast<service_id_t*>( access_struct->data_access.data );
	service_id_t service_id = *data_ptr;

	if ( ( service_id != DCOMSVC_ID_ACK_ID ) && ( service_id < DCOMSVC_ID_MAX_STD_USED_ID ) )
	{
		BF_Lib::Bit::Set( m_state_bits, service_id );

		uint8_t local_svc_id = m_config->m_service_table[service_id];
		BF_Lib::Services::Execute_Service( local_svc_id );

		BF_Lib::Bit::Clr( m_state_bits, service_id );

		/// start-hack
		/// REDBALL-943
		/// Mark.V's work-around because Check_In doesn't seem to work here
		*data_ptr = DCOMSVC_ID_ACK_ID;
		/// end-hack

		m_catch_dci->Check_In( DCOMSVC_ID_ACK_ID );
		m_catch_dci->Check_In();
	}

	return ( DCI_CBACK_RET_NO_FURTHER_PROC_VAL_CHANGE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Delta_Com_Services::Catch_Callback_Static(
	const DCI_CBACK_PARAM_TD cback_param,
	DCI_ACCESS_ST_TD* const access_struct )
{
	Delta_Com_Services* services = reinterpret_cast<Delta_Com_Services*>( cback_param );

	return ( services->Catch_Callback( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com_Services::service_id_t Delta_Com_Services::Reverse_Lookup(
	BF::System_Reset::reset_select_t reset_req ) const
{
	bool foundFlag = false;
	service_id_t service_id;

	for ( service_id = DCOMSVC_ID_FIRST_ID;
		  service_id < DCOMSVC_ID_MAX_STD_USED_ID;
		  service_id++ )
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
		 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting
		 * pointers, not enums.
		 */
		/*lint -e{930} # MISRA Deviation */
		if ( m_config->m_service_table[service_id] == static_cast<uint8_t>( reset_req ) )
		{
			foundFlag = true;
			break;
		}
	}

	BF_ASSERT( foundFlag != false );

	return ( service_id );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com_Services::Reset_Callback( BF::System_Reset::reset_select_t reset_req )
{
	BF_Lib::Unused<BF::System_Reset::reset_select_t>::Okay( reset_req );

	if ( m_reset_ready_flag == false )
	{
		if ( m_reset_timer->Is_Dead() )
		{
			m_reset_timer->Start( m_config->m_reset_timeout, false );

			service_id_t service_id = Reverse_Lookup( reset_req );
			Invoke( service_id );
		}
	}

	return ( m_reset_ready_flag );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com_Services::Reset_CallBack_Static(
	BF::System_Reset::cback_param_t handle,
	BF::System_Reset::reset_select_t reset_req )
{
	Delta_Com_Services* services = reinterpret_cast<Delta_Com_Services*>( handle );

	return ( services->Reset_Callback( reset_req ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Services::Timer_Callback( void )
{
	m_reset_ready_flag = true;
	m_reset_timer->Stop();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Com_Services::Timer_Callback_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	Delta_Com_Services* services = reinterpret_cast<Delta_Com_Services*>( param );

	return ( services->Timer_Callback() );
}

}
