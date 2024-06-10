// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "includes.h"
#include "IOT_Debug.h"
#include "Iot_Config.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/consolelogger.h"


// This ought to be long enough for the extended format part of the print statements
#define MAX_FORMAT_LINE 180

#if defined( __GNUC__ ) && ( !( defined NO_LOGGING ) )
__attribute__ ( ( format( printf, 6, 7 ) ) )
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void consolelogger_log( LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options,
						const char* format, ... )
{
#ifdef DEBUG_MANAGER

	// Ignore the False-Positive warning that tnow is never used; it is used, below.
	time_t tnow = get_time( NULL );
	struct tm* timeinfo = localtime( &tnow );
	if ( timeinfo == nullptr )
	{
		IOT_DEBUG_PRINT_FUNC( DBG_MSG_ERROR, func, "Failed to get local time" );
	}
	else
	{
		va_list args;
		va_start( args, format );

		char log_line[MAX_FORMAT_LINE];
		int char_cnt = 0;
		uint16_t msg_type;
		const char* func_name = func;

		// First show the preface
		switch ( log_category )
		{
			case AZ_LOG_INFO:
				// For Info, don't append CR+LF (so the Error messages stand out more)
				char_cnt = strftime( log_line, MAX_FORMAT_LINE, "[%H:%M:%S] Info: ", timeinfo );
				msg_type = DBG_MSG_INFO;
				options = 0;
				break;

			case AZ_LOG_ERROR:
				// I think that the file and line# don't add much, and we can grep for where the text comes from,
				// so reserving more line space for the error text instead
				// char_cnt = snprintf( log_line, MAX_FORMAT_LINE, "Error: [%.24s :%s:#%d] ", ctime(&tnow ), file, line
				// );
				char_cnt = strftime( log_line, MAX_FORMAT_LINE, "[%H:%M:%S] Error: ", timeinfo );
				msg_type = DBG_MSG_ERROR;
				break;

			default:
				// For Debug, don't waste space on the function name, and don't append CR+LF
				msg_type = DBG_MSG_DEBUG;
				func_name = "";
				options = 0;
				break;
		}

		int new_len = MAX_FORMAT_LINE - char_cnt;

		int return_len = vsnprintf( &log_line[char_cnt], new_len, format, args );
		va_end( args );
		if ( return_len > 0 )	/* Non-negative */
		{
			if ( ( strnlen( log_line, MAX_FORMAT_LINE ) < ( MAX_FORMAT_LINE - 4 ) ) && ( options & LOG_LINE ) )
			{
				// If enough room left, and wanted CR+LF, add it
				strncat( log_line, "\r\n", MAX_FORMAT_LINE - 1 );
			}
			log_line[MAX_FORMAT_LINE - 1] = 0;			// safe null terminator
			IOT_DEBUG_PRINT_FUNC( msg_type, func_name, log_line, args );
		}

	}
#endif
}
