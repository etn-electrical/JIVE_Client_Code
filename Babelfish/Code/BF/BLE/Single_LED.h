/**
 **********************************************************************************************
 * @file            Single_LED.h
 *
 * @brief           Contains the Single_LED Class.
 *
 * @details         It is designed to interface the Ble application with LED,
 *
 * @copyright       2018 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef Single_LED_H
#define Single_LED_H

#include "includes.h"
#include "LED.h"
#include "Ble_Display.h"
#include "DCI_Defines.h"
#include "DCI_Owner.h"

/**
 ****************************************************************************************
 * @brief This is a Single_LED class
 * @details It provides an LED indication for Ble Communication status,
 * @n @b Usage: handle the flashing behavior
 * It provides public methods to
 * @n @b 1. Turn on LED
 * @n @b 2. Turns LED solid off
 * @n @b 3. LED Flash
 ****************************************************************************************
 */
class Single_LED : public Ble_Display
{
	public:
		/**
		 *  @brief Constructor
		 *  @details It does nothing here.
		 */
		Single_LED( DCI_Owner* target_led_owner, BF_Misc::LED* led_ctrl, BF_Misc::LED* target_led_ctrl );

		/**
		 *  @brief Destructor
		 *  @details It does nothing here.
		 */
		~Single_LED( void );

		/**
		 *  @brief LED Flash.
		 *  @details this function will make Ble indication Flash with
		 *	On time: 50ms Off time: (100ms + Skip cycle every 1000ms)
		 *	This mode is displayed Some Faulty condition has occurred where
		 *	the bluetooth link is down or failed. Must determine conditions
		 *	under which this may happen.
		 *  @param[in] None
		 *  @return None
		 */
		void Attention( void );

		/**
		 *  @brief LED Flash.
		 *  @details this function will make Ble indication Flash with
		 *	On time: 1000ms Off time: 1000ms
		 *	This mode is displayed Some Faulty condition has occurred where
		 *	the bluetooth link is down or failed. Must determine conditions
		 *	under which this may happen.
		 *  @param[in] None
		 *  @return None
		 */
		void Device_Faulted( void );

		/**
		 *  @brief LED Flash.
		 *  @details this function will make Ble indication Flash with
		 *	On time: 50ms Off time: 1500ms
		 *	This mode is displayed A valid connection has been established
		 *	and devices are in sync.
		 *  @param[in] None
		 *  @return None
		 */
		void Device_Connected( void );

		/**
		 *  @brief LED Flash.
		 *  @details this function will make Ble indication Flash with
		 *	On time: 50ms Off time: 50ms
		 *	This mode is displayed while the device is in pairing mode.
		 *	The pairing state should be temporary to avoid causing user
		 *	anxiety. The colors should flash in sync.
		 *  @param[in] None
		 *  @return None
		 */
		void Pairing_Mode( void );

		/**
		 *  @brief LED Flash.
		 *  @details this function will make Ble indication Flash with
		 *	On time: 1500ms Off time: 50ms
		 *	This mode is displayed,If the BLE application
		 *	is currently polling and/or writing, this shall be the indication
		 *	state. What this indicates is that data is exchanged. There should
		 *	be a minimum time of flashing behavior for every access. At least one complete
		 *	cycle. For example: If the Bluetooth application polls only a single value once,
		 *	the LED should stay on for 1500ms before transitioning to the connected idle state.
		 *	If polls are continuous then the flash operations shall contine as defined.
		 *  @param[in] None
		 *  @return None
		 */
		void Data_Operations( void );

		/**
		 *  @brief Turns LED solid Off.
		 *  @details this function will make Ble indication LED solid Off
		 *	To indicate the Device is disconnected and reinitiated for
		 *	Advertising
		 *  @param[in] None
		 *  @return None
		 */
		void Device_Disconnected( void );

	private:
		BF_Misc::LED* m_led_control;
		BF_Misc::LED* m_ble_find_target_led;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_PAIRING = 50U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_PAIRING = 50U;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_FIND_TARGET = 50U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_FIND_TARGET = 50U;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_CONNECTED = 50U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_CONNECTED = 1500U;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_POLLING = 1500U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_POLLING = 50U;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_ATTENTION = 50U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_ATTENTION = 1100U;

		static const BF_Lib::Timers::TIMERS_TIME_TD LED_ON_TIME_FAULT = 1000U;
		static const BF_Lib::Timers::TIMERS_TIME_TD LED_OFF_TIME_FAULT = 1000U;

		/**
		 * @brief DCI owners for the user management
		 */
		DCI_Owner* m_target_led_owner;

		/**
		 * @brief                   Function handler for static Call back function of finding target device
		 * @param[in] handle        finding target device callback handle
		 * @[in] access_struct      acces struct passed in for the DCI Access
		 * @return None
		 */
		static DCI_CB_RET_TD Find_Target_Device_Callback_Static( DCI_CBACK_PARAM_TD handle,
																 DCI_ACCESS_ST_TD* access_struct );

		/**
		 * @brief                        Function handler for Call back function of finding target device
		 * @param[in] access_struct      acces struct passed in for the DCI Access
		 * @return DCI_CB_RET_TD         Return the status of the call back function
		 */
		DCI_CB_RET_TD Find_Target_Device_Callback( DCI_ACCESS_ST_TD* access_struct );

};

#endif
