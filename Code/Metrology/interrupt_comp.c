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
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 08/24/2023
 *
 ****************************************************************************/
#include "interrupt_comp.h"
#include "freertos/queue.h"
#include "metro.h"

/****************************************************************************
 *                              Global variables
 ****************************************************************************/
extern xQueueHandle g_metro_evt_queue;
extern metro_flag_t g_metroFlag;

/**
 * @brief      This function handles interrupt on IRQ1 pin
 * @param[in]  void *args pin no on which interrupt generates
 * @retval     bool: returns 0 or 1
*/
bool IRAM_ATTR EXTI_IRQ0_Handler(void *args)
{
	uint8_t MetroEvent = METRO_IRQ_READ_EVENT;
	if (g_metro_evt_queue != NULL)
	{
		xQueueSendFromISR(g_metro_evt_queue, &MetroEvent, NULL);
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

			if (Metro_Waveform_Cap_Init() != METRO_SUCCESS)
			{
				MetroEvent = WAVEFORM_INIT_FAIL_EVENT;
				if (g_metro_evt_queue != NULL)
				{
					xQueueSendToBackFromISR(g_metro_evt_queue, &MetroEvent, NULL);
				}
			}
			else
			{
				g_metroFlag.waveform_enable = true;
			}
		}
	}

#endif

	return 0;
}




