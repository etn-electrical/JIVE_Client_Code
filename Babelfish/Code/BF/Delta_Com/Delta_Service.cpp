/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Delta_Service.h"


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Service::Delta_Service( service_def_t const* service_def ) :
	m_tx( new DCI_Owner( DCI_ID_UNDEFINED ) ),
	m_rx( new DCI_Owner( DCI_ID_UNDEFINED ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Service::~Delta_Service( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Service::Send_Service( uint8_t service )
{
	uint8_t cmd;

	cmd = Convert_Service_To_DS( service );

	m_tx->Check_In_Init( &cmd );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Service::Idle( void )
{
	uint8_t cmd;
	bool idle = false;

	m_tx->Check_Out_Init( &cmd );

	if ( cmd == 0 )
	{
		idle = true;
	}

	return ( idle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Delta_Service::RX_Change( DCI_ACCESS_ST_TD* access_struct )
{
	uint8_t cmd;
	uint8_t resp;
	uint8_t svc_cmd;

	m_tx->Check_Out_Init( &cmd );
	m_rx->Check_Out_Init( &resp );

	if ( resp != 0 )
	{
		if ( cmd != 0 )
		{
			cmd = 0;
			m_tx->Check_In_Init( &cmd );
		}
		else
		{
			svc_cmd = Convert_DS_To_Service( resp );
			BF_Lib::Services::Execute_Service( svc_cmd );
		}
	}

	return ( DCI_CBACK_RET_PROCESS_NORMALLY );
}
