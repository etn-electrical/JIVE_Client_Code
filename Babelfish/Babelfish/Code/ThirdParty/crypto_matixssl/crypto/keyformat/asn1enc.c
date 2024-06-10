/**
 *      @file    asn1enc.c
 *
 *
 *      ASN.1 encoding for key and cert file writing.
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

#include "../cryptoImpl.h"

#if defined(USE_CERT_GEN) || defined(USE_CMS)
#include "osdep_limits.h"

/******************************************************************************/
/*
    Just the raw number of bytes needed to hold an integer value
 */
static int32 bytesToHoldInt(uint32 val)
{
    int32 bytes = 0;

    /* Check if the value is 0.  Still need one byte to hold that */
    if (val == 0)
    {
        return 1;
    }
    while (val)
    {
        val = val >> 8;
        bytes++;
    }
    return bytes;
}

/******************************************************************************/
/*
    The ASN number of bytes needed to hold a length specifier
 */
int32 asnBytesToHold(int32 val)
{
    int32 bytes;

    bytes = bytesToHoldInt(val);
    if (val > 127)
    {
        bytes++;
    }
    return bytes;
}

/******************************************************************************/
/*
    The L part of TLV.  The size param is how many bytes long the Value is.
    Return how many bytes were written to 'out'
 */
int32 asnHelpWriteLength(unsigned char *out, int32 size)
{
    int32 tmp, ret, bytes;

    ret = 0;
    bytes = bytesToHoldInt(size);
/*
    If the high bit is set, the lower 7 bits represent the number of
    bytes that follow and represent length
    If the high bit is not set, the lower 7 represent the actual length
 */
    if (bytes == 1)
    {
/*
        The corner case is a single byte value greater than 127.  Need to
        move that value to the next byte.
 */
        if (size > 127)
        {
            *out = 0x81;
        }
        else
        {
            /* Common case - a small single byte length.  Assign and leave */
            *out = size;
            out++; ret++;
            return 1;
        }
    }
    else if (bytes == 0)
    {
        /* Just trying to write a 0 */
        *out = 0x1;
        out++; ret++;
        return 1;
    }
    else
    {
        /* Multi-byte length */
        *out = bytes | 0x80;
    }
    out++; ret++;
    tmp = size;
    while (bytes)
    {
        *out = (tmp >> (8 * (bytes - 1)) & 0xFF);
        out++; ret++;
        bytes--;
    }
    return ret;
}

/******************************************************************************/
/*
    Return number of overhead bytes to write out the TL portion for a
    given data length.  Assumes standard tag such as OCTET_STRING or
    OID
 */
int32 asn1TLOverhead(int32 dataLen)
{
    return asnBytesToHold(dataLen) + 1; /* 1 is standard tag */
}

/******************************************************************************/
/*
    Returns how many bytes were written.  Moves out
 */
int32 asnWriteOidChars(unsigned char *out, unsigned char *oid, int32 oidLen)
{
    int32 i, bytes;

    *out = ASN_OID; out++;
    bytes = asnHelpWriteLength(out, oidLen);
    out += bytes;
    for (i = 0; i < oidLen; i++)
    {
        *out = oid[i];
        out++; bytes++;
    }
    return bytes + 1; /* +1 for ASN_OID at start */
}

/* Returns how many bytes were written.  Moves out */
int32 asn1WriteOctetString(unsigned char *out, unsigned char *data, int32 len)
{
    int32 bytes;

    *out = ASN_OCTET_STRING; out++;
    bytes = asnHelpWriteLength(out, len);
    out += bytes;
    Memcpy(out, data, len);
    return bytes + len + 1; /* +1 For the OCTET_STRING at the start */
}

/**  Write tag and length.

     Write tag and length to an already allocated and intiailized buffer.
     Try our best to avoid buffer overflow: check that the buffer has enough
     room for the tag, length and content octets. Advances the outBuf->end
     pointer to point to the first byte after the final length octet.

     @param[out] outBuf Buffer where to write the tag and the length octets.
     @param[in] tag The value of the tag to write
     @param[in] numContentOctets Number of contents octets needed for this
     ASN.1 type value.
     @return PS_ARG_FAIL Not enough room in outBuf.
     @return PS_LIMIT_FAIL Number of content octets is too large and cannot
     be supported.
     @return >0 Number of bytes written to outBuf.
*/
static int32 asn_write_tag_and_len(psBuf_t *outBuf,
        unsigned char tag,
        psSize32_t numContentOctets)
{
    psSize32_t spaceLeft = outBuf->size - (outBuf->end - outBuf->start);
    psSize32_t numLengthOctets;
    psSize32_t maxPlusOne;

    if (spaceLeft < 2)
    {
        return PS_ARG_FAIL;
    }

    /*
      If numContentOctets > 127, we have to use the long form of
      length encoding: first length octet encodes the number of
      following length octets and the following length octets encode
      the number of content octets, using base-256 encoding.
    */
    numLengthOctets = 1;
    if (numContentOctets > 127)
    {
        numLengthOctets = 2;
        maxPlusOne = 256; /* Upper bound for the length that can be
                             encoded with numLengthOctets octets. */
        while (numContentOctets > maxPlusOne)
        {
            if (maxPlusOne > PS_SIZE32_MAX/256)
            {
                return PS_LIMIT_FAIL;
            }
            maxPlusOne *= 256;
            numLengthOctets++;
            /*
              Elsewhere in ASN.1 encoding code, we assume that the max number
              of length octets is ASN_MAX_NUM_LENGTH_OCTETS. Enforce that here.
            */
            if (numLengthOctets > ASN_MAX_NUM_LENGTH_OCTETS)
            {
                return PS_LIMIT_FAIL;
            }
        }
    }

    if (spaceLeft < (1 + numLengthOctets + numContentOctets))
    {
        return PS_ARG_FAIL;
    }

    /* Have enough room. Now write out the TL encoding and advance
       the buf->end pointer to point to where the first V octet
       should be written. */
    *outBuf->end = tag;
    outBuf->end++;
    outBuf->end += asnHelpWriteLength(outBuf->end, numContentOctets);

    /* Return number of bytes written to outBuf. */
    return (outBuf->end - outBuf->start);
}

/** Write bits to outBuf as a BIT STRING using DER encoding. */
int32 psAsnWriteBitStringDER(psPool_t *pool,
        psBuf_t **outBuf,
        unsigned char *bits,
        psSize_t bitsLen,
        psSize_t numIgnoreBits,
        uint32_t optionFlags)
{
    psBuf_t *buf;
    int32 len, numWritten;
    int32 numContentOctets;
    unsigned char ignoreBitsOctet;
    int32 rc;

    /* Validate args. */
    if ((!(optionFlags & PS_ASN_ENCODE_SKIP_CONTENT_OCTETS))
        && bitsLen > 0 && bits == NULL)
    {
        return PS_ARG_FAIL;
    }
    if (numIgnoreBits > 7)
    {
        return PS_ARG_FAIL;
    }
    ignoreBitsOctet = (unsigned char)numIgnoreBits;

    /* Compute length. */
    numContentOctets = bitsLen + 1; /* +1 for the ignore bits byte. */
    len = ASN_BIT_STRING_OH + numContentOctets;

    /* Allocate and init buf. */
    buf = psMalloc(pool, sizeof(psBuf_t));
    if (buf == NULL)
    {
        return PS_MEM_FAIL;
    }
    if (psBufInit(pool, buf, len) == NULL)
    {
        psFree(buf, pool);
        return PS_MEM_FAIL;
    }

    /* Write out the encoding. */

    rc = asn_write_tag_and_len(buf, ASN_BIT_STRING, numContentOctets);
    if (rc < 0)
    {
        psBufUninit(pool, buf);
        psFree(buf, pool);
        return rc;
    }
    *buf->end = ignoreBitsOctet;
    buf->end++;

    if (!(optionFlags & PS_ASN_ENCODE_SKIP_CONTENT_OCTETS))
    {
        Memcpy(buf->end, bits, bitsLen);
        buf->end += bitsLen;
    }

    numWritten = (int32)(buf->end - buf->start);

    /* for coverity */
    if (numWritten >= 0)
        *outBuf = buf;
    else
        psFree(buf, pool);

    return numWritten;
}

#endif /* defined(USE_CERT_GEN) || defined(USE_CMS) */

/******************************************************************************/

#ifdef USE_CERT_GEN

static int32_t der_encode_integer(psPool_t *pool, const pstm_int *num,
                                  unsigned char *out, psSize_t *outlen);

/******************************************************************************/
/*
    ASN_INTEGER writer for bigs
 */
int32_t asnWriteBig(psPool_t *pool, psBuf_t *out, const pstm_int *val)
{
    psSize_t valLen;

    der_length_integer(val, &valLen);
    if (der_encode_integer(pool, val, out->end, &valLen) != 0)
    {
        return -1;
    }
    out->end += valLen;
    return 0;
}

/******************************************************************************/
/*
    Note:  Not a true ASN_INTEGER writer.  Not doing negative numbers here.
 */
int32 asnWriteInteger(psBuf_t *out, int32 value)
{
    int32 valLen;

    if (enoughAsnRoom(out, bytesToHoldInt(value)) < 0)
    {
        return -1;
    }
    *out->end = ASN_INTEGER; out->end++;
    valLen = asnHelpWriteLength(out->end, bytesToHoldInt(value));
    out->end += valLen;

    while (valLen)
    {
        *out->end = (value >> (8 * (valLen - 1)) & 0xFF);
        out->end++;
        valLen--;
    }
    return 0;
}

int32 asnWriteOid(psBuf_t *out, int32 oid[], int32 oidLen)
{
    int32 i;

    if (enoughAsnRoom(out, oidLen) < 0)
    {
        return -1;
    }

    *out->end = ASN_OID; out->end++;
    out->end += asnHelpWriteLength(out->end, oidLen);
    for (i = 0; i < oidLen; i++)
    {
        *out->end = oid[i];
        out->end++;
    }
    return 0;
}



/******************************************************************************/
/*
    ASN_SEQUENCE writer
 */
int32 asnWriteSequence(psBuf_t *out, int32 size)
{
    if (enoughAsnRoom(out, size) < 0)
    {
        return -1;
    }

    *out->end = ASN_CONSTRUCTED | ASN_SEQUENCE; out->end++;
    out->end += asnHelpWriteLength(out->end, size);
    return 0;
}

/******************************************************************************/
/*
    ASN_SET writer
 */
int32 asnWriteSet(psBuf_t *out, int32 size)
{
    if (enoughAsnRoom(out, size) < 0)
    {
        return -1;
    }

    *out->end = ASN_CONSTRUCTED | ASN_SET; out->end++;
    out->end += asnHelpWriteLength(out->end, size);
    return 0;
}

/******************************************************************************/
/*
    Determines if enough room in buffer to hold a length of 'size'
 */
int32 enoughAsnRoom(psBuf_t *out, int32 size)
{
    int32 bytes;

/*
    Big enough to hold? Need 1 for tag plus
    number of bytes to hold the length and then the actual value itself
 */
    bytes = asnBytesToHold(size);

    /* 1 is T, bytes is L, and size is V */
    if (((out->buf + out->size) - out->end) < (1 + bytes + size))
    {
        return -1;
    }
    return 0;
}

/******************************************************************************/
/*
    Room has been skipped at the start of an psBuf_t so that the
    ASN_SEQUENCE bit can be written.  This function manipulates that
    buffer to prepend it and returns the fixed up buffer
 */
int32 asnPrependSequence(psBuf_t *buf)
{
    int32 seqLen, tmp;
    unsigned char *realEnd;

    seqLen = (int32) (buf->end - buf->start);
    tmp = asnBytesToHold(seqLen) + 1;
    psAssert((buf->start - buf->buf) > tmp);
    buf->start -= tmp;
    realEnd = buf->end;
    buf->end = buf->start;
    if (asnWriteSequence(buf, seqLen) < 0)
    {
        return -1;
    }
    buf->end = realEnd;
    return 0;
}

/******************************************************************************/
/*
    Same as above routine, but writes an ASN_SET identifier
 */
int32 asnPrependSet(psBuf_t *buf)
{
    int32 setLen, tmp;
    unsigned char *realEnd;

    setLen = (int32) (buf->end - buf->start);
    tmp = asnBytesToHold(setLen) + 1;
    psAssert((buf->start - buf->buf) > tmp);
    buf->start -= tmp;
    realEnd = buf->end;
    buf->end = buf->start;
    asnWriteSet(buf, setLen);
    buf->end = realEnd;
    return 0;
}

/******************************************************************************/
/*
    Gets length of DER encoding of num including the INTEGER tag
    and length bytes
 */
int32_t der_length_integer(const pstm_int *num, psSize_t *outlen)
{
    unsigned long z, len;
    int32 leading_zero;

    psAssert(num != NULL);
    psAssert(outlen != NULL);

    if (pstm_cmp_d(num, 0) != PSTM_LT)
    {
        /* positive */
        /* we only need a leading zero if the msb of the first byte is one */
        if ((pstm_count_bits(num) & 7) == 0 || pstm_iszero(num) == PSTM_YES)
        {
            leading_zero = 1;
        }
        else
        {
            leading_zero = 0;
        }

        /* size for bignum */
        z = len = leading_zero + pstm_unsigned_bin_size(num);
    }
    else
    {
        /* it's negative */
        leading_zero = 0;
        z = pstm_count_bits(num);
        z = z + (8 - (z & 7));
        len = z = z >> 3;
    }

    /* now we need a length */
    if (z < 128)
    {
        /* short form */
        ++len;
    }
    else
    {
        /* long form (relies on z != 0), assumes length bytes < 128 */
        ++len;

        while (z)
        {
            ++len;
            z >>= 8;
        }
    }

    /* we need a 0x02 to indicate it's INTEGER */
    ++len;

    /* return length */
    *outlen = (int32) len;
    return PS_SUCCESS;
}

/******************************************************************************/
/*
    Exports a positive bignum as DER format (upto 2^32 bytes in size)
 */
static int32_t der_encode_integer(psPool_t *pool, const pstm_int *num,
    unsigned char *out, psSize_t *outlen)
{
    unsigned long y;
    int32 err, leading_zero;
    psSize_t tmplen;

    psAssert(num != NULL);
    psAssert(out != NULL);
    psAssert(outlen != NULL);

    /* find out how big this will be */
    if ((err = der_length_integer(num, &tmplen)) != PS_SUCCESS)
    {
        return err;
    }
    if (*outlen < tmplen)
    {
        return PS_LIMIT_FAIL;
    }
    if (pstm_cmp_d(num, 0) != PSTM_LT)
    {
        /* we only need a leading zero if the msb of the first byte is one */
        if ((pstm_count_bits(num) & 7) == 0 || pstm_iszero(num) == PSTM_YES)
        {
            leading_zero = 1;
        }
        else
        {
            leading_zero = 0;
        }

        /* get length of num in bytes */
        y = pstm_unsigned_bin_size(num) + leading_zero;
    }
    else
    {
        leading_zero = 0;
        y = pstm_count_bits(num);
        y = y + (8 - (y & 7));
        y = y >> 3;
    }
    /* now store initial data */
    *out++ = 0x02;
    if (y < 128)
    {
        /* short form */
        *out++ = (unsigned char) y;
    }
    else if (y < 256)
    {
        *out++ = 0x81;
        *out++ = (unsigned char) (y & 255);
    }
    else if (y < 65536UL)
    {
        *out++ = 0x82;
        *out++ = (unsigned char) ((y >> 8) & 255);
        *out++ = (unsigned char) (y & 255);
    }
    else if (y < 16777216UL)
    {
        *out++ = 0x83;
        *out++ = (unsigned char) ((y >> 16) & 255);
        *out++ = (unsigned char) ((y >> 8) & 255);
        *out++ = (unsigned char) (y & 255);
    }
    else
    {
        return PS_ARG_FAIL;
    }

    /* now store msbyte of zero if num is non-zero */
    if (leading_zero)
    {
        *out++ = 0x00;
    }

    /* if it's not zero store it as big endian */
    if (pstm_cmp_d(num, 0) == PSTM_GT)
    {
        /* now store the mpint */
        if ((err = pstm_to_unsigned_bin(pool, num, out)) != PSTM_OKAY)
        {
            return err;
        }
    }
    else if (pstm_iszero(num) != PSTM_YES)
    {
        pstm_int tmp;
        /* negative */
        if (pstm_init(pool, &tmp) != PSTM_OKAY)
        {
            return PS_MEM_FAIL;
        }

        /* 2^roundup and subtract */
        y = pstm_count_bits(num);
        y = y + (8 - (y & 7));
        if (pstm_2expt(&tmp, y) != PSTM_OKAY || pstm_add(&tmp, num, &tmp) !=
            PSTM_OKAY)
        {
            pstm_clear(&tmp);
            return PS_MEM_FAIL;
        }

        if ((err = pstm_to_unsigned_bin(pool, &tmp, out)) != PSTM_OKAY)
        {
            pstm_clear(&tmp);
            return err;
        }
        pstm_clear(&tmp);
    }

    /* we good */
    *outlen = tmplen;
    return PS_SUCCESS;
}

static
size_t get_next_sub_id_len(const char *in,
        const char *in_end)
{
    size_t len = 0;

    if (*in == '.')
    {
        in++;
    }

    if (in >= in_end)
    {
        return 0;
    }

    while (in != in_end)
    {
        if (*in == '.')
        {
            break;
        }
        len++;
        in++;
    }

    return len;
}

static
int32_t validate_oid_dot_notation(const char *oid_str,
        psSizeL_t oid_str_len)
{
    const char *in = oid_str;
    psSizeL_t in_len = oid_str_len;
    const char *in_end = in + in_len;
    int last_i = in_len - 1;
    int i;

    /* Minimum OID string length is 3 (e.g "1.2"). */
    if (in_len < 3)
    {
        psTraceCrypto("asnEncodeDotNotationOID: " \
                "OID too short\n");
        return PS_PARSE_FAIL;
    }

    /* Do not allow leading or trailing dots. */
    if (in[0] == '.' || in[last_i] == '.')
    {
        psTraceCrypto("asnEncodeDotNotationOID: " \
                "no leading or trailing dots allowed\n");
        return PS_PARSE_FAIL;
    }

    /* Do not allow two consecutive dots. */
    for (i = 1; i < in_len; i++)
    {
        if (in[i] == '.' && in[i-1] == '.')
        {
            psTraceCrypto("asnEncodeDotNotationOID: "   \
                    "empty subidentifiers not allowed\n");
            return PS_PARSE_FAIL;
        }
    }

    /* Allow only dots and positive decimal integers. */
    for (i = 0; i < in_len; i++)
    {
        if (in[i] != '.' && (in[i] > '9' || in[i] < '0'))
        {
            psTraceCrypto("asnEncodeDotNotationOID: " \
                "unsupported character in OID string\n");
            return PS_PARSE_FAIL;
        }
    }

    /* The OID must have at least two sub-identifiers.
       So there must be at least one dot. */
    for (i = 0; i < in_len; i++)
    {
        if (in[i] == '.')
        {
            break;
        }
    }
    if (i == in_len)
    {
        psTraceCrypto("asnEncodeDotNotationOID: " \
                "OID too short\n");
        return PS_PARSE_FAIL;
    }

    /* There are only three valid top-level arcs: 0, 1 and 2. */
    if (in[0] != '0' && in[0] != '1' && in[0] != '2')
    {
        psTraceCrypto("asnEncodeDotNotationOID: " \
                "Not a valid top-level arc.\n");
        return PS_PARSE_FAIL;
    }

    /* First sub-identifier must have length 1 */
    if (get_next_sub_id_len(in, in_end) != 1)
    {
        psTraceCrypto("asnEncodeDotNotationOID: " \
                "Not a valid top-level arc.\n");
        return PS_PARSE_FAIL;
    }

    return PS_SUCCESS;
}

/* Init and zero the pstm object x. When init fails,
   convert the PSTM return code to the PS_ equivalent. */
#define PSTM_INIT_ZERO(pool, x)                   \
    do {                                          \
        int32_t pstm_rc;                          \
                                                  \
        pstm_rc = pstm_init(pool, &x);            \
        if (pstm_rc == PSTM_MEM)                  \
        {                                         \
            rc = PS_MEM_FAIL;                     \
            goto out_fail;                        \
        }                                         \
        else if (pstm_rc == PSTM_OKAY)            \
        {                                         \
            rc = PS_SUCCESS;                      \
        }                                         \
        else {                                    \
            rc = PS_FAILURE;                      \
        }                                         \
    } while(0)

/**
   Encode an absolute OID given as a dot notation string (e.g. "1.2.3.4")
   into OBJECT IDENTIFIER type using DER/BER/PER (all three encoding rules
   will give identical results).

   Returns PS_OUTPUT_LENGTH when the the caller-provided output buffer
   is too small. Caller is responsible for guessing the next output
   buffer size to try.

   Arbitrary-precision sub-identifiers are supported.

   @param[in] pool Memory pool to use.
   @param[in] out_buf Output buffer where to put the encoding.
   @param[in] out_buf_len Length of the output buffer.
   @param[out] num_enc_octets When the return code is PS_SUCCESS, the
   pointed variable will be set to the number of octets in the encoding.
   Otherwise, the variable will be set to 0.
   @param[in] oid_dot_notation The OID dot-notation string. ASCII and
   UTF-8 are supported. This function will validate the string.
   @param[in] oid_dot_notation_len Length of the input string.

   @return PS_SUCCESS: Encoding successful.
   @return PS_PARSE_FAIL: The OID dot-notation string does not conform
   to specification.
   @return PS_OUTPUT_LENGTH: Output length negotiation. There is not
   enough room in out_buf for the encoding. Caller should try again
   with a larger output buffer size.
   @return PS_MEM_FAIL: Out of memory.
   @return PS_FAILURE: An unexpected failure.
*/
int32_t asnEncodeDotNotationOID(psPool_t *pool,
        unsigned char *out_buf,
        psSizeL_t out_buf_len,
        psSizeL_t *num_enc_octets,
        const char *oid_dot_notation,
        psSizeL_t oid_dot_notation_len)
{
    const char *in = oid_dot_notation;
    const char *in_end;
    unsigned char *out = out_buf;
    unsigned char *out_begin, *out_end;
    psSizeL_t in_len = oid_dot_notation_len;
    psSizeL_t len;
    int rc, i;
    int num_content_octets, len_octets, tag_len_overhead;
    pstm_int subid_1, subid_2, subid_12, bn;
    pstm_int q, r;

    if (out == NULL || in == NULL || num_enc_octets == NULL)
    {
        return PS_ARG_FAIL;
    }

    in_end = in + in_len;
    out_begin = out;
    out_end = out + out_buf_len;
    *num_enc_octets = 0;

    /* Minimum OID encoding is 3 octets. */
    if (out_buf_len < 3)
    {
        return PS_OUTPUT_LENGTH;
    }

    /* Now validate the input string. */
    if (validate_oid_dot_notation(in, in_len) < 0)
    {
        return PS_PARSE_FAIL;
    }

    /* Init and zeroize all pstm objects in advance.
       This way, we can call pstm_free on all of them at exit,
       even if we fail in the middle of this function. */
    PSTM_INIT_ZERO(pool, subid_1);
    PSTM_INIT_ZERO(pool, subid_2);
    PSTM_INIT_ZERO(pool, subid_12);
    PSTM_INIT_ZERO(pool, bn);
    PSTM_INIT_ZERO(pool, q);
    PSTM_INIT_ZERO(pool, r);

    /*
      The first two sub-identifiers X and Y will be encoded as
      Z = 40X + Y. This "packing" is due to the restriction
      that Y<40 when X=0 or X=1. When X=0 or X=1, the encoding
      of Z will consist only of one octet. When X=2, Y can be
      larger, and thus the encoding of Z may require multiple
      octets.

      We'd like to skip one pstm multiplication here (and avoid
      having to call atoi). Because we are smart, we know that:
      40 * "0" = "0",
      40 * "1" = "40", and
      40 * "2" = "80".
    */

    /* Handle sub-id #1. */
    if (*in == '0')
    {
        rc = pstm_read_radix(pool, &subid_1, "0", 1, 10);
    }
    else if (*in == '1')
    {
        rc = pstm_read_radix(pool, &subid_1, "40", 2, 10);
    }
    else /* *in == '2' */
    {
        rc = pstm_read_radix(pool, &subid_1, "80", 2, 10);
    }
    if (rc < 0)
    {
        goto out_fail;
    }
    in++;
    if (*in != '.')
    {
        rc = PS_PARSE_FAIL;
        goto out_fail;
    }
    in++;

    /* Handle sub-id #2. */
    len = get_next_sub_id_len(in, in_end);
    if (len == 0)
    {
        rc = PS_PARSE_FAIL;
        goto out_fail;
    }
    if (pstm_read_radix(pool, &subid_2, in, len, 10) < 0)
    {
        rc = PS_FAILURE;
        goto out_fail;
    }
    in += len;

    /* subid_12 = 40*(sub-id #1) + sub-id #2. */
    if (pstm_add(&subid_1, &subid_2, &subid_12) < 0)
    {
        rc = PS_FAILURE;
        goto out_fail;
    }

    /*
      Encode the combined sub-ids #1 and #2.
      The encoding can consist of multiple octets.
    */
    len = out_end - out;
    rc = pstm_to_base128_be_unsigned_bin(pool, &subid_12, out, &len);
    if (rc < 0)
    {
        goto out_fail;
    }

    /* Set leading bit of every octet, except the last, to 1. */
    for (i = 0; i < len-1; i++)
    {
        out[i] |= 0x80;
    }
    out += len;
    if (out > out_end)
    {
        rc = PS_OUTPUT_LENGTH;
        goto out_fail;
    }

    /* Handle sub-ids #3, #4, etc.
       Break if there are no further sub-ids.*/
    while(true)
    {
        psSizeL_t len;

        if (*in == '.')
        {
            in++;
        }

        len = get_next_sub_id_len(in, in_end);
        if (len <= 0)
        {
            break;
        }

        psAssert(in + len <= in_end);

        /* The subidentifier decimal string is (in...in+len). */

        /* Handle special case: sub-id is "0".
           OPTN: Could handle more cases like this, without pstm. */
        if (len == 1 && *in == '0')
        {
            *out = 0x00;
            in++;
            out++;
            continue; /* Next sub-id. */
        }

        pstm_zero(&bn); /* Could be a re-use. */
        if (pstm_read_radix(pool, &bn, in, len, 10) < 0)
        {
            rc = PS_FAILURE;
            goto out_fail;
        }
        in += len;

        len = out_end - out;
        rc = pstm_to_base128_be_unsigned_bin(pool, &bn, out, &len);
        if (rc < 0)
        {
            goto out_fail;
        }

        /* Set leading bit of every octet, except the last, to 1. */
        for (i = 0; i < len - 1; i++)
        {
            out[i] |= 0x80;
        }
        out += len;
    } /* End of sub-identifier encoding loop. */

    /* Now we have all the content octets in out.
       Now encode the tag and the length. */

    num_content_octets = out - out_begin;
    tag_len_overhead = asn1TLOverhead(num_content_octets);
    if (out_buf_len < (num_content_octets + tag_len_overhead))
    {
        rc = PS_OUTPUT_LENGTH;
        goto out_fail;
    }

    /* Make room for the tag and the length encoding. */
    out = out_begin;
    Memmove(out+tag_len_overhead, out, num_content_octets);
    *out = ASN_OID; out++;
    len_octets = asnHelpWriteLength(out, num_content_octets);
    psAssert(tag_len_overhead == 1+len_octets);

    *num_enc_octets = num_content_octets + tag_len_overhead;

out_fail:
    pstm_clear(&subid_1);
    pstm_clear(&subid_2);
    pstm_clear(&subid_12);
    pstm_clear(&bn);
    pstm_clear(&q);
    pstm_clear(&r);

    return rc;
}
#endif /* USE_CERT_GEN */

/******************************************************************************/
