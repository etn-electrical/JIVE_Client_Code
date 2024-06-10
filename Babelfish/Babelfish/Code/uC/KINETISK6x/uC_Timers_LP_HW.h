/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Slow Low Power Free Running Timer
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_TIMERS_LP_HW_H
#define uC_TIMERS_LP_HW_H

#include "Includes.h"

/**
 ****************************************************************************************
 * @brief
 *
 * @details
 *
 ****************************************************************************************
 */
class uC_Timers_LP_HW
{
	public:
		uC_Timers_LP_HW( UINT8 timer_num );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Timers_LP_HW() {}

		typedef uint32_t TICK_TD;

		inline void Enable( void )      { m_tc->CSR |= LPTMR_CSR_TEN_MASK; }

		inline void Disable( void )     { m_tc->CSR &= ~LPTMR_CSR_TEN_MASK; }

		TICK_TD Set_Timer_Frequency( uint32_t desired_frequency );

		void Set_Tick_Frequency( uint32_t desired_frequency );

		TICK_TD Get_Timer_Ticks( uint32_t desired_freq );	// How many ticks to get what freq.

		inline uint8_t Get_Timer_Byte_Size( void )         { return ( m_tmr_ctrl->cnt_size_bytes ); }

		// to read the counts, have to write to the register first
		inline TICK_TD Get_Timer_Value( void )             { m_tc->CNR = 0;return ( m_tc->CNR ); }

	// BOOL 	Time_Expired( UINT16 start_time, UINT16 duration )
	// { return ( ( m_tc->CNR - start_time ) >= duration ); }

	protected:
		LPTMR_Type* m_tc;
		uC_BASE_LP_TIMER_CTRL_STRUCT const* m_tmr_ctrl;
		uint32_t m_tclock_freq;

	private:

};

#endif
