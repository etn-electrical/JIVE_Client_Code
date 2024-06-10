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
#ifndef __MODBUS_BUFFER___
#define __MODBUS_BUFFER___

#include "Includes.h"
#include "Modbus_Serial.h"
#include "CR_Queue.h"
#include "CR_Tasker.h"
#include "USB.h"
#include "Timers_uSec.h"
#include "Timers_Lib.h"
#include "Modbus_DCI_Data.h"

namespace BF_Mbus
{

#define REMOTE_MODBUS_STATUS_LED            EIP_MS_LED_GREEN_IO_CTRL

class Modbus_Buffer : public Modbus_Serial
{
	public:

		Modbus_Buffer( USB* usb );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Modbus_Buffer( void ) {}

		void Enable_Port( MB_RX_PROC_CBACK rx_cback,
						  MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
						  tx_mode_t modbus_tx_mode = MODBUS_RTU_TX_MODE, parity_t parity_select = MODBUS_PARITY_EVEN,
						  uint32_t baud_rate = UNUSED_DUE_TO_USB, bool use_long_holdoff = true,
						  stop_bits_t stop_bit_select = MODBUS_1_STOP_BIT );

		void Disable_Port( void );

		bool Port_Enabled( void );

		bool Idle( void );

		void Send_Frame( MB_FRAME_STRUCT* tx_struct );

		static uint8_t Get_Remote_Modbus_Static_Address( void );

		static parity_t Get_Remote_Modbus_Parity_Mode( void );

		static tx_mode_t Get_Remote_Modbus_Tx_Mode( void );

		static uint16_t Get_Remote_Modbus_Baud_Rate( void );

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_Buffer( const Modbus_Buffer& object );
		Modbus_Buffer & operator =( const Modbus_Buffer& object );

		void RX_Msg_Handler( CR_Tasker* cr_task );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void RX_Msg_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( Modbus_Buffer* )handle )->RX_Msg_Handler( cr_task );
		}

		void Msg_Timeout_Check( CR_Tasker* cr_task );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Msg_Timeout_Check_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( Modbus_Buffer* )handle )->Msg_Timeout_Check( cr_task );
		}

		uint16_t Add_Data_To_Buffer( uint8_t* data, uint32_t length );

		USB* usb_ctrl;

		bool m_port_enabled;
		BF_Lib::Timers::TIMERS_TIME_TD m_msg_timeout_tmr;

		uint32_t m_rx_done_time;

		MB_RX_PROC_CBACK m_rx_cback;
		MB_RX_PROC_CBACK_HANDLE m_rx_cback_handle;

		CR_Queue* m_rx_que;
		CR_Tasker* m_msg_tmout_crt;

		uint8_t m_rx_state;

		uint8_t buffer[USB_SERIAL_BUFFER_SIZE];
		uint32_t buffer_cnt;
		static const uint16_t MODBUS_RTU_RX_DONE_LONG_HOLDOFF_TIME = 750U;
		static const uint16_t MODBUS_RTU_TX_LONG_HOLDOFF_TIME = 1750U;
		static const uint16_t MBUS_DN_BAUD_RATE_DIP_SWITCH_TRIGGER_VAL = 3U;
		static const uint8_t UNUSED_DUE_TO_USB = 0U;
		static const uint16_t MBUS_DN_MAC_ID_DIP_SWITCH_TRIGGER_VAL = 0U;
		static const uint8_t REMOTE_MODBUS_STATIC_ADDRESS = 1U;
		static const parity_t REMOTE_MODBUS_PARITY_MODE = MODBUS_PARITY_EVEN;
		static const tx_mode_t REMOTE_MODBUS_TX_MODE = MODBUS_RTU_TX_MODE;
		static const uint16_t REMOTE_MODBUS_BAUD_RATE = 19200U;

		// RX States
		static const uint8_t MODBUS_SERIAL_RX_IDLE = 0U;
		static const uint8_t MODBUS_SERIAL_RX_BUSY = 1U;
		static const uint8_t MODBUS_SERIAL_RX_WAITING = 2U;
		static const uint8_t MODBUS_SERIAL_RX_MSG_READY = 3U;
		static const uint8_t MODBUS_SERIAL_PROCESSING = 4U;
};

}

#endif	/* __MODBUS_BUFFER___ */
