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
 *  Declarations for Console I/O.
 *
 *  This file should not require modification.
 */

#ifndef _TMW_CON_H
#define _TMW_CON_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(TRUE)
#define TRUE 1
#define FALSE 0
#endif

#define TMW_CON_MAX_CHARS_PER_LINE 120
#define TMW_CON_MAX_HISTORY 10

typedef struct {
    char line_text[TMW_CON_MAX_HISTORY][TMW_CON_MAX_CHARS_PER_LINE];
    char match[TMW_CON_MAX_CHARS_PER_LINE];
    int last_cmd;
    int last_scroll;
} TMW_CON_History;

void TMW_CON_history_init(TMW_CON_History *history);

int TMW_CON_fgets(char *prompt, char *buffer, int buffLen, int *position, TMW_CON_History *history, FILE *fd);

#ifdef __cplusplus
}
;
#endif

#endif /* _TMW_CON_H */
