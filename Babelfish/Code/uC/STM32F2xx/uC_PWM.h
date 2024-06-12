/**
 **********************************************************************************************
 * @file            uC_PWM.h Header File for PWM Driver implementation.
 *
 * @brief           The file contains uC_PWM Class required for communication
 *                  over PWM channels on STM32F20x.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef uC_PWM_H
#define uC_PWM_H

#include "uC_Interrupt.h"
#include "uC_Base.h"

/**
 * @brief Various duty cycle polarity options
 */
enum
{
	/** @brief Active Low duty cycle */
	uC_PWM_DUTY_CYCLE_LOW,
	/** @brief Active High duty cycle */
	uC_PWM_DUTY_CYCLE_HIGH
};

/**
 * @brief Various PWM generation alignment modes
 */

enum
{
	/** @brief Left aligned or asymmetric PWM */
	uC_PWM_LEFT_ALIGNED,
	/** @brief Center aligned or symmetric PWM */
	uC_PWM_CENTER_ALIGNED
};

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
		 * @param[in] pwm_ctrl_block          PWM Channel control Block structure.
		 * @n
		 * @param[in] pwm_channel             Specifies PWM channel to be used in application.
		 * @n                                 STM32F207 features 4 PWM channels with each PWM control block.
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
		 * @brief                   This function configures PWM channel with desired frequency, Duty Cycle Polarity and
		 *PWM alignment
		 * @param[in] polarity      polarity of PWM signal. Possible values are mentioned below
		 * @n                       uC_PWM_DUTY_CYCLE_HIGH - active high duty cycle
		 * @n                       uC_PWM_DUTY_CYCLE_LOW - active low duty cycle
		 * @param[in] alignment     Alignment of PWM signal. Possible values are mentioned below
		 * @n                       uC_PWM_LEFT_ALIGNED - Left aligned or asymmetric PWM
		 * @n                       uC_PWM_CENTER_ALIGNED - Center aligned or symmetric PWM
		 * @param[in] desired_freq  Desired frequency on selected channel
		 * @param[in] default_state default state of output reference signal OCxREF. default value is false.
		 * @n                       TRUE - output reference signal is forced high
		 * @n                       FALSE - output reference signal is forced low
		 * @return
		 */
		void Configure( uint8_t polarity, uint8_t alignment,
						uint32_t desired_freq, bool default_state = false );

		/**
		 * @brief                   This function sets PWM duty cycle.
		 * @param[in] percent       This parameter takes duty cycle in 16bit percent value.
		 * @n                       For 25% duty cycle percent should be set to 0x4000
		 * @n                       For 50% duty cycle percent should be set to 0x8000
		 * @return
		 */
		void Set_Duty_Cycle_Pct( uint16_t percent );

	private:
		/**
		 * @brief PWM Control Definition Structure
		 */
		uC_BASE_TIMER_IO_STRUCT const* m_pwm_ctrl;
		/**
		 * @brief Member variable for storing channel number
		 */
		uint8_t m_pwm_ch;
		/**
		 * @brief Member variable for storing default state mask
		 */
		uint16_t m_def_state_mask;
		/**
		 * @brief Member variable for storing pre-loaded value to be compared to counter
		 */
		uint32_t volatile* m_compare_reg;
		/**
		 * @brief Member variable for duty cycle calculation
		 */
		uint32_t m_percent_adj;
		/**
		 * @brief Member variable for storing port pin control structure
		 */
		uC_PERIPH_IO_STRUCT const* m_port_io;
};

#endif

