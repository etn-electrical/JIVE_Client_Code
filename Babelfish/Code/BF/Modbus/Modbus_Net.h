/**
 **********************************************************************************************
 * @file            Modbus_Net.h
 *
 * @brief           Contains the Modbus_Net Class. This file Creates Remote Modbus slave with specific communication
 * settings
 *
 * @details         It is designed to communicate with Modbus-slave or Modbus host port with master port.
 *                  Communication settings are passed to this class while creating new Modbus slave object.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef MODBUS_NET_H
#define MODBUS_NET_H

#include "Modbus_Msg.h"
#include "Modbus_Serial.h"
#include "Modbus_Defines.h"
#include "Modbus_DCI.h"

/**
 **********************************************************************************************
 * @brief        Modbus_Net Class Declaration. Creates a Remote modbus host port to communicate with master device.
 * @details      It provides public methods to
 * @n @b 1.      Enable Modbus slave port
 * @n @b 2.      Disable Modbus slave port
 * @n @b 3.      Get Modbus port status
 **********************************************************************************************
 */
class Modbus_Net
{
	public:
		/**
		 * @brief Modbus RTU size. Modbus serial adds on the ASCII Size
		 */
		static const uint16_t MODBUS_DEFAULT_BUFFER_SIZE = MODBUS_NET_BUFFER_SIZE;	// 256 = 513 actual ASCII serial
																					// size used by serial.

		/**
		 * @brief Callback handle Modbus response message
		 */
		typedef void* mb_net_resp_cback_handle;

		/**
		 * @brief Function pointer to Modbus response message Callback
		 */
		typedef void (* mb_net_resp_cback)( mb_net_resp_cback_handle resp_handle,
											MB_FRAME_STRUCT* rx_struct );

		/**
		 * @brief The location for the Debug frame counter
		 */
		static const uint16_t DEBUG_FRAME_COUNT_IND = 0U;

		/**
		 * @brief The location where the debug frame data will go
		 */
		static const uint16_t DEBUG_FRAME_DATA_IND = 1U;

		/**
		 * @brief                         Creates a Modbus Host port.
		 * @param [in] address            The Modbus address to be used for host device
		 * @param [in] modbus_serial_ctrl Modbus serial control. both USART or TCP protocol control structure can be
		 * used.
		 * @param [in] modbus_dev_profile The lookup table which associates Modbus registers with DCIDs.
		 * @param [in] comm_dog_owner     Communication loss timeout.  If NULL is passed in then
		 *                                communication loss is not checked.  This value is in milliseconds.
		 * @param [in] buffer_size        The transmit and receive buffer size.  It is ideal to leave this
		 *                                as the default modbus buffer size which should account for standard message
		 * size.
		 *  @param[in] correct_out_of_range_sets Determines whether the out of range
		 *                                values should be corrected or an return error
		 */
		Modbus_Net( uint8_t address,
					Modbus_Serial* modbus_serial_ctrl,
					const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
					DCI_Owner* comm_dog_owner = nullptr,
					uint16_t buffer_size = MODBUS_DEFAULT_BUFFER_SIZE,
					bool correct_out_of_range_sets = true );

		/**
		 * @brief Creates a Modbus Host port.
		 * @details Used to pass in application specific com_Dog objects
		 * @param address The Modbus address to be used on the host.
		 * @param modbus_serial_ctrl Modbus serial control.  Can be USART or TCP.
		 * @param modbus_dev_profile The lookup table which associates
		 * Modbus registers with DCIDs.
		 * @param comm_dog Pointer to Com_Dog object
		 * @param buffer_size The transmit and receive buffer size.  It is ideal to leave this
		 * as the default modbus buffer size which should account for standard message size.
		 *  @param[in] correct_out_of_range_sets Determines whether the out of range
		 *  values should be corrected or an return error
		 * @return this.
		 */
		Modbus_Net( uint8_t address, Modbus_Serial* modbus_serial_ctrl,
					BF_Mbus::Modbus_Msg* mbus_msg );

		/**
		 * @brief                          This constructor is generally used for creating Modbus Master
		 * @details                        Creates a Modbus Host port which could be address by master.
		 * @param[in] modbus_serial_ctrl:  Modbus serial control.  Can be USART or TCP.
		 * @param[in] resp_cback:          Callback Function pointer to function being called.
		 * @param[in] resp_cback_handle:   Generic callback handle for callback function
		 */
		Modbus_Net( Modbus_Serial* modbus_serial_ctrl,
					mb_net_resp_cback resp_cback, mb_net_resp_cback_handle resp_cback_handle );

		/**
		 *  @brief                         Destructor
		 *  @details                       This will get called when object of Modbus_Net goes out of scope
		 */
		~Modbus_Net( void );

		/**
		 * @brief                          Enable modbus port with communication parameter settings
		 * @details                        This functions is used to enable the Modbus port with specific communication
		 * settings
		 * @param[in] modbus_tx_mode       Selects the tx mode for UART communication. Possible values are mentioned
		 * below -
		 * @n                              MODBUS_RTU_TX_MODE - Transmitted bytes are presented as binary (default)
		 * @n                              MODBUS_ASCII_TX_MODE - Transmitted bytes are presented as ASCII
		 * @param[in] parity_select        Selects parity type for UART communication. Possible values are mentioned
		 * below -
		 * @n                              MODBUS_PARITY_EVEN - Even Parity (default)
		 * @n                              MODBUS_PARITY_ODD - Odd Parity
		 * @n                              MODBUS_PARITY_NONE - No Parity
		 * @param[in] baud_rate            Selects baud rate for UART communication. Possible values are mentioned below
		 *-
		 * @n                              MODBUS_BAUD_19200  - 19200 Bits/PS (default)
		 * @n                              MODBUS_BAUD_9600   - 9600 Bits/PS
		 * @n                              MODBUS_BAUD_38400  - 38400 Bits/PS
		 * @n                              MODBUS_BAUD_57600  - 57600 Bits/PS
		 * @n                              MODBUS_BAUD_115200 - 115200 Bits/PS
		 * @n                              MODBUS_BAUD_230400 - 230400 Bits/PS
		 * @param[in] use_long_holdoff     true - 1.75ms is used as the min msg holdoff
		 *                                 false - timeouts will not be fixed but will be calculated
		 *                                 as 3.5 and 1.5 character times for that respective baudrate.
		 * @n                              As per Modbus spec,"The implementation of RTU reception driver may imply
		 *                                 the management of a lot of interruptions due to the t1.5 and t3.5 timers.
		 *                                 With high communication baud rates, this leads to a heavy CPU load.
		 *                                 Consequently these two timers must be strictly respected when the baud rate
		 * is equal or lower
		 *                                 than 19200 Bps. For baud rates greater than 19200 Bps,fixed values for the 2
		 * timers
		 *                                 should be used: it is recommended to use a value of 750µs for the
		 * inter-character time-out (t1.5)
		 *                                 and a value of 1.750ms for inter-frame delay (t3.5).
		 * @param[in] stop_bits            Selects stop bits for UART communication. Possible values are mentioned below
		 *-
		 * @n                              MODBUS_1_STOP_BIT - 1 Stop Bit (default)
		 * @n                              MODBUS_2_STOP_BIT - 2 Stop Bit
		 */
		void Enable_Port( tx_mode_t modbus_tx_mode = MODBUS_RTU_TX_MODE,
						  parity_t parity = MODBUS_PARITY_EVEN, uint32_t baud_rate = 19200,
						  bool use_long_holdoff = true, stop_bits_t stop_bits = MODBUS_1_STOP_BIT );

		/**
		 * @brief                         Disable the Modbus Port
		 * @details                       This function will disable the Rx & Tx communication on USART port.
		 * @param[in]                     None
		 */
		void Disable_Port( void );

		/**
		 * @brief                         Port enable status
		 * @details                       This function will check port enable status.
		 * @param[in]                     None
		 * @return                        True if port is enabled else False
		 */
		bool Port_Enabled( void );

		/**
		 * @brief                         Port Idle status
		 * @details                       This function will check if port is idle or busy
		 * @param[in]                     None
		 * @return                        True if port is idle, False if port is busy
		 */
		bool Port_Idle( void )
		{
			return ( m_mb_serial->Idle() );
		}

		/**
		 * @brief                         Change address
		 * @details                       This function will change the address of Modbus host.
		 * @param[in] address             new modbus address
		 */
		void Change_Address( uint8_t address );

		/**
		 * @brief                         Send frame through Modbus port
		 * @details                       This function will transmit the Modbus frame append with CRC
		 * @param[in] tx_struct:          To be transmitted standard Modbus frame structure
		 */
		void Send_Frame( MB_FRAME_STRUCT* tx_struct );

		/**
		 * @brief                         Attach_Callback user function
		 * @details                       A callback can be attached to control the parameter.
		 * @param[in] callback            Function pointer to function being called
		 * @n                             Once the callback is invoked attached function will get called. This function
		 * should do further processing on data
		 * @param[in] param               parameters of callback function
		 * @param[in] function_code       Modbus function code
		 */
		void Attach_User_Function( uint16_t function_code,
								   modbus_message_callback_param_t param, modbus_message_callback_t callback );

		/**
		 * @brief                         Monitor failed CRC count
		 * @details                       monitor the no of times CRC failed attempt
		 * @param[in] count_of_failures   DCI owner to store CRC failure counter
		 */
		void Monitor_Failed_CRC_Count( DCI_Owner* count_of_failures );

		/**
		 * @brief                          Will dump data out the serial port to be picked up and processed externally.
		 * @details                        If a master control uses this method, then a TX buffer must be passed in
		 *                                 with appropriate head room for the debug counter and EOL characters if they
		 * exist.
		 *                                 If this is run in the slave mode then the TX buffer is already allocated and
		 * the room
		 *                                 is already allocated for the counter.
		 * @param[in] data                 The data you would like to ship.
		 * @param[in] length               The amount of data to be shipped out.
		 */
		void Debug_Dump( uint8_t* data, uint8_t length );

		/**
		 *  @brief Process modbus command receviced from web service
		 *  @details Process modbus protocol request, in case of wrong request it shall return Modbus Error Codes.
		 *  in case any error found while accessing corresponding register then it shall return corresponding Modbus
		 * Error Codes.
		 *  @param[in] rx_struct,  pointer to Modbus frame which is send by master to slave, it has information of
		 * requested modbus frame.
		 *  @param[out] tx_struct, pointer to Modbus frame which is supposed to respond on the modbus request protocol.
		 *  @return modbus error code.
		 *  @retval 0U(MB_NO_ERROR_CODE):In case there is no error while prosessing modbus request.
		 *  @retval 01U(MB_ILLEGAL_FUNCTION_ERROR_CODE):  in case there is error in modbus function protocol
		 *  @retval 03U(MB_ILLEGAL_DATA_VALUE_ERROR_CODE):  in case there is error in inappropriate data in modbus
		 * protocol
		 */
		uint8_t Process_Msg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		/**
		 *  @brief Provides the ability to extract the current setting for the Modbus msg buffer
		 *  size.  This is used in the cases where someone was handed a Modbus Msg handle instead
		 *  of constructing it themselves.
		 *  @details This is used in the cases where someone was handed a Modbus Msg handle instead
		 *  of constructing it themselves.
		 *  @return The buffer size allowed in Modbus Msg.
		 */
		uint16_t Get_Msg_Buffer_Size( void ) { return ( m_tx_buff_size ); }

	private:
		/**
		 * @brief                 Privatized Copy Constructor
		 * @param[in] object      Modbus_Net object to be copied
		 */
		Modbus_Net( const Modbus_Net& object );

		/**
		 * @brief                 Privatized Copy Assignment Operator
		 * @param[in] object      Modbus_Net object to be copied
		 */
		Modbus_Net & operator =( const Modbus_Net& object );

		/**
		 * @brief                 Function to process received data through modbus frames
		 * @param[in] rx_struct   modbus frame structure
		 */
		void Process_Req( MB_FRAME_STRUCT* rx_struct );

		/**
		 * @brief                 Function handler for Function Process_Req
		 * @param[in] handle      handle to modbus host object
		 * @param[in] rx_struct   modbus frame structure
		 */
		static void Process_Req_Static( void* handle, MB_FRAME_STRUCT* rx_struct )
		{
			( ( Modbus_Net* )handle )->Process_Req( rx_struct );
		}

		/**
		 * @brief                 Function to process Response message in case of master device
		 * @param[in] rx_struct  modbus frame structure
		 */
		void Process_Resp( MB_FRAME_STRUCT* rx_struct );

		/**
		 * @brief                 Function handler for Function Process_Resp
		 * @param[in] handle      handle to modbus host object
		 * @param[in] rx_struct   modbus frame structure
		 */
		static void Process_Resp_Static( void* handle, MB_FRAME_STRUCT* rx_struct )
		{
			( ( Modbus_Net* )handle )->Process_Resp( rx_struct );
		}

		/**
		 * @brief                 Checks the received message for proper CRC then converts to RTU if necessary.
		 * @details               It also modifies the length to strip out the CRC length.
		 * @param[in] rx_struct   Modbus frame structure
		 */
		bool Check_CRC_And_Convert( MB_FRAME_STRUCT* rx_struct ) const;

		/**
		 * @brief                 Create an RTU frame before sending it to modbus port
		 * @details               It also calculate and append CRC for respective frame.
		 * @param[in] tx_data     Pointer to data buffer to be transmitted
		 * @param[in] tx_length   length or size of total data
		 */
		void Create_RTU_Frame( uint8_t* tx_data, uint16_t& tx_length ) const;

		/**
		 * @brief                 Create an ASCII frame before sending it to modbus port
		 * @details               It also calculate and append CRC for respective frame.
		 * @param[in] tx_data     Pointer to data buffer to be transmitted
		 * @param[in] tx_length   length or size of total data
		 */
		void Create_ASCII_Frame( uint8_t* tx_data, uint16_t& tx_length ) const;

		/**
		 * @brief                        Converts string of ASCII characters into Hex
		 * @details                      It doesn't convert data with Odd length and returns error.
		 * @param[in] src_data           pointer to source data buffer
		 * @param[in] length             Length or size of total data to be converted
		 * @param[in] dest_data          pointer to destination data buffer
		 * @param[in] high_nibble_first  true - Higher nibble(byte) inserted first in destination array
		 * @n                            false - Lower nibble(byte) is inserted first in destination array
		 * @return                       returns true if successful conversion else false
		 */
		bool Convert_String_ASCII2HEX( uint8_t* src_data, uint16_t& length,
									   uint8_t* dest_data, bool high_nibble_first = true ) const;

		/**
		 * @brief                        Converts ASCII character into Hex
		 * @param[in] ascii_data         single ASCII character to be converted to hex
		 * @return                       returns Hex equivalent of ASCII value passed as parameter
		 */
		uint8_t Convert_ASCII2HEX( uint8_t ascii_data ) const;

		/**
		 * @brief                        Converts string of HEX to ASCII characters
		 * @param[in] src_data           pointer to source data buffer
		 * @param[in] length             Length or size of total data to be converted
		 * @param[in] dest_data          pointer to destination data buffer
		 * @param[in] high_nibble_first  true - Higher nibble(byte) inserted first in destination array
		 * @n                            false - Lower nibble(byte) is inserted first in destination array
		 */
		void Convert_String_HEX2ASCII( uint8_t* src_data, uint16_t& length,
									   uint8_t* dest_data, bool high_byte_first = true ) const;

		/**
		 * @brief                        Converts HEX value to a ASCII character
		 * @param[in] ascii_data         Ascii data to be converted to hex
		 * @param[in] dest_string        pointer to destination buffer
		 * @param[in] high_byte_first    true - Higher byte inserted first in destination array
		 * @n                            false - Lower byte is inserted first in destination array
		 */
		void Convert_HEX2ASCII( uint8_t ascii_data, uint8_t* dest_string,
								bool high_byte_first ) const;

		/**
		 * @brief                        Checks if address of message is valid
		 * @details                      This function verifies if message address is global or same as of processing
		 * node.
		 * @                             It also claims message valid if Modbus Tx mode is TCP.
		 * @param[in] address            destination address in modbus message
		 * @return                       true - if message address is valid
		 * @n                            false - if message address is invalid
		 */
		bool Msg_For_Me( uint8_t address ) const;

		/**
		 * @brief                        Ready for Reset Callback function
		 * @param[in] reset_req          Reset Type
		 * @return                       true - if device is idle
		 * @n                            false - if device is not idle
		 */
		bool Ready_For_Reset( BF::System_Reset::reset_select_t reset_req );

		/**
		 * @brief                        handle for Callback function
		 * @param[in] handle             param for Ready_For_Reset function
		 * @param[in] reset_req          Reset Type
		 * @return                       true - if device is idle
		 * @n                            false - if device is not idle
		 */
		static bool Ready_For_Reset_Static( BF::System_Reset::cback_param_t handle,
											BF::System_Reset::reset_select_t reset_req )
		{
			return ( ( ( Modbus_Net* )handle )->Ready_For_Reset( reset_req ) );
		}

		/**
		 * @brief Modbus Exception/Error response constants
		 */
		static const uint8_t MB_EXCEPTION_RESP_FUNC_CODE_MASK = 0x80U;
		static const uint8_t MB_EXCEPTION_RESP_HEADER_SIZE = 3U;// Size is sans the CRC and address.
		static const uint8_t MB_EXCEPTION_ADDRESS_IND = 0U;
		static const uint8_t MB_EXCEPTION_FUNC_CODE_IND = 1U;
		static const uint8_t MB_EXCEPTION_CODE_IND = 2U;

		/**
		 * @brief Modbus ASCII message constants
		 */
		static const uint8_t MODBUS_ASCII_BOF_LOC = 0U;
		static const uint8_t MB_ASCII_BYTE_CRC_SIZE = 1U;
		static const uint8_t MB_ASCII_CHAR_EOF_SIZE = 2U;	// This is actually 1 LRC byte busted out to
															// ASCII which ends up being 2 ASCII characters.

		/**
		 * @brief Modbus RTU message constants
		 */
		static const uint8_t MB_RTU_ADDRESS_SIZE = 1U;
		static const uint8_t MB_RTU_CRC_SIZE = 2U;
		static const uint8_t MB_NET_RTU_ADDRESS_IND = 0U;
		static const uint8_t MB_NET_RTU_FUNC_CODE_IND = 1U;

		/**
		 * @brief Pointer to hold address of modbus serial object
		 */
		Modbus_Serial* m_mb_serial;

		/**
		 * @brief Pointer to hold address of Modbus_Msg object
		 */
		BF_Mbus::Modbus_Msg* m_mb_msg;

		/**
		 * @brief variable to hold Modbus transmission mode
		 */
		tx_mode_t m_tx_mode;

		/**
		 * @brief variable to hold Modbus port address
		 */
		uint8_t m_address;

		/**
		 * @brief Function pointer to store modbus message response callback
		 */
		mb_net_resp_cback m_resp_cback;

		/**
		 * @brief handle to store modbus message response callback
		 */
		mb_net_resp_cback_handle m_resp_cback_handle;

		/**
		 * @brief function pointer to callback for Process req or Process response for slave or master device
		 * respectively
		 */
		Modbus_Serial::MB_RX_PROC_CBACK m_serial_rx_proc_cback;

		/**
		 * @brief pointer to hold address of transmit buffer
		 */
		uint8_t m_tx_buff[MODBUS_DEFAULT_BUFFER_SIZE];

		/**
		 * @brief variable to hold size of tranmit buffer
		 */
		uint16_t m_tx_buff_size;

		/**
		 * @brief DCID owner to hold count of CRC failure
		 */
		DCI_Owner* m_count_of_crc_failures;

		/**
		 * @brief Variable to hold number of CRC errors
		 */
		uint16_t m_num_crc_errors;
};

#endif
