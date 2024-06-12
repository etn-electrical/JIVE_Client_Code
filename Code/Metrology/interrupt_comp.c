/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : interrupt_comp.c
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
 *
 ****************************************************************************/
#include "interrupt_comp.h"
#include "freertos/queue.h"
#include "metro.h"

/****************************************************************************
 *                              Global variables
 ****************************************************************************/
extern xQueueHandle Metro_evt_queue;
extern metro_flag_t g_metroFlag;

/**
 * @brief      This function handles interrupt on IRQ1 pin
 * @param[in]  void *args pin no on which interrupt generates
 * @retval     bool: returns 0 or 1
*/
bool IRAM_ATTR EXTI_IRQ0_Handler(void *args)
{
	uint8_t MetroEvent = METRO_IRQ_READ_EVENT;
	if (Metro_evt_queue != NULL)
	{
		xQueueSendFromISR(Metro_evt_queue, &MetroEvent, NULL);
	}

	return 0;
}


/**
 * @brief      This function handles interrupt on CF3_ZA pin
 * @param[in]  void *args pin no on which interrupt generates
 * @retval     bool: returns 0 or 1
*/
bool IRAM_ATTR EXTI_CF3_ZA_Handler(void *args)
{
	uint8_t MetroEvent;
	static bool FirstZeroCrossing = true;
	uint8_t ret = METRO_SUCCESS;

	if (FirstZeroCrossing == true)
	{
			//We need to skip first zero crossing

		FirstZeroCrossing = false;
	}
	else
	{
			//Occurs only after power cycle

		if (!g_metroFlag.waveform_enable)
		{
#ifdef AD_WFB_EN

			if ((ret = Metro_Waveform_Cap_Init()) != METRO_SUCCESS)
			{
				MetroEvent = WAVEFORM_INIT_FAIL_EVENT;
				if (Metro_evt_queue != NULL)
				{
					xQueueSendToBackFromISR(Metro_evt_queue, &MetroEvent, NULL);
				}
				ret = METRO_SPI_ERR;
			}
			else
			{
				g_metroFlag.waveform_enable = true;
				g_metroFlag.read_data = true;
			}
		}
	}

		//Notify once metrology data read event occurred

	if ((g_metroFlag.read_data) && (g_metroFlag.waveform_enable))
	{
		MetroEvent = METRO_READ_EVENT;
		g_metroFlag.read_data = false;
		if (Metro_evt_queue != NULL)
		{
			xQueueSendToBackFromISR(Metro_evt_queue, &MetroEvent, NULL);
		}
	}
#endif

	return 0;
}




