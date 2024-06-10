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
#ifndef DELTA_COM_H_
#define DELTA_COM_H_

#include "Includes.h"
#include "Delta_Com_Defines.h"

class Delta_Com
{
	public:

		typedef uint16_t param_id_t;
		typedef uint8_t attribute_t;
		typedef uint16_t length_t;
		typedef uint16_t count_t;

		static const uint8_t UNDEFINED_IDENTITY_DATA = 0xFF;
		static const uint16_t UNDEFINED_SUB_PROD_CODE = 0xFFFF;

		struct pid_to_dcid_map_t
		{
			param_id_t parameter_id;
			DCI_ID_TD dcid;
		};

		/**
		 * Delta Com Command Types
		 */
		typedef uint8_t cmd_type_t;
		static const cmd_type_t COMMAND = 0U;
		static const cmd_type_t ACK_REQUEST = 1U;
		static const cmd_type_t SUCCESSFUL_RESPONSE = 2U;
		static const cmd_type_t ERROR_COMMAND = 3U;
		static const cmd_type_t ATTENTION = 4U;
		static const cmd_type_t SERVER_OUT_OF_SYNC = 5U;
		static const cmd_type_t PARAMETER_MISMATCH = 6U;
		static const cmd_type_t COMMAND_TYPE_MAX = 7U;

		static const uint8_t COMMAND_TYPE_ENUMERATION_MASK = 0x70U;
		static const uint8_t COMMAND_TYPE_UMASK = 0x8FU;
		static const uint8_t COMMAND_TYPE_OFFSET = 4U;

		/**
		 * Delta Com Commands
		 */
		typedef uint8_t cmd_t;
		static const cmd_t SYSTEM_COMMAND = 0U;
		static const cmd_t GET_PARAMETER = 1U;
		static const cmd_t SET_PARAMETER = 2U;
		static const cmd_t EXCHANGE_MULTIPLE_PARAMS = 3U;
		static const cmd_t GET_MULTIPLE_ATTRIBUTES = 4U;
		static const cmd_t EXCHANGE_DELTA_PARAMETERS = 5U;
		static const cmd_t MANAGE_SUBSCRIPTIONS = 6U;
		static const cmd_t EXCHANGE_IDENTITY = 7U;

		static const uint8_t COMMAND_ENUMERATION_MASK = 0x0FU;
		static const uint8_t COMMAND_ENUMERATION_UMASK = 0xF0U;
		static const uint8_t COMMAND_ENUMERATION_OFFSET = 0U;

		/**
		 * Delta Com System Commands
		 */
		typedef uint8_t sys_cmd_t;
		static const sys_cmd_t SOFT_RESET = 0U;
		static const sys_cmd_t FACTORY_RESET = 1U;
		static const sys_cmd_t WATCHDOG_RESET = 2U;
		static const sys_cmd_t COMMUNICATION_LOSS = 3U;
		static const sys_cmd_t RUN_BOOTLOADER_BEHAVIOR = 4U;
		static const sys_cmd_t UNLOCK_MANUFACTURING_ACCESS = 5U;
		static const sys_cmd_t CHECK_FOR_SLAVE_DEVICE = 6U;
		static const sys_cmd_t RE_INITIALIZE = 7U;
		static const sys_cmd_t LED_TEST = 8U;
		static const sys_cmd_t CHANGE_DELTA_COM_TIMEOUT = 9U;
		static const sys_cmd_t UNLOCK_FULL_ACCESS = 10U;
		static const sys_cmd_t MAX_SYSTEM_COMMAND = 11U;


		static const uint8_t FRAGMENTATION_MASK = 0x80U;

		static bool Is_Fragmented( uint8_t command_byte );

		static uint8_t Set_Fragmented( uint8_t command_byte );


		/**
		 *  @brief Clears the command byte of anything.  Sets it to zero.
		 *	@param command_byte the command byte to clear
		 *  @return The resultant cleared byte.
		 */
		static uint8_t Clear_Command_Byte( uint8_t command_byte );

		/**
		 *  @brief Returns the command type from a command byte.  The command type is stored in
		 *  the COMMAND_TYPE_ENUMERATION_MASK portion of the byte.
		 *	@param command_byte the command byte
		 *  @return the command type
		 */
		static cmd_type_t Get_Command_Type( uint8_t command_byte );

		/**
		 *  @brief Returns the command from a command byte.  The command  is stored in
		 *  the COMMAND_ENUMERATION_MASK portion of the byte.
		 *	@param command_byte the command byte
		 *  @return the command
		 */
		static cmd_t Get_Command( uint8_t command_byte );

		/**
		 *  @brief This will set the command type in the byte pointed to at current_value.  It will
		 *  preserve all other bits in the byte
		 *  @param command_byte: The present command byte.
		 *	@param command_type: The command type to be set into the current value at the appropriate offset
		 *  @return The command byte with the command type added in.
		 */
		static uint8_t Set_Command_Type( uint8_t command_byte, cmd_type_t command_type );

		/**
		 *  @brief This will set the command in the byte pointed to at current_value.  It will
		 *  preserve all other bits in the byte
		 *  @param command_byte: The command byte to modify.
		 *	@param command The command to be set into the current value at the appropriate offset
		 *	@return The modified command with the new command in it.  Leaving the type alone.
		 */
		static uint8_t Set_Command( uint8_t command_byte, cmd_t command );

		static const uint8_t INITIAL_SYNC_KEY = 0xFF;

		static const param_id_t UNDEFINED_PARAMETER =
			static_cast<param_id_t>( ~static_cast<param_id_t>( 0U ) );

	private:
		Delta_Com();
		virtual ~Delta_Com();

		static const uint8_t major_version = 0U;
		static const uint8_t minor_version = 0U;
		static const uint8_t revision = 1;
};

#endif	/* DELTA_COM_H_ */
