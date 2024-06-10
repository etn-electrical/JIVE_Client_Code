#ifndef MATRIXOS_H
#define MATRIXOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>         /* malloc, free, etc... */
#include <string.h>

#define MATRIX_NO_POOL      (void *) 0x0
#define MAX_MEMORY_USAGE    0
#define psOpenMalloc()      0
#define psCloseMalloc()
#define psDefineHeap( A, B )
#define psAddPoolCache( A, B )
#define psMallocNoPool      malloc
#define psMalloc( A, B )      Tls_Mem_Malloc( B )
#define psCalloc( A, B, C )   Tls_Mem_Calloc( B, C )
#define psRealloc( A, B, C )  Tls_Mem_Realloc( A, B )
#define psFree( A, B )        Tls_Mem_Free( A )

#define HAVE_NATIVE_INT64
#define PSPUBLIC extern

#ifndef max
	#define max( a, b )    ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif //max

#include "TLS_Ram.h"

//Following typedefs are already declared in Etn_Types.h and stdint.h
#if     !defined( ETN_TYPES_H ) && !defined ( _STDINT )
typedef float float32_t;
typedef double float64_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned long long uint64_t;
typedef signed long long int64_t;
#endif
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef signed short int16;
typedef signed int int32;
typedef char char_t;
typedef int32_t psPool_t;

#ifdef HAVE_NATIVE_INT64
typedef signed long long int64;
typedef unsigned long long uint64;
#endif //HAVE_NATIVE_INT64

typedef uint32_t psTime_t;

#ifndef ESP32_SETUP
typedef psTime_t time_t;
#endif
	#define __inline inline
extern uint32_t Matrix_OS_Get_Epoch_Time( void );

#ifndef ESP32_SETUP
#define time(a) Matrix_OS_Get_Epoch_Time()
#endif

/******************************************************************************/
/*
    Defines of other derived types
 */

/* Size type commonly used by MatrixSSL. This is currently 16-bit to keep
   data structures small. In future the type can be larger. */
typedef uint16_t psSize_t;

/* At least 32-bit size for large inputs.
   On some platforms the type can be larger than 32 bits. */
typedef uint32_t psSize32_t;

/* An alias for largest possible object size on the target.
   Currently the same than size_t. */
typedef size_t psSizeL_t;

/* 16-bit identifier for cipher. This can be used in TLS protocol. */
typedef uint16_t psCipher16_t;

/* 16-bit identifier for curve. This can be used in cryptography protocol. */
typedef uint16_t psCurve16_t;

/* Result code of ps functions returning negative value for error and
   a positive value for success. The purpose of positive values
   returned is documented for each function. */
typedef int32_t psRes32_t;

/* Result code of ps functions returning negative value for error and
   a positive value for success. The positive value is size of operation
   or area returned.
   The full documentation of positive values returned is documented for
   each function. */
typedef int32_t psResSize_t;

/* Result code of ps functions, returning PS_SUCCESS for success and
   a negative value for failure. */
typedef int32_t psRes_t;

typedef uint8 psBool_t;

/******************************************************************************/
/*
    Limitations for the data types.
 */

#  define PS_SIZE_MIN ((psSize_t) 0)
#  define PS_SIZE_MAX (~(psSize_t) 0)
#  define PS_SIZE32_MIN ((psSize32_t) 0)
#  define PS_SIZE32_MAX (~(psSize32_t) 0)
#  define PS_SIZEL_MIN ((psSizeL_t) 0)
#  define PS_SIZEL_MAX (~(psSizeL_t) 0)
#  define PS_RES_OK_MIN ((psRes_t) 0)
#  define PS_RES_OK_MAX ((psRes_t) 0)
#  define PS_RES_SIZE_OK_MIN ((psResSize_t) 0)
#  define PS_RES_SIZE_OK_MAX ((psResSize_t) 0x7FFFFFFFUL)
#  define PS_BOOL_MIN ((psBool_t) PS_FALSE)
#  define PS_BOOL_MAX ((psBool_t) PS_TRUE)


struct tm* gmtime_r( const time_t*timer, struct tm*tmbuf ); //Prayas added

/******************************************************************************/
/*
    Raw trace and error
 */
PSPUBLIC void _psTrace( const char*msg );

PSPUBLIC void _psTraceInt( const char*msg, int32 val );

PSPUBLIC void _psTraceStr( const char*msg, const char*val );

PSPUBLIC void _psTracePtr( const char*message, const void*value );

PSPUBLIC void psTraceBytes( const char*tag, const unsigned char*p, int l );

PSPUBLIC void _psError( const char*msg );

PSPUBLIC void _psErrorInt( const char*msg, int32 val );

PSPUBLIC void _psErrorStr( const char*msg, const char*val );

/******************************************************************************/
int32 psGetEntropy( unsigned char*bytes, uint32 size, void*userPtr );

int32 psDiffMsecs( psTime_t then, psTime_t now, void*userPtr );

int32 psGetTime( psTime_t*t, void*userPtr );

void osdepEntropyClose( void );

int osdepEntropyOpen( void );

void osdepTimeClose( void );

int osdepTimeOpen( void );

/******************************************************************************/
/*
    Core trace
 */
#define psTrace Term_Print

#ifndef USE_CORE_TRACE
 #define psTraceCore( x )
 #define psTraceStrCore( x, y )
 #define psTraceIntCore( x, y )
 #define psTracePtrCore( x, y )
#else
 #define psTraceCore( x ) _psTrace( x )
 #define psTraceStrCore( x, y ) _psTraceStr( x, y )
 #define psTraceIntCore( x, y ) _psTraceInt( x, y )
 #define psTracePtrCore( x, y ) _psTracePtr( x, y )
#endif /* USE_CORE_TRACE */

/******************************************************************************/
/*
    HALT_ON_PS_ERROR define at compile-time determines whether to halt on
    psAssert and psError calls
 */
#ifdef USE_CORE_ASSERT
#define psAssert( C )  if ( C ){;}else \
	{halAlert();_psTraceStr( "psAssert %s", __FILE__ );_psTraceInt( ":%d ", __LINE__ ); \
	 _psError(#C );}
#else
#define psAssert( C )  if ( C ){;}else do { /* assert ignored. */} while ( 0 )
#endif

#ifdef USE_CORE_ERROR
#define psError( a ) \
	halAlert();_psTraceStr( "psError %s", __FILE__ );_psTraceInt( ":%d ", __LINE__ ); \
	_psError( a );

#define psErrorStr( a, b ) \
	halAlert();_psTraceStr( "psError %s", __FILE__ );_psTraceInt( ":%d ", __LINE__ ); \
	_psErrorStr( a, b )

#define psErrorInt( a, b ) \
	halAlert();_psTraceStr( "psError %s", __FILE__ );_psTraceInt( ":%d ", __LINE__ ); \
	_psErrorInt( a, b )
#else
#define psError( a ) do { /* error ignored. */} while ( 0 )
#define psErrorStr( a, b ) do { /* error ignored. */} while ( 0 )
#define psErrorInt( a, b ) do { /* error ignored. */} while ( 0 )
#endif

/******************************************************************************/
/******************************************************************************/
/*
    Hardware Abstraction Layer
 */
/* Hardware Abstraction Layer - define functions in HAL directory */
 #define halOpen() 0
 #define halClose()
 #define halAlert()



/******************************************************************************/
/******************************************************************************/
/*
    Secure memset/memzero
 */
typedef size_t rsize_t;
typedef int errno_t;
extern errno_t memset_s( void*s, rsize_t smax, int c, rsize_t n );

#define memzero_s( S, N ) memset_s( S, N, 0x0, N )

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //MATRIXOS_H
