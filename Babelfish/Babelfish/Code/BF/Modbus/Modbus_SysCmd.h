/**
 *****************************************************************************************
 *  @file
 *	@brief This file contains the modbus system command functions
 *
 *	@details Client(Modbus master) perform various system related service from server(slave)
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_SYSCMD_H
#define MODBUS_SYSCMD_H

#include "Modbus_Defines.h"
#include "System_Reset.h"
#include "Modbus_DCI.h"
#include "Bit.hpp"

/**
 ****************************************************************************************
 * @brief This is a Modbus_SysCmd class
 * @details It provides service to modbus system commands
 * It provides public methods to
 * @n @b 1. Execute Modbus diagnostic function code
 * @n @b 2. Execute exception status output in remote device
 * @n @b 3. Execute system command
 * @n @b 4. Set exception
 * @n @b 5. Clear exception
 * @n @b 6. Getting exception status
 ****************************************************************************************
 */
class Modbus_SysCmd
{
	public:
		/*
		 *****************************************************************************************
		 *		Typedefs
		 *****************************************************************************************
		 */
		/// Diagnostic Frames
		typedef struct
		{
			MB_FRAME_STRUCT* frame;

			BF_Lib::SPLIT_UINT16 diag_code;
			uint16_t data_len;
			uint8_t* data;
		} MB_DIAG_STRUCT;

		typedef struct
		{
			MB_FRAME_STRUCT* frame;

			uint16_t data_len;
			uint8_t* data;
		} MB_DIAG_RESP_STRUCT;

		/***********************************************************/
		/// Read Exception Frame
		typedef struct
		{
			MB_FRAME_STRUCT* frame;
		} MB_EXCEPT_STRUCT;

		typedef struct
		{
			MB_FRAME_STRUCT* frame;

			uint8_t exception;
		} MB_EXCEPT_RESP_STRUCT;

		// ***********************************************************/
		/// System Command Request Frame
		typedef struct
		{
			MB_FRAME_STRUCT* frame;

			uint8_t command;

			uint16_t data_len;
			uint8_t* data;
		} MB_SYS_CMD_REQ_STRUCT;

		typedef struct
		{
			MB_FRAME_STRUCT* frame;

			uint16_t data_len;
			uint8_t* data;
		} MB_SYS_CMD_RESP_STRUCT;

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus_SysCmd
		 *  @param[in] source_id: source id is an indication to source responsible to communication loss
		 *  @return this
		 */
		Modbus_SysCmd( DCI_SOURCE_ID_TD source_id );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_SysCmd goes out of scope
		 *  @return None
		 */
		~Modbus_SysCmd( void )
		{}

		/**
		 *  @brief Read diagnostics
		 *  @details Respond to modbus function code 08(series of tests for checking the communication system between
		 * client and server)
		 *  @param[in] diag_req: Requested Modbus diagnostic structure
		 *  @param[out] diag_resp: Responded Modbus diagnostic structure
		 *  @return 0U: MB_NO_ERROR_CODE
		 */
		uint8_t Read_Diagnostics( MB_DIAG_STRUCT* diag_req, MB_DIAG_RESP_STRUCT* diag_resp ) const;

		/**
		 *  @brief Read exceptions
		 *  @details Respond to modbus function code 07(Read exception status output in remote device)
		 *  @param[in] except_req: Requested Modbus exception structure
		 *  @param[out] except_resp: Responded Modbus exception structure
		 *  @retval 0U: MB_NO_ERROR_CODE
		 *  @retval 1U: MB_ILLEGAL_FUNCTION_ERROR_CODE
		 */
		uint8_t Read_Exceptions( MB_EXCEPT_STRUCT* except_req,
								 MB_EXCEPT_RESP_STRUCT* except_resp ) const;

		uint8_t Sys_Cmd_Request( MB_SYS_CMD_REQ_STRUCT* sys_cmd_req,
								 MB_SYS_CMD_RESP_STRUCT* sys_cmd_resp );

		/**
		 *  @brief setting exception
		 *  @details This fuction set the exception number as per request.
		 *  @param[in] exception_num: exception number to be set
		 *  @return None
		 */
		void Set_Exception( uint8_t exception_num )
		{
			BF_Lib::Bit::Set( m_exceptions, exception_num );
		}

		/**
		 *  @brief Clear exception
		 *  @details This fuction clear the exception number as per request.
		 *  @param[in] exception_num: exception number to be clear
		 *  @return None
		 */
		void Clr_Exception( uint8_t exception_num )
		{
			BF_Lib::Bit::Clr( m_exceptions, exception_num );
		}

		/**
		 *  @brief Get exception
		 *  @details This fuction shall provide confirmation of exception as per request.
		 *  @param[in] exception_num: exception number to be confirm
		 *  @retval 1: Exception truly exist
		 *  @retval 0: Exception does not exist
		 */
		bool Get_Exception( uint8_t exception_num )
		{
			return ( BF_Lib::Bit::Test( m_exceptions, exception_num ) );
		}

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_SysCmd( const Modbus_SysCmd& object );
		Modbus_SysCmd & operator =( const Modbus_SysCmd& object );

		uint8_t m_exceptions;
		DCI_SOURCE_ID_TD m_source_id;
};

#endif
