/**
 **********************************************************************************************
 * @file            stm32_wpan_entry.h
 * @brief           Entry application header file for STM32WPAN Middleware
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on. This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 * @copyright
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics. All rights reserved.</center></h2>
 * This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; You may not use this
 * file except in compliance with the License. You may obtain a copy of the License at: www.st.com/SLA0044
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_WPAN_ENTRY_H
#define STM32_WPAN_ENTRY_H

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
 * @brief    Initialize the BLE stack. Call this function from the main program to start the initialization of the
 * stack.
 */
void Stm32_Wpan_Init( void );

/**
 * @brief    Initialize the BLE stack. This function is automatically called by the stack after the low-level
 * initialization has taken place.
 */
void APP_BLE_Init( void );

#endif
