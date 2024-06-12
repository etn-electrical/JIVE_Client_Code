/**
 *****************************************************************************************
 * @file    Ram_C_Connector.h
 * @details    This is the C/C++ bridge file for Ram.cpp file. It is created in order to
               compile Pyramid EIP stack code with C compiler(Auto Extension in IAR).
 * @copyright    2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_C_CONNECTOR_H
#define RAM_C_CONNECTOR_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for Ram:Allocate( UINT16 ).
 *  @param[in]    size, size to allocate memory.
 *  @return    Pointer to allocated memory.
 */
void* Ram_Allocate( size_t size );

/**
 *  @brief    This is C/C++ bridge function for Ram:De_Allocate( void* ).
 *  @param[in]    ptr, pointer to allocated memory.
 *  @return    None
 */
void Ram_De_Allocate( void* ptr );

#ifdef __cplusplus
}
#endif


#endif	// RAM_C_CONNECTOR_H
