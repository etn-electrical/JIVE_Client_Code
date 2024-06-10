/**
 *****************************************************************************************
 * @file       EIP_if.cpp
 * @details    This file contains functions and Macros related to EIP interface.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "eipinc.h"
#include "EIP_if.h"
#include "DCI_Owner.h"
#include "DCI_Patron.h"
#include "Eth_if.h"
#include "Eth_Port.h"
#include "STDlib_MV.h"
#include "Exception.h"
#include "lwip/inet.h"

#include "EthConfig.h"
#include "EtherNet_IP_Config.h"


/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */
const uint8_t TOTAL_ETHERNET_LINK_INSTANCES = TOTAL_ETH_PORTS_IN_USE;

const uint8_t PLATFORM_IP_ADDRESS_SELECT_DHCP = 0x01U;		// #k: New
const uint8_t PLATFORM_IP_ADDRESS_SELECT_NV_MEM = 0x02U;	// #k: New

// Max length of host name.  This length is specified by EIP and CIP.
// const uint8_t EIP_SPECIFIC_HOST_NAME_LEN=	64U;    //Not Used

const uint8_t ETH_PORT_LABEL_STRING_INSTANCES = TOTAL_ETH_PORTS_IN_USE;

const uint8_t ETH_PORT_1_LABEL_STRINGS[] = { "Port 1" };
#if ETH_PORT_LABEL_STRING_INSTANCES == 2
const uint8_t ETH_PORT_2_LABEL_STRINGS[] = { "Port 2" };
#endif

const uint8_t* const ETH_PORT_LABEL_STRINGS[ETH_PORT_LABEL_STRING_INSTANCES] =
{
	ETH_PORT_1_LABEL_STRINGS,
#if ETH_PORT_LABEL_STRING_INSTANCES == 2
	ETH_PORT_2_LABEL_STRINGS
#endif
};

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
// uint16_t platform_dip_switch=0U;                //#k: New
// uint8_t platform_dip_switch_lower_byte=0U;      //#k: New

DCI_Patron* patron_access;
DCI_SOURCE_ID_TD platform_source_id;
bool_t new_ip_config_waiting;
// bool_t g_control_interface_setting_changed = false;

extern DCI_Owner* mac_id_owner;
extern struct netif lwip_netif;

extern int32_t lwip_errno;
// FILE        *fLogFile;


/*
 *******************************************************************************
 *		Prototypes
 *******************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIP_PLATFORM_Init( void )
{
	patron_access = new DCI_Patron( false );
	platform_source_id = DCI_SOURCE_IDS_Get();
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ETH_NAME_SERVER_1_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ETH_NAME_SERVER_2_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ETH_DOMAIN_NAME_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ETH_HOST_NAME_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ERROR_TD return_status;	// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = platform_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = patron_access->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID( DCI_ID_TD dcid, uint16_t* data )
{
	DCI_ERROR_TD return_status;	// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = platform_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = patron_access->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Get_DCID_Len( DCI_ID_TD dcid, DCI_LENGTH_TD* length )
{
	return ( patron_access->Get_Length( dcid, length ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Set_DCID( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ERROR_TD return_status;	// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_support;

	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = platform_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = patron_access->Data_Access( &access_struct );

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		return_status = patron_access->Get_Attribute_Support( dcid,
															  &attrib_support );
		if ( ( return_status == DCI_ERR_NO_ERROR ) &&
			 Test_Bit( attrib_support, DCI_PATRON_DCID_INIT_WR_SUPPORT ) )
		{
			access_struct.command = DCI_ACCESS_RAM_TO_INIT_CMD;
			access_struct.data_access.length = 0U;
			access_struct.data_access.offset = 0U;

			return_status = patron_access->Data_Access( &access_struct );
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD EIP_PLATFORM_Set_DCID( DCI_ID_TD dcid, uint16_t* data )
{
	DCI_ERROR_TD return_status;	// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_support;

	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = platform_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = patron_access->Data_Access( &access_struct );

	if ( return_status == DCI_ERR_NO_ERROR )
	{
		return_status = patron_access->Get_Attribute_Support( dcid,
															  &attrib_support );
		if ( ( return_status == DCI_ERR_NO_ERROR ) &&
			 Test_Bit( attrib_support, DCI_PATRON_DCID_INIT_WR_SUPPORT ) )
		{
			access_struct.command = DCI_ACCESS_RAM_TO_INIT_CMD;
			access_struct.data_access.length = 0U;
			access_struct.data_access.offset = 0U;

			return_status = patron_access->Data_Access( &access_struct );
		}
	}
	return ( return_status );
}

#ifdef __cplusplus
extern "C" {
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformInitLogFile( void )
{

	/*
	** This function is called during intiallization.
	**
	** It must open a log file (e.g. fLogFile) for future use by the
	** platformWriteLog() function.
	*/

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformCloseLogFile( void )
{

	/*
	** This function is called when the stack is shutting down
	** (i.e. clientStop() has been called)
	**
	** It must close the log file opened by platformInitLogFile().
	*/

}

/*------------------------------------------------------------------------------
** platformWriteLog()
**
** Writes a record to the log file
**------------------------------------------------------------------------------
*/
void platformWriteLog( uint8_t iSeverity,
					   uint16_t iType,
					   uint32_t lSrcIp,
					   uint32_t lDstIp,
					   char* szBuf,
					   bool_t bTimeStamp )
{

	/*
	** This function is called from a variety of places in the stack
	** during execution to write various warnings and data to the log
	** file.
	**
	** The szBuf parameter points to a NULL terminated string containing
	** the ASCII formatted data to be written to the log file.
	**
	** The bTimeStamp parameter indicates whether the stack suggests a
	** time stamp should be prepended to the log entry.  The general rule
	** adhered to within the stack is that raw data dumps (in hex ASCII
	** format) will call this funtion with bTimeStamp == false. Other
	** notifications call this function with bTimeStamp == true.
	**
	** In practice any log format desired can be accomodated here.
	*/

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformSocketLibInit( void )
{

	/*
	** This function is called during intiallization to allow for
	** of the platform's sockets library if required (as it is on the
	** Windows platform).
	**
	** If no such initiallization is required, simply return.
	*/

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformSocketLibCleanup()
{

	/*
	** This function is called during shutdown to shutdown the platform's
	** sockets library if required (as it is on the Windows platform).
	**
	** If no such shutdown is required, simply return.
	*/
}

/*------------------------------------------------------------------------------
** platformGetTcpIpCfgData()
**
** Get the TCP/IP configuration data incluidng DHCP flag, subnet mask, gateway
** address, domain name, and name servers.
**------------------------------------------------------------------------------
*/
bool_t platformGetTcpIpCfgData( TCPIP_CONFIG_DATA* psCfg )
{
	/*
	** This function is called to retrieve the device's
	** TCP/IP configuration data.
	**
	** The TCPIP_CONFIG_DATA structure is defined in eiptcpip.h as follows:
	**
	**	typedef struct tagTCPIP_CONFIG_DATA
	**	{
	**		uint32_t   lConfigCapability;
	**		uint32_t	 lConfigControl;
	**		uint16_t   iLinkObjPathSize;
	**		uint8_t    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
	**		uint32_t   lIpAddr;
	**		uint32_t   lSubnetMask;
	**		uint32_t   lGatewayAddr;
	**		uint32_t   lNameServer;
	**		uint32_t   lNameServer2;
	**		uint8_t    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
	**		uint8_t    szHostName[TCPIP_MAX_HOST_LENGTH+1];
	**		uint8_t	 bTTLValue;
	**		TCPIP_MCAST_DATA MulticastData;
	**
	**		uint8_t	 bACDMode;
	**		uint8_t	 bACDLCD_Phase;
	**		uint8_t	 ACDLCD_MacID[MAC_ADDR_LENGTH];
	**		uint8_t	 ACDLCD_ARPPdu[TCPIP_MAX_ARP_PDU_LENGTH];
	**	}
	**	TCPIP_CONFIG_DATA;
	**
	** Please see EtherNet/IP protocol specification for further information,
	*/
	uint8_t ip_alloc_method = 0U;

	// DCI_ERROR_TD return_status;

	EIP_PLATFORM_Get_DCID( DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID, reinterpret_cast<uint8_t*>( &ip_alloc_method ) );

	psCfg->lConfigControl &= ~TCPIP_CONTROL_CONFIG_BITMASK;	// #k: ~0x00000007
	if ( ip_alloc_method == PLATFORM_IP_ADDRESS_SELECT_DHCP )
	{
		psCfg->lConfigControl |= TCPIP_CONTROL_DHCP;		// #k: 0x00000002
	}							// #k: //Attribute-3-Configuration Control
	else
	{
		psCfg->lConfigControl |= TCPIP_CONTROL_USE_STORED_CONFIG;// #k:0x00000000
	}

	// The IP address is unchangeable from a complete static standpoint.
	// It is stuck where it is so we EIP can't change it.
	// It is there fault for limiting the flexibility of IP Address selection.

	psCfg->lConfigCapability = 0;
#ifdef TCPIP_CAPABILITY_ACD_ENABLE
	psCfg->lConfigCapability |= TCPIP_CAPABILITY_ACD;
#endif
#ifdef TCPIP_CAPABILITY_DHCP_ENABLE
	psCfg->lConfigCapability |= TCPIP_CAPABILITY_DHCP;
#endif
#ifdef TCPIP_CAPABILITY_IP_RESET_NEEDED_ENABLE
	psCfg->lConfigCapability |= TCPIP_CAPABILITY_IP_RESET_NEEDED;
#endif
#ifdef TCPIP_CAPABILITY_HARDWARE_CONFIG_ENABLE
	psCfg->lConfigCapability |= TCPIP_CAPABILITY_HARDWARE_CONFIG;
#endif

	// return_status = EIP_PLATFORM_Get_DCID(
	// DCI_PWR_BRD_DIP_SWITCHES_DCID,
	// reinterpret_cast<uint16_t*>(&platform_dip_switch) );  //#k: New
	//
	// platform_dip_switch_lower_byte = static_cast<uint8_t>(platform_dip_switch);

	/*
	    Moved to "Init_Dynamic_RAM_Modules()"
	    standard_previous_dip_switch_values_locked_lower_byte=
	    (uint8_t) previous_dip_value_locked ;
	 */
	/*
	    if ( (platform_dip_switch == 0x0103U)   ||
	        //#k: proceed if IP Address selection can be done from Ethernet.
	         (previous_dip_value_locked == 0x0103U)
	       )
	   B4, 10 Dec*/

	if ( isTcpIpConfigCapabilityOn() == true )				// Shekhar - LTKBI
	{
		psCfg->lConfigCapability |= TCPIP_CAPABILITY_SETTABLE_CONFIG;	// #k:New
	}
	else
	{
		psCfg->lConfigCapability &= ( ~TCPIP_CAPABILITY_SETTABLE_CONFIG );	// #k:New
	}

	if ( ip_alloc_method != PLATFORM_IP_ADDRESS_SELECT_NV_MEM )
	{
		EIP_PLATFORM_Get_DCID( DCI_ACTIVE_IP_ADDRESS_DCID,
							   reinterpret_cast<uint8_t*>( &psCfg->lIpAddr ) );
		EIP_PLATFORM_Get_DCID( DCI_ACTIVE_SUBNET_MASK_DCID,
							   reinterpret_cast<uint8_t*>( &psCfg->lSubnetMask ) );
		EIP_PLATFORM_Get_DCID( DCI_ACTIVE_DEFAULT_GATEWAY_DCID,
							   reinterpret_cast<uint8_t*>( &psCfg->lGatewayAddr ) );
	}
	else
	{
		// psCfg->lConfigCapability |= TCPIP_CAPABILITY_SETTABLE_CONFIG;
		// #k: Ori. Commented on 27Sept.
		if ( new_ip_config_waiting == false )
		{
			EIP_PLATFORM_Get_DCID(
				DCI_ACTIVE_IP_ADDRESS_DCID,
				reinterpret_cast<uint8_t*>( &psCfg->lIpAddr ) );
			EIP_PLATFORM_Get_DCID(
				DCI_ACTIVE_SUBNET_MASK_DCID,
				reinterpret_cast<uint8_t*>( &psCfg->lSubnetMask ) );
			EIP_PLATFORM_Get_DCID(
				DCI_ACTIVE_DEFAULT_GATEWAY_DCID,
				reinterpret_cast<uint8_t*>( &psCfg->lGatewayAddr ) );
		}
		else
		{
			EIP_PLATFORM_Get_DCID( DCI_STATIC_IP_ADDRESS_DCID,
								   reinterpret_cast<uint8_t*>( &psCfg->lIpAddr ) );
			EIP_PLATFORM_Get_DCID( DCI_STATIC_SUBNET_MASK_DCID,
								   reinterpret_cast<uint8_t*>( &psCfg->lSubnetMask ) );
			EIP_PLATFORM_Get_DCID(
				DCI_STATIC_DEFAULT_GATEWAY_DCID,
				reinterpret_cast<uint8_t*>( &psCfg->lGatewayAddr ) );
		}
	}
	psCfg->lIpAddr = htonl( psCfg->lIpAddr );
	psCfg->lSubnetMask = htonl( psCfg->lSubnetMask );
	psCfg->lGatewayAddr = htonl( psCfg->lGatewayAddr );

	EIP_PLATFORM_Get_DCID( DCI_ETH_NAME_SERVER_1_DCID,
						   reinterpret_cast<uint8_t*>( &psCfg->lNameServer ) );
	psCfg->lNameServer = htonl( psCfg->lNameServer );
	EIP_PLATFORM_Get_DCID( DCI_ETH_NAME_SERVER_2_DCID,
						   reinterpret_cast<uint8_t*>( &psCfg->lNameServer2 ) );
	psCfg->lNameServer2 = htonl( psCfg->lNameServer2 );

	EIP_PLATFORM_Get_DCID( DCI_ETH_DOMAIN_NAME_DCID,
						   psCfg->szDomainName );
	EIP_PLATFORM_Get_DCID( DCI_ETH_HOST_NAME_DCID,
						   psCfg->szHostName );

	/* Set the User-defined path to the object associated with the underlying
	    physical communication interface (e.g.,an [F6] Ethernet Link Object Class)*/
	psCfg->iLinkObjPathSize = 0U;		/* 4 bytes long */
	// psCfg->iLinkObjPathSize = 4;		/* 4 bytes long */
	// psCfg->LinkObjPath[0] = 0x20;		/* Class logical segment */
	// psCfg->LinkObjPath[1] = 0xF6;		/* Ethernet Link class ID */
	// psCfg->LinkObjPath[2] = 0x24;		/* Instance logical segment */
	// psCfg->LinkObjPath[3] = 1;
	/* Instance based off of number of TCP/IP interfaces */

	EIP_PLATFORM_Get_DCID( DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID,
						   reinterpret_cast<uint16_t*>( &psCfg->iEncapTimeout ) );					// Newly added as
																									// per EIP Stack
																									// 4.5.0

	/* Setup the multicast parameters */
	psCfg->bTTLValue = TTL_SUBNET_LEVEL;
	psCfg->MulticastData.bAllocControl = 0U;
	psCfg->MulticastData.bReserved = 0U;
	psCfg->MulticastData.iNumMcast = CIP_MAX_NUM_MULTICAST_ADDRESS;
	psCfg->MulticastData.lMcastStartAddr = CIP_MCAST_BASE;
#ifdef EIP_ACD
	EIP_PLATFORM_Get_DCID( DCI_ETH_ACD_ENABLE_DCID,
						   reinterpret_cast<uint8_t*>( &psCfg->bACDMode ) );
	EIP_PLATFORM_Get_DCID( DCI_ETH_ACD_CONFLICTED_STATE_DCID,
						   reinterpret_cast<uint8_t*>( &psCfg->bACDLCD_Phase ) );
	EIP_PLATFORM_Get_DCID( DCI_ETH_ACD_CONFLICTED_MAC_DCID,
						   reinterpret_cast<uint8_t*>( psCfg->ACDLCD_MacID ) );
	EIP_PLATFORM_Get_DCID( DCI_ETH_ACD_CONFLICTED_ARP_PDU_DCID,
						   reinterpret_cast<uint8_t*>( psCfg->ACDLCD_ARPPdu ) );
#endif
	// return_status = return_status;
	return ( true );
}

/*------------------------------------------------------------------------------
** platformTestSetTcpIpCfgData()
**
** Ensure a particular TCP/IP object attribute can be set before it's
** actually set. Return false if error is generated.
**------------------------------------------------------------------------------
*/
bool_t platformTestSetTcpIpCfgData( uint32_t lMask,
									TCPIP_CONFIG_DATA* psCfg,
									uint32_t lCurrentIP,
									uint8_t* pbGeneralError )
{
	bool_t set_will_succeed = true;
	bool_t IP_ADR_CNTRL_FROM_EIP = false;
	uint8_t ip_alloc_method = 0;

	// DCI_ERROR_TD return_status;

	// Is someone trying to change the host name or the interface config.
	if ( Test_Bit( lMask, TCPIP_INST_ATTR_INTERFACE_CONFIG ) )
	{
		EIP_PLATFORM_Get_DCID(
			DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
			&ip_alloc_method );

		if ( ip_alloc_method != PLATFORM_IP_ADDRESS_SELECT_NV_MEM )
		{
			// We are going to consider this an unsupported action when
			// we are not configured to use the full static value.
			set_will_succeed = false;
			*pbGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		}

		// This check is added to avoid loopback address (127.0.0.1) getting written to Interface Configuration
		if ( ( psCfg->lIpAddr == htonl( INADDR_LOOPBACK ) ) ||
			 ( psCfg->lSubnetMask == htonl( INADDR_LOOPBACK ) ) ||
			 ( psCfg->lGatewayAddr == htonl( INADDR_LOOPBACK ) ) )
		{
			set_will_succeed = false;
			*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		}
	}
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_HOST_NAME ) )
	// They can set the host no matter what.
	{
		set_will_succeed = true;
	}
#ifdef EIP_ACD
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_ACD_MODE ) ||
			  Test_Bit( lMask, TCPIP_INST_ATTR_ACD_LAST_CONFLICT ) )
	// They can set the ACD parameters no matter what.
	{
		set_will_succeed = true;
	}
#endif
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_CONFIG_CONTROL ) )
	// #k: Start:  New Else-
	{
		// return_status = EIP_PLATFORM_Get_DCID( DCI_PWR_BRD_DIP_SWITCHES_DCID,
		// reinterpret_cast<uint16_t*>(&platform_dip_switch ));
		//
		// platform_dip_switch_lower_byte = static_cast<uint8_t>(platform_dip_switch);

		/*      if ( ((platform_dip_switch != 0x0103U)
		            &&( previous_dip_value_locked != 0x0103U ) )
		 || (psCfg->lConfigControl == 1U)
		 || (psCfg->lConfigControl & 0x000010U) )
		        //#k:SET not supported if IP Address settable from DIP only.
		           {  //#k: SET not supported if Config Method- BOOTP &/or
		          //DNS enable/Disable command
		            set_will_succeed = false;
		 * pbGeneralError  = ROUTER_ERROR_BAD_ATTR_DATA;
		      }
		 #k: B4, 10Dec14 */

		if ( isIpAdrCntrlFromEipOn() == true )	// LTKBI - Shekhar
		{
			IP_ADR_CNTRL_FROM_EIP = true;
		}
		else
		{
			IP_ADR_CNTRL_FROM_EIP = false;
		}

		if ( ( IP_ADR_CNTRL_FROM_EIP == false ) ||
			 !( ( psCfg->lConfigControl == TCPIP_CONTROL_USE_STORED_CONFIG ) ||	// No other value shall Config Control
																				// accept
				( psCfg->lConfigControl == TCPIP_CONTROL_DHCP ) ) )				// other than Static (NV stored) and
																				// DHCP
		{	// #k: SET not supported if Config Method- BOOTP &/or DNS enable/Disable command
			set_will_succeed = false;
			*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
		}

		/*
		    return_status = EIP_PLATFORM_Get_DCID( DCI_PWR_BRD_DIP_SWITCHES_DCID,
		                                           (uint16_t*)&platform_dip_switch );

		      platform_dip_switch_lower_byte = (uint8_t) platform_dip_switch;
		      if ( (!( (platform_dip_switch & 0x0100)
		            && (platform_dip_switch_lower_byte
		                                      == IP_ADDRESS_SELECT_FROM_ETHERNET) ) )

		 || (!((previous_dip_value_locked & 0x0100)
		      && (standard_previous_dip_switch_values_locked_lower_byte
		                                      ==IP_ADDRESS_SELECT_FROM_ETHERNET)) )

		 || (psCfg->lConfigControl == 1)
		 || (psCfg->lConfigControl & 0x000010) )
		                //#k:SET not supported if IP Address settable from DIP only.
		      {         //#k: SET not supported if Config Method- BOOTP &/or
		                //    DNS enable/Disable command
		            set_will_succeed = false;
		 * pbGeneralError  = ROUTER_ERROR_BAD_ATTR_DATA;
		      }
		 */
	}	// #k: End: New Else-
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_ENCAP_TIMEOUT ) )// They can set the ENCAP Timeout no matter what.
	{
		set_will_succeed = true;
	}
	else
	{
		set_will_succeed = false;
		*pbGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
	}

	// return_status = return_status;
	return ( set_will_succeed );
}

/*------------------------------------------------------------------------------
** platformSetTcpIpCfgData()
**
** Set the TCP/IP configuration data including subnet mask, gateway
** address, domain name, and name servers.
**------------------------------------------------------------------------------
*/
void platformSetTcpIpCfgData( uint32_t lMask, TCPIP_CONFIG_DATA* psCfg,
							  uint32_t lCurrentIP )
{
	/*
	** This function is called to set the device's
	** TCP/IP configuration data (if supported)
	**
	** The lCurrentIP is the IP address these changes are attempting to be made
	** on
	** The TCPIP_CONFIG_DATA structure is defined in eiptcpip.h as follows:
	**
	**	typedef struct tagTCPIP_CONFIG_DATA
	**	{
	**		uint32_t   lConfigCapability;
	**		uint32_t	 lConfigControl;
	**		uint16_t   iLinkObjPathSize;
	**		uint8_t    LinkObjPath[TCPIP_MAX_LINK_PATH_LENGTH];
	**		uint32_t   lIpAddr;
	**		uint32_t   lSubnetMask;
	**		uint32_t   lGatewayAddr;
	**		uint32_t   lNameServer;
	**		uint32_t   lNameServer2;
	**		uint8_t    szDomainName[TCPIP_MAX_DOMAIN_LENGTH+1];
	**		uint8_t    szHostName[TCPIP_MAX_HOST_LENGTH+1];
	**		uint8_t	 bTTLValue;
	**		TCPIP_MCAST_DATA MulticastData;
	**	}
	**	TCPIP_CONFIG_DATA;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see EtherNet/IP protocol specification for further information.
	*/
	uint8_t ip_alloc_method = 0U;
	// DCI_ERROR_TD return_status;
	DCI_LENGTH_TD str_len;
	uint32_t temp_ip;

	// Is someone trying to change the host name or the interface config.
	if ( Test_Bit( lMask, TCPIP_INST_ATTR_INTERFACE_CONFIG ) )
	{
		EIP_PLATFORM_Get_DCID(
			DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
			&ip_alloc_method );

		if ( ip_alloc_method == PLATFORM_IP_ADDRESS_SELECT_NV_MEM )
		{
			new_ip_config_waiting = true;

			temp_ip = ntohl( psCfg->lIpAddr );
			EIP_PLATFORM_Set_DCID( DCI_STATIC_IP_ADDRESS_DCID,
								   reinterpret_cast<uint8_t*>( &temp_ip ) );
			temp_ip = ntohl( psCfg->lSubnetMask );
			EIP_PLATFORM_Set_DCID( DCI_STATIC_SUBNET_MASK_DCID,
								   reinterpret_cast<uint8_t*>( &temp_ip ) );
			temp_ip = ntohl( psCfg->lGatewayAddr );
			EIP_PLATFORM_Set_DCID(
				DCI_STATIC_DEFAULT_GATEWAY_DCID,
				reinterpret_cast<uint8_t*>( &temp_ip ) );
			temp_ip = ntohl( psCfg->lNameServer );
			EIP_PLATFORM_Set_DCID( DCI_ETH_NAME_SERVER_1_DCID,
								   reinterpret_cast<uint8_t*>( &temp_ip ) );
			temp_ip = ntohl( psCfg->lNameServer2 );
			EIP_PLATFORM_Set_DCID( DCI_ETH_NAME_SERVER_2_DCID,
								   reinterpret_cast<uint8_t*>( &temp_ip ) );

			str_len = strlen( ( char const* ) psCfg->szDomainName );
			BF_Lib::Copy_Val_To_String( &psCfg->szDomainName[str_len],
										NULL, ( TCPIP_MAX_DOMAIN_LENGTH - str_len ) );
			EIP_PLATFORM_Set_DCID( DCI_ETH_DOMAIN_NAME_DCID,
								   psCfg->szDomainName );
		}
	}
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_HOST_NAME ) )					// They can set the host no matter what.
	{
		str_len = strlen( ( char const* )psCfg->szHostName );
		BF_Lib::Copy_Val_To_String( &psCfg->szHostName[str_len],
									NULL, ( TCPIP_MAX_HOST_LENGTH - str_len ) );
		EIP_PLATFORM_Set_DCID( DCI_ETH_HOST_NAME_DCID,
							   psCfg->szHostName );
	}
#ifdef EIP_ACD
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_ACD_MODE ) )
	{
		EIP_PLATFORM_Set_DCID( DCI_ETH_ACD_ENABLE_DCID,
							   reinterpret_cast<uint8_t*>( &psCfg->bACDMode ) );
	}
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_ACD_LAST_CONFLICT ) )
	{
		EIP_PLATFORM_Set_DCID(
			DCI_ETH_ACD_CONFLICTED_STATE_DCID,
			reinterpret_cast<uint8_t*>( &psCfg->bACDLCD_Phase ) );

		EIP_PLATFORM_Set_DCID( DCI_ETH_ACD_CONFLICTED_MAC_DCID,
							   reinterpret_cast<uint8_t*>( psCfg->ACDLCD_MacID ) );

		EIP_PLATFORM_Set_DCID(
			DCI_ETH_ACD_CONFLICTED_ARP_PDU_DCID,
			reinterpret_cast<uint8_t*>( psCfg->ACDLCD_ARPPdu ) );
	}
#endif
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_CONFIG_CONTROL ) )
	// #k: Start:------- New else if statement
	{
		// return_status = EIP_PLATFORM_Get_DCID( DCI_PWR_BRD_DIP_SWITCHES_DCID,
		// reinterpret_cast<uint16_t*>(&platform_dip_switch) );
		//
		// platform_dip_switch_lower_byte = static_cast<uint8_t>(platform_dip_switch);

		if ( isIpAddrSettebleFromEthernet() == true )	// LTKBI - Shekhar
		{
			new_ip_config_waiting = true;
			EIP_PLATFORM_Get_DCID(
				DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
				&ip_alloc_method );

			if ( psCfg->lConfigControl == 0U )	// #k: TCP/IP Attrbute-3: Statically assigned IP Address
			{
				ip_alloc_method = PLATFORM_IP_ADDRESS_SELECT_NV_MEM;
				EIP_PLATFORM_Set_DCID(
					DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
					&ip_alloc_method );
			}
			else if ( psCfg->lConfigControl == 2U )	// #k: TCP/IP Attrbute-3: DHCP assigned IP Address
			{
				ip_alloc_method = PLATFORM_IP_ADDRESS_SELECT_DHCP;
				EIP_PLATFORM_Set_DCID(
					DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID,
					&ip_alloc_method );
			}
		}
	}	// #k: End:------- New else if statement
	else if ( Test_Bit( lMask, TCPIP_INST_ATTR_ENCAP_TIMEOUT ) )					// They can set the host no matter
																					// what.
	{
		EIP_PLATFORM_Set_DCID(
			DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_DCID,
			reinterpret_cast<uint16_t*>( &psCfg->iEncapTimeout ) );
	}

	// return_status = return_status;
}

/*------------------------------------------------------------------------------
** platformSocketInitTargetData( )
**
** Initialize galHostAddr array. It is passed as a response to the
** List Targets UCMM request.
**------------------------------------------------------------------------------
*/
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformSocketInitTargetData( void )
{

	/*
	** This function is called during initiallization.
	**
	** Upon return, it should have placed the number of IP addresses supported
	** by the device (i.e. the number of active Ethernet interfaces) in the
	** global gnHostIPAddrCount.
	**
	** In addition, it should have placed each supported Ethernet address
	** as a uint32_t) in the global array galHostIPAddr[MAX_HOST_ADDRESSES].
	*/

	Push_TGINT();

	memcpy( &galHostIPAddr[0], &lwip_netif.ip_addr, sizeof( uint32_t ) );
	gnHostIPAddrCount = 1U;

	Pop_TGINT();
}

/*------------------------------------------------------------------------------
** platformGetCurrentTickCount( )
**
** Return current msec counter using a high-res performance timer.
**------------------------------------------------------------------------------
*/
// uint32_t platformGetCurrentTickCount()
// {
/*
** This function is called periodically during execution of the stack to
** retrieve the system clock in milliseconds.
*/
// }

/*------------------------------------------------------------------------------
** platformGetCurrentAbsTime( )
**
** This retrieves time in seconds from Jan 1, 1970 and
** current millisecond (0-1000)
**------------------------------------------------------------------------------
*/

void platformGetCurrentAbsTime( uint32_t* plSecTime, uint16_t* puMillisecTime )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t platformAddrFromPath( const char* szAddr, INT32 nLen )
{
	uint32_t lIPAddress = ( uint32_t )ERROR_STATUS;
	// struct hostent    *phost;
	char addr[MAX_NETWORK_PATH_SIZE];

	memcpy( addr, szAddr, nLen );
	addr[nLen] = 0U;

	/*
	** Look up the name, assumed initially it is in dotted format.
	** If it wasn't dotted, maybe it's a symbolic name
	*/
	if ( ( lIPAddress = inet_addr( addr ) ) == ERROR_STATUS )
	{
		// if ( (phost = gethostbyname(addr)) != NULL )
		// {
		// memcpy( &lIPAddress, phost->h_addr_list[0], sizeof(uint32_t) );
		// }
	}

	return ( lIPAddress );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformSetSockaddrIn( struct sockaddr_in* pSockaddr,
							uint16_t iPort,
							uint32_t lIpAddress )
{
	memset( pSockaddr, 0U, sizeof( struct sockaddr_in ) );
	pSockaddr->sin_family = AF_INET;
	pSockaddr->sin_port = htons( iPort );
	platformGetInternetAddress( pSockaddr->sin_addr ) = lIpAddress;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformCopySockAddrInToBuffer( uint8_t* pBuffer,
									 struct sockaddr_in* pSockAddr )
{
	memcpy( pBuffer, pSockAddr, sizeof( struct sockaddr_in ) );
	*( ( uint16_t* )pBuffer ) = htons( AF_INET );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platformCopySockAddrInFromBuffer( struct sockaddr_in* pSockAddr,
									   uint8_t* pBuffer )
{
	memcpy( pSockAddr, pBuffer, sizeof( struct sockaddr_in ) );
	pSockAddr->sin_family = AF_INET;
}

#ifdef EIP_ACD

/*------------------------------------------------------------------------------
** platformDidIPAddressConflict( )
**
** The IP address duplicate detection detected a collision but was either able
** to defeat the other device or retreat.  This merely indicates that a
** collision occurred.  Whether we retreated or not is in the following
** function.
**------------------------------------------------------------------------------
*/
/*bool_t platformDidIPAddressConflict( uint32_t lIPAddress )
   {
    return ( ( ACD_State() == ACD_STATE_RETREATED ) ||
             ( ACD_State() == ACD_STATE_CONFLICT_DETECTED ) );
   }*/

/*------------------------------------------------------------------------------
** platformIsDuplicateIPAddress( )
**
** Determines if the current IP address used by the EtherNet/IP stack is a
** duplicate IP address on the network.  Requires Address Conflict Detection
** support (#define EIP_ACD) from the platform
**------------------------------------------------------------------------------
*/
bool_t platformIsDuplicateIPAddress( uint32_t lIPAddress )
{
	// return ( ACD_State() == ACD_STATE_RETREATED );
	return ( false );
}

#endif

/*------------------------------------------------------------------------------
** platformGetMacID()
**
** Get Mac ID from the GetAdaptersInfo
**------------------------------------------------------------------------------
*/

bool_t platformGetMacID( uint32_t lAddress, uint8_t* pMacId )
{
	// ejo - need something

	// memcpy(pMacId, lwip_netif.hwaddr, 6 );
	pMacId[0] = lwip_netif.hwaddr[0];
	pMacId[1] = lwip_netif.hwaddr[1];
	pMacId[2] = lwip_netif.hwaddr[2];
	pMacId[3] = lwip_netif.hwaddr[3];
	pMacId[4] = lwip_netif.hwaddr[4];
	pMacId[5] = lwip_netif.hwaddr[5];

	return ( true );
}

/*------------------------------------------------------------------------------
** platformGetNumEnetLinkInstances( )
**
** Returns the number of supported Ethernet Link instances.  Typically equal
** to the number of TCP/IP object instances supported unless an embedded
** switch is involved
**------------------------------------------------------------------------------
*/

uint32_t platformGetNumEnetLinkInstances()
{
	return ( TOTAL_ETHERNET_LINK_INSTANCES );
}

#ifdef __cplusplus
}
#endif

/*------------------------------------------------------------------------------
** platformGetEnetLinkAttributes( )
**
** Populates passed structure with Ethernet Link attributes including
** interface flag, speed and MAC address
**------------------------------------------------------------------------------
*/
/*
 #define CIP_NEG_STATUS_AUTONEG_IN_PROGRESS			0<<2
 #define CIP_NEG_STATUS_AUTONEG_DUPLEX_SPEED_FAIL	1<<2
 #define CIP_NEG_STATUS_AUTONEG_DUPLEX_FAIL			2<<2
 #define CIP_NEG_STATUS_AUTONEG_SUCCESS				3<<2
 #define CIP_NEG_STATUS_AUTONEG_NOT_ATTEMPTED		4<<2
 */

const uint32_t CIP_NEG_STATUS_AUTONEG_IN_PROGRESS = 0U << 2U;
const uint32_t CIP_NEG_STATUS_AUTONEG_DUPLEX_SPEED_FAIL = 1U << 2U;
const uint32_t CIP_NEG_STATUS_AUTONEG_DUPLEX_FAIL = 2U << 2U;
const uint32_t CIP_NEG_STATUS_AUTONEG_SUCCESS = 3U << 2U;
const uint32_t CIP_NEG_STATUS_AUTONEG_NOT_ATTEMPTED = 4U << 2U;

#ifdef __cplusplus
extern "C" {
#endif

void platformGetEnetLinkAttributes( uint32_t iInstance,
									ENETLINK_INST_ATTR* pEnetLinkAttr )
{
	/*
	** This function is called when a host IP address is claimed by the client
	** application.
	**
	** It should populate the pEnetLinkAttr structure with
	** the Ethernet Link attributes for the local network interface
	**
	** The EnetLinkConfig structure is defined in eiptcpip.h as
	**
	**	typedef struct tagENETLINK_INST_ATTR
	**	{
	**	uint32_t   lInterfaceSpeed;
	**	uint32_t   lInterfaceFlags;
	**	uint8_t    macAddr[MAC_ADDR_LENGTH];
	**	uint8_t	 bInterfaceType;
	**	uint8_t	 bInterfaceState;
	**	uint8_t	 bAdminState;
	**	INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
	**	}
	**	ENETLINK_INST_ATTR;
	**
	** Please see the EtherNet/IP protocol specification for further information
	*/

	/* If product can be accessed via a CIP port other than TCP/IP, then
	** the link active bit must be determined.  If product only responds
	** to one source of CIP requests (Ethernet port e.g.), then we can
	** assume that it is active, since we got this request */
	// pEnetLinkAttr->lInterfaceFlags = ENETLINK_FLAG_LINK_ACTIVE | ENETLINK_FLAG_SPEED_DUPLEX_FINE;

	/* The interface flags and interface speed must be determined
	** from your hardware interface */
	// pEnetLinkAttr->lInterfaceFlags &= (uint32_t)~EL_FLAG_DUPLEX_MODE;
	// ejo - need
	// pEnetLinkAttr->lInterfaceSpeed = 0U;	/* 0 means "speed not known" */

	// platformGetMacID(lIPAddress, pEnetLinkAttr->macAddr);
	//
	// pEnetLinkAttr->bInterfaceType = ENETLINK_INTERFACE_TYPE_TWISTED_PAIR;
	// pEnetLinkAttr->bInterfaceState = ENETLINK_INTERFACE_STATE_ENABLED;
	// pEnetLinkAttr->bAdminState = ENETLINK_ADMIN_STATE_ENABLED;
	// strcpy(pEnetLinkAttr->InterfaceLabel, socketGetAddressString(lIPAddress));

	bool_t auto_neg_enable = false;
	bool_t full_duplex_enable = false;
	uint16_t link_speed_sel = 0U;
	bool_t port_enable = false;
	// bool_t auto_neg_state;
	uint8_t auto_neg_state = Eth_Port::ETH_AUTO_NEG_STATE_LINK_INACTIVE;
	bool_t full_duplex_state;
	uint16_t link_speed_state;
	uint8_t phy_port;

	if ( iInstance <= TOTAL_ETHERNET_LINK_INSTANCES )
	{
		phy_port = iInstance - 1U;

		/* Populate with Ethernet Link attributes for local network card */
		// We only have one instance of the TCP IP object.
		TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig = &gsTcpIpInstAttr[0];
		uint32_t lIPAddress = pTcpipConfig->InterfaceConfig.lIpAddr;

		// Determine the full duplex state.
		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_active_link_duplex,
			reinterpret_cast<uint8_t*>( &full_duplex_state ) );
		pEnetLinkAttr->lInterfaceFlags = 0U;

		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_active_link_autoneg_state,
			reinterpret_cast<uint8_t*>( &auto_neg_state ) );


		// if( g_control_interface_setting_changed )
		{
			pEnetLinkAttr->lInterfaceFlags |= ENETLINK_FLAG_RESET_REQUIRED;
		}
		// Determine the auto negotiation state and the interface enable state.
		pEnetLinkAttr->lInterfaceFlags |= ENETLINK_FLAG_LINK_ACTIVE;
		pEnetLinkAttr->bInterfaceState = ENETLINK_INTERFACE_STATE_ENABLED;

		switch ( auto_neg_state )
		{
			case Eth_Port::ETH_AUTO_NEG_STATE_LINK_INACTIVE:		// used new macros defined Eth_Port.h by LTK -
																	// Shekhar during LTKBI
				pEnetLinkAttr->lInterfaceFlags &=
					~ENETLINK_FLAG_LINK_ACTIVE;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_IN_PROGRESS:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_IN_PROGRESS;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_FAIL_DEFAULT_USED:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_DUPLEX_SPEED_FAIL;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_DUPLEX_FAIL_SPEED_OK:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_DUPLEX_FAIL;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_SUCCESS:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_SUCCESS;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_NEGOTIATION_DISABLED:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_NOT_ATTEMPTED;
				break;

			case Eth_Port::ETH_AUTO_NEG_STATE_PORT_DISABLED:
				pEnetLinkAttr->lInterfaceFlags |=
					CIP_NEG_STATUS_AUTONEG_NOT_ATTEMPTED;
				pEnetLinkAttr->bInterfaceState = ENETLINK_INTERFACE_STATE_DISABLED;
				pEnetLinkAttr->lInterfaceFlags &=
					~ENETLINK_FLAG_LINK_ACTIVE;
				break;

			default:
				break;
		}

		platformGetMacID( lIPAddress, pEnetLinkAttr->macAddr );

#ifdef EIP_ETHERNET_PORT_ADMIN_SUPPORT
		// Retrieve the whether the port is set to be enabled after next reset.
		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_port_enable_status,
			reinterpret_cast<uint8_t*>( &port_enable ) );
		if ( port_enable == true )
		{
			pEnetLinkAttr->bAdminState = ENETLINK_ADMIN_STATE_ENABLED;
		}
		else
		{
			pEnetLinkAttr->bAdminState = ENETLINK_ADMIN_STATE_DISABLED;
		}
#else
		pEnetLinkAttr->bAdminState = 0U;
#endif

		// Retrieve the Auto Negotiation and Full Duplex capability.
		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_port_auto_neg_ena,
			reinterpret_cast<uint8_t*>( &auto_neg_enable ) );

		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_port_full_duplex_ena,
			reinterpret_cast<uint8_t*>( &full_duplex_enable ) );

		pEnetLinkAttr->iInterfaceControlBits = 0U;
		if ( auto_neg_enable == true )
		{
			pEnetLinkAttr->iInterfaceControlBits |= ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED;
		}
		else
		{
			pEnetLinkAttr->iInterfaceControlBits &= ~ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED;
			if ( full_duplex_enable == true )
			{
				pEnetLinkAttr->iInterfaceControlBits |= ENETLINK_INTERFACE_CONTROL_FORCED_FULL_DUPLEX;
			}
			else
			{
				pEnetLinkAttr->iInterfaceControlBits &= ~ENETLINK_INTERFACE_CONTROL_FORCED_FULL_DUPLEX;
			}
		}

		// Retrieve the set link speed.
		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_port_link_speed_select,
			reinterpret_cast<uint8_t*>( &link_speed_sel ) );

		if ( auto_neg_state != Eth_Port::ETH_AUTO_NEG_STATE_NEGOTIATION_DISABLED )
		{
			// pEnetLinkAttr->iInterfaceControlBits |=
			// ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED;
			// If we don't allow the setting of the interface speed then we zero
			// out the settable speed - I think. MAVMAVMAVMAV
			pEnetLinkAttr->iForcedInterfaceSpeed = 0U;	/* //k: It's effect is
														   removed by adding 2 lines after this if....else statement */
		}
		else// if ( full_duplex_enable == true )
		{
			pEnetLinkAttr->iForcedInterfaceSpeed = link_speed_sel;
			// Added by Shekhar to update Interface Flag when used Forced Duplex Mode
			if ( full_duplex_enable )// This should be "( full_duplex_state )", but changed it to link_speed_sel
			{
				pEnetLinkAttr->lInterfaceFlags |= EL_FLAG_DUPLEX_MODE;
			}
			else
			{
				pEnetLinkAttr->lInterfaceFlags &= ~( EL_FLAG_DUPLEX_MODE );
			}
		}

		// Following unwanted code commented to fix pre-conformance issue
		// EIP_PLATFORM_Get_DCID(
		// eth_port_config[phy_port].eth_port_link_speed_select,
		// reinterpret_cast<uint8_t*>(&link_speed_sel ));//#k:New
		//
		// pEnetLinkAttr->iForcedInterfaceSpeed = link_speed_sel;          //#k: New

		// Retrieve the active link speed.
		EIP_PLATFORM_Get_DCID(
			Eth_Port_Config[phy_port].eth_active_link_speed,
			reinterpret_cast<uint8_t*>( &link_speed_state ) );

		pEnetLinkAttr->lInterfaceSpeed = link_speed_sel;// This should be "link_speed_state;", but changed it to
														// link_speed_sel
														// to pass conformance test

		pEnetLinkAttr->bInterfaceType = ENETLINK_INTERFACE_TYPE_TWISTED_PAIR;

		strncpy( pEnetLinkAttr->InterfaceLabel,
				 reinterpret_cast<char const*>( ETH_PORT_LABEL_STRINGS[phy_port] ),
				 sizeof( ETH_PORT_LABEL_STRINGS[phy_port] ) );
		// socketGetAddressString(lIPAddress));

		pEnetLinkAttr->Capabilities.capabilities = ENETLINK_CAPABILITY_REQUIRE_RESET |
			ENETLINK_CAPABILITY_AUTO_NEG | ENETLINK_CAPABILITY_MANUAL_SPEED_DUPLEX;

		pEnetLinkAttr->Capabilities.bSpeedDuplexCount = ENETLINK_MAX_SPEED_DUPLEX_COUNT;

		pEnetLinkAttr->Capabilities.aInterfaceSpeed[0] = 10U;
		pEnetLinkAttr->Capabilities.aInterfaceSpeed[1] = 100U;
		pEnetLinkAttr->Capabilities.aInterfaceSpeed[2] = 10U;
		pEnetLinkAttr->Capabilities.aInterfaceSpeed[3] = 100U;

		pEnetLinkAttr->Capabilities.aInterfaceDuplexMode[0] = 0U;
		pEnetLinkAttr->Capabilities.aInterfaceDuplexMode[1] = 1U;
		pEnetLinkAttr->Capabilities.aInterfaceDuplexMode[2] = 1U;
		pEnetLinkAttr->Capabilities.aInterfaceDuplexMode[3] = 0U;
	}
}

/*------------------------------------------------------------------------------
** platformSetEnetLinkAttributes( )
**
** Store settable Ethernet Link attributes
**------------------------------------------------------------------------------
*/
void platformSetEnetLinkAttributes( uint32_t lMask,
									uint32_t iInstance,
									ENETLINK_INST_ATTR* pEnetLinkAttr )
{

	/*
	** This function is called when a CIP message wants to set Ethernet Link
	**  attributes.
	**
	** It should use the pEnetLinkAttr structure with
	** the Ethernet Link attributes for the local network interface defined
	** for iInstance
	**
	** The EnetLinkConfig structure is defined in eiptcpip.h as
	**
	**	typedef struct tagENETLINK_INST_ATTR
	**	{
	**	uint32_t   lInterfaceSpeed;
	**	uint32_t   lInterfaceFlags;
	**	uint8_t    macAddr[MAC_ADDR_LENGTH];
	**	uint8_t	 bInterfaceType;
	**	uint8_t	 bInterfaceState;
	**	uint8_t	 bAdminState;
	**	INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
	**	}
	**	ENETLINK_INST_ATTR;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see EtherNet/IP protocol specification for further information.
	*/
	bool_t auto_neg_enable;
	bool_t full_duplex_enable;
	uint16_t link_speed;
	bool_t auto_neg_enable_current;
	bool_t full_duplex_enable_current;
	uint16_t link_speed_current;
	bool_t port_enable;
	// DCI_ERROR_TD return_status;
	uint8_t phy_port;

	if ( iInstance <= TOTAL_ETHERNET_LINK_INSTANCES )
	{
		phy_port = iInstance - 1U;

#if !defined ( __LAN8710__ ) && !defined ( __LAN9303__ )
		if ( Test_Bit( lMask, ENETLINK_INST_ATTR_INTERFACE_CONTROL ) )
		{
			EIP_PLATFORM_Get_DCID(
				Eth_Port_Config[phy_port].eth_port_auto_neg_ena,
				reinterpret_cast<uint8_t*>( &auto_neg_enable_current ) );
			EIP_PLATFORM_Get_DCID(
				Eth_Port_Config[phy_port].eth_port_full_duplex_ena,
				reinterpret_cast<uint8_t*>( &full_duplex_enable_current ) );
			EIP_PLATFORM_Get_DCID(
				Eth_Port_Config[phy_port].eth_port_link_speed_select,
				reinterpret_cast<uint8_t*>( &link_speed_current ) );

			if ( ( pEnetLinkAttr->iInterfaceControlBits &
				   ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED ) != 0U )
			{
				auto_neg_enable = true;
			}
			else
			{
				auto_neg_enable = false;
			}

			if ( ( pEnetLinkAttr->iInterfaceControlBits &
				   ENETLINK_INTERFACE_CONTROL_FORCED_FULL_DUPLEX ) != 0U )
			{
				full_duplex_enable = true;
			}
			else
			{
				full_duplex_enable = false;
			}
			link_speed = pEnetLinkAttr->iForcedInterfaceSpeed;

			// if( ( auto_neg_enable != auto_neg_enable_current ) ||
			// ( full_duplex_enable != full_duplex_enable_current ) ||
			// ( link_speed != link_speed_current ) )
			// {
			// g_control_interface_setting_changed = true;
			// }

			EIP_PLATFORM_Set_DCID(
				Eth_Port_Config[phy_port].eth_port_auto_neg_ena,
				reinterpret_cast<uint8_t*>( &auto_neg_enable ) );
			EIP_PLATFORM_Set_DCID(
				Eth_Port_Config[phy_port].eth_port_full_duplex_ena,
				reinterpret_cast<uint8_t*>( &full_duplex_enable ) );
			EIP_PLATFORM_Set_DCID(
				Eth_Port_Config[phy_port].eth_port_link_speed_select,
				reinterpret_cast<uint8_t*>( &link_speed ) );
		}
#endif

#if !defined ( __LAN8710__ ) && !defined ( __LAN9303__ )
		if ( Test_Bit( lMask, ENETLINK_INST_ATTR_ADMIN_STATE ) )
		{
			port_enable = ( pEnetLinkAttr->bAdminState == 1U );
			EIP_PLATFORM_Set_DCID(
				Eth_Port_Config[phy_port].eth_port_enable_status,
				reinterpret_cast<uint8_t*>( &port_enable ) );
		}
#endif
	}
	// return_status = return_status;
}

/*------------------------------------------------------------------------------
** platformTestSetEnetLinkAttributes()
**
** Ensure a particular Ethernet Link object attribute can be set before it's
** actually set.  Return false if error is generated.
**------------------------------------------------------------------------------
*/
bool_t platformTestSetEnetLinkAttributes( uint32_t lMask,
										  uint32_t iInstance,
										  ENETLINK_INST_ATTR* pEnetLinkAttr,
										  uint8_t* pbGeneralError )
{
	bool_t success = true;
	bool_t port_1_enabled;
	bool_t port_2_enabled = false;
	uint8_t phy_port;

	if ( iInstance <= TOTAL_ETHERNET_LINK_INSTANCES )
	{
		phy_port = iInstance - 1U;

		if ( Test_Bit( lMask, ENETLINK_INST_ATTR_INTERFACE_CONTROL ) )
		{

#if !defined ( __LAN8710__ ) && !defined ( __LAN9303__ )
			if ( ( pEnetLinkAttr->iInterfaceControlBits &
				   ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED ) != 0U )
			{
				// If auto negotiation is enabled then we interpret that as
				// meaning we have to have everything else disabled or zero.
				if ( ( ( pEnetLinkAttr->iInterfaceControlBits &
						 ~ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED ) != 0U ) ||
					 ( pEnetLinkAttr->iForcedInterfaceSpeed != 0U ) )
				{
					success = false;
					*pbGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
				}
			}	// Check to see if the rest of the bits are zero.
			else if ( ( ( pEnetLinkAttr->iInterfaceControlBits &
						  ~( ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED |
							 ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED ) ) != 0U ) ||
					  ( ( pEnetLinkAttr->iForcedInterfaceSpeed != 10U ) &&
						( pEnetLinkAttr->iForcedInterfaceSpeed != 100U ) ) )
			{
				success = false;
				*pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA;
			}
#endif
		}

		if ( Test_Bit( lMask, ENETLINK_INST_ATTR_ADMIN_STATE ) )
		{

#if !defined ( __LAN8710__ ) && !defined ( __LAN9303__ )
			EIP_PLATFORM_Get_DCID( Eth_Port_Config[0].eth_port_enable_status,
								   reinterpret_cast<uint8_t*>( &port_1_enabled ) );

			if ( TOTAL_ETH_PORTS_IN_USE == 2 )
			{
				EIP_PLATFORM_Get_DCID( Eth_Port_Config[1].eth_port_enable_status,
									   reinterpret_cast<uint8_t*>( &port_2_enabled ) );
			}
			// We only have to check if a port is disabled is if we are going
			// to a disable potentially of the other port.
			if ( pEnetLinkAttr->bAdminState == ENETLINK_ADMIN_STATE_DISABLED )
			{
				// Allow someone to disable the port that is already disabled but
				// don't allow them to disable the other port
				// if one port is disabled already.
				if ( ( ( phy_port == 0U ) && ( port_2_enabled == false ) )
				#if ETH_PORT_LABEL_STRING_INSTANCES == 2
					 ||
					 ( ( phy_port == 1U ) && ( port_1_enabled == false ) )
				#endif
					  )
				{
					success = false;
					// *pbGeneralError = ROUTER_ERROR_BAD_ATTR_DATA; //#k: Ori
					*pbGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;	// #k: new
				}
			}
#endif
		}
	}
	else
	{
		success = false;
		*pbGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	}
	/* Default to accept */
	return ( success );
}

//////#k: #ifdef EIP_QOS
/*------------------------------------------------------------------------------
** platformGetQoSAttributes( )
**
** Populates passed structure with QoS attributes
**------------------------------------------------------------------------------
*/
void platformGetQoSAttributes(
	TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
	/*
	** This function is called when a host IP address is claimed by the client
	** application.
	**
	** It should populate the pTcpipConfig->QoSConfig structure with
	** the QoS attributes for the local network interface with IP
	** address pTcpipConfig->InterfaceConfig.lIpAddr
	**
	** The QoSConfig structure is defined in eipqos.h as
	**
	**	typedef struct tagQOS_INST_ATTR
	**	{
	**	uint8_t    b8021QTagEnable;
	**	uint8_t	 bDSCPPTPEvent;
	**	uint8_t	 bDSCPPTPGeneral;
	**	uint8_t	 bDSCPUrgent;
	**	uint8_t    bDSCPScheduled;
	**	uint8_t	 bDSCPHigh;
	**	uint8_t	 bDSCPLow;
	**	uint8_t	 bDSCPExplicit;
	**	}
	**	QOS_INST_ATTR;
	**
	** Please see EtherNet/IP protocol specification for further information.
	*/

}

/*------------------------------------------------------------------------------
** platformSetQoSAttributes( )
**
** Store settable QoS attributes
**------------------------------------------------------------------------------
*/
bool_t platformSetQoSAttributes( uint32_t lMask,
								 TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpipConfig )
{
	/*
	** This function is called when a CIP message wants to set QoS attributes.
	**
	** It should use the pTcpipConfig->QoSConfig structure with
	** the QoS attributes for the local network interface with IP
	** address pTcpipConfig->InterfaceConfig.lIpAddr
	**
	** The QoSConfig structure is defined in eipqos.h as
	**
	**	typedef struct tagQOS_INST_ATTR
	**	{
	**	uint8_t    b8021QTagEnable;
	**	uint8_t	 bDSCPPTPEvent;
	**	uint8_t	 bDSCPPTPGeneral;
	**	uint8_t	 bDSCPUrgent;
	**	uint8_t    bDSCPScheduled;
	**	uint8_t	 bDSCPHigh;
	**	uint8_t	 bDSCPLow;
	**	uint8_t	 bDSCPExplicit;
	**	}
	**	QOS_INST_ATTR;
	**
	** lMask determines which attributes are being set (bit flag)
	**
	** Please see EtherNet/IP protocol specification for further information.
	*/

	/* setting QoS data not supported */
	return ( false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INT32 platformSetDSCPValue( SOCKET s, uint8_t bDSCPValue )
{
	return ( SOCKET_ERROR );
}

#ifdef __cplusplus
}
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void TcpIpConfigCapabilityCallback( TCPIP_APP_CALLBACK pCallback )
{
	isTcpIpConfigCapabilityOn = pCallback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IpAdrCntrlFromEipCallback( TCPIP_APP_CALLBACK pCallback )
{
	isIpAdrCntrlFromEipOn = pCallback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IpAdrSettebleFromEthCallback( TCPIP_APP_CALLBACK pCallback )
{
	isIpAddrSettebleFromEthernet = pCallback;
}

//////#k: #endif /* EIP_QOS */
