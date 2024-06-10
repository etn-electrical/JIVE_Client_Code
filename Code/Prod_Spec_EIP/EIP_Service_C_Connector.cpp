/**
 *****************************************************************************************
 * @file       EIP_Service_C_Connector.cpp
 * @details    This file shall includes all the definition of C/C++ bridge functions
               created for EIP_Service.cpp file.
 * @copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "EIP_Service_C_Connector.h"
#include "EIP_Service.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void EIPService_Load_EIP_Identity_Attribute( uint16_t attribute )
{
	EIP_Service::Load_EIP_Identity_Attribute( attribute );
}
