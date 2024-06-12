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
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
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
#include "ADE9000.h"
#include "metro_utilities.h"
#include "../DeviceGPIO.h"

/*******************************************************************************
* GLOBAL VARIABLES:
*******************************************************************************/
#define AD_SPI_HOST    HSPI_HOST
#define SPI_TX_BUF_SIZE 6
#define SPI_RX_BUF_SIZE 8
#define BYTE 8

#define SPI_PACKET_16_BUF_SIZE 4
#define SPI_PACKET_16_BUF_SIZE_CRC 6
#define SPI_PACKET_16_START_INDEX 2
#define SPI_DATA_BYTES_16 2

#define SPI_PACKET_32_BUF_SIZE 6
#define SPI_PACKET_32_BUF_SIZE_CRC 8
#define SPI_PACKET_32_START_INDEX 2
#define SPI_DATA_BYTES_32 4

#define BURST_READ_TX_BUF_SIZE 4

#define SIGNAL_NOT_USED -1
#define SPI_DMA_MODE 3
#define SPI_QUEUE_SIZE 6
#define SPI_INPUT_DELAY_MS 10
#define SPI_CLK_FREQ 10000000

//#define METRO_SPI_CRC_EN
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
 uint8_t Read_SPI_ADE9039_Reg_16(uint16_t  us_ADE_Addr, uint16_t* spi_read_data);
 uint8_t Read_SPI_ADE9039_Reg_32(uint16_t  us_ADE_Addr, uint32_t* spi_read_data);
 uint8_t Write_SPI_ADE9039_Reg_32(uint16_t us_ADE_Addr, uint32_t Reg_Data);
 uint8_t Write_SPI_ADE9039_Reg_16(uint16_t us_ADE_Addr, uint16_t Reg_Data);
 uint8_t SPI_Comp_SendAndReceivePacket_16(uint8_t * data_snd, uint8_t * data_rcv);
 uint8_t SPI_Comp_SendAndReceivePacket_32(uint8_t * data_snd, uint8_t * data_rcv);
 uint8_t SPI_Comp_SendAndReceivePacket_burst(uint8_t * data_snd, uint16_t total_bytes, uint8_t *rx_buffer);
 uint8_t Burst_Read_SPI_ADE9039(uint16_t  us_ADE_Addr /*uint16_t *Reg_Data */,uint16_t total_bytes);
 uint8_t SPI_Init(void);
 void AD_En_CS(spi_transaction_t *t);
 void AD_Ds_CS(spi_transaction_t *t);
 uint16_t CRC_CCITT16_Cal32(uint8_t total_bytes);
 uint16_t CRC_Check(uint8_t total_bytes);

#endif
