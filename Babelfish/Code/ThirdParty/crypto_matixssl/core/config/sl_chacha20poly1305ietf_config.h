/**
 *      @file    ps_chacha20poly1305ietf_config.h
 *
 *
 *      Header for SafeZone Chacha20-poly1305: Configuration.
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/


/******************************************************************************/

#ifndef _h_SL_CHACHA20_POLY1305IETF_CONFIG
# define _h_SL_CHACHA20_POLY1305IETF_CONFIG

/* Define this if you wish to use generic utility functions even when
   you are not using Chacha20. */
#define USE_SL_SODIUM

/* Provide list of APIs and facilities available on the target platform and
   configure the components for optimal performance on the target. */

/* When porting the module to foreign target, it is possible that some of
   these need to be changed to best adapt for the platform. */

# if defined USE_SL_CHACHA20_POLY1305_IETF || defined USE_SL_SODIUM
#  ifdef USE_MULTITHREADING
#   define HAVE_PTHREAD_PRIO_INHERIT 1
#   define HAVE_PTHREAD 1
#   define _POSIX_PTHREAD_SEMANTICS 1
#  endif /* USE_MULTITHREADING */

#  ifndef STDC_HEADERS
#   define STDC_HEADERS
#  endif

#  if defined __linux__ || defined __android__
#   define HAVE_SYS_TYPES_H 1
#   define HAVE_SYS_STAT_H 1
#   define HAVE_SYS_MMAN_H 1
#   define HAVE_DLFCN_H 1
#  endif

#  define HAVE_STDLIB_H 1
#  define HAVE_STRING_H 1
#  define HAVE_MEMORY_H 1
#  define HAVE_STRINGS_H 1
#  define HAVE_INTTYPES_H 1
#  define HAVE_STDINT_H 1
#  define HAVE_UNISTD_H 1

/* X86/X86-64 SIMD instructions: MMX, SSE, and AVX. */
#  if defined __i386__ || defined __x86_64__
#   define HAVE_MMINTRIN_H 1
#   define HAVE_EMMINTRIN_H 1
#   define HAVE_PMMINTRIN_H 1
#   define HAVE_TMMINTRIN_H 1
#   define HAVE_SMMINTRIN_H 1
#   define HAVE_AVXINTRIN_H 1
#   ifdef USE_X86_AVX2 /* By default omit AVX2 which requires a new compiler. */
#    define HAVE_AVX2INTRIN_H 1
#   endif
#   define HAVE_WMMINTRIN_H 1
#   define HAVE_AVX_ASM 1
#  endif

#  define NATIVE_LITTLE_ENDIAN 1
#  ifdef __x86_64__
#   define HAVE_AMD64_ASM 1
#   define HAVE_TI_MODE 1
#  endif
#  if defined __i386__ || defined __x86_64__
#   define HAVE_CPUID 1
#  endif
#  define HAVE_WEAK_SYMBOLS 1
#  define HAVE_ATOMIC_OPS 1
#  define HAVE_MMAP 1
#  define HAVE_MLOCK 1
#  define HAVE_MADVISE 1
#  define HAVE_NANOSLEEP 1
#  define HAVE_POSIX_MEMALIGN 1
#  define HAVE_GETPID 1

#  define ASM_HIDE_SYMBOL .hidden
#  define CPU_UNALIGNED_ACCESS 1

#  define NO_SODIUM_MEMORY_MANAGEMENT 1 /* Omit memory management functions. */

#  ifdef __android__
#   define HAVE_ANDROID_GETCPUFEATURES
#  endif /* __android__ */

#  ifndef NO_SODIUM_MEMORY_MANAGEMENT
#   define HAVE_MPROTECT 1
#  else
#   undef HAVE_MPROTECT
#  endif

# endif
#endif /* _h_SL_CHACHA20_POLY1305IETF_CONFIG */

