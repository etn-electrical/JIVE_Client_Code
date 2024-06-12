/**
 **********************************************************************************************
 *	@file            Ethernet.h Header File for Ethernet module implementation.
 *
 *	@brief           The file shall wrap all the functions which are required for Ethernet
 * communication
 *                   This file will be processor and Physical transceiver independent.
 *	@details
 *	@copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef ETHERNET_H
#define ETHERNET_H

#include "Defines.h"
#include "DCI_Owner.h"
#include "OS_Tasker.h"
#include "LWIPService.h"
#ifndef ESP32_SETUP
#include "lwip/ACD.h"
#endif
#include "Eth_PHY_If.h"
#include "lwip/inet.h"
#include "lwip/def.h"
/**
 **********************************************************************************************
 * @brief IP address mode selection
 * @n                     0 = uC_IP_ADDRESS_SELECT_NV_MEM
 * @n                     0 = uC_ETHERNET_IP_SELECT_PASSED
 * @n                     1 = uC_ETHERNET_IP_SELECT_DHCP
 **********************************************************************************************
 */
typedef enum uC_ETHERNET_IP_SELECT
{
	uC_IP_ADDRESS_SELECT_NV_MEM = 0,
	uC_ETHERNET_IP_SELECT_PASSED = 0,
	uC_ETHERNET_IP_SELECT_DHCP
} uC_ETHERNET_IP_SELECT;

/**
 * @brief Structure to hold all the IP settings like IP address mode, ip address, defatult gateway,
 * subnetmask.
 */
typedef void CALLBACK_FUNC ( void );

typedef struct uC_ETHERNET_IP_CONFIG_ST
{
	uC_ETHERNET_IP_SELECT ip_select;
	uint32_t ip_address;
	uint32_t default_gateway;
	uint32_t subnet_mask;
} uC_ETHERNET_IP_CONFIG_ST;

/**
 **********************************************************************************************
 * @brief                   Ethernet Class Declaration. The class will handle the
 *                          functionalities related to Ethernet Initialization.
 **********************************************************************************************
 */
class Eth_if
{
	public:
		/**
		 * @brief                  Constructor to create the Ethernet class.
		 * @param[in] phy_ctrl :   Pointer to handle of PHY Driver(LAN or KSZ8863)
		 * @param[in] total ports: The total number active ports.
		 * @param[in] eth_dcids:   The structure consisting of all the Ethernet port DCID
		 * parameters.
		 * @param[in] ip_config:   IP address of the device.
		 * @param[in] post_setup_func:   Operations performed after LWIP stack initilization.
		 * @return                  None
		 */

		Eth_if( PHY_Driver* phy_ctrl, uint8_t total_ports, Eth_Port::eth_dcid_t const* eth_dcids,
				uC_ETHERNET_IP_CONFIG_ST* ip_config,
				CALLBACK_FUNC* post_setup_func );

		/**@brief Destructor to delete the instance of the ETH_PHY_If class when it goes out of
		   scope.*/
		~Eth_if( void )
		{}

		/**
		 * @brief                               Loads the IP address in to  active_ip_config
		 * structure.
		 * @param[in] * active_ip_config        Pointer of active IP address used by device.
		 * @return BOOL    m_ip_address_locked  returns true if IP address is locked.
		 */
		static BOOL Get_IP_Config( uC_ETHERNET_IP_CONFIG_ST* active_ip_config );

		/**
		 * @brief                               Static function to access Ethernet_Start_Thread.
		 * @param[in] os_param                  OS tasker handle.
		 * @return                              None
		 */

		static void Ethernet_Start_Thread_Static( OS_TASK_PARAM os_param )
		{
			( ( Eth_if* )os_param )->Ethernet_Start_Thread();
		}

		/**
		 * @brief   Calls 'LWIPServiceTaskInit' which inits Tcpip, ACD, DHCP.
		 * @n       Calls 'm_post_setup_func' which Starts EIPTask,inits Modbus-TCP port, & Http.
		 * @n       Monitors the Phy  Status.
		 */
		void Ethernet_Start_Thread( void ) const;

		/**
		 * @brief   Static function which call Calls 'Update_IP_Config'
		 */
		static void UpdateActiveIPConfig( void );

		/**
		 * @brief   Reads the Active IP address assigned by LWIP stack and loads into NV mem.
		 */
		void Update_IP_Config() const;

		/**
		 * @brief   Static function to call the Reset_PHY function of Ethernet PHY.
		 */
		static void Reset_PHY( void );

		/**
		 * @brief   Static function to call the InitialLinkCheck function of Ethernet PHY.
		 */
		static BOOL InitialLinkCheck( void );

		/**
		 * @brief   Function to read the diagnostics information of Ethernet PHY.
		 */
		uint32_t const* Read_Diag_Information( uint8_t port_index ) const;

		/**
		 * @brief   Function to read the error list of Ethernet PHY Driver.
		 */
		const uint8_t* const* Read_Phy_Driver_Error_List() const;

		/**
		 * @brief   Function to read the total number of port for the device.
		 */
		uint8_t Read_Port_Count() const;

		/**
		 * @brief   Function to set update host name handle called from httpd
		 */
		static void Update_Hostname_Handle( void ( * fptr_update_hostname )( uint32_t ) );

		/** @brief Handle to access functions of Eth_Phy_If class */
		static Eth_PHY_If* m_eth_phy;
		/** @brief Handle for Ethernet class to give access of its functions to LWIP stack*/
		static Eth_if* m_self;
		/** @brief function Pointer to set update host name function callback */
		static void (* m_update_hostname_cb)( uint32_t ip );

		/** @brief Should be called from OS task only.
		 * This function is added to wait till IP is allocated for functionality like IOT client.
		 * (additionaly) print allocated IP address on terminal.
		 */
		static void Wait_Till_Ip_Configured( void );

		/** @brief This function is to check whether IP is allocated or not.
		 * (additionally) print allocated IP address on terminal if allocated.
		 */
		static bool_t Is_Ip_Configured( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 * @n The fact that we are disallowing these standard items should be Doxygen
		 * documented.  Generally private members do not need to be Doxygen documented.
		 */
		Eth_if( const Eth_if& object );
		Eth_if & operator =( const Eth_if& object );

		static CALLBACK_FUNC* m_post_setup_func;
		static BOOL m_ip_address_locked;
		static uC_ETHERNET_IP_CONFIG_ST m_ip_config;
};

#endif
