/**
 **********************************************************************************************
 *  @file           I2C_Manager.cpp C++ Implementation File for I2C Manager module.
 *
 *  @brief          The file shall include the definitions of all the functions required for
 *                  I2C communication(Only for OS based systems)and which are declared in
 *                  corresponding header file
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @remark         Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "I2C_Manager.h"

volatile bool_t I2C_Manager::m_transfer_finish = false;
// OS_Binary_Semaphore* I2C_Manager::I2CBinSemaphore =
// static_cast<OS_Binary_Semaphore*>( NULL );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
I2C_Manager::I2C_Manager( uint8_t channel, uint16_t device_address,
						  uC_I2C::i2c_mode_t speed_mode, uC_I2C::address_type_t addr_mode,
						  uC_I2C::duty_type_t fastmode_duty_cycle )
{
	// I2CBinSemaphore = new OS_Binary_Semaphore( false );
	m_i2c_control = new uC_I2C( channel, device_address, speed_mode, addr_mode,
								false, true, fastmode_duty_cycle );
	m_i2c_control->Attach_Callback( &I2C_Manager::Transfer_Finish_Static, this );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Read_Data( uint16_t address, uint16_t length,
							 uint8_t* datasource, uC_I2C::i2c_status_t* read_status )
{
	I2C_Manager::m_transfer_finish = false;
	uC_I2C::i2c_status_t status;
	uint8_t uTimeout = 0;

	m_i2c_control->Read( address, length, datasource, &status );
	/* for receiving 1 byte it requires 90us @100Khz. So for receiving 11 byte it will require approx 1 ms
	   We don't expect the number of bytes more than 16 bytes. However this loop still can receive 33 bytes*/
	while ( ( false == I2C_Manager::m_transfer_finish ) && ( 0x03 > uTimeout ) )
	{
		OS_Tasker::Delay( 1U );	// I2C_Manager::I2CBinSemaphore->Pend( OS_TICK_RATE_MS );
		uTimeout++;
	}

	if ( I2C_Manager::m_transfer_finish == false )
	{
		if ( uC_I2C::I2C_DMA_TX_BUSY == status )
		{
			*read_status = uC_I2C::BYTE_RX_ERROR;
		}
		else
		{
			*read_status = status;
		}
	}
	else
	{
		*read_status = uC_I2C::I2C_FINISH_RX;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Write_Data( uint16_t address, uint16_t length,
							  uint8_t* datasource, uC_I2C::i2c_status_t* write_status )
{
	I2C_Manager::m_transfer_finish = false;
	uC_I2C::i2c_status_t status;
	uint8_t uTimeout = 0;

	m_i2c_control->Write( address, length, datasource, &status );
	/* for transmitting 1 byte it requires 90us @100Khz. So for transmiiting 11 byte it will require approx 1 ms
	   We dont expect the number of bytes more than 16 bytes. However this loop still can transmit 33 bytes*/
	while ( ( false == I2C_Manager::m_transfer_finish ) && ( 0x03 > uTimeout ) )
	{
		OS_Tasker::Delay( 1U );	// I2C_Manager::I2CBinSemaphore->Pend( OS_TICK_RATE_MS );
		uTimeout++;
	}

	if ( I2C_Manager::m_transfer_finish == false )
	{
		if ( uC_I2C::I2C_DMA_TX_BUSY == status )
		{
			*write_status = uC_I2C::BYTE_TX_ERROR;
		}
		else
		{
			*write_status = status;
		}
	}
	else
	{
		*write_status = uC_I2C::I2C_FINISH_TX;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void I2C_Manager::Transfer_Finish( void )
{
	I2C_Manager::m_transfer_finish = true;
	// static bool_t xTaskWoken;
	// I2CBinSemaphore->Post_From_ISR( &xTaskWoken );
}
