/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "httpd_common.h"
#include "INT_ASCII_Conversion.h"

/******************************************************************************/
// similar to standard C strncmp(), only change all letters to lower case
// returns 1 if str1>str2, -1 if str1<str2, or 0 if str1==str2
SINT8 caseInsensitiveStrNCmp( const uint8_t* str1, const uint8_t* str2, size_t num )
{
	size_t i;
	SINT8 return_val = 0;
	bool_t loop_break = false;

	for ( i = 0U; ( ( i < num ) && ( false == loop_break ) ); i++ )
	{
		if ( str1[i] == 0U )
		{
			if ( str2[i] > 0U )
			{
				return_val = -1;// str1 < str2
				loop_break = true;
			}
			else
			{
				return_val = 0;	// str1 == str2
				loop_break = true;
			}
		}
		else if ( str2[i] == 0U )
		{
			// assuming str1[i] != 0
			return_val = 1;	// str1 > str2
			loop_break = true;
		}
		else
		{
			if ( TO_LOWER( str1[i] ) > TO_LOWER( str2[i] ) )
			{
				return_val = 1;	// str1 > str2
				loop_break = true;
			}
			else if ( TO_LOWER( str1[i] ) < TO_LOWER( str2[i] ) )
			{
				return_val = -1;// str1 < str2
				loop_break = true;
			}
			else
			{
				// MISRA Suppress
			}
		}
	}
	return ( return_val );	// compared all "num" chars and no difference found
}

/******************************************************************************/

