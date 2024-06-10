/* sfzcldsprintf.h
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

#ifndef SFZCLDSPRINTF_H
#define SFZCLDSPRINTF_H

/* This function is similar to Snprintf (indeed, this function, too,
   uses vsnprintf()); it takes a format argument which specifies the
   subsequent arguments, and writes them to a string using the
   format-string. This function differs from snprintf in that this
   allocates the buffer itself, and returns a pointer to the allocated
   string (in str). This function never fails.  (if there is not
   enough memory, sfzcl_xrealloc() calls sfzcl_fatal())

   The returned string must be freed by the caller. Returns the number
   of characters written.  */
int sfzcl_dsprintf(char **str, const char *format, ...);
int sfzcl_dvsprintf(char **str, const char *format, va_list ap);

#endif                          /* SFZCLDSPRINTF_H */
