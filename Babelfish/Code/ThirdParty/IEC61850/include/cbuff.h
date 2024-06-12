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
 *  Declarations for circular buffers.
 *
 *  This file should not require modification.
 */

#ifndef _CBUFF_H
#define _CBUFF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long CircBuffIndex;
typedef unsigned long CircBuffLen;
typedef unsigned char CircBuffPass;

/*
 * ***************************************************************************
 */
typedef struct CircBuff {
    CircBuffIndex head;
    CircBuffIndex tail;
    CircBuffPass headPass;
    CircBuffPass tailPass;
    unsigned char *buffer;
    unsigned long buffLen;
} CircBuff;

/*
 * ***************************************************************************
 */
typedef enum {
    CircBuffFetchOctets,
    CircBuffFetchString
} CircBuffFetchType;

/*
 * ***************************************************************************
 */
typedef enum {
    CircBuffForward,
    CircBuffBackward
} CircBuffMotion;

typedef struct CircBuffData {
    unsigned char *buff;
    CircBuffIndex index;
    CircBuffPass pass;
    CircBuffLen storeLen;
    CircBuffLen maxLen;
    CircBuffFetchType fetchType;
} CircBuffData;

/*
 * ***************************************************************************
 */
void init_CircBuff(CircBuff *circBuff, unsigned char *buffer,
                   unsigned long buffLen);

/*
 * ***************************************************************************
 */
int isEmptyCircBuff(CircBuff *circBuff);

/*
 * ***************************************************************************
 */
CircBuffLen removeCircBuff(CircBuff *circBuff);

/*
 * ***************************************************************************
 */
TMW_CLIB_API int oldestCircBuff(CircBuff *circBuff, CircBuffIndex *index, CircBuffPass *pass);

/*
 * ***************************************************************************
 */
TMW_CLIB_API int newestCircBuff(CircBuff *circBuff, CircBuffIndex *index, CircBuffPass *pass);

/*
 * ***************************************************************************
 */
int mputCircBuff(CircBuff *circBuff, CircBuffData *itemList, int numItems);

/*
 * ***************************************************************************
 */
int mgetCircBuff(CircBuff *circBuff, CircBuffIndex *tail,
                 CircBuffPass *tailPass, CircBuffData *itemList, int numItems);

/*
 * ***************************************************************************
 */
CircBuffLen fetchCircBuff(CircBuff *circBuff, CircBuffFetchType fetchType,
                          CircBuffIndex *index, CircBuffPass *pass,
                          unsigned char *data, CircBuffLen fetchLen);

/*
 * ***************************************************************************
 */
CircBuffLen advanceCircBuff(CircBuff *circBuff, CircBuffLen nbytes,
                            CircBuffIndex *tail, CircBuffPass *pass);

/*
 * ***************************************************************************
 */
CircBuffLen backupCircBuff(CircBuff *circBuff, CircBuffLen nbytes,
                           CircBuffIndex *tail, CircBuffPass *pass);

/*
 * ***************************************************************************
 */
int moveBackCircBuff(CircBuff *circBuff, CircBuffIndex *retIndex,
                     CircBuffPass *retPass);

/*
 * ***************************************************************************
 */
int moveForwardCircBuff(CircBuff *circBuff, CircBuffIndex *retIndex,
                        CircBuffPass *retPass);

#ifdef __cplusplus
}
;
#endif

#endif  /* _CBUFF_H */
