/**
 *      @file    http.c
 *
 *
 *      Simple INCOMPLETE HTTP parser for example applications.
 */
/*
 *      Copyright (c) 2013-2017 INSIDE Secure Corporation
 *      All Rights Reserved
 *
 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF INSIDE.
 *
 *      Please do not edit this file without first consulting INSIDE support.
 *      Unauthorized changes to this file are not supported by INSIDE.
 *
 *      The copyright notice above does not evidence any actual or intended
 *      publication of such source code.
 *
 *      This Module contains Proprietary Information of INSIDE and should be
 *      treated as Confidential.
 *
 *      The information in this file is provided for the exclusive use of the
 *      licensees of INSIDE. Such users have the right to use, modify,
 *      and incorporate this code into products for purposes authorized by the
 *      license agreement provided they include this notice and the associated
 *      copyright notice with any such product.
 *
 *      The information in this file is provided "AS IS" without warranty.
 */
/******************************************************************************/

#include "app.h"
#include "core/psUtil.h"

/* #define TEST */

/******************************************************************************/
/*
    EXAMPLE ONLY - SHOULD NOT BE USED FOR PRODUCTION CODE

    Process an HTTP request from a client.
    Very simple - we just print it, and return success.
    No HTTP validation at all is done on the data.
 */
int32 httpBasicParse(httpConn_t *cp, unsigned char *buf, uint32 len,
    int32 trace)
{
    unsigned char *c, *end, *tmp;
    int32 l;

    /*
        SSL/TLS can provide zero length records, which we just ignore here
        because the code below assumes we have at least one byte
     */
    if (len == 0)
    {
        return HTTPS_PARTIAL;
    }

    c = buf;
    end = c + len;
/*
    If we have an existing partial HTTP buffer, append to it the data in buf
    up to the first newline, or 'len' data, if no newline is in buf.
 */
    if (cp->parsebuf != NULL)
    {
        for (tmp = c; c < end && *c != '\n'; c++)
        {
            ;
        }
        /* We want c to point to 'end' or to the byte after \r\n */
        if (*c == '\n')
        {
            c++;
        }
        l = (int32) (c - tmp);
        if (l > HTTPS_BUFFER_MAX)
        {
            return HTTPS_ERROR;
        }
        cp->parsebuf = (unsigned char *)Realloc(cp->parsebuf,
                                                l + cp->parsebuflen);
        Memcpy(cp->parsebuf + cp->parsebuflen, tmp, l);
        cp->parsebuflen += l;
        /* Parse the data out of the saved buffer first */
        c = cp->parsebuf;
        end = c + cp->parsebuflen;
        /* We've "moved" some data from buf into parsebuf, so account for it */
        buf += l;
        len -= l;
    }

L_PARSE_LINE:
    for (tmp = c; c < end && *c != '\n'; c++)
    {
        ;
    }
    if (c < end)
    {
        if (*(c - 1) != '\r')
        {
            return HTTPS_ERROR;
        }
        /* If the \r\n started the line, we're done reading headers */
        if (*tmp == '\r' && (tmp + 1 == c))
        {
/*
            if ((c + 1) != end) {
                psTrace("HTTP data parsing not supported, ignoring.\n");
            }
 */
            if (cp->parsebuf != NULL)
            {
                Free(cp->parsebuf); cp->parsebuf = NULL;
                cp->parsebuflen = 0;
                if (len != 0)
                {
                    psTrace("HTTP data parsing not supported, ignoring.\n");
                }
            }
            if (trace)
            {
                psTrace("RECV COMPLETE HTTP MESSAGE\n");
            }
            return HTTPS_COMPLETE;
        }
    }
    else
    {
        /* If parsebuf is non-null, we have already saved it */
        if (cp->parsebuf == NULL && (l = (int32) (end - tmp)) > 0)
        {
            cp->parsebuflen = l;
            cp->parsebuf = (unsigned char *)Malloc(cp->parsebuflen);
            psAssert(cp->parsebuf != NULL);
            Memcpy(cp->parsebuf, tmp, cp->parsebuflen);
        }
        return HTTPS_PARTIAL;
    }
    *(c - 1) = '\0';    /* Replace \r with \0 just for printing */
    if (trace)
    {
        psTraceStr("RECV PARSED: [%s]\n", (char *) tmp);
    }
    /* Finished parsing the saved buffer, now start parsing from incoming buf */
    if (cp->parsebuf != NULL)
    {
        Free(cp->parsebuf); cp->parsebuf = NULL;
        cp->parsebuflen = 0;
        c = buf;
        end = c + len;
    }
    else
    {
        c++;    /* point c to the next char after \r\n */
    }
    goto L_PARSE_LINE;

    /* This line if unreachable. */
}

/******************************************************************************/
