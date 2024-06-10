/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the LED indication interface with Ble protocols
 *
 *  @details LED indication for Ble protocol
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_DISPLAY_H
#define BLE_DISPLAY_H

/**
 ****************************************************************************************
 * @brief This is a Ble_Display class
 * @details It provides an LED indication for Ble protocol,
 * @n @b Usage: handle the flashing behavior
 * It provides public methods to
 * @n @b 1. Turn on LED
 * @n @b 2. Turns LED solid off
 ****************************************************************************************
 */
class Ble_Display
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Creates a Ble display
		 *  @param[in] None
		 *  @n @b Usage: Will handle the flashing and on/off behavior
		 *  @param[in] None
		 *  @return None
		 */
		Ble_Display( void ) {}

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Ble_Display goes out of scope
		 *  @return None
		 */
		virtual ~Ble_Display( void ){}

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details This mode is displayed Some Faulty condition has occurred where
		 *	the bluetooth link is down or failed. Must determine conditions
		 *	under which this may happen.
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Attention( void ) = 0;

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details This mode is displayed Some Faulty condition has occurred where
		 *	the bluetooth link is down or failed. Must determine conditions
		 *	under which this may happen.
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Device_Faulted( void ) = 0;

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details This mode is displayed A valid connection has been established
		 *	and devices are in sync.
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Device_Connected( void ) = 0;

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details This mode is displayed while the device is in pairing mode.
		 *	The pairing state should be temporary to avoid causing user
		 *	anxiety. The colors should flash in sync.
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Pairing_Mode( void ) = 0;

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details This mode is displayed,If the BLE application
		 *	is currently polling and/or writing, this shall be the indication
		 *	state. What this indicates is that data is exchanged. There should
		 *	be a minimum time of flashing behavior for every access. At least one complete
		 *	cycle.
		 *  @return None
		 */
		virtual void Data_Operations( void ) = 0;

		/**
		 *  @brief The virtual functions. These virtual functions implemented in the
		 *  Single LED.
		 *  @details this mode is to indicate the Device is disconnected and reinitiated for
		 *	Advertising
		 *  @param[in] None
		 *  @return None
		 */
		virtual void Device_Disconnected( void ) = 0;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Ble_Display( const Ble_Display& object );
		Ble_Display & operator =( const Ble_Display& object );

};

#endif
