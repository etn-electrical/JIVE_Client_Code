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
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_USART_SOFT_DMA_H
   #define uC_USART_SOFT_DMA_H

#include "Includes.h"
#include "uC_USART_HW.h"
#include "uC_USART_Buff.h"
#include "uC_Multi_Timers.h"

/**
 ****************************************************************************************
 * @brief
 *
 * @details
 *
 ****************************************************************************************
 */
class uC_USART_Soft_DMA : public uC_USART_HW
	, public uC_USART_Buff
{
	public:
		/**
		 * @brief Constructs a USART interface.
		 * @param usart_handle - Handle to the standard uC_USART object.  The uC_USART
		 * object must be preconfigured before it can be used by the uC_USART_DMA.  Preconfigured
		 * for baud rate, bits, etc.
		 * @param timer_ctrl - uC_Timer used for RX and TX.  Provides the timeout.
		 */
		uC_USART_Soft_DMA( uint8_t hw_usart_num, uint8_t hw_timer_num,
						   const uC_BASE_USART_IO_PORT_STRUCT* io_port = NULL );

		/**
		 * @brief The destructor.
		 */
		~uC_USART_Soft_DMA( void );

		/**
		 * @brief Configures the callback.  You pass in the function and parameter you want to be called
		 * as well as the reason(s) you want to be called.
		 * @param cback_func - The actual function.
		 * @param cback_param - Typically the "this" pointer.  Can be null.
		 * @param cback_req_mask - The bitfield indicating what reasons you want to be called.
		 * @param rx_byte_time (microseconds) - Wire quiet time before we consider it End of Message.  This can also be
		 * 0
		 * which indicates no timeout.
		 * @param tx_holdoff_time (microseconds) - Wire quiet time before we consider it End of Message.  This can also
		 * be 0
		 * which indicates no timeout.
		 */
		void Config_Callback( uC_USART_Buff::cback_func_t cback_func,
							  uC_USART_Buff::cback_param_t cback_param, uC_USART_Buff::cback_status_t cback_req_mask,
							  uint32_t rx_byte_time = 0U, uint32_t tx_holdoff_time = 0U );

		/**
		 * @brief Starts the receive method.  It will wait until one of the conditions is met before making
		 * the callback.
		 * @param data - Pointer to your target data string.
		 * @param length - Length of data you want to receive or max length that you can receive.
		 */
		void Start_RX( uint8_t* data, uint16_t length );

		/**
		 * @brief After an RX occurs the length could be less than the total length sent in.
		 * @return The length of data received.
		 */
		uint16_t Get_RX_Length( void );

		/**
		 * @brief Returns whether we are actively receiving or are triggered to receive.
		 * @return Whether we are actively waiting for a message or whether we are idle.  Idle = false.
		 */
		uC_USART_Buff::rx_state_t RX_State( void );

		/**
		 * @brief Aborts the receive process and puts the receiver into idle.
		 */
		void Stop_RX( void );


		/**
		 * @brief Starts the transmit method.  It will send out the data buffer immediately.
		 * @param data - Pointer to your target data string.  If a null data pointer or zero datalength
		 * is passed in, the tx holdoff will be kicked but no data will be sent.
		 * @param length - Length of data you want to transmit.
		 */
		void Start_TX( uint8_t* data, uint16_t length );

		/**
		 * @brief Returns whether we are actively transmitting.
		 * @return Whether we are actively transmitting.  Idle = false.
		 */
		uC_USART_Buff::tx_state_t TX_State( void );

		/**
		 * @brief Returns whether the holdoff time is active or not.
		 * @return Indicates that the holdoff is active.  Active = true.
		 */
		bool TX_Holdoff( void );

		/**
		 * @brief Aborts the transmit process and puts the transmitter into idle.
		 */
		void Stop_TX( void );

		/**
		 * @brief Configures the port.  Can be executed at any time.
		 * @param baud_rate - Baud rate in bps.
		 * @param char_size - Controls the number of bits per character.
		 * @param parity - Parity of the uart.
		 * @param stop_bit_count - Stop bit count.
		 * @return success - True if the configuration succeeded.  False if it failed to config.
		 */
		bool Config_Port( uint32_t baud_rate,
						  uC_USART_Buff::char_size_t char_size = uC_USART_Buff::CHAR_SIZE_8BIT,
						  uC_USART_Buff::parity_t parity = uC_USART_Buff::PARITY_NONE,
						  uC_USART_Buff::stop_bit_t stop_bit_count = uC_USART_Buff::STOP_BIT_1 );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		uC_USART_Soft_DMA( const uC_USART_Soft_DMA& rhs );
		uC_USART_Soft_DMA & operator =( const uC_USART_Soft_DMA& object );

		void USART_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void USART_Int_Static( uC_USART_HW::cback_param_t handle )
		{ ( ( uC_USART_Soft_DMA* )handle )->USART_Int(); }

		void RX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void RX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{ ( ( uC_USART_Soft_DMA* )handle )->RX_Timer_Int(); }

		void TX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void TX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{ ( ( uC_USART_Soft_DMA* )handle )->TX_Timer_Int(); }

		uint8_t* m_rx_buff;
		uint16_t m_rx_buff_len;
		uint16_t m_rx_buff_index;

		uint8_t* m_tx_buff;
		uint16_t m_tx_buff_len;
		uint16_t m_tx_buff_index;

		uC_USART_Buff::cback_status_t m_cback_request;
		uC_USART_Buff::cback_func_t m_cback_func;
		uC_USART_Buff::cback_param_t m_cback_param;

		bool m_tx_holdoff;

		uC_Multi_Timers* m_timer;

		rx_state_t m_rx_state;
		tx_state_t m_tx_state;

		uint16_t m_int_enabled_mask;
		bool m_premature_rx_timeout;
		bool m_premature_tx_timeout;

		/// We are converting from microseconds to hertz so we need to boost some stuff up.
		static const uint32_t M_TIME_TO_FREQ_CALC = 1000000;

		enum
		{
			RX_TIMER,
			TX_TIMER,
			TOTAL_TIMERS
		};

};


#endif
