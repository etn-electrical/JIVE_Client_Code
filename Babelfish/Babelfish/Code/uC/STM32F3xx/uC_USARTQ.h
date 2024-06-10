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
#ifndef uC_USARTQ_H
   #define uC_USARTQ_H

#include "uC_USART.h"
#include "uC_OS_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "uC_DMA.h"

/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */


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
class uC_USARTQ
{
	public:
		/*
		 *******************************************************
		 * The following is a simplified USART implementation.
		 * It implements a rudimentary queue functionality.
		 *******************************************************
		 */
		uC_USARTQ( uC_USART* usart_handle, uint16_t tx_buff_size, uint16_t rx_buff_size );

		bool Enque( uint8_t data );			// Send

		bool Enque_Full( void );

		bool Enque_Empty( void );

		void Enqueue_String( uint8_t* data, uint16_t length );

		bool Deque( uint8_t* data );		// Receive

		bool Deque_String( uint8_t* data, uint16_t& length );			// Receive

		bool Deque_Full( void );

		bool Deque_Empty( void );

	private:
		uC_USART* m_usart;

		uint8_t* m_tx_buff;
		uint16_t m_tx_buff_size;

		uint8_t* m_rx_buff;
		uint16_t m_rx_buff_size;

};


#endif
