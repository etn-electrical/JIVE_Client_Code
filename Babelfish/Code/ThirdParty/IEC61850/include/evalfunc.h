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
 *  This file contains declarations for default Evaluation Functions
 *
 *  for the basic MMS datatypes  from sclclasses/ed2primitives.xml.
 *
 *  Evaluation Functions are called by the reporting mechanisms to determine
 *  whether data has changed.
 *
 */

#ifndef _EVALFUNC_H
#define _EVALFUNC_H

#ifdef __cplusplus
extern "C" {
#endif


TMW_CLIB_API int MMSd_evalBoolean(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalBitString(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalInteger(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalLongInteger(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalLong64Integer(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalUnsigned(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalLongUnsigned(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalFloat(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalFloat64(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalOctetString(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalString(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalGeneralTime(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalBtime(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalUtcBtime(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalBCD(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalObjectId(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalpf(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalQuality(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalCounter(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalLongCounter(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

TMW_CLIB_API int MMSd_evalCounterUtcBtime(void *val, int siz, void *save, void *db, void *scale, unsigned char *reason);

#ifdef __cplusplus
}
;
#endif

#endif          /* _EVALFUNC_H */
