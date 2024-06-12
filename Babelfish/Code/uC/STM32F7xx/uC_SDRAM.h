/**
 **********************************************************************************************
 *@file           uC_SDRAM.h Header File for SDRAM implementation.
 *
 *@brief          The file shall wrap all the functions which are required for communication
 *                between STM microcontroller and SDRAM.
 *@details
 *@copyright      2020 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 *@Remark
 **********************************************************************************************
 */

#ifndef __uC_SDRAM_h__
#define __uC_SDRAM_h__

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_def.h"
#include "uC_FMC.h"

/**
 **********************************************************************************************
 * @brief                   SDRAM Class Declaration. The class will handle all the functionalities
 *                          related to SDRAM communication.
 **********************************************************************************************
 */
class uC_SDRAM
{
	public:

		/**
		 * @brief	Constructor to create instance of uc_SDRAM class.
		 * @param[in] fmc_sdram_select
		 * @param[in] io_ctrl	Structure pointer which contains information about the pins which are
		 *			  used for FMC SDRAM communication.
		 * @return
		 */
		uC_SDRAM( uint8_t fmc_sdram_select,
				  uC_BASE_FMC_SDRAM_IO_PORT_STRUCT const* io_ctrl = nullptr );

		/**
		 * @brief	Destructor to delete the uc_SDRAM instance when it goes out of scope.
		 */
		~uC_SDRAM( void );

		/**
		 * @brief  Performs the SDRAM device initialization sequence.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @param  Timing: Pointer to SDRAM control timing structure
		 * @retval none
		 */
		void uC_SDRAM_Init( SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_TimingTypeDef* Timing );

		/**
		 * @brief  Perform the SDRAM device initialization sequence.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval none
		 */
		void uC_SDRAM_DeInit( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  This function handles SDRAM refresh error interrupt request.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval none
		 */
		void uC_SDRAM_IRQHandler( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  SDRAM Refresh error callback.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval None
		 */
		void uC_SDRAM_RefreshErrorCallback( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  Reads 8-bit data buffer from the SDRAM memory.
		 * @param  pAddress: Pointer to read start address
		 * @param  pDstBuffer: Pointer to destination buffer
		 * @param  BufferSize: Size of the buffer to read from memory
		 * @retval none
		 */
		void uC_SDRAM_Read_8b( uint32_t* pAddress, uint8_t* pDstBuffer, uint32_t BufferSize );

		/**
		 * @brief  Writes 8-bit data buffer to SDRAM memory.
		 * @param  pAddress: Pointer to write start address
		 * @param  pSrcBuffer: Pointer to source buffer to write
		 * @param  BufferSize: Size of the buffer to write to memory
		 * @retval none
		 */
		void uC_SDRAM_Write_8b( uint32_t* pAddress, uint8_t* pSrcBuffer, uint32_t BufferSize );

		/**
		 * @brief  Reads 16-bit data buffer from the SDRAM memory.
		 * @param  pAddress: Pointer to read start address
		 * @param  pDstBuffer: Pointer to destination buffer
		 * @param  BufferSize: Size of the buffer to read from memory
		 * @retval none
		 */
		void uC_SDRAM_Read_16b( uint32_t* pAddress, uint16_t* pDstBuffer, uint32_t BufferSize );

		/**
		 * @brief  Writes 16-bit data buffer to SDRAM memory.
		 * @param  pAddress: Pointer to write start address
		 * @param  pSrcBuffer: Pointer to source buffer to write
		 * @param  BufferSize: Size of the buffer to write to memory
		 * @retval none
		 */
		void uC_SDRAM_Write_16b( uint32_t* pAddress, uint16_t* pSrcBuffer, uint32_t BufferSize );

		/**
		 * @brief  Reads 32-bit data buffer from the SDRAM memory.
		 * @param  pAddress: Pointer to read start address
		 * @param  pDstBuffer: Pointer to destination buffer
		 * @param  BufferSize: Size of the buffer to read from memory
		 * @retval none
		 */
		void uC_SDRAM_Read_32b( uint32_t* pAddress, uint32_t* pDstBuffer, uint32_t BufferSize );

		/**
		 * @brief  Writes 32-bit data buffer to SDRAM memory.
		 * @param  pAddress: Pointer to write start address
		 * @param  pSrcBuffer: Pointer to source buffer to write
		 * @param  BufferSize: Size of the buffer to write to memory
		 * @retval none
		 */
		void uC_SDRAM_Write_32b( uint32_t* pAddress, uint32_t* pSrcBuffer, uint32_t BufferSize );

		/**
		 * @brief  Enables dynamically SDRAM write protection.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval none
		 */
		void uC_SDRAM_WriteProtection_Enable( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  Disables dynamically SDRAM write protection.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval none
		 */
		void uC_SDRAM_WriteProtection_Disable( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  Sends Command to the SDRAM bank.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @param  Command: SDRAM command structure
		 * @param  Timeout: Timeout duration
		 * @retval none
		 */
		void uC_SDRAM_SendCommand( SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout );

		/**
		 * @brief  Programs the SDRAM Memory Refresh rate.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @param  RefreshRate: The SDRAM refresh rate value
		 * @retval none
		 */
		void uC_SDRAM_ProgramRefreshRate( SDRAM_HandleTypeDef* hsdram, uint32_t RefreshRate );


		/**
		 * @brief  Sets the Number of consecutive SDRAM Memory auto Refresh commands.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @param  AutoRefreshNumber: The SDRAM auto Refresh number
		 * @retval none
		 */
		void uC_SDRAM_SetAutoRefreshNumber( SDRAM_HandleTypeDef* hsdram, uint32_t AutoRefreshNumber );

		/**
		 * @brief  Returns the SDRAM memory current mode.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval The SDRAM memory mode.
		 */
		uint32_t uC_SDRAM_GetModeStatus( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  Returns the SDRAM state.
		 * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
		 *                the configuration information for SDRAM module.
		 * @retval HAL state
		 */
		uint32_t uC_SDRAM_GetState( SDRAM_HandleTypeDef* hsdram );

		/**
		 * @brief  Sets the GPIO pin configuration
		 * @param  pio_ctrl
		 * @retval none
		 */
		void Set_Periph_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl );

	private:

		uC_FMC* m_fmc_handle = nullptr;

		/**
		 * @brief FMC SDRAM pin configuration holding structure
		 */
		uC_BASE_FMC_SDRAM_IO_STRUCT const* m_config;

		FMC_Bank5_6_TypeDef* m_fmc_sdram;



};

#endif
