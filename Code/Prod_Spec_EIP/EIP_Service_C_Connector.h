/**
 *****************************************************************************************
 * @file       EIP_Service_C_Connector.h
 * @details    This is the C/C++ bridge file for EIP_Service.cpp file. It is created in order
 *             to compile Pyramid EIP stack code with C compiler(Auto Extension in IAR).
 * @copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef EIP_SERVICE_C_CONNECTOR_H
#define EIP_SERVICE_C_CONNECTOR_H

#include "EIP_if_C_Connector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for EIP_Service::Load_EIP_Identity_Attribute( UINT16 ).
 *  @param[in]    attribute, EIP Identity Attribute.
 *  @return    None.
 */
void EIPService_Load_EIP_Identity_Attribute( uint16_t attribute );

#ifdef __cplusplus
}
#endif


#endif	// EIP_SERVICE_C_CONNECTOR_H
