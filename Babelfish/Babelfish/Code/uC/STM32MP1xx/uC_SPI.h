/**
 **********************************************************************************************
 *  @file           uC_SPI.h Header File for SPI protocol implementation.
 *
 *  @brief          The file shall wrap all the functions which are required for communication
 *                  between STMmicrocontroller and SPI slave.
 *  @details
 *  @copyright      2020 Eaton Corporation. All Rights Reserved.
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef uC_SPI_H
#define uC_SPI_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_OS_Interrupt.h"
#include "OS_Semaphore.h"
#include "uC_DMA.h"

/**
 * @brief Various SPI modes depending on clock polarity(CPOL) and clock phase(CPHA)
 */
enum
{
	/** @brief SPI mode 0 (CPOL =0, CPHA =0) */
	uC_SPI_MODE_0,
	/** @brief SPI mode 1 (CPOL =0, CPHA =1) */
	uC_SPI_MODE_1,
	/** @brief SPI mode 2 (CPOL =1, CPHA =0) */
	uC_SPI_MODE_2,
	/** @brief SPI mode 3 (CPOL =1, CPHA =1) */
	uC_SPI_MODE_3,
	uC_SPI_MODE_MAX
};

/**
 * @brief Number of Bits to be transfered in one packet.
 */
enum
{
	/** @brief 8 bit transfer */
	uC_SPI_BIT_COUNT_8 = 8,
	/** @brief 16 bit transfer */
	uC_SPI_BIT_COUNT_16 = 16
};

/**
 * @brief                   SPI Class Declaration. The class will handle all the
 *                          functionalities related to SPI communication.
 */
class uC_SPI
{
	public:
		/**
		 * @brief                   Max Retry Count for uC_SPI Rx communication
		 */
		static const uint32_t SPI_RETRY_COUNT = 1000U;

		/**
		 * @brief                     Constructor to create instance of uc_SPI class.
		 * @param[in] spi_select      This parameter specifies which SPI channel of controller we want to use for
		 * communication.
		 *                            Possible values are mentioned below :
		 * @n                         0 : Selects SPI channel 1
		 * @n                         1 : Selects SPI channel 2
		 * @n                         2 : Selects SPI channel 3
		 * @param[in] master          Specifies if SPI channel is to be used in master mode or slave mode
		 * @n                         TRUE : Master mode
		 * @n                         FALSE : Slave mode
		 * @param[in] chip_sel_ctrls  Chip Select pin specification. This structure variable shall specify
		 *                            the GPIO port and pin number which is used for selecting SPI slave
		 *                            device
		 * @param[in] total_chip_sel  The total number of SPI devices interfaced to the microcontroller.
		 * @param[in] io_ctrl         Structure pointer which contains information about the pins which are
		 *                            used for SPI communication. If NULL - it initializes the SPI pins for
		 *                            the channel "spi_select". (See the parameter spi_select)
		 * @param[in] enable_dma      Enable/Disable DMA of the SPI peripheral.
		 * @n                         TRUE  : Enables DMA
		 * @n                         FALSE : Disables DMA
		 * @return
		 */
		uC_SPI( uint8_t spi_select, bool master,
				uC_USER_IO_STRUCT const* const* chip_sel_ctrls, uint8_t total_chip_sel,
				uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl = NULL, bool enable_dma = true );

		/**
		 * @brief                   Destructor to delete the uc_SPI instance when it goes out of scope
		 *                          scope.
		 */
		~uC_SPI( void );

		/**
		 * @brief                               Configures the settings for communication with
		 *                                      particular SPI device. Sets communications mode,
		 *                                      clock frequency for SPI communication.
		 * @param[in] spi_mode                  SPI communication mode. Possible values are
		 * @n                                   0 : SPI mode 0 (CPOL =0, CPHA =0)
		 * @n                                   1 : SPI mode 1 (CPOL =0, CPHA =1)
		 * @n                                   2 : SPI mode 2 (CPOL =1, CPHA =0)
		 * @n                                   3 : SPI mode 3 (CPOL =1, CPHA =1)
		 * @param[in] chip_select               The SPI device number with which your controller
		 *                                      should establish communication
		 * @param[in] desired_spi_clock_freq    SPI communication clock frequency
		 * @param[in] bits_to_transfer          Number of bits to be transfered in one frame.
		 * @n                                   uC_SPI_BIT_COUNT_8(8) : 8 bits in one frame. 8 bit transfer
		 * @n                                   uC_SPI_BIT_COUNT_16(16) : 16 bits in one frame.
		 * @param[in] manual_chip_select        manual chip select signal
		 * @n                                   TRUE : manual chip select signalling enabled
		 * @n                                   FALSE : manual chip select signalling disabled
		 * @note
		 * @n
		 * @n
		 */
		void Configure_Chip_Select( uint8_t spi_mode, uint8_t chip_select = 0,
									uint32_t desired_spi_clock_freq = 1,
									uint8_t bits_to_transfer = uC_SPI_BIT_COUNT_8,
									bool manual_chip_select = false );

		/**
		 * @brief                   Fills in the SPI peripheral data register for transmission purpose.
		 *                          For 8 bit SPI transfer
		 *
		 * @param[in] data          The 1 byte(8bit) data to be transmitted over an SPI
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline void TX_Byte( uint8_t data )
		{
			while ( ( m_spi->SR & SPI_SR_TXP ) == 0U ){}

			*( ( __IO uint8_t* )&m_spi->TXDR ) = data;
		}

		/**
		 * @brief                   Fills in the SPI peripheral data register for transmission purpose.
		 *                          For 16 bit SPI transfer
		 * @param[in] data          The word(16 bit data) to be transmitted over an SPI
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline void TX_Word( uint16_t data )
		{
			while ( ( m_spi->SR & SPI_SR_TXP ) == 0U ){}

			*( ( __IO uint16_t* )&m_spi->TXDR ) = data;
		}

		/**
		 * @brief                   Return the byte data recived over from SPI slave device
		 *                          The byte is received in SPI data register and this function return it.
		 *                          8 bit data reception in one frame.
		 * @return uint8_t          The byte data received from SPI device
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline uint8_t RX_Byte( void )
		{
			volatile uint8_t dump;
			uint32_t retry_count = 0U;

			while ( m_spi->SR & ( SPI_SR_RXWNE | SPI_SR_RXPLVL ) != 0U )
			{
				dump = *( ( __IO uint8_t* )( &m_spi->RXDR ) );
			}

			*( ( __IO uint8_t* )&m_spi->TXDR ) = ( uint8_t )0;

			while ( !RX_Data_Reg_Full() && retry_count++ < SPI_RETRY_COUNT )
			{}

			dump = *( ( __IO uint8_t* )( &m_spi->RXDR ) );
			return ( dump );
		}

		/**
		 * @brief                   Return the word data recived over from SPI slave device
		 *                          The word data is recived in SPI data register and this function
		 *                          return it. 16 bit data reception in one frame.
		 * @return uint16_t         The word data received from SPI device
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline uint16_t RX_UINT16( void )
		{
			volatile uint16_t dump;
			uint32_t retry_count = 0U;

			dump = *( ( __IO uint16_t* )( &m_spi->TXDR ) );
			m_spi->TXDR = 0;
			while ( !RX_Data_Reg_Full() && retry_count++ < SPI_RETRY_COUNT )
			{}
			dump = *( ( __IO uint16_t* )( &m_spi->RXDR ) );
			return ( dump );
		}

		/**
		 * @brief                    Read the data from SPI data register and dump it. Make the SPI.
		 *                           transmission buffer empty.
		 * @note                     Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline void Dump( void )
		{
			uint32_t retry_count = 0U;
			volatile uint16_t dump;

			dump = m_spi->RXDR;
			while ( RX_Data_Reg_Full() && retry_count++ < SPI_RETRY_COUNT )
			{}
		}

		/**
		 * @brief                   Check if SPI reception buffer is empty or full.
		 * @return bool             Two possible return values
		 * @n                       0 : Receive buffer empty
		 * @n                       1 : Receive buffer full
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline bool RX_Data_Reg_Full()
		{
			Nop();
			Nop();
			Nop();
			return ( ( m_spi->SR & ( SPI_SR_RXWNE | SPI_SR_RXPLVL ) ) != 0 );
		}

		/**
		 * @brief                   Check if SPI transmission buffer is empty or full.
		 * @return bool             Two possible return values
		 * @n                       0 : Transmit buffer not empty
		 * @n                       1 : Transmit buffer empty
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline bool TX_Data_Reg_Empty( void )
		{
			Nop();
			Nop();
			Nop();
			return ( ( m_spi->SR & SPI_SR_TXP ) != 0 );
		}

		/**
		 * @brief                   Check if SPI is not busy in communication and data can be shifted
		 *                          or transfered.
		 * @return bool             Two possible return values
		 * @n                       0 : SPI communication busy.
		 * @n                       1 : Transmit buffer empty. Data can be transfered.
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline bool TX_Shift_Empty( void )
		{
			Nop();
			Nop();
			Nop();
			return ( ( m_spi->SR & SPI_SR_TXC ) != 0 );
		}

		/**
		 * @brief                    Selects the chip for SPI communication. Selecting the chip means
		 *                           pulling low(logic 0) the slave select signal(SS) of SPI device.
		 *                           When SS pin is pulled low means controller wants to transfer data
		 *                           to SPI device.
		 *
		 * @param[in] chip_select   The SPI device number which need to be selected for communication
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Select_Chip( uint8_t chip_select );

		/**
		 * @brief                    Deselect the chip to stop the SPI communication.Deselecting the chip
		 *                           pulling high(logic 1) the slave select signal(SS) of SPI device.
		 *
		 * @param[in] chip_select   The SPI device number whose communication need to be disconnected
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void De_Select_Chip( uint8_t chip_select ) const;

		/**
		 * @brief                   Configure the DMA for SPI receive pupose.
		 *
		 * @param[in] data          The address/pointer at which data received over an is to be written.
		 * @param[in] length        The Number of data items/bytes to be received
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Set_RX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                    Enables SPI Receive DMA.
		 * @note                     Implemented as an inline function.
		 */
		void Enable_RX_DMA( void )
		{
			Push_TGINT();
			m_rx_dma->Enable();
			m_spi->CFG1 |= SPI_CFG1_RXDMAEN;
			Pop_TGINT();
		}

		/**
		 * @brief                    Disables SPI Receive DMA.
		 * @note                     Implemented as an inline function.
		 */
		void Disable_RX_DMA( void )
		{
			Push_TGINT();
			m_spi->CFG1 &= ~SPI_CFG1_RXDMAEN;
			m_rx_dma->Disable();
			Pop_TGINT();
		}

		/**
		 * @brief                    Check if SPI Receive DMA is busy.
		 * @n                        Notifies DMA transfer related to SPI receive is finished or not.
		 * @param[in] void           none
		 * @return bool              Possible values
		 * @n                        1 - SPI Receive DMA transfer is finished
		 * @n                        0 - SPI Receive DMA transfer is not finished.
		 * @note                     Implemented as an inline function.
		 */
		inline bool RX_DMA_Busy( void )
		{
			return ( m_rx_dma->Busy() );
		}

		/**
		 * @brief                   Configure the DMA for SPI transmit pupose.
		 * @param[in] data          The address/pointer from which data will be picked for transmitting
		 *                          over an SPI
		 * @param[in] length        The Number of data items/bytes to be transmitted
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Set_TX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                   Enables SPI Transmit DMA.
		 * @note                    Implemented as an inline function.
		 */
		void Enable_TX_DMA( void )
		{
			Push_TGINT();
			m_tx_dma->Enable();
			m_spi->CFG1 |= SPI_CFG1_TXDMAEN;
			Pop_TGINT();
		}

		/**
		 * @brief                   Disables SPI Transmit DMA.
		 * @note                    Implemented as an inline function.
		 */
		void Disable_TX_DMA( void )
		{
			Push_TGINT();
			m_spi->CFG1 &= ~SPI_CFG1_TXDMAEN;
			m_tx_dma->Disable();
			Pop_TGINT();
		}

		/**
		 * @brief                   Check if SPI Transmit DMA is busy.
		 * @n                       Notifies DMA transfer related to SPI transmit is finished or not.
		 * @return bool             Possible values
		 * @n                       1 - SPI transmit DMA transfer is finished
		 * @n                       0 - SPI transmit DMA transfer is not finished.
		 * @note                    Implemented as an inline function.
		 */
		inline bool TX_DMA_Busy( void )
		{
			return ( m_tx_dma->Busy() );
		}

		/**
		 * @brief                   Configure the DMA for SPI exchange( transmit and receive) purpose.
		 *
		 * @param[in] tx_data       The address/pointer from which data will be picked for transmitting
		 *                          over an SPI
		 * @param[in] rx_data       The address/pointer at which data received over an is to be written.
		 * @param[in] length        The Number of data items/bytes to be transmitted/received over an SPI
		 * @return
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Set_XChange_DMA( uint8_t* tx_data, uint8_t* rx_data, uint16_t length );

		/**
		 * @brief                   Enables SPI exchange(transmit and receive) DMA.
		 *
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Enable_XChange_DMA( void )
		{
			Push_TGINT();
			Dump();
			m_spi->CFG1 |= ( SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN );
			m_rx_dma->Enable();
			m_tx_dma->Enable();
			Pop_TGINT();
		}

		/**
		 * @brief                   Disables SPI exchange(transmit and receive) DMA.
		 *
		 * @note                    Implemented as an inline function.**
		 * @n
		 * @n
		 */
		void Disable_XChange_DMA( void )
		{
			Push_TGINT();
			m_rx_dma->Disable();
			m_tx_dma->Disable();
			m_spi->CFG1 &= ~( SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN );
			Pop_TGINT();
		}

		/**
		 * @brief                    Checks if the SPI receive or SPI transmit DMA is busy.
		 *
		 * @return bool              Two possible values
		 * @n                        TRUE : SPI TX or SPI RX DMA busy
		 * @n                        FALSE : SPI TX/ SPI RX DMA free
		 * @note                     Implemented as an inline function.
		 * @n
		 */
		inline bool XChange_DMA_Busy( void )
		{
			return ( m_rx_dma->Busy() );
		}

	private:
		/**
		 * @brief
		 */
		typedef struct CHIP_SELECT_CFG_STTD
		{
			uint32_t cr1;
			uint32_t cr2;
			uint32_t cfg1;
			uint32_t cfg2;
			bool manual_pin_ctrl;
		} CHIP_SELECT_CFG_STTD;

		/**
		 * @brief SPI pin configuration holding structure
		 */
		uC_BASE_SPI_IO_STRUCT const* m_config;

		/**
		 * @brief ***
		 */
		SPI_TypeDef* m_spi;

		/**
		 * @brief ***
		 */
		uC_USER_IO_STRUCT const* const* m_chip_sel_ctrls;

		/**
		 * @brief Hold Number of SPI chips connected to microcontroller
		 */
		uint8_t m_total_chip_sel;

		/**
		 * @brief ***
		 */
		CHIP_SELECT_CFG_STTD* m_chip_sel_cfg;

		/**
		 * @brief Pointer to SPI receive DMA
		 */
		uC_DMA* m_rx_dma;

		/**
		 * @brief Pointer to SPI transmit DMA
		 */
		uC_DMA* m_tx_dma;

#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
		OS_Semaphore* m_semaphore;
#else
		volatile bool m_spi_busy_status;
#endif
};

#endif
