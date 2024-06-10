/**
*****************************************************************************************
*   @file
*
*   @brief
*
*   @details
*
*   @version
*   C++ Style Guide Version 1.0
*
*   @copyright 2024 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/
#ifndef NV_ADDRESS_H
  #define NV_ADDRESS_H

//***************************************************************************
// NV Address and Control Masks
//***************************************************************************
typedef uint32_t nv_mem_address_t;
static const uint8_t NV_ADDRESS_CTRL_SELECT_SHIFT = 24U;
static const nv_mem_address_t NV_ADDRESS_CTRL_SELECT_MASK =
    static_cast<uint32_t>( ~( 0xFFFFFFFFU>>( ( sizeof( nv_mem_address_t ) * 8 ) - NV_ADDRESS_CTRL_SELECT_SHIFT ) ) );
static const nv_mem_address_t NV_ADDRESS_CTRL_ADDRESS_MASK = static_cast<uint32_t>( ~NV_ADDRESS_CTRL_SELECT_MASK );

static const uint16_t NV_MAX_DATA_VAL_LENGTH = 170U;



//***************************************************************************
// NV Ctrl 0 Addresses
//***************************************************************************

static const bool_t NV_DATA_CTRL0_CHECKSUM_SPACE_INCLUDED = true;

static const nv_mem_address_t NV_DATA_START_NVCTRL0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0000U );
static const nv_mem_address_t NV_CTRL_MEM_INIT_DONE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0002U );
static const nv_mem_address_t MSG_FOR_BOOT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0005U );
static const nv_mem_address_t DCI_NV_MEM_FACTORY_RESET_TRIG_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0008U );
static const nv_mem_address_t DEFENSIVE_UBERLOADER_BLOCK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x000BU );
static const nv_mem_address_t FAULTS_SORTED_QUE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0020U );
static const nv_mem_address_t FAULTS_EVENT_ORDERED_QUE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0028U );
static const nv_mem_address_t FAULTS_LATCHED_BITS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0030U );
static const nv_mem_address_t DCI_PRODUCT_SERIAL_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0034U );
static const nv_mem_address_t DCI_USER_APP_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x003AU );
static const nv_mem_address_t DCI_HARDWARE_VER_ASCII_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x005CU );
static const nv_mem_address_t DCI_HEAP_USED_MAX_PERC_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x006CU );
static const nv_mem_address_t DCI_RESET_COUNTER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x006FU );
static const nv_mem_address_t DCI_ETHERNET_MAC_ADDRESS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0073U );
static const nv_mem_address_t DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x007BU );
static const nv_mem_address_t DCI_IP_ADDRESS_ALLOCATION_METHOD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x007EU );
static const nv_mem_address_t DCI_STATIC_IP_ADDRESS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0081U );
static const nv_mem_address_t DCI_STATIC_SUBNET_MASK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0087U );
static const nv_mem_address_t DCI_STATIC_DEFAULT_GATEWAY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x008DU );
static const nv_mem_address_t DCI_PWR_BRD_DIP_SWITCHES_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0093U );
static const nv_mem_address_t DCI_PHY1_LINK_SPEED_SELECT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0097U );
static const nv_mem_address_t DCI_PHY1_FULL_DUPLEX_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x009BU );
static const nv_mem_address_t DCI_PHY1_AUTONEG_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x009EU );
static const nv_mem_address_t DCI_PHY1_PORT_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00A1U );
static const nv_mem_address_t DCI_PHY2_LINK_SPEED_SELECT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00A4U );
static const nv_mem_address_t DCI_PHY2_FULL_DUPLEX_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00A8U );
static const nv_mem_address_t DCI_PHY2_AUTONEG_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00ABU );
static const nv_mem_address_t DCI_PHY2_PORT_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00AEU );
static const nv_mem_address_t DCI_MULTICAST_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00B1U );
static const nv_mem_address_t DCI_BROADCAST_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00B7U );
static const nv_mem_address_t DCI_MODBUS_TCP_COMM_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00BDU );
static const nv_mem_address_t DCI_MODBUS_SERIAL_COMM_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00C1U );
static const nv_mem_address_t DCI_ETH_ACD_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00C5U );
static const nv_mem_address_t DCI_ETH_ACD_CONFLICTED_STATE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00C8U );
static const nv_mem_address_t DCI_ETH_ACD_CONFLICTED_MAC_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00CBU );
static const nv_mem_address_t DCI_ETH_ACD_CONFLICTED_ARP_PDU_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00D3U );
static const nv_mem_address_t DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00F1U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00F4U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00F8U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x00FCU );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0100U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0104U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_5_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0108U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_6_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x010CU );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_7_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0110U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_8_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0114U );
static const nv_mem_address_t DCI_VALID_BITS_ASSY_9_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0118U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x011CU );
static const nv_mem_address_t DCI_MEMBERS_ASSY_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0132U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0148U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x015EU );
static const nv_mem_address_t DCI_MEMBERS_ASSY_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0174U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_5_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x018AU );
static const nv_mem_address_t DCI_MEMBERS_ASSY_6_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01A0U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_7_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01B6U );
static const nv_mem_address_t DCI_MEMBERS_ASSY_8_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01CCU );
static const nv_mem_address_t DCI_MEMBERS_ASSY_9_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01E2U );
static const nv_mem_address_t DCI_EXAMPLE_MODBUS_SERIAL_BAUD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01F8U );
static const nv_mem_address_t DCI_EXAMPLE_MODBUS_SERIAL_PARITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x01FEU );
static const nv_mem_address_t DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0202U );
static const nv_mem_address_t DCI_APP_FIRM_UPGRADE_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0206U );
static const nv_mem_address_t DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x020CU );
static const nv_mem_address_t DCI_LAST_APP_FIRM_UPGRADE_USER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0212U );
static const nv_mem_address_t DCI_LAST_WEB_FIRM_UPGRADE_USER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0229U );
static const nv_mem_address_t DCI_WEB_FIRM_VER_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0240U );
static const nv_mem_address_t DCI_FW_UPGRADE_MODE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0246U );
static const nv_mem_address_t DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0249U );
static const nv_mem_address_t DCI_WEB_FIRM_UPGRADE_STATUS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x024CU );
static const nv_mem_address_t DCI_PRODUCT_PROC_SERIAL_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x024FU );
static const nv_mem_address_t DCI_USER_INACTIVITY_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0255U );
static const nv_mem_address_t DCI_VALID_BITS_USERS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0259U );
static const nv_mem_address_t DCI_USER_1_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x025DU );
static const nv_mem_address_t DCI_USER_1_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0280U );
static const nv_mem_address_t CRYPTO_NV_BLOCK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0297U );
static const nv_mem_address_t DCI_USER_1_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05A9U );
static const nv_mem_address_t DCI_USER_1_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05ACU );
static const nv_mem_address_t DCI_USER_1_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05B2U );
static const nv_mem_address_t DCI_USER_1_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05B8U );
static const nv_mem_address_t DCI_USER_1_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05BBU );
static const nv_mem_address_t DCI_USER_2_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05BFU );
static const nv_mem_address_t DCI_USER_2_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05E2U );
static const nv_mem_address_t DCI_USER_2_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05F9U );
static const nv_mem_address_t DCI_USER_2_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x05FCU );
static const nv_mem_address_t DCI_USER_2_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0602U );
static const nv_mem_address_t DCI_USER_2_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0608U );
static const nv_mem_address_t DCI_USER_2_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x060BU );
static const nv_mem_address_t DCI_USER_3_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x060FU );
static const nv_mem_address_t DCI_USER_3_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0632U );
static const nv_mem_address_t DCI_USER_3_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0649U );
static const nv_mem_address_t DCI_USER_3_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x064CU );
static const nv_mem_address_t DCI_USER_3_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0652U );
static const nv_mem_address_t DCI_USER_3_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0658U );
static const nv_mem_address_t DCI_USER_3_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x065BU );
static const nv_mem_address_t DCI_USER_4_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x065FU );
static const nv_mem_address_t DCI_USER_4_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0682U );
static const nv_mem_address_t DCI_USER_4_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0699U );
static const nv_mem_address_t DCI_USER_4_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x069CU );
static const nv_mem_address_t DCI_USER_4_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06A2U );
static const nv_mem_address_t DCI_USER_4_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06A8U );
static const nv_mem_address_t DCI_USER_4_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06ABU );
static const nv_mem_address_t DCI_USER_5_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06AFU );
static const nv_mem_address_t DCI_USER_5_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06D2U );
static const nv_mem_address_t DCI_USER_5_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06E9U );
static const nv_mem_address_t DCI_USER_5_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06ECU );
static const nv_mem_address_t DCI_USER_5_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06F2U );
static const nv_mem_address_t DCI_USER_5_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06F8U );
static const nv_mem_address_t DCI_USER_5_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06FBU );
static const nv_mem_address_t DCI_USER_6_FULLNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x06FFU );
static const nv_mem_address_t DCI_USER_6_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0722U );
static const nv_mem_address_t DCI_USER_6_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0739U );
static const nv_mem_address_t DCI_USER_6_PWD_SET_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x073CU );
static const nv_mem_address_t DCI_USER_6_LAST_LOGIN_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0742U );
static const nv_mem_address_t DCI_USER_6_PWD_COMPLEXITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0748U );
static const nv_mem_address_t DCI_USER_6_PWD_AGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x074BU );
static const nv_mem_address_t DCI_HTTP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x074FU );
static const nv_mem_address_t DCI_PRODUCT_ACTIVE_FAULT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0752U );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0756U );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x075CU );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0760U );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0763U );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_5_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0766U );
static const nv_mem_address_t DCI_SAMPLE_ACYCLIC_PARAMETER_6_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x076CU );
static const nv_mem_address_t DCI_ETH_NAME_SERVER_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0770U );
static const nv_mem_address_t DCI_ETH_NAME_SERVER_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0776U );
static const nv_mem_address_t DCI_ETH_DOMAIN_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x077CU );
static const nv_mem_address_t DCI_ETH_HOST_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x07AEU );
static const nv_mem_address_t DCI_ETH_ENCAP_INACTIVITY_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x07F0U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x07F4U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x07FAU );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0800U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0806U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_5_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x080CU );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_6_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0812U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_7_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0818U );
static const nv_mem_address_t DCI_DYN_IN_ASM_MEMBER_8_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x081EU );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0824U );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0828U );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x082CU );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0830U );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_5_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0834U );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_6_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0838U );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_7_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x083CU );
static const nv_mem_address_t DCI_DYN_IN_ASM_SELECT_8_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0840U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_MEMBER_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0844U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_MEMBER_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x084AU );
static const nv_mem_address_t DCI_DYN_OUT_ASM_MEMBER_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0850U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_MEMBER_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0856U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_SELECT_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x085CU );
static const nv_mem_address_t DCI_DYN_OUT_ASM_SELECT_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0860U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_SELECT_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0864U );
static const nv_mem_address_t DCI_DYN_OUT_ASM_SELECT_4_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0868U );
static const nv_mem_address_t DHCP_TO_STATIC_IP_ADDR_LOCK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x086CU );
static const nv_mem_address_t DCI_LTK_FAULT_QUEUE_EVENT_ORDER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x086FU );
static const nv_mem_address_t DCI_DHCP_TO_STATIC_IP_ADDR_LOCK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0885U );
static const nv_mem_address_t DCI_MAX_MASTER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0888U );
static const nv_mem_address_t DCI_MAX_INFO_FRAMES_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x088BU );
static const nv_mem_address_t DCI_BACNET_MSTP_BAUD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x088EU );
static const nv_mem_address_t DCI_BACNET_MSTP_PARITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0894U );
static const nv_mem_address_t DCI_BACNET_MSTP_STOPBIT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0898U );
static const nv_mem_address_t DCI_ANALOG_VALUE_0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x089CU );
static const nv_mem_address_t DCI_ANALOG_VALUE_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08A0U );
static const nv_mem_address_t DCI_AV_ANY_PARAM_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08A4U );
static const nv_mem_address_t DCI_AV_ANY_PARAM_VAL_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08A8U );
static const nv_mem_address_t DCI_DIGITAL_OUTPUT_WORD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08AEU );
static const nv_mem_address_t DCI_DIGITAL_OUTPUT_BIT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08B2U );
static const nv_mem_address_t DCI_MULTI_STATE_0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08B5U );
static const nv_mem_address_t DCI_MULTI_STATE_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08B8U );
static const nv_mem_address_t DCI_BACNET_IP_UPD_PORT_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08BBU );
static const nv_mem_address_t DCI_BACNET_IP_FOREIGN_DEVICE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08BFU );
static const nv_mem_address_t DCI_BACNET_IP_BBMD_IP_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08C2U );
static const nv_mem_address_t DCI_BACNET_IP_BBMD_PORT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08C8U );
static const nv_mem_address_t DCI_BACNET_IP_REGISTRATION_INTERVAL_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08CCU );
static const nv_mem_address_t DCI_BACNET_IP_COMM_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08D0U );
static const nv_mem_address_t DCI_BACNET_IP_FAULT_BEHAVIOR_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08D4U );
static const nv_mem_address_t DCI_ENABLE_BACNET_IP_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08D7U );
static const nv_mem_address_t DCI_BACNET_DEVICE_ADDR_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08DAU );
static const nv_mem_address_t DCI_BACNET_IP_INSTANCE_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08DDU );
static const nv_mem_address_t DCI_BACNET_MSTP_INSTANCE_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08E3U );
static const nv_mem_address_t DCI_BACNET_MSTP_COMM_TIMEOUT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08E9U );
static const nv_mem_address_t DCI_TRUSTED_IP_WHITELIST_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08EDU );
static const nv_mem_address_t DCI_TRUSTED_IP_MODBUS_TCP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08FBU );
static const nv_mem_address_t DCI_TRUSTED_IP_BACNET_IP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x08FEU );
static const nv_mem_address_t DCI_TRUSTED_IP_EIP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0901U );
static const nv_mem_address_t DCI_TIME_OFFSET_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0904U );
static const nv_mem_address_t DCI_DATE_FORMAT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0908U );
static const nv_mem_address_t DCI_TIME_FORMAT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x090BU );
static const nv_mem_address_t DCI_DEVICE_CERT_VALID_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x090EU );
static const nv_mem_address_t DCI_DEVICE_CERT_CONTROL_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0911U );
static const nv_mem_address_t DCI_FAULT_CATCHER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0914U );
static const nv_mem_address_t DCI_SNTP_SERVICE_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x096EU );
static const nv_mem_address_t DCI_SNTP_SERVER_1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0971U );
static const nv_mem_address_t DCI_SNTP_SERVER_2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x099BU );
static const nv_mem_address_t DCI_SNTP_SERVER_3_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09C5U );
static const nv_mem_address_t DCI_SNTP_UPDATE_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09EFU );
static const nv_mem_address_t DCI_SNTP_RETRY_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09F5U );
static const nv_mem_address_t DCI_IOT_CONNECT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09F9U );
static const nv_mem_address_t DCI_IOT_STATUS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09FCU );
static const nv_mem_address_t DCI_IOT_PROXY_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x09FFU );
static const nv_mem_address_t DCI_IOT_PROXY_SERVER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A02U );
static const nv_mem_address_t DCI_IOT_PROXY_PORT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A18U );
static const nv_mem_address_t DCI_IOT_PROXY_USERNAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A1CU );
static const nv_mem_address_t DCI_IOT_DEVICE_GUID_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A33U );
static const nv_mem_address_t DCI_IOT_DEVICE_PROFILE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A5AU );
static const nv_mem_address_t DCI_IOT_UPDATE_RATE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A81U );
static const nv_mem_address_t DCI_IOT_CADENCE_UPDATE_RATE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A84U );
static const nv_mem_address_t DCI_GROUP0_CADENCE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A88U );
static const nv_mem_address_t DCI_GROUP1_CADENCE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A8EU );
static const nv_mem_address_t DCI_GROUP2_CADENCE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A94U );
static const nv_mem_address_t DCI_LOG_INTERVAL_TIME_MS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0A9AU );
static const nv_mem_address_t DCI_CHANGE_BASED_LOGGING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AA0U );
static const nv_mem_address_t DCI_BACNET_DATABASE_REVISION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AA3U );
static const nv_mem_address_t DCI_BLE_USER1_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AA9U );
static const nv_mem_address_t DCI_BLE_USER1_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AACU );
static const nv_mem_address_t DCI_BLE_USER2_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0ACEU );
static const nv_mem_address_t DCI_BLE_USER2_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AD1U );
static const nv_mem_address_t DCI_BLE_USER3_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AF3U );
static const nv_mem_address_t DCI_BLE_USER3_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0AF6U );
static const nv_mem_address_t DCI_BLE_USER4_ROLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B18U );
static const nv_mem_address_t DCI_MODBUS_TCP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B1BU );
static const nv_mem_address_t DCI_BACNET_IP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B1EU );
static const nv_mem_address_t DCI_RESET_LINE_NUMBER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B21U );
static const nv_mem_address_t DCI_RESET_FUNCTION_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B27U );
static const nv_mem_address_t DCI_RESET_OS_TASK_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B3DU );
static const nv_mem_address_t DCI_RESET_OS_TASK_PRIORITY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B53U );
static const nv_mem_address_t DCI_RESET_OS_STACK_SIZE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B59U );
static const nv_mem_address_t DCI_RESET_CR_TASK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B5FU );
static const nv_mem_address_t DCI_RESET_CR_TASK_PARAM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B65U );
static const nv_mem_address_t DCI_RESET_CSTACK_SIZE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B6BU );
static const nv_mem_address_t DCI_RESET_HIGHEST_OS_STACK_SIZE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B71U );
static const nv_mem_address_t DCI_RESET_HIGHEST_OS_STACK_SIZE_TASK_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B77U );
static const nv_mem_address_t DCI_RESET_HEAP_USAGE_PERCENT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B8DU );
static const nv_mem_address_t DCI_RESET_SOURCE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B93U );
static const nv_mem_address_t DCI_ENABLE_SESSION_TAKEOVER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B99U );
static const nv_mem_address_t DCI_MAX_CONCURRENT_SESSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B9CU );
static const nv_mem_address_t DCI_ABSOLUTE_TIMEOUT_SEC_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0B9FU );
static const nv_mem_address_t DCI_USER_LOCK_TIME_SEC_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BA5U );
static const nv_mem_address_t DCI_MAX_FAILED_LOGIN_ATTEMPTS_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BA9U );
static const nv_mem_address_t DCI_CORS_TYPE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BADU );
static const nv_mem_address_t DCI_MDNS_LWM2M_SERVICE_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BB0U );
static const nv_mem_address_t DCI_MDNS_SERVER1_NAME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BB3U );
static const nv_mem_address_t DCI_DEVICE_PWD_SETTING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BF5U );
static const nv_mem_address_t DCI_LANG_PREF_SETTING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BF8U );
static const nv_mem_address_t DCI_COMMON_LANG_PREF_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0BFBU );
static const nv_mem_address_t DCI_LANG_FIRM_VER_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C03U );
static const nv_mem_address_t DCI_LAST_LANG_FIRM_UPGRADE_USER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C09U );
static const nv_mem_address_t DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C20U );
static const nv_mem_address_t DCI_PTP_ENABLE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C26U );
static const nv_mem_address_t DCI_GROUP3_CADENCE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C29U );
static const nv_mem_address_t DCI_DATA_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C2FU );
static const nv_mem_address_t DCI_EVENT_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C32U );
static const nv_mem_address_t DCI_AUDIT_POWER_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C35U );
static const nv_mem_address_t DCI_AUDIT_FW_UPGRADE_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C38U );
static const nv_mem_address_t DCI_AUDIT_USER_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C3BU );
static const nv_mem_address_t DCI_AUDIT_CONFIG_LOG_SPEC_VERSION_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C3EU );
static const nv_mem_address_t DCI_NV_MAP_VER_NUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C41U );
static const nv_mem_address_t DCI_IOT_DPS_ENDPOINT_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C45U );
static const nv_mem_address_t DCI_IOT_DPS_ID_SCOPE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0C97U );
static const nv_mem_address_t DCI_IOT_DPS_DEVICE_REG_ID_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0CADU );
static const nv_mem_address_t NV_DATA_END_NVCTRL0_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 0<<24 ) | 0x0CD4U );

static const nv_mem_address_t NV_DATA_CTRL0_RANGE_SIZE = ( NV_DATA_END_NVCTRL0_NVADD - NV_DATA_START_NVCTRL0_NVADD );


//***************************************************************************
// NV Ctrl 1 Addresses
//***************************************************************************

static const bool_t NV_DATA_CTRL1_CHECKSUM_SPACE_INCLUDED = false;

static const nv_mem_address_t NV_DATA_START_NVCTRL1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0000U );
static const nv_mem_address_t NV_CTRL_MEM_INIT_DONE_C1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0002U );
static const nv_mem_address_t DCI_USER_1_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0003U );
static const nv_mem_address_t DCI_USER_2_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0018U );
static const nv_mem_address_t DCI_USER_3_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x002DU );
static const nv_mem_address_t DCI_USER_4_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0042U );
static const nv_mem_address_t DCI_USER_5_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0057U );
static const nv_mem_address_t DCI_USER_6_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x006CU );
static const nv_mem_address_t DCI_IOT_PROXY_PASSWORD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0081U );
static const nv_mem_address_t DCI_IOT_CONN_STRING_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0096U );
static const nv_mem_address_t DCI_BLE_USER1_PASSWORD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0140U );
static const nv_mem_address_t DCI_BLE_USER2_PASSWORD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x015CU );
static const nv_mem_address_t DCI_IOT_DPS_SYMM_KEY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0178U );
static const nv_mem_address_t ANOTHER_FILLER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x01A5U );
static const nv_mem_address_t DCI_DEVICE_DEFAULT_PWD_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x01AFU );
static const nv_mem_address_t DCI_CLIENT_AP_SSID_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x01C4U );
static const nv_mem_address_t DCI_CLIENT_AP_PASSPHRASE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x01E4U );
static const nv_mem_address_t DCI_SOURCE_AP_SSID_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0204U );
static const nv_mem_address_t DCI_SOURCE_AP_PASSPHRASE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0224U );
static const nv_mem_address_t DCI_IOT_UNICAST_PRIMARY_UDP_KEY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0245U );
static const nv_mem_address_t DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0275U );
static const nv_mem_address_t DCI_IOT_UNICAST_SECONDARY_UDP_KEY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x02A5U );
static const nv_mem_address_t DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x02D5U );
static const nv_mem_address_t NV_DATA_END_NVCTRL1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0305U );
static const nv_mem_address_t FILLER_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0306U );
static const nv_mem_address_t DATA_SIZE_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x030AU );
static const nv_mem_address_t CRYPT_CHECKSUM_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x030EU );
static const nv_mem_address_t NV_CRYPTO_META_DATA_END_NVCTRL1_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 1<<24 ) | 0x0310U );

static const nv_mem_address_t NV_DATA_CTRL1_RANGE_SIZE = ( NV_DATA_END_NVCTRL1_NVADD - NV_DATA_START_NVCTRL1_NVADD );


//***************************************************************************
// NV Ctrl 2 Addresses
//***************************************************************************

static const bool_t NV_DATA_CTRL2_CHECKSUM_SPACE_INCLUDED = false;

static const nv_mem_address_t NV_DATA_START_NVCTRL2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 2<<24 ) | 0x0000U );
static const nv_mem_address_t NV_CTRL_MEM_INIT_DONE_C2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 2<<24 ) | 0x0002U );
static const nv_mem_address_t NV_DATA_END_NVCTRL2_NVADD = static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( 2<<24 ) | 0x0003U );

static const nv_mem_address_t NV_DATA_CTRL2_RANGE_SIZE = ( NV_DATA_END_NVCTRL2_NVADD - NV_DATA_START_NVCTRL2_NVADD );

#endif
