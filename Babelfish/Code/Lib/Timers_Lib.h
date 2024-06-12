/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the Timers class containing different functionalities associated with
 *	timers. It must be noted that while this uses the OS timer tick it does not
 *	work very well if you block in one of the timer callbacks.
 *
 *	@details It is used to work with the Timers and utilize its functionalities.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIMERS_LIB_H
#define TIMERS_LIB_H

#include "OS_List.h"
#include "CR_Tasker.h"
#include "Bit.hpp"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Timers class contains different Timer functionalities.
 * @details It provides functionalities for timers to start, stop, auto reload and check
 * different statuses like if the timer is dead, expired.
 * @n @b Usage:
 * @n @b 1. Used to work with the timers.
 * @n @b 2. It provides public methods to start and stop the timers, attach callbacks,
 * check the timer status whether dead, expired, auto reload enabled, to get the time
 * value, and the passed time value.
 ****************************************************************************************
 */
class Timers
{
	public:

		/**
		 * Type alias for the type OS_TICK_TD, used for the time values.
		 */
		typedef OS_TICK_TD TIMERS_TIME_TD;

		/**
		 * Type alias for the type void*, used for the call back parameter.
		 */
		typedef void* TIMERS_CBACK_PARAM_TD;

		/**
		 * Type alias for void (*)(void*), takes the call back function and returns
		 * a void pointer.
		 */
		typedef void (* TIMERS_CBACK_TD)( TIMERS_CBACK_PARAM_TD param );

		/// The default CR Tasker priority.
		static const CR_TASKER_PRIOR_TD TIMERS_DEFAULT_CR_PRIORITY = CR_TASKER_PRIORITY_1;

		/**
		 *  @brief Constructor
		 *  @details Initializes the data members.
		 *  @param[in] callback_func: the function to be called that gives a handle,
		 *  assigned to the member m_cback.
		 *  @param[in] param: The void pointer (to the object).
		 *  @param[in] priority: The CR Tasker priority.
		 *  @param[in] task_name: The name of the task to be executed.
		 *  @return None
		 */
		Timers( TIMERS_CBACK_TD callback_func, TIMERS_CBACK_PARAM_TD param = nullptr,
				CR_TASKER_PRIOR_TD priority = TIMERS_DEFAULT_CR_PRIORITY,
				char_t const* task_name = nullptr );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Timers goes out of scope.
		 *  @return None
		 */
		~Timers( void )
		{}

		/**
		 *  @brief
		 *  @details Assigns the callback and the call back parameter.
		 *  @param[in] callback_func: The function to be called that gives a handle.
		 *  @param[in] param: The void pointer (to the object). TODO
		 *  @return None
		 */
		void Attach_Callback( TIMERS_CBACK_TD callback_func,
							  TIMERS_CBACK_PARAM_TD param = nullptr );

		/**
		 *  @brief
		 *  @details To start the timer.
		 *  @param[in] time_value: The value of time out for the timer.
		 *  @param[in] auto_reload: The status bit whether to auto reload the timer or not (false).
		 *  @return None
		 */
		void Start( TIMERS_TIME_TD time_value, bool auto_reload = false );

		/**
		 *  @brief
		 *  @details To Stop the timer.
		 *  @return None
		 */
		void Stop( void );

		/**
		 *  @brief
		 *  @details To get the status whether the timer is dead or not.
		 *  @return The status of the timer.
		 */
		bool Is_Dead( void ) const;

		/**
		 *  @brief
		 *  @details The status whether the timer auto reload is enabled or not.
		 *  It is inline to reduce overhead.
		 *  @return The status if timer auto reload enabled or not(false).
		 */
		bool Auto_Reload_Enabled( void )
		{
			return ( Bit::Test( m_flags, TIMER_RELOAD ) );
		}

		/**
		 *  @brief
		 *  @details To get the tick count (time).
		 *  @return The value of the tick count.
		 */
		static TIMERS_TIME_TD Get_Time( void )
		{
			return ( CR_Tasker::Get_Interrupt_Tick_Count() );
		}

		/**
		 *  @brief
		 *  @details To know if the timer is expired depending on the duration,
		 *  start and tick count.
		 *  @param[in] start_time: The start time value for the timer.
		 *  @param[in] duration: The duration of the timer.
		 *  @return The status if the timer expired or not(false).
		 */
		static bool Expired( TIMERS_TIME_TD start_time, TIMERS_TIME_TD duration )
		{
			return ( ( CR_Tasker::Get_Interrupt_Tick_Count() - start_time ) >= duration );
		}

		/**
		 *  @brief
		 *  @details To know the value of the time passed, depending on the
		 *  start time and tick count.
		 *  @param[in] start_time: The start time value for the timer.
		 *  @return The value of the time passed.
		 */
		inline static TIMERS_TIME_TD Time_Passed( TIMERS_TIME_TD start_time )
		{
			return ( CR_Tasker::Get_Interrupt_Tick_Count() - start_time );
		}

	private:

		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Timers( const Timers& rhs );
		Timers & operator =( const Timers& object );

		static const uint8_t TIMERS_NO_AUTORELOAD = 0U;

		static void Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * @brief The following consts are labeling bits. In m_flags.
		 */
		static const uint8_t TIMER_WAS_RESET = 0U;
		static const uint8_t TIMER_RELOAD = 1U;
		static const uint8_t TIMER_STOPPED = 2U;

		uint8_t m_flags;
		TIMERS_TIME_TD m_timeout;
		TIMERS_TIME_TD m_overrun_adjust;
		TIMERS_TIME_TD m_overrun_time_sample;
		TIMERS_CBACK_TD m_cback;
		TIMERS_CBACK_PARAM_TD m_cback_param;
		CR_Tasker* m_cr_task;
};

}

#endif	/*TIMERS_LIB_H */
