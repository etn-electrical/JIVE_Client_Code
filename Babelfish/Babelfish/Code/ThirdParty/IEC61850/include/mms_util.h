/*****************************************************************************/
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
 *  Declarations for MMS bit and string operations
 *
 *  This file should not require modification.
 */

#ifndef _MMS_UTIL_H
#define _MMS_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Bit String manipulation routines
 --- These functions operate on the internal ASN.1 representation
 --- of the bitString type.
 ------------------------------------------------------------
 */

TMW_CLIB_API boolean MMSd_getbit(unsigned char *field, int bitnum);

TMW_CLIB_API void MMSd_setbit(int value, unsigned char *field, int bitnum);

int MMSd_bitNegotiate(unsigned char *field1, unsigned char *field2, unsigned char *result, int length);

/*
 String manipulation routines
 --- These routines operate on character strings.  These can
 --- be removed on platforms which support these functions
 --- within the run-time evironment.
 ------------------------------------------------------------
 */

int MMSd_stringCompare(char *name, int length, char *nullTerminatedName);
int MMSd_strcmp(char *szString1, char *szString2);

#ifdef __cplusplus
};
#endif

#endif /* _MMS_UTIL_H */
