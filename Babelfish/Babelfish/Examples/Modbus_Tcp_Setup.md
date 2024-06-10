\example  Modbus_Tcp Server Setup
\brief    The sample code to establish a Modbus TCP server setup on EasyMxPRoV7 dvelopement board.
\par Board Specific Settings

 \details
    1. Make sure, Microcontroller STM32F207 is placed on EasyMXPRoV7 development board.
    2. This Board uses small footprint Ethernet Transceiver "LAN8720A" from Microchip.
       This Trasceiver only features RMII mode. Please make sure that microcontrollers pins
       used for interfacing Trasceiver are not used in any other module. below link can provide
       more information about the Trasceiver -
       www.microchip.com/wwwproducts/Devices.aspx?product=LAN8720A



 \par Pin Configurations
 \details Not Applicable

 \par Dependency
 \details
    1. static Ip address of the server port is assigned through DCI DB Creator excel tool.
       A sample value for DCID are assigned here. User can use different but unique values for IP configuration.
       Following DCID needs to be created and assigned its default value for proper communication -
       Description                 DCID Name                    Data Type     Length    Default Values 
       Active IP Address           DCI_ACTIVE_IP_ADDRESS            UINT8       4       0xF0, 0x01, 0xA8, 0xC0
       Active Subnet Mask          DCI_ACTIVE_SUBNET_MASK           UINT8       4       0x00, 0xFF, 0xFF, 0xFF
       Active Default Gateway      DCI_ACTIVE_DEFAULT_GATEWAY       UINT8       4       0x01, 0x01, 0xA8, 0xC0
       Ethernet MAC Address        DCI_ETHERNET_MAC_ADDRESS         UINT8       6       assigned in source code
       Link Speed Select           DCI_PHY1_LINK_SPEED_SELECT       UINT16      2       100
       Link Speed Actual           DCI_PHY1_LINK_SPEED_ACTUAL       UINT16      2
       Duplex Select               DCI_PHY1_FULL_DUPLEX_ENABLE      BOOL        1       TRUE
       Duplex Actual               DCI_PHY1_FULL_DUPLEX_ACTUAL      BOOL        1
       Link Auto-Negotiate Enable  DCI_PHY1_AUTONEG_ENABLE          BOOL        1       TRUE
       Link Auto-Negotiate State   DCI_PHY1_AUTONEG_STATE           UINT8       1
       Port Enable                 DCI_PHY1_PORT_ENABLE             BOOL        1       TRUE
       Multicast settings          DCI_MULTICAST_ENABLE             UINT32      4       0x00000010
       Broadcast settings          DCI_BROADCAST_ENABLE             UINT32      4       0x00000000
       MB CommLossTimeoutValue(ms) DCI_MODBUS_TCP_COMM_TIMEOUT      UINT16      2       0
       ACD Enable                  DCI_ETH_ACD_ENABLE               BOOL        1       TRUE
       ACD Conflict State          DCI_ETH_ACD_CONFLICT_STATE       UINT8       1
       ACD Conflicted State        DCI_ETH_ACD_CONFLICTED_STATE     UINT8       1       0
       ACD Conflicted Device MAC   DCI_ETH_ACD_CONFLICTED_MAC       UINT8       6       0
       ACD ARP PDU Conflicted Msg  DCI_ETH_ACD_CONFLICTED_ARP_PDU   UINT8       28      0
    2. For ethernet communication, IP settings of host PC needs to be configured accordingly.
    3. In RMII mode to communicate properly, MCO out should be configured to 50MHz.
       Please make sure that system clock is defined/configured is such that 50MHz MCO out can be derived.
 \par Sample Code Listing

~~~~{.cpp}

#include "Includes.h"

#include "ACD.h"
#include "DCI_Owner.h"
#include "Modbus_DCI_Data.h"
#include "Modbus_Net.h"
#include "Modbus_TCP.h"
#include "Output_Null.h"
#include "StdLib_MV.h"
#include "uC_Ethernet.h"
#include "uC_EMAC.h"
#include "uC_Rand_Num.h"
#define TOTAL_PORTS_IN_USE  1
#define EMAC_COMM_MODE  uC_EMAC_RMII_MODE
#define MCO_CLOCK_SELECT    true
#define ETH_SWITCH_CHIP    false
#define LAN_PHY	            1           /*Configuration as per PatB board*/
#define PHY_ADDR            0           /*Configuration as per PatB board*/
// Modbus TCP default address.
static uint8_t MODBUS_TCP_DEFAULT_ADDRESS = 1U;
// MAC ID String length
#define MAC_ID_STRING_LENGTH		6
//The Ethernet module allocated MAC address range.
#define EATON_MAC_ADDRESS_RANGE_LEN	4

const uint8_t EATON_MAC_ADDRESS_RANGE[EATON_MAC_ADDRESS_RANGE_LEN] =
		{ 0x05, 0xAF, 0xD0, 0x00 };

// These are the roads into the active ip address owners which need to be
// created before the stack gets up and running.
DCI_Owner* active_ip_address_owner;
DCI_Owner* active_netmask_owner;
DCI_Owner* active_default_gateway_owner;

// To hold active Ip config details
uC_ETHERNET_IP_CONFIG_ST active_ip_address;

Modbus_Net* modbus_tcp;

/* This function does following Tasks -
 * 1. Generate 2 characters random number, append the random number to Eaton
 *    allocated mac string to create a 6 characters Unique MAC Address.
 * 2. Initialize MAC layer with EMAC control structure, RMII mode, MAC ID.
 *    Due to unavailability of External Crystal, For this example MCO output clock generated through PLL
 *    clock (through a configurable prescaler) on PA8 pin is used.
 * 3. Lastly It initialize Address conflict Detection Machine
 */

void Init_Ethernet_Module( void )
{
	BOOL eth_switch_enable;
	BOOL alr_learning_enable;

	/* Assign dnyamic memory to etherent module */
	Init_Dynamic_RAM_Modules();
	/* Read the etherent network settings are read the type of IP mode selection */
	Load_Network_Settings();

	/* Initialize the Emac */
	emac_control = new Stm32_Eth_Driver( static_cast<uint8_t>( uC_BASE_EMAC_CTRL_0 ),
		EMAC_COMM_MODE,
		Get_Mac_Id(), MCO_CLOCK_SELECT, &emac_config );
        /* pass the emac handle to ethernetif */
	ethernetif_emac( emac_control );

	/* Initialize the Ethernet PHY chip */

	eth_switch_enable = ETH_SWITCH_CHIP;   
	alr_learning_enable = false;
	BF_Lib::Output* reset_io_ctrl =
		new BF_Lib::Output_Null();
	phy_ctrl = new LAN( reset_io_ctrl, emac_control, eth_switch_enable,
		alr_learning_enable,
            PHY_ADDR );
	/* Initialize the Ethernet construction class */
	eth1_ctrl = new Ethernet( phy_ctrl, TOTAL_PORTS_IN_USE, eth_port_config, &active_ip_address,
                             ProdSpec_Post_Ethernet_Setup );

	/* Initialize the Address Conflict Detection machine for EtherNet/IP */
	new ACD( &acd_dcid_param );
	ACD::Set_State_Change_Callback( &ACD_State_Change_Callback );
}

void Init_Dynamic_RAM_Modules()
{
	hybrid_ram_ctrl->Enable_Dynamic_Mem();
}
void ProdSpec_Post_Ethernet_Setup()
{
	uC_ETHERNET_IP_CONFIG_ST temp_active_ip_address;

	Ethernet::Get_IP_Config( &temp_active_ip_address );

	active_ip_address_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.ip_address ) );

	active_netmask_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.subnet_mask ) );

	active_default_gateway_owner->Check_In(
		reinterpret_cast<uint8_t*>( &temp_active_ip_address.default_gateway ) );
    
	modbus_tcp = new Modbus_Net(
		MODBUS_TCP_DEFAULT_ADDRESS,
		new Modbus_TCP(),
		&modbus_dci_data_target_info,
		new DCI_Owner( DCI_MODBUS_TCP_COMM_TIMEOUT_DCID ),
                Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE, false );

	modbus_tcp->Enable_Port( MODBUS_TCP_TX_MODE );

	/* Initialize HTTP */
	httpd_init( &rest_dci_data_target_info, reinterpret_cast<DCI_Owner*>(NULL) ); //loTool kit

}
void Load_Network_Settings( void )
{
	uint8_t ip_address_select_method_from_ethernet = IP_ADDRESS_SELECT_RESTORE;

	/* Load the static network address settings from DCID */
	static_ip_address_owner = new DCI_Owner( DCI_STATIC_IP_ADDRESS_DCID );
	static_ip_address_owner->Check_Out(
		reinterpret_cast<uint8_t*>( &active_ip_address.ip_address ) );

	static_netmask_owner = new DCI_Owner( DCI_STATIC_SUBNET_MASK_DCID );
	static_netmask_owner->Check_Out(
		reinterpret_cast<uint8_t*>( &active_ip_address.subnet_mask ) );

	static_default_gateway_owner = new DCI_Owner(
		DCI_STATIC_DEFAULT_GATEWAY_DCID );
	static_default_gateway_owner->Check_Out(
		reinterpret_cast<uint8_t*>( &active_ip_address.default_gateway ) );

	/* Load the network address settings from NV memory DCID */
	active_ip_address_owner = new DCI_Owner( DCI_ACTIVE_IP_ADDRESS_DCID );
	active_ip_address_owner->Check_Out_Init(
		reinterpret_cast<uint8_t*>( &active_ip_address.ip_address ) );

	active_netmask_owner = new DCI_Owner( DCI_ACTIVE_SUBNET_MASK_DCID );

	active_netmask_owner->Check_Out_Init(
		reinterpret_cast<uint8_t*>( &active_ip_address.subnet_mask ) );

	active_default_gateway_owner = new DCI_Owner( DCI_ACTIVE_DEFAULT_GATEWAY_DCID );

	active_default_gateway_owner->Check_Out_Init(
		reinterpret_cast<uint8_t*>( &active_ip_address.default_gateway ) );

	/* Read the IP Address Allocation method */
	ip_address_allocation_method_owner =
		new DCI_Owner( DCI_IP_ADDRESS_ALLOCATION_METHOD_DCID );

	ip_address_allocation_method_owner->Check_Out_Init(
		reinterpret_cast<uint8_t*>( &ip_address_select_method_from_ethernet ) );
}
~~~~
