/**
 *****************************************************************************************
 *  @file
 *	@brief It provide the interface between Modbus master and Modbus slave
 *
 *	@details Modbus Master sends Modbus protocol to slave & then slave respond with modbus frame
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_MSG_H
#define MODBUS_MSG_H

#include "Modbus_DCI.h"
#include "Modbus_SysCmd.h"
#include "Com_Dog.h"

namespace BF_Mbus
{
/**
 ****************************************************************************************
 * @brief This is a Modbus_Msg class
 * @details It is the interface betweer Modbus master and modbus slave.
 * @n @b Usage: process the request of modbus master protocol.
 * It provides public methods to
 * @n @b 1. Process modbus command
 ****************************************************************************************
 */
class Modbus_Msg
{

	public:
		static const uint16_t MODBUS_MSG_DEFAULT_BUFFER_SIZE = MODBUS_MAX_RTU_MSG_LENGTH -
			MODBUS_MAX_RTU_MSG_CRC_LEN;

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus_Msg
		 *  @param[in] modbus_dev_profile:  Defines the Modbus target,
		 *  @a @n Usage: This is a container for the entire identity of a Modbus Interface
		 *  @param[in] comm_dog_owner: DCI owner for communication watchdog
		 *  @a @n Usage: Communication loss timeout.  If NULL is passed in then
		 *  communication loss is not checked.  This value is in milliseconds.
		 *  @param[in] buffer_size: buffer size for Modbus protocol frame
		 *  @a @n Usage: The transmit and receive buffer size.
		 *  @return this
		 */
		Modbus_Msg( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
					DCI_Owner* comm_dog_owner, uint16_t buffer_size = MODBUS_MSG_DEFAULT_BUFFER_SIZE,
					bool correct_out_of_range_sets = true );

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus_Msg
		 *  @details Used to pass in application specific com_Dog object
		 *  @param[in] modbus_dev_profile:  Defines the Modbus target,
		 *  @a @n Usage: This is a container for the entire identity of a Modbus Interface
		 *  @param comm_dog Pointer to Com_Dog object
		 *  @param[in] buffer_size: buffer size for Modbus protocol frame
		 *  @a @n Usage: The transmit and receive buffer size.
		 *  @return this
		 */
		Modbus_Msg( Modbus_DCI* modbus_dci_ctrl, BF_Misc::Com_Dog* comm_dog,
					Modbus_SysCmd* mbus_syscmd, uint16_t buffer_size = MODBUS_MSG_DEFAULT_BUFFER_SIZE );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_Msg goes out of scope
		 *  @return None
		 */
		~Modbus_Msg( void );

		/**
		 *  @brief Process modbus command
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
		 *  @brief Attach_Callback
		 *  @details A callback can be attached to handle the parameter.
		 *  @param[in] callback Function pointer to function being called
		 *  @n @b Usage: Once the callback is invoked attached function will get called which will do further processing
		 *  @param[in] param, parameters of callback function
		 *  @n @b Usage:
		 *  @return None
		 */
		void Attach_Callback( uint16_t function_code, modbus_message_callback_t callback,
							  modbus_message_callback_param_t param );

		/**
		 *  @brief Provides the ability to extract the current setting for the Modbus msg buffer
		 *  size.  This is used in the cases where someone was handed a Modbus Msg handle instead
		 *  of constructing it themselves.
		 *  @details This is used in the cases where someone was handed a Modbus Msg handle instead
		 *  of constructing it themselves.
		 *  @return The buffer size allowed in Modbus Msg.
		 */
		uint16_t Get_Msg_Buffer_Size( void ) { return ( m_buffer_size ); }

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Msg( const Modbus_Msg& object );
		Modbus_Msg & operator =( const Modbus_Msg& object );

		struct callback_list_item_t
		{
			uint16_t m_function_code;
			modbus_message_callback_t m_callback;
			modbus_message_callback_param_t m_param;
			callback_list_item_t* m_next_item;
		};

		uint8_t Msg_Length_Good( MB_FRAME_STRUCT const* x_struct, uint8_t func_code );

		uint8_t Read_Coils( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Read_Discrete_Inputs( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Read_Holding_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Read_Input_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Write_Single_Coil( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Write_Single_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Read_Exception_Status( MB_FRAME_STRUCT* rx_struct,
									   MB_FRAME_STRUCT* tx_struct );

		uint8_t Read_Diagnostics( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Write_Multiple_Coils( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Write_Multiple_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		uint8_t Write_Mask_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
		{
			return ( 0 );
		}	// Not defined

		uint8_t Read_Write_Multiple_Reg( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Sys_Cmd_User_Func( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		uint8_t Get_Device_Identity( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint16_t Bit_Index_Shift_Div( uint16_t value, uint8_t bit_shift )
		{ return ( ( value + ( ( 1U << bit_shift ) - 1U ) ) >> bit_shift ); }

		Modbus_DCI* m_dci;
		Modbus_SysCmd* m_syscmd;
		uint16_t m_buffer_size;
		BF_Misc::Com_Dog* m_com_watchdog;
		callback_list_item_t* m_user_defined_commands;
};

}

#endif	/* MODBUS_MSG_H */
