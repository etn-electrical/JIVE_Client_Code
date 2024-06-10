/* sfzclbase64.h

    Functions to convert to and from base64 format.
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

#ifndef SFZCLBASE64_H
#define SFZCLBASE64_H

/** Convert data from base64 format to binary.

    @return
    Returns xmallocated data buffer and length in buf_len.

 */
unsigned char *sfzcl_base64_to_buf(const unsigned char *str, size_t *buf_len);

/** Remove unneeded whitespace (everything that is not in base64!).

    If len is 0, use Strlen(str) to get length of data.

    @return
    Returns new xmallocated string containing the string.

 */
unsigned char *sfzcl_base64_remove_whitespace(const unsigned char *str,
                                              size_t len);

/** Removes headers/footers (and other crud) before and after the
    base64-encoded data.

    Will not modify the contents of str.

    @param str
    Pointer to the string.

    @param len
    The length of the string.

    @param start_ret
    Starting index of the base64 data.

    @param end_ret
    Ending index of the base64 data.

    @return
    Returns TRUE if successful. In case of an error, returns FALSE.

 */
bool sfzcl_base64_remove_headers(const unsigned char *str, size_t len,
                                 size_t *start_ret, size_t *end_ret);

#endif                          /* SFZCLBASE64_H */
