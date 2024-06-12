// *****************************************************************************
//
// Include standard library declarations.
//
// *****************************************************************************

#include <assert.h>
#include <string.h>

#include "Includes.h"
#include "OS_Tasker.h"

// *****************************************************************************
//
// lwIP API Header Files
//
// *****************************************************************************
#include "lwip/init.h"
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/mem.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwipopts.h"
#include "lwip/apps/sntp.h"

#ifndef ESP32_SETUP
#include "lwip/acd.h"
#endif
#include "LWIPService.h"
#include "Eth_if.h"
#include "EMAC_Driver.h"
#include "Ethernet_Debug.h"

#define LWIP_SERVICE_DELAY_IN_MS 500

int32_t lwip_errno;
#ifdef ESP32_SETUP
extern err_t Ethernet_IF_Init_ESP32( struct netif* netif );

#else
extern err_t ethernetif_init( struct netif* netif );

#endif
#ifdef __cplusplus
extern "C" {
#endif
#ifndef ESP32_SETUP
void sntp_init( SNTP_If* arg );

#else
void sntp_init_restart( SNTP_If* arg );

#endif

// *****************************************************************************
//
// The lwIP network interface structure for the Stellaris Ethernet MAC.
//
// *****************************************************************************
/*static */
struct netif lwip_netif;

static IP_CONFIG saved_static_ip_config;
static SNTP_If* sntp_if_handle = nullptr;	///< Used for Sntp_If object

#ifdef __cplusplus
}
#endif

// *****************************************************************************
//
//! Initializes the lwIP TCP/IP stack.
//! Call it from your main to initialize the lwip
//!
//! \param pucMAC is a pointer to a six byte array containing the MAC
//! address to be used for the interface.
//! \param ulIPAddr is the IP address to be used (static).
//! \param ulNetMask is the network mask to be used (static).
//! \param ulGWAddr is the Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function performs initialization of the lwIP TCP/IP stack for the
//! Stellaris Ethernet MAC, including DHCP and/or AutoIP, as configured.
//!
//! \return None.
//
// *****************************************************************************
void LWIPServiceTaskInit( void* pvParameters )
{
	struct ip4_addr ip_addr;
	struct ip4_addr net_mask;
	struct ip4_addr gw_addr;

	bool either_link_is_up = false;

	LWIP_ASSERT( "pvParameters != NULL", ( pvParameters != NULL ) );

	if ( pvParameters != NULL )
	{
		IP_CONFIG* ipCfg = ( IP_CONFIG* )pvParameters;

		// Check the parameters.
#if LWIP_DHCP && LWIP_AUTOIP
		assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) ||
				( ipCfg->IPMode == IPADDR_USE_DHCP ) ||
				( ipCfg->IPMode == IPADDR_USE_AUTOIP ) );
#elif LWIP_DHCP
		assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) || ( ipCfg->IPMode == IPADDR_USE_DHCP ) );
#elif LWIP_AUTOIP
		assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) ||
				( ipCfg->IPMode == IPADDR_USE_AUTOIP ) );
#else
		assert( ipCfg->IPMode == IPADDR_USE_STATIC );
#endif

#ifndef ESP32_SETUP
		// Start the TCP/IP thread & init stuff
		tcpip_init( LWIP_Service_TCPIP_Thread_CallBack, NULL );		///< LWIP_Service_TCPIP_Thread_CallBack()will be
		///< called in TCP_IP Task
#else
		tcpip_init( NULL, NULL );
#endif
		OS_Tasker::Delay( 100 / OS_TICK_RATE_MS );

		// Setup the network address values.
		if ( ipCfg->IPMode == IPADDR_USE_STATIC )
		{
			memcpy( &saved_static_ip_config, ipCfg, sizeof( IP_CONFIG ) );
#ifndef ESP32_SETUP
			if ( Is_ACD_Enabled() )
			{
				ip_addr.addr = 0;
				net_mask.addr = 0;
				gw_addr.addr = 0;
			}
			else
#endif
			{
				ip_addr.addr = htonl( ipCfg->IPAddr );
				net_mask.addr = htonl( ipCfg->NetMask );
				gw_addr.addr = htonl( ipCfg->GWAddr );
			}
		}
#if LWIP_DHCP || LWIP_AUTOIP
		else
		{
			ip_addr.addr = 0;
			net_mask.addr = 0;
			gw_addr.addr = 0;
		}
#endif

		// Create, configure and add the Ethernet controller interface with
		// default settings.
		// WARNING: This must only be run after the OS has been started.
		// Typically this is the case, however, if not, you must place this
		// in a post-OS initialization
		// @SEE http://lwip.wikia.com/wiki/Initialization_using_tcpip.c
#ifdef ESP32_SETUP
		netif_add( &lwip_netif, &ip_addr, &net_mask, &gw_addr, NULL, Ethernet_IF_Init_ESP32, tcpip_input );
#else
		netif_add( &lwip_netif, &ip_addr, &net_mask, &gw_addr, NULL, ethernetif_init, tcpip_input );
#endif
		netif_set_default( &lwip_netif );

		// TY - further reduced this delay from 3000 (ejo) to 500
		OS_Tasker::Delay( LWIP_SERVICE_DELAY_IN_MS / OS_TICK_RATE_MS );

		// Start DHCP, if enabled.
#if LWIP_DHCP
		if ( ipCfg->IPMode == IPADDR_USE_DHCP )
		{
			// coverity[no_escape]
			LWIP_DEBUGF( DHCP_DEBUG, ( "----- Starting DHCP client -----\n" ) );
#ifndef ESP32_SETUP
			Config_ACD( IPADDR_USE_DHCP, &lwip_netif );
#endif
			netif_set_up( &lwip_netif );
			dhcp_start( &lwip_netif );
		}
#ifdef ESP32_SETUP
		else
		{
			netif_set_up( &lwip_netif );
		}
#endif
#endif

		// Start AutoIP, if enabled and DHCP is not.
#if LWIP_AUTOIP
		if ( ipCfg->IPMode == IPADDR_USE_AUTOIP )
		{
			autoip_start( &lwip_netif );
		}
#endif

#ifndef ESP32_SETUP
		if ( ipCfg->IPMode == IPADDR_USE_STATIC )
		{
			Config_ACD( IPADDR_USE_STATIC, &lwip_netif );
			if ( !Is_ACD_Enabled() )
			{
				LWIPServiceBringStaticNetifUp( &lwip_netif );
			}
		}

#endif
		either_link_is_up = Eth_if::InitialLinkCheck();
		if ( either_link_is_up )
		{
			netif_set_link_up( &lwip_netif );
		}
		else
		{
			netif_set_link_down( &lwip_netif );
		}
		// OS_Tasker::Delay( 100 );

		/*
		   while ( 0 == netif_is_up( &lwip_netif ) )
		   {
		   OS_Tasker::Delay( 5000 / OS_TICK_RATE_MS );
		   if(0 == netif_is_up(&lwip_netif))
		   {
		   dhcp_renew(&lwip_netif);
		   }
		   }
		 */
	}
}

// *****************************************************************************
//
//! Returns the IP configuration for this interface.
//!
//! This function will read and return the currently assigned IP configuration for
//! the Stellaris Ethernet interface.
//!
//! \return ERR_OK if the interface is initialized
//! ERR_IFF if the interface is not initialized
//
// *****************************************************************************
void LWIPServiceGetIPDef( void* pvParameters )
{
	IP_CONFIG* ipCfg;

	ipCfg = ( IP_CONFIG* )pvParameters;

	memcpy( ( uint8_t* )&ipCfg->IPAddr, ( uint8_t* )&lwip_netif.ip_addr,
			sizeof ( ipCfg->IPAddr ) );
	memcpy( ( uint8_t* )&ipCfg->NetMask, ( uint8_t* )&lwip_netif.netmask,
			sizeof ( ipCfg->NetMask ) );
	memcpy( ( uint8_t* )&ipCfg->GWAddr, ( uint8_t* )&lwip_netif.gw,
			sizeof ( ipCfg->GWAddr ) );

	ipCfg->IPAddr = ntohl( ipCfg->IPAddr );
	ipCfg->NetMask = ntohl( ipCfg->NetMask );
	ipCfg->GWAddr = ntohl( ipCfg->GWAddr );
}

// *****************************************************************************
//
//! Returns the IP configuration for this interface.
//!
//! This function will read and return the currently assigned IP configuration for
//! the Stellaris Ethernet interface.
//!
//! \return ERR_OK if the interface is initialized
//! ERR_IFF if the interface is not initialized
//
// *****************************************************************************
err_t LWIPServiceTaskIPConfigGet( struct netif* netif, IP_CONFIG* ipCfg )
{
	LWIP_ASSERT( "netif != NULL", ( netif != NULL ) );

	if ( ( netif == NULL ) || ( !( netif_is_up( netif ) ) ) )
	{
		return ( ERR_IF );
	}

#ifndef ESP32_SETUP
	ipCfg->IPAddr = ( unsigned long )netif->ip_addr.addr;
	ipCfg->NetMask = ( unsigned long )netif->netmask.addr;
	ipCfg->GWAddr = ( unsigned long )netif->gw.addr;
#else
	ipCfg->IPAddr = ( unsigned long )netif->ip_addr.u_addr.ip4.addr;
	ipCfg->NetMask = ( unsigned long )netif->netmask.u_addr.ip4.addr;
	ipCfg->GWAddr = ( unsigned long )netif->gw.u_addr.ip4.addr;
#endif

	if ( netif->flags & NETIF_FLAG_ETHARP )
	{
		ipCfg->IPMode = IPADDR_USE_DHCP;
	}
	else
	{
		ipCfg->IPMode = IPADDR_USE_STATIC;
	}
	return ( ERR_OK );

}

// *****************************************************************************
//
//! Returns the local MAC/HW address for this interface.
//!
//! \param pucMAC is a pointer to an array of bytes used to store the MAC
//! address.
//!
//! This function will read the currently assigned MAC address into the array
//! passed in \e pucMAC.
//!
//! \return ERR_OK if the interface is initialized
//! ERR_IFF if the interface is not initialized
//
// *****************************************************************************
err_t LWIPServiceTaskMACGet( struct netif* netif, unsigned char* pucMAC )
{
	LWIP_ASSERT( "netif != NULL", ( netif != NULL ) );

	if ( netif == NULL )
	{
		return ( ERR_IF );
	}

	memcpy( pucMAC, &( netif->hwaddr[0] ), EMAC_Driver::ETH_MAC_HW_ADDRESS_LEN );
	return ( ERR_OK );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LWIPServiceBringStaticNetifDown( struct netif* netif )
{
	/* remove IP address from interface */
	netif_set_ipaddr( netif, ( const ip4_addr_t* )IP_ADDR_ANY );
	netif_set_gw( netif, ( const ip4_addr_t* )IP_ADDR_ANY );
	netif_set_netmask( netif, ( const ip4_addr_t* )IP_ADDR_ANY );
	Eth_if::UpdateActiveIPConfig();

	/* bring the interface down */
	netif_set_down( netif );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LWIPServiceBringStaticNetifUp( struct netif* netif )
{
	struct ip4_addr ip_addr;
	struct ip4_addr net_mask;
	struct ip4_addr gw_addr;

	ip_addr.addr = htonl( saved_static_ip_config.IPAddr );
	net_mask.addr = htonl( saved_static_ip_config.NetMask );
	gw_addr.addr = htonl( saved_static_ip_config.GWAddr );

	netif_set_ipaddr( netif, &ip_addr );
	netif_set_netmask( netif, &net_mask );
	netif_set_gw( netif, &gw_addr );
	Eth_if::UpdateActiveIPConfig();

	/* bring the interface up */
	netif_set_up( netif );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LWIPServiceStaticLinkUp( struct netif* netif )
{
#ifndef ESP32_SETUP
	// coverity[var_deref_op:FALSE]
	struct ip4_addr ip_addr_to_probe;

	ip_addr_to_probe.addr = htonl( saved_static_ip_config.IPAddr );

	if ( ACD_State() == ACD_STATE_RETREATED )
	{
		/* Handle last ACD retreat state on the new link up and reset ACD */
		ACD_Restart( &ip_addr_to_probe );
	}
	if ( Is_ACD_Enabled() )
	{
		LWIPServiceBringStaticNetifDown( netif );
		ACD_Start_Probing( &ip_addr_to_probe );
	}
	else
	{
		// Bring the interface up. - not really needed since we never took it down
		// LWIPServiceBringStaticNetifUp(netif);
	}
#endif
}

#if 0
// *****************************************************************************
//
//! Change the configuration of the lwIP network interface.
//!
//! \param ulIPAddr is the new IP address to be used (static).
//! \param ulNetMask is the new network mask to be used (static).
//! \param ulGWAddr is the new Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC 0 will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function will evaluate the new configuration data.  If necessary, the
//! interface will be brought down, reconfigured, and then brought back up
//! with the new configuration.
//!
//! \return None.
//
// *****************************************************************************
void lwIPNetworkConfigChange( struct netif* netif, IP_CONFIG* ipCfg )
{
	struct ip_addr ip_addr;
	struct ip_addr net_mask;
	struct ip_addr gw_addr;

	IP_CONFIG currentIPConfig;

	// Check the parameters.
#if LWIP_DHCP && LWIP_AUTOIP
	assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) ||
			( ipCfg->IPMode == IPADDR_USE_DHCP ) ||
			( ipCfg->IPMode == IPADDR_USE_AUTOIP ) );
#elif LWIP_DHCP
	assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) ||
			( ipCfg->IPMode == IPADDR_USE_DHCP ) );
#elif LWIP_AUTOIP
	assert( ( ipCfg->IPMode == IPADDR_USE_STATIC ) ||
			( ipCfg->IPMode == IPADDR_USE_AUTOIP ) );
#else
	assert( ipCfg->IPMode == IPADDR_USE_STATIC );
#endif

	// Setup the network address values.

	if ( ipCfg->IPMode == IPADDR_USE_STATIC )
	{
		ip_addr.addr = htonl( ipCfg->IPAddr );
		net_mask.addr = htonl( ipCfg->NetMask );
		gw_addr.addr = htonl( ipCfg->GWAddr );
	}
#if LWIP_DHCP || LWIP_AUTOIP
	else
	{
		ip_addr.addr = 0;
		net_mask.addr = 0;
		gw_addr.addr = 0;
	}
#endif

	// Switch on the current IP Address Aquisition mode.
	LWIPServiceTaskIPConfigGet( netif, &currentIPConfig );

	switch ( currentIPConfig.IPMode )
	{
		// Static IP

		case IPADDR_USE_STATIC:
		{
			// Set the new address parameters.  This will change the address
			// configuration in lwIP, and if necessary, will reset any links
			// that are active.  This is valid for all three modes.
			//
			netif_set_addr( netif, &ip_addr, &net_mask, &gw_addr );
			Eth_if::UpdateActiveIPConfig();

			// If we are going to DHCP mode, then start the DHCP server now.
#if LWIP_DHCP
			if ( ipCfg->IPMode == IPADDR_USE_DHCP )
			{
				dhcp_start( netif );
			}
#endif
			// If we are going to AutoIP mode, then start the AutoIP process
			// now.
#if LWIP_AUTOIP
			if ( ipCfg->IPMode == IPADDR_USE_AUTOIP )
			{
				autoip_start( netif );
			}
#endif
			// And we're done.
			break;
		}

			// DHCP (with AutoIP fallback).
#if LWIP_DHCP
		case IPADDR_USE_DHCP:
		{
			//
			// If we are going to static IP addressing, then disable DHCP and
			// force the new static IP address.
			//
			if ( ipCfg->IPMode == IPADDR_USE_STATIC )
			{
				dhcp_stop( netif );
				// SEE bug http://savannah.nongnu.org/bugs/?22804
				netif->flags &= ~NETIF_FLAG_DHCP;
				netif_set_addr( netif, &ip_addr, &net_mask, &gw_addr );
				Eth_if::UpdateActiveIPConfig();
			}
			// If we are going to AUTO IP addressing, then disable DHCP, set
			// the default addresses, and start AutoIP.
#if LWIP_AUTOIP
			else if ( ipCfg->IPMode == IPADDR_USE_AUTOIP )
			{
				dhcp_stop( netif );
				netif_set_addr( netif, &ip_addr, &net_mask, &gw_addr );
				Eth_if::UpdateActiveIPConfig();
				autoip_start( netif );
			}
#endif
			break;
		}

#endif
			// AUTOIP
#if LWIP_AUTOIP
		case IPADDR_USE_AUTOIP:
		{
			//
			// If we are going to static IP addressing, then disable AutoIP and
			// force the new static IP address.
			//
			if ( ulIPMode == IPADDR_USE_STATIC )
			{
				autoip_stop( netif );
				netif_set_addr( netif, &ip_addr, &net_mask, &gw_addr );
				Eth_if::UpdateActiveIPConfig();
			}

			//
			// If we are going to DHCP addressing, then disable AutoIP, set the
			// default addresses, and start dhcp.
			//
#if LWIP_DHCP
			else if ( ulIPMode == IPADDR_USE_AUTOIP )
			{
				autoip_stop( netif );
				netif_set_addr( netif, &ip_addr, &net_mask, &gw_addr );
				Eth_if::UpdateActiveIPConfig();
				dhcp_start( netif );
			}
#endif
			break;
		}
#endif
	}
}

#endif

// *****************************************************************************
//
// This call back will get passed as argument to TCP_IP thread to execute
// functionality which is dependant on TCP_IP thread
//
// *****************************************************************************
void LWIP_Service_TCPIP_Thread_CallBack( void* arg )
{
#if LWIP_SNTP
	if ( sntp_if_handle != nullptr )
	{
#ifndef ESP32_SETUP
		sntp_init( sntp_if_handle );
#else
		sntp_init_restart( sntp_if_handle );
#endif
	}
#endif
}

// *****************************************************************************
//
// Application space will pass handle of SNTP_If object through this interface
//
// *****************************************************************************
void Sntp_If_Init( SNTP_If* handle )
{
	sntp_if_handle = handle;
}

// *****************************************************************************
//
// Wrapper function for updating active ip. This function is used by lwip stack to update
// the ip config.
//
// *****************************************************************************
void Lwip_Update_Active_Ip_Config( void )
{
	Eth_if::UpdateActiveIPConfig();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t Get_LWIP_Errno( void )
{
	int32_t socket_err = lwip_errno;

	return ( socket_err );
}
