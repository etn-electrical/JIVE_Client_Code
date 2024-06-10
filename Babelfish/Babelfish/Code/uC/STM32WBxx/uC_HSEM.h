/**
 **********************************************************************************************
 * @file            uC_HSEM.h Header File for Hardware semaphore initialization.
 *
 * @brief           The file contains Hardware semaphore initialization.
 * @details
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef UC_HSEM_H
#define UC_HSEM_H

#include "stm32wbxx_hal_hsem.h"

#define HSEM_FLASH_PROTECT ( 2U )	/* use HW semaphore 2 FLASH - All registers*/
#define HSEM_CPU1_FLASH_PROTECT  ( 6U )	/* use HW semaphore 6 Used by CPU1 to prevent CPU2 from writing/erasing data in
										   Flash memory*/
#define HSEM_CPU2_FLASH_PROTECT ( 7U )	/* use HW semaphore 7 Used by CPU2 to prevent CPU1 from writing/erasing data in
										   Flash memory*/

#endif	// UC_HSEM_H