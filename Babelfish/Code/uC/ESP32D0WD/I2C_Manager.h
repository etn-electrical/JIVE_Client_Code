/**
 *****************************************************************************************
 *	@file		I2C_Manager.h
 *
 *	@brief		The file shall wrap all the functions which are written on top of uC_I2C class
 *	@n			This need to be used only  in case of OS based systems.
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef __I2C_MANAGER_h__
	#define __I2C_MANAGER_h__

#include "uC_I2C.h"

/**
 **********************************************************************************************
 * @brief					I2C_Manager Class Declaration. The class will handle all the
 * @n						functionalities related to I2C in full RTOS based system
 * @n						This class can only be used only with OS_Takser based application
 **********************************************************************************************
 */
class I2C_Manager
{
	public:

		/**
		 * @brief						Constructor to create instance of uc_I2C class.
		 * @param[in] channel			The I2C channel of microcontroller which is to be used for communication
		 * @param[in] device_address	The device address of chip either 7 bit of 10 bit
		 * @param[in] speed_mode		Speed of I2C bus for this channel.
		 * @parm[in] addr_mode			Addressing mode either 10 bit addressing or 7 bit addressing.
		 * @n							Possible values are
		 * @n							ADDRESS_7_BIT - 7 bit addressing
		 * @n							ADDRESS_10_BIT - 10 bit addressing
		 * @param[in] fastmode_duty_cycle	Duty cycle selection for fast mode.
		 * @return void					None
		 */
		I2C_Manager( uint8_t channel, uint16_t device_address,
					 uC_I2C::i2c_mode_t speed_mode = uC_I2C::I2C_STANDARD_MODE,
					 uC_I2C::address_type_t addr_mode = uC_I2C::ADDRESS_7_BIT,
					 uC_I2C::duty_type_t fastmode_duty_cycle =
					 uC_I2C::FASTMODE_DUTYCYCLE_16_9,
					 uC_BASE_I2C_IO_PORT_STRUCT const* io_port =
					 static_cast<uC_BASE_I2C_IO_PORT_STRUCT const*>( nullptr ) );

		/**
		 * @brief				Destructor to delete an instance of uc_I2C class
		 * @return
		 */
		~I2C_Manager( void )
		{}

		/**
		 * @brief					Establishes communication with addresses slave and reads data over
		 * @n						an I2C bus from the slave.
		 * @param[in] address		Address of slave from which data to be read
		 * @param[in] length		Amount of data to be read(number of bytes) from slave
		 * @param[in] datasource	Pointer/RAM location where the read data will be put.
		 * @param[in] read_statust	returns the status of transfer if the last transaction was successful or not.
		 * @param[in] cmd			i2c_cmd_handle_t structure reference
		 * @return Void				None
		 */
		void Read_Data( uint16_t address, uint16_t length, uint8_t* datasource,
						uC_I2C::i2c_status_t* read_status, i2c_cmd_handle_t cmd );

		/**
		 * @brief					Establishes communication with addresses slave and Write data over
		 * @n						an I2C bus to the slave.
		 * @param[in] address		Address of slave to which data to be transmitted
		 * @param[in] length		Amount of data to be transmitted(number of bytes) to slave
		 * @param[in] datasource	Pointer/RAM location where the read data will be put.
		 * @note					The user shall use this API to read the data from I2C slave.
		 * @param[in] write_status	It returns the status of transfer if the last transaction was successful or not.
		 * @param[in] cmd			i2c_cmd_handle_t structure reference
		 * @return Void				None
		 */
		void Write_Data( uint16_t address, uint16_t length, uint8_t* datasource,
						 uC_I2C::i2c_status_t* read_status, i2c_cmd_handle_t cmd );

	private:

		uC_I2C* m_i2c_control;	///< uC_I2C class pointer
		static volatile bool_t m_transfer_finish;	///< Flag to indicate transfer is finished or not

		/**
		 * @brief					Function to set the transfer finish flag as true
		 * @return Void				None
		 *
		 */
		void Transfer_Finish( void );

};

#endif
