/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_CODE_RANGE_H
#define uC_CODE_RANGE_H

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_Start_Address( void );

uint8_t* uC_App_Fw_End_Address( void );

uint8_t* uC_App_Fw_Crc_Start_Address( void );

uint8_t* uC_ROM_End_Address( void );

uint16_t uC_CODE_RANGE_CRC_Val( void );

uint8_t* uC_App_Real_Fw_Start_Address( void );

#endif
