/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Purely virtual SPI buffer interface.
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
#ifndef uC_SPI_BUFF_H
   #define uC_SPI_BUFF_H

#include "Includes.h"
#include "Babelfish_Assert.h"

/**
 ****************************************************************************************
 * @brief Provides a purely virtual hardware abstracted serial interface with
 * a basic interface and functionality.
 *
 * @details Creates a virtual interface which can be utilized by serial interfaces.
 * Purpose of this is to allow us to flexibly apply different hardware peripherals to
 * provide common functions of a SPI interface.
 *
 ****************************************************************************************
 */
class uC_SPI_Buff
{
	public:
		/**
		 * @brief Constructs a purely virtual SPI interface should not be run.
		 */
		uC_SPI_Buff( void ) { BF_ASSERT( false ); }

		virtual ~uC_SPI_Buff( void );

		/**
		 *  @brief Callback types.  These are used by the object owner to define what
		 *  types of callback you want.  The status is the bitfield indicating what
		 *  status triggered the callback.  Be aware that the object owners callback
		 *  will likely be called from within an interrupt to keep things simpler and quicker.
		 */
		typedef uint8_t cback_status_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param, cback_status_t status_bits );

		/**
		 *  @brief Callback Status bits.  The following will be used as bits and this is
		 *  why they are not a type.
		 */
		enum
		{
			RX_BYTE_TIMEOUT,		// < This is triggered by a bus quiet time.
			RX_BUFF_FULL,			// < The passed in array has hit the end of the buffer.
			TX_BUFF_EMPTY,			// < The data buffer is empty and ready to transmit again.
			TX_COMPLETE,			// < The shift register is empty and the wire should be clear.
			TXRX_HOLDOFF_COMPLETE,	// < Half duplex holdoff.  RX resets holdoff.  This will override
									// TX_HOLDOFF_COMPLETE.
			TX_HOLDOFF_COMPLETE		// < We only monitor the TX for holdoff.
		};

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
		virtual void Config_Callback( cback_func_t cback_func, cback_param_t cback_param,
									  cback_status_t cback_req_mask,
									  uint32_t rx_timeout = 0, uint32_t tx_holdoff_time = 0 ) = NULL;

		/**
		 * @brief Starts the receive method.  It will wait until one of the conditions is met before making
		 * the callback.
		 * @param data - Pointer to your target data string.
		 * @param length - Length of data you want to receive or max length that you can receive.
		 */
		virtual void Start_RX( uint8_t* data, uint16_t length ) = NULL;

		/**
		 * @brief After an RX occurs the length could be less than the total length sent in.
		 * @return The length of data received.
		 */
		virtual uint16_t Get_RX_Length( void );

		enum rx_state_t
		{
			RX_IDLE,
			RX_WAITING,
			RX_RECEIVING
		};

		/**
		 * @brief Returns whether we are actively receiving or are triggered to receive.
		 * @return Whether we are actively waiting for a message or whether we are idle.  Idle = false.
		 */
		virtual rx_state_t RX_State( void );

		/**
		 * @brief Aborts the receive process and puts the receiver into idle.
		 */
		virtual void Stop_RX( void );

		/**
		 * @brief Starts the transmit method.  It will send out the data buffer immediately.
		 * @param data - Pointer to your target data string.  If a null data pointer or zero datalength
		 * is passed in, the tx holdoff will be kicked but no data will be sent.
		 * @param length - Length of data you want to transmit.
		 */
		virtual void Start_TX( uint8_t* data, uint16_t length );

		enum tx_state_t
		{
			TX_IDLE,
			TX_RUNNING
		};

		/**
		 * @brief Returns whether we are actively transmitting.
		 * @return Whether we are actively transmitting.  Idle = false.
		 */
		virtual tx_state_t TX_State( void );

		/**
		 * @brief Returns whether the holdoff time is active or not.
		 * @return Indicates that the holdoff is active.  Active = true.
		 */
		virtual bool TX_Holdoff( void );

		/**
		 * @brief Aborts the transmit process and puts the transmitter into idle.
		 */
		virtual void Stop_TX( void );

};


#endif
