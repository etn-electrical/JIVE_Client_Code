/**
 *****************************************************************************************
 * @file    Ram_C_Connector.cpp
 * @details    This file shall includes all the definition of C/C++ bridge functions
               created for Ram.cpp file.
 * @copyright    2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Ram_C_Connector.h"
#include "Etn_Types.h"
#ifndef ESP32_SETUP
#include <intrinsics.h>
#endif
#include "Device_Config.h"
#include "Ram.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Ram_Allocate( size_t size )
{
	return ( Ram::Allocate( size ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ram_De_Allocate( void* ptr )
{
	Ram::De_Allocate( ptr );
}
