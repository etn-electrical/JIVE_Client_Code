/******************************************************************************
 * @file    uc_USART.cpp
 *
 * @brief   UART HAL module driver.
 *          This file provides firmware functions to manage the following
 *          functionalities of the Universal Asynchronous Receiver Transmitter
 *          (UART) peripheral:
 *           + Initialization and de-initialization functions
 *
 * @date    05 May 2017
 * @author  PXGREEN Team
 * @details Inital Draft
 *
 ******************************************************************************
   ================================================================================
 ##### How to use this driver #####
   ================================================================================
   [..]
   The UART HAL driver can be used as follows:

   (#) Declare a uC_BASE_USART_IO_PORT_STRUCT handle structure.

   (#) Initialize the UART low level resources by implementing the uC_USART :
   (##) Enable the USARTx interface clock.
   (##) UART pins configuration:
   (+++) Enable the clock for the UART GPIOs.
   (+++) Configure these UART pins as alternate function pull-up.
   (##) DMA Configuration if you need to use DMA process
   (+++) Declare a DMA handle structure for the Tx/Rx channel.
   (+++) Enable the DMAx interface clock.
   (+++) Configure the declared DMA handle structure with the required Tx/Rx parameters.
   (+++) Configure the DMA Tx/Rx channel.
   (+++) Associate the initilalized DMA handle to the UART DMA Tx/Rx handle.
   (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on the DMA
 #####Tx/Rx channel.

   (#) NVIC configuration if you need to use interrupt process Set_Int_Vect()
   (+++) Configure the USARTx interrupt priority.
   (+++) Declare and pass the call back function and Call back parameter

   (#) Program the Baud Rate in the Set_Baud_Rate()

   (#) Program the Word Length , Stop Bit, Parity in the Configure()

   (#) Enable and Disable Rx and Tx using Enable_TX() and Enable_RX() and Disable_TX()
   and Disable_RX()

   (#) Configuring the interrupt based on condition
   (+++) Read Data Register Not Empty Interrtupt(RXNE or RXNEIE) call Enable_RX_Ready_Int()
   (+++) Transmission complete Interrtupt(TC or TCIE) call Enable_TX_Empty_Int()
   (+++) Parity Error Interrupt (PE or PEIE) call Enable_Parity_Error_Int()

   (#) To read or write data call Get_RX_Data() or Set_TX_Data()
 */

/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_USART.h"
#include "Exception.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         uC_USART
 *  @brief      This constructor Enable the USARTx interface clock.
 *              Enable the clock for the UART GPIOs.
 *              Configure these UART pins as alternate function pull-up.
 *
 *  @param[in]  port_requested => GPIO port required to be enabled for UART
 *              io_port => Structure provides the pin to be used for Rx and Tx
 *              enable_dma => Decides whether UART with DMA or without DMA
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
uC_USART::uC_USART( uint8_t port_requested,
					const uC_BASE_USART_IO_PORT_STRUCT* io_port, bool enable_dma ) :
	m_io_ctrl( reinterpret_cast<uC_BASE_USART_IO_STRUCT const*>( nullptr ) ),
	m_usart( reinterpret_cast<USART_TypeDef*>( nullptr ) ),
	m_rx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma_ctrl( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_int_status_mask( 0U ),
	m_parity_bit_mask( 0U ),
	m_int_vect_cback( reinterpret_cast<Advanced_Vect*>( nullptr ) )
{
	m_io_ctrl = ( uC_Base::Self() )->Get_USART_Ctrl( port_requested );
	BF_ASSERT( m_io_ctrl != 0U );
	m_usart = m_io_ctrl->reg_ctrl;

	uC_Base::Reset_Periph( &m_io_ctrl->periph_def );
	uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );

	if ( io_port == NULL )
	{
		io_port = m_io_ctrl->default_port_io;
	}

	uC_IO_Config::Enable_Periph_Input_Pin( io_port->rx_port );
	uC_IO_Config::Enable_Periph_Output_Pin( io_port->tx_port );

	Disable_RX();
	Disable_TX();
#if DMA_ENABLED
	if ( ( m_io_ctrl->tx_dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) ) &&
		 ( m_io_ctrl->rx_dma_channel != static_cast<uint8_t>( uC_DMA_CHANNEL_DNE ) ) &&
		 ( enable_dma == true ) )
	{
		m_tx_dma_ctrl = new uC_DMA( m_io_ctrl->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
									static_cast<uint8_t>( sizeof( uint8_t ) ),
									uC_DMA_MEM_INC_TRUE,
									uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );
		m_rx_dma_ctrl = new uC_DMA( m_io_ctrl->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
									static_cast<uint8_t>( sizeof( uint8_t ) ),
									uC_DMA_MEM_INC_TRUE,
									uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );
	}
	else
	{
		m_tx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
		m_rx_dma_ctrl = reinterpret_cast<uC_DMA*>( nullptr );
	}
	m_int_status_mask = 0U;

	m_int_vect_cback = new Advanced_Vect();
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Enable_RX
 *  @brief      Enables the Receiver and UART
 *
 *  @param[in]
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Enable_RX( void )
{
	/*Enables the Receiver and UART Bit 2 and Bit 0 of CR1 set to 1 */
	m_usart->CR1 |= ( USART_CR1_RE | USART_CR1_UE );
}

/*******************************************************************************
 *  @fn         Disable_RX
 *  @brief      Disable the Receiver and UART
 *
 *  @param[in]
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Disable_RX( void )
{
	uint16_t temp_mask;

	temp_mask = USART_CR1_RE;
	if ( ( m_usart->CR1 & USART_CR1_TE ) == 0U )
	{
		temp_mask |= USART_CR1_UE;	// Disable the USART if the TX is not enabled.
	}
	/*Disables the Receiver and UART Bit 2 or Bit 0 of CR1 set to 0 */
	m_usart->CR1 &= ~temp_mask;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Enable_TX
 *  @brief      Enables the Transmitter and UART
 *
 *  @param[in]
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Enable_TX( void )
{
	/* Transmission and UART is enabled
	   Bit 3 and Bit 0 of CR1 set to 1*/
	m_usart->CR1 |= ( USART_CR1_TE | USART_CR1_UE );
}

/*******************************************************************************
 *  @fn         Disable_TX
 *  @brief      Disables the Transmitter and UART
 *
 *  @param[in]
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Disable_TX( void )
{
	uint16_t temp_mask;

	temp_mask = USART_CR1_TE;
	if ( ( m_usart->CR1 & USART_CR1_RE ) == 0U )
	{
		temp_mask |= USART_CR1_UE;	// Disable the USART if the TX is not enabled.
	}
	m_usart->CR1 &= ~temp_mask;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Set_Baud_Rate
 *  @brief      Program the Baud Rate at which the UART shall work
 *
 *  @param[in]  baud_rate => Provides the value of baud rate at which UART shall
 *                           work
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Set_Baud_Rate( uint32_t baud_rate )
{
	uint32_t usart_clock_freq;
	uint32_t temp_baud;

	usart_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq(
		m_io_ctrl->periph_def.pclock_reg_index );
	/* Over-Sampling by 16 method is used to calculate the baudrate */
	/* Example 2
	   To obtain 921.6 Kbaud with fCK = 48 MHz.
	   In case of oversampling by 16:
	   USARTDIV = 48 000 000/921 600 BRR = USARTDIV = 52d = 34h
	 */

	usart_clock_freq = usart_clock_freq << 4U;

	temp_baud = usart_clock_freq / ( baud_rate << MULT_BY_16 );

	m_usart->BRR = static_cast<uint16_t>( temp_baud );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*******************************************************************************
 *  @fn         Configure
 *  @brief      Configure the Word Length , Stop Bit, Parity.
 *              Parity = Even or odd or No parity
 *              Word Length = 7bit or 8bit or 9bit
 *              Stop Bit = 1bit or 1.5bit or 2bit
 *
 *  @param[in]  char_size => Size of data 7bit or 8bit or 9bit
 *              parity => Even or odd or No parity
 *              stop_bit_count => 1bit or 1.5bit or 2bit
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Configure( uint32_t char_size, uint32_t parity, uint32_t stop_bit_count )
{
	m_parity_bit_mask = 0x1FFU;	/// A data read will get the data including the parity bit in the
	// response.  We need to mask it off.

	// if parity is enabled we can only use 7 or 8bit character sizes because the 9th bit is for
	// parity.
	if ( parity != static_cast<uint32_t>( uC_USART_PARITY_NONE ) )
	{
		// Enable the parity checker.
		/*
		   Bit 10 PCE: Parity control enable
		   This bit selects the hardware parity control (generation and detection). When the parity
		   control is enabled, the computed parity is inserted at the MSB position (9th bit if M=1;
		      8th bit
		   if M=0) and parity is checked on the received data. This bit is set and cleared by
		      software.
		   Once it is set, PCE is active after the current byte (in reception and in transmission).
		   0: Parity control disabled
		   1: Parity control enabled
		 */
		m_usart->CR1 |= USART_CR1_PCE;
		/*
		   Bit 9 PS: Parity selection
		   0: Even parity
		   1: Odd parity
		 */
		switch ( parity )
		{
			case uC_USART_PARITY_EVEN:
				/* This selects Even Parity. */
				m_usart->CR1 &= ~USART_CR1_PS;
				break;

			case uC_USART_PARITY_ODD:
				/* This selects odd Parity.*/
				m_usart->CR1 |= USART_CR1_PS;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
		/*
		   The configuration of M0 & M1 register to be made for Character size
		   M[1:0] = 00: 1 Start bit, 8 data bits, n stop bits
		   M[1:0] = 01: 1 Start bit, 9 data bits, n stop bits
		   M[1:0] = 10: 1 Start bit, 7 data bits, n stop bits
		   Bit 28(M1) and Bit 12(M0) are required to be modifed
		 */
		switch ( char_size )
		{

			case uC_USART_CHAR_SIZE_9BIT:

				/* Set the M0 bit of the CR1 register */
				m_usart->CR1 |= ( USART_CR1_M );
				/*Clear the M1 bit of the CR1 register */
				m_usart->CR1 &= ~( 0x10000000 );
				break;

			case uC_USART_CHAR_SIZE_8BIT:

				/*Clear the M0 bit of the CR1 register */
				m_usart->CR1 &= ~( USART_CR1_M );
				/*Clear the M1 bit of the CR1 register */
				m_usart->CR1 &= ~( 0x10000000 );
				m_parity_bit_mask = 0xFFU;
				break;

			case uC_USART_CHAR_SIZE_7BIT:

				/*Clear the M0 bit of the CR1 register */
				m_usart->CR1 &= ~( USART_CR1_M );
				/*Set the M1 bit of the CR1 register */
				m_usart->CR1 |= ( 0x10000000 );
				m_parity_bit_mask = 0x7FU;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
	}
	else if ( char_size == static_cast<uint32_t>( uC_USART_CHAR_SIZE_9BIT ) )
	{
		/* Parity control disable */
		m_usart->CR1 &= ~USART_CR1_PCE;
		/* Set the M0 bit of the CR1 register */
		m_usart->CR1 |= ( USART_CR1_M );
		/*Clear the M1 bit of the CR1 register */
		m_usart->CR1 &= ~( 0x10000000 );
	}
	else if ( char_size == static_cast<uint32_t>( uC_USART_CHAR_SIZE_8BIT ) )
	{
		/*Clear the M0 bit and PCE bit of the CR1 register */
		m_usart->CR1 &= ~( USART_CR1_PCE | USART_CR1_M );
		/*Clear the M1 bit of the CR1 register */
		m_usart->CR1 &= ~( 0x10000000 );
	}
	else// We are unable to support anything else.  Like 7bit with no parity.
	{
#ifdef DEBUG
		BF_ASSERT( false );
#else
		m_usart->CR1 &= ~( USART_CR1_PCE | USART_CR1_M );
#endif
	}

	/*
	   Bits 13:12 STOP[1:0]: STOP bits
	   These bits are used for programming the stop bits.
	   00: 1 stop bit
	   01: Reserved
	   10: 2 stop bits
	   11: 1.5 stop bits
	 */
	/* Clear the stop bits */
	m_usart->CR2 &= ~USART_CR2_STOP;
	switch ( stop_bit_count )
	{
		case uC_USART_1p5_STOP_BIT:
			/* This selects 1.5 stop bits. USART_CR2_STOP = 0x00003000 */
			m_usart->CR2 |= USART_CR2_STOP;
			break;

		case uC_USART_2_STOP_BIT:
			/* This selects 2 stop bits. USART_CR2_STOP_1 = 0x00002000 */
			m_usart->CR2 |= USART_CR2_STOP_1;
			break;

		case uC_USART_1_STOP_BIT:
			/* This selects 1 stop bits.  USART_CR2_STOP = 0x00003000 */
			m_usart->CR2 &= ~( USART_CR2_STOP );

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

/*******************************************************************************
 *  @fn         Set_Int_Vect
 *  @brief      Configure the USARTx interrupt priority.
 *              Declare and pass the call back function and Call back parameter
 *
 *  @param[in]  int_handler => Call back function to be used
 *              param => Call back parameter to be used
 *              priority => priority of the USART interrrupt
 *  @param[out] None
 *  @return     None
 ******************************************************************************/
void uC_USART::Set_Int_Vect( cback_func_t int_handler,
							 cback_param_t param, uint8_t priority ) const
{
	m_int_vect_cback->Set_Vect( int_handler, param );
	uC_Interrupt::Set_Vector(
		reinterpret_cast<INT_CALLBACK_FUNC*>( m_int_vect_cback->Get_Int_Func() ),
		m_io_ctrl->irq_num, priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_OS_Int_Vect( cback_func_t int_handler,
								cback_param_t param, uint8_t priority ) const
{
	m_int_vect_cback->Set_Vect( int_handler, param );
	uC_OS_Interrupt::Set_OS_Vect(
		reinterpret_cast<INT_CALLBACK_FUNC*>( m_int_vect_cback->Get_Int_Func() ),
		m_io_ctrl->irq_num, priority );
}

#if DMA_ENABLED
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_tx_dma_ctrl->Disable();

	m_tx_dma_ctrl->Set_Buff( data, &m_usart->TDR, length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_USART::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_rx_dma_ctrl->Disable();

	m_rx_dma_ctrl->Set_Buff( data, &m_usart->RDR, length );
}

#endif
