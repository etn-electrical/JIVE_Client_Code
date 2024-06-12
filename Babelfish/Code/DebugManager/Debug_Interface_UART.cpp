/**
 **********************************************************************************************
 * @file            Debuf_Manager_UART.cpp C++ Implementation File for Debug Manager UART interface
 *
 * @brief           The file contains all APIs required for initializing and transmitting debug
 * 					message on serial (UART) interface.
 *
 * @details         See header file for module overview.
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "Babelfish_Assert.h"
#include "Debug_Interface_UART.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Interface_UART::Debug_Interface_UART() :
	m_shell_ptr( nullptr )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Interface_UART::~Debug_Interface_UART()
{
	m_shell_ptr = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_UART::Init( void )
{
	// Get Shell pointer to start communication on UART port
	m_shell_ptr = Shell::Command_Line_Editor::Get_Shell_Ptr();
	BF_ASSERT( m_shell_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_UART::Send( uint8_t* buffer, uint32_t length )
{
	m_shell_ptr->Put_String( reinterpret_cast<char_t*>( buffer ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_UART::Send_Now( uint8_t* buffer, uint32_t length )
{
	// Bypass other interface functionality and transmit data on interface
	m_shell_ptr->Put_String_Now( reinterpret_cast<char_t*>( buffer ), length );
}
