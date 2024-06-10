/**
 *****************************************************************************************
 *	@file STM32F4xx_USART_SPI.h
 *
 *	@brief Specialized Queued SPI class for the STM32F4xx architecture
 *
 *	@details
 *
 *	@version C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef STM32F4xx_USART_SPI_H
   #define STM32F4xx_USART_SPI_H

#include "uC_Queued_SPI.h"
#include "STM32F4xx.h"
#include "uC_Base.h"
#include "uC_DMA.h"

class Advanced_Vect;


/**
 ****************************************************************************************
 * @brief STM32F4xx_USART_SPI class
 *
 * @details Realizes Queued SPI interface for the STM32F4xx architecture
 *
 ****************************************************************************************
 */
class STM32F4xx_USART_SPI : public uC_Queued_SPI
{
	public:
		/**
		 * @brief                  Constructs a STM32F4xx_SPI object.
		 * @param[in] channel      spi channel of microcontroller
		 * @param[in] select_count    Total number of slaves interfaced to particular SPI channel
		 * @param[in] pio          Pointer to an array which holds the informationabout which
		 *                         GPIO pins used for chip slect purpose for each slave.
		 *                         e.g. If "no_slaves" parameter is 5 then the input array
		 *                         should contain 5 chip select GPIO pins.
		 * @param[in] usart_io     Pointer to the USART alternate peripheral io pins.  If not populated it will
		 * 							use a default set of pins defined in the HAL space.  Consider defining
		 * 							clearly to cover any potential future changes.
		 * @param[in] usart_clock_io    Pointer to the clock peripheral pin assigned for the clock.
		 * @param[in] master       SPI mode master or slave. Two possible values
		 * @n                      true - SPI channel configutred in mater mode
		 * @n                      false -  SPI channel configured in slave mode
		 * @param[in] enable_dma   Will choose whether to use the DMA assigned to the USART.
		 * @n                      true - use the dma assigned to this peripheral.
		 * @n                      false -  use an interrupt based handler for the characters.
		 */
		STM32F4xx_USART_SPI( uint8_t hw_usart_num, uint8_t total_chip_sel,
							 uC_USER_IO_STRUCT const* const* chip_sel_ctrls,
							 const uC_BASE_USART_IO_PORT_STRUCT* usart_io,
							 const uC_PERIPH_IO_STRUCT* usart_clock_io,
							 bool master = true, bool enable_dma = true );

		/**
		 * @brief Constructs a STM32F4xx_USART_SPI object.
		 * @param hw_usart_num: The UART index number selected.
		 * @param usart_io: Peripheral IO to be used with the above UART.
		 * @param usart_clock_io: Additional peripheral IO output for the SPI clock.
		 * @param enable_dma: Turns on the DMA instead of the UART interrupt for handling bytes.
		 * @param select_config: A pointer to a select_config_t structure array used to assign select
		 * resources and configuration information to the object.
		 * @param select_count - The number of structures in the select_config array.
		 * @param master - A flag indicating the object should act as an SPI master.
		 */
		~STM32F4xx_USART_SPI( void );

		/**
		 * @brief                 SPI Communication Configuration. The SPI chip
		 *                        configuration which variesfrom SPI based chip
		 *                        to chip.e.g it could be different for SPI based
		 *                        EEPROM and lets say SPI based RTC.
		 * @param[in] mode        Mode of the SPI slave chip. Possible values are
		 * @n                     MODE_0 - CPOL  = 0 & CPHASE = 0
		 * @n                     MODE_1 - CPOL  = 0 & CPHASE = 1
		 * @n                     MODE_2 - CPOL  = 1 & CPHASE = 0
		 * @n                     MODE_3 - CPOL  = 1 & CPHASE = 1
		 * @param[in] chip_clock_freq SPI chip clock frequency in Hz
		 * @param[in] assign_chip_id Assign the user defined chip id to each slave
		 *                        The chip id should start from 0 and maximum 255
		 *                        The chip should be in continuos succession
		 *                        e.g. 0,1,2,3....and so on
		 */
		void SPI_Chip_Configuration( spi_mode mode, uint32_t chip_clock_freq,
									 uint8_t assign_chip_id );

		/**
		 * @brief Handler for DMA interrupts.
		 */
		void DMA_ISR( void );

		/**
		 * @brief Handler for SPI interrupts.
		 */
		void ISR( void );

	protected:
		/**
		 * @brief A static class call back redirector to pass control to the appropriate
		 * object-specific DMA interrupt handler.
		 * @param object - pointer to STM32F4xx object call back is itended for.
		 */
		static void DMA_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_USART_SPI*>( object )->DMA_ISR();
		}

		/**
		 * @brief A static class call back redirector to pass control to the appropriate
		 * object-specific SPI interrupt handler.
		 * @param object - pointer to STM32F4xx object call back is itended for.
		 */
		static void ISR_Static( void* object )
		{
			static_cast<STM32F4xx_USART_SPI*>( object )->ISR();
		}

		/**
		 * @brief Initiates the exchange of an SPI message segment.
		 * @param new_exchange - Indicates exchange being initiated is the first segment of
		 * message being exchanged.
		 */
		virtual void Start_Exchange( bool new_exchange );

	protected:
		/**
		 * @brief Assigns SPI peripheral I/O pins to object/SPI peripheral.
		 * @param spi_io - Pointer to structure identifying I/O pins assigned to be used by object.
		 * @param master - A flag indicating the object should act as an SPI master. It determines
		 * if the slave select pin is assigned to the SPI peripheral and the default settings of
		 * the other SPI related pins.
		 */
		void Assume_IO_Control( uC_BASE_USART_IO_PORT_STRUCT const* usart_pio,
								uC_PERIPH_IO_STRUCT const* usart_clock_pio ) const;

		/**
		 * @brief Enables peripheral clocks for resources assigned to object.
		 * @param periph_def - Pointer to structure identifying peripheral clock to be initialized.
		 */
		void Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const;

	private:
		/**
		 * @brief Structure used to control chip selects and SPI
		 * configuration for each SPI device on bus.
		 */
		struct select_config_t
		{
			uC_USER_IO_STRUCT const* select_pio;
			uint16_t usart_cr2;
			uint16_t usart_brr;
			uint16_t dma_byte_count_theshold;
		};

		USART_TypeDef* m_usart;
		Advanced_Vect* m_uart_vector_obj;
		const uC_BASE_USART_IO_STRUCT* m_usart_periph_ctrl;
		select_config_t* m_slave_config;
		uint8_t m_dummy_data;
		select_config_t const* m_active_config;
		uC_DMA* m_rx_dma_ctrl;
		uC_DMA* m_tx_dma_ctrl;
		IRQn_Type m_uart_vector_number;
		uint8_t* m_rx_data;
		const uint8_t* m_tx_data;
		uint16_t m_exchange_length;

		/**
		 * @brief A private default constructor to inhibit the instantiation of the class without
		 * the appropriate arguments.
		 */
		STM32F4xx_USART_SPI( void ) {}

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		STM32F4xx_USART_SPI( const STM32F4xx_USART_SPI& object );
		STM32F4xx_USART_SPI & operator =( const STM32F4xx_USART_SPI& object );

};

#endif



