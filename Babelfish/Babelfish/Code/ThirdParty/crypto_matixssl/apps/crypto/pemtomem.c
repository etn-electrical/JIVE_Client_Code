/**
 *      @file    pemtomem.c
 *
 *
 *      Command line utility to convert PEM files to header files.
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

#include "crypto/cryptoApi.h"
/* Currently this example uses _psTrace for tracing, so osdep.h is needed: */
#include "core/osdep.h"

#if defined(MATRIX_USE_FILE_SYSTEM) && defined(USE_CERT_PARSE)

# include "osdep_stdlib.h"
# include "osdep_stdio.h"
# include "osdep_string.h"
# include <sys/stat.h>

/******************************************************************************/

static void createKeyMemHeader(const psX509Cert_t *cert, const char *fileName,
                               const char *structName, const unsigned char *certBin, psSize_t certBinLen);
static void createNames(char *fileName, char *outfile, char *varname,
                        uint32 count);

static uint8_t g_stdout;

/******************************************************************************/

static void usage()
{
    _psTrace("usage 1: matrixPem2Mem -cert cert.pem [-name varname] [-stdout]\n");
    _psTrace("usage 2: matrixPem2Mem -key privkey.pem [-pass password] [-name varname] [-stdout]\n");
    return;
}

int32 main(int argc, char **argv)
{
    psX509Cert_t *X509Cert, *next;
    unsigned char *privDERbuf;
    char *cert, *privkey, *password, *name;
    char outfile[64];
    char varname[64];
    psSize_t privLen, count = 0;

    (void)privLen;
    (void)privDERbuf;

    /* command line parse */
    cert = privkey = password = name = NULL;
    argv++;  argc--; /* skip exe name */
    g_stdout = 0;

    while (*argv)
    {
        if (Strcmp(*argv, "-cert") == 0)
        {
            argv++; argc--;
            if (*argv)
            {
                cert = *argv;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (Strcmp(*argv, "-name") == 0)
        {
            argv++; argc--;
            if (*argv)
            {
                name = *argv;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (Strcmp(*argv, "-key") == 0)
        {
            argv++; argc--;
            if (*argv)
            {
                privkey = *argv;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (Strcmp(*argv, "-pass") == 0)
        {
            argv++; argc--;
            if (*argv)
            {
                password = *argv;
            }
            else
            {
                usage();
                return EXIT_FAILURE;
            }
        }
        else if (Strcmp(*argv, "-stdout") == 0)
        {
            g_stdout = 1;
        }
        else
        {
            usage();
            return EXIT_FAILURE;
        }
        argv++; argc--;
    }

    /* Command line usage verification */
    if (cert == NULL && privkey == NULL)
    {
        _psTrace("Must specifiy a cert file (-cert) or key file (-key)\n");
        usage();
        return EXIT_FAILURE;
    }

    if (psCryptoOpen(PSCRYPTO_CONFIG) < PS_SUCCESS)
    {
        _psTrace("Failed to initialize library:  psCryptoOpen failed\n");
        return EXIT_FAILURE;
    }

    /* Certificate conversion */
    if (cert)
    {
        if (psX509ParseCertFile(NULL, cert, &X509Cert,
                CERT_STORE_UNPARSED_BUFFER) < PS_SUCCESS)
        {
            _psTraceStr("Unable to parse %s.  Not converted\n", cert);
            return EXIT_FAILURE;
        }
        next = X509Cert;

        while (next)
        {
            /* Construct a file and variable name based off the file name */
            createNames(cert, outfile, varname, count++);
            createKeyMemHeader(next, outfile, name ? name : varname, next->unparsedBin,
                next->binLen);

            next = next->next;
        }
        psX509FreeCert(X509Cert);
    }

    /* Private key conversion */
    if (privkey)
    {
# if defined(USE_PRIVATE_KEY_PARSING) && defined(USE_RSA)
        if (psPkcs1DecodePrivFile(NULL, privkey, password, &privDERbuf,
                &privLen) < PS_SUCCESS)
        {
            _psTraceStr("Failed to parse private key %s\n", privkey);
            return EXIT_FAILURE;
        }
        createNames(privkey, outfile, varname, 0);
        createKeyMemHeader(NULL, outfile, name ? name : varname, privDERbuf, privLen);
        psFree(privDERbuf, NULL);
# else
        _psTrace("Please enable USE_PRIVATE_KEY_PARSING\n");
        return 1;
# endif /* USE_PRIVATE_KEY_PARSING */
    }

    psCryptoClose();
    return 0;
}

static void createNames(char *fileName, char *outfile, char *varname,
    uint32 count)
{
    uint32 varindex, i;
    char *extension;

    varindex = i = 0;
    Memset(varname, 0, 64);
    Memset(outfile, 0, 64);
    extension = strrchr(fileName, '.');
    while (fileName[i] != '\0')
    {
        if (i == 48)
        {
            break;
        }
        /* Stop at last '.'  */
        if (fileName + i == extension)
        {
            break;
        }
        /* Only put alpha-numeric in var name */
        if ((fileName[i] >= 'a' && fileName[i] <= 'z') ||
            (fileName[i] >= 'A' && fileName[i] <= 'Z') ||
            (fileName[i] >= '0' && fileName[i] <= '9'))
        {
            varname[varindex++] = fileName[i];
        }
        outfile[i] = fileName[i];
        i++;
    }
    if (count > 0)
    {
        outfile[i++] = count + '0';
        varname[varindex++] = count + '0';
    }
    Memcpy(&outfile[i], ".h", 2);
}

static void write_hex_comment(FILE *f, uint8_t indent,
    const unsigned char *buf, psSize_t len)
{
    char space[80];
    uint16_t i;

    if (indent > 79)
    {
        indent = 79;
    }
    Memset(space, ' ', indent);
    space[indent] = '\0';
    for (i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            Fprintf(f, "%s", space);
        }
        if (i < (len - 1))
        {
            Fprintf(f, "%02x:", buf[i]);
        }
        else
        {
            Fprintf(f, "%02x", buf[i]);
        }
        if (i % 16 == 15 || i == (len - 1))
        {
            Fprintf(f, "\n");
        }
    }
}

/**
    Convert a X.509 date to a standard date format.
    YYYY/MM/DD
    @param[in] in Pointer to X.509 date, either 6 or 8 bytes depending on 'type'.
    @param[in] type If ASN_UTC, 'in' is 6 bytes YYMMDD, otherwise 8 bytes YYYYMMDD.
    @param[out] out Allocated buffer to hold output, must be at least
    11 bytes in length to hold date and null terminator.
 */
static void x509date(const char *in, int32_t type, unsigned char *out)
{
    unsigned int y;

    /* First 2 digits of year */
    if (type == ASN_UTCTIME)
    {
        /* UTCTIME, defined in 1982, has just a 2 digit year */
        /* year as unsigned int handles over/underflows */
        y =  2000 + 10 * (in[0] - '0') + (in[1] - '0');
        /* Years from '96 through '99 are in the 1900's */
        if (y >= 2096 || y < 2000)
        {
            *out++ = '1';
            *out++ = '9';
        }
        else
        {
            *out++ = '2';
            *out++ = '0';
        }
    }
    else
    {
        *out++ = *in++;
        *out++ = *in++;
    }
    /* Second 2 digits of year */
    *out++ = *in++;
    *out++ = *in++;
    *out++ = '/';
    /* Month */
    *out++ = *in++;
    *out++ = *in++;
    *out++ = '/';
    /* day */
    *out++ = *in++;
    *out++ = *in;
    *out = '\0';
}
/*****************************************************************************/
/*
    Accept a DER binary stream and create a C header file with the hex
    representation for use in matrixSslReadKeysMem
 */
static void createKeyMemHeader(const psX509Cert_t *cert, const char *fileName,
    const char *structName, const unsigned char *certBin,
    psSize_t certBinLen)
{
    FILE *f;
    int i;
    x509GeneralName_t *altname;
    x509OrgUnit_t *orgUnit;
    unsigned char *c;
    unsigned char d[11];

    if (g_stdout)
    {
        f = stdout;
    }
    else
    {
        if ((f = Fopen(fileName, "w")) == NULL)
        {
            _psTraceStr("Unable to open %s for writing\n", fileName);
            return;
        }
    }
    Fprintf(f,
        "/**\n"
        " *\t@file    %s\n"
        " *\t@" "version " "%%h" "%%d\n"
        " *\n"
        " *\tAuto generated from PEM file.\n"
        " */\n", fileName);
    if (cert)
    {
        Fprintf(f, "/*\n\tAuto generated X.509 certificate as binary DER.\n");
        Fprintf(f, "Certificate:\n");
        Fprintf(f, "    Data:\n");
        Fprintf(f, "        Version: %d\n", (cert->version + 1));
        Fprintf(f, "        Serial Number:\n");
        write_hex_comment(f, 12, cert->serialNumber, cert->serialNumberLen);
        Fprintf(f, "        Issuer: CN=%s, C=%s, ST=%s, O=%s",
            cert->issuer.commonName,
            cert->issuer.country,
            cert->issuer.state,
            cert->issuer.organization);
        for (orgUnit = cert->issuer.orgUnit; orgUnit != NULL; orgUnit = orgUnit->next)
        {
            _psTraceStr(", OU=%s", orgUnit->name);
        }
        Fprintf(f, "\n");

        Fprintf(f, "        Validity:\n");
        x509date(cert->notBefore, cert->notBeforeTimeType, d);
        Fprintf(f, "            Not Before: %s\n", d);
        x509date(cert->notAfter, cert->notAfterTimeType, d);
        Fprintf(f, "            Not After : %s\n", d);
        Fprintf(f, "        Subject: CN=%s, C=%s, ST=%s, O=%s",
            cert->subject.commonName,
            cert->subject.country,
            cert->subject.state,
            cert->subject.organization);
        for (orgUnit = cert->subject.orgUnit; orgUnit != NULL; orgUnit = orgUnit->next)
        {
            _psTraceStr(", OU=%s", orgUnit->name);
        }
        Fprintf(f, "\n");
        Fprintf(f, "        Subject Public Key Info:\n");
        switch (cert->publicKey.type)
        {
        case PS_RSA:
            Fprintf(f, "            Public Key Algorithm: rsaEncryption\n");
            Fprintf(f, "                Public-Key: (%d bit)\n", cert->publicKey.keysize * 8);
# ifdef USE_MATRIX_RSA
            Fprintf(f, "                Modulus:\n");
            i = pstm_unsigned_bin_size(&cert->publicKey.key.rsa.N);
            c = Malloc(i);
            pstm_to_unsigned_bin(NULL, &cert->publicKey.key.rsa.N, c);
            write_hex_comment(f, 20, c, i & 0xFFFF);
            Free(c);
            Fprintf(f, "                Exponent: %u\n",
                (uint32_t) cert->publicKey.key.rsa.e.dp[0]);
# endif
            break;
        case PS_ECC:
            Fprintf(f, "            Public Key Algorithm: id-ecPublicKey\n");
            Fprintf(f, "                Public-Key: (%d bit)\n", cert->publicKey.keysize * 4);
# ifdef USE_MATRIX_ECC
            Fprintf(f, "                pub:\n");
            i = pstm_unsigned_bin_size(&cert->publicKey.key.ecc.pubkey.x);
            c = Malloc(i);
            pstm_to_unsigned_bin(NULL, &cert->publicKey.key.ecc.pubkey.x, c);
            write_hex_comment(f, 20, c, i & 0xFFFF);
            Free(c);
            i = pstm_unsigned_bin_size(&cert->publicKey.key.ecc.pubkey.y);
            c = Malloc(i);
            pstm_to_unsigned_bin(NULL, &cert->publicKey.key.ecc.pubkey.y, c);
            write_hex_comment(f, 20, c, i & 0xFFFF);
            Free(c);
            Fprintf(f, "                ASN1 OID: %s\n",
                cert->publicKey.key.ecc.curve->name);
# endif
            break;
        }

        if (cert->extensions.critFlags)
        {
            Fprintf(f, "        X509v3 extensions (critical: 0x%x):\n",
                cert->extensions.critFlags);
        }
        else
        {
            Fprintf(f, "        X509v3 extensions:\n");
        }
        for (altname = cert->extensions.san; altname != NULL; altname = altname->next)
        {
            Fprintf(f, "            X509v3 Subject Alternative Name: %s\n", altname->name);
            switch (altname->id)
            {
            case GN_DNS:
                c = Malloc(altname->dataLen + 1);
                Memcpy(c, altname->data, altname->dataLen);
                c[altname->dataLen] = '\0';
                Fprintf(f, "                %s\n", c);
                Free(c);
                break;
            case GN_IP:
                c = altname->data;
                if (altname->dataLen == 4)
                {
                    Fprintf(f, "                %d.%d.%d.%d\n", c[0], c[1], c[2], c[3]);
                    break;
                }
            /* Fall though */
            default:
                write_hex_comment(f, 16, altname->data, altname->dataLen);
                break;
            }
        }
        Fprintf(f, "            X509v3 Basic Constraints:\n");
        Fprintf(f, "                CA:%s\n", cert->extensions.bc.cA ? "TRUE" : "FALSE");
        if (cert->extensions.bc.pathLenConstraint > 0)
        {
            Fprintf(f, "                Path Length Constraint:%d\n",
                cert->extensions.bc.pathLenConstraint);
        }

        Fprintf(f, "            X509v3 Key Usage: 0x%x\n",
            cert->extensions.keyUsageFlags);
        Fprintf(f, "            X509v3 Extended Key Usage: 0x%x\n",
            cert->extensions.ekuFlags);
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_TLS_SERVER_AUTH)
        {
            Fprintf(f, "                TLS WWW server authentication\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_TLS_CLIENT_AUTH)
        {
            Fprintf(f, "                TLS WWW client authentication\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_CODE_SIGNING)
        {
            Fprintf(f, "                Signing of downloadable executable code\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_EMAIL_PROTECTION)
        {
            Fprintf(f, "                Email protection\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_TIME_STAMPING)
        {
            Fprintf(f, "                Binding the hash of an object to a time\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_OCSP_SIGNING)
        {
            Fprintf(f, "                Signing OCSP responses\n");
        }
        if (cert->extensions.ekuFlags & EXT_KEY_USAGE_ANY)
        {
            Fprintf(f, "                Any\n");
        }
        if (cert->extensions.sk.id)
        {
            Fprintf(f, "            X509v3 Subject Key Identifier:\n");
            write_hex_comment(f, 16, cert->extensions.sk.id, cert->extensions.sk.len);
        }
        if (cert->extensions.ak.keyId)
        {
            Fprintf(f, "            X509v3 Authority Key Identifier:\n");
            write_hex_comment(f, 16, cert->extensions.ak.keyId, cert->extensions.ak.keyLen);
        }

        switch (cert->sigAlgorithm)
        {
        case OID_MD5_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: md5WithRSAEncryption\n");
            break;
        case OID_MD2_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: md2WithRSAEncryption\n");
            break;
        case OID_SHA1_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: sha1WithRSAEncryption\n");
            break;
        case OID_SHA256_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: sha256WithRSAEncryption\n");
            break;
        case OID_SHA384_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: sha384WithRSAEncryption\n");
            break;
        case OID_SHA512_RSA_SIG:
            Fprintf(f, "    Signature Algorithm: sha512WithRSAEncryption\n");
            break;
        case OID_SHA1_ECDSA_SIG:
            Fprintf(f, "    Signature Algorithm: ecdsa-with-SHA1\n");
            break;
        case OID_SHA256_ECDSA_SIG:
            Fprintf(f, "    Signature Algorithm: ecdsa-with-SHA256\n");
            break;
        case OID_SHA384_ECDSA_SIG:
            Fprintf(f, "    Signature Algorithm: ecdsa-with-SHA384\n");
            break;
        case OID_SHA512_ECDSA_SIG:
            Fprintf(f, "    Signature Algorithm: ecdsa-with-SHA512\n");
            break;
        case OID_RSASSA_PSS:
            Fprintf(f, "    Signature Algorithm: RSA-PSS\n");
            break;
        }
        write_hex_comment(f, 8, cert->signature, cert->signatureLen);

    }
    else
    {

        Fprintf(f, "/*\n\tAuto generated private key as binary DER.\n");

    }

    Fprintf(f, "*/\n#define %s_SIZE\t%d\n", structName, certBinLen);
    Fprintf(f, "const static unsigned char %s[%s_SIZE] =", structName, structName);
    for (i = 0; i < certBinLen; i++)
    {
        if (i % 16 == 0)
        {
            Fprintf(f, "\n\t\"");
        }
        Fprintf(f, "\\x%02x", certBin[i]);
        if (i % 16 == 15 || i == (certBinLen - 1))
        {
            Fprintf(f, "\"");
        }
    }
    Fprintf(f, ";\n");
    if (!g_stdout)
    {
        Fclose(f);
        _psTraceStr("Successfully wrote %s\n", fileName);
    }
    return;
}

#else

/******************************************************************************/
/*
    Stub main for compiling without features enabled
 */
int32 main(int32 argc, char **argv)
{
# ifndef MATRIX_USE_FILE_SYSTEM
    Printf("MATRIX_USE_FILE_SYSTEM must be enabled "
        "to run this application.\n");
# endif /* MATRIX_USE_FILE_SYSTEM */
# ifndef USE_CERT_PARSE
    Printf("USE_CERT_PARSE must be enabled "
        "to run this application.\n");
# endif /* USE_CERT_PARSE */
    return EXIT_FAILURE;
}

#endif /* MATRIX_USE_FILE_SYSTEM */

/******************************************************************************/
