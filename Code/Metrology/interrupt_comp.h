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
