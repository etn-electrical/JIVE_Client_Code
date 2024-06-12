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
 *                Considering the complexity of logic for the fourth(Synchronous data transfer
 *                using DAM) type. We have migrated this option to I2C_manager class.
 *                The I2C_manager is designed for OS based applications  and shoould not be
 *                configured for CR tasker based applications
 **********************************************************************************************
 */

#ifndef __uC_I2C_h__
#define __uC_I2C_h__

#include "Includes.h"
#include <intrinsics.h>
#include <Defines.h>
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_DMA.h"

/**
 **********************************************************************************************
 * @brief                   I2C Class Declaration. The class will handle all the functionalities
 *                          related to I2C communication.
 **********************************************************************************************
 */
class uC_I2C
{
	public:

		enum return_type
		{
			/** @brief NO Error in I2C communication */
			NO_ERROR = 0U,
			/** @brief Error in I2C communication */
			I2C_ERROR
		};

		enum channel_state_t
		{
			/** @brief I2C channel is free to initiate new transfer */
			CHANNEL_FREE = 0U,
			/** @brief I2C channel is busy */
			CHANNEL_BUSY
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
			/** @brief  No error initialization state */
			NO_ERROR_INIT,

		};

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

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
		 * @param[in] synchronus_trnasfer - The user can use this option to slect if the transfer
		 *                          will be synchronous or asynchronous. The more about synchronous
		 *                          and asynchronous transfer is explained in Remarks columns
		 * @param[in] enable_dma    If I2C will use DMA for data transfer.
		 * @n                       true -DMA enabled for I2C data transfer
		 * @n                       false -DMA not used for I2C data transfer
		 * @note                    Default parameters are passed to constructors. If you want to
		 *                          initialize I2C in standard/100k mode pass only first two parameters.
		 * @Remark                  The uC_I2C class is implemented to tansfer data using sysnchronous
		 *                          as well asasynchronous mode. Synchronous mode -> The call to functions
		 *                          Write() andRead() will exit only in case of successful data transfer
		 *                          or error. Asynchronus mode -> The user shall call Write() or Read()
		 *                          function to initiate data transfer and exit. Once complete data is
		 *                          transmitted callback will be generated to notify program that
		 *                          trasaction is finished.
		 *                          Depending on this there are four combinations
		 *                          1. Asynchronus data transfer using DMA
		 *                          2. Asynchronus data transfer using simple I2C interrupts
		 *                          3. Synchronous data transfer using simple I2C mechanism
		 *                          4. Synchronous data transfer using DMA.
		 *                          Considering the complexity of logic for the fourth(Synchronous
		 *                          data transfer using DAM) type. We have migrated this option to
		 *                          I2C_manager class. The I2C_manager is designed for OS based applications
		 *                          and shoould not be configured for CR tasker based applications
		 */
		uC_I2C( uint8_t channel, i2c_mode_t speed_mode = I2C_STANDARD_MODE,
				bool synchronus_trnasfer = false, bool enable_dma = false,
				uC_BASE_I2C_IO_PORT_STRUCT const* io_port = reinterpret_cast<uC_BASE_I2C_IO_PORT_STRUCT const*>( nullptr ) );

		/**
		 * @brief                   Destructor to delete an instance of uc_I2C class
		 * @return
		 */
		~uC_I2C( void )
		{}

		/**
		 * @brief                   Establishes communication with addresses slave and reads data over
		 *                          an I2C bus from the slave.
		 * @param[in] address       Address of slave from which data to be read
		 * @param[in] length        Amount of data to be read(number of bytes) from slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @param[in] read_status   This parameter is valid only in case of synchronous transfer.
		 *                          It returns the status of transfer if the last transaction was
		 *                          succesful or not. In case of asynchronous transfer this parameter
		 *                          is "Don't Care", as we have callback once transfer is finished.
		 *                          The possible values are -
		 * @n                       NO_ERROR_INIT(0) : No Error Initialization state
		 * @n                       START_GENERATION_ERROR(1) : Start bit generation error
		 * @n                       RSTART_GENERATION_ERROR(2) : ReStart condition generation error
		 * @n                       ADDRESS_ERROR(3) : Error during address transmission
		 * @n                       BYTE_TX_ERROR(4) : Data transmission error
		 * @n                       BYTE_RX_ERROR(5) : Data reception error
		 * @n                       ACK_ERROR(6) : Acknowledge error
		 * @n                       STOP_GENERATION_ERROR(7) : Stop generation error
		 * @n                       I2C_FINISH_TX(8) : I2C transmission finish
		 * @n                       I2C_DMA_TX_BUSY(10) : I2C transmission using DMA is not finished
		 * @n                       I2C_FINISH_RX(9) : I2C reception finish
		 * @n                       I2C_DMA_RX_BUSY(11) : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to read the data from I2C slave.
		 */
		void Read( uint16_t address, uint16_t length, uint8_t* datasource,
				   i2c_status_t* read_status = NULL );

		/**
		 * @brief                   Establishes communication with addresses slave and Write data over
		 *                          an I2C bus to the slave.
		 * @param[in] address       Address of slave to which data to be transmitted
		 * @param[in] length        Amount of data to be transmitted(number of bytes) to slave
		 * @param[in] datasource    Pointer/RAM location where the read data will be put.
		 * @note                    The user shall use this API to read the data from I2C slave.
		 * @param[in] write_status  This parameter is valid only in case of synchronous transfer.
		 *                          It returns the status of transfer if the last transaction was
		 *                          succesful or not. In case of asynchronous transfer this parameter
		 *                          is "Don't Care", as we have callback once transfer is finished.
		 *                          The possible values are -
		 * @n                       NO_ERROR_INIT(0) : No Error Initialization state
		 * @n                       START_GENERATION_ERROR(1) : Start bit generation error
		 * @n                       RSTART_GENERATION_ERROR(2) : ReStart condition generation error
		 * @n                       ADDRESS_ERROR(3) : Error during address transmission
		 * @n                       BYTE_TX_ERROR(4) : Data transmission error
		 * @n                       BYTE_RX_ERROR(5) : Data reception error
		 * @n                       ACK_ERROR(6) : Acknowledge error
		 * @n                       STOP_GENERATION_ERROR(7) : Stop generation error
		 * @n                       I2C_FINISH_TX(8) : I2C transmission finish
		 * @n                       I2C_DMA_TX_BUSY(10) : I2C transmission using DMA is not finished
		 * @n                       I2C_FINISH_RX(9) : I2C reception finish
		 * @n                       I2C_DMA_RX_BUSY(11) : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to write the data to I2C slave.
		 */
		void Write( uint16_t address, uint16_t length, uint8_t* datasource,
					i2c_status_t* write_status = NULL );

		/**
		 * @brief                   Application layer can enquire if the transfer of data transfer
		 *                          over I2C is finished or not
		 * @return i2c_status_t     It returns the status of I2C transfer
		 * @n                       NO_ERROR_INIT(0) : No Error Initialization state
		 * @n                       START_GENERATION_ERROR(1) : Start bit generation error
		 * @n                       RSTART_GENERATION_ERROR(2) : ReStart condition generation error
		 * @n                       ADDRESS_ERROR(3) : Error during address transmission
		 * @n                       BYTE_TX_ERROR(4) : Data transmission error
		 * @n                       BYTE_RX_ERROR(5) : Data reception error
		 * @n                       ACK_ERROR(6) : Acknowledge error
		 * @n                       STOP_GENERATION_ERROR(7) : Stop generation error
		 * @n                       I2C_FINISH_TX(8) : I2C transmission finish
		 * @n                       I2C_DMA_TX_BUSY(10) : I2C transmission using DMA is not finished
		 * @n                       I2C_FINISH_RX(9) : I2C reception finish
		 * @n                       I2C_DMA_RX_BUSY(11) : I2C transmission using DMA is not finished
		 * @note                    The user shall use this API to read the data from I2C slave.
		 */
		i2c_status_t I2C_Transfer_Status( void );

		/**
		 * @brief                   Attaching callback function for notifying that last transaction was
		 *                          successful.
		 * @param[in] cback_func    The callback function which will be called when data transfer will
		 *                          be completed.
		 * @note                    Thi function is valid only in case of asynchronous data transfer.
		 */
		void Attach_Callback( cback_func_t cback_func = NULL, cback_param_t param = NULL );

	private:

		/** @brief Initialization of an I2C channel*/
		void Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode );

		/** @brief Function which enables the I2C peripheral of microcontroller*/
		inline void Enable_I2C( void );

		/** @brief Function for generating start condition */
		return_type Generate_Start( void );

		/** @brief Function get I2C timing data */
		inline uint32_t Get_I2C_Timing( i2c_mode_t speed_mode );

		/** @brief Function for sending the data over an I2C bus */
		return_type Send_Data( uint8_t data );

		/** @brief Sets the stop condition */
		return_type Set_Stop( void );

		/** @brief Receives data from an I2C slave */
		return_type Receive_Data( uint8_t* data ) const;

		/** @brief Timeout function in case particular state of I2C protocol is not reached */
		return_type Wait_Flag_Timeout( uint32_t flag_mask, volatile uint32_t* reg, uint16_t val ) const;

		return_type Start_Frame_Timeout() const;

		/** @brief Receives data from an I2C slave */
		i2c_status_t Set_State( return_type error, i2c_states_t* current_state,
								i2c_states_t next_state, i2c_status_t state_specific_error );

		/**
		 * @brief                   Configure the DMA for I2C transmit pupose.
		 * @param[in] data          The address/pointer from which data will be picked for transmitting
		 *                          over an I2C
		 * @param[in] length        The Number of data items/bytes to be transmitted
		 */
		void Set_TX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                   Configure the DMA for I2C receive pupose.
		 * @param[in] data          The address/pointer at which data received over an is to be written.
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
		return_type Bus_Busy_TimeOut( void );

		/**
		 * @brief                    RX DMA ISR Callback
		 */
		void RX_DMA_Int( void );

		/**
		 * @brief                    RX DMA ISR
		 */
		static void RX_DMA_Int_Static( uC_DMA::cback_param_t handle )
		{
			( ( uC_I2C* )handle )->RX_DMA_Int();
		}

		/**
		 * @brief                    TX DMA ISR Callback
		 */
		void TX_DMA_Int( void );

		/**
		 * @brief                    TX DMA ISR
		 */
		static void TX_DMA_Int_Static( uC_DMA::cback_param_t handle )
		{
			( ( uC_I2C* )handle )->TX_DMA_Int();
		}

		/**
		 * @brief                   A static class call back redirector to pass control to the
		 *                          appropriate object-specific I2C interrupt handler.
		 * @param object[in]        pointer to STM32F2xx object call back is itended for.
		 */
		static void I2C_ISR_Static( void* object )
		{
			static_cast<uC_I2C*>( object )->I2C_ISR();
		}

		/**
		 * @brief                   Handler for I2C interrupts.
		 * @note                    An interrupt is generated when I2C event occurs.
		 *                          Event includes start generation, address recognition
		 *                          data transmit success, data receive success
		 */
		void I2C_ISR( void );

		/** @brief Device Address of an I2C slave */
		uint16_t m_chip_address;

		I2C_TypeDef* m_i2c;

		/** @brief if the data transfer between I2C devices is using DMA or not */
		bool m_dma_transfer;

		bool m_synchronous_type;

		cback_func_t m_call_back_ptr;

		cback_param_t m_cback_param;

		uC_BASE_I2C_IO_STRUCT const* m_config;

		/** @brief I2C interrupt event vector number */
		IRQn_Type m_i2c_vector_number;

		Advanced_Vect* m_i2c_vector_obj;

		volatile uint16_t m_data_sink;

		uC_BASE_I2C_IO_PORT_STRUCT const* m_io_ctrl;

		/** @brief I2C speed type standard mode/fast mode */
		i2c_mode_t m_i2c_speed_type;

		i2c_states_t m_state_machine;

		uint8_t* m_tx_data;

		uint8_t* m_rx_data;

		bool m_write_exchange;

		uint16_t m_transfer_length;

		uint8_t m_reload_length;

		bool m_reload_mode;

		/** @brief Holds the status of I2C data transmisson/reception */
		volatile i2c_status_t m_transfer_status;

		/** @brief instance of DMA for reception purpose */
		uC_DMA* m_rx_dma;

		/** @brief instance of DMA for transmission purpose */
		uC_DMA* m_tx_dma;
		/** @brief DMA interrupt vectors */
		Advanced_Vect* m_dma_vector_obj_tx;

		Advanced_Vect* m_dma_vector_obj_rx;

		/** @brief channel number associated with an I2C object */
		uint8_t m_channel_number;

		/**@brief I2C Read bit */
		static const uint8_t I2C_READ = 0x01U;

		/**@brief I2C Write bit */
		static const uint8_t I2C_WRITE = 0x00U;

		static const uint8_t MAX_RELOAD_LENGTH = 255;
};

#endif
