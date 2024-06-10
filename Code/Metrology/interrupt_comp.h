/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : interrupt_comp.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 08/24/2023
 *
 ****************************************************************************/

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

/****************************************************************************
 *                           	Include files
 ****************************************************************************/
#include "freertos/FreeRTOS.h"

 /****************************************************************************
  *                    Local function Declarations
  ****************************************************************************/
bool IRAM_ATTR EXTI_IRQ0_Handler(void *args);
bool IRAM_ATTR EXTI_CF3_ZA_Handler(void *args);

#endif /* INTERRUPT_H_ */
