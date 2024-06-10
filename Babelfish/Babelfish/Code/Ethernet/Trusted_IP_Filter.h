/**
 **********************************************************************************************
 * @file            Trusted_IP_Filter.h C++ Implementation File for Eth-Host IP Trusted filter
 * 					module
 *
 * @brief           The file contains all APIs required for supporting IP filtering feature.
 *                  This can be used for adding global IP white list  for IP protocols.
 * @details         See header file for module overview( currently supported only for TCP raw API ).
 * @copyright       2017 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef _TRUSTED_IP_FILTER_H_
#define _TRUSTED_IP_FILTER_H_

#include "Includes.h"
#include "DCI_Owner.h"
#include "lwip/err.h"

class Trusted_IP_Filter
{
	public:

		/** @brief struct to hold port number and access control DCID for each ip port/protocol*/
		struct trusted_host_port_info_t
		{
			const uint16_t port_num;
			DCI_ID_TD filter_enable_dcid;
			DCI_ID_TD trusted_ip_list;
		};

		/** @brief type define for tcp_accept callback */
		typedef err_t (* accept_t)( void* arg, struct tcp_pcb* newpcb, err_t err );

		/**
		 * @brief       Constructor to configure IP white list for IP protocols.
		 * @param[in]   total_ports_num - Number of IP ports, must be equal
		 *              to length of trusted_port_enable_t* ip_port_list array
		 * @param[in]   trusted_host_port_info_t * ip_port_list - array of structure trusted_port_enable_t,
		 *				should contain the information of the ports which needs IP filtering as well as ip filtering
		 * list DCID.
		 *              Host IP filtering can be applied in different ways and can be combined as well in DCID array -
		 *              a) 0xFFFFFFFF passes all host ip addresses
		 *              b) octet 0xFF passes all host ip address in the same subnet
		 *              	(eg - 0xFF01A8C0 - pass from  192.168.1.1 to  192.168.1.255
		 *              	or   eg - 0xFFFFA8C0 - pass from  192.168.1.1 to  192.168.255.255 )
		 *              c) blocks others except one(eg - 0x0901A8C0 - pass only 192.168.1.9)
		 */
		Trusted_IP_Filter( const uint8_t total_ports_num, trusted_host_port_info_t* ip_port_list );

		/**
		 * @brief       Registers the port specific accept callback to
		 *              be called after ip filtering validation. not required for Prod to call
		 *				explicitly.
		 * @param[in]   uint16_t port - port number ( For Example 80, 443, 502 )
		 * @param[in]   struct tcp_pcb* pcb - tcp pcb allocated block
		 *              should contain the information of the ports which needs IP filtering.
		 *              to length of trusted_port_enable_t* ip_port_list array
		 * @param[in]   accept_t accept - port specific accept callback
		 * @return      true if registered successfully else false
		 */
		static bool Register_Accept_Callback( const uint16_t port, struct tcp_pcb* pcb,
											  accept_t accept );

		/**
		 * @brief       Check givent ip is trusted or not.
		 * @param[in]   remote_ip 	Ip address of client
		 * @param[in]   port		Represents application (BACnet,ModbusTCP or http)
		 * @return      true if Ip is trusted else false
		 */
		static bool Is_Remote_Ip_Trusted( const uint32_t remote_ip, const uint16_t port );

		~Trusted_IP_Filter();

	private:

		static const uint32_t IP_PASS_ALL_MASK = 0xFFFFFFFFU;
		static const uint32_t IP_PASS_FOURTH_OCTET_MASK = 0xFF000000U;
		static const uint32_t IP_PASS_THIRD_OCTET_MASK = 0x00FF0000U;
		static const uint32_t IP_PASS_SECOND_OCTET_MASK = 0x0000FF00U;
		static const uint32_t IP_PASS_FIRST_OCTET_MASK = 0x000000FFU;

		struct trusted_host_port_info_owner_t
		{
			DCI_Owner* filter_enable_dcid_owner;
			DCI_Owner* trusted_ip_list_owner;
			uint16_t trusted_list_num;
			uint16_t port_num;
			accept_t accept;
		};

		static uint8_t m_total_ports;
		static uint16_t max_trusted_list_num;
		static trusted_host_port_info_owner_t* m_trusted_port;
		static uint32_t* m_trusted_filter_list_buff;

		static bool Is_Remote_Trusted( const uint32_t remote_ip, uint16_t port_offset );

		static err_t Trusted_IP_Accept_Cb( void* arg, struct tcp_pcb* pcb, err_t error );

};

#endif	/* _TRUSTED_IP_FILTER_H_ */
