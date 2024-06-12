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
#ifndef uC_FW_CODE_RANGE_H
#define uC_FW_CODE_RANGE_H

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
uint8_t* uC_Uberloader_Start_Address( void );

uint8_t* uC_Uberloader_End_Address( void );

uint8_t* uC_Web_Fw_Start_Address( void );

uint8_t* uC_Web_Fw_End_Address( void );

uint8_t* uC_Scratch_Area_Start_Address( void );

uint8_t* uC_Scratch_Area_End_Address( void );

uint8_t* uC_UberloaderInfo_Start_Address( void );

uint8_t* uC_Device_Cert_Start_Address( void );

uint8_t* uC_Device_Cert_End_Address( void );

uint8_t* uC_Language_Image_Start_Address( void );

uint8_t* uC_Language_Image_End_Address( void );



#endif
