/**
 **********************************************************************************************************************
 *	@file		uC_USART_DMA.h Header File for Direct Memory Access RX and TX functionality implementation with USART as
 * main peripheral.
 *
 *	@brief		This file shall wrap all the functions required for data transfer using USART_DMA
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 **********************************************************************************************************************
 */
#ifndef UC_USART_DMA_H
   #define UC_USART_DMA_H

#include "uC_USART_Buff.h"
#include "uC_UHCI.h"
#include "uC_IO_Define_ESP32.h"
#include "uC_USART_HW.h"

/**
 ****************************************************************************************
 * @brief			uC_USART_DMA Class Declaration. This class will handle all the
 * @n				functionalities related to USART_DMA data transfer and will wrap all the
 * @n				methods and parameters required for the RX and TX.
 ****************************************************************************************
 */
class uC_USART_DMA : public uC_USART_HW
	, public uC_USART_Buff
{
	public:
		/**
		 * @brief											Constructs a USART interface.
		 * @param[in] hw_usart_num							usart_handle - Handle to the standard uC_USART object
		 * @n												The uC_USART object must be pre configured before it can
		 * @n												be used by the uC_USART_DMA for baud rate, bits, etc.
		 * @param[in] hw_timer_num							timer_ctrl - uC_Timer used for Rx and Tx operations using
		 * @n												DMA, it provides the timeout.
		 * @param[in] io_port								Constant IO port
		 * @return[out] none								none
		 */
		uC_USART_DMA( uint8_t hw_usart_num, uint8_t hw_timer_num,
					  const uC_BASE_USART_IO_PORT_STRUCT* io_port = NULL );

		/**
		 * @brief Destructs a USART interface.
		 */
		~uC_USART_DMA( void );

		/**
		 * @brief                                       Configures the callback.  You pass in the function and parameter
		 * @n											you want to be called
		 * @n											as well as the reason(s) you want to be called.
		 * @param[in] cback_func                        The actual function.
		 * @param[in] cback_param                       Typically the "this" pointer.  Can be null.
		 * @param[in] cback_req_mask					The bit field indicating what reasons you want to be called.
		 * @param[in] rx_byte_time						Wire quiet time before we consider it End of Message.  This can
		 * @n											also be 0 .In (microseconds)
		 * @n											which indicates no timeout.
		 * @param[in] tx_holdoff_time					Wire quiet time before we consider it End of Message.  This can
		 * @n											also be 0. In (microseconds).
		 * @n											which indicates no timeout.
		 * @return[out]									none
		 */
		void Config_Callback( uC_USART_Buff::cback_func_t cback_func,
							  uC_USART_Buff::cback_param_t cback_param, uC_USART_Buff::cback_status_t cback_req_mask,
							  uint32_t rx_byte_time = 0U, uint32_t tx_holdoff_time = 0U );

		/**
		 * @brief										Starts the receive method.  It will wait until one of the
		 * @n											conditions is met before making the callback.
		 * @param[in] data								Pointer to your target data string.
		 * @param[in] length							Length of data you want to receive or max length
		 * @n											that you can receive.
		 * @return[out] none							None.
		 */
		void Start_RX( uint8_t* data, uint16_t length );

		/**
		 * @brief										After an RX occurs the length could be less than
		 * @n											the total length sent in.
		 * @return[out] uint16_t						The length of data received.
		 */
		uint16_t Get_RX_Length( void );

		/**
		 * @brief										Returns whether we are actively receiving or are
		 * @n											triggered to receive.
		 * @return[out]									Whether we are actively waiting for a message
		 * @n											or whether we are idle.  Idle = false.
		 */
		uC_USART_Buff::rx_state_t RX_State( void );

		/**
		 * @brief										Aborts the receive process and puts the receiver into idle.
		 * @return[out] none							None.
		 */
		void Stop_RX( void );

		/**
		 * @brief										Starts the transmit method.
		 * @n											It will send out the data buffer immediately.
		 * @param[in] data								Pointer to your target data string.
		 * @n											If a null data pointer or zero data length is passed in, the
		 * @n											tx hold off will be kicked but no data will be sent.
		 * @param[in] length							Length of data you want to transmit.
		 * @return[out] none							None.
		 */
		void Start_TX( uint8_t* data, uint16_t length );

		/**
		 * @brief										Returns whether we are actively transmitting.
		 * @return[out]									Whether we are actively transmitting.  Idle = false.
		 */
		uC_USART_Buff::tx_state_t TX_State( void );

		/**
		 * @brief										Returns whether the hol doff time is active or not.
		 * @return[out]									Indicates that the hold off is active.  Active = true.
		 */
		bool TX_Holdoff( void );

		/**
		 * @brief										Aborts the transmit process and puts the transmitter into idle.
		 * @return[out] none							None
		 */
		void Stop_TX( void );

		/**
		 * @brief										Configures the port. Can be executed at any time.
		 * @param[in] baud_rate							Baud rate in bps.
		 * @param[in] char_size							Controls the number of bits per character.
		 * @param[in] parity							Parity of the uart.
		 * @param[in] stop_bit_count					Stop bit count.
		 * @return[out] bool_t							success - True if the configuration succeeded.
		 * @n											False if it failed to config.
		 */
		bool Config_Port( uint32_t baud_rate,
						  uC_USART_Buff::char_size_t char_size = uC_USART_Buff::CHAR_SIZE_8BIT,
						  uC_USART_Buff::parity_t parity = uC_USART_Buff::PARITY_NONE,
						  uC_USART_Buff::stop_bit_t stop_bit_count = uC_USART_Buff::STOP_BIT_1 );

	private:
		/**
		 * @brief							We are converting from microseconds to hertz so we need to boost
		 * @n								some stuff up.
		 * @n								The multi timer indexes.
		 */
		static const uint32_t M_TIME_TO_FREQ_CALC = 1000000;
		static const uint8_t RX_TIMER = 0;
		static const uint8_t TX_TIMER = 1;
		static const uint8_t TOTAL_TIMERS = 2;

		/**
		 * @brief											Copy Constructor.
		 * @param[in] rhs									uC_USART_DMA object pointer
		 */
		uC_USART_DMA( const uC_USART_DMA& rhs );

		/**
		 * @brief											Copy Assignment Operator definitions.
		 * @param[in] object								uC_USART_DMA object pointer
		 */
		uC_USART_DMA & operator =( const uC_USART_DMA& object );

		/**
		 * @brief											USART Initialization
		 * @return none										None
		 */
		void USART_Int( void );

		/**
		 * @brief											uC_USART_DMA Private member variables
		 * @n												Call-Back status request variable
		 * @n												Call-back function pointer .
		 * @n												Call-back param
		 * @n												Receive DMA remaining counts
		 * @n												Transmit hold off condition.
		 * @n												uC_UHCI Object pointer.
		 * @n												Receive state
		 * @n												Transmit state
		 * @n												Interrupt masks.
		 * @n												Receive timeout
		 * @n												Transmit timeout
		 */
		uC_USART_Buff::cback_status_t m_cback_request;
		uC_USART_Buff::cback_func_t m_cback_func;
		uC_USART_Buff::cback_param_t m_cback_param;
		uint16_t m_rx_dma_counts_remaining;
		bool m_tx_holdoff;
		uC_UHCI* m_udma_ctrl;
		rx_state_t m_rx_state;
		tx_state_t m_tx_state;
		uint16_t m_int_enabled_mask;
		bool m_premature_rx_timeout;
		bool m_premature_tx_timeout;
};
#endif
