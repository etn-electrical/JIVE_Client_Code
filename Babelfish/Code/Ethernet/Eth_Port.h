/**
 **********************************************************************************************
 * @file            Eth_Port Header File for individual Ethernet Port communication.
 *
 * @brief           The file contains Eth_Port Class required for communication
 *                  over Ethernet port .
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef ETH_PORT_H
#define ETH_PORT_H

#include "PHY_Driver.h"
#include "DCI_Owner.h"

/**
 **********************************************************************************************
 * @brief        Ethernet _Port class declaration
 * @n            This class controls all port specific operations such
 *               as set port attributes, read port status.
 **********************************************************************************************
 */
class Eth_Port
{

	public:

		/**@brief     Structure consisting of all the attributes for the Ethernet link and port*/
		struct eth_dcid_t
		{
			DCI_ID_TD eth_active_link_speed;
			DCI_ID_TD eth_active_link_duplex;
			DCI_ID_TD eth_active_link_autoneg_state;
			DCI_ID_TD eth_port_enable_status;
			DCI_ID_TD eth_port_link_speed_select;
			DCI_ID_TD eth_port_full_duplex_ena;
			DCI_ID_TD eth_port_auto_neg_ena;
		};

		/** @brief      Port powered on current polling returns boolean output.*/
		bool m_port_up_on_curr_poll;

		/** @brief     Port powered on last polling returns boolean output.*/
		bool m_port_up_on_last_poll;

		/**
		 * @brief                      Constructor to create an instance of the Eth_Port class.
		 * @param [in] PortNumber       Ethernet port which is to be initialized
		 * @param [in] eth_dcids        The structure consisting of all the Ethernet port DCID
		 * parameters.
		 */
		Eth_Port( uint8_t port, eth_dcid_t const* eth_dcids );

		/** @brief                      Destructor to delete the the Eth_Port instance when it goes
		   out of scope.*/
		~Eth_Port()
		{}

		/**
		 * @brief   Function to set port configuration.
		 * @return  It returns a binary output true if successful else returns false.
		 */
		bool Set_Port_Config();

		/**
		 * @brief                      Function to update port attributes to port DCID parameters.
		 * @return                     It returns a binary output true if successful else returns
		 * false.
		 */
		void Update_Port_Status();

		/**
		 * @brief                      Function that returns the diagnostic information buffer.
		 * @return                     It returns a pointer to diagnostic information array.
		 */

		const uint32_t* Read_Port_Diag_Info();

		/**
		 * @brief                      This function is used to select the PHY driver.
		 * @param[in] phy_driver       The handle PHY driver to be selected.
		 */
		static void Select_PHY( PHY_Driver* phy_driver );

		static const uint8_t ETH_AUTO_NEG_STATE_LINK_INACTIVE = 0U;
		static const uint8_t ETH_AUTO_NEG_STATE_IN_PROGRESS = 1U;
		static const uint8_t ETH_AUTO_NEG_STATE_FAIL_DEFAULT_USED = 2U;
		static const uint8_t ETH_AUTO_NEG_STATE_DUPLEX_FAIL_SPEED_OK = 3U;
		static const uint8_t ETH_AUTO_NEG_STATE_SUCCESS = 4U;
		static const uint8_t ETH_AUTO_NEG_STATE_NEGOTIATION_DISABLED = 5U;
		static const uint8_t ETH_AUTO_NEG_STATE_PORT_DISABLED = 6U;

	private:

		static const uint16_t ETH_LINK_SPEED_UNDETERMINED = 0U;
		static const uint16_t ETH_LINK_SPEED_10M = 10U;
		static const uint16_t ETH_LINK_SPEED_100M = 100U;
		static const uint16_t ETH_LINK_SPEED_1G = 1000U;
		/** @brief    Data type for the Ethernet link speed */
		typedef uint16_t Eth_Link_Speed_t;

		/** @brief    Data type for the Ethernet auto negotiation  state */
		typedef uint8_t Eth_AN_State_t;

		static const Eth_Link_Speed_t LINK_SPEED_ARRAY[PHY_Driver::SPEED_OPTIONS];

		Eth_Port( const Eth_Port& );
		Eth_Port & operator =( const Eth_Port & );

		void Update_Port_Error_Counts( const PHY_Driver::port_config_t& read_status_word );

		DCI_Owner* m_active_autoneg_own;
		DCI_Owner* m_active_full_dup_own;
		DCI_Owner* m_active_speed_own;

		uint32_t m_diag_counter[PHY_Driver::DIAG_PARAM_COUNT];	// 0-Link up, 1 = Link down , 2 =
																// jabber, 3 = remote
		uint8_t m_link_status;
		uint8_t m_link_ack;
		uint8_t m_port;

		PHY_Driver::port_config_t m_port_config;
		static PHY_Driver* m_eth_phy_driver;

};

#endif

