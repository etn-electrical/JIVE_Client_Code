/**
 *****************************************************************************************
 * @file        PTP_Slave_If_C_Connector.h
 * @details     This is the C/C++ bridge file for PTP_Slave_If.cpp file. It is created in
                order to compile PTPd stack code with auto extension based compiler.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PTP_SLAVE_IF_C_CONNECTOR_H
#define PTP_SLAVE_IF_C_CONNECTOR_H

#include "../ptpd.h"	// PTPd stack file
#include "sys_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Set_Time( struct ptptime_t* timestamp ).
 *  @param[in]    sec, second value.
 *  @param[in]    nsec, nano-second value.
 *  @return    void.
 */
void Ptp_Interface_Set_Time( int32_t sec, int32_t nsec );

/**
 *  @brief    This is C/C++ bridge function for
 *            void PTP_Slave_If::Get_Time( struct ptptime_t* timestamp )
 *  @param[in]    sec, second pointer.
 *  @param[in]    nsec, nano-second pointer.
 *  @return    void.
 */
void Ptp_Interface_Get_Time( int32_t* sec, int32_t* nsec );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Update_Offset( struct ptptime_t* timestamp )
 *  @param[in]    sec, second value.
 *  @param[in]    nsec, nano-second value.
 *  @return    void.
 */
void Ptp_Interface_Update_Offset( int32_t sec, int32_t nsec );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Adj_Frq( int32_t adj )
 *  @param[in]    adj, time stamp adjustment value.
 *  @return    void.
 */
void Ptp_Interface_Adj_Frq( int32_t adj );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Init_Timers( void )
 *  @param[in]    ptpClock, PtpClock structure pointer.
 *  @return    void.
 */
void Ptp_Interface_Init_Timers( const PtpClock* ptpClock );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Stop_Timer( uint8_t index ).
 *  @param[in]    index, timer index.
 *  @return    void.
 */
void Ptp_Interface_Stop_Timer( int32_t index );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Start_Timer( uint8_t index, uint32_t time_interval ).
 *  @param[in]    index, timer index.
 *  @param[in]    interval_ms, interval in msec.
 *  @return    void.
 */
void Ptp_Interface_Start_Timer( int32_t index, uint32_t interval_ms );

/**
 *  @brief    This is C/C++ bridge function for
 *            PTP_Slave_If::Alert_Queue( void ).
 *  @param[in]    void.
 *  @return    sys_mbox_t*, mutex pointer.
 */
sys_mbox_t* Ptp_Interface_Alert_Queue( void );


#ifdef __cplusplus
}
#endif


#endif	// PTP_SLAVE_IF_C_CONNECTOR_H