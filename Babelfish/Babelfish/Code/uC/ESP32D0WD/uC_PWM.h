/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 **********************************************************************************************
 * @file            uC_PWM.h
 *
 * @brief           The file contains uC_PWM Class required for communication
 * @n               over PWM channels on ESP32.MCPWM is the Motor Control PWM unit of ESP32
 * @n				which is used here for generating PWM signals.
 *
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *
 **********************************************************************************************
 */

#ifndef uC_PWM_H
#define uC_PWM_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "Etn_Types.h"


/**
 **********************************************************************************************
 * @brief        uC_PWM Class Declaration. The class will handle all the
 *               functionalities related to PWM output.
 **********************************************************************************************
 */

class uC_PWM
{
	public:
		/**
		 * @brief                             Constructor to create instance of uC_PWM class.
		 * @param[in] pwm_ctrl_block          PWM Channel control Block structure.There are 2 control block
		 * @n								  units which can have 6 PWM signal channels each.
		 * @param[in] pwm_channel             Specifies PWM channel to be used in application.
		 * @n                                 ESP32 features 6 PWM channels with each PWM control block with pairs
		 * @n								  where Channel 1 & 2 ,Channel 3 & 4 and Channel 5 & 6. Here for each pair
		 * @n								  frequency,polarity,counter mode,duty mode will be common.But duty cycle
		 * can
		 * @n								  variable.
		 * @param[in] port_io                 Port IO associated with PWM channel
		 * @return
		 */
		uC_PWM( uint8_t pwm_ctrl_block, uint8_t pwm_channel,
				uC_PERIPH_IO_STRUCT const* port_io = NULL );

		/**
		 * @brief                   Destructor to delete the uc_PWM instance when it goes out of scope
		 * @param[in] void          None
		 * @return
		 */
		~uC_PWM() {}

		/**
		 * @brief                   This function turns ON PWM on configured channel
		 * @param[in] void          None
		 * @return
		 */
		void Turn_On( void );

		/**
		 * @brief                   This function turns OFF PWM on configured channel
		 * @param[in] void          None
		 * @return
		 */
		void Turn_Off( void );

		/**
		 * @brief                   This function checks whether PWM output is ON
		 * @param[in] void          None
		 * @return bool             TRUE: PWM output is ON on channel
		 * @n                       FALSE: PWM output is OFF on channel
		 */
		bool Is_On( void );

		/**
		 * @brief                   This function configures PWM channel with desired frequency,counter_mode, Duty
		 * Cycle, Polarity
		 * @n                       ESP32 features 6 PWM channels with each PWM control block with paired channels,
		 * @n						where Channel 1 & 2 ,Channel 3 & 4 and Channel 5 & 6. Here for each pair
		 * @n						frequency,polarity,counter mode,duty mode will be common.But duty cycle can
		 * @n						variable.
		 * @param[in] polarity      polarity of PWM signal. Possible values are mentioned below
		 * @n                       uC_PWM_DUTY_CYCLE_HIGH - active high duty cycle
		 * @n                       uC_PWM_DUTY_CYCLE_LOW - active low duty cycle
		 * @param[in] counter_mode  Select type of MCPWM counter. Possible values are mentioned below
		 * @n                       MCPWM_UP_COUNTER  For asymmetric MCPWM
		 * @n                       MCPWM_DOWN_COUNTER - For asymmetric MCPWM
		 * @n						MCPWM_UP_DOWN_COUNTER -For symmetric MCPWM, frequency is half of MCPWM frequency set
		 * @param[in] desired_freq  Desired frequency on selected channel
		 * @param[in] default_state default state is NOT used here
		 * @return
		 */
		void Configure( uint8_t polarity, uint8_t counter_mode,
						uint32_t desired_freq, bool default_state = false );

		/**
		 * @brief                   This function sets PWM duty cycle Percentage.
		 * @param[in] percent       This parameter takes duty cycle in float 32bit percent value.
		 * @n                       Resolution of values is here is .1  (Ex :25.1 )
		 * @return
		 */
		void Set_Duty_Cycle_Pct( float32_t percent );

	private:


		/**
		 * @brief                   Select Timer to use from channel number.
		 * @param[in] pwm_channel   This parameter takes PWM channel number.
		 * @n                       6 PWM channels will be divided for 3 Timers here
		 * @n
		 * @return[out]				Timer number to be used for PWM signal
		 */
		uint8_t Timer_Select( uint8_t pwm_channel );

		/**
		 * @brief                   Select Operating channel for each unit .
		 * @param[in] pwm_channel   This parameter takes PWM channel number.
		 * @return[out]				Operating channel number to be used for PWM signal
		 */
		uint8_t Operating_Channel_Select( uint8_t pwm_channel );

		/**
		 * @brief PWM Control Definition Structure
		 */
		uC_BASE_PWM_STRUCT* m_pwm_ctrl;

};

#endif

