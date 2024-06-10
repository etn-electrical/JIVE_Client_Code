/**
 **********************************************************************************************
 * @file            KSZ8081 source File for driver class.
 *
 * @brief           The file contains KSZ8081 driver class to read/write KSZ8081
 *                  single/dual port switch registers over a MII/RMII communication interface.
 * @details         This file is used to communicate KSZ8081.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "KSZ8081.h"
#include "netif.h"
#include "EthConfig.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Assert.h"

const uint8_t* const KSZ8081::s_phy_err_name[] =
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
KSZ8081::KSZ8081( BF_Lib::Output* reset_ctrl, EMAC_Driver* emac_ctrl, bool_t switch_enable,
				  bool_t alr_learn, uint8_t PhyAddr ) :
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
bool_t KSZ8081::Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr, uint16_t* read_ptr )
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
bool_t KSZ8081::Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
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
bool_t KSZ8081::Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
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
bool_t KSZ8081::Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
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
						reg_addr = static_cast<uint16_t>( PHY_CONTROL_1_STATUS_OFFSET );
						read_status = Read_PHY_Reg( Phy_port, reg_addr, &special_control_status_value );

						if ( read_status )
						{
							if ( Is_Invalid_Speed( special_control_status_value ) )
							{
								config_word->fields.speed = SPEED_INVALID;
								config_word->fields.duplex = SPEED_10MBPS;
							}

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
							/* storing the port and duplex information to be compared with previous
							   polled information */
							port_setting = ( config_word->fields.duplex << 4 ) | config_word->fields.speed;

							if ( ( ( port_setting != pre_port_setting ) ) &&
								 ( m_switch_enable == false ) )
							{
								/* Update the MAC register with change in port information */
								pre_port_setting = port_setting;
								m_emac_ctrl->Update_PHY_Config_To_MAC( config_word->fields.speed,
																	   config_word->fields.duplex );		// FULL_DUPLEX//
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
void KSZ8081::Initialize_PHY()
{
	uint16_t RegValue = 0U;
	bool status = false;

	/* Reset the PHY. */
	m_reset_output->On();
	/* Delay to assure PHY reset */
	OS_Tasker::Delay( PHY_RESET_DELAY );
	m_reset_output->Off();
	OS_Tasker::Delay( 2U );

	/* Read Phy Control Register again */
	status = Read_PHY_Reg( PHY_ADDR, PHY_CONTROL_REG, &RegValue );
	if ( false == status )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reading Physical Control Register Failed" );
	}

	/* Let the PHY know RMII is 50MHz */
	status = Read_PHY_Reg( PHY_ADDR, PHY_SCSR_REG, &RegValue );
	if ( false == status )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reading Special Control Status Register Failed" );
	}

	if ( EMAC_COMM_MODE == uC_EMAC_RMII_MODE )
	{
		RegValue |= PHY_SPECIAL_CONTROL_STATUS_RMII_CLK_SEL;
		RegValue |= PHY_SPECIAL_CONTROL_STATUS_HP_MDIX_SEL;
		status = Write_PHY_Reg( PHY_ADDR, PHY_SCSR_REG, &RegValue );
		if ( false == status )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Writing Special Control Status Register Failed for value: %u", RegValue );
		}

		status = Read_PHY_Reg( PHY_ADDR, PHY_SCSR_REG, &RegValue );
		if ( false == status )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reading Special Control Status Register Failed" );
		}
	}

	/* Enable PHY interrupts. */
	RegValue = ( PHY_KSZ8081_REG_INTRPT_JABBER |
				 PHY_KSZ8081_REG_INTRPT_RXERR |
				 PHY_KSZ8081_REG_INTRPT_PGERR |
				 PHY_KSZ8081_REG_INTRPT_PRLFAULT |
				 PHY_KSZ8081_REG_INTRPT_LPACK |
				 PHY_KSZ8081_REG_INTRPT_LINKDOWN |
				 PHY_KSZ8081_REG_INTRPT_REM_FAULT |
				 PHY_KSZ8081_REG_INTRPT_LINKUP ) << 8;
	status = Write_PHY_Reg( PHY_ADDR, PHY_KSZ8081_REG_INTRPT, &RegValue );
	if ( false == status )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Writing KSZ8081 Interrupt Register Failed for value: %u", RegValue );
	}

	/* Read PHY Control Register */
	status = Read_PHY_Reg( PHY_ADDR, PHY_CONTROL_REG, &RegValue );
	if ( false == status )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reading Physical Basic Control Register Failed" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void KSZ8081::Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word,
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
const uint8_t* const* KSZ8081::Get_Phy_Driver_Error_List()
{
	return ( s_phy_err_name );
}
