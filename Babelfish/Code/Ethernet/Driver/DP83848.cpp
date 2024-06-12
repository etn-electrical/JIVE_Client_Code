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

#include "Includes.h"
#include "DP83848.h"
#include "Stm32_Eth_Driver.h"
#include "Babelfish_Assert.h"

const uint8_t* const DP83848::s_phy_err_name[] =
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
DP83848::DP83848( BF_Lib::Output* reset_ctrl, EMAC_Driver* emac_ctrl, bool_t switch_enable,
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
bool_t DP83848::Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							  uint16_t* read_ptr )
{	
	BF_ASSERT ( Valid_Phy_Addr( phy_addr ) );
	BF_ASSERT ( Valid_Reg_Addr( reg_addr ) );
	
	return ( m_emac_ctrl->SMIRead16Bits( static_cast<uint16_t>( phy_addr ),
										 static_cast<uint16_t>( reg_addr ), read_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t DP83848::Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							   uint16_t* write_ptr )
{
	BF_ASSERT ( Valid_Phy_Addr( phy_addr ) );
	BF_ASSERT ( Valid_Reg_Addr( reg_addr ) );
	return ( m_emac_ctrl->SMIWrite16Bits( phy_addr, reg_addr, write_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t DP83848::Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
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
			if ( config_word->fields.duplex == FULL_DUPLEX )// Duplex
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
	if ( config_word->fields.port_enable == PORT_DISABLE )	// port disable
	{
		reg_val = static_cast<uint16_t>( PHY_CONTROL_PWR_DWN );
		result = ( ( Write_PHY_Reg( Phy_port, reg_addr, &reg_val ) && result ) ? true : false );
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t DP83848::Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	uint16_t reg_addr;
	bool_t read_status;
	uint16_t Phy_port;
	uint16_t basic_control_value;
	uint16_t basic_status_value;
	uint16_t phy_link_status_value;
	static bool_t OneTimeMACConfigFlag = true;
	static uint8_t pre_speed = 0xFF;

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
						/* auto neg is enabled now check if it is completed */
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
						reg_addr = static_cast<uint16_t>( PHY_PHYSTS_REG );
						read_status = Read_PHY_Reg( Phy_port, reg_addr, &phy_link_status_value );

						if ( read_status )
						{
							/* now we have valid speed/duplex indications */
							if ( Is_10mbps_Speed( phy_link_status_value ) )
							{
								config_word->fields.speed = SPEED_10MBPS;	// bit values '01'
							}
							else
							{
								config_word->fields.speed = SPEED_100MBPS;	// bit values '10'
							}

							if ( Is_Full_Duplex( phy_link_status_value ) )
							{
								config_word->fields.duplex = FULL_DUPLEX;
							}
							else
							{
								config_word->fields.duplex = HALF_DUPLEX;
							}

							/* When all link status received,
							 * set the MACCR register with detected link speed and duplex status */
							if ( ( pre_speed != config_word->fields.speed ) &&
								 ( m_switch_enable == false ) )
							{
								pre_speed = config_word->fields.speed;
								m_emac_ctrl->Update_PHY_Config_To_MAC( pre_speed, FULL_DUPLEX );
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
void DP83848::Initialize_PHY()
{
	uint16_t reg_val = PHY_RESET;
	uint32_t timeout = 0U;

	// Specification mentions at least 1 microsecond low pulse for hard reset, we will give 1 ms
	m_reset_output->Off();
	OS_Tasker::Delay( 1U );
	m_reset_output->On();

	// PHY Software Reset
	Write_PHY_Reg( m_PhyAddr, PHY_CONTROL_REG, &reg_val );

	// Wait for the reset to complete
	do
	{
		timeout++;
		Read_PHY_Reg( m_PhyAddr, PHY_CONTROL_REG, &reg_val );
	} while ( ( ( reg_val & PHY_RESET ) == PHY_RESET ) &&
			  ( timeout < static_cast<uint32_t>( PHY_READ_TO ) ) );

	// Delay as per specification
	OS_Tasker::Delay( 3U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DP83848::Update_PHY_Error_Counts( uint16_t Phy_port, PHY_Driver::port_config_t* config_word,
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
		config_word->fields.jabber = REMOTE_FAULT_ON;
	}

	reg_addr = static_cast<uint16_t>( PHY_ANLPAR_REG );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status );

	if ( read_status )
	{
		if ( Is_Link_Partner_Ack( basic_status, static_cast<uint16_t>( PHY_AN_LINK_PARTNER_ACK ) ) )
		{
			config_word->fields.link_partner_ack = LINK_PARNTER_ACK_ENABLE;
		}
	}

	reg_addr = static_cast<uint16_t>( PHY_ANER_REG );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status );

	if ( read_status )
	{
		if ( Is_Parallel_Detect_Fault( basic_status, static_cast<uint16_t>( PHY_AN_PARALLEL_DETECT_FAULT ) ) )
		{
			config_word->fields.parallel_detect_flt = PARALLEL_DETECT_FAULT_ON;
		}

		if ( Is_Page_Rx( basic_status, static_cast<uint16_t>( PHY_AN_PAGE_RX ) ) )
		{
			config_word->fields.page_rx = PAGE_RX;
		}
	}

	reg_addr = static_cast<uint16_t>( PHY_RECR_REG );
	read_status = Read_PHY_Reg( Phy_port, reg_addr, &basic_status );
	if ( read_status == false )
	{
		config_word->fields.rx_error_counter++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const uint8_t* const* DP83848::Get_Phy_Driver_Error_List()
{
	return ( s_phy_err_name );
}