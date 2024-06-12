/**
 **********************************************************************************************
 * @file            KSZ8863 source File for driver class.
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

#include "Includes.h"
#include "KSZ8863_Reg.h"
#include "KSZ8863.h"
#include "netif.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */
const uint8_t* const KSZ8863::s_phy_err_name[] =
{
	"link_up",
	"link_down",
	"na",
	"na",
	"na",
	"na",
	"na",
	"comm_read_error",
	"comm_write_error"
};

// There are two drive strength option provided by KSZ8863 manufacturer
// To set drive strength to 8ms - KSZ_IO_DriveStrenth = 0
// To set drive strength to 16ms - KSZ_IO_DriveStrenth = 0x40
static const uint8_t KSZ_IO_DRIVE_STRENGTH = 0U;

// Ethernet Switch Reset interval
static const uint8_t SWITCH_RESET_DELAY = 52U;	// Min - 11, Max - 100

// Mask for validating invalid speed of port
static const uint8_t INVALID_SPEED_MASK = static_cast<uint8_t>( PORT_CTRL12_ADVER_100_FULLDUPLEX )
	| static_cast<uint8_t>( PORT_CTRL12_ADVER_100_HALFDUPLEX )
	| static_cast<uint8_t>( PORT_CTRL12_ADVER_10_FULLDUPLEX )
	| static_cast<uint8_t>( PORT_CTRL12_ADVER_10_HALFDUPLEX );

// KSZ8863 register structure used in read/write functions
const struct ksz_reg_t
{
	const uint8_t port_ctrl;
	const uint8_t port_down;
	const uint8_t link_stus;
	const uint8_t port_stus;
} KSZ8863_REG =
{
	static_cast<uint8_t>( KSZ_R28_PORT1_CONTROL12 ),
	static_cast<uint8_t>( KSZ_R29_PORT1_CONTROL13 ),
	static_cast<uint8_t>( KSZ_R30_PORT1_STATUS0 ),
	static_cast<uint8_t>( KSZ_R31_PORT1_STATUS1 )
};
// KSZ8863 register address offset for port 1
static const uint8_t PORT_1_REG_OFFSET = 0U;
// KSZ8863 register address offset for port 2
static const uint8_t PORT_2_REG_OFFSET = 16U;

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
uint8_t KSZ8863::m_KSZ_WriteError_Counts = 0U;
uint8_t KSZ8863::m_KSZ_ReadError_Counts = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
KSZ8863::KSZ8863( BF_Lib::Output* reset_ctrl, uC_I2C* i2c_ctrl, EMAC_Driver* m_emac_ctrl ) :
	m_i2c_KSZ8863( i2c_ctrl ),
	m_emac_handle( m_emac_ctrl ),
	m_reset_output( reset_ctrl )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t KSZ8863::KSZ_I2CWrite( UINT8 address, UINT8 length, UINT8 datasource )
{
	bool_t result;

	uC_I2C::i2c_status_t i2c_write;

	// process only if length is non zero
	if ( length == 0U )
	{
		result = true;
	}
	else
	{
		m_i2c_KSZ8863->Write( address, length, &datasource, &i2c_write );

		if ( i2c_write != uC_I2C::I2C_FINISH_TX )
		{
			m_KSZ_WriteError_Counts++;
			result = false;
		}
		else
		{
			result = true;
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t KSZ8863::KSZ_I2CRead( UINT8 address, UINT8 length, UINT8* datasource )
{
	bool_t result;

	uC_I2C::i2c_status_t i2c_read;

	// process only if length is non zero
	if ( length == 0U )
	{
		result = true;
	}
	else
	{
		m_i2c_KSZ8863->Read( address, length, datasource, &i2c_read );

		// Update Error counter if error occurred
		if ( i2c_read != uC_I2C::I2C_FINISH_RX )
		{
			m_KSZ_ReadError_Counts++;
			result = false;
		}
		else
		{
			result = true;
		}

	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t KSZ8863::Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	bool_t result = false;
	uint8_t reg_addr;

	// verify if port value is in valid range
	if ( port < 2U )
	{
		// Set port register's base address
		if ( port == 0 )
		{
			port = PORT_1_REG_OFFSET;
		}
		else
		{
			port = PORT_2_REG_OFFSET;
		}
		// Set port settings to PHY register
		uint8_t reg_val = static_cast<uint8_t>( PORT_CTRL12_ADVER_FLOW_CONTROL )
			| static_cast<uint8_t>( PORT_CTRL12_ADVER_100_FULLDUPLEX )
			| static_cast<uint8_t>( PORT_CTRL12_ADVER_100_HALFDUPLEX )
			| static_cast<uint8_t>( PORT_CTRL12_ADVER_10_FULLDUPLEX )
			| static_cast<uint8_t>( PORT_CTRL12_ADVER_10_HALFDUPLEX );

		if ( config_word->fields.auto_neg == AUTO_NEG_ENABLE )	// auto_neg
		{
			reg_val |= static_cast<uint8_t>( PORT_CTRL12_AUTO_NEGO_ENABLE );
		}
		else
		{
			if ( config_word->fields.duplex == FULL_DUPLEX )// duplex
			{
				reg_val |= static_cast<uint8_t>( PORT_CTRL12_FORCE_DUPLEX );
			}

			if ( config_word->fields.speed == SPEED_100MBPS )	// speed
			{
				reg_val |= static_cast<uint8_t>( PORT_CTRL12_FORCE_SPEED_100 );
			}
		}

		reg_addr = KSZ8863_REG.port_ctrl + port;
		result = KSZ_I2CWrite( reg_addr, 1U, reg_val );
		OS_Tasker::Delay( I2C_INTER_WRITE_DELAY );

		// port enable /disable
		if ( config_word->fields.port_enable == PORT_DISABLE )
		{
			reg_val = static_cast<uint8_t>( PORT_CTRL13_POWER_DOWN );
			reg_addr = KSZ8863_REG.port_down + port;
			result = KSZ_I2CWrite( reg_addr, 1U, reg_val );
			// result = ( KSZ_I2CWrite( reg_addr, 1U, reg_val ) && result ) ? true : false;
			OS_Tasker::Delay( I2C_INTER_WRITE_DELAY );
		}
	}
	else
	{
		// comment to avoid misra warning
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t KSZ8863::Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	bool_t i2c_read = false;
	uint8_t reg_addr;
	uint8_t port_ena;

	// verify if port value is in valid range
	if ( port < 2U )
	{
		port = ( port == 0U ) ? PORT_1_REG_OFFSET : PORT_2_REG_OFFSET;
		// read power down register to verify if port is enabled
		reg_addr = KSZ8863_REG.port_down + port;
		i2c_read = KSZ_I2CRead( reg_addr, 1U, &port_ena );
		// Delays added in due to I2C messages overloading the switch with messages too quickly.
		// If too many messages are sent back to back to the switch over
		// I2C the switch will lock up the SDA line and not release it until a reset of the switch.
		// Bad news.
		OS_Tasker::Delay( I2C_INTER_READ_DELAY );

		if ( i2c_read == true )
		{
			if ( Is_Power_Down( port_ena, ( static_cast<uint8_t>( PORT_CTRL13_POWER_DOWN ) ) ) ==
				 false )
			{
				config_word->fields.port_enable = PORT_ENABLE;
				// Read PHY register to check is Ethernet link is good
				uint8_t link_good_val = 0U;
				reg_addr = KSZ8863_REG.link_stus + port;
				i2c_read = KSZ_I2CRead( reg_addr, 1U, &link_good_val );
				OS_Tasker::Delay( I2C_INTER_READ_DELAY );

				if ( i2c_read == true )
				{
					if ( Is_Link_Good( link_good_val,
									   ( static_cast<uint8_t>( PORT_STATUS0_LINK_GOOD ) ) ) ==
						 true )
					{
						config_word->fields.link_good = LINK_UP;
						// Read auto negotiation status
						uint8_t auto_neg = 0U;
						reg_addr = KSZ8863_REG.port_ctrl + port;
						i2c_read = KSZ_I2CRead( reg_addr, 1U, &auto_neg );
						OS_Tasker::Delay( I2C_INTER_READ_DELAY );

						if ( i2c_read == true )
						{
							if ( Is_Auto_Neg_Enable( auto_neg,
													 ( static_cast<uint8_t>( PORT_CTRL12_AUTO_NEGO_ENABLE ) ) ) ==
								 true )
							{
								/* AUTO_NEG is enabled now check if AUTO NEG is completed */
								config_word->fields.auto_neg = AUTO_NEG_ENABLE;

								// auto negotiation enabled
								if ( Is_Auto_Neg_Complete( link_good_val,
														   ( static_cast<uint8_t>
															 ( PORT_STATUS0_AUTO_NEGO_DONE ) ) ) == true )
								{
									config_word->fields.auto_neg_done = AUTO_NEG_DONE;
								}
								else
								{
									config_word->fields.auto_neg_done = AUTO_NEG_IN_PROGRESS;
								}
								// validate for valid speed indication
								if ( Is_Invalid_Speed( INVALID_SPEED_MASK, link_good_val ) == true )
								{
									config_word->fields.speed = SPEED_INVALID;	// 00
									i2c_read = false;
								}
							}
							else
							{
								config_word->fields.auto_neg = AUTO_NEG_DISABLE;
							}
							/* PHY link speed and duplex status bits are valid only if
							 * auto neg is disabled and valid link is detected or
							 * auto neg is enabled and auto neg is completed */
							if ( ( i2c_read == true ) &&
								 ( ( config_word->fields.auto_neg == AUTO_NEG_DISABLE ) ||
								   ( config_word->fields.auto_neg_done == AUTO_NEG_DONE ) ) )
							{
								// Read port's speed and duplex value from register
								uint8_t speed_duplex = 0U;
								reg_addr = KSZ8863_REG.port_stus + port;
								i2c_read = KSZ_I2CRead( reg_addr, 1U, &speed_duplex );
								OS_Tasker::Delay( I2C_INTER_READ_DELAY );

								if ( i2c_read == true )
								{
									// now we have valid speed/duplex indications
									if ( Is_100mbps_Speed( speed_duplex,
														   ( static_cast<uint8_t>
															 ( PORT_STATUS1_OPERATION_SPEED ) ) ) ==
										 true )
									{
										config_word->fields.speed = SPEED_100MBPS;
									}
									else
									{
										config_word->fields.speed = SPEED_10MBPS;
									}

									if ( Is_Full_Duplex( speed_duplex,
														 ( static_cast<uint8_t>
														   ( PORT_STATUS1_OPERATION_DUPLEX ) ) ) ==
										 true )
									{
										config_word->fields.duplex = FULL_DUPLEX;
									}
									else
									{
										config_word->fields.duplex = HALF_DUPLEX;
									}
								}
							}
						}
					}
				}
			}
		}
		Update_PHY_Error_Counts( port, config_word );
	}
	else
	{
		// comment to avoid misra warning
	}
	return ( i2c_read );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t KSZ8863::Put_Own_MAC_Address_In_Table( uint8_t* pMAC )
{
	uint8_t i;
	uint8_t Reg_Address;
	bool_t result = TRUE;

	BF_Lib::SPLIT_UINT64 split_u64;

	// Set mac address received
	split_u64.u32[0] = 0U;
	split_u64.u32[0] += pMAC[3];
	split_u64.u32[0] <<= 8U;
	split_u64.u32[0] += pMAC[2];
	split_u64.u32[0] <<= 8U;
	split_u64.u32[0] += pMAC[1];
	split_u64.u32[0] <<= 8U;
	split_u64.u32[0] += pMAC[0];
	split_u64.u32[1] = 0U;
	split_u64.u32[1] += pMAC[5];
	split_u64.u32[1] <<= 8U;
	split_u64.u32[1] += pMAC[4];
	split_u64.u32[1] &= 0x0000FFFFU;
	split_u64.u32[1] |= static_cast<uint32_t>( KSZ_STATIC_TABLE_FORWARDPORTS_1 );
	split_u64.u32[1] |= static_cast<uint32_t>( KSZ_STATIC_TABLE_VALIDENTRY );
	split_u64.u32[1] &= static_cast<uint32_t>( ~KSZ_STATIC_TABLE_OVERRIDE );
	split_u64.u32[1] &= static_cast<uint32_t>( ~KSZ_STATIC_TABLE_USE_FID_MAC );

	Reg_Address = static_cast<uint8_t>( KSZ_INDIRECT_DATA_REG_0 );

	for ( i = 0U; i < 8U; i++ )
	{
		result = ( KSZ_I2CWrite( Reg_Address, 1U, split_u64.u8[i] ) && result ) ? true : false;
		OS_Tasker::Delay( I2C_INTER_WRITE_DELAY );

		Reg_Address--;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void KSZ8863::Initialize_PHY()
{
	uint8_t pMAC[NETIF_MAX_HWADDR_LEN] =
	{ 0U };

	m_reset_output->On();
	/* Recommended Delay by Micrel Field Application Engineer */
	OS_Tasker::Delay( SWITCH_RESET_DELAY );
	/* Minimum default delay. useful if above delay gets set to 0 by mistake.*/
	OS_Tasker::Delay( 12U );
	m_reset_output->Off();
	OS_Tasker::Delay( 150U );

	// Write Drive Strength
	KSZ_I2CWrite( static_cast<uint8_t>( KSZ_R14_GBL_CTRL12 ), 1U, KSZ_IO_DRIVE_STRENGTH );
	OS_Tasker::Delay( I2C_INTER_WRITE_DELAY );

	m_emac_handle->Update_PHY_Config_To_MAC( PHY_Driver::SPEED_100MBPS, PHY_Driver::FULL_DUPLEX );

	// Read Controller MAC address
	m_emac_handle->Get_MAC_Address( pMAC );
	// put mac address to PHY static table
	Put_Own_MAC_Address_In_Table( pMAC );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void KSZ8863::Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word )
{
	config_word->fields.rx_error_counter = m_KSZ_ReadError_Counts;
	config_word->fields.tx_error_counter = m_KSZ_WriteError_Counts;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* const* KSZ8863::Get_Phy_Driver_Error_List()
{
	return ( s_phy_err_name );
}
