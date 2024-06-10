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
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/
#ifndef __METRO_UTILITIES_H
#define __METRO_UTILITIES_H

/****************************************************************************
 *                           	Include files
 ****************************************************************************/
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
/****************************************************************************
 *                           	Constants and Macros
 ****************************************************************************/
#define AD_WFB_EN
#define MAX_BURST_READ_DATA_BYTES 770
#define METRO_MAX_CURRENT_CH 2

/****************************************************************************
 *                  			 Types
 ****************************************************************************/
 /**
   * @brief METROLOGY  ERROR definition
   *
   */
typedef enum {
    METRO_SUCCESS = 0,
    METRO_SPI_ERR = 10,
} metro_err_t;


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
   * @brief METROLOGY  ENERGY definition
   *
   */
typedef struct
{
	//Line1, Line2 Active energy
	double active[2];

	//Line1, Line2 Reactive energy
	double	reactive[2];

	//Line1, Line2 Apparent energy
	double apparent[2];

	//Line1, Line2 Rev Active energy
	double revActive[2];

	//Line1, Line2 Rev Reactive energy
	double revReactive[2];

	//Line1, Line2 Rev Apparent energy
	double revApparent[2];
}Metro_Energy;


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
