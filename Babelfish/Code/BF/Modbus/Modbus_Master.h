/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Provides a Modbus master interface for a serial connection.
 *
 *	@details This Modbus master uses its own transmit buffer.  It is designed to use a
 *	coroutine and callback method.  In this case a callback is passed in.  On a
 *	request the transmit happens during the request and the response will call the
 *	callback passed in during construction.  This module use full RTOS
 *	functionality.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H

#include "Modbus_Defines.h"
#include "OS_Tasker.h"
#include "OS_Semaphore.h"
#include "Modbus_Net.h"
#include "OS_Queue.h"
#include "Timers_Lib.h"

namespace BF_Mbus
{
// typedef void 	MB_ROUTING_RESP_CBACK( MB_ROUTING_RESP_CBACK_HANDLE resp_handle, MB_FRAME_STRUCT* rx_struct, uint8_t
// resp_error );

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
/**
 ****************************************************************************************
 * @brief This is a Modbus_Master class
 * @details It provides Modbus master interface for a serial connection.
 * @n @b Usage: On a request the transmit happens during the request and the response will call the
 * callback passed in during construction.  This module use full RTOS functionality.
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

class Modbus_Master
{
	public:
		/**
		 * @brief Modbus Master constructor.
		 * @details initialize modbus master with communication settings.
		 * @param[in] modbus_serial_ctrl: It is the serial access port.
		 * @param[in] response_timeout: It is the amount of time to wait for the slave to respond
		 * in milliseconds.
		 * @param[in] tx_buffer_size: It is the amount of space to allocate for the tx buffer.
		 * @param[in] coil_byte_swapping_enable: flag to enable swapping of bytes for coil and discrete inputs.
		 * @returns This.
		 */
		Modbus_Master( Modbus_Serial* modbus_serial_ctrl,
					   OS_TICK_TD response_timeout = MB_MASTER_DEFAULT_RESPONSE_TIMEOUT,
					   uint16_t tx_buffer_size = Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE,
					   bool coil_byte_swapping_enable = true );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Master goes out of scope
		 *  @return None
		 */
		~Modbus_Master( void );


		/**
		 *  @brief read Modbus Coils.
		 *  @details this function code is used to read coil status details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] coil: The coil status to get.
		 *  @param[in] coil_count: The total number of coils to read.
		 *  @param[out] data: The pointer to the data destination,The data comes back as little endian.
		 *  @return  The modbus error code which was passed back.
		 */
		uint8_t Read_Coil( uint8_t address, uint16_t coil, uint16_t coil_count, uint8_t* data,
						   uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief read Modbus Discrete Inputs.
		 *  @details this function code is used to read discrete input details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] discrete_input: The discrete input to get.
		 *  @param[in] discrete_input_count: The total number of discrete inputs to read.
		 *  @param[out] data: The pointer to the data destination,The data comes back as little endian.
		 *  @return  The modbus error code which was passed back.
		 */
		uint8_t Read_Discrete_Input( uint8_t address, uint16_t discrete_input, uint16_t discrete_input_count,
									 uint8_t* data, uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief get Modbus Registers.
		 *  @details this function code is used to get holding registers details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] reg: The register to get.
		 *  @param[out] data: The pointer to the data destination,The data comes back as little endian.
		 *  @param[in] byte_count: The total number of bytes to get.
		 *  @return  The modbus error code which was passed back.
		 */
		uint8_t Get_Reg( uint8_t address, uint16_t reg, uint16_t byte_count, uint8_t* data,
						 uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );


		/**
		 *  @brief set Modbus Registers.
		 *  @details this function code is used to set holding registers details of a remote device(slave device) as per
		 * requested value.
		 *  @param[in] address: The modbus target address/node id.
		 *  @param[in] reg: The register to set.
		 *  @param[in] data: The pointer to the data source.
		 *  @param[in] byte_count: The total number of bytes to set.
		 *  @return  The modbus error code which was passed back.
		 */
		uint8_t Set_Reg( uint8_t address, uint16_t reg, uint16_t byte_count, uint8_t* data,
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
		 *  @return  The modbus error code which was passed back.
		 */
		uint8_t GetSet_Reg( uint8_t address,
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
		 *  @return contains the modbus fault code.
		 */
		uint8_t User_Func_Code( uint8_t address, uint8_t user_func,
								uint8_t* tx_data = nullptr, uint16_t tx_data_len = 0,
								uint8_t* rx_data = nullptr, uint16_t* rx_data_len = nullptr );

		static const uint32_t DEFAULT_BAUD_RATE = 19200U;

		/**
		 *  @brief Enable modbus port with communication parameter settings
		 *  @details This functions is used to enable the Modbus port with specific communication settings
		 *  @param[in] modbus_tx_mode
		 *      @n @b Usage: specify transmission mode(RTU/ASII)
		 *  @param[in] baud_rate
		 *      @n @b Usage:specify baud rate settings for communication, default is 19200
		 *  @param[in] parity
		 *      @n @b Usage: specify parity (EVEN/ODD/NO PARITY)
		 *  @param[in] use_long_holdoff
		 *      @n @b Usage: "use_long_holdoff = true" means 1.75ms is used as the min msg holdoff
		 *                    This should only be called once per instance to avoid memory leaks.
		 *                    To do otherwise would require modifications.
		 *  @param[in] stop_bits
		 *      @n @b Usage:  specify number of stop bits during communication(1/2/NONE)
		 *  @return  None
		 */
		void Enable_Port( tx_mode_t modbus_tx_mode, parity_t parity, uint32_t baud_rate =
						  DEFAULT_BAUD_RATE, bool use_long_holdoff = true,
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

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Master( const Modbus_Master& object );
		Modbus_Master & operator =( const Modbus_Master& object );

		typedef void* mb_routing_resp_cback_handle;

		static const uint8_t MB_RESP_MASK_CHECK = 0xFF;
		static const uint8_t MB_RESP_MASK_IGNORE = 0U;
		static const uint8_t MB_MASTER_CTRL_REQ_TIMEOUT = 50;	// In milliseconds.
		static const uint8_t MB_MASTER_RESP_CHECK_BUFF_SIZE = 20;
		static const uint8_t MB_MASTER_DEFAULT_RESPONSE_TIMEOUT = 250;	// In milliseconds.
		// static const uint16_t  MB_MASTER_DEFAULT_TX_BUFF_SIZE = MODBUS_DEFAULT_BUFFER_SIZE;

		void Response_Handler( MB_FRAME_STRUCT* rx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Response_Handler_Static( mb_routing_resp_cback_handle handle,
											 MB_FRAME_STRUCT* rx_struct )
		{
			( ( Modbus_Master* )handle )->Response_Handler( rx_struct );
		}

		bool Response_Check( MB_FRAME_STRUCT* rx_struct );

		void Create_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const;

		void Extract_MB_Data( uint8_t* src_data, uint8_t* dest_data, uint16_t length ) const;

		Modbus_Net* m_mb_net;

		OS_Queue* m_rx_queue;
		OS_Semaphore* m_busy_sph;

		bool m_resp_pending;

		uint8_t m_resp_check[MB_MASTER_RESP_CHECK_BUFF_SIZE];
		uint8_t m_resp_check_mask[MB_MASTER_RESP_CHECK_BUFF_SIZE];
		uint16_t m_resp_check_len;

		uint8_t m_tx_buff[Modbus_Net::MODBUS_DEFAULT_BUFFER_SIZE] = { 0 };
		uint16_t m_tx_buff_size;
		OS_TICK_TD m_response_timeout;

		uint8_t m_resp_check_buff[MB_MASTER_RESP_CHECK_BUFF_SIZE];
		uint8_t m_resp_mask_buff[MB_MASTER_RESP_CHECK_BUFF_SIZE];

		// Flag used for enabling swapping of bytes
		bool m_coil_byte_swapping_enable;
};

}

#endif	/* MODBUS_MASTER_H */
