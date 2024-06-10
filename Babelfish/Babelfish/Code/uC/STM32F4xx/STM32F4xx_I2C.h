/**
 **************************************************************************************************
 *@file           STM32F4xx_I2C.h Header File for I2C implementation.
 *
 *@brief          Its a processor dependent file for I2C queued class implementation
 *@details
 *@copyright      2014 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 **************************************************************************************************
 */
#ifndef STM32F4xx_I2C_H
#define STM32F4xx_I2C_H

#include "uC_Queued_I2C.h"
#include "stm32f4xx.h"
#include "uC_Base.h"

class Advanced_Vect;

/**
 **************************************************************************************************
 * @brief STM32F4xx_I2C class
 *
 * @details Realizes Queued I2C interface for the STM32F2xx architecture
 *
 **************************************************************************************************
 */
class STM32F4xx_I2C : public uC_Queued_I2C
{
	public:

		/** @brief enumeration which denotes the speed of I2C bus */
		enum i2c_speed_mode_t
		{
			/** @brief Standard Mode communication @100KHz */
			I2C_STANDARD_SPEED,
			/** @brief Standard Mode communication @400KHz */
			I2C_FAST_SPEED
		};

		/** @brief enumeration which denotes the duty cycle in case of fast mode */
		enum duty_cycle_t
		{
			/** @brief I2C fast mode Tlow/Thigh = 2 */
			DUTYCYCLE_FASTMODE_2,
			/** @brief I2C fast mode Tlow/Thigh = 16/9 */
			DUTYCYCLE_FASTMODE_16_9
		};

		/** @brief denotes various states of I2C bus */
		enum i2c_state_t
		{
			/** @brief  start Condition */
			START_STATE_ACK,
			/** @brief  Set device address */
			SET_DEVICE_ADDRESS_ACK,
			/* @brief Set Address Upper Byte */
			SET_ADDRESS_BYTE_2,
			/** @brief  send command */
			SET_COMMAND_ACK,
			/** @brief  repeated start generation */
			REPEATED_START_STATE_ACK,
			/** @brief  Repeat device address */
			REPEAT_DEVICE_ADDRESS_ACK,
			/** @brief  Sending data */
			SEND_DATA_ACK,
			/** @brief  receive data state */
			RECEIVE_DATA_ACK,
			/** @brief  Generate stop condition state */
			STOP_CONDITION_ACK,
			/** @brief Max state reached */
			MAX_STATE_ACK
			/* Do not add any enum after this */
		};

		/**
		 * @brief Struture used to by constructor to initialize class.
		 * @description it will hold the information about the i2c channel used
		 */
		struct class_config_t
		{
			const uC_BASE_I2C_IO_STRUCT* i2c;
			const uC_BASE_DMA_IO_STRUCT* rx_dma;
			const uC_BASE_DMA_IO_STRUCT* tx_dma;
			const uC_BASE_I2C_IO_PORT_STRUCT* i2c_io;
			uint8_t int_priority;
		};

		/**
		 * @brief This structure shall hold the settings need to done for
		 *        transmit or receive exchange
		 */
		struct select_config_t
		{
			uC_USER_IO_STRUCT* select_pio;
			uint16_t i2c_cr1;
			uint16_t i2c_cr2;
			uint16_t i2c_ccr;
			uint16_t i2c_trise;
			uint16_t dma_byte_count_theshold;

			/**
			 * @brief                      Function for configuring I2C channel settings
			 *
			 * @param[in] i2c_mode         The mode of I2C in which bus will run
			 * @n                          Possible values
			 * @n                          I2C_STANDARD_SPEED - 100KHz
			 * @n                          I2C_FAST_SPEED - 400KHz
			 * @param[in] i2c_pclk1_freq   Peripheral clock frequency
			 * @param[in] fmode_duty_cycle fast mode duty cycle.
			 * @param[in] byte_count_theshold Byte count threshold for DMA transfer
			 * @note
			 */
			void Configure( i2c_speed_mode_t i2c_mode,
							uint32_t i2c_pclk1_freq,
							duty_cycle_t fmode_duty_cycle,
							uint16_t byte_count_theshold );

		};

		/**
		 * @brief                      Constructor to create an instance of
		 *                             STM32F4xx_I2C class.
		 *
		 * @param[in] i2c_assy         A pointer and class_config_t structure
		 *                             used to assign processor peripheral
		 *                             resources and attributes to the object.
		 * @param[in] select_config    A pointer to a select_config_t structure
		 *                             array used to assign select resources and
		 *                             configuration information to the object
		 * @note
		 */
		STM32F4xx_I2C( class_config_t const* i2c_assy,
					   select_config_t const* const select_config );

		/**
		 * @brief                   Destructor to delete an instance of STM32F4xx_I2C class
		 * @return
		 */
		~STM32F4xx_I2C( void );

		/**
		 * @brief                   Handler for DMA Tx interrupt.
		 * @ note                   An interrupt will be generated when DMA transfer
		 *                          is finished
		 */
		void DMA_TX_ISR( void );

		/**
		 * @brief                   Handler for DMA interrupts.
		 * @ note                   An interrupt will be generated when DMA receive
		 *                          is finished
		 */
		void DMA_RX_ISR( void );

		/**
		 * @brief                   Handler for I2C interrupts.
		 * @note                    An interrupt is generated when I2C event occurs.
		 *                          Event includes start generation, address recognition
		 *                          data transmit success, data receive success
		 */
		void I2C_ISR( void );

		uint8_t m_dummy_data;

		/**
		 * @brief                   Starts the I2C exchange with I2C slaves.
		 * @note                    Exchange involves I2C read, I2C write.
		 */
		void Start_Exchange( bool_t new_exchange );

	protected:
		/**
		 * @brief                   A static class call back redirector to pass control to the
		 *                          appropriate object-specific DMA TX interrupt handler.
		 * @param[in] object        An object pointer to STM32F2xx object call back is itended for.
		 */
		static void DMA_TX_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_I2C*>( object )->DMA_TX_ISR();
		}

		/**
		 * @brief                   A static class call back redirector to pass control to the
		 *                          appropriate object-specific DMA RX interrupt handler.
		 * @param[in]               An object  pointer to STM32F2xx object call back is itended for.
		 */
		static void DMA_RX_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_I2C*>( object )->DMA_RX_ISR();
		}

		/**
		 * @brief                   A static class call back redirector to pass control to the
		 *                          appropriate object-specific I2C interrupt handler.
		 * @param object[in]        pointer to STM32F2xx object call back is itended for.
		 */
		static void I2C_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_I2C*>( object )->I2C_ISR();
		}

		/**
		 * @brief                    Wait while Bus is busy
		 */
		void Bus_Busy_TimeOut( void );

		select_config_t const* const m_select;
		select_config_t const* m_active_config;
		I2C_TypeDef* const m_i2c;
		DMA_Stream_TypeDef* m_rx_stream;
		DMA_Stream_TypeDef* m_tx_stream;
		DMA_TypeDef* m_rx_dma_ctrl;
		DMA_TypeDef* m_tx_dma_ctrl;
		Advanced_Vect* m_i2c_vector_obj;
		Advanced_Vect* m_dma_vector_obj_tx;
		Advanced_Vect* m_dma_vector_obj_rx;
		/** @brief I2C interrupt event vector number */
		IRQn_Type m_i2c_vector_number;
		/** @brief DMA Tx interrupt event vector number */
		IRQn_Type m_dma_vector_number_tx;
		/** @brief DMA Rx interrupt event vector number */
		IRQn_Type m_dma_vector_number_rx;
		/** @brief received data pointer */
		uint8_t* m_rx_data;
		/** @brief transmit data pointer */
		const uint8_t* m_tx_data;
		/** @brief amount of data to be transmitted to the slave(in bytes) */
		uint16_t m_tx_length;
		/** @brief amount of data to be received from slave( in bytes) */
		uint16_t m_rx_length;
		/** @brief Receive DMA ID*/
		uint8_t m_rx_dma_id;
		/** @brief Transmit DMA ID */
		uint8_t m_tx_dma_id;
		/** @brief for flushing the old data from data register */
		volatile uint16_t m_data_sink;
		/** @brief signifies various states of I2C transfer */
		i2c_state_t state_machine;
		/** @brief the I2C transaction either read transaction or write transaction */
		i2c_exchange_type transaction;
		/** @brief DMA channel for transmit purpose */
		uint32_t tx_channel;
		/** @brief DMA channel for receive purpose */
		uint32_t rx_channel;
		/** @brief dyanamic detection if dma is used or not */
		bool_t dma_use;
		/** @brief chip address of slave */
		uint16_t device_address;

	protected:
		/**
		 * @brief                   Assigns I2C peripheral I/O pins to object/I2C peripheral.
		 * @param i2c_io[in]        Pointer to structure identifying I/O pins assigned to be
		 *                          used by object.
		 */
		void Assume_IO_Control( const uC_BASE_I2C_IO_PORT_STRUCT* i2c_io ) const;

		/**
		 * @brief                   Enables peripheral clocks for resources assigned to object.
		 * @param periph_def[in]    Pointer to structure identifying peripheral clock to be
		 *                          initialized.
		 */
		void Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const;

		/**
		 * @brief                   Clears interrupt flags in DMA peripheral assigned to object.
		 * @return bool_t type      TRUE - Interrupt cleared but Error in DMA transfer
		 * @n                       FALSE - Interrupt cleared and no error in DMA present
		 */
		bool_t Clear_DMA_Interrupts( void );

	private:
		/** @brief Function for generating start condition */
		void Generate_Start( void );

		/** @brief Function to read the status register1 of I2C peripheral */
		inline uint16_t Get_SR1( void ) const;

		/** @brief Function to read the status register2 of I2C peripheral */
		inline uint16_t Get_SR2( void ) const;

		/** @brief Sets the stop condition */
		void Set_Stop( void );

		/** @brief Timeout function in case particular state of I2C protocol is not reached */
		void Wait_Flag_Timeout( uint32_t flag_mask ) const;

		/**
		 * @brief A private default constructor to inhibit the instantiation of the class without
		 * the appropriate arguements.
		 */
		STM32F4xx_I2C( void ) :
			m_select( nullptr ),
			m_i2c( nullptr )
		{}

		/**@brief I2C speed 100kbps */
		static const uint32_t I2C_ClockSpeed_100KHz = ( 100000U );
		/**@brief I2C speed 400kbps */
		static const uint32_t I2C_ClockSpeed_400KHz = ( 400000U );

		static const uint8_t I2C_TIMEOUT = ( 16U );
		/** @brief Processor frequency in MHz used in timeout calculation */
		static const uint8_t PROCESSOR_FREQ_MHZ = ( MASTER_CLOCK_FREQUENCY / 1000000U );
		/** @brief Header in case of 10 bit addressing mode */
		static const uint8_t ADDR_HEADER_10_BIT = ( 0xF0U );
};

#endif

