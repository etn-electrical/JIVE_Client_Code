/**
 **********************************************************************************************
 *@file           I2C_Manager.h Header File for I2C implementation.
 *
 *@brief          The file shall wrap all the functions which are written on top of uC_I2C class
 *@brief          This file will take care of synchronus transfer using DMA. This need to be used
 *@brief          only  in case of OS based systems.
 *@details
 *@copyright      2014 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 *@Remark         The uC_I2C class is implemented to tansfer data using sysnchronous as well as
 *                asynchronous mode. Synchronous mode -> The call to functions Write() and
 *                Read() will exit only in case of successful data transfer or error
 *                Asynchronus mode -> The user shall call Write() or Read() function to
 *                initiate data transfer and exit. Once complete data is transmitted callback
 *                will be generated to notify program that trasaction is finished.
 *                Depending on this there are four combinations
 *                1. Asynchronus data transfer using DMA
 *                2. Asynchronus data transfer using simple I2C interrupts
 *                3. Synchronous data transfer using simple I2C mechanism
 *                4. Synchronous data transfer using DMA.
 *
 *                I2C Manager Class implements the type 4(Synchronous data transfer using DMA)
 *                here. This class can only be used only with OS_Takser based application
 **********************************************************************************************
 */
#ifndef __I2C_MANAGER_h__
#define __I2C_MANAGER_h__

#include "Includes.h"
#include "uC_I2C.h"
#include "Sys_arch.h"
#include "OS_Tasker.h"

/**
 **********************************************************************************************
 * @brief                   I2C_Manager Class Declaration. The class will handle all the
 *                          functionalities related to I2C(Synchronus DMA type) communication
 *                          in full RTOS based system
 *                          This class can only be used only with OS_Takser based application
 **********************************************************************************************
 */
class I2C_Manager
{
	public:

		/**
		 * @brief                   Constructor to create instance of uc_I2C class.
		 * @param[in] channel       The I2C channel of microcontroller which is to be used for
		 *                          communication. Possible values
		 * @n                       uC_BASE_I2C_CTRL_1 - for channel 0
		 * @n                       uC_BASE_I2C_CTRL_2 - for channel 1
		 * @n                       uC_BASE_I2C_CTRL_3 - for channel 2
		 * @param[in] device_address The device address of chip either 7 bit of 10 bit
		 * @param[in] speed_mode    Speed of I2C bus for this channel. Possible vaqlues are
		 * @n                       I2C_STANDARD_MODE - standard mode 100KHz
		 * @n                       I2C_FAST_MODE - fast mode 400KHz
		 * @parm[in] addr_mode      Addressing mode either 10 bit addressing or 7 bit addressing.
		 *                          Possible values are
		 * @n                       ADDRESS_7_BIT - 7 bit addressing
		 * @n                       ADDRESS_10_BIT - 10 bit addressing
		 * @param[in] fastmode_duty_cycle Duty cycle selection for fast mode. Possible values are
		 * @n                       FASTMODE_DUTYCYCLE_2 - Tlow/Thigh = 2
		 * @n                       FASTMODE_DUTYCYCLE_16_9 - Tlow/Thigh = 16/9
		 * @note                    Default parameters are passed to constructors. If you want to
		 *                          initialize I2C in standard/100k mode pass only first two
		 * parameters.
		 */
		I2C_Manager( uint8_t channel, uint16_t device_address,
					 uC_I2C::i2c_mode_t speed_mode = uC_I2C::I2C_STANDARD_MODE,
					 uC_I2C::address_type_t addr_mode = uC_I2C::ADDRESS_7_BIT,
					 uC_I2C::duty_type_t fastmode_duty_cycle =
					 uC_I2C::FASTMODE_DUTYCYCLE_16_9,
					 uC_BASE_I2C_IO_PORT_STRUCT const* io_port =
					 reinterpret_cast<uC_BASE_I2C_IO_PORT_STRUCT const*>( nullptr ) );

		/**
		 * @brief                   Establishes communication with addresses slave and reads data
		 * over
		 *                          an I2C bus from the slave.
		 * @param[in] address       Address of slave from which data to be read
		 * @param[in] length        Amount of data to be read(number of bytes) from slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @param[in] read_status   t returns the status of transfer if the last transaction was
		 *                          succesful or not.
		 */
		void Read_Data( uint16_t address, uint16_t length, uint8_t* datasource,
						uC_I2C::i2c_status_t* read_status );

		/**
		 * @brief                   Establishes communication with addresses slave and Write data
		 * over
		 *                          an I2C bus to the slave.
		 * @param[in] address       Address of slave to which data to be transmitted
		 * @param[in] length        Amount of data to be transmitted(number of bytes) to slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @note                    The user shall use this API to read the data from I2C slave.
		 * @param[in] write_status  It returns the status of transfer if the last transaction was
		 *                          succesful or not.
		 */
		void Write_Data( uint16_t address, uint16_t length, uint8_t* datasource,
						 uC_I2C::i2c_status_t* read_status );

	// static OS_Binary_Semaphore* I2CBinSemaphore;

	private:

		uC_I2C* m_i2c_control;

		static volatile bool_t m_transfer_finish;

		void Transfer_Finish( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Transfer_Finish_Static( uC_I2C::cback_param_t param )
		{
			( ( I2C_Manager* )param )->Transfer_Finish();
		}

};

#endif
