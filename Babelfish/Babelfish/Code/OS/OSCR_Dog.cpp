/**
 *****************************************************************************************
 * @file
 *
 * @brief   Implementation of class OSCR_Dog::.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "OS_Tasker.h"
#include "OSCR_Dog.h"
#include "Babelfish_Assert.h"

namespace BF_OS
{

uC_Single_Timer* OSCR_Dog::m_timer = NULL;
OS_Task* OSCR_Dog::m_cr_task = NULL;
uint8_t OSCR_Dog::m_elevated = 0U;
OSCR_Dog_Config const* OSCR_Dog::m_config = NULL;
uint32_t OSCR_Dog::m_elevation_cnt = 0U;
uint32_t OSCR_Dog::m_normal_cnt = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OSCR_Dog::OSCR_Dog( OS_Task* cr_task, OSCR_Dog_Config const* config )
{
	BF_ASSERT( m_timer == NULL );
	m_cr_task = cr_task;
	m_config = config;

	m_timer = new uC_Single_Timer( m_config->m_timer_num );
	m_timer->Set_Timeout( m_config->m_timer_freq, true );
	m_timer->Set_Callback( Timer_Elapsed_Callback, NULL, m_config->m_timer_hwint_priority );
	m_timer->Start_Timeout();

	new CR_Tasker( Feed_Dog_Task, 0, "OSCR_Dog task" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OSCR_Dog::~OSCR_Dog()
{
	m_timer = NULL;
	m_cr_task = NULL;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Dog::Feed_Dog_Task( CR_TASKER_PARAM param )
{
	// hold off the timer
	m_timer->Restart_Timeout();

	if ( m_elevated != 0 )
	{
		m_elevated--;
		if ( m_elevated == 0 )
		{
			// restore normal priority
			m_normal_cnt++;
			OS_Tasker::Set_Priority( m_config->m_normal_priority, m_cr_task );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OSCR_Dog::Timer_Elapsed_Callback( uC_Single_Timer::cback_param_t param )
{
	if ( m_elevated == 0 )
	{
		// elevate priority
		m_elevated = 2;
		m_elevation_cnt++;
		OS_Tasker::Set_Priority( m_config->m_elevated_priority, m_cr_task );
	}
	// else
	// {
	// The issue observed here is that ::Stop_Here() causes reset if CR_Tasker itself is taking significant time
	// For example even after elevated priority, CR_Taskers Flash Erase(typically 2 sec for one 128Kb sector),
	// Re-program operation tasks will block
	// CR_Tasker kick_dog from execution till current CR operation is complete. Therefore commented the
	// BF_ASSERT(false);.
	// Now, Timer will be elapsed(but will not force a reset). Finally Reset will be caused only if watchdog is not
	// kicked.
	//
	// BF_ASSERT(false);
	// }
}

}	/* namespace BF_OS */
