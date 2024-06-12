/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the interface between Modbus host port and DCI database.
 *
 *	@details Modbus host port communication settings can update by DCI owners related to modbus communication
 *  @n DCI - Data store
 *  @n DCI Owners - Owners of the store and hence has more privileges
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DCI_CONFIG_H
#define MODBUS_DCI_CONFIG_H

#include "Modbus_Defines.h"
#include "Modbus_Net.h"
#include "Modbus_DCI.h"
#include "DCI_Owner.h"

namespace BF_Mbus
{

/**
 ****************************************************************************************
 * @brief This is a Modbus_DCI_Config class
 * @details It provides an interface between Modbus host and DCI database.
 * @n @b Usage: Modbus host port communication settings
 * It provides public methods to
 * @n @b 1. To Update transmission mode
 * @n @b 2. To Create the Patron
 * @n @b 3. To Update parity
 * @n @b 4. To Enable Modbus host port
 * @n @b 5. To Disable Modbus host port
 ****************************************************************************************
 */
class Modbus_DCI_Config
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus_DCI_Config
		 *  @param[in] modbus_net_ctrl is pointer to Modbus_Net
		 *    @n @b Usage: initialize Modbus host port
		 *  @param[in] address_id DCI ID
		 *    @n @b Usage: modbus slave address
		 *  @param[in] parity_id DCI ID
		 *    @n @b Usage: modbus communication parity
		 *  @param[in] address_id DCI ID
		 *    @n @b Usage: modbus slave address
		 *  @param[in] tx_mode_id DCI ID
		 *    @n @b Usage: modbus transmission mode
		 *  @param[in] baud_rate_id DCI ID
		 *    @n @b Usage: modbus transmission baud rate
		 *  @param[in] use_long_holdoff
		 *    @n @b Usage: "use_long_holdoff = true" means 1.75ms is used as the min msg holdoff.
		 *    As per Modbus spec,"The implementation of RTU reception driver may imply
		 *    the management of a lot of interruptions due to the t1.5 and t3.5 timers.
		 *    With high communication baud rates, this leads to a heavy CPU load.
		 *    Consequently these two timers must be strictly respected when the baud rate is equal or lower
		 *    than 19200 Bps. For baud rates greater than 19200 Bps,fixed values for the 2 timers
		 *    should be used: it is recommended to use a value of 750µs for the inter-character time-out (t1.5)
		 *    and a value of 1.750ms for inter-frame delay (t3.5)"So in its Usage update that if use_long_holdoff
		 *    is true means fixed timeout ised for above 19200 baudrate, If its value is false means timeouts will
		 *    not be fixed but will be  calculated as 3.5 and 1.5 character times for that respective baudrate
		 *  @return None
		 */

		Modbus_DCI_Config( Modbus_Net* modbus_net_ctrl,
						   DCI_ID_TD address_id, DCI_ID_TD parity_id,
						   DCI_ID_TD tx_mode_id, DCI_ID_TD baud_rate_id,
						   bool use_long_holdoff = true );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_DCI_Config goes out of scope
		 *  @return None
		 */
		~Modbus_DCI_Config( void );

		/// See Modbus_Defines for the following values.

		/**
		 *  @brief Update transmission mode
		 *  @details Writes the value of tx_mode value to DCI RAM/NV RAM depend upon DCI attribute.
		 *  @return None
		 */
		void Update_TX_Mode( uint8_t tx_mode ) const;

		/**
		 *  @brief Update transmission mode
		 *  @details Writes the value of baud_rate to the DCI NV RAM data value depend upon DCI attribute
		 *  @return None
		 */
		void Update_Baud_Rate( uint32_t baud_rate ) const;

		/**
		 *  @brief Update Parity
		 *  @details Writes the value of parity to the DCI RAM/NVRAM data value depend upon DCI attribute
		 *  @param[in] None
		 *  @return None
		 */
		void Update_Parity( uint8_t parity ) const;

		/**
		 *  @brief
		 *  @details Enable modbus host port with specified communication settings by DCI owners of modbus communication
		 *  @return None
		 */
		void Enable_Modbus( void );

		/**
		 *  @brief
		 *  @details Disable modbus host port
		 *  @return None
		 */
		void Disable_Modbus( void );

		/**
		 *  @brief
		 *  @details Determine Modbus host enable status
		 *  @return True if enable & vice-versa.
		 */
		bool Is_Modbus_Enabled( void );

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_DCI_Config( const Modbus_DCI_Config& object );
		Modbus_DCI_Config & operator =( const Modbus_DCI_Config& object );

		DCI_Owner* m_address_owner;
		DCI_Owner* m_parity_owner;
		DCI_Owner* m_stop_bit_owner;
		DCI_Owner* m_tx_mode_owner;
		DCI_Owner* m_baud_rate_owner;
		Modbus_Net* m_modbus_net;
		bool m_use_long_holdoff;
};

}

#endif	/* MODBUS_DCI_CONFIG_H */
