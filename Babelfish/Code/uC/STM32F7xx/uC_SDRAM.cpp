/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Base.h"
#include "RAM.h"
#include "uC_SDRAM.h"
#include "stm32f7xx_hal.h"
#include "uC_FMC.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SDRAM::uC_SDRAM( uint8_t fmc_sdram_select, uC_BASE_FMC_SDRAM_IO_PORT_STRUCT const* io_ctrl ) :
	m_config( reinterpret_cast<uC_BASE_FMC_SDRAM_IO_STRUCT const*>( nullptr ) ),
	m_fmc_sdram( reinterpret_cast<FMC_Bank5_6_TypeDef*>( nullptr ) )
{
	m_config = ( uC_Base::Self() )->Get_FMC_SDRAM_Ctrl( fmc_sdram_select );
	BF_ASSERT( m_config != reinterpret_cast<uC_BASE_FMC_SDRAM_IO_STRUCT const*>( nullptr ) );
	m_fmc_sdram = m_config->reg_ctrl;

	uC_Base::Reset_Periph( &m_config->periph_def );
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	if ( io_ctrl == nullptr )
	{
		io_ctrl = m_config->default_port_io;
	}

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a0_pio );
	Set_Periph_Pin( io_ctrl->a0_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a1_pio );
	Set_Periph_Pin( io_ctrl->a1_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a2_pio );
	Set_Periph_Pin( io_ctrl->a2_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a3_pio );
	Set_Periph_Pin( io_ctrl->a3_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a4_pio );
	Set_Periph_Pin( io_ctrl->a4_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a5_pio );
	Set_Periph_Pin( io_ctrl->a5_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a6_pio );
	Set_Periph_Pin( io_ctrl->a6_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a7_pio );
	Set_Periph_Pin( io_ctrl->a8_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a8_pio );
	Set_Periph_Pin( io_ctrl->a8_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a9_pio );
	Set_Periph_Pin( io_ctrl->a9_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a10_pio );
	Set_Periph_Pin( io_ctrl->a10_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->a11_pio );
	Set_Periph_Pin( io_ctrl->a11_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->b0_pio );
	Set_Periph_Pin( io_ctrl->b0_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->b1_pio );
	Set_Periph_Pin( io_ctrl->b1_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d0_pio );
	Set_Periph_Pin( io_ctrl->d0_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d1_pio );
	Set_Periph_Pin( io_ctrl->d1_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d2_pio );
	Set_Periph_Pin( io_ctrl->d2_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d3_pio );
	Set_Periph_Pin( io_ctrl->d3_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d4_pio );
	Set_Periph_Pin( io_ctrl->d4_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d5_pio );
	Set_Periph_Pin( io_ctrl->d5_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d6_pio );
	Set_Periph_Pin( io_ctrl->d6_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d7_pio );
	Set_Periph_Pin( io_ctrl->d7_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d8_pio );
	Set_Periph_Pin( io_ctrl->d8_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d9_pio );
	Set_Periph_Pin( io_ctrl->d9_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d10_pio );
	Set_Periph_Pin( io_ctrl->d10_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d11_pio );
	Set_Periph_Pin( io_ctrl->d11_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d12_pio );
	Set_Periph_Pin( io_ctrl->d12_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d13_pio );
	Set_Periph_Pin( io_ctrl->d13_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d14_pio );
	Set_Periph_Pin( io_ctrl->d14_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->d15_pio );
	Set_Periph_Pin( io_ctrl->d15_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->nbl0_pio );
	Set_Periph_Pin( io_ctrl->nbl0_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->nbl1_pio );
	Set_Periph_Pin( io_ctrl->nbl1_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdclk_pio );
	Set_Periph_Pin( io_ctrl->sdclk_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdnwe_pio );
	Set_Periph_Pin( io_ctrl->sdnwe_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdnras_pio );
	Set_Periph_Pin( io_ctrl->sdnras_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdncas_pio );
	Set_Periph_Pin( io_ctrl->sdncas_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdcke0_pio );
	Set_Periph_Pin( io_ctrl->sdcke0_pio );

	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->sdne0_pio );
	Set_Periph_Pin( io_ctrl->sdne0_pio );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SDRAM::~uC_SDRAM( void )
{ }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Init( SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_TimingTypeDef* Timing )
{


	m_fmc_handle = new uC_FMC();

	BF_ASSERT( m_fmc_handle != nullptr );
	/* Initialize SDRAM control Interface */
	m_fmc_handle->FMC_SDRAM_Init( hsdram->Instance, &( hsdram->Init ) );
	/* Initialize SDRAM timing Interface */
	m_fmc_handle->FMC_SDRAM_Timing_Init( hsdram->Instance, Timing, hsdram->Init.SDBank );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_DeInit( SDRAM_HandleTypeDef* hsdram )
{
	/* Initialize the low level hardware (MSP) */
	HAL_SDRAM_MspDeInit( hsdram );

	/* Configure the SDRAM registers with their reset values */
	FMC_SDRAM_DeInit( hsdram->Instance, hsdram->Init.SDBank );

	/* Reset the SDRAM controller state */
	hsdram->State = HAL_SDRAM_STATE_RESET;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_IRQHandler( SDRAM_HandleTypeDef* hsdram )
{
	/* Check SDRAM interrupt Rising edge flag */
	if ( __FMC_SDRAM_GET_FLAG( hsdram->Instance, FMC_SDRAM_FLAG_REFRESH_IT ) )
	{
		/* SDRAM refresh error interrupt callback */
		uC_SDRAM_RefreshErrorCallback( hsdram );

		/* Clear SDRAM refresh error interrupt pending bit */
		__FMC_SDRAM_CLEAR_FLAG( hsdram->Instance, FMC_SDRAM_FLAG_REFRESH_ERROR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM_RefreshErrorCallback( SDRAM_HandleTypeDef* hsdram )
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED( hsdram );

	/* NOTE: This function Should not be modified, when the callback is needed,
	          the HAL_SDRAM_RefreshErrorCallback could be implemented in the user file
	 */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Read_8b( uint32_t* pAddress, uint8_t* pDstBuffer, uint32_t BufferSize )
{
	__IO uint8_t* pSdramAddress = ( uint8_t* )pAddress;

	/* Read data from source */
	for (; BufferSize != 0; BufferSize-- )
	{
		*pDstBuffer = *( __IO uint8_t* )pSdramAddress;
		pDstBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Write_8b( uint32_t* pAddress, uint8_t* pSrcBuffer, uint32_t BufferSize )
{
	__IO uint8_t* pSdramAddress = ( uint8_t* )pAddress;

	/* Write data to memory */
	for (; BufferSize != 0; BufferSize-- )
	{
		*( __IO uint8_t* )pSdramAddress = *pSrcBuffer;
		pSrcBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Read_16b( uint32_t* pAddress, uint16_t* pDstBuffer, uint32_t BufferSize )
{
	__IO uint16_t* pSdramAddress = ( uint16_t* )pAddress;

	/* Read data from source */
	for (; BufferSize != 0; BufferSize-- )
	{
		*pDstBuffer = *( __IO uint16_t* )pSdramAddress;
		pDstBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Write_16b( uint32_t* pAddress, uint16_t* pSrcBuffer, uint32_t BufferSize )
{
	__IO uint16_t* pSdramAddress = ( uint16_t* )pAddress;

	/* Write data to memory */
	for (; BufferSize != 0; BufferSize-- )
	{
		*( __IO uint16_t* )pSdramAddress = *pSrcBuffer;
		pSrcBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Read_32b( uint32_t* pAddress, uint32_t* pDstBuffer, uint32_t BufferSize )
{
	__IO uint32_t* pSdramAddress = ( uint32_t* )pAddress;

	/* Read data from source */
	for (; BufferSize != 0; BufferSize-- )
	{
		*pDstBuffer = *( __IO uint32_t* )pSdramAddress;
		pDstBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_Write_32b( uint32_t* pAddress, uint32_t* pSrcBuffer, uint32_t BufferSize )
{
	__IO uint32_t* pSdramAddress = ( uint32_t* )pAddress;

	/* Write data to memory */
	for (; BufferSize != 0; BufferSize-- )
	{
		*( __IO uint32_t* )pSdramAddress = *pSrcBuffer;
		pSrcBuffer++;
		pSdramAddress++;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_WriteProtection_Enable( SDRAM_HandleTypeDef* hsdram )
{

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_BUSY;

	/* Enable write protection */
	FMC_SDRAM_WriteProtection_Enable( hsdram->Instance, hsdram->Init.SDBank );

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_WRITE_PROTECTED;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_WriteProtection_Disable( SDRAM_HandleTypeDef* hsdram )
{

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_BUSY;

	/* Disable write protection */
	FMC_SDRAM_WriteProtection_Disable( hsdram->Instance, hsdram->Init.SDBank );

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_READY;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_SendCommand( SDRAM_HandleTypeDef* hsdram, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout )
{

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_BUSY;

	/* Send SDRAM command */
	m_fmc_handle->FMC_SDRAM_SendCommand( hsdram->Instance, Command, Timeout );

	/* Update the SDRAM controller state state */
	if ( Command->CommandMode == FMC_SDRAM_CMD_PALL )
	{
		hsdram->State = HAL_SDRAM_STATE_PRECHARGED;
	}
	else
	{
		hsdram->State = HAL_SDRAM_STATE_READY;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_ProgramRefreshRate( SDRAM_HandleTypeDef* hsdram, uint32_t RefreshRate )
{

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_BUSY;

	/* Program the refresh rate */
	FMC_SDRAM_ProgramRefreshRate( hsdram->Instance, RefreshRate );

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_READY;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::uC_SDRAM_SetAutoRefreshNumber( SDRAM_HandleTypeDef* hsdram, uint32_t AutoRefreshNumber )
{

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_BUSY;

	/* Set the Auto-Refresh number */
	FMC_SDRAM_SetAutoRefreshNumber( hsdram->Instance, AutoRefreshNumber );

	/* Update the SDRAM state */
	hsdram->State = HAL_SDRAM_STATE_READY;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_SDRAM::uC_SDRAM_GetModeStatus( SDRAM_HandleTypeDef* hsdram )
{
	/* Return the SDRAM memory current mode */
	return ( FMC_SDRAM_GetModeStatus( hsdram->Instance, hsdram->Init.SDBank ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_SDRAM::uC_SDRAM_GetState( SDRAM_HandleTypeDef* hsdram )
{
	return ( hsdram->State );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_SDRAM::Set_Periph_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl )
{
	__IO UINT32* config_reg = nullptr;

	BF_ASSERT( pio_ctrl != nullptr );
	/* Get the access to Speed register */
	config_reg = &( pio_ctrl )->reg_ctrl->OSPEEDR;
	/* Clear the pin speed configuration*/
	*config_reg &= ~( 0x00000003 << ( ( pio_ctrl )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to Fast speed mode */
	*config_reg |= ( 0x00000002 << ( ( pio_ctrl )->pio_num * MULT_BY_4 ) );
	/* Get the access to Pull up pull down register */
	config_reg = &( pio_ctrl )->reg_ctrl->PUPDR;
	/* Clear the pin Pull up pull down configuration*/
	*config_reg &= ~( 0x00000003 << ( ( pio_ctrl )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to pull up */
	*config_reg |= ( 0x00000001 << ( ( pio_ctrl )->pio_num * MULT_BY_4 ) );
}
