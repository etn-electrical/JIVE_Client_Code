/**
 **********************************************************************************************
 * @file            PHY_Driver abstract class for KSZ8863 or LAN Driver class.
 *
 * @brief           The file contains PHY_Driver, an abstract class containing
 *                  common driver APIs and constants
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef PHY_DRIVER_H_
#define PHY_DRIVER_H_

#include "Includes.h"

class PHY_Driver
{
	public:
		static const uint8_t PORT_DISABLE = 0U;
		static const uint8_t PORT_ENABLE = 1U;
		static const uint8_t AUTO_NEG_DISABLE = 0U;
		static const uint8_t AUTO_NEG_ENABLE = 1U;
		static const uint8_t HALF_DUPLEX = 0U;
		static const uint8_t FULL_DUPLEX = 1U;
		static const uint8_t LINK_DOWN = 0U;
		static const uint8_t LINK_UP = 1U;
		static const uint8_t JABBER_DETECT_OFF = 0U;
		static const uint8_t JABBER_DETECT_ON = 1U;
		static const uint8_t REMOTE_FAULT_OFF = 0U;
		static const uint8_t REMOTE_FAULT_ON = 1U;
		static const uint8_t LINK_PARNTER_ACK_DISABLE = 0U;
		static const uint8_t LINK_PARNTER_ACK_ENABLE = 1U;
		static const uint8_t PARALLEL_DETECT_FAULT_OFF = 0U;
		static const uint8_t PARALLEL_DETECT_FAULT_ON = 1U;
		static const uint8_t PAGE_NOT_RX = 0U;
		static const uint8_t PAGE_RX = 1U;
		static const uint8_t AUTO_NEG_IN_PROGRESS = 0U;
		static const uint8_t AUTO_NEG_DONE = 1U;
		static const uint8_t SPEED_INVALID = 0U;
		static const uint8_t SPEED_10MBPS = 1U;
		static const uint8_t SPEED_100MBPS = 2U;
		static const uint8_t SPEED_1GBPS = 3U;
		static const uint8_t SPEED_OPTIONS = 4U;// This values should come after max speed which is supported

		static const uint8_t CLEAR_CONFIG_WORD = 0U;

		union port_config_t
		{
			struct
			{
				uint32_t port_enable : 1;		// enable - 1, disable - 0
				uint32_t auto_neg : 1;			// enable - 1, disable - 0
				uint32_t duplex : 1;		// full - 1, half - 0
				uint32_t speed : 2;			// 00 -  Invalid, 01 - 10MPBS, 10 - 100 MBPS, 11 - 1GBPS
				uint32_t link_good : 1;			// up - 1, down - 0
				uint32_t auto_neg_done : 1;			// 1 - done, 0 - In Progress
				uint32_t jabber : 1;		// ON - 1, OFF - 0
				uint32_t remote_fault : 1;			// ON - 1, OFF - 0
				uint32_t link_partner_ack : 1;			// ENABLE - 1, DISABLE - 0
				uint32_t parallel_detect_flt : 1;		// ENABLE - 1, DISABLE - 0
				uint32_t page_rx : 1;		// ENABLE - 1, DISABLE - 0
				uint32_t rx_error_counter : 8;
				uint32_t tx_error_counter : 8;
			} fields;
			uint32_t port_config_word;
		};

		static const uint8_t LINK_UP_OFFSET = 0U;
		static const uint8_t LINK_DOWN_OFFSET = 1U;
		static const uint8_t JABBER_DETECT_OFFSET = 2U;
		static const uint8_t REMOTE_FAULT_OFFSET = 3U;
		static const uint8_t LINK_PARNTER_ACK_OFFSET = 4U;
		static const uint8_t PARALLEL_DETECT_FAULT_OFFSET = 5U;
		static const uint8_t PAGE_RX_OFFSET = 6U;
		static const uint8_t RX_ERROR_OFFSET = 7U;
		static const uint8_t TX_ERROR_OFFSET = 8U;
		static const uint8_t DIAG_PARAM_COUNT = 9U;

		virtual ~PHY_Driver() = 0;
		virtual void Initialize_PHY() = 0;

		virtual bool Write_Port_Config( uint8_t port, port_config_t* config_word ) = 0;

		virtual bool Read_Port_Config( uint8_t port, port_config_t* config_word ) = 0;

		virtual const uint8_t* const* Get_Phy_Driver_Error_List() = 0;

};

#endif
