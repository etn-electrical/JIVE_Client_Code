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

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define NETIF_INTERFACE_TASK_STACK_SIZE				( 350U )
#define NETIF_INTERFACE_TASK_PRIORITY				OS_TASK_PRIORITY_5
#define LINK_SPEED_OF_YOUR_NETIF_IN_BPS (100U)

enum init_status_t
{
	SUCCEED,
	FAILURE
};

void ethernetif_emac( EMAC_Driver* emac );

static void ethernetif_input( void* pParams );

init_status_t EthernetIf_Exchange_Init( void );
OS_Mutex* ETHTxAccessMutex = reinterpret_cast<OS_Mutex*>( NULL );

EMAC_Driver* emac_handle;
/* Ethernet transmit Buffer */
uint8_t Tx_Buff[EMAC_Driver::ETH_TX_BUFNB][EMAC_Driver::ETH_MAX_PACKET_SIZE];
/* Received data pbuf pointer */
struct pbuf* rx_buffptr[EMAC_Driver::ETH_RX_BUFNB];
const uint16_t lwip_buffer_size = ( ( LWIP_MEM_ALIGN_SIZE( PBUF_POOL_BUFSIZE ) )
	- ( LWIP_MEM_ALIGN_SIZE( 0U ) ) );

/**
 * @brief      -   Initialization for transmit and receive purpose.
 *                 It allocates memeory buffers for receive purpose.
 *                 The addresses of those buffers are passed to
 *                 EMAC driver for receiving data at those locations.
 *                 The start address from which data shall be transmitted
 *                 is also passed to EMAC driver
 */
init_status_t EthernetIf_Exchange_Init( void )
{
	uint32_t* rx_payload[EMAC_Driver::ETH_RX_BUFNB];
	init_status_t return_value = SUCCEED;

	for ( uint8_t i = 0; ( i < EMAC_Driver::ETH_RX_BUFNB ) && ( return_value == SUCCEED ); i++ )
	{
		rx_buffptr[i] = pbuf_alloc( PBUF_RAW,
			( ( LWIP_MEM_ALIGN_SIZE( PBUF_POOL_BUFSIZE ) ) - ( LWIP_MEM_ALIGN_SIZE( 0U ) ) ),
			PBUF_POOL );
		if ( NULL == rx_buffptr[i] )
		{
			return_value = FAILURE;
		}
		else
		{
			rx_payload[i] = reinterpret_cast<uint32_t*>( rx_buffptr[i]->payload );
		}
	}
	if ( return_value == SUCCEED )
	{
		emac_handle->Init_Buff_Desc( &Tx_Buff[0U][0U], rx_payload, lwip_buffer_size );
	}

	return ( return_value );
}

/**
 * @brief       -  Receives the EMAc driver handle
 * @param emac  -  EMAC handle
 */
void ethernetif_emac( EMAC_Driver* emac )
{
	emac_handle = emac;
}

/**
 * @brief        -  In this function, the hardware should be initialized.
 * @details      -  Called from ethernetif_init().
 * @param  netif -  the already initialized lwip network interface structure
 *                  for this ethernetif
 */
static err_t low_level_init( struct netif* netif )
{
	err_t return_value;
	init_status_t eth_init_status;
	ETHTxAccessMutex = new OS_Mutex();
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	LWIP_DEBUGF( NETIF_DEBUG, ( "low_level_transmit: frame sent\n" ) );
	/* set MAC hardware address */
	emac_handle->Get_MAC_Address( netif->hwaddr );

	/* maximum transfer unit */
	netif->mtu = EMAC_Driver::ETH_MTU;
	LWIP_DEBUGF( NETIF_DEBUG, ("low_level_init: MTU set to %"U16_F"\n",netif->mtu) );
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	/* Initialize Tx and Rx Descriptors list: Chain Mode  */
	eth_init_status = EthernetIf_Exchange_Init();

	/* Create the task that handles the incoming packets */
	if ( OS_Tasker::Create_Task( ethernetif_input, NETIF_INTERFACE_TASK_STACK_SIZE,
								 NETIF_INTERFACE_TASK_PRIORITY, "ETH_INPUT", ( void* )netif ) )
	{
		if ( eth_init_status == SUCCEED )
		{
			Eth_if::Reset_PHY();
			emac_handle->Init_EMAC_Hardware();

			LWIP_DEBUGF( NETIF_DEBUG, ( "low_level_init: Ethernet link is up\n" ) );

			return_value = ERR_OK;
		}
		else
		{
			return_value = ERR_IF;
		}

	}
	else
	{
		LWIP_DEBUGF( NETIF_DEBUG, ( "low_level_init: create receive task error\n" ) );
		return_value = ERR_IF;
	}

	return ( return_value );
}

/**
 * @brief        -  This function should be called when a packet is ready to be read
 *                  from the interface. It uses the function low_level_input() that
 *                  should handle the actual reception of bytes from the network
 *                  interface. Then the type of the received packet is determined and
 *                  the appropriate input function is called.
 * @param  netif -  the lwip network interface structure for this ethernetif
 */
static void ethernetif_input( void * pParams )
{
	struct netif* netif = ( struct netif* )pParams;
	struct pbuf* p = NULL;
	struct pbuf* new_p = NULL;
	struct pbuf* p_temp = NULL;
	data_states_t state = INIT;
	static uint8_t packet_index = 0U;

	while ( 1 )
	{
		new_p = NULL;
		new_p = pbuf_alloc( PBUF_RAW,
							( ( LWIP_MEM_ALIGN_SIZE( PBUF_POOL_BUFSIZE ) ) - ( LWIP_MEM_ALIGN_SIZE( 0U ) ) ),
							PBUF_POOL );
		void* payload = ( new_p != NULL ? new_p->payload : NULL );
		state = emac_handle->Get_Next_Rx_Pkt( ( reinterpret_cast<uint32_t*>( payload ) ),
			&packet_index );

		if ( ( ERROR_STATE > state ) && ( state > INIT ) )
		{
			if ( packet_index < EMAC_Driver::ETH_RX_BUFNB )
			{
				p_temp = rx_buffptr[packet_index];
			}
			else
			{
				state = ERROR_STATE;
			}
		}

		if ( ( START == state ) || ( SINGLE_PKT_FRM == state ) )
		{
			p = p_temp;
		}

		if ( ( INTERIM == state ) || ( LAST == state ) )
		{
			pbuf_cat( p, p_temp );
		}

		if ( ( SINGLE_PKT_FRM == state ) || ( LAST == state ) )
		{
			LWIP_DEBUGF( NETIF_DEBUG, ( "ethernetif_input: frame received\n" ) );

			if ( ERR_OK != netif->input( p, netif ) )
			{
				LWIP_DEBUGF( NETIF_DEBUG, ( "ethernetif_input: input error\n" ) );
				pbuf_free( p );
				/* Need to sleep some time as TCP can't accepts more packets and
				 * Watchdog can be refreshed to prevent Device reset
				 */
				OS_Tasker::Delay( ETHERNETIF_SLEEP_TIME_MS );		/* Added to fix LTK-2915  */
			}
			p = NULL;
		}
		if ( ( state >= ERROR_STATE ) || ( state <= INIT ) )
		{
			pbuf_free( p );
			p = NULL;
			pbuf_free( new_p );
			new_p = NULL;
			LWIP_ASSERT( "pbuf NULL error", p != _NULL );
		}
		if ( new_p != NULL )
		{
			rx_buffptr[packet_index] = new_p;
		}
	}
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
static err_t low_level_output( struct netif* netif, struct pbuf* p )
{
	err_t status = ERR_OK;
	struct pbuf* q;
	uint8_t* buffer = NULL;
	uint16_t l = 0;

	/* Bump the reference count on the pbuf to prevent it from being
	 freed till we are done with it.
	 */
	pbuf_ref( p );

	ETHTxAccessMutex->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT );
	emac_handle->Wait_Tx_Ready();

	buffer = (uint8_t*)emac_handle->Get_Next_Avail_Desc_Buff();

	if ( buffer !=  NULL )
	{
		for ( q = p; q != NULL; q = q->next )
		{
			/* Send the data from the pbuf to the interface, one pbuf at a
		 	time. The size of the data in each pbuf is kept in the ->len
		 	variable. */

			memcpy( buffer, (u8_t*)q->payload, q->len );
			l += q->len;
			buffer += q->len;
		}

		emac_handle->Tx_Frame( l );
	}

	ETHTxAccessMutex->Release();

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
err_t ethernetif_init( struct netif* netif )
{
	LWIP_ASSERT( "netif != NULL", ( netif != NULL ) );

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

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
	netif->linkoutput = low_level_output;

	/* initialize the hardware */
	return ( low_level_init( netif ) );
}
