/**
 **********************************************************************************************
 * @file            Debuf_Manager_Terminal_IO.cpp C++ Implementation File for Debug Manager
 * 					Terminal IO interface
 *
 * @brief           The file contains all APIs required for initializing and transmitting debug
 * 					message on Terminal IO interface.
 *
 * @details         See header file for module overview.
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include <stdio.h>
#include "Includes.h"
#include "Debug_Interface_Term_IO.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Interface_Term_IO::Debug_Interface_Term_IO()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Interface_Term_IO::~Debug_Interface_Term_IO()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_Term_IO::Init( void )
{
	// Nothing to initialize
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_Term_IO::Send( uint8_t* buffer, uint32_t bytes_to_print )
{
	printf( "%.*s", bytes_to_print, buffer );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Interface_Term_IO::Send_Now( uint8_t* buffer, uint32_t length )
{
	printf( "%.*s", length, buffer );
}
