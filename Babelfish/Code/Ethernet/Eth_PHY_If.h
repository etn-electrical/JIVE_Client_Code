/**
 **********************************************************************************************
 * @file            Eth_PHY_If Header File for interface class.
 *
 * @brief           The file contains Eth_PHY_If, an interface class required
 *                  for communication with Ethernet PHY/SWITCH.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef ETH_PHY_IF_H_
#define ETH_PHY_IF_H_

#include "Eth_Port.h"

/**
 **********************************************************************************************
 * @brief        Ethernet _PHY_If class declaration.
 * @n            This class includes all the functions required for communication
 *               with a Ethernet PHY/Switch
 **********************************************************************************************
 */
class Eth_PHY_If
{
	public:

		/**
		 * @brief                  Constructor to create an instance of the ETH_PHY_If class.
		 * @param[in] phy_ctrl :   Pointer to handle of PHY Driver(LAN or KSZ8863)
		 * @param[in] total ports: The total number active ports.
		 * @param[in] eth_dcids:   The structure consisting of all the Ethernet port DCID parameters.
		 */
		Eth_PHY_If( PHY_Driver* phy_ctrl, uint8_t total_ports,
					Eth_Port::eth_dcid_t const* eth_dcids );

		/**@brief Destructor to delete the instance of the ETH_PHY_If class when it goes out of scope.*/
		~Eth_PHY_If()
		{}

		/**
		 * @brief      Function used for initial link checking.
		 * @param[out]  returns true if any port's link is up in current polling, else returns false.
		 */
		bool Initial_Link_Check();

		/**@brief      Function to reset PHY.*/
		void Reset_PHY();

		/**
		 * @brief    Function to update the PHY status.
		 * @n        Take updates of Auto negotiation, Speed, Mode, Ethernet Link is Up or Down
		 */
		void Update_PHY_Status();

		/**
		 * @brief    Function to read the diagnostics information of Ethernet PHY..
		 */
		uint32_t const* Read_Diag_Information( uint8_t ) const;

		/**
		 * @brief    Function to to read the error list of Ethernet PHY Driver.
		 */
		const uint8_t* const* Read_Phy_Driver_Error_List() const;

		/**
		 * @brief   Returns the number of port of the PHY.
		 */
		uint8_t Read_Port_Count() const;

	private:

		Eth_PHY_If( const Eth_PHY_If& );
		Eth_PHY_If & operator =( const Eth_PHY_If & );

		void Set_Netif_Link() const;

		bool m_initial_polling;

		class PHY_Driver* m_phy_handle;
		Eth_Port** m_port;
		uint8_t m_total_ports;
};

#endif
