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
#ifndef uC_TEST_FLASH_H
	#define uC_TEST_FLASH_H

#define uC_FLASH_TEST_START_POINT           0x0801e800
#define uC_FLASH_TEST_LENGTH                0x1800
#define uC_FLASH_TEST_PAGE_SIZE             0x800
// #define uC_FLASH_TEST_LENGTH				0x5
#define uC_FLASH_TEST_STRING_LEN            ( uC_FLASH_TEST_LENGTH / 2 )
#define uC_FLASH_TEST_CHECKSUM_LEN          2

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */
void uC_TEST_Flash_Init( void );


#endif
