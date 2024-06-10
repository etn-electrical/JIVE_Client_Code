/**
 **********************************************************************************************
 *  @file           uC_SPI.h Header File for SPI protocol implementation.
 *
 *  @brief          The file shall wrap all the functions which are required for communication
 *                  between STMmicrocontroller and SPI slave.
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
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
#define uC_SPI_OS_SEMAPHORE_SUPPORT

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
		 * @brief                     Constructor to create instance of uc_SPI class.
		 * @param[in] spi_select      The STM32F207 features three SPIs in slave and master modes in
		 *                            full-duplex and simplex communication modes. This parameter specifies
		 *                            which SPI channel of controller we want to use for communication.
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
									uint8_t bits_to_transfer = uC_SPI_BIT_COUNT_8, bool manual_chip_select = false );


		/**
		 * @brief                   Exchanges 8-bit data via SPI.
		 *
		 * @param[in] data          The 1 byte(8bit) data to be transmitted over an SPI
		 * @return uint8_t          The byte data received from SPI device
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline uint8_t Exchange_Byte( uint8_t tx_data )
		{
			m_spi->PUSHR = tx_data;
			while ( !RX_Data_Reg_Empty() ){}
			return ( m_spi->POPR );
		}

		/**
		 * @brief                   Exchanges 16-bit data via SPI.
		 *
		 * @param[in] data          The word (16-bit) data to be transmitted over an SPI
		 * @return uint8_t          The word data received from SPI device
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline uint16_t Exchange_Word( uint16_t tx_data )
		{
			m_spi->PUSHR = tx_data;
			while ( !RX_Data_Reg_Empty() ){}
			return ( m_spi->POPR );
		}

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
			m_spi->SR |= SPI_SR_TCF_MASK;
			m_spi->PUSHR = data;
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
			m_spi->SR |= SPI_SR_TCF_MASK;
			m_spi->PUSHR = data;
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
			volatile UINT8 data;

			// pop off any leftovers
			while ( !RX_Data_Reg_Empty() )
			{
				data = m_spi->POPR;
				m_spi->SR |= SPI_SR_RFDF_MASK;
			}
			m_spi->PUSHR = 0;
			while ( RX_Data_Reg_Empty() ){}
			data = m_spi->POPR;
			m_spi->SR |= SPI_SR_RFDF_MASK;
			return ( data );
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
		/* inline uint16_t RX_UINT16( void )
		   {
		    volatile uint16_t dump;

		    dump = m_spi->DR;
		    m_spi->DR = 0;
		    while ( !RX_Data_Reg_Full() )
		    {
		    };
		    return ( m_spi->DR );
		   }*/

		/**
		 * @brief                    Read the data from SPI data register and dump it. Make the SPI.
		 *                           transmission buffer empty.
		 * @note                     Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline void Dump( void )
		{
			volatile uint8_t dump;

			// pop off any leftovers
			while ( !RX_Data_Reg_Empty() )
			{
				dump = m_spi->POPR;
				m_spi->SR |= SPI_SR_RFDF_MASK;
			}
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
			// Nop();
			// Nop();
			// Nop();
			return ( /*( m_spi->SR & SPI_SR_RXNE ) != 0*/ 0 );
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

		inline bool RX_Data_Reg_Empty()
		{
			Nop();
			Nop();
			Nop();
			return ( ( m_spi->SR & SPI_SR_RFDF_MASK ) == 0 );
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
			// return ( ( m_spi->SR & SPI_SR_TFFF_MASK ) != 0 );
			return ( ( ( m_spi->SR & SPI_SR_TCF_MASK ) != 0 ) && ( ( m_spi->SR & SPI_SR_TFFF_MASK ) != 0 ) );
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
			return ( ( ( m_spi->SR & SPI_SR_TCF_MASK ) != 0 ) && ( ( m_spi->SR & SPI_SR_TFFF_MASK ) != 0 ) );
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
		 * @brief                   This Function acquires the semaphore.(Not used in Low toolkit)
		 *
		 * @param[in] time_to_wait  Wait this amount of time before acquiring semapore.
		 * @return bool             Two possible values
		 * @n                       TRUE - Semaphore acquired
		 * @n                       FALSE - Semaphore not acquired
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline bool Get_Semaphore( uint32_t time_to_wait )
		{
#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
			return ( m_semaphore->Pend( time_to_wait ) );

#else
			bool status = false;
			Push_TGINT();
			if ( m_spi_busy_status != true )
			{
				m_spi_busy_status = true;
				status = true;
			}
			else
			{
				status = false;
			}
			Pop_TGINT();
			return ( status );

#endif
		}

		/**
		 * @brief                   This Function releases the semapore.(Not used in Low toolkit)
		 *
		 * @return bool             Two possible values
		 * @n                       TRUE - Releasing the semaphore successfull
		 * @n                       FALSE - Semaphore not release
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		inline bool Release_Semaphore( void )
		{
#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
			return ( m_semaphore->Post() );

#else
			m_spi_busy_status = false;
			return ( true );

#endif
		}

		/**
		 * @brief                   Configure the DMA for SPI receive pupose.
		 *
		 * @param[in] data          The address/pointer at which data received over an is to be written.
		 * @param[in] length        The Number of data items/bytes to be received
		 * @return
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Set_RX_DMA( uint8_t* data, uint16_t length );

		/**
		 * @brief                    Enables SPI Receive DMA.
		 * @note                     Implemented as an inline function.
		 */
		void Enable_RX_DMA( void );

		/**
		 * @brief                    Disables SPI Receive DMA.
		 * @note                     Implemented as an inline function.
		 */
		void Disable_RX_DMA( void );

		/**
		 * @brief                    Check if SPI Receive DMA is busy.
		 * @n                        Notifies DMA transfer related to SPI receive is finished or not.
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
		void Enable_TX_DMA( void );

		/**
		 * @brief                   Disables SPI Transmit DMA.
		 * @note                    Implemented as an inline function.
		 */
		void Disable_TX_DMA( void );

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
		void Enable_XChange_DMA( void );

		/**
		 * @brief                   Disables SPI exchange(transmit and receive) DMA.
		 *
		 * @note                    Implemented as an inline function.**
		 * @n
		 * @n
		 */
		void Disable_XChange_DMA( void );

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

	// UINT32			Get_RX_DMA_Ptr( void )					{ return ( m_spi->SPI_RPR ); }
	// UINT32			Get_RX_DMA_Ctr( void )					{ return ( m_spi->SPI_RCR ); }
	////****************************
	//
	//
	// uint8_t			Exchange_Data( uint8_t data );
	////****************************
	//
	// void 			Set_RX_DMA( uint8_t* data, uint16_t length );
	// UINT32			Get_RX_DMA_Ptr( void )					{ return ( m_spi->SPI_RPR ); }
	// UINT32			Get_RX_DMA_Ctr( void )					{ return ( m_spi->SPI_RCR ); }
	// void			Enable_RX_DMA( void )					{ m_spi->SPI_PTCR = AT91C_PDC_RXTEN; }
	// void			Disable_RX_DMA( void )					{ m_spi->SPI_PTCR = AT91C_PDC_RXTDIS; }
	//
	// void			Set_TX_DMA( uint8_t* data, uint16_t length );
	// void			Enable_TX_DMA( void )					{ m_spi->SPI_PTCR = AT91C_PDC_TXTEN; }
	// void			Disable_TX_DMA( void )					{ m_spi->SPI_PTCR = AT91C_PDC_TXTDIS; }
	//
	// void			Enable_DMA( void )						{ m_spi->SPI_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN; }
	// void			Disable_DMA( void )						{ m_spi->SPI_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS; }
	//
	// void 			Exchange_Data( uint8_t* tx_data, uint8_t* rx_data, uint16_t length );
	//
	//// Interrupt behavior follows.
	// BOOL 			Set_Int_Vect( INT_CALLBACK_FUNC* int_handler, uint8_t priority );
	// BOOL 			Set_OS_Int_Vect( OS_INT_CALLBACK_FUNC* int_handler, uint8_t priority );
	//
	// void			Enable_Int( void ) 						{ uC_Interrupt::Enable_Int( m_config->pid ); }
	// void			Disable_Int( void ) 					{ uC_Interrupt::Disable_Int( m_config->pid ); }
	//
	// void			Enable_RX_Data_Reg_Full_Int( void ) 	{ m_spi->SPI_IER |= AT91C_SPI_RDRF; }
	// void			Disable_RX_Data_Reg_Full_Int( void ) 	{ m_spi->SPI_IDR |= AT91C_SPI_RDRF; }
	//
	// void			Enable_TX_Data_Reg_Empty_Int( void ) 	{ m_spi->SPI_IER |= AT91C_SPI_TDRE; }
	// void			Disable_TX_Data_Reg_Empty_Int( void ) 	{ m_spi->SPI_IDR |= AT91C_SPI_TDRE; }
	//
	// void			Enable_RX_Buff_End_Int( void ) 			{ m_spi->SPI_IER |= AT91C_SPI_ENDRX; }
	// void			Disable_RX_Buff_End_Int( void ) 		{ m_spi->SPI_IDR |= AT91C_SPI_ENDRX; }
	// inline BOOL		RX_Buff_End( void )						{ return ( ( m_spi->SPI_SR & AT91C_SPI_ENDRX ) != 0 ); }
	//
	// void			Enable_TX_Buff_End_Int( void ) 			{ m_spi->SPI_IER |= AT91C_SPI_ENDTX; }
	// void			Disable_TX_Buff_End_Int( void ) 		{ m_spi->SPI_IDR |= AT91C_SPI_ENDTX; }
	// inline BOOL		TX_Buff_End( void )						{ return ( ( m_spi->SPI_SR & AT91C_SPI_ENDTX ) != 0 ); }
	//
	// void			Enable_RX_Buff_Full_Int( void ) 		{ m_spi->SPI_IER |= AT91C_SPI_RXBUFF; }
	// void			Disable_RX_Buff_Full_Int( void ) 		{ m_spi->SPI_IDR |= AT91C_SPI_RXBUFF; }
	// inline BOOL		RX_Buff_Full( void )					{ return ( ( m_spi->SPI_SR & AT91C_SPI_RXBUFF ) != 0 );
	// }
	//
	// void			Enable_TX_Buff_Empty_Int( void ) 		{ m_spi->SPI_IER |= AT91C_SPI_TXBUFE; }
	// void			Disable_TX_Buff_Empty_Int( void ) 		{ m_spi->SPI_IDR |= AT91C_SPI_TXBUFE; }
	// inline BOOL		TX_Buff_Empty( void )					{ return ( ( m_spi->SPI_SR & AT91C_SPI_TXBUFE ) != 0 );
	// }
	//
	// void			Enable_TX_Shift_Empty_Int( void ) 		{ m_spi->SPI_IER |= AT91C_SPI_TXEMPTY; }
	// void			Disable_TX_Shift_Empty_Int( void ) 		{ m_spi->SPI_IDR |= AT91C_SPI_TXEMPTY; }

	private:
		/**
		 * @brief ***
		 */
		typedef struct CHIP_SELECT_CFG_STTD
		{
			uint16_t cr1;
			uint16_t cr2;
			bool manual_pin_ctrl;
		} CHIP_SELECT_CFG_STTD;

		/**
		 * @brief SPI pin configuration holding structure
		 */
		uC_BASE_SPI_IO_STRUCT const* m_config;

		/**
		 * @brief ***
		 */
		SPI_Type* m_spi;

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
		 * @brief ***
		 */
		CHIP_SELECT_CFG_STTD m_base_cfg;

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
