/**
 **********************************************************************************************
 *@file           uC_FMC.h Header File for FMC implementation.
 *
 *@brief          The file shall wrap all the functions which are required for communication
 *                between STM microcontroller and FMC.
 *@details
 *@copyright      2020 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 *@Remark
 **********************************************************************************************
 */

#ifndef __uC_FMC_h__
#define __uC_FMC_h__

#include "stm32f7xx_ll_fmc.h"
#include "stm32f7xx_hal_def.h"

/**
 **********************************************************************************************
 * @brief                   FMC Class Declaration. The class will handle all the functionalities
 *                          related to FMC communication.
 **********************************************************************************************
 */
class uC_FMC
{
	public:

		/**
		 * @brief                     Constructor to create instance of uc_FMC class.
		 * @return
		 */
		uC_FMC( void );

		/**
		 * @brief                   Destructor to delete the uc_FMC instance when it goes out of
		 *                          scope.
		 */
		~uC_FMC( void );

		/**
		 * @brief  Initializes the FMC_SDRAM device according to the specified
		 *         control parameters in the FMC_SDRAM_InitTypeDef
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  Init: Pointer to SDRAM Initialization structure
		 * @retval none
		 */
		void FMC_SDRAM_Init( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_InitTypeDef* Init );

		/**
		 * @brief  Initializes the FMC_SDRAM device timing according to the specified
		 *         parameters in the FMC_SDRAM_TimingTypeDef
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  Timing: Pointer to SDRAM Timing structure
		 * @param  Bank: SDRAM bank number
		 * @retval none
		 */
		void FMC_SDRAM_Timing_Init( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_TimingTypeDef* Timing, uint32_t Bank );

		/**
		 * @brief  DeInitializes the FMC_SDRAM peripheral
		 * @param  Device: Pointer to SDRAM device instance
		 * @retval none
		 */
		void FMC_SDRAM_DeInit( FMC_SDRAM_TypeDef* Device, uint32_t Bank );

		/**
		 * @brief  Enables dynamically FMC_SDRAM write protection.
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  Bank: SDRAM bank number
		 * @retval none
		 */
		void FMC_SDRAM_WriteProtection_Enable( FMC_SDRAM_TypeDef* Device, uint32_t Bank );

		/**
		 * @brief  Disables dynamically FMC_SDRAM write protection.
		 * @param  hsdram: FMC_SDRAM handle
		 * @retval none
		 */
		void FMC_SDRAM_WriteProtection_Disable( FMC_SDRAM_TypeDef* Device, uint32_t Bank );

		/**
		 * @brief  Send Command to the FMC SDRAM bank
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  Command: Pointer to SDRAM command structure
		 * @param  Timing: Pointer to SDRAM Timing structure
		 * @param  Timeout: Timeout wait value
		 * @retval none
		 */
		void FMC_SDRAM_SendCommand( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout );

		/**
		 * @brief  Program the SDRAM Memory Refresh rate.
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  RefreshRate: The SDRAM refresh rate value.
		 * @retval none
		 */
		void FMC_SDRAM_ProgramRefreshRate( FMC_SDRAM_TypeDef* Device, uint32_t RefreshRate );

		/**
		 * @brief  Set the Number of consecutive SDRAM Memory auto Refresh commands.
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  AutoRefreshNumber: Specifies the auto Refresh number.
		 * @retval none
		 */
		void FMC_SDRAM_SetAutoRefreshNumber( FMC_SDRAM_TypeDef* Device, uint32_t AutoRefreshNumber );

		/**
		 * @brief  Returns the indicated FMC SDRAM bank mode status.
		 * @param  Device: Pointer to SDRAM device instance
		 * @param  Bank: Defines the FMC SDRAM bank. This parameter can be
		 *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM.
		 * @retval The FMC SDRAM bank mode status, could be on of the following values:
		 *         FMC_SDRAM_NORMAL_MODE, FMC_SDRAM_SELF_REFRESH_MODE or
		 *         FMC_SDRAM_POWER_DOWN_MODE.
		 */
		uint32_t FMC_SDRAM_GetModeStatus( FMC_SDRAM_TypeDef* Device, uint32_t Bank );

};

#endif



