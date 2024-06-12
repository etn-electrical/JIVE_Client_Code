/**
 *****************************************************************************************
 *	@file STM32F4xx_SPI.h
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
#ifndef STM32H7xx_SPI_H
#define STM32H7xx_SPI_H

#include "uC_Queued_SPI.h"
#include "stm32h7xx.h"
#include "uC_Base.h"
#include "uC_DMA.h"

class Advanced_Vect;

/**
 ****************************************************************************************
 * @brief STM32F4xx_SPI class
 *
 * @details Realizes Queued SPI interface for the STM32F4xx architecture
 *
 ****************************************************************************************
 */
class STM32F4xx_SPI : public uC_Queued_SPI
{
	public:

		/**
		 * @brief                  Constructs a STM32F4xx_SPI object.
		 * @param[in] channel      spi channel of microcontroller
		 * @param[in] no_slaves    total number of slaves interfaced to particular SPI channel
		 * @param[in] master       SPI mode master or slave. Two possible values
		 * @n                      true - SPI channel configutred in mater mode
		 * @n                      false -  SPI channel configured in slave mode
		 * @param[in] io_ctrl      Pointer to the structure which information about pins
		 *                         used for SPI purpose.
		 * @param[in] pio          Pointer to an array which holds the informationabout which
		 *                         GPIO pins used for chip slect purpose for each slave.
		 *                         e.g. If "no_slaves" parameter is 5 then the input array
		 *                         should contain 5 chip select GPIO pins.
		 */
		STM32F4xx_SPI( uint8_t channel, uint8_t no_slaves, bool_t master,
					   uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl, uC_USER_IO_STRUCT const* const pio[] );

		/** @brief Destructor to delete an instance of STM32F4xx_SPI class */
		~STM32F4xx_SPI( void );

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

	protected:

		/**
		 * @brief Handler for DMA interrupts.
		 */
		void DMA_ISR( void );

		/**
		 * @brief          A static class call back redirector to pass control to
		 *                 the appropriate object-specific DMA interrupt handler.
		 * @param[in] object - pointer to STM32F4xx object call back is itended for.
		 */
		static void DMA_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_SPI*>( object )->DMA_ISR();
		}

		/**
		 * @brief Handler for SPI interrupts.
		 */
		void SPI_ISR( void );

		/**
		 * @brief          A static class call back redirector to pass control
		 *                 to the appropriateobject-specific SPI interrupt handler.
		 * @param[in] object - pointer to STM32F4xx object call back is itended for.
		 */
		static void SPI_ISR_Static( void* object )
		{
			static_cast<STM32F4xx_SPI*>( object )->SPI_ISR();
		}

		/**
		 * @brief                    Initiates the exchange of an SPI message segment.
		 * @param[in] new_exchange - Indicates exchange being initiated is the first
		 *                           segment of message being exchanged.
		 */
		virtual void Start_Exchange( bool_t new_exchange );

	protected:
		/**
		 * @brief             Assigns SPI peripheral I/O pins to object/SPI peripheral.
		 * @param[in] spi_io  Pointer to structure identifying I/O pins assigned to be used by object.
		 * @param[in] master  A flag indicating the object should act as an SPI master. It determines
		 *                    if the slave select pin is assigned to the SPI peripheral and the
		 *                    default settings of the other SPI related pins.
		 */
		void Assume_IO_Control( const uC_BASE_SPI_IO_PORT_STRUCT* spi_io,
								bool_t master_operation ) const;

		/**
		 * @brief                 Enables peripheral clocks for resources assigned to object.
		 * @param[in] periph_def  Pointer to structure identifying peripheral clock to be initialized.
		 */
		void Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const;

		/**
		 * @brief                Clears interrupt flags in DMA peripheral assigned to object.
		 * @return               true- if interrupt clear was successful else false
		 */
		bool_t Clear_DMA_Interrupts( void );

	private:
		/**
		 * @brief A private default constructor to inhibit the instantiation of the class without
		 * the appropriate arguements.
		 */
		STM32F4xx_SPI( void ) : m_spi( nullptr )
		{}

		/**
		 * @brief Struture used to by constructor to initialize class.
		 */
		struct class_config_t
		{
			const uC_BASE_SPI_IO_STRUCT* spi;
			const uC_BASE_DMA_IO_STRUCT* rx_dma;
			const uC_BASE_DMA_IO_STRUCT* tx_dma;
			const uC_BASE_SPI_IO_PORT_STRUCT* spi_io;
			uint8_t int_priority;
		};

		/**
		 * @brief  Struture used to control chip selects and SPI
		 *         configuration for each SPI device on bus.
		 */
		struct select_config_t
		{
			uC_USER_IO_STRUCT* select_pio;
			uint16_t spi_cr1;
			uint16_t spi_cr2;
		};

		select_config_t const* m_active_config;
		SPI_TypeDef* m_spi;
		DMA_Stream_TypeDef* m_rx_stream;
		DMA_Stream_TypeDef* m_tx_stream;
		DMA_TypeDef* m_rx_dma_ctrl;
		DMA_TypeDef* m_tx_dma_ctrl;
		Advanced_Vect* m_spi_vector_obj;
		Advanced_Vect* m_dma_vector_obj;
		IRQn_Type m_spi_vector_number;
		IRQn_Type m_dma_vector_number;
		uint8_t* m_rx_data;
		const uint8_t* m_tx_data;
		uint16_t m_exchange_length;
		uint8_t m_rx_dma_id;
		uint8_t m_tx_dma_id;
		bool_t m_master;
		volatile uint16_t m_data_sink;

		uC_BASE_SPI_IO_STRUCT const* m_config;
		uC_BASE_DMA_IO_STRUCT const* m_rx_dma;
		uC_BASE_DMA_IO_STRUCT const* m_tx_dma;
		uC_BASE_SPI_IO_PORT_STRUCT const* m_spi_io;
		uint8_t m_total_devices;
		select_config_t* m_slave_config;
		volatile_item_t m_volatile_item_rx;
		volatile_item_t m_volatile_item_tx;
		uint8_t m_dummy_data;

		static const uint16_t SPI_CR1_MASK = SPI_CR1_DFF |
			SPI_CR1_SSM |
			SPI_CR1_SSI |
			SPI_CR1_BR |
			SPI_CR1_CPOL |
			SPI_CR1_CPHA;

		static const uint16_t SPI_CR2_MASK = SPI_CR2_SSOE;
};

#endif

