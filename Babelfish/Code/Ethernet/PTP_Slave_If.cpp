/**
 **********************************************************************************************
 *	@file            PPT_If_Slave.cpp C++ Implementation File for Ethernet common functions.
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   PTP initialization.
 *	@details
 *	@copyright       2020 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "PTP_Slave_If.h"
#include "Babelfish_Assert.h"
#include "OS_Tasker.h"
#include "OS_Task_Config.h"
#include "PTP_Debug.h"

uC_PTP_Slave* PTP_Slave_If::s_ptp = reinterpret_cast<uC_PTP_Slave*>( nullptr );
PtpClock PTP_Slave_If::m_clock = {};
ForeignMasterRecord PTP_Slave_If::m_foreign_records[DEFAULT_MAX_FOREIGN_RECORDS] = {};

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

/* Timer callback to check if timer are expired or not */
void timerCallback( void* arg );

#ifdef __cplusplus
}
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PTP_Slave_If::PTP_Slave_If( uint8_t node_type, uint8_t update_method, uint8_t pkt_snooping ) :
	m_announce_receipt_timer( nullptr ),
	m_delayreq_timer( nullptr ),
	m_pdelayreq_timer( nullptr ),
	m_sync_timer( nullptr ),
	m_announce_interval_timer( nullptr ),
	m_qualification_timer( nullptr ),
	m_task_ptr( nullptr )
{
	s_ptp = new uC_PTP_Slave( node_type, update_method, pkt_snooping );
	m_clock.user_arg = reinterpret_cast<void*>( this );
	Init_PTP();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Set_Time( struct ptptime_t* timestamp )
{
	s_ptp->Set_Time( timestamp );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Get_Time( struct ptptime_t* timestamp )
{
	s_ptp->Get_System_Time( timestamp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Init_Timers( void )
{
	m_announce_receipt_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								ANNOUNCE_RECEIPT_TIMER ) );
	m_delayreq_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								DELAYREQ_INTERVAL_TIMER ) );
	m_pdelayreq_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								PDELAYREQ_INTERVAL_TIMER ) );
	m_sync_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								SYNC_INTERVAL_TIMER ) );
	m_announce_interval_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								ANNOUNCE_INTERVAL_TIMER ) );
	m_qualification_timer =
		new BF_Lib::Timers( &timerCallback,
							reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>(
								QUALIFICATION_TIMEOUT ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Start_Timer( uint8_t index, uint32_t time_interval )
{
	switch ( index )
	{
		case PDELAYREQ_INTERVAL_TIMER:
			m_pdelayreq_timer->Start( time_interval );
			break;

		case DELAYREQ_INTERVAL_TIMER:
			m_delayreq_timer->Start( time_interval );
			break;

		case SYNC_INTERVAL_TIMER:
			m_sync_timer->Start( time_interval );
			break;

		case ANNOUNCE_RECEIPT_TIMER:
			m_announce_receipt_timer->Start( time_interval );
			break;

		case ANNOUNCE_INTERVAL_TIMER:
			m_announce_interval_timer->Start( time_interval );
			break;

		case QUALIFICATION_TIMEOUT:
			m_qualification_timer->Start( time_interval );
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Stop_Timer( uint8_t index )
{
	switch ( index )
	{
		case PDELAYREQ_INTERVAL_TIMER:
			m_pdelayreq_timer->Stop();
			break;

		case DELAYREQ_INTERVAL_TIMER:
			m_delayreq_timer->Stop();
			break;

		case SYNC_INTERVAL_TIMER:
			m_sync_timer->Stop();
			break;

		case ANNOUNCE_RECEIPT_TIMER:
			m_announce_receipt_timer->Stop();
			break;

		case ANNOUNCE_INTERVAL_TIMER:
			m_announce_interval_timer->Stop();
			break;

		case QUALIFICATION_TIMEOUT:
			m_qualification_timer->Stop();
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Process( void )
{
	/* Loop forever */
	void* msg = nullptr;

	CR_Tasker_Begin( m_task_ptr );
	while ( 1U )
	{

		/* Process the current state. */
		do
		{
			/*doState() has a switch for the actions and events to be
			 * checked for 'port_state'. The actions and events may or may not change
			 * 'port_state' by calling toState(), but once they are done we loop around
			 * again and perform the actions required for the new 'port_state'.
			 */
			if ( ( m_clock.portDS.portState == PTP_INITIALIZING ) )
			{
				CR_Tasker_Delay( m_task_ptr, PTP_CR_TASK_INIT_DELAY );
			}
			else
			{
				CR_Tasker_Delay( m_task_ptr, PTP_CR_TASK_DELAY );
			}
			/* Wait up to 50ms for something to do, then do something anyway. */
			sys_arch_mbox_fetch( &m_ptp_alert_queue, &msg, 50 );
			doState( &m_clock );

		} while ( netSelect( &m_clock.netPath, 0 ) > 0 );

		/* Wait up to 100ms for something to do, then do something anyway. */
		// sys_arch_mbox_fetch( &m_ptp_alert_queue, &msg, 100 );
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Init_PTP( void )
{
	/*Initialize run-time options to default values. */
	m_rt_opts.announceInterval = DEF_ANNOUNCE_INTERVAL;
	m_rt_opts.syncInterval = DEF_SYNC_INTERVAL;
	m_rt_opts.clockQuality.clockAccuracy = DEF_CLOCK_ACCURACY;
	m_rt_opts.clockQuality.clockClass = DEF_CLOCK_CLASS;
	m_rt_opts.clockQuality.offsetScaledLogVariance = DEF_CLOCK_VARIANCE;/* 7.6.3.3 */
	m_rt_opts.priority1 = DEF_PRIORITY1;
	m_rt_opts.priority2 = DEF_PRIORITY2;
	m_rt_opts.domainNumber = DEF_DOMAIN_NUMBER;
	m_rt_opts.slaveOnly = DEF_SLAVE_ONLY;
	m_rt_opts.currentUtcOffset = DEFAULT_UTC_OFFSET;
	m_rt_opts.servo.noResetClock = DEFAULT_NO_RESET_CLOCK;
	m_rt_opts.servo.noAdjust = NO_ADJUST;
	m_rt_opts.inboundLatency.nanoseconds = DEFAULT_INBOUND_LATENCY;
	m_rt_opts.outboundLatency.nanoseconds = DEFAULT_OUTBOUND_LATENCY;
	m_rt_opts.servo.sDelay = DEFAULT_DELAY_S;
	m_rt_opts.servo.sOffset = DEFAULT_OFFSET_S;
	m_rt_opts.servo.ap = DEFAULT_AP;
	m_rt_opts.servo.ai = DEFAULT_AI;
	m_rt_opts.maxForeignRecords = sizeof( m_foreign_records ) / sizeof( m_foreign_records[0] );
	m_rt_opts.stats = PTP_TEXT_STATS;
	m_rt_opts.delayMechanism = DEFAULT_DELAY_MECHANISM;
	uint16_t return_val = 0U;

	/*Initialize run time options. */
	if ( ( return_val = ptpdStartup( &m_clock, &m_rt_opts, m_foreign_records ) ) != 0 )
	{
		PTP_DEBUG_PRINT( DBG_MSG_INFO, "PTPD: startup failed" );
		BF_ASSERT( return_val );
	}

	/* Create the alert queue mailbox. */
	if ( sys_mbox_new( &m_ptp_alert_queue, 8 ) != ERR_OK )
	{
		BF_ASSERT( ERR_MEM );
		PTP_DEBUG_PRINT( DBG_MSG_INFO, "PTPD: failed to create ptp_alert_queue mbox" );
	}
	else
	{}

	/* Create the PTP coroutine task .*/
	m_task_ptr = new CR_Tasker( &Process_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
								CR_TASKER_IDLE_PRIORITY, "PTP slave routine" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
sys_mbox_t* PTP_Slave_If::Alert_Queue( void )
{
	return ( &m_ptp_alert_queue );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PTP_Slave_If::~PTP_Slave_If()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Update_Offset( struct ptptime_t* timestamp )
{
	s_ptp->Time_Stamp_Update_Offset( timestamp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Slave_If::Adj_Frq( int32_t adj )
{
	s_ptp->Time_Stamp_Adj_Frq( adj );
}
