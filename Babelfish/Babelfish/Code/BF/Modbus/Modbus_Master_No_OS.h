/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Provides a Modbus master interface for a serial connection.
 *
 *	@details This Modbus master uses its own transmit buffer.  It is designed to use a
 *	coroutine and callback method.  In this case a callback is passed in.  On a
 *	request the transmit happens during the request and the response will call the
 *	callback passed in during construction.  This module does not use full RTOS
 *	functionality.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_MASTER_NO_OS_H
#define MODBUS_MASTER_NO_OS_H

#include "Includes.h"
#include "Modbus_Defines.h"
#include "Modbus_Net.h"
#include "Timers_Lib.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// typedef void 	MB_ROUTING_RESP_CBACK( MB_ROUTING_RESP_CBACK_HANDLE resp_handle, MB_FRAME_STRUCT* rx_struct, uint8_t
// resp_error );
/**
 ****************************************************************************************
 * @brief This is a Modbus_Master_No_OS class
 * @details It provides Modbus master interface for a serial connection.
 * @n @b Usage: On a request the transmit happens during the request and the response will call the
 * callback passed in during construction.  This module does not use full RTOS functionality.
 * It provides public methods to
 * @n @b 1. Get Modbus slave Coils
 * @n @b 2. Get Modbus slave Discrete Inputs
 * @n @b 3. Get Modbus slave Registers
 * @n @b 4. Set Modbus slave Registers
 * @n @b 5. To Enable initialized USART  port with specified parity, baud rate, stop bits, transmission mode.
 * @n @b 6. To Disable initialized USART port.
 * @n @b 7. To find out port enable status.
 ****************************************************************************************
 */
class Modbus_Master_No_OS
{
	public:

		static const uint16_t DEFAULT_RESPONSE_TIMEOUT = 250U;		///< In milliseconds.
		static const uint16_t RESPONSE_TIMEOUT_DISABLE_VAL = 0U;
		static const uint32_t DEFAULT_BAUD_RATE = 19200U;

		typedef void* response_callback_param_t;
		typedef void response_callback_t( response_callback_param_t resp_handle,
										  uint8_t* data, uint16_t length, uint8_t resp_error );


		/**
		 * @brief Modbus Master constructor.
		 * @details initialize modbus master with communication settings.
		 * @param[in] modbus_serial_ctrl: It is the serial access port.
		 * @param[in] response_callback_param: The callback parameter to be passed into the callback.
		 * @param[in] response_callback: The callback to be used when a response or failed response has occurred.
		 * @param[in] response_timeout: It is the amount of time to wait for the slave to respond.
		 * @param[in] tx_buffer_size: It is the amount of space to allocate for the tx buffer.
		 * @param[in] coil_byte_swapping_enable: flag to enable swapping of bytes for coil and discrete inputs.
		 * @returns This.
		 */
		Modbus_Master_No_OS( Modbus_Serial* modbus_serial_ctrl,
							 response_callback_t* response_callback,
							 response_callback_param_t response_callback_param = nullptr,
							 BF_Lib::Timers::TIMERS_TIME_TD response_timeout = DEFAULT_RESPONSE_TIMEOUT,
							 uint16_t tx_buffer_size = MODBUS_DEFAULT_BUFFER_SIZE,
							 bool coil_byte_swapping_enable = true );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Master_No_OS goes out of scope
		 *  @return None
		 */

		~Modbus_Master_No_OS( void );


		/**
		 *  @brief read Modbus Coils.
		 *  @details this function code is used to read coil status details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] coil: The coil status to get.
		 *  @param[in] coil_count: The total number of coils to read.
		 *  @param[out] get_data: The pointer to the data destination,The data comes back as little endian.
		 *  @return  Success
		 */
		bool Read_Coil( uint8_t address, uint16_t coil, uint16_t coil_count,
						uint8_t* get_data, uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief read Modbus Discrete Inputs.
		 *  @details this function code is used to read discrete input details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] discrete_input: The register to get.
		 *  @param[in] discrete_input_count: The total number of discrete inputs to read.
		 *  @param[out] get_data: The pointer to the data destination,The data comes back as little endian.
		 *  @return  Success
		 */
		bool Read_Discrete_Input( uint8_t address, uint16_t discrete_input, uint16_t discrete_input_count,
								  uint8_t* get_data, uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief get Modbus Registers.
		 *  @details this function code is used to get holding registers details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] reg: The register to get.
		 *  @param[out] get_data: The pointer to the data destination,The data comes back as little endian.
		 *  @param[in] byte_count: The total number of bytes to get.
		 *  @return  Success
		 */
		bool Get_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
					  uint8_t* get_data,
					  uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );

		/**
		 *  @brief set Modbus Registers.
		 *  @details this function code is used to set holding registers details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] reg: The register to set.
		 *  @param[in] set_data: The pointer to the data source.
		 *  @param[in] byte_count: The total number of bytes to set.
		 *  @return  Success
		 */
		bool Set_Reg( uint8_t address, uint16_t reg, uint16_t byte_count,
					  uint8_t* set_data,
					  uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief set-get Modbus Registers.
		 *  @details this function code is used to set & get holding registers details of a remote device(slave device)
		 * as per request.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] get_reg: The register to get.
		 *  @param[in] get_byte_count: The total number of bytes to get.
		 *  @param[out] get_data: The pointer to the data destination.
		 *  @param[in] set_reg:The register to set.
		 *  @param[in] set_byte_count: The total number of bytes to set
		 *  @param[in] set_data: The pointer to the data source.
		 *  @return  Success
		 */
		bool GetSet_Reg( uint8_t address,
						 uint16_t get_reg, uint16_t get_byte_count, uint8_t* get_data,
						 uint16_t set_reg, uint16_t set_byte_count, uint8_t* set_data,
						 uint8_t get_attribute = MB_ATTRIB_NORMAL_ACCESS,
						 uint8_t set_attribute = MB_ATTRIB_NORMAL_ACCESS );

		/**
		 *  @brief User function code.
		 *  @details This function code is used for user defined modbus function code
		 *  @param[in] address: The target modbus address.
		 *  @param[in] user_func: It is the user defined modbus function that is required.
		 *  @param[in] tx_data is used for sending.
		 *  @param[in] tx_data_len: amount of data to send.
		 *  @param[in] rx_data is used for the received data.
		 *  @param[in] rx_data_len: It is the data length which was returned.
		 *  @return  Success
		 */
		bool User_Func_Code( uint8_t address, uint8_t user_func,
							 uint8_t* tx_data = nullptr, uint16_t tx_data_len = 0U,
							 uint8_t* rx_data = nullptr, uint16_t rx_data_len = 0U );

		/**
		 *  @brief Enable modbus port with communication parameter settings
		 *  @details This functions is used to enable the USART port with specific communication settings
		 *  @param[in] modbus_tx_mode
		 *      @n @b Usage: specify transmission mode(RTU/ASII)
		 *  @param[in] baud_rate
		 *      @n @b Usage:specify baud rate settings for communication, default is 19200
		 *  @param[in] parity
		 *      @n @b Usage: specify parity (EVEN/ODD/NO PARITY)
		 *  @param[in] use_long_holdoff
		 *      @n @b Usage: "use_long_holdoff = true" means 1.75ms is used as the min msg holdoff
		 *      As per Modbus spec,"The implementation of RTU reception driver may imply
		 *      the management of a lot of interruptions due to the t1.5 and t3.5 timers.
		 *      With high communication baud rates, this leads to a heavy CPU load.
		 *      Consequently these two timers must be strictly respected when the baud rate is equal or lower
		 *      than 19200 Bps. For baud rates greater than 19200 Bps,fixed values for the 2 timers
		 *      should be used: it is recommended to use a value of 750µs for the inter-character time-out (t1.5)
		 *      and a value of 1.750ms for inter-frame delay (t3.5)"So in its Usage update that if use_long_holdoff
		 *      is true means fixed timeout ised for above 19200 baudrate, If its value is false means timeouts will
		 *      not be fixed but will be  calculated as 3.5 and 1.5 character times for that respective baudrate
		 *  @param[in] stop_bits
		 *      @n @b Usage:  specify number of stop bits during communication(1/2/NONE)
		 *  @return  None
		 */
		void Enable_Port( tx_mode_t modbus_tx_mode, parity_t parity,
						  uint32_t baud_rate = DEFAULT_BAUD_RATE, bool use_long_holdoff = false,
						  stop_bits_t stop_bits = MODBUS_1_STOP_BIT )
		{
			m_mb_net->Enable_Port( modbus_tx_mode, parity, baud_rate, use_long_holdoff, stop_bits );
		}

		/**
		 *  @brief Disable the Modbus Port
		 *  @details This function will disable the Rx & Tx communication on USART port.
		 *  @param[in] None
		 *  @return None
		 */
		void Disable_Port( void )
		{
			m_mb_net->Disable_Port();
		}

		/**
		 *  @brief Port enable status
		 *  @details This function will check port enable status.
		 *  @param[in] None
		 *  @return True if port is enable else False
		 */
		bool Port_Enabled( void )
		{
			return ( m_mb_net->Port_Enabled() );
		}

		/**
		 * @brief  Attach callback
		 * @detail Provides the ability to change the response handler.
		 * @param[in] response_callback: The callback to be used when a response or failed response has occurred.
		 * @param[in] response_callback_param: The callback parameter to be passed into the callback.
		 * @return None
		 */
		void Attach_Callback( response_callback_t* response_callback,
							  response_callback_param_t response_callback_param = nullptr );

		/**
		 * @brief Provides the ability to attach a callback for the times when a response
		 * was received which was not a match to what was requested.
		 * @details When the master sends out a request it also creates a response match field.
		 * When the response does not match the original request pattern we typically throw away
		 * this response.  In some cases we need to further evaluate this sporadic unrequested
		 * response.  This callback provides this ability.
		 * @param response_callback:  The callback to be used when a sporadic response
		 * response has occurred.
		 * @param response_callback_param: The callback parameter to be passed into the callback.
		 */
		void Attach_Sporadic_Response_Callback( response_callback_t* response_callback,
												response_callback_param_t response_callback_param = nullptr );

		/**
		 * @brief  setting timeout for modbus protocol.
		 * @detail Provides the time frame to modbus slave to respond the request.
		 * @param[in] response_timeout: Time out in milisecond.
		 * @return None
		 */
		void Set_Timeout( BF_Lib::Timers::TIMERS_TIME_TD response_timeout );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_Master_No_OS( const Modbus_Master_No_OS& object );
		Modbus_Master_No_OS & operator =( const Modbus_Master_No_OS& object );

		static const uint8_t RESP_CHECK_BUFF_SIZE = 20U;
		static const uint8_t MB_RESP_MASK_CHECK = 0xFFU;
		static const uint8_t MB_RESP_MASK_IGNORE = 0x00U;
		static const uint16_t MODBUS_DEFAULT_BUFFER_SIZE = MODBUS_NET_BUFFER_SIZE;	// 256 = 513 actual ASCII serial
																					// size used by serial.
		typedef void* MB_ROUTING_RESP_CBACK_HANDLE;

		void Response_Handler( MB_FRAME_STRUCT* rx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Response_Handler_Static( Modbus_Net::mb_net_resp_cback_handle handle,
											 MB_FRAME_STRUCT* rx_struct )
		{
			( ( Modbus_Master_No_OS* )handle )->Response_Handler( rx_struct );
		}

		void Response_Timeout( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Response_Timeout_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( Modbus_Master_No_OS* )handle )->Response_Timeout();
		}

		bool Response_Check( MB_FRAME_STRUCT* rx_struct );

		void Create_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const;

		void Extract_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const;

		BF_Lib::Timers* m_response_timer;
		BF_Lib::Timers::TIMERS_TIME_TD m_response_timeout;

		Modbus_Net* m_mb_net;

		uint8_t m_resp_check[RESP_CHECK_BUFF_SIZE];
		uint8_t m_resp_check_mask[RESP_CHECK_BUFF_SIZE];
		uint16_t m_resp_check_len;

		response_callback_param_t m_response_callback_param;
		response_callback_t* m_response_callback;

		response_callback_param_t m_sporadic_response_callback_param;
		response_callback_t* m_sporadic_response_callback;

		uint8_t m_tx_buff[MODBUS_DEFAULT_BUFFER_SIZE] = { 0 };
		uint16_t m_tx_buff_size;

		// The pointer to the RX Buffer passed in from the creator.
		uint8_t* m_passed_rx_buff;
		uint16_t m_passed_rx_buff_len;

		uint8_t m_message_sent;
		// Flag used for enabling swapping of bytes
		bool m_coil_byte_swapping_enable;

		static const uint8_t NO_MSG_PENDING = 0U;
		static const uint8_t GET_REG = 1U;
		static const uint8_t SET_REG = 2U;
		static const uint8_t GET_SET_REG = 3U;
		static const uint8_t USER_FUNC = 4U;
		static const uint8_t READ_COIL = 5U;
		static const uint8_t READ_DISCRETE_INPUT = 6U;
};

}

#endif	/* MODBUS_MASTER_NO_OS_H */
