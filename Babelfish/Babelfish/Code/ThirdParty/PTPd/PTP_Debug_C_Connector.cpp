/**
 *****************************************************************************************
 * @file        PTP_Debug_C_Connector.cpp
 * @details     This file shall includes all the definition of C/C++ bridge functions
                created for BF_DEBUG_PRINT and BF_ASSERT macro.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include <stdarg.h>
#include <stdio.h>
#include "PTP_Debug_C_Connector.h"
#include "PTP_Debug.h"
#include "Babelfish_Debug.h"
#include "Debug_Manager.h"

#define DBG_BUF_LEN  256U

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_Debug_Print( unsigned char msg_type, const char* func, const char* format, ... )
{
	va_list args;
	char buf[DBG_BUF_LEN];

	va_start( args, format );
	vsnprintf( buf, sizeof( buf ), format, args );
	va_end( args );

	if ( msg_type == ERROR_MSG )
	{
		PTP_DEBUG_PRINT_FUNC( DBG_MSG_ERROR, func, buf );
	}
	else if ( msg_type == INFO_MSG )
	{
		PTP_DEBUG_PRINT_FUNC( DBG_MSG_INFO, func, buf );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PTP_BF_Assert( const unsigned int line_num, const char* func_name )
{
	Debug_Manager::Debug_Assert( line_num, func_name );
}
