/**
 *****************************************************************************************
 * @file    Sntp_If_C_Connector.cpp
 * @details    This file shall includes all the definition of C/C++ bridge functions
               created for sntp_if.cpp file.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "sntp_if.h"
#include "Sntp_If_C_Connector.h"
#include "lwip/timeouts.h"
#ifdef ESP32_SETUP
#include "sntp_opts.h"
#endif

static SNTP_If* sntp_if_ptr;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP32_SETUP
void sntp_init( SNTP_If* arg );

#else
void sntp_init_restart( SNTP_If* arg );

#endif

#ifdef __cplusplus
}
#endif

/**
 *  @brief    This function initialize sntp module.
 *            Send out request instantly or after SNTP_STARTUP_DELAY(_FUNC).
 *  @param[in]    arg, reference of SNTP_If class object.
 *  @return    void.
 */
#ifndef ESP32_SETUP
void sntp_init( SNTP_If* arg )
#else
void sntp_init_restart( SNTP_If* arg )
#endif
{
	sntp_if_ptr = arg;
	sys_timeout( SNTP_STARTUP_DELAY, SNTP_If::Update_Static, sntp_if_ptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Set_Epoch_Time( uint32_t* sec, uint32_t* us )
{
	sntp_if_ptr->Set_Epoch_Time( *sec, *us );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t SNTP_Get_Sntp_Retry_Time( void )
{
	return ( sntp_if_ptr->Get_Sntp_Retry_Time() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SNTP_Get_Sntp_Update_Time( void )
{
	return ( sntp_if_ptr->Get_Sntp_Update_Time() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Get_Epoch_Time( uint32_t* sec, uint32_t* us )
{
	sntp_if_ptr->Get_Epoch_Time( *sec, *us );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Calculate_Offset_Time( uint32_t* t1, uint32_t* t2, uint32_t* t3, uint32_t* t4,
								 uint32_t* offset )
{
	sntp_if_ptr->Calculate_Offset_Time( t1, t2, t3, t4, offset );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Set_Active_Server_Status( uint8_t connection_status, uint8_t server_index )
{
	sntp_if_ptr->Set_Active_Server_Status( connection_status, server_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Reset_Active_Server_Status( uint8_t connection_status )
{
	sntp_if_ptr->Set_Active_Server_Status( connection_status );
}
