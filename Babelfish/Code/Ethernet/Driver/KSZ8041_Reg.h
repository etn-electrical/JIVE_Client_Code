/* author: Ting Yan, August 22, 2010
   KSZ8041 PHY related definitions*/


#ifndef KSZ8041REG_H
#define KSZ8041REG_H

#ifdef __cplusplus
extern "C" {
#endif

#define PHY_ADDR_SEL_STRAP 1					/* PHYADDR_LED5P pin value at POR or nRST */

enum port_phy_addr
{
	VPHY_ADDR = PHY_ADDR_SEL_STRAP,
	PHY1_ADDR,
	PHY2_ADDR
};

// #define KSZ8041_VIRTUAL_PORT_ADDR   0

/* serially addressable registers
   common for Virtual PHY, PHY 1 and PHY2*/
#define PHY_BASIC_CONTROL                                   0
	#define PHY_BASIC_CONTROL_WRITE_MASK_PHY_X              0xFB80U
	#define PHY_BASIC_CONTROL_WRITE_MASK_VPHY               0xFFC0U
	#define PHY_BASIC_CONTROL_RST                           0x8000U
	#define PHY_BASIC_CONTROL_LOOPBACK                      0x4000U
	#define PHY_BASIC_CONTROL_SPEED_SEL_LSB                 0x2000U
	#define PHY_BASIC_CONTROL_AN                            0x1000U
	#define PHY_BASIC_CONTROL_PWR_DWN                       0x0800U
	#define PHY_BASIC_CONTROL_ISO                           0x0400U
	#define PHY_BASIC_CONTROL_RST_AN                        0x0200U
	#define PHY_BASIC_CONTROL_DUPLEX                        0x0100U
	#define PHY_BASIC_CONTROL_COL_TEST                      0x0080U
#define PHY_BASIC_STATUS                                    1
	#define PHY_BASIC_STATUS_100BASE_T4                     0x8000U
	#define PHY_BASIC_STATUS_100BASE_X_FULL_DUPLEX          0x4000U
	#define PHY_BASIC_STATUS_100BASE_X_HALF_DUPLEX          0x2000U
	#define PHY_BASIC_STATUS_10BASE_T_FULL_DUPLEX           0x1000U
	#define PHY_BASIC_STATUS_10BASE_T_HALF_DUPLEX           0x0800U
	#define PHY_BASIC_STATUS_100BASE_T2_FULL_DUPLEX         0x0400U
	#define PHY_BASIC_STATUS_100BASE_T2_HALF_DUPLEX         0x0200U
	#define PHY_BASIC_STATUS_EXTENDED_STATUS_VPHY           0x0100U
	#define PHY_BASIC_STATUS_MF_PREAMBLE_SUPPRESSION_VPHY   0x0040U
	#define PHY_BASIC_STATUS_AUTO_NEGOTIATION_COMPLETE      0x0020U
	#define PHY_BASIC_STATUS_REMOTE_FAULT                   0x0010U
	#define PHY_BASIC_STATUS_AUTO_NEGOTIATION_ABILITY       0x0008U
	#define PHY_BASIC_STATUS_LINK_STATUS                    0x0004U
	#define PHY_BASIC_STATUS_JABBER_DETECT                  0x0002U
	#define PHY_BASIC_STATUS_EXTENDED_CAPABILITY            0x0001U
#define PHY_ID_MSB                                          2
#define PHY_ID_LSB                                          3
	#define PHY_ID_LSB_PHY_ID_MASK                          0xFC00U
	#define PHY_ID_LSB_MODEL_NUMBER_MASK                    0x03F0U
	#define PHY_ID_LSB_REVISION_NUMBER_MASK                 0x000FU
#define PHY_AN_ADV                                          4
	#define PHY_AN_ADV_NEXT_PAGE_VPHY                       0x8000U
	#define PHY_AN_ADV_REMOTE_FAULT                         0x2000U
	#define PHY_AN_ADV_ASYMMETRIC_PAUSE                     0x0800U
	#define PHY_AN_ADV_SYMMETRIC_PAUSE                      0x0400U
	#define PHY_AN_ADV_100BASE_T4                           0x0200U
	#define PHY_AN_ADV_100BASE_X_FULL_DUPLEX                0x0100U
	#define PHY_AN_ADV_100BASE_X_HALF_DUPLEX                0x0080U
	#define PHY_AN_ADV_10BASE_T_FULL_DUPLEX                 0x0040U
	#define PHY_AN_ADV_10BASE_T_HALF_DUPLEX                 0x0020U
	#define PHY_AN_ADV_SELECTOR_FIELD_MASK                  0x001FU
#define PHY_AN_LP_BASE_ABILITY                              5
	#define PHY_AN_LP_BASE_ABILITY_NEXT_PAGE                0x8000U
	#define PHY_AN_LP_BASE_ABILITY_ACKNOWLEDGE              0x4000U
	#define PHY_AN_LP_BASE_ABILITY_REMOTE_FAULT             0x2000U
	#define PHY_AN_LP_BASE_ABILITY_ASYMMETRIC_PAUSE         0x0800U
	#define PHY_AN_LP_BASE_ABILITY_SYMMETRIC_PAUSE          0x0400U
	#define PHY_AN_LP_BASE_ABILITY_100BASE_T4               0x0200U
	#define PHY_AN_LP_BASE_ABILITY_100BASE_X_FULL_DUPLEX    0x0100U
	#define PHY_AN_LP_BASE_ABILITY_100BASE_X_HALF_DUPLEX    0x0080U
	#define PHY_AN_LP_BASE_ABILITY_10BASE_T_FULL_DUPLEX     0x0040U
	#define PHY_AN_LP_BASE_ABILITY_10BASE_T_HALF_DUPLEX     0x0020U
	#define PHY_AN_LP_BASE_ABILITY_SELECTOR_FIELD_MASK      0x001FU
#define PHY_AN_EXP                                          6
	#define PHY_AN_EXP_PARALLEL_DETECTION_FAULT             0x0010U
	#define PHY_AN_EXP_LINK_PARTNER_NEXT_PAGE_ABLE          0x0008U
	#define PHY_AN_EXP_LOCAL_DEVICE_NEXT_PAGE_ABLE          0x0004U
	#define PHY_AN_EXP_PAGE_RECEIVED                        0x0002U
	#define PHY_AN_EXP_LINK_PARTNER_AUTO_NEGOTIATION_ABLE   0x0001U
#define PHY_SPEC_CTRL_STATUS                                31
	#define PHY_SPEC_CTRL_STATUS_SWITCH_LOOPBACK_PHY        0x4000U
	#define PHY_SPEC_CTRL_STATUS_TURBO_MII_ENABLE_PHY       0x0400U
	#define PHY_SPEC_CTRL_STATUS_MODE_MASK_PHY              0x0300U
	#define PHY_SPEC_CTRL_STATUS_SWITCH_COLLISION_TEST_PHY  0x0080U
	#define PHY_SPEC_CTRL_STATUS_RMII_CLOCK_OUTCLK_PHY      0x0040U
	#define PHY_SPEC_CTRL_STATUS_CLOCK_STRENGTH_PHY         0x0020U
	#define PHY_SPEC_CTRL_STATUS_SQEOFF_PHY                 0x0001U
	#define PHY_SPEC_CTRL_STATUS_AUTO_DONE                  0x1000U
	#define PHY_SPEC_CTRL_STATUS_SPEED_INDICATION_MASK      0x001CU

/* common for PHY1 and PHY2, NOT for Virtual PHY */
#define PHY_MODE_CONTROL_STATUS                             17
#define PHY_SPECIAL_MODES                                   18
#define PHY_SPECIAL_CONTROL_STAT_IND                        27
#define PHY_INTERRUPT_SOURCE                                29
#define PHY_INTERRUPT_MASK                                  30

#define SWITCH_FABRIC_READ_RETRY                            5
#define SWITCH_FABRIC_WRITE_RETRY                           5

/* Switch Fabric CSR Interface Data */
#define LAN9303_SWITCH_CSR_DATA                             0x1AC

/* Switch Fabric CSR Interface Command */
#define LAN9303_SWITCH_CSR_CMD                              0x1B0
	#define LAN9303_SWITCH_CSR_BUSY_MASK                    0x80000000UL
	#define LAN9303_SWITCH_CSR_BUSY                         0x80000000UL
	#define LAN9303_SWITCH_CSR_RW_MASK                      0x40000000UL
	#define LAN9303_SWITCH_CSR_WRITE                        0x00000000UL
	#define LAN9303_SWITCH_CSR_READ                         0x40000000UL
	#define LAN9303_SWITCH_CSR_AUTO_INC_MASK                0x20000000UL
	#define LAN9303_SWITCH_CSR_AUTO_INC_ENABLE              0x20000000UL
	#define LAN9303_SWITCH_CSR_AUTO_INC_DISABLE             0x00000000UL
	#define LAN9303_SWITCH_CSR_AUTO_DEC_MASK                0x10000000UL
	#define LAN9303_SWITCH_CSR_AUTO_DEC_ENABLE              0x10000000UL
	#define LAN9303_SWITCH_CSR_AUTO_DEC_DISABLE             0x00000000UL
	#define LAN9303_SWITCH_CSR_BE_MASK                      0x000F0000UL
	#define LAN9303_SWITCH_CSR_BE_ALL_4_BYTES               0x000F0000UL

#define SWE_ALR_CMD                                         0x1800
#define SWE_ALR_MAKE_ENTRY_MASK                             0x00000004UL
#define SWE_ALR_MAKE_ENTRY_ENABLED                          0x00000004UL
#define SWE_ALR_MAKE_ENTRY_DISABLED                         0x00000000UL
#define SWE_ALR_GET_FIRST_ENTRY_MASK                        0x00000002UL
#define SWE_ALR_GET_FIRST_ENTRY_ENABLED                     0x00000002UL
#define SWE_ALR_GET_FIRST_ENTRY_DISABLED                    0x00000000UL
#define SWE_ALR_GET_NEXT_ENTRY_MASK                         0x00000001UL
#define SWE_ALR_GET_NEXT_ENTRY_ENABLED                      0x00000001UL
#define SWE_ALR_GET_NEXT_ENTRY_DISABLED                     0x00000000UL
#define SWE_ALR_ALL_CLEARED                                 0x00000000UL

#define SWE_ALR_WR_DAT_0                                    0x1801
#define SWE_ALR_WR_DAT_1                                    0x1802
#define SWE_ALR_RD_DAT_0                                    0x1805
#define SWE_ALR_RD_DAT_1                                    0x1806

#define SWE_ALR_WR_DAT_1_VALID                              0x04000000UL
#define SWE_ALR_WR_DAT_1_STATIC                             0x01000000UL
#define SWE_ALR_WR_DAT_1_PORT_MASK                          0x00070000UL
#define SWE_ALR_WR_DAT_1_PORT_0                             0x00000000UL
#define SWE_ALR_WR_DAT_1_PORT_1                             0x00010000UL
#define SWE_ALR_WR_DAT_1_PORT_2                             0x00020000UL
#define SWE_ALR_WR_DAT_1_PORT_01                            0x00040000UL
#define SWE_ALR_WR_DAT_1_PORT_02                            0x00050000UL
#define SWE_ALR_WR_DAT_1_PORT_12                            0x00060000UL
#define SWE_ALR_WR_DAT_1_PORT_012                           0x00070000UL


#define SWE_ALR_RD_DAT_1_VALID                              0x04000000UL
#define SWE_ALR_RD_DAT_1_END_OF_TABLE                       0x02000000UL

#define SWE_ALR_MAKE_WAIT_RETRY                             5
#define SWE_ALR_INITIALIZED_WAIT_RETRY                      2

#define SWE_ALR_CMD_STS                                     0x1808
#define SWE_ALR_INIT_DONE                                   0x00000002UL
#define SWE_ALR_MAKE_PENDING                                0x00000001UL

#define SWE_PORT_INGRSS_CFG                                 0x1841
#define SWE_PORT_INGRESS_CFG_NON_RESERVED_MASK              0x0000003FUL
#define SWE_PORT_INGRSS_CFG_ENABLE_LEARNING_MASK            0x00000038UL
#define SWE_PORT_INGRSS_CFG_LEARNING_DISABLED               0x00000000UL

#define PHY_BASIC_CONTROL_OFFSET                            0U
#define PHY_BASIC_STATUS_OFFSET                             1U
#define PHY_SPECIAL_CONTROL_STATUS_OFFSET                   31U
#define PHY_CONTROL_1_STATUS_OFFSET                         30U
#define PHY_REG_INTRPT                                      27U

// PHY_BASIC_CONTROL_x
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
#define PHY_BASIC_STATUS_AN_COMPLETED                       0x0020U
#define PHY_BASIC_STATUS_AN_ABILITY                         0x0008U
#define PHY_BASIC_STATUS_LINK_UP                            0x0004U


// PHY_SPECIAL_CONTROL_STATUS_x
#define PHY_SPECIAL_CONTROL_STATUS_RMII_CLK_SEL             0x0080U
#define PHY_SPECIAL_CONTROL_STATUS_HP_MDIX_SEL              0x8000U

/* Interrupt Control/Status 0x1B bits; shift up by 8 bits to get enable. */
#define PHY_KSZ8041_REG_INTRPT_JABBER    0x80
#define PHY_KSZ8041_REG_INTRPT_RXERR     0x40
#define PHY_KSZ8041_REG_INTRPT_PGERR     0x20
#define PHY_KSZ8041_REG_INTRPT_PRLFAULT  0x10
#define PHY_KSZ8041_REG_INTRPT_LPACK     0x08
#define PHY_KSZ8041_REG_INTRPT_LINKDOWN  0x04
#define PHY_KSZ8041_REG_INTRPT_REM_FAULT 0x02
#define PHY_KSZ8041_REG_INTRPT_LINKUP    0x01

/* KSZ8041 Special Register definitions. */

/* Link Partner Next Page Ability */
#define PHY_KSZ8041_REG_LPNPA            0x08
/* Use for power saving, will probably not need this... */
#define PHY_KSZ8041_REG_AFE_CTRL1        0x11
/* RXER Counter (Receive error counter for Symbol Error frames) */
#define PHY_KSZ8041_REG_RXER             0x15
/* Override the bootstrap settings. */
#define PHY_KSZ8041_REG_STRAP_OVERRIDE   0x16
/* Oeration mode strap status. */
#define PHY_KSZ8041_REG_STRAP_STRAPST    0x17
/* Expanded control. */
#define PHY_KSZ8041_REG_STRAP_EXPCTRL    0x18
/* Interrupt Control/Status. */
#define PHY_KSZ8041_REG_INTRPT           0x1B
/* LinkMD. */
#define PHY_KSZ8041_REG_STRAP_LINKMD     0x1D

static const uint16_t PHY_SPECIAL_CONTROL_STATUS_AUTODONE_COMPLETED = 0x1000;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK = 0x000C;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_DUPLEX_MASK = 0x001C;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_MODE_FULL_DUPLEX = 0x0010;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_100M = 0x0008;


#define INVALID_SPEED_INDICATION( v ) \
	( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_HALF_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX )

#define PHY_CONTROL_REG         PHY_BASIC_CONTROL_OFFSET
#define PHY_STATUS_REG          PHY_BASIC_STATUS_OFFSET
#define PHY_SCSR_REG            PHY_SPECIAL_CONTROL_STATUS_OFFSET

#define PHY_CONTROL_RESET       PHY_BASIC_CONTROL_PHY_RESET_SET
#define PHY_STATUS_PWR_DWN      PHY_BASIC_CONTROL_PHY_PWR_DWN_SET
#define PHY_AN_ENABLED          PHY_BASIC_CONTROL_PHY_AN_ENABLED
#define PHY_DUPLEX_FULL         PHY_BASIC_CONTROL_PHY_DUPLEX_FULL
#define PHY_DUPLEX_HALF         PHY_BASIC_CONTROL_PHY_DUPLEX_HALF
#define PHY_SPEED_100M          PHY_BASIC_CONTROL_PHY_SPEED_SEL_LSB_100M
#define PHY_SPEED_10M           PHY_BASIC_CONTROL_PHY_SPEED_SEL_LSB_10M

#define PHY_STATUS_LINK         PHY_BASIC_STATUS_LINK_UP
#define PHY_AN_FINISH           PHY_BASIC_STATUS_AN_COMPLETED
#define PHY_JABBER_DETECTED     PHY_BASIC_STATUS_JABBER_DETECT
#define PHY_REMOTE_FAULT        PHY_BASIC_STATUS_REMOTE_FAULT

// ANLPAR register
#define PHY_AN_LINK_PARTNER_ACK         PHY_AN_LP_BASE_ABILITY_ACKNOWLEDGE
// ANER register
#define PHY_AN_PARALLEL_DETECT_FAULT    PHY_AN_EXP_PARALLEL_DETECTION_FAULT
#define PHY_AN_PAGE_RX                  PHY_AN_EXP_PAGE_RECEIVED

#ifdef __cplusplus
}
#endif

#endif

