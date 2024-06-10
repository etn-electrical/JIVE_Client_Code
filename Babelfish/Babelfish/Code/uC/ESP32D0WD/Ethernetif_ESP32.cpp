/**
 *****************************************************************************************
 * @file            ethernetif.cpp Header File for interface with lwip.
 *
 * @brief           handshaking file between LWIP and ethernet hardware
 *
 * @details         This file is a skeleton for developing Ethernet network interface
 *                  drivers for lwIP. Add code to the low_level functions and do a
 *                  search-and-replace for the word "ethernetif" to replace it with
 *                  something that better describes your network interface.
 *
 * @copyright (c)   2001-2004 Swedish Institute of Computer Science.
 *                  All rights reserved.
 *
 *                  Redistribution and use in source and binary forms, with or without modification,
 *                  are permitted provided that the following conditions are met:
 *
 *                  1. Redistributions of source code must retain the above copyright notice,
 *                     this list of conditions and the following disclaimer.
 *                  2. Redistributions in binary form must reproduce the above copyright notice,
 *                     this list of conditions and the following disclaimer in the documentation
 *                     and/or other materials provided with the distribution.
 *                  3. The name of the author may not be used to endorse or promote products
 *                     derived from this software without specific prior written permission.
 *
 *                  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *                  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *                  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 *                  SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *                  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 *                  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *                  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *                  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *                  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 *                  OF SUCH DAMAGE.
 *
 *                  This file is part of the lwIP TCP/IP stack.
 *
 *                  Author: Adam Dunkels <adam@sics.se>

 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */

#include "Includes.h"
#include <string.h>
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "OS_Tasker.h"
#include "OS_Queue.h"
#include "OS_Mutex.h"
#include "OS_Binary_Semaphore.h"
#include "EMAC_Driver.h"
#include "Eth_if.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Debug.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define NETIF_INTERFACE_TASK_STACK_SIZE             ( 5000U )
#define NETIF_INTERFACE_TASK_PRIORITY               OS_TASK_PRIORITY_5
#define LINK_SPEED_OF_YOUR_NETIF_IN_BPS ( 100U )

#define ETHERNETIF_SLEEP_TIME_MS                5

enum init_status_t
{
	SUCCEED,
	FAILURE
};

static bool dhcp_ip_get = false;

void Ethernet_If_EMAC( EMAC_Driver* emac );

void Ethernet_IF_Input_Func( void* netif, uint8_t* buf, uint32_t length );

OS_Mutex* ETH_Tx_Access_Mutex = reinterpret_cast<OS_Mutex*>( NULL );

EMAC_Driver* emac_handle;
uint8_t* Tx_Buf = nullptr;
/**
 * @brief       -  Receives the EMAc driver handle
 * @param emac  -  EMAC handle
 */
void Ethernet_If_EMAC( EMAC_Driver* emac )
{
	emac_handle = emac;
}

/**
 * @brief                   This function Free buf created
 * @param[in] netif         The already initialized lwip network interface structure
 *                          for this ethernetif
 * @param[in] buf           buf pointer which is to be freed
 * @param[out] none         None
 * @n
 */
static void Ethernet_Free_Rx_Buf_L2( struct netif* netif, void* buf )
{
	free( buf );
}

/**
 * @brief                   This function is a interface between lwip and ethernet hardware,
 *                          which pass data to input function for processing
 * @param[in] netif         The already initialized lwip network interface structure
 *                          for this ethernetif
 * @param[in] buffer        buffer pointer which holds data received
 * @param[in] len           length of the data to be transfered
 * @param[out] none         None
 */
void Ethernet_IF_Input_Func( void* netif, uint8_t* buffer, uint32_t len )
{
	struct netif* netif_ptr = reinterpret_cast<struct netif*>( netif );
	struct pbuf* pbuf_ptr;

	if ( unlikely( ( buffer == NULL ) || !netif_is_up( netif_ptr ) ) )
	{
		if ( buffer )
		{
			Ethernet_Free_Rx_Buf_L2( netif_ptr, buffer );
		}
		return;
	}

	/* acquire new pbuf, type: PBUF_REF */
	pbuf_ptr = pbuf_alloc( PBUF_RAW, len, PBUF_REF );
	if ( pbuf_ptr == NULL )
	{
		Ethernet_Free_Rx_Buf_L2( netif_ptr, buffer );
		return;
	}
	pbuf_ptr->payload = buffer;
#if ESP_LWIP
	pbuf_ptr->l2_owner = netif_ptr;
	pbuf_ptr->l2_buf = buffer;
#endif

	/* full packet send to tcpip_thread to process */
	if ( unlikely( netif_ptr->input( pbuf_ptr, netif_ptr ) != ERR_OK ) )
	{
		// coverity[no_escape]
		LWIP_DEBUGF( NETIF_DEBUG, ( "ethernetif_input: IP input error\n" ) );
		pbuf_free( pbuf_ptr );
	}
	/* the pbuf will be free in upper layer, eg: ethernet_input */

	if ( dhcp_ip_get == false )
	{
		if ( netif_ptr->ip_addr.u_addr.ip4.addr != 0 )
		{
			char ip_buff[4];
			ip_buff[0] = ( ( netif_ptr->ip_addr.u_addr.ip4.addr ) >> 0 & 0xFF );
			ip_buff[1] = ( ( netif_ptr->ip_addr.u_addr.ip4.addr ) >> 8 & 0xFF );
			ip_buff[2] = ( ( netif_ptr->ip_addr.u_addr.ip4.addr ) >> 16 & 0xFF );
			ip_buff[3] = ( ( netif_ptr->ip_addr.u_addr.ip4.addr ) >> 24 & 0xFF );
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "netif_ip_addr4 : %d.%d.%d.%d\r\n", ip_buff[0], ip_buff[1], ip_buff[2],
							 ip_buff[3] );

			// Update dcid data with allocated IP
			Eth_if::UpdateActiveIPConfig();

			dhcp_ip_get = true;
		}
	}
}

/**
 * @brief        -  In this function, the hardware should be initialized.
 * @details      -  Called from ethernetif_init_ESP32().
 * @param  netif -  the already initialized lwip network interface structure
 *                  for this ethernetif
 */
static err_t Low_Level_Init( struct netif* netif )
{
	ETH_Tx_Access_Mutex = new OS_Mutex();
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	// coverity[no_escape]
	LWIP_DEBUGF( NETIF_DEBUG, ( "low_level_transmit: frame sent\n" ) );
	/* set MAC hardware address */
	emac_handle->Get_MAC_Address( netif->hwaddr );
	ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "MAC Address [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", netif->hwaddr[0],
					 netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5] );

	/* maximum transfer unit */
	netif->mtu = EMAC_Driver::ETH_MTU;
	/*LWIP_DEBUGF( NETIF_DEBUG, ( "low_level_init: MTU set to %"U16_F "\n", netif->mtu ) );*/
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	emac_handle->Receive_Frame_Attach_FuncPtr( Ethernet_IF_Input_Func, netif );
	emac_handle->Init_EMAC_Hardware();
	Eth_if::Reset_PHY();
	ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "low_level_init: Ethernet link is up\n" );

	return ( ERR_OK );

}

/**
 * @brief        -  This function should do the actual transmission of the packet. The packet is
 *                  contained in the pbuf that is passed to the function. This pbuf
 *                  might be chained.
 * @param  netif -  the already initialized lwip network interface structure
 *                  for this ethernetif
 * @param p      -  the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return err_t -  ERR_OK if the packet could be sent.
 *                  an err_t value if the packet couldn't be sent
 * @note         -  Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *                  strange results. You might consider waiting for space in the DMA queue
 *                  to become availale since the stack doesn't retry to send a packet
 *                  dropped because of memory failure (except for the TCP timers).
 */
static err_t Low_Level_Output( struct netif* netif, struct pbuf* p )
{
	err_t status = ERR_OK;

	/* Bump the reference count on the pbuf to prevent it from being
	   freed till we are done with it.
	 */
	pbuf_ref( p );

	ETH_Tx_Access_Mutex->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT );

	Tx_Buf = ( uint8_t* ) p->payload;
	emac_handle->Tx_Frame( p->len );

	ETH_Tx_Access_Mutex->Release();

	pbuf_free( p );
	return ( status );
}

/**
 * @brief        -  It calls the function low_level_init() to do the
 *                  actual setup of the hardware.Should be called at the beginning
 *                  of the program to set up the network interface.
 *                  This function should be passed as a parameter to netif_add().
 * @param  netif -  the already initialized lwip network interface structure
 *                  for this ethernetif
 * @return err_t -  ERR_OK if the loopif is initialized
 *                  ERR_MEM if private data couldn't be allocated
 *                  any other err_t on error
 */
err_t Ethernet_IF_Init_ESP32( struct netif* netif )
{
	LWIP_ASSERT( "netif != NULL", ( netif != NULL ) );

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif	/* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
#if LWIP_SNMP

	NETIF_INIT_SNMP( netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS );
#endif

	netif->name[0U] = IFNAME0;
	netif->name[1U] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = Low_Level_Output;
	netif->l2_buffer_free_notify = Ethernet_Free_Rx_Buf_L2;

	/* initialize the hardware */
	return ( Low_Level_Init( netif ) );
}
