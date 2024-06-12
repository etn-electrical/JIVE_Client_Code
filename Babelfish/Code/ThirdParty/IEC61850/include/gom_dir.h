//*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Declarations for GOMDIR.
 *
 *  Generic Object Models for Substation and Feeder Equipment Directories are built from
 *  the xml files in sclclasses along with any user additions.
 *
 *  Version 11 requires that each dictionary is added with a call to TMW_GOMSFE_addDirectory()
 *  rather than static compiling.
 *
 *  This file should not require modification.
 */
#ifndef _GOM_DIR_H
#define _GOM_DIR_H

#ifdef __cplusplus
extern "C" {
#endif


extern MMSd_RTG_Gomsfe_Release *MMSd_GOMSFE_Directory[];

extern int MMSd_GOMSFE_Directory_Size;

TMW_CLIB_API int TMW_GOMSFE_addDirectory( const MMSd_RTG_Gomsfe_Release * newDirectory);

#ifdef __cplusplus
};
#endif

#endif /*  _GOM_DIR_H */
