/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This file Contains the functionalities to calculate the CRC. This module implements the 16bit CRC-CCITT
 *	algorithm (0x11021).
 *
 *	@details The algorithm used for this implementation is quite fast on an embedded
 *	system in the non-table format.  The table format may be a little quicker but this
 *	should be tested if speed is crucial.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef MODBUS_CRC_H
#define MODBUS_CRC_H

namespace BF_Mbus
{
/*
 *****************************************************************************************
 *		Constants for internal switching between different CRC calc methods.
 *****************************************************************************************
 */

#define MODBUS_CRC_TABLE_METHOD     0U
#define MODBUS_CRC_SHIFT_METHOD     1U
// #define  MODBUS_CRC_METHOD     MODBUS_CRC_SHIFT_METHOD
#define  MODBUS_CRC_METHOD     MODBUS_CRC_TABLE_METHOD

static const uint16_t MODBUS_CRC_INIT = 0xffffU;// < The default CRC value.
static const uint8_t MODBUS_OVERRUN_CRC_GOOD = 0U;

/**
 * @brief Will calculate the CRC value from a singly byte.  It can be used
 * where a CRC is generated in steps.
 * @param[in] data:  New byte to add.
 * @param[in] prev_CRC: The previous or initial CRC depending on how the string is
 * checked.
 * @return The new CRC value.
 */
// uint16_t MODBUS_CRC_Calc_On_Byte( uint8_t data, uint16_t prev_crc );

/**
 * @brief Calculates a CRC on a string.
 * @param[in] data: The pointer to the data.
 * @param[in] length: The length of data to check.
 * @return The new CRC value.
 */
uint16_t MODBUS_CRC_Calc_On_String( uint8_t* data, uint16_t length );

/**
 * @brief Verify string for good CRC.
 * @param[in] data: The pointer to the data.
 * @param[in] length: The length of data to check.
 * @return True if String with good CRC & vice-versa.
 */
bool MODBUS_CRC_Check_String_For_Good_CRC( uint8_t* data, uint16_t length );

// uint8_t MODBUS_LRC_Calc_On_Byte( uint8_t data, uint16_t prev_lrc );
uint8_t MODBUS_LRC_Calc_On_String( uint8_t* data, uint16_t length );

bool_t MODBUS_LRC_Check_String_For_Good_LRC( uint8_t* data, uint16_t length );

// #define MODBUS_CRC_Get_Init_Value()			MODBUS_CRC_INIT

}

#endif	/* MODBUS_CRC_H */
