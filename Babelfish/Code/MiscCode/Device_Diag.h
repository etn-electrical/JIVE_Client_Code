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
#ifndef DEVICE_DIAG_H
	#define DEVICE_DIAG_H

#include "Timers_Lib.h"
#include "DCI_Owner.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define DEVICE_DIAG_TASK_USAGE_OWNID        DCI_CRITICAL_OS_TASK_STACK_SIZE_DCID
#define DEVICE_DIAG_TASK_NAME_OWNID         DCI_CRITICAL_OS_TASK_STACK_NAME_DCID
#define DEVICE_DIAG_HEAP_SIZE_OWNID         DCI_HEAP_USED_PERC_DCID
#define DEVICE_DIAG_HEAP_USE_MAX_OWNID      DCI_HEAP_USED_MAX_PERC_DCID
#define DEVICE_DIAG_TASK_USAGE_MAX_OWNID    DCI_CRITICAL_OS_TASK_STACK_USAGE_PERC_DCID
#define DEVICE_DIAG_CSTACK_USAGE_OWNID      DCI_CSTACK_USAGE_PERCENT_DCID
#define DEVICE_DIAG_NULL_TEMP_CTRL_TEMP     25		// In degrees C.

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Device_Diag
{
	public:
		// If you pass in a null temperature control then the temperature is hard
		// coded at DEVICE_DIAG_NULL_TEMP_CTRL_TEMP.
		static void Init( void );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Device_Diag( void );
		~Device_Diag( void );
		Device_Diag( const Device_Diag& rhs );
		Device_Diag & operator =( const Device_Diag& object );

		static void Checker_Task( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		static DCI_Owner* m_critical_os_task_stack_name;
		static DCI_Owner* m_critical_os_task_stack_size;
		static DCI_Owner* m_heap_usage;
		static DCI_Owner* m_max_temperature;
		static DCI_Owner* m_heap_usage_max;
		static DCI_Owner* m_os_stack_usage_max;
		static DCI_Owner* m_cstack_usage;


};

#endif	/*DEVICE_DIAG_H_*/
