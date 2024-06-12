/**
 *****************************************************************************************
 *	@file		uC_USART_Buff.h
 *
 *	@brief		Purely virtual USART buffer interface.
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_USART_BUFF_H
   #define uC_USART_BUFF_H

#include "uC_UHCI.h"

/**
 ****************************************************************************************
 * @brief			Provides a purely virtual hardware abstracted serial interface with
 * @n				a basic interface and functionality.
 * @details			Creates a virtual interface which can be utilized by serial interfaces.
 * @n				Purpose of this is to allow us to flexibly apply different hardware
 * @n				peripherals to provide common functions of a USART interface.
 ****************************************************************************************
 */
class uC_USART_Buff
{
	public:
		/**
		 * @brief							Constructs a purely USART interface.
		 */
		uC_USART_Buff( void ) {}

		/**
		 * @brief							Distructor a purely USART interface.
		 */
		virtual ~uC_USART_Buff( void ) {}

		/**
		 * @brief							Callback types.  These are used by the object owner to define what
		 * @n								types of callback you want.  The status is the bitfield indicating what
		 * @n								status triggered the callback.  Be aware that the object owners callback
		 * @n								will likely be called from within an interrupt to keep things
		 * @n								simpler and quicker.
		 */
		typedef uint8_t cback_status_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param, cback_status_t status_bits );

		/**
		 * @brief							Callback Status bits.  The following will be used as bits and this is
		 * @n								why they are not a type.
		 */
		static const cback_status_t RX_BYTE_TIMEOUT = 0U;			///< This is triggered by a bus quiet time.
		static const cback_status_t RX_BUFF_FULL = 1U;				///< The passed in array has hit the end of the
		///< buffer.
		static const cback_status_t TX_BUFF_EMPTY = 2U;				///< The data buffer is empty and ready to transmit
		///< again.
		static const cback_status_t TX_COMPLETE = 3U;				///< The shift register is empty and the wire
		///< should be clear.
		static const cback_status_t TXRX_HOLDOFF_COMPLETE = 4U;		///< Half duplex holdoff.  RX resets holdoff.
		///< This will override TX_HOLDOFF_COMPLETE.
		static const cback_status_t TX_HOLDOFF_COMPLETE = 5U;		///< We only monitor the TX for holdoff.

		/**
		 * @brief							Configures the callback.
		 * @n								You pass in the function and parameter you want to be called
		 * @n								as well as the reason(s) you want to be called.
		 * @param[in] cback_func -			The actual function.
		 * @param[in] cback_param -			Typically the "this" pointer.  Can be null.
		 * @param[in] cback_req_mask -		The bitfield indicating what reasons you want to be called.
		 * @param[in] rx_byte_time -		Wire quiet time before we consider it End of Message.
		 * @n								This can also be 0 which indicates no timeout.
		 * @param[in] tx_holdoff_time -		Wire quiet time before we consider it End of Message.
		 * @n								This can also be 0.which indicates no timeout.
		 */
		virtual void Config_Callback( cback_func_t cback_func, cback_param_t cback_param,
									  cback_status_t cback_req_mask,
									  uint32_t rx_timeout = 0U, uint32_t tx_holdoff_time = 0U ) = 0;

		/**
		 * @brief							Starts the receive method.
		 * @n								It will wait until one of the conditions is met before making the callback.
		 * @param[in] data -				Pointer to your target data string.
		 * @param[in] length -				Length of data you want to receive or max length that you can receive.
		 */
		virtual void Start_RX( uint8_t* data, uint16_t length ) = 0;

		/**
		 * @brief							After an RX occurs the length could be less than the total length sent in.
		 * @return							The length of data received.
		 */
		virtual uint16_t Get_RX_Length( void ) = 0;

		/**
		 ****************************************************************************************
		 * @brief Recevie State of UART
		 *
		 ****************************************************************************************
		 */
		enum rx_state_t
		{
			RX_IDLE,								///< Receive Idle state
			RX_WAITING,								///< Receive Waiting state
			RX_RECEIVING							///< Receiving state
		};

		/**
		 ****************************************************************************************
		 * @brief Transmit state UART
		 *
		 ****************************************************************************************
		 */
		enum tx_state_t
		{
			TX_IDLE,								///< Transmit Idle state
			TX_RUNNING								///< Transmit Running state
		};

		/**
		 ****************************************************************************************
		 * @brief Character size 5 to 9 bits
		 *
		 ****************************************************************************************
		 */
		enum char_size_t
		{
			CHAR_SIZE_5BIT = 5,						///< Character size 5 bits
			CHAR_SIZE_6BIT = 6,						///< Character size 6 bits
			CHAR_SIZE_7BIT = 7,						///< Character size 7 bits
			CHAR_SIZE_8BIT = 8,						///< Character size 8 bits
			CHAR_SIZE_9BIT = 9						///< Character size 9 bits
		};

		/**
		 ****************************************************************************************
		 * @brief Uart parity
		 *
		 ****************************************************************************************
		 */
		enum parity_t
		{
			PARITY_EVEN,							///< Even parity
			PARITY_ODD,								///< Odd parity
			PARITY_SPACE,							///< Space parity
			PARITY_MARK,							///< Mark Parity
			PARITY_NONE,							///< None Parity
			PARITY_MAX_OPTIONS
		};

		/**
		 ****************************************************************************************
		 * @brief Uart stop bit indications
		 *
		 ****************************************************************************************
		 */
		enum stop_bit_t
		{
			STOP_BIT_1,								///< Stop bit 1
			STOP_BIT_1p5,							///< Stop bit 1p5
			STOP_BIT_2								///< Stop bit 2
		};

		/**
		 * @brief							Returns whether we are actively receiving or are triggered to receive.
		 * @return							Whether we are actively waiting for a message or whether we are idle.
		 * @n								Idle = false.
		 */
		virtual rx_state_t RX_State( void ) = 0;

		/**
		 * @brief							Aborts the receive process and puts the receiver into idle.
		 */
		virtual void Stop_RX( void ) = 0;

		/**
		 * @brief							Starts the transmit method.  It will send out the data buffer immediately.
		 * @param[in] data -				Pointer to your target data string.  If a null data pointer or zero
		 * @n								datalength is passed in, the tx hold off will be kicked
		 * @n								but no data will be sent.
		 * @param[in] length -				Length of data you want to transmit.
		 */
		virtual void Start_TX( uint8_t* data, uint16_t length ) = 0;

		/**
		 * @brief							Returns whether we are actively transmitting.
		 * @return							Whether we are actively transmitting.  Idle = false.
		 */
		virtual tx_state_t TX_State( void ) = 0;

		/**
		 * @brief							Returns whether the holdoff time is active or not.
		 * @return							Indicates that the holdoff is active.  Active = true.
		 */
		virtual bool TX_Holdoff( void ) = 0;

		/**
		 * @brief							Aborts the transmit process and puts the transmitter into idle.
		 */
		virtual void Stop_TX( void ) = 0;

		/**
		 * @brief							Configures the port.  Can be executed at any time.
		 * @param[in]						baud_rate - Baud rate in bps.
		 * @param[in] char_size -			Controls the number of bits per character.
		 * @param[in] parity -				Parity of the uart.
		 * @param[in] stop_bit_count -		Stop bit count.
		 * @param[in] stop_bit_count -		Stop bit count.
		 * @return success -				True if the configuration succeeded.  False if it failed to config.
		 */
		virtual bool Config_Port( uint32_t baud_rate,
								  char_size_t char_size = CHAR_SIZE_8BIT,
								  parity_t parity = PARITY_NONE,
								  stop_bit_t stop_bit_count = STOP_BIT_1 ) = 0;

};

#endif
