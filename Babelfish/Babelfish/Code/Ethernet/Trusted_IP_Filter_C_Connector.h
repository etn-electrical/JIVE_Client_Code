/**
 *****************************************************************************************
 * @file    Trusted_IP_Filter_C_Connector.h
 * @details    This is the C/C++ bridge file for Trusted_IP_Filter.cpp file. It is created in
               order to compile Pyramid EIP stack code with C compiler(Auto Extension in IAR).
 * @copyright    2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TRUSTED_IP_FILTER_C_CONNECTOR_H
#define TRUSTED_IP_FILTER_C_CONNECTOR_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for
              Trusted_IP_Filter::Is_Remote_Ip_Trusted( const uint32_t, const uint16_t ).
 *  @param[in]    remote_ip, remote ip address.
    @param[in]    port, remote port number.
 *  @return    True or False.
 */
bool Is_Remote_IP_Trusted( const uint32_t remote_ip, const uint16_t port );


#ifdef __cplusplus
}
#endif


#endif	// TRUSTED_IP_FILTER_C_CONNECTOR_H
