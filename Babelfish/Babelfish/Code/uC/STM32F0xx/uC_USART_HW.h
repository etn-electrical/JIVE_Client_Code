/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Hardware Based USART Interface
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_USART_HW_H
#define uC_USART_HW_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "uC_DMA.h"
#include "Advanced_Vect.h"

/**
 ****************************************************************************************
 * @brief Provides the low level interface and config for the USART.  This will likely
 * always be replaced by another derived class.
 *
 * @details
 *
 ****************************************************************************************
 */
class uC_USART_HW
{
	public:
		typedef void (* int_cback_t)( void );
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		uC_USART_HW( uint8_t port_requested,
					 uC_BASE_USART_IO_PORT_STRUCT const* io_port = NULL,
					 bool enable_dma = true );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_USART_HW(){}

		void Enable_RX( void );

		void Disable_RX( void );

		void Enable_TX( void );

		void Disable_TX( void );

		enum char_size_t
		{
			CHAR_SIZE_5BIT = 5,
			CHAR_SIZE_6BIT = 6,
			CHAR_SIZE_7BIT = 7,
			CHAR_SIZE_8BIT = 8,
			CHAR_SIZE_9BIT = 9
		};

		enum parity_t
		{
			PARITY_EVEN,
			PARITY_ODD,
			PARITY_SPACE,
			PARITY_MARK,
			PARITY_NONE,
			PARITY_MAX_OPTIONS
		};

		enum stop_bit_t
		{
			STOP_BIT_1,
			STOP_BIT_1p5,
			STOP_BIT_2
		};

		void Configure( uint32_t baud_rate,
						char_size_t char_size = CHAR_SIZE_8BIT,
						parity_t parity = PARITY_NONE,
						stop_bit_t stop_bit_count = STOP_BIT_1 );

		void Set_Baud_Rate( uint32_t baud_rate );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		uint8_t USART_Port_Number( void )
		{
			return ( m_io_ctrl->usart_id );
		}

		void Set_Int_Vect( cback_func_t int_handler, cback_param_t param, uint8_t priority );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_Int( void )
		{
			uC_Interrupt::Enable_Int( m_io_ctrl->irq_num );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_Int( void )
		{
			uC_Interrupt::Disable_Int( m_io_ctrl->irq_num );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Int_Enabled( void )
		{
			return ( uC_Interrupt::Is_Enabled( m_io_ctrl->irq_num ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_RX_Ready_Int( void )
		{
			m_usart->CR1 |= USART_CR1_RXNEIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_RX_Ready_Int( void )
		{
			m_usart->CR1 &= ~USART_CR1_RXNEIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool RX_Ready_Int_Enabled( void )
		{
			return ( ( m_usart->CR1 & USART_CR1_RXNEIE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool RX_Ready_Int_Status( uint32_t status )
		{
			return ( ( status & USART_ISR_RXNE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_TX_Empty_Int( void )
		{
			m_usart->CR1 |= USART_CR1_TCIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_TX_Empty_Int( void )
		{
			m_usart->CR1 &= ~USART_CR1_TCIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_Empty_Int_Enabled( void )
		{
			return ( ( m_usart->CR1 & USART_CR1_TCIE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_Empty_Int_Status( uint32_t status )
		{
			return ( ( status & USART_ISR_TC ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_Empty_Status( void )
		{
			return ( ( m_usart->ISR & USART_ISR_TC ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool TX_Data_Empty_Status( void )
		{
			return ( ( m_usart->ISR & USART_ISR_TXE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Enable_Parity_Error_Int( void )
		{
			m_usart->CR1 |= USART_CR1_PEIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Disable_Parity_Error_Int( void )
		{
			m_usart->CR1 &= ~USART_CR1_PEIE;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Parity_Error_Int_Enabled( void )
		{
			return ( ( m_usart->CR1 & USART_CR1_PEIE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Parity_Error_Int_Status( uint32_t status )
		{
			return ( ( status & USART_ISR_PE ) != 0 );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Clear_Int_Status( void )
		{
			m_usart->ISR = 0;
		}

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
			return ( m_usart->ISR );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint8_t Get_RX_Data( void )
		{
			return ( m_usart->RDR & m_parity_bit_mask );
		}

		/* Clear the RXNE flag..
		   This allows to discard the received data without reading it, and avoid an overrun
		      condition.*/
		inline void Rx_Data_Flush( void )
		{
			m_usart->RQR &= ( ~USART_RQR_RXFRQ );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_TX_Data( uint32_t data )
		{
			m_usart->TDR = data;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Screen_RX_Data( uint8_t* data, uint16_t length )
		{
			if ( m_parity_bit_mask != 0xFFU )
			{
				for ( uint16_t i = 0; i < length; i++ )
				{
					data[i] &= m_parity_bit_mask;
				}
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint32_t Get_Error( void )
		{
			return ( m_usart->ISR & ( USART_ISR_PE | USART_ISR_FE
									  | USART_ISR_ORE | USART_ISR_NE ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Clear_Interrupt( uint32_t Flag_Mask )
		{
			m_usart->ICR &= ~( Flag_Mask );
		}

		/**
		 ****************************************************************************************
		 * @brief Mask the parity bit to get only the data.
		 * @param[in, out] data: Points to the start of receive buffer. The last received byte
		 * may get altered 
		 * @param[in] length: Total number of bytes received.
		 *
		 ****************************************************************************************
		 */
		inline void Screen_Last_RX_Byte( uint8_t* data, uint16_t length )
		{
			if ( m_parity_bit_mask != 0xFFU )
			{
				data[length-1] &= m_parity_bit_mask;
			}
		}
	protected:
		uC_DMA* m_rx_dma_ctrl;
		uC_DMA* m_tx_dma_ctrl;
		USART_TypeDef* m_usart;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		uC_USART_HW( const uC_USART_HW& rhs );
		uC_USART_HW & operator =( const uC_USART_HW& object );

		uC_BASE_USART_IO_STRUCT const* m_io_ctrl;
		uint16_t m_parity_bit_mask;

		Advanced_Vect* m_int_vect_cback;
};

#endif
