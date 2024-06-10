/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : spi_comp.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/

#ifndef _SPI_COMP_H_
#define _SPI_COMP_H_

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "metro_utilities.h"
#include "../DeviceGPIO.h"
#include "ADE9039.h"

/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
#define SPI_TX_BUF_SIZE 6
#define SPI_RX_BUF_SIZE 8
#define BYTE 8

#define SPI_PACKET16_BUF_SIZE 4
#define SPI_PACKET16_BUF_SIZE_CRC 6
#define SPI_PACKET32_BUF_SIZE 6
#define SPI_PACKET32_BUF_SIZE_CRC 8

#define SPI_DATA16_SIZE 2
#define SPI_DATA32_SIZE 4

#define SPI_CRC_SIZE 2

#define SPI_DATA_START_INDEX 2
#define SPI_DATA16_CRC_START_INDEX 4
#define SPI_DATA32_CRC_START_INDEX 6

#define BURST_READ_TX_BUF_SIZE 4
#define SIGNAL_NOT_USED -1
#define SPI_QUEUE_SIZE 6
#define SPI_INPUT_DELAY_MS 10
#define SPI_CLK_FREQ 10000000	//10Mhz

#define METRO_SPI_CRC_EN 1
 /*******************************************************************************
 * TYPES:
 *******************************************************************************/
 /**
   * @brief SPI CRC collection union
   *
   */
 typedef union
 {
 	uint16_t ad_crc;
 	uint8_t data[2];
 }crc_union;



 /**
 * @brief	SPI data collection union
 */
 typedef union
 {
 	uint32_t pm_data_32;		//32 bit data
 	uint16_t pm_data_16;		//16 bit data
 	uint8_t temp[4];		//access data bytes
 }spi_read_data_union;

 /*******************************************************************************
 * FUNCTIONS:
 *******************************************************************************/
 metro_err_t SPI_Read_ADE9039_Reg_16(uint16_t  us_ADE_Addr, uint16_t* spi_read_data);
 metro_err_t SPI_Read_ADE9039_Reg_32(uint16_t  us_ADE_Addr, uint32_t* spi_read_data);
 metro_err_t SPI_Write_ADE9039_Reg_32(uint16_t us_ADE_Addr, uint32_t Reg_Data);
 metro_err_t SPI_Write_ADE9039_Reg_16(uint16_t us_ADE_Addr, uint16_t Reg_Data);
 metro_err_t SPI_Comp_SendAndReceivePacket_16(uint8_t * data_snd, uint8_t * data_rcv);
 metro_err_t SPI_Comp_SendAndReceivePacket_32(uint8_t * data_snd, uint8_t * data_rcv);
 metro_err_t SPI_Comp_SendAndReceivePacket_burst(uint8_t * data_snd, uint16_t total_bytes, uint8_t *rx_buffer);
 metro_err_t SPI_Burst_Read(uint16_t  us_ADE_Addr);
 metro_err_t SPI_Init(void);
 void SPI_AD_En_CS(spi_transaction_t *t);
 void SPI_AD_Ds_CS(spi_transaction_t *t);
 uint16_t SPI_CRC_CCITT16_Cal32(uint8_t data_rcv[SPI_RX_BUF_SIZE], uint8_t total_bytes);
 metro_err_t SPI_CRC_Check(uint8_t total_bytes, uint8_t data_rcv[SPI_RX_BUF_SIZE]);

#endif
