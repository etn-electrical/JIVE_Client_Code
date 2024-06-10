/**
 **********************************************************************************************
 * @file            IP101GRI source File for driver class.
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

#include "Includes.h"
#include "IP101GRI.h"
#include "lwip/netif.h"
#include "ESP32_Eth_Driver.h"
#include "IP101GRI_Reg.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Assert.h"
#include "driver/gpio.h"

static const uint8_t COUNTER_INIT = 0U;
static const uint8_t REG_VAL_INIT = 0U;
static const uint8_t REG_ADDR_INIT = 0U;
static const uint8_t PHY_PORT_INIT = 1U;
static const uint8_t BASIC_CONTROL_VAL_INIT = 0U;
static const uint8_t BASIC_STATUS_VAL_INIT = 0U;
static const uint8_t SPECIAL_CONTROL_STATUS_VAL_INIT = 0U;
static const uint8_t PRE_PORT_SETTING_INIT = 0xFF;
static const uint8_t PORT_SETTING_INIT = 0U;
static const uint8_t PAGE_CONTROL_REG_ADDR = 20U;
static const uint8_t PAGE_CONTROL_REG_VAL = 0x0010;
static const uint8_t RESET_TIMEOUT_DIV = 10;
static const uint8_t DELAY_MS_10 = 10;
static const uint8_t BASIC_STATUS = 0U;

const uint8_t* const IP101GRI::s_phy_err_name[] =
{
	( uint8_t* ) "link_up",
	( uint8_t* ) "link_down",
	( uint8_t* ) "jabber",
	( uint8_t* ) "remote_fault",
	( uint8_t* ) "link_partner_ack",
	( uint8_t* ) "parallel_detect_fault",
	( uint8_t* ) "page_rx",
	( uint8_t* ) "comm_read_err",
	( uint8_t* ) "na"
};
static const uint32_t PHY_ADDRESS_LIMIT = 16U;
static const uint32_t PHY_ADDRESS_MAX = 0xFFFF;
static const uint32_t PHY_ADDRESS_MIN = 0U;
static const uint32_t POWER_DOWN_ENABLE = 1U;
static const uint32_t POWER_DOWN_DISABLE = 0U;
static const uint32_t CLOCK_DIVISION = 10U;
static const uint32_t RESET_ENABLE = 1U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IP101GRI::IP101GRI( eth_phy_config_t phy_config, EMAC_Driver* emac_ctrl ) :
	m_PHY_Config( phy_config ),
	m_EMAC_Ctrl( emac_ctrl )
{
	IP101_Reset_Hardware();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IP101GRI::Read_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr, uint16_t* read_ptr )
{
	return ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( phy_addr ),
										 static_cast<uint16_t>( reg_addr ), read_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IP101GRI::Write_PHY_Reg( uint16_t phy_addr, uint16_t reg_addr,
							  uint16_t* write_ptr )
{
	return ( m_EMAC_Ctrl->SMIWrite16Bits( phy_addr, reg_addr, write_ptr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IP101GRI::Write_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	uint16_t reg_val = REG_VAL_INIT;
	uint16_t reg_addr = REG_ADDR_INIT;
	bool result = false;
	uint16_t phy_port = PHY_PORT_INIT;

	reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );

	// port enable /disable
	if ( config_word->fields.port_enable == PORT_ENABLE )
	{
		reg_val = REG_VAL_INIT;
		if ( config_word->fields.auto_neg == AUTO_NEG_ENABLE )
		{
			reg_val = static_cast<uint16_t>( PHY_AN_ENABLED );
			result = Write_PHY_Reg( phy_port, reg_addr, &reg_val );
		}
		else
		{
			if ( config_word->fields.duplex == FULL_DUPLEX )
			{
				reg_val |= static_cast<uint16_t>( PHY_DUPLEX_FULL );
			}
			if ( config_word->fields.speed == SPEED_100MBPS )
			{
				reg_val |= static_cast<uint16_t>( PHY_SPEED_100M );
			}

			result = Write_PHY_Reg( phy_port, reg_addr, &reg_val );
		}
	}
	if ( config_word->fields.port_enable == PORT_DISABLE )
	{
		reg_val = static_cast<uint16_t>( PHY_STATUS_PWR_DWN );
		result = ( ( Write_PHY_Reg( phy_port, reg_addr, &reg_val ) && result ) ? true : false );
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool IP101GRI::Read_Port_Config( uint8_t port, PHY_Driver::port_config_t* config_word )
{
	uint16_t reg_addr = REG_ADDR_INIT;
	bool read_status = false;
	uint16_t phy_port = PHY_PORT_INIT;
	uint16_t basic_control_value = BASIC_CONTROL_VAL_INIT;
	uint16_t basic_status_value = BASIC_STATUS_VAL_INIT;
	uint16_t special_control_status_value = SPECIAL_CONTROL_STATUS_VAL_INIT;
	static uint8_t pre_port_setting = PRE_PORT_SETTING_INIT;
	uint8_t port_setting = PORT_SETTING_INIT;
	uint16_t reg_val = REG_VAL_INIT;

	reg_addr = PAGE_CONTROL_REG_ADDR;
	reg_val = PAGE_CONTROL_REG_VAL;
	bool ret = Write_PHY_Reg( phy_port, reg_addr, &reg_val );

	if ( ret == true )
	{
		reg_addr = static_cast<uint16_t>( PHY_CONTROL_REG );
		read_status = Read_PHY_Reg( phy_port, reg_addr, &basic_control_value );
	}
	if ( read_status )
	{
		if ( !Is_Power_Down( basic_control_value, static_cast<uint16_t>( PHY_STATUS_PWR_DWN ) ) )
		{
			config_word->fields.port_enable = PORT_ENABLE;

			reg_addr = static_cast<uint16_t>( PHY_STATUS_REG );
			read_status = Read_PHY_Reg( phy_port, reg_addr, &basic_status_value );

			if ( read_status )
			{
				if ( Is_Link_Good( basic_status_value, static_cast<uint16_t>( PHY_STATUS_LINK ) ) )
				{
					config_word->fields.link_good = LINK_UP;

					if ( Is_Auto_Neg_Enable( basic_control_value,
											 static_cast<uint16_t>( PHY_AN_ENABLED ) ) )
					{
						/* AUTO_NEG IS ENABLED, NOW CHECK IF AUTO NEG IS COMPLETED */
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
						read_status = Read_PHY_Reg( phy_port, reg_addr,
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
									config_word->fields.speed = SPEED_100MBPS;
								}
								else
								{
									config_word->fields.speed = SPEED_10MBPS;
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

								if ( port_setting != pre_port_setting )
								{
									/* Update the MAC register with change in port information */
									pre_port_setting = port_setting;
									m_EMAC_Ctrl->Update_PHY_Config_To_MAC( config_word->fields.speed,
																		   config_word->fields.duplex );
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
				Update_PHY_Error_Counts( phy_port, config_word, basic_status_value );
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
esp_err_t IP101GRI::IP101_Reset_Hardware( void )
{
	if ( this->m_PHY_Config.reset_gpio_num >= 0 )
	{
		gpio_pad_select_gpio( ( gpio_num_t )this->m_PHY_Config.reset_gpio_num );
		gpio_set_direction( ( gpio_num_t )this->m_PHY_Config.reset_gpio_num, GPIO_MODE_OUTPUT );
		gpio_set_level( ( gpio_num_t )this->m_PHY_Config.reset_gpio_num, 0 );
		ets_delay_us( 100 );// insert min input assert time
		gpio_set_level( ( gpio_num_t )this->m_PHY_Config.reset_gpio_num, 1 );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t IP101GRI::Esp_Eth_Detect_Phy_Addr( uint32_t* detected_addr )
{
	if ( detected_addr == nullptr )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Detected_addr Can't Be Null" );
		return ( ESP_ERR_INVALID_ARG );
	}
	uint32_t addr_try = REG_ADDR_INIT;
	uint32_t reg_value = REG_VAL_INIT;

	for ( addr_try = REG_ADDR_INIT; addr_try < PHY_ADDRESS_LIMIT; addr_try++ )
	{
		bool ret = m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( addr_try ), ETH_PHY_IDR1_REG_ADDR,
											   reinterpret_cast<uint16_t*>( &reg_value ) );
		if ( ret == true )
		{
			if ( ( reg_value != PHY_ADDRESS_MAX ) && ( reg_value != PHY_ADDRESS_MIN ) )
			{
				*detected_addr = addr_try;
				break;
			}
		}
	}
	if ( addr_try < PHY_ADDRESS_LIMIT )
	{
		ETH_DEBUG_PRINT( DBG_MSG_DEBUG, "Found PHY address: %d", addr_try );
		return ( ESP_OK );
	}
	else
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "No PHY device detected" );
		return ( ESP_ERR_NOT_FOUND );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t IP101GRI::IP101_Pwrctl( bool enable )
{
	bmcr_reg_t bmcr;

	if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_BMCR_REG_ADDR,
									 reinterpret_cast<uint16_t*>( &( bmcr.val ) ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Read BMCR Failed" );
		return ( ESP_FAIL );

	}
	if ( !enable )
	{
		/* Enable IEEE Power Down Mode */
		bmcr.power_down = POWER_DOWN_ENABLE;
	}
	else
	{
		/* Disable IEEE Power Down Mode */
		bmcr.power_down = POWER_DOWN_DISABLE;
	}

	if ( m_EMAC_Ctrl->SMIWrite16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_BMCR_REG_ADDR,
									  reinterpret_cast<uint16_t*>( &( bmcr.val ) ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "write BMCR failed" );
		return ( ESP_FAIL );
	}

	if ( !enable )
	{
		if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_BMCR_REG_ADDR,
										 reinterpret_cast<uint16_t*>( &( bmcr.val ) ) ) != true )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "read BMCR failed" );
			return ( ESP_FAIL );

		}
		if ( ( bmcr.power_down == POWER_DOWN_ENABLE ) != ESP_OK )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "power down failed" );
			return ( ESP_FAIL );
		}
	}
	else
	{
		/* wait for power up complete */

		uint32_t counter = 0;
		for ( counter = 0; counter < this->m_PHY_Config.reset_timeout_ms / CLOCK_DIVISION; counter++ )
		{
			vTaskDelay( pdMS_TO_TICKS( CLOCK_DIVISION ) );
			if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ),
											 ETH_PHY_BMCR_REG_ADDR,
											 reinterpret_cast<uint16_t*>( &( bmcr.val ) ) ) != true )
			{
				ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Read BMCR Failed" );
				return ( ESP_FAIL );

			}
			if ( bmcr.power_down == POWER_DOWN_DISABLE )
			{
				break;
			}
		}
		if ( ( counter >= this->m_PHY_Config.reset_timeout_ms / CLOCK_DIVISION ) )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Power Up Timeout" );
			return ( ESP_FAIL );
		}
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t IP101GRI::IP101_Reset( void )
{
	bmcr_reg_t bmcr = { .reset = RESET_ENABLE };

	if ( m_EMAC_Ctrl->SMIWrite16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_BMCR_REG_ADDR,
									  reinterpret_cast<uint16_t*>( &bmcr.val ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "write BMCR failed" );
		return ( ESP_FAIL );

	}
	/* wait for reset complete */
	uint32_t counter = COUNTER_INIT;

	for ( counter = COUNTER_INIT; counter < this->m_PHY_Config.reset_timeout_ms / RESET_TIMEOUT_DIV; counter++ )
	{
		vTaskDelay( pdMS_TO_TICKS( DELAY_MS_10 ) );
		if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_BMCR_REG_ADDR,
										 reinterpret_cast<uint16_t*>( &( bmcr.val ) ) ) != true )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Read BMCR Failed" );
			return ( ESP_FAIL );

		}
		if ( !bmcr.reset )
		{
			break;
		}
	}
	if ( ( counter >= this->m_PHY_Config.reset_timeout_ms / CLOCK_DIVISION ) )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reset Timeout" );
		return ( ESP_FAIL );

	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t IP101GRI::IP101_Init( void )
{
	// Detect PHY address
	if ( this->m_PHY_Config.phy_addr == ESP_ETH_PHY_ADDR_AUTO )
	{

		if ( Esp_Eth_Detect_Phy_Addr( ( uint32_t* )&this->m_PHY_Config.phy_addr ) != ESP_OK )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Detect PHY address failed" );
			return ( ESP_FAIL );
		}
	}
	if ( IP101_Pwrctl( true ) != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Power Control Failed" );
		return ( ESP_FAIL );
	}

	/* Reset Ethernet PHY */
	if ( IP101_Reset() != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reset Failed" );
		return ( ESP_FAIL );
	}

	/* Check PHY ID */
	phyidr1_reg_t ID1;
	phyidr2_reg_t ID2;

	if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_IDR1_REG_ADDR,
									 reinterpret_cast<uint16_t*>( &( ID1.val ) ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Read ID1 failed" );
		return ( ESP_FAIL );
	}
	if ( m_EMAC_Ctrl->SMIRead16Bits( static_cast<uint16_t>( this->m_PHY_Config.phy_addr ), ETH_PHY_IDR2_REG_ADDR,
									 reinterpret_cast<uint16_t*>( &( ID2.val ) ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Read ID2 Failed" );
		return ( ESP_FAIL );
	}
	if ( ( ( ID1.oui_msb == 0x243 ) && ( ID2.oui_lsb == 0x3 ) && ( ID2.vendor_model == 0x5 ) ) != true )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Wrong Chip ID" );
		return ( ESP_FAIL );
	}

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IP101GRI::Initialize_PHY( void )
{
	/* IP101GR Initialization */
	IP101_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void IP101GRI::Update_PHY_Error_Counts( uint16_t phy_port, PHY_Driver::port_config_t* config_word,
										uint16_t basic_status )
{
	bool read_status = false;
	uint16_t reg_addr = 0U;

	/*Jabber Detected here*/
	if ( Is_Jabber_Detected( basic_status, static_cast<uint16_t>( PHY_JABBER_DETECTED ) ) )
	{
		config_word->fields.jabber = JABBER_DETECT_ON;
	}

	if ( Is_Remote_Fault( basic_status, static_cast<uint16_t>( PHY_REMOTE_FAULT ) ) )
	{
		config_word->fields.remote_fault = REMOTE_FAULT_ON;
	}

	basic_status = BASIC_STATUS;
	reg_addr = static_cast<uint16_t>( PHY_AN_LP_BASE_ABILITY );
	read_status = Read_PHY_Reg( phy_port, reg_addr, &basic_status );

	if ( read_status )
	{
		if ( Is_Link_Partner_Ack( basic_status, static_cast<uint16_t>( PHY_AN_LINK_PARTNER_ACK ) ) )
		{
			config_word->fields.link_partner_ack = LINK_PARNTER_ACK_ENABLE;
		}
	}

	reg_addr = static_cast<uint16_t>( PHY_AN_EXP );
	read_status = Read_PHY_Reg( phy_port, reg_addr, &basic_status );

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
const uint8_t* const* IP101GRI::Get_Phy_Driver_Error_List()
{
	return ( s_phy_err_name );
}

PHY_Driver::~PHY_Driver()
{}

IP101GRI::~IP101GRI()
{}
