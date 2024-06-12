/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_SPI_H
   #define uC_SPI_H


#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_OS_Interrupt.h"
#include "OS_Semaphore.h"
#include "uC_DMA.h"


// #define uC_SPI_OS_SEMAPHORE_SUPPORT

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
enum
{
	uC_SPI_MODE_0,
	uC_SPI_MODE_1,
	uC_SPI_MODE_2,
	uC_SPI_MODE_3,
	uC_SPI_MODE_MAX
};

enum
{
	uC_SPI_BIT_COUNT_8 = 8,
	uC_SPI_BIT_COUNT_16 = 16
};


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_SPI
{
	public:

		uC_SPI( uint8_t spi_select, bool master,
				uC_USER_IO_STRUCT const* const* chip_sel_ctrls, uint8_t total_chip_sel,
				uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl = NULL, bool enable_dma = true );

		~uC_SPI( void );

		void Configure_Chip_Select( uint8_t spi_mode, uint8_t chip_select = 0, uint32_t desired_spi_clock_freq = 1,
									uint8_t bits_to_transfer = uC_SPI_BIT_COUNT_8, bool manual_chip_select = false );

		// ****************************
		// Simple single word access
		inline void TX_Byte( uint8_t data )     { m_spi->DR = data; }

		inline void TX_Word( uint16_t data )        { m_spi->DR = data; }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint8_t RX_Byte( void )
		{
			volatile uint16_t dump;

			dump = m_spi->DR;
			m_spi->DR = 0;
			while ( !RX_Data_Reg_Full() ){}
			return ( m_spi->DR );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Dump( void )
		{
			volatile uint16_t dump;

			dump = m_spi->DR;
			while ( RX_Data_Reg_Full() ){}
		}

		inline bool RX_Data_Reg_Full()                      { return ( ( m_spi->SR & SPI_SR_RXNE ) != 0 ); }

		inline bool TX_Data_Reg_Empty( void )               { return ( ( m_spi->SR & SPI_SR_TXE ) != 0 ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_Shift_Empty( void )
		{ return ( ( ( m_spi->SR & SPI_SR_BSY ) == 0 ) && ( ( m_spi->SR & SPI_SR_TXE ) != 0 ) ); }

		// ****************************
		// Chip Select Control
		void Select_Chip( uint8_t chip_select );

		void De_Select_Chip( uint8_t chip_select );

		// ****************************
		// Semaphore Control
		bool Get_Semaphore( uint32_t time_to_wait )
		{
			return ( true );						// m_semaphore->Pend( time_to_wait ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Release_Semaphore( void )
		{
			return ( true );						// m_semaphore->Post() );
		}

		void Set_RX_DMA( uint8_t* data, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Enable_RX_DMA( void )
		{ Push_TGINT();m_rx_dma->Enable();m_spi->CR2 |= SPI_CR2_RXDMAEN;Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Disable_RX_DMA( void )
		{ Push_TGINT();m_spi->CR2 &= ~SPI_CR2_RXDMAEN;m_rx_dma->Disable();Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool RX_DMA_Busy( void )
		{ return ( m_rx_dma->Busy() ); }

		void Set_TX_DMA( uint8_t* data, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Enable_TX_DMA( void )
		{ Push_TGINT();m_tx_dma->Enable();m_spi->CR2 |= SPI_CR2_TXDMAEN;Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Disable_TX_DMA( void )
		{ Push_TGINT();m_spi->CR2 &= ~SPI_CR2_TXDMAEN;m_tx_dma->Disable();Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_DMA_Busy( void )
		{ return ( m_tx_dma->Busy() ); }

		void Set_XChange_DMA( uint8_t* tx_data, uint8_t* rx_data, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Enable_XChange_DMA( void )
		{
			Push_TGINT();

			Dump();
			m_spi->CR2 |= ( SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN );
			m_rx_dma->Enable();
			m_tx_dma->Enable();

			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Disable_XChange_DMA( void )
		{
			Push_TGINT();

			m_rx_dma->Disable();
			m_tx_dma->Disable();
			m_spi->CR2 &= ~( SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN );

			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool XChange_DMA_Busy( void )
		{ return ( m_rx_dma->Busy() ); }

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
		typedef struct CHIP_SELECT_CFG_STTD
		{
			uint16_t cr1;
			uint16_t cr2;
			bool manual_pin_ctrl;
		} CHIP_SELECT_CFG_STTD;

		uC_BASE_SPI_IO_STRUCT const* m_config;
		SPI_TypeDef* m_spi;

		uC_USER_IO_STRUCT const* const* m_chip_sel_ctrls;
		uint8_t m_total_chip_sel;
		CHIP_SELECT_CFG_STTD* m_chip_sel_cfg;

		uC_DMA* m_rx_dma;
		uC_DMA* m_tx_dma;


#ifdef uC_SPI_OS_SEMAPHORE_SUPPORT
		OS_Semaphore* m_semaphore;
#endif
};


#endif



