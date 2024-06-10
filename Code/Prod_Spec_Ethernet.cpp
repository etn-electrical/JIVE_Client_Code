/**
 **********************************************************************************************
 * @file            Prod_Spec_Ethernet.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product.
 *                  Adopter can initialize the ethernet/EMAC here
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec_LTK_ESP32.h"

#ifdef ETHERNET_SETUP

#include "Prod_Spec_Ethernet.h"
#ifdef WEB_SERVER_SUPPORT
#include "Prod_Spec_Webserver.h"
#endif
#include "Prod_Spec_OS.h"
#include "Prod_Spec_MEM.h"
#include "EthConfig.h"
#include "Prod_Spec_BACNET.h"
#include "StdLib_MV.h"
#include "CR_Tasker.h"
#include "Ram_Hybrid.h"
#include "Ram_NV.h"
#include "DCI_NV_Mem.h"
#include "NV_Mem.h"
#include "NV_Address.h"
#include "uC_Rand_Num.h"
#include "Output_Null.h"
#include "Eth_if.h"
#include "Eth_Port.h"
#include "Eth_PHY_If.h"
#if defined ( __IP101GRI__ )
#include "IP101GRI.h"		/// > @TODO: Include this file
#endif

#include "EMAC_Driver.h"
#include "ESP32_Eth_Driver.h"
#include "httpd.h"
#include "Modbus_TCP.h"
#include "Modbus_DCI_Data.h"
#include "Modbus_Net.h"
#include "Prod_Spec_IOT.h"
#include "Trusted_IP_Filter.h"
#ifdef BACNET_IP_SETUP
#include "vsbhp.h"
#endif
#include "uC_Rand_Num.h"
#include "CRC16.h"
#include "Prod_Spec_Debug.h"
#include "Prod_Spec_REST.h"
#include "esp_eth.h"
#include "Format_Handler.h"
#include "Prod_Spec_Services.h"
#include "Services.h"
#include "uC_Base.h"
#include "EIP_if.h"
#ifdef EIP_SETUP
#include "Prod_Spec_EIP.h"
#include "eipinc.h"
#endif


#if TOTAL_ETH_PORTS_IN_USE == 2
#define  ETH_PORT_CONFIG_ARR_SIZE 2U
#else
#define  ETH_PORT_CONFIG_ARR_SIZE 1U
#endif

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// Note: these are redundant with DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM, eg RESTORE_IP = 0U;
const uint8_t IP_ADDRESS_SELECT_FROM_ETHERNET_STATUS = 0x02U;
const uint8_t IP_ADDRESS_SELECT_RESTORE = 0x00U;
const uint8_t IP_ADDRESS_SELECT_DHCP = 0x01U;
const uint8_t IP_ADDRESS_SELECT_NV = 0x02U;
const uint8_t IP_ADDRESS_SELECT_FROM_SWITCH = 0x03U;
/*
   // The following IP Addresses are used as default settings.
   const uint32_t DIP_SEL_ROM_IP_ADDRESS = 0xC0A801FEU;// 0xC0A801FEU =192.168.1.254
   const uint32_t DIP_SEL_ROM_SUBNET_MASK = 0xFFFFFF00U;	// 255.255.255.0
   const uint32_t DIP_SEL_ROM_DEFAULT_GATEWAY = 0xC0A87801;	// 192.168.120.1
 */

const Eth_Port::eth_dcid_t Eth_Port_Config[ETH_PORT_CONFIG_ARR_SIZE] =
{
	{
		DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
		DCI_PHY1_FULL_DUPLEX_ACTUAL_DCID,
		DCI_PHY1_AUTONEG_STATE_DCID,
		DCI_PHY1_PORT_ENABLE_DCID,
		DCI_PHY1_LINK_SPEED_SELECT_DCID,
		DCI_PHY1_FULL_DUPLEX_ENABLE_DCID,
		DCI_PHY1_AUTONEG_ENABLE_DCID
	},

};

const EMAC_Driver::dcid_emac_params_t EMAC_Config =
{
	DCI_BROADCAST_ENABLE_DCID,
	DCI_MULTICAST_ENABLE_DCID
};

/*  The Ethernet module allocated MAC address range.*/
static const uint8_t MAC_ADDR_LEN = 6U;
static const uint8_t EATON_MAC_ADDRESS_RANGE_LEN = 4U;
static const uint8_t EATON_MAC_ADDRESS_RANGE[EATON_MAC_ADDRESS_RANGE_LEN] =
{ 0x05U, 0xAFU, 0xD0U, 0x00U };

// Defines the filter enable DCIDs and associated ports.
#ifdef TRUSTED_IP_FILTER
Trusted_IP_Filter::trusted_host_port_info_t TRUSTED_HOST_PORTS_LIST[] =
{
#ifdef MODBUS_TCP_SETUP
	{
		MODBUS_TCP_DEFAULT_PORT,
		DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_DCID,
		DCI_TRUSTED_IP_WHITELIST_DCID,
	},
#endif	// MODBUS_TCP_SETUP
 #ifdef BACNET_IP_SETUP
	{
		BACNET_IP_DEFAULT_PORT,
		DCI_TRUSTED_IP_BACNET_IP_ENABLE_DCID,
		DCI_TRUSTED_IP_WHITELIST_DCID,
	},
  #endif	// BACNET_IP_SETUP
#ifdef EIP_SETUP
	{
		ENCAP_SERVER_PORT,
		DCI_TRUSTED_IP_EIP_ENABLE_DCID,
		DCI_TRUSTED_IP_WHITELIST_DCID,
	}
#endif	// EIP_SETUP

};
enum
{
#ifdef MODBUS_TCP_SETUP
	MODBUS_TCP_TRUSTED_IP_ENBLE,
#endif	// MODBUS_TCP_SETUP

#ifdef BACNET_IP_SETUP
	BACNET_TRUSTED_IP_ENABLE,
#endif	// BACNET_IP_SETUP

#ifdef EIP_SETUP
	EIP_TRUSTED_IP_ENABLE,
#endif	// EIP_SETUP

	TOTAL_NUM_TRUSTED_IP_ENBLE
};

#endif	// TRUSTED_IP_FILTER
/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
extern DCI_Owner* active_ip_address_owner;
extern DCI_Owner* active_netmask_owner;
extern DCI_Owner* active_default_gateway_owner;
extern DCI_Owner* static_ip_address_owner;
extern DCI_Owner* static_netmask_owner;
extern DCI_Owner* static_default_gateway_owner;
extern BF_Lib::Ram_Hybrid* hybrid_ram_ctrl;
DCI_Patron* httpd_dci_access = nullptr;

PHY_Driver* PHY_Ctrl = nullptr;
Eth_if* Ethernet_Ctrl = nullptr;
EMAC_Driver* EMAC_Control = nullptr;

DCI_Owner* ip_address_allocation_method_owner = nullptr;
DCI_Owner* ip_address_allocation_method_status_owner = nullptr;
DCI_Owner* ip_adrs_alloc_cntrl_ETH_or_DIP_status_owner = nullptr;
DCI_Owner* master_hardcoded_IP_Address_owner = nullptr;
DCI_Owner* dip_switch_powerboard_owner = nullptr;
DCI_Owner* mac_id_owner = nullptr;

Modbus_Net* modbus_tcp = nullptr;
static DCI_Owner* modbus_tcp_timeout_owner = nullptr;


bool ip_adrs_cntrl_from_ethernet_lock_active_ip = false;
uC_ETHERNET_IP_CONFIG_ST Active_IP_Address;
uC_ETHERNET_IP_CONFIG_ST Static_IP_Address;
uint8_t MAC_ID_String[MAC_ADDR_LEN];

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
extern void Update_Ethernet_Handle( Eth_if* ethernet_ctrl );

extern void Ethernet_If_EMAC( EMAC_Driver* EMAC );

void ProdSpec_Post_Ethernet_Setup( void );

void Load_Network_Settings( void );

uint8_t* Get_Mac_Id( void );

void Set_IP_address_allocation_method(
	uint8_t IP_Address_Select_Method_From_Ethernet );

void Ethernet_Low_Level_Init( void );

void Modbus_Tcp_Setup( void );

/**
 **********************************************************************************************
 * @brief                     Ethernet/EMAC initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */

void PROD_SPEC_ETH_Init( void )
{
	/* Read the etherent network settings are read the type of IP mode selection */
	Load_Network_Settings();

	Ethernet_Low_Level_Init();

#ifdef WEB_SERVER_SUPPORT
	PROD_SPEC_REST_Init();
#endif
}

/**
 **********************************************************************************************
 * @brief                     Ethernet Low Level Initialization
 * @param[in] void            none
 * @return[out] Ethernet*     Ethernet object handle
 * @n
 **********************************************************************************************
 */
void Ethernet_Low_Level_Init( void )
{
	eth_mac_config_t MAC_Config = ETH_MAC_DEFAULT_CONFIG();

	MAC_Config.smi_mdc_gpio_num = ETH_MDC_GPIO;
	MAC_Config.smi_mdio_gpio_num = ETH_MDIO_GPIO;
	static ESP32_Eth_Driver EMAC_Obj( MAC_Config, Get_Mac_Id(), &EMAC_Config );

	EMAC_Control = &EMAC_Obj;

	/* pass the emac handle to ethernetif */
	Ethernet_If_EMAC( EMAC_Control );

	/* Initialize the Ethernet PHY chip and pass the handle to Ethernet::Ethernet() constructor */
#if defined ( __IP101GRI__ )
	eth_phy_config_t PHY_Config = ETH_PHY_DEFAULT_CONFIG();
	PHY_Config.phy_addr = ETH_PHY_ADDR;
	/* Assign the processors port pin for the reset control of PHY */
	PHY_Config.reset_gpio_num = ETH_PHY_RST_GPIO;
	PHY_Ctrl = new IP101GRI( PHY_Config, EMAC_Control );
#endif

	if ( httpd_dci_access == nullptr )
	{
		httpd_dci_access = new DCI_Patron( FALSE );	// not auto-correcting out-of-range data

	}
	/* Initialize the Ethernet construction class */
	Ethernet_Ctrl = new Eth_if( PHY_Ctrl, TOTAL_ETH_PORTS_IN_USE, Eth_Port_Config, &Active_IP_Address,
								ProdSpec_Post_Ethernet_Setup );
#ifdef WEB_SERVER_SUPPORT
	Update_Ethernet_Handle( Ethernet_Ctrl );
#endif

}

/**
 **********************************************************************************************
 * @brief                     Initialization to be done post ethernet initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void ProdSpec_Post_Ethernet_Setup( void )
{
#ifdef WEB_SERVER_SUPPORT
	PROD_SPEC_WEBSERVER_Init();
#endif

#ifdef MODBUS_TCP_SETUP
	modbus_tcp_timeout_owner = new DCI_Owner( DCI_MODBUS_TCP_COMM_TIMEOUT_DCID );

	Modbus_Tcp_Setup();
#endif

#ifdef EIP_SETUP
	PROD_SPEC_EIP_Stack_Init();
#endif

#ifdef BACNET_IP_SETUP
	if ( Is_Bacnet_Ip_Enabled() == true )
	{
		frStartup( portBIP );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "BACnet/IP initialised." );
	}
#endif

}

/**
 **********************************************************************************************
 * @brief                     Load Network settings parameter from DCI database
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Load_Network_Settings( void )
{
	uint8_t IP_Address_Select_Method_From_Ethernet = IP_ADDRESS_SELECT_RESTORE;
	DCI_Owner* dhcp_to_static_ip_addr_lock_owner;

	/* Load the static network address settings from DCID */
	static_ip_address_owner = new DCI_Owner( DCI_STATIC_IP_ADDRESS_DCID );
	static_netmask_owner = new DCI_Owner( DCI_STATIC_SUBNET_MASK_DCID );
	static_default_gateway_owner = new DCI_Owner( DCI_STATIC_DEFAULT_GATEWAY_DCID );

	/* Load the network address settings from NV memory DCID */
	active_ip_address_owner = new DCI_Owner( DCI_ACTIVE_IP_ADDRESS_DCID );
	active_netmask_owner = new DCI_Owner( DCI_ACTIVE_SUBNET_MASK_DCID );
	active_default_gateway_owner = new DCI_Owner( DCI_ACTIVE_DEFAULT_GATEWAY_DCID );

	/* Read the IP Address Allocation method */
	ip_address_allocation_method_owner = new DCI_Owner( DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID );

	/* Store the status of IP Address Allocation method */
	ip_address_allocation_method_status_owner =
		new DCI_Owner( DCI_IP_ADDRESS_ALLOCATION_METHOD_STATUS_DCID );

	ip_adrs_alloc_cntrl_ETH_or_DIP_status_owner =
		new DCI_Owner( DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_DCID );

	dhcp_to_static_ip_addr_lock_owner = new DCI_Owner( DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_DCID );
	dhcp_to_static_ip_addr_lock_owner->Check_Out( ip_adrs_cntrl_from_ethernet_lock_active_ip );

	ip_address_allocation_method_owner->Check_Out(
		reinterpret_cast<uint8_t*>( &IP_Address_Select_Method_From_Ethernet ) );
	/* If allocation method is invalid assign to RESTORE type */
	if ( IP_Address_Select_Method_From_Ethernet >= 4U )
	{
		IP_Address_Select_Method_From_Ethernet = IP_ADDRESS_SELECT_RESTORE;
	}

	Set_IP_address_allocation_method( IP_Address_Select_Method_From_Ethernet );
}

/**
 **********************************************************************************************
 * @brief                     Get Mac ID of the etehrnet hardware
 * @param[in] void            none
 * @return[out] uint8_t*      pointer to MAC ID string
 * @n
 **********************************************************************************************
 */
uint8_t* Get_Mac_Id( void )
{
	uint32_t MAC_ID_Check = 0U;
	const uint16_t Str_Buf_Size = 20U;
	uint8_t MAC_String[Str_Buf_Size];

	mac_id_owner = new DCI_Owner( DCI_ETHERNET_MAC_ADDRESS_DCID );
	mac_id_owner->Attach_Callback(
		&PROD_SPEC_SERVICES_MFG_Unlock_Write_Check_CB,
		nullptr,
		( DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_SET_INIT_CMD_MSK ) );

	mac_id_owner->Check_Out( MAC_ID_String );

	for ( BF_Lib::MUINT8 Counter = 0U;
		  Counter < static_cast<uint8_t>( sizeof ( MAC_ID_String ) );
		  Counter++ )
	{
		MAC_ID_Check += MAC_ID_String[Counter];
	}

	/* Check to see if the MAC id value is defined (not zero).
	 * If MAC ID is not present in NV then use the emac address from efuse.
	 */
	if ( MAC_ID_Check == 0U )
	{
		/* get emac address from efuse */
		if ( esp_read_mac( MAC_ID_String, ESP_MAC_ETH ) != ESP_OK )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,  "Fetch Ethernet MAC Address Failed\r\n" );
		}
	}
	FORMAT_HANDLER_MAC_ADDRESS( MAC_ID_String, MAC_ADDR_LEN, MAC_String, Str_Buf_Size, RAW_TO_STRING );
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "MAC Address - %s\r\n", MAC_String );
	return ( MAC_ID_String );
}

/**
 **********************************************************************************************
 * @brief                     Sets the IP allocation method for ethernet hardware
 * @param[in] void            none
 * @return[out] uint8_t*      pointer to MAC ID string
 * @n
 **********************************************************************************************
 */
void Set_IP_address_allocation_method( uint8_t IP_Address_Select_Method_From_Ethernet )
{

	uint8_t IP_Adrs_Select_Control_Status = IP_ADDRESS_SELECT_FROM_ETHERNET_STATUS;

	if ( IP_Address_Select_Method_From_Ethernet == IP_ADDRESS_SELECT_RESTORE )
	{
		Active_IP_Address.ip_select = uC_ETHERNET_IP_SELECT_PASSED;
		static_ip_address_owner->Check_Out( reinterpret_cast<uint8_t*>( &Active_IP_Address.ip_address ) );
		static_netmask_owner->Check_Out( reinterpret_cast<uint8_t*>( &Active_IP_Address.subnet_mask ) );
		static_default_gateway_owner->Check_Out( reinterpret_cast<uint8_t*>( &Active_IP_Address.default_gateway ) );
		/*Commenting as taken care from DCI sheet*/
		/*Active_IP_Address.ip_address = DIP_SEL_ROM_IP_ADDRESS;
		   Active_IP_Address.subnet_mask = DIP_SEL_ROM_SUBNET_MASK;
		   Active_IP_Address.default_gateway = DIP_SEL_ROM_DEFAULT_GATEWAY;*/
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "IP Allocation method used - Static" );
	}
	else if ( IP_Address_Select_Method_From_Ethernet == IP_ADDRESS_SELECT_DHCP )
	{
		Active_IP_Address.ip_select = uC_ETHERNET_IP_SELECT_DHCP;
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "IP Allocation method used - DHCP" );
	}
	else if ( IP_Address_Select_Method_From_Ethernet == IP_ADDRESS_SELECT_NV )
	{
		Active_IP_Address.ip_select = uC_ETHERNET_IP_SELECT_PASSED;

		static_ip_address_owner->Check_Out_Init(
			reinterpret_cast<uint8_t*>( &Static_IP_Address.ip_address ) );

		static_netmask_owner->Check_Out_Init(
			reinterpret_cast<uint8_t*>( &Static_IP_Address.subnet_mask ) );

		static_default_gateway_owner->Check_Out_Init(
			reinterpret_cast<uint8_t*>( &Static_IP_Address.default_gateway ) );

		Active_IP_Address = Static_IP_Address;

		active_ip_address_owner->Check_In(
			reinterpret_cast<uint8_t*>( &Active_IP_Address.ip_address ) );

		active_netmask_owner->Check_In(
			reinterpret_cast<uint8_t*>( &Active_IP_Address.subnet_mask ) );

		active_default_gateway_owner->Check_In(
			reinterpret_cast<uint8_t*>( &Active_IP_Address.default_gateway ) );
	}
	else if ( IP_Address_Select_Method_From_Ethernet == IP_ADDRESS_SELECT_FROM_SWITCH )
	{}

	if ( IP_Adrs_Select_Control_Status == IP_ADDRESS_SELECT_FROM_ETHERNET_STATUS )
	{
		ip_address_allocation_method_owner->Check_In( IP_Address_Select_Method_From_Ethernet );

		ip_address_allocation_method_owner->Check_In_Init(
			reinterpret_cast<uint8_t*>( &IP_Address_Select_Method_From_Ethernet ) );

		ip_address_allocation_method_status_owner->Check_In(
			IP_Address_Select_Method_From_Ethernet );
	}
}

/**
 **********************************************************************************************
 * @brief                     Modbus TCP Setup function
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Modbus_Tcp_Setup( void )
{
#ifdef MODBUS_TCP_SETUP
	static const uint8_t MODBUS_TCP_DEFAULT_ADDRESS = 1U;
	DCI_Owner* modbus_tcp_enable_owner = new DCI_Owner( DCI_MODBUS_TCP_ENABLE_DCID );
	bool_t modbus_init = false;
	modbus_tcp_enable_owner->Check_Out( modbus_init );
	if ( true == modbus_init )
	{
		modbus_tcp = new Modbus_Net( MODBUS_TCP_DEFAULT_ADDRESS,
									 new Modbus_TCP( nullptr, MODBUS_TCP_DEFAULT_PORT,
													 modbus_tcp_timeout_owner ),
									 &modbus_dci_data_target_info, modbus_tcp_timeout_owner,
									 Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE, false );

		modbus_tcp->Enable_Port( MODBUS_TCP_TX_MODE );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Modbus TCP initialised" );

#ifdef LOGGING_SUPPORT
		PROD_SPEC_LOG_Modbus( modbus_tcp );
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Logging enabled for Modbus TCP" );
#endif	/*Logging*/
	}
#endif	/*MODBUS_TCP_SETUP*/
	// coverity[leaked_storage]
}

#endif	/*ETHERNET_SETUP*/
