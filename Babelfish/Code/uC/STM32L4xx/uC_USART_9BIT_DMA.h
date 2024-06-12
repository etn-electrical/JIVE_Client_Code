/**
 *****************************************************************************************
 *	@file			uC_USART_9BIT_DMA.h Header File for Direct Memory Access RX and TX functionality implementation with
 *USART as main peripheral.
 *
 *	@brief			This file shall wrap all the functions required for data transfer using USART_DMA
 *
 *	@details
 *
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */
#ifndef uC_USART_9BIT_DMA_H
#define uC_USART_9BIT_DMA_H

#include "Includes.h"
#include "uC_USART_9BIT_HW.h"
#include "uC_USART_9BIT_Buff.h"
#include "uC_Multi_Timers.h"


/**
 **********************************************************************************************
 * @brief        uC_USART_9BIT_DMA Class Declaration. This class will handle all the
 *               functionalities related to USART_DMA data transfer and will wrap all the
 *               methods and parameters required for the RX and TX.
 **********************************************************************************************
 */

class uC_USART_9BIT_DMA : public uC_USART_9BIT_HW
	, public uC_USART_9BIT_Buff
{
	public:
		/**
		 * @brief       									Constructs a USART interface.
		 * @param[in] 	uint8_t	 hw_usart_num				usart_handle - Handle to the standard uC_USART object
		 * @n                                 				The uC_USART object must be pre configured before it can be
		 *used by the uC_USART_9BIT_DMA
		 * @n                                 				for baud rate, bits, etc.
		 * @param[in] 	uint8_t	 hw_timer_num				timer_ctrl - uC_Timer used for Rx and Tx operations using
		 *DMA, it provides the timeout.
		 * @n
		 * @n
		 * @param[in] 	const	 uC_BASE_USART_IO_PORT_STRUCT
		 * @return[out] none	 none
		 */
		uC_USART_9BIT_DMA( uint8_t hw_usart_num, uint8_t hw_timer_num,
						   const uC_BASE_USART_IO_PORT_STRUCT* io_port = NULL );

		/**
		 * @brief Destructs a USART interface.
		 */
		~uC_USART_9BIT_DMA( void );

		/**
		 * @brief 										Configures the callback.  You pass in the function and parameter
		 *you want to be called
		 * @n											as well as the reason(s) you want to be called.
		 * @param[in] 	  cback_func - 					The actual function.
		 * @param[in]	  cback_param - 				Typically the "this" pointer.  Can be null.
		 * @param[in] 	  cback_req_mask 				The bit field indicating what reasons you want to be called.
		 * @param[in] 	  rx_byte_time (microseconds)   Wire quiet time before we consider it End of Message.  This can
		 *also be 0
		 * @n										  	which indicates no timeout.
		 * @param[in] 	  tx_holdoff_time (microseconds)Wire quiet time before we consider it End of Message.  This can
		 *also be 0
		 * @n												which indicates no timeout.
		 * @return[out]   none
		 */
		void Config_Callback( uC_USART_9BIT_Buff::cback_func_t cback_func,
							  uC_USART_9BIT_Buff::cback_param_t cback_param,
							  uC_USART_9BIT_Buff::cback_status_t cback_req_mask,
							  uint32_t rx_byte_time = 0U, uint32_t tx_holdoff_time = 0U );

		/**
		 * @brief 							    Starts the receive method.  It will wait until one of the conditions is
		 *met before making the callback.
		 * @param[in]	uint8_t	    data		Pointer to your target data string.
		 * @param[in]	uint16_t	length		Length of data you want to receive or max length that you can receive.
		 * @return[out] none
		 */
		void Start_RX( uint8_t* data, uint16_t length );

		/**
		 * @brief 						After an RX occurs the length could be less than the total length sent in.
		 * @return[out] 	uint16_t	The length of data received.
		 */
		uint16_t Get_RX_Length( void );

		/**
		 * @brief 			Returns whether we are actively receiving or are triggered to receive.
		 * @return[out] 	Whether we are actively waiting for a message or whether we are idle.  Idle = false.
		 */
		uC_USART_9BIT_Buff::rx_state_t RX_State( void );

		/**
		 * @brief 		Aborts the receive process and puts the receiver into idle.
		 * @return[out]	none
		 */
		void Stop_RX( void );

		/**
		 * @brief		 				Starts the transmit method.  It will send out the data buffer immediately.
		 * @param[in]	uint8_t	data	Pointer to your target data string.  If a null data pointer or zero datalength
		 * @n 							is passed in, the tx holdoff will be kicked but no data will be sent.
		 * @param[in]	uint16_t	length 	Length of data you want to transmit.
		 */
		void Start_TX( uint8_t* data, uint16_t length );

		/**
		 * @brief 			Returns whether we are actively transmitting.
		 * @return[out] 	Whether we are actively transmitting.  Idle = false.
		 */
		uC_USART_9BIT_Buff::tx_state_t TX_State( void );

		/**
		 * @brief			Returns whether the holdoff time is active or not.
		 * @return[out] 	Indicates that the holdoff is active.  Active = true.
		 */
		bool TX_Holdoff( void );

		/**
		 * @brief 			Aborts the transmit process and puts the transmitter into idle.
		 * @return[out]		none
		 */
		void Stop_TX( void );

		/**
		 * @brief 									Configures the port. Can be executed at any time.
		 * @param[in]	uint32_t	baud_rate		Baud rate in bps.
		 * @param[in]	 			char_size		Controls the number of bits per character.
		 * @param[in]			 	parity			Parity of the uart.
		 * @param[in]				stop_bit_count	Stop bit count.
		 * @return[out] bool_t						success - True if the configuration succeeded.  False if it failed
		 *to config.
		 */
		bool Config_Port( uint32_t baud_rate,
						  uC_USART_9BIT_Buff::char_size_t char_size = uC_USART_9BIT_Buff::CHAR_SIZE_8BIT,
						  uC_USART_9BIT_Buff::parity_t parity = uC_USART_9BIT_Buff::PARITY_NONE,
						  uC_USART_9BIT_Buff::stop_bit_t stop_bit_count = uC_USART_9BIT_Buff::STOP_BIT_1 );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		uC_USART_9BIT_DMA( const uC_USART_9BIT_DMA& rhs );
		uC_USART_9BIT_DMA & operator =( const uC_USART_9BIT_DMA& object );

		void USART_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void USART_Int_Static( uC_USART_9BIT_HW::cback_param_t handle )
		{
			( ( uC_USART_9BIT_DMA* )handle )->USART_Int();
		}

		void RX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void RX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{
			( ( uC_USART_9BIT_DMA* )handle )->RX_Timer_Int();
		}

		void TX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void TX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{
			( ( uC_USART_9BIT_DMA* )handle )->TX_Timer_Int();
		}

		void RX_DMA_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void RX_DMA_Int_Static( uC_DMA::cback_param_t handle )
		{
			( ( uC_USART_9BIT_DMA* )handle )->RX_DMA_Int();
		}

		void TX_DMA_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void TX_DMA_Int_Static( uC_DMA::cback_param_t handle )
		{
			( ( uC_USART_9BIT_DMA* )handle )->TX_DMA_Int();
		}

		uC_USART_9BIT_Buff::cback_status_t m_cback_request;
		uC_USART_9BIT_Buff::cback_func_t m_cback_func;
		uC_USART_9BIT_Buff::cback_param_t m_cback_param;
		uint16_t m_rx_dma_counts_remaining;

		bool m_tx_holdoff;

		uC_Multi_Timers* m_timer;

		rx_state_t m_rx_state;
		tx_state_t m_tx_state;

		uint16_t m_int_enabled_mask;
		bool m_premature_rx_timeout;
		bool m_premature_tx_timeout;
		uint8_t* m_buffer_head;

		/// We are converting from microseconds to hertz so we need to boost some stuff up.
		static const uint32_t M_TIME_TO_FREQ_CALC = 1000000;

		// The multi timer indexes.
		static const uint8_t RX_TIMER = 0;
		static const uint8_t TX_TIMER = 1;
		static const uint8_t TOTAL_TIMERS = 2;
};


#endif
