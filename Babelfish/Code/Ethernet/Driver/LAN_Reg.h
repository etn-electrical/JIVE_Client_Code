/* author: Ting Yan, August 22, 2010
   SMSC LAN9303 related definitions*/


#ifndef LAN9303_H
#define LAN9303_H

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

/*

   Register Bit Types - from SMSC LAN9303 Datasheet Revision 1.3 08-27-09, p137

   R:      can be read
   W:      can be write
   RO:     read only, writing has no effect
   WO:     write only, reading returns unspecified data
   WC:     writing 1 clears value, writing 0 has no effect
   WAC:    writing anything to clear
   RC:     reading clears contents, writing has no effect
   LL:     latch low, clear on read of register
   LH:     latch high, clear on read of register
   SC:     self-clearing, self-cleared after set, writing 0 no effect, can be read
   SS:     self-setting, self-set after cleared, writing 1 no effect, can be read
   RO/LH:  read only, latch high: stay high if has not been read, regardless of
        high condition; after being read, if high condition remains, high; if
        high condition is removed, low
   NASR:   not affected by software reset
   RESERVED:
        must be written with 0's, value not guaranteed on read

 */

/*

   Register Map - from SMSC LAN9303 Datasheet Revision 1.3 08-27-09, p136

   Register Address Range: 000H ~ 3FFH (10 bit address)

   Reserved: 000H~04CH, and 2E0H~3FFH
   System Control and Status Registers (CSRs): 050H~2DCH
   in which,
    Reserved: 0ACH~19CH
    PHY Management Interface Registers:         0A4H~0A8H
    Switch Interface Registers:                 1ACH~1B0H
    Virtual PHY Registers:                      1C0H~1DCH
    Switch CSR Direct Data Registers:           200H~2DCH


 */

/* GPIO pins */
enum enum_LAN9303_GPIO
{
	LAN9303_GPIO_0,
	LAN9303_GPIO_1,
	LAN9303_GPIO_2,
	LAN9303_GPIO_3,
	LAN9303_GPIO_4,
	LAN9303_GPIO_5
};

/* LED pins */
enum enum_LAN9303_LED
{
	LAN9303_LED_0,
	LAN9303_LED_1,
	LAN9303_LED_2,
	LAN9303_LED_3,
	LAN9303_LED_4,
	LAN9303_LED_5
};

// This is virtual port address for 9303
// #define PHY_DEFAULT_ADDR    0     //9303
// #define PHY_DEFAULT_ADDR    4     //8710
// #define PHY_DEFAULT_ADDR    0     //8720

#define LAN9303_VIRTUAL_PORT_ADDR   0



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

/* register addresses */

/* Chip ID and Revision,RO */
#define LAN9303_ID_REV                                      0x050
	#define LAN9303_ID_REV_CHIP_ID                          0x9303
	#define LAN9303_ID_REV_CHIP_ID_MASK                     0xFFFF0000UL
	#define LAN9303_ID_REV_CHIP_ID_OFFSET                   16
	#define LAN9303_ID_REV_CHIP_REVISION_MASK               0x0000FFFFUL

/* Interrupt Configuration */
#define LAN9303_IRQ_CFG                                     0x054
	#define LAN9303_IRQ_CFG_WRITE_MASK                      0xFF004111UL
/* interrupt de-assertion interval, multiples of 10uS, default 0x00 */
	#define LAN9303_IRQ_CFG_INT_DEAS_MASK                   0xFF000000UL
/* interrupt de-assertion interval clear, R/W, SC, default 0 */
	#define LAN9303_IRQ_CFG_INT_DEAS_CLR                    0x00004000UL
/* interrupts in (1)/not in (0) de-assertion interval, RO SC, default 0 */
	#define LAN9303_IRQ_CFG_INT_DEAS_STS_IN_DEAS_INTERVAL   0x00002000UL
/* one or more enabled interrupts active, RO, default 0 */
	#define LAN9303_IRQ_CFG_IRQ_INT_ACTIVE                  0x00002000UL
/* enable/disable final interrupt output to IRQ pin, R/W, default 0 */
	#define LAN9303_IRQ_CFG_IRQ_EN                          0x00000100UL
/* IRQ_TYPE==1: active low(0) or high (1), R/W NASR, default 0 */
	#define LAN9303_IRQ_CFG_IRQ_POL_ACTIVE_HIGH             0x00000010UL
/* open drain (0) or push-pull (1), R/W NASR, default 0 */
	#define LAN9303_IRQ_CFG_IRQ_TYPE_PUSH_PULL              0x00000001UL


/* Interrupt Status, bits default all 0 */
#define LAN9303_INT_STS                                     0x058
/* software interrupt, R/WC */
	#define LAN9303_INT_STS_SW_INT                          0x80000000UL
/* device ready, R/WC */
	#define LAN9303_INT_STS_READY                           0x40000000UL
/* switch fabric interrupt event, RO */
	#define LAN9303_INT_STS_SWITCH_INT                      0x10000000UL
/* PHY2 interrupt event, RO */
	#define LAN9303_INT_STS_PHY_INT2                        0x08000000UL
/* PHY1 interrupt event, RO */
	#define LAN9303_INT_STS_PHY_INT1                        0x04000000UL
/* general purpose timer interrupt, R/WC */
	#define LAN9303_INT_STS_GPT_INT                         0x00080000UL
/* general purpose interrupt event, RO */
	#define LAN9303_INT_STS_GPIO                            0x00001000UL


/* Interrupt Enable, R/W for non-reserved bits default all 0 */
#define LAN9303_INT_EN                                      0x05C
	#define LAN9303_INT_EN_WRITE_MASK                       0xDC081000UL
	#define LAN9303_INT_EN_SW_INT_EN                        0x80000000UL
	#define LAN9303_INT_EN_READY_EN                         0x40000000UL
	#define LAN9303_INT_EN_SWITCH_INT_EN                    0x10000000UL
	#define LAN9303_INT_EN_PHY_INT2_EN                      0x08000000UL
	#define LAN9303_INT_EN_PHY_INT1_EN                      0x04000000UL
	#define LAN9303_INT_EN_GPT_INT_EN                       0x00080000UL
	#define LAN9303_INT_EN_GPIO_INT_EN                      0x00001000UL


/* Byte Order Test, RO
   can be read while device not ready; when ready correct pattern returned
   SMI mode: may only read either half of the register*/
#define LAN9303_BYTE_TEST                                   0x064
	#define LAN9303_BYTE_TEST_VALID_PATTERN                 0x87654321UL

/* Hardware Configuration, RO
   SMI mode: may only read either half of the register*/
#define LAN9303_HW_CFG                                      0x074
	#define LAN9303_HW_CFG_READY                            0x08000000UL
	#define LAN9303_HW_CFG_AMDIX_EN_PORT2                   0x04000000UL
	#define LAN9303_HW_CFG_AMDIX_EN_PORT1                   0x02000000UL


/* General Purpose Timer Configuration */
#define LAN9303_GPT_CFG                                     0x08C
	#define LAN9303_GPT_CFG_MASK                            0x2000FFFFUL
	#define LAN9303_GPT_CFG_TIMER_EN                        0x20000000UL
	#define LAN9303_GPT_CFG_LOAD_MASK                       0x0000FFFFUL

/* General Purpose Timer Count, RO */
#define LAN9303_GPT_CNT                                     0x090
	#define LAN9303_GPT_CNT_MASK                            0x0000FFFFUL

/* Free Running Counter, RO: 25MHz, 32-bit counter, rollover */
#define LAN9303_FREE_RUN                                    0x09C

/* PHY Management Interface Data */
#define LAN9303_PMI_DATA                                    0x0A4
	#define LAN9303_PMI_DATA_MII_DATA_MASK                  0x0000FFFFUL

/* PHY Management Interface Access */
#define LAN9303_PMI_ACCESS                                  0x0A8
	#define LAN9303_PMI_ACCESS_WRITE_MASK                   0x0000FFC2UL
/* PHY addr, 0~31, R/W, default 0 */
	#define LAN9303_PMI_ACCESS_PHY_ADDR_MASK                0x0000F800UL
/* register index, 0~31, R/W, default 0 */
	#define LAN9303_PMI_ACCESS_MIIRINDA_MASK                0x000007C0UL
/* write 1, read 0, R/W, default 0 */
	#define LAN9303_PMI_ACCESS_MIIWnR                       0x00000002UL
/* must be read as 0 before writing to PMI_DATA/ACCESS
   automatically set when this register is written
   automatically cleared when access is completed*/
	#define LAN9303_PMI_ACCESS_MIIBZY                       0x00000001UL

/* Port 1 Manual Flow Control */
#define LAN9303_MANUAL_FC_1                                 0x1A0

/* Port 2 Manual Flow Control */
#define LAN9303_MANUAL_FC_2                                 0x1A4

/* Port 0 Manual Flow Control */
#define LAN9303_MANUAL_FC_0                                 0x1A8

	#define LAN9303_MANUAL_FC_WRITE_MASK_X                  0x00000047UL
/* enable (1) /disable (0) port X half-duplex backpressure R/W */
	#define LAN9303_MANUAL_FC_BP_EN_X                       0x00000040UL
/* port X: full-duplex (0) or half-duplex (1), RO */
	#define LAN9303_MANUAL_FC_CUR_DUP_X                     0x00000020UL
/* port X: actual receive flow disable (0) or enable (1), RO */
	#define LAN9303_MANUAL_FC_CUR_RX_FC_X                   0x00000010UL
/* port X: actual transmit flow disable (0) or enable (1), RO */
	#define LAN9303_MANUAL_FC_CUR_TX_FC_X                   0x00000008UL
/* port X: disable (0) enable (1) full-duplex receive flow control, R/W */
	#define LAN9303_MANUAL_FC_RX_FC_X                       0x00000004UL
/* port X: disable (0) enable (1) full-duplex transmit flow control, R/W */
	#define LAN9303_MANUAL_FC_TX_FC_X                       0x00000002UL
/* port X: manual flow control, R/W */
	#define LAN9303_MANUAL_FC_MANUAL_FC_X                   0x00000001UL



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


/* EEPROM Command */
#define LAN9303_E2P_CMD                                     0x1B4

/* EEPROM Data */
#define LAN9303_E2P_DATA                                    0x1B8

/* LED Configuration Register */
#define LAN9303_LED_CFG                                     0x1BC
	#define LAN9303_LED_CFG_WRITE_MASK                      0x0000033FUL
/* LED functions: SMSC LAN9303 Datasheet Revision 1.3 08-27-09, p133
   R/W, default LED_fun_strap[1:0]*/
	#define LAN9303_LED_CFG_LED_FUN_00B                     0x00000000UL
	#define LAN9303_LED_CFG_LED_FUN_01B                     0x00000100UL
	#define LAN9303_LED_CFG_LED_FUN_10B                     0x00000200UL
	#define LAN9303_LED_CFG_LED_FUN_11B                     0x00000300UL
	#define LAN9303_LED_CFG_LED_FUN_MASK                    0x00000300UL
/* LED enable: 0 GPIO, 1 LED, R/W, LED_en_strap[5:0] */
	#define LAN9303_LED_CFG_LED_EN                          0x00000001UL




/* Virtual PHY Basic Control */
#define LAN9303_VPHY_BASIC_CTRL                             0x1C0

/* Virtual PHY Basic Status */
#define LAN9303_VPHY_BASIC_STATUS                           0x1C4

/* Virtual PHY Identification MSB */
#define LAN9303_VPHY_ID_MSB                                 0x1C8

/* Virtual PHY Identification LSB */
#define LAN9303_VPHY_ID_LSB                                 0x1CC

/* Virtual PHY Auto-Negotiation Advertisement */
#define LAN9303_VPHY_AN_ADV                                 0x1D0

/* Virtual PHY Auto-Negotiation Link Partner Base Page Ability */
#define LAN9303_VPHY_AN_LP_BASE_ABILITY                     0x1D4

/* Virtual PHY Auto-Negotiation Expansion */
#define LAN9303_VPHY_AN_EXP                                 0x1D8

/* Virtual PHY Special Control/Status */
#define LAN9303_VPHY_SPECIAL_CONTROL_STATUS                 0x1DC

/* General Purpose I/O Configuration */
#define LAN9303_GPIO_CFG                                    0x1E0
	#define LAN9303_GPIO_CFG_WRITE_MASK                     0x003F003FUL
/* GPIO interrupt poloarity: low (0) or high (1) logic, R/W, default 0 */
	#define LAN9303_GPIO_CFG_GPIO_INT_POL_HIGH              0x00010000UL
/* GPIO buffer type:open-drain (0) or push/pull (1), R/W, default 0 */
	#define LAN9303_GPIO_CFG_GPIOBUF_PUSH_PULL              0x00000001UL

/* General Purpose I/O Data & Direction */
#define LAN9303_GPIO_DATA_DIR                               0x1E4
	#define LAN9303_GPIO_DATA_DIR_WRITE_MASK                0x003F003FUL
/* GPIO direction: input (0) or output (1), R/W, default 0 */
	#define LAN9303_GPIO_DATA_DIR_GPDIR_OUTPUT              0x00010000UL
/* GPIO data, input state or last written output, default 0 */
	#define LAN9303_GPIO_DATA_DIR_GPIOD                     0x00000001UL

/* General Purpose I/O Interrupt Status and Enable */
#define LAN9303_GPIO_INT_STS_EN                             0x1E8
	#define LAN9303_GPIO_INT_STS_EN_WRITE_MASK              0x003F0000UL
/* GPIO interrupt enable: enable (1), disable (0), R/W, default 0 */
	#define LAN9303_GPIO_INT_STS_EN_GPIO_INT_EN             0x00010000UL
/* GPIO interrupt, write 1 to acknowledge/clear, R/WC, default 0 */
	#define LAN9303_GPIO_INT_STS_EN_GPIO_INT                0x00000001UL


/* Switch MAC Address High */
#define LAN9303_SWITCH_MAC_ADDRH                            0x1F0

/* Switch MAC Address Low */
#define LAN9303_SWITCH_MAC_ADDRL                            0x1F4

/* Reset Control, R/W SC, self cleared after released from reset
   SMI mode: may only read either half of the register
   PHY2 or PHY1 reset holds the PHY reset for 102uS
   DIGITAL_RST resets the hold chip except PLL, VPHY, PHY1 and PHY2*/
#define LAN9303_RESET_CTL                                   0x1F8
	#define LAN9303_RESET_CTL_VPHY_RST                      0x00000008UL
	#define LAN9303_RESET_CTL_PHY2_RST                      0x00000004UL
	#define LAN9303_RESET_CTL_DIGITAL_RST                   0x00000001UL


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
// #define PHY_SPECIAL_CONTROL_STATUS_AUTODONE_COMPLETED       0x1000
// #define PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK               0x001C
// #define PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_HALF_DUPLEX    0x0004
// #define PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX   0x0008
// #define PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX    0x0014
// #define PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX   0x0018


static const uint16_t PHY_SPECIAL_CONTROL_STATUS_AUTODONE_COMPLETED = 0x1000;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK = 0x001C;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_HALF_DUPLEX = 0x0004;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX = 0x0008;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX = 0x0014;
static const uint16_t PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX = 0x0018;


#define INVALID_SPEED_INDICATION( v ) \
	( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_HALF_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX \
	  && ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) != PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX )

// #define IS_100M(v) \
//    ( (v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK) == PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX \
//      || (v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK) == PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX )

// #define IS_FULL_DUPLEX(v) \
//    ( (v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK) == PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX \
//      || (v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK) == PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX )



// LAN9303 only uses 0~3
// #define LAN9303_VALID_PHY_ADDR(addr) ((((UINT16) addr) & 0xFFFC) == 0)
// #define VALID_PHY_ADDR(addr) ((((uint16_t) addr) & 0xFFE0) == 0)
// #define VALID_REG_ADDR(addr) ((((uint16_t) addr) & 0xFFE0) == 0)

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

// #endif     // #ifdef EDAP_CARD_LAN9303

#endif

