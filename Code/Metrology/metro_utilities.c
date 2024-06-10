/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : metro_utilities.c
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 08/24/2023
 *
 ****************************************************************************/
#include "metro_utilities.h"
metro_waveform_burst_data g_metroWaveformData;

/**
 * @brief      This function reverse (bytes) for 16 bit data to change order from LSB--MSB to MSB--LSB
 * @param[in]  data: 16 bit data for byte reverse
 * @retval     uint16_t reversed bytes for 16 bits
*/
uint16_t Metro_Reverse_bytes_16(uint16_t data)
{
	uint16_t rev_bytes;

	rev_bytes = ((data >> 8) & 0x00ff) | ((data << 8) & 0xff00);

	return rev_bytes;
}



/**
 * @brief      This function reverse (bytes) for 32 bit data to change order from LSB--MSB to MSB--LSB
 * @param[in]  data: 32 bit data for byte reverse
 * @retval     uint32_t reversed bytes for 16 bits
*/
uint32_t Metro_Reverse_bytes_32(uint32_t data)
{
	uint32_t rev_bytes;

	rev_bytes = ((data >> 24) & 0x000000ff) | ((data << 24) & 0xff000000) |
			((data >> 8) & 0x0000ff00) | ((data << 8) & 0x00ff0000);

	return rev_bytes;
}
