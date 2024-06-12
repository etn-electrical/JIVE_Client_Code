/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DP83848_REG_H
#define DP83848_REG_H

#ifdef __cplusplus
extern "C"
{
#endif

// DP83848 registers
#define PHY_CONTROL_REG                 0x0000U
#define PHY_STATUS_REG                  0x0001U
#define PHY_PHYIDR1_REG                 0x0002U
#define PHY_PHYIDR2_REG                 0x0003U
#define PHY_ANAR_REG                    0x0004U
#define PHY_ANLPAR_REG                  0x0005U
#define PHY_ANER_REG                    0x0006U
#define PHY_ANNPTR_REG                  0x0007U
#define PHY_PHYSTS_REG                  0x0010U
#define PHY_MICR_REG                    0x0011U
#define PHY_MISR_REG                    0x0012U
#define PHY_FCSCR_REG                   0x0014U
#define PHY_RECR_REG                    0x0015U
#define PHY_PCSR_REG                    0x0016U
#define PHY_RBR_REG                     0x0017U
#define PHY_LEDCR_REG                   0x0018U
#define PHY_PHYCR_REG                   0x0019U
#define PHY_10BTSCR_REG                 0x001AU
#define PHY_CDCTRL1_REG                 0x001BU
#define PHY_EDCR_REG                    0x001DU

// BMCR register
#define BMCR_RESET                      0x8000U	// (1U << 15)
#define BMCR_LOOPBACK                   0x4000U	// (1U << 14)
#define BMCR_SPEED_SEL                  0x2000U	// (1U << 13)
#define BMCR_AN_EN                      0x1000U	// (1U << 12)
#define BMCR_POWER_DOWN                 0x0800U	// (1U << 11)
#define BMCR_ISOLATE                    0x0400U	// (1U << 10)
#define BMCR_RESTART_AN                 0x0200U	// (1U << 9)
#define BMCR_DUPLEX_MODE                0x0100U	// (1U << 8)
#define BMCR_COL_TEST                   0x0080U	// (1U << 7)

// BMSR register
#define BMSR_100BT4                     0x8000U	// (1U << 15)
#define BMSR_100BTX_FD                  0x4000U	// (1U << 14)
#define BMSR_100BTX                     0x2000U	// (1U << 13)
#define BMSR_10BT_FD                    0x1000U	// (1U << 12)
#define BMSR_10BT                       0x0800U	// (1U << 11)
#define BMSR_NO_PREAMBLE                0x0040U	// (1U << 6)
#define BMSR_AN_COMPLETE                0x0020U	// (1U << 5)
#define BMSR_REMOTE_FAULT               0x0010U	// (1U << 4)
#define BMSR_AN_ABLE                    0x0008U	// (1U << 3)
#define BMSR_LINK_STATUS                0x0004U	// (1U << 2)
#define BMSR_JABBER_DETECT              0x0002U	// (1U << 1)
#define BMSR_EXTENDED_CAP               0x0001U	// (1U << 0)

// ANAR register
#define ANAR_NP                         0x8000U	// (1U << 15)
#define ANAR_RF                         0x2000U	// (1U << 13)
#define ANAR_ASM_DIR                    0x0800U	// (1U << 11)
#define ANAR_PAUSE                      0x0400U	// (1U << 10)
#define ANAR_100BT4                     0x0200U	// (1U << 9)
#define ANAR_100BTX_FD                  0x0100U	// (1U << 8)
#define ANAR_100BTX                     0x0080U	// (1U << 7)
#define ANAR_10BT_FD                    0x0040U	// (1U << 6)
#define ANAR_10BT                       0x0020U	// (1U << 5)
#define ANAR_SELECTOR4                  0x0010U	// (1U << 4)
#define ANAR_SELECTOR3                  0x0008U	// (1U << 3)
#define ANAR_SELECTOR2                  0x0004U	// (1U << 2)
#define ANAR_SELECTOR1                  0x0002U	// (1U << 1)
#define ANAR_SELECTOR0                  0x0001U	// (1U << 0)

// ANLPAR register
#define ANLPAR_NP                       0x8000U	// (1U << 15)
#define ANLPAR_ACK                      0x4000U	// (1U << 14)
#define ANLPAR_RF                       0x2000U	// (1U << 13)
#define ANLPAR_ASM_DIR                  0x0800U	// (1U << 11)
#define ANLPAR_PAUSE                    0x0400U	// (1U << 10)
#define ANLPAR_100BT4                   0x0200U	// (1U << 9)
#define ANLPAR_100BTX_FD                0x0100U	// (1U << 8)
#define ANLPAR_100BTX                   0x0080U	// (1U << 7)
#define ANLPAR_10BT_FD                  0x0040U	// (1U << 6)
#define ANLPAR_10BT                     0x0020U	// (1U << 5)
#define ANLPAR_SELECTOR4                0x0010U	// (1U << 4)
#define ANLPAR_SELECTOR3                0x0008U	// (1U << 3)
#define ANLPAR_SELECTOR2                0x0004U	// (1U << 2)
#define ANLPAR_SELECTOR1                0x0002U	// (1U << 1)
#define ANLPAR_SELECTOR0                0x0001U	// (1U << 0)

// ANER register
#define ANER_PDF                        0x0010U	// (1U << 4)
#define ANER_LP_NP_ABLE                 0x0008U	// (1U << 3)
#define ANER_NP_ABLE                    0x0004U	// (1U << 2)
#define ANER_PAGE_RX                    0x0002U	// (1U << 1)
#define ANER_LP_AN_ABLE                 0x0001U	// (1U << 0)

// ANNPTR register
#define ANNPTR_NP                       0x8000U	// (1U << 15)
#define ANNPTR_MP                       0x2000U	// (1U << 13)
#define ANNPTR_ACK2                     0x1000U	// (1U << 12)
#define ANNPTR_TOG_TX                   0x0800U	// (1U << 11)
#define ANNPTR_CODE10                   0x0400U	// (1U << 10)
#define ANNPTR_CODE9                    0x0200U	// (1U << 9)
#define ANNPTR_CODE8                    0x0100U	// (1U << 8)
#define ANNPTR_CODE7                    0x0080U	// (1U << 7)
#define ANNPTR_CODE6                    0x0040U	// (1U << 6)
#define ANNPTR_CODE5                    0x0020U	// (1U << 5)
#define ANNPTR_CODE4                    0x0010U	// (1U << 4)
#define ANNPTR_CODE3                    0x0008U	// (1U << 3)
#define ANNPTR_CODE2                    0x0004U	// (1U << 2)
#define ANNPTR_CODE1                    0x0002U	// (1U << 1)
#define ANNPTR_CODE0                    0x0001U	// (1U << 0)

// PHYSTS register
#define PHYSTS_MDIX_MODE                0x4000U	// (1U << 14)
#define PHYSTS_RX_ERROR_LATCH           0x2000U	// (1U << 13)
#define PHYSTS_POLARITY_STATUS          0x1000U	// (1U << 12)
#define PHYSTS_FALSE_CARRIER_SENSE      0x0800U	// (1U << 11)
#define PHYSTS_SIGNAL_DETECT            0x0400U	// (1U << 10)
#define PHYSTS_DESCRAMBLER_LOCK         0x0200U	// (1U << 9)
#define PHYSTS_PAGE_RECEIVED            0x0100U	// (1U << 8)
#define PHYSTS_MII_INTERRUPT            0x0080U	// (1U << 7)
#define PHYSTS_REMOTE_FAULT             0x0040U	// (1U << 6)
#define PHYSTS_JABBER_DETECT            0x0020U	// (1U << 5)
#define PHYSTS_AN_COMPLETE              0x0010U	// (1U << 4)
#define PHYSTS_LOOPBACK_STATUS          0x0008U	// (1U << 3)
#define PHYSTS_DUPLEX_STATUS            0x0004U	// (1U << 2)
#define PHYSTS_SPEED_STATUS             0x0002U	// (1U << 1)
#define PHYSTS_LINK_STATUS              0x0001U	// (1U << 0)

// MICR register
#define MICR_TINT                       0x0004U	// (1U << 2)
#define MICR_INTEN                      0x0002U	// (1U << 1)
#define MICR_INT_OE                     0x0001U	// (1U << 0)

// MISR register
#define MISR_ED_INT                     0x4000U	// (1U << 14)
#define MISR_LINK_INT                   0x2000U	// (1U << 13)
#define MISR_SPD_INT                    0x1000U	// (1U << 12)
#define MISR_DUP_INT                    0x0800U	// (1U << 11)
#define MISR_ANC_INT                    0x0400U	// (1U << 10)
#define MISR_FHF_INT                    0x0200U	// (1U << 9)
#define MISR_RHF_INT                    0x0100U	// (1U << 8)
#define MISR_ED_INT_EN                  0x0040U	// (1U << 6)
#define MISR_LINK_INT_EN                0x0020U	// (1U << 5)
#define MISR_SPD_INT_EN                 0x0010U	// (1U << 4)
#define MISR_DUP_INT_EN                 0x0008U	// (1U << 3)
#define MISR_ANC_INT_EN                 0x0004U	// (1U << 2)
#define MISR_FHF_INT_EN                 0x0002U	// (1U << 1)
#define MISR_RHF_INT_EN                 0x0001U	// (1U << 0)

// FCSCR register
#define FCSCR_FCSCNT7                   0x0080U	// (1U << 7)
#define FCSCR_FCSCNT6                   0x0040U	// (1U << 6)
#define FCSCR_FCSCNT5                   0x0020U	// (1U << 5)
#define FCSCR_FCSCNT4                   0x0010U	// (1U << 4)
#define FCSCR_FCSCNT3                   0x0008U	// (1U << 3)
#define FCSCR_FCSCNT2                   0x0004U	// (1U << 2)
#define FCSCR_FCSCNT1                   0x0002U	// (1U << 1)
#define FCSCR_FCSCNT0                   0x0001U	// (1U << 0)

// RECR register
#define RECR_RXERCNT7                   0x0080U	// (1U << 7)
#define RECR_RXERCNT6                   0x0040U	// (1U << 6)
#define RECR_RXERCNT5                   0x0020U	// (1U << 5)
#define RECR_RXERCNT4                   0x0010U	// (1U << 4)
#define RECR_RXERCNT3                   0x0008U	// (1U << 3)
#define RECR_RXERCNT2                   0x0004U	// (1U << 2)
#define RECR_RXERCNT1                   0x0002U	// (1U << 1)
#define RECR_RXERCNT0                   0x0001U	// (1U << 0)

// PCSR register
#define PCSR_TQ_EN                      0x0400U	// (1U << 10)
#define PCSR_SD_FORCE_PMA               0x0200U	// (1U << 9)
#define PCSR_SD_OPTION                  0x0100U	// (1U << 8)
#define PCSR_DESC_TIME                  0x0080U	// (1U << 7)
#define PCSR_FORCE_100_OK               0x0020U	// (1U << 5)
#define PCSR_NRZI_BYPASS                0x0004U	// (1U << 2)

// RBR register
#define RBR_RMII_MODE                   0x0020U	// (1U << 5)
#define RBR_RMII_REV1_0                 0x0010U	// (1U << 4)
#define RBR_RX_OVF_STS                  0x0008U	// (1U << 3)
#define RBR_RX_UNF_STS                  0x0004U	// (1U << 2)
#define RBR_ELAST_BUF1                  0x0002U	// (1U << 1)
#define RBR_ELAST_BUF0                  0x0001U	// (1U << 0)

// LEDCR register
#define LEDCR_DRV_SPDLED                0x0020U	// (1U << 5)
#define LEDCR_DRV_LNKLED                0x0010U	// (1U << 4)
#define LEDCR_DRV_ACTLED                0x0008U	// (1U << 3)
#define LEDCR_SPDLED                    0x0004U	// (1U << 2)
#define LEDCR_LNKLED                    0x0002U	// (1U << 1)
#define LEDCR_ACTLED                    0x0001U	// (1U << 0)

// PHYCR register
#define PHYCR_MDIX_EN                   0x8000U	// (1U << 15)
#define PHYCR_FORCE_MDIX                0x4000U	// (1U << 14)
#define PHYCR_PAUSE_RX                  0x2000U	// (1U << 13)
#define PHYCR_PAUSE_TX                  0x1000U	// (1U << 12)
#define PHYCR_BIST_FE                   0x0800U	// (1U << 11)
#define PHYCR_PSR_15                    0x0400U	// (1U << 10)
#define PHYCR_BIST_STATUS               0x0200U	// (1U << 9)
#define PHYCR_BIST_START                0x0100U	// (1U << 8)
#define PHYCR_BP_STRETCH                0x0080U	// (1U << 7)
#define PHYCR_LED_CNFG1                 0x0040U	// (1U << 6)
#define PHYCR_LED_CNFG0                 0x0020U	// (1U << 5)
#define PHYCR_PHYADDR4                  0x0010U	// (1U << 4)
#define PHYCR_PHYADDR3                  0x0008U	// (1U << 3)
#define PHYCR_PHYADDR2                  0x0004U	// (1U << 2)
#define PHYCR_PHYADDR1                  0x0002U	// (1U << 1)
#define PHYCR_PHYADDR0                  0x0001U	// (1U << 0)

// 10BTSCR register
#define _10BTSCR_10BT_SERIAL            0x8000U	// (1U << 15)
#define _10BTSCR_SQUELCH2               0x0800U	// (1U << 11)
#define _10BTSCR_SQUELCH1               0x0400U	// (1U << 10)
#define _10BTSCR_SQUELCH0               0x0200U	// (1U << 9)
#define _10BTSCR_LOOPBACK_10_DIS        0x0100U	// (1U << 8)
#define _10BTSCR_LP_DIS                 0x0080U	// (1U << 7)
#define _10BTSCR_FORCE_LINK_10          0x0040U	// (1U << 6)
#define _10BTSCR_POLARITY               0x0010U	// (1U << 4)
#define _10BTSCR_HEARTBEAT_DIS          0x0002U	// (1U << 1)
#define _10BTSCR_JABBER_DIS             0x0001U	// (1U << 0)

// CDCTRL1 register
#define CDCTRL1_BIST_ERROR_COUNT7       0x8000U	// (1U << 15)
#define CDCTRL1_BIST_ERROR_COUNT6       0x4000U	// (1U << 14)
#define CDCTRL1_BIST_ERROR_COUNT5       0x2000U	// (1U << 13)
#define CDCTRL1_BIST_ERROR_COUNT4       0x1000U	// (1U << 12)
#define CDCTRL1_BIST_ERROR_COUNT3       0x0800U	// (1U << 11)
#define CDCTRL1_BIST_ERROR_COUNT2       0x0400U	// (1U << 10)
#define CDCTRL1_BIST_ERROR_COUNT1       0x0200U	// (1U << 9)
#define CDCTRL1_BIST_ERROR_COUNT0       0x0100U	// (1U << 8)
#define CDCTRL1_BIST_CONT_MODE          0x0020U	// (1U << 5)
#define CDCTRL1_CDPATTEN_10             0x0010U	// (1U << 4)
#define CDCTRL1_10MEG_PATT_GAP          0x0004U	// (1U << 2)
#define CDCTRL1_CDPATTSEL1              0x0002U	// (1U << 1)
#define CDCTRL1_CDPATTSEL0              0x0001U	// (1U << 0)

// EDCR register
#define EDCR_ED_EN                      0x8000U	// (1U << 15)
#define EDCR_ED_AUTO_UP                 0x4000U	// (1U << 14)
#define EDCR_ED_AUTO_DOWN               0x2000U	// (1U << 13)
#define EDCR_ED_MAN                     0x1000U	// (1U << 12)
#define EDCR_ED_BURST_DIS               0x0800U	// (1U << 11)
#define EDCR_ED_PWR_STATE               0x0400U	// (1U << 10)
#define EDCR_ED_ERR_MET                 0x0200U	// (1U << 9)
#define EDCR_ED_DATA_MET                0x0100U	// (1U << 8)
#define EDCR_ED_ERR_COUNT3              0x0080U	// (1U << 7)
#define EDCR_ED_ERR_COUNT2              0x0040U	// (1U << 6)
#define EDCR_ED_ERR_COUNT1              0x0020U	// (1U << 5)
#define EDCR_ED_ERR_COUNT0              0x0010U	// (1U << 4)
#define EDCR_ED_DATA_COUNT3             0x0008U	// (1U << 3)
#define EDCR_ED_DATA_COUNT2             0x0004U	// (1U << 2)
#define EDCR_ED_DATA_COUNT1             0x0002U	// (1U << 1)
#define EDCR_ED_DATA_COUNT0             0x0001U	// (1U << 0)

// Basic Mode Control Register
#define PHY_CONTROL_PWR_DWN             BMCR_POWER_DOWN
#define PHY_STATUS_PWR_DWN              BMCR_POWER_DOWN
#define PHY_AN_ENABLED                  BMCR_AN_EN
#define PHY_DUPLEX_FULL                 BMCR_DUPLEX_MODE
#define PHY_DUPLEX_HALF                 0x00U
#define PHY_SPEED_100M                  BMCR_SPEED_SEL
#define PHY_SPEED_10M                   0x00U
#define PHY_RESET                       BMCR_RESET

// Basic Mode Status Register
#define PHY_STATUS_LINK                 BMSR_LINK_STATUS
#define PHY_AN_FINISH                   BMSR_AN_COMPLETE
#define PHY_JABBER_DETECTED             BMSR_JABBER_DETECT
#define PHY_REMOTE_FAULT                BMSR_REMOTE_FAULT

// ANLPAR register
#define PHY_AN_LINK_PARTNER_ACK         ANLPAR_ACK
// ANER register
#define PHY_AN_PARALLEL_DETECT_FAULT    ANER_PDF
#define PHY_AN_PAGE_RX                  ANER_PAGE_RX

// Extended PHY Status Register
#define PHY_PHYSTS_FULL_DUPLEX_STATUS   PHYSTS_DUPLEX_STATUS
#define PHY_PHYSTS_HALF_DUPLEX_STATUS   0x00U
#define PHY_PHYSTS_10MBPS_STATUS        PHYSTS_SPEED_STATUS
#define PHY_PHYSTS_100MBPS_STATUS       0x00U

#ifdef __cplusplus
}
#endif

#endif

