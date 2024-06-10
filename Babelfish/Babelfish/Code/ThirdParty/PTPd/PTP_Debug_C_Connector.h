/**
 *****************************************************************************************
 * @file        PTP_Debug_C_Connector.h
 * @details     This is the C/C++ bridge file for BF_DEBUG_PRINT and BF_ASSERT macro. It is
                created in order to compile PTPd stack code with auto extension based compiler.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PTP_DEBUG_C_CONNECTOR_H
#define PTP_DEBUG_C_CONNECTOR_H

#define EMERGENCY_MSG   4
#define ERROR_MSG       3
#define CRITICAL_MSG    2
#define INFO_MSG        1
#define DEBUG_MSG       0

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_DEBUG_PRINT_FUNC macro.
 *  @param[in]    msg_type, info or error msg.
 *  @param[in]    func, calling function name.
 *  @param[in]    format, varaible argument for msg.
 *  @return    void.
 */
void PTP_Debug_Print( unsigned char msg_type, const char* func, const char* format, ... );

/**
 *  @brief    This is C/C++ bridge function for
 *            Debug_Manager::Debug_Assert( const uint32_t line_num, const char_t* func_name ).
 *  @param[in]    line_num, code line number from where it is called.
 *  @param[in]    func_name, calling function name.
 *  @return    void.
 */
void PTP_BF_Assert( const unsigned int line_num, const char* func_name );

#ifdef __cplusplus
}
#endif


#endif	// PTP_DEBUG_C_CONNECTOR_H