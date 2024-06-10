/*
 ********************************************************************************
 *		$Workfile: KSZ8863_Reg.h$
 *
 *		$Author: Pranav Kodre$
 *		$Date: 09/27/2012 02:16:00 PM$
 *		Copyright© 2013-2014, Eaton Corporation. All Rights Reserved.
 *		Micrel KSZ8863 related definitions
 ********************************************************************************
 */

#ifndef KSZ8863_Reg_H
 #define KSZ8863_Reg_H

// #include "REDBALL_DEFINE.h" //No Need as definition is added in following line only
#define REDBALL_CARD_KSZ8863
#ifdef  REDBALL_CARD_KSZ8863	// #k: New for RedBall card    EDAP_CARD_LAN9303

#ifdef __cplusplus
extern "C" {
#endif

  #define PHY_ADDR_SEL_STRAP          1

enum port_phy_addr
{
	PHY1_ADDR = PHY_ADDR_SEL_STRAP,							// #k: 1
	PHY2_ADDR,
	VPHY_ADDR
};

   #define KSZ_PHYID_MSB                                   0x0022
   #define KSZ_PHYID_LSB                                   0x1430

/* serially addressable registers
   common for PHY1 and PHY2*/

	#define PHY_BASIC_CONTROL_OFFSET                            0
	#define PHY_BASIC_STATUS_OFFSET                             1
	#define PHY_SPECIAL_CONTROL_STATUS_OFFSET                   31

	#define PHY_MII_BASIC_CONTROL                          0
	#define PHY_BASIC_CONTROL_SOFTRESET                    0x0000U
	#define PHY_BASIC_CONTROL_LOOPBACK                     0x4000U
	#define PHY_BASIC_CONTROL_100MBPS                      0x2000U
	#define PHY_BASIC_CONTROL_AN_ENABLED                   0x1000U
	#define PHY_BASIC_CONTROL_POWERDOWN                    0x0800U
	#define PHY_BASIC_CONTROL_RESTART_AUTONEGO             0x0200U
	#define PHY_BASIC_CONTROL_FULL_DUPLEX                  0x0100U
	#define PHY_BASIC_CONTROL_HPAUTO_MDIX_MODE             0x0020U
	#define PHY_BASIC_CONTROL_FORCE_MDI                    0x0010U
	#define PHY_BASIC_CONTROL_DISABLE_MDIX                 0x0008U
	#define PHY_BASIC_CONTROL_DISABLE_FARENDFAULT          0x0004U
	#define PHY_BASIC_CONTROL_DISABLE_TX                   0x0002U
	#define PHY_BASIC_CONTROL_DISABLE_LED                  0x0001U

	#define PHY_MII_BASIC_STATUS                           1

	#define PHY_BASIC_STATUS_T4_CAPABLE                    0x8000U
	#define PHY_BASIC_STATUS_100FULL_CAPABLE               0x4000U
	#define PHY_BASIC_STATUS_100HALF_CAPABLE               0x2000U
	#define PHY_BASIC_STATUS_10FULL_CAPABLE                0x1000U
	#define PHY_BASIC_STATUS_10HALF_CAPABLE                0x0800U
	#define PHY_BASIC_STATUS_AUTONEGO_COMPLETE             0x0200U
	#define PHY_BASIC_STATUS_FULL_FARENDFAULT_DETECT       0x0100U
	#define PHY_BASIC_STATUS_AUTONEGO_CAPABLE              0x0080U
	#define PHY_BASIC_STATUS_LINKUP                        0x0004U
	#define PHY_BASIC_STATUS_NOT_EXTENDED_REG_CAPABLE      0x0000U

// ------------------------------------------------------------------------------

// #define PHY_ID_LSB_PHY_ID_MASK                          0xFC00U
// #define PHY_ID_LSB_MODEL_NUMBER_MASK                    0x03F0U
// #define PHY_ID_LSB_REVISION_NUMBER_MASK                 0x000FU

	#define PHY_AN_ADV                                      4
	#define PHY_AN_ADV_NEXT_PAGE_VPHY                       0x8000U

// #define PHY_AN_ADV_REMOTE_FAULT                   0x2000U //Not Supported

// #define PHY_AN_ADV_ASYMMETRIC_PAUSE                     0x0800U
	#define PHY_AN_ADV_SYMMETRIC_PAUSE                      0x0400U
// #define PHY_AN_ADV_100BASE_T4                     0x0200U //Not Supported

	#define PHY_AN_ADV_100BASE_X_FULL_DUPLEX                0x0100U
	#define PHY_AN_ADV_100BASE_X_HALF_DUPLEX                0x0080U
	#define PHY_AN_ADV_10BASE_T_FULL_DUPLEX                 0x0040U
	#define PHY_AN_ADV_10BASE_T_HALF_DUPLEX                 0x0020U
	#define PHY_AN_ADV_SELECTOR_FIELD_MASK                  0x001FU

	#define PHY_AN_LP_BASE_ABILITY                          5
// #define PHY_AN_LP_BASE_ABILITY_NEXT_PAGE                0x8000U
// #define PHY_AN_LP_BASE_ABILITY_ACKNOWLEDGE              0x4000U
// #define PHY_AN_LP_BASE_ABILITY_REMOTE_FAULT             0x2000U
// #define PHY_AN_LP_BASE_ABILITY_ASYMMETRIC_PAUSE         0x0800U
	#define PHY_AN_LP_PAUSE_CAPABILITY                      0x0400U
// #define PHY_AN_LP_BASE_ABILITY_100BASE_T4             0x0200U
	#define PHY_AN_ADV_LP_100_FULL                          0x0100U
	#define PHY_AN_ADV_LP_100_HALF                          0x0080U
	#define PHY_AN_ADV_LP_10_FULL                           0x0040U
	#define PHY_AN_ADV_LP_10_HALF                           0x0020U
// #define PHY_AN_LP_BASE_ABILITY_SELECTOR_FIELD_MASK    0x001FU

#define PHY2_LINKMD_CONTROL_STATUS       29
   #define PHY2_LINKMD_CONTROL_STATUS_VCT_ENABLE            0x8000U
   #define PHY2_LINKMD_CONTROL_STATUS_VCT_RESULT            0x6000U
   #define PHY2_LINKMD_CONTROL_STATUS_VCT_10METER_SHORT     0x1000U
   #define PHY2_LINKMD_CONTROL_STATUS_VCT_FAULT_COUNT       0x01FFU

#define PHY_SPECIAL_CONTROL_STATUS       31
   #define PHY_SPECIAL_CONTROL_STATUS_POLARITY_REVERSED     0x0020U
   #define PHY_SPECIAL_CONTROL_STATUS_MDIX                  0x0010U
   #define PHY_SPECIAL_CONTROL_STATUS_FORCELINKPASS         0x0008U
   #define PHY_SPECIAL_CONTROL_STATUS_POWERSAVING_EN_DS     0x0004U
   #define PHY_SPECIAL_CONTROL_STATUS_REMOTE_LOOPBACK       0x0002U


// #k:---------------------------- START: KSZ8863--------------------------------

#define REG121_KSZ_INDIRECT_ACCESS_CONTROL_0                   121
#define KSZ_INDIRECT_ACCESS_RD_OPERATION                0x10
#define KSZ_INDIRECT_ACCESS_WR_OPERATION                0xEF
#define KSZ_INDIRECT_ACCESS_STATIC_TABLE_SELECT         0xF0
#define KSZ_INDIRECT_ACCESS_VLAN_TABLE_SELECT           0x01
#define KSZ_INDIRECT_ACCESS_DYNAMIC_TABLE_SELECT        0x02
#define KSZ_INDIRECT_ACCESS_MIB_COUNTER_SELECT          0x03

#define REG122_KSZ_INDIRECT_ACCESS_CONTROL_1                   122

#define KSZ_INDIRECT_DATA_REG_8                         123
#define KSZ_INDIRECT_DATA_REG_7                         124
#define KSZ_INDIRECT_DATA_REG_6                         125
#define KSZ_INDIRECT_DATA_REG_5                         126
#define KSZ_INDIRECT_DATA_REG_4                         127
#define KSZ_INDIRECT_DATA_REG_3                         128
#define KSZ_INDIRECT_DATA_REG_2                         129
#define KSZ_INDIRECT_DATA_REG_1                         130
#define KSZ_INDIRECT_DATA_REG_0                         131

#define KSZ_STATIC_TABLE_FORWARDPORTS_1                 0x00010000UL
#define KSZ_STATIC_TABLE_VALIDENTRY                     0x00080000UL
#define KSZ_STATIC_TABLE_OVERRIDE                       0x00100000UL
#define KSZ_STATIC_TABLE_USE_FID_MAC                    0x00200000UL
#define KSZ_STATIC_TABLE_FID_VALUE                      0x03C00000UL

// #k:---------------------------- END: KSZ8863----------------------------------

#define KSZ_R0_GBL_CNTL_0_CHIPID              0

#define KSZ8863_FAMILY_ID                             0x88
#define KSZ8863_CHIP_ID                               0x31

#define KSZ_R1_GBL_CNTL_1_CHIPID1             1
  #define GBL_REG_CHIP_ID                             0x31
  #define GBL_REG_START_SWITCH                        0x01


#define KSZ_R2_GBL_CTRL0                      2
  #define GBL_CTRL0_NEW_BACKOFF_ENABLE                0x80
  #define GBL_CTRL0_FLUSH_DYNAMIC_MAC_TABLE           0x20
  #define GBL_CTRL0_FLUSH_STATIC_MAC_TABLE            0x10
  #define GBL_CTRL0_PASS_FLOW_CONTROL_PACKET          0x08

#define KSZ_R3_GBL_CTRL1                      3
  #define GBL_CTRL1_PASS_ALL_PACKETS                  0x80
  #define GBL_CTRL1_PORT3_TAILTAG_ENABLE              0x40
  #define GBL_CTRL1_TX_DIR_FLOW_CONTROL_ENABLE        0x20
  #define GBL_CTRL1_RX_DIR_FLOW_CONTROL_ENABLE        0x10
  #define GBL_CTRL1_FRAME_LENGTH_FIELD_CHECK          0x08
  #define GBL_CTRL1_AGING_ENABLE                      0x04
  #define GBL_CTRL1_FAST_AGE_ENABLE                   0x02
  #define GBL_CTRL1_AGGRESSIVE_BACK_OGG_ENABLE        0x01

#define KSZ_R4_GBL_CTRL2                      4
  #define GBL_CTRL2_UNICAST_PORT_VLAN_MISMATCH_DISCARD 0x80
  #define GBL_CTRL2_MULTICAST_STORM_PROTECT_DISABLE    0x40
  #define GBL_CTRL2_BACK_PRESSURE_MODE                 0x20
  #define GBL_CTRL2_FLOW_CTRL_BACK_PRESSURE_FAIR_MODE  0x10
  #define GBL_CTRL2_NO_EXCESSIVE_COLLISION_DROP        0x08
  #define GBL_CTRL2_HUGE_PACKET_SUPPORT                0x04
  #define GBL_CTRL2_LEGEL_MAX_PACKET_SIZE              0x02
  #define GBL_CTRL2_RESERVED                           0x01

#define KSZ_R5_GBL_CTRL3                      5
  #define GBL_CTRL3_VLAN_ENABLE                        0x80
  #define GBL_CTRL3_IGMP_ENABLE                        0x40
  #define GBL_CTRL3_RESERVED1                          0x20
  #define GBL_CTRL3_RESERVED2                          0x10
  #define GBL_CTRL3_WEIGHTED_FAIR_Q_ENABLE             0x08
  #define GBL_CTRL3_RESERVED3                          0x04
  #define GBL_CTRL3_RESERVED4                          0x02
  #define GBL_CTRL3_SNIFF_MODE_SELECT                  0x01

#define KSZ_R6_GBL_CTRL4                     6
  #define GBL_CTRL4_RESERVED1                          0x80
  #define GBL_CTRL4_MII_HALF_DUPLEX_MODE_ENABLE        0x40
  #define GBL_CTRL4_MII_FULL_DUPLEX_FLOW_CTRL_ENABLE   0x20
  #define GBL_CTRL4_MII_10_100_MODE                    0x10
  #define GBL_CTRL4_REPLACE_NULL_VID                   0x08
  #define GBL_CTRL4_BROADCAST_STORM_PROTECT            0x07

#define KSZ_R7_GBL_CTRL5                     7
   #define GBL_CTRL5_BROADCAST_STORM_PROTECT_RATE           0xFF
   #define GBL_CTRL5_BROADCAST_STORM_PROTECT_RATE_DEFAULT   0x63

#define KSZ_R8_GBL_CTRL6                     8
  #define GBL_CTRL6_FACTORY_TEST_DEFAULT                    0x00

#define KSZ_R9_GBL_CTRL7                     9
  #define GBL_CTRL7_FACTORY_TEST_DEFAULT                    0x24


#define KSZ_R10_GBL_CTRL8                     10
	 #define GBL_CTRL8_FACTORY_TEST_DEFAULT                 0x35

#define KSZ_R11_GBL_CTRL9                     11
	 #define GBL_CTRL9_CPU_INTERFACE_CLOCK_SPEED            0xC0
	 #define GBL_CTRL9_RESERVED1                            0x30
	 #define GBL_CTRL9_RESERVED2                            0x0C
	 #define GBL_CTRL9_RESERVED3                            0x02
	 #define GBL_CTRL9_RESERVED4                            0x01

#define KSZ_R12_GBL_CTRL10                    12
	 #define GBL_CTRL10_TAG0X3                              0xC0
	 #define GBL_CTRL10_TAG0X2                              0x30
	 #define GBL_CTRL10_TAG0X1                              0x0C
	 #define GBL_CTRL10_TAG0X0                              0x03


#define KSZ_R13_GBL_CTRL11                    13
	 #define GBL_CTRL11_TAG0X7                              0xC0
	 #define GBL_CTRL11_TAG0X6                              0x30
	 #define GBL_CTRL11_TAG0X5                              0x0C
	 #define GBL_CTRL11_TAG0X4                              0x03

#define KSZ_R14_GBL_CTRL12                    14
	 #define GBL_CTRL12_UNKNOWN_PACKET_DEFAULTPORT_ENABLE   0x80
	 #define GBL_CTRL12_DRIVE_STRENGTH_IO_PAD               0x40
	 #define GBL_CTRL12_RESERVED1                           0x20
	 #define GBL_CTRL12_RESERVED2                           0x10
	 #define GBL_CTRL12_RESERVED3                           0x08
	 #define GBL_CTRL12_UNKNOWN_PACKET_DEFAULTPORT          0x07
	 #define GBL_CTRL12_UNKNOWN_PACKET_DEFAULTPORT_DEFAULT  0x07

	 #define KSZ_R15_GBL_CTRL13                15
	 #define GBL_CTRL13_PHY_ADDRESS                         0xF1
	 #define GBL_CTRL13_RESERVED                            0x07

#define KSZ_R16_PORT1_CTRL0                    16
#define KSZ_R32_PORT2_CTRL0                    32
#define KSZ_R48_PORT3_CTRL0                    48

#define PORT_CTRL0_BROADCAST_STORM_PROTECT_ENABLE           0x80
#define PORT_CTRL0_DIFFSERV_CLASSIFICATION_ENABLE           0x40
#define PORT_CTRL0_802_1P_CLASSIFICATION_ENABLE             0x20
#define PORT_CTRL0_PORT_PRIORITY_CLASSIFICATION             0x18
#define PORT_CTRL0_TAG_INSERT                               0x04
#define PORT_CTRL0_TAG_REMOVE                               0x02
#define PORT_CTRL0_TXQ_SPLIT_ENABLE                         0x01

#define KSZ_R17_PORT1_CTRL1                    17
#define KSZ_R33_PORT2_CTRL1                    33
#define KSZ_R49_PORT3_CTRL1                    49
#define PORT_CTRL1_SNIFFERPORT_ENABLE                       0x80
#define PORT_CTRL1_RX_SNIFF_MARK                            0x40
#define PORT_CTRL1_TX_SNIFF_MARK                            0x20
#define PORT_CTRL1_USER_PRIORITY_CEILING                    0x10
#define PORT_CTRL1_VLAN_MEMBERSHIP                          0x08
#define PORT_CTRL1_VLAN_MEMBERSHIP_DEFAULT                  0x07

#define KSZ_R18_PORT1_CTRL2                    18
#define KSZ_R34_PORT2_CTRL2                    34
#define KSZ_R50_PORT3_CTRL2                    50
#define PORT3_CTRL2_QUEUE_SPLIT_ENABLE                      0x80
#define PORT3_CTRL2_INGRESS_VLAN_FILTER_ENABLE              0x40
#define PORT3_CTRL2_DISCARD_NON_PVID_PACKETS                0x20
#define PORT3_CTRL2_FORCE_FLOW_CONTROL                      0x10
#define PORT3_CTRL2_BACK_PRESSURE_ENABLE                    0x08
#define PORT3_CTRL2_TX_ENABLE                               0x04
#define PORT3_CTRL2_RX_ENABLE                               0x02
#define PORT3_CTRL2_LEARNING_DISABLE                        0x01

#define KSZ_R19_PORT1_CTRL3                    19
#define KSZ_R35_PORT2_CTRL3                    35
#define KSZ_R51_PORT3_CTRL3                    51

#define KSZ_R20_PORT1_CTRL4                    20
#define KSZ_R36_PORT2_CTRL4                    36
#define KSZ_R52_PORT3_CTRL4                    52

#define KSZ_R21_PORT1_CTRL5                    21
#define KSZ_R37_PORT2_CTRL5                    37
#define KSZ_R53_PORT3_CTRL5                    53
#define PORT3_CTRL5_MII_MAC_PHY_MODE                        0x80
#define PORT3_CTRL5_PORT1_SELF_ADRS_FILTER_ENABLE           0x40
#define PORT3_CTRL5_PORT2_SELF_ADRS_FILTER_ENABLE           0x20
#define PORT3_CTRL5_DROP_INGRESS_FRAME                      0x10
#define PORT3_CTRL5_INGRESS_LIMIT_MODE                      0x0C
#define PORT3_CTRL5_COUNT_IFG                               0x02
#define PORT3_CTRL5_COUNT_PREAMBLE                          0x01


#define KSZ_R22_PORT1_Q0_INGRESS_DATA_RATE_LIMIT                    22
#define KSZ_R38_PORT2_Q0_INGRESS_DATA_RATE_LIMIT                    38
#define KSZ_R54_PORT3_Q0_INGRESS_DATA_RATE_LIMIT                    54

#define PORT_Q0_INGRESS_RMII_REFCLK_SELECT                  0x80
#define PORT_Q0_INGRESS_DATA_RATE_LIMIT_MASK                0x7F

#define KSZ_R23_PORT1_Q1_INGRESS_DATA_RATE_LIMIT            23
#define KSZ_R39_PORT2_Q1_INGRESS_DATA_RATE_LIMIT            39
#define KSZ_R55_PORT3_Q1_INGRESS_DATA_RATE_LIMIT            55

#define PORT_Q1_INGRESS_DATA_RATE_LIMIT_MASK                0x7F

#define KSZ_R24_PORT1_Q2_INGRESS_DATA_RATE_LIMIT            24
#define KSZ_R40_PORT2_Q2_INGRESS_DATA_RATE_LIMIT            40
#define KSZ_R56_PORT3_Q2_INGRESS_DATA_RATE_LIMIT            56

#define PORT_Q2_INGRESS_DATA_RATE_LIMIT_MASK                0x7F

#define KSZ_R25_PORT1_Q3_INGRESS_DATA_RATE_LIMIT                    25
#define KSZ_R41_PORT2_Q3_INGRESS_DATA_RATE_LIMIT                    41
#define KSZ_R57_PORT3_Q3_INGRESS_DATA_RATE_LIMIT                    57

#define KSZ_R7F_PORT_Q3_INGRESS_DATA_RATE_LIMIT_MASK                0x7F

#define KSZ_R26_PORT1_PHY_SPECIAL_CONTROL_STATUS                    26
#define KSZ_R42_PORT2_PHY_SPECIAL_CONTROL_STATUS                    42
#define PORT_PHY_SPECIAL_VCT_10METER                        0x80
#define PORT_PHY_VCT_RESULT_MASK                            0x60
#define PORT_PHY_VCT_ENABLE                                 0x10
#define PORT_PHY_FORCE_LINK                                 0x08
#define PORT_PHY_REMOTE_LOOPBACK                            0x02
#define PORT_PHY_VCT_FAULTCOUNT                             0x01

#define KSZ_R43_PORT2_LINKMD_RESULT            43

#define KSZ_R28_PORT1_CONTROL12                28
#define KSZ_R44_PORT2_CONTROL12                44
// NOT APPLICABLE FOR PORT-3
#define PORT_CTRL12_AUTO_NEGO_ENABLE                       0x80
#define PORT_CTRL12_FORCE_SPEED_100                        0x40
#define PORT_CTRL12_FORCE_DUPLEX                           0x20
#define PORT_CTRL12_ADVER_FLOW_CONTROL                     0x10
#define PORT_CTRL12_ADVER_100_FULLDUPLEX                   0x08
#define PORT_CTRL12_ADVER_100_HALFDUPLEX                   0x04
#define PORT_CTRL12_ADVER_10_FULLDUPLEX                    0x02
#define PORT_CTRL12_ADVER_10_HALFDUPLEX                    0x01

#define KSZ_R29_PORT1_CONTROL13                 29
#define KSZ_R45_PORT2_CONTROL13                 45
// NOT APPLICABLE FOR PORT-3
#define PORT_CTRL13_TURN_OFF_LED                            0x80
#define PORT_CTRL13_PORT_TX_DISABLE                         0x40
#define PORT_CTRL13_RESTART_AUTO_NEGO                       0x20
#define PORT_CTRL13_FAREND_FAULT_DISABLE                    0x10
#define PORT_CTRL13_POWER_DOWN                              0x08
#define PORT_CTRL13_AUTO_MDI_X_DISABLE                      0x04
#define PORT_CTRL13_FORCE_MDI                               0x02
#define PORT_CTRL13_LOOPBACK                                0x01

#define KSZ_R30_PORT1_STATUS0                    30
#define KSZ_R46_PORT2_STATUS0                    46			// READ ONLY
// NOT APPLICABLE FOR PORT-3
#define PORT_STATUS0_MDIX                                   0x80
#define PORT_STATUS0_AUTO_NEGO_DONE                         0x40
#define PORT_STATUS0_LINK_GOOD                              0x20
#define PORT_STATUS0_FLOWCTRL_CAPABLE                       0x10
#define PORT_STATUS0_100_FULL_DUPLEX_CAPABLE                0x08
#define PORT_STATUS0_100_HALF_DUPLEX_CAPABLE                0x04
#define PORT_STATUS0_10_FULL_DUPLEX_CAPABLE                 0x02
#define PORT_STATUS0_10_HALF_DUPLEX_CAPABLE                 0x01

#define KSZ_R31_PORT1_STATUS1                    31
#define KSZ_R47_PORT2_STATUS1                    47			// READ ONLY
#define KSZ_R63_PORT3_STATUS1                    63

#define PORT_STATUS1_HPMDIX                             0x80
// #define PORT_STATUS1_AUTO_NEGO_DONE                   0x40
#define PORT_STATUS1_REVERSE_POLARITY                   0x20
#define PORT_STATUS1_TX_FLOWCTRL_ENABLE                 0x10
#define PORT_STATUS1_RX_FLOWCTRL_ENABLE                 0x08
#define PORT_STATUS1_OPERATION_SPEED                    0x04
#define PORT_STATUS1_OPERATION_DUPLEX                   0x02
#define PORT_STATUS1_FAREND_FAULT                       0x01

#define KSZ_R67_KSZ_RESET                        67
#define KSZ_RESET_SOFT                                  0x10
#define KSZ_RESET_PCS                                   0x01

#define KSZ_R121_INDIRECT_ACCESS_CONTROL_0         121
#define KSZ_INDIRECT_ACCESS_RD_OPERATION                0x10
#define KSZ_INDIRECT_ACCESS_WR_OPERATION                0xEF
#define KSZ_INDIRECT_ACCESS_STATIC_TABLE_SELECT         0xF0
#define KSZ_INDIRECT_ACCESS_VLAN_TABLE_SELECT           0x01
#define KSZ_INDIRECT_ACCESS_DYNAMIC_TABLE_SELECT        0x02
#define KSZ_INDIRECT_ACCESS_MIB_COUNTER_SELECT          0x03

#define KSZ_R122_INDIRECT_ACCESS_CONTROL_1                   122

#define KSZ_R123_INDIRECT_DATA_REG_8                         123
#define KSZ_R124_INDIRECT_DATA_REG_7                         124
#define KSZ_R125_INDIRECT_DATA_REG_6                         125
#define KSZ_R126_INDIRECT_DATA_REG_5                         126
#define KSZ_R127_INDIRECT_DATA_REG_4                         127
#define KSZ_R128_INDIRECT_DATA_REG_3                         128
#define KSZ_R129_INDIRECT_DATA_REG_2                         129
#define KSZ_R130_INDIRECT_DATA_REG_1                         130
#define KSZ_R131_INDIRECT_DATA_REG_0                         131


#define KSZ_STATIC_TABLE_FORWARDPORTS_1                 0x00010000UL
#define KSZ_STATIC_TABLE_VALIDENTRY                     0x00080000UL
#define KSZ_STATIC_TABLE_OVERRIDE                       0x00100000UL
#define KSZ_STATIC_TABLE_USE_FID_MAC                    0x00200000UL
#define KSZ_STATIC_TABLE_FID_VALUE                      0x03C00000UL
// ------------------------------------------------------------------------------

#define PHY_BASIC_CONTROL_PHY_RESET_SET                     0x8000
#define PHY_BASIC_CONTROL_PHY_RESET_CLEARED                 0x0000
#define PHY_BASIC_CONTROL_PHY_LOOPBACK_ENABLED              0x4000
#define PHY_BASIC_CONTROL_PHY_LOOPBACK_DISABLED             0x0000
#define PHY_BASIC_CONTROL_PHY_SPEED_SEL_LSB_100M            0x2000
#define PHY_BASIC_CONTROL_PHY_SPEED_SEL_LSB_10M             0x0000
#define PHY_BASIC_CONTROL_PHY_AN_ENABLED                    0x1000
#define PHY_BASIC_CONTROL_PHY_AN_DISABLED                   0x0000
#define PHY_BASIC_CONTROL_PHY_PWR_DWN_SET                   0x0800
#define PHY_BASIC_CONTROL_PHY_PWR_DWN_CLEARED               0x0000
#define PHY_BASIC_CONTROL_PHY_RST_AN_SET                    0x0200
#define PHY_BASIC_CONTROL_PHY_RST_AN_CLEARED                0x0000
#define PHY_BASIC_CONTROL_PHY_DUPLEX_FULL                   0x0100
#define PHY_BASIC_CONTROL_PHY_DUPLEX_HALF                   0x0000
#define PHY_BASIC_CONTROL_PHY_COL_TEST_ENABLED              0x0080
#define PHY_BASIC_CONTROL_PHY_COL_TEST_DISABLED             0x0000

// PHY_BASIC_STATUS_x
#define PHY_BASIC_STATUS_AN_COMPLETED                       0x0020
#define PHY_BASIC_STATUS_AN_ABILITY                         0x0008
#define PHY_BASIC_STATUS_LINK_UP                            0x0004

#define PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK               0x01E0


#define IS_100M( v ) \
	( ( v & PHY_AN_ADV_LP_100_HALF ) == PHY_AN_ADV_LP_100_HALF \
	  || ( v & PHY_AN_ADV_LP_100_FULL ) == PHY_AN_ADV_LP_100_FULL )

#define IS_FULL_DUPLEX( v ) \
	( ( v & PHY_AN_ADV_LP_100_FULL ) == PHY_AN_ADV_LP_100_FULL \
	  || ( v & PHY_AN_ADV_LP_10_FULL ) == PHY_AN_ADV_LP_10_FULL )

#ifdef __cplusplus
}
#endif

#endif		// REDBALL_CARD_KSZ8863

#endif

