/* sfzclenum.h

   Functions for mapping keywords to numbers and vice versa.
 */

/*****************************************************************************
* Copyright (c) 2006-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef SFZCLENUM_H
#define SFZCLENUM_H

/* Array of keyword - numeric value pairs.  The array is terminated by
   an entry with NULL name. */
typedef struct
{
    const char *name;
    long code;
} *SfzclKeyword, SfzclKeywordStruct;

/* Finds the name of a keyword corresponding to the numeric value.
   Returns a pointer to a constant name string, or NULL if there is no
   keyword matching the numeric value. */
const char *sfzcl_find_keyword_name(const SfzclKeywordStruct *keywords,
                                    long code);

#endif                          /* SFZCLENUM_H */
