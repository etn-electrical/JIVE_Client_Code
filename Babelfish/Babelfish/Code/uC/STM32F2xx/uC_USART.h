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
#ifndef uC_USART_H
   #define uC_USART_H

#include "uC_Interrupt.h"
#include "uC_OS_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "uC_DMA.h"
#include "Advanced_Vect.h"

/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */
#define USART_INTERRUPT_PRIORITY        uC_INT_HW_PRIORITY_5

enum
{
	uC_USART_CHAR_SIZE_5BIT = 5,
	uC_USART_CHAR_SIZE_6BIT = 6,
	uC_USART_CHAR_SIZE_7BIT = 7,
	uC_USART_CHAR_SIZE_8BIT = 8,
	uC_USART_CHAR_SIZE_9BIT = 9
};

enum
{
	uC_USART_SAMPLE_COUNT_8,
	uC_USART_SAMPLE_COUNT_16
};	// Sample counter. This is essentially the oversampling.

enum
{
	uC_USART_PARITY_EVEN,
	uC_USART_PARITY_ODD,
	uC_USART_PARITY_SPACE,
	uC_USART_PARITY_MARK,
	uC_USART_PARITY_NONE,
	uC_USART_PARITY_MULTIDROP,
	uC_USART_PARITY_MAX_OPTIONS
};

enum
{
	uC_USART_1_STOP_BIT,
	uC_USART_1p5_STOP_BIT,
	uC_USART_2_STOP_BIT
};

enum
{
	uC_USART_INT_RX_READY,		// = AT91C_US_RXRDY,
	uC_USART_INT_TX_READY,		// = AT91C_US_TXRDY,
	uC_USART_INT_BREAK_RXED,	// = AT91C_US_RXBRK,
	uC_USART_INT_END_OF_RX,		// = AT91C_US_ENDRX,
	uC_USART_INT_END_OF_TX,		// = AT91C_US_ENDTX,
	uC_USART_INT_PARITY_ERROR,	// = AT91C_US_PARE,
	uC_USART_INT_RX_TIMEOUT,	// = AT91C_US_TIMEOUT,
	uC_USART_INT_TX_EMPTY,		// = AT91C_US_TXEMPTY,
	uC_USART_INT_TX_BUF_EMPTY,	// = AT91C_US_TXBUFE,
	uC_USART_INT_RX_BUF_EMPTY	// = AT91C_US_RXBUFF
};


/*
 **************************************************************************************************
 *  Typedefs
   --------------------------------------------------------------------------------------------------
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_USART
{
	public:
		typedef void (* int_cback_t)( void );
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/*
		 *******************************************************
		 * The following is a simplified USART implementation.
		 * It implements a rudimentary queue functionality.
		 *******************************************************

		                uC_USART( uint8_t port_requested, uint16_t que_byte_size );
		        BOOL	Enque( uint16_t data );		// Send
		        BOOL	Enque( uint8_t data );		// Send
		        BOOL 	Enque_Full( void );
		        BOOL 	Enque_Empty( void );

		        BOOL	Deque( uint16_t* data );		// Receive
		        BOOL	Deque( uint8_t* data );		// Receive
		        BOOL 	Deque_Full( void );
		        BOOL 	Deque_Empty( void );
		 */

		/*
		 *******************************************************
		 * The following is the full featured access.
		 *******************************************************
		 */
		uC_USART( uint8_t port_requested,
				  const uC_BASE_USART_IO_PORT_STRUCT* io_port = NULL,
				  bool enable_dma = true );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_USART() {}

		void Enable_RX( void );

		void Disable_RX( void );

		void Enable_TX( void );

		void Disable_TX( void );

		void Set_Baud_Rate( uint32_t baud_rate );

		void Configure( uint32_t char_size = uC_USART_CHAR_SIZE_8BIT,
						uint32_t parity = uC_USART_PARITY_NONE,
						uint32_t stop_bit_count = uC_USART_1_STOP_BIT );

		uint8_t USART_Port_Number( void )       { return ( m_io_ctrl->usart_id ); }

		void Set_Int_Vect( cback_func_t int_handler, cback_param_t param, uint8_t priority ) const;

		void Set_OS_Int_Vect( cback_func_t int_handler, cback_param_t param, uint8_t priority ) const;

		void Set_TX_DMA( uint8_t* data, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_TX_DMA( void )
		{ Push_TGINT();m_tx_dma_ctrl->Enable();m_usart->CR3 |= USART_CR3_DMAT;Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_TX_DMA( void )
		{ Push_TGINT();m_usart->CR3 &= ~USART_CR3_DMAR;m_tx_dma_ctrl->Disable();Pop_TGINT(); }

		inline uint16_t Get_TX_DMA_Length( void ) { return ( m_tx_dma_ctrl->Counts_Remaining() ); }

		void Set_RX_DMA( uint8_t* data, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_RX_DMA( void )
		{ Push_TGINT();m_rx_dma_ctrl->Enable();m_usart->CR3 |= USART_CR3_DMAR;Pop_TGINT(); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_RX_DMA( void )
		{ Push_TGINT();m_usart->CR3 &= ~USART_CR3_DMAR;m_rx_dma_ctrl->Disable();Pop_TGINT(); }

		inline uint16_t Get_RX_DMA_Length( void ) { return ( m_rx_dma_ctrl->Counts_Done() ); }

		inline void Enable_Int( void )          { uC_Interrupt::Enable_Int( m_io_ctrl->irq_num ); }

		inline void Disable_Int( void )         { uC_Interrupt::Disable_Int( m_io_ctrl->irq_num ); }

		inline bool Int_Enabled( void )         { return ( uC_Interrupt::Is_Enabled( m_io_ctrl->irq_num ) ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_RX_Ready_Int( void )
		{ m_int_status_mask |= USART_SR_RXNE;m_usart->CR1 |= USART_CR1_RXNEIE; }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_RX_Ready_Int( void )
		{ m_int_status_mask &= ~USART_SR_RXNE;m_usart->CR1 &= ~USART_CR1_RXNEIE; }

		inline bool RX_Ready_Int_Enabled( void )            { return ( ( m_usart->CR1 & USART_CR1_RXNEIE ) != 0 ); }

		inline bool RX_Ready_Int_Status( uint32_t status )  { return ( ( status & USART_SR_RXNE ) != 0 ); }

		inline bool RX_Ready_Status( void )                 { return ( ( m_usart->SR & USART_SR_RXNE ) != 0 ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_TX_Empty_Int( void )
		{ m_int_status_mask |= USART_SR_TC;m_usart->CR1 |= USART_CR1_TCIE; }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_TX_Empty_Int( void )
		{ m_int_status_mask &= ~USART_SR_TC;m_usart->CR1 &= ~USART_CR1_TCIE; }

		inline bool TX_Empty_Int_Enabled( void )                { return ( ( m_usart->CR1 & USART_CR1_TCIE ) != 0 ); }

		inline bool TX_Empty_Int_Status( uint32_t status )      { return ( ( status & USART_SR_TC ) != 0 ); }

		inline bool TX_Empty_Status( void )                     { return ( ( m_usart->SR & USART_SR_TC ) != 0 ); }

		inline bool TX_Data_Empty_Status( void )                { return ( ( m_usart->SR & USART_SR_TXE ) != 0 ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_Parity_Error_Int( void )
		{ m_int_status_mask |= USART_SR_PE;m_usart->CR1 |= USART_CR1_PEIE; }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_Parity_Error_Int( void )
		{ m_int_status_mask &= ~USART_SR_PE;m_usart->CR1 &= ~USART_CR1_PEIE; }

		inline bool Parity_Error_Int_Enabled( void )            { return ( ( m_usart->CR1 & USART_CR1_PEIE ) != 0 ); }

		inline bool Parity_Error_Int_Status( uint32_t status )  { return ( ( status & USART_SR_PE ) != 0 ); }

		inline void Clear_Int_Status( void )                { m_usart->SR = 0; }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_All_Ints( void )
		{
			m_usart->CR1 &= ~( USART_CR1_PEIE | USART_CR1_TCIE |
							   USART_CR1_TCIE | USART_CR1_RXNEIE | USART_CR1_IDLEIE );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint32_t Get_Int_Status_Bits( void )
		{
			return ( m_int_status_mask & m_usart->SR );
		}

		inline uint8_t Get_RX_Data( void )         { return ( m_usart->DR & m_parity_bit_mask ); }

		inline void Set_TX_Data( uint32_t data )    { m_usart->DR = data; }

	private:
		uC_BASE_USART_IO_STRUCT const* m_io_ctrl;
		USART_TypeDef* m_usart;

		uC_DMA* m_rx_dma_ctrl;
		uC_DMA* m_tx_dma_ctrl;
		uint32_t m_int_status_mask;
		uint16_t m_parity_bit_mask;

		Advanced_Vect* m_int_vect_cback;

};

#endif
