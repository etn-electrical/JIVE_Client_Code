/**
 *****************************************************************************************
 * @file	: uC_Time_nSec.h
 *
 * @brief
 * @details : This file contains declaration and API related to nano second time
 *
 *
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_TIME_NSEC_H
#define UC_TIME_NSEC_H
#include "Device_Config.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t NANO_SEC_TIME_MULTIPLICATION_FACTOR = 1000U;
/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

/**
 * @brief: Initializes nano second timer
 * @param[in] void
 * @param[out] void
 */
inline void uC_TIME_NSEC_Init( void )
{}

/**
 * @brief: Returns cpu cycle count required for nano second time value
 * @param[in] void
 * @param[out] uint32_t
 */
inline uint32_t uC_TIME_NSEC_Sample( void )
{
	return ( esp_cpu_get_ccount() );
}

/**
 * @brief: Returns nano second elapsed time value from the specified sample count
 * @param[in] uint32_t
 * @param[out] uint32_t
 */
inline uint32_t uC_TIME_NSEC_Elapsed( uint32_t sample_time )
{
	uint32_t elapsed_time;

	elapsed_time = esp_cpu_get_ccount();

	elapsed_time = elapsed_time - sample_time;

	elapsed_time = ( elapsed_time * NANO_SEC_TIME_MULTIPLICATION_FACTOR ) / ( MASTER_CLOCK_FREQUENCY );

	return ( elapsed_time );
}

#endif
