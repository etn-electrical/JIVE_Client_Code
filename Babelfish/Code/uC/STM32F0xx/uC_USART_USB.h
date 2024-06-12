/**
 *****************************************************************************************
 *	@file
 *
 *	@brief	uC_USART_USB defines a USART emulator using USB CDC virtual com port
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_USART_USB_H
   #define uC_USART_USB_H

#include "Includes.h"
#include "uC_USART_Buff.h"
#include "uC_Multi_Timers.h"

extern "C"
{
#include "usbd_def.h"
#include "usbd_cdc.h"
}

/**
 ****************************************************************************************
 * @brief Class represents a USB CDC Virtual Com Port link.
 *
 * @details Derives from abstract base class uC_USART_Buff:: such that uC_USART_USB may be
 * employed any place where uC_USART_Buff may be used (for example, Modbus_UART::).
 *
 ****************************************************************************************
 */
class uC_USART_USB : public uC_USART_Buff
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		uC_USART_USB( uint8_t hw_timer_num, uint8_t id );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~uC_USART_USB( void );

		/**
		 * @brief Configures the callback.  You pass in the function and parameter you want to be
		 * called
		 * as well as the reason(s) you want to be called.
		 * @param cback_func - The actual function.
		 * @param cback_param - Typically the "this" pointer.  Can be null.
		 * @param cback_req_mask - The bitfield indicating what reasons you want to be called.
		 * @param rx_byte_time (microseconds) - 1.5 character times at the given baud rate.
		 * @param tx_holdoff_time (microseconds) - 3.5 character times at the given baud rate.
		 */
		void Config_Callback( uC_USART_Buff::cback_func_t cback_func,
							  uC_USART_Buff::cback_param_t cback_param,
							  uC_USART_Buff::cback_status_t cback_req_mask,
							  uint32_t rx_timeout = 0U,
							  uint32_t tx_holdoff_time = 0U );

		/**
		 * @brief Starts the receive method.  It will wait until one of the conditions is met before
		 * making
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
		 * @return Whether we are actively waiting for a message or whether we are idle.  Idle =
		 * false.
		 */
		uC_USART_Buff::rx_state_t RX_State( void );

		/**
		 * @brief Aborts the receive process and puts the receiver into idle.
		 */
		void Stop_RX( void );

		/**
		 * @brief Starts the transmit method.  It will send out the data buffer immediately.
		 * @param data - Pointer to your target data string.  If a null data pointer or zero
		 * datalength
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
		 * @param baud_rate - Baud rate in bps. Not used, don't care.
		 * @param char_size - Controls the number of bits per character. Not used, don't care.
		 * @param parity - Parity of the uart. Not used, don't care.
		 * @param stop_bit_count - Stop bit count. Not used, don't care.
		 * @return success - True if the configuration succeeded.  False if it failed to config.
		 */
		bool Config_Port( uint32_t baud_rate,
						  uC_USART_Buff::char_size_t char_size = uC_USART_Buff::CHAR_SIZE_8BIT,
						  uC_USART_Buff::parity_t parity = uC_USART_Buff::PARITY_NONE,
						  uC_USART_Buff::stop_bit_t stop_bit_count = uC_USART_Buff::STOP_BIT_1 );


		/**
		 * @brief Internal callback upon USB CDC connection establishment.
		 */
		int8_t Usb_Cdc_Init( void );

		/**
		 * @brief Internal callback upon USB CDC disconnection.
		 */
		int8_t Usb_Cdc_DeInit( void );

		/**
		 * @brief Internal callback upon USB CDC control message.
		 */
		int8_t Usb_Cdc_Control( uint8_t cmd, uint8_t* pbuf, uint16_t length );

		/**
		 * @brief Internal callback upon USB CDC receipt of data IN message.
		 */
		int8_t Usb_Cdc_Receive( uint8_t* Buf, uint32_t* Len );

		/**
		 * @brief Internal callback upon USB CDC completion of data OUT message.
		 */
		int8_t Usb_Cdc_Transmit( uint8_t* Buf, uint32_t* Len );

	private:
		/**
		 * @brief Configures the receive timer given the receive byte time, and the
		 * transmit timer given the transmit holdoff time.
		 * @param rx_byte_time (microseconds) - 1.5 character times at the desired baud rate.
		 * @param tx_holdoff_time (microseconds) - 3.5 character times at the desired baud rate.
		 */
		void Config_Timers( uint32_t rx_byte_time, uint32_t tx_holdoff_time );

		/**
		 * @brief Flag indicating whether USB CDC is presently connected to remote Host.
		 */
		bool m_is_connected;

		//// >>>>>>>>>>>>
		void RX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void RX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{ ( ( uC_USART_USB* )handle )->RX_Timer_Int(); }

		void TX_Timer_Int( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void TX_Timer_Int_Static( uC_Multi_Timers::cback_param_t handle )
		{ ( ( uC_USART_USB* )handle )->TX_Timer_Int(); }

		//// <<<<<<<<<<<<

		/**
		 * @brief Pointer to receive buffer.
		 */
		uint8_t* m_rx_buff;
		/**
		 * @brief Allocated length of receive buffer.
		 */
		uint16_t m_rx_buff_len;
		/**
		 * @brief Index to next available slot in receive buffer. Also, length of message in buffer.
		 */
		uint16_t m_rx_buff_index;

		/**
		 * @brief Pointer to transmit buffer.
		 */
		uint8_t* m_tx_buff;
		/**
		 * @brief Length of message in transmit buffer.
		 */
		uint16_t m_tx_buff_len;
		/**
		 * @brief Index to currently transmiting segment within transmit buffer.
		 */
		uint16_t m_tx_buff_index;

		/**
		 * @brief Bit field flags for requested reasons for callback.
		 */
		uC_USART_Buff::cback_status_t m_cback_request;

		/**
		 * @brief Callback function.
		 */
		uC_USART_Buff::cback_func_t m_cback_func;

		/**
		 * @brief Callback parameter.
		 */
		uC_USART_Buff::cback_param_t m_cback_param;

		//// >>>>>>>>>>>>
		bool m_tx_holdoff;

		uC_Multi_Timers* m_timer;
		//// <<<<<<<<<<<<

		/**
		 * @brief Receive state variable.
		 */
		rx_state_t m_rx_state;
		/**
		 * @brief Transmit state variable.
		 */
		tx_state_t m_tx_state;

		//// >>>>>>>>>>>>
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

		//// <<<<<<<<<<<<

		/**
		 * @brief Bit rate (not used, don't care).
		 */
		uint32_t m_bitrate;
		/**
		 * @brief Character format (not used, don't care).
		 */
		uint8_t m_format;
		/**
		 * @brief Parity (not used, don't care).
		 */
		uint8_t m_paritytype;
		/**
		 * @brief Data size in bits (not used, don't care).
		 */
		uint8_t m_datatype;

		/**
		 * @brief USB CDC device structure.
		 */
		USBD_HandleTypeDef m_USBD_CDC_Device;
};

/**
 * @brief Table of callback functions for USB CDC class.
 */
extern "C" USBD_CDC_ItfTypeDef USBD_CDC_fops;


#endif
