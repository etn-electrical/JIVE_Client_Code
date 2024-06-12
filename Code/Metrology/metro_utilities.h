/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : metro_utilities.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
 *
 ****************************************************************************/
#ifndef __METRO_UTILITIES_H
#define __METRO_UTILITIES_H

/****************************************************************************
 *                           	Include files
 ****************************************************************************/
#include "esp_log.h"
#include "esp_system.h"

/****************************************************************************
 *                           	Constants and Macros
 ****************************************************************************/
#define AD_WFB_EN
#define MAX_BURST_READ_DATA_BYTES 770
#define METRO_SPI_ERR 10
#define METRO_SUCCESS 0
#define METRO_MAX_CURRENT_CH 2
/****************************************************************************
 *                  			 Types
 ****************************************************************************/

 /**
   * @brief METROLOGY  CHANNEL definition
   *
   */
 typedef enum
 {
   CHANNEL_1,
   CHANNEL_2,
 }METRO_Channel_t;



 /**
 * @brief	waveform burst data collection union
 */
 typedef union
 {
 	uint16_t data_16;
 	uint8_t spi_read_data[MAX_BURST_READ_DATA_BYTES];
 }metro_waveform_burst_data;

 /****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
 uint16_t Metro_Reverse_bytes_16(uint16_t data);
 uint32_t Metro_Reverse_bytes_32(uint32_t data);

#endif /*METRO_UTILITIES_H */
