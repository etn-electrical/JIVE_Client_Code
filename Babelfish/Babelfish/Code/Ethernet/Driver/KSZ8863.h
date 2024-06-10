/**
 **********************************************************************************************
 * @file            KSZ8863 Header File for driver class.
 *
 * @brief           The file contains KSZ8863 driver class to read/write KSZ8863
 *                  dual port switch registers over a I2C communication interface.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef KSZ8863_H
#define KSZ8863_H

#include "DCI_Owner.h"
#include "PHY_Driver.h"
#include "uC_I2C.h"
#include "Output.h"
#include "EMAC_Driver.h"

/**
 **********************************************************************************************
 * @brief        KSZ8863 Class Declaration. This is a driver class inherited from Abstract
 *               class PHY_Driver. The  class provides read/write operation of KSZ8863 registers
 *               and hardware specific functions
 **********************************************************************************************
 */
class KSZ8863 : public PHY_Driver
{
	public:
		/**
		 * @brief                             Constructor to create instance of KSZ8863 class.
		 * @param[in] reset_ctrl              PHY Reset control pin
		 * @param[in] i2c_ctrl                I2C control (KSZ8863 communicates over I2C bus)
		 * @n
		 */
		KSZ8863( BF_Lib::Output* reset_ctrl, uC_I2C* i2c_ctrl, EMAC_Driver* m_emac_ctrl );

		/**
		 * @brief                             Destructor to delete the KSZ8863 instance when it goes
		 * out of scope
		 */
		~KSZ8863()
		{}

		/**
		 * @brief                             Updates Port registers with port settings provided in
		 * config_word
		 * @param[in] port                    PHY port number
		 * @param[in] config_word             port configuration which is to be written to PHY
		 * registers
		 * @return bool                       returns true if success else returns false
		 */
		bool_t Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word );

		/**
		 * @brief                             Read Port registers and update config_word with
		 * received port settings.
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

		/** @brief KSZ8863 physical address on I2C */
		static const uint8_t KSZ8863_ADDRESS = 0x5FU;

	private:

		uC_I2C* m_i2c_KSZ8863;
		EMAC_Driver* m_emac_handle;
		BF_Lib::Output* m_reset_output;
		static uint8_t m_KSZ_WriteError_Counts;
		static uint8_t m_KSZ_ReadError_Counts;
		void Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word );

		bool_t KSZ_I2CWrite( UINT8 address, UINT8 length, UINT8 datasource );

		bool_t KSZ_I2CRead( UINT8 address, UINT8 length, UINT8* datasource );

		bool_t Put_Own_MAC_Address_In_Table( uint8_t* pMAC );

		// Delays added in due to I2C messages overloading the switch with messages too quickly.
		// If too many messages are sent back to back to the switch over
		// I2C the switch will lock up the SDA line and not release it until a reset of the switch.
		// These numbers were selected based on bus utilization seen on an oscilloscope.
		static const uint16_t I2C_INTER_READ_DELAY = 5U;
		static const uint16_t I2C_INTER_WRITE_DELAY = 5U;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Power_Down( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Link_Good( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Enable( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Auto_Neg_Complete( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_100mbps_Speed( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Full_Duplex( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool_t Is_Invalid_Speed( uint8_t a, uint8_t b )
		{
			return ( ( a & b ) == 0U );
		}

		static const uint8_t* const s_phy_err_name[PHY_Driver::DIAG_PARAM_COUNT];
};
#endif
