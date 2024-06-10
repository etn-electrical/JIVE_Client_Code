/**
 ******************************************************************************
 * @file    QSPI/QSPI_ReadWrite_DMA/Src/main.c
 * @author  MCD Application Team
 * @brief   This example describes how to configure and use QuadSPI through
 *          the STM32F7xx HAL API.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "uC_QSPI.h"
#include "Device_Defines.h"
#include "stm32l4xx_hal.h"
#include "Timers_Lib.h"
/** @addtogroup STM32F7xx_HAL_Examples
 * @{
 */

/** @addtogroup QSPI_ReadWrite_DMA
 * @{
 */

/* Private typedef -----------------------------------------------------------
   Private define ------------------------------------------------------------
   Private macro -------------------------------------------------------------
   Private variables ---------------------------------------------------------*/

__IO uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch, TimeOut;

/* Buffer used for transmission */
uint8_t aTxBuffer[] =
	" ****QSPI communication based on DMA****  ****QSPI communication based on DMA****  ****QSPI communication based on DMA****  ****QSPI communication based on DMA****  ****QSPI communication based on DMA****  ****QSPI communication based on DMA**** ";

QSPI_HandleTypeDef QSPIHandle;
/* Private function prototypes -----------------------------------------------*/
// void QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
// void QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi);
// void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi);
// static void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi);


uC_QSPI::uC_QSPI( uC_USER_IO_STRUCT const* const* chip_sel_ctrls,
				  uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl, bool enable_dma ) :
	m_semaphore( nullptr ),
	m_config( nullptr )
{
	QSPIHandle.Instance = QUADSPI;
	/* Call the DeInit function to reset the driver */
	HAL_QSPI_DeInit( &QSPIHandle );
	/* System level initialization */
	HAL_QSPI_MspInit( &QSPIHandle );
	/* QSPI initialization
	   QSPI freq = SYSCLK /(1 + ClockPrescaler) = 216 MHz/(1+1) = 108 Mhz*/
	QSPIHandle.Init.ClockPrescaler = 3U;
	QSPIHandle.Init.FifoThreshold = 4U;
	QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	QSPIHandle.Init.FlashSize = QSPI_FLASH_SIZE;
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_3;
	QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;
	QSPIHandle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;


	HAL_QSPI_Init( &QSPIHandle );
	/* QSPI memory reset */
	QSPI_ResetMemory();

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData = 0U;
	sCommand.DataMode = 0U;
	sCommand.AddressMode = 0U;
	sCommand.Instruction = 0U;
	sCommand.Address = 0U;
	sCommand.AlternateBytes = 0U;
	sCommand.AlternateBytesSize = 0U;
	sCommand.DummyCycles = 0U;

	/* Configuration of the dummy cycles on QSPI memory side */
	QSPI_DummyCyclesCfg();

	/* enable QE bit in status register */
	QSPI_WriteEnable();
	QSPI_WriteStatusRegister( 0x40 );
	uC_Delay( 1U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_QSPI::~uC_QSPI( void )
{
	QSPIHandle.Instance = QUADSPI;
	/* Call the DeInit function to reset the driver */
	if ( HAL_QSPI_DeInit( &QSPIHandle ) != HAL_OK )
	{
		// return QSPI_ERROR;
	}

	/* System level De-initialization */
	HAL_QSPI_MspDeInit( &QSPIHandle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Select_Chip( uint8_t chip_select )
{
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );

	uC_IO_Config::Clr_Out( &PIOB_PIN_6_STRUCT );

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::De_Select_Chip( uint8_t chip_select ) const
{
	uC_IO_Config::Set_Out( &PIOB_PIN_6_STRUCT );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::QSPI_ResetMemory()
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the reset enable command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = RESET_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0U;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Send the command */
	Select_Chip( 0 );
	HAL_QSPI_Command( &QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE );
	De_Select_Chip( 0 );

	/* Send the reset memory command */
	s_command.Instruction = RESET_MEMORY_CMD;
	Select_Chip( 0 );
	HAL_QSPI_Command( &QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE );
	De_Select_Chip( 0 );

	/* Configure automatic polling mode to wait the memory is ready */
	QSPI_AutoPollingMemReady();
}

/**
 * @brief  This function send a Write Enable and wait it is effective.
 * @param  hqspi: QSPI handle
 * @retval None
 */
void uC_QSPI::QSPI_WriteEnable()
{
	QSPI_CommandTypeDef sCommand;
	QSPI_AutoPollingTypeDef sConfig;

	/* Enable write operations ------------------------------------------ */
	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = WRITE_ENABLE_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_NONE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}

	/* Configure automatic polling mode to wait for write enabling ---- */
	sConfig.Match = 0x02;
	sConfig.Mask = 0x02;			// WAIT WEL is 1
	sConfig.MatchMode = QSPI_MATCH_MODE_AND;
	sConfig.StatusBytesSize = 1U;
	sConfig.Interval = 0x10U;
	sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	sCommand.Instruction = READ_STATUS_REG_CMD;
	sCommand.DataMode = QSPI_DATA_1_LINE;

	if ( HAL_QSPI_AutoPolling( &QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::QSPI_WriteStatusRegister( uint8_t status )
{
	QSPI_CommandTypeDef sCommand;

	// QSPI_AutoPollingTypeDef sConfig;

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = WRITE_STATUS_REG_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_1_LINE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData = 1U;

	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}

	if ( HAL_QSPI_Transmit( &QSPIHandle, &status, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_QSPI::QSPI_ReadStatusRegister( void )
{
	QSPI_CommandTypeDef s_command;
	// QSPI_AutoPollingTypeDef sConfig;
	uint8_t status;

	/* Initialize the read flag status register command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0U;
	s_command.NbData = 1U;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if ( HAL_QSPI_Command( &QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}

	if ( HAL_QSPI_Receive( &QSPIHandle, &status, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Write_Data( uint8_t* data, uint32_t address, uint32_t length )
{
	QSPI_CommandTypeDef sCommand;

	// QSPI_AutoPollingTypeDef sConfig;

	/* Initialize the program command */
	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_4_LINES;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.Address = address;
	sCommand.NbData = length;

	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}

	if ( HAL_QSPI_Transmit( &QSPIHandle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Read_Data( uint8_t* data, uint32_t address, uint32_t length )
{
	QSPI_CommandTypeDef sCommand;

	// QSPI_AutoPollingTypeDef sConfig;

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = QUAD_OUT_FAST_READ_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
	sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	sCommand.Address = address;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_4_LINES;
	sCommand.DummyCycles = DUMMY_CLOCK_CYCLES_READ_QUAD;
	sCommand.NbData = length;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;


	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}

	// Set S# timing for Read command
	MODIFY_REG( QSPIHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE );

	if ( HAL_QSPI_Receive( &QSPIHandle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
	// Restore S# timing for nonRead commands
	MODIFY_REG( QSPIHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE );
	/*sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	   sCommand.Instruction = 0x5A;
	   sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
	   sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
	   sCommand.Address     = 0x00U;
	   sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	   sCommand.DataMode    = QSPI_DATA_4_LINES;
	   sCommand.DummyCycles = 8;
	   sCommand.DdrMode     = QSPI_DDR_MODE_DISABLE;
	   sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	   sCommand.SIOOMode    = QSPI_SIOO_INST_EVERY_CMD;

	   if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	   {
	    Error_Handler();
	   }

	   // Set S# timing for Read command
	   MODIFY_REG(QSPIHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_8_CYCLE);

	   if (HAL_QSPI_Receive(&QSPIHandle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	   {
	    Error_Handler();
	   }
	   // Restore S# timing for nonRead commands
	   MODIFY_REG(QSPIHandle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Erase_Sector( uint32_t address )
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the erase command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = SECTOR_ERASE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_24_BITS;
	s_command.Address = address;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0U;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if ( HAL_QSPI_Command( &QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Erase_Block( uint8_t erase_cmd, uint32_t address )
{
	QSPI_CommandTypeDef s_command;

	/* Initialize the erase command */
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction = erase_cmd;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_24_BITS;
	s_command.Address = address;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0U;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if ( HAL_QSPI_Command( &QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Change_QSPI_Mode( uint8_t mode )
{
	QSPI_CommandTypeDef sCommand;

	// QSPI_AutoPollingTypeDef sConfig;

	/* Enable write operations ------------------------------------------ */
	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = mode;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_NONE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		Error_Handler();
	}
}

/**
 * @brief  This function read the SR of the memory and wait the EOP. WIP is 0, which indicates that the flash is ready
 * @param  hqspi: QSPI handle
 * @retval None
 */

void uC_QSPI::QSPI_AutoPollingMemReady()
{
	// wait until the flash is ready(WIP is 0)
	QSPI_CommandTypeDef sCommand;
	QSPI_AutoPollingTypeDef sConfig;

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = READ_STATUS_REG_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_1_LINE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	sConfig.Match = 0x00U;
	sConfig.Mask = 0x01;			// bit 0 is WIP
	sConfig.MatchMode = QSPI_MATCH_MODE_AND;
	sConfig.StatusBytesSize = 1U;
	sConfig.Interval = 0x10U;
	sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	if ( HAL_QSPI_AutoPolling( &QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )	// QSPI
																												// Automatic
																												// Polling
																												// Mode
																												// in
																												// blocking
																												// mode.
	{
		// Error_Handler();
	}
}

/**
 * @brief  This function configure the dummy cycles on memory side.
 * @param  hqspi: QSPI handle
 * @retval None
 */

void uC_QSPI::QSPI_DummyCyclesCfg()
{
	QSPI_CommandTypeDef sCommand;
	uint8_t reg;

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = READ_VOL_CFG_REG_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_1_LINE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData = 1U;

	Select_Chip( 0 );
	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}

	if ( HAL_QSPI_Receive( &QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}
	De_Select_Chip( 0 );
	/*Select_Chip(0);

	   QSPI_WriteEnable();
	   De_Select_Chip(0);*/

	sCommand.Instruction = WRITE_VOL_CFG_REG_CMD;
	sCommand.DummyCycles = 0U;
	MODIFY_REG( reg, 0x78, ( DUMMY_CLOCK_CYCLES_READ_QUAD << 3 ) );

	Select_Chip( 0 );
	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}

	if ( HAL_QSPI_Transmit( &QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}
	De_Select_Chip( 0 );

	sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction = READ_VOL_CFG_REG_CMD;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_1_LINE;
	sCommand.DummyCycles = 0U;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData = 1U;

	Select_Chip( 0 );
	if ( HAL_QSPI_Command( &QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}

	if ( HAL_QSPI_Receive( &QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
	{
		// Error_Handler();
	}
	De_Select_Chip( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_QSPI::Error_Handler()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
extern "C" uint32_t HAL_GetTick()
{
	static uint32_t tempTick = 0U;

	if ( BF_Lib::Timers::Get_Time() == 0U )
	{
		tempTick++;
		return ( tempTick );
	}
	else
	{
		return ( BF_Lib::Timers::Get_Time() );
	}
}

/**
 * @brief  Command completed callbacks.
 * @param  hqspi: QSPI handle
 * @retval None
 */
extern "C" void HAL_QSPI_CmdCpltCallback( QSPI_HandleTypeDef* hqspi )
{
	CmdCplt++;
}

/**
 * @brief  Rx Transfer completed callbacks.
 * @param  hqspi: QSPI handle
 * @retval None
 */
extern "C" void HAL_QSPI_RxCpltCallback( QSPI_HandleTypeDef* hqspi )
{
	RxCplt++;
}

/**
 * @brief  Tx Transfer completed callbacks.
 * @param  hqspi: QSPI handle
 * @retval None
 */
extern "C" void HAL_QSPI_TxCpltCallback( QSPI_HandleTypeDef* hqspi )
{
	TxCplt++;
}

/**
 * @brief  Status Match callbacks
 * @param  hqspi: QSPI handle
 * @retval None
 */
extern "C" void HAL_QSPI_StatusMatchCallback( QSPI_HandleTypeDef* hqspi )
{
	StatusMatch++;
}
