/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the TLS Ram class with functionalities mainly for Memory allocation
 *	and de-allocation.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TLS_RAM_H
#define TLS_RAM_H

// #include "Ram_Handler.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* As per the RFC 5246 of TLS 1.2, the maximum Plain Text fragment length should be 2^14
   i.e. 16384.
 */

void* Tls_Mem_Malloc( size_t size );

void* Tls_Mem_Calloc( size_t num, size_t size );

void* Tls_Mem_Realloc( void* ptr, size_t size );

void Tls_Mem_Free( void* ptr );

#ifdef __cplusplus
}
#endif

#endif
