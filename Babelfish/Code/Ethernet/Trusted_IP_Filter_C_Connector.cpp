/**
 *****************************************************************************************
 * @file    Trusted_IP_Filter_C_Connector.cpp
 * @details    This file shall includes all the definition of C/C++ bridge functions
               created for Trusted_IP_Filter.cpp file.
 * @copyright    2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Etn_Types.h"
#include "Trusted_IP_Filter_C_Connector.h"
#include "Trusted_IP_Filter.h"
#include "lwip/def.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Is_Remote_IP_Trusted( const uint32_t remote_ip, const uint16_t port )
{
	return ( Trusted_IP_Filter::Is_Remote_Ip_Trusted( remote_ip, htons( port ) ) );
}
