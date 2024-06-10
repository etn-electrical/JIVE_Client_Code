/**
 **********************************************************************************************
 * @file            Ble_UART.h
 *
 * @brief           Contains the Ble_UART Class. This interfaces with the low level USART driver.
 *
 * @details         It is designed to interface the Ble application with USART driver,
 *                  It initialize particular USART port as specified in Ble application.
 * @copyright       2018 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef BLE_UART_H
#define BLE_UART_H

#include "Ble_Serial.h"
#include "uC_USART_Buff.h"
#include "Ble_Display.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"
#include <stdlib.h>

/**
 **********************************************************************************************
 * @brief        Ble_UART Class Declaration.
 *				 The class will provide interface to communicate ble frames.
 * @details      It provides public methods to
 * @n @b 1.      To Enable initialized USART  port with specified parity,
 *				 baud rate, stop bits, transmission mode.
 * @n @b 2.      To Disable initialized USART port.
 * @n @b 3.      To find out port enable status.
 * @n @b 4.      To find out port idle status so that further data transfer(Tx/Rx) can perform.
 **********************************************************************************************
 */
class Ble_UART : public Ble_Serial
{
	public:
		/**
		 * @brief Ble Transmit/Receive buffer size
		 */
		static const uint16_t DEFAULT_BUFFER_SIZE = 256U;

		/**
		 * @brief Ble default baud rate
		 */
		static const uint32_t DEFAULT_BAUD_RATE = BLE_BAUD_115200;

		/**
		 * @brief                         Constructor to create instance of Ble_UART class.
		 * @param[in] usart_ctrl          Pointer to DMA object.
		 *								  A pointer to class uC_USART_DMA
		 *                                or uC_USART_Soft_DMA can be passed
		 *								  depending upon DMA requirement.
		 * @param[in] Ble_display_ctrl    object of class Ble_Display. Provides the LED
		 *								  indication with respect to initialized hardware port.
		 * @param[in] buffer_size         Ble Receive/Transmit buffer size. if not specified by
		 *								  application, default value is 256 byte.
		 * @param[in] priority            assigns priority for Ble RX_Msg_Handler Task
		 * @n
		 */
		Ble_UART( uC_USART_Buff* usart_ctrl, Ble_Display* Ble_display_ctrl,
				  uint16_t buffer_size = DEFAULT_BUFFER_SIZE,
				  uint8_t priority = CR_TASKER_PRIORITY_2 );

		/**
		 * @brief                   Destructor to delete the Ble_UART instance
		 *							when it goes out of scope
		 * @param[in] void          none
		 * @return
		 */
		~Ble_UART( void );

		/**
		 * @brief                       Function to enable Ble UART port.
		 * @param[in] parity_select     Selects parity type for UART communication.
		 *								Possible values are mentioned below -
		 * @n                           BLE_PARITY_EVEN - Even Parity (default)
		 * @n                           BLE_PARITY_ODD - Odd Parity
		 * @n                           BLE_PARITY_NONE - No Parity
		 * @param[in] baud_rate         Selects baud rate for UART communication.
		 *								Possible values are mentioned below -
		 * @n                           BLE_BAUD_19200  - 19200 Bits/PS (default)
		 * @n                           BLE_BAUD_9600   - 9600 Bits/PS
		 * @n                           BLE_BAUD_38400  - 38400 Bits/PS
		 * @n                           BLE_BAUD_57600  - 57600 Bits/PS
		 * @n                           BLE_BAUD_115200 - 115200 Bits/PS
		 * @n                           BLE_BAUD_230400 - 230400 Bits/PS
		 * @param[in] stop_bits         Selects stop bits for UART communication.
		 *							    Possible values are mentioned below -
		 * @n                           BLE_1_STOP_BIT - 1 Stop Bit (default)
		 * @n                           BLE_2_STOP_BIT - 2 Stop Bit
		 */
		void Enable_Port( ble_parity_t parity_select = BLE_PARITY_NONE,
						  uint32_t baud_rate = DEFAULT_BAUD_RATE,
						  ble_stop_bits_t stop_bit_select = BLE_1_STOP_BIT );

		/**
		 * @brief                   Function to disable Ble_UART channel
		 * @param[in] void          none
		 * @return
		 */
		void Disable_Port( void );

		/**
		 * @brief                   Function to check if Ble_UART channel is enabled
		 * @param[in] void          none
		 * @return bool             true -  channel is enabled
		 * @n                       false - channel is disabled
		 */
		bool Port_Enabled( void );

		/**
		 * @brief                   Function to send ble frame. before sending It verifies
		 *                          if tx mode is BLE_ASCII_TX_MODE or TX_Holdoff time is active
		 * @param[in] tx_struct     ble frame structure
		 * @return                  none
		 */
		void Send_Frame( ble_frame_struct_t* tx_struct );

		/**
		 * @brief                   function handler for Function Bg_Lib_Read
		 * @param[in] data_length    Datalength to be sent to BG Lib as input
		 * @param[in] data		    Data to be sent to BG Lib as input
		 * @return int32t           if the data is present then read out using deQueue
		 *                          and return it.
		 * @n						if the queue is empty then return -1
		 */
		int32_t Bg_Lib_Read( uint32_t data_length, uint8_t* data );

		/**
		 * @brief                   function handler for Function Bg_Lib_Write
		 * @param[in] data_length    datalength to be sent over the UART from the Bg_Lib
		 * @param[in] data		    data to be sent over the UART from the Bg_Lib
		 * @return                  none
		 */
		void Bg_Lib_Write( uint32_t data_length, uint8_t* data );

		/**
		 * @brief                   function handler for checking the
		 *                          rx_uart_buffer_queue is full or not
		 * @param[in] void          none
		 * @return bool             true -  Queue is full
		 * @n                       false - Queue is not full
		 */
		bool Is_Queue_Full( void );

		/**
		 * @brief                   function handler for checking the
		 *							rx_uart_buffer_queue is empty or not
		 * @param[in] void          none
		 * @return bool             true -  Queue is full
		 * @n                       false - Queue is not full
		 */
		bool Is_Queue_Empty( void );

		/**
		 * @brief                   function handler for pushing the date
		 *							into the the rx_uart_buffer_queue
		 * @param[in] element       data element to be pushed into the queue
		 * @return                  none
		 */
		void enQueue( int element );

		/**
		 * @brief                   function handler for pulling the data
		 *							from the rx_uart_buffer_queue
		 * @param[in] element
		 * @return int8_t           if the data is present then pulled out data from the queue
		 *                          and return it.
		 * @n						if the queue is empty then return -1
		 */
		int8_t deQueue( void );

		/**
		 * @brief                   function handler for display the data
		 *							on the rx_uart_buffer_queue
		 * @param[in] void          none
		 * @return                  none
		 */
		void display( void );

		/**
		 * @brief                   function handler for display the data
		 *							on the rx_uart_buffer_queue
		 * @param[in] void          none
		 * @return bool             none
		 */
		void Ble_Led_Display_Control( void );

	private:

		/**
		 * @brief                   Copy Constructor. Definitions made private to disallow usage
		 * @param[in] object        Ble_UART class object
		 * @return
		 */
		Ble_UART( const Ble_UART& object );

		/**
		 * @brief                   Copy Assignment Operator.
		 *							definitions made private to disallow usage
		 * @param[in] object        Ble_UART class object
		 * @return
		 */
		Ble_UART & operator =( const Ble_UART& object );

		/**
		 * @brief           Function to calculate ble msg Rx hold off time
		 * @details         Since we need 3.5 characters we can multiply by 3 to be safe,
		 *                  We need to multiply the bits per character times
		 *					the number of characters hold off.
		 *                  If we have 11 bits per character and 3 character times.
		 * @n               This is merely the bit times for an EOF RX Idle interrupt.
		 * @n               Time calc is boosted by a value which boosts things
		 *					into a microsecond type of framework.
		 * @n               Calculation is microsecond boost * bits_per_char * msg_done_characters
		 * @param[in] void  none
		 * @return uint32_t unsigned32 Rx hold off time in microseconds.
		 */
		inline uint32_t Get_Rx_Done_Time_Calc( void )
		{
			return ( 1000000.0 * 11.0 * 1.5 );
		}

		/**
		 * @brief           Function to calculate ble msg Tx hold off time
		 * @details         Calculation is microsecond boost * bits_per_char * msg_done_character
		 * @param[in] void  none
		 * @return uint32_t unsigned32 Tx hold off time in microseconds.
		 */
		inline uint32_t Get_Tx_Holdoff_Time_Calc()
		{
			return ( 1000000.0 * 11.0 * 3.5 );
		}

		/**
		 * @brief           Function to send ble frame.
		 *					This function doesn't verify if bus is busy.
		 * @param[in] none  void
		 * @return
		 */
		void Send_Frame_Right_Now( void );

		/**
		 * @brief            Function to process received ble frames.
		 * @n                This function is attached to CR_Tasker in Ble_UART constructor,
		 * @n                executed periodically depending upon configured priority.
		 * @param[in] none   void
		 * @return
		 */
		void RX_Msg_Handler( CR_Tasker* cr_task );

		/**
		 * @brief            function handler for Function RX_Msg_Handler
		 * @param[in] none   void
		 * @return
		 */
		static void RX_Msg_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( Ble_UART* ) handle )->RX_Msg_Handler( cr_task );
		}

		/**
		 * @brief            Call back function executed when any of configured
		 *					 Ble uart operation occurs -
		 * @param[in] status Ble Uart operation status bits, possible values are mentioned below
		 * @n                a. RX_BYTE_TIMEOUT
		 * @n                b. TX_COMPLETE
		 * @n                c. TXRX_HOLDOFF_COMPLETE
		 * @return
		 */
		void UART_Callback( uC_USART_Buff::cback_status_t status );

		/**
		 * @brief            Function handler for Call back function UART_Callback
		 * @param[in] param  uC_USART_Buff callback handle
		 * @[in] status      Ble Uart operation status bits
		 * @return
		 */
		static void UART_Callback_Static( uC_USART_Buff::cback_param_t param,
										  uC_USART_Buff::cback_status_t status )
		{
			( ( Ble_UART* ) param )->UART_Callback( status );
		}

		/**
		 * @brief Minimum ble frame Rx hold off time in microseconds
		 */
		static const uint32_t RX_DONE_LONG_HOLDOFF_TIME = 750U;

		/**
		 * @brief Minimum ble frame Tx hold off time in microseconds
		 */
		static const uint32_t TX_LONG_HOLDOFF_TIME = 1750U;

		/**
		 * @brief Minimum baudrate required for use of above minimum hold off time
		 */
		static const uint32_t RTU_LONG_HOLDOFF_BAUD_RATE_MIN = 19200U;

		/**
		 * @brief Various ble frame receive states
		 */
		static const uint8_t RX_IDLE = 0U;
		static const uint8_t RX_BUSY = 1U;
		static const uint8_t RX_WAITING = 2U;
		static const uint8_t RX_MSG_READY = 3U;
		static const uint8_t PROCESSING = 4U;

		/**
		 * @brief Various ble frame Transmit states
		 */
		static const uint8_t TX_IDLE = 0U;
		static const uint8_t TX_DATA_READY = 1U;
		static const uint8_t TX_BUSY = 2U;

		/**
		 * @brief Various ble frame Transmit states
		 */
		static const int8_t QUEUE_IS_EMPTY = -1;
		static const int8_t QUEUE_IS_NOT_EMPTY = 1U;
		static const int8_t NO_SUFFICIENT_DATA_IN_QUEUE = -1;
		static const int8_t QUEUE_FIRST_POSITION = -1;

		/**
		 * @brief ble Port Enabled constant
		 */
		static const uint8_t PORT_ENABLED = 0x01U;

		/**
		 * @brief variable to store various ble frame attributes
		 */
		uint8_t m_port_flags_u8;

		/**
		 * @brief variable to store length of received data frame
		 */
		uint16_t m_rx_data_length_u16;

		/**
		 * @brief pointer to dynamically allocated data Rx buffer
		 */
		uint8_t* m_rx_buff;

		/**
		 * @brief pointer to dynamically allocated data Tx buffer
		 */
		uint8_t* m_tx_buff;

		/**
		 * @brief variable to store receive data buffer size
		 */
		uint16_t m_rx_buffer_size_u16;

		/**
		 * @brief structure to store ble frame to be transmitted
		 */
		ble_frame_struct_t m_tx_struct;

		/**
		 * @brief variable to store current state of processing on transmitted frame
		 */
		uint8_t m_tx_state_u8;

		/**
		 * @brief pointer to store UART port usart_ctrl structure passed from application
		 */
		uC_USART_Buff* m_usart;

		/**
		 * @brief pointer to store LED indication port for initialized hardware port
		 */
		Ble_Display* m_ble_display;

		/**
		 * @brief pointer to store dynamically created CR_Queue object
		 */
		CR_Queue* m_rx_que;

		/**
		 * @brief receive buffer used to forward the message to Bg_Lib_Read.
		 */
		uint8_t m_ble_recv_buf[DEFAULT_BUFFER_SIZE] = { 0U };

		/**
		 * @brief Circular Queue Implementation.
		 */
		int32_t m_front, m_rear;

};

#endif
