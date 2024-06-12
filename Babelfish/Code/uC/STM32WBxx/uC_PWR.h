/**
 **********************************************************************************************
 * @file            uC_PWR.h Header File for Power Down Driver implementation.
 *
 * @brief           The file contains uC_PWR Class required for accessing Power Down Mode
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef uC_PWR_H
   #define uC_PWR_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "CR_Tasker.h"
#include "Change_Tracker.h"


/* ------------------ PWR registers bit mask ------------------------ */

/* @defgroup PWR_Low_Power_Mode_Selection  PWR Low Power Mode Selection */
#define PWR_LP_MODE_STOP0              ( 0x00000000u )			/*!< Stop 0: stop mode with main regulator */
#define PWR_LP_MODE_STOP1              ( PWR_CR1_LPMS_0 )		/*!< Stop 1: stop mode with low power regulator */
#define PWR_LP_MODE_STOP2              ( PWR_CR1_LPMS_1 )		/*!< Stop 2: stop mode with low power regulator
																   and VDD12I interruptible digital core domain \
																   supply OFF (less peripherals activated than low
																   power mode stop 1 to reduce power consumption)*/
#define PWR_LP_MODE_STANDBY            ( PWR_CR1_LPMS_0 | PWR_CR1_LPMS_1 )		/*!< Standby mode */
#define PWR_LP_MODE_SHUTDOWN           ( PWR_CR1_LPMS_2 )

/* @defgroup PWR_Regulator_state_in_SLEEP_STOP_mode  PWR regulator mode */
#define PWR_MAINREGULATOR_ON           ( 0x00000000U )				/*!< Regulator in main mode      */
#define PWR_LOWPOWERREGULATOR_ON       ( PWR_CR1_LPR )				/*!< Regulator in low-power mode */

/* @defgroup PWR_STOP_mode_entry  PWR STOP mode entry */
#define PWR_STOPENTRY_WFI              ( ( uint8_t )0x01 )			/*!< Wait For Interruption instruction to enter Stop
																	   mode */
#define PWR_STOPENTRY_WFE              ( ( uint8_t )0x02 )			/*!< Wait For Event instruction to enter Stop mode
																	 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_PWR
{
	public:
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		uC_PWR( void );
		~uC_PWR(){}

		/**
		 * @brief Configure the microcontroller to go in to Sleep mode.
		 * @param power_sleep_entry - specifies if SLEEP mode in entered with
		 * WFI or WFE instruction.
		 */
		void Enter_PowerDown_SleepMode( uint8_t power_sleep_entry );


		/**
		 * @brief Configure the microcontroller for Stop mode.
		 * @param STOP_mode_level - specifies the stop mode level.
		 * @param
		 */
		void Configure_StopMode_Level( uint32_t STOP_mode_level );

		/**
		 * @brief Configure the microcontroller to go in to Stop mode.
		 * @param power_sleep_entry - specifies if STOP mode in entered with WFI or WFE instruction.
		 */
		void Enter_PowerDown_StopMode( uint8_t power_sleep_entry );

		/**
		 * @brief Enables or disables the WakeUp Pin functionality..
		 * @param PWR_WakeUpPin - specifies the WakeUpPin.
		 * @param pinState - new state of the WakeUp Pin functionality.
		 */
		void Configure_Wakeup_Pin( uint32_t PWR_WakeUpPin, uint8_t pinState );

		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

		static const uint8_t WAIT_FOR_INTERRUPT = 0U;
		static const uint8_t WAIT_FOR_EVENT = 1U;

	private:

		/**
		 * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
		 *         and select PLL as system clock source.
		 * @param  None
		 * @retval None
		 */
		void SYSCLKConfig_STOP( void );

		cback_func_t m_cback_func;
		cback_param_t m_cback_param;
};

#endif
