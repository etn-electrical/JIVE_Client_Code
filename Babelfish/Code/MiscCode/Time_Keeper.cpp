/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Time_Keeper.h"
#include "Exception.h"
#include "OS_Tasker.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
#define TIME_KEEPER_UPDATED_STACK_SIZE      100
#define TIME_KEEPER_UPDATE_TIME_TICK        1000
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
Time* Time_Keeper::m_current_time;


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Time_Keeper::Time_Keeper( void )
{
	m_current_time = new Time();

	OS_Tasker::Create_Task( Update_Time, TIME_KEEPER_UPDATED_STACK_SIZE,
							OS_TASK_PRIORITY_2, "Time_Keeper" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Time_Keeper::Update_Time( OS_TASK_PARAM task_param )
{
	while ( true )
	{
		OS_Tasker::Delay( TIME_KEEPER_UPDATE_TIME_TICK );
		m_current_time->Inc_Seconds();
	}
}
