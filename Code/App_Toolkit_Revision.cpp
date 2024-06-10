/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "App_Toolkit_Revision.h"
/*
 *****************************************************************************************
 *		Defines and Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Toolkit_Revision_Check( void )
{
#if ( CONST_LTK_APP_MAJOR != CONST_LTK_MAJOR || CONST_LTK_APP_MINOR != CONST_LTK_MINOR || \
	  CONST_LTK_APP_BUILD != CONST_LTK_BUILD )
	#pragma message("\n!--> APP Version = " CONST_APP_VER_ASCII)
	#pragma message("!--> RTK Version = " CONST_LTK_VER_ASCII)
	#error \
	"Application And Babelfish Release version are not matching. Please make sure that both repositories are in sync"
#endif
}
