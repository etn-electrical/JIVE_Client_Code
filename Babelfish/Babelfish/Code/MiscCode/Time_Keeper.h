/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIME_KEEPER_H
   #define TIME_KEEPER_H

#include "OS_Tasker.h"
#include "Time.h"

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Time_Keeper
{
	public:
		Time_Keeper( void );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Time_Keeper() {}

		static void Update_Time( OS_TASK_PARAM task_param );

	private:
		static Time* m_current_time;

};



#endif
