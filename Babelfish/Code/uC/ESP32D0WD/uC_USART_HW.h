/**
 *****************************************************************************************
 *	@file		uC_USART_HW.h
 *
 *	@brief		USART Hardware configuration Interface
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_USART_HW_H
#define uC_USART_HW_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "freertos/portmacro.h"
#include "soc/soc.h"
#include "soc/uart_struct.h"
#include "soc/uart_reg.h"
#include "driver/uart.h"
#include "esp32/clk.h"

/**
 ****************************************************************************************
 * @brief Provides the low level interface and configuration  for the USART.
 *
 * @details
 *
 ****************************************************************************************
 */
class uC_USART_HW
{
	public:

		/**
		 * @brief The following callback types are for applications where you are planning on using
		 * @n The coroutine functionality.
		 */
		typedef void (* int_cback_t)( void );
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief Constructs a USART hardware configuration interface.
		 * @param[in] port_requested port_requested - Port number for UART configuration
		 * @n The uC_USART object will be be configured before it can
		 * @n be used by the uC_USART_DMA for baud rate, bits, etc.
		 * @param[in] io_port io_port - RX & TX port pin configurations to enable
		 * @n enable& set Pins.
		 * @param[in] enable_dma DMA enable/disable.
		 */

		uC_USART_HW( uint8_t port_requested,
					 uC_BASE_USART_IO_PORT_STRUCT const* io_port = NULL,
					 bool enable_dma = true );
		/**
		 * @brief Destructs a USART_HW interface.
		 */

		~uC_USART_HW( void ) {}

		/**
		 * @brief Enable UART RX functionality
		 * @return void	None
		 */
		void Enable_RX( void )  {}

		/**
		 * @brief Disable UART RX functionality
		 * @return void	None
		 */

		void Disable_RX( void )  {}

		/**
		 * @brief Enable UART TX functionality
		 * @return void	None
		 */
		void Enable_TX( void )  {}

		/**
		 * @brief Disable RX functionality
		 * @return[out] void
		 */
		void Disable_TX( void )  {}

		/**
		 * @brief UART configuration conditions for char size
		 *
		 */
		enum char_size_t
		{
			CHAR_SIZE_5BIT = 0,				///< UART char bit size 5
			CHAR_SIZE_6BIT = 1,				///< UART char bit size 6
			CHAR_SIZE_7BIT = 2,				///< UART char bit size 7
			CHAR_SIZE_8BIT = 3				///< UART char bit size 8
		};

		/**
		 * @brief UART configuration conditions for Parity
		 *
		 */
		enum parity_t
		{
			PARITY_NONE = 0x0,				///< UART Parity None
			PARITY_EVEN = 0x2,				///< UART Parity Even
			PARITY_ODD = 0x3,				///< UART Parity Odd
			PARITY_MAX_OPTIONS				///< UART Max Parity options
		};

		/**
		 * @brief UART configuration conditions for stop bit
		 *
		 */
		enum stop_bit_t
		{
			STOP_BIT_1 = 0x1,				///< UART Stop Bit 1
			STOP_BIT_1P5 = 0x2,				///< UART Stop Bit 1p5
			STOP_BIT_2 = 0x3				///< UART Stop Bit 2
		};

		/**
		 * @brief Configures the UART communication.Can be executed at any time.
		 * @param[in] baud_rate	Baud rate in bps.
		 * @param[in] char_size	Controls the number of bits per character.
		 * @param[in] parity Parity of the uart.
		 * @param[in] stop_bit_count Stop bit count.
		 * @return void
		 */
		void Configure( uint32_t baud_rate,
						char_size_t char_size = CHAR_SIZE_8BIT,
						parity_t parity = PARITY_NONE,
						stop_bit_t stop_bit_count = STOP_BIT_1 );

		/**
		 * @brief Configures the baud rate.
		 * @param[in] baud_rate	Baud rate in bps.
		 * @return void	None
		 */
		void Set_Baud_Rate( uint32_t baud_rate );

		/**
		 * @brief Returns the configured uart number.
		 * @param[in] void
		 * @return usart_num uart Port number
		 */
		uint8_t USART_Port_Number( void )   { return ( m_io_ctrl->usart_num ); }

		/**
		 * @brief Configures the UART Interrupt.
		 * @param[in] int_handler Interrupt handler
		 * @param[in] param Param for setting call back
		 * @param[in] priority Parity of the uart.
		 * @return void	None
		 */
		void Set_Int_Vect( cback_func_t int_handler, cback_param_t param, uint8_t priority ){}

	protected:

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		uC_USART_HW( const uC_USART_HW& rhs );
		uC_USART_HW & operator =( const uC_USART_HW& object );

		/**
		 * @brief Pointer to Base UART structure using for Hardware port configuration.
		 */
		uC_BASE_USART_IO_STRUCT* m_io_ctrl;
		/**
		 * @brief Set stop bit for UART.
		 * @param[in] stop_bit_t stop bit value to set
		 * @return Void	None
		 */

		void Set_Stop_Bit( stop_bit_t stop_bit );

		/**
		 * @brief Reset RX Fifo .
		 * @param[in] void None
		 * @return void	None
		 */
		void Reset_Rx_Fifo( void );

};

#endif
