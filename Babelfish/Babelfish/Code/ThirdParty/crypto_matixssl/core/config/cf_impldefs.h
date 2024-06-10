/* cf_impldefs.h
 *
 * Description: Configuration options for Framework/IMPLDEFS implementation
 */

/*****************************************************************************
* Copyright (c) 2007-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef INCLUDE_GUARD_CF_IMPLDEFS_H
#define INCLUDE_GUARD_CF_IMPLDEFS_H 1

/*
   All L_PRINTFs (ie. all debug/trace and panic messages).
 */
#undef  IMPLDEFS_CF_DISABLE_DEBUG_L_PRINTF


/*
   L_DEBUG print outs.
 */
#undef  IMPLDEFS_CF_DISABLE_L_DEBUG

#ifdef CFG_IMPLDEFS_NO_DEBUG
# define IMPLDEFS_CF_DISABLE_L_DEBUG
#endif


/*
   L_TRACE print outs.
 */
#undef  IMPLDEFS_CF_DISABLE_L_TRACE

/*
   ASSERT() macro, i.e. assertion checks.
 */
#undef  IMPLDEFS_CF_DISABLE_ASSERT

#ifdef CFG_IMPLDEFS_NO_DEBUG
# define IMPLDEFS_CF_DISABLE_ASSERT
#endif

/*
   PRECONDITION() macro, ie. function contract input checks.
 */
#undef  IMPLDEFS_CF_DISABLE_PRECONDITION

#ifdef CFG_IMPLDEFS_NO_DEBUG
# define IMPLDEFS_CF_DISABLE_PRECONDITION
#endif

/*
   POSTCONDITION() macro, ie. function contract output checks.
 */
#undef  IMPLDEFS_CF_DISABLE_POSTCONDITION

#ifdef CFG_IMPLDEFS_NO_DEBUG
# define IMPLDEFS_CF_DISABLE_POSTCONDITION
#endif

/**
   All assertion and function contract checks.
   (Ie. ASSERT(), PRECONDITION(), and POSTCONDITION() macros.)
 */
#undef  IMPLDEFS_CF_DISABLE_ASSERTION_CHECK

#ifdef CFG_IMPLDEFS_NO_DEBUG
# define IMPLDEFS_CF_DISABLE_ASSERTION_CHECK
#endif

/** Functionality that is omitted.
    Old sfzcl_snprintf (with renderer support) is no longer used by default. */
#define NO_SFZCL_SNPRINTF 1

/* Support for ISO 8859-2, 8859-3, and 8859-4 character set is omitted.
   For characters beyond ASCII, UTF-8 needs to be used instead. */
/* #define USE_SFZCL_CHARSET_ISO_8859_2_CONVERTER 1 */
/* #define USE_SFZCL_CHARSET_ISO_8859_3_CONVERTER 2 */
/* #define USE_SFZCL_CHARSET_ISO_8859_4_CONVERTER 3 */

#endif /* INCLUDE_GUARD_CF_IMPLDEFS_H */

/* end of file cf_impldefs.h */
