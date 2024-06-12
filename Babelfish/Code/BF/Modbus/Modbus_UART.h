/**
 **********************************************************************************************
 * @file            Modbus_UART.h
 *
 * @brief           Contains the Modbus_UART Class. This interfaces with the low level USART driver.
 *
 * @details         It is designed to interface the Modbus application with USART driver,
 *                  It initialize particular USART port as specified in Modbus application.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef MODBUS_UART_H
#define MODBUS_UART_H

#include "Modbus_Serial.h"
#include "uC_USART_Buff.h"
#include "Modbus_Display.h"
#include "Modbus_Defines.h"
#include "Output.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"

/**
 **********************************************************************************************
 * @brief        Modbus_UART Class Declaration. The class will provide interface to communicate modbus frames.
 * @details      It provides public methods to
 * @n @b 1.      To Enable initialized USART  port with specified parity, baud rate, stop bits, transmission mode.
 * @n @b 2.      To Disable initialized USART port.
 * @n @b 3.      To find out port enable status.
 * @n @b 4.      To find out port idle status so that further data transfer(Tx/Rx) can perform.
 **********************************************************************************************
 */
class Modbus_UART : public Modbus_Serial
{
	public:
		/**
		 * @brief Modbus receive buffer size
		 */
		static const uint16_t DEFAULT_BUFFER_SIZE = MODBUS_UART_BUFFER_SIZE;///< actual ASCII serial size used by
																			///< serial.

		/**
		 * @brief Modbus default baud rate
		 */
		static const uint32_t DEFAULT_BAUD_RATE = 19200;

		/**
		 * @brief modbus ASCII support constant
		 */
		static const bool SUPPORT_ASCII = true;

		/**
		 * @brief modbus disable ASCII support constant
		 */
		static const bool DISABLE_ASCII = false;
		/**
		 * @brief                             Constructor to create instance of Modbus_UART class.
		 * @param[in] usart_ctrl              Pointer to DMA object. a pointer to class uC_USART_DMA
		 *                                    or uC_USART_Soft_DMA can be passed depending upon DMA requirement.
		 * @param[in] modbus_display_ctrl     object of class Modbus_Display. Provides the LED indication with respect
		 * to initialized hardware port.
		 * @param[in] tx_enable_out           It is used To set the port pin to enable RS485 buffer. In case of no
		 * control Output_Null() can be passed as parameter.
		 * @param[in] rx_buffer_size          Modbus receive buffer size. if not specified by application, default value
		 * is 256 byte.
		 * @param[in] priority                assigns priority for Modbus RX_Msg_Handler Task
		 * @param[in] ascii_support: An ASCII Port will allocate a large enough buffer to
		 * account for the ASCII overhead. If ascii support is disabled then it is not possible to
		 * RX a full ASCII frame. During construction you can determine whether it is a requirement
		 * which must be supported.
		 * @n
		 */
		Modbus_UART( uC_USART_Buff* usart_ctrl, BF_Mbus::Modbus_Display* modbus_display_ctrl,
					 BF_Lib::Output* tx_enable_out = nullptr,
					 uint16_t rx_buffer_size = DEFAULT_BUFFER_SIZE,
					 uint8_t priority = CR_TASKER_PRIORITY_2,
					 bool ascii_support = SUPPORT_ASCII );

		/**
		 * @brief                   Destructor to delete the Modbus_UART instance when it goes out of scope
		 * @param[in] void          none
		 * @return
		 */
		~Modbus_UART( void );

		/**
		 * @brief                             Function to enable Modbus UART port.
		 * @param[in] rx_cback                Modbus Process_Req callback function.
		 * @n                                 If not assigned explicitly, Modbus_Net assigns Process_Req callback in its
		 * constructor.
		 *                                    it will be called once modbus frame is received  on USART port
		 * @param[in] rx_cback_handle         handle for Modbus Process_Req callback function
		 * @param[in] modbus_tx_mode          Selects the tx mode for UART communication. Possible values are mentioned
		 * below -
		 * @n                                 MODBUS_RTU_TX_MODE - Transmitted bytes are presented as binary
		 * @n                                 MODBUS_ASCII_TX_MODE - Transmitted bytes are presented as ASCII
		 * @param[in] parity_select           Selects parity type for UART communication. Possible values are mentioned
		 * below -
		 * @n                                 MODBUS_PARITY_EVEN - Even Parity (default)
		 * @n                                 MODBUS_PARITY_ODD - Odd Parity
		 * @n                                 MODBUS_PARITY_NONE - No Parity
		 * @param[in] baud_rate               Selects baud rate for UART communication. Possible values are mentioned
		 * below -
		 * @n                                 MODBUS_BAUD_19200  - 19200 Bits/PS (default)
		 * @n                                 MODBUS_BAUD_9600   - 9600 Bits/PS
		 * @n                                 MODBUS_BAUD_38400  - 38400 Bits/PS
		 * @n                                 MODBUS_BAUD_57600  - 57600 Bits/PS
		 * @n                                 MODBUS_BAUD_115200 - 115200 Bits/PS
		 * @n                                 MODBUS_BAUD_230400 - 230400 Bits/PS
		 * @param[in] use_long_holdoff        This parameter defines min msg hold off time.
		 * @n                                 true - 1.75ms is used as the tx msg hold off time.
		 * @n                                        0.75ms is used as the rx msg hold off time.
		 * @n                                 false - hold off time shall be calculated from below inline functions -
		 * @n                                         Get_Tx_Holdoff_Time_Calc()
		 * @n                                         Get_Rx_Done_Time_Calc()
		 * @param[in] stop_bits               Selects stop bits for UART communication. Possible values are mentioned
		 * below -
		 * @n                                 MODBUS_1_STOP_BIT - 1 Stop Bit (default)
		 * @n                                 MODBUS_2_STOP_BIT - 2 Stop Bit
		 */
		void Enable_Port( MB_RX_PROC_CBACK rx_cback,
						  MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
						  tx_mode_t modbus_tx_mode, parity_t parity_select = MODBUS_PARITY_EVEN,
						  uint32_t baud_rate = DEFAULT_BAUD_RATE, bool use_long_holdoff = true,
						  stop_bits_t stop_bit_select = MODBUS_1_STOP_BIT );

		/**
		 * @brief                   Function to disable Modbus_UART channel
		 * @param[in] void          none
		 * @return
		 */
		void Disable_Port( void );

		/**
		 * @brief                   Function to check if Modbus_UART channel is enabled
		 * @param[in] void          none
		 * @return bool             true -  channel is enabled
		 * @n                       false - channel is disabled
		 */
		bool Port_Enabled( void );

		/**
		 * @brief                   Function to check if Modbus_UART channel is Idle for communication
		 * @param[in] void          none
		 * @return bool             true -  channel is idle
		 * @n                       false - channel is busy
		 */
		bool_t Idle( void );

		/**
		 * @brief                   Function to send modbus frame. before sending It verifies
		 *                          if tx mode is MODBUS_ASCII_TX_MODE or TX_Holdoff time is active
		 * @param[in] tx_struct     modbus frame structure
		 * @return
		 */
		void Send_Frame( MB_FRAME_STRUCT* tx_struct );

	private:

		/**
		 * @brief                   Copy Constructor. definitions made private to disallow usage
		 * @param[in] object        Modbus_UART class object
		 * @return
		 */
		Modbus_UART( const Modbus_UART& object );

		/**
		 * @brief                   Copy Assignment Operator. definitions made private to disallow usage
		 * @param[in] object        Modbus_UART class object
		 * @return
		 */
		Modbus_UART & operator =( const Modbus_UART& object );

		/**
		 * @brief           Function to calculate modbus msg Rx hold off time
		 * @details         Since we need 3.5 characters we can multiply by 3 to be safe,
		 *                  We need to multiply the bits per character times the number of characters hold off.
		 *                  If we have 11 bits per character and 3 character times.
		 * @n               This is merely the bit times for an EOF RX Idle interrupt.
		 * @n               Time calc is boosted by a value which boosts things into a microsecond type of framework.
		 * @n               Calculation is microsecond boost * bits_per_char * msg_done_characters
		 * @param[in] void  none
		 * @return uint32_t unsigned32 Rx hold off time in microseconds.
		 */
		inline uint32_t Get_Rx_Done_Time_Calc()
		{
			return ( 1000000.0 * 11.0 * 1.5 );
		}

		/**
		 * @brief           Function to calculate modbus msg Tx hold off time
		 * @details         Calculation is microsecond boost * bits_per_char * msg_done_character
		 * @param[in] void  none
		 * @return uint32_t unsigned32 Tx hold off time in microseconds.
		 */
		inline uint32_t Get_Tx_Holdoff_Time_Calc()
		{
			return ( 1000000.0 * 11.0 * 3.5 );
		}

		/**
		 * @brief                   Function to send modbus frame. This function doesn't verify if bus is busy
		 * @param[in] none          void
		 * @return
		 */
		void Send_Frame_Right_Now( void );

		/**
		 * @brief                   Function to process received modbus frames.
		 * @n                       This function is attached to CR_Tasker in Modbus_UART constructor,
		 * @n                       executed periodically depending upon configured priority.
		 * @param[in] none          void
		 * @return
		 */
		void RX_Msg_Handler( CR_Tasker* cr_task );

		/**
		 * @brief                   function handler for Function RX_Msg_Handler
		 * @param[in] none          void
		 * @return
		 */
		static void RX_Msg_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( Modbus_UART* )handle )->RX_Msg_Handler( cr_task );
		}

		/**
		 * @brief                   Call back function executed when any of configured Modbus uart operation occurs -
		 * @param[in] status        Modbus Uart operation status bits, possible values are mentioned below -
		 * @n                       a. RX_BYTE_TIMEOUT
		 * @n                       b. TX_COMPLETE
		 * @n                       c. TXRX_HOLDOFF_COMPLETE
		 * @return
		 */
		void UART_Callback( uC_USART_Buff::cback_status_t status );

		/**
		 * @brief                   Function handler for Call back function UART_Callback
		 * @param[in] param         uC_USART_Buff callback handle
		 * @[in] status             Modbus Uart operation status bits
		 * @return
		 */
		static void UART_Callback_Static( uC_USART_Buff::cback_param_t param,
										  uC_USART_Buff::cback_status_t status )
		{
			( ( Modbus_UART* )param )->UART_Callback( status );
		}

		/**
		 * @brief Minimum modbus frame Rx hold off time in microseconds
		 */
		static const uint32_t RX_DONE_LONG_HOLDOFF_TIME = 750U;

		/**
		 * @brief Modbus ASCII inter character delay in microseconds
		 * @details Per Modbus ASCII requirement, inter character delay is set as 1 Sec.
		 */
		static const uint32_t MODBUS_ASCII_INTER_CHARACTER_DELAY = 1000000U;

		/**
		 * @brief Minimum modbus frame Tx hold off time in microseconds
		 */
		static const uint32_t TX_LONG_HOLDOFF_TIME = 1750U;

		/**
		 * @brief Minimum baudrate required for use of above minimum hold off time
		 */
		static const uint32_t RTU_LONG_HOLDOFF_BAUD_RATE_MIN = 19200U;

		/**
		 * @brief Various modbus frame receive states
		 */
		static const uint8_t RX_IDLE = 0U;
		static const uint8_t RX_BUSY = 1U;
		static const uint8_t RX_WAITING = 2U;
		static const uint8_t RX_MSG_READY = 3U;
		static const uint8_t PROCESSING = 4U;

		/**
		 * @brief Various modbus frame Transmit states
		 */
		static const uint8_t TX_IDLE = 0U;
		static const uint8_t TX_DATA_READY = 1U;
		static const uint8_t TX_BUSY = 2U;

		/**
		 * @brief modbus TX mode ASCII constant
		 */
		static const uint8_t TX_MODE_ASCII = 0x00U;

		/**
		 * @brief modbus Port Enabled constant
		 */
		static const uint8_t PORT_ENABLED = 0x01U;

		/**
		 * @brief modbus ASCII support constant
		 */
		static const uint8_t ASCII_SUPPORT = 0x02U;

		/**
		 * @brief variable to store various modbus frame attributes
		 */
		uint8_t m_port_flags;

		/**
		 * @brief pointer to receive data buffer m_rx_buff
		 */
		uint8_t* m_rx_data_ptr;

		/**
		 * @brief variable to store length of received data frame
		 */
		uint16_t m_rx_data_length;

#ifndef MODBUS_ASCII_SUPPORT
		static uint16_t const RX_BUFFER_SIZE_INTERNAL = DEFAULT_BUFFER_SIZE + 1U;
#else
		static uint16_t const RX_BUFFER_SIZE_INTERNAL = ( DEFAULT_BUFFER_SIZE * 2U ) + 1U;
#endif
		/**
		 * @brief Rx buffer array for enabled ASCII support
		 */
		uint8_t m_rx_buff[RX_BUFFER_SIZE_INTERNAL];

		/**
		 * @brief variable to store current state of processing on received frame
		 */
		uint8_t m_rx_state;

		/**
		 * @brief structure to store modbus frame to be transmitted
		 */
		MB_FRAME_STRUCT m_tx_struct;

		/**
		 * @brief structure to store received modbus frame
		 */
		MB_FRAME_STRUCT m_rx_struct;

		/**
		 * @brief variable to store current state of processing on transmitted frame
		 */
		uint8_t m_tx_state;

		/**
		 * @brief pointer storing port pin IO structure t enable RS485 buffer
		 */
		BF_Lib::Output* m_tx_enable_out;

		/**
		 * @brief pointer to store UART port usart_ctrl structure passed from application
		 */
		uC_USART_Buff* m_usart;

		/**
		 * @brief pointer to store LED indication port for initialized hardware port
		 */
		BF_Mbus::Modbus_Display* m_modbus_display;

		/**
		 * @brief function pointer to store receive callback
		 */
		MB_RX_PROC_CBACK m_rx_cback;

		/**
		 * @brief handle to receive callback function pointer
		 */
		MB_RX_PROC_CBACK_HANDLE m_rx_cback_handle;

		/**
		 * @brief pointer to store dynamically created CR_Queue object
		 */
		CR_Queue* m_rx_que;

};

#endif
