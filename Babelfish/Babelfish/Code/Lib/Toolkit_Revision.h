/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module updates the Toolkit revision number.
 *
 *	@details This module updates the Toolkit revision number.
 *
 *	@version C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TOOLKIT_REVISION_H
#define TOOLKIT_REVISION_H

#include "Includes.h"
#include "DCI_Owner.h"

#define QUOTES( s ) STR( s )
#define STR( s )  #s

/* Don't add any suffix(like U) to numeric values, as It will also get added in ASCII version string */
#define CONST_LTK_MAJOR  2
#define CONST_LTK_MINOR  19
#define CONST_LTK_BUILD  3
#define LTK_STRING RTK

#define CONST_LTK_VER_ASCII QUOTES( LTK_STRING.CONST_LTK_MAJOR.CONST_LTK_MINOR.CONST_LTK_BUILD )

// MAJOR.MINOR.BUILD
// LTK_REV "2.11.1"
// LTK_REV_ASCII "RTK.2.11.1"

/**
 ****************************************************************************************
 *	@brief This module will update the toolkit revision to the revision dci parameters.
 *
 *	@details This module will update the toolkit revision to the revision dci parameters. This must not be modified in
 * the application.
 *
 ****************************************************************************************
 */

class Toolkit_Revision
{
	public:
		Toolkit_Revision();
		~Toolkit_Revision();

	private:
};

#endif
