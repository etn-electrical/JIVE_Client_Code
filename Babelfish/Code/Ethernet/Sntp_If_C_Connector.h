/**
 *****************************************************************************************
 * @file    Sntp_If_C_Connector.h
 * @details    This is the C/C++ bridge file for sntp_if.cpp file. It is created in
               order to compile LWIP stack code with auto extension compiler.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SNTP_IF_C_CONNECTOR_H
#define SNTP_IF_C_CONNECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Set_Epoch_Time( uint32_t *sec, uint32_t *us  ).
 *  @param[in]    sec, second pointer.
    @param[in]    us, microsecond pointer.
 *  @return    void.
 */
void SNTP_Set_Epoch_Time( uint32_t* sec, uint32_t* us );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Get_Sntp_Retry_Time( ).
 *  @param[in]    None.
 *  @return    Retry time.
 */
uint16_t SNTP_Get_Sntp_Retry_Time( void );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Get_Sntp_Update_Time().
 *  @param[in]    None.
 *  @return    time.
 */
uint32_t SNTP_Get_Sntp_Update_Time( void );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Get_Epoch_Time( uint32_t *sec, uint32_t *us  ).
 *  @param[out]    sec, second pointer.
    @param[out]    us, microsecond pointer.
 *  @return    void.
 */
void SNTP_Get_Epoch_Time( uint32_t* sec, uint32_t* us );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Calculate_Offset_Time( uint32_t* t1, uint32_t* t2, uint32_t* t3,
                                            uint32_t* t4, uint32_t* offset );
 *  @param[in]    t1, second pointer.
    @param[in]    t2, second pointer.
    @param[in]    t3, second pointer.
    @param[in]    t4, second pointer.
    @param[out]   offset, offset pointer.
 *  @return    void.
 */
void SNTP_Calculate_Offset_Time( uint32_t* t1, uint32_t* t2, uint32_t* t3, uint32_t* t4,
								 uint32_t* offset );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Set_Active_Server_Status( uint8_t connection_status,
                                    uint8_t server_index ).
 *  @param[in]    connection_status, connected or disconnected.
    @param[in]    server_index, 0 or 1.
 *  @return    void.
 */
void SNTP_Set_Active_Server_Status( uint8_t connection_status,
									uint8_t server_index );

/**
 *  @brief    This is C/C++ bridge function for
              SNTP_If::Set_Active_Server_Status( uint8_t connection_status,
                                    uint8_t server_index ).
 *  @param[in]    connection_status, connected or disconnected.
 *  @return    void.
 */
void SNTP_Reset_Active_Server_Status( uint8_t connection_status );

#ifdef __cplusplus
}
#endif


#endif	// SNTP_IF_C_CONNECTOR_H