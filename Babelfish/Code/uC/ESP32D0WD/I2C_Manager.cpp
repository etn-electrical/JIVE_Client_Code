/**
 **********************************************************************************************
 * @file			I2C_Manager.cpp
 * @brief			The file shall include the definitions of all the functions required for
 * @n				I2C communication(Only for OS based systems)and which are declared in
 * @n				corresponding header file
 * @details			See header file for module overview.
 * @copyright		2019 Eaton Corporation. All Rights Reserved.
 **********************************************************************************************
 */

#include "I2C_Manager.h"

volatile bool_t I2C_Manager::m_transfer_finish = false;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
I2C_Manager::I2C_Manager( uint8_t channel, uint16_t device_address,
						  uC_I2C::i2c_mode_t speed_mode, uC_I2C::address_type_t addr_mode,
						  uC_I2C::duty_type_t fastmode_duty_cycle, uC_BASE_I2C_IO_PORT_STRUCT const* io_port )
{
	m_i2c_control = new uC_I2C( channel, device_address, speed_mode, addr_mode,
								false, false, fastmode_duty_cycle, io_port );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Read_Data( uint16_t address, uint16_t length,
							 uint8_t* datasource, uC_I2C::i2c_status_t* read_status, i2c_cmd_handle_t cmd )
{
	I2C_Manager::m_transfer_finish = false;

	m_i2c_control->Read( address, length, datasource, read_status, cmd );
	Transfer_Finish();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Write_Data( uint16_t address, uint16_t length,
							  uint8_t* datasource, uC_I2C::i2c_status_t* write_status, i2c_cmd_handle_t cmd )
{
	I2C_Manager::m_transfer_finish = false;

	m_i2c_control->Write( address, length, datasource, write_status, cmd );
	Transfer_Finish();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Transfer_Finish( void )
{
	I2C_Manager::m_transfer_finish = true;
}
