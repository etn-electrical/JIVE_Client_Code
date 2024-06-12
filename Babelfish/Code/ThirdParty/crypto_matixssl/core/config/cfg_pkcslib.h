/* cfg_pkcslib.h
 *
 * Description: PKCS library distribution constants
 */

/*****************************************************************************
* Copyright (c) 2007-2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef CFG_PKCSLIB_H
#define CFG_PKCSLIB_H

/* This configure must be enabled to use MD5 digests.
   The MD5 digests have been demonstrated to be insecure and therefore
   they are not recommended. Support for MD5 digests can
   be disabled by this switch if needed. */
#define SFZCLDIST_CRYPT_MD5

/* Uncomment if making freestanding build. */
/* Freestanding build will work on bare operating systems without
   basic ISO C90/C99/C11 time, string, I/O and dynamic memory management
   libraries. */
/* #define SFZCLDIST_FREESTANDING */

/* Provide SIZEOF_LONG to sfzclmp. */
#ifndef SIZEOF_LONG
# ifdef __LP64__
#  define SIZEOF_LONG 8
# else
#  define SIZEOF_LONG 4
# endif
#endif

/* ---- Predefined options ---- */

/* The library is designed for this option combination and
   altering these options means the build procedure and tests need
   to be tweaked to adjust. */

/* Cryptographic options. */
#define WITH_RSA
#define SFZCLDIST_CERT
#define SFZCLDIST_CRYPT_DL
#define SFZCLDIST_CRYPTO_HASH
#define SFZCLDIST_CRYPT_SHA
#define SFZCLDIST_CRYPT_DSA
#define SFZCLDIST_CRYPTO_PK
#define SFZCLDIST_CRYPTO
#define SFZCLDIST_CRYPT
#define SFZCLDIST_CRYPT_DES
#define SFZCLDIST_CRYPT_SHA256

#endif /* CFG_PKCSLIB_H */

/* end of file cfg_pkcslib.h */
