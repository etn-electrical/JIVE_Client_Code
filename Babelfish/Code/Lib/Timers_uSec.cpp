/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Timers_uSec.h"

namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
uC_Timers_HW* Timers_uSec::m_timer_ctrl;
uint32_t Timers_uSec::m_timer_mask;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timers_uSec::Init( uC_Timers_HW* timer_ctrl )
{
	m_timer_ctrl = timer_ctrl;
	m_timer_ctrl->Set_Tick_Frequency( uSEC_FREQ );

	if ( m_timer_ctrl->Get_Timer_Byte_Size() == 2U )
	{
		m_timer_mask = 0xFFFFU;
	}
	else
	{
		m_timer_mask = 0xFFFFFFFFU;
	}

	m_timer_ctrl->Enable();
	m_timer_ctrl->Run();
}

}	/* end namespace BF_Lib for this module */
