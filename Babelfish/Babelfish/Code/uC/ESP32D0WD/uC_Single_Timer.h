/**
 *****************************************************************************************
 *	@file		uC_Single_Timer.h
 *
 *	@brief		Single timeout timer.  It uses an overflow time.  It is quite accurate and
 *	@n			easy to reset compared to multi-timer.
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef uC_SINGLE_TIMER_H
#define uC_SINGLE_TIMER_H

#include "uC_Base.h"
#include "freertos/portmacro.h"
#include "uC_Base.h"
#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "uC_Interrupt.h"

/**
 * @brief		The following callback type is for applications where you are planning on using
 * @n			The coroutine functionality.
 */

/**
 ****************************************************************************************
 * @brief		Provides a single HW timer interface.
 * @details		Will take a single hardware timer and turn it into a quite precise
 * @n			interval timer.  This will tend to be quicker than a multi-timer.
 *
 ****************************************************************************************
 */
class uC_Single_Timer
{
	public:
		/**
		 * @brief	Typedef to get the count of the timer ticks.
		 */
		typedef uint32_t TICK_TD;

		typedef void* cback_param_t;
		/**
		 * @brief								Constructor to create instance of uC_Single_Timer class.
		 * @param[in] timer_num					The timer number of microcontroller which is to be  initialized
		 * @n									For timing purpose
		 * @param[in] timer_io					Timer block definition structure
		 * @note								The constructor is used to select the timer number.
		 * @n									It is used to reset the peripheral clock.
		 * @n									It is used enable the peripheral clock and set the clock frequency.
		 */
		uC_Single_Timer( uint8_t timer_num = 0, uC_BASE_TIMER_IO_STRUCT const* timer_io = NULL );

		/**
		 * @brief						Destructor to delete the uC_Single_Timer instance
		 * @n							when it goes out of scope
		 * @param[in] void				None
		 * @return						None
		 */
		~uC_Single_Timer( void ) {}

		/**
		 * @brief		The following callback type is for applications where you are planning on using
		 * @n			The coroutine functionality with a function pointor cback_param_t.
		 */
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief							Call back function
		 * @param[in] func_callback			Callback function pointer
		 * @param[in] func_param			Function param pointer
		 * @param[in] int_priority			Interrupt priority
		 * @return							None
		 */
		void Set_Callback( cback_func_t func_callback,
						   cback_param_t func_param, uint8_t int_priority );

		/**
		 * @brief							This function sets the timeout
		 * @param[in] desired_frequency		Frequency at which timer has to run
		 * @param[in] repeat				True- If repeat is configured
		 * @n								False - If repeat is not configured
		 * @return	TICK_TD					Tick value
		 */
		TICK_TD Set_Timeout( uint32_t desired_frequency, bool repeat = true );

		/**
		 * @brief				This function restarts the timer
		 * @return none			None
		 */
		void Restart_Timeout( void );

		/**
		 * @brief				This function starts the timer
		 * @return none			None
		 */
		void Start_Timeout( void );

		/**
		 * @brief				This function is used to stop the timer
		 * @return none			None
		 */
		void Stop_Timeout( void );

		/**
		 * @brief				This function is used to check whether the
		 * @n					timer si running or not
		 * @return bool			true - If Timer is running
		 *						false - If timer is not running
		 */
		bool Timeout_Running( void ) const;

		/**
		 * @brief               This function is used to reload timer
		 * @param[in]           None
		 * @return none         None
		 */
		void Reload_Timer( void );

	private:
		/**
		 * @brief pointer to Base timer IO structure.
		 */
		uC_BASE_TIMER_IO_STRUCT const* m_tmr_ctrl;

		/**
		 * @brief							Function to check the Timer
		 * @param[in] validation_result		true if the validation is correct or else false will be sent
		 * @param[in] str					Timer string used for displaying the error message
		 * @param[in] error_type			esp Error type
		 * @return esp_err_t				esp error type
		 */
		esp_err_t Check_Timer( bool validation_result, const char* str, esp_err_t error_type );

};

#endif

#ifdef __cplusplus
}
#endif
