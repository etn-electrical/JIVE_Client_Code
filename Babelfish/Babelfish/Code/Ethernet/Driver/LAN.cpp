/**
 **********************************************************************************************
 * @file            LAN source File for driver class.
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

#include "Includes.h"
#include "LAN.h"
#include "lwip/netif.h"
#include "Stm32_Eth_Driver.h"
#include "Babelfish_Assert.h"

const uint8_t* const LAN::s_phy_err_name[] =
{
	"link_up",
	"link_down",
	"jabber",
	"remote_fault",
	"link_partner_ack",
	"parallel_detect_fault",
	"page_rx",
	"comm_read_err",
	"na"
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
LAN::LAN( BF_Lib::Output* reset_ctrl, EMAC_Driver* emac_ctrl, bool_t switch_enable,
		  bool_t alr_learn,
		  uint8_t PhyAddr ) :
	m_reset_output( reset_ctrl ),
	m_emac_ctrl( emac_ctrl ),
	m_switch_enable( switch_enable ),
	m_alr_learn( alr_learn ),
	m_PhyAddr( PhyAddr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
						  uint16_t* read_ptr )
{
	BF_ASSERT( Valid_Phy_Addr( phy_addr ) );
	BF_ASSERT( Valid_Reg_Addr( reg_addr ) );
	return ( m_emac_ctrl->SMIRead16Bits( static_cast<uint16_t>( phy_addr ),
										 static_cast<uint16_t>( reg_addr ), read_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
						   uint16_t* write_ptr )
{
	BF_ASSERT( Valid_Phy_Addr( phy_addr ) );
	BF_ASSERT( Valid_Reg_Addr( reg_addr ) );
	return ( m_emac_ctrl->SMIWrite16Bits( phy_addr, reg_addr, write_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	uint16_t reg_val;
	uint16_t reg_addr;
	bool_t result = true;
	uint16_t Phy_port;

	Phy_port = static_cast<uint16_t>( port ) + static_cast<uint16_t>( m_PhyAddr );
	reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );

	// port enable /disable
	if ( config_word->fields.port_enable == PORT_ENABLE )
	{
		reg_val = 0U;
		if ( config_word->fields.auto_neg == AUTO_NEG_ENABLE )	// auto_neg
		{
			reg_val = static_cast<uint16_t>( PHY_AN_ENABLED );
			result = Write_PHY_Reg( Phy_port, reg_addr, &reg_val );
		}
		else
		{
			if ( config_word->fields.duplex == FULL_DUPLEX )// duplex
			{
				reg_val |= static_cast<uint16_t>( PHY_DUPLEX_FULL );
			}
			if ( config_word->fields.speed == SPEED_100MBPS )	// speed
			{
				reg_val |= static_cast<uint16_t>( PHY_SPEED_100M );
			}

			result = Write_PHY_Reg( Phy_port, reg_addr, &reg_val );
		}
	}
	if ( config_word->fields.port_enable == PORT_DISABLE )
	{
		reg_val = static_cast<uint16_t>( PHY_STATUS_PWR_DWN );
		result = ( ( Write_PHY_Reg( Phy_port, reg_addr, &reg_val ) && result ) ? true : false );
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	uint16_t reg_addr;
	bool_t read_status;
	uint16_t Phy_port;
	uint16_t basic_control_value = 0U;
	uint16_t basic_status_value = 0U;
	uint16_t special_control_status_value = 0U;
	static uint8_t pre_port_setting = 0xFF;
	uint8_t port_setting = 0U;

	Phy_port = static_cast<uint16_t>( port ) + static_cast<uint16_t>( m_PhyAddr );

	reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_control_value );

	if ( read_status )
	{
		if ( !Is_Power_Down( basic_control_value, static_cast<uint16_t>( PHY_STATUS_PWR_DWN ) ) )
		{
			config_word->fields.port_enable = PORT_ENABLE;

			reg_addr = static_cast<uint16_t>( PHY_STATUS_REG );
			read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status_value );

			if ( read_status )
			{
				if ( Is_Link_Good( basic_status_value, static_cast<uint16_t>( PHY_STATUS_LINK ) ) )
				{
					config_word->fields.link_good = LINK_UP;

					if ( Is_Auto_Neg_Enable( basic_control_value,
											 static_cast<uint16_t>( PHY_AN_ENABLED ) ) )
					{
						/*** AUTO_NEG IS ENABLED, NOW CHECK IF AUTO NEG IS COMPLETED *****/
						config_word->fields.auto_neg = AUTO_NEG_ENABLE;

						/* auto negotiation enabled */
						if ( Is_Auto_Neg_Complete( basic_status_value,
												   static_cast<uint16_t>( PHY_AN_FINISH ) ) )
						{
							config_word->fields.auto_neg_done = AUTO_NEG_DONE;

						}
						else
						{
							config_word->fields.auto_neg_done = AUTO_NEG_IN_PROGRESS;
						}
					}
					/* PHY link speed and duplex status bits are valid only if
					 * auto neg is disabled and valid link is detected or
					 * auto neg is enabled and auto neg is completed */
					if ( ( config_word->fields.auto_neg == AUTO_NEG_DISABLE ) ||
						 ( config_word->fields.auto_neg_done == AUTO_NEG_DONE ) )
					{
						reg_addr = static_cast<uint16_t>( PHY_SCSR_REG );
						read_status = Read_PHY_Reg( Phy_port, reg_addr,
													&special_control_status_value );

						if ( read_status )
						{
							if ( Is_Invalid_Speed( special_control_status_value ) )
							{
								config_word->fields.speed = SPEED_INVALID;
								config_word->fields.duplex = HALF_DUPLEX;
							}
							else
							{
								/* now we have valid speed/duplex indications */

								if ( Is_100mbps_Speed( special_control_status_value ) )
								{
									config_word->fields.speed = SPEED_100MBPS;	// bit values '10'
								}
								else
								{
									config_word->fields.speed = SPEED_10MBPS;	// bit values '01'
								}
								if ( Is_Full_Duplex( special_control_status_value ) )
								{
									config_word->fields.duplex = FULL_DUPLEX;
								}
								else
								{
									config_word->fields.duplex = HALF_DUPLEX;
								}
								/* storing the port and duplex information to be compared with previous polled
								   information */
								port_setting = ( config_word->fields.duplex << 4 ) | config_word->fields.speed;

								if ( ( ( port_setting != pre_port_setting ) ) &&
									 ( m_switch_enable == false ) )
								{
									/* Update the MAC register with change in port information */
									pre_port_setting = port_setting;
									m_emac_ctrl->Update_PHY_Config_To_MAC( config_word->fields.speed,
																		   config_word->fields.duplex );// FULL_DUPLEX
																										// );
								}
							}
						}
						else
						{
							config_word->fields.speed = SPEED_INVALID;
							config_word->fields.duplex = HALF_DUPLEX;
						}
					}
				}
				Update_PHY_Error_Counts( Phy_port, config_word, basic_status_value );
			}
		}
	}
	return ( read_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::SMIReadSwitchFabricCSR( UINT16 csr_addr, UINT32* read_ptr )
{
	UINT32 command_reg;
	SINT16 retry = SWITCH_FABRIC_READ_RETRY;
	bool_t busy_cleared = false;
	bool_t ret_val = false;
	bool_t loop_exit = false;
	bool_t function_end = false;

	// step 1. write command register
	command_reg = static_cast<uint32_t>( LAN9303_SWITCH_CSR_BUSY )
		| static_cast<uint32_t>( LAN9303_SWITCH_CSR_READ )
		| static_cast<uint32_t>( LAN9303_SWITCH_CSR_AUTO_INC_DISABLE )
		// | static_cast<uint32_t>(LAN9303_SWITCH_CSR_AUTO_DEC_DISABLE)    /*Commented as OR value is
		// 0*/
		// | static_cast<uint32_t>(LAN9303_SWITCH_CSR_BE_ALL_4_BYTES)      /*Commented as OR value is
		// 0*/
		| static_cast<uint32_t>( csr_addr );
	if ( false ==
		 m_emac_ctrl->SMIWrite32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_CMD ),
									  &command_reg ) )
	{
		ret_val = false;
	}
	else
	{
		// step 2. read command register until CRS_BUSY is 0U
		while ( ( retry > 0 ) && ( loop_exit == false ) )
		{
			retry--;
			if ( false ==
				 m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_CMD ),
											 &command_reg ) )
			{
				loop_exit = true;
				function_end = true;
				ret_val = false;
			}
			else
			{
				if ( 0U == ( command_reg & static_cast<uint32_t>( LAN9303_SWITCH_CSR_BUSY_MASK ) ) )
				{
					busy_cleared = true;
					loop_exit = true;
				}
				else
				{
					OS_Tasker::Delay( 1U );
				}
			}
		}

		if ( function_end == false )
		{
			if ( !busy_cleared )
			{
				ret_val = false;
			}	// step 3. read data register
			else
			{
				if ( false ==
					 m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_DATA ),
												 read_ptr ) )
				{
					ret_val = false;
				}
				else
				{
					ret_val = true;
				}
			}
		}
	}
	return ( ret_val );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::SMIWriteSwitchFabricCSR( UINT16 csr_addr, const UINT32* write_ptr )
{
	UINT32 command_reg;
	SINT16 retry = SWITCH_FABRIC_WRITE_RETRY;
	bool_t busy_cleared = false;
	bool_t ret_val = false;
	bool_t loop_exit = false;
	bool_t function_end = false;

	// step 1. write data register
	if ( false ==
		 m_emac_ctrl->SMIWrite32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_DATA ),
									  write_ptr ) )
	{
		ret_val = false;
	}
	else
	{
		// step 2. write command register
		command_reg = static_cast<uint32_t>( LAN9303_SWITCH_CSR_BUSY )
			| static_cast<uint32_t>( LAN9303_SWITCH_CSR_WRITE ) | csr_addr;
		// | static_cast<uint32_t>(LAN9303_SWITCH_CSR_AUTO_INC_DISABLE)    /*Commented as OR value is
		// 0*/
		// | static_cast<uint32_t>(LAN9303_SWITCH_CSR_AUTO_DEC_DISABLE)    /*Commented as OR value is
		// 0*/
		// | static_cast<uint32_t>(LAN9303_SWITCH_CSR_BE_ALL_4_BYTES)      /*Commented as OR value is
		// 0*/
		// | csr_addr;
		if ( false ==
			 m_emac_ctrl->SMIWrite32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_CMD ),
										  &command_reg ) )
		{
			ret_val = false;
		}
		else
		{

			// step 3. read command register until CRS_BUSY is 0U
			while ( retry > 0 && ( loop_exit == false ) )
			{
				retry--;
				if ( false ==
					 m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_SWITCH_CSR_CMD ),
												 &command_reg ) )
				{
					loop_exit = true;
					function_end = true;
					ret_val = false;
				}
				else
				{
					if ( 0U == ( command_reg & LAN9303_SWITCH_CSR_BUSY_MASK ) )
					{
						busy_cleared = true;
						loop_exit = true;
					}
					else
					{
						OS_Tasker::Delay( 1U );
					}
				}
			}

			if ( function_end == false )
			{
				if ( !busy_cleared )
				{
					ret_val = false;
				}
				else
				{
					ret_val = true;
				}
			}
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::ClearALRCmd()
{
	UINT32 data = 0U;

	return ( SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD ), &data ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL LAN::WaitForALRInitialized()
{
	UINT32 data = 0U;
	SINT16 retry = SWE_ALR_INITIALIZED_WAIT_RETRY;
	bool_t initialized = false;
	bool_t ret_val = false;
	bool_t loop_exit = false;
	bool_t function_end = false;

	while ( retry > 0 && ( loop_exit == false ) )
	{
		retry--;
		if ( false == SMIReadSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD_STS ), &data ) )
		{
			loop_exit = true;
			function_end = true;
			ret_val = false;
		}
		else
		{
			if ( ( static_cast<uint32_t>( SWE_ALR_INIT_DONE ) & data ) != 0U )
			{
				initialized = true;
				loop_exit = true;
			}
			else
			{
				OS_Tasker::Delay( 1U );
			}
		}
	}

	if ( function_end == false )
	{
		if ( !initialized )
		{
			ret_val = false;
		}
		else
		{
			ret_val = true;
		}
	}
	return ( ret_val );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::WaitForALRMakeFinish()
{
	UINT32 data = 0U;
	SINT16 retry = static_cast<int16_t>( SWE_ALR_MAKE_WAIT_RETRY );
	bool_t make_finished = false;
	bool_t ret_val = false;
	bool_t loop_exit = false;
	bool_t function_end = false;

	while ( retry > 0 && ( loop_exit == false ) )
	{
		retry--;
		if ( false == SMIReadSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD_STS ), &data ) )
		{
			loop_exit = true;
			function_end = true;
			ret_val = false;
		}
		else
		{
			if ( ( static_cast<uint32_t>( SWE_ALR_MAKE_PENDING ) & data ) == 0U )
			{
				make_finished = true;
				loop_exit = true;
			}
			else
			{
				OS_Tasker::Delay( 1U );
			}
		}
	}

	if ( function_end == false )
	{
		if ( !make_finished )
		{
			ret_val = false;
		}
		else
		{
			ret_val = true;
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::PollALREntry( UINT64* pRes )
{
	UINT32 data = 0U;

	BF_Lib::SPLIT_UINT64 split_u64;
	bool_t ret_val;

	if ( false == SMIReadSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_RD_DAT_0 ), &data ) )
	{
		ret_val = false;
	}
	else
	{
		split_u64.u32[0] = data;
		if ( false == SMIReadSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_RD_DAT_1 ), &data ) )
		{
			ret_val = false;
		}
		else
		{
			split_u64.u32[1] = data;
			*pRes = split_u64.u64;
			ret_val = true;
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::InitiateGetALREntry( UINT64* pRes, UINT32 alr_cmd )
{
	UINT32 data;
	bool_t ret_val;

	data = alr_cmd;
	if ( false == SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD ), &data ) )
	{
		ret_val = false;
	}
	else
	{
		data = SWE_ALR_ALL_CLEARED;
		if ( false == SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD ), &data ) )
		{
			ret_val = false;
		}
		else
		{
			ret_val = PollALREntry( pRes );
		}
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::InitiateGetFirstALREntry( UINT64* pRes )
{
	return ( InitiateGetALREntry( pRes,
								  static_cast<uint32_t>( SWE_ALR_GET_FIRST_ENTRY_ENABLED ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::InitiateGetNextALREntry( UINT64* pRes )
{
	return ( InitiateGetALREntry( pRes, static_cast<uint32_t>( SWE_ALR_GET_NEXT_ENTRY_ENABLED ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::WriteALREntry( UINT64 entry )
{
	UINT32 data;

	BF_Lib::SPLIT_UINT64 su64;

	su64.u64 = entry;
	bool_t ret_val;

	if ( false ==
		 SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_WR_DAT_0 ), &su64.u32[0] ) )
	{
		ret_val = false;
	}
	else if ( false ==
			  SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_WR_DAT_1 ), &su64.u32[1] ) )
	{
		ret_val = false;
	}
	else
	{
		data = static_cast<uint32_t>( SWE_ALR_MAKE_ENTRY_ENABLED );
		if ( false == SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD ), &data ) )
		{
			ret_val = false;
		}
		else
		{
			if ( false == WaitForALRMakeFinish() )
			{
				ret_val = false;
			}
			else
			{
				data = SWE_ALR_MAKE_ENTRY_DISABLED;
				if ( false ==
					 SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_ALR_CMD ), &data ) )
				{
					ret_val = false;
				}
				else
				{
					ret_val = true;
				}
			}
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LAN::InvalidateEntry( UINT64* entry ) const
{
	BF_Lib::SPLIT_UINT64 su64;

	su64.u64 = *entry;
	su64.u32[1] &= ~( static_cast<uint32_t>( SWE_ALR_WR_DAT_1_VALID ) );
	*entry = su64.u64;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::IsEntryValid( UINT64 entry ) const
{
	bool_t ret_val;

	BF_Lib::SPLIT_UINT64 split_u64_entry;

	split_u64_entry.u64 = entry;
	// return ( split_u64_entry.u32[1] & (static_cast<uint32_t>(SWE_ALR_RD_DAT_1_VALID)) ? true :
	// false);
	if ( ( split_u64_entry.u32[1] & ( static_cast<uint32_t>( SWE_ALR_RD_DAT_1_VALID ) ) ) == 0U )
	{
		ret_val = false;
	}
	else
	{
		ret_val = true;
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::IsEntryEndOfTable( UINT64 entry ) const
{
	bool_t ret_val;

	BF_Lib::SPLIT_UINT64 split_u64_entry;

	split_u64_entry.u64 = entry;
	// return ( split_u64_entry.u32[1] & (static_cast<uint32_t>(SWE_ALR_RD_DAT_1_END_OF_TABLE )) ?
	// true : false);
	if ( ( split_u64_entry.u32[1] & ( static_cast<uint32_t>( SWE_ALR_RD_DAT_1_END_OF_TABLE ) ) ) ==
		 0U )
	{
		ret_val = false;
	}
	else
	{
		ret_val = true;
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::DisableALRLearning()
{
	UINT32 data = 0U;
	bool_t ret_val;

	if ( false == SMIReadSwitchFabricCSR( static_cast<uint16_t>( SWE_PORT_INGRSS_CFG ), &data ) )
	{
		ret_val = false;
	}
	else
	{
		data &= static_cast<uint32_t>( SWE_PORT_INGRESS_CFG_NON_RESERVED_MASK );
		data &= ~( static_cast<uint32_t>( SWE_PORT_INGRSS_CFG_ENABLE_LEARNING_MASK ) );
		// data |= static_cast<uint32_t>(SWE_PORT_INGRSS_CFG_LEARNING_DISABLED);   /*Commented as OR
		// value is 0*/
		if ( false ==
			 SMIWriteSwitchFabricCSR( static_cast<uint16_t>( SWE_PORT_INGRSS_CFG ), &data ) )
		{
			ret_val = false;
		}
		else
		{
			ret_val = true;
		}
	}
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// call DisableALRLearning() before this function
bool_t LAN::RemoveAllALREntries()
{
	bool_t access_success = false;
	UINT64 u64 = 0U;
	bool_t last_entry = false;
	bool_t ret_val = true;
	bool_t loop_exit = false;
	bool_t function_end = false;

	do
	{
		access_success = InitiateGetFirstALREntry( &u64 );

		// LAN9303 manual:
		// 3. Poll the Valid and End of Table bits in the Switch Engine ALR
		// Read Data 1 Register (SWE_ALR_RD_DAT_1) until either is set
		while ( ( access_success ) &&
				( false == IsEntryValid( u64 ) ) &&
				( false == IsEntryEndOfTable( u64 ) ) )
		{
			OS_Tasker::Delay( static_cast<uint16_t>( 1U ) );
			access_success = PollALREntry( &u64 );
		}
		if ( false == access_success )
		{
			ret_val = false;
			loop_exit = true;
			function_end = true;
		}
		if ( function_end == false )
		{
			if ( true == IsEntryValid( u64 ) )
			{
				InvalidateEntry( &u64 );
				if ( false == WriteALREntry( u64 ) )
				{
					ret_val = false;
					loop_exit = true;
					function_end = true;
				}
			}
			if ( function_end == false )
			{
				if ( true == IsEntryEndOfTable( u64 ) )
				{
					last_entry = true;
				}
			}
		}
	} while ( ( false == last_entry ) && ( loop_exit == false ) );

	return ( ret_val );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t LAN::Put_Own_MAC_Address_In_Table( uint8_t* pMAC )
{
	BF_Lib::SPLIT_UINT64 split_u64;

	// ETH_GetMACAddress(ETH_MAC_Address0, pMAC);

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

	split_u64.u32[1] |= static_cast<uint32_t>( SWE_ALR_WR_DAT_1_VALID );
	split_u64.u32[1] |= static_cast<uint32_t>( SWE_ALR_WR_DAT_1_STATIC );
	split_u64.u32[1] &= ~static_cast<uint32_t>( SWE_ALR_WR_DAT_1_PORT_MASK );
	// split_u64.u32[1] |= static_cast<uint32_t>(SWE_ALR_WR_DAT_1_PORT_0); /*Commented as OR value is
	// 0*/

	return ( WriteALREntry( split_u64.u64 ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LAN::WaitFor9303SwichHardwareReady()
{
	UINT32 test_number = 0U;

	m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_BYTE_TEST ), &test_number );

	while ( static_cast<uint32_t>( LAN9303_BYTE_TEST_VALID_PATTERN ) != test_number )
	{
		OS_Tasker::Delay( 1U );
		m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_BYTE_TEST ), &test_number );
	}

	m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_HW_CFG ), &test_number );

	while ( 0U == ( static_cast<uint32_t>( LAN9303_HW_CFG_READY ) & test_number ) )
	{
		OS_Tasker::Delay( 1U );
		m_emac_ctrl->SMIRead32Bits( static_cast<uint16_t>( LAN9303_HW_CFG ), &test_number );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LAN::Initialize_PHY()
{

	uint8_t pMAC[NETIF_MAX_HWADDR_LEN];

	m_reset_output->On();
	/* Recommended Delay by Micrel Field Application Engineer */
	// OS_Tasker::Delay( SWITCH_RESET_DELAY );
	/* Minimum default delay. useful if above delay gets set to 0 by mistake.*/
	OS_Tasker::Delay( 15 );
	m_reset_output->Off();
	OS_Tasker::Delay( 2U );

	if ( m_switch_enable )
	{
		WaitFor9303SwichHardwareReady();
		Configure_Address_Learning( m_alr_learn );
		m_emac_ctrl->Get_MAC_Address( pMAC );
		Put_Own_MAC_Address_In_Table( pMAC );
		Virtual_Port_Config( static_cast<uint16_t>( LAN9303_VIRTUAL_PORT_ADDR ) );
	}
	else
	{
		// Virtual_Port_Config( static_cast<uint16_t>(0x04) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LAN::Configure_Address_Learning( bool_t alr_disable )
{
	if ( alr_disable == true )
	{
		DisableALRLearning();
		RemoveAllALREntries();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

bool_t LAN::Virtual_Port_Config( uint16_t PhyAddr )
{
	uint32_t timeout = 0U;
	uint16_t reg_val;
	uint16_t reg_addr;
	uint16_t basic_status_value = 0U;
	uint16_t special_control_status_value = 0U;
	bool_t result = true;
	uint8_t Speed = 0U;
	uint8_t Mode = 0U;
	bool_t LinkGoodStatus = false;
	bool_t AutoNegFinishStatus = false;
	bool_t loop_exit = false;

	reg_val = static_cast<uint16_t>( PHY_BASIC_CONTROL_PHY_RESET_SET );
	if ( !( Write_PHY_Reg( PhyAddr, static_cast<uint16_t>( PHY_BCR ), &reg_val ) ) )
	{
		/* result ERROR in case of write timeout */
		result = false;
	}

	/* Delay to assure PHY reset. As per 9303 manual it requires 1uSec to reset. We wait for 1 mSec
	 */
	OS_Tasker::Delay( 1U );

	/* We wait for linked satus... */
	reg_addr = static_cast<uint16_t>( PHY_STATUS_REG );

	do
	{
		timeout++;
		if ( Read_PHY_Reg( PhyAddr, reg_addr, &basic_status_value ) == true )
		{
			LinkGoodStatus = Is_Link_Good( basic_status_value,
										   static_cast<uint16_t>( PHY_AN_FINISH ) );
		}
		else
		{
			result = false;
			loop_exit = true;
		}
	} while ( ( LinkGoodStatus == false ) && ( timeout < static_cast<uint32_t>( PHY_READ_TO ) ) &&
			  ( loop_exit == false ) );

	// while (!(ETH_ReadPHYRegister(PHYAddress, PHY_BSR) & PHY_Linked_Status) && (timeout <
	// PHY_READ_TO));

	/* result ERROR in case of timeout */
	if ( timeout == static_cast<uint32_t>( PHY_READ_TO ) )
	{
		result = false;
	}
	/* Reset Timeout counter */
	timeout = 0U;

	/* Enable Auto-Negotiation */
	if ( true == result )
	{
		reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );
		reg_val = static_cast<uint16_t>( PHY_AN_ENABLED );

		if ( !( Write_PHY_Reg( PhyAddr, reg_addr, &reg_val ) ) )
		{
			result = false;
			/* result ERROR in case of write timeout */
		}
		if ( true == result )
		{
			reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );

			/* Wait until the autonegotiation will be completed */
			loop_exit = false;
			do
			{
				timeout++;
				if ( true == Read_PHY_Reg( PhyAddr, reg_addr, &basic_status_value ) )
				{
					AutoNegFinishStatus = Is_Auto_Neg_Complete( basic_status_value,
																static_cast<uint16_t>( PHY_AN_FINISH ) );
				}
				else
				{
					result = false;
					loop_exit = true;
				}
			} while ( ( true == AutoNegFinishStatus ) &&
					  ( timeout < static_cast<uint32_t>( PHY_READ_TO ) ) &&
					  ( loop_exit == false ) );

			/* result ERROR in case of timeout */
			if ( timeout == static_cast<uint32_t>( PHY_READ_TO ) )
			{
				result = false;
			}
			/* Reset Timeout counter */
			timeout = 0U;

			if ( true == result )
			{
				reg_addr = PHY_SCSR_REG;
				/* Read the result of the autonegotiation */
				result = Read_PHY_Reg( PhyAddr, reg_addr, &special_control_status_value );
				if ( result )
				{
					/* Configure the MAC with the Duplex Mode fixed by the autonegotiation process */
					if ( Is_Full_Duplex( special_control_status_value ) )
					{
						/* Set Ethernet duplex mode to FullDuplex following the autonegotiation */
						Mode = FULL_DUPLEX;
					}
					else
					{
						/* Set Ethernet duplex mode to HalfDuplex following the autonegotiation */
						Mode = HALF_DUPLEX;
					}
					/* Configure the MAC with the speed fixed by the autonegotiation process */
					if ( Is_100mbps_Speed( special_control_status_value ) )
					{
						/* Set Ethernet speed to 10M following the autonegotiation */
						Speed = SPEED_10MBPS;
					}
					else
					{
						/* Set Ethernet speed to 100M following the autonegotiation */
						Speed = SPEED_100MBPS;
					}
				}
			}
		}
		Mode = FULL_DUPLEX;
		Speed = SPEED_100MBPS;
		m_emac_ctrl->Update_PHY_Config_To_MAC( Speed, Mode );
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void LAN::Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word,
								   uint16_t basic_status )
{
	bool_t read_status;
	uint16_t reg_addr;

	if ( Is_Jabber_Detected( basic_status, static_cast<uint16_t>( PHY_JABBER_DETECTED ) ) )
	{
		config_word->fields.jabber = JABBER_DETECT_ON;
	}

	if ( Is_Remote_Fault( basic_status, static_cast<uint16_t>( PHY_REMOTE_FAULT ) ) )
	{
		config_word->fields.remote_fault = REMOTE_FAULT_ON;
	}

	basic_status = 0U;
	reg_addr = static_cast<uint16_t>( PHY_AN_LP_BASE_ABILITY );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status );

	if ( read_status )
	{
		if ( Is_Link_Partner_Ack( basic_status, static_cast<uint16_t>( PHY_AN_LINK_PARTNER_ACK ) ) )
		{
			config_word->fields.link_partner_ack = LINK_PARNTER_ACK_ENABLE;
		}
	}

	reg_addr = static_cast<uint16_t>( PHY_AN_EXP );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status );

	if ( read_status )
	{
		if ( Is_Parallel_Detect_Fault( basic_status,
									   static_cast<uint16_t>( PHY_AN_PARALLEL_DETECT_FAULT ) ) )
		{
			config_word->fields.parallel_detect_flt = PARALLEL_DETECT_FAULT_ON;
		}

		if ( Is_Page_Rx( basic_status, static_cast<uint16_t>( PHY_AN_PAGE_RX ) ) )
		{
			config_word->fields.page_rx = PAGE_RX;
		}
	}
	else
	{
		config_word->fields.rx_error_counter++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* const* LAN::Get_Phy_Driver_Error_List()
{
	return ( s_phy_err_name );
}
