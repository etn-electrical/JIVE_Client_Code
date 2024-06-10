/**
 *****************************************************************************************
 * @file       EIP_if_C_Connector.cpp
 * @details    This file shall includes all the definition of C/C++ bridge functions
               created for EIP_if_C_Connector.h file.
 * @copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "EIP_if_C_Connector.h"
#include "OS_Tasker.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void OS_Tasker_Delay( uint32_t x )
{
	OS_Tasker::Delay( x );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t OS_Tasker_Get_Tick_Count( void )
{
	return ( OS_Tasker::Get_Tick_Count() );
}
