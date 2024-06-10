/**
 *****************************************************************************************
 *	@file        PTP header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for webservices using PTP protocol.
 *
 *	@details     The PTP services are designed primarily for use of time synchronization.
 *               Its purpose is to sync time with master clock and reduce latency between
 *               master and slave clock.
 *

 *  @copyright   2020 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed
 *               here in.  This technical information should not be reproduced or used
 *               without direct written permission from Eaton Corporation.
 *
 *****************************************************************************************
 */
#ifndef __PTP_SLAVE_IF_H__
#define __PTP_SLAVE_IF_H__

#include "Includes.h"
#include "DCI_Owner.h"
#include "uC_PTP_Slave.h"
#include "../ptpd.h"	// PTPd stack file
#include "Timers_Lib.h"
#include "PTP_Config.h"
#include "Exception.h"

class PTP_Slave_If
{
	public:

		/**
		 * @brief           -  Constructs a PTP_Slave_If object.
		 * @param node_type -  PTP devices category
		 * @n                  ordinary clock
		 * @n                  boundary clock
		 * @n                  E2E Transparent clock
		 * @n                  P2P Transparent clock
		 * @param update_method  -  Fine correction or Coarse method
		 * @param pkt_snooping   -  select PTP version 1 or version 2
		 */
		PTP_Slave_If( uint8_t node_type = uC_PTP_Slave::ORDINARY_CLOCK,
					  uint8_t update_method = ETH_PTP_FineUpdate,
					  uint8_t pkt_snooping = uC_PTP_Slave::PTP_VER_2 );

		/**
		 * @brief         -  Processing of state machine.
		 * @param handle  -  handler for processing state machine
		 *
		 */
		static void Process_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
		{reinterpret_cast<PTP_Slave_If*>( param )->Process();}

		/**
		 * @brief        -  Destructor.
		 *
		 */
		~PTP_Slave_If();

		/**
		 * @brief           -  Set system time.
		 * @param timestamp - pointer to timestamp
		 *
		 */
		static void Set_Time( struct ptptime_t* timestamp );

		/**
		 * @brief           -  Get system time.
		 * @param timestamp - pointer to timestamp
		 *
		 */
		static void Get_Time( struct ptptime_t* timestamp );

		/**
		 * @brief      -  initialize system timers.
		 * @param None -
		 *
		 */
		void Init_Timers( void );

		/**
		 * @brief      -  initialize Queue.
		 * @param None -
		 *
		 */
		sys_mbox_t* Alert_Queue( void );

		/**
		 * @brief      -  Start system timers.
		 * @param index - index of system timers
		 * @param time_interval - time interval for system timers
		 *
		 */
		void Start_Timer( uint8_t index, uint32_t time_interval );

		/**
		 * @brief      -  stop system timers.
		 * @param None -
		 *
		 */
		void Stop_Timer( uint8_t index );

		/**
		 * @brief      -  Get queue.
		 * @param None -
		 *
		 */
		sys_mbox_t* Get_Queue( void );

		/**
		 * @brief           -  update system time offset.
		 * @param timestamp - pointer to timestamp
		 *
		 */
		static void Update_Offset( struct ptptime_t* timestamp );

		/**
		 * @brief      -  Updates time stamp addend register
		 * @param adj - correction value in thousandth of ppm (Adj*10^9)
		 *
		 */
		void Adj_Frq( int32_t adj );

	private:

		/**
		 * @brief                -  Processing of state machine.
		 * @param None - Time stamp update method
		 *
		 */
		void Process( void );

		/**
		 * @brief      -  initialize PTP with run time options.
		 * @param None
		 *
		 */
		void Init_PTP( void );

		/* timers for maintaining time interval for announce message interval, delayrequest
		   interval,
		    pdelayrequest interval, sync timer, announce receipt timer, qualification timer */
		BF_Lib::Timers* m_announce_receipt_timer;
		BF_Lib::Timers* m_delayreq_timer;
		BF_Lib::Timers* m_pdelayreq_timer;
		BF_Lib::Timers* m_sync_timer;
		BF_Lib::Timers* m_announce_interval_timer;
		BF_Lib::Timers* m_qualification_timer;

		/**
		 * @brief                      static instance of uC_RTC class.
		 */
		static uC_PTP_Slave* s_ptp;

		/**
		 * @brief                 Run time option configuration data.
		 */
		RunTimeOpts m_rt_opts;

		/**
		 * @brief                 PTP clock configuration data.
		 */
		static PtpClock m_clock;

		/**
		 * @brief                 records of foreign masters.
		 */
		static ForeignMasterRecord m_foreign_records[DEFAULT_MAX_FOREIGN_RECORDS];

		/**
		 * @brief                      static instance of queue.
		 */
		sys_mbox_t m_ptp_alert_queue;

		/**
		 * @brief                    delay in Task during initialization of port.
		 */
		static const uint16_t PTP_CR_TASK_INIT_DELAY = 600U;

		/**
		 * @brief                    delay in Task.
		 */
		static const uint16_t PTP_CR_TASK_DELAY = 200U;

		/**
		 * @brief Pointer to coroutine task.
		 */
		CR_Tasker* m_task_ptr;
};

#endif	// __PTP_SLAVE_IF_H__
