/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module updates the application Toolkit revision number.
 *
 *	@details This module updates the application Toolkit revision number.
 *
 *	@version C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef APP_TOOLKIT_REVISION_H
#define APP_TOOLKIT_REVISION_H

#include "Toolkit_Revision.h"

/*
   Before every Release, version defined in \Babelfish\Code\Lib\Toolkit_Revision.h and in App_Toolkit_Revision.h should
      be ubdated. This file is added to cross check at compile time that release version of both repository(Application
      and Babelfish) are in sync.
 */

#define CONST_LTK_APP_MAJOR  2
#define CONST_LTK_APP_MINOR  19
#define CONST_LTK_APP_BUILD  3
#define APP_STRING APP

#define CONST_APP_VER_ASCII QUOTES( APP_STRING.CONST_LTK_APP_MAJOR.CONST_LTK_APP_MINOR.CONST_LTK_APP_BUILD )

void Toolkit_Revision_Check( void );

#endif
