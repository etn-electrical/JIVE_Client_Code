/**
 *****************************************************************************************
 *	@file		Spi_flash_test_example.h
 *
 *	@brief		The file contains the Flash testing details
 *
 *****************************************************************************************
 */
#include <stdio.h>
#include "uC_Flash.h"
#include "uC_Fw_Code_Range.h"

/**
 * @brief						Spi Flash Test App Main.
 * @return void					None
 */
void Spi_Flash_Test_App_Main( void );

/**
 * @brief						Function to test Synchronous uC_Flash Read() and Write() .
 * @return void					None
 */
void Sync_Read_Write_Test( void );

/**
 * @brief						Function to test Asynchronous uC_Flash Read() and Write() .
 * @return void					None
 */
void Async_Read_Write_Test( void );


