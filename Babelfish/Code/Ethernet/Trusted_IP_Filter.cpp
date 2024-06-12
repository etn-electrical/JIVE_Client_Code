/**
 **********************************************************************************************
 * @file            Trusted_IP_Filter.cpp C++ Implementation File for Eth-Host IP Trusted filter
 *                  module
 *
 * @brief           The file contains all APIs required for supporting IP filtering feature.
 *                  This can be used for adding global IP white list  for IP protocols.
 * @details         See header file for module overview.
 * @copyright       2017 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Trusted_IP_Filter.h"
#include "lwip/tcp.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Assert.h"

uint8_t Trusted_IP_Filter::m_total_ports = 0U;
uint16_t Trusted_IP_Filter::max_trusted_list_num = 0U;
Trusted_IP_Filter::trusted_host_port_info_owner_t* Trusted_IP_Filter::m_trusted_port =
	reinterpret_cast<Trusted_IP_Filter::trusted_host_port_info_owner_t*>( nullptr );
uint32_t* Trusted_IP_Filter::m_trusted_filter_list_buff =
	reinterpret_cast<uint32_t*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Trusted_IP_Filter::Trusted_IP_Filter( const uint8_t total_ports_num,
									  trusted_host_port_info_t* ip_port_list )
{
	bool create_owner;

	// total ports is zero, disable "#define TRUSTED_IP_FILTER" if IP filtering not required.
	BF_ASSERT( total_ports_num != 0U );

	m_total_ports = total_ports_num;

	m_trusted_port = new trusted_host_port_info_owner_t[total_ports_num];

	uint8_t j = 0;

	for ( uint8_t i = 0U; i < total_ports_num; i++ )
	{
		create_owner = true;

		/* create owner for first trusted IP whitelist dcid */
		if ( i != 0U )
		{
			/* check for different trusted IP whitelist dcid */
			for ( j = 0; j < i; j++ )
			{
				if ( ( ( &ip_port_list[i] )->trusted_ip_list == ( &ip_port_list[j] )->trusted_ip_list ) )
				{
					create_owner = false;
					break;	// break the loop the owner is created
				}
			}
		}

		if ( create_owner == true )
		{
			m_trusted_port[i].trusted_ip_list_owner = new DCI_Owner(
				( &ip_port_list[i] )->trusted_ip_list );

			ETH_DEBUG_PRINT( DBG_MSG_INFO, "DCI Owner created for DCI ID - %d",
							 ( &ip_port_list[i] )->trusted_ip_list );

			uint8_t ip_arr_len = m_trusted_port[i].trusted_ip_list_owner->Get_Length();

			if ( ip_arr_len % 4 )
			{
				// should be in multiple of four bytes
				BF_ASSERT( false );
			}
			else
			{
				m_trusted_port[i].trusted_list_num = ip_arr_len >> 2U;

				if ( max_trusted_list_num < m_trusted_port[i].trusted_list_num )
				{
					max_trusted_list_num = m_trusted_port[i].trusted_list_num;
				}
			}
		}
		else
		{
			m_trusted_port[i].trusted_ip_list_owner = m_trusted_port[j].trusted_ip_list_owner;
			m_trusted_port[i].trusted_list_num = m_trusted_port[j].trusted_list_num;
		}

		m_trusted_port[i].filter_enable_dcid_owner = new DCI_Owner(
			( &ip_port_list[i] )->filter_enable_dcid );
		ETH_DEBUG_PRINT( DBG_MSG_INFO, "DCI Owner created for DCI ID - %d",
						 ( &ip_port_list[i] )->filter_enable_dcid );

		m_trusted_port[i].port_num = ( &ip_port_list[i] )->port_num;
		m_trusted_port[i].accept = reinterpret_cast<accept_t>( nullptr );
	}

	m_trusted_filter_list_buff = new uint32_t[max_trusted_list_num];
	ETH_DEBUG_PRINT( DBG_MSG_INFO, "Max length of trusted IP whitelist is - %d", max_trusted_list_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Trusted_IP_Filter::Is_Remote_Trusted( const uint32_t remote_ip, uint16_t offset )
{
	bool trusted = false;

	// temp solution, reverse 32 bit ip octets
	uint32_t temp_remote_ip = ( ( remote_ip & IP_PASS_FOURTH_OCTET_MASK ) >> 24 )
		| ( ( ( remote_ip & IP_PASS_THIRD_OCTET_MASK ) >> 16 ) << 8 )
		| ( ( ( remote_ip & IP_PASS_SECOND_OCTET_MASK ) >> 8 ) << 16 )
		| ( ( remote_ip & IP_PASS_FIRST_OCTET_MASK ) << 24 );

	m_trusted_port[offset].trusted_ip_list_owner->Check_Out( m_trusted_filter_list_buff );

	for ( uint32_t i = 0U; ( i < m_trusted_port[offset].trusted_list_num && trusted == false );
		  i++ )
	{
		if ( m_trusted_filter_list_buff[i] != IP_PASS_ALL_MASK )
		{
			if ( ( m_trusted_filter_list_buff[i] & IP_PASS_FIRST_OCTET_MASK ) !=
				 IP_PASS_FIRST_OCTET_MASK )
			{
				if ( temp_remote_ip == m_trusted_filter_list_buff[i] )
				{
					trusted = true;
				}
			}
			else if ( ( m_trusted_filter_list_buff[i] & IP_PASS_SECOND_OCTET_MASK ) !=
					  IP_PASS_SECOND_OCTET_MASK )
			{
				if ( ( temp_remote_ip
					   & ( IP_PASS_SECOND_OCTET_MASK | IP_PASS_THIRD_OCTET_MASK
						   | IP_PASS_FOURTH_OCTET_MASK ) ) ==
					 ( m_trusted_filter_list_buff[i]
					   & ( IP_PASS_SECOND_OCTET_MASK | IP_PASS_THIRD_OCTET_MASK
						   | IP_PASS_FOURTH_OCTET_MASK ) ) )
				{
					trusted = true;
				}
			}
			else if ( ( m_trusted_filter_list_buff[i] & IP_PASS_THIRD_OCTET_MASK ) !=
					  IP_PASS_THIRD_OCTET_MASK )
			{
				if ( ( temp_remote_ip &
					   ( IP_PASS_THIRD_OCTET_MASK | IP_PASS_FOURTH_OCTET_MASK ) ) ==
					 ( m_trusted_filter_list_buff[i]
					   & ( IP_PASS_THIRD_OCTET_MASK | IP_PASS_FOURTH_OCTET_MASK ) ) )
				{
					trusted = true;
				}
			}
			else if ( ( temp_remote_ip & IP_PASS_FOURTH_OCTET_MASK ) ==
					  ( m_trusted_filter_list_buff[i] & IP_PASS_FOURTH_OCTET_MASK ) )
			{
				trusted = true;
			}
		}
		else
		{
			trusted = true;
		}

		if ( trusted == false )
		{
			ETH_DEBUG_PRINT( DBG_MSG_INFO, "Remote IP %x is not trusted, port - %d", remote_ip,
							 m_trusted_port[offset].port_num );
		}
	}
	return ( trusted );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
err_t Trusted_IP_Filter::Trusted_IP_Accept_Cb( void* arg, struct tcp_pcb* pcb, err_t error )
{
	bool port_filter_ena = true;
	bool trusted_ip = true;
	err_t cb_error = ERR_RST;

	for ( uint8_t i = 0U; i < m_total_ports; i++ )
	{
		if ( pcb->local_port == m_trusted_port[i].port_num )
		{
			m_trusted_port[i].filter_enable_dcid_owner->Check_Out( &port_filter_ena );

			if ( port_filter_ena == true )
			{
#ifndef ESP32_SETUP
				trusted_ip = Is_Remote_Trusted( pcb->remote_ip.addr, i );
#else
				trusted_ip = Is_Remote_Trusted( pcb->remote_ip.u_addr.ip4.addr, i );
#endif
			}

			if ( trusted_ip == true )
			{
				if ( m_trusted_port[i].accept != reinterpret_cast<accept_t>( nullptr ) )
				{
					cb_error = m_trusted_port[i].accept( arg, pcb, error );
				}
			}
			i = m_total_ports;
		}
	}
	return ( cb_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Trusted_IP_Filter::Register_Accept_Callback( uint16_t port, struct tcp_pcb* pcb,
												  accept_t accept )
{
	bool registered = false;

	for ( uint8_t i = 0U; i < m_total_ports; i++ )
	{
		if ( port == m_trusted_port[i].port_num )
		{
			tcp_accept( pcb, Trusted_IP_Accept_Cb );
			m_trusted_port[i].accept = accept;
			registered = true;
			i = m_total_ports;
		}
	}
	return ( registered );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Trusted_IP_Filter::Is_Remote_Ip_Trusted( const uint32_t remote_ip, const uint16_t port )
{
	bool port_filter_ena = true;
	bool trusted_ip = true;

	for ( uint8_t i = 0U; i < m_total_ports; i++ )
	{
		if ( port == m_trusted_port[i].port_num )
		{
			m_trusted_port[i].filter_enable_dcid_owner->Check_Out( &port_filter_ena );

			if ( port_filter_ena == true )
			{
				trusted_ip = Is_Remote_Trusted( remote_ip, i );
			}

			i = m_total_ports;
		}
	}
	return ( trusted_ip );
}
