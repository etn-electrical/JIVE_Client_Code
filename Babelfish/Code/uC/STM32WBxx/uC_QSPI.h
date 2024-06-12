/**
 ******************************************************************************
 * @file    QSPI/QSPI_ReadWrite_DMA/Inc/main.h
 * @author  MCD Application Team
 * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef uC_QSPI_H
#define uC_QSPI_H

/* Includes ------------------------------------------------------------------*/
#include "Includes.h"
#include "stm32l4xx_hal_dma.h"
#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_qspi.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_cortex.h"
// #include "stm32f769i_eval.h"

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_OS_Interrupt.h"
#include "OS_Semaphore.h"

/* QSPI Error codes */
#define QSPI_OK            ( ( uint8_t )0x00 )
#define QSPI_ERROR         ( ( uint8_t )0x01 )
#define QSPI_BUSY          ( ( uint8_t )0x02 )
#define QSPI_NOT_SUPPORTED ( ( uint8_t )0x04 )
#define QSPI_SUSPENDED     ( ( uint8_t )0x08 )

/* Exported types ------------------------------------------------------------
   Exported constants --------------------------------------------------------
   Definition for QSPI clock resources*/
#define QSPI_CLK_ENABLE()          __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()         __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D0_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define QSPI_D3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define QSPI_DMA_CLK_ENABLE()      __HAL_RCC_DMA2_CLK_ENABLE()

#define QSPI_FORCE_RESET()         __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()       __HAL_RCC_QSPI_RELEASE_RESET()

/* Definition for QSPI Pins */
// const uC_USER_IO_STRUCT       PIOB_PIN_6_STRUCT = { GPIOB, IO_PIN_6, uC_PIO_INDEX_B };
#define QSPI_CS_PIN                GPIO_PIN_11
#define QSPI_CS_GPIO_PORT          GPIOB
#define QSPI_CLK_PIN               GPIO_PIN_10
#define QSPI_CLK_GPIO_PORT         GPIOB
#define QSPI_D0_PIN                GPIO_PIN_1
#define QSPI_D0_GPIO_PORT          GPIOB
#define QSPI_D1_PIN                GPIO_PIN_0
#define QSPI_D1_GPIO_PORT          GPIOB
#define QSPI_D2_PIN                GPIO_PIN_7
#define QSPI_D2_GPIO_PORT          GPIOA
#define QSPI_D3_PIN                GPIO_PIN_6
#define QSPI_D3_GPIO_PORT          GPIOA

/* Definition for QSPI DMA */
/*#define QSPI_DMA_INSTANCE          DMA2_Stream7
 #define QSPI_DMA_CHANNEL           DMA_CHANNEL_3
 #define QSPI_DMA_IRQ               DMA2_Stream7_IRQn
 #define QSPI_DMA_IRQ_HANDLER       DMA2_Stream7_IRQHandler*/

/* IS25 memory
   Size of the flash*/
#define QSPI_FLASH_SIZE                      21
#define QSPI_PAGE_SIZE                       256

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9E
#define READ_ID_CMD2                         0x9F
#define MULTIPLE_IO_READ_ID_CMD              0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define READ_4_BYTE_ADDR_CMD                 0x13

#define FAST_READ_CMD                        0x0B
#define FAST_READ_DTR_CMD                    0x0D
#define FAST_READ_4_BYTE_ADDR_CMD            0x0C

#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_OUT_FAST_READ_DTR_CMD           0x3D
#define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x3C

#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define DUAL_INOUT_FAST_READ_DTR_CMD         0xBD
#define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_OUT_FAST_READ_DTR_CMD           0x6D
#define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x6C

#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_FAST_READ_DTR_CMD         0xED
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                  0x05
#define WRITE_STATUS_REG_CMD                 0x01

#define READ_LOCK_REG_CMD                    0xE8
#define WRITE_LOCK_REG_CMD                   0xE5

#define READ_FLAG_STATUS_REG_CMD             0x70
#define CLEAR_FLAG_STATUS_REG_CMD            0x50

#define READ_NONVOL_CFG_REG_CMD              0xB5
#define WRITE_NONVOL_CFG_REG_CMD             0xB1

#define READ_VOL_CFG_REG_CMD                 0x61
#define WRITE_VOL_CFG_REG_CMD                0xC0

#define READ_ENHANCED_VOL_CFG_REG_CMD        0x65
#define WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61

#define READ_EXT_ADDR_REG_CMD                0xC8
#define WRITE_EXT_ADDR_REG_CMD               0xC5

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define PAGE_PROG_4_BYTE_ADDR_CMD            0x12

#define DUAL_IN_FAST_PROG_CMD                0xA2
#define EXT_DUAL_IN_FAST_PROG_CMD            0xD2

#define QUAD_IN_FAST_PROG_CMD                0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12	/*0x38*/
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x34

/* Erase Operations */
#define SUBSECTOR_ERASE_CMD                  0x20
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD      0x21

#define SECTOR_ERASE_CMD                     0xD7
#define SECTOR_ERASE_4_BYTE_ADDR_CMD         0xDC

#define BULK_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* One-Time Programmable Operations */
#define READ_OTP_ARRAY_CMD                   0x4B
#define PROG_OTP_ARRAY_CMD                   0x42

/* 4-byte Address Mode Operations */
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

/* Quad Operations */
#define ENTER_QUAD_CMD                       0x35
#define EXIT_QUAD_CMD                        0xF5

/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ              8
#define DUMMY_CLOCK_CYCLES_READ_QUAD         8

#define DUMMY_CLOCK_CYCLES_READ_DTR          6
#define DUMMY_CLOCK_CYCLES_READ_QUAD_DTR     8

/* End address of the QSPI memory */
#define QSPI_END_ADDR              ( 1 << QSPI_FLASH_SIZE )


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF( __BUFFER__ )        ( sizeof( __BUFFER__ ) / sizeof( *( __BUFFER__ ) ) )

/* Exported functions ------------------------------------------------------- */



class uC_QSPI
{
	public:
		/**
		 * @brief                   Max Retry Count for uC_SPI Rx communication
		 */
		static const uint32_t QSPI_RETRY_COUNT = 1000U;

		/**
		 * @brief                     Constructor to create instance of uc_QSPI class.

		 * @param[in] chip_sel_ctrls  Chip Select pin specification. This structure variable shall specify
		 *                            the GPIO port and pin number which is used for selecting QSPI slave
		 *                            device
		 * @param[in] io_ctrl         Structure pointer which contains information about the pins which are
		 *                            used for QSPI communication. If NULL - it initializes the QSPI pins for
		 *                            the channel "spi_select". (See the parameter spi_select)
		 * @param[in] enable_dma      Enable/Disable DMA of the SPI peripheral.
		 * @n                         TRUE  : Enables DMA
		 * @n                         FALSE : Disables DMA
		 * @return
		 */
		uC_QSPI( uC_USER_IO_STRUCT const* const* chip_sel_ctrls,
				 uC_BASE_SPI_IO_PORT_STRUCT const* io_ctrl = NULL, bool enable_dma = true );

		/**
		 * @brief                   Destructor to delete the uc_QSPI instance when it goes out of scope
		 *                          scope.
		 */
		~uC_QSPI( void );




		/**
		 * @brief                    Selects the chip for SPI communication. Selecting the chip means
		 *                           pulling low(logic 0) the slave select signal(SS) of SPI device.
		 *                           When SS pin is pulled low means controller wants to transfer data
		 *                           to SPI device.
		 *
		 * @param[in] chip_select   The SPI device number which need to be selected for communication
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void Select_Chip( uint8_t chip_select );

		/**
		 * @brief                    Deselect the chip to stop the SPI communication.Deselecting the chip
		 *                           pulling high(logic 1) the slave select signal(SS) of SPI device.
		 *
		 * @param[in] chip_select   The SPI device number whose communication need to be disconnected
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		void De_Select_Chip( uint8_t chip_select ) const;

		/**
		 * @brief                   This Function acquires the semaphore.(Not used in Low toolkit)
		 *
		 * @param[in] time_to_wait  Wait this amount of time before acquiring semapore.
		 * @return bool             Two possible values
		 * @n                       TRUE - Semaphore acquired
		 * @n                       FALSE - Semaphore not acquired
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		bool Get_Semaphore( uint32_t time_to_wait )
		{
			return ( true );	// m_semaphore->Pend( time_to_wait ) );
		}

		/**
		 * @brief                   This Function releases the semapore.(Not used in Low toolkit)
		 *
		 *
		 * @return bool             Two possible values
		 * @n                       TRUE - Releasing the semaphore successfull
		 * @n                       FALSE - Semaphore not release
		 * @note                    Implemented as an inline function.
		 * @n
		 * @n
		 */
		bool Release_Semaphore( void )
		{
			return ( true );	// m_semaphore->Post() );
		}

		void QSPI_WriteEnable();

		void QSPI_WriteStatusRegister( uint8_t status );

		uint8_t QSPI_ReadStatusRegister( void );

		void Write_Data( uint8_t* data, uint32_t address, uint32_t length );

		void Read_Data( uint8_t* data, uint32_t address, uint32_t length );

		void Erase_Sector( uint32_t address );

		void Change_QSPI_Mode( uint8_t mode );

		void QSPI_ResetMemory();

		void QSPI_AutoPollingMemReady();

		void QSPI_DummyCyclesCfg();

		void Erase_Block( uint8_t erase_cmd, uint32_t address );

	private:
		void Error_Handler();

		/**
		 * @brief SPI pin configuration holding structure
		 */
		uC_BASE_SPI_IO_STRUCT const* m_config;

		/**
		 * @brief ***
		 */
		// QSPI_HandleTypeDef QSPIHandle;
		QSPI_CommandTypeDef sCommand;

		OS_Semaphore* m_semaphore;
};

#endif	/* uC_QSPI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
