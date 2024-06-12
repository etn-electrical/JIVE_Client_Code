/**
 **********************************************************************************************
 * @file            LAN Header File for driver class.
 *
 * @brief           The file contains LAN driver class to read/write LAN
 *                  single/dual port switch registers over a MII/RMII communication interface.
 * @details         This file is used to communicate LAN8710, LAN8720 and LAN9303.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef LAN_H
#define LAN_H

#include "DCI_Owner.h"
#include "PHY_Driver.h"
#include "EMAC_Driver.h"
#include "Output.h"
#include "LAN_Reg.h"

/**
 **********************************************************************************************
 * @brief        LAN Class Declaration. This is a driver class inherited from Abstract
 *               class PHY_Driver. The  class provides read/write operation of LAN phy and switch registers
 *               and hardware specific functions
 **********************************************************************************************
 */


class LAN : public PHY_Driver
{
	public:
		/**
		 * @brief                             Constructor to create instance of LAN class.
		 * @param[in] reset_ctrl              PHY Reset control pin
		 * @param[in] emac_ctrl               emac control
		 * @param[in] switch_enable           parameter to decide if ethernet swich is used in hardware or not.
		 * @param[in] alr_learn               parameter to decide if alr learning process is to be performed.
		 * @param[in] PhyAddr                 Address of the PHY chip based on hardware straps.
		 * @n
		 */
		LAN( BF_Lib::Output* reset_ctrl, EMAC_Driver* emac_ctrl, bool_t switch_enable, bool_t alr_learn,
			 uint8_t PhyAddr );

		/**
		 * @brief                             Destructor to delete the LAN instance when it goes out of scope
		 */
		~LAN(){}

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

		bool_t Put_Own_MAC_Address_In_Table( uint8_t* pMAC );

		void Configure_Address_Learning( bool_t alr_disable );

		bool_t Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							 uint16_t* read_ptr );

		bool_t Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							  uint16_t* write_ptr );

		void Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word,
									  uint16_t basic_status );

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
			return ( ( a & b ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Link_Good( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Enable( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Complete( uint16_t a, uint16_t b )
		{
			return ( ( a & b ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_100mbps_Speed( uint16_t v )
		{
			return ( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_HALF_DUPLEX || \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Full_Duplex( uint16_t v )
		{
			return ( ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_10M_FULL_DUPLEX || \
					 ( v & PHY_SPECIAL_CONTROL_STATUS_SPEED_MASK ) ==
					 PHY_SPECIAL_CONTROL_STATUS_SPEED_100M_FULL_DUPLEX );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Invalid_Speed( uint16_t v )
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

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Valid_Phy_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0 ) == 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Valid_Reg_Addr( uint16_t a )
		{
			return ( ( a & 0xFFE0 ) == 0 );
		}

		bool_t DisableALRLearning();

		bool_t RemoveAllALREntries();

		bool_t PutOwnMACAddressIntoALRTable();

		bool_t SMIReadSwitchFabricCSR( UINT16 csr_addr, UINT32* read_ptr );

		bool_t SMIWriteSwitchFabricCSR( UINT16 csr_addr, const UINT32* write_ptr );

		bool_t ClearALRCmd();

		bool_t WaitForALRInitialized();

		bool_t WaitForALRMakeFinish();

		bool_t PollALREntry( UINT64* pRes );

		bool_t InitiateGetALREntry( UINT64* pRes, UINT32 alr_cmd );

		bool_t InitiateGetFirstALREntry( UINT64* pRes );

		bool_t InitiateGetNextALREntry( UINT64* pRes );

		bool_t WriteALREntry( UINT64 entry );

		void InvalidateEntry( UINT64* entry ) const;

		bool_t IsEntryValid( UINT64 entry ) const;

		bool_t IsEntryEndOfTable( UINT64 entry ) const;

		void WaitFor9303SwichHardwareReady();

		bool_t Virtual_Port_Config( uint16_t PhyAddr );

		static const uint32_t PHY_READ_TO = 0x0004FFFFU;
		static const uint32_t PHY_WRITE_TO = 0x0004FFFFU;

		static const uint8_t* const s_phy_err_name[PHY_Driver::DIAG_PARAM_COUNT];
};
#endif
