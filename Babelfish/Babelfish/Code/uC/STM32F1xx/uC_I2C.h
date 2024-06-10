/**
 **********************************************************************************************
 *@file           uC_I2C.h Header File for I2C implementation.
 *
 *@brief          The file shall wrap all the functions which are required for communication
 *                between STM microcontroller and I2C slave.
 *@details
 *@copyright      2014 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef __uC_I2C_h__
#define __uC_I2C_h__

#include <intrinsics.h>
#include <Defines.h>
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_DMA.h"
#include "OS_Tasker.h"

/**
 **********************************************************************************************
 * @brief                   I2C Class Declaration. The class will handle all the functionalities
 *                          related to I2C communication.
 **********************************************************************************************
 */
class uC_I2C
{
#if 1

	public:

		enum return_type
		{
			/** @brief NO Error in I2C communication */
			NO_ERROR = 0U,
			/** @brief Error in I2C communication */
			I2C_ERROR
		};

		enum address_type_t
		{
			/** @brief 7 bit addressing mode */
			ADDRESS_7_BIT,
			/** @brief 10 bit addressing mode */
			ADDRESS_10_BIT
		};

		enum channel_state_t
		{
			/** @brief I2C channel is free to initiate new transfer */
			CHANNEL_FREE = 0U,
			/** @brief I2C channel is busy */
			CHANNEL_BUSY
		};

		enum duty_type_t
		{
			/** @brief I2C fast mode Tlow/Thigh = 2 */
			FASTMODE_DUTYCYCLE_2,
			/** @brief I2C fast mode Tlow/Thigh = 16/9 */
			FASTMODE_DUTYCYCLE_16_9
		};

		enum i2c_mode_t
		{
			/** @brief Standard Mode communication @100KHz */
			I2C_STANDARD_MODE,
			/** @brief Standard Mode communication @400KHz */
			I2C_FAST_MODE
		};

		enum i2c_states_t
		{
			/** @brief  start Condition */
			START_STATE,
			/** @brief  Set device address */
			SET_DEVICE_ADDRESS,
			/** @brief  send command */
			SET_COMMAND,
			/** @brief  repeated start generation */
			REPEATED_START_STATE,
			/** @brief  Repeat device address */
			REPEAT_DEVICE_ADDRESS,
			/** @brief  Sending data */
			SEND_DATA,
			/** @brief  receive data state */
			RECEIVE_DATA,
			/** @brief  Generate stop condition state */
			STOP_CONDITION,
			/** @brief Max state reached */
			MAX_STATE,
			/* Do not add any enum after this */
		};

		enum i2c_status_t
		{
			/** @brief  No error initialization state */
			NO_ERROR_INIT,
			/** @brief  Error in start condition generation */
			START_GENERATION_ERROR,
			/** @brief  Error in repeated start condition generation */
			RSTART_GENERATION_ERROR,
			/** @brief  Error in Device addressing- address mismatch or address not sent */
			ADDRESS_ERROR,
			/** @brief  data transmission error */
			BYTE_TX_ERROR,
			/** @brief  data reception error */
			BYTE_RX_ERROR,
			/** @brief  Acknowledge error from slave */
			ACK_ERROR,
			/** @brief I2C bus Busy Error */
			BUS_BUSY_ERROR,
			/** @brief  Error in stop bit generation */
			STOP_GENERATION_ERROR,
			/** @brief  I2C transmission complete */
			I2C_FINISH_TX,
			/** @brief  I2C reception complete */
			I2C_FINISH_RX,
			/** @brief DMA is busy in performing I2C data transmission */
			I2C_DMA_TX_BUSY,
			/** @brief DMA is busy in performing I2C data reception */
			I2C_DMA_RX_BUSY,

		};

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
		 * @n                       0 - 7 bit addressing
		 * @n                       1 - 10 bit addressing
		 * @param[in] enable_dma    If I2C will use DMA for data transfer.
		 * @n                       true -DMA enabled for I2C data transfer
		 * @n                       true -DMA not used for I2C data transfer
		 * @param[in] fastmode_duty_cycle Duty cycle selection for fast mode. Possible values are
		 * @n                       FASTMODE_DUTYCYCLE_2 - Tlow/Thigh = 2
		 * @n                       FASTMODE_DUTYCYCLE_16_9 - Tlow/Thigh = 16/9
		 * @note                    Default parameters are passed to constructors. If you want to
		 *                          initialize I2C in standard/100k mode pass only first two
		 * parameters.
		 */
		uC_I2C( uint8_t channel, uint16_t device_address,
				i2c_mode_t speed_mode = I2C_STANDARD_MODE, address_type_t addr_mode = ADDRESS_7_BIT,
				bool_t enable_dma =
				false, duty_type_t fastmode_duty_cycle =
				FASTMODE_DUTYCYCLE_16_9,
				uC_BASE_I2C_IO_PORT_STRUCT const* io_port =
				reinterpret_cast<uC_BASE_I2C_IO_PORT_STRUCT const*>( nullptr ) );

		/**
		 * @brief                   Destructor to delete an instance of uc_I2C class
		 * @return
		 */
		~uC_I2C( void )
		{}

		/**
		 * @brief                   Establishes communication with addresses slave and reads data
		 * over
		 *                          an I2C bus from the slave.
		 * @param[in] address       Address of slave from which data to be read
		 * @param[in] length        Amount of data to be read(number of bytes) from slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @return i2c_status_t     It returns the status of I2C read and specific error if any
		 * @n                       NO_ERROR_INIT(0) : No Error Initialization state
		 * @n                       START_GENERATION_ERROR(1) : Start bit generation error
		 * @n                       RSTART_GENERATION_ERROR(2) : ReStart condition generation error
		 * @n                       ADDRESS_ERROR(3) : Error during address transmission
		 * @n                       BYTE_RX_ERROR(5) : Data reception error
		 * @n                       ACK_ERROR(6) : Acknowledge error
		 * @n                       STOP_GENERATION_ERROR(7) : Stop generation error
		 * @n                       I2C_FINISH_RX(9) : I2C reception finish
		 * @n                       I2C_DMA_RX_BUSY(11) : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to read the data from I2C slave.
		 */
		i2c_status_t Read( uint16_t address, uint16_t length, uint8_t* datasource );

		/**
		 * @brief                   Establishes communication with addresses slave and Write data
		 * over
		 *                          an I2C bus to the slave.
		 * @param[in] address       Address of slave to which data to be transmitted
		 * @param[in] length        Amount of data to be transmitted(number of bytes) to slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @return i2c_status_t     It returns the status of I2C read and specific error if any
		 * @n                       NO_ERROR_INIT(0) : No Error Initialization state
		 * @n                       START_GENERATION_ERROR(1) : Start bit generation error
		 * @n                       RSTART_GENERATION_ERROR(2) : ReStart condition generation error
		 * @n                       ADDRESS_ERROR(3) : Error during address transmission
		 * @n                       BYTE_TX_ERROR(4) : Data transmission error
		 * @n                       ACK_ERROR(6) : Acknowledge error
		 * @n                       STOP_GENERATION_ERROR(7) : Stop generation error
		 * @n                       I2C_FINISH_TX(8) : I2C transmission finish
		 * @n                       I2C_DMA_TX_BUSY(10) : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to read the data from I2C slave.
		 */
		i2c_status_t Write( uint16_t address, uint16_t length, uint8_t* datasource );

		/**
		 * @brief                   Function for knowing the status of channel. It coudl be possible
		 * that
		 *                          communication is going on channelx and you are initiating data
		 * transfer
		 *                          over the same channel. If channel is busy we should not use the
		 * channel
		 *                          until it is free.
		 * @param[in] channel_no    The channel number whose status is to be enquired
		 * @note                    This function is very importanat in case we have multiple slaves
		 *                          on single I2C channel
		 */
		static bool_t Is_Channel_Free( uint8_t channel_no );

		/**
		 * @brief                   Performs software reset of I2C and reinitializes the I2C
		 *                          channel with old settings
		 * @note                    The software reset can be used if appliaction layer is getting
		 * error
		 *                          during transmission and data reception over an I2C bus
		 */
		void Reset_I2C( void );

	private:

		/** @brief Initialization of an I2C channel*/
		void Init_I2C( uint8_t i2c_channel,
					   i2c_mode_t i2c_mode, duty_type_t fmode_duty_cycle );

		/** @brief Function which enables the I2C peripheral of microcontroller*/
		inline void Enable_I2C( void );

		/** @brief Function for generating start condition */
		return_type Generate_Start( void );

		/** @brief Function to read the status register1 of microcontroller */
		inline uint16_t Get_SR1( void ) const;

		/** @brief Function to read the status register2 of microcontroller */
		inline uint16_t Get_SR2( void ) const;

		/** @brief Function which clears the errors */
		void Clear_Error( void );

		/** @brief Function for sending the data over an I2C bus */
		return_type Send_Data( uint8_t data );

		/** @brief Sets the stop condition */
		return_type Set_Stop( void );

		/** @brief Receives data from an I2C slave */
		return_type Receive_Data( uint8_t* data ) const;

		/** @brief Timeout function in case particular state of I2C protocol is not reached */
		return_type Wait_Flag_Timeout( uint32_t flag_mask ) const;

		/** @brief Receives data from an I2C slave */
		i2c_status_t Set_State( return_type error, i2c_states_t* current_state,
								i2c_states_t next_state, i2c_status_t state_specific_error ) const;

		/** @brief Sends the device address of the slave */
		return_type Send_Device_Address( uint16_t adr, uint8_t dir );

		/**
		 * @brief                   Configure the DMA for I2C transmit pupose.
		 * @param[in] data          The address/pointer from which data will be picked for
		 * transmitting
		 *                          over an I2C
		 * @param[in] length        The Number of data items/bytes to be transmitted
		 */
		void Set_TX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                   Configure the DMA for I2C receive pupose.
		 * @param[in] data          The address/pointer at which data received over an is to be
		 * written.
		 * @param[in] length        The Number of data items/bytes to be received
		 */
		void Set_RX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                    Enables I2C Receive DMA.
		 */
		void Enable_RX_DMA( void );

		/**
		 * @brief                    Disabales I2C Receive DMA.
		 */
		void Disable_RX_DMA( void );

		/**
		 * @brief                    Enables I2C Transmit DMA.
		 */
		void Enable_TX_DMA( void );

		/**
		 * @brief                    Disables I2C Transmit DMA.
		 */
		void Disable_TX_DMA( void );

		/**
		 * @brief                    Wait while Bus is busy
		 */
		void Bus_Busy_TimeOut( void );

		/**
		 * @brief                   Application layer can enquire if the transfer of data using DMA
		 *                          is finished or not
		 * @return i2c_status_t     It returns the status of I2C transfer if DMA is used for purpose
		 * @n                       I2C_FINISH_TX : I2C transmission finish
		 * @n                       I2C_FINISH_RX : I2C reception finish
		 * @n                       I2C_DMA_TX_BUSY : I2C transmission using DMA is not finished
		 * @n                       I2C_DMA_RX_BUSY : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to read the data from I2C slave.
		 */
		i2c_status_t DMA_Transfer_Status( void );

		/** @brief Device Address of an I2C slave */
		uint16_t chip_address;

		I2C_TypeDef* m_i2c;

		/** @brief addressing mode of the device */
		address_type_t addressing_mode;

		/** @brief if the data transfer between I2C devices is using DMA or not */
		bool_t dma_transfer;

		uC_BASE_I2C_IO_STRUCT const* m_config;

		uC_BASE_I2C_IO_PORT_STRUCT const* m_io_ctrl;

		/** @brief signifies if I2C channel of micro is busy or not */
		static channel_state_t channel_busy[3];

		/** @brief fast mode duty cycle */
		duty_type_t f_duty_cycle;

		/** @brief I2C speed type standard mode/fast mode */
		i2c_mode_t i2c_speed_type;

		/** @brief Holds the status of I2C data transmisson */
		i2c_status_t write_status;

		/** @brief Holds the status of I2C data reception */
		i2c_status_t read_status;

		/** @ brief instance of DMA for reception purpose */
		uC_DMA* rx_dma;

		/** @ brief instance of DMA for transmission purpose */
		uC_DMA* tx_dma;

		/** @brief channel number associated with an I2C object */
		uint8_t channel_number;

		/**@brief I2C speed 100kbps */
		static const uint32_t I2C_ClockSpeed_100K = static_cast<uint32_t>( 100000U );

		/**@brief I2C speed 400kbps */
		static const uint32_t I2C_ClockSpeed_400K = static_cast<uint32_t>( 400000U );

		/**@brief I2C Read bit */
		static const uint8_t I2C_READ = 0x01U;

		/**@brief I2C Write bit */
		static const uint8_t I2C_WRITE = 0x00U;

		/**@brief I2C 10bit header upper byte */
		static const uint8_t ADDR_HEADER_10_BIT = ( 0xF0U );

		/**@brief I2C bus worst case Timeout in uSec considering ACK time and setup and hold time */
		static const uint8_t I2C_TIMEOUT = ( 16U );

		/** @brief Processor frequency in MHz used in timeout calculation */
		static const uint8_t PROCESSOR_FREQ_MHZ = ( MASTER_CLOCK_FREQUENCY / 1000000U );

		/** @brief I2C error mask byte */
		static const uint16_t CPAL_I2C_STATUS_ERR_MASK = static_cast<uint16_t>( 0x0F00U );
#endif
};

#endif
