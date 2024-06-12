/**
 **********************************************************************************************
 * @file            DP83848 Header File for driver class.
 *
 * @brief           The file contains DP83848 driver class to read/write
 *                  single port PHY registers over a MII/RMII communication interface.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef DP83848_H
#define DP83848_H

#include "PHY_Driver.h"
#include "EMAC_Driver.h"
#include "Output.h"
#include "DP83848_Reg.h"

/**
 **********************************************************************************************
 * @brief        DP83848 Class Declaration. This is a driver class inherited from Abstract
 *               class PHY_Driver. The  class provides read/write operation of phy registers
 *               and hardware specific functions
 **********************************************************************************************
 */

class DP83848 : public PHY_Driver
{
	public:
		/**
		 * @brief                             Constructor to create instance of DP83848 class.
		 * @param[in] reset_ctrl              PHY Reset control pin
		 * @param[in] emac_ctrl               emac control
		 * @param[in] switch_enable           parameter to decide if ethernet swich is used in hardware or not (not
		 *applicable for DP83848).
		 * @param[in] alr_learn               parameter to decide if alr learning process is to be performed (not
		 *applicable for DP83848).
		 * @param[in] PhyAddr                 Address of the PHY chip based on hardware straps.
		 * @n
		 */
		DP83848( BF_Lib::Output* reset_ctrl, EMAC_Driver* emac_ctrl, bool_t switch_enable,
				 bool_t alr_learn,
				 uint8_t PhyAddr );

		/**
		 * @brief                             Destructor to delete the DP83848 instance when it goes out of scope
		 */
		~DP83848()
		{}

		/**
		 * @brief                             Updates Port registers with port settings provided in config_word
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration which is to be written to PHY registers
		 * @return bool                       returns true if success else returns false
		 */
		bool_t Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word );

		/**
		 * @brief                             Read Port registers and update config_word with received port settings.
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration buffer
		 * @return bool                       returns true if success else returns false
		 */
		bool_t Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word );

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

		bool_t Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							 uint16_t* read_ptr );

		bool_t Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							  uint16_t* write_ptr );

		void Update_PHY_Error_Counts( uint16_t, PHY_Driver::port_config_t*, uint16_t );

		BF_Lib::Output* m_reset_output;
		EMAC_Driver* m_emac_ctrl;

		bool_t m_switch_enable;
		bool_t m_alr_learn;
		uint8_t m_PhyAddr;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Power_Down( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Link_Good( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Enable( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Complete( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_10mbps_Speed( uint16_t v )
		{
			return ( ( v & PHY_PHYSTS_10MBPS_STATUS ) == PHY_PHYSTS_10MBPS_STATUS );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Full_Duplex( uint16_t v )
		{
			return ( ( v & PHY_PHYSTS_FULL_DUPLEX_STATUS ) == PHY_PHYSTS_FULL_DUPLEX_STATUS );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Valid_Phy_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0U ) == 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Valid_Reg_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0U ) == 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Jabber_Detected( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Remote_Fault( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Link_Partner_Ack( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Parallel_Detect_Fault( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Page_Rx( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0U );
		}

		static const uint32_t PHY_READ_TO = 0x0004FFFFU;

		static const uint8_t* const s_phy_err_name[PHY_Driver::DIAG_PARAM_COUNT];
};
#endif
