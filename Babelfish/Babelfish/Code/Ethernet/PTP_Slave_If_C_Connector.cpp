/**
 *****************************************************************************************
 * @file        PTP_Slave_If_C_Connector.cpp
 * @details     This file shall includes all the definition of C/C++ bridge functions
                created for PTP_Slave_If.cpp file.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include "uC_PTP_Slave.h"
#include "PTP_Slave_If_C_Connector.h"
#include "PTP_Slave_If.h"

static PTP_Slave_If* ptp_interface = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Set_Time( int32_t sec, int32_t nsec )
{
	struct ptptime_t ts;

	ts.tv_sec = sec;
	ts.tv_nsec = nsec;
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Set_Time( &ts );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Get_Time( int32_t* sec, int32_t* nsec )
{
	struct ptptime_t timestamp = {0, 0};

	if ( ptp_interface != nullptr )
	{
		ptp_interface->Get_Time( &timestamp );
	}
	*sec = timestamp.tv_sec;
	*nsec = timestamp.tv_nsec;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Update_Offset( int32_t sec, int32_t nsec )
{
	struct ptptime_t timeoffset;

	timeoffset.tv_sec = -sec;
	timeoffset.tv_nsec = -nsec;

	/* Coarse update method */
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Update_Offset( &timeoffset );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Adj_Frq( int32_t adj )
{
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Adj_Frq( adj );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Init_Timers( const PtpClock* ptpClock )
{
	ptp_interface = reinterpret_cast<PTP_Slave_If*>( ptpClock->user_arg );
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Init_Timers();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Stop_Timer( int32_t index )
{
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Stop_Timer( index );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ptp_Interface_Start_Timer( int32_t index, uint32_t interval_ms )
{
	if ( ptp_interface != nullptr )
	{
		ptp_interface->Start_Timer( index, interval_ms );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
sys_mbox_t* Ptp_Interface_Alert_Queue( void )
{
	return ( ptp_interface->Alert_Queue() );
}
