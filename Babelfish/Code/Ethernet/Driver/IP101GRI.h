/**
 **********************************************************************************************
 * @file            IP101GRI Header File for driver class.
 *
 * @brief           The file contains IP101GRI driver class to read/write IP101GRI
 *                  single/dual port switch registers over a MII/RMII communication interface.
 * @details         This file is used to communicate IP101GRI.
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef IP101GRI_H
#define IP101GRI_H

#include "PHY_Driver.h"
#include "EMAC_Driver.h"

#include "IP101GRI_Reg.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "eth_phy_regs_struct.h"

/**
 **********************************************************************************************
 * @brief        IP101GRI Class Declaration. This is a driver class inherited from Abstract
 *               class PHY_Driver. The  class provides read/write operation of IP101GRI phy and switch registers
 *               and hardware specific functions
 **********************************************************************************************
 */

class IP101GRI : public PHY_Driver
{
	public:

		/**
		 * @brief                             Constructor to create instance of IP101GRI class.
		 * @param[in] phy_config              PHY Reset control pin
		 * @param[in] emac_ctrl               emac control
		 * @n
		 */

		IP101GRI( eth_phy_config_t phy_config, EMAC_Driver* emac_ctrl );

		/**
		 * @brief                             Destructor to delete the IP101GRI instance when it goes out of scope
		 */
		virtual ~IP101GRI( void );

		/**
		 * @brief                             Updates Port registers with port settings provided in config_word
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration which is to be written to PHY registers
		 * @return bool                       returns true if success else returns false
		 */
		bool Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word );

		/**
		 * @brief                             Read Port registers and update config_word with received port settings.
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration buffer
		 * @return bool                       returns true if success else returns false
		 */
		bool Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word );

		/**
		 * @brief                             Does hardware specific initialization, Hard Resets PHY
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration buffer
		 * @return bool                       returns true if success else returns false
		 */
		void Initialize_PHY();

		/**
		 * @brief                             Read PHY error name list.
		 * @param[in] void                    none
		 * @return const uint8_t*             returns pointer to constant error name list
		 */
		const uint8_t* const* Get_Phy_Driver_Error_List();

	private:

		eth_phy_config_t m_PHY_Config;

		EMAC_Driver* m_EMAC_Ctrl;

		esp_err_t IP101_Reset_Hardware();

		esp_err_t Esp_Eth_Detect_Phy_Addr( uint32_t* detected_addr );

		esp_err_t IP101_Pwrctl( bool enable );

		esp_err_t IP101_Reset();

		esp_err_t IP101_Init();

		bool Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
						   uint16_t* read_ptr );

		bool Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							uint16_t* write_ptr );

		void Update_PHY_Error_Counts( uint16_t phy_port, PHY_Driver::port_config_t* config_word,
									  uint16_t basic_status );

		static inline bool Is_Power_Down( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		static inline bool Is_Link_Good( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		static inline bool Is_Auto_Neg_Enable( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		static inline bool Is_Auto_Neg_Complete( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		static inline bool Is_100mbps_Speed( uint16_t v )
		{
			return ( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX || \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX );
		}

		static inline bool Is_Full_Duplex( uint16_t v )
		{
			return ( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX || \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX );
		}

		static inline bool Is_Invalid_Speed( uint16_t v )
		{
			return ( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) !=
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_HALF_DUPLEX && \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) !=
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX && \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) !=
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX && \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) !=
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX );
		}

		static inline bool Is_Jabber_Detected( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static inline bool Is_Remote_Fault( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static inline bool Is_Link_Partner_Ack( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static inline bool Is_Parallel_Detect_Fault( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static inline bool Is_Page_Rx( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static inline bool Valid_Phy_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0 ) == 0 );
		}

		static inline bool Valid_Reg_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0 ) == 0 );
		}

		static const uint8_t* const s_phy_err_name[PHY_Driver::DIAG_PARAM_COUNT];
};
#endif
