/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This interfaces with the low level USART driver.
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
#ifndef BACNET_UART_H
	#define BACNET_UART_H

#include "uC_USART_Buff.h"
#include "BACNET_Defines.h"
#include "Output.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"
#include "Timers_Lib.h"

#define rxmax    1024U

/**
 ****************************************************************************************
 * @brief This interfaces with the low level USART driver.
 *
 * @details
 *
 * @note
 *
 ****************************************************************************************
 */
class BACnet_UART
{
	public:
		static const uint16_t BAC_DEFAULT_BUFFER_SIZE = rxmax;
		static const uint32_t BAC_DEFAULT_BAUD_RATE = 19200U;
		BACnet_UART();
		BACnet_UART( uC_USART_Buff* usart_ctrl,
					 BF_Lib::Output* tx_enable_out = NULL,
					 uint16_t bac_rx_buffer_size = BAC_DEFAULT_BUFFER_SIZE,
					 CR_TASKER_PRIOR_TD priority = CR_TASKER_PRIORITY_2 );
		~BACnet_UART( void );

		// This should only be called once per instance to avoid memory leaks.
		// To do otherwise would require modifications.
		void Enable_Port( BACNET_TX_MODES_EN modbus_tx_mode, uint8_t parity_select =
						  BACNET_PARITY_EVEN, uint32_t baud_rate = 19200, uint8_t stop_bits = BACNET_1_STOP_BIT );

		void Disable_Port( void );

		bool_t Port_Enabled( void );

		bool_t Idle( void );

		void BACnet_Send_Frame( uint8_t* data, uint16_t len );

	private:
		// RX States
		enum bacnet_rx_state_t
		{
			BACnet_RX_IDLE,
			BACnet_RX_BUSY,
			BACnet_RX_WAITING,
			BACnet_RX_MSG_READY,
			BACnet_PROCESSING
		};

		// TX States
		enum bacnet_tx_state_t
		{
			BACnet_TX_IDLE,
			BACnet_TX_DATA_READY,
			BACnet_TX_BUSY
		};

		void Send_BACNET_Frame_Right_Now( void );

		void BACnet_RX_Msg_Handler( CR_Tasker* cr_task );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void BACnet_RX_Msg_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( BACnet_UART* )handle )->BACnet_RX_Msg_Handler( cr_task );
		}

		void BACnet_Silencer( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void BACnet_Silencer_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( BACnet_UART* )handle )->BACnet_Silencer();
		}

		void BACnet_UART_Callback( uC_USART_Buff::cback_status_t status );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void BACNET_UART_Callback_Static( uC_USART_Buff::cback_param_t param,
												 uC_USART_Buff::cback_status_t status )
		{
			( ( BACnet_UART* )param )->BACnet_UART_Callback( status );
		}

		BACNET_TX_MODES_EN m_b_tx_mode;
		bool_t m_b_port_enabled;
		bool_t m_b_parity_error;

		uint8_t* m_b_rx_data_ptr;
		uint16_t m_b_rx_data_length;

		uint8_t* m_b_rx_buff;
		uint16_t m_b_rx_buff_size;

		bacnet_rx_state_t m_b_rx_state;
		uint16_t m_b_bad_frame_count;

		BACNET_FRAME_STRUCT m_b_tx_struct;
		bacnet_tx_state_t m_b_tx_state;

		BF_Lib::Output* m_b_tx_enable_out;

		uC_USART_Buff* m_b_usart;
		// Modbus_Display* b_modbus_display;

		CR_Queue* m_b_rx_que;

		/*BACnet's Tframe_gap ( The maximum idle time a sending node may allow
		   to elapse between octets of a frame the node is transmitting: 20 bit times.) */
		// Time calc is boosted by a value which boosts things into a microsecond type of framework.
		// Calculation is microsecond boost * bits_per_char * msg_done_characters
		static const uint32_t BACNET_RX_DONE_TIME_CALC;

		/*BACnet's Tturnaround The minimum time after the end of the stop bit of
		   the final octet of a received frame before a node may enable its EIA-485
		   driver: 40 bit times.*/
		// Calculation is microsecond boost * bits_per_char * msg_done_characters
		static const uint32_t BACNET_TX_HOLDOFF_TIME_CALC;

};

#endif
